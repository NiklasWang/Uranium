#ifndef _SIRIUS_CLIENT_H_
#define _SIRIUS_CLIENT_H_

#include "configuration.h"
#include "server_client_common.h"

namespace sirius {

int32_t connect_to_server(int32_t *fd, const char *socketName = SERVER_SOCKET_NAME);

int32_t pull_server_fd(int32_t serverfd, int32_t *sharedfd);

int32_t poll_server_fd_wait(int32_t serverfd, int32_t *sharedfd, bool *cancel);

int32_t disconnect_server(int32_t serverfd);

};

#endif
