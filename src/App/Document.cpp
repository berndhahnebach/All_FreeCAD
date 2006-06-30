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


#include "PreCompiled.h"

#ifndef _PreComp_
#endif


#include "Document.h"
#include "DocumentPy.h"
#include "Application.h"
#include "Feature.h"
#include "PropertyLinks.h"

#include <Base/PyExport.h>
#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/FileInfo.h>
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

using namespace zipios ;


PROPERTY_SOURCE(App::Document, App::PropertyContainer)


bool Document::undo(void)					
{
  if(_iUndoMode)
  {
    if(activUndoTransaction)
      commitCommand();
  
    assert(mUndoTransactions.size()!=0);

    // redo
    activUndoTransaction = new Transaction();
    // applieing the undo
    mUndoTransactions.top()->apply(*this);
    // save the redo
    mRedoTransactions.push(activUndoTransaction);
    activUndoTransaction = 0;
    
    delete mUndoTransactions.top();
    mUndoTransactions.pop();
  }

  return false; 
}

bool Document::redo(void)
{
  if(_iUndoMode)
  {
    if(activUndoTransaction)
      commitCommand();
  
    assert(mRedoTransactions.size()!=0);

    // undo
    activUndoTransaction = new Transaction();
    mRedoTransactions.top()->apply(*this);
    mUndoTransactions.push(activUndoTransaction);
    activUndoTransaction = 0;
    
    delete mRedoTransactions.top();
    mRedoTransactions.pop();
  }

  return false; 
}

void Document::newCommand() 
{
  openCommand();
}

void Document::openCommand()
{
  if(_iUndoMode)
  {
    if(activUndoTransaction)
      commitCommand();

    activUndoTransaction = new Transaction();
  }
  
}

void Document::commitCommand()
{
  if(activUndoTransaction)
  {
    mUndoTransactions.push(activUndoTransaction);
    activUndoTransaction = 0;
  }
  
}

void Document::abortCommand()
{
   
}


void Document::clearUndos()
{

  while(!mUndoTransactions.empty())
  {
    delete mUndoTransactions.top();
    mUndoTransactions.pop();
  }

  while(!mRedoTransactions.empty())
  {
    delete mRedoTransactions.top();
    mRedoTransactions.pop();
  }
}

int Document::getAvailableUndos() const
{
  return mUndoTransactions.size();
}

int Document::getAvailableRedos() const
{
  return mRedoTransactions.size();
}

void Document::setUndoMode(int iMode)
{
  _iUndoMode = iMode;
}


void Document::onBevorChangeProperty(const DocumentObject *Who, const Property *What)
{
  if(activUndoTransaction)
  {
    activUndoTransaction->addObjectChange(Who,What);

  }else
    // if the Undo is switched on, never do a change without openCommand()!
    assert(_iUndoMode==0);

}

void Document::onChangedProperty(const DocumentObject *Who, const Property *What)
{
  if(this->activTransaction)
  {
    this->activTransaction->addObjectChange(Who,What);

  }
}

void Document::setTransactionMode(int iMode)
{
  if(_iTransactionMode == 0 && iMode == 1)
    beginTransaction();
   
  if(activTransaction && iMode == 0)
    endTransaction();
  
  _iTransactionMode = iMode;

}


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







//--------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------
Document::Document(void)
: _iTransactionMode(0),
  iTransactionCount(0),
  activTransaction(0),
  _iUndoMode(0),
  activUndoTransaction(0),
  pActiveObject(0),
  pDocumentHook(0),
  _pcDocPy(0)
{
  // Remark: In a constructor we should never increment a Python object as we cannot be sure
  // if the Python interpreter gets a reference of it. E.g. if we increment but Python don't
  // get a reference then the object wouldn't get deleted in the destructor.
  // So, we must increment only if the interpreter gets a reference.
	_pcDocPy = new DocumentPy(this);

  Console().Log("+App::Document: %p\n",this,_pcDocPy);


  ADD_PROPERTY(Name,("Unnamed"));
  ADD_PROPERTY(FileName,(""));


}

Document::~Document()
{
  Console().Log("-App::Document: %s %p\n",getName(), this);

  clearUndos();
  
  std::map<std::string,DocumentObject*>::iterator it;

  Console().Log("-Delete Features of %s \n",getName());

  for(it = ObjectMap.begin(); it != ObjectMap.end(); ++it)
  {
    delete(it->second);
  }

  // Call before decrementing the reference counter, otherwise a heap error can occur
  _pcDocPy->setInvalid();
  _pcDocPy->DecRef(); // decrement by one
}





//--------------------------------------------------------------------------
// Exported functions
//--------------------------------------------------------------------------


void Document::Save (Writer &writer) const
{
  writer << "<?xml version='1.0' encoding='utf-8'?>" << endl
         << "<!--" << endl
         << " FreeCAD Document, see http://free-cad.sourceforge.net for more informations..." << endl
         << "-->" << endl;

  writer << "<Document SchemaVersion=\"3\">" << endl;

  PropertyContainer::Save(writer);

  // writing the features types
  writer << writer.ind() << "<Objects Count=\"" << ObjectMap.size() <<"\">" << endl;

  std::map<std::string,DocumentObject*>::const_iterator it;
  for(it = ObjectMap.begin(); it != ObjectMap.end(); ++it)
  {
    DocumentObject* obj = it->second;
    writer << writer.ind() << "<Object " 
                             << "type=\"" << obj->getTypeId().getName() << "\" "
                             << "name=\"" << obj->name.getValue()       << "\" "
                           << "/>" << endl;    
  }

  writer << writer.ind() << "</Objects>" << endl;
  
  // writing the features itself
  writer << writer.ind() << "<ObjectData Count=\"" << ObjectMap.size() <<"\">" << endl;

  for(it = ObjectMap.begin(); it != ObjectMap.end(); ++it)
  {
    DocumentObject* obj = it->second;
    writer << writer.ind() << "<Object name=\"" << obj->name.getValue() << "\">" << endl;   
    obj->Save(writer);
    writer << writer.ind() << "</Object>" << endl;
  }

  writer << writer.ind() << "</ObjectData>" << endl;
  writer << "</Document>" << endl;

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

      addObject(type.c_str(),name.c_str());
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

      addObject(type.c_str(),name.c_str());
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

// Save the Document under a new Name
void Document::saveAs (const char* name)
{
  Base::FileInfo File(name);

  //Base::ogzstream file(File.filePath().c_str());
  //ofstream file(File.filePath().c_str());

  std::string oldName = Name.getValue();
  Name.setValue(File.fileNamePure());
  FileName.setValue(File.filePath());

  Document::save();

  GetApplication().renameDocument(oldName.c_str(), Name.getValue());

  DocChanges DocChange;
  DocChange.Why = DocChanges::Rename;

  Notify(DocChange);
}

// Save the document under the name its been opened
bool Document::save (void)
{
  int compression = App::GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Preferences/Document")->GetInt("CompressionLevel",3);

  if(*(FileName.getValue()) != '\0')
  {
    Base::Writer writer(FileName.getValue());

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

    writer.close();

    return true;
  }

  return false;
}

// Open the document
bool Document::open (void)
{
  std::string FilePath = FileName.getValue();
  zipios::ZipInputStream zipstream(FileName.getValue());

  Base::XMLReader reader(FileName.getValue(), zipstream);

  if ( reader.isValid() )
  {
    Document::Restore(reader);

    // We must restore the correct 'FileName' property again because the stored value could be invalid.
    FileName.setValue(FilePath.c_str());

    reader.readFiles(zipstream);

    // notify all as new
    DocChanges DocChange;
    for(std::map<std::string,DocumentObject*>::iterator It = ObjectMap.begin();It != ObjectMap.end();++It) {
      DocChange.NewObjects.insert(It->second);
      if(It->second->getTypeId().isDerivedFrom(AbstractFeature::getClassTypeId()) )
      {
        AbstractFeature* feat = dynamic_cast<AbstractFeature*>(It->second);
        feat->touchTime.setToActual();
        if ( feat->status.getValue() == AbstractFeature::New )
          feat->status.setValue( AbstractFeature::Valid );
      }
    }

    Notify(DocChange);

    // Special handling for Gui document
    if (pDocumentHook) {
      zipios::ConstEntryPointer entry = zipstream.getNextEntry();
      if ( entry->isValid() ) {
        pDocumentHook->RestoreDocFile( zipstream );
      }
    }

    return true;
  }
  return false;
}

bool Document::isSaved() const
{
  std::string name = FileName.getValue();
	return !name.empty();
}

/// Get the document name of a saved document
const char* Document::getName() const
{
  return Name.getValue();
}

/// Get the path of a saved document
const char* Document::getPath() const
{
  return FileName.getValue();//_hDoc->GetPath().ToExtString();
}


/// Remove all modifications. After this call The document becomesagain Valid.
void Document::PurgeModified()
{
  //_hDoc->PurgeModified(); 
}

/// Recompute if the document was  not valid and propagate the reccorded modification.
void Document::recompute()
{  
  int iSentinel = 20;
  bool goOn;
  DocChanges DocChange;
  DocChange.Why = DocChanges::Recompute;

  std::set<DocumentObject*>::iterator i;
  std::set<AbstractFeature*>::iterator l;
  std::set<AbstractFeature*> tempErr;
  
//  TDF_MapIteratorOfLabelMap It;
  Base::Console().Log("Solv: Start recomputation of document: \"%s\"\n",Name.getValue());

  do{
    goOn = false;
    tempErr = DocChange.ErrorFeatures;

    std::map<std::string,DocumentObject*>::iterator It;

    for(It = ObjectMap.begin();It != ObjectMap.end();++It)
    {
      if( ! (It->second->getTypeId().isDerivedFrom(AbstractFeature::getClassTypeId()) ) )
        continue;
      AbstractFeature *feat = dynamic_cast<AbstractFeature *>(It->second);

      // map the new features
      if (feat->getStatus() == AbstractFeature::New)
        DocChange.NewObjects.insert(feat);

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
  for(i = DocChange.NewObjects.begin();i!=DocChange.NewObjects.end();++i)
    DocChange.UpdatedObjects.erase(*i);

  Notify(DocChange);

  for(l = DocChange.ErrorFeatures.begin();l!=DocChange.ErrorFeatures.end();++l)
    Base::Console().Log("Error in Feature \"%s\": %s\n",(*l)->name.getValue(),(*l)->getErrorString());

  Base::Console().Log("Solv: Recomputation of Document \"%s\" with %d new, %d Updated and %d errors finished\n",
                      getName(),
                      DocChange.NewObjects.size(),
                      DocChange.UpdatedObjects.size(),
                      DocChange.ErrorFeatures.size());
  
}

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
    Base::Console().Error("Memory exception in feature '%s' thrown: %s\n",Feat->name.getValue(),e.what());
    Feat->setError(e.what());
    succes = 3;
  }catch(Base::Exception &e){
    e.ReportException();
    succes = 3;
  }catch(std::exception &e){
    Base::Console().Warning("CasCade exception in Feature \"%s\" thrown: %s\n",Feat->name.getValue(),e.what());
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
  }
}



DocumentObject *Document::addObject(const char* sType, const char* pObjectName)
{
  App::DocumentObject* pcObject = (App::DocumentObject*) Base::Type::createInstanceByName(sType,true);


  string ObjectName;

	if(pcObject)
	{
    assert(pcObject->getTypeId().isDerivedFrom(App::DocumentObject::getClassTypeId()));

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

    ObjectMap[ObjectName] = pcObject;

    pcObject->name.setValue( ObjectName );
	
    // return the Object
		return pcObject;

	}else 
    return 0;

}

void Document::_addObject(DocumentObject* pcObject, const char* pObjectName)
{

  ObjectMap[pObjectName] = pcObject;

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

  Notify(DocChange);

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

}


DocumentObject *Document::getActiveObject(void)
{
  
  return pActiveObject;
}

DocumentObject *Document::getObject(const char *Name)
{
  std::map<std::string,DocumentObject*>::iterator pos;
  
  pos = ObjectMap.find(Name);

  if(pos != ObjectMap.end())
    return pos->second;
  else
    return 0;
}

const char *Document::getObjectName(DocumentObject *pFeat)
{
  std::map<std::string,DocumentObject*>::iterator pos;

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
    sprintf(szName, "%s%d", CleanName.c_str(), nSuff + 1);
	
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



Base::PyObjectBase * Document::GetPyObject(void)
{
  _pcDocPy->IncRef();
  return _pcDocPy;
}


