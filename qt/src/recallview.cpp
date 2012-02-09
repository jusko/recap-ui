//------------------------------------------------------------------------------
#include "recallview.h"
#include "qtserializerwrapper.h"
#include "itemmodel.h"
#include "recapapp.h"
#include "cryptomediator.h"
#include "recallpage.h"
//------------------------------------------------------------------------------
#include <QCommonStyle>
#include <QToolBar>
#include <QMessageBox>
#include <QAction>
#include <QTabWidget>
#include <QKeyEvent>
//------------------------------------------------------------------------------
#define NEW_TAB "&New tab"
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Ctor
//------------------------------------------------------------------------------
RecallView::RecallView(const QtSerializerWrapper& serializer,
                       QWidget *parent)
    : QMainWindow(parent),
      m_itemModel(new ItemModel(serializer, this)),
      m_toolbar(0),
      m_tags(serializer.tags()),
      m_trashAction(0),
      m_saveAction(0),
      m_addEncryptionAction(0),
      m_removeEncryptionAction(0) {

    initGui(serializer);

    // Model notifications
    connect(m_itemModel, SIGNAL(errorNotification(QString)),
            this, SLOT(notifyError(QString)));

    connect(m_itemModel, SIGNAL(itemEncoded(const QtItemWrapper*)),
            this, SLOT(updateOnItemChange(const QtItemWrapper*)));

    setActions();
}

//------------------------------------------------------------------------------
void RecallView::initGui(const QtSerializerWrapper& serializer) {
      setWindowTitle(tr("Recap - Recall Mode"));
      setGeometry(0, 0, 800, 600);

      setToolbar();

      m_tabWidget = new QTabWidget;
      setCentralWidget(m_tabWidget);

      addTab();

      // Restore settings
      QSettings conf(RecapApp::ConfFile, QSettings::NativeFormat);
      conf.beginGroup("RecallView");
      restoreGeometry(conf.value("geometry").toByteArray());
      restoreState(conf.value("state").toByteArray());
      conf.endGroup();
}

//------------------------------------------------------------------------------
void RecallView::setToolbar() {
      m_toolbar = new QToolBar;
      m_toolbar->setObjectName("toolbar");
      m_toolbar->setMovable(true);

      // Trash action
      m_trashAction = m_toolbar->addAction(
          QCommonStyle().standardIcon(QCommonStyle::SP_TrashIcon),
          tr("Trash the selected item."),
          this,
          SLOT(trashItem())
      );
      m_trashAction->setEnabled(false);

      // Save action
      m_saveAction = m_toolbar->addAction(
          QCommonStyle().standardIcon(QCommonStyle::SP_DialogSaveButton),
          tr("Save changes"),
          m_itemModel,
          SLOT(saveItems())
      );
      m_saveAction->setShortcut(Qt::CTRL + Qt::Key_S);
      m_saveAction->setEnabled(false);

      connect(m_itemModel,  SIGNAL(itemEdited(bool)),
              m_saveAction, SLOT(setEnabled(bool)));

      connect(m_itemModel,  SIGNAL(itemsSaved(bool)),
              m_saveAction, SLOT(setDisabled(bool)));

      // Add encryption
      m_addEncryptionAction = m_toolbar->addAction(
            QIcon(":/img/encrypt"),
            tr("Encrypt item notes."),
            this,
            SLOT(addEncryption())
      );
      m_addEncryptionAction->setVisible(false);
      m_addEncryptionAction->setEnabled(false);

      // Remove Encryption
      m_removeEncryptionAction = m_toolbar->addAction(
            QIcon(":/img/decrypt"),
            tr("Remove encryption on item notes."),
            this,
            SLOT(removeEncryption())
      );
      m_removeEncryptionAction->setVisible(false);
      m_removeEncryptionAction->setEnabled(false);

      addToolBar(Qt::LeftToolBarArea, m_toolbar);
}

//------------------------------------------------------------------------------
void RecallView::setActions() {
    m_newTabAction = new QAction(this);
    connect(m_newTabAction, SIGNAL(triggered()), this, SLOT(addTab()));
    m_newTabAction->setShortcut(Qt::CTRL + Qt::Key_T);

    m_closeTabAction = new QAction(this);
    m_closeTabAction->setShortcut(Qt::CTRL + Qt::Key_W);
    connect(m_closeTabAction, SIGNAL(triggered()), this, SLOT(removeTab()));

    m_tabWidget->addAction(m_newTabAction);
    m_tabWidget->addAction(m_closeTabAction);
}

//------------------------------------------------------------------------------
// Updates widget state based on that of item
// @post If the item is encrypted, the remove encryption option becomes
//		 available.
//	     If the item is unencrypted, the add encryption option becomes available.
//		 The trash action is enabled.
//		 The current tab title is set with that of the item.
//		 If the item is NULL, disable the trash, add encryption and remove
//		 encryption actions; and clear the tab title and current tab.
//------------------------------------------------------------------------------
void RecallView::updateOnItemChange(const QtItemWrapper* item) {
    if (item) {
        m_trashAction->setEnabled(true);
        m_removeEncryptionAction->setEnabled(item->encrypted);
        m_addEncryptionAction->setEnabled(!item->encrypted);

        // TODO: Implement algorithm to insert '&' into title
        m_tabWidget->setTabText(m_tabWidget->currentIndex(), item->title);
    }
    else {
        m_trashAction->setEnabled(false);
        m_removeEncryptionAction->setEnabled(false);
        m_addEncryptionAction->setEnabled(false);
        m_tabWidget->setTabText(m_tabWidget->currentIndex(), NEW_TAB);
        dynamic_cast<RecallPage*>(m_tabWidget->currentWidget())->clear();
    }
}

//------------------------------------------------------------------------------
void RecallView::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Escape) {
        if (m_itemModel->hasEdits() &&
            QMessageBox::warning(
                this,  "Recap",
                QObject::tr("Changes have made and will be lost if you choose to "
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
        dynamic_cast<RecallPage*>(m_tabWidget->currentWidget())->currentItem()
    );
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
void RecallView::showEvent(QShowEvent *) {
    initCrypto();
}

//------------------------------------------------------------------------------
// Initialise the cryptographic engine (which may prompt the user for several
// choices on the first run) and setup crypto features accordingly.
//------------------------------------------------------------------------------
void RecallView::initCrypto() {
    CryptoMediator* cm = new CryptoMediator(this);
    setEncryption(cm->gpgEnabled());
}

//----------------------------------------------------------geometry--------------------
void RecallView::setEncryption(bool enabled) {
    m_addEncryptionAction->setVisible(enabled);
    m_removeEncryptionAction->setVisible(enabled);
}

//------------------------------------------------------------------------------
// @pre  An unencrypted item is selected
// @post A request to encrypt the item's notes is sent
//------------------------------------------------------------------------------
void RecallView::addEncryption() {
    m_itemModel->encryptItem(
        dynamic_cast<RecallPage*>(m_tabWidget->currentWidget())->currentItem()
    );
}

//------------------------------------------------------------------------------
// @pre  An encrypted item is selected
// @post A request to decrypt the item's notes is sent
//------------------------------------------------------------------------------
void RecallView::removeEncryption() {
    m_itemModel->decryptItem(
        dynamic_cast<RecallPage*>(m_tabWidget->currentWidget())->currentItem()
    );
}

//------------------------------------------------------------------------------
void RecallView::notifyError(const QString& message) {
    QMessageBox::critical(this, "Recap", message);
}

//------------------------------------------------------------------------------
void RecallView::addTab() {
      RecallPage* page = new RecallPage(m_itemModel, m_tags);

      connect(page, SIGNAL(currentItemChanged(const QtItemWrapper*)),
              this, SLOT(updateOnItemChange(const QtItemWrapper*)));

      m_tabWidget->setCurrentIndex(m_tabWidget->addTab(page, NEW_TAB));
}

//------------------------------------------------------------------------------
void RecallView::removeTab() {
      if (m_tabWidget->count() > 1) {
          RecallPage* current =
                dynamic_cast<RecallPage*>(m_tabWidget->currentWidget());

          disconnect(current, SIGNAL(currentItemChanged(const QtItemWrapper*)),
                     this, SLOT(updateOnItemChange(const QtItemWrapper*)));

          m_tabWidget->removeTab(m_tabWidget->currentIndex());
      }
}
