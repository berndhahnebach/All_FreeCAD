
// This file is generated by src/Tools/generateTemaplates/templateClassPyExport.py out of the XML file
// Every change you make here get lost at the next full rebuild!
#ifndef MESH_MESHPY_H
#define MESH_MESHPY_H

#include <App/ComplexGeoDataPy.h>

namespace Mesh
{

class MeshObject;

//===========================================================================
// MeshPy - Python wrapper
//===========================================================================

/** The python export class for MeshObject
 */
class MeshExport MeshPy : public Data::ComplexGeoDataPy
{
public:
    static PyTypeObject   Type;
    static PyMethodDef    Methods[];
    static PyGetSetDef    GetterSetter[];
    static PyParentObject Parents[];
    virtual PyTypeObject *GetType(void) {return &Type;};
    virtual PyParentObject *GetParents(void) {return Parents;}

public:
    MeshPy(MeshObject *pcMeshObjectObject, PyTypeObject *T = &Type);
    static PyObject *PyMake(PyTypeObject*, PyObject*, PyObject*);
    static int PyInit(PyObject*, PyObject*, PyObject*);
    ~MeshPy();

    /// sets this object to a new mesh kernel, the old will be deleted!
    void setMesh(MeshObject* pcMesh);
    MeshObject* getMesh(void) const;

    virtual PyObject *_repr(void);        // the representation
    const char *representation(void) const;

    /** @name callbacks and implementers for the python object methods */
    //@{
    /// callback for the read() method
    static PyObject * staticCallback_read (PyObject *self, PyObject *args, PyObject *kwd);
    /// implementer for the read() method
    PyObject*  read(PyObject *args);
    /// callback for the write() method
    static PyObject * staticCallback_write (PyObject *self, PyObject *args, PyObject *kwd);
    /// implementer for the write() method
    PyObject*  write(PyObject *args);
    /// callback for the offset() method
    static PyObject * staticCallback_offset (PyObject *self, PyObject *args, PyObject *kwd);
    /// implementer for the offset() method
    PyObject*  offset(PyObject *args);
    /// callback for the offsetSpecial() method
    static PyObject * staticCallback_offsetSpecial (PyObject *self, PyObject *args, PyObject *kwd);
    /// implementer for the offsetSpecial() method
    PyObject*  offsetSpecial(PyObject *args);
    /// callback for the unite() method
    static PyObject * staticCallback_unite (PyObject *self, PyObject *args, PyObject *kwd);
    /// implementer for the unite() method
    PyObject*  unite(PyObject *args);
    /// callback for the intersect() method
    static PyObject * staticCallback_intersect (PyObject *self, PyObject *args, PyObject *kwd);
    /// implementer for the intersect() method
    PyObject*  intersect(PyObject *args);
    /// callback for the difference() method
    static PyObject * staticCallback_difference (PyObject *self, PyObject *args, PyObject *kwd);
    /// implementer for the difference() method
    PyObject*  difference(PyObject *args);
    /// callback for the inner() method
    static PyObject * staticCallback_inner (PyObject *self, PyObject *args, PyObject *kwd);
    /// implementer for the inner() method
    PyObject*  inner(PyObject *args);
    /// callback for the outer() method
    static PyObject * staticCallback_outer (PyObject *self, PyObject *args, PyObject *kwd);
    /// implementer for the outer() method
    PyObject*  outer(PyObject *args);
    /// callback for the coarsen() method
    static PyObject * staticCallback_coarsen (PyObject *self, PyObject *args, PyObject *kwd);
    /// implementer for the coarsen() method
    PyObject*  coarsen(PyObject *args);
    /// callback for the translate() method
    static PyObject * staticCallback_translate (PyObject *self, PyObject *args, PyObject *kwd);
    /// implementer for the translate() method
    PyObject*  translate(PyObject *args);
    /// callback for the rotate() method
    static PyObject * staticCallback_rotate (PyObject *self, PyObject *args, PyObject *kwd);
    /// implementer for the rotate() method
    PyObject*  rotate(PyObject *args);
    /// callback for the transformToEigen() method
    static PyObject * staticCallback_transformToEigen (PyObject *self, PyObject *args, PyObject *kwd);
    /// implementer for the transformToEigen() method
    PyObject*  transformToEigen(PyObject *args);
    /// callback for the addFacet() method
    static PyObject * staticCallback_addFacet (PyObject *self, PyObject *args, PyObject *kwd);
    /// implementer for the addFacet() method
    PyObject*  addFacet(PyObject *args);
    /// callback for the addFacets() method
    static PyObject * staticCallback_addFacets (PyObject *self, PyObject *args, PyObject *kwd);
    /// implementer for the addFacets() method
    PyObject*  addFacets(PyObject *args);
    /// callback for the clear() method
    static PyObject * staticCallback_clear (PyObject *self, PyObject *args, PyObject *kwd);
    /// implementer for the clear() method
    PyObject*  clear(PyObject *args);
    /// callback for the isSolid() method
    static PyObject * staticCallback_isSolid (PyObject *self, PyObject *args, PyObject *kwd);
    /// implementer for the isSolid() method
    PyObject*  isSolid(PyObject *args);
    /// callback for the hasNonManifolds() method
    static PyObject * staticCallback_hasNonManifolds (PyObject *self, PyObject *args, PyObject *kwd);
    /// implementer for the hasNonManifolds() method
    PyObject*  hasNonManifolds(PyObject *args);
    /// callback for the removeNonManifolds() method
    static PyObject * staticCallback_removeNonManifolds (PyObject *self, PyObject *args, PyObject *kwd);
    /// implementer for the removeNonManifolds() method
    PyObject*  removeNonManifolds(PyObject *args);
    /// callback for the hasSelfIntersections() method
    static PyObject * staticCallback_hasSelfIntersections (PyObject *self, PyObject *args, PyObject *kwd);
    /// implementer for the hasSelfIntersections() method
    PyObject*  hasSelfIntersections(PyObject *args);
    /// callback for the fixSelfIntersections() method
    static PyObject * staticCallback_fixSelfIntersections (PyObject *self, PyObject *args, PyObject *kwd);
    /// implementer for the fixSelfIntersections() method
    PyObject*  fixSelfIntersections(PyObject *args);
    /// callback for the flipNormals() method
    static PyObject * staticCallback_flipNormals (PyObject *self, PyObject *args, PyObject *kwd);
    /// implementer for the flipNormals() method
    PyObject*  flipNormals(PyObject *args);
    /// callback for the hasNonUnifomOrientedFacets() method
    static PyObject * staticCallback_hasNonUnifomOrientedFacets (PyObject *self, PyObject *args, PyObject *kwd);
    /// implementer for the hasNonUnifomOrientedFacets() method
    PyObject*  hasNonUnifomOrientedFacets(PyObject *args);
    /// callback for the countNonUnifomOrientedFacets() method
    static PyObject * staticCallback_countNonUnifomOrientedFacets (PyObject *self, PyObject *args, PyObject *kwd);
    /// implementer for the countNonUnifomOrientedFacets() method
    PyObject*  countNonUnifomOrientedFacets(PyObject *args);
    /// callback for the harmonizeNormals() method
    static PyObject * staticCallback_harmonizeNormals (PyObject *self, PyObject *args, PyObject *kwd);
    /// implementer for the harmonizeNormals() method
    PyObject*  harmonizeNormals(PyObject *args);
    /// callback for the countComponents() method
    static PyObject * staticCallback_countComponents (PyObject *self, PyObject *args, PyObject *kwd);
    /// implementer for the countComponents() method
    PyObject*  countComponents(PyObject *args);
    /// callback for the removeComponents() method
    static PyObject * staticCallback_removeComponents (PyObject *self, PyObject *args, PyObject *kwd);
    /// implementer for the removeComponents() method
    PyObject*  removeComponents(PyObject *args);
    /// callback for the fillupHoles() method
    static PyObject * staticCallback_fillupHoles (PyObject *self, PyObject *args, PyObject *kwd);
    /// implementer for the fillupHoles() method
    PyObject*  fillupHoles(PyObject *args);
    /// callback for the fixIndices() method
    static PyObject * staticCallback_fixIndices (PyObject *self, PyObject *args, PyObject *kwd);
    /// implementer for the fixIndices() method
    PyObject*  fixIndices(PyObject *args);
    /// callback for the fixDeformations() method
    static PyObject * staticCallback_fixDeformations (PyObject *self, PyObject *args, PyObject *kwd);
    /// implementer for the fixDeformations() method
    PyObject*  fixDeformations(PyObject *args);
    /// callback for the fixDegenerations() method
    static PyObject * staticCallback_fixDegenerations (PyObject *self, PyObject *args, PyObject *kwd);
    /// implementer for the fixDegenerations() method
    PyObject*  fixDegenerations(PyObject *args);
    /// callback for the removeDuplicatedPoints() method
    static PyObject * staticCallback_removeDuplicatedPoints (PyObject *self, PyObject *args, PyObject *kwd);
    /// implementer for the removeDuplicatedPoints() method
    PyObject*  removeDuplicatedPoints(PyObject *args);
    /// callback for the removeDuplicatedFacets() method
    static PyObject * staticCallback_removeDuplicatedFacets (PyObject *self, PyObject *args, PyObject *kwd);
    /// implementer for the removeDuplicatedFacets() method
    PyObject*  removeDuplicatedFacets(PyObject *args);
    /// callback for the refine() method
    static PyObject * staticCallback_refine (PyObject *self, PyObject *args, PyObject *kwd);
    /// implementer for the refine() method
    PyObject*  refine(PyObject *args);
    /// callback for the optimizeTopology() method
    static PyObject * staticCallback_optimizeTopology (PyObject *self, PyObject *args, PyObject *kwd);
    /// implementer for the optimizeTopology() method
    PyObject*  optimizeTopology(PyObject *args);
    /// callback for the optimizeEdges() method
    static PyObject * staticCallback_optimizeEdges (PyObject *self, PyObject *args, PyObject *kwd);
    /// implementer for the optimizeEdges() method
    PyObject*  optimizeEdges(PyObject *args);
    /// callback for the splitEdges() method
    static PyObject * staticCallback_splitEdges (PyObject *self, PyObject *args, PyObject *kwd);
    /// implementer for the splitEdges() method
    PyObject*  splitEdges(PyObject *args);
    /// callback for the splitEdge() method
    static PyObject * staticCallback_splitEdge (PyObject *self, PyObject *args, PyObject *kwd);
    /// implementer for the splitEdge() method
    PyObject*  splitEdge(PyObject *args);
    /// callback for the splitFacet() method
    static PyObject * staticCallback_splitFacet (PyObject *self, PyObject *args, PyObject *kwd);
    /// implementer for the splitFacet() method
    PyObject*  splitFacet(PyObject *args);
    /// callback for the swapEdge() method
    static PyObject * staticCallback_swapEdge (PyObject *self, PyObject *args, PyObject *kwd);
    /// implementer for the swapEdge() method
    PyObject*  swapEdge(PyObject *args);
    /// callback for the collapseEdge() method
    static PyObject * staticCallback_collapseEdge (PyObject *self, PyObject *args, PyObject *kwd);
    /// implementer for the collapseEdge() method
    PyObject*  collapseEdge(PyObject *args);
    /// callback for the collapseFacet() method
    static PyObject * staticCallback_collapseFacet (PyObject *self, PyObject *args, PyObject *kwd);
    /// implementer for the collapseFacet() method
    PyObject*  collapseFacet(PyObject *args);
    /// callback for the collapseFacets() method
    static PyObject * staticCallback_collapseFacets (PyObject *self, PyObject *args, PyObject *kwd);
    /// implementer for the collapseFacets() method
    PyObject*  collapseFacets(PyObject *args);
    /// callback for the insertVertex() method
    static PyObject * staticCallback_insertVertex (PyObject *self, PyObject *args, PyObject *kwd);
    /// implementer for the insertVertex() method
    PyObject*  insertVertex(PyObject *args);
    /// callback for the snapVertex() method
    static PyObject * staticCallback_snapVertex (PyObject *self, PyObject *args, PyObject *kwd);
    /// implementer for the snapVertex() method
    PyObject*  snapVertex(PyObject *args);
    /// callback for the printInfo() method
    static PyObject * staticCallback_printInfo (PyObject *self, PyObject *args, PyObject *kwd);
    /// implementer for the printInfo() method
    PyObject*  printInfo(PyObject *args);
    //@}


    /** @name callbacks and implementers for the python object methods */
    //@{
    ///getter callback for the CountPoints attribute
    static PyObject * staticCallback_getCountPoints (PyObject *self, void *closure);
    /// getter for the CountPoints attribute
    Py::Int getCountPoints(void) const;
    /// setter callback for the CountPoints attribute
    static int staticCallback_setCountPoints (PyObject *self, PyObject *value, void *closure);
    // no setter method,  CountPoints is read only!
    ///getter callback for the CountFacets attribute
    static PyObject * staticCallback_getCountFacets (PyObject *self, void *closure);
    /// getter for the CountFacets attribute
    Py::Int getCountFacets(void) const;
    /// setter callback for the CountFacets attribute
    static int staticCallback_setCountFacets (PyObject *self, PyObject *value, void *closure);
    // no setter method,  CountFacets is read only!
    /// getter method for special attributes (e.g. dynamic ones)
    PyObject *getCustomAttributes(const char* attr) const;
    /// setter for special attributes (e.g. dynamic ones)
    int setCustomAttributes(const char* attr, PyObject *obj);
    PyObject *_getattr(char *attr);              // __getattr__ function
    int _setattr(char *attr, PyObject *value);        // __setattr__ function
    //@}

    /// getter for the object handled by this class
    MeshObject *getMeshObjectObject(void) const;
    Base::Reference<MeshObject> _meshObject;
};

#define PARENTSMeshMeshPy &MeshPy::Type,PARENTSDataComplexGeoDataPy

}  //namespace Mesh

#endif  // MESH_MESHPY_H


