#include "PreCompiled.h"

#ifndef _PreComp_
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


#if QT_VERSION <= 230
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


#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////

/***************************************************************************
  -------------------
  description       Implementation of StepStyle
  -------------------
  begin             Tue Feb 15 2000
  author            David Johnson <david@usermode.org>
  -------------------
  Copyright 2000, David Johnson
  All rights reserved.

  Please see the header file for copyright and license information
 ***************************************************************************/


// NOTE: QStyle::drawToolBarHandle is not virtual, so it can't be reimplemented
// effectively. This means that it will appear like the WindowsStyle until
// Qt3.0. At that time I intend for it to be either a plain rect or a rect with
// a NeXT style circle

StepStyle::StepStyle() { ; }

StepStyle::~StepStyle() { ; }

void StepStyle::polish(QApplication *a)
{
    // save the palette for unpolishing later
    savePalette = pal = a->palette();
    // make highlights more appropriate for NeXT widgets. also see polish(QWidget)
    pal.setColor(QPalette::Active, QColorGroup::Highlight, pal.active().background());
    pal.setColor(QPalette::Active, QColorGroup::HighlightedText, pal.active().text());
    pal.setColor(QPalette::Disabled, QColorGroup::Highlight, pal.disabled().background());
    pal.setColor(QPalette::Disabled, QColorGroup::HighlightedText, pal.disabled().text());
    pal.setColor(QPalette::Disabled, QColorGroup::Base, pal.disabled().button());
#if QT_VERSION >= 210
    pal.setColor(QPalette::Inactive, QColorGroup::Highlight, pal.inactive().background());
    pal.setColor(QPalette::Inactive, QColorGroup::HighlightedText, pal.inactive().text());
#else
    pal.setColor(QPalette::Normal, QColorGroup::Highlight, pal.normal().background());
    pal.setColor(QPalette::Normal, QColorGroup::HighlightedText, pal.normal().text());
#endif // QT_VERSION
    a->setPalette(pal, true);
}

void StepStyle::polish(QWidget *w)
{
    // some widgets use PaletteBase as the background, so fix for NeXT style
    // a base background should only be used for editable widgets
    if (w->inherits("QListBox") || w->inherits("QListView")) {
        pal = w->palette();
        pal.setColor(QPalette::Active, QColorGroup::Highlight, pal.active().base());
        pal.setColor(QPalette::Active, QColorGroup::Base, pal.active().background());
#if QT_VERSION >= 210
        pal.setColor(QPalette::Inactive, QColorGroup::Highlight, pal.active().base());
        pal.setColor(QPalette::Inactive, QColorGroup::Base, pal.active().background());
#else // QT_VERSION < 210
        pal.setColor(QPalette::Normal, QColorGroup::Highlight, pal.normal().base());
        pal.setColor(QPalette::Normal, QColorGroup::Base, pal.normal().background());
#endif // QT_VERSION
        w->setPalette(pal);
    }
    // Progress bar should have a contrasting highlight
    if (w->inherits("QProgressBar")) {
        pal = w->palette();
        pal.setColor(QPalette::Active, QColorGroup::Highlight, pal.active().shadow());
#if QT_VERSION >= 210
        pal.setColor(QPalette::Inactive, QColorGroup::Highlight, pal.active().shadow());
#else // QT_VERSION < 210
        pal.setColor(QPalette::Normal, QColorGroup::Highlight, pal.normal().shadow());
#endif // QT_VERSION
        w->setPalette(pal);
    }
}

void StepStyle::polish(QPalette& pal) { ; }

void StepStyle::unPolish(QApplication *a)
{
    // restore old palette
    a->setPalette(savePalette, true);
}

void StepStyle::unPolish(QWidget *w)
{
    // unset the palette
    if (w->inherits("QListBox") || w->inherits("QListView") || w->inherits("QProgressBar"))
#if QT_VERSION >= 210
        w->unsetPalette();
#else
        w->setPalette(savePalette);
#endif //QT_VERSION
}

void StepStyle::unPolish(QPalette& pal) { ; }

void StepStyle::drawButton(QPainter *p, int x, int y, int w, int h,
            const QColorGroup &g, bool sunken, const QBrush* fill)
{
    // the QWindowsStyle button was close, but not quite...
    QPen savePen = p->pen();

    // fill in the button
    p->fillRect(x+1, y+1, w-2, h-2, fill ? *fill : g.brush(QColorGroup::Button));
    // draw sunken button
    if (sunken) {
        p->setPen(g.shadow());
        p->drawLine(x, y, x+w-1, y);
        p->drawLine(x, y, x, y+h-1);
        p->setPen(g.dark());
        p->drawLine(x+1, y+1, x+w-2, y+1);
        p->drawLine(x+1, y+1, x+1, y+h-2);
        p->setPen(g.light());
        p->drawLine(x+1, y+h-1, x+w-1, y+h-1);
        p->drawLine(x+w-1, y+1, x+w-1, y+h-1);
    // draw raised button
    } else {
        p->setPen(g.light());
        p->drawLine(x, y, x+w-2, y);
        p->drawLine(x, y, x, y+h-2);
        p->setPen(g.dark());
        p->drawLine(x+1, y+h-2, x+w-2, y+h-2);
        p->drawLine(x+w-2, y+1, x+w-2, y+h-2);
        p->setPen(g.shadow());
        p->drawLine(x, y+h-1, x+w-1, y+h-1);
        p->drawLine(x+w-1, y, x+w-1, y+h-1);
    }
    p->setPen(savePen);
}

void StepStyle::drawBevelButton(QPainter *p, int x, int y, int w, int h,
                const QColorGroup &g, bool sunken, const QBrush* fill)
{
    // a bevel button is identical to a basic button
    drawButton(p, x, y, w, h, g, sunken, fill);
}

void StepStyle::drawPushButton(QPushButton* b, QPainter *p)
{
    // get info about pushbutton
    QColorGroup g = b->colorGroup();
    int x, y, w, h;
    b->rect().rect(&x, &y, &w, &h);

    // get proper color for button
    QBrush fill;
    if (b->isDown())
        fill = g.brush(QColorGroup::Base);
    else
        fill = g.brush(QColorGroup::Button);
    // now draw it
    drawButton(p, x, y, w, h, g, b->isOn() || b->isDown(), &fill);
}

void StepStyle::drawPushButtonLabel(QPushButton* b, QPainter *p)
{
    // get the info we need from QPushButton
    QColorGroup g = b->colorGroup();
    // get the contents rect
#if QT_VERSION >= 210
    QRect r = pushButtonContentsRect(b);
#else
    QRect r = b->rect();
#endif // QT_VERSION
    int x, y, w, h;
    r.rect(&x, &y, &w, &h);

    // the return key symbol
    // TODO: a polyline might work better for this...
    static QCOORD returnshadow[] = {
        4,8, 3,7, 2,6, 1,5, 0,4,
        1,3, 2,2, 3,1, 4,0, 5,0,
        6,2, 7,2, 8,2, 9,2, 10,2,
        10,1, 10,0, 11,0, 12,0, 13,0 };
    static QCOORD returndark[] = {
        5,1, 5,2 };
    static QCOORD returnlight[] = {
        5,8, 5,7, 5,6,
        6,6, 7,6, 8,6, 9,6, 10,6, 11,6, 12,6, 13,6, 14,6,
        14,5, 14,4, 14,3, 14,2, 14,1, 14,0 };
    static int returnwidth = 15;
    static int returnheight = 9;

    // draw any special 'flares' on button
    if (b->isDefault()) { // not autoDefault
        // draw the return key symbol
        QPen savePen = p->pen();
        int xx = x + w - returnwidth - 4;
        int yy = y + ((h-returnheight) / 2);
        QPointArray a(sizeof(returnshadow)/(sizeof(QCOORD)*2), returnshadow);
        a.translate(xx, yy);
        p->setPen(g.shadow());
        p->drawPoints(a);
        a.setPoints(sizeof(returndark)/(sizeof(QCOORD)*2), returndark);
        a.translate(xx, yy);
        p->setPen(g.dark());
        p->drawPoints(a);
        a.setPoints(sizeof(returnlight)/(sizeof(QCOORD)*2), returnlight);
        a.translate(xx, yy);
        p->setPen(g.light());
        p->drawPoints(a);
        p->setPen(savePen);
        // the text now has to fit in a smaller space
        w = w - returnwidth;
    } else if ( b->isMenuButton() ) { // can't be both default and menu
        // draw a halfsize arrow
#if QT_VERSION >= 210
        int dx = menuButtonIndicatorWidth(b->height()) / 2;
#else
        int dx = h / 2;
#endif // QT_VERSION
        drawArrow(p, DownArrow, false, x+w-dx-8, y+2, dx, h-4, g, b->isEnabled(), &g.brush(QColorGroup::Text));
        // the text now has to fit in a smaller space
        w -= dx;
    }

    // now go draw the text
    drawItem(p, x, y, w, h, AlignCenter | ShowPrefix, g, b->isEnabled(), b->pixmap(), b->text(), -1, &g.buttonText());
}

void StepStyle::getButtonShift(int &x, int &y)
{
    // shifts the button contents when button is pressed for a 3D effect
    x = 1;
    y = 1;
}

void StepStyle::drawComboButton(QPainter *p, int x, int y, int w, int h, const QColorGroup &g,
            bool sunken, bool editable,bool enabled, const QBrush *fill)
{
    QPen savePen = p->pen();

    // draw the basic button
    drawButton(p, x, y, w, h, g, fill);

    // draw the 'handle', a 9x6 gadget
    int yy = (h-6)/2;
    drawButton(p, w-15, yy, 9, 6, g, false, fill);
    // draw a shadow for the handle
    p->setPen(g.dark());
    p->drawLine(w-6, yy+1, w-6, yy+6);
    p->drawLine(w-6, yy+6, w-14, yy+6);

    p->setPen( savePen );
}

QRect StepStyle::comboButtonRect(int x, int y, int w, int h)
{
    return QRect(x+3, y+3, w-24, h-7);
}

QRect StepStyle::comboButtonFocusRect(int x, int y, int w, int h)
{
    return comboButtonRect(x, y, w, h);
}

void StepStyle::drawScrollBarControls(QPainter* p, const QScrollBar* sb,
            int sliderStart, uint controls, uint activeControl)
{
    // get the info that we need
    QColorGroup g  = sb->colorGroup();
    int sliderMin, sliderMax, sliderLength, buttonDim;
    scrollBarMetrics(sb, sliderMin, sliderMax, sliderLength, buttonDim);
    bool horiz = (sb->orientation() == QScrollBar::Horizontal);
    int length = (horiz) ? sb->width() : sb->height();
    int breadth = (horiz) ? sb->height() : sb->width();
    if (sliderStart > sliderMax) sliderStart = sliderMax;
    int sliderEnd = sliderStart + sliderLength;

    // dimensions for the slider controls
    QRect addRect, subRect, subPageRect, addPageRect, sliderRect;
    int addX, addY, subX, subY;

    // compute dimensions for controls
    if (horiz) {
        addY = subY = (breadth - buttonDim)/2;
        subX = 0;
        addX = buttonDim;
        subRect.setRect(subX, subY, buttonDim, buttonDim);
        addRect.setRect(addX, addY, buttonDim, buttonDim);
        subPageRect.setRect(buttonDim*2, 0, sliderStart-(buttonDim*2), breadth);
        addPageRect.setRect(sliderEnd, 0, length-sliderEnd, breadth);
        sliderRect.setRect(sliderStart, 0, sliderLength, breadth);
    } else {
        addX = subX = (breadth - buttonDim) / 2;
        addY = length - buttonDim;
        subY = length - (buttonDim*2);
        addRect.setRect(addX, addY, buttonDim, buttonDim);
        subRect.setRect(subX, subY, buttonDim, buttonDim);
        subPageRect.setRect(0, 0, breadth, sliderStart);
        addPageRect.setRect(0, sliderEnd, breadth, subY - sliderEnd);
        sliderRect .setRect(0, sliderStart, breadth, sliderLength);
    }

    QPen savePen = p->pen();

    // draw "up" button
    if (controls & SubLine) {
        drawPanel(p, subRect.x()+1, subRect.y()+1, subRect.width()-2, subRect.height()-2,
                g, (activeControl == SubLine), 1, &g.brush(QColorGroup::Button));
        p->setPen(g.shadow());
        p->drawRect(subRect);
        drawArrow(p, (horiz) ? LeftArrow : UpArrow, false,
                subRect.x()+4, subRect.y()+4, subRect.width()-8, subRect.height()-8,
                g, true, 0);
    }
    // draw "down" button
    if (controls & AddLine) {
        drawPanel(p, addRect.x()+1, addRect.y()+1, addRect.width()-2, addRect.height()-2,
                g, (activeControl == AddLine), 1, &g.brush(QColorGroup::Button));
        p->setPen(g.shadow());
        p->drawRect(addRect);
        drawArrow(p, (horiz) ? RightArrow : DownArrow, false,
                addRect.x()+4, addRect.y()+4, addRect.width()-8, addRect.height()-8,
                g, true, 0);
    }
    // draw "up" groove
    if (controls & SubPage) {
        drawScrollBarGroove(p, subPageRect, sb, g);
    }
    // draw "down" groove
    if (controls & AddPage) {
        drawScrollBarGroove(p, addPageRect, sb, g);
    }
    // draw the slider
    if (controls & Slider) {
        drawPanel(p, sliderRect.x()+1,sliderRect.y()+1, sliderRect.width()-2, sliderRect.height()-2,
                g, false, 1, &g.brush(QColorGroup::Button));
        // is there room to draw the dot?
        if ((sliderRect.width() > 10) && (sliderRect.height() > 10))
            drawCircle(p, sliderRect.x(), sliderRect.y(), sliderRect.width(), sliderRect.height(), g);
        p->setPen(g.shadow());
        p->drawRect(sliderRect);
    }
    p->setPen(savePen);
}

void StepStyle::scrollBarMetrics(const QScrollBar* sb, int &sliderMin, int &sliderMax, int &sliderLength, int& buttonDim)
{
    // get the info we need
    bool horiz = (sb->orientation() == QScrollBar::Horizontal);
    int length = (horiz) ? sb->width() : sb->height();
    int breadth = (horiz) ? sb->height() : sb->width();

    // dimension of the buttons
    if (length > (breadth * 2) + 4)
        buttonDim = breadth;
    else
        // start shrinking the buttons to fit
        buttonDim = (length / 2) - 2;

    // sanity check
    if (buttonDim < 2) buttonDim = 2;

    // start at very top or very left
    if (horiz)
        sliderMin = buttonDim*2;
    else
        sliderMin = 0;

    // length of slider groove
    int grooveLength = length - (buttonDim * 2);

    // calculate length of slider
    sliderLength = (sb->pageStep()*grooveLength) / (sb->maxValue()-sb->minValue()+sb->pageStep());

    // give slider a reasonable length if it isn't
    if (sliderLength > grooveLength)
        sliderLength = grooveLength;
    if (sliderLength < 12)
        // keep the dot in view
        sliderLength = 12;

    // end of slider
    sliderMax = sliderMin + grooveLength - sliderLength;
}

QStyle::ScrollControl StepStyle::scrollBarPointOver(const QScrollBar* sb, int sliderStart, const QPoint& p)
{
    // check to see if there's any work to do...
    if (!sb->rect().contains(p))
        return NoScroll;

    // get the info we need
    int sliderMin, sliderMax, sliderLength, buttonDim;
    scrollBarMetrics(sb, sliderMin, sliderMax, sliderLength, buttonDim);

    int position;
    if (sb->orientation() == QScrollBar::Horizontal) {
        // get position of point
        position = p.x();
        // which control?
        if (position <= buttonDim)                  return (SubLine);
        if (position <= buttonDim*2)                return (AddLine);
        if (position < sliderStart)                 return (SubPage);
        if (position < sliderStart+sliderLength)    return (Slider);
        /* else */                                  return (AddPage);
    } else {
        // get position of point
        position = p.y();
        // which control?
        if (position < sliderStart)                 return (SubPage);
        if (position < sliderStart+sliderLength)    return (Slider);
        if (position < sliderMax+sliderLength)      return (AddPage);
        if (position < sliderMax+sliderLength+buttonDim)    return (SubLine);
        /* else */                                  return (AddLine);
    }
}

void StepStyle::drawScrollBarGroove(QPainter *p, QRect r, const QWidget *sb, const QColorGroup &g)
{
    // return if invalid rect
    if (!r.isValid()) return;

    QPen savePen = p->pen();

    // adjust fill rect so it doesn't overdraw the border
    if (r.top() == 0)                   r.setTop(1);
    if (r.left() == 0)                  r.setLeft(1);
    if (r.bottom() == sb->height()-1)   r.setBottom(sb->height()-2);
    if (r.right() == sb->width()-1)     r.setRight(sb->width()-2);
    // fill in the groove
    p->fillRect(r, g.brush(QColorGroup::Mid));
    // now outline the whole scroll bar in shadow
    p->setPen(g.shadow());
    p->drawRect(0, 0, sb->width(), sb->height());

    p->setPen(savePen);
}

void StepStyle::drawIndicator(QPainter* p, int x, int y, int w, int h,
            const QColorGroup &g, int state, bool down, bool /* enabled */)
{
    // draw the basic checkbox button
    drawButton(p, x, y, w, h, g, false);    // NeXT checkboxes don't depress
    if(state != QButton::Off) {
        // draw a checkmark
        if (state == QButton::On)
            drawCheckMark(p, x, y, w, h, g, true, false);   // a regular checkmark
        else
            drawCheckMark(p, x, y, w, h, g, true, true);    // a 'grayed' checkmark
    }
}

QSize StepStyle::indicatorSize() const { return QSize(16,16); }

void StepStyle::drawExclusiveIndicator(QPainter* p, int x, int y, int w, int h,
            const QColorGroup &g, bool on, bool down, bool /* enabled */)
{
    // the radio button shape
    static QCOORD circleshadow[] = { // clockwise
        5,1, 6,1, 7,1, 8,1, 9,1, 10,1,
        1,9, 1,8, 1,7, 1,6, 1,5,
        2,4, 2,3, 3,2, 4,2,
        5,1, 6,1, 7,1, 8,1, 9,1
    };
    static QCOORD circlemid[] = { // clockwise
        2,12, 1,11, 1,10, 2,10, 0,9, 0,8, 0,7, 0,6, 0,5,
        1,4, 1,3, 2,2, 3,3, 3,1, 4,1,
        5,0, 6,0, 7,0, 8,0, 9,0, 10,1, 11,1, 10,2, 12,2
    };
    static QCOORD circlelight[] = { // clockwise
        13,4, 14,5, 14,6, 14,7, 14,8, 14,9, 13,10, 13,11, 12,12,
        11,13,  10,13, 9,14, 8,14, 7,14, 6,14, 5,14, 4,13,
    };

    // the dot in the selected button
    static QCOORD dot[] = { // the dot
        6,3, 9,3,
        4,4, 11,4, 4,5, 11,5,
        3,6, 12,6, 3,7, 12,7, 3,8, 12,8, 3,9, 12,9,
        4,10, 11,10, 4,11, 11,11,
        6,12, 9,12};

    QPen savePen = p->pen();

    // first erase the control
    p->fillRect(x, y, w, h, g.brush(QColorGroup::Background));

    // now draw the button
    QPointArray a(sizeof(circleshadow)/(sizeof(QCOORD)*2), circleshadow);
    a.translate(x, y);
    p->setPen(g.shadow());
    p->drawPoints(a);
    a.setPoints(sizeof(circlemid)/(sizeof(QCOORD)*2), circlemid);
    a.translate(x, y);
    p->setPen(g.mid());
    p->drawPoints(a);
    a.setPoints(sizeof(circlelight)/(sizeof(QCOORD)*2), circlelight);
    a.translate(x, y);
    p->setPen(g.light());
    p->drawPoints(a);
    // now draw the dot
    if (on || down) {
        a.setPoints(sizeof(dot)/(sizeof(QCOORD)*2), dot);
        a.translate(x,y);
        p->setPen(g.base());
        p->drawLineSegments(a);
    }
    p->setPen(savePen);
}

QSize StepStyle::exclusiveIndicatorSize() const { return QSize(15,15); }

void StepStyle::drawSlider(QPainter *p, int x, int y, int w, int h,
            const QColorGroup &g, Orientation orient, bool, bool)
{
    // draw the slider button
    drawButton(p, x+1, y+1, w-2, h-2, g, false, &g.brush( QColorGroup::Button));
    // draw the center notch
    if (orient == Horizontal) {
        int mid = x + (w/2);
        qDrawShadeLine(p, mid, y+1, mid, y+h-2, g, true, 1);
    } else{
        int mid = y + (h/2);
        qDrawShadeLine(p, x+1, mid, x+w-2, mid, g, true, 1);
    }
}

int StepStyle::sliderLength() const { return 21; }

void StepStyle::drawSliderGroove(QPainter *p, int x, int y, int w, int h,
            const QColorGroup& g, QCOORD c, Orientation orient)
{
    // draw the slider groove (depressed with Mid color)
    drawButton(p, x, y, w, h, g, true, &g.brush(QColorGroup::Mid));
}

int StepStyle::maximumSliderDragDistance() const { return -1; }

#ifdef INCLUDE_MENUITEM_DEF
void StepStyle::drawPopupMenuItem(QPainter* p, bool checkable, int maxpmw, int tab,
            QMenuItem* mi, const QPalette& pal, bool act, bool enabled, int x, int y, int w, int h)
{
    // there's no good way to draw a line between each menu item as is done in
    // NeXT, so I'll leave it up to the user to insert a separator between each

    static const int checkWidth    = 12;    // width of a checkmark
    static const int border        = 2;    // clear border of menu

    // exit if no menuitem
    if (!mi) return;

    // set up colors
    QColorGroup cgroup = (enabled) ? pal.active() : pal.disabled();

    // make sure checkmarks fit
    if (checkable) maxpmw = QMAX(maxpmw, checkWidth);

    // draw a separator
    if (mi->isSeparator()) {
        qDrawShadeLine(p, x, y+1, x+w, y+1, cgroup, true, 1);
        return;
    }

    // highlight item appropriately
    if (act && enabled)
        p->fillRect(x, y, w, h, cgroup.brush( QColorGroup::Base ));
    else
        p->fillRect(x, y, w, h, cgroup.brush( QColorGroup::Button ));

    // draw a checkmark
    if (checkable) {
        if (mi->isChecked())
            drawCheckMark(p, x+border, y+border, maxpmw, h, cgroup, act, !enabled);
    } else
    // draw an iconset
    if (mi->iconSet()) {
        QIconSet::Mode mode;
        if (enabled) {
            if (act) mode = QIconSet::Active;    // enabled and active
            else mode = QIconSet::Normal;        // enabled and inactive
        } else mode = QIconSet::Disabled;        // disabled
        QPixmap pixmap = mi->iconSet()->pixmap(QIconSet::Small, mode);
        QRect checkRect(x+border, y+border, maxpmw, h-(border*2) );
        QRect pixmapRect(0, 0, pixmap.width(), pixmap.height());
        // center the pixmap
        pixmapRect.moveCenter(checkRect.center());
        // draw it
        p->drawPixmap(pixmapRect.topLeft(), pixmap);
    }

    // set text color based on state
    if (enabled)
        p->setPen(cgroup.buttonText());
    else
        p->setPen(cgroup.text());

    // calc text positions
    int textx = x + maxpmw + (border * 2);
    int textw = w - maxpmw + (border * 2) - tab + 1;
    int texty = y + border;
    int texth = h - (border * 2);

    // a custom menu item
#if QT_VERSION >= 210
    if (mi->custom()) {
        p->save();
        mi->custom()->paint(p, cgroup, act, enabled, textx, texty, textw, texth);
        p->restore();
        return;
    }
#endif // QT_VERSION
    // draw a pixmap
    if (mi->pixmap()) {
        p->drawPixmap(textx, texty, *mi->pixmap());
        return;
    }

    // draw the text
    if (!mi->text().isNull()) {
        int flags = AlignLeft | AlignVCenter | ShowPrefix;
        int tabpos = mi->text().find('\t'); // find the tab
        // draw the text before the tab
        p->drawText(textx, texty, textw, texth, flags, mi->text(), tabpos);
        // draw the text after the tab
        if (tabpos >= 0)
            p->drawText(x+w-tab-(border*2), texty, tab, texth, flags, mi->text().mid(tabpos+1));
    }

    // draw a submenu arrow
    if (mi->popup()) {
        int center = texth / 2;
        drawArrow(p, RightArrow, true, x+w-border-center-6 /*room for menu overlap*/,
                    (y+h/2) - (center/2), center, center, cgroup, mi->isEnabled(), 0);
    }
}
#endif

void StepStyle::drawTab(QPainter* p, const QTabBar* tb, QTab* t, bool selected)
{
    // I found no examples of tabs in the NeXT documentation, and I don't know
    // if the WINGs tabs are accurate, so I'll make them square for now
    QRect rect(t->r); // shorthand

    // adjust slightly since we're using qWinPanel for the panels
    if (!rect.left()) rect.setLeft(rect.left()+1);

    // draw 'rounded' above tab
    if (tb->shape() == QTabBar::RoundedAbove) {
        // draw the line below the tab
        if (selected) { // selected tab doesn't get line below
            p->setPen(tb->colorGroup().background());
            p->drawLine(rect.left()+1, rect.bottom(), rect.right()-1, rect.bottom());
        } else {
            p->setPen(tb->colorGroup().light());
            p->drawLine(rect.left(), rect.bottom(), rect.right(), rect.bottom());
            // make rect smaller for unselected tabs
            rect.setRect(rect.left()+2, rect.top()+2, rect.width()-4, rect.height()-2);
        }

        // draw line on the left
        p->setPen(tb->colorGroup().light());
        p->drawLine(rect.left(), rect.bottom(), rect.left(), rect.top());

        // draw the line on the top
        p->drawLine(rect.left(), rect.top(), rect.right(), rect.top());

        // draw line on the right
        p->setPen(tb->colorGroup().dark());
        p->drawLine(rect.right()-1, rect.top()+1, rect.right()-1, rect.bottom()-1);
        if (selected) p->drawPoint(rect.right()-1, rect.bottom()); // minor tweak
        p->setPen(tb->colorGroup().shadow());
        p->drawLine(rect.right(), rect.top(), rect.right(), rect.bottom()-1);
        // need a light point to blend into the next button
        p->setPen(tb->colorGroup().light());
        p->drawPoint(rect.right(), rect.bottom());
    }
    // draw the rounded below tab
    else if (tb->shape() == QTabBar::RoundedBelow) {
        // draw the line above the tab
        if (selected) { // selected tab doesn't get line above
            p->setPen(tb->colorGroup().background());
            p->drawLine(rect.left(), rect.top(), rect.right(), rect.top());
            p->drawLine(rect.left(), rect.top()+1, rect.right(), rect.top()+1);
        } else {
            p->setPen(tb->colorGroup().dark());
            p->drawLine(rect.left(), rect.top(), rect.right(), rect.top());
            p->setPen(tb->colorGroup().shadow());
            p->drawLine(rect.left(), rect.top()+1, rect.right(), rect.top()+1);
            // adjust size of unselected tab
            rect.setRect(rect.left()+2, rect.top(), rect.width()-4, rect.height()-2);
        }

        // draw line on the left
        p->setPen(tb->colorGroup().light());
        p->drawLine(rect.left(), rect.bottom()-1, rect.left(), rect.top()+2);
        // tweak the line on the left if selected tab
        if (selected) {
            p->drawLine(rect.left(), rect.top()+1, rect.left(), rect.top());
            if (rect.left() > 1) {
                p->setPen(tb->colorGroup().dark());
                p->drawPoint(rect.left(), rect.top());
            }
        }

        // draw the line on the bottom
        p->setPen(tb->colorGroup().dark());
        p->drawLine(rect.left()+1, rect.bottom()-1, rect.right()-1, rect.bottom()-1);
        p->setPen(tb->colorGroup().shadow());
        p->drawLine(rect.left(), rect.bottom(), rect.right(), rect.bottom());

        // draw line on the right
        p->setPen(tb->colorGroup().dark());
        p->drawLine(rect.right()-1, rect.top()+2, rect.right()-1, rect.bottom()-1);
        // need a dark point to blend into the next button
        p->drawPoint(rect.right(), rect.top());
        if (selected) p->drawLine(rect.right()-1, rect.top(), rect.right()-1, rect.top()+1);
        p->setPen(tb->colorGroup().shadow());
        p->drawLine(rect.right(), rect.top()+1, rect.right(), rect.bottom()-1);
    }
    // pass everything else to QWindowsStyle (triangular tabs)
    else {
        QWindowsStyle::drawTab(p, tb, t, selected);
    }
}

void StepStyle::drawSplitter(QPainter *p, int x, int y, int w, int h,
            const QColorGroup &g, Orientation)
{
    // just a circle in the center
    drawCircle(p, x, y, w, h, g);
}

int StepStyle::splitterWidth() const
{
    return 8;
}

void StepStyle::drawPanel(QPainter *p, int x, int y, int w, int h,
            const QColorGroup &cg, bool sunken, int lineWidth, const QBrush *fill)
{
    if (lineWidth > 1)
        // a 'thicker' panel
        qDrawWinPanel(p, x, y, w, h, cg, sunken, fill);
    else
        // a 'flatter' panel
        QStyle::drawPanel(p, x, y, w, h, cg, sunken, lineWidth, fill);
}

void StepStyle::drawFocusRect(QPainter* p, const QRect& r, const QColorGroup &g,
            const QColor* bg, bool atBorder)
{
    // I can't find any examples of NeXT focii, but they are very useful things
    // kept the windows style focus because it is more low-key
    QWindowsStyle::drawFocusRect(p, r, g, bg, atBorder);
}

void StepStyle::drawArrow(QPainter *p, Qt::ArrowType type, bool down, int x,
            int y, int w, int h, const QColorGroup &g, bool enabled, const QBrush *)
{
    // adjust coords to center arrow in rect
    if (w > h) {
        h -= 2;
        x += (w-h)/2;
        w = h;
    }
    else if (h > w) {
        w -= 2;
        y += (h-w)/2;
        h = w;
    }
    int x2 = x+w-1;
    int y2 = y+h-1;

    // now get triangle coords based on orientation
    // the order of points is important when drawing a depressed arrow
    QPointArray a;
    switch (type){
    case Qt::UpArrow:
        a.setPoints(4, x+w/2,y, x2,y2, x,y2, x+w/2,y);
        break;
    case Qt::DownArrow:
        a.setPoints(4, x,y, x+w/2,y2, x2,y, x,y);
        break;
    case Qt::LeftArrow:
        a.setPoints(4, x,y+h/2, x2,y2, x2,y, x,y+h/2);
        break;
    case Qt::RightArrow:
        a.setPoints(4, x,y, x2,y+h/2, x,y2, x,y);
        break;
    }
    QBrush saveBrush = p->brush();
    QPen savePen = p->pen();
    // draw the depressed arrow
    if (down) {
        p->setPen(g.dark());
        p->drawLine(a.point(0), a.point(1));
        p->setPen(g.light());
        p->drawLine(a.point(1), a.point(2));
        p->setPen(g.shadow());
        p->drawLine(a.point(2), a.point(3));
    }
    // draw the normal arrow
    else {
        if (enabled) {
            p->setPen(g.shadow());
            p->setBrush(g.brush(QColorGroup::Shadow));
        } else {
            p->setPen(g.mid());
            p->setBrush(g.brush(QColorGroup::Mid));
        }
        p->drawPolygon(a);
    }
    p->setBrush(saveBrush);
    p->setPen(savePen);
}

void StepStyle::drawCircle(QPainter* p, int x, int y, int w, int h, const QColorGroup &g)
{
    // the NeXT style dot (6x6)
    static QCOORD circleshadow[] = {
        0,4, 0,3, 0,2, 1,1, 2,0, 3,0, 4,0
    };
    static QCOORD circledark[] = {
        1,0, 0,1, 1,5, 1,4, 1,3, 1,2, 2,2, 2,1, 3,1, 4,1, 5,1
    };
    static QCOORD circlelight[] = {
        3,5, 4,5, 3,4, 4,4, 5,4, 4,3, 5,3
    };

    // get center coords
    x += (w/2) - 3;
    y += (h/2) - 3;

    // now draw it
    QPen savePen = p->pen();
    QPointArray a(sizeof(circleshadow)/(sizeof(QCOORD)*2), circleshadow);
    a.translate(x, y);
    p->setPen(g.shadow());
    p->drawPoints(a);
    a.setPoints(sizeof(circledark)/(sizeof(QCOORD)*2), circledark);
    a.translate(x, y);
    p->setPen(g.dark());
    p->drawPoints(a);
    a.setPoints(sizeof(circlelight)/(sizeof(QCOORD)*2), circlelight);
    a.translate(x, y);
    p->setPen(g.light());
    p->drawPoints(a);
    p->setPen(savePen);
}

void StepStyle::drawCheckMark(QPainter *p, int x, int y, int w, int h,
            const QColorGroup &g, bool /*act*/, bool dis)
{
    // the nextStep style checkmark (9x10)
    static QCOORD checkshadow[] = {
        8,2, 7,3, 6,4, 5,5, 4,6, 3,7, 2,8, 1,9, 0,9,
        1,4, 1,5, 1,6
    };
    static QCOORD checkmid[] = {
        8,1, 7,2, 6,3, 5,4, 4,5, 3,6, 2,7, 1,8
    };
    static QCOORD checklight[] = {
        8,0, 7,1, 6,2, 5,3, 4,4, 3,5, 2,6, 1,7, 0,8,
        0,7, 0,6, 0,5, 0,4
    };

    // get coords for check
    x += (w/2) - 4 - 1;
    y += (h/2) - 5 - 1;

    // now draw it
    QPen savePen = p->pen();
    QPointArray a(sizeof(checkshadow)/(sizeof(QCOORD)*2), checkshadow);
    a.translate(x, y);
    (dis) ? p->setPen(g.mid()) : p->setPen(g.shadow());
    p->drawPoints(a);
    a.setPoints(sizeof(checkmid)/(sizeof(QCOORD)*2), checkmid);
    a.translate(x, y);
    (dis) ? p->setPen(g.mid()) : p->setPen(g.mid());
    p->drawPoints(a);
    a.setPoints(sizeof(checklight)/(sizeof(QCOORD)*2), checklight);
    a.translate(x, y);
    (dis) ? p->setPen(g.mid()) : p->setPen(g.light());
    p->drawPoints(a);
    p->setPen(savePen);
}


#include "moc_Themes.cpp"
