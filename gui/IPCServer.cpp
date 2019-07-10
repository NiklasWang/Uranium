#include <QtCore/QDebug>

#include "common.h"
#include "IPCServer.h"
#include "IPCInstruction.h"

namespace uranium {

int32_t IPCServer::construct()
{
    int32_t rc = NO_ERROR;

    if (mConstructed) {
        rc = ALREADY_INITED;
    }

    if (SUCCEED(rc)) {
        mSocketServer = new QTcpServer(this);
        if (ISNULL(mSocketServer)) {
            LOGE(mModule, "Failed to new QIPCServer");
            rc = NO_MEMORY;
        }
    }

    if (SUCCEED(rc)) {
        if (!mSocketServer->listen(QHostAddress::Any, mPort)) {
            LOGE(mModule, "Failed to listening on port %d, IPCServer", mPort);
            rc = UNKNOWN_ERROR;
        } else {
            connect(mSocketServer, SIGNAL(newConnection()),
                    this, SLOT(onNewConnection()));

            connect(mSocketServer, SIGNAL(acceptError(QAbstractSocket::SocketError)),
                    this, SLOT(onAcceptError(QAbstractSocket::SocketError)));

            LOGD(mModule, "Listening on port %d succeed, IPCServer", mPort);
        }
    }

    if (SUCCEED(rc)) {
        mConstructed = true;
    }

    return rc;
}

int32_t IPCServer::destruct()
{
    int32_t rc = NO_ERROR;

    if (!mConstructed) {
        rc = NOT_INITED;
    } else {
        mConstructed = false;
    }

    if (SUCCEED(rc)) {
        mSocketServer->close();
        qDeleteAll(mClients.begin(), mClients.end());
        mSocketServer->deleteLater();
    }

    return RETURNIGNORE(rc, NOT_INITED);
}

int32_t IPCServer::waitForReadyRead(int32_t ms)
{
    int32_t rc = NOT_READY;
    QTcpSocket *pSocket = nullptr;

    if (mClients.size() > 0) {
        rc = NO_ERROR;
    }

    if (SUCCEED(rc)) {
        pSocket = *(mClients.end() - 1);
        if (ISNULL(pSocket)) {
            rc = NOT_READY;
            LOGE(mModule, "Invalid socket client, nullptr");
        }
    }

    if (SUCCEED(rc)) {
        if (!pSocket->waitForReadyRead(ms)) {
            rc = TIMEDOUT;
            LOGE(mModule, "Failed to wait for ready read in %d ms", ms);
        }
    }

    return rc;
}

IPCServer::IPCServer(quint16 port,
    std::function<int32_t (const QByteArray &)> msgCb) :
    mConstructed(false),
    mModule(MODULE_IPC),
    mPort(port),
    mSocketServer(nullptr),
    mMsgCb(msgCb)
{
}

IPCServer::~IPCServer()
{
    if (mConstructed) {
        destruct();
    }
}

void IPCServer::onNewConnection()
{
    LOGD(mModule, "New socket connection.");

    QTcpSocket *pSocket = mSocketServer->nextPendingConnection();

    connect(pSocket, &QTcpSocket::readyRead,
            this, &IPCServer::onReadyRead);

    connect(pSocket, &QTcpSocket::disconnected,
            this, &IPCServer::socketDisconnected);

    mClients << pSocket;
}

int32_t IPCServer::onReadyRead()
{
    int32_t rc = NO_ERROR;
    QTcpSocket *pSocket = *(mClients.end() - 1);
    QByteArray data;

    if (SUCCEED(rc)) {
        if (ISNULL(pSocket)) {
            rc = UNKNOWN_ERROR;
            LOGE(mModule, "Invalid client ipc.");
        }
    }

    if (SUCCEED(rc)) {
        data = pSocket->readAll();
        if (data.size() == 0) {
            rc = UNKNOWN_ERROR;
            LOGE(mModule, "Socket ready to read but empty.");
        }
    }

    if (SUCCEED(rc)) {
        rc = processMessage(data);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to process data, %d", rc);
        }
    }

    return rc;
}

int32_t IPCServer::processMessage(const QByteArray &message)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        rc = newData(message);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to emit msg, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        rc = mMsgCb(message);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to process message in cb, %d", rc);
        }
    }

    QTcpSocket *pClient = qobject_cast<QTcpSocket *>(sender());
    if (pClient) {
        QString msg(message);
        msg.append(SUCCEED(rc) ? REPLY_SUCCEED : REPLY_FAILED);
        pClient->write(msg.toLatin1());
    }

    return rc;
}

void IPCServer::socketDisconnected()
{
    LOGD(mModule, "Socket disconnected.");

    QTcpSocket *pClient = qobject_cast<QTcpSocket *>(sender());
    if (pClient) {
        mClients.removeAll(pClient);
        pClient->deleteLater();
    }
}

void IPCServer::onAcceptError(QAbstractSocket::SocketError err)
{
    LOGE(mModule, "Accept client error, %d, %s", err);
    LOGE(mModule, "Possible raeson: %s.",
         NOTNULL(mSocketServer) ?
             mSocketServer->errorString().toLatin1().data() :
             "null");
}

}
