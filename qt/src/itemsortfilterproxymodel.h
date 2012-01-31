#ifndef ITEMSORTFILTERPROXYMODEL_H
#define ITEMSORTFILTERPROXYMODEL_H
//------------------------------------------------------------------------------
#include <QSortFilterProxyModel>
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Proxy model to improve filtering of Items and also to make column sorting
// available in the view.
//------------------------------------------------------------------------------
class ItemSortFilterProxyModel : public QSortFilterProxyModel {
    Q_OBJECT

    public:
        explicit ItemSortFilterProxyModel(QObject* parent = 0);

    protected slots:
        virtual bool lessThan(const QModelIndex& left,
                              const QModelIndex& right) const;

        virtual bool filterAcceptsRow(int source_row,
                                      const QModelIndex& source_parent) const;
};

#endif // ITEMSORTFILTERPROXYMODEL_H
