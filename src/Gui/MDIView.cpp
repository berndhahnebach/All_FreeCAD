/***************************************************************************
 *   Copyright (c) 2007 Werner Mayer <werner.wm.mayer@gmx.de>              *
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
# include <qapplication.h>
# include <qregexp.h>
# include <QEvent>
# include <QCloseEvent>
#endif


#include "MDIView.h"
#include "Command.h"
#include "Document.h"
#include "Application.h"
#include "MainWindow.h"

using namespace Gui;

TYPESYSTEM_SOURCE_ABSTRACT(Gui::MDIView,Gui::BaseView);


MDIView::MDIView( Gui::Document* pcDocument,QWidget* parent, Qt::WFlags wflags )
  : QMainWindow(parent, wflags), BaseView(pcDocument),currentMode(Child), wstate(Qt::WindowNoState)
{
    setAttribute(Qt::WA_DeleteOnClose);
}

MDIView::~MDIView()
{
}

void MDIView::onRelabel(Gui::Document *pDoc)
{
    if (!bIsPassive) {
        // Try to separate document name and view number if there is one
        QString cap = windowTitle();
        // Either with dirty flag ...
        QRegExp rx("(\\s\\:\\s\\d+\\[\\*\\])$");
        int pos = rx.lastIndexIn(cap);
        if (pos == -1) {
            // ... or not
            rx.setPattern("(\\s\\:\\s\\d+)$");
            pos = rx.lastIndexIn(cap);
        }
        if (pos != -1) {
            cap = QString::fromUtf8(pDoc->getDocument()->Label.getValue());
            cap += rx.cap();
            setWindowTitle(cap);
        }
        else {
            cap = QString::fromUtf8(pDoc->getDocument()->Label.getValue());
            cap = QString("%1[*]").arg(cap);
            setWindowTitle(cap);
        }
    }
}

/// receive a message
bool MDIView::onMsg(const char* pMsg,const char** ppReturn)
{
    return false;
}

bool MDIView::onHasMsg(const char* pMsg) const
{
    return false;
}

void MDIView::closeEvent(QCloseEvent *e)
{
    if (bIsPassive){
        if (canClose()){
            e->accept();
            // avoid flickering
            getMainWindow()->removeWindow(this);
            QMainWindow::closeEvent(e);
        }
    }
    else{
        if (getGuiDocument()->isLastView()) {
            getGuiDocument()->canClose(e);
            if (e->isAccepted()) {
                // avoid flickering
                getMainWindow()->removeWindow(this);
                QMainWindow::closeEvent(e);
            }
        }
        else
            e->accept();
    }
}

void MDIView::windowStateChanged( MDIView* )
{
}

void MDIView::print()
{
    // print command specified but print method not overriden!
    assert(0);
}

void MDIView::printPdf()
{
    // print command specified but print method not overriden!
    assert(0);
}

QSize MDIView::minimumSizeHint () const
{
    return QSize(400, 300);
}

void MDIView::changeEvent(QEvent *e)
{
    switch (e->type()) {
        case QEvent::ActivationChange:
            {
                // Forces this top-level window to be the active view of the main window
                if (isActiveWindow()) {
                    if (getMainWindow()->activeWindow() != this)
                        getMainWindow()->setActiveWindow(this);
                }
            }   break;
        case QEvent::WindowTitleChange:
        case QEvent::ModifiedChange:
            {
                // sets the appropriate tab of the tabbar
                getMainWindow()->tabChanged( this );
            }   break;
        default:
            {
                QMainWindow::changeEvent(e);
            }   break;
    }
}

void MDIView::setCurrentViewMode(ViewMode mode)
{
    switch (mode) {
        // go to normal mode
        case Child:
            {
                if (this->currentMode == FullScreen) {
                    showNormal();
                    setWindowFlags(windowFlags() & ~Qt::Window);
                } else if (this->currentMode == TopLevel) {
                    this->wstate = windowState();
                    setWindowFlags( windowFlags() & ~Qt::Window );
                }

                if (this->currentMode != Child) {
                    this->currentMode = Child;
                    getMainWindow()->addWindow(this);
                    getMainWindow()->activateWindow();
                    update();
                }
            }   break;
        // go to top-level mode
        case TopLevel:
            {
                if (this->currentMode == Child) {
                    getMainWindow()->removeWindow(this);
                    setWindowFlags(windowFlags() | Qt::Window);
                    setParent(0, Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | 
                                 Qt::WindowMinMaxButtonsHint);
                    if (this->wstate & Qt::WindowMaximized)
                        showMaximized();
                    else
                        showNormal();

#if defined(Q_WS_X11)
                    extern void qt_x11_wait_for_window_manager( QWidget* w ); // defined in qwidget_x11.cpp
                    qt_x11_wait_for_window_manager(this);
#endif
                    activateWindow();
                } else if (this->currentMode == FullScreen) {
                    if (this->wstate & Qt::WindowMaximized)
                        showMaximized();
                    else
                        showNormal();
                }
            
                this->currentMode = TopLevel;
                update();
            }   break;
        // go to fullscreen mode
        case FullScreen:
            {
                if (this->currentMode == Child) {
                    getMainWindow()->removeWindow(this);
                    setWindowFlags(windowFlags() | Qt::Window);
                    setParent(0, Qt::Window);
                    showFullScreen();
                } else if (this->currentMode == TopLevel) {
                    this->wstate = windowState();
                    showFullScreen();
                }
                
                this->currentMode = FullScreen;
                update();
            }   break;
    }
}

#include "moc_MDIView.cpp"
