/***************************************************************************
 *   Copyright (c) 2004 Werner Mayer <werner.wm.mayer@gmx.de>              *
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
# include <qcheckbox.h>
# include <qfile.h>
# include <qlabel.h>
# include <qlayout.h>
# include <qtextstream.h>
#endif

#include "DlgTipOfTheDayImp.h"
#include "Application.h"
#include "MainWindow.h"

#include <Base/Parameter.h>
#include <Base/Console.h>

using namespace Gui::Dialog;

/**
 *  Constructs a DlgTipOfTheDayImp which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DlgTipOfTheDayImp::DlgTipOfTheDayImp( QWidget* parent, Qt::WFlags fl )
  : QDialog( parent, fl | Qt::WindowTitleHint | Qt::WindowSystemMenuHint ),
  WindowParameter("General")
{
    setupUi(this);
    _http = new QHttp;
    connect(_http, SIGNAL(done(bool)), 
            this, SLOT(onDone(bool)));
    connect(_http, SIGNAL(stateChanged(int)), 
            this, SLOT(onStateChanged(int)));
    connect(_http, SIGNAL(responseHeaderReceived(const QHttpResponseHeader &)), 
            this, SLOT(onResponseHeaderReceived(const QHttpResponseHeader &)));

    bool tips = getWindowParameter()->GetBool("Tipoftheday", true);
    checkShowTips->setChecked(tips);

    // Since the resize mode of DlgTipOfTheDayBase does not
    // work properly so set this by hand 
    setMinimumSize(QSize(320, 250));
    layout()->setSizeConstraint( QLayout::SetNoConstraint );

    reload();
    on_buttonNextTip_clicked();
}

/** Destroys the object and frees any allocated resources */
DlgTipOfTheDayImp::~DlgTipOfTheDayImp()
{
    delete _http;
    getWindowParameter()->SetBool("Tipoftheday", checkShowTips->isChecked());
}

/** Shows next tip taken from the Tip-of-the-day site. */
void DlgTipOfTheDayImp::on_buttonNextTip_clicked()
{
    _iCurrentTip = ( _iCurrentTip + 1 ) % _lTips.size();
    textTip->setText( _lTips[_iCurrentTip] );
}

/** Reloads all tips from Tip-of-the-day site. */
void DlgTipOfTheDayImp::reload()
{
    //QUrl url("http://juergen-riegel.net/FreeCAD/Docu/index.php?title=Tip_of_the_day");
    //_http->setHost(url.host());
    //_http->get(url.path(), 0);
    _http->setHost("juergen-riegel.net");
    _http->get("/FreeCAD/Docu/index.php?title=Tip_of_the_day", 0);

    // search for the Wiki Tip-of-the-day site
    QString home = App::GetApplication().GetHomePath();
    QString path = getDefaultParameter()->GetASCII("OnlineDocDir", "/doc/free-cad.sourceforge.net/wiki").c_str();
    QString file = home + path + "index.php@TipOfTheDay.html";

    QFile f ( file );
    QTextStream txt( &f );

    if (f.open( QIODevice::ReadOnly )) {
        do {
            QString inp = txt.readLine();
            if (inp.startsWith("<li> ")) {
                _lTips << inp;
            }
        }
        //HACK Not sure this works (was  while ( !txt.eof()) )
        while ( !txt.atEnd() );
    }

    _iCurrentTip = 0;

    // show standard tip if site is not found
    if (_lTips.empty()) {
        //_lTips << QString("If you want to make use of the Tip-of-the-day you must download "
        //                  "FreeCAD's online help from <a href=\"http://juergen-riegel.net/FreeCAD/Docu/index.php?title=Main_Page\">"
        //                  "http://freecad.juergen-riegel.net/Docu/</a>."
        //                  "To download the online help press Help -> Download online help.");
        _lTips << QString("If you want to learn more about FreeCAD you must go to "
                          "<a href=\"http://juergen-riegel.net/FreeCAD/Docu/index.php?title=Main_Page\">"
                          "http://freecad.juergen-riegel.net/Docu/</a> or press the Help item in the Help menu.");
    }
}

void DlgTipOfTheDayImp::onResponseHeaderReceived(const QHttpResponseHeader & responseHeader)
{
    if (responseHeader.statusCode() != 200) {
        QString msg = tr("Download failed: %1\n").arg(responseHeader.reasonPhrase());
        Base::Console().Log(msg.toAscii());
        _http->abort();
    }
}

void DlgTipOfTheDayImp::onDone(bool err)
{
    if (err)
        return;

    //FIXME: Quick hack
    QVariant data(_http->readAll());
    QString text = data.toString();
    QStringList lines = text.split("\n");
    bool found = false;
    QString buffer;
    for (QStringList::Iterator it = lines.begin(); it != lines.end(); ++it) {
        if (!found && it->startsWith("<div class=\"editsection\" style")) {
            found = true;
            continue;
        }

        if (found) {
            buffer += *it;
            if (it->startsWith("</p>")) {
                found = false;
                _lTips.push_back(buffer);
                buffer.clear();
            }
        }
    }
}

/**
 * Prints some status information.
 */
void DlgTipOfTheDayImp::onStateChanged (int state)
{
    switch (state) {
        case QHttp::Connecting:
            Base::Console().Log("Connecting to host...\n");
            break;
        case QHttp::Sending:
            Base::Console().Log("Sending to host...\n");
            break;
        case QHttp::Reading:
            Base::Console().Log("Reading from host...\n");
            break;
        case QHttp::Closing:
        case QHttp::Unconnected:
            Base::Console().Log(_http->errorString().toAscii());
            break;
        default:
            break;
  }
}

#include "moc_DlgTipOfTheDayImp.cpp"
