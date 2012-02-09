#ifndef RECALLPAGE_H
#define RECALLPAGE_H
//------------------------------------------------------------------------------
#include <QWidget>
#include <QModelIndex>
//------------------------------------------------------------------------------
class TagLineEdit;
class QTreeView;
class ItemSortFilterProxyModel;
class QPlainTextEdit;
class QSplitter;
class ItemModel;
class QtItemWrapper;
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// A page consists of a search bar, a list of items and the notes of the
// currently displayed item. Each tab in RecallView displays a page as a view
// onto the same model of items.
//------------------------------------------------------------------------------
class RecallPage : public QWidget {
    Q_OBJECT

    public:
        //---------------------------------------------------------------------
        // Initialise a new page.
        //
        // @param model
        //	      The model of items to filter, view and modify.
        // @param tags
        //		  A current list of all the item tags.
        // @post  A new page is created as a view onto the model.
        //---------------------------------------------------------------------
        RecallPage(ItemModel* model, const QStringList& tags,
                   QWidget* parent = 0, Qt::WindowFlags flags = 0);

        //---------------------------------------------------------------------
        // @post The splitter state and geometry in the current view is stored.
        //		 This means that the settings configured in the last tab widget
        //		 will be restored.
        // TODO: Improve this behaviour when restoring previous session's tabs.
        //---------------------------------------------------------------------
        ~RecallPage();

        //---------------------------------------------------------------------
        // Accesses the item currently selected in the view.
        // @return The QModel index of the item. isValid() will return false
        //		   on the index if no item is currently selected.
        //---------------------------------------------------------------------
        QModelIndex currentItem() const;

        //---------------------------------------------------------------------
        // Clears and disables the notes display.
        //---------------------------------------------------------------------
        void clear();

    signals:
        //---------------------------------------------------------------------
        // Notifies with the currently selected item whenever it changes.
        // NOTE: A NULL value is emitted if the selection is cleared (e.g.,
        //		 after an item is trashed).
        //---------------------------------------------------------------------
        void currentItemChanged(const QtItemWrapper*);

    private slots:
        void updateOnItemChange(const QModelIndex&, const QModelIndex&);
        void setRegexpFilter(const QString&);
        void updateOnNotesChanged();

    private:
        void setLockImage(bool);
        void showEvent(QShowEvent*);
        QString decrypt(const QtItemWrapper*);

        TagLineEdit* 			  m_searchBar;
        QTreeView* 				  m_itemView;
        ItemSortFilterProxyModel* m_itemSFProxy;
        QPlainTextEdit* 		  m_notesEdit;
        QSplitter*				  m_splitter;
        ItemModel*				  m_itemModel;
};
#endif // RECALLPAGE_H
