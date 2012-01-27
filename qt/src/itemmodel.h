#ifndef ITEMMODEL_H
#define ITEMMODEL_H
//------------------------------------------------------------------------------
#include <QAbstractListModel>
#include <QVector>
#include <QMap>
//------------------------------------------------------------------------------
class QtItemWrapper;
class QtSerializerWrapper;
class QModelIndex;
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Model to view a list of items (by title) and their tags.
//------------------------------------------------------------------------------
class ItemModel : public QAbstractListModel {
    Q_OBJECT

    public:
        //---------------------------------------------------------------------
        // Return  the item associated with the given index.
        // @param  index
        //		   The index of the item to retreive.
        // @pre    The index must be valid.
        // @return The address of the item associated with the model index.
        //		   If no item could be found, NULL is returned.
        //---------------------------------------------------------------------
        QtItemWrapper* itemAt(const QModelIndex& index) const;

        //---------------------------------------------------------------------
        // Query items edited state.
        // @return true  If items have been edited and not updated.
        // @return false If no items have been edited since the last reset.
        //---------------------------------------------------------------------
        bool hasEdits() const;

        ItemModel(QObject *parent = 0);

        ~ItemModel();

    public slots:
        //---------------------------------------------------------------------
        // Resets the model with the received items
        // @pre   The received items are valid pointers.
        // @post  The previous items are deleted and the model is set with the
        //		  items received.
        //---------------------------------------------------------------------
        void resetWith(const QVector<QtItemWrapper*>& items);

        //---------------------------------------------------------------------
        // Remove the item from the model and forward request for trashing it.
        // @pre   The item exists in the model.
        // @post  The item is removed from the model and the sendTrashRequest()
        //		  signal is emitted.
        //---------------------------------------------------------------------
        void trashItem(const QModelIndex& index);

        //---------------------------------------------------------------------
        // Forwards a request to save all items changed since the last reset.
        // @post The edit history is reset and the sendUpdateRequest() signal
        //	     is emitted.
        //---------------------------------------------------------------------
        void saveItems();

        //---------------------------------------------------------------------
        // Updates an item's notes.
        // @param index
        //		  The index of the item to update.
        // @param notes
        //        The contents of the notes.
        // @pre   The item exists in the model.
        // @post  The item's notes are updated and it is flagged as having been
        //		  edited.
        //---------------------------------------------------------------------
        void updateNotes(const QModelIndex& index, const QString& notes);

    public:
        //---------------------------------------------------------------------
        // Reimplemented QAbstractListModel interface
        //---------------------------------------------------------------------
        virtual int rowCount(const QModelIndex &parent) const;

        virtual int columnCount(const QModelIndex &parent) const;

        virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;

        virtual QVariant data(const QModelIndex &index, int role) const;

        virtual bool setData(const QModelIndex &index, const QVariant &value, int role);

        virtual Qt::ItemFlags flags(const QModelIndex &index) const;

    signals:
        void sendTrashRequest(const QtItemWrapper&);
        void sendUpdateRequest(const QtItemWrapper&);

    private:
        void deleteItems();
        void deleteEditMap();

        QVector<QtItemWrapper*>   m_items;
        QMap<int, QtItemWrapper*> m_editMap;
};
#endif // ITEMMODEL_H
