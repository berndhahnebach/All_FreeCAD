/***************************************************************************
 *   (c) Jürgen Riegel (juergen.riegel@web.de) 2008                        *   
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License (LGPL)   *
 *   as published by the Free Software Foundation; either version 2 of     *
 *   the License, or (at your option) any later version.                   *
 *   for detail see the LICENCE text file.                                 *
 *                                                                         *
 *   FreeCAD is distributed in the hope that it will be useful,            *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        * 
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with FreeCAD; if not, write to the Free Software        * 
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
 *   USA                                                                   *
 *                                                                         *
 *   Juergen Riegel 2002                                                   *
 ***************************************************************************/
#include <FCConfig.h>

#ifdef _PreComp_
#   undef _PreComp_
#endif

#ifdef FC_LINUX
#   include <unistd.h>
#endif

#if HAVE_CONFIG_H
#   include <config.h>
#endif // HAVE_CONFIG_H

#include <cstdio>
#include <QApplication>
#include <QMessageBox>

// FreeCAD header
#include <Base/Console.h>
#include <Base/Interpreter.h>
#include <Base/Parameter.h>
#include <Base/Exception.h>
#include <Base/Factory.h>
#include <App/Application.h>
#include <Gui/BitmapFactory.h>
#include <Gui/Icons/background.xpm>
#include <Gui/Application.h>

// If you stumble here, run the target "BuildExtractRevision" on Windows systems or the Python script "SubWCRev.py" on Linux based systems
// which builds src/Build/Version.h. Or create your own from src/Build/Version.h.in!
#include "../Build/Version.h"

void PrintInitHelp(void);

const char sBanner[] = "\xc2\xa9 Juergen Riegel, Werner Mayer 2001-2010\n"\
"  #####                 ####  ###   ####  \n" \
"  #                    #      # #   #   # \n" \
"  #     ##  #### ####  #     #   #  #   # \n" \
"  ####  # # #  # #  #  #     #####  #   # \n" \
"  #     #   #### ####  #    #     # #   # \n" \
"  #     #   #    #     #    #     # #   #  ##  ##  ##\n" \
"  #     #   #### ####   ### #     # ####   ##  ##  ##\n\n" ;

class ProgramOptions
{
public:
    ProgramOptions()
    {
        newcout = new ProgramOptionsStream(out);
        oldcout = std::cout.rdbuf(newcout);
        out.reserve(80);
        newcerr = new ProgramOptionsStream(err);
        oldcerr = std::cerr.rdbuf(newcerr);
        err.reserve(80);

        error = true;
        ::atexit(ProgramOptions::failure);
    }
    ~ProgramOptions()
    {
        std::cout.rdbuf(oldcout);
        delete newcout;
        std::cerr.rdbuf(oldcerr);
        delete newcerr;
        error = false;
    }
    static void failure()
    {
        if (error) {
            int argc=0;
            QApplication app(argc,0);
            QString appName = QString::fromAscii(App::Application::Config()["ExeName"].c_str());
            if (!err.empty()) {
                QString msg = QString::fromAscii(err.c_str());
                QString s = QLatin1String("<pre>") + msg + QLatin1String("</pre>");
                QMessageBox::critical(0, appName, s);
            }
            else if (!out.empty()) {
                QString msg = QString::fromAscii(out.c_str());
                QString s = QLatin1String("<pre>") + msg + QLatin1String("</pre>");
                QMessageBox::information(0, appName, s);
            }
        }
    }

private:
    class ProgramOptionsStream : public std::streambuf
    {
    public:
        ProgramOptionsStream(std::string& s) : buf(s)
        {
        }
        int overflow(int c = EOF)
        {
            if (c != EOF)
                buf.push_back((char)c);
            return c;
        }
        int sync()
        {
            return 0;
        }
    private:
        std::string& buf;
    };

private:
    friend class ProgramOptionsStream;
    std::streambuf* oldcout;
    std::streambuf* newcout;
    std::streambuf* oldcerr;
    std::streambuf* newcerr;
    static std::string out, err;
    static bool error;
};

bool ProgramOptions::error = false;
std::string ProgramOptions::out;
std::string ProgramOptions::err;

int main( int argc, char ** argv )
{
    // Make sure that we use '.' as decimal point
#if defined(FC_OS_LINUX)
    putenv("LANG=C");
    putenv("LC_ALL=C");
#else
    setlocale(LC_NUMERIC, "C");
#endif

    // Name and Version of the Application
    App::Application::Config()["ExeName"] = "FreeCAD";
    App::Application::Config()["ExeVendor"] = "FreeCAD";
    App::Application::Config()["AppDataSkipVendor"] = "true";
    App::Application::Config()["MaintainerUrl"] = "http://apps.sourceforge.net/mediawiki/free-cad/index.php?title=Main_Page";

    // set the banner (for logging and console)
    App::Application::Config()["ConsoleBanner"] = sBanner;
    App::Application::Config()["AppIcon"] = "freecad";
    App::Application::Config()["SplashPicture"] = "freecadsplash";
    App::Application::Config()["StartWorkbench"] = "CompleteWorkbench";
    //App::Application::Config()["HiddenDockWindow"] = "Property editor";
    App::Application::Config()["SplashAlignment" ] = "Bottom|Left";
    App::Application::Config()["SplashTextColor" ] = "#ffffff"; // white

    try {
        // Init phase ===========================================================
        // sets the default run mode for FC, starts with gui if not overridden in InitConfig...
        App::Application::Config()["RunMode"] = "Gui";

        // Inits the Application 
        ProgramOptions po;
        App::Application::init(argc,argv);
        Gui::Application::initApplication();
    }
    catch (const Base::Exception& e) {
        // Popup an own dialog box instead of that one of Windows
        QApplication app(argc,argv);
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
        QApplication app(argc,argv);
        QString appName = QString::fromAscii(App::Application::Config()["ExeName"].c_str());
        QString msg = QObject::tr("Unknown runtime error occurred while initializing %1.\n\n"
                                  "Please contact the application's support team for more information.\n\n").arg(appName);
        QMessageBox::critical(0, QObject::tr("Initialization of %1 failed").arg(appName), msg);
        exit(101);
    }

    // Run phase ===========================================================
    Base::RedirectStdOutput stdcout;
    Base::RedirectStdLog    stdclog;
    Base::RedirectStdError  stdcerr;
    std::streambuf* oldcout = std::cout.rdbuf(&stdcout);
    std::streambuf* oldclog = std::clog.rdbuf(&stdclog);
    std::streambuf* oldcerr = std::cerr.rdbuf(&stdcerr);

    try {
        if (App::Application::Config()["RunMode"] == "Gui")
            Gui::Application::runApplication();
        else
            App::Application::runApplication();
    }
    catch (const Base::Exception& e) {
        Base::Console().Error("%s\n", e.what());
    }
    catch (...) {
        Base::Console().Error("Application unexpectedly terminated\n");
    }

    std::cout.rdbuf(oldcout);
    std::clog.rdbuf(oldclog);
    std::cerr.rdbuf(oldcerr);

    // Destruction phase ===========================================================
    Base::Console().Log("%s terminating...\n",App::Application::Config()["ExeName"].c_str());

    // cleans up 
    App::Application::destruct();

    Base::Console().Log("%s completely terminated\n",App::Application::Config()["ExeName"].c_str());

    return 0;
}
