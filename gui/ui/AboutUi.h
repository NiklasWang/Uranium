#ifndef ABOUTUI_H
#define ABOUTUI_H

#include <QObject>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

namespace uranium {

class AboutUi
{
public:
    int32_t setupUi();
    int32_t retranslateUi();

public:
    explicit AboutUi(QDialog *dialog);
    virtual ~AboutUi();

private:
    QDialog     *mDialog;
    QWidget     *mGridLayoutWidget;
    QGridLayout *mGridLayout;
    QLabel      *mPictureLabel;
    QLabel      *mTitleLabel;
    QLabel      *mVersionLabel;
    QLabel      *mCopyrightLabel;
    QLabel      *mContactLabel;
    QLabel      *mSupportLabel;
    QPushButton *mOkButton;
    QSpacerItem *mVerticalSpacer;
    QSpacerItem *mLeftHorizontalSpacer;
    QSpacerItem *mMiddleHorizontalSpacer;
    QSpacerItem *mRightHorizontalSpacer;
};

}

#endif
