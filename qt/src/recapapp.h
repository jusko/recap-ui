#ifndef RECAPAPP_H
#define RECAPAPP_H

#include <QApplication>
#include <stdexcept>
class QtSerializerWrapper;
class CaptureForm;
class RecallView;

//------------------------------------------------------------------------------
// Runner class responsible for initiating the app.
//------------------------------------------------------------------------------
class RecapApp : public QApplication {
    public:
        //----------------------------------------------------------------------
        // Runs the app in capture or debug mode based on the arguments
        // received.
        // @throw If invalid args are specified, the usage message is thrown.
        //----------------------------------------------------------------------
        RecapApp(int& argc, char** argv)
            throw(std::runtime_error);

        ~RecapApp();

        //----------------------------------------------------------------------
        // Reimplemented to handle exceptions thrown by event handlers.
        //----------------------------------------------------------------------
        bool notify(QObject* receiver, QEvent* event);

    private:
        QtSerializerWrapper* m_serializerWrapper;
        CaptureForm*         m_captureForm;
        RecallView*			 m_recallView;

        bool isValid(const char* arg)     const;
        bool captureMode(const char* arg) const;
        bool recallMode(const char* arg)  const;
};

#endif // RECAPAPP_H
