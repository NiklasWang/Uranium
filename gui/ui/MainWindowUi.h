#ifndef MainWindowUi_H
#define MainWindowUi_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

#include "Config.h"

namespace uranium {

class CoreHandler;

class MainWindowUi :
    public QObject
{
    Q_OBJECT

public:
    MainWindowUi();
    virtual ~MainWindowUi() override;

public:
    int32_t updateUi();
    int32_t setupUi(QMainWindow *MainWindow);
    void retranslateUi(QMainWindow *MainWindow);
    int32_t setupCore();
    int32_t destructCore();
    int32_t updateConfig(ConfigItem item, bool value);
    int32_t updateConfig(ConfigItem item, const QString &value);
    int32_t updateConfigResult(ConfigItem item, bool value);

public:
    int32_t onStarted(int32_t rc);
    int32_t onStopped(int32_t rc);
    int32_t onInitialized(int32_t rc);
    int32_t appendDebugger(const QString &str);
    int32_t appendShell(const QString &str);

public:
    void getDebuggerSetting(QFont &, QString &);
    void getRemoteControlSetting(QFont &, QString &);

private:
    int32_t saveSettings();
    bool eventFilter(QObject *obj, QEvent *event) override;

Q_SIGNALS:
    void quit();
    void about();
    void debuggerSetting();

private Q_SLOTS:
    int32_t onStartButtonClicked();
    int32_t onSelectButonClicked();
    void ShowDebugTextEditMenu(QPoint);
    int32_t setConfig(bool checked);
    int32_t setConfig(const QString &setting);
    void showShellWindow(bool checked);

public Q_SLOTS:
    void onDebugTextEditorNewSetting(const QFont, const QString);
    void onRemoteControlEditorNewSetting(const QFont, const QString);

private:
    bool         mStarted;
    CoreHandler *mCore;
    QString      mDebugColor;
    QString      mDebugBg;
    QString      mShellColor;
    QString      mShellBg;
    bool         mUpdatingItem[CONFIG_MAX_INVALID];

private:
    QAction     *mActionAbout;
    QAction     *mActionExit;
    QWidget     *mCentralWidget;
    QWidget     *mHorizontalLayoutWidget;
    QHBoxLayout *mSettingHorizontalLayout;
    QGroupBox   *mSettingGroupBox;
    QWidget     *mVerticalLayoutWidget;
    QVBoxLayout *mSettingverticalLayout;
    QGridLayout *mCheckBoxGridLayout;
    QCheckBox   *mMasterCheckBox;
    QPushButton *mStartPushButton;
    QSpacerItem *mCheckBoxHorizontalSpacer;
    QCheckBox   *mDebugCheckBox;
    QCheckBox   *mRemoteControlCheckBox;
    QCheckBox   *mEncryptionCheckBox;
    QLabel      *mStatusLabel;
    QLabel      *mStartedLabel;
    QLabel      *mSettingBlanking1;
    QGridLayout *mInputBoxGridLayout;
    QLabel      *mLocalDirLabel;
    QLineEdit   *mLocalDirLineEdit;
    QLabel      *mPasswordLabel;
    QPushButton *mSelectPushButton;
    QLabel      *mUserNameLabel;
    QLabel      *mRemoteDirLabel;
    QLineEdit   *mRemoteDirLineEdit;
    QLineEdit   *mPasswordLineEdit;
    QLineEdit   *mUserNameLineEdit;
    QLabel      *mSettingBlanking2;
    QTextEdit   *mDebugTextEdit;
    QGroupBox   *mShellGroupBox;
    QTextEdit   *mShellTextEditor;
    QMenuBar    *mMenuBar;
    QMenu       *mMenuFile;
    QMenu       *mMenuHelp;
    QStatusBar  *mStatusBar;
    QMenu       *mDebugTextEditorMenu;
    QAction     *mDebugTextEditorActionClear;
    QAction     *mDebugTextEditorActionSetting;
    QMainWindow *mMainWindow;
};

}

namespace Ui {
    class MainWindow: public uranium::MainWindowUi {};
}


#endif
