#include <QObject>

#include "version.h"
#include "utils/common.h"
#include "ui/AboutUi.h"

#define ABOUT_DIALOG_WIDTH  420
#define ABOUT_DIALOG_HEIGHT 320
#define ABOUT_DIALOG_LEFT_MARGIN 10
#define ABOUT_DIALOG_TOP_MARGIN  10

namespace uranium {

AboutUi::AboutUi(QDialog *dialog) :
    mDialog(dialog),
    mGridLayoutWidget(nullptr),
    mGridLayout(nullptr),
    mPictureLabel(nullptr),
    mTitleLabel(nullptr),
    mVersionLabel(nullptr),
    mCopyrightLabel(nullptr),
    mContactLabel(nullptr),
    mSupportLabel(nullptr),
    mOkButton(nullptr),
    mVerticalSpacer(nullptr),
    mLeftHorizontalSpacer(nullptr),
    mMiddleHorizontalSpacer(nullptr),
    mRightHorizontalSpacer(nullptr)
{
}

AboutUi::~AboutUi()
{
    if (NOTNULL(mGridLayoutWidget)) {
        mGridLayoutWidget->setParent(nullptr);
        mGridLayoutWidget->deleteLater();
    }
}

int32_t AboutUi::setupUi()
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        if (mDialog->objectName().isEmpty()) {
            mDialog->setObjectName(QStringLiteral("AboutDialog"));
        }
        mDialog->resize(ABOUT_DIALOG_WIDTH, ABOUT_DIALOG_HEIGHT);
    }

    if (SUCCEED(rc)) {
        mGridLayoutWidget = new QWidget(mDialog);
        mGridLayoutWidget->setObjectName(QStringLiteral("mGridLayoutWidget"));
        mGridLayoutWidget->setGeometry(QRect(
            ABOUT_DIALOG_LEFT_MARGIN, ABOUT_DIALOG_TOP_MARGIN,
            ABOUT_DIALOG_WIDTH - 2 * ABOUT_DIALOG_LEFT_MARGIN,
            ABOUT_DIALOG_HEIGHT - 2 * ABOUT_DIALOG_TOP_MARGIN));
        mGridLayout = new QGridLayout(mGridLayoutWidget);
        mGridLayout->setObjectName(QStringLiteral("mGridLayout"));
        mGridLayout->setContentsMargins(0, 0, 0, 0);
    }

    if (SUCCEED(rc)) {
        mPictureLabel = new QLabel(mGridLayoutWidget);
        mPictureLabel->setObjectName(QStringLiteral("mPictureLabel"));
        mPictureLabel->setMinimumSize(QSize(125, 200));

        QMatrix rotation;
        rotation.rotate(90);
        QImage Image;
        Image.load(":/icon/IconColor_16x10");
        QPixmap pixmap = QPixmap::fromImage(Image);
        pixmap.transformed(rotation, Qt::SmoothTransformation);
        QPixmap fitPixmap = pixmap.scaled(mPictureLabel->width(),
            mPictureLabel->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        mPictureLabel->setPixmap(fitPixmap);
        mGridLayout->addWidget(mPictureLabel, 0, 1, 5, 1);
    }

    if (SUCCEED(rc)) {
        mTitleLabel = new QLabel(mGridLayoutWidget);
        mTitleLabel->setObjectName(QStringLiteral("mTitleLabel"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(mTitleLabel->sizePolicy().hasHeightForWidth());
        mTitleLabel->setSizePolicy(sizePolicy);
        QFont font;
        font.setFamily(QStringLiteral("Courier"));
        font.setPointSize(20);
        font.setBold(true);
        font.setWeight(75);
        mTitleLabel->setFont(font);
        mGridLayout->addWidget(mTitleLabel, 0, 3, 1, 1);
    }

    if (SUCCEED(rc)) {
        mVersionLabel = new QLabel(mGridLayoutWidget);
        mVersionLabel->setObjectName(QStringLiteral("mVersionLabel"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(mVersionLabel->sizePolicy().hasHeightForWidth());
        mVersionLabel->setSizePolicy(sizePolicy);
        QFont font;
        font.setFamily(QStringLiteral("Courier"));
        font.setPointSize(13);
        font.setBold(true);
        font.setWeight(75);
        mVersionLabel->setFont(font);
        mGridLayout->addWidget(mVersionLabel, 1, 3, 1, 1);
    }

    if (SUCCEED(rc)) {
        mContactLabel = new QLabel(mGridLayoutWidget);
        mContactLabel->setObjectName(QStringLiteral("mContactLabel"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(mContactLabel->sizePolicy().hasHeightForWidth());
        mContactLabel->setSizePolicy(sizePolicy);
        QFont font;
        font.setFamily(QStringLiteral("Calibri"));
        font.setPointSize(11);
        mContactLabel->setFont(font);
        mContactLabel->setFrameShape(QFrame::Box);
        mContactLabel->setStyleSheet("border-width: 1px; "
            "border-style: solid; border-color: blue;");
        mGridLayout->addWidget(mContactLabel, 3, 3, 1, 1);
    }

    if (SUCCEED(rc)) {
        mCopyrightLabel = new QLabel(mGridLayoutWidget);
        mCopyrightLabel->setObjectName(QStringLiteral("mCopyrightLabel"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(mCopyrightLabel->sizePolicy().hasHeightForWidth());
        mCopyrightLabel->setSizePolicy(sizePolicy);
        QFont font;
        font.setFamily(QStringLiteral("Calibri"));
        font.setPointSize(11);
        mCopyrightLabel->setFont(font);
        mGridLayout->addWidget(mCopyrightLabel, 2, 3, 1, 1);
    }

    if (SUCCEED(rc)) {
        mSupportLabel = new QLabel(mGridLayoutWidget);
        mSupportLabel->setObjectName(QStringLiteral("mSupportLabel"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(mSupportLabel->sizePolicy().hasHeightForWidth());
        mSupportLabel->setSizePolicy(sizePolicy);
        QFont font;
        font.setFamily(QStringLiteral("Calibri"));
        font.setPointSize(10);
        mSupportLabel->setFont(font);
        mGridLayout->addWidget(mSupportLabel, 4, 3, 1, 1);
    }

    if (SUCCEED(rc)) {
        mVerticalSpacer = new QSpacerItem(20, 40,
            QSizePolicy::Minimum, QSizePolicy::Expanding);
        mGridLayout->addItem(mVerticalSpacer, 5, 1, 1, 3);

        mLeftHorizontalSpacer = new QSpacerItem(40, 20,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
        mGridLayout->addItem(mLeftHorizontalSpacer, 0, 0, 7, 1);

        mMiddleHorizontalSpacer = new QSpacerItem(40, 20,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
        mGridLayout->addItem(mMiddleHorizontalSpacer, 0, 2, 5, 1);

        mRightHorizontalSpacer = new QSpacerItem(40, 20,
            QSizePolicy::Expanding, QSizePolicy::Minimum);
        mGridLayout->addItem(mRightHorizontalSpacer, 0, 4, 7, 1);
    }

    if (SUCCEED(rc)) {
        mOkButton = new QPushButton(mGridLayoutWidget);
        mOkButton->setObjectName(QStringLiteral("mOkButton"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(mOkButton->sizePolicy().hasHeightForWidth());
        mOkButton->setSizePolicy(sizePolicy);
        mOkButton->setMinimumSize(QSize(80, 30));
        QFont font;
        font.setFamily(QStringLiteral("Courier"));
        font.setPointSize(13);
        font.setBold(true);
        font.setWeight(75);
        mOkButton->setFont(font);
        mGridLayout->addWidget(mOkButton, 6, 1, 1, 3, Qt::AlignHCenter);
    }

    if (SUCCEED(rc)) {
        rc = retranslateUi();
        mGridLayoutWidget->show();
        QMetaObject::connectSlotsByName(mDialog);

        QObject::connect(mOkButton, SIGNAL(clicked(bool)),
                         mDialog,   SLOT(close()));
    }

    return rc;
}

int32_t AboutUi::retranslateUi()
{
    QString proj = PROJNAME;
    QString title = "About " + proj;
    mDialog->setWindowTitle(QApplication::translate("mDialog",
        title.toLocal8Bit().data(), nullptr));
    mTitleLabel->setText(QApplication::translate("mDialog",
        proj.toLocal8Bit().data(), nullptr));
    QString ver = VERSION;
    QString version = "Version " + ver + " - 32bit";
    mVersionLabel->setText(QApplication::translate("mDialog",
        version.toLocal8Bit().data(), nullptr));
    mCopyrightLabel->setText(QApplication::translate("mDialog",
        "Copyright (C) 2018\n"
        "by Wang Liang\n"
        "Lenovo Corporation", nullptr));
    mContactLabel->setText(QApplication::translate("mDialog",
        "Contact:\n"
        "+86 186 1153 3741\n"
        "wangliang17@lenovo.com\n"
        "niklas.wang@outlook.com", nullptr));
    mSupportLabel->setText(QApplication::translate("mDialog",
        "Thanks for choosing.\n"
        "Please contact Wang Liang\n"
        "for technical support.", nullptr));
    mOkButton->setText(QApplication::translate("mDialog",
        "OK", nullptr));

    return NO_ERROR;
}

}
