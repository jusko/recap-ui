#ifndef ITEMMODEL_H
#define ITEMMODEL_H

#include <QAbstractListModel>
#include <QVector>
class QtItemWrapper;
class QtSerializerWrapper;

//------------------------------------------------------------------------------
// Model for items retrieved in recall mode.
//------------------------------------------------------------------------------
class ItemModel : public QAbstractListModel {
    Q_OBJECT

    public:
        ItemModel(const QtSerializerWrapper&, QObject *parent = 0);

        ~ItemModel();

        //---------------------------------------------------------------------
        // QAbstractListModel interface
        //---------------------------------------------------------------------
        int rowCount(const QModelIndex &parent) const;

        int columnCount(const QModelIndex &parent) const;

        QVariant headerData(int section, Qt::Orientation orientation, int role) const;

        QVariant data(const QModelIndex &index, int role) const;

        bool setData(const QModelIndex &index, const QVariant &value, int role);

        Qt::ItemFlags flags(const QModelIndex &index) const;

    signals:
        void getItems(const QStringList& tags);
        void writeItems(const QVector<QtItemWrapper*>& items);

    public slots:
        void setModelData(const QVector<QtItemWrapper*>& items);

    private:
        QVector<QtItemWrapper*> m_items;
};

#endif // ITEMMODEL_H
