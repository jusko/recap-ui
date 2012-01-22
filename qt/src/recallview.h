#ifndef RECALLVIEW_H
#define RECALLVIEW_H

#include <QDialog>
class ItemModel;
class QListView;
class TagLineEdit;
class QComboBox;
class QPushButton;
class QTextEdit;
class TagShortcutDialog;

//------------------------------------------------------------------------------
// Provides a folding tree view onto the items associated with a selection of
// tags.
//------------------------------------------------------------------------------
class RecallView : public QDialog
{
    Q_OBJECT

    public:
        RecallView(const QStringList& tags,
                   ItemModel& model,
                   QWidget *parent = 0);
    signals:

    private:
        ItemModel*         m_itemModel;
        QListView*         m_itemView;
        TagLineEdit*       m_tagEdit;
        QComboBox*         m_tagsBox;
        QPushButton*       m_tagShortcutButton;
        TagShortcutDialog* m_tagShortcutDialog;
        QTextEdit*		   m_contentEdit;

    private slots:
        void on_tagsBox_activated(const QString& tag);
        void on_tagShortcutButton_clicked(bool);
};

#endif // RECALLVIEW_H
