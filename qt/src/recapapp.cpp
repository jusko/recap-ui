//------------------------------------------------------------------------------
#include "recapapp.h"
#include "qtserializerwrapper.h"
#include "captureform.h"
#include "recallview.h"
//------------------------------------------------------------------------------
#include <QObject>
#include <QDebug>
#include <QDir>
#include <QString>
#include <QCursor>
//------------------------------------------------------------------------------
#include <cstring>
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Configuration constants
// TODO: Replace with config file
//------------------------------------------------------------------------------
const QString RecapApp::HomeDirectory = QDir::homePath() + "/.recap";
const QString RecapApp::ConfFile = RecapApp::HomeDirectory + "/recap.conf";
const QString RECAP_DB = RecapApp::HomeDirectory + "/recap.sqlite3";

//------------------------------------------------------------------------------
// Command line option helpers
//------------------------------------------------------------------------------

inline std::string usage(const char* prog_name) {
    return QObject::tr(
        "Usage %1 OPTION\n\n"
        "Options:\n"
        "--capture-mode, -c\tShow capture mode dialog\n"
        "--recall-mode,  -r\tShow recall mode Ui\n"
    ).arg(prog_name).toStdString();
}

inline bool RecapApp::captureMode(const char *arg) const {
    return strcmp(arg, "-c") == 0 ||
           strcmp(arg, "--capture-mode") == 0;
}

inline bool RecapApp::recallMode(const char *arg) const {
    return strcmp(arg, "-r") == 0 ||
           strcmp(arg, "--recall-mode") == 0;
}

inline bool RecapApp::isValid(const char* arg) const {
    return captureMode(arg) || recallMode(arg);
}

//------------------------------------------------------------------------------
// Ctor:
//------------------------------------------------------------------------------
RecapApp::RecapApp(int& argc, char** argv)
    throw(std::runtime_error)

    : QApplication(argc, argv),
      m_serializerWrapper(0),
      m_captureForm(0),
      m_recallView(0) {

    if (argc != 2 || !isValid(argv[1])) {
        throw std::runtime_error(usage(argv[0]));
    }

    //---
    // TODO: Parse config file options
    //---

    // Create home directory
    QDir homeDir(HomeDirectory);
    if (!homeDir.exists()) {
        if (!homeDir.mkpath(HomeDirectory)) {
            throw std::runtime_error(
                QString(tr("Failed to create ")).append(HomeDirectory).toStdString()
            );
        }
    }
    m_serializerWrapper = new QtSerializerWrapper(RECAP_DB.toStdString().c_str());

    // Display based on mode
    QWidget* screen;
    if (captureMode(argv[1])) {
        screen = m_captureForm = new CaptureForm(*m_serializerWrapper);
    }
    else {
        screen = m_recallView = new RecallView(*m_serializerWrapper);
    }

    // TODO: Improve so that never splits across virtual desktop
    QPoint pt = QCursor::pos();
    screen->setGeometry(pt.x() - screen->width() / 2,
                        pt.y() - screen->height() /2,
                        screen->width(), screen->height());
    screen->show();
}

//------------------------------------------------------------------------------
// Dtor
//------------------------------------------------------------------------------
RecapApp::~RecapApp() {
    if (m_serializerWrapper) {
        delete m_serializerWrapper;
        m_serializerWrapper = 0;
    }
    if (m_captureForm) {
        delete m_captureForm;
        m_captureForm = 0;
    }
    if (m_recallView) {
        delete m_recallView;
        m_recallView = 0;
    }
}

//------------------------------------------------------------------------------
// REMOVE: Once notification/logging system is implemented.
//------------------------------------------------------------------------------
bool RecapApp::notify(QObject* receiver, QEvent* event) {
    try {
        return QApplication::notify(receiver, event);
    }
    catch(const std::runtime_error& err) {
        qDebug() << err.what();
        return false;
    }
}
