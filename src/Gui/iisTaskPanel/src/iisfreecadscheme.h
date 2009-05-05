/***************************************************************************
 *                                                                         *
 *   Copyright: http://www.ii-system.com                                   *
 *   License:   LGPL                                                       *
 *                                                                         *
 ***************************************************************************/

#ifndef IISFREECADTASKPANELSCHEME_H
#define IISFREECADTASKPANELSCHEME_H

#include "iistaskpanelscheme.h"

#include "iistaskpanel_global.h"


class IISTASKPANEL_EXPORT iisFreeCADTaskPanelScheme : public iisTaskPanelScheme
{
public:
	iisFreeCADTaskPanelScheme(QObject *parent=0);
	~iisFreeCADTaskPanelScheme();

	static iisTaskPanelScheme* defaultScheme();

protected:
	static iisFreeCADTaskPanelScheme *myDefaultXPScheme;
};

#endif // IISFREECADTASKPANELSCHEME_H
