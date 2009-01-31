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

#include <QObject>
#include <string>

namespace App {
    class Document;
    class DocumentObject;
}

namespace Sandbox {

class SandboxAppExport DocumentProtector
{
public:
    DocumentProtector(App::Document*);
    ~DocumentProtector();

    App::DocumentObject *addObject(const std::string& type, const std::string& name="");
    void removeObject(const std::string& name);
    void recompute();

private:
    App::Document* doc;
    App::DocumentObject* obj;

    // friends
    friend class DocumentReceiver;
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
    void wakeupThread();
    void postEventAndWait(QEvent*);

    // friends
    friend class DocumentProtector;
};

}

#endif // SANDBOX_DOCUMENTPROTECTOR_H

