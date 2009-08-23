/***************************************************************************
 *   Copyright (c) Juergen Riegel         <juergen.riegel@web.de>          *
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


#ifndef GUI_SELECTION_H
#define GUI_SELECTION_H

// Std. configurations

#include <string>
#include <vector>
#include <list>
#include <map>
#include <CXX/Objects.hxx>

#include <Base/Observer.h>
#include <Base/PyExport.h>
#include <Base/Type.h>
#include <App/Document.h>

namespace App
{
  class DocumentObject;
  class Document;
}

namespace Gui
{


/** Transport the changes of the Selection
 * This class transports closer information what was changed in the
 * selection. It's an optional information and not all commands set this
 * information. If not set all observer of the selection assume a full change
 * and update everything (e.g 3D view). This is not a very good idea if, e.g. only
 * a small parameter has changed. Therefore one can use this class and make the
 * update of the document much faster!
 * @see FCObserver
 */
class GuiExport SelectionChanges
{
public:
    enum MsgType {
        AddSelection,
        RmvSelection,
        SetSelection,
        ClrSelection,
        SetPreselect,
        RmvPreselect
    };

    MsgType Type;

    const char* pDocName;
    const char* pObjectName;
    const char* pSubName;
    float x;
    float y;
    float z;
};

} //namespace Gui



// Export an instance of the base class (to avoid warning C4275, see also 
// C++ Language Reference/General Rules and Limitations on MSDN for more details.)
//
// For compiler gcc4.1 we need to define the template class outside namespace 'Gui' otherwise we get the compiler error: 
// 'explicit instantiation of 'class Base::Subject<const Gui::SelectionChanges&>' in namespace 'Gui' 
// (which does not enclose namespace 'Base')

#if defined(__GNUC__) && defined(__GNUC_MINOR__)
#if ((__GNUC__)*100+(__GNUC_MINOR__)*10) >= 410
template class GuiExport Base::Subject<const Gui::SelectionChanges&>;
#endif
#endif

namespace Gui
{

/**
 * The SelectionObserver class simplyfies the step to write classes that listen
 * to what happens to the selection.
 *
 * @author Werner Mayer
 */
class GuiExport SelectionObserver
{

public:
    /// Constructor
    SelectionObserver();
    virtual ~SelectionObserver();
    bool blockConnection(bool block);
    bool isConnectionBlocked() const;

    /** Attaches to the selection. */
    void attachSelection();
    /** Detaches from the selection. */
    void detachSelection();

private:
    virtual void onSelectionChanged(const SelectionChanges& msg) = 0;

private:
    typedef boost::signals::connection Connection;
    Connection connectSelection;
};

/**
 * The SelectionObserverPython class implements a mechanism to register
 * a Python class instance implementing the required interface in order
 * to be notified on selection changes.
 *
 * @author Werner Mayer
 */
class GuiExport SelectionObserverPython : public SelectionObserver
{

public:
    /// Constructor
    SelectionObserverPython(const Py::Object& obj);
    virtual ~SelectionObserverPython();

    static void addObserver(const Py::Object& obj);
    static void removeObserver(const Py::Object& obj);

private:
    void onSelectionChanged(const SelectionChanges& msg);
    void addSelection(const SelectionChanges&);
    void removeSelection(const SelectionChanges&);
    void setSelection(const SelectionChanges&);
    void clearSelection(const SelectionChanges&);
    void setPreselection(const SelectionChanges&);
    void removePreselection(const SelectionChanges&);

private:
    Py::Object inst;
    static std::vector<SelectionObserverPython*> _instances;
};

/** The Selection singleton class
 */
class GuiExport SelectionSingleton : public Base::Subject<const SelectionChanges&>
{
public:
    /// Add to selection 
    bool addSelection(const char* pDocName, const char* pObjectName=0, const char* pSubName=0, float x=0, float y=0, float z=0);
    /// Remove from selection (for internal use)
    void rmvSelection(const char* pDocName, const char* pObjectName=0, const char* pSubName=0);
    /// Set the selection for a document
    void setSelection(const char* pDocName, const std::vector<App::DocumentObject*>&);
    /// Clear the selection of document \a pDocName. If the document name is not given the selection of the active document is cleared.
    void clearSelection(const char* pDocName=0);
    /// Clear the selection of all documents
    void clearCompleteSelection();
    /// Check if selected
    bool isSelected(const char* pDocName, const char* pObjectName=0, const char* pSubName=0) const;
    /// Check if selected
    bool isSelected(App::DocumentObject*) const;

	/// set the preselected object (mostly by the 3D View)
    bool setPreselect(const char* pDocName, const char* pObjectName, const char* pSubName, float x=0, float y=0, float z=0);
	/// remove the present preselection
    void rmvPreselect();
	/// returns the present preselection 


    /** Returns the number of selected objects with an special Object type
     * Its the convenient way to check if the right Objects are selected to 
     * perform an operation (GuiCommand). The checking also detect base type. 
     * E.g. "Part" also fits on "PartImport" or "PartTransform types.
     * If no document name is given the active document is assumed.
     */
    unsigned int countObjectsOfType(const Base::Type& typeId, const char* pDocName=0) const;

    /**
     * Does basically the same as the method above unless that it accepts a string literal as first argument.
     * \a typeName must be a registered type, otherwise 0 is returned.
     */
    unsigned int countObjectsOfType(const char* typeName, const char* pDocName=0) const;

    /** Returns a vector of features of type \a TypeName selected for the given document name \a pDocName.
     * If no document name is specified the Objects from the active document are regarded.
     * If no objects of this document are selected an empty vector is returned.
     * @note The vector reflects the sequence of selection.
     */
    std::vector<App::DocumentObject*> getObjectsOfType(const Base::Type& typeId, const char* pDocName=0) const;

    /**
     * Does basically the same as the method above unless that it accepts a string literal as first argument.
     * \a typeName must be a registered type otherwise an empty array is returned.
     */
    std::vector<App::DocumentObject*> getObjectsOfType(const char* typeName, const char* pDocName=0) const;

    struct SelObj {
        const char* DocName;
        const char* FeatName;
        const char* SubName;
        const char* TypeName;
        App::Document* pDoc;
        App::DocumentObject*  pObject;
		float x,y,z;
    };

    /// signal on new Object
    boost::signal<void (const SelectionChanges& msg)> signalSelectionChanged;

    /** returns a vector of selection objects
     * if no document name is given the objects of the active are returned.
     * If nothing for this Document is selected a empty vector is returnd.
     * The vector reflects the sequence of selection!
     */
    std::vector<SelObj> getSelection(const char* pDocName=0) const;

    /** Returns a vector of all selection objects of all documents. */
    std::vector<SelObj> getCompleteSelection() const;

    /// size of selcted entities for all docuements
    unsigned int size(void) const {return _SelList.size();}

    static SelectionSingleton& instance(void);
    static void destruct (void);

    // Python interface
    static PyMethodDef    Methods[];

protected:
    static PyObject *sAddSelection   (PyObject *self,PyObject *args,PyObject *kwd);
    static PyObject *sRemoveSelection(PyObject *self,PyObject *args,PyObject *kwd);
    static PyObject *sClearSelection (PyObject *self,PyObject *args,PyObject *kwd);
    static PyObject *sIsSelected     (PyObject *self,PyObject *args,PyObject *kwd);
    static PyObject *sGetSelection   (PyObject *self,PyObject *args,PyObject *kwd);
    static PyObject *sGetSelectionEx (PyObject *self,PyObject *args,PyObject *kwd);
    static PyObject *sAddSelObserver (PyObject *self,PyObject *args,PyObject *kwd);
    static PyObject *sRemSelObserver (PyObject *self,PyObject *args,PyObject *kwd);

protected:
    /// Construction
    SelectionSingleton();
    /// Destruction
    virtual ~SelectionSingleton();

    /// Observer message from the App doc
    void slotRenamedObject(App::DocumentObject&);
    void slotDeletedObject(App::DocumentObject&);

    /// helper to retrieve document by name
    App::Document* getDocument(const char* pDocName=0) const;

    struct _SelObj {
        std::string DocName;
        std::string FeatName;
        std::string SubName;
        std::string TypeName;
        App::Document* pDoc;
        App::DocumentObject*  pObject;
        float x,y,z;
    };
    std::list<_SelObj> _SelList;

    static SelectionSingleton* _pcSingleton;

    std::string DocName;
    std::string FeatName;
    std::string SubName;
    float hx,hy,hz;
};

/// Get the global instance
inline SelectionSingleton& Selection(void)
{
    return SelectionSingleton::instance();
}

} //namespace Gui

#endif // GUI_SELECTION_H
