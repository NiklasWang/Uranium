#include <string>

#include "MonitorCore.h"
#include "md5.h"
int main(int argc, char **argv)
{
    int32_t rc = 0;
    uranium::FileManager fileManager("/home/binson/libarchive/");
    fileManager.construct();
    fileManager.fileTarFromPath("/tmp/test.tar.bz2");
    fileManager.fileUntarToPath("/tmp/test.tar.bz2");

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
