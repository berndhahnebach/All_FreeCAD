/***************************************************************************
                          Widgets.h  -  description
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
#include <qiconview.h>
#include <qdragobject.h>

class QHBoxLayout; 
class QTime;
class QAction;

/*  
 *  Using the Qt's open/save dialogs with own adjustments
 */
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

/*  
 *  FreeCAD's progressbar for long operations
 */
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

/*  
 *  Icon items used by the 'FCCmdView' and 'FCDlgCustomize' classes
 */
class FCCmdViewItem : public QIconViewItem
{
  public:
    FCCmdViewItem ( QIconView * parent, QAction* pAct );
    virtual ~FCCmdViewItem ();

    QString text() const;
    QAction* GetAction();

  protected:
    QAction * pAction;
    QString description;
};

/*  
 *  Icon view class
 */
class FCCmdView : public QIconView
{
  Q_OBJECT

  public:
    FCCmdView ( QWidget * parent = 0, const char * name = 0, WFlags f = 0 );
    virtual ~FCCmdView ();

  protected:
    void contentsMousePressEvent ( QMouseEvent * e );

  protected slots:
    void slotSelectionChanged(QIconViewItem * item);

  signals:
    void emitSelectionChanged(QString);
};

/*  
 *  Class to drag a 'QAction' object
 */
class FCActionDrag : public QStoredDrag
{
  public:
    FCActionDrag ( QAction* action = 0, QWidget * dragSource = 0, const char * name = 0 );
    virtual ~FCActionDrag ();

    static bool canDecode ( const QMimeSource * e );
    static bool decode ( const QMimeSource * e, QAction*  a );

  public:
    static QAction* pAction;
};

/*  
 *  Toolbar class that knows 'drag and drop'
 */
class FCToolBar : public QToolBar
{
  public:
    FCToolBar ( const QString & label, QMainWindow *, QMainWindow::ToolBarDock = QMainWindow::Top, bool newLine = FALSE, const char * name = 0 );
    FCToolBar ( const QString & label, QMainWindow *, QWidget *, bool newLine = FALSE, const char * name = 0, WFlags f = 0 );
    FCToolBar ( QMainWindow * parent = 0, const char * name = 0 );
    virtual ~FCToolBar();

  protected:
    void dropEvent ( QDropEvent * );
    void dragEnterEvent ( QDragEnterEvent * );
    void dragLeaveEvent ( QDragLeaveEvent * );
    void dragMoveEvent ( QDragMoveEvent * );
};

#endif // __FC_WIDGETS_H__
