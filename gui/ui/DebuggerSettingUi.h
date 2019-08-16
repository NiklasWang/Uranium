#ifndef DEBUGGERSETTING_UI_H_
#define DEBUGGERSETTING_UI_H_

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFontComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>
#include <QColorDialog>

namespace uranium {

class DebuggerSettingUi :
    public QObject
{
    Q_OBJECT

public:
    int32_t setupUi(const QFont &, const QString &);
    void retranslateUi();

public:
    explicit DebuggerSettingUi(QDialog *dialog);
    virtual ~DebuggerSettingUi() override;

Q_SIGNALS:
    void newSetting(const QFont, const QString);

private slots:
    void onFontChanged(const QFont &f);
    void onSizeChanged(int size);
    void onColorChanged(QColor);
    void onBgColorChanged(QColor);
    void onRestoreSettings();

private:
    QDialog          *mDialog;
    QDialogButtonBox *mButtonBox;
    QGroupBox        *mGroupBox;
    QWidget          *gridLayoutWidget;
    QGridLayout      *mGridLayout;
    QLabel           *mFontLabel;
    QLabel           *mSizeLabel;
    QPushButton      *mColorChangeBtn;
    QFontComboBox    *mFontComboBox;
    QSpinBox         *mSpinBox;
    QLabel           *mColorLabel;
    QLabel           *mBgLabel;
    QLabel           *mExampleLabel;
    QPushButton      *mBgChangeBtn;
    QColorDialog     *mColorDialog;
    QString           mColorString;
    QColorDialog     *mBgColorDialog;
    QString           mBgColorString;
    QFont             mOrigFont;
    QString           mOrigColorString;
};

}

#endif
