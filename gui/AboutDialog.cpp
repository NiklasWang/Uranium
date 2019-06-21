#include "common.h"
#include "Dialogs.h"
#include "AboutDialog.h"

namespace uranium {

int32_t AboutDialog::setup()
{
    int32_t rc = NO_ERROR;

    if (ISNULL(mUi)) {
        mUi = new AboutUi(this);
        if (ISNULL(mUi)) {
            showError("Failed to create about ui");
            rc = NO_MEMORY;
        } else {
            setAttribute(Qt::WA_DeleteOnClose);
        }
    }

    if (SUCCEED(rc)) {
        rc = mUi->setupUi();
        if (!SUCCEED(rc)) {
            showError("Failed to setup about ui");
        }
    }

    return rc;
}

AboutDialog::AboutDialog() :
    mUi(nullptr)
{
}

AboutDialog::~AboutDialog()
{
    if (NOTNULL(mUi)) {
        SECURE_DELETE(mUi);
    }
}

}
