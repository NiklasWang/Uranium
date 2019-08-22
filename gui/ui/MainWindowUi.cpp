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
#include <QFileDialog>

#include "version.h"
#include "common.h"
#include "ui/MainWindowUi.h"
#include "CoreHandler.h"
#include "Dialogs.h"

#define DEFAULT_WINDOW_WIDTH  608
#define WINDOW_SHELL_WIDTH    1209
#define DEFAULT_WINDOW_HEIGHT 848
#define MAX_DEBUG_LINE_COUNT  65535
#define MAX_SHELL_LINE_COUNT  65535

static const int32_t gDevMonitorScale = 192;

#define SCALE(x) (static_cast<int32_t>((x) * uranium::gCurMonitorScale * 1.0f / gDevMonitorScale))

namespace uranium {

MainWindowUi::MainWindowUi() :
    mStarted(false),
    mCore(nullptr)
{
}

MainWindowUi::~MainWindowUi()
{
}

int32_t MainWindowUi::setupUi(QMainWindow *MainWindow)
{
    int32_t rc = NO_ERROR;
    QFont checkBoxFont;
    QFont lineEditFont;
    QFont settingBlankingFont;
    QSizePolicy virtualExpandingPolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    if (SUCCEED(rc)) {
        mMainWindow = MainWindow;
        if (MainWindow->objectName().isEmpty()) {
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        }
        MainWindow->resize(SCALE(DEFAULT_WINDOW_WIDTH), SCALE(DEFAULT_WINDOW_HEIGHT));
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
        checkBoxFont.setPointSize(13);
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
        mStartPushButton->setEnabled(false);
        QFont font1;
        font1.setFamily(QStringLiteral("Consolas"));
        font1.setPointSize(22);
        font1.setBold(true);
        font1.setWeight(75);
        mStartPushButton->setFont(font1);
        mCheckBoxGridLayout->addWidget(mStartPushButton, 0, 4, 5, 1);
        connect(mStartPushButton, SIGNAL(clicked()), this, SLOT(onStartButtonClicked()));
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
        connect(mRemoteControlCheckBox, SIGNAL(toggled(bool)), this, SLOT(showShellWindow(bool)));
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
        connect(mSelectPushButton, SIGNAL(clicked()), this, SLOT(onSelectButonClicked()));
    }

    if (SUCCEED(rc)) {
        mPasswordLabel = new QLabel(mVerticalLayoutWidget);
        mPasswordLabel->setObjectName(QStringLiteral("mPasswordLabel"));
        mPasswordLabel->setFont(lineEditFont);
        mInputBoxGridLayout->addWidget(mPasswordLabel, 1, 0, 1, 1);

        mPasswordLineEdit = new QLineEdit(mVerticalLayoutWidget);
        mPasswordLineEdit->setObjectName(QStringLiteral("mPasswordLineEdit"));
        mPasswordLineEdit->setFont(lineEditFont);
        mPasswordLineEdit->setEchoMode(QLineEdit::Password);
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
        mDebugTextEdit->document()->setMaximumBlockCount(MAX_DEBUG_LINE_COUNT);
        mDebugTextEdit->setReadOnly(true);
        mDebugTextEditorMenu = nullptr;
        mDebugTextEdit->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(mDebugTextEdit, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(ShowDebugTextEditMenu(QPoint)));
        mDebugTextEdit->viewport()->installEventFilter(this);
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
        mShellTextEditor->document()->setMaximumBlockCount(MAX_SHELL_LINE_COUNT);
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

bool MainWindowUi::eventFilter(QObject *obj, QEvent *event)
{
    if ((obj == mDebugTextEdit || obj == mDebugTextEdit->viewport()) &&
        event->type() == QEvent::MouseButtonDblClick) {
        debuggerSetting();
    }

    return mDebugTextEdit->viewport()->eventFilter(obj, event);
}

void MainWindowUi::ShowDebugTextEditMenu(QPoint)
{
    if (ISNULL(mDebugTextEditorMenu)) {
        mDebugTextEditorMenu = mDebugTextEdit->createStandardContextMenu();
        mDebugTextEditorActionClear = new QAction("Clear", mDebugTextEdit);
        connect(mDebugTextEditorActionClear, SIGNAL(triggered()), mDebugTextEdit, SLOT(clear()));
        mDebugTextEditorActionSetting = new QAction("Setting...", mDebugTextEdit);
        connect(mDebugTextEditorActionSetting, SIGNAL(triggered()), this, SIGNAL(debuggerSetting()));
        mDebugTextEditorMenu->addAction(mDebugTextEditorActionClear);
        mDebugTextEditorMenu->addAction(mDebugTextEditorActionSetting);
    }
    mDebugTextEditorMenu->move(QCursor::pos());
    mDebugTextEditorMenu->show();
}

void MainWindowUi::onDebugTextEditorNewSetting(const QFont newFont, const QString newStyle)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        for (uint32_t i = 0; i < sizeof(mUpdatingItem); i++) {
            mUpdatingItem[i] = false;
        }
        QFont curFont = mDebugTextEdit->font();
        if (curFont.pointSize() != newFont.pointSize()) {
            mUpdatingItem[CONFIG_DEBUG_SIZE] = true;
        }
        if (curFont.family() != newFont.family()) {
            mUpdatingItem[CONFIG_DEBUG_FONT] = true;
        }
    }

    if (SUCCEED(rc)) {
        QString curBg, curColor, newBg, newColor;
        QStringList newSections = newStyle.split(QRegExp("[;]"));
        if (newSections.at(0).contains("background")) {
            newBg = newSections.at(0);
            newColor = newSections.at(1);
        } else {
            newBg = newSections.at(1);
            newColor = newSections.at(0);
        }
        QStringList curSections = mDebugTextEdit->styleSheet().split(QRegExp("[;]"));
        if (curSections.at(0).contains("background")) {
            curBg = curSections.at(0);
            curColor = curSections.at(1);
        } else {
            curBg = curSections.at(1);
            curColor = curSections.at(0);
        }
        if (curBg != newBg) {
            mUpdatingItem[CONFIG_DEBUG_BG] = true;
        }
        if (curColor != newColor) {
            mUpdatingItem[CONFIG_DEBUG_COLOR] = true;
        }
        mDebugBg = newBg;
        mDebugColor = newColor;
    }

    if (SUCCEED(rc)) {
        mDebugTextEdit->setFont(newFont);
        mDebugTextEdit->setStyleSheet(newStyle);
    }

    if (SUCCEED(rc)) {
        rc = saveSettings();
        if (FAILED(rc)) {
            appendDebugger("Failed to save settings.");
        }
    }

    return;
}

void MainWindowUi::onRemoteControlEditorNewSetting(const QFont font, const QString style)
{
    mShellTextEditor->setFont(font);
    mShellTextEditor->setStyleSheet(style);
}

void MainWindowUi::getDebuggerSetting(QFont &font, QString &style)
{
    font = mDebugTextEdit->font();
    style = mDebugTextEdit->styleSheet();
}

void MainWindowUi::getRemoteControlSetting(QFont &font, QString &style)
{
    font = mShellTextEditor->font();
    style = mShellTextEditor->styleSheet();
}

void MainWindowUi::retranslateUi(QMainWindow *MainWindow)
{
    MainWindow->setWindowTitle(QApplication::translate("MainWindow", PROJNAME " " VERSION, nullptr));
    mActionAbout->setText(QApplication::translate("MainWindow", "About", nullptr));
    mActionExit->setText(QApplication::translate("MainWindow", "Exit", nullptr));
    mSettingGroupBox->setTitle(QApplication::translate("MainWindow", "Settings", nullptr));
    mMasterCheckBox->setText(QApplication::translate("MainWindow", "Override Remote", nullptr));
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
    mPasswordLineEdit->setText(QApplication::translate("MainWindow", "Loading...", nullptr));
    mUserNameLineEdit->setText(QApplication::translate("MainWindow", "Loading...", nullptr));
    mRemoteDirLineEdit->setText(QApplication::translate("MainWindow", "Loading...", nullptr));
    mLocalDirLineEdit->setText(QApplication::translate("MainWindow", "Loading...", nullptr));
    mMenuFile->setTitle(QApplication::translate("MainWindow", "File", nullptr));
    mMenuHelp->setTitle(QApplication::translate("MainWindow", "Help", nullptr));
}

int32_t MainWindowUi::setupCore()
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        mCore = new CoreHandler(this);
        if (NOTNULL(mCore)) {
            rc = mCore->construct();
            if (FAILED(rc)) {
                showError("Failed to construct core.");
            }
        }
    }

    return rc;
}

int32_t MainWindowUi::saveSettings()
{
    int32_t rc = NO_ERROR;
    QFont font = mDebugTextEdit->font();

    if (SUCCEED(rc)) {
        if (mUpdatingItem[CONFIG_DEBUG_SIZE]) {
            QByteArray value = QString::number(font.pointSize()).toLatin1();
            std::string result = value.data();
            rc = mCore->setConfig(CONFIG_DEBUG_SIZE, result);
            if (!SUCCEED(rc)) {
                QString err = "Failed to set debug size ";
                showError(err + value);
            }
            rc = JUMP_DONE;
        }
    }

    if (SUCCEED(rc)) {
        if (mUpdatingItem[CONFIG_DEBUG_FONT]) {
            QByteArray value = font.family().toLatin1();
            std::string result = value.data();
            rc = mCore->setConfig(CONFIG_DEBUG_FONT, result);
            if (!SUCCEED(rc)) {
                QString err = "Failed to set debug font ";
                showError(err + value);
            }
            rc = JUMP_DONE;
        }
    }

    if (SUCCEED(rc)) {
        if (mUpdatingItem[CONFIG_DEBUG_COLOR]) {
            QStringList sections = mDebugColor.split(QRegExp("[()]"));
            QByteArray value = sections.at(1).toLatin1();
            if (value.size() == 0) {
                showError("Invalid origin color, NULL");
                rc = BAD_PROTOCAL;
            } else {
                std::string setting = value.data();
                rc = mCore->setConfig(CONFIG_DEBUG_COLOR, setting);
                if (!SUCCEED(rc)) {
                    QString err = "Failed to set debug color ";
                    showError(err + value);
                }
            }
            rc = JUMP_DONE;
        }
    }

    if (SUCCEED(rc)) {
        if (mUpdatingItem[CONFIG_DEBUG_BG]) {
            QStringList sections = mDebugBg.split(QRegExp("[()]"));
            QByteArray value = sections.at(1).toLatin1();
            if (value.size() == 0) {
                showError("Invalid origin background, NULL");
                rc = BAD_PROTOCAL;
            } else {
                std::string setting = value.data();
                rc = mCore->setConfig(CONFIG_DEBUG_BG, setting);
                if (!SUCCEED(rc)) {
                    QString err = "Failed to set debug size ";
                    showError(err + value);
                }
            }
            rc = JUMP_DONE;
        }
    }

    return RETURNIGNORE(rc, JUMP_DONE);
}

int32_t MainWindowUi::updateConfigResult(ConfigItem item, bool)
{
    int32_t rc = NO_ERROR;

    switch (item) {
        case CONFIG_DEBUG_SIZE:
        case CONFIG_DEBUG_FONT:
        case CONFIG_DEBUG_COLOR:
        case CONFIG_DEBUG_BG:
        case CONFIG_SHELL_SIZE:
        case CONFIG_SHELL_FONT:
        case CONFIG_SHELL_COLOR:
        case CONFIG_SHELL_BG: {
            mUpdatingItem[item] = false;
            rc = saveSettings();
        } break;
        default: {
        }break;
    }

    if (FAILED(rc)) {
        appendDebugger("Update config result error.");
    }

    return rc;
}

int32_t MainWindowUi::destructCore()
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        if (NOTNULL(mCore)) {
            rc = mCore->destruct();
            if (FAILED(rc)) {
                showError("Failed to destruct core.");
            }
        }
        SECURE_DELETE(mCore);
    }

    return rc;
}

int32_t MainWindowUi::updateUi()
{
    return NO_ERROR;
}

int32_t MainWindowUi::onStarted(int32_t rc)
{
    mStarted = SUCCEED(rc);

    QImage Image;
    Image.load(mStarted ? ":/status/succeed" : ":/status/failed");
    QPixmap pixmap = QPixmap::fromImage(Image);
    QPixmap fitPixmap = pixmap.scaled(mStartedLabel->width(),
        mStartedLabel->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    mStartedLabel->setPixmap(fitPixmap);
    mStartPushButton->setText(QApplication::translate("MainWindow",
        mStarted ? " Stop " : " Start ", nullptr));

    return NO_ERROR;
}

int32_t MainWindowUi::onStopped(int32_t rc)
{
    mStarted = FAILED(rc);

    QImage Image;
    Image.load(!mStarted ? ":/status/question" : ":/status/failed");
    QPixmap pixmap = QPixmap::fromImage(Image);
    QPixmap fitPixmap = pixmap.scaled(mStartedLabel->width(),
        mStartedLabel->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    mStartedLabel->setPixmap(fitPixmap);
    mStartPushButton->setText(QApplication::translate("MainWindow",
        !mStarted ? " Start " : " Stop ", nullptr));

    return NO_ERROR;
}

int32_t MainWindowUi::onInitialized(int32_t result)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        QImage Image;
        Image.load(SUCCEED(result) ? ":/status/succeed" : ":/status/failed");
        QPixmap pixmap = QPixmap::fromImage(Image);
        QPixmap fitPixmap = pixmap.scaled(mStatusLabel->width(),
            mStatusLabel->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        mStatusLabel->setPixmap(fitPixmap);
        mStartPushButton->setEnabled(SUCCEED(result));
    }

    if (SUCCEED(rc)) {
        rc = mCore->loadConfig();
        if (FAILED(rc)) {
            showError("Failed to load config.");
        }
    }

    return NO_ERROR;
}

int32_t MainWindowUi::appendDebugger(const QString &str)
{
    mDebugTextEdit->append(str);
    QTextCursor cursor = mDebugTextEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    mDebugTextEdit->setTextCursor(cursor);

    return NO_ERROR;
}

int32_t MainWindowUi::appendShell(const QString &str)
{
    mShellTextEditor->append(str);
    QTextCursor cursor = mShellTextEditor->textCursor();
    cursor.movePosition(QTextCursor::End);
    mShellTextEditor->setTextCursor(cursor);

    return NO_ERROR;
}

void MainWindowUi::showShellWindow(bool checked)
{
    int32_t width = checked ? SCALE(WINDOW_SHELL_WIDTH) : SCALE(DEFAULT_WINDOW_WIDTH);
    int32_t height = SCALE(DEFAULT_WINDOW_HEIGHT);
    mMainWindow->resize(width, height);
    mMainWindow->setFixedSize(width, height);
}

int32_t MainWindowUi::onStartButtonClicked()
{
    int32_t rc = NO_ERROR;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_lock(&mutex);

    if (SUCCEED(rc)) {
        if (mStarted) {
            rc = mCore->stop();
            if (!SUCCEED(rc)) {
                showError("Failed to stop core.");
            }
        } else {
            rc = mCore->start();
            if (!SUCCEED(rc)) {
                showError("Failed to start core.");
            }
        }
    }
    pthread_mutex_unlock(&mutex);

    return rc;
}

int32_t MainWindowUi::onSelectButonClicked()
{
    int32_t rc = NO_ERROR;
    QString path;

    if (SUCCEED(rc)) {
        QString old = mLocalDirLineEdit->text();
        path = QFileDialog::getExistingDirectory(mCentralWidget,
            "Choose Path Directory", old,
            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    }

    if (SUCCEED(rc)) {
        if (path.length() <= 0) {
            rc = NOT_READY;
        } else {
            QByteArray value = path.toLatin1();
            mLocalDirLineEdit->setText(QApplication::translate("MainWindow",
                value.data(), nullptr));
        }
    }

    return rc;
}

int32_t MainWindowUi::updateConfig(ConfigItem item, bool value)
{
    int32_t rc = NO_ERROR;

    switch (item) {
        case CONFIG_MASTER_MODE: {
            mMasterCheckBox->setChecked(value);
            connect(mMasterCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(setConfig(bool)));
        } break;
        case CONFIG_ENCRYPTION: {
            mEncryptionCheckBox->setChecked(value);
            connect(mEncryptionCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(setConfig(bool)));
        } break;
        case CONFIG_DEBUG_MODE: {
            mDebugCheckBox->setChecked(value);
            connect(mDebugCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(setConfig(bool)));
        } break;
        case CONFIG_REMOTE_SHELL: {
            mRemoteControlCheckBox->setChecked(value);
            connect(mRemoteControlCheckBox, SIGNAL(toggled(bool)),
                    this, SLOT(setConfig(bool)));
        } break;
        default: {
            rc = UNKNOWN_ERROR;
        } break;
    }

    return rc;
}

int32_t MainWindowUi::updateConfig(ConfigItem item, const QString &value)
{
    int32_t rc = NO_ERROR;

    switch (item) {
        case CONFIG_USERNAME: {
            mUserNameLineEdit->setText(value);
            connect(mUserNameLineEdit, SIGNAL(textChanged(const QString &)),
                    this, SLOT(setConfig(const QString &)));
        } break;
        case CONFIG_PASSWORD: {
            mPasswordLineEdit->setText(value);
            connect(mPasswordLineEdit, SIGNAL(textChanged(const QString &)),
                    this, SLOT(setConfig(const QString &)));
        } break;
        case CONFIG_LOCAL_PATH: {
            mLocalDirLineEdit->setText(value);
            connect(mLocalDirLineEdit, SIGNAL(textChanged(const QString &)),
                    this, SLOT(setConfig(const QString &)));
        } break;
        case CONFIG_REMOTE_PATH: {
            mRemoteDirLineEdit->setText(value);
            connect(mRemoteDirLineEdit, SIGNAL(textChanged(const QString &)),
                    this, SLOT(setConfig(const QString &)));
        } break;
        case CONFIG_DEBUG_SIZE:
        case CONFIG_DEBUG_FONT: {
            QFont font = mDebugTextEdit->font();
            if (item == CONFIG_DEBUG_SIZE) {
                font.setPointSize(value.toInt());
            } else {
                font.setFamily(value);
            }
            mDebugTextEdit->setFont(font);
        } break;
        case CONFIG_DEBUG_COLOR:
        case CONFIG_DEBUG_BG: {
            int32_t r, g, b;
            QByteArray byte = value.toLatin1();
            sscanf(byte.data(), "%d,%d,%d", &r, &g, &b);
            if (item == CONFIG_DEBUG_COLOR) {
                mDebugColor = QString("color:rgb(%1,%2,%3);").arg(r).arg(g).arg(b);
            } else {
                mDebugBg = QString("background-color:rgb(%1,%2,%3);").arg(r).arg(g).arg(b);
            }
            mDebugTextEdit->setStyleSheet(mDebugColor + mDebugBg);
        } break;
        case CONFIG_SHELL_SIZE:
        case CONFIG_SHELL_FONT:
        case CONFIG_SHELL_COLOR:
        case CONFIG_SHELL_BG: {
        } break;
        default: {
            rc = UNKNOWN_ERROR;
        } break;
    }

    return rc;
}

int32_t MainWindowUi::setConfig(bool checked)
{
    int32_t rc = NO_ERROR;
    QString name = sender()->objectName();

    if (name == "mMasterCheckBox") {
        rc = mCore->setConfig(CONFIG_MASTER_MODE, checked);
    } else if (name == "mDebugCheckBox") {
        rc = mCore->setConfig(CONFIG_DEBUG_MODE, checked);
    } else if (name == "mEncryptionCheckBox") {
        rc = mCore->setConfig(CONFIG_ENCRYPTION, checked);
    } else if (name == "mRemoteControlCheckBox") {
        rc = mCore->setConfig(CONFIG_REMOTE_SHELL, checked);
    }

    if (FAILED(rc)) {
        appendDebugger(name + " Failed to set config.");
    }

    return rc;
}

int32_t MainWindowUi::setConfig(const QString &set)
{
    int32_t rc = NO_ERROR;
    std::string value = set.toLatin1().data();
    QString name = sender()->objectName();

    if (name == "mPasswordLineEdit") {
        rc = mCore->setConfig(CONFIG_PASSWORD, value);
    } else if (name == "mUserNameLineEdit") {
        rc = mCore->setConfig(CONFIG_USERNAME, value);
    } else if (name == "mRemoteDirLineEdit") {
        rc = mCore->setConfig(CONFIG_REMOTE_PATH, value);
    } else if (name == "mLocalDirLineEdit") {
        rc = mCore->setConfig(CONFIG_LOCAL_PATH, value);
    }

    if (FAILED(rc)) {
        appendDebugger(name + " Failed to set config.");
    }

    return rc;
}

}
