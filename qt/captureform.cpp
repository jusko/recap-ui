#include "captureform.h"
#include "recap.h"
#include "taglineedit.h"
#include "tagshortcutdialog.h"
#include "globals.h"

#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QTextEdit>
#include <QGridLayout>
#include <QCompleter>

//------------------------------------------------------------------------------
// Ctor
//------------------------------------------------------------------------------
CaptureForm::CaptureForm(Item* item,
                         const QStringList& taglist,
                         QWidget* parent) :
    QDialog(parent),
    m_item(item),
    m_titleEdit(0),
    m_tagsEdit(0),
    m_tagsBox(0),
    m_tagShortcutButton(0),
    m_contentEdit(0),
    m_okButton(0),
    m_cancelButton(0),
    m_tagShortcutDialog(new TagShortcutDialog(taglist, this)) {

    initGui(taglist);
    setConnections();
}

//------------------------------------------------------------------------------
// Dtor
//------------------------------------------------------------------------------
CaptureForm::~CaptureForm() {
    // NOTE: This is only for testing. External management will be done on the item.
    if (m_item) { delete m_item;
        m_item = 0;
    }
}

//------------------------------------------------------------------------------
// Initialise all gui elements
// TODO: Add capture icon
//------------------------------------------------------------------------------
void CaptureForm::initGui(const QStringList& taglist) {
    setWindowTitle(tr("Recap - Capture Mode"));
    setGeometry(0, 0, 400, 300);

    QGridLayout* gl = new QGridLayout(this);

    // Title
    QLabel* titleLabel = new QLabel(tr("&Title"));
    gl->addWidget(titleLabel, 0, 0);
    gl->addWidget((m_titleEdit = new QLineEdit), 0, 1, 1, 3);
    titleLabel->setBuddy(m_titleEdit);

    // Tags
    QLabel* tagsLabel = new QLabel(tr("T&ags"));
    gl->addWidget(tagsLabel, 1, 0);
    gl->addWidget((m_tagsEdit = new TagLineEdit(taglist)), 1, 1);
    tagsLabel->setBuddy(m_tagsEdit);

    gl->addWidget((m_tagsBox = new QComboBox), 1, 2);
    m_tagsBox->addItems(taglist);

    gl->addWidget((m_tagShortcutButton = new QPushButton(QIcon(TAG_ICON), "")), 1, 3);

    // Content
    QLabel* contentLabel = new QLabel(tr("&Notes"));
    gl->addWidget(contentLabel, 2, 0);
    gl->addWidget((m_contentEdit = new QTextEdit), 3, 0, 4, 4);
    contentLabel->setBuddy(m_contentEdit);
    m_contentEdit->setTabStopWidth(8);
    m_contentEdit->setAutoFormatting(QTextEdit::AutoBulletList);
    m_contentEdit->setWhatsThis(tr("Pasted notes can be formatted (html and rich text are both supported). "
                                   "Bullet points can be added by entering the '*' or '-' characters."));
    // OK/Cancel
    gl->addWidget((m_okButton = new QPushButton(tr("&OK"))), 7, 2);
    gl->addWidget((m_cancelButton = new QPushButton(tr("&Cancel"))), 7, 3);
    m_okButton->setIcon(QIcon(OK_ICON));
    m_cancelButton->setIcon(QIcon(CANCEL_ICON));
}

//------------------------------------------------------------------------------
// Connect all signals/slots
//------------------------------------------------------------------------------
void CaptureForm::setConnections() {
    connect(m_tagsBox, SIGNAL(activated(QString)),
            this, SLOT(on_tagsBox_activated(QString)));

    connect(m_tagShortcutButton, SIGNAL(clicked(bool)),
            this, SLOT(on_tagsShortcutButton_clicked(bool)));
}

//------------------------------------------------------------------------------
// Displays the tag shortcut manager dialog.
//------------------------------------------------------------------------------
void CaptureForm::on_tagsShortcutButton_clicked(bool clicked) {
    m_tagShortcutDialog->show();
}

//------------------------------------------------------------------------------
// Form accepted.
//------------------------------------------------------------------------------
void CaptureForm::on_okButton_clicked(bool clicked) {
}

//------------------------------------------------------------------------------
// Form rejected.
//------------------------------------------------------------------------------
void CaptureForm::on_cancelButton_clicked(bool clicked) {
}

//------------------------------------------------------------------------------
// Clear form of existing data
//------------------------------------------------------------------------------
void CaptureForm::clear() {
}


//------------------------------------------------------------------------------
// Add a new tag to the line edit when combo box selection is made
//------------------------------------------------------------------------------
void CaptureForm::on_tagsBox_activated(const QString &tag) {
    g_addTag(*m_tagsEdit, tag);
}
