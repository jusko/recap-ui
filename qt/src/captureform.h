#ifndef CAPTUREFORM_H
#define CAPTUREFORM_H
//------------------------------------------------------------------------------
#include <QDialog>
//------------------------------------------------------------------------------
class QtSerializerWrapper;
class QtItemWrapper;
class QLineEdit;
class QComboBox;
class QTextEdit;
class TagLineEdit;
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Form used to tag and capture notes.
//------------------------------------------------------------------------------
class CaptureForm : public QDialog {
    Q_OBJECT

    public:
        //----------------------------------------------------------------------
        // Creates a new capture form.
        // @param writer
        //		  The serializer to write the notes if the form is
        //
        // @post  The form is ready to be displayed with a call to show().
        //----------------------------------------------------------------------
        CaptureForm(const QtSerializerWrapper& writer,
                    QWidget *parent = 0);

        //----------------------------------------------------------------------
        // Displays the capture form with a new or existing item.
        // @param item
        //		  The item whose data is to be displayed via the form.
        //		  A null parameter implies that data for a new item is to be
        //		  created.
        // @pre   The item points either to null or a valid item address.
        // @post  Any changes or new data added via the form is saved if the
        //		  form is accepted, or abandonded otherwise.
        //----------------------------------------------------------------------
        void show(QtItemWrapper *item = 0);

        ~CaptureForm();

    signals:
        //----------------------------------------------------------------------
        // Emitted when the form is accepted in order to save the item.
        //----------------------------------------------------------------------
        void requestWrite(const QtItemWrapper&);

    private slots:
        void setItemTitle(const QString&);
        void updateTags(const QString&);
        void updateContentNotes();
        void addTag(const QString&);
        void acceptForm(bool);
        void rejectForm(bool);

    private:
        QtItemWrapper* m_item;
        QLineEdit*     m_titleEdit;
        TagLineEdit*   m_tagsEdit;
        QComboBox*     m_tagsBox;
        QTextEdit*     m_contentEdit;
        QPushButton*   m_okButton;
        QPushButton*   m_cancelButton;

        void initGui(const QtSerializerWrapper&);
        void setConnections(const QtSerializerWrapper&);
        void setItem(QtItemWrapper *);
        void resetForm();
};
#endif // CAPTUREFORM_H
