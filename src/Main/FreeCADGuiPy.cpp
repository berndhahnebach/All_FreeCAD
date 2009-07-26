#include <FCConfig.h>

#if HAVE_CONFIG_H
#   include <config.h>
#endif // HAVE_CONFIG_H

#include <QApplication>
#include <QIcon>
#include <QMessageBox>
#include <Python.h>
#include <Inventor/Qt/SoQt.h>

// FreeCAD Base header
#include <Base/Exception.h>
#include <Base/Interpreter.h>
#include <App/Application.h>
#include <Gui/Application.h>
#include <Gui/MainWindow.h>
#include <Gui/SoFCDB.h>


#ifdef FC_OS_WIN32
#   define MainExport __declspec(dllexport)
#else
#   define MainExport
#endif

extern "C"
{
    void MainExport initFreeCADGui()
    {
        try {
            Base::Interpreter().loadModule("FreeCAD");
            App::Application::Config()["AppIcon"] = "FCIcon";
            App::Application::Config()["ConsoleBanner"] = "(c) Juergen Riegel, Werner Mayer 2001-2009\n";
            Gui::Application::initApplication();
            std::string appName =  App::Application::Config()["ExeName"];
            if (qApp) {
                appName = (const char*)qApp->applicationName().toUtf8();
            }

            static Gui::Application *app = new Gui::Application();

            // init the Inventor subsystem
            SoDB::init();
            SoQt::init(appName.c_str());
            Gui::SoFCDB::init();
        }
        catch (const Base::Exception& e) {
            // Popup an own dialog box instead of that one of Windows
            QString appName = QString::fromAscii(App::Application::Config()["ExeName"].c_str());
            QString msg;
            msg = QObject::tr("While initializing %1 the  following exception occurred: '%2'\n\n"
                              "Python is searching for its files in the following directories:\n%3\n\n"
                              "Python version information:\n%4\n")
                              .arg(appName).arg(QString::fromUtf8(e.what()))
                              .arg(QString::fromUtf8(Py_GetPath())).arg(QString::fromAscii(Py_GetVersion()));
            const char* pythonhome = getenv("PYTHONHOME");
            if (pythonhome) {
                msg += QObject::tr("\nThe environment variable PYTHONHOME is set to '%1'.")
                    .arg(QString::fromUtf8(pythonhome));
                msg += QObject::tr("\nSetting this environment variable might cause Python to fail. "
                    "Please contact your administrator to unset it on your system.\n\n");
            } else {
                msg += QObject::tr("\nPlease contact the application's support team for more information.\n\n");
            }

            QMessageBox::critical(0, QObject::tr("Initialization of %1 failed").arg(appName), msg);
            exit(100);
        }
        catch (...) {
            // Popup an own dialog box instead of that one of Windows
            QString appName = QString::fromAscii(App::Application::Config()["ExeName"].c_str());
            QString msg = QObject::tr("Unknown runtime error occurred while initializing %1.\n\n"
                                      "Please contact the application's support team for more information.\n\n").arg(appName);
            QMessageBox::critical(0, QObject::tr("Initialization of %1 failed").arg(appName), msg);
            exit(101);
        }
    }
} // extern "C"

