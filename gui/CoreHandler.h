#ifndef COREHANDLER_H
#define COREHANDLER_H

#include <QObject>
#include <QProcess>

#include "common.h"
#include "TimedSemaphore.h"
#include "GuiCallback.h"
#include "IPCServer.h"
#include "IPCClient.h"
#include "Config.h"
#include "Semaphore.h"

namespace uranium {

class MainWindowUi;

class CoreHandler :
    public QObject
{
    Q_OBJECT

public:
    int32_t start();
    int32_t stop();
    int32_t loadConfig();
    int32_t getConfig(ConfigItem key);

    int32_t setConfig(ConfigItem key, std::string &value);
    int32_t setConfig(ConfigItem key, bool value);

    int32_t onStarted(int32_t rc);
    int32_t onStopped(int32_t rc);
    int32_t onInitialized(int32_t rc);

    virtual int32_t appendDebugger(const QString &str);
    virtual int32_t appendShell(const QString &str);

signals:
    int32_t exec(std::function<int32_t ()> func);
    void exitServer();

private slots:
    int32_t onExec(std::function<int32_t ()> func);
    void onProcessError(QProcess::ProcessError error);
    int32_t onCoreLost();

public:
    int32_t construct();
    int32_t destruct();
    CoreHandler(MainWindowUi *ui = nullptr);
    virtual ~CoreHandler() override;

private:
    int32_t onCoreReady();
    int32_t onIPCData(const QByteArray &data);
    int32_t onGetConfig(const QString &value);
    int32_t onSetConfig(const QString &value);
    int32_t sendCoreMessage(QString &msg);
    int32_t killCore();
    int32_t launchCore();

private:
    bool           mConstructed;
    ModuleType     mModule;
    MainWindowUi  *mUi;
    QProcess      *mCoreProcess;
    bool           mCoreReady;
    ConfigItem     mConfigLoading;
    IPCServer     *mIPCServer;
    IPCClient     *mIPCClient;
};

}

#endif
