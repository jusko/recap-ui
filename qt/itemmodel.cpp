#include "itemmodel.h"
#include "qtserializerwrapper.h"
#include <string>
#include <vector>

//------------------------------------------------------------------------------
// Ctor:
//------------------------------------------------------------------------------
ItemModel::ItemModel(const QtSerializerWrapper& serializer, QObject *parent) :
    QAbstractListModel(parent) {

    connect(this, SIGNAL(getItems(QStringList)),
            &serializer, SLOT(readItems(QStringList)));

    connect(this, SIGNAL(writeItems(QVector<QtItemWrapper*>)),
            &serializer, SLOT(writeItems(QVector<QtItemWrapper*>)));

    connect(&serializer, SIGNAL(readComplete(QVector<QtItemWrapper*>)),
            this, SLOT(setModelData(QVector<QtItemWrapper*>)));
}

//------------------------------------------------------------------------------
ItemModel::~ItemModel() {
}

//------------------------------------------------------------------------------
void ItemModel::setModelData(const QVector<QtItemWrapper*>& items) {
    m_items = items;
}

//------------------------------------------------------------------------------
int ItemModel::rowCount(const QModelIndex &) const {
    return m_items.size();
}

//------------------------------------------------------------------------------
int ItemModel::columnCount(const QModelIndex &) const {
    return 2;
}

//------------------------------------------------------------------------------
QVariant ItemModel::data(const QModelIndex &index, int role) const {

    if (index.isValid() && role == Qt::DisplayRole) {

        int row = index.row();
        if (row < m_items.size()) {

            int col = index.column();
            if (col == 0) {
                return m_items[row]->title;
            }
            else if (col == 1) {
                return m_items[row]->tags.join(", ");
            }
        }
    }
    return QVariant();
}

//------------------------------------------------------------------------------
QVariant ItemModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal && orientation < 1) {
        return section == 0 ? tr("Title") : tr("Tags");
    }
    return QVariant();
}

//------------------------------------------------------------------------------
bool ItemModel::setData(const QModelIndex &index, const QVariant& /*value*/, int role) {

    if (index.isValid() && role == Qt::EditRole) {

        int row = index.row();
        if (row < m_items.size()) {

            int col = index.column();
            if (col == 0) {
                m_items[row]->title = *static_cast<QString*>(index.internalPointer());
            }
            else if (col == 1) {
                m_items[row]->tags = static_cast<QString*>(index.internalPointer())->split(",");
            }
        }
    }
    return false;
}

//------------------------------------------------------------------------------
Qt::ItemFlags ItemModel::flags(const QModelIndex &index) const {
    return index.isValid() ? Qt::ItemIsEditable | Qt::ItemIsSelectable
                           : Qt::NoItemFlags;
}
