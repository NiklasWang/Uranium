#include "common.h"
#include "configuration.h"
#include "socket_server.h"

#define TEST_FILE_NAME "/tmp/uranium_shared_file"
#define SOCKET_NAME    "uranium_socket"

namespace uranium {

#include <sys/stat.h>
#include <fcntl.h>

int _main_server_tester()
{
    int32_t rc = NO_ERROR;
    int32_t filefd = -1;
    int32_t sockfd = -1;
    int32_t clientfd = -1;
    char data[SOCKET_DATA_MAX_LEN];
    int32_t read_len = 0;

    if (SUCCEED(rc)) {
        filefd = open(TEST_FILE_NAME, O_RDWR | O_CREAT | O_TRUNC, 0777);
        if (filefd < 0) {
            LOGE(MODULE_TESTER, "Failed to create file %s, %s",
                TEST_FILE_NAME, strerror(errno));
            rc = UNKNOWN_ERROR;
        }
    }

    if (SUCCEED(rc)) {
        rc = start_server(&sockfd, SOCKET_NAME);
        if (!SUCCEED(rc)) {
            LOGE(MODULE_TESTER, "Failed to start server, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        bool cancel = false;
        rc = poll_accept_wait(sockfd, &clientfd, &cancel);
        if (!SUCCEED(rc)) {
            LOGE(MODULE_TESTER, "Failed to poll data while sleeping, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        bool cancel = false;
        data[0] = '\0';
        rc = poll_read_wait(clientfd, data, sizeof(data), &read_len, &cancel);
        if (!SUCCEED(rc)) {
            LOGE(MODULE_TESTER, "Failed to poll data while sleeping, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        rc = sc_send_fd(clientfd, filefd);
        if (!SUCCEED(rc)) {
            LOGE(MODULE_TESTER, "Failed to send fd %d to client", filefd);
        }
    }

    if (SUCCEED(rc)) {
        bool cancel = false;
        data[0] = '\0';
        rc = poll_read_wait(clientfd, data, sizeof(data), &read_len, &cancel);
        if (!SUCCEED(rc)) {
            LOGE(MODULE_TESTER, "Failed to poll data while sleeping, %d", rc);
        }
    }

    if (SUCCEED(rc) || !SUCCEED(rc)) {
        if (filefd > 0) {
            close(filefd);
        }
        if (clientfd > 0) {
            disconnect_client(clientfd);
        }
        if (sockfd > 0) {
            stop_server(sockfd, SOCKET_NAME);
        }
    }

    return rc;
}

};

int main()
{
    return uranium::_main_server_tester();
}


