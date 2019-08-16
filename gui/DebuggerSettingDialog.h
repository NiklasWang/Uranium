#ifndef DEBUGGERSETTINGDIALOG_H
#define DEBUGGERSETTINGDIALOG_H

#include <QDialog>

#include "ui/DebuggerSettingUi.h"

namespace uranium {

class DebuggerSettingDialog :
    public QDialog
{
    Q_OBJECT

public:
    int32_t setup(const QFont &, const QString &);

Q_SIGNALS:
    void newSetting(const QFont, const QString);

public:
    DebuggerSettingDialog();
    virtual ~DebuggerSettingDialog();

private:
    DebuggerSettingUi *mUi;
};

}

#endif
