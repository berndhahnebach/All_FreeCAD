#include "../Config.h"
#ifdef _PreComp_
#	include "PreCompiled.h"
#else
#	include "qapplication.h"
#	include "qpainter.h"
#	include "qdrawutil.h" // for now
#	include "qpixmap.h" // for now
#	include "qpalette.h" // for now
#	include "qwidget.h"
#	include "qlabel.h"
#	include "qimage.h"
#	include "qpushbutton.h"
#	include "qwidget.h"
#	include "qrangecontrol.h"
#	include "qscrollbar.h"
#	include <limits.h>
#endif




#include "Themes.h"




/////////////////////////////////////////////////////////
#include "Icons/stonedark.xpm"
#include "Icons/stone1.xpm"
#include "Icons/marble.xpm"
#include "Icons/polish.xpm"
#include "Icons/button.xpm"
///////////////////////////////////////////////////////



MetalStyle::MetalStyle() : QWindowsStyle() { }

/*!
  Reimplementation from QStyle
 */
void MetalStyle::polish( QApplication *app)
{
    oldPalette = app->palette();

    // we simply create a nice QColorGroup with a couple of fancy
    // pixmaps here and apply to it all widgets

    QFont f("times", app->font().pointSize() );
    f.setBold( TRUE );
    f.setItalic( TRUE );
    app->setFont( f, TRUE, "QMenuBar");
    app->setFont( f, TRUE, "QPopupMenu");


    //QPixmap button( stone1_xpm );
    QPixmap button( stonedark_xpm );
    QPixmap background(marble_xpm);
#if 0

    int i;
    for (i=0; i<img.numColors(); i++) {
	QRgb rgb = img.color(i);
	QColor c(rgb);
	rgb = c.dark().rgb();
	img.setColor(i,rgb);
    }
    QPixmap mid;
    mid.convertFromImage(img);

    img = orig;
    for (i=0; i<img.numColors(); i++) {
	QRgb rgb = img.color(i);
	QColor c(rgb);
	rgb = c.light().rgb();
	img.setColor(i,rgb);
    }
    QPixmap light;
    light.convertFromImage(img);

    img = orig;
    for (i=0; i<img.numColors(); i++) {
	QRgb rgb = img.color(i);
	QColor c(rgb);
	rgb = c.dark().rgb();
	img.setColor(i,rgb);
    }
    QPixmap dark;
    dark.convertFromImage(img);
#else
    QPixmap dark( 1, 1 ); dark.fill( red.dark() );
    QPixmap mid( stone1_xpm );
    QPixmap light( stone1_xpm );//1, 1 ); light.fill( green );
#endif
    QPalette op = app->palette();

    QColor backCol( 227,227,227 );

    // QPalette op(white);
    QColorGroup active (op.normal().foreground(),
		     QBrush(op.normal().button(),button),
		     QBrush(op.normal().light(), light),
		     QBrush(op.normal().dark(), dark),
		     QBrush(op.normal().mid(), mid),
		     op.normal().text(),
		     Qt::white,
		     op.normal().base(),//		     QColor(236,182,120),
		     QBrush(backCol, background)
		     );
    active.setColor( QColorGroup::ButtonText,  Qt::white  );
    active.setColor( QColorGroup::Shadow,  Qt::black  );
    QColorGroup disabled (op.disabled().foreground(),
		     QBrush(op.disabled().button(),button),
		     QBrush(op.disabled().light(), light),
		     op.disabled().dark(),
		     QBrush(op.disabled().mid(), mid),
		     op.disabled().text(),
		     Qt::white,
		     op.disabled().base(),//		     QColor(236,182,120),
		     QBrush(backCol, background)
		     );

    QPalette newPalette( active, disabled, active );
    app->setPalette( newPalette, TRUE );
}

/*!
  Reimplementation from QStyle
 */
void MetalStyle::unPolish( QApplication *app)
{
    app->setPalette(oldPalette, TRUE);
    app->setFont( app->font(), TRUE );
}

/*!
  Reimplementation from QStyle
 */
void MetalStyle::polish( QWidget* w)
{

   // the polish function sets some widgets to transparent mode and
    // some to translate background mode in order to get the full
    // benefit from the nice pixmaps in the color group.

    if (w->inherits("QPushButton")){
	w->setBackgroundMode( QWidget::NoBackground );
	return;
    }

    if (w->inherits("QTipLabel") || w->inherits("QLCDNumber") ){
	return;
    }

    if ( !w->isTopLevel() ) {
	if ( w->inherits("QGroupBox")
	     || w->inherits("QTabWidget") ) {
	    w->setAutoMask( TRUE );
	    return;
	}
 	if (w->inherits("QLabel")
 	    || w->inherits("QSlider")
 	    || w->inherits("QButton")
	    || w->inherits("QProgressBar")
	    ){
	    w->setBackgroundOrigin( QWidget::ParentOrigin );
 	}
    }
}

void MetalStyle::unPolish( QWidget* w)
{

   // the polish function sets some widgets to transparent mode and
    // some to translate background mode in order to get the full
    // benefit from the nice pixmaps in the color group.

    if (w->inherits("QPushButton")){
	w->setBackgroundMode( QWidget::PaletteButton );
	return;
    }

    if (w->inherits("QTipLabel") || w->inherits("QLCDNumber") ){
	return;
    }

    if ( !w->isTopLevel() ) {
	if ( w->inherits("QGroupBox")
	     || w->inherits("QTabWidget") ) {
	    w->setAutoMask( FALSE );
	    return;
	}
 	if (w->inherits("QLabel")
 	    || w->inherits("QSlider")
 	    || w->inherits("QButton")
	    || w->inherits("QProgressBar")
	    ){
	    w->setBackgroundOrigin( QWidget::WidgetOrigin );
 	}
    }

}

/*!
  Draw a metallic button, sunken if \a sunken is TRUE, horizontal if 
  /a horz is TRUE.
*/

void MetalStyle::drawMetalButton( QPainter *p, int x, int y, int w, int h,
  bool sunken, bool horz )
{
    QColor top1("#878769691515");
    QColor top2("#C6C6B4B44949");
    
    QColor bot2("#70705B5B1414");
    QColor bot1("56564A4A0E0E"); //first from the bottom
    
    QColor highlight("#E8E8DDDD6565");
    QColor subh1("#CECEBDBD5151");
    QColor subh2("#BFBFACAC4545");
    
    QColor topgrad("#B9B9A5A54040");
    QColor botgrad("#89896C6C1A1A");
    

    int x2 = x + w - 1;
    int y2 = y + h - 1;
    
    //frame:
    
    p->setPen( top1 );
    p->drawLine( x, y2, x, y );
    p->drawLine( x, y, x2-1, y );
    p->setPen( top2 );
    p->drawLine( x+1, y2 -1, x+1, y+1 );
    p->drawLine( x+1, y+1 , x2-2, y+1 );

    p->setPen( bot1 );
    p->drawLine( x+1, y2, x2, y2 );
    p->drawLine( x2, y2, x2, y );
    p->setPen( bot2 );
    p->drawLine( x+1, y2-1, x2-1, y2-1 );
    p->drawLine( x2-1, y2-1, x2-1, y+1 );

    // highlight:
    int i = 0;
    int x1 = x + 2;
    int y1 = y + 2;
    if ( horz ) 
	x2 = x2 - 2; 
    else
	y2 = y2 - 2;
    // Note that x2/y2 mean something else from this point down...

#define DRAWLINE if (horz) \
                    p->drawLine( x1, y1+i, x2, y1+i ); \
		 else \
                    p->drawLine( x1+i, y1, x1+i, y2 ); \
                 i++; 
    
    if ( !sunken ) {
	p->setPen( highlight );
	DRAWLINE;
	DRAWLINE;
	p->setPen( subh1 );
	DRAWLINE;
	p->setPen( subh2 );
	DRAWLINE;
    }
    // gradient:
    int ng = (horz ? h : w) - 8; // how many lines for the gradient?
    
    int h1, h2, s1, s2, v1, v2;
    if ( !sunken ) {
	topgrad.hsv( &h1, &s1, &v1 );
	botgrad.hsv( &h2, &s2, &v2 );
    } else {
	botgrad.hsv( &h1, &s1, &v1 );
	topgrad.hsv( &h2, &s2, &v2 );
    }
    
    if ( ng > 1 ) {
	
	for ( int j =0; j < ng; j++ ) {
	    p->setPen( QColor( h1 + ((h2-h1)*j)/(ng-1), 
			       s1 + ((s2-s1)*j)/(ng-1), 
			       v1 + ((v2-v1)*j)/(ng-1),  QColor::Hsv ) ); 
	    DRAWLINE;
	}
    } else if ( ng == 1 ) {
	p->setPen( QColor( (h1+h2)/2, (s1+s2)/2, (v1+v2)/2, QColor::Hsv ) ); 
	DRAWLINE;
    }
    if ( sunken ) {
	p->setPen( subh2 );
	DRAWLINE;
	
	p->setPen( subh1 );
	DRAWLINE;

	p->setPen( highlight );
	DRAWLINE;
	DRAWLINE;
    }
    
}



/*!
  Reimplementation from QStyle
 */
void MetalStyle::drawButton( QPainter *p, int x, int y, int w, int h,
			     const QColorGroup &, bool sunken, const QBrush*)
{

    drawMetalButton( p, x, y, w, h, sunken, TRUE );
}

/*!
  Reimplementation from QStyle
 */
void MetalStyle::drawBevelButton( QPainter *p, int x, int y, int w, int h,
				const QColorGroup &, bool sunken, const QBrush*)
{
    MetalStyle::drawMetalButton(p, x, y, w, h, sunken, TRUE );
}

/*!
  Reimplementation from QStyle
 */
void MetalStyle::drawPushButton( QPushButton* btn, QPainter *p)
{
    QColorGroup g = btn->colorGroup();
    int x1, y1, x2, y2;

    btn->rect().coords( &x1, &y1, &x2, &y2 );	// get coordinates

    p->setPen( g.foreground() );
    p->setBrush( QBrush(g.button(),NoBrush) );

    QBrush fill;
    if ( btn->isDown() )
	fill = g.brush( QColorGroup::Mid );
    else if ( btn->isOn() )
	fill = QBrush( g.mid(), Dense4Pattern );
    else
	fill = g.brush( QColorGroup::Button );	

    if ( btn->isDefault() ) {
	QPointArray a;
	a.setPoints( 9,
		     x1, y1, x2, y1, x2, y2, x1, y2, x1, y1+1,
		     x2-1, y1+1, x2-1, y2-1, x1+1, y2-1, x1+1, y1+1 );
	p->setPen( Qt::black );
	p->drawPolyline( a );
	x1 += 2;
	y1 += 2;
	x2 -= 2;
	y2 -= 2;
    }
	
    drawMetalButton( p, x1, y1, x2-x1+1, y2-y1+1, btn->isOn() || btn->isDown(),
		     TRUE ); // always horizontal
	

    if ( btn->isMenuButton() ) {
	int dx = (y1-y2-4)/3;
	drawArrow( p, DownArrow, FALSE,
		   x2 - dx, dx, y1, y2 - y1,
		   g, btn->isEnabled() );
    }

    if ( p->brush().style() != NoBrush )
	p->setBrush( NoBrush );

}


/*!
  Reimplementation from QStyle
 */
void MetalStyle::drawPushButtonLabel( QPushButton* btn, QPainter *p)
{
    QRect r = btn->rect();
    int x, y, w, h;
    r.rect( &x, &y, &w, &h );

    int x1, y1, x2, y2;
    btn->rect().coords( &x1, &y1, &x2, &y2 );	// get coordinates
    int dx = 0;
    int dy = 0;
    if ( btn->isMenuButton() )
	dx = (y2-y1) / 3;
    if ( btn->isOn() || btn->isDown() ) {
	dx--;
	dy--;
    }
    if ( dx || dy )
	p->translate( dx, dy );

    x += 2;  y += 2;  w -= 4;  h -= 4;
    QColorGroup g = btn->colorGroup();
    drawItem( p, x, y, w, h,
	      AlignCenter|ShowPrefix,
	      g, btn->isEnabled(),
	      btn->pixmap(), btn->text(), -1,
	      (btn->isDown() || btn->isOn())?&btn->colorGroup().brightText():&btn->colorGroup().buttonText());

    if ( dx || dy )
	p->translate( -dx, -dy );
}


void MetalStyle::drawPanel( QPainter *p, int x, int y, int w, int h,
			    const QColorGroup &g, bool sunken,
			    int lineWidth, const QBrush *fill )
{

    QStyle::drawPanel( p,  x,  y,  w,  h,
			    g, sunken,
			    lineWidth, fill );
}


/*!
  Reimplemented
*/

void MetalStyle::drawSlider( QPainter *p, int x, int y, int w, int h,
			     const QColorGroup &, Orientation orient, 
			     bool /*tickAbove*/, bool /*tickBelow*/ )
{
    drawMetalButton( p, x, y, w, h, FALSE, orient != Horizontal );
}


void MetalStyle::drawScrollBarControls( QPainter* p, const QScrollBar* sb,
					int sliderStart, uint controls,
					uint activeControl )
{
    QWindowsStyle::drawScrollBarControls( p, sb, sliderStart, controls & ~(AddLine|SubLine|Slider),
					activeControl & ~(AddLine|SubLine|Slider) );
    bool horz = sb->orientation() == QScrollBar::Horizontal;
    int b = 2;
    int w = horz ? sb->height() : sb->width();

    QColorGroup g = sb->colorGroup();

    int sliderMin, sliderMax, sliderLength, buttonDim;
    scrollBarMetrics( sb, sliderMin, sliderMax, sliderLength, buttonDim );

    if (sliderStart > sliderMax) { // sanity check
	sliderStart = sliderMax;
    }
    bool maxedOut = (sb->maxValue() == sb->minValue());

   
    if ( controls & AddLine ) {
	bool sunken = activeControl & AddLine;
	QRect r( b, b, w-2*b, w-2*b ) ;
	if ( horz )
	    r.moveBy( sb->width() - w, 0 );
	else
	    r.moveBy( 0, sb->height() - w );
	
	drawMetalButton( p, r.x(), r.y(), r.width(), r.height(), 
			 sunken, !horz );
	drawArrow(  p, horz ? RightArrow : DownArrow,  sunken, 
		    r.x(), r.y(), r.width(), r.height(), g, !maxedOut );

    } 
    if ( controls & SubLine ) {
	bool sunken = activeControl & SubLine;
	QRect r( b, b, w-2*b, w-2*b ) ;
	drawMetalButton( p, r.x(), r.y(), r.width(), r.height(), 
			 sunken, !horz );
	drawArrow(  p, horz ? LeftArrow : UpArrow,  sunken, 
		    r.x(), r.y(), r.width(), r.height(), g, !maxedOut );
    }

    QRect sliderR;
    if ( horz ) {
	sliderR .setRect( sliderStart, b, sliderLength, w-2*b );
    } else {
	sliderR .setRect( b, sliderStart, w-2*b, sliderLength );
    }
    if ( controls & Slider ) {
	if ( !maxedOut ) {
	    drawMetalButton( p, sliderR.x(), sliderR.y(),
				 sliderR.width(), sliderR.height(),
				 FALSE, horz ); 
	}
    }

}


void MetalStyle::drawComboButton( QPainter *p, int x, int y, int w, int h,
				  const QColorGroup &g, bool sunken,
				  bool /*editable*/,
				  bool enabled,
				  const QBrush *fill)
{
    
     qDrawWinPanel(p, x, y, w, h, g, TRUE,
		   fill?fill:(enabled?&g.brush( QColorGroup::Base ):
				      &g.brush( QColorGroup::Background )));


     drawMetalButton( p, x+w-2-16, y+2, 16, h-4, sunken, TRUE );
     
     drawArrow( p, QStyle::DownArrow, sunken,
		x+w-2-16+ 2, y+2+ 2, 16- 4, h-4- 4, g, enabled );
   
}





NorwegianWoodStyle::NorwegianWoodStyle( int sbext ) : QWindowsStyle()
{
    if ( sbext >= 0 )
	setScrollBarExtent( sbext );
}

/*!
  Reimplementation from QStyle
 */
void NorwegianWoodStyle::polish( QApplication *app)
{

    oldPalette = app->palette();

    // we simply create a nice QColorGroup with a couple of fancy wood
    // pixmaps here and apply to it all widgets

    QImage img(button_xpm);
    QImage orig = img;
    orig.detach();
    QPixmap button;
    button.convertFromImage(img);


    int i;
    for (i=0; i<img.numColors(); i++) {
	QRgb rgb = img.color(i);
	QColor c(rgb);
	rgb = c.dark(120).rgb();
	img.setColor(i,rgb);
    }
    QPixmap mid;
    mid.convertFromImage(img);

    img = orig;
    img.detach();
    for (i=0; i<img.numColors(); i++) {
	QRgb rgb = img.color(i);
	QColor c(rgb);
	rgb = c.light().rgb();
	img.setColor(i,rgb);
    }
    QPixmap light;
    light.convertFromImage(img);

    img = orig;
    img.detach();
    for (i=0; i<img.numColors(); i++) {
	QRgb rgb = img.color(i);
	QColor c(rgb);
	rgb = c.dark(180).rgb();
	img.setColor(i,rgb);
    }
    QPixmap dark;
    dark.convertFromImage(img);

    
    QImage bgimage(polish_xpm);
    QPixmap background;
    background.convertFromImage(bgimage);

    img = bgimage;
    img.detach();
    for (i=0; i<img.numColors(); i++) {
	QRgb rgb = img.color(i);
	QColor c(rgb);
	rgb = c.dark(180).rgb();
	img.setColor(i,rgb);
    }
    sunkenDark = new QPixmap;
    sunkenDark->convertFromImage(img);
    
    
    img = bgimage;
    img.detach();
    for (i=0; i<img.numColors(); i++) {
	QRgb rgb = img.color(i);
	QColor c(rgb);
	rgb = c.light(130).rgb();
	img.setColor(i,rgb);
    }
    sunkenLight= new QPixmap;
    sunkenLight->convertFromImage(img);
    
    
    
    QPalette op(QColor(212,140,95));
    // QPalette op(white);
    QColorGroup active (op.normal().foreground(),
		     QBrush(op.normal().button(),button),
		     QBrush(op.normal().light(), light),
		     QBrush(op.normal().dark(), dark),
		     QBrush(op.normal().mid(), mid),
		     op.normal().text(),
		     Qt::white,
		     QColor(236,182,120),
		     QBrush(op.normal().background(), background)
		     );
    QColorGroup disabled (op.disabled().foreground(),
		     QBrush(op.disabled().button(),button),
		     QBrush(op.disabled().light(), light),
		     op.disabled().dark(),
		     QBrush(op.disabled().mid(), mid),
		     op.disabled().text(),
		     Qt::white,
		     QColor(236,182,120),
		     QBrush(op.disabled().background(), background)
		     );

   app->setPalette(QPalette(active, disabled, active), TRUE );

}

void NorwegianWoodStyle::unPolish( QApplication *app)
{
    app->setPalette(oldPalette, TRUE);
}

/*!
  Reimplementation from QStyle
 */
void NorwegianWoodStyle::polish( QWidget* w)
{

    // the polish function sets some widgets to transparent mode and
    // some to translate background mode in order to get the full
    // benefit from the nice pixmaps in the color group.

    if (w->inherits("QTipLabel") || w->inherits("QLCDNumber") ){
	return;
    }

    if ( !w->isTopLevel() ) {
	if ( w->inherits("QPushButton")
	     || w->inherits("QToolButton")
	     || w->inherits("QGroupBox")
	     || w->inherits("QTabWidget")
	     || w->inherits("QComboBox") ) {
	    w->setAutoMask( TRUE );
	    return;
	}
 	if (w->inherits("QLabel")
	    || w->inherits("QButton") ) {
	    w->setBackgroundOrigin( QWidget::ParentOrigin );
 	}
    }
}

void NorwegianWoodStyle::unPolish( QWidget* w)
{
    // the polish function sets some widgets to transparent mode and
    // some to translate background mode in order to get the full
    // benefit from the nice pixmaps in the color group.
    if (w->inherits("QTipLabel") || w->inherits("QLCDNumber") ){
	return;
    }
    if ( !w->isTopLevel() ) {
	if ( w->inherits("QPushButton")
	     || w->inherits("QToolButton")
	     || w->inherits("QGroupBox")
	     || w->inherits("QTabWidget")
	     || w->inherits("QComboBox") ) {
	    w->setAutoMask( FALSE );
	    return;
	}
 	if (w->inherits("QLabel")
	    || w->inherits("QButton") ) {
	    w->setBackgroundOrigin( QWidget::WidgetOrigin );
 	}
    }
}

static void drawroundrect( QPainter *p, QCOORD x, QCOORD y,
			   QCOORD w, QCOORD h, QCOORD d )
{
    int rx = (200*d)/w;
    int ry = (200*d)/h;
    p->drawRoundRect( x, y, w, h, rx, ry );
}




static QRegion roundRectRegion( const QRect& g, int r )
{
    QPointArray a;
    a.setPoints( 8, g.x()+r, g.y(), g.right()-r, g.y(),
		 g.right(), g.y()+r, g.right(), g.bottom()-r,
		 g.right()-r, g.bottom(), g.x()+r, g.bottom(),
		 g.x(), g.bottom()-r, g.x(), g.y()+r );  
    QRegion reg( a );
    int d = r*2-1;
    reg += QRegion( g.x(),g.y(),r*2,r*2, QRegion::Ellipse );
    reg += QRegion( g.right()-d,g.y(),r*2,r*2, QRegion::Ellipse );
    reg += QRegion( g.x(),g.bottom()-d,r*2,r*2, QRegion::Ellipse );
    reg += QRegion( g.right()-d,g.bottom()-d,r*2,r*2, QRegion::Ellipse );
    return reg;
}




static int get_combo_extra_width( int h, int *return_awh=0 )
{
    int awh;
    if ( h < 8 ) {
	awh = 6;
    } else if ( h < 14 ) {
	awh = h - 2;
    } else {
	awh = h/2;
    }
    if ( return_awh )
	*return_awh = awh;
    return awh*3/2;
}


static void get_combo_parameters( const QRect &r,
				  int &ew, int &awh, int &ax,
				  int &ay, int &sh, int &dh,
				  int &sy )
{
    ew = get_combo_extra_width( r.height(), &awh );

    sh = (awh+3)/4;
    if ( sh < 3 )
	sh = 3;
    dh = sh/2 + 1;

    ay = r.y() + (r.height()-awh-sh-dh)/2;
    if ( ay < 0 ) {
	//panic mode
	ay = 0;
	sy = r.height();
    } else {
	sy = ay+awh+dh;
    }
    ax = r.x() + r.width() - ew +(ew-awh)/2;
}


void NorwegianWoodStyle::drawComboButton( QPainter *p, int x, int y, int w, int h,
				    const QColorGroup &g,
				    bool /* sunken */,
				    bool editable,
				    bool /*enabled */,
				    const QBrush *fb )
{
    QBrush fill = fb ? *fb : g.brush( QColorGroup::Button );

    int awh, ax, ay, sh, sy, dh, ew;
    get_combo_parameters( buttonRect(x,y,w,h), ew, awh, ax, ay, sh, dh, sy );

    drawButton( p, x, y, w, h, g, FALSE, &fill );

    qDrawArrow( p, DownArrow, MotifStyle, FALSE,
		ax, ay, awh, awh, g, TRUE );

    p->setPen( g.light() );
    p->drawLine( ax, sy, ax+awh-1, sy );
    p->drawLine( ax, sy, ax, sy+sh-1 );
    p->setPen( g.dark() );
    p->drawLine( ax+1, sy+sh-1, ax+awh-1, sy+sh-1 );
    p->drawLine( ax+awh-1, sy+1, ax+awh-1, sy+sh-1 );

    if ( 0 && editable ) {
	QRect r( comboButtonRect(x-1,y-1,w+2,h+2) );
	qDrawShadePanel( p, r, g, TRUE, 1, &fill );
    }
}


static inline int buttonthickness( int d )
{ return  d > 20 ? 5 : ( d < 10 ? 2: 3 ); }

enum { PointUp, PointDown, PointLeft, PointRight };


void NorwegianWoodStyle::drawSemicircleButton( QPainter *p, const QRect &r, 
					       int dir, bool sunken,
					       const QColorGroup &g )
{
    int b = scrollBarExtent().height() > 20 ? 3 : 2;
    
    QRegion extrn(  r.x(),   r.y(),   r.width(),     r.height(),     QRegion::Ellipse );
    QRegion intern( r.x()+b, r.y()+b, r.width()-2*b, r.height()-2*b, QRegion::Ellipse );
    int w2 = r.width()/2;
    int h2 = r.height()/2;

    int bug = 1; //off-by-one somewhere!!!???
    
    switch( dir ) {
    case PointRight:
	extrn +=  QRegion( r.x(),  r.y(),   w2,     r.height() );
	intern += QRegion( r.x()+b,r.y()+b, w2-2*b, r.height()-2*b );
	break;
    case PointLeft:
	extrn +=  QRegion( r.x()+w2,  r.y(),   w2,     r.height() );
	intern += QRegion( r.x()+w2+b,r.y()+b, w2-2*b, r.height()-2*b );
	break;
    case PointUp:
	extrn +=  QRegion( r.x(),  r.y()+h2,   r.width(),     h2 );
	intern += QRegion( r.x()+b,r.y()+h2+b, r.width()-2*b-bug, h2-2*b-bug );
	break;
    case PointDown:
	extrn +=  QRegion( r.x(),  r.y(),   r.width(),     h2 );
	intern += QRegion( r.x()+b,r.y()+b, r.width()-2*b-bug, h2-2*b-bug );
	break;
    }

    extrn = extrn - intern;
    QPointArray a;
    a.setPoints( 3, r.x(), r.y(), r.x(), r.bottom(), r.right(), r.top() );
    
    QRegion oldClip = p->clipRegion();
    p->setClipRegion( intern );
    p->fillRect( r, g.brush( QColorGroup::Button ) );

    p->setClipRegion( QRegion(a)&extrn );
    p->fillRect( r, sunken ? g.dark() : g.light() );

    a.setPoints( 3, r.right(), r.bottom(), r.x(), r.bottom(), r.right(), r.top() );
    p->setClipRegion( QRegion(a) &  extrn );
    p->fillRect( r, sunken ? g.light() : g.dark() );    

    p->setClipRegion( oldClip );
}



void NorwegianWoodStyle::drawScrollBarControls( QPainter* p, const QScrollBar* sb,
						int sliderStart, uint controls,
						uint activeControl )
{
    QWindowsStyle::drawScrollBarControls( p, sb, sliderStart, controls & ~(AddLine|SubLine),
					activeControl & ~(AddLine|SubLine) );
    bool horz = sb->orientation() == QScrollBar::Horizontal;
    int b = 2;
    int w = horz ? sb->height() : sb->width();

    QColorGroup g = sb->colorGroup();
    
    if ( controls & AddLine ) {
	bool sunken = activeControl & AddLine;
	QRect r( b, b, w-2*b, w-2*b ) ;
	if ( horz ) {
	    r.moveBy( sb->width() - w, 0 );
	    drawSemicircleButton( p, r, PointRight, sunken, g );
	} else {
	    r.moveBy( 0, sb->height() - w );
	    drawSemicircleButton( p, r, PointDown, sunken, g );
	}
    } 
    if ( controls & SubLine ) {
	bool sunken = activeControl & SubLine;
	QRect r( b, b, w-2*b, w-2*b ) ;
	if ( horz ) {
	    drawSemicircleButton( p, r, PointLeft, sunken, g );
	} else {
	    drawSemicircleButton( p, r, PointUp, sunken, g );
	}
    }
}

/*!
  Reimplementation from QStyle
 */
void NorwegianWoodStyle::drawButton( QPainter *p, int x, int y, int w, int h,
			     const QColorGroup &g, bool sunken, const QBrush* fill)
{
    int d = QMIN(w,h)/2;

    int b = buttonthickness( d );
    
    QRegion internR = roundRectRegion( QRect(x+b, y+b, w-2*b, h-2*b), d-b );
    //    qDrawShadePanel( p, x, y, w, h, g, sunken, 5);
    QPen oldPen = p->pen();

    QBrush brush( fill ? *fill : (sunken ? g.brush( QColorGroup::Mid ) :
                                          g.brush( QColorGroup::Button )));
    p->setClipRegion( internR );
    p->fillRect( x, y, w, h, brush );

    
    int e = QMIN( w, h )/2; 
    
    QPoint p2(x+w-1-e,y+e);
    QPoint p3(x+e, y+h-1-e);
    
    QPointArray a;
    a.setPoints( 5, x,y, x+w-1, y, p2.x(),p2.y(), p3.x(),p3.y(), x, y+h-1 );
    
    p->setClipRegion( QRegion( a )- internR );
    
    p->fillRect( x,y,w,h, (sunken ? QBrush( g.dark(), *sunkenDark )
			   : g.brush( QColorGroup::Light ) ));

    
    a.setPoint( 0, x+w-1, y+w-1 );
    p->setClipRegion( QRegion( a ) - internR );
    
    p->fillRect( x,y,w,h, (sunken ? QBrush( g.light(), *sunkenLight )
			   : g.brush( QColorGroup::Dark )));
    
    /*    
    QBrush oldBrush = p->brush();

    p->setPen( NoPen );
    p->setBrush( fill ? *fill : (sunken ? g.brush( QColorGroup::Mid ) :
                                          g.brush( QColorGroup::Button )));
    drawroundrect( p, x+3, y+3, w-6, h-6, 5 );
    p->setBrush( oldBrush );
    */
        p->setClipRegion( internR );
    p->setClipping( FALSE );
    p->setPen( g.foreground() );
    drawroundrect( p, x, y, w, h, d );
    p->setPen( oldPen );
}

/*!
  Reimplementation from QStyle

  \sa QStyle
  */
void NorwegianWoodStyle::drawBevelButton( QPainter *p, int x, int y, int w, int h,
				const QColorGroup &g, bool sunken, const QBrush* fill)
{
    QWindowsStyle::drawBevelButton(p, x, y, w, h, g, sunken, fill);
}

/*!
  Reimplementation from QStyle
 */
void NorwegianWoodStyle::drawPushButton( QPushButton* btn, QPainter *p)
{
    QColorGroup g = btn->colorGroup();
    int x1, y1, x2, y2;

    btn->rect().coords( &x1, &y1, &x2, &y2 );	// get coordinates

    p->setPen( g.foreground() );
    p->setBrush( QBrush(g.button(),NoBrush) );

    QBrush fill;
    if ( btn->isDown() )
	fill = g.brush( QColorGroup::Mid );
    else if ( btn->isOn() )
	fill = QBrush( g.mid(), Dense4Pattern );
    else
	fill = g.brush( QColorGroup::Button );	

    if ( btn->isDefault() ) {
	x1 += 2;
	y1 += 2;
	x2 -= 2;
	y2 -= 2;
    }
	
    drawButton( p, x1, y1, x2-x1+1, y2-y1+1, g, btn->isOn() || btn->isDown(), &fill);
    
    if ( btn->isDefault() ) {
	QPen pen( Qt::black, 4 );
	pen.setCapStyle( Qt::RoundCap );
	pen.setJoinStyle( Qt::RoundJoin );
	p->setPen( pen );
	drawroundrect( p, x1-1, y1-1, x2-x1+3, y2-y1+3, 8 );
    }


    if ( btn->isMenuButton() ) {
	int dx = (y1-y2-4)/3;
	drawArrow( p, DownArrow, FALSE,
		   x2 - dx, dx, y1, y2 - y1,
		   g, btn->isEnabled() );
    }

    if ( p->brush().style() != NoBrush )
	p->setBrush( NoBrush );

}


/*!
  Reimplementation from QStyle
 */
void NorwegianWoodStyle::drawPushButtonLabel( QPushButton* btn, QPainter *p)
{
    QRect r = btn->rect();
    int x, y, w, h;
    r.rect( &x, &y, &w, &h );

    int x1, y1, x2, y2;
    btn->rect().coords( &x1, &y1, &x2, &y2 );	// get coordinates
    int dx = 0;
    int dy = 0;
    if ( btn->isMenuButton() )
	dx = (y2-y1) / 3;
    if ( dx || dy )
	p->translate( dx, dy );

    x += 2;  y += 2;  w -= 4;  h -= 4;
    QColorGroup g = btn->colorGroup();
    drawItem( p, x, y, w, h,
	      AlignCenter|ShowPrefix,
	      g, btn->isEnabled(),
	      btn->pixmap(), btn->text(), -1,
	      (btn->isDown() || btn->isOn())?&btn->colorGroup().brightText():&btn->colorGroup().buttonText());

    if ( dx || dy )
	p->translate( -dx, -dy );
}

/*!
  Reimplementation from QStyle
 */
QRect NorwegianWoodStyle::buttonRect( int x, int y, int w, int h){
    int d = QMIN(w,h)/2;
    int b = buttonthickness( d );

    d -= b;
    b++;
    
    if ( w<h )
	return QRect(x+b, y+d, w-2*b, h-2*d);
    else
	return QRect(x+d, y+b, w-2*d, h-2*b);
}

/*!
  Reimplementation from QStyle
 */
void NorwegianWoodStyle::drawButtonMask( QPainter *p, int x, int y, int w, int h)
{
    int d = QMIN(w,h)/2;
    drawroundrect( p, x, y, w, h, d );
}
