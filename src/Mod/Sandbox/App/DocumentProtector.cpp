/***************************************************************************
 *   Copyright (c) 2009 Werner Mayer <wmayer@users.sourceforge.net>        *
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
# include <QCoreApplication>
# include <QEvent>
# include <QMutex>
# include <QMutexLocker>
# include <QWaitCondition>
#endif

#include "DocumentProtector.h"

#include <App/Document.h>

using namespace Sandbox;


namespace Sandbox {

QMutex gProtectorStopMutex(QMutex::Recursive);
QWaitCondition gProtectorEventProcessed;
QMutex gProtectorWaitMutex(QMutex::NonRecursive);  // must be non-recursive when used with a wait condition

static const int QT_CUSTOM_EVENT_ADD_OBJECT = 10000;
static const int QT_CUSTOM_EVENT_REM_OBJECT = 10001;
static const int QT_CUSTOM_EVENT_RECOMPUTE  = 10002;

class CustomAddObjectEvent : public QEvent
{
public:
    CustomAddObjectEvent(DocumentProtector* dp, const std::string& type, const std::string& name)
        : QEvent(QEvent::Type(QT_CUSTOM_EVENT_ADD_OBJECT)), dp(dp), type(type), name(name)
    {
    }
    ~CustomAddObjectEvent()
    {
    }

    DocumentProtector* dp;
    std::string type, name;
};

class CustomRemoveObjectEvent : public QEvent
{
public:
    CustomRemoveObjectEvent(DocumentProtector* dp, const std::string& name)
        : QEvent(QEvent::Type(QT_CUSTOM_EVENT_REM_OBJECT)), dp(dp), name(name)
    {
    }
    ~CustomRemoveObjectEvent()
    {
    }

    DocumentProtector* dp;
    std::string name;
};

class CustomRecomputeEvent : public QEvent
{
public:
    CustomRecomputeEvent(DocumentProtector* dp)
        : QEvent(QEvent::Type(QT_CUSTOM_EVENT_RECOMPUTE)), dp(dp)
    {
    }
    ~CustomRecomputeEvent()
    {
    }

    DocumentProtector* dp;
};

Q_GLOBAL_STATIC(DocumentReceiver, theInstance)

DocumentReceiver *DocumentReceiver::globalInstance()
{
    return theInstance();
}

DocumentReceiver* dp = DocumentReceiver::globalInstance();

// Posts an event and waits for it to finish processing
void DocumentReceiver::postEventAndWait(QEvent* e)
{
    QMutexLocker lock(&gProtectorWaitMutex);
    QCoreApplication::postEvent(this, e);
    gProtectorEventProcessed.wait(&gProtectorWaitMutex); // waits for wait to finish
}

// Wake up the waiting automation thread
void DocumentReceiver::wakeupThread()
{
    QMutexLocker lock(&gProtectorWaitMutex);
    gProtectorEventProcessed.wakeAll();
}

void DocumentReceiver::customEvent(QEvent* e)
{
    if ((int)e->type() == QT_CUSTOM_EVENT_ADD_OBJECT) {
        CustomAddObjectEvent *ev = static_cast<CustomAddObjectEvent*>(e);
        DocumentProtector* dp = ev->dp;
        dp->obj = dp->doc->addObject(ev->type.c_str(), ev->name.c_str());
        wakeupThread();
    }
    else if ((int)e->type() == QT_CUSTOM_EVENT_REM_OBJECT) {
        CustomRemoveObjectEvent *ev = static_cast<CustomRemoveObjectEvent*>(e);
        DocumentProtector* dp = ev->dp;
        dp->doc->remObject(ev->name.c_str());
        wakeupThread();
    }
    else if ((int)e->type() == QT_CUSTOM_EVENT_RECOMPUTE) {
        CustomRecomputeEvent *ev = static_cast<CustomRecomputeEvent*>(e);
        ev->dp->doc->recompute();
        wakeupThread();
    }
}

}

DocumentProtector::DocumentProtector(App::Document* doc)
  : doc(doc), obj(0)
{
}

DocumentProtector::~DocumentProtector()
{
}

App::DocumentObject *DocumentProtector::addObject(const std::string& type, const std::string& name)
{
    DocumentReceiver::globalInstance()->postEventAndWait(new CustomAddObjectEvent(this, type, name));
    return this->obj;
}

void DocumentProtector::removeObject(const std::string& name)
{
}

void DocumentProtector::recompute()
{
    DocumentReceiver::globalInstance()->postEventAndWait(new CustomRecomputeEvent(this));
}
