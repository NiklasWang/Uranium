#include <string>
#include <zlib.h>
#include<stdio.h>
#include<sys/types.h>
#include<dirent.h>
#include<string.h>
#include<sys/stat.h>
#include<unistd.h>
#include "MonitorCore.h"
#include "md5.h"
//filename: ./test/file
struct file_info { //用于保存文件名和对应的inode,用于判断是否为硬链接文件
    char filename[100];
    ino_t inode;
} filesave[1024];

void tarfile(const char* filename, FILE* fpOut) //打包文件
{

    struct stat stat_buf;
    stat(filename, &stat_buf);
    filesave[1024];
    int i;
    static int n = 0; //记录写入文件的真实数量，不包括硬链接文件
    for (i = 0; i <= n; i++) { //判断文件是否已写入
        if (filesave[i].inode == stat_buf.st_ino) { //判断是硬链接文件
            fprintf(fpOut, "h\n%s\n%s\n", filename, filesave[i].filename); //写入标记h->newname->oldname
            return;
        }
    }

    fprintf(fpOut, "f\n%s\n%d\n", filename, (int)stat_buf.st_size);
    FILE *fpIn = fopen(filename, "r");
    char buf[4096];
    while (1) {
        int ret = fread(buf, 1, sizeof(buf), fpIn);
        if (ret <= 0) {
            break;
        }
        fwrite(buf, ret, 1, fpOut);
    }
    strcpy(filesave[n].filename, filename); //将新打包的文件写入结构体记录
    filesave[n].inode = stat_buf.st_ino;
    n++;
    fclose(fpIn);
}

//dirname: ./test
int tardir(const char* dirname, FILE* fpOut) //打包目录
{
    char filepath[1024];
    fprintf(fpOut, "d\n"); //d目录标记
    fprintf(fpOut, "%s\n", dirname); //打包的根目录

    DIR* dir = opendir(dirname); //打开文件目录项
    struct dirent* entry = readdir(dir);
    while (entry) {
        //./test/file
        sprintf(filepath, "%s/%s", dirname, entry->d_name); //拼凑每个目录项的路径
        if (entry->d_type == DT_REG) { //判断是否为文件
            tarfile(filepath, fpOut); //打包文件
        } else if (entry->d_type == DT_DIR) { //判断是否为目录,若是就继续递归
            if ((strcmp(entry->d_name, ".") == 0) ||
                (strcmp(entry->d_name, "..") == 0)) { //. ..忽略
                entry = readdir(dir);
                continue;
            }
            tardir(filepath, fpOut);
        }
        entry = readdir(dir);
    }
    closedir(dir);
}

int tar(const char* dirname, const char *outfile)
{
    FILE* fpOut = fopen(outfile, "w");
    fprintf(fpOut, "xgltar\n"); //标记打包文件类型
    fprintf(fpOut, "1.0\n"); //版本
    int ret = tardir(dirname, fpOut); //打包目录
    fclose(fpOut);
    return ret;
}

int untarfile(FILE *fin)
{
    char buf[1024];
    if (fgets(buf, sizeof(buf), fin) == NULL) {
        return -1;
    }
    printf("now utar type=%s", buf);
    if (strcmp(buf, "d\n") == 0) { //目录标记
        fgets(buf, sizeof(buf), fin);
        buf[strlen(buf) - 1] = 0;
        mkdir(buf, 0777);
        printf("mkdir %s\n", buf);
    } else if (strcmp(buf, "f\n") == 0) { //文件标记
        fgets(buf, sizeof(buf), fin);
        buf[strlen(buf) - 1] = 0;
        FILE *out = fopen(buf, "w");
        printf("create file %s\n", buf);
        fgets(buf, sizeof(buf), fin);
        int len = atol(buf);
        printf("filelen %s\n", buf);
        while (len > 0) {
            int readlen = len < sizeof(buf) ? len : sizeof(buf);
            int ret = fread(buf, 1, readlen, fin);
            fwrite(buf, 1, ret, out);
            len -= ret;
        }
        fclose(out);
    } else if (strcmp(buf, "h\n") == 0) { //硬链接文件标记
        fgets(buf, sizeof(buf), fin); //读取链接文件名
        buf[strlen(buf) - 1] = 0;
        char oldbuf[1024];//被链接的文件名
        fgets(oldbuf, sizeof(oldbuf), fin);
        oldbuf[strlen(oldbuf) - 1] = 0;
        link(oldbuf, buf);
    }
    return 0;
}
//解包
int untar(const char* tarfile)
{
    char buf[1024];
    FILE *fin = fopen(tarfile, "r");
    fgets(buf, sizeof(buf), fin);
    if (strcmp(buf, "xgltar\n") != 0) { //判断是否为打包文件类型
        printf("unknown file format\n");
        return -1;
    }
    fgets(buf, sizeof(buf), fin);
    if (strcmp(buf, "1.0\n") == 0) { //判断版本是否正确
        while (1) {
            int ret = untarfile(fin); //解包
            if (ret != 0) {
                break;
            }
        }
    } else {
        printf("unknown version\n");
        return -1;
    }
    return 0;

}
//./tar -c test tartest
// Demonstration of zlib utility functions
size_t file_size(char *filename)
{
    FILE *pFile = fopen(filename, "rb");
    fseek(pFile, 0, SEEK_END);
    size_t size = ftell(pFile);
    fclose(pFile);
    return size;
}


int decompress_one_file(char *infilename, char *outfilename)
{

    int num_read = 0;
    char buffer[128] = {0};

    gzFile infile = gzopen(infilename, "rb");
    FILE *outfile = fopen(outfilename, "wb");

    if (!infile || !outfile) {
        return -1;
    }

    while ((num_read = gzread(infile, buffer, sizeof(buffer))) > 0) {
        fwrite(buffer, 1, num_read, outfile);
        memset(buffer, 0, 128);
    }
    gzclose(infile);
    fclose(outfile);
    return 0;
}


int compress_one_file(char *infilename, char *outfilename)
{
    int num_read = 0;
    char inbuffer[128] = {0};
    unsigned long total_read = 0, total_wrote = 0;
    FILE *infile = fopen(infilename, "rb");
    gzFile outfile = gzopen(outfilename, "wb");
    if (!infile || !outfile) {
        return -1;
    }

    while ((num_read = fread(inbuffer, 1, sizeof(inbuffer), infile)) > 0) {
        total_read += num_read;
        gzwrite(outfile, inbuffer, num_read);
        memset(inbuffer, 0, 128);
    }
    fclose(infile);
    gzclose(outfile);
    printf("Read %ld bytes, Wrote %ld bytes,"
           "Compression factor %4.2f%%\n",
           total_read, file_size(outfilename),
           (1.0 - file_size(outfilename) * 1.0 / total_read) * 100.0);
    return 0;
}
int main(int argc, char **argv)
{
    int32_t rc = 0;
#if 0
    tar("URI_TMP", "Lhb.tar");
    compress_one_file("modir.tar.bz2", "LHB.BIN");
    decompress_one_file("LHB.BIN", "modir.tar.bz2.orig");
#else
    // uranium::FileManager fileManager("test");
    uranium::FileManager fileManager("URI_TMP_LHB");
    fileManager.construct();
    // fileManager.fileTarFromPath("lhb.tar.bz2");
    fileManager.fileUntarToPath("lhb.tar.bz2", "URI_TMP_LHB");
#endif
#if 0
    uranium::MonitorCore monitCore("/mnt/d/lenvov_wokspace/source/Uranium");

    monitCore.construct();
    rc = monitCore.monitorDirStart();
    std::cout << std::endl << "start return = " << rc << std::endl;
    //monitCore.monitorDirStop();
    monitCore.monitorLoopProcess();

    // monitCore.destruct();
    while (1) {
        sleep(1);
    }
#if 0
    fileMang.fileScanToInis();
    fileMang.fileInfosSave("/tmp/lhb.ini");
    fileMang.fileInfosLoad("/tmp/lhb.ini");
#endif

    std::string path_bash = "/mnt/d/lenvov_wokspace/source/Uranium/";
    std::string current_pathStr = p.string();
    std::string::size_type pos = current_pathStr.find(path_bash);
    pos += path_bash.length();
    std::cout << current_pathStr.substr(pos) << std::endl;

    std::cout << "base = " << current_pathStr << " " << "PATH=" << path_bash << std::endl;
    // p.root_path(p_bas);
    std::cout << "relative_path = " << p.relative_path() << std::endl;
    std::cout << "root path = " << p.root_path() << std::endl;
    std::cout << "root directory" << p.root_directory() << std::endl;
#endif
    return 0;
}
