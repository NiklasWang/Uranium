#ifndef COREHANDLER_H
#define COREHANDLER_H

#include <QObject>
#include <QProcess>

#include "common.h"
#include "GuiCallback.h"
#include "IPCServer.h"
#include "IPCClient.h"
#include "Config.h"
#include "Semaphore.h"

namespace uranium {

class MainWindowUi;

class CoreHandler :
    public QObject,
    public GuiCallback
{
    Q_OBJECT

public:
    int32_t start();
    int32_t stop();

    int32_t getConfig(ConfigItem key, std::string &value);
    int32_t getConfig(ConfigItem key, bool &value);

    int32_t setConfig(ConfigItem key, std::string &value);
    int32_t setConfig(ConfigItem key, bool value);

    int32_t onStarted(int32_t rc);
    int32_t onStopped(int32_t rc);
    int32_t onInitialized(int32_t rc);

    virtual int32_t appendDebugger(const std::string &str) override;
    virtual int32_t appendShell(const std::string &str) override;

signals:
    int32_t drawUi(std::function<int32_t ()> func);

private slots:
    int32_t onDrawUi(std::function<int32_t ()> func);
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
    int32_t sendCoreMessage(QString &msg);

private:
    bool          mConstructed;
    ModuleType    mModule;
    MainWindowUi *mUi;
    QProcess      mCoreProcess;
    bool          mCoreReady;
    Semaphore     mGetSem;
    Semaphore     mExitSem;
    std::string   mGetResult;
    IPCServer *mIPCServer;
    IPCClient *mIPCClient;
};

}

#endif
