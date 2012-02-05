#ifndef RECALLVIEW_H
#define RECALLVIEW_H
//------------------------------------------------------------------------------
#include <QMainWindow>
//------------------------------------------------------------------------------
class QToolBar;
class ItemModel;
class QModelIndex;
class QTreeView;
class TagLineEdit;
class QComboBox;
class QPushButton;
class QPlainTextEdit;
class QtItemWrapper;
class QtSerializerWrapper;
class QDockWidget;
class QToolBar;
class ItemSortFilterProxyModel;
class QAction;
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// The Ui seen in the application's "recall mode". Sole purpose is to recall
// and edit notes saved in "capture mode" both _rapidly_ and _intuitively_.
//------------------------------------------------------------------------------
class RecallView : public QMainWindow {
    Q_OBJECT

    public:
        //----------------------------------------------------------------------
        // Initialises the view and connects signals and slots.
        // @param reader
        //		  The wrapped serializer required to perform all item queries.
        // @post  The Ui is ready to be displayed via a call to show().
        //----------------------------------------------------------------------
        RecallView(const QtSerializerWrapper &serializer,
                   QWidget *parent = 0);

    signals:
        //----------------------------------------------------------------------
        // Emitted directly to QtSerializerWrapper to request a read query.
        //----------------------------------------------------------------------
        void sendQueryRequest(const QStringList& tags);

        //----------------------------------------------------------------------
        // Emitted to ItemModel to notify when an item's notes are edited.
        //----------------------------------------------------------------------
        void notesChanged(const QModelIndex&, const QString text);

    public slots:
        void setEncryption(bool);
        void notifyError(const QString&);

    private slots:
        void updateNotes(const QModelIndex&, const QModelIndex&);
        void update(const QtItemWrapper*);
        void setRegexp(const QString&);
        void addEncryption();
        void removeEncryption();
        void trashItem();
        void notesChanged();

    private:
        void initGui(const QtSerializerWrapper&);
        void setConnections(const QtSerializerWrapper&);
        void keyPressEvent(QKeyEvent*);
        void showEvent(QShowEvent*);
        void closeEvent(QCloseEvent*);
        void setLockImage(bool show);
        void initCrypto();
        void setToolbar();

        QString decrypt(const QtItemWrapper *);
        QtItemWrapper* currentItem(const QModelIndex &item) const;

        ItemModel*   	 		  m_itemModel;
        QTreeView*   	 		  m_itemView;
        TagLineEdit* 	 		  m_tagsEdit;
//        QComboBox*   	 		  m_tagsBox;
        QPlainTextEdit*	 		  m_contentEdit;
        QToolBar*		 		  m_toolbar;
        QStringList   	 		  m_tags;
        QDockWidget* 	 		  m_tagListDock;
        ItemSortFilterProxyModel* m_itemSFProxy;
        QAction*				  m_trashAction;
        QAction*				  m_saveAction;
        QAction*				  m_addEncryptionAction;
        QAction*				  m_removeEncryptionAction;

        bool			 itemNotesChanged;
};
#endif // RECALLVIEW_H
