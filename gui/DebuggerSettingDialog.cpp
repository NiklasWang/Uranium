#include "common.h"
#include "Dialogs.h"
#include "DebuggerSettingDialog.h"

namespace uranium {

int32_t DebuggerSettingDialog::setup(const QFont &font, const QString &style)
{
    int32_t rc = NO_ERROR;

    if (ISNULL(mUi)) {
        mUi = new DebuggerSettingUi(this);
        if (ISNULL(mUi)) {
            showError("Failed to create about ui");
            rc = NO_MEMORY;
        } else {
            setAttribute(Qt::WA_DeleteOnClose);
        }
    }

    if (SUCCEED(rc)) {
        rc = mUi->setupUi(font, style);
        if (!SUCCEED(rc)) {
            showError("Failed to setup about ui");
        } else {
            connect(mUi,  &DebuggerSettingUi::newSetting,
                    this, &DebuggerSettingDialog::newSetting);
        }
    }

    return rc;
}

DebuggerSettingDialog::DebuggerSettingDialog() :
    mUi(nullptr)
{
}

DebuggerSettingDialog::~DebuggerSettingDialog()
{
    if (NOTNULL(mUi)) {
        SECURE_DELETE(mUi);
    }
}

}
