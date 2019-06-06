#ifndef COREHANDLER_H
#define COREHANDLER_H

#include <QObject>

#include "common.h"
#include "Core.h"
#include "GuiCallback.h"

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

    virtual int32_t appendDebugger(std::string str) override;
    virtual int32_t appendShell(std::string str) override;

signals:
    int32_t drawUi(std::function<int32_t ()> func);

private slots:
    int32_t onDrawUi(std::function<int32_t ()> func);

public:
    int32_t construct();
    int32_t destruct();
    CoreHandler(MainWindowUi *ui = nullptr);
    ~CoreHandler();

private:
    bool          mConstructed;
    ModuleType    mModule;
    Core         *mCore;
    MainWindowUi *mUi;
};

}

#endif
