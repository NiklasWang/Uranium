#include <QtCore/QDebug>
#include <QtCore/QEventLoop>

#include "common.h"
#include "IPCServer.h"
#include "IPCInstruction.h"

namespace uranium {

void IPCServer::run()
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        rc = construct();
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to construct ipc server, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        QEventLoop loop;
        connect(this, SIGNAL(exitServer()), &loop, SLOT(quit()));
        loop.exec();
    }

    if (SUCCEED(rc)) {
        rc = destruct();
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to destruct ipc server, %d", rc);
        }
    }

    return;
}

int32_t IPCServer::construct()
{
    int32_t rc = NO_ERROR;

    if (mConstructed) {
        rc = ALREADY_INITED;
    }

    if (SUCCEED(rc)) {
        mSocketServer = new QTcpServer();
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
    QTcpSocket *pSocket = static_cast<QTcpSocket *>(sender());
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
        rc = mMsgCb(message);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to process message in cb, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        rc = newData(message);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to emit msg, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        QTcpSocket *pClient = qobject_cast<QTcpSocket *>(sender());
        if (pClient) {
            QString msg(message);
            msg.append(SUCCEED(rc) ? REPLY_SUCCEED : REPLY_FAILED);
            pClient->write(msg.toLatin1());
        }
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
