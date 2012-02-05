#ifndef CRYPTOMEDIATOR_H
#define CRYPTOMEDIATOR_H
//------------------------------------------------------------------------------
#include <QString>
#include <QSettings>
//------------------------------------------------------------------------------
class QWidget;
class QtItemWrapper;
class GPGME_Wrapper;
class CaptureForm;
class RecallView;
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Monostate mediator of cryptographic requirements
// Responsible for:
// - Disabling GPG features unless it's installed.
// - Presenting a selection and remembering the selection of public keys.
// - GPG encryption and decryption.
//------------------------------------------------------------------------------
class CryptoMediator : public QObject {
    public:
        //----------------------------------------------------------------------
        // Initialises GPG features if possible.
        // @param  parent
        //		   The parent window in front of which any dialogs may appear.
        //
        // @post   An appropriate information message is displayed if problems
        //		   are encountered which provide the option to remember not
        //		   to use GPG in future sessions.
        //
        //		   If the initialisation is successful and the user selected an
        //		   encryption key, it is remembered for use in future sessions.
        //
        //		   If the user did not choose a key, they are presented with an
        //		   option to disable GPG features anyway, until they configure
        //		   it via some means as yet to be implemented.
        //----------------------------------------------------------------------
        CryptoMediator(QWidget* parent = 0);

        //----------------------------------------------------------------------
        // Queries whether GPG features are available or not.
        // @return true  if GPG features are available.
        // @return false if GPG features are not available.
        //----------------------------------------------------------------------
        bool gpgEnabled() const;

        //----------------------------------------------------------------------
        // Encryption operation
        // @param  item
        //		   The item whose notes are to be encrypted
        // @return true  if the operation succeeded
        // @return fales if the operation failed
        // @pre    Encryption features are enabled and a public key has been
        //		   selected.
        // @post   The item's notes are encrypted and ASCII armored. If the
        //		   operation failed, lastError() will return a description of
        //		   the error.
        //----------------------------------------------------------------------
        bool encrypt(QtItemWrapper& item) const;

        //----------------------------------------------------------------------
        // Decryption operation
        // @param  item
        //		   The item whose notes are to be decrypted
        // @return true  if the operation succeeded
        // @return fales if the operation failed
        // @pre    Decryption features are enabled and a public key has been
        //		   selected.
        // @post   The item's notes are decrypted into plaintext. If the
        //		   operation failed, lastError() will return a description of
        //		   the error.
        //----------------------------------------------------------------------
        bool decrypt(QtItemWrapper& item) const;

        //----------------------------------------------------------------------
        // Decryption operation (overload)
        // @param  item
        //	       The item whose notes are to be decrypted
        // @param  outBuffer
        //		   The buffer to store the decrypted plain text.
        // @return true  if the operation succeeds
        // @return false if the operation failse
        // @pre    Encryption features are enabled and a public key has been
        //		   selected.
        // @post   The outBuffer is loaded with the plain text decrypted from
        //		   the item's notes. If the operation failed, lastError() will
        //		   return a description of the error.
        //
        // @note   This operation is different to the original in that
        //		   the item's contents /remain encrypted/ for persistance.
        //----------------------------------------------------------------------
        bool decrypt(const QtItemWrapper& item, QString& outBuffer) const;

        //----------------------------------------------------------------------
        // @return The last error set through the API or an empty string if no
        //		   has occurred.
        //----------------------------------------------------------------------
        const QString& lastError() const;

    private:
        void init(QWidget* parent);
        bool selectKey(QWidget* parent, QString&);

        static GPGME_Wrapper*  sm_gpgmeWrapper;
        static QSettings	   sm_settings;
        static QString		   sm_publicKey;
        static bool            sm_gpgEnabled;
        static bool			   sm_initialised;
        static QString         sm_lastError;
};

#endif // CRYPTOMEDIATOR_H
