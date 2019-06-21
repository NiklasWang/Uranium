#ifndef _DIALOGS_H
#define _DIALOGS_H

#include <QString>

namespace uranium {

class MainWindow;

// only one main window in this application
extern MainWindow *gMW;

enum MessageType {
    MESSAGE_TYPE_INFORMATION,
    MESSAGE_TYPE_QUESTION,
    MESSAGE_TYPE_WARNING,
    MESSAGE_TYPE_ERROR,
    MESSAGE_TYPE_CRITICAL,
    MESSAGE_TYPE_MAX_INVALID,
};

int32_t showError(const QString &msg);

int32_t showWarning(const QString &msg);

int32_t showCritical(const QString &msg);

int32_t showQuestion(const QString &msg);

int32_t showInformation(const QString &msg);

int32_t showDialog(MessageType type, const QString &msg);

}

#endif
