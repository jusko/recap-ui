#ifndef CAPTUREFORM_H
#define CAPTUREFORM_H

#include <QDialog>
struct Item;
class QLineEdit;
class QComboBox;
class QTextEdit;
class TagLineEdit;
class TagShortcutDialog;

//------------------------------------------------------------------------------
// Form used to feed capture and tag information notes.
//------------------------------------------------------------------------------
class CaptureForm : public QDialog
{
    Q_OBJECT

    public:
        CaptureForm(Item* item,
                    const QStringList& taglist,
                    QWidget *parent = 0);

        ~CaptureForm();

        void clear();
    
    private:
        Item*              m_item;
        QLineEdit*         m_titleEdit;
        TagLineEdit*       m_tagsEdit;
        QComboBox*         m_tagsBox;
        QPushButton*       m_tagShortcutButton;
        QTextEdit*         m_contentEdit;
        QPushButton*       m_okButton;
        QPushButton* 	   m_cancelButton;
        TagShortcutDialog* m_tagShortcutDialog;

        void initGui(const QStringList& tags);
        void setConnections();

    private slots:
        void on_tagsBox_activated(const QString&);
        void on_tagsShortcutButton_clicked(bool);
        void on_okButton_clicked(bool);
        void on_cancelButton_clicked(bool);
};

#endif // CAPTUREFORM_H
