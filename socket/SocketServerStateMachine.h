#ifndef _SIRIUS_SOCKET_SERVER_STATE_MACHINE__
#define _SIRIUS_SOCKET_SERVER_STATE_MACHINE__

#include "common.h"
#include "configuration.h"
#include "SyncType.h"
#include "ThreadT.h"

namespace sirius {

class SocketServerStateMachine :
    public noncopyable {
public:
    int32_t startServer();
    int32_t waitForConnect();
    int32_t waitForConnect(int32_t *clientfd);
    int32_t receiveMsg(char *data, int32_t maxlen);
    int32_t receiveMsg(int32_t clientfd, char *data, int32_t maxlen);
    int32_t sendMsg(const char *data, int32_t len);
    int32_t sendMsg(int32_t clientfd, const char *data, int32_t len);
    int32_t receiveFd(int32_t *fd);
    int32_t sendFd(int32_t fd);

    bool connected();
    int32_t cancelWaitConnect();
    bool waitingMsg();
    int32_t cancelWaitMsg();
    int32_t setClientFd(int32_t fd);

private:
    enum cmd_type {
        CMD_START_SERVER,
        CMD_WAIT_CONNECTION,
        CMD_RECEIVE_MSG,
        CMD_SEND_MSG,
        CMD_RECEIVE_FD,
        CMD_SEND_FD,
    };

    enum status {
        STATUS_UNINITED,
        STATUS_INITED,
        STATUS_STARTED,
        STATUS_ACCEPTED_CLIENT,
        STATUS_RECEIVING_MSG,
        STATUS_SENDING_MSG,
        STATUS_RECEIVING_FD,
        STATUS_SENDING_FD,
    };

    struct msg_info {
        char *msg;
        int32_t len;
        int32_t max_len;
        int32_t fd;
    };

    struct cmd_info {
        cmd_type cmd;
        union {
            msg_info *msg;
            int32_t *fd;
        } u;
        int32_t  rc;
        SyncType sync;
    };

    int32_t processTask(cmd_info *dat);
    int32_t taskDone(cmd_info *dat, int32_t rc);

    int32_t stateMachine(cmd_type cmd, void *arg);
    int32_t procCmdUninitedState(cmd_type cmd, void *arg);
    int32_t procCmdInitedState(cmd_type cmd, void *arg);
    int32_t procCmdStartedState(cmd_type cmd, void *arg);
    int32_t procCmdAcceptedClientState(cmd_type cmd, void *arg);
    int32_t procCmdReceivingMsgState(cmd_type cmd, void *arg);
    int32_t procCmdSendingMsgState(cmd_type cmd, void *arg);
    int32_t procCmdReceivingFdState(cmd_type cmd, void *arg);
    int32_t procCmdSendingFdState(cmd_type cmd, void *arg);

    const char *stateName(status stat);
    const char *cmdName(cmd_type cmd);
    void logInvalidCmd(cmd_type cmd);
    void updateToNewStatus(status state);

public:
    SocketServerStateMachine(const char *socketName = SERVER_SOCKET_NAME);
    virtual ~SocketServerStateMachine();
    SocketServerStateMachine(const SocketServerStateMachine &rhs);
    SocketServerStateMachine &operator=(const SocketServerStateMachine &rhs);
    int32_t construct();
    int32_t destruct();

private:
    typedef int32_t (SocketServerStateMachine::*SMFunc)(cmd_type cmd, void *arg);
    int32_t executeOnThread(cmd_info *task);

private:
    bool    mConstructed;
    bool    mOwnServer;
    int32_t mServerFd;
    int32_t mClientFd;
    status  mStatus;
    bool    mWaitingMsg;
    bool    mCancelConnect;
    bool    mCancelMsg;
    ModuleType  mModule;
    const char *mSocketName;
    ThreadT<cmd_info> mThread;

private:
    static const SMFunc kFunc[];
    static const char * const kStateStr[];
    static const char * const kCmdStr[];
};

};

#endif
