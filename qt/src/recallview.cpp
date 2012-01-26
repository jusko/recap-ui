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
#include <QPlainTextEdit>
#include <QSplitter>
#include <QVariant>
#include <QDockWidget>
#include <QCommonStyle>
#include <QList>
#include <QToolBar>
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Ctor
//------------------------------------------------------------------------------
RecallView::RecallView(const QtSerializerWrapper& reader,
                       QWidget *parent)
    : QMainWindow(parent),
      m_itemModel(new ItemModel),
      m_itemView(0),
      m_tagsEdit(0),
      m_tagsBox(0),
      m_contentEdit(0),
      m_toolbar(0),
      m_tags(reader.tags()) {

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
void RecallView::initGui(const QtSerializerWrapper& reader) {
      setWindowTitle(tr("Recap - Recall Mode"));
      setGeometry(0, 0, 800, 600);

      // Toolbar
      QToolBar* m_toolbar = new QToolBar;
      m_toolbar->setMovable(true);
      m_toolbar->addAction(
          QCommonStyle().standardIcon(QCommonStyle::SP_TrashIcon),
          "Trash the selected item.",
          this,
          SLOT(trashItem())
      );
      addToolBar(Qt::LeftToolBarArea, m_toolbar);

      // Left Dock
      QDockWidget* tagListControl = new QDockWidget;
      tagListControl->setFeatures(QDockWidget::DockWidgetMovable);
      addDockWidget(Qt::LeftDockWidgetArea, tagListControl);
      QWidget* dockChildWidget = new QWidget;
      QGridLayout* l_layout = new QGridLayout(dockChildWidget);
      tagListControl->setWidget(dockChildWidget);

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
      QWidget* centralWidget = new QWidget;
      setCentralWidget(centralWidget);
      QGridLayout* r_layout = new QGridLayout(centralWidget);
      QLabel* notesLabel = new QLabel(tr("&Notes"));
      r_layout->addWidget(notesLabel, 0, 0);
      m_contentEdit = new QPlainTextEdit;
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

    connect(m_itemModel, SIGNAL(sendTrashRequest(QtItemWrapper)),
            &reader, SLOT(trash(QtItemWrapper)));

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
            m_contentEdit->setPlainText(item->content);
        }
    }
}

//------------------------------------------------------------------------------
void RecallView::reloadModel() {
    QStringList tags = m_tagsEdit->text().split(TagLineEdit::TagSeparator,
                                                QString::SkipEmptyParts);

    // Show all items if no tags are entered
    emit sendQueryRequest(tags.isEmpty() ? m_tags : tags);
}

//------------------------------------------------------------------------------
void RecallView::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Escape) {
        close();
    }
}

//------------------------------------------------------------------------------
void RecallView::trashItem() {
    m_itemModel->trashItem(m_itemView->currentIndex());
}
