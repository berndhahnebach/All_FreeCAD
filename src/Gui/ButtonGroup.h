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


#ifndef __BUTTON_GROUP_H__
#define __BUTTON_GROUP_H__

#ifndef _PreComp_
# include <qbuttongroup.h>
#endif

class QScrollView;
class QPopupMenu;

namespace Gui {
namespace DockWnd {

/** The ButtonGroup widget organizes buttons in a group.
 * \author Werner Mayer
 */
class GuiExport ButtonGroup : public QButtonGroup
{
  Q_OBJECT

public:
  ButtonGroup(QWidget * parent=0, const char * name=0);
  ButtonGroup(const QString & title, QWidget * parent=0, const char * name=0);
  ButtonGroup(int strips, Orientation o, QWidget * parent=0, const char * name=0);
  ButtonGroup(int strips, Orientation o, const QString & title, QWidget * parent=0, const char * name=0);
  virtual ~ButtonGroup(void);

protected:
  void resizeColumns(void);
  void resizeEvent (QResizeEvent * e);
  void mousePressEvent( QMouseEvent * );

protected slots:
  void onPopupMenuAboutToShow();
  void onSetNewBackgroundColor();
  void onResetBackgroundColor();
  void onShowIcons();
  void onShowText();

private:
  /// initialize the button group
  void initialize(void);

private:
  int m_iChildWidth;
  int m_iChildHeight;
  int m_iWidth, m_iHeight;
  QColor       m_Color;
  QPopupMenu*  m_Popup;
  std::map<int, QPixmap> m_Pixmaps;
  QScrollView *pScrollWidget;
};

} // namespace DockWnd
} // namespace Gui

#endif // __BUTTON_GROUP_H__
