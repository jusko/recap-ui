#include "recapapp.h"
int main(int argc, char* argv[]) {
    try {
        RecapApp app(argc, argv);
        return app.exec();
    }
    catch (const std::exception& e) {
        // TODO: replace with logging/notification system
        qDebug(e.what());
        return 1;
    }
}
