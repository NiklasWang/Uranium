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
    QAction     *mAactionExit;
    QWidget     *mCentralWidget;
    QWidget     *mHorizontalLayoutWidget;
    QHBoxLayout *mSettingHorizontalLayout;
    QGroupBox   *mSettingGroupBox;
    QWidget     *mVerticalLayoutWidget;
    QVBoxLayout *mSettingverticalLayout;
    QFormLayout *mCheckBoxFormLayout;
    QCheckBox   *mMasterCheckBox;
    QCheckBox   *mEncryptionCheckBox;
    QCheckBox   *mDebugCheckBox;
    QCheckBox   *mRemoteControlCheckBox;
    QLabel      *mSettingBlanking1;
    QGridLayout *mInputBoxGridLayout;
    QLineEdit   *mLocalDirLineEdit;
    QPushButton *mSelectPushButton;
    QLabel      *mPasswordLabel;
    QLineEdit   *mPasswordLineEdit;
    QLabel      *mUserNameLabel;
    QLabel      *mRemoteDirLabel;
    QLabel      *mLocalDirLabel;
    QLineEdit   *mRemoteDirLineEdit;
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
