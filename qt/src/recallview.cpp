//------------------------------------------------------------------------------
#include "recallview.h"
#include "taglineedit.h"
#include "qtserializerwrapper.h"
#include "itemmodel.h"
#include "recapapp.h"
#include "itemsortfilterproxymodel.h"
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
#include <QAction>
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Ctor
//------------------------------------------------------------------------------
RecallView::RecallView(const QtSerializerWrapper& serializer,
                       QWidget *parent)
    : QMainWindow(parent),
      m_itemModel(new ItemModel(serializer, this)),
      m_itemView(0),
      m_tagsEdit(0),
//      m_tagsBox(0),
      m_contentEdit(0),
      m_toolbar(0),
      m_tags(serializer.tags()),
      m_tagListDock(0),
      m_itemSFProxy(0),
      m_trashAction(0),
      m_saveAction(0),
      itemNotesChanged(false) {

    initGui(serializer);
    setConnections(serializer);
}

//------------------------------------------------------------------------------
// TODO: Refactor long method
void RecallView::initGui(const QtSerializerWrapper& serializer) {
      setWindowTitle(tr("Recap - Recall Mode"));
      setGeometry(0, 0, 800, 600);

      // Toolbar
      m_toolbar = new QToolBar;
      m_toolbar->setObjectName("toolbar");
      m_toolbar->setMovable(true);
      m_trashAction = m_toolbar->addAction(
          QCommonStyle().standardIcon(QCommonStyle::SP_TrashIcon),
          "Trash the selected item.",
          this,
          SLOT(trashItem())
      );
      m_trashAction->setDisabled(true);

      m_saveAction = m_toolbar->addAction(
          QCommonStyle().standardIcon(QCommonStyle::SP_DialogSaveButton),
          "Save changes",
          m_itemModel,
          SLOT(saveItems())
      );
      m_saveAction->setEnabled(false);
      connect(m_itemModel,  SIGNAL(itemEdited(bool)),
              m_saveAction, SLOT(setEnabled(bool)));

      connect(m_itemModel,  SIGNAL(itemsSaved(bool)),
              m_saveAction, SLOT(setDisabled(bool)));

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
      QLabel* tagEditLabel = new QLabel(tr("&Search"));
      l_layout->addWidget(tagEditLabel, 0, 0);
      m_tagsEdit = new TagLineEdit(serializer.tags(), this);
      l_layout->addWidget(m_tagsEdit, 0, 1);
      tagEditLabel->setBuddy(m_tagsEdit);

      // Tag box
//      m_tagsBox = new QComboBox;
//      m_tagsBox->addItems(serializer.tags());
//      l_layout->addWidget(m_tagsBox, 0, 3);

      m_itemView = new QTreeView;
      m_itemView->setRootIsDecorated(false);
      m_itemView->setAlternatingRowColors(true);
      m_itemView->setSortingEnabled(true);

      m_itemSFProxy = new ItemSortFilterProxyModel(this);
      m_itemSFProxy->setDynamicSortFilter(true);
      m_itemSFProxy->setSourceModel(m_itemModel);
      m_itemSFProxy->setFilterCaseSensitivity(Qt::CaseInsensitive);

      m_itemView->setModel(m_itemSFProxy);
      m_itemView->resizeColumnToContents(0);

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

    // Model updating
    connect(m_tagsEdit, SIGNAL(textChanged(QString)),
            m_itemSFProxy, SLOT(setFilterRegExp(QString)));

    // Content notes updating
    connect(m_itemView->selectionModel(), SIGNAL(currentChanged(QModelIndex,
                                                                QModelIndex)),
            this, SLOT(updateNotes(QModelIndex, QModelIndex)));
}

//------------------------------------------------------------------------------
// Notify if the previous item's notes were changed and update the notes
// displayed with those of the selected item.
//------------------------------------------------------------------------------
void RecallView::updateNotes(const QModelIndex& current,
                             const QModelIndex& previous) {

    if (current.isValid()) {
        const QtItemWrapper* item = m_itemModel->itemAt(
            m_itemSFProxy->mapToSource(current)
        );
        if (item) {

            // QPlainTextEdit doesn't have a signal that ignores
            // programmatic edits, so we have to do this.
            disconnect(m_contentEdit, SIGNAL(textChanged()),
                       this, SLOT(notesChanged()));

            m_contentEdit->setPlainText(item->content);

            connect(m_contentEdit, SIGNAL(textChanged()),
                    this, SLOT(notesChanged()));

            m_trashAction->setEnabled(true);
            return;
        }
    }
    m_trashAction->setDisabled(true);
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
    m_itemModel->trashItem(
        m_itemSFProxy->mapToSource(m_itemView->currentIndex())
    );
}

//------------------------------------------------------------------------------
void RecallView::notesChanged() {
    m_itemModel->updateNotes(
        m_itemSFProxy->mapToSource(
            m_itemView->selectionModel()->currentIndex()
        ),
        m_contentEdit->toPlainText()
    );
    m_saveAction->setEnabled(true);
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

//------------------------------------------------------------------------------
// Set the filter proxy with a regexp to match any title or tag with the
// text entered in the search bar.
//------------------------------------------------------------------------------
void RecallView::setRegexp(const QString& text) {

    m_itemSFProxy->setFilterRegExp(QRegExp(text, Qt::CaseInsensitive, QRegExp::FixedString));
}
