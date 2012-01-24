#include "itemmodel.h"
#include "qtserializerwrapper.h"
#include "taglineedit.h"
#include <string>
#include <vector>

//------------------------------------------------------------------------------
// Ctor:
//------------------------------------------------------------------------------
ItemModel::ItemModel(QObject *parent) :
    QAbstractListModel(parent) {
}

//------------------------------------------------------------------------------
ItemModel::~ItemModel() {
}

//------------------------------------------------------------------------------
#include <QDebug>
void ItemModel::setModel(const QVector<QtItemWrapper*>& items) {
    if (!m_items.empty()) {
        beginRemoveRows(QModelIndex(), 0, m_items.size() - 1);
        m_items.clear();
        endRemoveRows();
    }
    beginInsertRows(QModelIndex(), 0, 0);
    m_items = items;
    endInsertRows();
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
                return m_items[row]->tags.join(TagLineEdit::TagSeparator);
            }
        }
    }
    return QVariant();
}

//------------------------------------------------------------------------------
QVariant ItemModel::headerData(int section, Qt::Orientation orientation, int role) const {

    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        return section == 0 ? QString(tr("Title")) : QString(tr("Tags"));
    }
    return QVariant();
}

//------------------------------------------------------------------------------
Qt::ItemFlags ItemModel::flags(const QModelIndex &index) const {

    return index.isValid() ? QAbstractItemModel::flags(index) : Qt::NoItemFlags;
}

//------------------------------------------------------------------------------
const QtItemWrapper* ItemModel::itemAt(const QModelIndex& index) const {
    if (index.isValid() && index.row() < m_items.size()) {
        return m_items[index.row()];
    }
    return 0;
}
