#include <sstream>

#include <QProcess>
#include <QMutexLocker>

#include "version.h"
#include "common.h"
#include "IPCInstruction.h"
#include "CoreHandler.h"
#include "WebSocketClient.h"
#include "ui/MainWindowUi.h"

namespace uranium {

int32_t CoreHandler::construct()
{
    int32_t rc = NO_ERROR;

    if (mConstructed) {
        rc = ALREADY_INITED;
    }

    if (SUCCEED(rc)) {
        mSocketServer = new WebSocketServer(GUI_SOCK_PORT,
            [this](const QString &msg) -> int32_t {
                return onSocketMessage(msg);
            },
            [this](const QByteArray &data) -> int32_t {
                return onSocketData(data);
            }
        );
        if (ISNULL(mSocketServer)) {
            LOGE(mModule, "Failed to new socket server");
            rc = NO_MEMORY;
        }
    }

    if (SUCCEED(rc)) {
        rc = mSocketServer->construct();
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to construct socket server, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        mCoreProcess.start(PROJNAME ".exe");
        mStartSem.wait();
        if (!SUCCEED(mCoreProcessStatus)) {
            LOGF(mModule, "Fatal error from core process, %d", mCoreProcessStatus);
            rc = SYS_ERROR;
        }
    }

    if (SUCCEED(rc)) {
        QString msg(CORE_INIT);
        rc = sendCoreMessage(msg);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to send msg %s to core", CORE_INIT);
        }
    }

    if (SUCCEED(rc)) {
        mConstructed = true;
    }

    return rc;
}

int32_t CoreHandler::sendCoreMessage(QString &msg)
{
    int32_t rc = NO_ERROR;
    QString url = "ws://127.0.0.1:";

    url.append(CORE_SOCK_PORT);
    WebSocketClient client(url);
    int32_t size = client.sendMessage(msg);
    if (size != msg.size()) {
        LOGE(mModule, "Message %s sent %d bytes.", msg.toLatin1().data(), size);
        rc = UNKNOWN_ERROR;
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
        mExitSem.wait();
        if (!mCoreProcess.waitForFinished()) {
            LOGE(mModule, "Failed to exit core process");
            rc = UNKNOWN_ERROR;
        }
    }

    if (SUCCEED(rc)) {
        rc = mSocketServer->destruct();
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to destructed socket server, %d", rc);
        }
        SECURE_DELETE(mSocketServer);
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
        if (checkValid(key)) {
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

    return drawUi(
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

    return drawUi(
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

    return drawUi(
        [&]() -> int32_t {
            mUi->onInitialized(rc);
            return NO_ERROR;
        }
    );
}

int32_t CoreHandler::appendDebugger(std::string str)
{
    return drawUi(
        [&]() -> int32_t {
            mUi->appendDebugger(str);
            return NO_ERROR;
        }
    );
}

int32_t CoreHandler::appendShell(std::string str)
{
    return drawUi(
        [&]() -> int32_t {
            mUi->appendShell(str);
            return NO_ERROR;
        }
    );
}

int32_t CoreHandler::onSocketMessage(const QString &msg)
{
    int32_t rc = NO_ERROR;
    char *str = msg.toLatin1().data();

    if (COMPARE_SAME_STRING(str, GREETING_GUI)) {
        mStartSem.signal();
    } else if (COMPARE_SAME_STRING(str, BYE_GUI)) {
        mExitSem.signal();
    } else if (COMPARE_SAME_LEN_STRING(str, CORE_INIT, strlen(CORE_INIT))) {
        int32_t _rc = ISNULL(strstr(str, REPLY_SUCCEED)) ? NO_ERROR : UNKNOWN_ERROR;
        onInitialized(_rc);
    } else if (COMPARE_SAME_LEN_STRING(str, CORE_START, strlen(CORE_START))) {
        int32_t _rc = ISNULL(strstr(str, REPLY_SUCCEED)) ? NO_ERROR : UNKNOWN_ERROR;
        onStarted(_rc);
    } else if (COMPARE_SAME_LEN_STRING(str, CORE_STOP, strlen(CORE_STOP))) {
        int32_t _rc = ISNULL(strstr(str, REPLY_SUCCEED)) ? NO_ERROR : UNKNOWN_ERROR;
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

int32_t CoreHandler::onSocketData(const QByteArray &data)
{
    LOGE(mModule, "Received unknown data, %d bytes", data.size());

    return NO_ERROR;
}

CoreHandler::CoreHandler(MainWindowUi *ui) :
    mConstructed(false),
    mModule(MODULE_GUI),
    mUi(ui),
    mCoreProcessStatus(NO_ERROR),
    mSocketServer(nullptr)
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
        mCoreProcessStatus = error + 1;
        mStartSem.signal();
    }
}

}
