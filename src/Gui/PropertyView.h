/** \file PropertyView.h
 *  \brief Framework to show and work with all kind of properies
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 */

/***************************************************************************
 *   (c) Jürgen Riegel (juergen.riegel@web.de) 2003                        *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License (LGPL)   *
 *   as published by the Free Software Foundation; either version 2 of     *
 *   the License, or (at your option) any later version.                   *
 *   for detail see the LICENCE text file.                                 *
 *                                                                         *
 *   FreeCAD is distributed in the hope that it will be useful,            *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with FreeCAD; if not, write to the Free Software        *
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
 *   USA                                                                   *
 *                                                                         *
 *   Juergen Riegel 2002                                                   *
 ***************************************************************************/


#ifndef __PROPTERYVIEW_H__
#define __PROPTERYVIEW_H__

#include "View.h"

#ifndef _PreComp_
#if QT_VERSION < 300
#	include <qlist.h>
#  else
#	include <qptrlist.h>
#  endif
#endif

class QPixmap;
class FCProperty;
class KexiPropertyEditor;

/** A test class. A more elaborate class description.
 */
class FCPropertyView :public FCDockView
{
  public:
       
	  /**
	   * A constructor.
	   * A more elaborate description of the constructor.
	   */
	  FCPropertyView(FCGuiDocument*  pcDocument,QWidget *parent=0,const char *name=0);

	  /**
	   * A destructor.
	   * A more elaborate description of the destructor.
	   */
	  virtual ~FCPropertyView();

  	bool OnMsg(const char* pMsg);

	  virtual const char *GetName(void){return "PropertyView";}

	  /// is called when the above function is called to handle document change stuff
	  virtual void OnNewDocument(FCGuiDocument* pcOldDocument,FCGuiDocument* pcNewDocument);
	  /// get called when the document is changed or updated
	  virtual void Update(void);

  	static QPixmap *pcLabelOpen, *pcLabelClosed, *pcAttribute;

  private:
    KexiPropertyEditor * _pPropEditor;;
};

#endif // __PROPTERYVIEW_H__
