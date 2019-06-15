#ifndef WEBSOCKETSERVER_H
#define WEBSOCKETSERVER_H

#include <functional>

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QByteArray>

#include "modules.h"

class QWebSocketServer;
class QWebSocket;

namespace uranium {

class WebSocketServer :
    public QObject
{
    Q_OBJECT

public:
    int32_t construct();
    int32_t destruct();
    explicit WebSocketServer(quint16 port,
        std::function<int32_t (const QString &)> textMsgCb =
            [](const QString &) ->int32_t { return 0; },
        std::function<int32_t (const QByteArray &)> BinMsgCb =
            [](const QByteArray &) ->int32_t { return 0; });
    ~WebSocketServer();

Q_SIGNALS:
    void closed();

private Q_SLOTS:
    void onNewConnection();
    void processTextMessage(QString message);
    void processBinaryMessage(QByteArray message);
    void socketDisconnected();

private:
    bool                 mConstructed;
    ModuleType           mModule;
    uint16_t             mPort;
    QWebSocketServer    *mWebSocketServer;
    QList<QWebSocket *>  mClients;
    std::function<int32_t (const QString &)>    mTextMsgCb;
    std::function<int32_t (const QByteArray &)> mBinMsgCb;
};

};

#endif
