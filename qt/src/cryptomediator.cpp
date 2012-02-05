//------------------------------------------------------------------------------
#include "cryptomediator.h"
#include "qtserializerwrapper.h"
#include "recapapp.h"
#include "gpgme_wrapper.h"
#include "captureform.h"
#include "recallview.h"
//------------------------------------------------------------------------------
#include <QMessageBox>
#include <QCheckBox>
#include <QPushButton>
#include <QStyle>
#include <QHBoxLayout>
#include <QStringListModel>
#include <QListView>
#include <QCommonStyle>
#include <stdexcept>
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Config file strings
//------------------------------------------------------------------------------
#define CRYPT_GROUP "CryptoMediator"
#define GPG_ENABLED	"sm_gpgEnabled"
#define GPG_KEY		"sm_publicKey"

//------------------------------------------------------------------------------
// Static data members
//------------------------------------------------------------------------------
GPGME_Wrapper* CryptoMediator::sm_gpgmeWrapper = 0;
QSettings	   CryptoMediator::sm_settings(RecapApp::ConfFile, QSettings::NativeFormat);
QString		   CryptoMediator::sm_publicKey("");
QString		   CryptoMediator::sm_lastError("");
bool           CryptoMediator::sm_gpgEnabled = true;
bool           CryptoMediator::sm_initialised= false;

//------------------------------------------------------------------------------
// HELPER CLASS: Key selection dialog
//------------------------------------------------------------------------------
class KeyListDialog : public QDialog {
    Q_OBJECT

public:
    KeyListDialog(const QStringList& sl,
                  QWidget* parent = 0,
                  Qt::WindowFlags f = 0)

        : QDialog(parent, f),
          m_parent(parent) {

        const int w = 600, h = 200;

        setWindowTitle("Select a public key for encryption");
        setGeometry(parent->x() + 50,
                    parent->y() - 80, w, h);
        setWindowIcon(QIcon(":/img/key"));

        m_model = new QStringListModel(sl);
        m_view  = new QListView(this);
        m_view->setModel(m_model);

        QPushButton* okButton 	  = new QPushButton(tr("&OK"), this);
        QPushButton* cancelButton = new QPushButton(tr("&Cancel"), this);
        okButton->setIcon(
            QCommonStyle().standardIcon(QCommonStyle::SP_DialogOkButton)
        );
        cancelButton->setIcon(
            QCommonStyle().standardIcon(QCommonStyle::SP_DialogCancelButton)
        );

        QGridLayout* layout = new QGridLayout(this);
        layout->setSizeConstraint(QLayout::SetMaximumSize);
        layout->addWidget(m_view, 0, 0, 1, 4);
        layout->addWidget(okButton, 4, 2);
        layout->addWidget(cancelButton, 4, 3);

        connect(
            m_view->selectionModel(),
            SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this,
            SLOT(setCurrentKey(QModelIndex,QModelIndex))
        );
        connect(okButton, 	  SIGNAL(clicked()), this, SLOT(confirmSelection()));
        connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    }

    const QString& selectedKey() const {
        return m_key;
    }

private slots:
    void setCurrentKey(const QModelIndex& current,
                       const QModelIndex& previous) {

        m_key = m_model->data(current, Qt::DisplayRole).toString();
    }
    void confirmSelection() {
        QString keyID = m_key.section('\t', 0, 0);
        QString uid   = m_key.section('\t', 1);
        uid.truncate(uid.indexOf('('));
        QString email =  m_key.section(' ', 2).remove(QRegExp("(\\(|\\))"));
        email.remove(")");

        if (QMessageBox::question(
            m_parent,
            tr("Recap - Confirm key selection"),
            tr("Is this the key you wish to use for encryption?\n\n"
               "Key ID:\t%1\n"
               "User ID:\t%2\n"
               "Email:\t%3\n").arg(keyID).arg(uid).arg(email),
            QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {

            accept();
        }
    }

private:
    QStringListModel* m_model;
    QListView*        m_view;
    QString			  m_key;
    QWidget*		  m_parent;
};
#include "cryptomediator.moc"

//------------------------------------------------------------------------------
// Ctor
//------------------------------------------------------------------------------
CryptoMediator::CryptoMediator(QWidget *parent)
    : QObject(parent) {

    init(parent);
}

//------------------------------------------------------------------------------
// Initialisation only occurs once (iff a parent window is given).
//------------------------------------------------------------------------------
void CryptoMediator::init(QWidget *parent) {

    if (parent && !sm_initialised) {
        // Check if GPG was disabled previously
        sm_settings.beginGroup(CRYPT_GROUP);
        if (sm_settings.contains(GPG_ENABLED)) {
            sm_gpgEnabled = sm_settings.value(GPG_ENABLED).toBool();
        }
        // Initialise agent
        if (sm_gpgEnabled) {
            try {
                sm_gpgmeWrapper = new GPGME_Wrapper();
                sm_gpgEnabled = selectKey(parent, sm_publicKey);
            }
            catch (const std::exception& e) {

                if (QMessageBox::warning(
                        parent, "Recap",
                        QObject::tr(
                            "The GPG backend could not be initialised ("
                            "%1). GPG features will be disabled."
                            "Do you wish to skip GPG feature initialisation "
                            "in future sessions?"
                        ).arg(e.what()),
                        QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {

                    sm_settings.setValue(GPG_ENABLED, false);
                }
                sm_gpgEnabled = false;
            }
        }
        sm_initialised = true;
    }
}

//------------------------------------------------------------------------------
bool CryptoMediator::gpgEnabled() const {
    return sm_gpgEnabled;
}

//------------------------------------------------------------------------------
// Presents a selection of public keys to the user and stores the
// chosen key for use in future sessions.
// @param  parent
//		   The parent window in front of which the dialog will show.
// @param  key
//		   An out parameter which will store the user's key if a selection is
//		   made.
// @pre    The namespace is initialised and at least one public
// 		   GPG key is present on the system.
// @return true  if a selection was made
// @return false if errors occurred. GPG cannot then be used.
// @post   If a key is selected, it is stored in the configuration settings for
//		   use in future settings (in which case the dialog will not show again).
//------------------------------------------------------------------------------
bool CryptoMediator::selectKey(QWidget* parent, QString& key) {

    sm_lastError.clear();

    // If a previous selection exists, we're done
    if (sm_settings.contains(GPG_KEY)) {
        key = sm_settings.value(GPG_KEY).toString();
        return true;
    }

    // Otherwise prompt for a key selection
    std::vector<std::string> keys = sm_gpgmeWrapper->all_keys();
    QStringList stringList;
    for (size_t i = 0; i < keys.size(); ++i) {
        stringList.push_back(QString(keys[i].c_str()));
    }
    QMessageBox message(
        QMessageBox::NoIcon,
        QObject::tr("Recap - GPG Setup"),
        QObject::tr("GPG encryption appears to be configured on your system. "
                    "Nice!\n\nThe following dialog will allow you to select"
                    " a public key which you may use to encrypt sensitive "
                    "information if you so wish.\n\nNote that if you opt "
                    "not to select a key now, it can be configured later"
                    "... if you can figure out how!"),
        QMessageBox::Ok,
        parent
    );
    message.setIconPixmap(QPixmap(":img/gpg"));
    message.exec();

    KeyListDialog* dialog = new KeyListDialog(stringList, parent);
    int result = dialog->exec();

    // Save the selection if made, else prompt to disable GPG in the future
    if (result == QDialog::Accepted) {
        key = dialog->selectedKey();
        sm_settings.setValue(GPG_KEY, key);
    }
    else {
        if (QMessageBox::question(
                parent,
                "Recap",
                QObject::tr("GPG features will be disabled as no key was "
                            "selected. Would you like to disable GPG features "
                            "in future sessions too?"),
                QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {

            sm_settings.setValue(GPG_ENABLED, false);
        }
    }
    delete dialog;
    return result == QDialog::Accepted;
}

//------------------------------------------------------------------------------
const QString& CryptoMediator::lastError() const {
    return sm_lastError;
}

//------------------------------------------------------------------------------
bool CryptoMediator::encrypt(QtItemWrapper& item) const {
    sm_lastError.clear();
    if (sm_gpgEnabled) {
        try {
            QString cipher = QString::fromStdString(
                sm_gpgmeWrapper->encrypt(item.content.toStdString(),
                                         sm_publicKey.toStdString())
            );
            item.encrypted = true;
            item.content = cipher;
            return true;
        }
        catch (std::exception& e) {
            sm_lastError = e.what();
            return false;
        }
    }
    else {
        sm_lastError = "GPG features disabled";
        return false;
    }
}

//------------------------------------------------------------------------------
bool CryptoMediator::decrypt(QtItemWrapper& item) const {
    if (decrypt(item, item.content)) {
        item.encrypted = false;
        return true;
    }
    else {
        return false;
    }
}

//------------------------------------------------------------------------------
bool CryptoMediator::decrypt(const QtItemWrapper& item,
                             QString& outBuffer) const {

    sm_lastError.clear();
    if (sm_gpgEnabled) {
        try {
            outBuffer = QString::fromStdString(
                sm_gpgmeWrapper->decrypt(item.content.toStdString())
            );
            return true;
        }
        catch (const std::exception& e) {
            outBuffer.clear();
            sm_lastError = e.what();
            return false;
        }
    }
    else {
        sm_lastError = "GPG features disabled";
        return false;
    }
}
