/***************************************************************************
                          ReportView.h  -  description
                             -------------------
    begin                : 2003/08/16 10:47:44
    copyright            : (C) 2003 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file ReportView.h
 *  \brief The output window of FreeCAD
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
 

#ifndef __FC_REPORT_VIEW_H__
#define __FC_REPORT_VIEW_H__

#include "Window.h"
#include "SciEditor.h"

class FCReportOutput;
class FCReportOutputPrivate;

class FCReportView : public FCDockWindow
{ 
    Q_OBJECT

  public:
    FCReportView( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~FCReportView();

  protected:
    QTabWidget* tab;
    QGridLayout* tabLayout;
    FCReportOutput* mle;
};

class FCReportOutput : public QTextBrowser, public FCConsoleObserver
{
  Q_OBJECT

  public:
    FCReportOutput(QWidget* parent=0, const char* name=0);
    virtual ~FCReportOutput();

    void Warning(const char * s);
	  void Message(const char * s);
    void Error  (const char * s);
	  void Log (const char * s);

  protected:
    void appendLog(const char * s, const char * color = 0);
    bool event( QEvent* ev );
    void viewportMousePressEvent (QMouseEvent * e);

  public slots:
    void onClear();
    void onSaveAs();

  private:
    FCReportOutputPrivate* d;
};

#endif //__FC_REPORT_VIEW_H__
