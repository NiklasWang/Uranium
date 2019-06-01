#include "common.h"
#include "Dialogs.h"
#include "MainWindow.h"

namespace uranium {

int32_t showError(const QString &msg)
{
    return showDialog(MESSAGE_TYPE_ERROR, msg);
}

int32_t showWarning(const QString &msg)
{
    return showDialog(MESSAGE_TYPE_ERROR, msg);
}

int32_t showCritical(const QString &msg)
{
    return showDialog(MESSAGE_TYPE_CRITICAL, msg);
}

int32_t showQuestion(const QString &msg)
{
    return showDialog(MESSAGE_TYPE_QUESTION, msg);
}

int32_t showInformation(const QString &msg)
{
    return showDialog(MESSAGE_TYPE_INFORMATION, msg);
}

int32_t showDialog(MessageType type, const QString &msg)
{
    return NOTNULL(gMW) ? gMW->showDialog(type, msg) : NOT_INITED;
}

}
