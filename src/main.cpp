#include <bb/cascades/Application>

#include <QLocale>
#include <QTranslator>
#include "applicationui.hpp"
#include "Util.h"

#include "InviteToDownload.hpp"
#include "RegistrationHandler.hpp"

// include JS Debugger / CS Profiler enabler
// this feature is enabled by default in the debug build only
//#define QT_DECLARATIVE_DEBUG
#include <Qt/qdeclarativedebug.h>

using namespace bb::cascades;
using namespace javelind::bb::pbuddy;

Q_DECL_EXPORT int main(int argc, char **argv)
{
    // Create a Cascades application (this is where the server is started etc)
    Application app(argc, argv);

    // Configure localization support
    QTranslator translator;
    QString locale_string = QLocale().name();
    QString filename = QString( "PresentBuddy_%1" ).arg( locale_string );
    if (translator.load(filename, "app/native/qm")) {
        app.installTranslator( &translator );
    }

    // Register classes with the Qt meta-object system
    qmlRegisterType<QTimer>("my.library", 1, 0, "QTimer");

    new ApplicationUI(&app);

    // we complete the transaction started in the app constructor and start the client event loop here
    return Application::exec();
    // when loop is exited the Application deletes the scene which deletes all its children (per qt rules for children)
}
