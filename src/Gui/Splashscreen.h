/***************************************************************************
                          Splashscreen.h  -  description
                             -------------------
    begin                : 2002/08/19 21:11:52
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file Splashscreen.h
 *  \brief The splash screen module
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




#ifndef __SPLASHSCREEN_H__
#define __SPLASHSCREEN_H__

#include "Window.h"
#include <qvariant.h>
#include <qlabel.h>
#include <qthread.h>
#include <qtextbrowser.h>

class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QLabel;
class QProgressBar;
class QTextView;

/**
 * text browser for the help dialog
 */
class FCSplashBrowser : public QTextBrowser
{
  Q_OBJECT

  public:
    FCSplashBrowser(QWidget * parent=0, const char * name=0);
    void setSource ( const QString & name );

  signals:
    void linkClicked(const QString& txt);
};

/**
 * splash widget base class
 */
class GuiExport FCSplashWidget : public QLabel, public QThread
{
  Q_OBJECT

  public:
    FCSplashWidget( QWidget* parent = 0, const char* name = 0, WFlags f = 0 );
    ~FCSplashWidget();

    QString SplasherText;
    bool bRun;

  protected:
    /// implement this method in inherited classes
    virtual void run() = 0;
    virtual QString getName() = 0;
    void hideEvent ( QHideEvent * e);
    std::map<std::string, std::pair<std::string, QPixmap> > _aclDevelopers;

  protected slots:
    void aboutToQuit();
};

// splasher at startup
class GuiExport FCSplashScreen : public FCSplashWidget
{ 
  Q_OBJECT

  public:
    FCSplashScreen( QWidget* parent = 0, const char* name = 0 );
    ~FCSplashScreen();

  protected:
    /// run the thread
    virtual void run();
    virtual QString getName();
    QVBoxLayout* SplasherDialogLayout;
    QLabel* PixmapLabel;
    QTextView* SplasherTextView;
    QProgressBar* SplasherProgress;
};

// splasher for the help dialog
class GuiExport FCSplashAbout : public FCSplashWidget
{ 
  Q_OBJECT

  public:
	  static void Destruct(void);
  	static FCSplashAbout* Instance(void);
    void setFCPixmap(const QPixmap& image0);

  private:
    FCSplashAbout( QWidget* parent = 0, const char* name = 0 );
    ~FCSplashAbout();
    static FCSplashAbout* _pclSingleton;

  protected:
    /// run the thread
    virtual void run();
    virtual QString getName();
    void keyPressEvent ( QKeyEvent * e );
    void hideEvent ( QHideEvent * e);
    QVBoxLayout* SplasherDialogLayout;
    QPushButton* ButtonOK;
    QLabel* RemainingTime;
    QLabel* PixmapLabel;
    FCSplashBrowser* SplasherTextView;

  protected slots:
    void clicked();
    void linkClicked(const QString& txt);
};

#endif // __SPLASHSCREEN_H__
