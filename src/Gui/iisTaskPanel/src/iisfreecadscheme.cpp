/***************************************************************************
 *   Copyright (c) 2009 Jürgen Riegel <juergen.riegel@web.de>              *
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

