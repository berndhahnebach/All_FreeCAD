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

#include <qsplashscreen.h>
#include <qdialog.h>

class QHBoxLayout;
class QGridLayout;
class QLabel;
class QProgressBar;
class QPushButton;


namespace Gui {

class SplashObserver;

/** This widget provides a splash screen that can be shown  during application startup.
 *
 * \author Werner Mayer
 */
class SplashScreen : public QSplashScreen
{
public:
  SplashScreen(  const QPixmap & pixmap = QPixmap ( ), WFlags f = 0 );
  ~SplashScreen();

protected:
  void drawContents ( QPainter * painter );

private:
  QProgressBar* progBar;
  SplashObserver* messages;
};

/** This widget provides the "About dialog" of an application. 
 * This shows the current version, the build number and date. 
 * \author Werner Mayer
 */
class AboutDialog : public QDialog
{
  Q_OBJECT

public:
  AboutDialog( QWidget* parent = 0, const char* name = 0 );
  ~AboutDialog();

protected slots:
  void languageChange();

private:
  QPixmap image0;
  QLabel* pixmapLabel1;
  QPushButton* pushButton1;
  QLabel* textLabel1;
  QGridLayout* Form1Layout;
  QHBoxLayout* layout1;
};

} // namespace Gui


#endif // __SPLASHSCREEN_H__
