//------------------------------------------------------------------------------
#include "recallview.h"
#include "taglineedit.h"
#include "qtserializerwrapper.h"
#include "itemmodel.h"
#include "recapapp.h"
//------------------------------------------------------------------------------
#include <QSettings>
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
#include <QMessageBox>
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Ctor
//------------------------------------------------------------------------------
RecallView::RecallView(const QtSerializerWrapper& serializer,
                       QWidget *parent)
    : QMainWindow(parent),
      m_itemModel(new ItemModel),
      m_itemView(0),
      m_tagsEdit(0),
      m_tagsBox(0),
      m_contentEdit(0),
      m_toolbar(0),
      m_tags(serializer.tags()),
      itemNotesChanged(false),
      m_tagListDock(0) {

    initGui(serializer);
    setConnections(serializer);
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
void RecallView::initGui(const QtSerializerWrapper& serializer) {
      setWindowTitle(tr("Recap - Recall Mode"));
      setGeometry(0, 0, 800, 600);

      // Toolbar
      m_toolbar = new QToolBar;
      m_toolbar->setObjectName("toolbar");
      m_toolbar->setMovable(true);
      m_toolbar->addAction(
          QCommonStyle().standardIcon(QCommonStyle::SP_TrashIcon),
          "Trash the selected item.",
          this,
          SLOT(trashItem())
      );
      m_toolbar->addAction(
          QCommonStyle().standardIcon(QCommonStyle::SP_DialogSaveButton),
          "Save changes",
          m_itemModel,
          SLOT(saveItems())
      );
      addToolBar(Qt::LeftToolBarArea, m_toolbar);

      // Left Dock
      m_tagListDock = new QDockWidget;
      m_tagListDock->setObjectName("tagListDock");
      m_tagListDock->setFeatures(QDockWidget::DockWidgetMovable);
      addDockWidget(Qt::LeftDockWidgetArea, m_tagListDock);
      QWidget* dockChildWidget = new QWidget;
      QGridLayout* l_layout = new QGridLayout(dockChildWidget);
      m_tagListDock->setWidget(dockChildWidget);

      // Tag edit
      QLabel* tagEditLabel = new QLabel(tr("T&ags"));
      l_layout->addWidget(tagEditLabel, 0, 0);
      m_tagsEdit = new TagLineEdit(serializer.tags(), this);
      l_layout->addWidget(m_tagsEdit, 0, 1);
      tagEditLabel->setBuddy(m_tagsEdit);

      // Tag box
      m_tagsBox = new QComboBox;
      m_tagsBox->addItems(serializer.tags());
      l_layout->addWidget(m_tagsBox, 0, 3);

      // Item list
      m_itemModel->resetWith(serializer.items());

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
      m_contentEdit->setTabStopWidth(8);
      r_layout->addWidget(m_contentEdit, 1, 0);
      notesLabel->setBuddy(m_contentEdit);

      // Restore settings
      QSettings conf(RecapApp::ConfFile, QSettings::NativeFormat);
      conf.beginGroup("RecallView");
      restoreGeometry(conf.value("geometry").toByteArray());
      restoreState(conf.value("state").toByteArray());
      conf.endGroup();
}

//------------------------------------------------------------------------------
void RecallView::setConnections(const QtSerializerWrapper& serializer) {

    // Tag updates
    connect(m_tagsBox, SIGNAL(activated(QString)),
            this, SLOT(updateItemSet(QString)));

    // Model updating
    connect(m_tagsEdit, SIGNAL(editingFinished()),
            this, SLOT(reloadModel()));

    connect(m_itemModel, SIGNAL(sendTrashRequest(QtItemWrapper)),
            &serializer, SLOT(trash(QtItemWrapper)));

    connect(m_itemModel, SIGNAL(sendUpdateRequest(QtItemWrapper)),
            &serializer, SLOT(write(QtItemWrapper)));

    // Request a read from this class...
    connect(this, SIGNAL(sendQueryRequest(QStringList)),
            &serializer, SLOT(read(QStringList)));

    // ...but hanlde the request in the ItemModel
    connect(&serializer, SIGNAL(readCompleted(QVector<QtItemWrapper*>)),
            m_itemModel, SLOT(resetWith(QVector<QtItemWrapper*>)));

    // Content notes updating
    connect(m_itemView->selectionModel(), SIGNAL(currentChanged(QModelIndex,
                                                                QModelIndex)),
            this, SLOT(updateNotes(QModelIndex, QModelIndex)));

    connect(this, SIGNAL(notesChanged(QModelIndex,QString)),
            m_itemModel, SLOT(updateNotes(QModelIndex,QString)));
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
// Notify if the previous item's notes were changed and update the notes
// displayed with those of the selected item.
//------------------------------------------------------------------------------
void RecallView::updateNotes(const QModelIndex& current,
                             const QModelIndex& previous) {

    if (itemNotesChanged) {
        emit notesChanged(previous, m_contentEdit->toPlainText());
        itemNotesChanged = false;
    }
    if (current.isValid()) {
        const QtItemWrapper* item = m_itemModel->itemAt(current);
        if (item) {

            // QPlainTextEdit doesn't have a signal that ignores
            // programmatic edits, so we have to do this.
            disconnect(m_contentEdit, SIGNAL(textChanged()),
                       this, SLOT(notesChanged()));

            m_contentEdit->setPlainText(item->content);

            connect(m_contentEdit, SIGNAL(textChanged()),
                    this, SLOT(notesChanged()));
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
        if (m_itemModel->hasEdits() &&
            QMessageBox::warning(
                this,  "Recap",
                tr("Changes have made and will be lost if you choose to "
                   "continue. Are you sure you want to exit?"),
                QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes) {

            return;
        }
        close();
    }
}

//------------------------------------------------------------------------------
void RecallView::trashItem() {
    m_itemModel->trashItem(m_itemView->currentIndex());
}

//------------------------------------------------------------------------------
void RecallView::notesChanged() {
    itemNotesChanged = true;
}

//------------------------------------------------------------------------------
// Save the Ui state when exiting so that it can be restored next time.
//------------------------------------------------------------------------------
void RecallView::closeEvent(QCloseEvent* event) {

    QSettings conf(RecapApp::ConfFile, QSettings::NativeFormat);
    conf.beginGroup("RecallView");
    conf.setValue("geometry", saveGeometry());
    conf.setValue("state", saveState());
    conf.endGroup();
    QMainWindow::closeEvent(event);
}

