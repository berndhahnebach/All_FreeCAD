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


#ifndef SANDBOX_DOCUMENTPROTECTOR_H
#define SANDBOX_DOCUMENTPROTECTOR_H

#include <string>
#include <boost/signals.hpp>
#include <boost/bind.hpp>
#include <App/DocumentObserver.h>

namespace App {
    class Document;
    class DocumentObject;
}

namespace Sandbox {

class SandboxAppExport DocumentProtector : public App::DocumentObserver
{
public:
    DocumentProtector(App::Document*);
    ~DocumentProtector();

    static void init();

    App::DocumentObject *addObject(const std::string& type, const std::string& name="");
    void removeObject(const std::string& name);
    void recompute();

private:
    /** Checks if a new document was created */
    void slotCreatedDocument(App::Document& Doc);
    /** Checks if the given document is about to be closed */
    void slotDeletedDocument(App::Document& Doc);
    /** Checks if a new object was added. */
    void slotCreatedObject(App::DocumentObject& Obj);
    /** Checks if the given object is about to be removed. */
    void slotDeletedObject(App::DocumentObject& Obj);
    /** The property of an observed object has changed */
    void slotChangedObject(App::DocumentObject& Obj, App::Property& Prop);
    void validate();

private:
    App::DocumentObject* obj;

    // friends
    friend class CustomDocumentProtectorEvent;
    friend class CustomAddObjectEvent;
    friend class CustomRemoveObjectEvent;
    friend class CustomRecomputeEvent;
};

}

#endif // SANDBOX_DOCUMENTPROTECTOR_H

