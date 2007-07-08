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



void Document::writeDependencyGraphViz(std::ostream &out)
{
  //  // caching vertex to DocObject
  //std::map<Vertex,DocumentObject*> VertexMap;
  //for(std::map<DocumentObject*,Vertex>::const_iterator It1= _DepConMap.begin();It1 != _DepConMap.end(); ++It1)
  //  VertexMap[It1->second] = It1->first;

  out << "digraph G {" << endl;
  out << "\tordering=out;" << endl;
  out << "\tnode [shape = box];" << endl;

  for(std::map<std::string,DocumentObject*>::const_iterator It = ObjectMap.begin(); It != ObjectMap.end();++It)
  { 
    out << "\t" << It->first << ";" <<endl;
    std::vector<DocumentObject*> OutList = It->second->getOutList();
    for(std::vector<DocumentObject*>::const_iterator It2=OutList.begin();It2!=OutList.end();++It2)
      if(*It2)
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
  if(_iUndoMode)
  {
    if(activUndoTransaction)
      commitTransaction();
    else
      assert(mUndoTransactions.size()!=0);

    DocChanges DocChange;
    DocChange.Why = DocChanges::UndoRedo;

    // redo
    activUndoTransaction = new Transaction();
    activUndoTransaction->Name = mUndoTransactions.back()->Name;

    // applying the undo
    mUndoTransactions.back()->apply(*this,DocChange);
    
    // save the redo
    mRedoTransactions.push_back(activUndoTransaction);
    activUndoTransaction = 0;
    
    delete mUndoTransactions.back();
    mUndoTransactions.pop_back();

    // notify all the listeners (e.g. GuiDocument)
    Notify(DocChange);
  }

  return false; 
}

bool Document::redo(void)
{
  if(_iUndoMode)
  {
    if(activUndoTransaction)
      commitTransaction();
  
    assert(mRedoTransactions.size()!=0);

    DocChanges DocChange;
    DocChange.Why = DocChanges::UndoRedo;

    // undo
    activUndoTransaction = new Transaction();
    activUndoTransaction->Name = mRedoTransactions.back()->Name;

    // do the redo
    mRedoTransactions.back()->apply(*this,DocChange);
    mUndoTransactions.push_back(activUndoTransaction);
    activUndoTransaction = 0;
    
    delete mRedoTransactions.back();
    mRedoTransactions.pop_back();

    // notiefy all the listeners (e.g. GuiDocument)
    Notify(DocChange);
  }

  return false; 
}

std::vector<std::string> Document::getAvailableUndoNames() const
{
  std::vector<std::string> vList;
  if(activUndoTransaction)
    vList.push_back(this->activUndoTransaction->Name);
  for(std::list<Transaction*>::const_reverse_iterator It=mUndoTransactions.rbegin();It!=mUndoTransactions.rend();++It)
    vList.push_back((**It).Name);
  return vList;
}
std::vector<std::string> Document::getAvailableRedoNames() const
{
  std::vector<std::string> vList;
  for(std::list<Transaction*>::const_reverse_iterator It=mRedoTransactions.rbegin();It!=mRedoTransactions.rend();++It)
    vList.push_back((**It).Name);
  return vList;
}


void Document::openTransaction(const char* name)
{
  if(_iUndoMode)
  {
    if(activUndoTransaction)
      commitTransaction();
    _clearRedos();

    activUndoTransaction = new Transaction();
    if(name)
      activUndoTransaction->Name = name;
  }
  
}

void Document::_clearRedos()
{
  while(!mRedoTransactions.empty())
  {
    delete mRedoTransactions.back();
    mRedoTransactions.pop_back();
  }
}

void Document::commitTransaction()
{
  if(activUndoTransaction)
  {
    mUndoTransactions.push_back(activUndoTransaction);
    activUndoTransaction = 0;
  }
  
}

void Document::abortTransaction()
{
  DocChanges dummy;
  if(activUndoTransaction)
  {
    bRollback = true;
   // applieing the so far made changes
    activUndoTransaction->apply(*this,dummy);
    bRollback = false;

    // destroy the undo
    delete activUndoTransaction;
    activUndoTransaction = 0;
 }
   
}


void Document::clearUndos()
{
  if(activUndoTransaction)
    commitTransaction();

  while(!mUndoTransactions.empty())
  {
    delete mUndoTransactions.back();
    mUndoTransactions.pop_back();
  }

  _clearRedos();
}

int Document::getAvailableUndos() const
{
  if(activUndoTransaction)
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
  if(_iUndoMode && !iMode)
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
  if(activUndoTransaction && !bRollback)
    activUndoTransaction->addObjectChange(Who,What);
}

void Document::onChangedProperty(const DocumentObject *Who, const Property *What)
{
  if(this->activTransaction && !bRollback)
    this->activTransaction->addObjectChange(Who,What);
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
  if(activTransaction)
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
  if(pos == -1)
    return activTransaction;
  else
  {
    std::map<int,Transaction*>::const_iterator Pos( mTransactions.find(pos));
    if(Pos != mTransactions.end())
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


    ADD_PROPERTY(Name,("Unnamed"));
    ADD_PROPERTY(FileName,(""));
    ADD_PROPERTY(CreatedBy,(""));
    ADD_PROPERTY(CreationDate,(Base::TimeInfo::currentDateTimeString()));
    ADD_PROPERTY(LastModifiedBy,(""));
    ADD_PROPERTY(LastModifiedDate,("Unknown"));
    ADD_PROPERTY(Company,(""));
    ADD_PROPERTY(Comment,(""));
}

Document::~Document()
{
    Console().Log("-App::Document: %s %p\n",Name.getValue(), this);

    clearUndos();

    std::map<std::string,DocumentObject*>::iterator it;

    Console().Log("-Delete Features of %s \n",Name.getValue());

    ObjectArray.clear();
    for(it = ObjectMap.begin(); it != ObjectMap.end(); ++it)
    {
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
  for(it = ObjectArray.begin(); it != ObjectArray.end(); ++it)
  {
    writer.Stream() << writer.ind() << "<Object " 
                                    << "type=\"" << (*it)->getTypeId().getName() << "\" "
                                    << "name=\"" << (*it)->name.getValue()       << "\" "
                                    << "/>" << endl;    
  }

  writer.decInd();  // indention for 'Object type'
  writer.Stream() << writer.ind() << "</Objects>" << endl;
  
  // writing the features itself
  writer.Stream() << writer.ind() << "<ObjectData Count=\"" << ObjectArray.size() <<"\">" << endl;

  writer.incInd(); // indention for 'Object name'
  for(it = ObjectArray.begin(); it != ObjectArray.end(); ++it)
  {
    writer.Stream() << writer.ind() << "<Object name=\"" << (*it)->name.getValue() << "\">" << endl;   
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
    for(i=0 ;i<Cnt ;i++)
    {
      reader.readElement("Feature");
      string type = reader.getAttribute("type");
      string name = reader.getAttribute("name");

      try {
        addObject(type.c_str(),name.c_str());
      } catch ( Base::Exception& ) {
        Base::Console().Message("Cannot create object '%s'\n", name.c_str());
      }
    }
    reader.readEndElement("Features");

    // read the features itself
    reader.readElement("FeatureData");
    Cnt = reader.getAttributeAsInteger("Count");
    for(i=0 ;i<Cnt ;i++)
    {
      reader.readElement("Feature");
      string name = reader.getAttribute("name");
      DocumentObject* pObj = getObject(name.c_str());
      if(pObj) // check if this feature has been registered
      {
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
    for(i=0 ;i<Cnt ;i++)
    {
      reader.readElement("Object");
      string type = reader.getAttribute("type");
      string name = reader.getAttribute("name");

      try {
        addObject(type.c_str(),name.c_str());
      } catch ( Base::Exception& ) {
        Base::Console().Message("Cannot create object '%s'\n", name.c_str());
      }
    }
    reader.readEndElement("Objects");

    // read the features itself
    reader.readElement("ObjectData");
    Cnt = reader.getAttributeAsInteger("Count");
    for(i=0 ;i<Cnt ;i++)
    {
      reader.readElement("Object");
      string name = reader.getAttribute("name");
      DocumentObject* pObj = getObject(name.c_str());
      if(pObj) // check if this feature has been registered
      {
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
  for(it = ObjectArray.begin(); it != ObjectArray.end(); ++it)
    size += (*it)->getMemSize();

  // size of the document properties...
  size += PropertyContainer::getMemSize();

  // Undo Redo size
  size += getUndoMemSize();

  return size;

}
// Save the Document under a new Name
/* Obsolet
void Document::saveAs (const char* name)
{
  Base::FileInfo File(name);

  std::string oldName = Name.getValue();
  std::string newName = File.fileNamePure();

  // make sure that the new document name is a valid Python specifier
  newName = GetApplication().getUniqueDocumentName(newName.c_str());
  Name.setValue(newName);
  FileName.setValue(File.filePath());

  Document::save();

  GetApplication().renameDocument(oldName.c_str(), Name.getValue());

  DocChanges DocChange;
  DocChange.Why = DocChanges::Rename;

  Notify(DocChange);
}
*/
// Save the document under the name its been opened
bool Document::save (void)
{
  int compression = App::GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Preferences/Document")->GetInt("CompressionLevel",3);

  if(*(FileName.getValue()) != '\0')
  {
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
  std::string FilePath = FileName.getValue();
  std::string OrigName = Name.getValue();
  
  zipios::ZipInputStream zipstream(FileName.getValue());
  Base::XMLReader reader(FileName.getValue(), zipstream);

  if ( ! reader.isValid() )
    throw Base::FileException("Document::open(): Error reading file",FilePath.c_str()); 

  Document::Restore(reader);

  // We must restore the correct 'FileName' property again because the stored value could be invalid.
  FileName.setValue(FilePath.c_str());

  // When this document has been created it got a preliminary name which might have changed now,
  // because the document XML file can contain a name different from the original name.
  // So firstly we must make sure that the new name is unique and secondly we must notify the 
  // application and the observers of this.
  std::string NewName = Name.getValue();
  if ( NewName != OrigName )
  {
    // The document's name has changed. We make sure that this new name is unique then.
    std::string NewUniqueName = GetApplication().getUniqueDocumentName(NewName.c_str());
    Name.setValue(NewUniqueName);
    // Notify the application and all observers
    GetApplication().renameDocument(OrigName.c_str(), NewUniqueName.c_str());
    DocChanges DocChange;
    DocChange.Why = DocChanges::Rename;
    Notify(DocChange);
  }

  reader.readFiles(zipstream);

  // notify all as new
  DocChanges DocChange;
  DocChange.Why = DocChanges::Recompute;
  DocChange.NewObjects = ObjectArray;
  for(std::map<std::string,DocumentObject*>::iterator It = ObjectMap.begin();It != ObjectMap.end();++It) {
    if(It->second->getTypeId().isDerivedFrom(AbstractFeature::getClassTypeId()) )
    {
      AbstractFeature* feat = dynamic_cast<AbstractFeature*>(It->second);
      feat->touchTime.setToActual();
      feat->setModified(false);
      if ( feat->status.getValue() == AbstractFeature::New )
        feat->status.setValue( AbstractFeature::Valid );
    }
  }

  Notify(DocChange);

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
void Document::PurgeModified()
{
  //_hDoc->PurgeModified(); 
}

void Document::update(DocumentObject* obj)
{
  DocChanges DocChange;
  DocChange.Why = DocChanges::Recompute;
  DocChange.UpdatedObjects.insert(obj);
  Notify(DocChange);
  signalChangedObject(*obj);
}

void Document::update(const char *name)
{
  std::map<std::string,DocumentObject*>::iterator pos = ObjectMap.find(name);
  if(pos != ObjectMap.end())
    update(pos->second);
}



#if 1
/// Recompute if the document was  not valid and propagate the reccorded modification.
void Document::recompute()
{
  int iSentinel = 20;
  bool goOn;
  DocChanges DocChange;
  DocChange.Why = DocChanges::Recompute;

//  std::set<DocumentObject*>::iterator i;
  std::set<AbstractFeature*>::iterator l;
  std::set<AbstractFeature*> tempErr;

//  TDF_MapIteratorOfLabelMap It;
  Base::Console().Log("Solv: Start recomputation of document: \"%s\"\n",Name.getValue());

  do{
    goOn = false;
    tempErr = DocChange.ErrorFeatures;

    std::vector<DocumentObject*>::iterator It;

    for(It = ObjectArray.begin();It != ObjectArray.end();++It)
    {
      if( ! ((*It)->getTypeId().isDerivedFrom(AbstractFeature::getClassTypeId()) ) )
        continue;
      AbstractFeature *feat = dynamic_cast<AbstractFeature *>(*It);

      // map the new features
      if (feat->getStatus() == AbstractFeature::New)
        DocChange.NewObjects.push_back(feat);

		  if (feat->mustExecute())
		  {
        // if a feature change a other (earlier) Feature could be impacted, so go on ...
        goOn = true;

        _recomputeFeature(feat);

        if(feat->getStatus() == AbstractFeature::Error)
          DocChange.ErrorFeatures.insert(feat);
        
        if(feat->getStatus() == AbstractFeature::Valid)
        {
          DocChange.UpdatedObjects.insert(feat);
          DocChange.ErrorFeatures.erase(feat);
        }
		  }
    }
    // for detecting recursions
    iSentinel --;

  }while(iSentinel > 0 && goOn && tempErr != DocChange.ErrorFeatures);

  if(iSentinel <= 0)
    Base::Console().Warning("Document::recompute(): bailing out with to high solver count, possible recursion!\n");
  
  // remove the new features from the update set, get updated anyway
  for(std::vector<App::DocumentObject*>::iterator it = DocChange.NewObjects.begin();it!=DocChange.NewObjects.end();++it)
    DocChange.UpdatedObjects.erase(*it);

  Notify(DocChange);

  // signal new objects
  for(std::vector<App::DocumentObject*>::iterator it = DocChange.NewObjects.begin();it!=DocChange.NewObjects.end();++it)
      signalNewObject(**it);

  // signal updated objects
  for(std::set<App::DocumentObject*>::iterator it = DocChange.UpdatedObjects.begin();it!=DocChange.UpdatedObjects.end();++it)
      signalChangedObject(**it);

  for(l = DocChange.ErrorFeatures.begin();l!=DocChange.ErrorFeatures.end();++l)
    Base::Console().Log("Error in Feature \"%s\": %s\n",(*l)->name.getValue(),(*l)->getErrorString());

  Base::Console().Log("Solv: Recomputation of Document \"%s\" with %d new, %d Updated and %d errors finished\n",
                      Name.getValue(),
                      DocChange.NewObjects.size(),
                      DocChange.UpdatedObjects.size(),
                      DocChange.ErrorFeatures.size());
  
}
// call the recompute of the Feature and handle the exceptions and errors.
void Document::_recomputeFeature(AbstractFeature* Feat)
{
  Base::Console().Log("Solv: Executing Feature: %s\n",Feat->name.getValue());

  Feat->status.setValue(AbstractFeature::Recompute);
  int  succes;
  try{
    succes = Feat->execute();
  }catch(Base::AbortException &e){
    e.ReportException();
    succes = 4;
  }catch(const Base::MemoryException& e){
    if ( !e.isHandled() )
      Base::Console().Error("Memory exception in feature '%s' thrown: %s\n",Feat->name.getValue(),e.what());
    else
      Base::Console().Log("Memory exception in feature '%s' thrown: %s\n",Feat->name.getValue(),e.what());
    Feat->setError(e.what());
    succes = 4; // Must not rerun twice
  }catch(Base::Exception &e){
    e.ReportException();
    succes = 3;
  }catch(std::exception &e){
    Base::Console().Warning("exception in Feature \"%s\" thrown: %s\n",Feat->name.getValue(),e.what());
    Feat->setError(e.what());
    succes = 3;
  }
#ifndef FC_DEBUG
  catch(...){
    Base::Console().Error("App::Document::_RecomputeFeature(): Unknown exception in Feature \"%s\" thrown\n",Feat->name.getValue());
    succes = 3;
  }
#endif

  // special error code to avoid to execute a feature twice
  if (succes == 4){
    Feat->status.setValue(AbstractFeature::Inactive);
  }else if(succes > 0){
    Feat->status.setValue(AbstractFeature::Error);
  }else {
    // set the time of change
    Feat->status.setValue(AbstractFeature::Valid);

    Feat->touchTime.setToActual();
    Feat->setModified(false);
  }
}
#else
void Document::recompute()
{
  DependencyList DepList;
  std::map<DocumentObject*,Vertex> VertexObjectList;

  // Filling up the adjacency List 
  for(std::map<std::string,DocumentObject*>::const_iterator It = ObjectMap.begin(); It != ObjectMap.end();++It)
    // add the object as Vertex and remember the index
    VertexObjectList[It->second] = add_vertex(DepList);

  for(std::map<std::string,DocumentObject*>::const_iterator It = ObjectMap.begin(); It != ObjectMap.end();++It)
  {
    std::vector<DocumentObject*> OutList = It->second->getOutList();
    for(std::vector<DocumentObject*>::const_iterator It2=OutList.begin();It2!=OutList.end();++It2)
      if(*It2)
        add_edge(VertexObjectList[It->second],VertexObjectList[*It2],DepList);
  }

  std::list<Vertex> make_order;
  DependencyList::out_edge_iterator j, jend;
  boost::topological_sort(DepList, std::front_inserter(make_order));
 
  // caching vertex to DocObject
  std::map<Vertex,DocumentObject*> VertexMap;
  for(std::map<DocumentObject*,Vertex>::const_iterator It1= VertexObjectList.begin();It1 != VertexObjectList.end(); ++It1)
    VertexMap[It1->second] = It1->first;


  std::cout << "make ordering: " << endl;
  for (std::list<Vertex>::reverse_iterator i = make_order.rbegin();i != make_order.rend(); ++i)
  {
    std::cout << VertexMap[*i]->getNameInDocument() << " dep on: " ;
    for (boost::tie(j, jend) = out_edges(*i, DepList); j != jend; ++j)
       std::cout << VertexMap[target(*j, DepList)]->getNameInDocument() << ", " ;
      
    std::cout << endl;
  }

  
}

// call the recompute of the Feature and handle the exceptions and errors.
void Document::_recomputeFeature(AbstractFeature* Feat)
{
  Base::Console().Log("Solv: Executing Feature: %s\n",Feat->name.getValue());

  Feat->status.setValue(AbstractFeature::Recompute);
  int  succes;
  try{
    succes = Feat->execute();
  }catch(Base::AbortException &e){
    e.ReportException();
    succes = 4;
  }catch(const Base::MemoryException& e){
    if ( !e.isHandled() )
      Base::Console().Error("Memory exception in feature '%s' thrown: %s\n",Feat->name.getValue(),e.what());
    else
      Base::Console().Log("Memory exception in feature '%s' thrown: %s\n",Feat->name.getValue(),e.what());
    Feat->setError(e.what());
    succes = 4; // Must not rerun twice
  }catch(Base::Exception &e){
    e.ReportException();
    succes = 3;
  }catch(std::exception &e){
    Base::Console().Warning("exception in Feature \"%s\" thrown: %s\n",Feat->name.getValue(),e.what());
    Feat->setError(e.what());
    succes = 3;
  }
#ifndef FC_DEBUG
  catch(...){
    Base::Console().Error("App::Document::_RecomputeFeature(): Unknown exception in Feature \"%s\" thrown\n",Feat->name.getValue());
    succes = 3;
  }
#endif

  // special error code to avoid to execute a feature twice
  if (succes == 4){
    Feat->status.setValue(AbstractFeature::Inactive);
  }else if(succes > 0){
    Feat->status.setValue(AbstractFeature::Error);
  }else {
    // set the time of change
    Feat->status.setValue(AbstractFeature::Valid);

    Feat->touchTime.setToActual();
    Feat->setModified(false);
  }
}
#endif


void Document::recomputeFeature(AbstractFeature* Feat)
{
  DocChanges DocChange;
  DocChange.Why = DocChanges::Recompute;

  _recomputeFeature(Feat);

  if(Feat->getStatus() == AbstractFeature::Error)
    DocChange.ErrorFeatures.insert(Feat);

  if(Feat->getStatus() == AbstractFeature::Valid)
    DocChange.UpdatedObjects.insert(Feat);

  Notify(DocChange);

}





DocumentObject *Document::addObject(const char* sType, const char* pObjectName)
{
  App::DocumentObject* pcObject = (App::DocumentObject*) Base::Type::createInstanceByName(sType,true);


  string ObjectName;

	if(pcObject)
	{
    assert(pcObject->getTypeId().isDerivedFrom(App::DocumentObject::getClassTypeId()));
    // Note: A simple assert() is not sufficient since the user is able to create a wrong
    // object e.g. from the Python console and the application crashes in Release mode.
    if (!pcObject->getTypeId().isDerivedFrom(App::DocumentObject::getClassTypeId()))
    {
      delete pcObject;
      char szBuf[200];
      snprintf(szBuf, 200, "'%s' is not a document object type", sType);
      throw Base::Exception(szBuf);
    }

    pcObject->setDocument(this);
     
    // Transaction stuff
    if(activTransaction)
      activTransaction->addObjectNew(pcObject);
    // Undo stuff
    if(activUndoTransaction)
      activUndoTransaction->addObjectDel(pcObject);

    // get Unique name
    if(pObjectName)
      ObjectName = getUniqueObjectName(pObjectName);
    else
      ObjectName = getUniqueObjectName(sType);

		pActiveObject = pcObject;

    // insert in the name map
    ObjectMap[ObjectName] = pcObject;
    // insert in the vector
    ObjectArray.push_back(pcObject);
    // insert in the adjacence list and referenc through the ConectionMap
    //_DepConMap[pcObject] = add_vertex(_DepList);

    pcObject->name.setValue( ObjectName );
	
    // return the Object
		return pcObject;

	}else 
    return 0;

}

void Document::_addObject(DocumentObject* pcObject, const char* pObjectName)
{

  ObjectMap[pObjectName] = pcObject;
  ObjectArray.push_back(pcObject);

  // Transaction stuff
  if(activTransaction)
    activTransaction->addObjectNew(pcObject);
  // Undo stuff
  if(activUndoTransaction)
    activUndoTransaction->addObjectDel(pcObject);


}

/// Remove an object out of the document
void Document::remObject(const char* sName)
{
  DocChanges DocChange;
  DocChange.Why = DocChanges::Recompute;

  std::map<std::string,DocumentObject*>::iterator pos = ObjectMap.find(sName);

  // name not found?
  if(pos == ObjectMap.end())
    return;

  DocChange.DeletedObjects.insert(pos->second);

  if(pActiveObject == pos->second)
    pActiveObject = 0;

  Notify(DocChange);
  signalDeletedObject(*(pos->second));

  // Before deleting we must nullify all dependant objects
  for ( std::map<std::string,DocumentObject*>::iterator it = ObjectMap.begin(); it != ObjectMap.end(); ++it )
  {
    std::map<std::string,App::Property*> Map;
    it->second->getPropertyMap(Map);
    // search for all properties that could have a link to the object
    for( std::map<std::string,App::Property*>::iterator pt = Map.begin(); pt != Map.end(); ++pt )
    {
      if ( pt->second->getTypeId().isDerivedFrom(PropertyLink::getClassTypeId()) )
      {
        PropertyLink* link = (PropertyLink*)pt->second;
        if ( link->getValue() == pos->second )
          link->setValue(0);
      }
      else if ( pt->second->getTypeId().isDerivedFrom(PropertyLinkList::getClassTypeId()) )
      {
        PropertyLinkList* link = (PropertyLinkList*)pt->second;
        // copy the list (not the features)
        std::vector<DocumentObject*>::const_iterator fIt;
        std::vector<DocumentObject*> copy_linked;
        const std::vector<DocumentObject*>& linked = link->getValues();
        for ( fIt = linked.begin(); fIt != linked.end(); ++fIt )
        {
          if ( (*fIt) != pos->second )
            copy_linked.push_back( *fIt );
        }

        // reset the the copied list without the object(s) to be deleted
        link->setSize( copy_linked.size() );
        for ( fIt = copy_linked.begin(); fIt != copy_linked.end(); ++fIt )
          link->set1Value(fIt-copy_linked.begin(), *fIt );
      }
    }
  }

  // Transaction stuff
  if(this->activTransaction)
    this->activTransaction->addObjectDel(pos->second);

  // Undo stuff
  if(activUndoTransaction)
    activUndoTransaction->addObjectNew(pos->second);
  else
    // if not saved in undo -> delete
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
  // Transaction stuff
  if(this->activTransaction)
    this->activTransaction->addObjectDel(pcObject);

  // Undo stuff
  if(activUndoTransaction)
    activUndoTransaction->addObjectNew(pcObject);
  else
    // if not saved in undo -> delete
    delete pcObject;

  ObjectMap.erase(pcObject->name.getValue());
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

  if(pos != ObjectMap.end())
    return pos->second;
  else
    return 0;
}

const char *Document::getObjectName(DocumentObject *pFeat) const
{
  std::map<std::string,DocumentObject*>::const_iterator pos;

  for(pos = ObjectMap.begin();pos != ObjectMap.end();++pos)
    if(pos->second == pFeat)
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

  while(*It != '\0')
  {
    if(   (*It>=48 && *It<=57)   // Numbers
        ||(*It>=65 && *It<=90)   // Upercase letters
        ||(*It>=97 && *It<=122)  // Upercase letters
       )
    {
      CleanName += *It;
    }else{
      switch(*It)
      {
      case 'ä': CleanName += "ae"; break;
      case 'ü': CleanName += "ue"; break;
      case 'ö': CleanName += "oe"; break;
      case 'Ä': CleanName += "Ae"; break;
      case 'Ü': CleanName += "Ue"; break;
      case 'Ö': CleanName += "Oe"; break;
      default:
        CleanName += '_';
      }
    }
    It++;
  }

  std::map<std::string,DocumentObject*>::const_iterator pos;

  // name in use?
  pos = ObjectMap.find(CleanName);

  if (pos == ObjectMap.end())
    // if not, name is OK
    return CleanName;
  else
  {
    // find highes sufix
    int nSuff = 0;  
    for(pos = ObjectMap.begin();pos != ObjectMap.end();++pos)
    {
      const string &rclObjName = pos->first;
      if (rclObjName.substr(0, strlen(CleanName.c_str())) == CleanName)  // Prefix gleich
      {
        string clSuffix(rclObjName.substr(strlen(CleanName.c_str())));
        if (clSuffix.size() > 0){
          int nPos = clSuffix.find_first_not_of("0123456789");
          if(nPos==-1)
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
  for( std::map<std::string,DocumentObject*>::const_iterator it = ObjectMap.begin(); it != ObjectMap.end(); ++it )
    Objects.push_back( it->second );
  return Objects;
}

std::vector<DocumentObject*> Document::getObjectsOfType(const Base::Type& typeId) const
{
  std::vector<DocumentObject*> Objects;
  for( std::map<std::string,DocumentObject*>::const_iterator it = ObjectMap.begin(); it != ObjectMap.end(); ++it )
  {
    if ( it->second->getTypeId().isDerivedFrom( typeId ) )
      Objects.push_back( it->second );
  }
  return Objects;
}

int Document::countObjectsOfType(const Base::Type& typeId) const
{
  int ct=0;
  for( std::map<std::string,DocumentObject*>::const_iterator it = ObjectMap.begin(); it != ObjectMap.end(); ++it )
  {
    if ( it->second->getTypeId().isDerivedFrom( typeId ) )
      ct++;
  }

  return ct;
}



PyObject * Document::getPyObject(void)
{
  return Py::new_reference_to(DocumentPythonObject);
}


