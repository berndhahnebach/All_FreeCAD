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
# include <QObject>
# include <QEvent>
# include <QMutex>
# include <QMutexLocker>
# include <QSemaphore>
# include <QThread>
# include <QWaitCondition>
#endif

#include "DocumentProtector.h"

#include <App/Document.h>
#include <Base/Exception.h>

using namespace Sandbox;


namespace Sandbox {

static const int QT_CUSTOM_EVENT_ADD_OBJECT = 10000;
static const int QT_CUSTOM_EVENT_REM_OBJECT = 10001;
static const int QT_CUSTOM_EVENT_RECOMPUTE  = 10002;

class CustomDocumentProtectorEvent : public QEvent
{
public:
    CustomDocumentProtectorEvent(int type, DocumentProtector* dp)
        : QEvent(QEvent::Type(type)), dp(dp), semaphore(0)
    {
    }
    ~CustomDocumentProtectorEvent()
    {
        if (semaphore)
            semaphore->release();
    }

    DocumentProtector* dp;
    QSemaphore* semaphore;
};

class CustomAddObjectEvent : public CustomDocumentProtectorEvent
{
public:
    CustomAddObjectEvent(DocumentProtector* dp, const std::string& type, const std::string& name)
        : CustomDocumentProtectorEvent(QT_CUSTOM_EVENT_ADD_OBJECT, dp), type(type), name(name)
    {
    }
    ~CustomAddObjectEvent()
    {
    }

    std::string type, name;
};

class CustomRemoveObjectEvent : public CustomDocumentProtectorEvent
{
public:
    CustomRemoveObjectEvent(DocumentProtector* dp, const std::string& name)
        : CustomDocumentProtectorEvent(QT_CUSTOM_EVENT_REM_OBJECT, dp), name(name)
    {
    }
    ~CustomRemoveObjectEvent()
    {
    }

    std::string name;
};

class CustomRecomputeEvent : public CustomDocumentProtectorEvent
{
public:
    CustomRecomputeEvent(DocumentProtector* dp)
        : CustomDocumentProtectorEvent(QT_CUSTOM_EVENT_RECOMPUTE, dp)
    {
    }
    ~CustomRecomputeEvent()
    {
    }
};

class DocumentReceiver : public QObject
{

public:
    DocumentReceiver(QObject *parent = 0) : QObject(parent)
    {
    }
    ~DocumentReceiver()
    {
    }

    static DocumentReceiver *globalInstance();

protected:
    void customEvent(QEvent*);
    void postEventAndWait(CustomDocumentProtectorEvent*);

    // friends
    friend class DocumentProtector;
};

Q_GLOBAL_STATIC(DocumentReceiver, theInstance)

DocumentReceiver *DocumentReceiver::globalInstance()
{
    return theInstance();
}

// Posts an event and waits for it to finish processing
void DocumentReceiver::postEventAndWait(CustomDocumentProtectorEvent* e)
{
    QThread *currentThread = QThread::currentThread();
    QThread *thr = this->thread(); // this is the main thread

    if (currentThread == thr) {
        // we're in the main thread
        QCoreApplication::sendEvent(this, e);
        delete e;
    }
    else {
        QSemaphore semaphore;
        e->semaphore = &semaphore;
        QCoreApplication::postEvent(DocumentReceiver::globalInstance(), e);
        // wait until the event has been processed
        semaphore.acquire();
    }
}

void DocumentReceiver::customEvent(QEvent* e)
{
    if ((int)e->type() == QT_CUSTOM_EVENT_ADD_OBJECT) {
        CustomAddObjectEvent *ev = static_cast<CustomAddObjectEvent*>(e);
        DocumentProtector* dp = ev->dp;
        dp->obj = dp->getDocument()->addObject(ev->type.c_str(), ev->name.c_str());
    }
    else if ((int)e->type() == QT_CUSTOM_EVENT_REM_OBJECT) {
        CustomRemoveObjectEvent *ev = static_cast<CustomRemoveObjectEvent*>(e);
        DocumentProtector* dp = ev->dp;
        dp->getDocument()->remObject(ev->name.c_str());
    }
    else if ((int)e->type() == QT_CUSTOM_EVENT_RECOMPUTE) {
        CustomRecomputeEvent *ev = static_cast<CustomRecomputeEvent*>(e);
        ev->dp->getDocument()->recompute();
    }
}

}

DocumentProtector::DocumentProtector(App::Document* doc)
  : App::DocumentObserver(doc), obj(0)
{
}

DocumentProtector::~DocumentProtector()
{
}

void DocumentProtector::init()
{
    // this method must be called somewhere from the main thread
    (void)DocumentReceiver::globalInstance();
}

void DocumentProtector::slotCreatedDocument(App::Document& Doc)
{
}

void DocumentProtector::slotDeletedDocument(App::Document& Doc)
{
    if (&Doc == getDocument()) {
        this->detachDocument();
    }
}

void DocumentProtector::slotCreatedObject(App::DocumentObject& Obj)
{
}

void DocumentProtector::slotDeletedObject(App::DocumentObject& Obj)
{
}

void DocumentProtector::slotChangedObject(App::DocumentObject& Obj, App::Property& Prop)
{
}

void DocumentProtector::validate()
{
    if (!this->getDocument())
        throw Base::Exception("Handled document is null");
}

App::DocumentObject *DocumentProtector::addObject(const std::string& type, const std::string& name)
{
    validate();
    DocumentReceiver::globalInstance()->postEventAndWait(new CustomAddObjectEvent(this, type, name));
    return this->obj;
}

void DocumentProtector::removeObject(const std::string& name)
{
    validate();
}

void DocumentProtector::recompute()
{
    validate();
    DocumentReceiver::globalInstance()->postEventAndWait(new CustomRecomputeEvent(this));
}
