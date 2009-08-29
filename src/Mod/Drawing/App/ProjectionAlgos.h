/***************************************************************************
 *   Copyright (c) J�rgen Riegel          (juergen.riegel@web.de) 2009     *
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

 


#ifndef _ProjectionAlgos_h_
#define _ProjectionAlgos_h_

#include <TopoDS_Shape.hxx>
#include <Base/Vector3D.h>
#include <string>

namespace Drawing
{


/** Algo class for projecting shapes and creat SVG out of it
 */
class AppDrawingExport ProjectionAlgos
{

public:
	/// Constructor
	ProjectionAlgos(const TopoDS_Shape &Input,const Base::Vector3f &Dir);
	virtual ~ProjectionAlgos();

	void ProjectionAlgos::execute(void);

	static std::string Edges2SVG(const TopoDS_Shape &);

	enum SvgExtractioType { 
		Plain,
		WithHidden
    };

	std::string getSVG(SvgExtractioType type);

	const TopoDS_Shape &Input;
	const Base::Vector3f &Direction;

    TopoDS_Shape V ;// hard edge visibly
    TopoDS_Shape V1;// Smoth edges visibly
    TopoDS_Shape VN;// contour edges visibly
    TopoDS_Shape VO;// contours apparents visibly
    TopoDS_Shape VI;// isoparamtriques   visibly
    TopoDS_Shape H ;// hard edge       invisibly
    TopoDS_Shape H1;// Smoth edges  invisibly
    TopoDS_Shape HN;// contour edges invisibly
    TopoDS_Shape HO;// contours apparents invisibly
    TopoDS_Shape HI;// isoparamtriques   invisibly



};


} //namespace Drawing



#endif
