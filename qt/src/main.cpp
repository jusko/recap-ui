#include "recap.h"
#include "captureform.h"
#include "recallview.h"
#include "qtserializerwrapper.h"
#include "itemmodel.h"
#include <QApplication>

#include <QDebug>
#include <stdexcept>
class Recap : public QApplication {
public:
    Recap(int& argc, char** argv) : QApplication(argc, argv) {}

    bool notify(QObject* receiver, QEvent* event) {
        try {
            return QApplication::notify(receiver, event);
        }
        catch(const std::runtime_error& err) {
            qDebug() << err.what();
            return false;
        }
    }
};
int main(int argc, char* argv[]) {
    Recap app(argc, argv);

    QtSerializerWrapper glue("test.db");

//    CaptureForm cf(glue);
//    QtItemWrapper fobo;
//    fobo.title = "title";
//    fobo.content = "<h1>some content</h1><br> <para>fobo wobo wibbly dibbly</para>";
//    fobo.tags << "fob" << "wob" << "wibiddy" << "woboo";
//    cf.show(fobo);

    RecallView rv(glue);
    rv.show();

    return app.exec();
}
