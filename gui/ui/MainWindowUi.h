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

namespace uranium {

class MainWindowUi :
    public QObject
{
    Q_OBJECT

public:
    int32_t setupUi(QMainWindow *MainWindow);
    void retranslateUi(QMainWindow *MainWindow);

signals:
    void quit();
    void about();

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
};

}

namespace Ui {
    class MainWindow: public uranium::MainWindowUi {};
} // namespace Ui


#endif
