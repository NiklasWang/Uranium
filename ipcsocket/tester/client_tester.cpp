#include "common.h"
#include "configuration.h"
#include "socket_client.h"

namespace uranium {

#define GREETING_STR "Hello, uranium."
#define GOODBYE_STR  "Bye, uranium."
#define WRITE_SHARED_FD_STR "I'm client and I'm writting."

int _main_client_tester()
{
    int32_t rc = NO_ERROR;
    int32_t sharedfd = -1;
    int32_t sockfd = -1;
    char data[SOCKET_DATA_MAX_LEN];
    int32_t write_len = 0;

    if (SUCCEED(rc)) {
        rc = connect_to_server(&sockfd);
        if (!SUCCEED(rc)) {
            LOGE(MODULE_TESTER, "Failed to connect server, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        strcpy(data, GREETING_STR);
        rc = sc_send_data(sockfd, data, strlen(data) + 1, CLIENT);
        if (!SUCCEED(rc)) {
            LOGE(MODULE_TESTER, "Failed to send data %s to server, %d", data, rc);
        }
    }

    if (SUCCEED(rc)) {
        bool cancel = false;
        rc = poll_server_fd_wait(sockfd, &sharedfd, &cancel);
        if (!SUCCEED(rc)) {
            LOGE(MODULE_TESTER, "Failed to poll fd while sleeping, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        strcpy(data, GOODBYE_STR);
        rc = sc_send_data(sockfd, data, strlen(data) + 1, CLIENT);
        if (!SUCCEED(rc)) {
            LOGE(MODULE_TESTER, "Failed to send data %s to server, %d", data, rc);
        }
    }

    if (SUCCEED(rc)) {
        strcpy(data, WRITE_SHARED_FD_STR);
        write_len = write(sharedfd, data, strlen(data));
        if (write_len < 0) {
            LOGE(MODULE_TESTER, "Failed to write shared fd, %s", strerror(errno));
            rc = UNKNOWN_ERROR;
        }
    }

    if (SUCCEED(rc) || !SUCCEED(rc)) {
        if (sharedfd > 0) {
            close(sharedfd);
        }

        if (sockfd > 0) {
            disconnect_server(sockfd);
        }
    }

    return rc;
}

}

int main()
{
    return uranium::_main_client_tester();
}

