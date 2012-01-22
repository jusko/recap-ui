#ifndef CAPTUREFORM_H
#define CAPTUREFORM_H

#include <QDialog>
#include "qtserializerwrapper.h"
class QLineEdit;
class QComboBox;
class QTextEdit;
class TagLineEdit;
class TagShortcutDialog;

//------------------------------------------------------------------------------
// Form used to feed capture and tag information notes.
//------------------------------------------------------------------------------
class CaptureForm : private QDialog {
    Q_OBJECT

    public:
        CaptureForm(const QtSerializerWrapper& writer,
                    QWidget *parent = 0);

        ~CaptureForm();

        void show(const QtItemWrapper& item);

        void clear();
    
    private:
        QtItemWrapper      m_item;
        QLineEdit*         m_titleEdit;
        TagLineEdit*       m_tagsEdit;
        QComboBox*         m_tagsBox;
        QPushButton*       m_tagShortcutButton;
        QTextEdit*         m_contentEdit;
        QPushButton*       m_okButton;
        QPushButton* 	   m_cancelButton;
        TagShortcutDialog* m_tagShortcutDialog;

        void initGui(const QtSerializerWrapper&);
        void setConnections(const QtSerializerWrapper&);
        void setItem(const QtItemWrapper&);

    signals:
        void requestWrite(const QtItemWrapper&);

    private slots:
        void on_titleEdit_textEdited(const QString&);

        void on_tagsEdit_textEdited(const QString&);

        void on_contentEdit_textChanged();

        void on_tagsBox_activated(const QString&);
        void updateTagsBoxItems(const QStringList&);

        void on_okButton_clicked(bool);
        void on_cancelButton_clicked(bool);

        // REMOVE
        void on_tagsShortcutButton_clicked(bool);
};

#endif // CAPTUREFORM_H
