//------------------------------------------------------------------------------
#include "recallpage.h"
#include "taglineedit.h"
#include "itemmodel.h"
#include "recapapp.h"
#include "itemsortfilterproxymodel.h"
#include "qtserializerwrapper.h"
#include "cryptomediator.h"
//------------------------------------------------------------------------------
#include <QSettings>
#include <QGridLayout>
#include <QLabel>
#include <QTreeView>
#include <QPlainTextEdit>
#include <QSplitter>
#include <QMessageBox>
//------------------------------------------------------------------------------
#define PAGE_SPLIT_GEOMETRY "pageSplitGeometry"
#define PAGE_SPLIT_STATE    "pageSplitState"

//------------------------------------------------------------------------------
// Ctor
// TODO: Refactor long method
//------------------------------------------------------------------------------
RecallPage::RecallPage(ItemModel* model, const QStringList& tags,
                      QWidget* parent, Qt::WindowFlags flags)

        : QWidget(parent, flags),
          m_searchBar(0),
          m_itemView(0),
          m_itemSFProxy(0),
          m_notesEdit(0),
          m_itemModel(model) {

    QHBoxLayout* parentLayout = new QHBoxLayout(this);
    m_splitter   = new QSplitter(Qt::Vertical, this);
    parentLayout->addWidget(m_splitter);

    QWidget* searchWidget = new QWidget(this);
    QGridLayout* layout = new QGridLayout(searchWidget);

    // Search Bar
    QLabel* searchBarLabel = new QLabel(tr("&Search"));
    m_searchBar = new TagLineEdit(tags, this);
    layout->addWidget(searchBarLabel, 0, 0);
    layout->addWidget(m_searchBar, 0, 1);
    searchBarLabel->setBuddy(m_searchBar);
    connect(m_searchBar, SIGNAL(textChanged(QString)),
            this, SLOT(setRegexpFilter(QString)));

    // Item view
    m_itemView = new QTreeView;
    m_itemView->setRootIsDecorated(false);
    m_itemView->setAlternatingRowColors(true);
    m_itemView->setSortingEnabled(true);

    m_itemSFProxy = new ItemSortFilterProxyModel(this);
    m_itemSFProxy->setDynamicSortFilter(true);
    m_itemSFProxy->setSourceModel(model);
    m_itemSFProxy->setFilterCaseSensitivity(Qt::CaseInsensitive);

    m_itemView->setModel(m_itemSFProxy);
    m_itemView->resizeColumnToContents(0);

    layout->addWidget(m_itemView, 1, 0, 1, 4);
    connect(m_itemView->selectionModel(), SIGNAL(currentChanged(QModelIndex,
                                                                QModelIndex)),
            this, SLOT(updateOnItemChange(QModelIndex,QModelIndex)));

    m_splitter->addWidget(searchWidget);

    // Notes
    QWidget* notesWidget = new QWidget(this);
    layout = new QGridLayout(notesWidget);

    QLabel* notesLabel = new QLabel(tr("&Notes"));
    m_notesEdit = new QPlainTextEdit;
    m_notesEdit->setTabStopWidth(40);
    m_notesEdit->setEnabled(false);
    layout->addWidget(notesLabel, 0, 0);
    layout->addWidget(m_notesEdit, 1, 0, 1, 4);
    notesLabel->setBuddy(m_notesEdit);
    connect(m_notesEdit, SIGNAL(textChanged()),
            this, SLOT(updateOnNotesChanged()));

    m_splitter->addWidget(notesWidget);

    // TODO: View columns
    QSettings conf(RecapApp::ConfFile, QSettings::NativeFormat);
    conf.beginGroup("RecallPage");
    m_splitter->restoreGeometry(conf.value(PAGE_SPLIT_GEOMETRY).toByteArray());
    m_splitter->restoreState(conf.value(PAGE_SPLIT_STATE).toByteArray());
    conf.endGroup();
}

//------------------------------------------------------------------------------
// Dtor
//------------------------------------------------------------------------------
RecallPage::~RecallPage() {
    QSettings conf(RecapApp::ConfFile, QSettings::NativeFormat);
    conf.beginGroup("RecallPage");
    conf.setValue(PAGE_SPLIT_GEOMETRY, m_splitter->saveGeometry());
    conf.setValue(PAGE_SPLIT_STATE, m_splitter->saveState());
    conf.endGroup();
}

//------------------------------------------------------------------------------
void RecallPage::updateOnItemChange(const QModelIndex& current,
                                    const QModelIndex&) {
    if (current.isValid()) {
        const QtItemWrapper* item = m_itemModel->itemAt(
            m_itemSFProxy->mapToSource(current)
        );
        if (item) {

            // QPlainTextEdit doesn't have a signal that ignores
            // programmatic edits, so we have to do this.
            disconnect(m_notesEdit, SIGNAL(textChanged()),
                       this, SLOT(updateOnNotesChanged()));

            m_notesEdit->setPlainText(decrypt(item));

            connect(m_notesEdit, SIGNAL(textChanged()),
                    this, SLOT(updateOnNotesChanged()));

            emit currentItemChanged(item);
            return;
        }
    }
    emit currentItemChanged(NULL);
}

//------------------------------------------------------------------------------
void RecallPage::updateOnNotesChanged() {
    m_itemModel->updateNotes(
        m_itemSFProxy->mapToSource(
            m_itemView->selectionModel()->currentIndex()
        ),
        m_notesEdit->toPlainText()
    );
}

//------------------------------------------------------------------------------
// Attempt to decrypt the item's content if it's encrypted. Password entry
// is taken care of by pinentry and GPG Agent.
//
// @pre  If the item is encrypted and GPG encryption features are enabled.
//	     If the item is unencrypted, there are no preconditions that bear
//		 mentioning.
//
// @post If the password entry and decryption is successful, return the plain
//		 text and ensure m_notesEdit is enabled.
//
//		 If password entry fails, or if the notes are encrypted but GPG features
//		 are disabled, returns a blank string, disables m_notesEdit and shows
//	     a lock image.
//
//		 If the item is unencrypted, return its contents and ensure m_notesEdit
//		 is enabled.
//------------------------------------------------------------------------------
QString RecallPage::decrypt(const QtItemWrapper* item) {
    setLockImage(false);
    if (!item->encrypted) {
        return item->content;
    }

    QString plaintext;
    CryptoMediator cm;

    if (!cm.decrypt(*item, plaintext)) {
        setLockImage(true);
        if (!cm.lastError().isEmpty()) {
            QMessageBox::critical(
                this, tr("Recap - Decryption Error"),
                cm.lastError()
            );
        }
    }
    return plaintext;
}

//------------------------------------------------------------------------------
void RecallPage::setLockImage(bool show) {
    if (show) {
        m_notesEdit->setStyleSheet(
            "QPlainTextEdit { "
                "background-image: url(:/img/lockdown); "
                "background-repeat: no-repeat;"
                "background-position: center;"
            "}"
        );
        m_notesEdit->setEnabled(false);
    }
    else {
        m_notesEdit->setStyleSheet("");
        m_notesEdit->setEnabled(true);
    }
}

//------------------------------------------------------------------------------
// Set the filter proxy with a regexp to match any title or tag with the
// text entered in the search bar.
//------------------------------------------------------------------------------
void RecallPage::setRegexpFilter(const QString& text) {
    m_itemSFProxy->setFilterRegExp(
        QRegExp(text, Qt::CaseInsensitive, QRegExp::FixedString)
    );
}

//------------------------------------------------------------------------------
QModelIndex RecallPage::currentItem() const {
    return m_itemSFProxy->mapToSource(
        m_itemView->currentIndex()
    );
}

//------------------------------------------------------------------------------
void RecallPage::clear() {
    m_notesEdit->clear();
    m_notesEdit->setEnabled(false);
}

//------------------------------------------------------------------------------
void RecallPage::showEvent(QShowEvent*) {
    m_searchBar->setFocus();
}
