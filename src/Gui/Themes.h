#ifndef __THEMES_H__
#define __THEMES_H__

#include <qwindowsstyle.h>
#include <qpalette.h>

#if QT_VERSION <= 230
class NorwegianWoodStyle : public QWindowsStyle
{
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
#endif
