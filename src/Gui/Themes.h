#ifndef __THEMES_H__
#define __THEMES_H__


#if QT_VERSION <= 230
class NorwegianWoodStyle : public QWindowsStyle
{
  Q_OBJECT

public:
    NorwegianWoodStyle( int sbext = -1 );
    void polish( QApplication*);
    void polish( QWidget* );
    void unPolish( QWidget* );
    void unPolish( QApplication*);

    void drawButton( QPainter *p, int x, int y, int w, int h,
			     const QColorGroup &g, bool sunken = FALSE,
			     const QBrush *fill = 0 );
    void drawBevelButton( QPainter *p, int x, int y, int w, int h,
			  const QColorGroup &g, bool sunken = FALSE,
			  const QBrush *fill = 0 );
    QRect buttonRect( int x, int y, int w, int h);
    void drawButtonMask( QPainter *p, int x, int y, int w, int h);
    void drawComboButton( QPainter *p, int x, int y, int w, int h,
				    const QColorGroup &g,
				    bool /* sunken */,
				    bool editable,
				    bool /*enabled */,
				    const QBrush *fb );


    void drawPushButton( QPushButton* btn, QPainter *p);
    void drawPushButtonLabel( QPushButton* btn, QPainter *p);
    void drawScrollBarControls( QPainter*,  const QScrollBar*, int sliderStart, uint controls, uint activeControl );

private:
    void drawSemicircleButton(QPainter *p, const QRect &r, int dir, 
			      bool sunken, const QColorGroup &g );
    QPalette oldPalette;
    QPixmap *sunkenDark;
    QPixmap *sunkenLight;

};


class MetalStyle : public QWindowsStyle
{
  Q_OBJECT
public:
    MetalStyle();
    void polish( QApplication*);
    void unPolish( QApplication*);
    void polish( QWidget* );
    void unPolish( QWidget* );

    
    void drawMetalButton( QPainter *p, int x, int y, int w, int h,
			  bool sunken = FALSE, bool horz = TRUE );
    
    
    void drawButton( QPainter *p, int x, int y, int w, int h,
			     const QColorGroup &g, bool sunken = FALSE,
			     const QBrush *fill = 0 );
    void drawBevelButton( QPainter *p, int x, int y, int w, int h,
			  const QColorGroup &g, bool sunken = FALSE,
			  const QBrush *fill = 0 );

    void drawPushButton( QPushButton* btn, QPainter *p);
    void drawPushButtonLabel( QPushButton* btn, QPainter *p);
    void drawPanel( QPainter *p, int x, int y, int w, int h,
		    const QColorGroup &, bool sunken,
		    int lineWidth, const QBrush *fill );

    
    void drawSlider( QPainter *p,
		     int x, int y, int w, int h,
		     const QColorGroup &g,
		     Orientation orient, bool tickAbove, bool tickBelow );
    
    void drawScrollBarControls( QPainter* p, const QScrollBar* sb,
				int sliderStart, uint controls,
				uint activeControl );

    
    void drawComboButton( QPainter *p, int x, int y, int w, int h,
			  const QColorGroup &g, bool sunken = FALSE,
			  bool editable = FALSE,
			  bool enabled = TRUE,
			  const QBrush *fill = 0 );

private:
    QPalette oldPalette;
};

#endif


/***************************************************************************
  version 0.1
  -------------------
  description   A Qt style that attempts to imitate the look and
                feel of the NeXT platform. However, the imitation
                deviates from NeXT in several areas, so it is up
                to the user to implement an appropriate layout
                for the best effect.
  -------------------
  begin         Tue Feb 15 2000
  author        David Johnson <david@usermode.org>
  -------------------
  Copyright 2000, David Johnson
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  1. Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

  3. Neither name of the copyright holders nor the names of its contributors may
  be used to endorse or promote products derived from this software without
  specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ***************************************************************************/

/**
  * A Qt style that attempts to imitate the look and feel of the NeXT platform.
  * However, the imitation deviates from NeXT in several areas, so it is up
  * to the user to implement an appropriate layout for the best effect.
  *
  * This class uses the same API as the QStyle family of classes. Fuller
  * descriptions of these methods can be obtained from the Qt documentation.
  */

class StepStyle : public QWindowsStyle
{
  Q_OBJECT
public:
    /** Constructor */
    StepStyle();
    /** Destructor */
    virtual ~StepStyle();

    /** Initialize the appearance of the style */
    void polish(QPalette& pal);
    void polish(QWidget *w);
    void polish(QApplication *a);
    /** Restores the appearance of the style */
    void unPolish(QPalette& pal);
    void unPolish(QWidget *w);
    void unPolish(QApplication *a);

    /** Draws the basic button */
    void drawButton(QPainter *p, int x, int y, int w, int h,
                const QColorGroup &g, bool sunken = false, const QBrush *fill = 0);
    /** Draws a beveled button */
    void drawBevelButton(QPainter *p, int x, int y, int w, int h,
                const QColorGroup &g, bool sunken = false, const QBrush *fill = 0);
    /** Draws a push button */
    void drawPushButton(QPushButton* b, QPainter *p);
    /** Draws the label for a pushbutton */
    void drawPushButtonLabel(QPushButton* b, QPainter *p);
    /** Shift button contents when pressed */
    void getButtonShift(int &x, int &y);

    /** Draws a combo button */
    void drawComboButton(QPainter *p, int x, int y, int w, int h,
                const QColorGroup &g, bool sunken = false, bool editable = false,
                bool enabled = true, const QBrush *fill = 0);
    /** Returns the rectangle for the contents of a combo box */
    QRect comboButtonRect(int x, int y, int w, int h);
    /** Returns the focus rectangle for a combo box */
    QRect comboButtonFocusRect(int x, int y, int w, int h);

    /** Draws a checkbox indicator */
    void drawIndicator(QPainter* p, int x, int y, int w, int h, const QColorGroup &g,
                int state, bool down = true, bool enabled = true);
    /** Returns the size of a checkbox */
    QSize indicatorSize() const;

    /** Draws a radio button indicator */
    void drawExclusiveIndicator(QPainter* p,  int x, int y, int w, int h, const QColorGroup &g,
                bool on, bool down = false, bool enabled = true);
    /** Returns the size of a radio button indicator */
    QSize exclusiveIndicatorSize() const;
	
    /** Draws a scrollbar */
    void drawScrollBarControls(QPainter*, const QScrollBar*, int sliderStart, uint controls,
                uint activeControl);
    /** Returns the metrics of a scrollbar */
    void scrollBarMetrics(const QScrollBar*, int&, int&, int&, int&);
    /** Returns the scrollbar control the mouse is over */
    ScrollControl scrollBarPointOver(const QScrollBar* sb, int sliderStart, const QPoint& p);

    /** Draws a slider 'knob' */
    void drawSlider(QPainter *p, int x, int y, int w, int h,
                const QColorGroup &g, Orientation, bool tickabove, bool tickbelow);
    /** Returns the length of a slider */
    int sliderLength() const;
    /** Draws the slider 'groove' */
    void drawSliderGroove(QPainter *p, int x, int y, int w, int h,
                const QColorGroup& g, QCOORD c, Orientation);
    /** Distance the mouse can move before the slider "pops' back */
    int maximumSliderDragDistance() const;

#ifdef INCLUDE_MENUITEM_DEF
    /** Draws a menu item */
    void drawPopupMenuItem(QPainter* p, bool checkable, int maxpmw, int tab, QMenuItem* mi,
                const QPalette& pal, bool act, bool enabled, int x, int y, int w, int h);
#endif

    /** Draws a tabbar tab */
    void drawTab(QPainter* p, const QTabBar* tb, QTab* t, bool selected);

    /** Draws a splitter */
    void drawSplitter(QPainter *p, int x, int y, int w, int h, const QColorGroup &g, Orientation);
    /** Returns the width of a splitter */
    int splitterWidth() const;

    /** Draws a panel */
    void drawPanel(QPainter *p, int x, int y, int w, int h,
                const QColorGroup &cg, bool sunken, int lineWidth, const QBrush *fill);

    /** Draws a focus rectangle */
    void drawFocusRect(QPainter* p, const QRect& r, const QColorGroup &g, const QColor* bg, bool atBorder);

protected:
    /** Draws a NeXT style arrow */
    void drawArrow(QPainter *p, Qt::ArrowType type, bool down, int x,
                int y, int w, int h, const QColorGroup &g, bool enabled, const QBrush *);
    /** Draws a NeXT style 'dot' */
    void drawCircle(QPainter* p,  int x, int y, int w, int h, const QColorGroup &g);
    /** Draws the background of the scrollbar */
    void drawScrollBarGroove(QPainter *p, QRect r, const QWidget *sb, const QColorGroup &g);
    /** Draws a checkmark */
    void drawCheckMark(QPainter *p, int x, int y, int w, int h, const QColorGroup &g, bool act, bool dis);

private:
    QPalette savePalette;   // save the palette during polish
    QPalette pal;           // a working palette
};



#endif
