#ifndef RECALLVIEW_H
#define RECALLVIEW_H
//------------------------------------------------------------------------------
#include <QMainWindow>
//------------------------------------------------------------------------------
class ItemModel;
class QModelIndex;
class QtItemWrapper;
class QtSerializerWrapper;
class QToolBar;
class QTabWidget;
class QAction;
class RecallPage;
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
        void updateOnItemModified(const QModelIndex&, const QString text);

    public slots:
        //----------------------------------------------------------------------
        // Initialises encryption features
        //----------------------------------------------------------------------
        void setEncryption(bool);

        //----------------------------------------------------------------------
        // Displays an error message with the string passed as a parameter
        //----------------------------------------------------------------------
        void notifyError(const QString&);

        //----------------------------------------------------------------------
        // Updates the toolbar button states based on that of the item.
        //----------------------------------------------------------------------
        void updateOnItemChange(const QtItemWrapper*);

    private slots:
        void addEncryption();
        void removeEncryption();
        void trashItem();
        void addTab();
        void removeTab();

    private:
        void initGui(const QtSerializerWrapper&);
        void setToolbar();
        void setActions();
        void keyPressEvent(QKeyEvent*);
        void showEvent(QShowEvent*);
        void closeEvent(QCloseEvent*);
        void initCrypto();

        ItemModel*  m_itemModel;
        QToolBar*	m_toolbar;
        QStringList m_tags;
        QAction*	m_trashAction;
        QAction*	m_saveAction;
        QAction*	m_addEncryptionAction;
        QAction*	m_removeEncryptionAction;
        QAction*	m_newTabAction;
        QAction*	m_closeTabAction;
        QTabWidget* m_tabWidget;
};
#endif // RECALLVIEW_H
