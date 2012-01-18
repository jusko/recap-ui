#include "tagshortcutdialog.h"
#include <QLabel>

//------------------------------------------------------------------------------
// Ctor
//------------------------------------------------------------------------------
TagShortcutDialog::TagShortcutDialog(const QStringList& tags, QWidget *parent) :
    QDialog(parent) {

    setModal(true);
    new QLabel("TBD: Tag shortcut management", this);

}
