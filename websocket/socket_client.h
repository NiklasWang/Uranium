#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "configuration.h"
#include "server_client_common.h"

namespace uranium {

int32_t connect_to_server(int32_t *fd, int32_t port);

int32_t pull_server_fd(int32_t serverfd, int32_t *sharedfd);

int32_t poll_server_fd_wait(int32_t serverfd, int32_t *sharedfd, bool *cancel);

int32_t disconnect_server(int32_t serverfd);

};

#endif
