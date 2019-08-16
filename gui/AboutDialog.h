#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

#include "ui/AboutUi.h"

namespace uranium {

class AboutDialog :
    public QDialog
{
public:
    int32_t setup();

public:
    AboutDialog();
    virtual ~AboutDialog();

private:
    AboutUi *mUi;
};

}

#endif
