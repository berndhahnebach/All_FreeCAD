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
