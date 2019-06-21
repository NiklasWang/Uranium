#include <QMessageBox>

#include "common.h"
#include "Dialogs.h"
#include "MainWindow.h"
#include "ui\MainWindowUi.h"

namespace uranium {

MainWindow *gMW = nullptr;

MainWindow::MainWindow(QApplication *app, QWidget *parent) :
    QMainWindow(parent),
    mConstructed(false),
    mApp(app),
    mParent(parent),
    mUi(new Ui::MainWindow),
    mAbout(nullptr)
{
    gMW = this;

    connect(this, SIGNAL(showDialogSignal(MessageType, const QString)),
            this, SLOT(onDialogShow(MessageType, const QString)));

    connect(mUi, SIGNAL(quit()),  this, SLOT(close()));
    connect(mUi, SIGNAL(about()), this, SLOT(showAbout()));
}

MainWindow::~MainWindow()
{
    if (mConstructed) {
        destruct();
    }
}

int32_t MainWindow::construct()
{
    int32_t rc = NO_ERROR;

    if (mConstructed) {
        rc = ALREADY_INITED;
    }

    if (SUCCEED(rc)) {
        setWindowIcon(QIcon(":/icon/logo1"));
    }


    if (SUCCEED(rc)) {
        rc = mUi->setupUi(this);
        if (rc != NO_ERROR) {
            showError("Failed to construct main window ui, " + rc);
        }
    }

    if (SUCCEED(rc)) {
        rc = mUi->setupCore();
        if (!SUCCEED(rc)) {
            showError("Failed to setup core" + rc);
        }
    }

    if (SUCCEED(rc)) {
        mConstructed = true;
    }

    return rc;
}

int32_t MainWindow::destruct()
{
    int32_t rc = NO_ERROR;

    if (!mConstructed) {
        rc = NOT_INITED;
    } else {
        mConstructed = false;
    }

    if (SUCCEED(rc)) {
        SECURE_DELETE(mUi);
    }

    return rc;
}

int32_t MainWindow::showDialog(MessageType type, const QString &msg)
{
    return showDialogSignal(type, msg);
}

int32_t MainWindow::onDialogShow(MessageType type, const QString msg)
{
    QMessageBox::StandardButton btn =
        QMessageBox::StandardButton::NoButton;

    switch (type) {
        case MESSAGE_TYPE_INFORMATION: {
            btn = QMessageBox::information(this, nullptr, msg);
        }; break;
        case MESSAGE_TYPE_QUESTION: {
            btn = QMessageBox::question(this, nullptr, msg);
        }; break;
        case MESSAGE_TYPE_WARNING: {
            btn = QMessageBox::warning(this, nullptr, msg);
        }; break;
        case MESSAGE_TYPE_ERROR: {
            btn = QMessageBox::warning(this, nullptr, msg);
        }; break;
        case MESSAGE_TYPE_CRITICAL: {
            btn = QMessageBox::critical(this, nullptr, msg);
        }; break;
        case MESSAGE_TYPE_MAX_INVALID:
        default: {
            btn = QMessageBox::critical(this, nullptr, msg);
        }; break;
    }

    return btn;
}

int32_t MainWindow::showAbout()
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        mAbout = new AboutDialog();
        if (ISNULL(mAbout)) {
            showError("Failed to create about dialog");
            rc = NO_MEMORY;
        } else {
            connect(mAbout, SIGNAL(destroyed(QObject*)),
                    this,   SLOT(closeAbout()));
        }
    }

    if (SUCCEED(rc)) {
        rc = mAbout->setup();
        if (!SUCCEED(rc)) {
            showError("Failed to setup about dialog");
        }
    }

    if (SUCCEED(rc)) {
        mAbout->show();
    }

    return rc;
}

void MainWindow::closeAbout()
{
    if (NOTNULL(mAbout)) {
        mAbout->deleteLater();
        mAbout = nullptr;
    }
}

}
