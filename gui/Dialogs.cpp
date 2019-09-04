#include "common.h"
#include "Dialogs.h"
#include "MainWindow.h"

namespace uranium {

int32_t showError(const QString &msg)
{
    QString newMsg = msg;
    newMsg.append("\n");
    newMsg.append("Please contact author for technical support:\n");
    newMsg.append(" - " AUTHOR1 "\n");
    newMsg.append(" - " AUTHOR0 "\n");
    return showDialog(MESSAGE_TYPE_ERROR, newMsg);
}

int32_t showWarning(const QString &msg)
{
    return showDialog(MESSAGE_TYPE_ERROR, msg);
}

int32_t showCritical(const QString &msg)
{
    QString newMsg = msg;
    newMsg.append("\n");
    newMsg.append("Please contact author for technical support:\n");
    newMsg.append(" - " AUTHOR1 "\n");
    newMsg.append(" - " AUTHOR0 "\n");
    return showDialog(MESSAGE_TYPE_CRITICAL, newMsg);
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
