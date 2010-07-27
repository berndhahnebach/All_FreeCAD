/***************************************************************************
 *   Copyright (c) 2010 Werner Mayer <wmayer[at]users.sourceforge.net>     *
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
# include <Standard_math.hxx>
# include <BRep_Builder.hxx>
# include <BRepAlgoAPI_Section.hxx>
# include <BRepBuilderAPI_MakeWire.hxx>
# include <TopoDS.hxx>
# include <TopoDS_Compound.hxx>
# include <TopExp_Explorer.hxx>
# include <gp_Pln.hxx>
# include <cfloat>
# include <QFuture>
# include <QFutureWatcher>
# include <QtConcurrentMap>
# include <boost/bind.hpp>
# include <Inventor/nodes/SoBaseColor.h>
# include <Inventor/nodes/SoCoordinate3.h>
# include <Inventor/nodes/SoDrawStyle.h>
# include <Inventor/nodes/SoFaceSet.h>
# include <Inventor/nodes/SoLineSet.h>
# include <Inventor/nodes/SoSeparator.h>
#endif

#include "ui_CrossSections.h"
#include "CrossSections.h"
#include <Mod/Part/App/PartFeature.h>
#include <Gui/BitmapFactory.h>
#include <Gui/ViewProvider.h>
#include <Gui/Application.h>
#include <Gui/Document.h>
#include <Gui/View3DInventor.h>
#include <Gui/View3DInventorViewer.h>
#include <Base/Sequencer.h>

using namespace PartGui;
#undef CS_FUTURE // multi-threading causes some problems

namespace PartGui {
class ViewProviderCrossSections : public Gui::ViewProvider
{
public:
    ViewProviderCrossSections()
    {
        coords = new SoCoordinate3();
        coords->ref();
        planes = new SoLineSet();
        planes->ref();
        SoBaseColor* color = new SoBaseColor();
        color->rgb.setValue(1.0f, 0.447059f, 0.337255f);
        SoDrawStyle* style = new SoDrawStyle();
        style->lineWidth.setValue(2.0f);
        this->pcRoot->addChild(color);
        this->pcRoot->addChild(style);
        this->pcRoot->addChild(coords);
        this->pcRoot->addChild(planes);
    }
    ~ViewProviderCrossSections()
    {
        coords->unref();
        planes->unref();
    }
    void updateData(const App::Property*)
    {
    }
    const char* getDefaultDisplayMode() const
    {
        return "";
    }
    std::vector<std::string> getDisplayModes(void) const
    {
        return std::vector<std::string>();
    }
    void setCoords(const std::vector<Base::Vector3f>& v)
    {
        coords->point.setNum(v.size());
        SbVec3f* p = coords->point.startEditing();
        for (unsigned int i=0; i<v.size(); i++) {
            const Base::Vector3f& pt = v[i];
            p[i].setValue(pt.x,pt.y,pt.z);
        }
        coords->point.finishEditing();
        unsigned int count = v.size()/5;
        planes->numVertices.setNum(count);
        int32_t* l = planes->numVertices.startEditing();
        for (unsigned int i=0; i<count; i++) {
            l[i] = 5;
        }
        planes->numVertices.finishEditing();
    }

private:
    SoCoordinate3* coords;
    SoLineSet* planes;
};

#if 0 // per shape
class CrossSection
{
public:
    CrossSection(float a, float b, float c, const std::vector<float>& d)
        : a(a), b(b), c(c), d(d)
    {
    }

    std::pair<std::string, Part::TopoShape> section(Part::Feature* o) const
    {
        TopoDS_Compound comp;
        BRep_Builder builder;
        builder.MakeCompound(comp);
        for (std::vector<float>::const_iterator it = d.begin(); it != d.end(); ++it) {
            BRepAlgoAPI_Section cs(o->Shape.getValue(), gp_Pln(-a,-b,-c,*it));
            if (cs.IsDone()) {
                std::list<TopoDS_Edge> edges;
                TopExp_Explorer xp;
                for (xp.Init(cs.Shape(), TopAbs_EDGE); xp.More(); xp.Next())
                    edges.push_back(TopoDS::Edge(xp.Current()));
                std::list<TopoDS_Wire> wires;
                connectEdges(edges, wires);
                for (std::list<TopoDS_Wire>::iterator wt = wires.begin(); wt != wires.end(); ++wt)
                    builder.Add(comp, *wt);
            }
        }

        Part::TopoShape s;
        s._Shape = comp;
        return std::make_pair<std::string, Part::TopoShape>
            (o->getNameInDocument(), s);
    }

    void connectEdges (const std::list<TopoDS_Edge>& edges, std::list<TopoDS_Wire>& wires) const
    {
        std::list<TopoDS_Edge> edge_list = edges;
        while (edge_list.size() > 0) {
            BRepBuilderAPI_MakeWire mkWire;
            // add and erase first edge
            mkWire.Add(edge_list.front());
            edge_list.erase(edge_list.begin());

            TopoDS_Wire new_wire = mkWire.Wire();  // current new wire

            // try to connect each edge to the wire, the wire is complete if no more egdes are connectible
            bool found = false;
            do {
                found = false;
                for (std::list<TopoDS_Edge>::iterator pE = edge_list.begin(); !found && pE != edge_list.end();++pE) {
                    mkWire.Add(*pE);
                    if (mkWire.Error() != BRepBuilderAPI_DisconnectedWire) {
                        // edge added ==> remove it from list
                        found = true;
                        edge_list.erase(pE);
                        new_wire = mkWire.Wire();
                    }
                }
            }
            while (found);
            wires.push_back(new_wire);
        }
    }

private:
    float a,b,c;
    const std::vector<float>& d;
};
#endif

class CrossSection // per section
{
public:
    CrossSection(float a, float b, float c, const TopoDS_Shape& s)
        : a(a), b(b), c(c), s(s)
    {
    }

    std::list<TopoDS_Wire> section(float d) const
    {
        std::list<TopoDS_Wire> wires;
        BRepAlgoAPI_Section cs(s, gp_Pln(a,b,c,-d));
        if (cs.IsDone()) {
            std::list<TopoDS_Edge> edges;
            TopExp_Explorer xp;
            for (xp.Init(cs.Shape(), TopAbs_EDGE); xp.More(); xp.Next())
                edges.push_back(TopoDS::Edge(xp.Current()));
            connectEdges(edges, wires);
        }

        return wires;
    }

    void connectEdges (const std::list<TopoDS_Edge>& edges, std::list<TopoDS_Wire>& wires) const
    {
        std::list<TopoDS_Edge> edge_list = edges;
        while (edge_list.size() > 0) {
            BRepBuilderAPI_MakeWire mkWire;
            // add and erase first edge
            mkWire.Add(edge_list.front());
            edge_list.erase(edge_list.begin());

            TopoDS_Wire new_wire = mkWire.Wire();  // current new wire

            // try to connect each edge to the wire, the wire is complete if no more egdes are connectible
            bool found = false;
            do {
                found = false;
                for (std::list<TopoDS_Edge>::iterator pE = edge_list.begin(); pE != edge_list.end();++pE) {
                    mkWire.Add(*pE);
                    if (mkWire.Error() != BRepBuilderAPI_DisconnectedWire) {
                        // edge added ==> remove it from list
                        found = true;
                        edge_list.erase(pE);
                        new_wire = mkWire.Wire();
                        break;
                    }
                }
            }
            while (found);
            wires.push_back(new_wire);
        }
    }

private:
    float a,b,c;
    const TopoDS_Shape& s;
};
}

CrossSections::CrossSections(const Base::BoundBox3f& bb, QWidget* parent, Qt::WFlags fl)
  : QDialog(parent, fl), bbox(bb)
{
    ui = new Ui_CrossSections();
    ui->setupUi(this);
    ui->position->setRange(-DBL_MAX, DBL_MAX);
    vp = new ViewProviderCrossSections();

    Base::Vector3f c = bbox.CalcCenter();
    calcPlane(CrossSections::XY, c.z);
    ui->position->setValue(c.z);

    Gui::Document* doc = Gui::Application::Instance->activeDocument();
    view = qobject_cast<Gui::View3DInventor*>(doc->getActiveView());
    if (view) {
        view->getViewer()->addViewProvider(vp);
    }
}

/*  
 *  Destroys the object and frees any allocated resources
 */
CrossSections::~CrossSections()
{
    // no need to delete child widgets, Qt does it all for us
    delete ui;
    if (view) {
        view->getViewer()->removeViewProvider(vp);
    }
    delete vp;
}

CrossSections::Plane CrossSections::plane() const
{
    if (ui->xyPlane->isChecked())
        return CrossSections::XY;
    else if (ui->xzPlane->isChecked())
        return CrossSections::XZ;
    else
        return CrossSections::YZ;
}

void CrossSections::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    else {
        QDialog::changeEvent(e);
    }
}

void CrossSections::accept()
{
    apply();
    QDialog::accept();
}

void CrossSections::apply()
{
#if 1
    std::vector<App::DocumentObject*> obj = Gui::Selection().
        getObjectsOfType(Part::Feature::getClassTypeId());

    std::vector<float> d;
    if (ui->sectionsBox->isChecked())
        d = getPlanes();
    else
        d.push_back((float)ui->position->value());
    float a=0,b=0,c=0;
    switch (plane()) {
        case CrossSections::XY:
            c = 1.0;
            break;
        case CrossSections::XZ:
            b = 1.0;
            break;
        case CrossSections::YZ:
            a = 1.0;
            break;
    }

#ifdef CS_FUTURE
    Standard::SetReentrant(Standard_True);
#endif

    Base::SequencerLauncher seq("Cross-sections...", obj.size() * (d.size() +1));
    for (std::vector<App::DocumentObject*>::iterator it = obj.begin(); it != obj.end(); ++it) {
        CrossSection cs(a,b,c,static_cast<Part::Feature*>(*it)->Shape.getValue());
#ifdef CS_FUTURE
        QFuture< std::list<TopoDS_Wire> > future = QtConcurrent::mapped
            (d, boost::bind(&CrossSection::section, &cs, _1));
        future.waitForFinished();
        QFuture< std::list<TopoDS_Wire> >::const_iterator ft;
#else
        std::vector< std::list<TopoDS_Wire> > future;
        for (std::vector<float>::iterator jt = d.begin(); jt != d.end(); ++jt) {
            future.push_back(cs.section(*jt));
            seq.next();
        }
        std::vector< std::list<TopoDS_Wire> >::const_iterator ft;
#endif

        TopoDS_Compound comp;
        BRep_Builder builder;
        builder.MakeCompound(comp);

        for (ft = future.begin(); ft != future.end(); ++ft) {
            const std::list<TopoDS_Wire>& w = *ft;
            for (std::list<TopoDS_Wire>::const_iterator wt = w.begin(); wt != w.end(); ++wt) {
                builder.Add(comp, *wt);
            }
        }

        App::Document* doc = (*it)->getDocument();
        std::string s = (*it)->getNameInDocument();
        s += "_cs";
        Part::Feature* section = static_cast<Part::Feature*>
            (doc->addObject("Part::Feature",s.c_str()));
        section->Shape.setValue(comp);
        section->purgeTouched();

        seq.next();
    }

#else
    std::vector<Part::Feature*> shapes;
    for (std::vector<App::DocumentObject*>::iterator it = obj.begin(); it != obj.end(); ++it)
        shapes.push_back(static_cast<Part::Feature*>(*it));

    CrossSection cs(a,b,c,d);

#if CS_FUTURE
    Standard::SetReentrant(Standard_True);
    QFuture< std::pair<std::string, Part::TopoShape> > future = QtConcurrent::mapped
        (shapes, boost::bind(&CrossSection::section, &cs, _1));
    future.waitForFinished();
    QFuture< std::pair<std::string, Part::TopoShape> >::const_iterator ft;
#else
    std::vector< std::pair<std::string, Part::TopoShape> > future;
    for (std::vector<Part::Feature*>::iterator it = shapes.begin(); it != shapes.end(); ++it)
        future.push_back(cs.section(*it));
    std::vector< std::pair<std::string, Part::TopoShape> >::const_iterator ft;
#endif

    App::Document* doc = App::GetApplication().getActiveDocument();
    for (ft = future.begin(); ft != future.end(); ++ft) {
        std::string s = ft->first + "_cs";
        Part::Feature* shape = static_cast<Part::Feature*>
            (doc->addObject("Part::Feature",s.c_str()));
        shape->Shape.setValue(ft->second);
        shape->purgeTouched();
    }
#endif
}

void CrossSections::on_xyPlane_clicked()
{
    Base::Vector3f c = bbox.CalcCenter();
    ui->position->setValue(c.z);
    if (!ui->sectionsBox->isChecked()) {
        calcPlane(CrossSections::XY, c.z);
    }
    else {
        float dist = bbox.LengthZ() / ui->countSections->value();
        if (!ui->checkBothSides->isChecked())
            dist *= 0.5f;
        ui->distance->setValue(dist);
        calcPlanes(CrossSections::XY);
    }
}

void CrossSections::on_xzPlane_clicked()
{
    Base::Vector3f c = bbox.CalcCenter();
    ui->position->setValue(c.y);
    if (!ui->sectionsBox->isChecked()) {
        calcPlane(CrossSections::XZ, c.y);
    }
    else {
        float dist = bbox.LengthY() / ui->countSections->value();
        if (!ui->checkBothSides->isChecked())
            dist *= 0.5f;
        ui->distance->setValue(dist);
        calcPlanes(CrossSections::XZ);
    }
}

void CrossSections::on_yzPlane_clicked()
{
    Base::Vector3f c = bbox.CalcCenter();
    ui->position->setValue(c.x);
    if (!ui->sectionsBox->isChecked()) {
        calcPlane(CrossSections::YZ, c.x);
    }
    else {
        float dist = bbox.LengthX() / ui->countSections->value();
        if (!ui->checkBothSides->isChecked())
            dist *= 0.5f;
        ui->distance->setValue(dist);
        calcPlanes(CrossSections::YZ);
    }
}

void CrossSections::on_position_valueChanged(double v)
{
    if (!ui->sectionsBox->isChecked()) {
        calcPlane(plane(), v);
    }
    else {
        calcPlanes(plane());
    }
}

void CrossSections::on_sectionsBox_toggled(bool b)
{
    if (b) {
        on_countSections_valueChanged(ui->countSections->value());
    }
    else {
        CrossSections::Plane type = plane();
        Base::Vector3f c = bbox.CalcCenter();
        float value;
        switch (type) {
            case CrossSections::XY:
                value = c.z;
                break;
            case CrossSections::XZ:
                value = c.y;
                break;
            case CrossSections::YZ:
                value = c.x;
                break;
        }

        ui->position->setValue(value);
        calcPlane(type, value);
    }
}

void CrossSections::on_checkBothSides_toggled(bool b)
{
    double d = ui->distance->value();
    d = b ? 2.0 * d : 0.5 * d;
    ui->distance->setValue(d);
    calcPlanes(plane());
}

void CrossSections::on_countSections_valueChanged(int v)
{
    CrossSections::Plane type = plane();
    float dist;
    switch (type) {
        case CrossSections::XY:
            dist = bbox.LengthZ() / v;
            break;
        case CrossSections::XZ:
            dist = bbox.LengthY() / v;
            break;
        case CrossSections::YZ:
            dist = bbox.LengthX() / v;
            break;
    }
    if (!ui->checkBothSides->isChecked())
        dist *= 0.5f;
    ui->distance->setValue(dist);
    calcPlanes(type);
}

void CrossSections::on_distance_valueChanged(double)
{
    calcPlanes(plane());
}

void CrossSections::calcPlane(Plane type, double pos)
{
    float bound[4];
    switch (type) {
        case XY:
            bound[0] = bbox.MinX;
            bound[1] = bbox.MaxX;
            bound[2] = bbox.MinY;
            bound[3] = bbox.MaxY;
            break;
        case XZ:
            bound[0] = bbox.MinX;
            bound[1] = bbox.MaxX;
            bound[2] = bbox.MinZ;
            bound[3] = bbox.MaxZ;
            break;
        case YZ:
            bound[0] = bbox.MinY;
            bound[1] = bbox.MaxY;
            bound[2] = bbox.MinZ;
            bound[3] = bbox.MaxZ;
            break;
    }

    std::vector<float> d;
    d.push_back(pos);
    makePlanes(type, d, bound);
}

void CrossSections::calcPlanes(Plane type)
{
    float bound[4];
    switch (type) {
        case XY:
            bound[0] = bbox.MinX;
            bound[1] = bbox.MaxX;
            bound[2] = bbox.MinY;
            bound[3] = bbox.MaxY;
            break;
        case XZ:
            bound[0] = bbox.MinX;
            bound[1] = bbox.MaxX;
            bound[2] = bbox.MinZ;
            bound[3] = bbox.MaxZ;
            break;
        case YZ:
            bound[0] = bbox.MinY;
            bound[1] = bbox.MaxY;
            bound[2] = bbox.MinZ;
            bound[3] = bbox.MaxZ;
            break;
    }

    std::vector<float> d = getPlanes();
    makePlanes(type, d, bound);
}

std::vector<float> CrossSections::getPlanes() const
{
    int count = ui->countSections->value();
    float pos = (float)ui->position->value();
    float stp = (float)ui->distance->value();
    bool both = ui->checkBothSides->isChecked();

    std::vector<float> d;
    if (both) {
        float start = pos-0.5f*(count-1)*stp;
        for (int i=0; i<count; i++) {
            d.push_back(start+(float)i*stp);
        }
    }
    else {
        for (int i=0; i<count; i++) {
            d.push_back(pos+(float)i*stp);
        }
    }
    return d;
}

void CrossSections::makePlanes(Plane type, const std::vector<float>& d, float bound[4])
{
    std::vector<Base::Vector3f> points;
    for (std::vector<float>::const_iterator it = d.begin(); it != d.end(); ++it) {
        Base::Vector3f v[4];
        switch (type) {
            case XY:
                v[0].Set(bound[0],bound[2],*it);
                v[1].Set(bound[1],bound[2],*it);
                v[2].Set(bound[1],bound[3],*it);
                v[3].Set(bound[0],bound[3],*it);
                break;
            case XZ:
                v[0].Set(bound[0],*it,bound[2]);
                v[1].Set(bound[1],*it,bound[2]);
                v[2].Set(bound[1],*it,bound[3]);
                v[3].Set(bound[0],*it,bound[3]);
                break;
            case YZ:
                v[0].Set(*it,bound[0],bound[2]);
                v[1].Set(*it,bound[1],bound[2]);
                v[2].Set(*it,bound[1],bound[3]);
                v[3].Set(*it,bound[0],bound[3]);
                break;
        }

        points.push_back(v[0]);
        points.push_back(v[1]);
        points.push_back(v[2]);
        points.push_back(v[3]);
        points.push_back(v[0]);
    }
    vp->setCoords(points);
}

// ---------------------------------------

TaskCrossSections::TaskCrossSections(const Base::BoundBox3f& bb)
{
    widget = new CrossSections(bb);
    taskbox = new Gui::TaskView::TaskBox(
        Gui::BitmapFactory().pixmap("Part_Section"),
        widget->windowTitle(), true, 0);
    taskbox->groupLayout()->addWidget(widget);
    Content.push_back(taskbox);
}

TaskCrossSections::~TaskCrossSections()
{
    // automatically deleted in the sub-class
}

bool TaskCrossSections::accept()
{
    widget->accept();
    return (widget->result() == QDialog::Accepted);
}

void TaskCrossSections::clicked(int id)
{
    if (id == QDialogButtonBox::Apply) {
        widget->apply();
    }
}

#include "moc_CrossSections.cpp"
