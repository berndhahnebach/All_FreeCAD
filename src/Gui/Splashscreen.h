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

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLabel;
class QProgressBar;
class QTextView;
class QPushButton;
class QCheckBox;
class FCSplashObserver;

/** Splasher at startup */
class FCSplashScreen : public QSplashScreen
{
  public:
    FCSplashScreen(  const QPixmap & pixmap = QPixmap ( ), WFlags f = 0 );
    ~FCSplashScreen();

  protected:
		void drawContents ( QPainter * painter );

    QProgressBar* progBar;
		FCSplashObserver* messages;
};

/** About dialog */
class AboutDlg : public QDialog
{
	Q_OBJECT

	public:
		AboutDlg( QWidget* parent = 0, const char* name = 0 );
		~AboutDlg();

		QLabel* pixmapLabel1;
		QPushButton* pushButton1;
		QLabel* textLabel1;

	protected:
		QGridLayout* Form1Layout;
		QHBoxLayout* layout1;

	protected slots:
		void languageChange();

	private:
		QPixmap image0;
};

#endif // __SPLASHSCREEN_H__
