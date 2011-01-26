/***************************************************************************
 *   Copyright (c) 2011 Werner Mayer <wmayer[at]users.sourceforge.net>     *
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
# include <QMenu>
# include <QMessageBox>
#endif

#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/lists/SoPickedPointList.h> 
#include <Inventor/details/SoFaceDetail.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/errors/SoDebugError.h>

#include <Base/Exception.h>
#include <App/PropertyLinks.h>
#include <App/GeoFeature.h>
#include <Gui/Application.h>
#include <Gui/Document.h>
#include <Gui/MainWindow.h>
#include <Gui/SoFCColorBar.h>
#include <Gui/SoFCSelection.h>
#include <Gui/ViewProviderGeometryObject.h>
#include <Gui/View3DInventorViewer.h>
#include <Gui/Widgets.h>
#include <Mod/Mesh/App/MeshFeature.h>
#include <Mod/Mesh/App/Core/Elements.h>
//#include <Mod/Points/App/PointsFeature.h>
#include <Mod/Inspection/App/InspectionFeature.h>

#include "ViewProviderInspection.h"


using namespace InspectionGui;


bool ViewProviderInspection::addflag = false;

PROPERTY_SOURCE(InspectionGui::ViewProviderInspection, Gui::ViewProviderDocumentObject)

ViewProviderInspection::ViewProviderInspection() : search_radius(FLT_MAX)
{
    ADD_PROPERTY_TYPE(OutsideGrayed,(false),"",(App::PropertyType) (App::Prop_Output|App::Prop_Hidden),"");
    pcColorRoot = new SoSeparator();
    pcColorRoot->ref();
    pcColorMat = new SoMaterial;
    pcColorMat->ref();
    pcColorStyle = new SoDrawStyle(); 
    pcColorRoot->addChild(pcColorStyle);
    // simple color bar
    pcColorBar = new Gui::SoFCColorBar;
    pcColorBar->Attach(this);
    pcColorBar->ref();
    pcColorBar->setRange( -0.1f, 0.1f, 3 );
    pcLinkRoot = new SoGroup;
    pcLinkRoot->ref();
}

ViewProviderInspection::~ViewProviderInspection()
{
    pcColorRoot->unref();
    pcColorMat->unref();
    pcColorBar->Detach(this);
    pcColorBar->unref();
    pcLinkRoot->unref();
}

void ViewProviderInspection::onChanged(const App::Property* prop)
{
    if (prop == &OutsideGrayed) {
        if (pcColorBar) {
            pcColorBar->setOutsideGrayed(OutsideGrayed.getValue());
            pcColorBar->Notify(0);
        }
    }
    else {
        inherited::onChanged(prop);
    }
}

void ViewProviderInspection::hide(void)
{
    inherited::hide();
    pcColorStyle->style = SoDrawStyle::INVISIBLE;
}

void ViewProviderInspection::show(void)
{
    inherited::show();
    pcColorStyle->style = SoDrawStyle::FILLED;
}

void ViewProviderInspection::attach(App::DocumentObject *pcFeat)
{
    // creats the standard viewing modes
    inherited::attach(pcFeat);
    attachDocument(pcFeat->getDocument());

    SoShapeHints * flathints = new SoShapeHints;
    flathints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE ;
    flathints->shapeType = SoShapeHints::UNKNOWN_SHAPE_TYPE;

    SoGroup* pcColorShadedRoot = new SoGroup();
    pcColorShadedRoot->addChild(flathints);

    // color shaded  ------------------------------------------
    SoDrawStyle *pcFlatStyle = new SoDrawStyle();
    pcFlatStyle->style = SoDrawStyle::FILLED;
    pcColorShadedRoot->addChild(pcFlatStyle);

    SoMaterialBinding* pcMatBinding = new SoMaterialBinding;
    pcMatBinding->value = SoMaterialBinding::PER_VERTEX_INDEXED;
    pcColorShadedRoot->addChild(pcColorMat);
    pcColorShadedRoot->addChild(pcMatBinding);
    pcColorShadedRoot->addChild(pcLinkRoot);

    addDisplayMaskMode(pcColorShadedRoot, "ColorShaded");

    // Check for an already existing color bar
    Gui::SoFCColorBar* pcBar = ((Gui::SoFCColorBar*)findFrontRootOfType(Gui::SoFCColorBar::getClassTypeId()));
    if (pcBar) {
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

    pcColorRoot->addChild(pcColorBar);
}

void ViewProviderInspection::updateData(const App::Property* prop)
{
    // set to the expected size
    if (prop->getTypeId() == App::PropertyLink::getClassTypeId()) {
        App::GeoFeature* object = static_cast<const App::PropertyLink*>(prop)->getValue<App::GeoFeature*>();
        this->pcLinkRoot->removeAllChildren();
        if (object) {
            // set the Distance property to the correct size to sync size of material node with number
            // of vertices/points of the referenced geometry
            if (object->getTypeId().isDerivedFrom(Mesh::Feature::getClassTypeId())) {
                const Mesh::MeshObject& kernel = static_cast<Mesh::Feature*>(object)->Mesh.getValue();
                static_cast<Inspection::Feature*>(pcObject)->Distances.setSize(kernel.countPoints());
                pcColorMat->diffuseColor.setNum((int)kernel.countPoints());
                pcColorMat->transparency.setNum((int)kernel.countPoints());
            }
            //else if (object->getTypeId().isDerivedFrom(Points::Feature::getClassTypeId())) {
            //    // TODO
            //}

            // get the view provider of the associated mesh feature
            App::Document* rDoc = pcObject->getDocument();
            Gui::Document* pDoc = Gui::Application::Instance->getDocument(rDoc);
            Gui::ViewProviderGeometryObject* view = static_cast<Gui::ViewProviderGeometryObject*>(pDoc->getViewProvider(object));
            this->pcLinkRoot->addChild(view->getHighlightNode());
        }
    }
    else if (prop->getTypeId() == App::PropertyFloatList::getClassTypeId()) {
        setDistances();
    }
    else if (prop->getTypeId() == App::PropertyFloat::getClassTypeId()) {
        if (strcmp(prop->getName(), "SearchRadius") == 0) {
            float fSearchRadius = ((App::PropertyFloat*)prop)->getValue();
            this->search_radius = fSearchRadius;
            pcColorBar->setRange( -fSearchRadius, fSearchRadius, 4 );
            pcColorBar->Notify(0);
        }
    }
}

void ViewProviderInspection::slotCreatedObject(const App::DocumentObject& Obj)
{
}

void ViewProviderInspection::slotDeletedObject(const App::DocumentObject& Obj)
{
}

void ViewProviderInspection::slotChangedObject(const App::DocumentObject& Obj, const App::Property& Prop)
{
    App::DocumentObject* object = static_cast<Inspection::Feature*>
        (pcObject)->Actual.getValue();
    // we get this for any object for that a property has changed. Thus, we must regard that object
    // which is linked by our link property
    if (object == &Obj) {
        // set the Distance property to the correct size to sync size of material node with number
        // of vertices/points of the referenced geometry
        if (object->getTypeId().isDerivedFrom(Mesh::Feature::getClassTypeId())) {
            const Mesh::PropertyMeshKernel& mesh = static_cast<Mesh::Feature*>(object)->Mesh;
            if ((&mesh) == (&Prop)) {
                const Mesh::MeshObject& kernel = mesh.getValue();
                static_cast<Inspection::Feature*>(pcObject)->Distances.setSize((int)kernel.countPoints());
                pcColorMat->diffuseColor.setNum((int)kernel.countPoints());
                pcColorMat->transparency.setNum((int)kernel.countPoints());
            }
        }
        //else if (object->getTypeId().isDerivedFrom(Points::Feature::getClassTypeId())) {
        //    // TODO
        //}
    }
}

void ViewProviderInspection::slotCreatedDocument(const App::Document& Doc)
{
}

void ViewProviderInspection::slotDeletedDocument(const App::Document& Doc)
{
}

SoSeparator* ViewProviderInspection::getFrontRoot(void) const
{
    return pcColorRoot;
    //return pcColorBar;
}

void ViewProviderInspection::setDistances()
{
    App::Property* pDistances = pcObject->getPropertyByName("Distances");
    if (!pDistances) {
        SoDebugError::post("ViewProviderInspection::setDistances", "Unknown property 'Distances'");
        return;
    }
    if (pDistances->getTypeId() != App::PropertyFloatList::getClassTypeId()) {
        SoDebugError::post("ViewProviderInspection::setDistances", 
            "Property 'Distances' has type %s (App::PropertyFloatList was expected)", pDistances->getTypeId().getName());
        return;
    }

    // distance values
    const std::vector<float>& fValues = ((App::PropertyFloatList*)pDistances)->getValues();
    if (pcColorMat->diffuseColor.getNum() != (int)fValues.size()) pcColorMat->diffuseColor.setNum((int)fValues.size());
    if (pcColorMat->transparency.getNum() != (int)fValues.size()) pcColorMat->transparency.setNum((int)fValues.size());

    SbColor * cols = pcColorMat->diffuseColor.startEditing();
    float   * tran = pcColorMat->transparency.startEditing();

    unsigned long j=0;
    for (std::vector<float>::const_iterator jt = fValues.begin(); jt != fValues.end(); ++jt, j++) {
        App::Color col = pcColorBar->getColor(*jt);
        cols[j] = SbColor(col.r, col.g, col.b);
        if (pcColorBar->isVisible(*jt)) {
            tran[j] = 0.0f;
        }
        else {
            tran[j] = 0.8f;
        }
    }

    pcColorMat->diffuseColor.finishEditing();
    pcColorMat->transparency.finishEditing();
}

QIcon ViewProviderInspection::getIcon() const
{
    // Get the icon of the view provider to the associated feature
    QIcon px = inherited::getIcon();
    App::Property* pActual = pcObject->getPropertyByName("Actual");
    if (pActual && pActual->getTypeId().isDerivedFrom( App::PropertyLink::getClassTypeId())) {
        App::DocumentObject* docobj = ((App::PropertyLink*)pActual)->getValue();
        if (docobj) {
            Gui::Document* doc = Gui::Application::Instance->getDocument(docobj->getDocument());
            Gui::ViewProvider* view = doc->getViewProvider(docobj);
            px = view->getIcon();
        }
    }

    return px;
}

void ViewProviderInspection::setDisplayMode(const char* ModeName)
{
    if (strcmp("Visual Inspection",ModeName)==0) {
        setDistances();
        setDisplayMaskMode("ColorShaded");
    }

    inherited::setDisplayMode(ModeName);
}

std::vector<std::string> ViewProviderInspection::getDisplayModes(void) const
{
    // add modes
    std::vector<std::string> StrList;
    StrList.push_back("Visual Inspection");
    return StrList;
}

void ViewProviderInspection::OnChange(Base::Subject<int> &rCaller,int rcReason)
{
    setActiveMode();
}

namespace InspectionGui {
// Proxy class that receives an asynchronous custom event
class ViewProviderProxyObject : public QObject
{
public:
    ViewProviderProxyObject(QWidget* w) : QObject(0), widget(w) {}
    ~ViewProviderProxyObject() {}
    void customEvent(QEvent *)
    {
        if (!widget.isNull()) {
            QList<Gui::Flag*> flags = widget->findChildren<Gui::Flag*>();
            if (!flags.isEmpty()) {
                int ret = QMessageBox::question(Gui::getMainWindow(),
                    QObject::tr("Remove annotations"),
                    QObject::tr("Do you want to remove all annotations?"),
                    QMessageBox::Yes,QMessageBox::No);
                if (ret == QMessageBox::Yes) {
                    for (QList<Gui::Flag*>::iterator it = flags.begin(); it != flags.end(); ++it)
                        (*it)->deleteLater();
                }
            }
        }

        this->deleteLater();
    }

private:
    QPointer<QWidget> widget;
};

void addFlag(Gui::View3DInventorViewer* view, const QString& text, const SoPickedPoint * point)
{
    Gui::Flag* flag = new Gui::Flag;
    QPalette p;
    p.setColor(QPalette::Window, QColor(85,0,127));
    p.setColor(QPalette::Text, QColor(220,220,220));
    flag->setPalette(p);
    flag->setText(text);
    flag->setOrigin(point->getPoint());
    view->addFlag(flag, Gui::FlagLayout::BottomLeft);
}
}

void ViewProviderInspection::inspectCallback(void * ud, SoEventCallback * n)
{
    Gui::View3DInventorViewer* view  = reinterpret_cast<Gui::View3DInventorViewer*>(n->getUserData());
    const SoEvent* ev = n->getEvent();
    if (ev->getTypeId() == SoMouseButtonEvent::getClassTypeId()) {
        const SoMouseButtonEvent * mbe = static_cast<const SoMouseButtonEvent *>(ev);

        // Mark all incoming mouse button events as handled, especially, to deactivate the selection node
        n->getAction()->setHandled();
        n->setHandled();
        if (mbe->getButton() == SoMouseButtonEvent::BUTTON2 && mbe->getState() == SoButtonEvent::UP) {
            n->setHandled();
            // context-menu
            QMenu menu;
            QAction* fl = menu.addAction(QObject::tr("Annotation"));
            fl->setCheckable(true);
            fl->setChecked(addflag);
            QAction* cl = menu.addAction(QObject::tr("Leave info mode"));
            QAction* id = menu.exec(QCursor::pos());
            if (fl == id) {
                addflag = fl->isChecked();
            }
            else if (cl == id) {
                // post an event to a proxy object to make sure to avoid problems
                // when opening a modal dialog
                QApplication::postEvent(
                    new ViewProviderProxyObject(view->getGLWidget()),
                    new QEvent(QEvent::User));
                view->setEditing(false);
                view->getWidget()->setCursor(QCursor(Qt::ArrowCursor));
                view->setRedirectToSceneGraph(false);
                view->removeEventCallback(SoButtonEvent::getClassTypeId(), inspectCallback);
            }
        }
        else if (mbe->getButton() == SoMouseButtonEvent::BUTTON1 && mbe->getState() == SoButtonEvent::UP) {
            const SoPickedPoint * point = n->getPickedPoint();
            if (point == NULL) {
                Base::Console().Message("No point picked.\n");
                return;
            }

            n->setHandled();

            // check if we have picked one a node of the view provider we are insterested in
            Gui::ViewProvider* vp = static_cast<Gui::ViewProvider*>(view->getViewProviderByPath(point->getPath()));
            if (vp && vp->getTypeId().isDerivedFrom(ViewProviderInspection::getClassTypeId())) {
                ViewProviderInspection* that = static_cast<ViewProviderInspection*>(vp);
                QString info = that->inspectDistance(point);
                Gui::getMainWindow()->setPaneText(1,info);
                if (addflag)
                    addFlag(view, info, point);
                else
                    Gui::ToolTip::showText(QCursor::pos(), info);
            }
            else {
                // the nearest picked point was not part of the view provider
                SoRayPickAction action(view->getViewportRegion());
                action.setPickAll(TRUE);
                action.setPoint(mbe->getPosition());
                action.apply(view->getSceneManager()->getSceneGraph());

                const SoPickedPointList& pps = action.getPickedPointList();
                for (int i=0; i<pps.getLength(); ++i) {
                    const SoPickedPoint * point = pps[i];
                    vp = static_cast<Gui::ViewProvider*>(view->getViewProviderByPath(point->getPath()));
                    if (vp && vp->getTypeId().isDerivedFrom(ViewProviderInspection::getClassTypeId())) {
                        ViewProviderInspection* that = static_cast<ViewProviderInspection*>(vp);
                        QString info = that->inspectDistance(point);
                        Gui::getMainWindow()->setPaneText(1,info);
                        if (addflag)
                            addFlag(view, info, point);
                        else
                            Gui::ToolTip::showText(QCursor::pos(), info);
                        break;
                    }
                }
            }
        }
    }
    // toogle between inspection and navigation mode
    else if (ev->getTypeId().isDerivedFrom(SoKeyboardEvent::getClassTypeId())) {
        const SoKeyboardEvent * const ke = static_cast<const SoKeyboardEvent *>(ev);
        if (ke->getState() == SoButtonEvent::DOWN &&
            ke->getKey() == SoKeyboardEvent::ESCAPE) {
            SbBool toggle = view->isRedirectedToSceneGraph();
            view->setRedirectToSceneGraph(!toggle);
            n->setHandled();
        }
    }
}

QString ViewProviderInspection::inspectDistance(const SoPickedPoint* pp) const
{
    QString info;
    const SoDetail* detail = pp->getDetail(pp->getPath()->getTail());
    if (detail && detail->getTypeId() == SoFaceDetail::getClassTypeId()) {
        // get the distances of the three points of the picked facet
        SoFaceDetail * facedetail = (SoFaceDetail *)detail;
        int index1 = facedetail->getPoint(0)->getCoordinateIndex();
        int index2 = facedetail->getPoint(1)->getCoordinateIndex();
        int index3 = facedetail->getPoint(2)->getCoordinateIndex();
        App::Property* pDistance = this->pcObject->getPropertyByName("Distances");
        if (pDistance && pDistance->getTypeId() == App::PropertyFloatList::getClassTypeId()) {
            App::PropertyFloatList* dist = (App::PropertyFloatList*)pDistance;
            float fVal1 = (*dist)[index1];
            float fVal2 = (*dist)[index2];
            float fVal3 = (*dist)[index3];
          
            App::Property* pActual = this->pcObject->getPropertyByName("Actual");
            if (pActual && pActual->getTypeId().isDerivedFrom( App::PropertyLink::getClassTypeId())) {
                // Search for the associated view provider
                Mesh::Feature* mesh = (Mesh::Feature*)((App::PropertyLink*)pActual)->getValue();
                const MeshCore::MeshKernel& rcMesh = mesh->Mesh.getValue().getKernel();
                // get the weights
                float w1, w2, w3;
                const SbVec3f& p = pp->getObjectPoint();
                rcMesh.GetFacet(facedetail->getFaceIndex()).Weights(Base::Vector3f(p[0], p[1], p[2]), w1, w2, w3);
          
                float fSearchRadius = this->search_radius;
                if (fVal1 > fSearchRadius || fVal2 > fSearchRadius || fVal3 > fSearchRadius) {
                    info = QObject::tr("Distance: > %1").arg(fSearchRadius);
                }
                else if (fVal1 < -fSearchRadius || fVal2 < -fSearchRadius || fVal3 < -fSearchRadius) {
                    info = QObject::tr("Distance: < %1").arg(-fSearchRadius);
                }
                else {
                    float fVal = w1*fVal1+w2*fVal2+w3*fVal3;
                    info = QObject::tr("Distance: %1").arg(fVal);
                }
            }
        }
    }
    else if (detail && detail->getTypeId() == SoPointDetail::getClassTypeId()) {
        // safe downward cast, know the type
        SoPointDetail * pointdetail = (SoPointDetail *)detail;

        // get the distance of the picked point
        int index = pointdetail->getCoordinateIndex();
        App::Property* prop = this->pcObject->getPropertyByName("Distances");
        if ( prop && prop->getTypeId() == App::PropertyFloatList::getClassTypeId() ) {
            App::PropertyFloatList* dist = (App::PropertyFloatList*)prop;
            float fVal = (*dist)[index];
            info = QObject::tr("Distance: %1").arg(fVal);
        }
    }

    return info;
}

// -----------------------------------------------

PROPERTY_SOURCE(InspectionGui::ViewProviderInspectionGroup, Gui::ViewProviderDocumentObjectGroup)


/**
 * Creates the view provider for an object group.
 */
ViewProviderInspectionGroup::ViewProviderInspectionGroup()
{
}

ViewProviderInspectionGroup::~ViewProviderInspectionGroup()
{
}

/**
 * Returns the pixmap for the opened list item.
 */
QIcon ViewProviderInspectionGroup::getIcon() const
{
    static const char * const ScanViewOpen[]={
        "16 16 10 1",
        "c c #000000",
        ". c None",
        "h c #808000",
        "# c #808080",
        "a c #ffff00",
        "r c #ff0000",
        "o c #ffff00",
        "g c #00ff00",
        "t c #00ffff",
        "b c #0000ff",
        "................",
        "...#####........",
        "..#hhhhh#.......",
        ".#hhhhhhh######.",
        ".#hhhhhhhhhhhh#c",
        ".#hhhhhhhhhhhh#c",
        ".#hhhhhhhhhhhh#c",
        "#############h#c",
        "#aaaaaaaaaa##h#c",
        "#aarroggtbbac##c",
        ".#arroggtbbaac#c",
        ".#aarroggtbbac#c",
        "..#aaaaaaaaaa#cc",
        "..#############c",
        "...ccccccccccccc",
        "................"};

    static const char * const ScanViewClosed[] = {
        "16 16 9 1",
        "c c #000000",
        ". c None",
        "# c #808080",
        "a c #ffff00",
        "r c #ff0000",
        "o c #ffff00",
        "g c #00ff00",
        "t c #00ffff",
        "b c #0000ff",
        "................",
        "...#####........",
        "..#aaaaa#.......",
        ".#aaaaaaa######.",
        ".#aaaaaaaaaaaa#c",
        ".#aarroggtbbaa#c",
        ".#aarroggtbbaa#c",
        ".#aarroggtbbaa#c",
        ".#aarroggtbbaa#c",
        ".#aarroggtbbaa#c",
        ".#aarroggtbbaa#c",
        ".#aarroggtbbaa#c",
        ".#aaaaaaaaaaaa#c",
        ".##############c",
        "..cccccccccccccc",
        "................"};

    QIcon groupIcon;
    groupIcon.addPixmap(QPixmap(ScanViewClosed), QIcon::Normal, QIcon::Off);
    groupIcon.addPixmap(QPixmap(ScanViewOpen), QIcon::Normal, QIcon::On);
    return groupIcon;
}
