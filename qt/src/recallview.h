#ifndef RECALLVIEW_H
#define RECALLVIEW_H

#include <QDialog>
class ItemModel;
class QModelIndex;
class QTreeView;
class TagLineEdit;
class QComboBox;
class QPushButton;
class QTextEdit;
class QtSerializerWrapper;

//------------------------------------------------------------------------------
// Provides a folding tree view onto the items associated with a selection of
// tags.
//------------------------------------------------------------------------------
class RecallView : public QDialog
{
    Q_OBJECT

    public:
        RecallView(const QtSerializerWrapper &reader,
                   QWidget *parent = 0);

        ~RecallView();

    signals:

    private:
        ItemModel*         m_itemModel;
        QTreeView*         m_itemView;
        TagLineEdit*       m_tagsEdit;
        QComboBox*         m_tagsBox;
        QTextEdit*		   m_contentEdit;

    signals:
        void sendQueryRequest(const QStringList& tags);

    private slots:
        void on_tagsBox_activated(const QString& tag);
        void on_tagsBox_tagsUpdated(const QStringList& tags);
        void updateTagsBoxItems(const QStringList&);
        void on_itemView_clicked(const QModelIndex&);
        void reloadModel();

    private:
        void initGui(const QtSerializerWrapper&);
        void setConnections(const QtSerializerWrapper&);
};

#endif // RECALLVIEW_H
