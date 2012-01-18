#include "recap.h"
#include "captureform.h"
#include "recallview.h"
#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    Item* i = new Item;
    QStringList tags;
    tags << "foo" << "bar" << "baz" << "fobo";
    CaptureForm f(i, tags);
    f.show();
    RecallView r(tags);
    r.show();

    return app.exec();
}
