#ifndef ITEMMODEL_H
#define ITEMMODEL_H

#include <QAbstractListModel>
#include <QVector>
class QtItemWrapper;
class QtSerializerWrapper;
class QModelIndex;

//------------------------------------------------------------------------------
// Model for items retrieved in recall mode.
//------------------------------------------------------------------------------
class ItemModel : public QAbstractListModel {
    Q_OBJECT

    public:
        ItemModel(QObject *parent = 0);

        ~ItemModel();

        const QtItemWrapper *itemAt(const QModelIndex& index) const;

        //---------------------------------------------------------------------
        // QAbstractListModel interface
        //---------------------------------------------------------------------
        int rowCount(const QModelIndex &parent) const;

        int columnCount(const QModelIndex &parent) const;

        QVariant headerData(int section, Qt::Orientation orientation, int role) const;

        QVariant data(const QModelIndex &index, int role) const;

        Qt::ItemFlags flags(const QModelIndex &index) const;

    public slots:
        void setModel(const QVector<QtItemWrapper*>& items);

    private:
        QVector<QtItemWrapper*> m_items;
};

#endif // ITEMMODEL_H
