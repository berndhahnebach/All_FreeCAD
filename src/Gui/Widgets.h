/***************************************************************************
                          FCWidgets.h  -  description
                             -------------------
    begin                : 2002/12/20 10:47:44
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file $RCSfile$
 *  \brief A set of FreeCAD widgets
 *  \author Werner Mayer
 *  \version $Revision$
 *  \date    $Date$
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
 

#ifndef __FC_WIDGETS_H__
#define __FC_WIDGETS_H__
#include "window.h"
#include <qprogressbar.h>
#include <qlabel.h>

class QHBoxLayout; 
class QTime;

class GuiExport FCFileDialog
{
  public:
    static QString getOpenFileName( const QString &initially = QString::null,
				                            const QString &filter = QString::null,
				                            QWidget *parent = 0, const char* name = 0 );
    static QString getOpenFileName( const QString &initially, const QString &filter,
				                            QWidget *parent, const char* name, const QString& caption );
    static QString getSaveFileName ( const QString & initially = QString::null, 
                                     const QString & filter = QString::null, QWidget * parent = 0, 
                                     const char * name = 0 );
    static QString getSaveFileName ( const QString & initially, const QString & filter, 
                                     QWidget * parent, const char * name, const QString & caption );
};

class FCProgressBar : public QProgressBar
{
  public:
    FCProgressBar ( QWidget * parent=0, const char * name=0, WFlags f=0 );
    void Start(QString txt, int steps, bool& flag);
    void Next();
    void Stop ();

  protected:
    virtual void keyPressEvent(QKeyEvent* e);
    virtual void drawContents( QPainter *p );
    bool setIndicator ( QString & indicator, int progress, int totalSteps );
    bool bSeveralInstances;
    int iStep;
    int iTimeStep;
    int iTotalTime;
    int iFirstTotal;
    int iStartedProgresses;
    QTime   measureTime;
    QString remainingTime;
};

#endif // __FC_WIDGETS_H__
