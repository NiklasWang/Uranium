#include "common.h"
#include "WebSocketClient.h"

#include <QtCore/QDebug>

namespace uranium {

WebSocketClient::WebSocketClient(const QUrl &url,
    std::function<int32_t (const QString &)> textMsgCb,
    std::function<int32_t (const QByteArray &)> BinMsgCb) :
    QObject(this),
    mUrl(url),
    mTextMsgCb(textMsgCb),
    mBinMsgCb(BinMsgCb)
{
    connect(&mWebSocket, SIGNAL(QWebSocket::connected),
            this, SLOT(WebSocketClient::onConnected));

    connect(&mWebSocket, SIGNAL(QWebSocket::disconnected),
            this, SLOT(WebSocketClient::closed));

    connect(&mWebSocket, SIGNAL(QWebSocket::error),
            this, SLOT(WebSocketClient::onError));

    mWebSocket.open(QUrl(url));
}

WebSocketClient::~WebSocketClient()
{
    mWebSocket.close();
}

void WebSocketClient::onConnected()
{
    connect(&mWebSocket, &QWebSocket::textMessageReceived,
            this, &WebSocketClient::onMessageReceived);

    connect(&mWebSocket, &QWebSocket::binaryMessageReceived,
            this, &WebSocketClient::onDataReceived);
}

int32_t WebSocketClient::sendMessage(QString &msg)
{
    return mWebSocket.sendTextMessage(msg) > 0 ? NO_ERROR : UNKNOWN_ERROR;
}

int32_t WebSocketClient::sendData(QByteArray &data)
{
    return mWebSocket.sendBinaryMessage(data) > 0 ? NO_ERROR : UNKNOWN_ERROR;
}

void WebSocketClient::onMessageReceived(QString message)
{
    mTextMsgCb(message);
}

void WebSocketClient::onDataReceived(QByteArray data)
{
    mBinMsgCb(data);
}

void WebSocketClient::onError(QAbstractSocket::SocketError error)
{
    LOGE(MODULE_SOCKET, "Socket client error: %d", error);
}

}
