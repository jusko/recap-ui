#ifndef ITEMMODEL_H
#define ITEMMODEL_H
//------------------------------------------------------------------------------
#include <QAbstractListModel>
#include <QVector>
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

    public:
        //---------------------------------------------------------------------
        // Reimplemented QAbstractListModel interface
        //---------------------------------------------------------------------
        virtual int rowCount(const QModelIndex &parent) const;

        virtual int columnCount(const QModelIndex &parent) const;

        virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;

        virtual QVariant data(const QModelIndex &index, int role) const;

        virtual Qt::ItemFlags flags(const QModelIndex &index) const;

    signals:
        void sendTrashRequest(const QtItemWrapper&);

    private:
        void deleteItems();

        QVector<QtItemWrapper*> m_items;
};
#endif // ITEMMODEL_H
