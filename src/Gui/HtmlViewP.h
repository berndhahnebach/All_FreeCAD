/***************************************************************************
                          HtmlViewP.h  -  description
                             -------------------
    begin                : 2002/10/19 10:47:44
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file HtmlViewP.h
 *  \brief The Html view module
 *  \author Werner Mayer
 *  \version 0.1
 *  \date    2003/01/06
 */


/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *   for detail see the LICENCE text file.                                 *
 *   Werner Mayer 2002                                                     *
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

