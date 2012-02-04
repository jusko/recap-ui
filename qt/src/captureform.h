#ifndef CAPTUREFORM_H
#define CAPTUREFORM_H
//------------------------------------------------------------------------------
#include <QDialog>
//------------------------------------------------------------------------------
class QtSerializerWrapper;
class QtItemWrapper;
class QLineEdit;
class QComboBox;
class QPlainTextEdit;
class TagLineEdit;
class QKeyEvent;
class QCheckBox;
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

        ~CaptureForm();

    public slots:
        //----------------------------------------------------------------------
        // Enables or disables encryption features.
        //----------------------------------------------------------------------
        void setEncryption(bool enable);

    signals:
        //----------------------------------------------------------------------
        // Emitted when the form is accepted in order to save the item.
        //----------------------------------------------------------------------
        void requestWrite(const QtItemWrapper&);

    private slots:
        void setItemTitle(const QString&);
        void setTags();
        void updateContentNotes();
        void addTag(const QString&);
        void acceptForm(bool);
        void rejectForm(bool);

    private:
        QtItemWrapper*  m_item;
        QLineEdit*      m_titleEdit;
        TagLineEdit*    m_tagsEdit;
        QComboBox*      m_tagsBox;
        QPlainTextEdit* m_contentEdit;
        QPushButton*    m_okButton;
        QPushButton*    m_cancelButton;
        QCheckBox* 		m_encryptCheckBox;

        void keyPressEvent(QKeyEvent *);
        void showEvent(QShowEvent *);
        void initGui(const QtSerializerWrapper&);
        void setConnections(const QtSerializerWrapper&);
        void setItem(QtItemWrapper *);
        bool validateForm() const;
        void resetForm();
        void initCrypto();
};
#endif // CAPTUREFORM_H
