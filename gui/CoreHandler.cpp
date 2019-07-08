#include <sstream>

#include <QProcess>
#include <QMutexLocker>

#include "version.h"
#include "common.h"
#include "IPCInstruction.h"
#include "CoreHandler.h"
#include "IPCClient.h"
#include "ui/MainWindowUi.h"

#define MAX_WAIT_CORE_TIME 1000 // ms

namespace uranium {

int32_t CoreHandler::construct()
{
    int32_t rc = NO_ERROR;

    if (mConstructed) {
        rc = ALREADY_INITED;
    }

    if (SUCCEED(rc)) {
        mIPCServer = new IPCServer(GUI_SOCK_PORT,
            [this](const QByteArray &data) -> int32_t {
                return onIPCData(data);
            }
        );
        if (ISNULL(mIPCServer)) {
            LOGE(mModule, "Failed to new socket server");
            rc = NO_MEMORY;
        }
    }

    if (SUCCEED(rc)) {
        rc = mIPCServer->construct();
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to construct socket server, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        mCoreProcess.start(PROJNAME ".exe");
    }

    if (SUCCEED(rc)) {
        mConstructed = true;
    }

    return rc;
}

int32_t CoreHandler::onCoreReady()
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        mIPCClient = new IPCClient(
            "127.0.0.1", CORE_SOCK_PORT,
            [this](const QByteArray &data) -> int32_t {
                return onIPCData(data);
            }
        );
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to create WebSocketClient");
            rc = NO_MEMORY;
        } else {
            connect(mIPCClient, SIGNAL(disconnected()),
                    this, SLOT(onCoreLost()));
        }
    }

    if (SUCCEED(rc)) {
        rc = mIPCClient->construct();
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to construct ipc client, %d", rc);
        } else {
            mCoreReady = true;
        }
    }

    if (SUCCEED(rc)) {
        QString msg(CORE_INIT);
        rc = sendCoreMessage(msg);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to send msg %s to core", CORE_INIT);
        }
    }

    return rc;
}

int32_t CoreHandler::onCoreLost()
{
    mCoreReady = false;
    LOGE(mModule, "Lost control with core.");

    return NO_ERROR;
}

int32_t CoreHandler::sendCoreMessage(QString &msg)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        if (!mCoreReady) {
            rc = NOT_READY;
            LOGE(mModule, "Can't send message '%s', core not ready yet.", msg.toLatin1().data());
        }
    }

    if (SUCCEED(rc)) {
        rc = mIPCClient->send(msg.toLatin1());
        if (FAILED(rc)) {
            LOGE(mModule, "Send message failed.");
            rc = UNKNOWN_ERROR;
        } else {
            LOGD(mModule, "Sent msg: '%s'", msg.toLatin1().data());
        }
    }

    return rc;
}

int32_t CoreHandler::destruct()
{
    int32_t rc = NO_ERROR;

    if (!mConstructed) {
        rc = NOT_INITED;
    } else {
        mConstructed = false;
    }

    if (SUCCEED(rc)) {
        QString msg(CORE_EXIT);
        rc = sendCoreMessage(msg);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to send msg %s to core", BYE_GUI);
        }
    }

    if (SUCCEED(rc)) {
        if (!mCoreProcess.waitForFinished(MAX_WAIT_CORE_TIME)) {
            LOGE(mModule, "Failed to exit core process, force to exit later.");
            rc = UNKNOWN_ERROR;
        }
    }

    if (SUCCEED(rc)) {
        rc = mIPCServer->destruct();
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to destructed ipc server, %d", rc);
        }
        SECURE_DELETE(mIPCServer);
    }

    if (SUCCEED(rc)) {
        rc = mIPCClient->destruct();
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to destructed ipc client, %d", rc);
        }
        SECURE_DELETE(mIPCClient);
    }

    if (SUCCEED(rc)) {
        QStringList params;
        params << "/F" << "/IM" << PROJNAME ".exe";
        QProcess process;
        process.start("taskkill", params);
        if (!process.waitForFinished()) {
            LOGE(mModule, "Failed to force exit core process");
            rc = UNKNOWN_ERROR;
            RESETRESULT(rc);
        }
    }

    return RETURNIGNORE(rc, NOT_INITED);
}

int32_t CoreHandler::start()
{
    QString msg(CORE_START);
    int32_t rc = sendCoreMessage(msg);
    if (!SUCCEED(rc)) {
        LOGE(mModule, "Failed to send msg %s to core", CORE_START);
    }

    return rc;
}

int32_t CoreHandler::stop()
{
    QString msg(CORE_STOP);
    int32_t rc = sendCoreMessage(msg);
    if (!SUCCEED(rc)) {
        LOGE(mModule, "Failed to send msg %s to core", CORE_STOP);
    }

    return rc;
}

int32_t CoreHandler::getConfig(ConfigItem key, std::string &value)
{
    int32_t rc = NO_ERROR;
    QMutex mutex;
    mutex.lock();

    if (SUCCEED(rc)) {
        if (!checkValid(key)) {
            LOGE(mModule, "Invalid key, %d", key);
            rc = PARAM_INVALID;
        }
    }

    if (SUCCEED(rc)) {
        QString msg(CORE_GET_CONFIG " ");
        msg += whoamI(key);
        rc = sendCoreMessage(msg);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to send msg %s to core", msg.toLatin1().data());
        } else {
            mGetSem.wait();
        }
    }

    if (SUCCEED(rc)) {
        std::string tmp;
        std::istringstream ss(mGetResult);
        ss >> tmp;
        if (tmp != CORE_GET_CONFIG) {
            LOGE(mModule, "Unknown msg received, %s", mGetResult.c_str());
        }
        ss >> tmp;
        if (tmp != whoamI(key)) {
            LOGE(mModule, "Item mismatch, %s", mGetResult.c_str());
        }
        ss >> value;
    }

    mutex.unlock();
    return rc;
}

int32_t CoreHandler::getConfig(ConfigItem key, bool &value)
{
    int32_t rc = NO_ERROR;
    std::string str;

    value = false;
    rc = getConfig(key, str);
    if (SUCCEED(rc) && str == BOOL_TRUE) {
        value = true;
    }

    return rc;
}

int32_t CoreHandler::setConfig(ConfigItem key, std::string &value)
{
    int32_t rc = NO_ERROR;
    QString msg(CORE_GET_CONFIG " ");

    if (SUCCEED(rc)) {
        if (checkValid(key)) {
            LOGE(mModule, "Invalid key, %d", key);
            rc = PARAM_INVALID;
        }
    }

    if (SUCCEED(rc)) {
        msg.append(whoamI(key));
        msg.append(" ");
        msg.append(value.c_str());
        int32_t rc = sendCoreMessage(msg);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to send msg %s to core", msg.toLatin1().data());
        }
    }

    return rc;
}

int32_t CoreHandler::setConfig(ConfigItem key, bool value)
{
    std::string str = value ? BOOL_TRUE : BOOL_FALSE;
    return setConfig(key, str);
}

int32_t CoreHandler::onStarted(int32_t rc)
{
    if (FAILED(rc)) {
        LOGE(mModule, "Core start failed, %d", rc);
    }

    return onDrawUi(
        [&]() -> int32_t {
            mUi->onStarted(rc);
            return NO_ERROR;
        }
    );
}

int32_t CoreHandler::onStopped(int32_t rc)
{
    if (FAILED(rc)) {
        LOGE(mModule, "Core stop failed, %d", rc);
    }

    return onDrawUi(
        [&]() -> int32_t {
            mUi->onStopped(rc);
            return NO_ERROR;
        }
    );
}

int32_t CoreHandler::onInitialized(int32_t rc)
{
    if (FAILED(rc)) {
        LOGE(mModule, "Core initialize failed, %d", rc);
    }

    return onDrawUi(
        [&]() -> int32_t {
            mUi->onInitialized(rc);
            return NO_ERROR;
        }
    );
}

int32_t CoreHandler::appendDebugger(const std::string &str)
{
    return onDrawUi(
        [&]() -> int32_t {
            mUi->appendDebugger(str);
            return NO_ERROR;
        }
    );
}

int32_t CoreHandler::appendShell(const std::string &str)
{
    return onDrawUi(
        [&]() -> int32_t {
            mUi->appendShell(str);
            return NO_ERROR;
        }
    );
}

int32_t CoreHandler::onIPCData(const QByteArray &data)
{
    int32_t rc = NO_ERROR;
    QString msg(data);
    char *str = msg.toLatin1().data();

    LOGD(mModule, "Received msg: '%s'", str);
    if (COMPARE_SAME_STRING(str, GREETING_GUI)) {
        rc = onCoreReady();
    } else if (COMPARE_SAME_STRING(str, BYE_GUI)) {
        LOGD(mModule, "Core process exited.");
    } else if (COMPARE_SAME_LEN_STRING(str, CORE_INIT, strlen(CORE_INIT))) {
        int32_t _rc = NOTNULL(strstr(str, REPLY_SUCCEED)) ? NO_ERROR : UNKNOWN_ERROR;
        onInitialized(_rc);
    } else if (COMPARE_SAME_LEN_STRING(str, CORE_START, strlen(CORE_START))) {
        int32_t _rc = NOTNULL(strstr(str, REPLY_SUCCEED)) ? NO_ERROR : UNKNOWN_ERROR;
        onStarted(_rc);
    } else if (COMPARE_SAME_LEN_STRING(str, CORE_STOP, strlen(CORE_STOP))) {
        int32_t _rc = NOTNULL(strstr(str, REPLY_SUCCEED)) ? NO_ERROR : UNKNOWN_ERROR;
        onStopped(_rc);
    } else if (COMPARE_SAME_LEN_STRING(str, GUI_DEBUG, strlen(GUI_DEBUG))) {
        appendDebugger(str);
    } else if (COMPARE_SAME_LEN_STRING(str, GUI_SHELL, strlen(GUI_SHELL))) {
        appendShell(str);
    } else if (COMPARE_SAME_LEN_STRING(str, CORE_GET_CONFIG, strlen(CORE_GET_CONFIG))) {
        mGetResult = str;
        mGetSem.signal();
    } else if (COMPARE_SAME_LEN_STRING(str, CORE_SET_CONFIG, strlen(GUI_SHELL))) {
        // don't care set config reply
    }

    return rc;
}

CoreHandler::CoreHandler(MainWindowUi *ui) :
    mConstructed(false),
    mModule(MODULE_GUI),
    mUi(ui),
    mCoreReady(false),
    mIPCServer(nullptr),
    mIPCClient(nullptr)
{
    qRegisterMetaType<std::function<int32_t ()> >("std::function<int32_t ()>");

    connect(this, SIGNAL(drawUi(std::function<int32_t ()>)),
            this, SLOT(onDrawUi(std::function<int32_t ()>)),
            Qt::BlockingQueuedConnection);

    connect(&mCoreProcess, SIGNAL(error(QProcess::ProcessError)),
            this, SLOT(onProcessError(QProcess::ProcessError)));
}

CoreHandler::~CoreHandler()
{
    if (mConstructed) {
        destruct();
    }
}

int32_t CoreHandler::onDrawUi(std::function<int32_t ()> func)
{
    return func();
}

void CoreHandler::onProcessError(QProcess::ProcessError error)
{
    LOGE(mModule, "Core process error, %d", error);

    if (error == QProcess::FailedToStart) {
        LOGF(mModule, "Core process failed to start.");
    }
}

}
