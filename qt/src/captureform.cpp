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
CaptureForm::CaptureForm(const QtSerializerWrapper &writer, QWidget *parent)
    : QDialog(parent),
      m_titleEdit(0),
      m_tagsEdit(0),
      m_tagsBox(0),
      m_tagShortcutButton(0),
      m_contentEdit(0),
      m_okButton(0),
      m_cancelButton(0),
      //REMOVE
      m_tagShortcutDialog(new TagShortcutDialog(writer.tags(), this)) {

    initGui(writer);
    setConnections(writer);
}

//------------------------------------------------------------------------------
// Dtor
//------------------------------------------------------------------------------
CaptureForm::~CaptureForm() {
    // NOTE: This is only for testing. External management will be done on the item.
//    if (m_item) {
//        delete m_item;
//        m_item = 0;
//    }
}

//------------------------------------------------------------------------------
void CaptureForm::show(const QtItemWrapper &item) {
    setItem(item);
    QDialog::show();
}

void CaptureForm::setItem(const QtItemWrapper& item) {
    m_item = item;
    m_titleEdit->setText(item.title);
    m_tagsEdit->setText(item.tags.join(QString("%1 ").arg(G_SEPARATOR)));;
    m_contentEdit->setHtml(item.content);
}

//------------------------------------------------------------------------------
// Initialise all gui elements
// TODO: Add capture icon
//------------------------------------------------------------------------------
void CaptureForm::initGui(const QtSerializerWrapper &writer) {
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
    gl->addWidget((m_tagsEdit = new TagLineEdit(writer.tags())), 1, 1);
    tagsLabel->setBuddy(m_tagsEdit);

    gl->addWidget((m_tagsBox = new QComboBox), 1, 2);
    m_tagsBox->addItems(writer.tags());

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
void CaptureForm::setConnections(const QtSerializerWrapper& writer) {
    connect(m_titleEdit, SIGNAL(textEdited(QString)),
            this, SLOT(on_titleEdit_textEdited(QString)));

    connect(m_tagsEdit, SIGNAL(textEdited(QString)),
            this, SLOT(on_tagsEdit_textEdited(QString)));

    connect(m_contentEdit, SIGNAL(textChanged()),
            this, SLOT(on_contentEdit_textChanged()));

    connect(m_tagsBox, SIGNAL(activated(QString)),
            this, SLOT(on_tagsBox_activated(QString)));

    connect(&writer, SIGNAL(tagsUpdated(const QStringList&)),
            this, SLOT(updateTagsBoxItems(const QStringList&)));

    connect(this, SIGNAL(requestWrite(QtItemWrapper)),
            &writer, SLOT(write(QtItemWrapper)));

    connect(m_okButton, SIGNAL(clicked(bool)),
            this, SLOT(on_okButton_clicked(bool)));

    connect(m_cancelButton, SIGNAL(clicked(bool)),
            this, SLOT(on_cancelButton_clicked(bool)));

    // REMOVE: shortcuts can just be set in a config file
    connect(m_tagShortcutButton, SIGNAL(clicked(bool)),
            this, SLOT(on_tagsShortcutButton_clicked(bool)));

}

//------------------------------------------------------------------------------
// Displays the tag shortcut manager dialog.
//------------------------------------------------------------------------------
void CaptureForm::on_tagsShortcutButton_clicked(bool) {
    m_tagShortcutDialog->show();
}

//------------------------------------------------------------------------------
// Form accepted.
//------------------------------------------------------------------------------
void CaptureForm::on_okButton_clicked(bool) {
    emit requestWrite(m_item);
    accept();
}

//------------------------------------------------------------------------------
// Form rejected.
//------------------------------------------------------------------------------
void CaptureForm::on_cancelButton_clicked(bool) {
    reject();
    clear();
}

//------------------------------------------------------------------------------
// Clear form of existing data
//------------------------------------------------------------------------------
void CaptureForm::clear() {
    m_item.id = 0;

    m_item.title.clear();
    m_titleEdit->clear();

    m_item.content.clear();
    m_contentEdit->clear();

    m_item.tags.clear();
    m_tagsEdit->clear();
}

//------------------------------------------------------------------------------
void CaptureForm::on_titleEdit_textEdited(const QString& title) {
    m_item.title = title;
}

//------------------------------------------------------------------------------
void CaptureForm::on_tagsEdit_textEdited(const QString& tagString) {
    QStringList tags = tagString.split(G_SEPARATOR, QString::SkipEmptyParts);
    m_item.tags = tags;
}

//------------------------------------------------------------------------------
void CaptureForm::on_contentEdit_textChanged() {
    m_item.content = m_contentEdit->toHtml();
}


//------------------------------------------------------------------------------
// Add a new tag to the line edit when combo box selection is made
//------------------------------------------------------------------------------
void CaptureForm::on_tagsBox_activated(const QString &tag) {
    g_addTag(*m_tagsEdit, tag);
    if (!m_item.tags.contains(tag)) {
        m_item.tags.push_back(tag);
    }
}

//------------------------------------------------------------------------------
// Updates the list of tags.
// Signalled by QtSerializerWrapper after new tags are added to the DB.
//------------------------------------------------------------------------------
void CaptureForm::updateTagsBoxItems(const QStringList &tags) {
    g_update(*m_tagsBox, *m_tagsEdit, tags);
}
