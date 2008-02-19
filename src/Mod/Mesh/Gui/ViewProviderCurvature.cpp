/***************************************************************************
 *   Copyright (c) 2004 Werner Mayer <werner.wm.mayer@gmx.de>              *
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


#include "PreCompiled.h"

#ifndef _PreComp_
# include <Inventor/SoPickedPoint.h>
# include <Inventor/details/SoFaceDetail.h>
# include <Inventor/details/SoPointDetail.h>
# include <Inventor/events/SoMouseButtonEvent.h>
# include <Inventor/nodes/SoDrawStyle.h>
# include <Inventor/nodes/SoIndexedFaceSet.h>
# include <Inventor/nodes/SoMaterial.h>
# include <Inventor/nodes/SoMaterialBinding.h>
# include <Inventor/nodes/SoShapeHints.h>
# include <algorithm>
#endif

// Here the FreeCAD includes sorted by Base,App,Gui......
#include <Base/Console.h>
#include <Base/Parameter.h>
#include <Base/Exception.h>
#include <Base/Sequencer.h>
#include <App/Application.h>
#include <App/Feature.h>
#include <Gui/Application.h>
#include <Gui/Document.h>
#include <Gui/MainWindow.h>
#include <Gui/Selection.h>
#include <Gui/SoFCSelection.h>
#include <Gui/SoFCColorBar.h>
#include <Gui/View3DInventorViewer.h>
#include <Gui/ViewProviderGeometryObject.h>

#include <Mod/Mesh/App/MeshProperties.h>
#include <Mod/Mesh/App/MeshFeature.h>

#include "ViewProvider.h"
#include "ViewProviderCurvature.h"

using namespace Mesh;
using namespace MeshGui;
using namespace std;


PROPERTY_SOURCE(MeshGui::ViewProviderMeshCurvature, Gui::ViewProviderDocumentObject)

ViewProviderMeshCurvature::ViewProviderMeshCurvature()
{
    pcColorMat = new SoMaterial;
    pcColorMat->ref();
    // simple color bar
    pcColorBar = new Gui::SoFCColorBar;
    pcColorBar->Attach(this);
    pcColorBar->ref();
    pcColorBar->setRange( -0.1f, 0.1f, 3 );
    pcLinkRoot = new SoGroup;
    pcLinkRoot->ref();
}

ViewProviderMeshCurvature::~ViewProviderMeshCurvature()
{
    pcColorMat->unref();
    pcColorBar->Detach(this);
    pcColorBar->unref();
    pcLinkRoot->unref();
}

void ViewProviderMeshCurvature::init(const Mesh::PropertyCurvatureList* pCurvInfo)
{
    std::vector<float> aMinValues, aMaxValues;
    const std::vector<Mesh::CurvatureInfo>& fCurvInfo = pCurvInfo->getValues();
    aMinValues.reserve(fCurvInfo.size());
    aMaxValues.reserve(fCurvInfo.size());

    for ( std::vector<Mesh::CurvatureInfo>::const_iterator jt=fCurvInfo.begin();jt!=fCurvInfo.end(); ++jt ) {
        aMinValues.push_back( jt->fMinCurvature );
        aMaxValues.push_back( jt->fMaxCurvature );
    }

    if ( aMinValues.empty() || aMaxValues.empty() ) 
        return; // no values inside

    float fMin = *std::min_element( aMinValues.begin(), aMinValues.end() );
    float fMax = *std::max_element( aMinValues.begin(), aMinValues.end() );

    // histogram over all values
    std::map<int, int> aHistogram;
    for ( std::vector<float>::const_iterator kt = aMinValues.begin(); kt != aMinValues.end(); ++kt ) {
        int grp = (int)(10.0f*( *kt - fMin )/( fMax - fMin ));
        aHistogram[grp]++;
    }

    float fRMin=-1.0f;
    for ( std::map<int, int>::iterator mIt = aHistogram.begin(); mIt != aHistogram.end(); ++mIt ) {
        if ( (float)mIt->second / (float)aMinValues.size() > 0.15f ) {
            fRMin = mIt->first * ( fMax - fMin )/10.0f + fMin;
            break;
        }
    }

    fMin = *std::min_element( aMaxValues.begin(), aMaxValues.end() );
    fMax = *std::max_element( aMaxValues.begin(), aMaxValues.end() );

    // histogram over all values
    aHistogram.clear();
    for ( std::vector<float>::const_iterator it2 = aMaxValues.begin(); it2 != aMaxValues.end(); ++it2 ) {
        int grp = (int)(10.0f*( *it2 - fMin )/( fMax - fMin ));
        aHistogram[grp]++;
    }

    float fRMax=1.0f;
    for ( std::map<int, int>::reverse_iterator rIt2 = aHistogram.rbegin(); rIt2 != aHistogram.rend(); ++rIt2 ) {
        if ( (float)rIt2->second / (float)aMaxValues.size() > 0.15f ) {
            fRMax = rIt2->first * ( fMax - fMin )/10.0f + fMin;
            break;
        }
    }

    float fAbs = std::max<float>(fabs(fRMin), fabs(fRMax));
    fRMin = -fAbs;
    fRMax =  fAbs;
    fMin = fRMin; fMax = fRMax;
    pcColorBar->setRange( fMin, fMax, 3 );
}

void ViewProviderMeshCurvature::attach(App::DocumentObject *pcFeat)
{
    // creats the standard viewing modes
    inherited::attach(pcFeat);

    SoShapeHints * flathints = new SoShapeHints;
    flathints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE ;
    flathints->shapeType = SoShapeHints::UNKNOWN_SHAPE_TYPE;

    SoGroup* pcColorShadedRoot = new SoGroup();
    pcColorShadedRoot->addChild(flathints);

    // color shaded
    SoDrawStyle *pcFlatStyle = new SoDrawStyle();
    pcFlatStyle->style = SoDrawStyle::FILLED;
    pcColorShadedRoot->addChild(pcFlatStyle);

    SoMaterialBinding* pcMatBinding = new SoMaterialBinding;
    pcMatBinding->value = SoMaterialBinding::PER_VERTEX_INDEXED;
    pcColorShadedRoot->addChild(pcColorMat);
    pcColorShadedRoot->addChild(pcMatBinding);
    pcColorShadedRoot->addChild(pcLinkRoot);

    addDisplayMaskMode(pcColorShadedRoot, "ColorShaded");
}

void ViewProviderMeshCurvature::updateData(const App::Property* prop)
{
    // set to the expected size
    if (prop->getTypeId() == App::PropertyLink::getClassTypeId()) {
        Mesh::Feature* object = static_cast<const App::PropertyLink*>(prop)->getValue<Mesh::Feature*>();
        this->pcLinkRoot->removeAllChildren();
        if (object) {
            const Mesh::MeshObject& kernel = object->Mesh.getValue();
            pcColorMat->diffuseColor.setNum((int)kernel.countPoints());
            pcColorMat->transparency.setNum((int)kernel.countPoints());

            // get the view provider of the associated mesh feature
            App::Document& rDoc = pcObject->getDocument();
            Gui::Document* pDoc = Gui::Application::Instance->getDocument(&rDoc);
            Gui::ViewProviderGeometryObject* view = static_cast<Gui::ViewProviderGeometryObject*>(pDoc->getViewProvider(object));
            this->pcLinkRoot->addChild(view->getHighlightNode());
        }
    }
    else if (prop->getTypeId() == Mesh::PropertyCurvatureList::getClassTypeId()) {
        const Mesh::PropertyCurvatureList* curv = static_cast<const Mesh::PropertyCurvatureList*>(prop);
        if (curv->getSize() < 3) return; // invalid array
        init(curv); // init color bar
        setActiveMode();
        // Check for an already existing color bar
        Gui::SoFCColorBar* pcBar = ((Gui::SoFCColorBar*)findFrontRootOfType( Gui::SoFCColorBar::getClassTypeId() ));
        if ( pcBar ) {
            float fMin = pcColorBar->getMinValue();
            float fMax = pcColorBar->getMaxValue();

            // Attach to the foreign color bar and delete our own bar
            pcBar->Attach(this);
            pcBar->ref();
            pcBar->setRange(fMin, fMax, 3);
            pcBar->Notify(0);
            pcColorBar->Detach(this);
            pcColorBar->unref();
            pcColorBar = pcBar;
        }
    }
}

SoSeparator* ViewProviderMeshCurvature::getFrontRoot(void) const
{
    return pcColorBar;
}

void ViewProviderMeshCurvature::setVertexCurvatureMode(int mode)
{
    Mesh::PropertyCurvatureList* pCurvInfo=0;
    std::map<std::string,App::Property*> Map;
    pcObject->getPropertyMap(Map);
    for( std::map<std::string,App::Property*>::iterator it = Map.begin(); it != Map.end(); ++it ) {
        Base::Type t = it->second->getTypeId();
        if ( t==Mesh::PropertyCurvatureList::getClassTypeId() ) {
            pCurvInfo = (Mesh::PropertyCurvatureList*)it->second;
            break;
        }
    }

    if ( !pCurvInfo )
        return; // cannot display this feature type due to missing curvature property

    // curvature values
    std::vector<float> fValues = pCurvInfo->getCurvature( mode ); 
    unsigned long j=0;
    for ( std::vector<float>::const_iterator jt = fValues.begin(); jt != fValues.end(); ++jt, j++ ) {
        App::Color col = pcColorBar->getColor( *jt );
        pcColorMat->diffuseColor.set1Value(j, SbColor(col.r, col.g, col.b));
        if ( pcColorBar->isVisible( *jt ) ) {
            pcColorMat->transparency.set1Value(j, 0.0f);
        } else {
            pcColorMat->transparency.set1Value(j, 0.8f);
        }
    }
}

QIcon ViewProviderMeshCurvature::getIcon() const
{
    static const char * Mesh_Feature_xpm[] = {
        "16 16 7 1",
        ".	c None",
        "#	c #000000",
        "s	c #BEC2FC",
        "g	c #00FF00",
        "y	c #FFFF00",
        "b	c #0000FF",
        "r	c #FF0000",
        ".......##.......",
        "....#######.....",
        "..##ggg#yyy#....",
        "##ggggg#yyyy##..",
        "#b#ggg#yyyyyy##.",
        "#bb#gg#yyyy###s.",
        "#bb#gg#yy##yy#s.",
        "#bbb#####rrr#ss.",
        "#bbbb##rrrr#ss..",
        ".#b##b#rrrr#s...",
        ".##bbb#rrr#ss...",
        ".##bbb#r#ss.....",
        "..s#####r#s.....",
        "....sss##ss.....",
        "........ss......",
        "................"};
    QPixmap px(Mesh_Feature_xpm);
    return px;
}

void ViewProviderMeshCurvature::setDisplayMode(const char* ModeName)
{
    if ( strcmp("Mean curvature",ModeName)==0 ) {
        setVertexCurvatureMode(Mesh::PropertyCurvatureList::MeanCurvature);
        setDisplayMaskMode("ColorShaded");
    }
    else if ( strcmp("Gaussian curvature",ModeName)==0  ) {
        setVertexCurvatureMode(Mesh::PropertyCurvatureList::GaussCurvature);
        setDisplayMaskMode("ColorShaded");
    }
    else if ( strcmp("Maximum curvature",ModeName)==0  ) {
        setVertexCurvatureMode(Mesh::PropertyCurvatureList::MaxCurvature);
        setDisplayMaskMode("ColorShaded");
    }
    else if ( strcmp("Minimum curvature",ModeName)==0  ) {
        setVertexCurvatureMode(Mesh::PropertyCurvatureList::MinCurvature);
        setDisplayMaskMode("ColorShaded");
    }
    else if ( strcmp("Absolute curvature",ModeName)==0  ) {
        setVertexCurvatureMode(Mesh::PropertyCurvatureList::AbsCurvature);
        setDisplayMaskMode("ColorShaded");
    }

    inherited::setDisplayMode(ModeName);
}

const char* ViewProviderMeshCurvature::getDefaultDisplayMode() const
{
    return "Absolute curvature";
}

std::vector<std::string> ViewProviderMeshCurvature::getDisplayModes(void) const
{
    std::vector<std::string> StrList = inherited::getDisplayModes();

    // add modes
    StrList.push_back("Absolute curvature");
    StrList.push_back("Mean curvature");
    StrList.push_back("Gaussian curvature");
    StrList.push_back("Maximum curvature");
    StrList.push_back("Minimum curvature");

    return StrList;
}

void ViewProviderMeshCurvature::OnChange(Base::Subject<int> &rCaller,int rcReason)
{
    setActiveMode();
}

void ViewProviderMeshCurvature::curvatureInfoCallback(void * ud, SoEventCallback * n)
{
    if (n->getEvent()->getTypeId() == SoMouseButtonEvent::getClassTypeId()) {
        const SoMouseButtonEvent * mbe = static_cast<const SoMouseButtonEvent *>(n->getEvent());
        Gui::View3DInventorViewer* view  = reinterpret_cast<Gui::View3DInventorViewer*>(n->getUserData());

        // Mark all incoming mouse button events as handled, especially, to deactivate the selection node
        n->getAction()->setHandled();
        if (mbe->getButton() == SoMouseButtonEvent::BUTTON2 && mbe->getState() == SoButtonEvent::UP) {
            n->setHandled();
            view->setEditing(false);
            view->getWidget()->setCursor(QCursor(Qt::ArrowCursor));
            view->removeEventCallback(SoMouseButtonEvent::getClassTypeId(), curvatureInfoCallback);
        }
        else if (mbe->getButton() == SoMouseButtonEvent::BUTTON1 && mbe->getState() == SoButtonEvent::DOWN) {
            const SoPickedPoint * point = n->getPickedPoint();
            if (point == NULL) {
                Base::Console().Message("No facet picked.\n");
                return;
            }

            n->setHandled();

            // By specifying the indexed mesh node 'pcFaceSet' we make sure that the picked point is
            // really from the mesh we render and not from any other geometry
            Gui::ViewProvider* vp = static_cast<Gui::ViewProvider*>(view->getViewProviderByPath(point->getPath()));
            if (!vp || !vp->getTypeId().isDerivedFrom(ViewProviderMeshCurvature::getClassTypeId()))
                return;
            ViewProviderMeshCurvature* that = static_cast<ViewProviderMeshCurvature*>(vp);
            const SoDetail* detail = point->getDetail(point->getPath()->getTail());
            if (detail && detail->getTypeId() == SoFaceDetail::getClassTypeId()) {
                // safe downward cast, know the type
                SoFaceDetail * facedetail = (SoFaceDetail *)detail;
                // get the curvature info of the three points of the picked facet
                int index1 = facedetail->getPoint(0)->getCoordinateIndex();
                int index2 = facedetail->getPoint(1)->getCoordinateIndex();
                int index3 = facedetail->getPoint(2)->getCoordinateIndex();
                that->curvatureInfo(index1, index2, index3);
            }
        }
    }
}

void ViewProviderMeshCurvature::curvatureInfo(int index1, int index2, int index3) const
{
    // get the curvature info of the three points of the picked facet
    App::Property* prop = pcObject->getPropertyByName("CurvInfo");
    if (prop && prop->getTypeId() == Mesh::PropertyCurvatureList::getClassTypeId()) {
        Mesh::PropertyCurvatureList* curv = (Mesh::PropertyCurvatureList*)prop;
        const Mesh::CurvatureInfo& cVal1 = (*curv)[index1];
        const Mesh::CurvatureInfo& cVal2 = (*curv)[index2];
        const Mesh::CurvatureInfo& cVal3 = (*curv)[index3];
        float fVal1 = 0.0f; float fVal2 = 0.0f; float fVal3 = 0.0f;

        bool print=true;
        QString mode = getActiveDisplayMode().c_str();
        if (mode == "Minimum curvature") {
            fVal1 = cVal1.fMinCurvature;
            fVal2 = cVal1.fMinCurvature;
            fVal3 = cVal1.fMinCurvature;
        }
        else if (mode == "Maximum curvature") {
            fVal1 = cVal1.fMaxCurvature;
            fVal2 = cVal1.fMaxCurvature;
            fVal3 = cVal1.fMaxCurvature;
        }
        else if (mode == "Gaussian curvature") {
            fVal1 = cVal1.fMaxCurvature*cVal1.fMinCurvature;
            fVal2 = cVal1.fMaxCurvature*cVal2.fMinCurvature;
            fVal3 = cVal1.fMaxCurvature*cVal3.fMinCurvature;
        }
        else if (mode == "Mean curvature") {
            fVal1 = 0.5f*(cVal1.fMaxCurvature+cVal1.fMinCurvature);
            fVal2 = 0.5f*(cVal1.fMaxCurvature+cVal2.fMinCurvature);
            fVal3 = 0.5f*(cVal1.fMaxCurvature+cVal3.fMinCurvature);
        }
        else if (mode == "Absolute curvature") {
            fVal1 = fabs(cVal1.fMaxCurvature) > fabs(cVal1.fMinCurvature) ? cVal1.fMaxCurvature : cVal1.fMinCurvature;
            fVal2 = fabs(cVal2.fMaxCurvature) > fabs(cVal2.fMinCurvature) ? cVal2.fMaxCurvature : cVal2.fMinCurvature;
            fVal3 = fabs(cVal3.fMaxCurvature) > fabs(cVal3.fMinCurvature) ? cVal3.fMaxCurvature : cVal3.fMinCurvature;
        }
        else {
            print = false;
        }

        if (print) {
            mode += QString(": <%1, %2, %3>").arg(fVal1).arg(fVal2).arg(fVal3);
        }
        else {
            mode = "No curvature mode set";
        }

        Gui::getMainWindow()->setPaneText(1,mode);
    }
}
