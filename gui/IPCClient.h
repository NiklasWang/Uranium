#ifndef IPCClient_H
#define IPCClient_H

#include <QtCore/QObject>
#include <QtWebSockets/QWebSocket>

#include "common.h"

namespace uranium {

class IPCClient :
    public QObject
{
    Q_OBJECT

public:
    int32_t send(const QByteArray &data);
    explicit IPCClient(QString ip, uint16_t port,
        std::function<int32_t (const QByteArray &)> msgCb =
            [](const QByteArray &) -> int32_t { return 0; });
    virtual ~IPCClient() override;

public:
    int32_t construct();
    int32_t destruct();

Q_SIGNALS:
    void disconnected();

private Q_SLOTS:
    void onConnected();
    int32_t onReadyRead();
    void onError(QAbstractSocket::SocketError error);

private:
    int32_t onDataReceived(const QByteArray &data);

private:
    ModuleType  mModule;
    bool        mConstructed;
    QTcpSocket  mSocket;
    QString     mIp;
    uint16_t    mPort;
    bool        mReady;
    std::function<int32_t (const QByteArray &)> mMsgCb;
};

}

#endif
