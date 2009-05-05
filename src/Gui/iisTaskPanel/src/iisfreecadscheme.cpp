/***************************************************************************
 *                                                                         *
 *   Copyright: http://www.ii-system.com                                   *
 *   License:   LGPL                                                       *
 *                                                                         *
 ***************************************************************************/

#include "iisfreecadscheme.h"


iisFreeCADTaskPanelScheme* iisFreeCADTaskPanelScheme::myDefaultXPScheme = 0;

iisFreeCADTaskPanelScheme::iisFreeCADTaskPanelScheme(QObject *parent)
	: iisTaskPanelScheme(parent)
{
	QLinearGradient panelBackgroundGrd(0,0, 0,300);
	panelBackgroundGrd.setColorAt(1, QColor(93,93,150));
	panelBackgroundGrd.setColorAt(0, QColor(207,210,255));
	panelBackground = panelBackgroundGrd;

	QLinearGradient headerBackgroundGrd(0,0, 300,0);
	headerBackgroundGrd.setColorAt(0, 0xffffff);
	headerBackgroundGrd.setColorAt(1, 0xc6d3f7);
	headerBackground = headerBackgroundGrd;

	headerBorder = QPen(Qt::NoPen);
	headerSize = 25;
	headerAnimation = false;

	headerLabelScheme.text = QColor(0x215dc6);
	headerLabelScheme.textOver = QColor(0x428eff);
	headerLabelScheme.iconSize = 22;

	headerButtonFold = QPixmap(":/Resources/headerButtonFold_XPBlue2.png");
	headerButtonFoldOver = QPixmap(":/Resources/headerButtonFoldOver_XPBlue2.png");
	headerButtonUnfold = QPixmap(":/Resources/headerButtonUnfold_XPBlue2.png");
	headerButtonUnfoldOver = QPixmap(":/Resources/headerButtonUnfoldOver_XPBlue2.png");
	headerButtonSize = QSize(17,17);

	groupBackground = QBrush(0xd6dff7); 
	groupBorder = QColor(0xffffff);

	taskLabelScheme.text = QColor(0x215dc6);
	taskLabelScheme.textOver = QColor(0x428eff);
}

iisFreeCADTaskPanelScheme::~iisFreeCADTaskPanelScheme()
{

}

iisTaskPanelScheme* iisFreeCADTaskPanelScheme::defaultScheme()	
{ 
	if (!myDefaultXPScheme)
		myDefaultXPScheme = new iisFreeCADTaskPanelScheme();

	return myDefaultXPScheme; 
}

