#include "common.h"
#include "DebuggerSettingUi.h"

namespace uranium {

void DebuggerSettingUi::onFontChanged(const QFont &f)
{
    QFont font = mExampleLabel->font();
    font.setFamily(f.family());
    mExampleLabel->setFont(font);
    newSetting(font, mColorString + mBgColorString);
}

void DebuggerSettingUi::onSizeChanged(int size)
{
    QFont font = mExampleLabel->font();
    font.setPointSize(size);
    mExampleLabel->setFont(font);
    newSetting(font, mColorString + mBgColorString);
}

void DebuggerSettingUi::onColorChanged(QColor c)
{
    mColorString = QString("color:rgb(%1,%2,%3);").arg(c.red()).arg(c.green()).arg(c.blue());
    mExampleLabel->setStyleSheet(mColorString + mBgColorString);
    newSetting(mExampleLabel->font(), mColorString + mBgColorString);
}

void DebuggerSettingUi::onBgColorChanged(QColor c)
{
    mBgColorString = QString("background-color:rgb(%1,%2,%3);").arg(c.red()).arg(c.green()).arg(c.blue());
    mExampleLabel->setStyleSheet(mColorString + mBgColorString);
    newSetting(mExampleLabel->font(), mColorString + mBgColorString);
}

DebuggerSettingUi::DebuggerSettingUi(QDialog *dialog) :
    mDialog(dialog),
    mButtonBox(nullptr),
    mGroupBox(nullptr)
{
}

DebuggerSettingUi::~DebuggerSettingUi()
{
    if (NOTNULL(mGroupBox)) {
        mGroupBox->setParent(nullptr);
        mGroupBox->deleteLater();
    }
    if (NOTNULL(mButtonBox)) {
        mButtonBox->setParent(nullptr);
        mButtonBox->deleteLater();
    }
}

int32_t DebuggerSettingUi::setupUi()
{
    if (mDialog->objectName().isEmpty())
        mDialog->setObjectName(QString::fromUtf8("Dialog"));
    mButtonBox = new QDialogButtonBox(mDialog);
    mButtonBox->setObjectName(QString::fromUtf8("mButtonBox"));
    mButtonBox->setGeometry(QRect(30, 250, 341, 32));
    mButtonBox->setOrientation(Qt::Horizontal);
    mButtonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
    mButtonBox->setCenterButtons(true);
    mGroupBox = new QGroupBox(mDialog);
    mGroupBox->setObjectName(QString::fromUtf8("mGroupBox"));
    mGroupBox->setGeometry(QRect(20, 10, 371, 231));
    gridLayoutWidget = new QWidget(mGroupBox);
    gridLayoutWidget->setObjectName(QString::fromUtf8("gridLayoutWidget"));
    gridLayoutWidget->setGeometry(QRect(10, 30, 351, 191));
    mGridLayout = new QGridLayout(gridLayoutWidget);
    mGridLayout->setObjectName(QString::fromUtf8("mGridLayout"));
    mGridLayout->setContentsMargins(0, 0, 0, 0);
    mFontLabel = new QLabel(gridLayoutWidget);
    mFontLabel->setObjectName(QString::fromUtf8("mFontLabel"));
    QFont font;
    font.setPointSize(10);
    mFontLabel->setFont(font);

    mGridLayout->addWidget(mFontLabel, 1, 0, 1, 1);

    mSizeLabel = new QLabel(gridLayoutWidget);
    mSizeLabel->setObjectName(QString::fromUtf8("mSizeLabel"));
    mSizeLabel->setFont(font);

    mGridLayout->addWidget(mSizeLabel, 0, 0, 1, 1);

    mColorChangeBtn = new QPushButton(gridLayoutWidget);
    mColorChangeBtn->setObjectName(QString::fromUtf8("mColorChangeBtn"));

    mColorDialog = new QColorDialog();
    connect(mColorDialog, SIGNAL(colorSelected(QColor)), this, SLOT(onColorChanged(QColor)));

    connect(mColorChangeBtn, SIGNAL(clicked(bool)), mColorDialog, SLOT(show()));

    mGridLayout->addWidget(mColorChangeBtn, 2, 2, 1, 1);

    mFontComboBox = new QFontComboBox(gridLayoutWidget);
    mFontComboBox->setObjectName(QString::fromUtf8("mFontComboBox"));
    connect(mFontComboBox, SIGNAL(currentFontChanged(const QFont &)), this, SLOT(onFontChanged(const QFont &)));

    mGridLayout->addWidget(mFontComboBox, 1, 1, 1, 2);

    mSpinBox = new QSpinBox(gridLayoutWidget);
    mSpinBox->setObjectName(QString::fromUtf8("mSpinBox"));
    connect(mSpinBox, SIGNAL(valueChanged(int)), this, SLOT(onSizeChanged(int)));

    mGridLayout->addWidget(mSpinBox, 0, 1, 1, 2);

    mColorLabel = new QLabel(gridLayoutWidget);
    mColorLabel->setObjectName(QString::fromUtf8("mColorLabel"));
    mColorLabel->setFont(font);

    mGridLayout->addWidget(mColorLabel, 2, 0, 1, 1);

    mBgLabel = new QLabel(gridLayoutWidget);
    mBgLabel->setObjectName(QString::fromUtf8("mBgLabel"));
    mBgLabel->setFont(font);

    mGridLayout->addWidget(mBgLabel, 3, 0, 1, 1);

    mBgChangeBtn = new QPushButton(gridLayoutWidget);
    mBgChangeBtn->setObjectName(QString::fromUtf8("mBgChangeBtn"));

    mBgColorDialog = new QColorDialog();
    connect(mBgColorDialog, SIGNAL(colorSelected(QColor)), this, SLOT(onBgColorChanged(QColor)));

    connect(mBgChangeBtn, SIGNAL(clicked(bool)), mBgColorDialog, SLOT(show()));

    mGridLayout->addWidget(mBgChangeBtn, 3, 2, 1, 1);

    mExampleLabel = new QLabel(gridLayoutWidget);
    mExampleLabel->setObjectName(QString::fromUtf8("mExampleLabel"));
    QFont font1;
    font1.setPointSize(6);
    mExampleLabel->setFont(font1);
    mExampleLabel->setAlignment(Qt::AlignCenter);

    mGridLayout->addWidget(mExampleLabel, 2, 1, 2, 1);

    retranslateUi();
    QObject::connect(mButtonBox, SIGNAL(accepted()), mDialog, SLOT(accept()));
    QObject::connect(mButtonBox, SIGNAL(rejected()), mDialog, SLOT(reject()));

    return NO_ERROR;
}

void DebuggerSettingUi::retranslateUi()
{
    mDialog->setWindowTitle(QApplication::translate("Dialog", "Font Dialog", nullptr));
    mGroupBox->setTitle(QApplication::translate("Dialog", "Settings", nullptr));
    mFontLabel->setText(QApplication::translate("Dialog", "Font", nullptr));
    mSizeLabel->setText(QApplication::translate("Dialog", "Size", nullptr));
    mColorChangeBtn->setText(QApplication::translate("Dialog", "Change ...", nullptr));
    mColorLabel->setText(QApplication::translate("Dialog", "Color", nullptr));
    mBgLabel->setText(QApplication::translate("Dialog", "Background", nullptr));
    mExampleLabel->setText(QApplication::translate("Dialog", "Example", nullptr));
    mBgChangeBtn->setText(QApplication::translate("Dialog", "Change ...", nullptr));
}

}
