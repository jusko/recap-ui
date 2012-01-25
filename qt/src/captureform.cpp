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
    gl->addWidget((m_okButton = new QPushButton(tr("&OK"))), 7, 2);
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

    connect(m_tagsEdit, SIGNAL(textEdited(QString)),
            this, SLOT(updateTags(QString)));

    connect(m_contentEdit, SIGNAL(textChanged()),
            this, SLOT(updateContentNotes()));

    connect(m_tagsBox, SIGNAL(activated(QString)),
            this, SLOT(addTag(QString)));

    connect(this, SIGNAL(requestWrite(QtItemWrapper)),
            &writer, SLOT(write(QtItemWrapper)));

    connect(m_okButton, SIGNAL(clicked(bool)),
            this, SLOT(acceptForm(bool)));

    connect(m_cancelButton, SIGNAL(clicked(bool)),
            this, SLOT(rejectForm(bool)));

}

//------------------------------------------------------------------------------
void CaptureForm::acceptForm(bool) {
    emit requestWrite(*m_item);
    accept();
}

//------------------------------------------------------------------------------
// Form rejected.
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
void CaptureForm::updateTags(const QString& tagString) {
    QStringList tags = tagString.split(TagLineEdit::TagSeparator,
                                       QString::SkipEmptyParts);
    m_item->tags = tags;
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
