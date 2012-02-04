//------------------------------------------------------------------------------
#include "captureform.h"
#include "recap.h"
#include "taglineedit.h"
#include "qtserializerwrapper.h"
#include "cryptomediator.h"
//------------------------------------------------------------------------------
#include <QLabel>
#include <QCommonStyle>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QGridLayout>
#include <QKeyEvent>
#include <QMessageBox>
#include <QCheckBox>
#include <QHBoxLayout>
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Ctor
//------------------------------------------------------------------------------
CaptureForm::CaptureForm(const QtSerializerWrapper &writer, QWidget *parent)
    : QDialog(parent),
      m_item(new QtItemWrapper),
      m_titleEdit(0),
      m_tagsEdit(0),
      m_tagsBox(0),
      m_contentEdit(0),
      m_okButton(0),
      m_cancelButton(0) {

    m_item->id = 0;
    initGui(writer);
    setConnections(writer);
}

//------------------------------------------------------------------------------
// Dtor
//------------------------------------------------------------------------------
CaptureForm::~CaptureForm() {
    if (m_item) {
        delete m_item;
        m_item = 0;
    }
}

//------------------------------------------------------------------------------
void CaptureForm::showEvent(QShowEvent* ) {
    initCrypto();
}

//------------------------------------------------------------------------------
void CaptureForm::setItem(QtItemWrapper* item) {
    m_item = item;
    m_titleEdit->setText(item->title);
    m_tagsEdit->setText(item->tags.join(TagLineEdit::TagSeparator));;
    m_contentEdit->setPlainText(item->content);
}

//------------------------------------------------------------------------------
void CaptureForm::initGui(const QtSerializerWrapper &writer) {
    setWindowTitle(tr("Recap - Capture Mode"));
    setGeometry(0, 0, 400, 300);

    QGridLayout* gl = new QGridLayout(this);
    QHBoxLayout* line1 = new QHBoxLayout(this);
    QHBoxLayout* line2 = new QHBoxLayout(this);

    // Title
    QLabel* titleLabel = new QLabel(tr("&Title"));
    line1->addWidget(titleLabel);
    m_titleEdit = new QLineEdit;
    line1->addWidget(m_titleEdit);
    titleLabel->setBuddy(m_titleEdit);
    gl->addLayout(line1, 0, 0, 1, 4);

    // Tags
    QLabel* tagsLabel = new QLabel(tr("T&ags"));
    line2->addWidget(tagsLabel);
    line2->addWidget((m_tagsEdit = new TagLineEdit(writer.tags())));
    tagsLabel->setBuddy(m_tagsEdit);

    line2->addWidget((m_tagsBox = new QComboBox));
    m_tagsBox->addItems(writer.tags());
    gl->addLayout(line2, 1, 0, 1, 4);

    // Content
    QLabel* contentLabel = new QLabel(tr("&Notes"));
    gl->addWidget(contentLabel, 3, 0);
    gl->addWidget((m_contentEdit = new QPlainTextEdit), 4, 0, 4, 4);
    contentLabel->setBuddy(m_contentEdit);
    m_contentEdit->setTabStopWidth(8);
    m_contentEdit->setWhatsThis(tr("Pasted notes can be formatted (html and rich text are both supported). "
                                   "Bullet points can be added by entering the '*' or '-' characters."));

    // Encrypt
    m_encryptCheckBox = new QCheckBox(tr("&Encrypt"), this);
    m_encryptCheckBox->setToolTip(tr("The notes of the current item will be encrypted."));
    gl->addWidget(m_encryptCheckBox, 8, 0);
    m_encryptCheckBox->setVisible(false);
    m_encryptCheckBox->setEnabled(false);

    // OK/Cancel
    gl->addWidget((m_okButton = new QPushButton(tr("&Save"))), 8, 2);
    gl->addWidget((m_cancelButton = new QPushButton(tr("&Cancel"))), 8, 3);
    m_okButton->setIcon(QCommonStyle().standardIcon(QCommonStyle::SP_DialogOkButton));
    m_cancelButton->setIcon(QCommonStyle().standardIcon(QCommonStyle::SP_DialogCancelButton));
}

//------------------------------------------------------------------------------
// Connect all signals/slots
//------------------------------------------------------------------------------
void CaptureForm::setConnections(const QtSerializerWrapper& writer) {
    connect(m_titleEdit, SIGNAL(textEdited(QString)),
            this, SLOT(setItemTitle(QString)));

    connect(m_tagsEdit, SIGNAL(editingFinished()),
            this, SLOT(setTags()));

    connect(m_tagsBox, SIGNAL(activated(QString)),
            this, SLOT(addTag(QString)));

    connect(m_contentEdit, SIGNAL(textChanged()),
            this, SLOT(updateContentNotes()));

    connect(this, SIGNAL(requestWrite(QtItemWrapper)),
            &writer, SLOT(write(QtItemWrapper)));

    connect(m_okButton, SIGNAL(clicked(bool)),
            this, SLOT(acceptForm(bool)));

    connect(m_cancelButton, SIGNAL(clicked(bool)),
            this, SLOT(rejectForm(bool)));
}

//------------------------------------------------------------------------------
// Warn about items that will be orphaned.
//------------------------------------------------------------------------------
void CaptureForm::acceptForm(bool) {
    if (!validateForm()) {
        return;
    }
    CryptoMediator cm;
    if (cm.gpgEnabled() &&
        m_encryptCheckBox->isChecked() &&
        !cm.encrypt(*m_item)) {

        QMessageBox::StandardButton reply =  QMessageBox::critical(
            this,
            "Recap",
            tr("The encryption operation failed (%1).\n\n"
               "You can opt to save the item unencrypted anyway, discard the "
               "item or cancel to go back and re-edit it.").arg(cm.lastError()),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel
        );
        switch (reply) {
            case QMessageBox::Save:
                break;
            case QMessageBox::Discard:
                reject();
                return;
            case QMessageBox::Cancel:
            default:
                return;
        }
    }
    emit requestWrite(*m_item);
    accept();
}

//------------------------------------------------------------------------------
// Checks whether the form input is valid.
// @return false
//		   If no title or content is present or if the user canceled the saving
//		   of an orphaned item (i.e., an item without tags).
// @return true
//		   If all fields contain input or if only the tags field is empty, but
//		   the user confirmed the warning about that.
//------------------------------------------------------------------------------
bool CaptureForm::validateForm() const {
    if (m_titleEdit->text().isEmpty()) {
        QMessageBox::critical(const_cast<CaptureForm*>(this), "Recap",
                              tr("Please enter a title."));
        m_titleEdit->setFocus();
        return false;
    }
    if (m_contentEdit->toPlainText().isEmpty()) {
        QMessageBox::critical(const_cast<CaptureForm*>(this), "Recap",
                              tr("No notes present."));
        m_contentEdit->setFocus();
        return false;
    }
    if (m_tagsEdit->isEmpty() &&
        QMessageBox::warning(const_cast<CaptureForm*>(this),
                             tr("Recap"),
                             tr("The note is about to be saved but no tags "
                                "have been entered. The notes will be orphaned."
                                " Is this what you want?"),
                             QMessageBox::Yes |
                             QMessageBox::No) != QMessageBox::Yes) {
        return false;
    }
    return true;
}
//------------------------------------------------------------------------------
void CaptureForm::rejectForm(bool) {
    reject();
    resetForm();
}

//------------------------------------------------------------------------------
void CaptureForm::resetForm() {
    m_item->id = 0;

    m_item->title.clear();
    m_titleEdit->clear();

    m_item->content.clear();
    m_contentEdit->clear();

    m_item->tags.clear();
    m_tagsEdit->clear();
}

//------------------------------------------------------------------------------
void CaptureForm::setItemTitle(const QString& title) {
    m_item->title = title;
}

//------------------------------------------------------------------------------
void CaptureForm::setTags() {
    m_item->tags = m_tagsEdit->tags();
}

//------------------------------------------------------------------------------
void CaptureForm::updateContentNotes() {
    m_item->content = m_contentEdit->toPlainText();
}

//------------------------------------------------------------------------------
void CaptureForm::addTag(const QString &tag) {
    m_tagsEdit->addTag(tag);
    if (!m_item->tags.contains(tag)) {
        m_item->tags.push_back(tag);
    }
}

//------------------------------------------------------------------------------
// Confirm canceling through the Esc key.
//------------------------------------------------------------------------------
void CaptureForm::keyPressEvent(QKeyEvent* event) {
    if (!m_titleEdit->text().isEmpty() 			&&
        !m_contentEdit->toPlainText().isEmpty() &&
        event->key() == Qt::Key_Escape 			&&
        QMessageBox::warning(this,
                             tr("Recap"),
                             tr("Discard the current note?"),
                             QMessageBox::Yes |
                             QMessageBox::No) != QMessageBox::Yes) {
        return;
    }
    QDialog::keyPressEvent(event);
}

//------------------------------------------------------------------------------
void CaptureForm::setEncryption(bool enable) {
    m_encryptCheckBox->setVisible(enable);
    m_encryptCheckBox->setEnabled(enable);
}

//------------------------------------------------------------------------------
// Initialise the cryptographic engine (which may prompt the user for several
// choices on the first run) and setup crypto features accordingly.
//------------------------------------------------------------------------------
void CaptureForm::initCrypto() {
    CryptoMediator* cm = new CryptoMediator(this);
    setEncryption(cm->gpgEnabled());
}
