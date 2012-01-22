#include "tagshortcutdialog.h"
#include <QLabel>

//------------------------------------------------------------------------------
// Ctor
// REOMOVE
//------------------------------------------------------------------------------
TagShortcutDialog::TagShortcutDialog(const QStringList& , QWidget *parent) :
    QDialog(parent) {

    setModal(true);
    new QLabel("TBD: Tag shortcut management", this);

}
