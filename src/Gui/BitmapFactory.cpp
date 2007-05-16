/***************************************************************************
 *   Copyright (c) 2004 Jürgen Riegel <juergen.riegel@web.de>              *
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

#include <Base/Console.h>
#include <App/Application.h>

#include "BitmapFactory.h"
#include "Tools.h"
#include "Icons/images.cpp"
#include "Icons/Feature.xpm"
#include "Icons/Icons.h"
#include "Icons/Document.xpm"
#include "Icons/BmpFactoryIcons.cpp"

using namespace Gui;

namespace Gui {
class BitmapFactoryInstP
{
public:
    QMap<QString, const char**> xpmMap;
    QMap<QString, QPixmap> xpmCache;
    QStringList paths;
};
}

BitmapFactoryInst* BitmapFactoryInst::_pcSingleton = NULL;

BitmapFactoryInst& BitmapFactoryInst::instance(void)
{
    if (_pcSingleton == NULL)
    {
        _pcSingleton = new BitmapFactoryInst;
        _pcSingleton->addPath("../Icons");
        _pcSingleton->addPath(App::GetApplication().GetHomePath());

        RegisterIcons();
    }

    return *_pcSingleton;
}

void BitmapFactoryInst::destruct (void)
{
    if (_pcSingleton != 0)
    delete _pcSingleton;
    _pcSingleton = 0;
}

BitmapFactoryInst::BitmapFactoryInst()
{
    d = new BitmapFactoryInstP;
}

BitmapFactoryInst::~BitmapFactoryInst()
{
    delete d;
}

void BitmapFactoryInst::addPath(const char* sPath)
{
    d->paths.push_back( sPath );
}

void BitmapFactoryInst::removePath(const char* sPath)
{
    d->paths.erase( d->paths.find( sPath ) );
}

void BitmapFactoryInst::addXPM(const char* sName, const char** pXPM)
{
    d->xpmMap[sName] = pXPM;
}

void BitmapFactoryInst::addXPM(const char* sName, const QPixmap& pXPM)
{
    d->xpmCache[sName] = pXPM;
}

bool BitmapFactoryInst::hasXPM(const char* sName) const
{
    if (d->xpmMap.find(sName) != d->xpmMap.end())
        return true;
    if (d->xpmCache.find(sName) != d->xpmCache.end())
        return true;
    return false;
}

QPixmap BitmapFactoryInst::pixmap(const char* sName) const
{
    // as very first test check whether the pixmap is in the cache
    QMap<QString, QPixmap>::ConstIterator it = d->xpmCache.find(sName);
    if (it != d->xpmCache.end())
        return it.value();

    // now try to find it in the built-in XPM
    QPixmap icon;
    QMap<QString,const char**>::ConstIterator It = d->xpmMap.find(sName);
    if (It != d->xpmMap.end())
        icon = QPixmap(It.value());

    // If an absolute path is given
    if (icon.isNull() && QFile(sName).exists())
        icon.load(sName);

    // try to find it in the given directories
    if (icon.isNull()) {
        QList<QByteArray> formats = QImageReader::supportedImageFormats();
        for (QStringList::ConstIterator pt = d->paths.begin(); pt != d->paths.end(); ++pt) {
            QDir d(*pt);
            QString fileName = d.path() + QDir::separator() + sName;
            if (QFile(fileName).exists()) {
                icon.load(fileName);
                break;
            } else {
                for (QList<QByteArray>::iterator fm = formats.begin(); fm != formats.end(); ++fm) {
                    QString path = fileName + "." + QString((*fm).lower().data());
                    if (QFile(path).exists()) {
                        icon.load(path);
                        break;
                    }
                }
            }
        }
    }

    if (!icon.isNull()) {
        d->xpmCache[sName] = icon;
        return icon;
    }

    Base::Console().Warning("Can't find Pixmap:%s\n",sName);
    return QPixmap(px);
}

QPixmap BitmapFactoryInst::pixmap(const char* sName, const char* sMask, Position pos) const
{
    QPixmap p1 = pixmap(sName);
    QPixmap p2 = pixmap(sMask);

    int x = 0, y = 0;

    switch (pos)
    {
    case Qt::TopLeftCorner:
        break;
    case Qt::TopRightCorner:
        x = p1.width () - p2.width ();
        break;
    case Qt::BottomLeftCorner:
        y = p1.height() - p2.height();
        break;
    case Qt::BottomRightCorner:
        x = p1.width () - p2.width ();
        y = p1.height() - p2.height();
        break;
    }

    QPixmap p = p1;
    p = Tools::fillOpaqueRect(x, y, p2.width(), p2.height(), p);

    QPainter pt;
    pt.begin( &p );
    pt.setPen(Qt::NoPen);
    pt.drawRect(x, y, p2.width(), p2.height());
    pt.drawPixmap(x, y, p2);
    pt.end();

    return p;
}

QStringList BitmapFactoryInst::pixmapNames() const
{
    QStringList names;
    for ( QMap<QString,const char**>::ConstIterator It = d->xpmMap.begin(); It != d->xpmMap.end(); ++It )
        names << It.key();
    return names;
}
