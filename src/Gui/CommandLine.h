/***************************************************************************
                          CommandLine.h  -  description
                             -------------------
    begin                : 2002/08/06 19:52:20
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file CommandLine.h
 *  \brief The command line module
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




#ifndef __COMMAND_LINE_H__
#define __COMMAND_LINE_H__
 

#include <qlineedit.h>
#include <list>
#include <string>
#include "Window.h"
#include <qwindowsstyle.h>
#include <qvalidator.h>

/** Validate input of the command line
 */
class FCConsoleValidator : public QValidator
{
  public:
    FCConsoleValidator ( QWidget * parent, const char * name = 0 );
    virtual ~FCConsoleValidator ();

    virtual State validate ( QString &, int & ) const;
    virtual void fixup ( QString & ) const;
};

/** The windows style for the command line
 */
class FCWindowsStyle : public QWindowsStyle
{
  public:

#if QT_VER <= 230
    void drawComboButton ( QPainter * p, int x, int y, int w, int h, 
                           const QColorGroup & g, bool sunken = FALSE, 
                           bool editable = FALSE, bool enabled = TRUE, 
                           const QBrush * fill = 0 )
    {
      qDrawWinPanel(p, x, y, w, h, g, TRUE, 
              	    fill?fill:(enabled?&g.brush( QColorGroup::Base ): &g.brush( QColorGroup::Base )));

      drawArrow( p, QStyle::DownArrow, sunken, x+w-2-16+ 2, y+2+ 2, 16- 4, h-4- 4, g, enabled,
        	       fill ? fill : &g.brush( QColorGroup::Base ) );
    }

    void drawScrollBarControls ( QPainter * p, const QScrollBar * sb, int sliderStart, uint controls, uint activeControl )
    {

#define HORIZONTAL false
#define VERTICAL true
#define ADD_LINE_ACTIVE ( activeControl == AddLine )
#define SUB_LINE_ACTIVE ( activeControl == SubLine )
       
      QColorGroup g  = sb->colorGroup();

      int sliderMin, sliderMax, sliderLength, buttonDim;
      scrollBarMetrics( sb, sliderMin, sliderMax, sliderLength, buttonDim );

      if (sliderStart > sliderMax) 
      {
	      sliderStart = sliderMax;
      }

      int b = 0;
      int dimB = buttonDim;
      QRect addB;
      QRect subB;
      QRect addPageR;
      QRect subPageR;
      QRect sliderR;
      int addX, addY, subX, subY;
      int length = HORIZONTAL ? sb->width()  : sb->height();
      int extent = HORIZONTAL ? sb->height() : sb->width();

      if ( HORIZONTAL ) 
      {
	      subY = addY = ( extent - dimB ) / 2;
	      subX = b;
	      addX = length - dimB - b;
      }
      else
      {
	      subX = addX = ( extent - dimB ) / 2;
	      subY = b;
	      addY = length - dimB - b;
      }

      subB.setRect( subX,subY,dimB,dimB );
      addB.setRect( addX,addY,dimB,dimB );

      int sliderEnd = sliderStart + sliderLength;
      int sliderW = extent - b*2;
      if ( HORIZONTAL ) 
      {
	      subPageR.setRect( subB.right() + 1, b, sliderStart - subB.right() - 1 , sliderW );
	      addPageR.setRect( sliderEnd, b, addX - sliderEnd, sliderW );
	      sliderR .setRect( sliderStart, b, sliderLength, sliderW );
      }
      else 
      {
	      subPageR.setRect( b, subB.bottom() + 1, sliderW, sliderStart - subB.bottom() - 1 );
	      addPageR.setRect( b, sliderEnd, sliderW, addY - sliderEnd );
	      sliderR .setRect( b, sliderStart, sliderW, sliderLength );
      }

      bool maxedOut = (sb->maxValue() == sb->minValue());
      if ( controls & AddLine ) 
      {
//        if (sb->maxValue() != sb->value())
	        drawArrow( p, VERTICAL ? DownArrow : RightArrow, ADD_LINE_ACTIVE, addB.x()+2, addB.y()+2,
        		         addB.width()-4, addB.height()-4, g, !maxedOut );
      }
      if ( controls & SubLine ) 
      {
//        if (sb->minValue() != sb->value())
	        drawArrow( p, VERTICAL ? UpArrow : LeftArrow, SUB_LINE_ACTIVE, subB.x()+2, subB.y()+2,
         		         subB.width()-4, subB.height()-4, g, !maxedOut );
      }
    }
#elif QT_VER > 230
    void drawPrimitive( PrimitiveElement pe, QPainter * p, const QRect & r, const QColorGroup & cg, SFlags flags, const QStyleOption & opt ) const
    {
      QWindowsStyle::drawPrimitive( pe, p, r, cg, flags, opt );
    }
#endif
};

/** The command line class
 */
class GuiExport FCCommandLine : public QComboBox, public FCWindow
{
  Q_OBJECT

private:
	// Singleton
	FCCommandLine(void);
	~FCCommandLine(void);

	static FCCommandLine *_pcSingleton;

  std::vector<std::string> _astrRunCmds;

protected:
  void keyPressEvent ( QKeyEvent * e );
  void mousePressEvent ( QMouseEvent * e );
  void wheelEvent ( QWheelEvent * e );
  void enterEvent ( QEvent * );
  void leaveEvent ( QEvent * );
  QPopupMenu* CreatePopupMenu();
  /// import old commands from the preferences
  void ReadCmdList();
  /// export new commands into the preferences
  void SaveCmdList();

public:
  void SetParent(QWidget* parent);
	static void Destruct(void);
	static FCCommandLine &Instance(void);
  /// gloabal function to get command line object
	friend FCCommandLine &GetCmdLine(void); 

protected slots:
  /// clear all new commands
  void slotClearConsole();
  void slotOpenConsole();
  /// launch the command
  void slotLaunchCommand();
};

inline FCCommandLine &GetCmdLine(void)
{
  return FCCommandLine::Instance();
}

#endif // __COMMAND_LINE_H__
