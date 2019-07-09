#include "common.h"
#include "IPCClient.h"

#include <QtCore/QDebug>

namespace uranium {

IPCClient::IPCClient(QString ip, uint16_t port,
    std::function<int32_t (const QByteArray &)> msgCb) :
    mModule(MODULE_IPC),
    mIp(ip),
    mPort(port),
    mReady(false),
    mMsgCb(msgCb)
{
    connect(&mSocket, SIGNAL(connected()),
            this, SLOT(onConnected()));

    connect(&mSocket, SIGNAL(disconnected()),
            this, SIGNAL(disconnected()));

    connect(&mSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(onError(QAbstractSocket::SocketError)));
}

int32_t IPCClient::construct()
{
    int32_t rc = NO_ERROR;

    if (mConstructed) {
        rc = ALREADY_INITED;
    }

    if (SUCCEED(rc)) {
        mSocket.abort();
        mSocket.connectToHost(mIp, mPort);
    }

    if (SUCCEED(rc)) {
        rc = mSocket.waitForConnected() ? NO_ERROR : NOT_READY;
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to connect to ipc server, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        mReady = true;
        mConstructed = true;
    }

    return rc;
}

int32_t IPCClient::destruct()
{
    int32_t rc = NO_ERROR;

    if (!mConstructed) {
        rc = NOT_INITED;
    } else {
        mConstructed = false;
    }

    if (SUCCEED(rc)) {
        mSocket.disconnectFromHost();
        mSocket.close();
    }

    return RETURNIGNORE(rc, NOT_INITED);
}

IPCClient::~IPCClient()
{
    if (mConstructed) {
        destruct();
    }
}

void IPCClient::onConnected()
{
    LOGD(mModule, "Web socket connected to client.");

    connect(&mSocket, SIGNAL(readyRead()),
            this, SLOT(onReadyRead()));
}

int32_t IPCClient::onReadyRead()
{
    int32_t rc = NO_ERROR;
    QByteArray data;

    if (SUCCEED(rc)) {
        data = mSocket.readAll();
        if (data.size() == 0) {
            rc = UNKNOWN_ERROR;
            LOGE(mModule, "Socket ready to read but empty.");
        }
    }

    if (SUCCEED(rc)) {
        rc = onDataReceived(data);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to process data, %d", rc);
        }
    }

    return rc;
}

int32_t IPCClient::send(const QByteArray &data)
{
    int32_t rc = NO_ERROR;

    if (!mReady) {
        rc = NOT_INITED;
        LOGE(mModule, "Not connect to ipc server yet.");
    }

    if (SUCCEED(rc)) {
        int64_t size = mSocket.write(data);
        if (size != data.size()) {
            rc = UNKNOWN_ERROR;
            LOGE(mModule, "Failed to send all data, %d/%d", size, data.size());
        }
    }

    if (SUCCEED(rc)) {
        if (!mSocket.waitForBytesWritten()) {
            LOGE(mModule, "Failed to wait msg written.");
        }
    }

    return rc;
}

int32_t IPCClient::onDataReceived(const QByteArray &data)
{
    return mMsgCb(data);
}

void IPCClient::onError(QAbstractSocket::SocketError error)
{
    LOGE(MODULE_SOCKET, "Socket client error: %d", error);
}

}
