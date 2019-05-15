#include "common.h"
#include "server_client_common.h"

namespace sirius {

#include <sys/socket.h>

const char *sc_type_str[] = {
    [SERVER] = "server",
    [CLIENT] = "client",
    [SC_INVALID_MAX] = "invalid",
};

const char *get_type(sc_type type)
{
    const char *ret = NULL;

    if (type >= 0 && type < SC_INVALID_MAX) {
        ret = sc_type_str[type];
    } else {
        ret = sc_type_str[SC_INVALID_MAX];
    }

    return ret;
}

int32_t sc_read_data(int32_t pairfd,
    char *dat, uint32_t max_len, int32_t *read_len, sc_type type)
{
    int32_t rc = NO_ERROR;

    struct iovec count_vec = {
        .iov_base = dat,
        .iov_len = max_len,
    };

    struct msghdr msg;
    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    msg.msg_control = NULL;
    msg.msg_controllen = 0;
    msg.msg_iov = &count_vec;
    msg.msg_iovlen = 1;
    msg.msg_flags = 0;

    *read_len = recvmsg(pairfd, &msg, 0);
    if (*read_len < 0) {
        LOGE(MODULE_SOCKET, "%s read data error, %s",
            get_type(type), strerror(errno));
        rc = SYS_ERROR;
    } else {
        dat[*read_len] = '\0'; // important
    }

    return rc;
}

int32_t sc_send_data(int32_t pairfd,
    char *dat, uint32_t len, sc_type type)
{
    int32_t rc = NO_ERROR;
    int32_t sent = 0;

    struct iovec count_vec = {
        .iov_base = dat,
        .iov_len = len,
    };

    struct msghdr msg;
    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    msg.msg_control = NULL;
    msg.msg_controllen = 0;
    msg.msg_iov = &count_vec;
    msg.msg_iovlen = 1;
    msg.msg_flags = 0;

    sent = sendmsg(pairfd, &msg, 0);
    if (sent < 0 || sent != (int32_t)len) {
        LOGE(MODULE_SOCKET, "%s failed to send data via %d, %d/%d bytes",
            get_type(type), pairfd, sent, len);
        rc = SYS_ERROR;
    }

    return rc;
}

int32_t sc_send_fd(int32_t pairfd, int32_t fd, sc_type type)
{
    int32_t rc = NO_ERROR;
    int32_t num_fd = 1;
    char buf[CMSG_SPACE(sizeof(int32_t))];

    struct iovec count_vec = {
        .iov_base = &num_fd,
        .iov_len = sizeof num_fd,
    };

    struct msghdr msg;
    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    msg.msg_control = buf,
    msg.msg_controllen = sizeof buf,
    msg.msg_iov = &count_vec,
    msg.msg_iovlen = 1,
    msg.msg_flags = 0;

    struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    cmsg->cmsg_len = CMSG_LEN(sizeof(int));
    *(int32_t *)CMSG_DATA(cmsg) = fd;

    if (sendmsg(pairfd, &msg, 0) < 0) {
        LOGE(MODULE_SOCKET, "%s failed to send fd %d, %s",
            get_type(type), pairfd, strerror(errno));
        rc = SYS_ERROR;
    }

    return rc;
}

int32_t sc_read_fd(int32_t pairfd, int32_t *fd, sc_type type)
{
    int32_t rc = NO_ERROR;
    int32_t num_fd = 1;
    char buf[CMSG_SPACE(sizeof(int32_t))];

    struct iovec count_vec = {
        .iov_base = &num_fd,
        .iov_len = sizeof num_fd,
    };

    struct msghdr msg;
    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    msg.msg_control = buf;
    msg.msg_controllen = sizeof(buf);
    msg.msg_iov = &count_vec;
    msg.msg_iovlen = 1;
    msg.msg_flags = 0;

    struct cmsghdr *cmsg;

    if (recvmsg(pairfd, &msg, 0) < 0) {
        LOGE(MODULE_SOCKET, "%d read data error, %s",
            get_type(type), strerror(errno));
        rc = SYS_ERROR;
    }

    if (SUCCEED(rc)) {
        cmsg = CMSG_FIRSTHDR(&msg);
        if (ISNULL(cmsg)) {
            LOGE(MODULE_SOCKET, "%s no cmsg received, %s",
                get_type(type), strerror(errno));
            rc = UNKNOWN_ERROR;
        }
    }

    if (SUCCEED(rc)) {
        *fd = *(int32_t*)CMSG_DATA(cmsg);
        if (*fd < 0) {
            LOGE(MODULE_SOCKET, "%s invalid fd %d read",
                get_type(type), fd);
            rc = SYS_ERROR;
        }
    }

    return rc;
}

}

