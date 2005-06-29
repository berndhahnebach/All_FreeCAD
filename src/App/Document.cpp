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
#	include <TDocStd_Application.hxx>
#	include <TDataStd_Real.hxx>
#	include <TDataStd_Name.hxx>
#	include <Handle_TDataStd_Name.hxx>
#	include <TDataStd_Integer.hxx>
#	include <Handle_TDataStd_Integer.hxx>
# include <TDF_MapIteratorOfLabelMap.hxx>
#endif


#include "Document.h"
#include "Feature.h"
#include "FeatureAttr.h"
#include "Label.h"

#include "../Base/PyExport.h"
#include "../Base/Console.h"
#include "../Base/Exception.h"
#include "Application.h"
#include "Attribute.h"
//#include "DocType.h"
//#include "DocTypeAttr.h"
#include "DocumentPy.h"

using Base::Console;
using Base::streq;
using namespace App;




//--------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------
Document::Document(const Handle_TDocStd_Document &hDoc)
 : _hDoc(hDoc)
{
	_pcDocPy = new DocumentPy(this);
	Console().Log("+App::Document: %p\n",this,_pcDocPy);

}

Document::~Document()
{
	_hDoc.Nullify();
	Console().Log("-App::Document %p\n",this);
}



/*
void Document::InitType(App::DocType *pcDocType)
{
	// attach the type object

	FCDocTypeAttr::Set(Main(),pcDocType);

	
	//Main()->GetOCCLabel().AddAttribute(new FCDocTypeAttr(pcDocType));

	// set up document
	pcDocType->Init(this);

}
*/

/*
DocType *Document::GetDocType(void)
{
	Handle_FCDocTypeAttr hAttr;

	Main().FindAttribute(FCDocTypeAttr::GetID(),hAttr);

	return hAttr->Get();
*/
	
	/*	Handle(FCDocTypeAttr) TSR;
	if (!Main()->GetOCCLabel().FindAttribute(FCDocTypeAttr::GetID(), TSR )) 
		return 0;

	return TSR->Get();*/
//}

//--------------------------------------------------------------------------
// Exported functions
//--------------------------------------------------------------------------

// Save the Document under a new Name
void Document::SaveAs (const char* Name)
{
	// creat a non const buffer
	Base::PyBuf name(Name);

	Handle(ApplicationOCC) hApp = GetApplication().GetOCCApp();
	if(hApp->SaveAs(_hDoc,(Standard_CString)name.str)==CDF_SS_Failure) 
		throw Base::Exception("SaveAs failed");
}
// Save the document under the name its been opened
void Document::Save (void)
{
	Handle(TDocStd_Application)::DownCast(_hDoc->Application())->Save(_hDoc);
}

bool Document::Undo(void)					
{
  return _hDoc->Undo() != 0; 
}

bool Document::Redo(void)
{
  return _hDoc->Redo() != 0; 
}

bool Document::IsSaved() const
{
	return _hDoc->IsSaved() != 0;
}


/// Get the document name of a saved document
const short* Document::GetName() const
{
  return _hDoc->GetName().ToExtString(); 
}

/// Get the path of a saved document
const short* Document::GetPath() const
{
  return _hDoc->GetPath().ToExtString();
}

/*
/// Get the Main Label of the document
FCPyHandle<FCLabel> Document::Main() 
{
	if(!_hcMain){
		_hcMain = new FCLabel(_hDoc->Main(),this);
		//_hcMain->IncRef();
		mcLabelMap[_hDoc->Main()] = _pcMain;
	}
	return  FCPyHandle<FCLabel>(_pcMain);
}
*/
/// Get the Main Label of the document
TDF_Label Document::Main() 
{
	return  _hDoc->Main();
}

/// Test if the document is empty
bool Document::IsEmpty() const
{
  return _hDoc->IsEmpty() != 0; 
}

/// Returns False if the  document  contains notified modifications.
bool Document::IsValid() const
{
  return _hDoc->IsValid() != 0;
}

/*
/// Set a special Label as modified
void Document::SetModified(FCLabel* L)
{
  _hDoc->SetModified(L->GetOCCLabel()); 
}
*/
/// Remove all modifications. After this call The document becomesagain Valid.
void Document::PurgeModified()
{
  _hDoc->PurgeModified(); 
}

/// New Command (Short cut for Commit and Open transaction)
void Document::NewCommand() 
{
  _hDoc->NewCommand(); 
}

/// returns True if a Command transaction is open
bool Document::HasOpenCommand() const
{
  return _hDoc->HasOpenCommand() != 0; 
}

/** Open a new command transaction.
 *  Raise a OCC Exception If a Command is already open.
 *  You may   check  it with the   previous method <HasOpenCommand>.
 */
void Document::OpenCommand()
{
  _hDoc->OpenCommand(); 
}

/// Commit the Command transaction. Do nothing If there is no Command transaction open.
void Document::CommitCommand()
{
  _hDoc->CommitCommand();
}

/// Abort the  Command  transaction. Do nothing If there is no Command transaction open.
void Document::AbortCommand()
{
  _hDoc->AbortCommand(); 
}

/// The current limit on the number of undos
int Document::GetUndoLimit() const
{
  return _hDoc->GetUndoLimit(); 
}

/** Set the limit on the number of Undo Deltas stored.
 *  0 will disable Undo on the document A negative value
 *  means no limit. Note that by default Undo is disabled.
 *  Enabling it will take effect with the next call to
 *  NewCommand. Of course this limit is the same for Redo.
 */
void Document::SetUndoLimit(const int L)
{
  _hDoc->SetUndoLimit(L); 
}

/// Remove all stored Undos and Redos
void Document::ClearUndos()
{
  _hDoc->ClearUndos(); 
}

/// Returns the  number  of stored Undos. If greater than 0 Undo will be effective.
int Document::GetAvailableUndos() const
{
  return _hDoc->GetAvailableUndos(); 
}

/// Returns the number   of stored Redos. If greater than 0 Redo will be effective.
int Document::GetAvailableRedos() const
{
  return _hDoc->GetAvailableRedos(); 
}

/// Recompute if the document was  not valid and propagate the reccorded modification.
void Document::Recompute()
{  
  DocChanges DocChange;
  
  TDF_MapIteratorOfLabelMap It;

  for(It.Initialize(_LogBook.GetTouched());It.More();It.Next())
  {
    // Get the Feature of this label
    Feature *Feat = Feature::GetFeature(It.Key());
    if(!Feat) continue;

    // find the name of this label
    Handle(TDataStd_Name) hName;
    if(! It.Key().FindAttribute(TDataStd_Name::GetID(),hName))
      throw Base::Exception("DocTypeStd::UpdateDoc() internal error, feature label with no name\n");

    // map the new features
    if (Feat->_eStatus == Feature::New)
      DocChange.NewFeatures.push_back(Feat);

		if (Feat->MustExecute(_LogBook))
		{
			//_LogBook.SetTouched(It.Key());
      Base::Console().Log("Update: %s\n",TCollection_AsciiString(hName->Get()).ToCString());
      if(Feat->Execute(_LogBook)){
        Feat->_eStatus = Feature::Error;
        Base::Console().Message("Recompute of Feature failed (%s)\n",Feat->getStatus());
        DocChange.ErrorFeatures.push_back(Feat);
      }else{
        DocChange.UpdatedFeatures.push_back(Feat);
        Feat->_eStatus = Feature::Valid;

      }
		}

  }

  _LogBook.Clear();

  _hDoc->Recompute(); 

  Notify(DocChange);

}

Feature *Document::AddFeature(const char* sName)
{
	Feature *pcFeature = FeatureFactory().Produce(sName);

	if(pcFeature)
	{
		// next free label
		TDF_Label FeatureLabel = _lFeature.FindChild(_iNextFreeFeature++);
		// mount the feature on its place
		FeatureAttr::Set(FeatureLabel,pcFeature);
		// name
		TDataStd_Name::Set(FeatureLabel,TCollection_ExtendedString((Standard_CString) sName ));
		// the rest of the setup do the feature itself
		pcFeature->AttachLabel(FeatureLabel,this);

    // set the status of the feature to New
    pcFeature->_eStatus = Feature::New;

		// update the pointer
		_lActiveFeature = FeatureLabel;

		// return the feature
		return pcFeature;

	}else return 0;

}

Feature *Document::GetActiveFeature(void)
{
  // checks if there is no active Feature
  if(_lActiveFeature.IsNull()) 
    return 0;
  
  return Feature::GetFeature(_lActiveFeature);
}


void Document::Init (void)
{
	TDF_Label lMain = Main();

	_lBase    = lMain.FindChild(1);
	_lPos     = lMain.FindChild(2);
	_lFeature = lMain.FindChild(3);


	TDataStd_Name::Set(_lBase,    TCollection_ExtendedString((Standard_CString)"Base"));
	TDataStd_Name::Set(_lPos,     TCollection_ExtendedString((Standard_CString)"Pos"));
	TDataStd_Name::Set(_lFeature, TCollection_ExtendedString((Standard_CString)"Features"));

	_iNextFreeFeature = 1;
//	_lActiveFeature; 
}

/// Returns the storage string of the document.
const short* Document::StorageFormat() const
{
  return _hDoc->StorageFormat().ToExtString(); 
}

/// Change the storage format of the document.
void Document::ChangeStorageFormat(const short* sStorageFormat) 
{
  _hDoc->ChangeStorageFormat((Standard_ExtString)sStorageFormat); 
}

void Document::TouchState(const TDF_Label &l)
{
  _LogBook.SetTouched(l);
}

void Document::ImpactState(const TDF_Label &l)
{
  _LogBook.SetImpacted(l);
}



/*
FCLabel *Document::HasLabel(TDF_Label cLabel)
{
	FCLabel *pcL;
	std::map <TDF_Label,FCLabel*,LabelLess>::iterator It;
	
	// find a FCLabel if possible
	It = mcLabelMap.find(cLabel);

	// if not
	if(It == mcLabelMap.end())
	{
		// create a new label and append it to the map
		pcL = new FCLabel(cLabel,this);
		mcLabelMap[cLabel] = pcL;
		// taking care that python not delete the object
		pcL->IncRef();
		return pcL;
	}
	// if yes return the found FCLabel
	return It->second;

}

*/

void Document::Dump(void)
{
  _hDoc->Main().Dump(std::cout);
}

Base::PyObjectBase * Document::GetPyObject(void)
{
	return _pcDocPy;
}


