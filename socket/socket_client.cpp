#include <sys/socket.h>
#include <sys/un.h>
#include <poll.h>

#include "common.h"
#include "configuration.h"
#include "socket_client.h"

namespace sirius {

int32_t connect_to_server(int32_t *fd, const char *socketName)
{
    struct sockaddr_un server_addr;
    socklen_t addr_len;
    int sockfd = -1;
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (sockfd < 0) {
            LOGE(MODULE_SOCKET_CLIENT,
                "Failed to open listening socket for server, %s",
                strerror(errno));
            rc = SYS_ERROR;
        }
    }

    if (SUCCEED(rc)) {
        memset((unsigned char *)&server_addr, 0, sizeof(server_addr));
        server_addr.sun_family = AF_UNIX;
        strcpy(server_addr.sun_path, SERVER_SOCKET_PATH);
        strcat(server_addr.sun_path, socketName);
        addr_len = strlen(server_addr.sun_path) + sizeof(server_addr.sun_family);
    }

    if (SUCCEED(rc)) {
        rc = connect(sockfd, (struct sockaddr *)&server_addr, addr_len);
        if (rc != 0) {
            rc = NOT_READY;
            LOGD(MODULE_SOCKET_CLIENT, "Client connect to server "
                "socket %s failed, %s, may not started",
                server_addr.sun_path, strerror(errno));
        } else {
            LOGD(MODULE_SOCKET_CLIENT, "Client connected to server "
                "socket", server_addr.sun_path);
        }
    }

    if (SUCCEED(rc)) {
        *fd = sockfd;
    }

    if (!SUCCEED(rc)) {
        if (sockfd != -1) {
            close(sockfd);
        }
    }

    return rc;
}

int32_t poll_server_fd(int32_t serverfd, int32_t *sharedfd)
{
    int32_t rc = NO_ERROR;
    struct pollfd connected_pollfd;
    connected_pollfd.fd = serverfd;
    connected_pollfd.events = POLLIN | POLLPRI;

    rc = poll(&connected_pollfd, 1, CLIENT_POLL_DATA_TIMEOUT);
    if (rc == -1) {
        LOGE(MODULE_SOCKET_CLIENT, "Client poll data error, %s", strerror(errno));
        rc = SYS_ERROR;
    } else if (rc == 0) {
        // Client poll data timeout, check if need to exit outside
        rc = TIMEDOUT;
    } else {
        if (connected_pollfd.revents & POLLHUP) {
            LOGE(MODULE_SOCKET_CLIENT, "Client poll data return, POLLHUP");
            rc = UNKNOWN_ERROR;
        } else if (connected_pollfd.revents & POLLPRI) {
            LOGE(MODULE_SOCKET_CLIENT, "Client poll data return, POLLPRI");
            rc = UNKNOWN_ERROR;
        } else if (connected_pollfd.revents & POLLIN) {
            rc = sc_read_fd(serverfd, sharedfd, CLIENT);
            if (!SUCCEED(rc)) {
                LOGE(MODULE_SOCKET_CLIENT,
                    "Client failed to read server fd, %d", rc);
            }
        }
    }

    return rc;
}

int32_t poll_server_fd_wait(int32_t serverfd,
    int32_t *sharedfd, bool *cancel)
{
    int32_t rc = NO_ERROR;

    do {
        rc = poll_server_fd(serverfd, sharedfd);
        if (SUCCEED(rc)) {
            break;
        } else if (rc != TIMEDOUT) {
            LOGE(MODULE_SOCKET_CLIENT, "Failed to poll data from client, %d", rc);
            break;
        }
        if (*cancel) {
            LOGI(MODULE_SOCKET_SERVER, "Cancelled to wait server fd");
            break;
        }
    } while (true);

    return rc;
}

int32_t disconnect_server(int32_t serverfd)
{
    shutdown(serverfd, SHUT_RDWR);
    close(serverfd);

    return NO_ERROR;
}

};
