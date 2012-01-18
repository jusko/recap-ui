#ifndef TAGSHORTCUTDIALOG_H
#define TAGSHORTCUTDIALOG_H

#include <QDialog>
class QStringListModel;
class QListView;

//------------------------------------------------------------------------------
// Dialog used to configure shortcuts for commonly used tags
//------------------------------------------------------------------------------
class TagShortcutDialog : public QDialog
{
    Q_OBJECT

    public:
        TagShortcutDialog(const QStringList& tags, QWidget *parent = 0);

    private:
        //QStringListModel* m_model;
        //QListView*		  m_view;
    
};

#endif // TAGSHORTCUTDIALOG_H
