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

#include "version.h"
#include "common.h"
#include "ui/MainWindowUi.h"

namespace uranium {

int32_t MainWindowUi::setupUi(QMainWindow *MainWindow)
{
    if (MainWindow->objectName().isEmpty()) {
        MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
    }
    MainWindow->resize(1209, 848);
    mActionAbout = new QAction(MainWindow);
    mActionAbout->setObjectName(QString::fromUtf8("mActionAbout"));
    connect(mActionAbout, SIGNAL(triggered(bool)), this, SIGNAL(about()));
    mAactionExit = new QAction(MainWindow);
    mAactionExit->setObjectName(QString::fromUtf8("mAactionExit"));
    connect(mAactionExit, SIGNAL(triggered(bool)), this, SIGNAL(quit()));
    mCentralWidget = new QWidget(MainWindow);
    mCentralWidget->setObjectName(QString::fromUtf8("mCentralWidget"));
    mHorizontalLayoutWidget = new QWidget(mCentralWidget);
    mHorizontalLayoutWidget->setObjectName(QString::fromUtf8("mHorizontalLayoutWidget"));
    mHorizontalLayoutWidget->setGeometry(QRect(10, 10, 1191, 791));
    mSettingHorizontalLayout = new QHBoxLayout(mHorizontalLayoutWidget);
    mSettingHorizontalLayout->setSpacing(6);
    mSettingHorizontalLayout->setContentsMargins(11, 11, 11, 11);
    mSettingHorizontalLayout->setObjectName(QString::fromUtf8("mSettingHorizontalLayout"));
    mSettingHorizontalLayout->setContentsMargins(0, 0, 0, 0);
    mSettingGroupBox = new QGroupBox(mHorizontalLayoutWidget);
    mSettingGroupBox->setObjectName(QString::fromUtf8("mSettingGroupBox"));
    mVerticalLayoutWidget = new QWidget(mSettingGroupBox);
    mVerticalLayoutWidget->setObjectName(QString::fromUtf8("mVerticalLayoutWidget"));
    mVerticalLayoutWidget->setGeometry(QRect(10, 30, 571, 751));
    mSettingverticalLayout = new QVBoxLayout(mVerticalLayoutWidget);
    mSettingverticalLayout->setSpacing(6);
    mSettingverticalLayout->setContentsMargins(11, 11, 11, 11);
    mSettingverticalLayout->setObjectName(QString::fromUtf8("mSettingverticalLayout"));
    mSettingverticalLayout->setContentsMargins(0, 0, 0, 0);
    mCheckBoxFormLayout = new QFormLayout();
    mCheckBoxFormLayout->setSpacing(6);
    mCheckBoxFormLayout->setObjectName(QString::fromUtf8("mCheckBoxFormLayout"));
    mMasterCheckBox = new QCheckBox(mVerticalLayoutWidget);
    mMasterCheckBox->setObjectName(QString::fromUtf8("mMasterCheckBox"));
    QFont font;
    font.setPointSize(14);
    font.setBold(true);
    font.setWeight(75);
    mMasterCheckBox->setFont(font);

    mCheckBoxFormLayout->setWidget(0, QFormLayout::LabelRole, mMasterCheckBox);

    mEncryptionCheckBox = new QCheckBox(mVerticalLayoutWidget);
    mEncryptionCheckBox->setObjectName(QString::fromUtf8("mEncryptionCheckBox"));
    mEncryptionCheckBox->setFont(font);

    mCheckBoxFormLayout->setWidget(1, QFormLayout::LabelRole, mEncryptionCheckBox);

    mDebugCheckBox = new QCheckBox(mVerticalLayoutWidget);
    mDebugCheckBox->setObjectName(QString::fromUtf8("mDebugCheckBox"));
    mDebugCheckBox->setFont(font);

    mCheckBoxFormLayout->setWidget(2, QFormLayout::LabelRole, mDebugCheckBox);

    mRemoteControlCheckBox = new QCheckBox(mVerticalLayoutWidget);
    mRemoteControlCheckBox->setObjectName(QString::fromUtf8("mRemoteControlCheckBox"));
    mRemoteControlCheckBox->setFont(font);

    mCheckBoxFormLayout->setWidget(3, QFormLayout::LabelRole, mRemoteControlCheckBox);


    mSettingverticalLayout->addLayout(mCheckBoxFormLayout);

    mSettingBlanking1 = new QLabel(mVerticalLayoutWidget);
    mSettingBlanking1->setObjectName(QString::fromUtf8("mSettingBlanking1"));
    QFont font1;
    font1.setPointSize(10);
    mSettingBlanking1->setFont(font1);

    mSettingverticalLayout->addWidget(mSettingBlanking1);

    mInputBoxGridLayout = new QGridLayout();
    mInputBoxGridLayout->setSpacing(6);
    mInputBoxGridLayout->setObjectName(QString::fromUtf8("mInputBoxGridLayout"));
    mLocalDirLineEdit = new QLineEdit(mVerticalLayoutWidget);
    mLocalDirLineEdit->setObjectName(QString::fromUtf8("mLocalDirLineEdit"));
    QFont font2;
    font2.setPointSize(12);
    mLocalDirLineEdit->setFont(font2);

    mInputBoxGridLayout->addWidget(mLocalDirLineEdit, 3, 1, 1, 1);

    mSelectPushButton = new QPushButton(mVerticalLayoutWidget);
    mSelectPushButton->setObjectName(QString::fromUtf8("mSelectPushButton"));
    QFont font3;
    font3.setPointSize(12);
    font3.setBold(false);
    font3.setWeight(50);
    mSelectPushButton->setFont(font3);

    mInputBoxGridLayout->addWidget(mSelectPushButton, 3, 2, 1, 1);

    mPasswordLabel = new QLabel(mVerticalLayoutWidget);
    mPasswordLabel->setObjectName(QString::fromUtf8("mPasswordLabel"));
    mPasswordLabel->setFont(font3);

    mInputBoxGridLayout->addWidget(mPasswordLabel, 1, 0, 1, 1);

    mPasswordLineEdit = new QLineEdit(mVerticalLayoutWidget);
    mPasswordLineEdit->setObjectName(QString::fromUtf8("mPasswordLineEdit"));
    mPasswordLineEdit->setFont(font2);

    mInputBoxGridLayout->addWidget(mPasswordLineEdit, 1, 1, 1, 1);

    mUserNameLabel = new QLabel(mVerticalLayoutWidget);
    mUserNameLabel->setObjectName(QString::fromUtf8("mUserNameLabel"));
    mUserNameLabel->setFont(font3);

    mInputBoxGridLayout->addWidget(mUserNameLabel, 0, 0, 1, 1);

    mRemoteDirLabel = new QLabel(mVerticalLayoutWidget);
    mRemoteDirLabel->setObjectName(QString::fromUtf8("mRemoteDirLabel"));
    mRemoteDirLabel->setFont(font2);

    mInputBoxGridLayout->addWidget(mRemoteDirLabel, 2, 0, 1, 1);

    mLocalDirLabel = new QLabel(mVerticalLayoutWidget);
    mLocalDirLabel->setObjectName(QString::fromUtf8("mLocalDirLabel"));
    mLocalDirLabel->setFont(font2);

    mInputBoxGridLayout->addWidget(mLocalDirLabel, 3, 0, 1, 1);

    mRemoteDirLineEdit = new QLineEdit(mVerticalLayoutWidget);
    mRemoteDirLineEdit->setObjectName(QString::fromUtf8("mRemoteDirLineEdit"));
    mRemoteDirLineEdit->setFont(font2);

    mInputBoxGridLayout->addWidget(mRemoteDirLineEdit, 2, 1, 1, 1);

    mUserNameLineEdit = new QLineEdit(mVerticalLayoutWidget);
    mUserNameLineEdit->setObjectName(QString::fromUtf8("mUserNameLineEdit"));
    mUserNameLineEdit->setFont(font2);

    mInputBoxGridLayout->addWidget(mUserNameLineEdit, 0, 1, 1, 1);


    mSettingverticalLayout->addLayout(mInputBoxGridLayout);

    mSettingBlanking2 = new QLabel(mVerticalLayoutWidget);
    mSettingBlanking2->setObjectName(QString::fromUtf8("mSettingBlanking2"));
    QFont font4;
    font4.setPointSize(10);
    font4.setBold(true);
    font4.setWeight(75);
    mSettingBlanking2->setFont(font4);

    mSettingverticalLayout->addWidget(mSettingBlanking2);

    mDebugTextEdit = new QTextEdit(mVerticalLayoutWidget);
    mDebugTextEdit->setObjectName(QString::fromUtf8("mDebugTextEdit"));
    QFont font5;
    font5.setFamily(QString::fromUtf8("Consolas"));
    font5.setPointSize(10);
    mDebugTextEdit->setFont(font5);

    mSettingverticalLayout->addWidget(mDebugTextEdit);


    mSettingHorizontalLayout->addWidget(mSettingGroupBox);

    mShellGroupBox = new QGroupBox(mHorizontalLayoutWidget);
    mShellGroupBox->setObjectName(QString::fromUtf8("mShellGroupBox"));
    mShellTextEditor = new QTextEdit(mShellGroupBox);
    mShellTextEditor->setObjectName(QString::fromUtf8("mShellTextEditor"));
    mShellTextEditor->setGeometry(QRect(10, 30, 571, 751));
    QFont font6;
    font6.setFamily(QString::fromUtf8("Consolas"));
    font6.setPointSize(10);
    font6.setBold(true);
    font6.setWeight(75);
    mShellTextEditor->setFont(font6);

    mSettingHorizontalLayout->addWidget(mShellGroupBox);

    MainWindow->setCentralWidget(mCentralWidget);
    mMenuBar = new QMenuBar(MainWindow);
    mMenuBar->setObjectName(QString::fromUtf8("mMenuBar"));
    mMenuBar->setGeometry(QRect(0, 0, 1209, 21));
    mMenuFile = new QMenu(mMenuBar);
    mMenuFile->setObjectName(QString::fromUtf8("mMenuFile"));
    mMenuHelp = new QMenu(mMenuBar);
    mMenuHelp->setObjectName(QString::fromUtf8("mMenuHelp"));
    MainWindow->setMenuBar(mMenuBar);
    mStatusBar = new QStatusBar(MainWindow);
    mStatusBar->setObjectName(QString::fromUtf8("mStatusBar"));
    MainWindow->setStatusBar(mStatusBar);
    QWidget::setTabOrder(mMasterCheckBox, mEncryptionCheckBox);
    QWidget::setTabOrder(mEncryptionCheckBox, mDebugCheckBox);
    QWidget::setTabOrder(mDebugCheckBox, mRemoteControlCheckBox);
    QWidget::setTabOrder(mRemoteControlCheckBox, mUserNameLineEdit);
    QWidget::setTabOrder(mUserNameLineEdit, mPasswordLineEdit);
    QWidget::setTabOrder(mPasswordLineEdit, mRemoteDirLineEdit);
    QWidget::setTabOrder(mRemoteDirLineEdit, mLocalDirLineEdit);
    QWidget::setTabOrder(mLocalDirLineEdit, mSelectPushButton);
    QWidget::setTabOrder(mSelectPushButton, mDebugTextEdit);
    QWidget::setTabOrder(mDebugTextEdit, mEncryptionCheckBox);
    QWidget::setTabOrder(mEncryptionCheckBox, mDebugCheckBox);
    QWidget::setTabOrder(mDebugCheckBox, mRemoteControlCheckBox);
    QWidget::setTabOrder(mRemoteControlCheckBox, mSelectPushButton);
    QWidget::setTabOrder(mSelectPushButton, mPasswordLineEdit);
    QWidget::setTabOrder(mPasswordLineEdit, mLocalDirLineEdit);
    QWidget::setTabOrder(mLocalDirLineEdit, mRemoteDirLineEdit);
    QWidget::setTabOrder(mRemoteDirLineEdit, mUserNameLineEdit);
    QWidget::setTabOrder(mUserNameLineEdit, mMasterCheckBox);

    mMenuBar->addAction(mMenuFile->menuAction());
    mMenuBar->addAction(mMenuHelp->menuAction());
    mMenuFile->addAction(mAactionExit);
    mMenuHelp->addAction(mActionAbout);

    retranslateUi(MainWindow);

    QMetaObject::connectSlotsByName(MainWindow);

    return NO_ERROR;
}

void MainWindowUi::retranslateUi(QMainWindow *MainWindow)
{
    MainWindow->setWindowTitle(QApplication::translate("MainWindow", PROJNAME " " VERSION, nullptr));
    mActionAbout->setText(QApplication::translate("MainWindow", "About", nullptr));
    mAactionExit->setText(QApplication::translate("MainWindow", "Exit", nullptr));
    mSettingGroupBox->setTitle(QApplication::translate("MainWindow", "Settings", nullptr));
    mMasterCheckBox->setText(QApplication::translate("MainWindow", "Master to Override Remote", nullptr));
    mEncryptionCheckBox->setText(QApplication::translate("MainWindow", "Encryption", nullptr));
    mDebugCheckBox->setText(QApplication::translate("MainWindow", "Debug Mode", nullptr));
    mRemoteControlCheckBox->setText(QApplication::translate("MainWindow", "Enable Remote Control", nullptr));
    mSettingBlanking1->setText(QString());
    mSelectPushButton->setText(QApplication::translate("MainWindow", "Select ...", nullptr));
    mPasswordLabel->setText(QApplication::translate("MainWindow", "Password", nullptr));
    mUserNameLabel->setText(QApplication::translate("MainWindow", "User Name", nullptr));
    mRemoteDirLabel->setText(QApplication::translate("MainWindow", "Remote Directory", nullptr));
    mLocalDirLabel->setText(QApplication::translate("MainWindow", "Local Directory", nullptr));
    mSettingBlanking2->setText(QString());
    mDebugTextEdit->setHtml(QApplication::translate("MainWindow",
        "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
        "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
        "p, li { white-space: pre-wrap; }\n"
        "</style></head><body style=\" font-family:'Consolas'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
        "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
            "<span style=\" font-family:'Courier New'; font-size:12pt;\">Debug Window &gt; </span></p>\n"
        "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
            "<span style=\" font-family:'Courier New'; font-size:12pt;\">Wish you have a pleasant day. :)</span></p></body></html>",
                nullptr));
    mShellGroupBox->setTitle(QApplication::translate("MainWindow", "Remote Shell", nullptr));
    mShellTextEditor->setHtml(QApplication::translate("MainWindow",
        "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
        "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
        "p, li { white-space: pre-wrap; }\n"
        "</style></head><body style=\" font-family:'Consolas'; font-size:10pt; font-weight:600; font-style:normal;\">\n"
        "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
            "Unknown@Remote:/home/Unknown$</p></body></html>", nullptr));
    mMenuFile->setTitle(QApplication::translate("MainWindow", "File", nullptr));
    mMenuHelp->setTitle(QApplication::translate("MainWindow", "Help", nullptr));
}

}
