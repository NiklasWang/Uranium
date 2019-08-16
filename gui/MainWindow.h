#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "Dialogs.h"

namespace Ui {
class MainWindow;
}

namespace uranium {

class AboutDialog;
class DebuggerSettingDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    int32_t showDialog(MessageType type, const QString &msg);

signals:
    int32_t showDialogSignal(MessageType type, const QString msg);
    void newPathSelected(QString path);

private slots:
    int32_t onDialogShow(MessageType type, const QString msg);
    int32_t showAbout();
    int32_t showDebuggerSetting();
    void closeAbout();
    void closeDebuggerSetting();

public:
    explicit MainWindow(QApplication *app, QWidget *parent = nullptr);
    virtual ~MainWindow() override;
    int32_t construct();
    int32_t destruct();

private:
    bool mConstructed;
    QApplication   *mApp;
    QWidget        *mParent;
    Ui::MainWindow *mUi;
    AboutDialog    *mAbout;
    DebuggerSettingDialog *mDebuggerSettingDialog;
};

}

#endif
