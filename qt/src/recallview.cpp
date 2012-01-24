#include "recallview.h"
#include "taglineedit.h"
#include "qtserializerwrapper.h"
#include "itemmodel.h"

#include <QComboBox>
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QTreeView>
#include <QTextEdit>
#include <QSplitter>
#include <QVariant>
#include <QList>

//------------------------------------------------------------------------------
// RecallView Implementation: Ctor
//------------------------------------------------------------------------------
RecallView::RecallView(const QtSerializerWrapper& reader,
                       QWidget *parent)
    : QDialog(parent),
      m_itemModel(new ItemModel),
      m_itemView(0),
      m_tagsEdit(0),
      m_tagsBox(0) {

    initGui(reader);
    setConnections(reader);
}

//------------------------------------------------------------------------------
RecallView::~RecallView() {
    if (m_itemModel) {
        delete m_itemModel;
        m_itemModel = 0;
    }
}

//------------------------------------------------------------------------------
void RecallView::initGui(const QtSerializerWrapper& reader) {
      setWindowTitle(tr("Recap - Recall Mode"));
      setGeometry(0, 0, 800, 600);

      // Splitter layout
      QGridLayout* layout = new QGridLayout(this);
      QSplitter* splitter = new QSplitter;
      layout->addWidget(splitter, 0, 0);

      QFrame* 	   l_frame  = new QFrame;
      QGridLayout* l_layout = new QGridLayout(l_frame);
      splitter->addWidget(l_frame);
      l_frame->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);

      QFrame*      r_frame  = new QFrame;
      QGridLayout* r_layout = new QGridLayout(r_frame);
      splitter->addWidget(r_frame);
      r_frame->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);

      QList<int> sizes;
      sizes.push_back(300);
      sizes.push_back(500);
      splitter->setSizes(sizes);

      // Tag edit
      QLabel* tagEditLabel = new QLabel(tr("T&ags"));
      l_layout->addWidget(tagEditLabel, 0, 0);
      m_tagsEdit = new TagLineEdit(reader.tags(), this);
      l_layout->addWidget(m_tagsEdit, 0, 1);
      tagEditLabel->setBuddy(m_tagsEdit);

      // Tag box
      m_tagsBox = new QComboBox;
      m_tagsBox->addItems(reader.tags());
      l_layout->addWidget(m_tagsBox, 0, 3);

      // Item list
      m_itemModel->setModel(reader.items());

      m_itemView = new QTreeView;
      m_itemView->setRootIsDecorated(false);
      m_itemView->setModel(m_itemModel);
      l_layout->addWidget(m_itemView, 1, 0, 1, 4);

      // Content notes
      QLabel* notesLabel = new QLabel(tr("&Notes"));
      r_layout->addWidget(notesLabel, 0, 0);
      m_contentEdit = new QTextEdit;
      m_contentEdit->setReadOnly(true);
      r_layout->addWidget(m_contentEdit, 1, 0);
      notesLabel->setBuddy(m_contentEdit);
}

//------------------------------------------------------------------------------
void RecallView::setConnections(const QtSerializerWrapper& reader) {

    // Tag updates
    connect(m_tagsBox, SIGNAL(activated(QString)),
          this, SLOT(on_tagsBox_activated(QString)));

    // REMOVE (not needed)
    connect(&reader, SIGNAL(tagsUpdated(QStringList)),
            this, SLOT(on_tagsBox_tagsUpdated(QStringList)));

    // Model updating
    connect(m_tagsEdit, SIGNAL(editingFinished()),
            this, SLOT(reloadModel()));

    connect(this, SIGNAL(sendQueryRequest(QStringList)),
            &reader, SLOT(read(QStringList)));

    connect(&reader, SIGNAL(readCompleted(QVector<QtItemWrapper*>)),
            m_itemModel, SLOT(setModel(QVector<QtItemWrapper*>)));

    // Content notes updating
    connect(m_itemView, SIGNAL(clicked(QModelIndex)),
            this, SLOT(on_itemView_clicked(QModelIndex)));
}

//------------------------------------------------------------------------------
// REMOVE
#include <QDebug>
void RecallView::on_tagsBox_tagsUpdated(const QStringList&) {
    qDebug() << "Tags update";
}

//------------------------------------------------------------------------------
// Add a new tag to the line edit when combo box selection is made
//------------------------------------------------------------------------------
void RecallView::on_tagsBox_activated(const QString& tag) {
    if (m_tagsEdit->addTag(tag)) {
        reloadModel();
    }
}

//------------------------------------------------------------------------------
// Update the combobox and line edit completer when available
// tags change.
//------------------------------------------------------------------------------
void RecallView::updateTagsBoxItems(const QStringList& tags) {
    m_tagsBox->clear();
    m_tagsBox->addItems(tags);
}

//------------------------------------------------------------------------------
void RecallView::on_itemView_clicked(const QModelIndex& index) {
    if (index.isValid()) {
        const QtItemWrapper* item = m_itemModel->itemAt(index);
        if (item) {
            m_contentEdit->setHtml(item->content);
        }
    }
}

//------------------------------------------------------------------------------
void RecallView::reloadModel() {
    emit sendQueryRequest(m_tagsEdit->text().split(TagLineEdit::TagSeparator, QString::SkipEmptyParts));
}
