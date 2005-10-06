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
# include <fcntl.h>
# include <TopTools_HSequenceOfShape.hxx>
# include <IGESControl_Writer.hxx>
# include <IGESControl_Reader.hxx>
# include <TopoDS_Shape.hxx>
# include <TFunction_Logbook.hxx>
#endif

#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/Sequencer.h>
#include "FeatureImportIges.h"


using namespace Import;

void FeatureImportIges::InitLabel(const TDF_Label &rcLabel)
{
	Base::Console().Log("FeaturePartImportStep::InitLabel()\n");

	addProperty("String","FileName");

}

/*
bool FeaturePartImportStep::MustExecute(const TFunction_Logbook& log)
{
	Base::Console().Log("PartBoxFeature::MustExecute()\n");
	return false;
}
*/
Standard_Integer FeatureImportIges::Execute(TFunction_Logbook& log)
{
	Base::Console().Log("FeaturePartImportIges::Execute()\n");

/*  cout << GetFloatProperty("x") << endl;
  cout << GetFloatProperty("y") << endl;
  cout << GetFloatProperty("z") << endl;
  cout << GetFloatProperty("l") << endl;
  cout << GetFloatProperty("h") << endl;
  cout << GetFloatProperty("w") << endl;*/

  try{

    IGESControl_Reader aReader;
    TopoDS_Shape aShape;

    std::string FileName = getPropertyString("FileName");

    int i=open(FileName.c_str(),O_RDONLY);
	  if( i != -1)
	  {
		  close(i);
	  }else{
      Base::Console().Log("FeaturePartImportIges::Execute() not able to open %s!\n",FileName.c_str());
		  return 1;
	  }

    // just do show the wait cursor when the Gui is up
    Base::Sequencer().start("Load IGES", 1);
    Base::Sequencer().next();

    // read iges-file
    if (aReader.ReadFile((const Standard_CString)FileName.c_str()) != IFSelect_RetDone)
      throw Base::Exception("IGES read failed (load file)");
  
    // check iges-file (memory)
    //if (!aReader.Check(Standard_True))
    //  Base::Console().Warning( "IGES model contains errors! try loading anyway....\n" );
  
    // make brep
    aReader.TransferRoots();
    // one shape, who contain's all subshapes
    aShape = aReader.OneShape();

	  setShape(aShape);
    Base::Sequencer().stop();
  }
  catch(...){
    Base::Sequencer().halt();
    Base::Console().Error("FeaturePartImportIges::Execute() failed!");
    return 1;
  }

  return 0;
}


void FeatureImportIges::Validate(TFunction_Logbook& log)
{
	Base::Console().Log("FeaturePartImportStep::Validate()\n");
 
  // We validate the object label ( Label() ), all the arguments and the results of the object:
  log.SetValid(Label(), Standard_True);


}




// sample from OCC Ocaf
#ifdef false

void CCmdLoadShapeFile::Do (void)
{
  TopoDS_Shape aShape, cl_Topo_DS_IgesShape;
  
  // extractin name and suffix
  CRSString::size_type pos = _clFileName.find_last_of("\\");
  CRSString name = _clFileName.substr(pos+1);
  pos = name.find_last_of(".");
  CRSString endung = name.substr(pos);
  RSfor_each(endung.begin(),endung.end(),RSTools::ToLower);
  CRSString name_pure = name.substr(0,pos);
  
  // Containers
  HRSGeoShapeModel  hGeoShapeModel;
  HRSGeoFeatureGrp  hFeatureGroup;
  
  CRSString               sName;
  try{
    if(endung == ".brep" || endung == ".rle")
      LoadBREP(aShape);
    else if(endung == ".igs"|| endung == ".iges")
      LoadIGES(aShape);
    else if(endung == ".vda"|| endung == ".vdafs")
      LoadVDAFS(aShape,hFeatureGroup);
    else if(endung == ".stp"|| endung == ".step")
      LoadSTEP(aShape);
    else
    {
      GetReportOutput().PrintWrg( "Unsuported Type....!\r\n" );
    }
  }catch(...){
    GetReportOutput().PrintErr( "Read Failed....!\r\n" );
  }
  
  if ( !aShape.IsNull() ) {
    hGeoShapeModel = new CRSGeoShapeModel(aShape);
    hGeoShapeModel->SetName(name_pure);
    
    TopExp_Explorer  cExp;
    int i;
    
    // extracting face Attributes
    for (cExp.Init(hGeoShapeModel->GetShape(),TopAbs_FACE), i=1; cExp.More(); cExp.Next(), i++) {
      TopoDS_Face F = TopoDS::Face(cExp.Current());
      HRSGeoFace hFace = new  CRSGeoFace(F);
      
      //sprintf(szTemp, "Face_%04d", i); 
      //sName = szTemp;
      //hFace->SetName(sName);
      //hFace->SetColor(3);
      
      hGeoShapeModel->AddAttribute(hFace);
    }
    
    //if (hGeoShapeModel->GetNumberOfChilds() == 0)
    {
      // extracting wire Attributes
      for (cExp.Init(hGeoShapeModel->GetShape(),TopAbs_WIRE,TopAbs_FACE), i=1; cExp.More(); cExp.Next(), i++) {
        TopoDS_Wire F = TopoDS::Wire(cExp.Current());
        HRSGeoWire hWire = new  CRSGeoWire(F);
        
        //sprintf(szTemp, "Edge_%04d", i); 
        //sName = szTemp;
        //hWire->SetName(sName);
        //hWire->SetColor(3);
        
        hGeoShapeModel->AddAttribute(hWire);
      }
    }
    //if (hGeoShapeModel->GetNumberOfChilds() == 0)
    {
      // extracting wire Attributes
      for (cExp.Init(hGeoShapeModel->GetShape(),TopAbs_EDGE,TopAbs_WIRE), i=1; cExp.More(); cExp.Next(), i++) {
        TopoDS_Edge F = TopoDS::Edge(cExp.Current());
        HRSGeoEdge hEdge = new  CRSGeoEdge(F);
        
        //sprintf(szTemp, "Edge_%04d", i); 
        //sName = szTemp;
        //hEdge->SetName(sName);
        //hEdge->SetColor(3);
        
        hGeoShapeModel->AddAttribute(hEdge);
      }
    }
    /*
    if (hGeoShapeModel->GetNumberOfChilds() == 0)
    {
    // extracting wire Attributes
    for (cExp.Init(hGeoShapeModel->GetShape(),TopAbs_VERTEX), i=1; cExp.More(); cExp.Next(), i++) {
    TopoDS_Vertex F = TopoDS::Vertex(cExp.Current());
    HRSGeoVertex hVertex = new  CRSGeoVertex(F);
    
      sprintf(szTemp, "Vertex_%04d", i); 
      sName = szTemp;
      hVertex->SetName(sName);
      //hEdge->SetColor(3);
      
        hGeoShapeModel->AddAttribute(hEdge);
        }
        }
    */
    GetDoc()->Append(hGeoShapeModel);
    GetDoc()->Activate(hGeoShapeModel);
    _hObj = hGeoShapeModel;
  }
  if ( hFeatureGroup.IsValid() ) {
    hFeatureGroup->SetName(GetDoc()->GetUniqueName(name_pure + "_MDIS"));
    GetDoc()->Append(hFeatureGroup);
    _hObj2 = hFeatureGroup;
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Importing BREP
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void CCmdLoadShapeFile::LoadBREP(TopoDS_Shape &aShape)
{
  BRep_Builder aBuilder;
  try{
    BRepTools::Read(aShape,(const Standard_CString)_clFileName.c_str(),aBuilder);
  }
  // Boeser Fehler ;-)
  catch(...){
    throw new CIOException("Error loading BREP file");
  }  
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Importing IGES
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CCmdLoadShapeFile::LoadIGES(TopoDS_Shape &aShape)
{
#ifdef _ADVANCED_DATA_EXC_
  IGESControl_Reader     clIgesReader;
#else
  IGESControlStd_Reader  clIgesReader;
#endif
  // read iges-file
  if (clIgesReader.ReadFile((const Standard_CString)_clFileName.c_str()) != IFSelect_RetDone)
    throw new CIOException("IGES read failed (load file)");
  
  // check iges-file (memory)
  if (!clIgesReader.Check(Standard_True))
    GetReportOutput().PrintWrg( "IGES model contains errors ....!\r\ntry loading anyway....\r\n" );
  
  try{
    // make brep
    clIgesReader.TransferRoots(Standard_False);
    // one shape, who contain's all subshapes
    aShape = clIgesReader.OneShape();
  }
  // Boeser Fehler ;-)
  catch(...)
  {throw new CIOException("IGES Reader crashd!");}
  
  // Lesefehler
  if (aShape.IsNull())
    throw new CIOException("IGES read failed");
  
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Importing STEP
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CCmdLoadShapeFile::LoadSTEP(TopoDS_Shape &aShape)
{
#ifdef _ADVANCED_DATA_EXC_
  STEPControl_Reader aReader;
#else
  STEPControlStd_Reader aReader;
#endif
  Handle(TopTools_HSequenceOfShape) aHSequenceOfShape = new TopTools_HSequenceOfShape;
  if (aReader.ReadFile((const Standard_CString)_clFileName.c_str()) != IFSelect_RetDone)
  {}
  
  // Root transfers
  Standard_Integer nbr = aReader.NbRootsForTransfer();
  //aReader.PrintCheckTransfer (failsonly, IFSelect_ItemsByEntity);
  for ( Standard_Integer n = 1; n<= nbr; n++)
  {
    printf("STEP: Transfering Root %d\n",n);
    Standard_Boolean ok = aReader.TransferRoot(n);
    // Collecting resulting entities
    Standard_Integer nbs = aReader.NbShapes();
    if (nbs == 0) {
      aHSequenceOfShape.Nullify();
      return ;
    } else {
      for (Standard_Integer i =1; i<=nbs; i++) {
        printf("STEP:   Transfering Shape %d\n",n);
        aShape=aReader.Shape(i);
        aHSequenceOfShape->Append(aShape);
      }
    }
  }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Importing VDAFS
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


void CCmdLoadShapeFile::LoadVDAFS(TopoDS_Shape &aShape,HRSGeoFeatureGrp &hFeatureGroup)
{
#ifndef _NO_VDALIB_  
  bool bMDIRead = false;
  VDAAPI_Reader cReader;
  
  cReader.ReadFile((const Standard_CString)_clFileName.c_str());
  if(!cReader.IsDone())
    throw new CIOException("VDA read failed (load file)");
  cReader.Transfer();
  if(!cReader.IsDone()) 
    throw new CIOException("VDA read failed (transfer)");
  
  // one shape, who contain's all subshapes
  aShape = cReader.Shape();
  
  //    char cBuff[80];
  int l=1;
  /* Features read has now its own functions
  // extracting MDIs
  for ( VDAdata_EntityIterator iter(cReader.Model()); iter.More(); iter.Next() ) {
  Handle(VDAdata_Mdi) mdi = Handle(VDAdata_Mdi)::DownCast(iter.Entity());
  if ( !mdi.IsNull() ) {
  bMDIRead = true;
  for ( int i = 1; i <= mdi->Length(); i++ ) {
  HRSGeoFeaturePoint hPoint = new CRSGeoFeaturePoint;
  sprintf(cBuff,"VDA_MDI_%d",l++);
  hPoint->SetName(CRSString(cBuff));
  
    const gp_Pnt& p = mdi->Point(i);
    const gp_Vec& d = mdi->Normal(i);
    
      hPoint->SetPoint(CVector3D(float(p.X()),float(p.Y()),float(p.Z())));
      hPoint->SetW    (CVector3D(float(d.X()),float(d.Y()),float(d.Z())));
      
        if ( hFeatureGroup.IsNull() ){
        hFeatureGroup = new CRSGeoFeatureGrp();
        hFeatureGroup->SetName(GetDoc()->GetUniqueName("VDAFS_MDI"));
        }
        
          hFeatureGroup->AddChild(hPoint);
          }
          }
}*/
  
  // Lesefehler
  if (aShape.IsNull() && bMDIRead) 
    throw new CIOException("VDAFS read failed");
#else
  GetReportOutput().PrintWrg( "read VDAFS not licensed in this version ....!\r\n" );
  return;
#endif
}


bool CCmdLoadShapeFile::Undo (void)
{
  if ( _hObj.IsValid() ) _hObj->SetModified(CRSSubject::Deleted);
  if ( _hObj2.IsValid() ) _hObj2->SetModified(CRSSubject::Deleted);
  GetDoc()->Update();
  
  return true;
}

bool CCmdLoadShapeFile::Redo (void)
{
  if ( _hObj .IsValid() ) GetDoc()->Append(_hObj );
  if ( _hObj2.IsValid() ) GetDoc()->Append(_hObj2);
  return true;
}

void CCmdSaveShapeFile::Do (void)
{
  TopoDS_Shape    aShape, cl_Topo_DS_IgesShape;
  TopoDS_Compound aComp;
  
  CRSString::size_type pos = _clFileName.find_last_of("\\");
  CRSString name = _clFileName.substr(pos+1);
  pos = name.find_last_of(".");
  CRSString endung;
  if(pos == CRSString::npos){
    // Wenn nicht angegeben als brep
    endung = ".brep";
    _clFileName += ".brep";
  }else{
    endung = name.substr(pos);
  }
  const CCmdSelection &rcSel =  CCmdSelection::Instance();
  if(rcSel.Size() == 0) return; 
  
  if(endung == ".brep" || endung == ".rle"){
    // stuff selection in one compound shape
    MakeOneShape(aComp);
    // Write the Shape           
    Standard_Boolean result = BRepTools::Write(aComp,(const Standard_CString)_clFileName.c_str()); 
    // Error handling
    if(!result) throw new CIOException("BREP write failed");
    
  } else
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // Exporting IGES
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    
    if(endung == ".igs"|| endung == ".iges"){
      
#ifdef _ADVANCED_DATA_EXC_
      IGESControl_Controller::Init();
      IGESControl_Writer ICW (Interface_Static::CVal("XSTEP.iges.unit"),
#else 
        IGESControlStd_Controller::Init();
      IGESControlStd_Writer ICW (Interface_Static::CVal("XSTEP.iges.unit"),
#endif
        Interface_Static::IVal("XSTEP.iges.writebrep.mode"));
      
      // stuff selection in one compound shape
      MakeOneShape(aComp);
      
      ICW.AddShape (aComp);       
      ICW.ComputeModel();
      
      if(!ICW.Write((const Standard_CString)_clFileName.c_str()))
        throw new CIOException("IGES write failed");
    }else
      
      //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      // Exporting STEP
      //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      if(endung == ".stp"|| endung == ".step"){
        // CREATE THE WRITER
#ifdef _ADVANCED_DATA_EXC_
        STEPControl_Writer aWriter;
#else
        STEPControlStd_Writer aWriter;
#endif
        IFSelect_ReturnStatus status;
        
        // stuff selection in one compound shape
        MakeOneShape(aComp);
        
        //status =  aWriter.Transfer(aComp, TopoDSToCc1Act_ManifoldSolidBrep);
        //status =  aWriter.Transfer(aComp, STEPControlStd_ManifoldSolidBrep);
#ifdef QS_CAS40
        status =  aWriter.Transfer(aComp, STEPControl_AsIs);
#else
        status =  aWriter.Transfer(aComp, STEPControlStd_AsIs);
#endif
        if ( status != IFSelect_RetDone ) 
          throw new CIOException("STEP write failed (transfere)");
        
        try{
          status = aWriter.Write((const Standard_CString)_clFileName.c_str());
        }catch(...){
          throw new CIOException("STEP write failed (write exception)");
        }
        
        if ( status != IFSelect_RetDone ) 
          throw new CIOException("STEP write failed (write)");
        
      }
        
        //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        // Exporting STL
        //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        //    if(endung == ".stl"|| endung == ".astl"){
        
        //MakeOneShape(aComp);
        //StlAPI::write(OSD_Path, aComp, 0.1, Standard_True, Standard_True);
        
        //    }else
        //    if(endung == ".bstl"){
        
        
        //    }else
        
        //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        // Exporting VRML
        //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        //    if(endung == ".wrl"|| endung == ".vrml"){
        /*
        //--------------------------------------------
        // Define File
        //--------------------------------------------
        //filebuf foc;
        ofstream outFile(_clFileName.c_str());
        Vrml::VrmlHeaderWriter(outFile);
        Vrml::CommentWriter(" VRML by QSpect (tm) DaimlerChrysler 99 ",outFile);
        //--------------------------------------------
        // Definition of the Attribute Manager
        //--------------------------------------------
        Handle(VrmlConverter_Drawer) aDrawer = new VrmlConverter_Drawer;
        aDrawer->SetMaximalChordialDeviation(0.01);
        aDrawer->SetDiscretisation(1);
        //--------------------------------------------
        // Write Shaded Shape
        //--------------------------------------------  
        MakeOneShape(aComp);
        VrmlConverter_ShadedShape::Add(outFile, aComp, aDrawer);
        */
        //    }else
        
        //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        // Exporting VDAFS
        //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifndef _NO_VDALIB_
        else if(endung == ".vda"|| endung == ".vdafs"){
          VDAAPI_Writer cWriter;
          
          MakeOneShape(aComp);
          
          cWriter.Transfer(aComp);
          
          if( !cWriter.IsDone() )
            throw new CIOException("unable to write VDAFS data"); 
          
          cWriter.WriteFile((const Standard_CString)_clFileName.c_str());
          if( !cWriter.IsDone() )
            throw new CIOException("unable to write VDAFS data to disk"); 
          
        }
#endif
        else
        {
          throw new CIOException("Unknown save formate");
        }
}

void CCmdSaveShapeFile::MakeOneShape(TopoDS_Compound &aComp){
  BRep_Builder aBuilder;
  aBuilder.MakeCompound(aComp);
  const CCmdSelection &rcSel =  CCmdSelection::Instance();
  int n = 0;            
  for (CCmdSelection::_TConstIterator It=rcSel.Begin();It != rcSel.End();It++){
    if(It->_hObject->IsTypeOf(typeid(CRSGeoShape))){
      HRSGeoShape hShape;
      hShape.AssignFrom(It->_hObject);
      const TopoDS_Shape& shape = hShape->GetShape();
      // Add Shape to Compound
      n++;
      if ( n == 1 ) {
        if ( shape.ShapeType() == TopAbs_COMPOUND )
          aComp = TopoDS::Compound(shape);
        else {
          aBuilder.MakeCompound(aComp);
          aBuilder.Add(aComp,shape);
          n=2;
        }
      }
      else if ( n == 2 ) {
        TopoDS_Compound com;
        aBuilder.MakeCompound(com);
        aBuilder.Add(com,aComp);
        aBuilder.Add(com,shape);
        aComp=com;
      }
      else
        aBuilder.Add(aComp,shape);
      
    }
  }
  
}

IFSelect_ReturnStatus CCmdSaveShapeFile::SaveDXF(const Standard_CString& aFileName,
                                                 const Handle(TopTools_HSequenceOfShape)& aHSequenceOfShape)
{
/*
DxfControl_Controller::Init();
Handle(XSControl_WorkSession) WS = new XSControl_WorkSession;
WS->SelectNorm ("DXF");
DxfControl_Writer aWriter (WS);
TopoDS_Compound C;
BRep_Builder MKCP;
MKCP.MakeCompound(C);
IFSelect_ReturnStatus status;
for (Standard_Integer i=1;i<=aHSequenceOfShape->Length();i++)  
{
TopoDS_Shape S = aHSequenceOfShape->Value(i);
cout<< S.ShapeType()<<endl;
// get faces for solid and shells because the interface deals only with faces, edges and vertices
if (S.ShapeType() == TopAbs_SOLID || S.ShapeType() == TopAbs_SHELL) {
TopExp_Explorer Ex;
for (Ex.Init(S,TopAbs_FACE); Ex.More(); Ex.Next()) 
MKCP.Add(C,Ex.Current());
status =  aWriter.TransferShape(C);
}
else
status =  aWriter.TransferShape(aHSequenceOfShape->Value(i));
if ( status != IFSelect_RetDone ) return status;
}     
status = aWriter.WriteFile(aFileName);
return status;
  */
  return IFSelect_RetError;
}

IFSelect_ReturnStatus CCmdSaveShapeFile::ReadDXF(const Standard_CString& aFileName,
                                                 Handle(TopTools_HSequenceOfShape)& aHSequenceOfShape)
{
/*
DxfControl_Controller::Init();
DxfControl_Reader dxfReader;
//  MoniTool_TraceFile::SetDefStandard();
//  MoniTool_TraceFile::SetDefLevel(2);
IFSelect_ReturnStatus status = dxfReader.ReadFile(aFileName);
if (status == IFSelect_RetDone){

  // Transfer roots
  Standard_Integer numRoots = dxfReader.NbRootsForTransfer();
  cout<<"Nbre de racines: "<<numRoots<<endl;
  TopoDS_Compound RES;
  BRep_Builder MKCP;
  MKCP.MakeCompound(RES);
  //BarIndicator bar(NULL,numRoots);
  //bar.Create(BarIndicator::IDD,NULL);
  //bar.CenterWindow(NULL);
  for (Standard_Integer i=1;i<=numRoots;i++) {
  //bar.m_progress.OffsetPos(1);
  dxfReader.ClearShapes();
  if (dxfReader.TransferOneRoot(i)) {
  // Get the resulting shapes
  Standard_Integer numShapes = dxfReader.NbShapes();
  for (Standard_Integer j=1;j<=numShapes;j++) {
  MKCP.Add(RES,dxfReader.Shape(j));
  }
  }
  
    }
    aHSequenceOfShape->Append(RES);
    }
    else
    {
    aHSequenceOfShape.Nullify();
    }
    cout<<"Transfert termine"<<endl;
    return status;
  */
  return IFSelect_RetError;
}

IFSelect_ReturnStatus CCmdSaveShapeFile::ReadSAT(const Standard_CString& aFileName,
                                                 Handle(TopTools_HSequenceOfShape)& aHSequenceOfShape)
{
/*
DxfSatControl_Controller::Init();
DxfSatControl_Reader satReader;
IFSelect_ReturnStatus status = satReader.ReadFile(aFileName);

  if (status == IFSelect_RetDone){
  
    // Transfer roots
    Standard_Integer numRoots = satReader.NbRootsForTransfer();
    Standard_Integer i;
    TopoDS_Compound RES;
    BRep_Builder MKCP;
    MKCP.MakeCompound(RES);
    //BarIndicator bar(NULL,numRoots);
    //bar.Create(BarIndicator::IDD,NULL);
    //bar.CenterWindow(NULL);
    for (i=1;i<=numRoots;i++) {
    //bar.m_progress.OffsetPos(1);
    if (satReader.TransferOneRoot(i)) {
    // Get the resulting shapes
    TopoDS_Shape S;
    Standard_Integer numShapes = satReader.NbShapes();
    for (Standard_Integer j=1;j<=numShapes;j++) {
    S = satReader.Shape(j);
    MKCP.Add(RES,S);
    }
    }
    
      }
      aHSequenceOfShape->Append(RES);
      }
      else
      {
      aHSequenceOfShape.Nullify();
      }
      return status;
  */
  return IFSelect_RetError;
}

#endif
