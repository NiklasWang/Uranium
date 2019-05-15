#ifndef SIRIUS_SERVER_CLIENT_COMMON_H_
#define SIRIUS_SERVER_CLIENT_COMMON_H_

namespace sirius {

#include <stdint.h>

enum sc_type {
    SERVER,
    CLIENT,
    SC_INVALID_MAX,
};

int32_t sc_read_data(int32_t pairfd,
    char *dat, uint32_t max_len, int32_t *read_len, sc_type type = SERVER);

int32_t sc_send_data(int32_t pairfd,
    char *dat, uint32_t len, sc_type type = SERVER);

int32_t sc_send_fd(int32_t pairfd, int32_t fd, sc_type type = SERVER);

int32_t sc_read_fd(int32_t pairfd, int32_t *fd, sc_type type = SERVER);

};

#endif
