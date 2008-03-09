/***************************************************************************
 *   Copyright (c) 2006 Werner Mayer <werner.wm.mayer@gmx.de>              *
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


#include <QtDesigner/QDesignerContainerExtension>
#include <QtDesigner/QDesignerCustomWidgetCollectionInterface>

#include <QtCore/qplugin.h>
#include <QtGui/QIcon>

class QDesignerFormEditorInterface;

class CustomWidgetPlugin : public QObject, public QDesignerCustomWidgetCollectionInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetCollectionInterface)
public:
    CustomWidgetPlugin(QObject *parent = 0);
    QList<QDesignerCustomWidgetInterface *> customWidgets () const;
};

class WizardPlugin : public QObject, 
                     public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)
public:
    WizardPlugin();
    QWidget *createWidget(QWidget *parent);
    
    QString group() const;
    QIcon icon() const;
    QString includeFile() const;
    QString toolTip() const;
    QString whatsThis() const;
    bool isContainer() const;
    QString name() const;
    bool isInitialized() const;
    void initialize(QDesignerFormEditorInterface *formEditor);
    QString domXml() const;

private Q_SLOTS:
    void currentIndexChanged(int index);
    void pageTitleChanged(const QString &title);

private:
    bool initialized;
};
