//------------------------------------------------------------------------------
#include "recallview.h"
#include "taglineedit.h"
#include "qtserializerwrapper.h"
#include "itemmodel.h"
//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
// Ctor
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
// Dtor
//------------------------------------------------------------------------------
RecallView::~RecallView() {
    if (m_itemModel) {
        delete m_itemModel;
        m_itemModel = 0;
    }
}

//------------------------------------------------------------------------------
// TODO: Try QDockWidget again (might be easiest to inherit QMainWindow).
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
      m_itemModel->resetWith(reader.items());

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
            this, SLOT(updateItemSet(QString)));

    // Model updating
    connect(m_tagsEdit, SIGNAL(editingFinished()),
            this, SLOT(reloadModel()));

    // Request a read from this class...
    connect(this, SIGNAL(sendQueryRequest(QStringList)),
            &reader, SLOT(read(QStringList)));

    // ...but hanlde the request in the ItemModel
    connect(&reader, SIGNAL(readCompleted(QVector<QtItemWrapper*>)),
            m_itemModel, SLOT(resetWith(QVector<QtItemWrapper*>)));

    // Content notes updating
    connect(m_itemView, SIGNAL(clicked(QModelIndex)),
            this, SLOT(updateNotes(QModelIndex)));
}

//------------------------------------------------------------------------------
// Add a new tag to the line edit when combo box selection is made, and
// reload the items displayed accordingly.
//------------------------------------------------------------------------------
void RecallView::updateItemSet(const QString& tag) {
    if (m_tagsEdit->addTag(tag)) {
        reloadModel();
    }
}

//------------------------------------------------------------------------------
// Update the notes displayed with those of the selected item
//------------------------------------------------------------------------------
void RecallView::updateNotes(const QModelIndex& index) {
    if (index.isValid()) {
        const QtItemWrapper* item = m_itemModel->itemAt(index);
        if (item) {
            m_contentEdit->setHtml(item->content);
        }
    }
}

//------------------------------------------------------------------------------
void RecallView::reloadModel() {
    emit sendQueryRequest(m_tagsEdit->text().split(TagLineEdit::TagSeparator,
                                                   QString::SkipEmptyParts));
}
