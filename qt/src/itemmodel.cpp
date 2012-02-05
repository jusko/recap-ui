//------------------------------------------------------------------------------
#include "itemmodel.h"
#include "qtserializerwrapper.h"
#include "taglineedit.h"
#include "cryptomediator.h"
#include <string>
#include <vector>
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Ctor
//------------------------------------------------------------------------------
ItemModel::ItemModel(const QtSerializerWrapper& serializer, QObject *parent)
    : QAbstractListModel(parent) {

    resetWith(serializer.items());

    connect(this, SIGNAL(sendTrashRequest(QtItemWrapper)),
            &serializer, SLOT(trash(QtItemWrapper)));

    connect(this, SIGNAL(sendUpdateRequest(QtItemWrapper)),
            &serializer, SLOT(write(QtItemWrapper)));

    connect(&serializer, SIGNAL(readCompleted(QVector<QtItemWrapper*>)),
            this, SLOT(resetWith(QVector<QtItemWrapper*>)));
}

//------------------------------------------------------------------------------
// Dtor
//------------------------------------------------------------------------------
ItemModel::~ItemModel() {
    deleteItems();
}

//------------------------------------------------------------------------------
void ItemModel::deleteItems() {
    foreach (QtItemWrapper* i, m_items) {
        if (i && !m_editMap.contains(i->id)) {
            delete i;
            i = 0;
        }
    }
    m_items.clear();
}

//------------------------------------------------------------------------------
void ItemModel::deleteEditMap() {
    foreach (QtItemWrapper* i, m_editMap) {
        if (i && !m_items.contains(i)) {
            delete i;
            i = 0;
        }
    }
    m_editMap.clear();
}

//------------------------------------------------------------------------------
void ItemModel::resetWith(const QVector<QtItemWrapper*>& items) {

    // Keep items that have been edited
    foreach(QtItemWrapper* i, items) {
        if (m_editMap.contains(i->id)) {
            *i = *m_editMap[i->id];
        }
    }
    if (!m_items.empty()) {
        beginRemoveRows(QModelIndex(), 0, m_items.size() - 1);
        deleteItems();
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
    return 3;
}

//------------------------------------------------------------------------------
QVariant ItemModel::headerData(int section,
                               Qt::Orientation orientation, int role) const {

    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
            case 0:
                return tr("Title");
            case 1:
                return tr("Tags");
            case 2:
                return tr("Last Edit");
            default:
                return QVariant();
        }
    }
    return QVariant();
}

//------------------------------------------------------------------------------
QVariant ItemModel::data(const QModelIndex &index, int role) const {

    if (index.isValid()) {
        int row = index.row();
        int col = index.column();

        if (role == Qt::DisplayRole) {
            if (row < m_items.size()) {
                switch (col) {
                    case 0:
                        return m_items[row]->title;
                    case 1:
                        return m_items[row]->tags.join(
                            TagLineEdit::TagSeparator
                        );
                    case 2:
                        return m_items[row]->timestamp;
                    default:
                        return QVariant();
                }
            }
        }
        if (role == Qt::DecorationRole) {
            if (row < m_items.size() && col == 0 && m_items[row]->encrypted) {
                return QPixmap(":/img/item-encrypted");
            }
        }
    }
    return QVariant();
}

//------------------------------------------------------------------------------
// TODO: Retain original text initially on edit (esp. tags).
bool ItemModel::setData(const QModelIndex &index,
                        const QVariant &value, int role) {

    QString valueStr = value.toString();

    if (index.isValid() && role == Qt::EditRole && !valueStr.isEmpty()) {

        int row = index.row(), col = index.column();
        if (row < m_items.size()) {
            QtItemWrapper* item = m_items[row];

            if (col == 0) {
                m_items[row]->title = valueStr;
                m_editMap.insert(item->id, item);
                emit itemEdited(true);
                return true;
            }
            else if (col == 1) {
                QString tagstring  = valueStr;
                m_items[row]->tags = tagstring.split(TagLineEdit::TagSeparator,
                                                     QString::SkipEmptyParts);
                m_editMap.insert(item->id, item);
                emit itemEdited(true);
                return true;
            }
        }
    }
    return false;
}

//------------------------------------------------------------------------------
Qt::ItemFlags ItemModel::flags(const QModelIndex &index) const {

    return index.isValid() ? Qt::ItemIsEnabled    |
                             Qt::ItemIsSelectable |
                             Qt::ItemIsEditable   : Qt::NoItemFlags;
}

//------------------------------------------------------------------------------
QtItemWrapper* ItemModel::itemAt(const QModelIndex& index) const {
    if (index.isValid() && index.row() < m_items.size()) {
        return m_items[index.row()];
    }
    return 0;
}

//------------------------------------------------------------------------------
void ItemModel::trashItem(const QModelIndex &index) {
    QtItemWrapper* item = itemAt(index);
    if (item) {
        int row = index.row();
        beginRemoveRows(QModelIndex(), row, row);
        m_items.remove(row);
        emit sendTrashRequest(*item);
        endRemoveRows();
    }
}

//------------------------------------------------------------------------------
// TODO: Improve performance if it becomes an issue (encrypted items get
//	     rencrypted on each call---but it seems performant enough for now).
//------------------------------------------------------------------------------
void ItemModel::updateNotes(const QModelIndex &index, const QString &notes) {
    QtItemWrapper* item = itemAt(index);
    if (item) {
        if (item->encrypted) {
            CryptoMediator cm;
            QString original = item->content;

            item->content = notes;
            if (cm.encrypt(*item)) {
                m_editMap.insert(item->id, item);
                emit itemEdited(true);
            }
            else {
                item->content = original;
                emit errorNotification(cm.lastError());
            }
        }
        else {
            item->content = notes;
            m_editMap.insert(item->id, item);
            emit itemEdited(true);
        }
    }
}

//------------------------------------------------------------------------------
bool ItemModel::hasEdits() const {
    return !m_editMap.empty();
}

//------------------------------------------------------------------------------
void ItemModel::saveItems() {
    if (hasEdits()) {
        foreach(QtItemWrapper* i, m_editMap) {
            emit sendUpdateRequest(*i);
        }
        deleteEditMap();
        emit itemsSaved(true);
    }
}

//------------------------------------------------------------------------------
void ItemModel::encryptItem(const QModelIndex &index) {
    QtItemWrapper* item = itemAt(index);
    Q_ASSERT(item);

    CryptoMediator cm;
    if (cm.encrypt(*item)) {
        m_editMap.insert(item->id, item);
        emit itemEdited(true);
        emit itemEncoded(item);
    }
    else {
        emit errorNotification(cm.lastError());
    }
}

//------------------------------------------------------------------------------
void ItemModel::decryptItem(const QModelIndex &index) {
    QtItemWrapper* item = itemAt(index);
    Q_ASSERT(item);

    CryptoMediator cm;
    if (cm.decrypt(*item)) {
        m_editMap.insert(item->id, item);
        emit itemEdited(true);
        emit itemEncoded(item);
    }
    else {
        emit errorNotification(cm.lastError());
    }
}
