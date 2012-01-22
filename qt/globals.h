#ifndef GLOBALS_H
#define GLOBALS_H
#include <QString>
#include "taglineedit.h"

//------------------------------------------------------------------------------
// Icon file specs
//------------------------------------------------------------------------------
#define OK_ICON     "../../../res/img/dialog-ok.png"
#define CANCEL_ICON "../../../res/img/dialog-cancel.png"
#define TAG_ICON    "../../../res/img/tag.png"

const QString G_SEPARATOR = ",";

//------------------------------------------------------------------------------
// Adds a tag to a line edit if it the tag is not already present.
//------------------------------------------------------------------------------
static void g_addTag(TagLineEdit& edit, const QString& tag) {
    if (!edit.text().contains(tag)) {
        if (!edit.text().isEmpty()) {
            edit.setText(QString("%1, %2").arg(edit.text()).arg(tag));
        }
        else {
            edit.setText(tag);
        }
    }
}

#endif // GLOBALS_H
