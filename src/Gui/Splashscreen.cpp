/***************************************************************************
 *   Copyright (c) 2004 Werner Mayer <wmayer[at]users.sourceforge.net>     *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/


#include "PreCompiled.h"
#ifndef _PreComp_
# include <QMutex>
# include <QWaitCondition>
#endif

#include "Splashscreen.h"
#include <Base/Console.h>
#include <App/Application.h>
#include <Gui/MainWindow.h>


using namespace Gui;
using namespace Gui::Dialog;

namespace Gui {
/** Displays all messages at startup inside the splash screen.
 * \author Werner Mayer
 */
class SplashObserver : public Base::ConsoleObserver
{
public:
    SplashObserver(QSplashScreen* splasher=0, const char* name=0)
      : splash(splasher), alignment(Qt::AlignBottom|Qt::AlignLeft), textColor(Qt::black)
    {
        Base::Console().AttachObserver(this);

        // allow to customize text position and color
        const std::map<std::string,std::string>& cfg = App::GetApplication().Config();
        std::map<std::string,std::string>::const_iterator al = cfg.find("SplashAlignment");
        if (al != cfg.end()) {
            QString alt = QString::fromAscii(al->second.c_str());
            int align=0;
            if (alt.startsWith(QLatin1String("VCenter")))
                align = Qt::AlignVCenter;
            else if (alt.startsWith(QLatin1String("Top")))
                align = Qt::AlignTop;
            else
                align = Qt::AlignBottom;

            if (alt.endsWith(QLatin1String("HCenter")))
                align += Qt::AlignHCenter;
            else if (alt.endsWith(QLatin1String("Right")))
                align += Qt::AlignRight;
            else
                align += Qt::AlignLeft;

            alignment = align;
        }

        // choose text color
        std::map<std::string,std::string>::const_iterator tc = cfg.find("SplashTextColor");
        if (tc != cfg.end()) {
            QColor col; col.setNamedColor(QString::fromAscii(tc->second.c_str()));
            if (col.isValid())
                textColor = col;
        }
    }

    virtual ~SplashObserver()
    {
        Base::Console().DetachObserver(this);
    }

    void Warning(const char * s)
    {
        Log(s);
    }
    void Message(const char * s)
    {
        Log(s);
    }
    void Error  (const char * s)
    {
        Log(s);
    }
    void Log (const char * s)
    {
        QString msg(QString::fromUtf8(s));
        QRegExp rx;
        // ignore 'Init:' and 'Mod:' prefixes
        rx.setPattern(QLatin1String("^\\s*(Init:|Mod:)\\s*"));
        int pos = rx.indexIn(msg);
        if (pos != -1) {
            msg = msg.mid(rx.matchedLength());
        }
        else {
            // ignore activation of commands
            rx.setPattern(QLatin1String("^\\s*(\\+App::|Create|CmdC:|CmdG:|Act:)\\s*"));
            pos = rx.indexIn(msg);
            if (pos == 0)
                return;
        }

        splash->showMessage(msg.replace(QLatin1String("\n"), QString()), alignment, textColor);
        QMutex mutex;
        mutex.lock();
        QWaitCondition().wait(&mutex, 50);
    }

private:
    QSplashScreen* splash;
    int alignment;
    QColor textColor;
};
} // namespace Gui

// ------------------------------------------------------------------------------

/**
 * Constructs a splash screen that will display the pixmap.
 */
SplashScreen::SplashScreen(  const QPixmap & pixmap , Qt::WFlags f )
    : QSplashScreen(pixmap, f)
{
    // write the messages to splasher
    messages = new SplashObserver(this);
}

/** Destruction. */
SplashScreen::~SplashScreen()
{
    delete messages;
}

/** 
 * Draws the contents of the splash screen using painter \a painter. The default 
 * implementation draws the message passed by message().
 */
void SplashScreen::drawContents ( QPainter * painter )
{
    QSplashScreen::drawContents(painter);
}

// ------------------------------------------------------------------------------

/* TRANSLATOR Gui::Dialog::AboutDialog */

/**
 *  Constructs an AboutDialog which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'WStyle_Customize|WStyle_NoBorder|WType_Modal'
 *
 *  The dialog will be modal.
 */
AboutDialog::AboutDialog( QWidget* parent )
  : QDialog(parent, Qt::FramelessWindowHint)
{
    setModal(true);
    ui.setupUi(this);
    ui.labelSplashPicture->setPixmap(getMainWindow()->splashImage());
    setupLabels();
}

/**
 *  Destroys the object and frees any allocated resources
 */
AboutDialog::~AboutDialog()
{
  // no need to delete child widgets, Qt does it all for us
}

void AboutDialog::setupLabels()
{
    QString exeName = QString::fromAscii(App::Application::Config()["ExeName"].c_str());
    QString banner  = QString::fromUtf8(App::Application::Config()["ConsoleBanner"].c_str());
    banner = banner.left( banner.indexOf(QLatin1Char('\n')) );
    QString major  = QString::fromAscii(App::Application::Config()["BuildVersionMajor"].c_str());
    QString minor  = QString::fromAscii(App::Application::Config()["BuildVersionMinor"].c_str());
    QString build  = QString::fromAscii(App::Application::Config()["BuildRevision"].c_str());
    QString disda  = QString::fromAscii(App::Application::Config()["BuildRevisionDate"].c_str());
    QString mturl  = QString::fromAscii(App::Application::Config()["MaintainerUrl"].c_str());

    QString author = QString::fromAscii("<html><head><meta name=\"qrichtext\" content=\"1\" /></head>"
        "<body style=\" white-space: pre-wrap; font-family:MS Shell Dlg 2; font-size:7.8pt; "
        "font-weight:400; font-style:normal; text-decoration:none;\"><p style=\" margin-top:0px; "
        "margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
        "<span style=\" font-size:8pt; font-weight:600; text-decoration: underline; color:#0000ff;\">"
        "%1 %2</span></p></body></html>").arg(exeName).arg(banner);
    QString version = QString::fromAscii("<html><head><meta name=\"qrichtext\" content=\"1\" /></head>"
        "<body style=\" white-space: pre-wrap; font-family:MS Shell Dlg 2; font-size:7.8pt; "
        "font-weight:400; font-style:normal; text-decoration:none;\"><p style=\" margin-top:0px; "
        "margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
        "<span style=\" font-size:8pt; font-weight:600;\">%1.%2</span></p></body></html>").arg(major).arg(minor);
    QString revision = QString::fromAscii("<html><head><meta name=\"qrichtext\" content=\"1\" /></head>"
        "<body style=\" white-space: pre-wrap; font-family:MS Shell Dlg 2; font-size:7.8pt; "
        "font-weight:400; font-style:normal; text-decoration:none;\"><p style=\" margin-top:0px; "
        "margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
        "<span style=\" font-size:8pt; font-weight:600;\">%1</span></p></body></html>").arg(build);
    QString date = QString::fromAscii("<html><head><meta name=\"qrichtext\" content=\"1\" /></head>"
        "<body style=\" white-space: pre-wrap; font-family:MS Shell Dlg 2; font-size:7.8pt; "
        "font-weight:400; font-style:normal; text-decoration:none;\"><p style=\" margin-top:0px; "
        "margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
        "<span style=\" font-size:8pt; font-weight:600;\">%1</span></p></body></html>").arg(disda);
    ui.labelAuthor->setText(author);
    ui.labelAuthor->setUrl(mturl);
    ui.labelBuildVersion->setText(version);
    ui.labelBuildRevision->setText(revision);
    ui.labelBuildDate->setText(date);
}

#include "moc_Splashscreen.cpp"
