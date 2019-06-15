#ifndef WEBSOCKETCLIENT_H
#define WEBSOCKETCLIENT_H

#include <QtCore/QObject>
#include <QtWebSockets/QWebSocket>

namespace uranium {

class WebSocketClient :
    public QObject
{
    Q_OBJECT

public:
    int32_t sendMessage(QString &msg);
    int32_t sendData(QByteArray &data);
    explicit WebSocketClient(const QUrl &url,
        std::function<int32_t (const QString &)> textMsgCb =
            [](const QString &) -> int32_t { return 0; },
        std::function<int32_t (const QByteArray &)> BinMsgCb =
            [](const QByteArray &) -> int32_t { return 0; });
    virtual ~WebSocketClient() override;

Q_SIGNALS:
    void closed();

private Q_SLOTS:
    void onConnected();
    void onMessageReceived(QString message);
    void onDataReceived(QByteArray data);
    void onError(QAbstractSocket::SocketError error);

private:
    QWebSocket mWebSocket;
    QUrl       mUrl;
    std::function<int32_t (const QString &)>    mTextMsgCb;
    std::function<int32_t (const QByteArray &)> mBinMsgCb;
};

}

#endif
