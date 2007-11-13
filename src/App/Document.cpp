/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de) 2002     *
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


/*! \class App::Document
This is bisides the Application class the most importand class in FreeCAD
It contains all the data of the opend, saved or newly created FreeCAD Document.
The Document manage the Undo and Redo mechanism and the linking of documents.

Note: the documents are not free objects. They are completly handled by the
App::Application. Only the Application can Open or destroy a document.

\section Exception Exception handling
As the document is the main data structure of FreeCAD we have to take a close
look on how Exceptions affect the integratie of the App::Document.

\section UndoRedo Undo Redo an Transactions
Undo Redo handling is one of the major machanism of an document in terms of
user friendlines and speed (no on will whait for Undo to long).

\section Dependency Graph and dependency handling
The FreeCAD document handles the dependencies of its DocumentObjects with
a adjacence list. This gives the opertunetie to calculate the shortest
recompute path. Also enabels more complicated dependencies beond trees.


@see App::Application
@see App::DocumentObject
@see App::AbstractFeature
*/



#include "PreCompiled.h"

#ifndef _PreComp_
#endif

#include <boost/graph/topological_sort.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/visitors.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/bind.hpp>


#include "Document.h"
#include "DocumentPy.h"
#include "Application.h"
#include "Feature.h"
#include "PropertyLinks.h"

#include <Base/PyExport.h>
#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/FileInfo.h>
#include <Base/TimeInfo.h>
#include <Base/Interpreter.h>
#include <Base/Reader.h>
#include <Base/Writer.h>

#include <Base/zipios/zipios-config.h>
#include <Base/zipios/zipfile.h>
#include <Base/zipios/zipinputstream.h>
#include <Base/zipios/zipoutputstream.h>
#include <Base/zipios/meta-iostreams.h>

#include "Application.h"
#include "Transactions.h"

using Base::Console;
using Base::streq;
using Base::Writer;
using namespace App;
using namespace std;
using namespace boost;
using namespace zipios ;


#ifdef MemDebugOn
# define new DEBUG_CLIENTBLOCK
#endif


using namespace zipios ;


PROPERTY_SOURCE(App::Document, App::PropertyContainer)


// typedef boost::property<boost::vertex_root_t, DocumentObject* > VertexProperty;
typedef boost::adjacency_list <
boost::vecS,           // class OutEdgeListS  : a Sequence or an AssociativeContainer
boost::vecS,           // class VertexListS   : a Sequence or a RandomAccessContainer
boost::directedS,      // class DirectedS     : This is a directed graph
boost::no_property,    // class VertexProperty:
boost::no_property,    // class EdgeProperty:
boost::no_property,    // class GraphProperty:
boost::listS           // class EdgeListS:
> DependencyList;
typedef boost::graph_traits<DependencyList> Traits;
typedef Traits::vertex_descriptor Vertex;
typedef Traits::edge_descriptor Edge;



void Document::writeDependencyGraphViz(std::ostream &out)
{
    //  // caching vertex to DocObject
    //std::map<Vertex,DocumentObject*> VertexMap;
    //for(std::map<DocumentObject*,Vertex>::const_iterator It1= _DepConMap.begin();It1 != _DepConMap.end(); ++It1)
    //  VertexMap[It1->second] = It1->first;

    out << "digraph G {" << endl;
    out << "\tordering=out;" << endl;
    out << "\tnode [shape = box];" << endl;

    for (std::map<std::string,DocumentObject*>::const_iterator It = ObjectMap.begin(); It != ObjectMap.end();++It) {
        out << "\t" << It->first << ";" <<endl;
        std::vector<DocumentObject*> OutList = It->second->getOutList();
        for (std::vector<DocumentObject*>::const_iterator It2=OutList.begin();It2!=OutList.end();++It2)
            if (*It2)
                out << "\t" << It->first << "->" << (*It2)->getNameInDocument() << ";" <<endl;
    }

    /*
    graph_traits<DependencyList>::edge_iterator ei, ei_end;
    for (tie(ei,ei_end) = edges(_DepList); ei != ei_end; ++ei)
      out << "\t"
          << VertexMap[source(*ei, _DepList)]->getNameInDocument()
          << " -> "
          << VertexMap[target(*ei, _DepList)]->getNameInDocument()
          << ";" << endl;
    */
    out << "}" << endl;
}


//bool _has_cycle_dfs(const DependencyList & g, vertex_t u, default_color_type * color)
//{
//  color[u] = gray_color;
//  graph_traits < DependencyList >::adjacency_iterator vi, vi_end;
//  for (tie(vi, vi_end) = adjacent_vertices(u, g); vi != vi_end; ++vi)
//    if (color[*vi] == white_color)
//      if (has_cycle_dfs(g, *vi, color))
//        return true;            // cycle detected, return immediately
//      else if (color[*vi] == gray_color)        // *vi is an ancestor!
//        return true;
//  color[u] = black_color;
//  return false;
//}

bool Document::checkOnCycle(void)
{/*
  std::vector < default_color_type > color(num_vertices(_DepList), white_color);
  graph_traits < DependencyList >::vertex_iterator vi, vi_end;
  for (tie(vi, vi_end) = vertices(_DepList); vi != vi_end; ++vi)
    if (color[*vi] == white_color)
      if (_has_cycle_dfs(_DepList, *vi, &color[0]))
        return true; */
    return false;
}


bool Document::undo(void)
{
    if (_iUndoMode) {
        if (activUndoTransaction)
            commitTransaction();
        else
            assert(mUndoTransactions.size()!=0);

        // redo
        activUndoTransaction = new Transaction();
        activUndoTransaction->Name = mUndoTransactions.back()->Name;

        // applying the undo
        mUndoTransactions.back()->apply(*this/*,DocChange*/);

        // save the redo
        mRedoTransactions.push_back(activUndoTransaction);
        activUndoTransaction = 0;

        delete mUndoTransactions.back();
        mUndoTransactions.pop_back();

     }

    return false;
}

bool Document::redo(void)
{
    if (_iUndoMode) {
        if (activUndoTransaction)
            commitTransaction();

        assert(mRedoTransactions.size()!=0);

        // undo
        activUndoTransaction = new Transaction();
        activUndoTransaction->Name = mRedoTransactions.back()->Name;

        // do the redo
        mRedoTransactions.back()->apply(*this/*,DocChange*/);
        mUndoTransactions.push_back(activUndoTransaction);
        activUndoTransaction = 0;

        delete mRedoTransactions.back();
        mRedoTransactions.pop_back();

    }

    return false;
}

std::vector<std::string> Document::getAvailableUndoNames() const
{
    std::vector<std::string> vList;
    if (activUndoTransaction)
        vList.push_back(this->activUndoTransaction->Name);
    for (std::list<Transaction*>::const_reverse_iterator It=mUndoTransactions.rbegin();It!=mUndoTransactions.rend();++It)
        vList.push_back((**It).Name);
    return vList;
}
std::vector<std::string> Document::getAvailableRedoNames() const
{
    std::vector<std::string> vList;
    for (std::list<Transaction*>::const_reverse_iterator It=mRedoTransactions.rbegin();It!=mRedoTransactions.rend();++It)
        vList.push_back((**It).Name);
    return vList;
}


void Document::openTransaction(const char* name)
{
    if (_iUndoMode) {
        if (activUndoTransaction)
            commitTransaction();
        _clearRedos();

        activUndoTransaction = new Transaction();
        if (name)
            activUndoTransaction->Name = name;
    }

}

void Document::_clearRedos()
{
    while (!mRedoTransactions.empty()) {
        delete mRedoTransactions.back();
        mRedoTransactions.pop_back();
    }
}

void Document::commitTransaction()
{
    if (activUndoTransaction) {
        mUndoTransactions.push_back(activUndoTransaction);
        activUndoTransaction = 0;
    }

}

void Document::abortTransaction()
{
    if (activUndoTransaction) {
        bRollback = true;
        // applieing the so far made changes
        activUndoTransaction->apply(*this);
        bRollback = false;

        // destroy the undo
        delete activUndoTransaction;
        activUndoTransaction = 0;
    }

}


void Document::clearUndos()
{
    if (activUndoTransaction)
        commitTransaction();

    while (!mUndoTransactions.empty()) {
        delete mUndoTransactions.back();
        mUndoTransactions.pop_back();
    }

    _clearRedos();
}

int Document::getAvailableUndos() const
{
    if (activUndoTransaction)
        return mUndoTransactions.size() + 1;
    else
        return mUndoTransactions.size();
}

int Document::getAvailableRedos() const
{
    return mRedoTransactions.size();
}

void Document::setUndoMode(int iMode)
{
    if (_iUndoMode && !iMode)
        clearUndos();

    _iUndoMode = iMode;
}

int Document::getUndoMode(void) const
{
    return _iUndoMode;
}

unsigned int Document::getUndoMemSize (void) const
{
    return 0;
}

void Document::onBevorChangeProperty(const DocumentObject *Who, const Property *What)
{
    if (activUndoTransaction && !bRollback)
        activUndoTransaction->addObjectChange(Who,What);
}

void Document::onChangedProperty(const DocumentObject *Who, const Property *What)
{
    if (this->activTransaction && !bRollback)
        this->activTransaction->addObjectChange(Who,What);
    signalChangedObject(const_cast<DocumentObject&>(*Who), const_cast<Property&>(*What));
}

void Document::setTransactionMode(int iMode)
{
    /*  if(_iTransactionMode == 0 && iMode == 1)
        beginTransaction();

      if(activTransaction && iMode == 0)
        endTransaction();
      */
    _iTransactionMode = iMode;

}


#if 0
/// starts a new transaction
int  Document::beginTransaction(void)
{
    if (activTransaction)
        endTransaction();

    iTransactionCount++;

    activTransaction = new Transaction(iTransactionCount);
    mTransactions[iTransactionCount] = activTransaction;

    return iTransactionCount;
}

/// revert all changes to the document since beginTransaction()
void Document::rollbackTransaction(void)
{
    // ToDo
    assert(0);
    endTransaction();
}

/// ends the open Transaction
int  Document::endTransaction(void)
{
    activTransaction = 0;
    return iTransactionCount;
}

/// returns the named Transaction
const Transaction *Document::getTransaction(int pos) const
{
    if (pos == -1)
        return activTransaction;
    else {
        std::map<int,Transaction*>::const_iterator Pos( mTransactions.find(pos));
        if (Pos != mTransactions.end())
            return Pos->second;
        else
            return 0;
    }
}


#endif




//--------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------
Document::Document(void)
    : _iTransactionMode(0),
    iTransactionCount(0),
    activTransaction(0),
    bRollback(false),
    _iUndoMode(0),
    activUndoTransaction(0),
    pActiveObject(0),
    pDocumentHook(0)
{
    // Remark: In a constructor we should never increment a Python object as we cannot be sure
    // if the Python interpreter gets a reference of it. E.g. if we increment but Python don't
    // get a reference then the object wouldn't get deleted in the destructor.
    // So, we must increment only if the interpreter gets a reference.
    // Remark: We force the document Python object to own the DocumentPy instance, thus we don't
    // have to care about ref counting any more.
    DocumentPythonObject.set(new DocumentPy(this), true);

    Console().Log("+App::Document: %p\n",this);


    ADD_PROPERTY_TYPE(Name,("Unnamed"),0,Prop_ReadOnly,"The name of the document");
    ADD_PROPERTY_TYPE(FileName,(""),0,Prop_None,"The path to the file where the document is saved to");
    ADD_PROPERTY_TYPE(CreatedBy,(""),0,Prop_None,"The creator of the document");
    ADD_PROPERTY_TYPE(CreationDate,(Base::TimeInfo::currentDateTimeString()),0,Prop_ReadOnly,"Date of creation");
    ADD_PROPERTY_TYPE(LastModifiedBy,(""),0,Prop_None,0);
    ADD_PROPERTY_TYPE(LastModifiedDate,("Unknown"),0,Prop_ReadOnly,"Date of last modification");
    ADD_PROPERTY_TYPE(Company,(""),0,Prop_None,"Additional tag to save the the name of the company");
    ADD_PROPERTY_TYPE(Comment,(""),0,Prop_None,"Additional tag to save a comment");
}

Document::~Document()
{
    Console().Log("-App::Document: %s %p\n",Name.getValue(), this);

    clearUndos();

    std::map<std::string,DocumentObject*>::iterator it;

    Console().Log("-Delete Features of %s \n",Name.getValue());

    ObjectArray.clear();
    for (it = ObjectMap.begin(); it != ObjectMap.end(); ++it) {
        delete(it->second);
    }

    // Remark: The API of Py::Object has been changed to set whether the wrapper owns the passed
    // Python object or not. In the constructor we forced the wrapper to own the object so we need
    // not to dec'ref the Python object any more.
    // But we must still invalidate the Python object because it need not to be
    // destructed right now because the interpreter can own several references to it.
    Base::PyObjectBase* doc = (Base::PyObjectBase*)DocumentPythonObject.ptr();
    // Call before decrementing the reference counter, otherwise a heap error can occur
    doc->setInvalid();
}





//--------------------------------------------------------------------------
// Exported functions
//--------------------------------------------------------------------------


void Document::Save (Writer &writer) const
{
    writer.Stream() << "<?xml version='1.0' encoding='utf-8'?>" << endl
    << "<!--" << endl
    << " FreeCAD Document, see http://free-cad.sourceforge.net for more informations..." << endl
    << "-->" << endl;

    writer.Stream() << "<Document SchemaVersion=\"3\">" << endl;

    PropertyContainer::Save(writer);

    // writing the features types
    writer.incInd(); // indention for 'Objects count'
    writer.Stream() << writer.ind() << "<Objects Count=\"" << ObjectArray.size() <<"\">" << endl;

    writer.incInd(); // indention for 'Object type'
    std::vector<DocumentObject*>::const_iterator it;
    for (it = ObjectArray.begin(); it != ObjectArray.end(); ++it) {
        writer.Stream() << writer.ind() << "<Object "
        << "type=\"" << (*it)->getTypeId().getName() << "\" "
        << "name=\"" << (*it)->getNameInDocument()       << "\" "
        << "/>" << endl;
    }

    writer.decInd();  // indention for 'Object type'
    writer.Stream() << writer.ind() << "</Objects>" << endl;

    // writing the features itself
    writer.Stream() << writer.ind() << "<ObjectData Count=\"" << ObjectArray.size() <<"\">" << endl;

    writer.incInd(); // indention for 'Object name'
    for (it = ObjectArray.begin(); it != ObjectArray.end(); ++it) {
        writer.Stream() << writer.ind() << "<Object name=\"" << (*it)->getNameInDocument() << "\">" << endl;
        (*it)->Save(writer);
        writer.Stream() << writer.ind() << "</Object>" << endl;
    }

    writer.decInd(); // indention for 'Object name'
    writer.Stream() << writer.ind() << "</ObjectData>" << endl;
    writer.decInd();  // indention for 'Objects count'
    writer.Stream() << "</Document>" << endl;

}

void Document::Restore(Base::XMLReader &reader)
{
    int i,Cnt;

    reader.readElement("Document");
    long scheme = reader.getAttributeAsInteger("SchemaVersion");

    // read the Document Properties
    PropertyContainer::Restore(reader);

    // SchemeVersion "2"
    if ( scheme == 2 ) {
        // read the feature types
        reader.readElement("Features");
        Cnt = reader.getAttributeAsInteger("Count");
        for (i=0 ;i<Cnt ;i++) {
            reader.readElement("Feature");
            string type = reader.getAttribute("type");
            string name = reader.getAttribute("name");

            try {
                addObject(type.c_str(),name.c_str());
            }
            catch ( Base::Exception& ) {
                Base::Console().Message("Cannot create object '%s'\n", name.c_str());
            }
        }
        reader.readEndElement("Features");

        // read the features itself
        reader.readElement("FeatureData");
        Cnt = reader.getAttributeAsInteger("Count");
        for (i=0 ;i<Cnt ;i++) {
            reader.readElement("Feature");
            string name = reader.getAttribute("name");
            DocumentObject* pObj = getObject(name.c_str());
            if (pObj) { // check if this feature has been registered
                //FIXME: We must save/restore that state of a feature
                pObj->Restore(reader);
            }
            reader.readEndElement("Feature");
        }
        reader.readEndElement("FeatureData");
    } // SchemeVersion "3"
    else if ( scheme == 3 ) {
        // read the feature types
        reader.readElement("Objects");
        Cnt = reader.getAttributeAsInteger("Count");
        for (i=0 ;i<Cnt ;i++) {
            reader.readElement("Object");
            string type = reader.getAttribute("type");
            string name = reader.getAttribute("name");

            try {
                addObject(type.c_str(),name.c_str());
            }
            catch ( Base::Exception& ) {
                Base::Console().Message("Cannot create object '%s'\n", name.c_str());
            }
        }
        reader.readEndElement("Objects");

        // read the features itself
        reader.readElement("ObjectData");
        Cnt = reader.getAttributeAsInteger("Count");
        for (i=0 ;i<Cnt ;i++) {
            reader.readElement("Object");
            string name = reader.getAttribute("name");
            DocumentObject* pObj = getObject(name.c_str());
            if (pObj) { // check if this feature has been registered
                //FIXME: We must save/restore that state of a feature
                pObj->Restore(reader);
            }
            reader.readEndElement("Object");
        }
        reader.readEndElement("ObjectData");
    }

    reader.readEndElement("Document");
}

unsigned int Document::getMemSize (void) const
{
    unsigned int size = 0;

    // size of the DocObjects in the document
    std::vector<DocumentObject*>::const_iterator it;
    for (it = ObjectArray.begin(); it != ObjectArray.end(); ++it)
        size += (*it)->getMemSize();

    // size of the document properties...
    size += PropertyContainer::getMemSize();

    // Undo Redo size
    size += getUndoMemSize();

    return size;

}

// Save the document under the name its been opened
bool Document::save (void)
{
    int compression = App::GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Preferences/Document")->GetInt("CompressionLevel",3);

    if (*(FileName.getValue()) != '\0') {
        LastModifiedDate.setValue(Base::TimeInfo::currentDateTimeString());
        Base::ZipWriter writer(FileName.getValue());

        writer.setComment("FreeCAD Document");
        writer.setLevel( compression );
        writer.putNextEntry("Document.xml");

        Document::Save(writer);

        // Special handling for Gui document.
        if (pDocumentHook) {
            pDocumentHook->Save(writer);
        }

        // write additional files
        writer.writeFiles();

        return true;
    }

    return false;
}

// Open the document
void Document::restore (void)
{
    // clean up if the document is not empty
    // !TODO mind exeptions wile restoring!
    clearUndos();
    for ( std::vector<DocumentObject*>::iterator obj = ObjectArray.begin(); obj != ObjectArray.end(); ++obj ) {
        signalDeletedObject(*(*obj));
        delete *obj;
    }
    ObjectArray.clear();
    ObjectMap.clear();
    pActiveObject = 0;


    std::string FilePath = FileName.getValue();
    std::string OrigName = Name.getValue();

    zipios::ZipInputStream zipstream(FileName.getValue());
    Base::XMLReader reader(FileName.getValue(), zipstream);

    if ( ! reader.isValid() )
        throw Base::FileException("Document::open(): Error reading file",FilePath.c_str());

    Document::Restore(reader);

    // We must restore the correct 'FileName' property again because the stored
    // value could be invalid.
    FileName.setValue(FilePath.c_str());

    // When this document has been created it got a preliminary name which might have changed now,
    // because the document XML file can contain a name different from the original name.
    // So firstly we must make sure that the new name is unique and secondly we must notify the
    // application and the observers of this.
    std::string NewName = Name.getValue();
    if ( NewName != OrigName ) {
        // The document's name has changed. We make sure that this new name is unique then.
        std::string NewUniqueName = GetApplication().getUniqueDocumentName(NewName.c_str());
        Name.setValue(NewUniqueName);
        // Notify the application and all observers
        GetApplication().renameDocument(OrigName.c_str(), NewUniqueName.c_str());
    }

    reader.readFiles(zipstream);

    // notify all as new
    //for (std::map<std::string,DocumentObject*>::iterator It = ObjectMap.begin();It != ObjectMap.end();++It) {
        //if (It->second->getTypeId().isDerivedFrom(AbstractFeature::getClassTypeId()) ) {
            //AbstractFeature* feat = dynamic_cast<AbstractFeature*>(It->second);
            //feat->touchTime.setToActual();
            //feat->setModified(false);
            //if ( feat->status.getValue() == AbstractFeature::New )
            //  feat->status.setValue( AbstractFeature::Valid );
        //}
        //signalNewObject(*(It->second));
    //}

    //Notify(DocChange);

    // Special handling for Gui document, the view representations must already
    // exist, what is done in Notify().
    if (pDocumentHook) {
        pDocumentHook->Restore( reader );
        const std::vector<std::string>& files = reader.getFilenames();
        if ( !files.empty() ) {
            // That's the name of the GUI related stuff
            std::string GuiDocument = files.back();
            zipios::ConstEntryPointer entry = zipstream.getNextEntry();
            // If this file is there then we search for it.
            // Note: This file doesn't need to be available if the document has been created
            // without GUI. But if available then it's the last file inside the ZIP.
            while ( entry->isValid() && entry->getName() != GuiDocument )
                entry = zipstream.getNextEntry();
            // Okay, the file is available
            if ( entry->isValid() )
                pDocumentHook->RestoreDocFile( zipstream );
        }
    }
}

bool Document::isSaved() const
{
    std::string name = FileName.getValue();
    return !name.empty();
}



/// Remove all modifications. After this call The document becomesagain Valid.
void Document::purgeTouched()
{
    for (std::vector<DocumentObject*>::iterator It = ObjectArray.begin();It != ObjectArray.end();++It)
        (*It)->purgeTouched();
}
bool Document::isTouched() const
{
    for (std::vector<DocumentObject*>::const_iterator It = ObjectArray.begin();It != ObjectArray.end();++It)
        if ((*It)->isTouched())
            return true;
    return false;
}

vector<DocumentObject*> Document::getTouched(void) const
{
    vector<DocumentObject*> result;

    for (std::vector<DocumentObject*>::const_iterator It = ObjectArray.begin();It != ObjectArray.end();++It)
        if ((*It)->isTouched())
            result.push_back(*It);

    return result;
}



void Document::recompute()
{
    // delete recompute log
    for( std::vector<App::DocumentObjectExecReturn*>::iterator it=_RecomputeLog.begin();it!=_RecomputeLog.end();++it)
        delete *it;
    _RecomputeLog.clear();

    DependencyList DepList;
    std::map<DocumentObject*,Vertex> VertexObjectList;

    // Filling up the adjacency List
    for (std::map<std::string,DocumentObject*>::const_iterator It = ObjectMap.begin(); It != ObjectMap.end();++It)
        // add the object as Vertex and remember the index
        VertexObjectList[It->second] = add_vertex(DepList);
    // add the edges
    for (std::map<std::string,DocumentObject*>::const_iterator It = ObjectMap.begin(); It != ObjectMap.end();++It) {
        std::vector<DocumentObject*> OutList = It->second->getOutList();
        for (std::vector<DocumentObject*>::const_iterator It2=OutList.begin();It2!=OutList.end();++It2)
            if (*It2)
                add_edge(VertexObjectList[It->second],VertexObjectList[*It2],DepList);
    }

    std::list<Vertex> make_order;
    DependencyList::out_edge_iterator j, jend;
    // this sort gives the execute
    boost::topological_sort(DepList, std::front_inserter(make_order));

    // caching vertex to DocObject
    std::map<Vertex,DocumentObject*> VertexMap;
    for (std::map<DocumentObject*,Vertex>::const_iterator It1= VertexObjectList.begin();It1 != VertexObjectList.end(); ++It1)
        VertexMap[It1->second] = It1->first;


    std::cout << "make ordering: " << endl;
    for (std::list<Vertex>::reverse_iterator i = make_order.rbegin();i != make_order.rend(); ++i) {
        DocumentObject* Cur = VertexMap[*i];
        std::cout << Cur->getNameInDocument() << " dep on: " ;

        bool NeedUpdate = false;

        // ask the object if it shut be recomputed
        if (Cur->mustExecute() == 1)
            NeedUpdate = true;
        else if (Cur->mustExecute() == -1)
            // update if one of the dependencies is touched
            for (boost::tie(j, jend) = out_edges(*i, DepList); j != jend; ++j) {
                DocumentObject* Test = VertexMap[target(*j, DepList)];
                std::cout << Test->getNameInDocument() << ", " ;
                if (Test->isTouched()) {
                    NeedUpdate = true;
                    break;
                }
            }
        std::cout << endl;
        // if one touched recompute
        if (NeedUpdate) {
            std::cout << "Recompute" << endl;
            if(_recomputeFeature(Cur))
                // if somthing happen break execution of recompute
                return;
        }

    }
    // reset all touched
    for (std::map<DocumentObject*,Vertex>::const_iterator It1= VertexObjectList.begin();It1 != VertexObjectList.end(); ++It1)
        It1->first->purgeTouched();

}

const char *Document::getErrorDescription(const App::DocumentObject*Obj) const
{
    for (std::vector<App::DocumentObjectExecReturn*>::const_iterator it=_RecomputeLog.begin();it!=_RecomputeLog.end();++it)
        if ((*it)->Which == Obj)
            return (*it)->Why.c_str();
    return 0;
}

// call the recompute of the Feature and handle the exceptions and errors.
bool Document::_recomputeFeature(DocumentObject* Feat)
{
    Base::Console().Log("Solv: Executing Feature: %s\n",Feat->getNameInDocument());

    //Feat->status.setValue(AbstractFeature::Recompute);
    DocumentObjectExecReturn  *returnCode = 0;
    try {
        returnCode = Feat->execute();
    }
    catch(Base::AbortException &e){
        e.ReportException();
        _RecomputeLog.push_back(new DocumentObjectExecReturn("User abort",Feat));
        Feat->setError();
        return true;
    }
    catch (const Base::MemoryException& e) {
        Base::Console().Error("Memory exception in feature '%s' thrown: %s\n",Feat->getNameInDocument(),e.what());
        _RecomputeLog.push_back(new DocumentObjectExecReturn("Out of memory exception",Feat));
        Feat->setError();
        return true;
    }
    catch (Base::Exception &e) {
        e.ReportException();
        _RecomputeLog.push_back(new DocumentObjectExecReturn(e.what(),Feat));
        Feat->setError();
        return false;
    }
    catch (std::exception &e) {
        Base::Console().Warning("exception in Feature \"%s\" thrown: %s\n",Feat->getNameInDocument(),e.what());
        _RecomputeLog.push_back(new DocumentObjectExecReturn(e.what(),Feat));
        Feat->setError();
        return false;
    }
#ifndef FC_DEBUG
    catch (...) {
        Base::Console().Error("App::Document::_RecomputeFeature(): Unknown exception in Feature \"%s\" thrown\n",Feat->getNameInDocument());
        _RecomputeLog.push_back(new DocumentObjectExecReturn("Unknown exeption!"));
        Feat->setError();
        return true;
    }
#endif

    // error code
    if (returnCode == DocumentObject::StdReturn) {
        //Feat->status.setValue(AbstractFeature::Error);
        Feat->resetError();
    }
    else {
        returnCode->Which = Feat;
        _RecomputeLog.push_back(returnCode);
        Feat->setError();
    }
    return false;
}



void Document::recomputeFeature(AbstractFeature* Feat)
{
     // delete recompute log
    for( std::vector<App::DocumentObjectExecReturn*>::iterator it=_RecomputeLog.begin();it!=_RecomputeLog.end();++it)
        delete *it;
    _RecomputeLog.clear();

    _recomputeFeature(Feat);
}

DocumentObject *Document::addObject(const char* sType, const char* pObjectName)
{
    App::DocumentObject* pcObject = (App::DocumentObject*) Base::Type::createInstanceByName(sType,true);

    string ObjectName;
    if (!pcObject)
        return 0;
    if (!pcObject->getTypeId().isDerivedFrom(App::DocumentObject::getClassTypeId())) {
        delete pcObject;
        char szBuf[200];
        snprintf(szBuf, 200, "'%s' is not a document object type", sType);
        throw Base::Exception(szBuf);
    }

    pcObject->setDocument(this);

    // Transaction stuff
    if (activTransaction)
        activTransaction->addObjectNew(pcObject);
    // Undo stuff
    if (activUndoTransaction)
        activUndoTransaction->addObjectDel(pcObject);

    // get Unique name
    if (pObjectName)
        ObjectName = getUniqueObjectName(pObjectName);
    else
        ObjectName = getUniqueObjectName(sType);


    pActiveObject = pcObject;

    // insert in the name map
    ObjectMap[ObjectName] = pcObject;
    // cache the pointer to the name string in the Object (for performance of DocumentObject::getNameInDocument())
    pcObject->pcNameInDocument = &(ObjectMap.find(ObjectName)->first);
    // insert in the vector
    ObjectArray.push_back(pcObject);
    // insert in the adjacence list and referenc through the ConectionMap
    //_DepConMap[pcObject] = add_vertex(_DepList);

    pcObject->Label.setValue( ObjectName );

    // mark the object as new (i.e. set status bit 2) and send the signal
    pcObject->StatusBits.set(2);
    signalNewObject(*pcObject);
    signalActivatedObject(*pcObject);

    // return the Object
    return pcObject;
}

void Document::_addObject(DocumentObject* pcObject, const char* pObjectName)
{

    ObjectMap[pObjectName] = pcObject;
    ObjectArray.push_back(pcObject);
    // cache the pointer to the name string in the Object (for performance of DocumentObject::getNameInDocument())
    pcObject->pcNameInDocument = &(ObjectMap.find(pObjectName)->first);

    // Transaction stuff
    if (activTransaction)
        activTransaction->addObjectNew(pcObject);
    // Undo stuff
    if (activUndoTransaction)
        activUndoTransaction->addObjectDel(pcObject);

    // send the signal
    signalNewObject(*pcObject);


}

/// Remove an object out of the document
void Document::remObject(const char* sName)
{
    std::map<std::string,DocumentObject*>::iterator pos = ObjectMap.find(sName);

    // name not found?
    if (pos == ObjectMap.end())
        return;

    if (pActiveObject == pos->second)
        pActiveObject = 0;

    signalDeletedObject(*(pos->second));

    // Before deleting we must nullify all dependant objects
    for ( std::map<std::string,DocumentObject*>::iterator it = ObjectMap.begin(); it != ObjectMap.end(); ++it ) {
        std::map<std::string,App::Property*> Map;
        it->second->getPropertyMap(Map);
        // search for all properties that could have a link to the object
        for ( std::map<std::string,App::Property*>::iterator pt = Map.begin(); pt != Map.end(); ++pt ) {
            if ( pt->second->getTypeId().isDerivedFrom(PropertyLink::getClassTypeId()) ) {
                PropertyLink* link = (PropertyLink*)pt->second;
                if (link->getValue() == pos->second)
                    link->setValue(0);
                else if (link->getContainer() == pos->second)
                    link->setValue(0);
            }
            else if ( pt->second->getTypeId().isDerivedFrom(PropertyLinkList::getClassTypeId()) ) {
                PropertyLinkList* link = (PropertyLinkList*)pt->second;
                if (link->getContainer() == pos->second) {
                    link->setValues(std::vector<DocumentObject*>());
                }
                else {
                    // copy the list (not the objects)
                    std::vector<DocumentObject*> linked = link->getValues();
                    for (std::vector<DocumentObject*>::iterator fIt = linked.begin(); fIt != linked.end(); ++fIt) {
                        if ((*fIt) == pos->second) {
                            // reassign the the list without the object to be deleted
                            linked.erase(fIt);
                            link->setValues(linked);
                            break;
                        }
                    }
                }
            }
        }
    }

    // Transaction stuff
    if (this->activTransaction)
        this->activTransaction->addObjectDel(pos->second);

    // Undo stuff
    if (activUndoTransaction) {
        // in this case transaction delete or save the object
        activUndoTransaction->addObjectNew(pos->second);
        // set name cache false
        //pos->second->pcNameInDocument = 0;
    }
    else
        // if not saved in undo -> delete object
        delete pos->second;


    for ( std::vector<DocumentObject*>::iterator obj = ObjectArray.begin(); obj != ObjectArray.end(); ++obj ) {
        if ( *obj == pos->second ) {
            ObjectArray.erase(obj);
            break;
        }
    }
    // remove from adjancy list
    //remove_vertex(_DepConMap[pos->second],_DepList);
    //_DepConMap.erase(pos->second);
    ObjectMap.erase(pos);
}

/// Remove an object out of the document (internal)
void Document::_remObject(DocumentObject* pcObject)
{
    std::map<std::string,DocumentObject*>::iterator pos = ObjectMap.find(pcObject->getNameInDocument());

    signalDeletedObject(*pcObject);

    // Transaction stuff
    if (this->activTransaction)
        this->activTransaction->addObjectDel(pcObject);

    // Undo stuff
    if (activUndoTransaction)
        activUndoTransaction->addObjectNew(pcObject);
    else{
        // shut never happen
        assert(0);
        // if not saved in undo -> delete
        delete pcObject;
    }

    // remove from map
    ObjectMap.erase(pos);
    //// set name cache false
    //pcObject->pcNameInDocument = 0;

    for ( std::vector<DocumentObject*>::iterator it = ObjectArray.begin(); it != ObjectArray.end(); ++it ) {
        if ( *it == pcObject ) {
            ObjectArray.erase(it);
            break;
        }
    }

}


DocumentObject *Document::getActiveObject(void) const
{

    return pActiveObject;
}

DocumentObject *Document::getObject(const char *Name) const
{
    std::map<std::string,DocumentObject*>::const_iterator pos;

    pos = ObjectMap.find(Name);

    if (pos != ObjectMap.end())
        return pos->second;
    else
        return 0;
}

const char *Document::getObjectName(DocumentObject *pFeat) const
{
    std::map<std::string,DocumentObject*>::const_iterator pos;

    for (pos = ObjectMap.begin();pos != ObjectMap.end();++pos)
        if (pos->second == pFeat)
            return pos->first.c_str();

    return 0;
}

string Document::getUniqueObjectName(const char *Name) const
{

    // strip ilegal chars
    string CleanName;
    const char *It=Name;

    // check for first character whether it's a digit
    if ((*It != '\0') && (*It>=48 && *It<=57))
        CleanName = "_";

    while (*It != '\0') {
        if (   (*It>=48 && *It<=57)  // Numbers
                ||(*It>=65 && *It<=90)   // lowercase letters
                ||(*It>=97 && *It<=122)  // Upercase letters
           ) {
            CleanName += *It;
        }
        else {
            // All other letters gets replaced
            CleanName += '_';
        }
        It++;
    }

    std::map<std::string,DocumentObject*>::const_iterator pos;

    // name in use?
    pos = ObjectMap.find(CleanName);

    if (pos == ObjectMap.end())
        // if not, name is OK
        return CleanName;
    else {
        // find highes sufix
        int nSuff = 0;
        for (pos = ObjectMap.begin();pos != ObjectMap.end();++pos) {
            const string &rclObjName = pos->first;
            if (rclObjName.substr(0, strlen(CleanName.c_str())) == CleanName) { // Prefix gleich
                string clSuffix(rclObjName.substr(strlen(CleanName.c_str())));
                if (clSuffix.size() > 0) {
                    int nPos = clSuffix.find_first_not_of("0123456789");
                    if (nPos==-1)
                        nSuff = max<int>(nSuff, atol(clSuffix.c_str()));
                }
            }
        }
        char szName[200];
        snprintf(szName, 200, "%s%d", CleanName.c_str(), nSuff + 1);

        return string(szName);
    }
}

std::vector<DocumentObject*> Document::getObjects() const
{
    std::vector<DocumentObject*> Objects;
    for ( std::map<std::string,DocumentObject*>::const_iterator it = ObjectMap.begin(); it != ObjectMap.end(); ++it )
        Objects.push_back( it->second );
    return Objects;
}

std::vector<DocumentObject*> Document::getObjectsOfType(const Base::Type& typeId) const
{
    std::vector<DocumentObject*> Objects;
    for ( std::map<std::string,DocumentObject*>::const_iterator it = ObjectMap.begin(); it != ObjectMap.end(); ++it ) {
        if ( it->second->getTypeId().isDerivedFrom( typeId ) )
            Objects.push_back( it->second );
    }
    return Objects;
}

int Document::countObjectsOfType(const Base::Type& typeId) const
{
    int ct=0;
    for ( std::map<std::string,DocumentObject*>::const_iterator it = ObjectMap.begin(); it != ObjectMap.end(); ++it ) {
        if ( it->second->getTypeId().isDerivedFrom( typeId ) )
            ct++;
    }

    return ct;
}



PyObject * Document::getPyObject(void)
{
    return Py::new_reference_to(DocumentPythonObject);
}
