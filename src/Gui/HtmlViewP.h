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


#ifndef __HTML_VIEW_P_H__
#define __HTML_VIEW_P_H__

class FCWhatsThisPrivate: public QObject
{
  Q_OBJECT

public:

struct FCWhatsThisItem : public QShared
  {
    FCWhatsThisItem() : QShared() { whatsthis = 0; }
    ~FCWhatsThisItem() {}
    QString txt;
    FCWhatsThis* whatsthis;
  };

  enum TMode { Inactive, Active };

  // create and clear the WhatsThis object
  static void clearWhatsThis();
  static void createWhatsThis();

  FCWhatsThisPrivate();
  ~FCWhatsThisPrivate();

  bool eventFilter( QObject *, QEvent * );

  FCWhatsThisItem* item( QWidget * widget );
  void add( QWidget * widget, FCWhatsThis* special );
  void add( QWidget * widget, const QString& text );

  // show the help
  void showWhatsThis( QWidget *, const QString&, const QPoint&  );
  void leaveWhatsThisMode();

  // members
  std::map<QWidget*, FCWhatsThisItem*> mWidgetItem;
  std::map<QWidget*, QWidget*> topLevelWidget;
  TMode mode;

  QCursor * cursor;
  QString currentText;

private slots:
  void removeWidget()
  {
    const QObject* o = sender();
    if ( o->isWidgetType() )
      FCWhatsThis::remove((QWidget*)o);
  }
};

#endif //__HTML_VIEW_P_H__

