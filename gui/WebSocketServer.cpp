#include <QtCore/QDebug>
#include <QtWebSockets/qwebsocketserver.h>
#include <QtWebSockets/qwebsocket.h>

#include "common.h"
#include "WebSocketServer.h"
#include "IPCInstruction.h"

namespace uranium {

int32_t WebSocketServer::construct()
{
    int32_t rc = NO_ERROR;

    if (mConstructed) {
        rc = ALREADY_INITED;
    }

    if (SUCCEED(rc)) {
        mWebSocketServer = new QWebSocketServer(
            QStringLiteral("IPC Web Server"), QWebSocketServer::NonSecureMode, this);
        if (ISNULL(mWebSocketServer)) {
            LOGE(mModule, "Failed to new QWebSocketServer");
            rc = NO_MEMORY;
        }
    }

    if (SUCCEED(rc)) {
        if (!mWebSocketServer->listen(QHostAddress::LocalHost, mPort)) {
            LOGE(mModule, "Failed to listening on port %d, WebSocketServer", mPort);
            rc = UNKNOWN_ERROR;
        } else {
            connect(mWebSocketServer, &QWebSocketServer::newConnection,
                    this, &WebSocketServer::onNewConnection);

            connect(mWebSocketServer, &QWebSocketServer::closed,
                    this, &WebSocketServer::closed);

            LOGD(mModule, "Listening on port %d succeed, WebSocketServer", mPort);
        }
    }

    if (SUCCEED(rc)) {
        mConstructed = true;
    }

    return rc;
}

int32_t WebSocketServer::destruct()
{
    int32_t rc = NO_ERROR;

    if (!mConstructed) {
        rc = NOT_INITED;
    } else {
        mConstructed = false;
    }

    if (SUCCEED(rc)) {
        mWebSocketServer->close();
        qDeleteAll(mClients.begin(), mClients.end());
    }

    return RETURNIGNORE(rc, NOT_INITED);
}

WebSocketServer::WebSocketServer(quint16 port,
    std::function<int32_t (const QString &)> textMsgCb,
    std::function<int32_t (const QByteArray &)> BinMsgCb) :
    mConstructed(false),
    mModule(MODULE_SOCKET),
    mPort(port),
    mWebSocketServer(nullptr),
    mTextMsgCb(textMsgCb),
    mBinMsgCb(BinMsgCb)
{
}

WebSocketServer::~WebSocketServer()
{
    if (mConstructed) {
        destruct();
    }
}

void WebSocketServer::onNewConnection()
{
    QWebSocket *pSocket = mWebSocketServer->nextPendingConnection();

    connect(pSocket, &QWebSocket::textMessageReceived,
            this, &WebSocketServer::processTextMessage);

    connect(pSocket, &QWebSocket::binaryMessageReceived,
            this, &WebSocketServer::processBinaryMessage);

    connect(pSocket, &QWebSocket::disconnected,
            this, &WebSocketServer::socketDisconnected);

    mClients << pSocket;
}

void WebSocketServer::processTextMessage(QString message)
{
     int32_t rc = mTextMsgCb(message);

     QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
     if (pClient) {
         pClient->sendTextMessage(message + (SUCCEED(rc) ? REPLY_SUCCEED : REPLY_FAILED));
     }
}

void WebSocketServer::processBinaryMessage(QByteArray message)
{
    int32_t rc = mBinMsgCb(message);

    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if (pClient) {
        pClient->sendTextMessage(message + (SUCCEED(rc) ? REPLY_SUCCEED : REPLY_FAILED));
    }
}

void WebSocketServer::socketDisconnected()
{
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if (pClient) {
        mClients.removeAll(pClient);
        pClient->deleteLater();
    }
}

}
