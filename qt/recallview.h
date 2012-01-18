#ifndef RECALLVIEW_H
#define RECALLVIEW_H

#include <QMainWindow>
class ItemFinder;
class NotesEdit;

//------------------------------------------------------------------------------
// Provides a folding tree view onto the items associated with a selection of
// tags.
//------------------------------------------------------------------------------
class RecallView : public QMainWindow
{
    Q_OBJECT

    public:
        RecallView(const QStringList& tags, QWidget *parent = 0);

    signals:

    private:
        ItemFinder* m_itemFinder;
        NotesEdit*  m_notesEdit;
};

#endif // RECALLVIEW_H
