//------------------------------------------------------------------------------
#include "itemsortfilterproxymodel.h"
//------------------------------------------------------------------------------
#include <QDateTime>
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Ctor
//------------------------------------------------------------------------------
ItemSortFilterProxyModel::ItemSortFilterProxyModel(QObject *parent) :
    QSortFilterProxyModel(parent) {
}

//------------------------------------------------------------------------------
bool ItemSortFilterProxyModel::lessThan(const QModelIndex &left,
                                        const QModelIndex &right) const {

    QVariant leftData  = sourceModel()->data(left);
    QVariant rightData = sourceModel()->data(right);

    if (leftData.type() == QVariant::DateTime) {
        return leftData.toDateTime() < rightData.toDateTime();
    }
    else if (left.column() == 0 || left.column() == 1) {
        // This should improve efficiency somewhat, but might cause
        // a few incorrectly sorted cornercases.
        QRegExp pattern("(\\w+)\\s?");

        QString leftString  = leftData.toString();
        QString rightString = rightData.toString();

        if (pattern.indexIn(leftString) != -1 ) {
            leftString = pattern.cap(1);
        }
        if (pattern.indexIn(rightString) != -1 ) {
            rightString = pattern.cap(1);
        }
        return leftString.localeAwareCompare(rightString) < 0;
    }
}

//------------------------------------------------------------------------------
// Rows are filtered using the set filterRegExp matched against item
// titles and tags.
//------------------------------------------------------------------------------
bool ItemSortFilterProxyModel::filterAcceptsRow(int sourceRow,
                                                const QModelIndex& parent) const {

    QModelIndex index0 = sourceModel()->index(sourceRow, 0, parent);
    QModelIndex index1 = sourceModel()->index(sourceRow, 1, parent);

    return (sourceModel()->data(index0).toString().contains(filterRegExp()) ||
            sourceModel()->data(index1).toString().contains(filterRegExp()));
}
