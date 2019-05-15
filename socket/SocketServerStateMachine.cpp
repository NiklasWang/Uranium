#include "SocketServerStateMachine.h"
#include "socket_server.h"
#include "configuration.h"

namespace sirius {


int32_t SocketServerStateMachine::startServer()
{
    return stateMachine(CMD_START_SERVER, NULL);
}

int32_t SocketServerStateMachine::waitForConnect()
{
    return stateMachine(CMD_WAIT_CONNECTION, &mClientFd);
}

int32_t SocketServerStateMachine::waitForConnect(int32_t *clientFd)
{
    return stateMachine(CMD_WAIT_CONNECTION, clientFd);
}

int32_t SocketServerStateMachine::receiveMsg(char *data, int32_t maxlen)
{
    mWaitingMsg = true;
    msg_info msg = {
        .msg = data,
        .len = 0,
        .max_len  = maxlen,
        .fd  = mClientFd,
    };

    return stateMachine(CMD_RECEIVE_MSG, &msg);
}

int32_t SocketServerStateMachine::receiveMsg(int32_t clientfd, char *data, int32_t maxlen)
{
    mWaitingMsg = true;
    msg_info msg = {
        .msg = data,
        .len = 0,
        .max_len  = maxlen,
        .fd  = clientfd,
    };

    return stateMachine(CMD_RECEIVE_MSG, &msg);
}

int32_t SocketServerStateMachine::sendMsg(const char *data, int32_t msglen)
{
    msg_info msg = {
        .msg = const_cast<char *>(data),
        .len = msglen,
        .max_len = msglen,
        .fd  = mClientFd,
    };
    return stateMachine(CMD_SEND_MSG, &msg);
}

int32_t SocketServerStateMachine::sendMsg(int32_t clientfd, const char *data, int32_t msglen)
{
    msg_info msg = {
        .msg = const_cast<char *>(data),
        .len = msglen,
        .max_len = msglen,
        .fd  = clientfd,
    };
    return stateMachine(CMD_SEND_MSG, &msg);
}

int32_t SocketServerStateMachine::receiveFd(int32_t *fd)
{
    return stateMachine(CMD_RECEIVE_FD, fd);
}

int32_t SocketServerStateMachine::sendFd(int32_t fd)
{
    int32_t copied_fd = fd;
    return stateMachine(CMD_SEND_FD, &copied_fd);
}

bool SocketServerStateMachine::connected()
{
    return mClientFd != -1;
}

int32_t SocketServerStateMachine::setClientFd(int32_t fd)
{
    int32_t rc = NOT_INITED;

    mClientFd = fd;

    if (mClientFd != -1) {
        updateToNewStatus(STATUS_STARTED);
    }

    if (mStatus == STATUS_STARTED) {
        updateToNewStatus(STATUS_ACCEPTED_CLIENT);
        rc = NO_ERROR;
    }

    return rc;
}

int32_t SocketServerStateMachine::cancelWaitConnect()
{
    mCancelConnect = true;
    return NO_ERROR;
}

bool SocketServerStateMachine::waitingMsg()
{
    return mWaitingMsg;
}

int32_t SocketServerStateMachine::cancelWaitMsg()
{
    mCancelMsg = true;
    return NO_ERROR;
}

SocketServerStateMachine::SocketServerStateMachine(
    const char *socketName) :
    mConstructed(false),
    mOwnServer(false),
    mServerFd(-1),
    mClientFd(-1),
    mStatus(STATUS_UNINITED),
    mWaitingMsg(false),
    mCancelConnect(false),
    mModule(MODULE_SOCKET_SERVER_SM),
    mSocketName(socketName),
    mThread(getModuleName(mModule))
{
}

SocketServerStateMachine::~SocketServerStateMachine()
{
    if (mConstructed) {
        destruct();
    }
}

int32_t SocketServerStateMachine::construct()
{
    int32_t rc = NO_ERROR;

    if (mConstructed) {
        rc = ALREADY_INITED;
    }

    if (SUCCEED(rc)) {
        rc = mThread.construct();
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to construct thread");
        }
    }

    if (SUCCEED(rc)) {
        mStatus = STATUS_INITED;
        mConstructed = true;
    }

    return RETURNIGNORE(rc, ALREADY_INITED);
}

int32_t SocketServerStateMachine::destruct()
{
    int32_t rc = NO_ERROR;

    if (!mConstructed) {
        rc = NOT_INITED;
    } else {
        mConstructed = false;
    }

    if (SUCCEED(rc)) {
        rc = cancelWaitMsg();
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to cancel wait msg, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        rc = cancelWaitConnect();
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to cancel wait client, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        rc = mThread.destruct();
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to destruct thread, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        if (mClientFd > 0) {
            disconnect_client(mClientFd);
            mClientFd = -1;
        }
        if (mOwnServer && mServerFd > 0) {
            stop_server(mServerFd, mSocketName);
            mServerFd = -1;
        }
    }

    return RETURNIGNORE(rc, NOT_INITED);
}

int32_t SocketServerStateMachine::processTask(cmd_info *info)
{
    int32_t rc = NO_ERROR;

    switch (info->cmd) {
        case CMD_START_SERVER: {
            rc = start_server(&mServerFd, mSocketName);
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to start server, %d", rc);
            } else {
                mOwnServer = true;
            }
        } break;
        case CMD_WAIT_CONNECTION: {
            mCancelConnect = false;
            rc = poll_accept_wait(mServerFd, info->u.fd, &mCancelConnect);
            if (rc == USER_ABORTED) {
                LOGI(mModule, "Cancelled to wait connection.");
            } else if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to poll data while sleeping, %d", rc);
            }
        } break;
        case CMD_RECEIVE_MSG: {
            mCancelMsg   = false;
            char *data   = info->u.msg->msg;
            int32_t *len = &info->u.msg->len;
            int32_t max_len = info->u.msg->max_len;
            int32_t fd   = info->u.msg->fd;
            data[0] = '\0';
            rc = poll_read_wait(fd, data, max_len, len, &mCancelMsg);
            if (rc == USER_ABORTED) {
                LOGI(mModule, "Cancelled to read msg from client.");
            } else if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to poll data while sleeping, %d", rc);
            }
            mWaitingMsg = false;
        } break;
        case CMD_SEND_MSG: {
            char *data = info->u.msg->msg;
            int32_t len = info->u.msg->len;
            int32_t fd   = info->u.msg->fd;
            rc = sc_send_data(fd, data, len);
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to send data, %d", rc);
            }
        } break;
        case CMD_RECEIVE_FD: {
            // TODO:: not needed currently
        } break;
        case CMD_SEND_FD: {
            int32_t fd = *info->u.fd;
            rc = sc_send_fd(mClientFd, fd);
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to send fd %d to client", fd);
            }
        } break;
        default: {
            LOGE(mModule, "Invalid cmd %d", info->cmd);
        } break;
    }

    return rc;
}

int32_t SocketServerStateMachine::taskDone(cmd_info *info, int32_t result)
{
    int32_t rc = NO_ERROR;
    rc = info->rc = result;

    if (info->sync == SYNC_TYPE) {
        info->sync.signal();
    }

    return rc;
}

const SocketServerStateMachine::SMFunc
    SocketServerStateMachine::kFunc[] = {
    [STATUS_UNINITED]        = &SocketServerStateMachine::procCmdUninitedState,
    [STATUS_INITED]          = &SocketServerStateMachine::procCmdInitedState,
    [STATUS_STARTED]         = &SocketServerStateMachine::procCmdStartedState,
    [STATUS_ACCEPTED_CLIENT] = &SocketServerStateMachine::procCmdAcceptedClientState,
    [STATUS_RECEIVING_MSG]   = &SocketServerStateMachine::procCmdReceivingMsgState,
    [STATUS_SENDING_MSG]     = &SocketServerStateMachine::procCmdSendingMsgState,
    [STATUS_RECEIVING_FD]    = &SocketServerStateMachine::procCmdReceivingFdState,
    [STATUS_SENDING_FD]      = &SocketServerStateMachine::procCmdSendingFdState,
};

int32_t SocketServerStateMachine::stateMachine(cmd_type cmd, void *arg)
{
    return (this->*(kFunc[mStatus]))(cmd, arg);
}

int32_t SocketServerStateMachine::executeOnThread(
    SocketServerStateMachine::cmd_info *task)
{
    int32_t rc = mThread.runWait(
        [this](cmd_info *_task) -> int32_t {
            int32_t _rc = processTask(_task);
            taskDone(_task, _rc);
            return _rc;
        },
        task
    );
    if (!SUCCEED(rc)) {
        LOGE(mModule, "Failed to %s on status %s, %d",
            cmdName(task->cmd), stateName(mStatus), rc);
    }

    return rc;
}

const char * const SocketServerStateMachine::kStateStr[] = {
    [STATUS_UNINITED]        = "uninited",
    [STATUS_INITED]          = "inited",
    [STATUS_STARTED]         = "server started",
    [STATUS_ACCEPTED_CLIENT] = "connect client",
    [STATUS_RECEIVING_MSG]   = "receiving message",
    [STATUS_SENDING_MSG]     = "sending message",
    [STATUS_RECEIVING_FD]    = "receiving message",
    [STATUS_SENDING_FD]      = "sending message"
};

const char * const SocketServerStateMachine::kCmdStr[] = {
    [CMD_START_SERVER]    = "start server",
    [CMD_WAIT_CONNECTION] = "parpare accept client",
    [CMD_RECEIVE_MSG]     = "receive message",
    [CMD_SEND_MSG]        = "send message",
    [CMD_RECEIVE_FD]      = "receive fd",
    [CMD_SEND_FD]         = "send fd",
};

const char *SocketServerStateMachine::stateName(
    SocketServerStateMachine::status state)
{
    return kStateStr[
        (state < 0 || state > STATUS_SENDING_FD) ?
        STATUS_SENDING_FD : state];
}

const char *SocketServerStateMachine::cmdName(
    SocketServerStateMachine::cmd_type type)
{
    return kCmdStr[
        (type < 0 || type > CMD_SEND_FD) ?
        CMD_SEND_FD : type];
}

void SocketServerStateMachine::logInvalidCmd(cmd_type cmd)
{
    LOGE(mModule, "Invalid cmd %s in %s state", cmdName(cmd), stateName(mStatus));
}

void SocketServerStateMachine::updateToNewStatus(status state)
{
    mStatus = state;
}

int32_t SocketServerStateMachine::procCmdUninitedState(
    cmd_type cmd, void * /*arg*/)
{
    logInvalidCmd(cmd);

    return NOT_INITED;
}

int32_t SocketServerStateMachine::procCmdInitedState(
    cmd_type cmd, void * /*arg*/)
{
    int32_t rc = NO_ERROR;

    switch (cmd) {
        case CMD_START_SERVER: {
            cmd_info info;
            info.cmd = cmd;
            rc = executeOnThread(&info);
            if (SUCCEED(rc)) {
                info.sync.wait();
                rc = info.rc;
                if (SUCCEED(rc)) {
                    updateToNewStatus(STATUS_STARTED);
                    LOGD(mModule, "Socket server started.");
                } else {
                    LOGE(mModule, "Failed to start socket server, %d", rc);
                }
            }
        }; break;
        default: {
            logInvalidCmd(cmd);
        } break;
    }

    return rc;
}

int32_t SocketServerStateMachine::procCmdStartedState(
    cmd_type cmd, void *arg)
{
    int32_t rc = NO_ERROR;

    switch (cmd) {
        case CMD_WAIT_CONNECTION: {
            cmd_info info;
            info.cmd  = cmd;
            info.u.fd = (int32_t *)arg;
            rc = executeOnThread(&info);
            if (SUCCEED(rc)) {
                info.sync.wait();
                rc = info.rc;
                if (SUCCEED(rc)) {
                    updateToNewStatus(STATUS_ACCEPTED_CLIENT);
                    LOGD(mModule, "Client connect to server.");
                } else {
                    LOGE(mModule, "Failed to connect client.");
                }
            }
        }; break;
        default: {
            logInvalidCmd(cmd);
        } break;
    }

    return rc;
}

int32_t SocketServerStateMachine::procCmdAcceptedClientState(
    cmd_type cmd, void *arg)
{
    int32_t rc = NO_ERROR;

    switch (cmd) {
        case CMD_WAIT_CONNECTION: {
            cmd_info info;
            info.cmd  = cmd;
            info.u.fd = (int32_t *)arg;
            rc = executeOnThread(&info);
            if (SUCCEED(rc)) {
                info.sync.wait();
                rc = info.rc;
                if (SUCCEED(rc)) {
                    updateToNewStatus(STATUS_ACCEPTED_CLIENT);
                    LOGD(mModule, "Client connect to server.");
                } else {
                    LOGE(mModule, "Failed to connect client.");
                }
            }
        }; break;
        case CMD_RECEIVE_MSG: {
            updateToNewStatus(STATUS_RECEIVING_MSG);
            cmd_info info;
            info.cmd = cmd;
            info.u.msg = (msg_info *)arg;
            rc = executeOnThread(&info);
            if (SUCCEED(rc)) {
                info.sync.wait();
                rc = info.rc;
                if (!SUCCEED(rc)) {
                    LOGE(mModule, "Failed to receive msg from client.");
                }
            }
            updateToNewStatus(STATUS_ACCEPTED_CLIENT);
        }; break;
        case CMD_SEND_MSG: {
            updateToNewStatus(STATUS_SENDING_MSG);
            cmd_info info;
            info.cmd = cmd;
            info.u.msg = (msg_info *)arg;
            rc = executeOnThread(&info);
            if (SUCCEED(rc)) {
                info.sync.wait();
                rc = info.rc;
                if (!SUCCEED(rc)) {
                    LOGE(mModule, "Failed to send msg to client.");
                }
            }
            updateToNewStatus(STATUS_ACCEPTED_CLIENT);
        }; break;
        case CMD_RECEIVE_FD: {
            updateToNewStatus(STATUS_RECEIVING_FD);

            cmd_info info;
            info.cmd = cmd;
            info.u.fd = (int32_t *)arg;
            rc = executeOnThread(&info);
            if (SUCCEED(rc)) {
                info.sync.wait();
                rc = info.rc;
                if (!SUCCEED(rc)) {
                    LOGE(mModule, "Failed to receive fd from client.");
                }
            }
            updateToNewStatus(STATUS_ACCEPTED_CLIENT);
        }; break;
        case CMD_SEND_FD: {
            updateToNewStatus(STATUS_SENDING_FD);

            cmd_info info;
            info.cmd = cmd;
            info.u.fd = (int32_t *)arg;
            rc = executeOnThread(&info);
            if (SUCCEED(rc)) {
                info.sync.wait();
                rc = info.rc;
                if (!SUCCEED(rc)) {
                    LOGE(mModule, "Failed to send fd to client.");
                }
            }
            updateToNewStatus(STATUS_ACCEPTED_CLIENT);
        }; break;
        default: {
            logInvalidCmd(cmd);
        } break;
    }

    return rc;
}

int32_t SocketServerStateMachine::procCmdReceivingMsgState(
    cmd_type cmd, void *arg)
{
    int32_t rc = NO_ERROR;

    LOGE(mModule, "Invalid cmd %s in %s state arg addr %p", cmdName(cmd), stateName(mStatus), arg);

    return rc;
}

int32_t SocketServerStateMachine::procCmdSendingMsgState(
    cmd_type cmd, void *arg)
{
    int32_t rc = NO_ERROR;

    LOGE(mModule, "Invalid cmd %s in %s state arg addr %p", cmdName(cmd), stateName(mStatus), arg);

    return rc;
}

int32_t SocketServerStateMachine::procCmdReceivingFdState(
    cmd_type cmd, void *arg)
{
    int32_t rc = NO_ERROR;

    LOGE(mModule, "Invalid cmd %s in %s state arg addr %p", cmdName(cmd), stateName(mStatus), arg);

    return rc;
}

int32_t SocketServerStateMachine::procCmdSendingFdState(
    cmd_type cmd, void *arg)
{
    int32_t rc = NO_ERROR;

    LOGE(mModule, "Invalid cmd %s in %s state arg addr %p", cmdName(cmd), stateName(mStatus), arg);

    return rc;
}

SocketServerStateMachine::SocketServerStateMachine(
    const SocketServerStateMachine &rhs)
{
    *this = rhs;
}

SocketServerStateMachine &SocketServerStateMachine::operator=(
    const SocketServerStateMachine &rhs)
{
    mConstructed = false;
    mStatus = STATUS_UNINITED;
    mCancelConnect = false;
    mModule = rhs.mModule;
    mClientFd = -1;
    mSocketName = rhs.mSocketName;

    if (!SUCCEED(construct())) {
        LOGE(mModule, "Failed to construct while copy construction");
    }

    mStatus = rhs.mStatus;

    return *this;
}

};

