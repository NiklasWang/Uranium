#include "../common.h"
#include "../configuration.h"
#include "../client.h"

#undef SERVER_SOCKET_PATH
#define SERVER_SOCKET_PATH "/tmp/"

namespace sirius {

#define GREETING_STR "Hello, sirius."
#define GOODBYE_STR  "Bye, sirius."
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
            LOGE("Failed to connect server, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        strcpy(data, GREETING_STR);
        rc = sc_send_data(sockfd, data, strlen(data) + 1, CLIENT);
        if (!SUCCEED(rc)) {
            LOGE("Failed to send data %s to server, %d", data, rc);
        }
    }

    if (SUCCEED(rc)) {
        rc = poll_server_fd_wait(sockfd, &sharedfd);
        if (!SUCCEED(rc)) {
            LOGE("Failed to poll fd while sleeping, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        strcpy(data, GOODBYE_STR);
        rc = sc_send_data(sockfd, data, strlen(data) + 1, CLIENT);
        if (!SUCCEED(rc)) {
            LOGE("Failed to send data %s to server, %d", data, rc);
        }
    }

    if (SUCCEED(rc)) {
        strcpy(data, WRITE_SHARED_FD_STR);
        write_len = write(sharedfd, data, strlen(data));
        if (write_len < 0) {
            LOGE("Failed to write shared fd, %s", strerror(errno));
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
    return sirius::_main_client_tester();
}

