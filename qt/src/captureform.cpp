//------------------------------------------------------------------------------
#include "captureform.h"
#include "recap.h"
#include "taglineedit.h"
#include "qtserializerwrapper.h"
//------------------------------------------------------------------------------
#include <QLabel>
#include <QCommonStyle>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QTextEdit>
#include <QGridLayout>
#include <QKeyEvent>
#include <QMessageBox>
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
// PRE: m_item has been set with the address of a new item
//------------------------------------------------------------------------------
void CaptureForm::show(QtItemWrapper *item) {
    if (item) {
        setItem(item);
    }
    else if (m_item) {
        resetForm();
    }
    QDialog::show();
}

//------------------------------------------------------------------------------
void CaptureForm::setItem(QtItemWrapper* item) {
    m_item = item;
    m_titleEdit->setText(item->title);
    m_tagsEdit->setText(item->tags.join(TagLineEdit::TagSeparator));;
    m_contentEdit->setHtml(item->content);
}

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
    gl->addWidget((m_okButton = new QPushButton(tr("&Save"))), 7, 2);
    gl->addWidget((m_cancelButton = new QPushButton(tr("&Cancel"))), 7, 3);
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
    m_item->content = m_contentEdit->toHtml();
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
    if (event->key() == Qt::Key_Escape &&
        QMessageBox::warning(this,
                             tr("Recap"),
                             tr("Discard the current note?"),
                             QMessageBox::Yes |
                             QMessageBox::No) != QMessageBox::Yes) {
        return;
    }
    QDialog::keyPressEvent(event);
}
