#include <QApplication>
#include <iostream>

#include "common.h"
#include "MainWindow.h"
#include "cygload.h"

using namespace uranium;

int main(int argc, char *argv[])
{
    cygwin::padding padding;
    cygwin::padding::check(&std::cout);

    int32_t rc = NO_ERROR;

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication a(argc, argv);
    MainWindow w(&a);

    rc = w.construct();
    if (SUCCEED(rc)) {
        w.show();
        rc = a.exec();
    } else {
        uranium::showError("Failed to construct main window, " + rc);
    }

    return rc;
}
