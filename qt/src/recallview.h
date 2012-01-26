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
        RecallView(const QtSerializerWrapper &reader,
                   QWidget *parent = 0);

        ~RecallView();

    signals:
        void sendQueryRequest(const QStringList& tags);

    private slots:
        void updateItemSet(const QString& tag);
        void updateNotes(const QModelIndex&);
        void trashItem();
        void reloadModel();

    private:
        void initGui(const QtSerializerWrapper&);
        void setConnections(const QtSerializerWrapper&);
        void keyPressEvent(QKeyEvent *);

        ItemModel*   	 m_itemModel;
        QTreeView*   	 m_itemView;
        TagLineEdit* 	 m_tagsEdit;
        QComboBox*   	 m_tagsBox;
        QPlainTextEdit*	 m_contentEdit;
        QToolBar*		 m_toolbar;
        QStringList   	 m_tags;
};
#endif // RECALLVIEW_H
