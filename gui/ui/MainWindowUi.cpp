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
    int32_t rc = NO_ERROR;
    QFont checkBoxFont;
    QFont lineEditFont;
    QFont settingBlankingFont;
    QSizePolicy virtualExpandingPolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    if (SUCCEED(rc)) {
        if (MainWindow->objectName().isEmpty()) {
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        }
        MainWindow->resize(1209, 848);
    }

    if (SUCCEED(rc)) {
        mActionAbout = new QAction(MainWindow);
        mActionAbout->setObjectName(QStringLiteral("mActionAbout"));
        connect(mActionAbout, SIGNAL(triggered(bool)), this, SIGNAL(about()));
        mActionExit = new QAction(MainWindow);
        mActionExit->setObjectName(QStringLiteral("mActionExit"));
        connect(mActionExit, SIGNAL(triggered(bool)), this, SIGNAL(quit()));
    }

    if (SUCCEED(rc)) {
        mCentralWidget = new QWidget(MainWindow);
        mCentralWidget->setObjectName(QStringLiteral("mCentralWidget"));
        mHorizontalLayoutWidget = new QWidget(mCentralWidget);
        mHorizontalLayoutWidget->setObjectName(QStringLiteral("mHorizontalLayoutWidget"));
        mHorizontalLayoutWidget->setGeometry(QRect(10, 10, 1191, 791));
    }

    if (SUCCEED(rc)) {
        mSettingHorizontalLayout = new QHBoxLayout(mHorizontalLayoutWidget);
        mSettingHorizontalLayout->setSpacing(6);
        mSettingHorizontalLayout->setContentsMargins(11, 11, 11, 11);
        mSettingHorizontalLayout->setObjectName(QStringLiteral("mSettingHorizontalLayout"));
        mSettingHorizontalLayout->setContentsMargins(0, 0, 0, 0);
    }

    if (SUCCEED(rc)) {
        mSettingGroupBox = new QGroupBox(mHorizontalLayoutWidget);
        mSettingGroupBox->setObjectName(QStringLiteral("mSettingGroupBox"));
        mVerticalLayoutWidget = new QWidget(mSettingGroupBox);
        mVerticalLayoutWidget->setObjectName(QStringLiteral("mVerticalLayoutWidget"));
        mVerticalLayoutWidget->setGeometry(QRect(10, 10, 571, 770));
        mSettingverticalLayout = new QVBoxLayout(mVerticalLayoutWidget);
        mSettingverticalLayout->setSpacing(6);
        mSettingverticalLayout->setContentsMargins(11, 11, 11, 11);
        mSettingverticalLayout->setObjectName(QStringLiteral("mSettingverticalLayout"));
        mSettingverticalLayout->setContentsMargins(0, 0, 0, 0);
    }

    if (SUCCEED(rc)) {
        mCheckBoxGridLayout = new QGridLayout();
        mCheckBoxGridLayout->setSpacing(6);
        mCheckBoxGridLayout->setObjectName(QStringLiteral("mCheckBoxGridLayout"));
    }

    if (SUCCEED(rc)) {
        mMasterCheckBox = new QCheckBox(mVerticalLayoutWidget);
        mMasterCheckBox->setObjectName(QStringLiteral("mMasterCheckBox"));
        checkBoxFont.setPointSize(14);
        checkBoxFont.setBold(true);
        checkBoxFont.setWeight(75);
        mMasterCheckBox->setFont(checkBoxFont);
        mCheckBoxGridLayout->addWidget(mMasterCheckBox, 1, 0, 1, 1);
    }

    if (SUCCEED(rc)) {
        mStartPushButton = new QPushButton(mVerticalLayoutWidget);
        mStartPushButton->setObjectName(QStringLiteral("mStartPushButton"));
        virtualExpandingPolicy.setHorizontalStretch(0);
        virtualExpandingPolicy.setVerticalStretch(0);
        virtualExpandingPolicy.setHeightForWidth(mStartPushButton->sizePolicy().hasHeightForWidth());
        mStartPushButton->setSizePolicy(virtualExpandingPolicy);
        QFont font1;
        font1.setFamily(QStringLiteral("Consolas"));
        font1.setPointSize(22);
        font1.setBold(true);
        font1.setWeight(75);
        mStartPushButton->setFont(font1);
        mCheckBoxGridLayout->addWidget(mStartPushButton, 0, 4, 5, 1);
    }

    if (SUCCEED(rc)) {
        mCheckBoxHorizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        mCheckBoxGridLayout->addItem(mCheckBoxHorizontalSpacer, 1, 1, 4, 1);

        mDebugCheckBox = new QCheckBox(mVerticalLayoutWidget);
        mDebugCheckBox->setObjectName(QStringLiteral("mDebugCheckBox"));
        mDebugCheckBox->setEnabled(false);
        mDebugCheckBox->setFont(checkBoxFont);
        mDebugCheckBox->setChecked(true);
        mCheckBoxGridLayout->addWidget(mDebugCheckBox, 3, 0, 1, 1);
    }

    if (SUCCEED(rc)) {
        mRemoteControlCheckBox = new QCheckBox(mVerticalLayoutWidget);
        mRemoteControlCheckBox->setObjectName(QStringLiteral("mRemoteControlCheckBox"));
        mRemoteControlCheckBox->setFont(checkBoxFont);
        mCheckBoxGridLayout->addWidget(mRemoteControlCheckBox, 4, 0, 1, 1);
    }

    if (SUCCEED(rc)) {
        mEncryptionCheckBox = new QCheckBox(mVerticalLayoutWidget);
        mEncryptionCheckBox->setObjectName(QStringLiteral("mEncryptionCheckBox"));
        mEncryptionCheckBox->setEnabled(false);
        mEncryptionCheckBox->setFont(checkBoxFont);
        mEncryptionCheckBox->setCheckable(true);
        mEncryptionCheckBox->setChecked(true);
        mCheckBoxGridLayout->addWidget(mEncryptionCheckBox, 2, 0, 1, 1);
    }


    if (SUCCEED(rc)) {
        mStatusLabel = new QLabel(mVerticalLayoutWidget);
        mStatusLabel->setObjectName(QStringLiteral("mStatusLabel"));
        mStatusLabel->setSizePolicy(virtualExpandingPolicy);
        mStatusLabel->resize(mStatusLabel->width(), mStatusLabel->width());
        QImage Image;
        Image.load(":/status/question");
        QPixmap pixmap = QPixmap::fromImage(Image);
        QPixmap fitPixmap = pixmap.scaled(mStatusLabel->width(),
            mStatusLabel->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        mStatusLabel->setPixmap(fitPixmap);
        mCheckBoxGridLayout->addWidget(mStatusLabel, 1, 2, 4, 1);
    }

    if (SUCCEED(rc)) {
        mStartedLabel = new QLabel(mVerticalLayoutWidget);
        mStartedLabel->setObjectName(QStringLiteral("mStartedLabel"));
        mStartedLabel->setSizePolicy(virtualExpandingPolicy);
        mStartedLabel->resize(mStartedLabel->width(), mStartedLabel->width());
        QImage Image;
        Image.load(":/status/question");
        QPixmap pixmap = QPixmap::fromImage(Image);
        QPixmap fitPixmap = pixmap.scaled(mStatusLabel->width(),
            mStatusLabel->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        mStartedLabel->setPixmap(fitPixmap);
        mCheckBoxGridLayout->addWidget(mStartedLabel, 1, 3, 4, 1);
        mSettingverticalLayout->addLayout(mCheckBoxGridLayout);
    }

    if (SUCCEED(rc)) {
        mSettingBlanking1 = new QLabel(mVerticalLayoutWidget);
        mSettingBlanking1->setObjectName(QStringLiteral("mSettingBlanking1"));
        settingBlankingFont.setPointSize(10);
        settingBlankingFont.setBold(true);
        settingBlankingFont.setWeight(75);
        mSettingBlanking1->setFont(settingBlankingFont);
        mSettingverticalLayout->addWidget(mSettingBlanking1);
    }

    if (SUCCEED(rc)) {
        mInputBoxGridLayout = new QGridLayout();
        mInputBoxGridLayout->setSpacing(6);
        mInputBoxGridLayout->setObjectName(QStringLiteral("mInputBoxGridLayout"));
    }

    if (SUCCEED(rc)) {
        mLocalDirLabel = new QLabel(mVerticalLayoutWidget);
        mLocalDirLabel->setObjectName(QStringLiteral("mLocalDirLabel"));
        lineEditFont.setPointSize(12);
        lineEditFont.setBold(false);
        lineEditFont.setWeight(50);
        mLocalDirLabel->setFont(lineEditFont);
        mInputBoxGridLayout->addWidget(mLocalDirLabel, 3, 0, 1, 1);

        mLocalDirLineEdit = new QLineEdit(mVerticalLayoutWidget);
        mLocalDirLineEdit->setObjectName(QStringLiteral("mLocalDirLineEdit"));
        mLocalDirLineEdit->setFont(lineEditFont);
        mInputBoxGridLayout->addWidget(mLocalDirLineEdit, 3, 1, 1, 1);

        mSelectPushButton = new QPushButton(mVerticalLayoutWidget);
        mSelectPushButton->setObjectName(QStringLiteral("mSelectPushButton"));
        mSelectPushButton->setFont(lineEditFont);
        mInputBoxGridLayout->addWidget(mSelectPushButton, 3, 2, 1, 1);
    }

    if (SUCCEED(rc)) {
        mPasswordLabel = new QLabel(mVerticalLayoutWidget);
        mPasswordLabel->setObjectName(QStringLiteral("mPasswordLabel"));
        mPasswordLabel->setFont(lineEditFont);
        mInputBoxGridLayout->addWidget(mPasswordLabel, 1, 0, 1, 1);

        mPasswordLineEdit = new QLineEdit(mVerticalLayoutWidget);
        mPasswordLineEdit->setObjectName(QStringLiteral("mPasswordLineEdit"));
        mPasswordLineEdit->setFont(lineEditFont);
        mInputBoxGridLayout->addWidget(mPasswordLineEdit, 1, 1, 1, 2);
    }

    if (SUCCEED(rc)) {
        mUserNameLabel = new QLabel(mVerticalLayoutWidget);
        mUserNameLabel->setObjectName(QStringLiteral("mUserNameLabel"));
        mUserNameLabel->setFont(lineEditFont);
        mInputBoxGridLayout->addWidget(mUserNameLabel, 0, 0, 1, 1);

        mUserNameLineEdit = new QLineEdit(mVerticalLayoutWidget);
        mUserNameLineEdit->setObjectName(QStringLiteral("mUserNameLineEdit"));
        mUserNameLineEdit->setFont(lineEditFont);
        mInputBoxGridLayout->addWidget(mUserNameLineEdit, 0, 1, 1, 2);
    }

    if (SUCCEED(rc)) {
        mRemoteDirLabel = new QLabel(mVerticalLayoutWidget);
        mRemoteDirLabel->setObjectName(QStringLiteral("mRemoteDirLabel"));
        mRemoteDirLabel->setFont(lineEditFont);
        mInputBoxGridLayout->addWidget(mRemoteDirLabel, 2, 0, 1, 1);

        mRemoteDirLineEdit = new QLineEdit(mVerticalLayoutWidget);
        mRemoteDirLineEdit->setObjectName(QStringLiteral("mRemoteDirLineEdit"));
        mRemoteDirLineEdit->setFont(lineEditFont);
        mInputBoxGridLayout->addWidget(mRemoteDirLineEdit, 2, 1, 1, 2);
    }

    if (SUCCEED(rc)) {
        mSettingverticalLayout->addLayout(mInputBoxGridLayout);

        mSettingBlanking2 = new QLabel(mVerticalLayoutWidget);
        mSettingBlanking2->setObjectName(QStringLiteral("mSettingBlanking2"));
        mSettingBlanking2->setFont(settingBlankingFont);
        mSettingverticalLayout->addWidget(mSettingBlanking2);
    }

    if (SUCCEED(rc)) {
        mDebugTextEdit = new QTextEdit(mVerticalLayoutWidget);
        mDebugTextEdit->setObjectName(QStringLiteral("mDebugTextEdit"));
        QFont font6;
        font6.setFamily(QStringLiteral("Consolas"));
        font6.setPointSize(10);
        mDebugTextEdit->setFont(font6);
        mSettingverticalLayout->addWidget(mDebugTextEdit);

        mSettingHorizontalLayout->addWidget(mSettingGroupBox);
    }

    if (SUCCEED(rc)) {
        mShellGroupBox = new QGroupBox(mHorizontalLayoutWidget);
        mShellGroupBox->setObjectName(QStringLiteral("mShellGroupBox"));
        mShellTextEditor = new QTextEdit(mShellGroupBox);
        mShellTextEditor->setObjectName(QStringLiteral("mShellTextEditor"));
        mShellTextEditor->setGeometry(QRect(10, 20, 571, 761));
        QFont font7;
        font7.setFamily(QStringLiteral("Consolas"));
        font7.setPointSize(10);
        font7.setBold(true);
        font7.setWeight(75);
        mShellTextEditor->setFont(font7);
        mSettingHorizontalLayout->addWidget(mShellGroupBox);
    }

    if (SUCCEED(rc)) {
        MainWindow->setCentralWidget(mCentralWidget);
        mMenuBar = new QMenuBar(MainWindow);
        mMenuBar->setObjectName(QStringLiteral("mMenuBar"));
        mMenuBar->setGeometry(QRect(0, 0, 1209, 17));
        mMenuFile = new QMenu(mMenuBar);
        mMenuFile->setObjectName(QStringLiteral("mMenuFile"));
        mMenuHelp = new QMenu(mMenuBar);
        mMenuHelp->setObjectName(QStringLiteral("mMenuHelp"));
        MainWindow->setMenuBar(mMenuBar);
        mStatusBar = new QStatusBar(MainWindow);
        mStatusBar->setObjectName(QStringLiteral("mStatusBar"));
        MainWindow->setStatusBar(mStatusBar);
    }

    if (SUCCEED(rc)) {
        QWidget::setTabOrder(mMasterCheckBox, mStartPushButton);
        QWidget::setTabOrder(mStartPushButton, mEncryptionCheckBox);
        QWidget::setTabOrder(mEncryptionCheckBox, mDebugCheckBox);
        QWidget::setTabOrder(mDebugCheckBox, mRemoteControlCheckBox);
        QWidget::setTabOrder(mRemoteControlCheckBox, mUserNameLineEdit);
        QWidget::setTabOrder(mUserNameLineEdit, mPasswordLineEdit);
        QWidget::setTabOrder(mPasswordLineEdit, mRemoteDirLineEdit);
        QWidget::setTabOrder(mRemoteDirLineEdit, mLocalDirLineEdit);
        QWidget::setTabOrder(mLocalDirLineEdit, mLocalDirLineEdit);
        QWidget::setTabOrder(mLocalDirLineEdit, mRemoteDirLineEdit);
        QWidget::setTabOrder(mRemoteDirLineEdit, mPasswordLineEdit);
        QWidget::setTabOrder(mPasswordLineEdit, mUserNameLineEdit);
        QWidget::setTabOrder(mUserNameLineEdit, mSelectPushButton);
        QWidget::setTabOrder(mSelectPushButton, mSelectPushButton);
        QWidget::setTabOrder(mSelectPushButton, mDebugTextEdit);
        QWidget::setTabOrder(mDebugTextEdit, mShellTextEditor);
    }

    if (SUCCEED(rc)) {
        mMenuBar->addAction(mMenuFile->menuAction());
        mMenuBar->addAction(mMenuHelp->menuAction());
        mMenuFile->addAction(mActionExit);
        mMenuHelp->addAction(mActionAbout);
    }

    if (SUCCEED(rc) || !SUCCEED(rc)) {
        retranslateUi(MainWindow);
        QMetaObject::connectSlotsByName(MainWindow);
    }

    return NO_ERROR;
}

void MainWindowUi::retranslateUi(QMainWindow *MainWindow)
{
    MainWindow->setWindowTitle(QApplication::translate("MainWindow", PROJNAME " " VERSION, nullptr));
    mActionAbout->setText(QApplication::translate("MainWindow", "About", nullptr));
    mActionExit->setText(QApplication::translate("MainWindow", "Exit", nullptr));
    mSettingGroupBox->setTitle(QApplication::translate("MainWindow", "Settings", nullptr));
    mMasterCheckBox->setText(QApplication::translate("MainWindow", "Master to Override Remote", nullptr));
    mStartPushButton->setText(QApplication::translate("MainWindow", " Start ", nullptr));
    mDebugCheckBox->setText(QApplication::translate("MainWindow", "Debug Mode", nullptr));
    mRemoteControlCheckBox->setText(QApplication::translate("MainWindow", "Enable Remote Control", nullptr));
    mEncryptionCheckBox->setText(QApplication::translate("MainWindow", "Encryption", nullptr));
    mStatusLabel->setText(QString());
    mStartedLabel->setText(QString());
    mSettingBlanking1->setText(QString());
    mLocalDirLabel->setText(QApplication::translate("MainWindow", "Local Directory", nullptr));
    mPasswordLabel->setText(QApplication::translate("MainWindow", "Password", nullptr));
    mSelectPushButton->setText(QApplication::translate("MainWindow", "Select ...", nullptr));
    mUserNameLabel->setText(QApplication::translate("MainWindow", "User Name", nullptr));
    mRemoteDirLabel->setText(QApplication::translate("MainWindow", "Remote Directory", nullptr));
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
