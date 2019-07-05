#ifndef IPCServer_H
#define IPCServer_H

#include <functional>

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QByteArray>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

#include "modules.h"

namespace uranium {

class IPCServer :
    public QObject
{
    Q_OBJECT

public:
    int32_t construct();
    int32_t destruct();
    explicit IPCServer(quint16 port,
        std::function<int32_t (const QByteArray &)> msgCb =
            [](const QByteArray &) ->int32_t { return 0; });
    ~IPCServer();

Q_SIGNALS:
    void closed();

private Q_SLOTS:
    void onNewConnection();
    int32_t onReadyRead();
    void socketDisconnected();
    void onAcceptError(QAbstractSocket::SocketError);

private:
    int32_t processMessage(const QByteArray &message);

private:
    bool                 mConstructed;
    ModuleType           mModule;
    uint16_t             mPort;
    QTcpServer          *mSocketServer;
    QList<QTcpSocket *>  mClients;
    std::function<int32_t (const QByteArray &)> mMsgCb;
};

};

#endif
