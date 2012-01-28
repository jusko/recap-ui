//------------------------------------------------------------------------------
#ifndef RECAPAPP_H
#define RECAPAPP_H
//------------------------------------------------------------------------------
#include <QApplication>
#include <stdexcept>
//------------------------------------------------------------------------------
class QtSerializerWrapper;
class CaptureForm;
class RecallView;
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Runner class responsible for initiating the app.
//------------------------------------------------------------------------------
class RecapApp : public QApplication {
    public:
        //----------------------------------------------------------------------
        // Runs the app in capture or recall mode based on the arguments
        // received.
        // @post  The application's home directory and datastore are created.
        //		  The view for the given mode is set to show.
        // @throw If invalid args are specified, the usage message is thrown.
        //----------------------------------------------------------------------
        RecapApp(int& argc, char** argv)
            throw(std::runtime_error);

        //----------------------------------------------------------------------
        // Reimplemented to handle exceptions thrown by event handlers.
        // REMOVE: Once notification/logging system is implemented.
        //----------------------------------------------------------------------
        bool notify(QObject* receiver, QEvent* event);

        ~RecapApp();

        static const QString HomeDirectory;
        static const QString ConfFile;

    private:
        QtSerializerWrapper* m_serializerWrapper;
        CaptureForm*         m_captureForm;
        RecallView*			 m_recallView;

        bool isValid(const char* arg)     const;
        bool captureMode(const char* arg) const;
        bool recallMode(const char* arg)  const;
};
#endif // RECAPAPP_H
