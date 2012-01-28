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
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// The Ui seen in the application's "recall mode". Sole purpose is to recall
// notes saved in "capture mode" both rapidly and intuitively.
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

        ~RecallView();

    signals:
        //----------------------------------------------------------------------
        // Emitted directly to QtSerializerWrapper to request a read query.
        //----------------------------------------------------------------------
        void sendQueryRequest(const QStringList& tags);

        //----------------------------------------------------------------------
        // Emitted to ItemModel to notify when an item's notes have been edited.
        //----------------------------------------------------------------------
        void notesChanged(const QModelIndex&, const QString text);

    private slots:
        void updateItemSet(const QString& tag);
        void updateNotes(const QModelIndex&, const QModelIndex&);
        void trashItem();
        void reloadModel();
        void notesChanged();

    private:
        void initGui(const QtSerializerWrapper&);
        void setConnections(const QtSerializerWrapper&);
        void keyPressEvent(QKeyEvent*);
        void closeEvent(QCloseEvent*);

        ItemModel*   	 m_itemModel;
        QTreeView*   	 m_itemView;
        TagLineEdit* 	 m_tagsEdit;
        QComboBox*   	 m_tagsBox;
        QPlainTextEdit*	 m_contentEdit;
        QToolBar*		 m_toolbar;
        QStringList   	 m_tags;
        QDockWidget* 	 m_tagListDock;

        bool			 itemNotesChanged;
};
#endif // RECALLVIEW_H
