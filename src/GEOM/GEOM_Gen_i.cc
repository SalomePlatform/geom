//  GEOM GEOM : implementaion of GEOM_Gen.idl and GEOM_Shape.idl
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
// 
//  This library is free software; you can redistribute it and/or 
//  modify it under the terms of the GNU Lesser General Public 
//  License as published by the Free Software Foundation; either 
//  version 2.1 of the License. 
// 
//  This library is distributed in the hope that it will be useful, 
//  but WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
//  Lesser General Public License for more details. 
// 
//  You should have received a copy of the GNU Lesser General Public 
//  License along with this library; if not, write to the Free Software 
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
// 
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : GEOM_GEN_i.cc file
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

using namespace std;
#include "GEOM_Gen_i.hh"

#include "Partition_Spliter.hxx"
#include "Archimede_VolumeSection.hxx"
#include "Sketcher_Profile.hxx"

#include "Utils_CorbaException.hxx"
#include "utilities.h"

#include <stdio.h>

// Cascade headers

#include <Standard_Failure.hxx>

#include <gp_Circ.hxx>
#include <gp_Pln.hxx>
#include <gp_Elips.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Line.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <GeomFill_Line.hxx>
#include <GeomFill_AppSurf.hxx>
#include <GeomFill_SectionGenerator.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GC_MakeArcOfCircle.hxx>
#include <GeomAPI_PointsToBSpline.hxx>
#include <GC_Root.hxx>

#include <BRepCheck_Analyzer.hxx>
#if OCC_VERSION_MAJOR >= 5
#include <BRepAlgo.hxx>
#else
#include <BRepAlgoAPI.hxx>
#endif
#include <BRepAlgo_FaceRestrictor.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepAlgoAPI_Common.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgoAPI_Section.hxx>
#include <BRepOffsetAPI_Sewing.hxx>
#include <BRepOffsetAPI_MakePipe.hxx>

#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>

#include <BRepLib.hxx>
#include <BRepBndLib.hxx>
#include <Bnd_Box.hxx>

#include <BRepBuilderAPI_MakeShell.hxx>
#include <BRepPrim_Builder.hxx>
#include <BRepBuilderAPI_MakeSolid.hxx>
#include <BRepClass3d_SolidClassifier.hxx>

#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepPrimAPI_MakeRevol.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepPrimAPI_MakeTorus.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeCone.hxx>
#include <BRepFilletAPI_MakeFillet.hxx>
#include <BRepFilletAPI_MakeChamfer.hxx>
#include <BRepTools.hxx>
#include <BRepTools_Quilt.hxx>
#include <BRep_Tool.hxx>

#include <GeomAPI_ProjectPointOnCurve.hxx>

#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <Precision.hxx>

//VRV: OCC 4.0 migration
#include <STEPControl_Reader.hxx>
#include <IGESControl_Reader.hxx>
//VRV: OCC 4.0 migration

#include <IFSelect_ReturnStatus.hxx>
#include <TColStd_HSequenceOfTransient.hxx>

//VRV: OCC 4.0 migration
#include <IGESControl_Writer.hxx>
#include <IGESControl_Controller.hxx>
#include <STEPControl_Writer.hxx>
#include <Interface_Static.hxx>
//#include <STEPControlStd_StepModelType.hxx>
//VRV: OCC 4.0 migration

#include <TopoDS_Shape.hxx>
#include <TopAbs.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Solid.hxx>

#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_Array1OfShape.hxx>

#include <IGESData_IGESEntity.hxx>

#include <TDF_Tool.hxx>
#include <TDF_Label.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_Comment.hxx>
#include <TDF_Reference.hxx>
#include <TDF_Data.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_Tool.hxx>
//  #include <TDocStd_Owner.hxx>

#include "SALOMEDS_Tool.hxx"
#include "GEOMDS_Commands.hxx"
#include "GEOMDS_Explorer.hxx"

#include CORBA_SERVER_HEADER(SALOMEDS_Attributes)

#include "Utils_ExceptHandlers.hxx"

Standard_EXPORT static Standard_Boolean IsValid(const TopoDS_Shape& S) {
#if OCC_VERSION_MAJOR >= 5
  return BRepAlgo::IsValid(S);
#else
  return BRepAlgoAPI::IsValid(S);
#endif
}

//============================================================================
// function : GEOM_Gen_i()
// purpose  : constructor to be called for servant creation. 
//============================================================================
GEOM_Gen_i::GEOM_Gen_i(CORBA::ORB_ptr orb,
		       PortableServer::POA_ptr poa,
		       PortableServer::ObjectId * contId, 
		       const char *instanceName, 
		       const char *interfaceName) :
  Engines_Component_i(orb, poa, contId, instanceName, interfaceName)
{
  _thisObj = this ;
  _id = _poa->activate_object(_thisObj);
  // SCRUTE(this)
  name_service = new SALOME_NamingService(_orb);
  myOCAFApp    = new GEOMDS_Application();
  myStudyID    = -1;
  GetCurrentStudy(0);//for correct work of SuperVisor
}



//============================================================================
// function : ~GEOM_Gen_i()
// purpose  : destructor
//============================================================================
GEOM_Gen_i::~GEOM_Gen_i() {
  delete name_service;
}


//============================================================================
// function : IORToLocalPersistentID()
// purpose  :
//============================================================================
char* GEOM_Gen_i::IORToLocalPersistentID(SALOMEDS::SObject_ptr theSObject,
					 const char* IORString,
					 CORBA::Boolean isMultiFile,
					 CORBA::Boolean isASCII)
{
  GEOM::GEOM_Shape_var aShape = GEOM::GEOM_Shape::_narrow(_orb->string_to_object(IORString));
  if (!CORBA::is_nil(aShape)) {
    return CORBA::string_dup(aShape->ShapeId());
  }
  return 0;
}


//============================================================================
// function : LocalPersistentIDToIOR()
// purpose  : Create/Load CORBA object from a persistent ref (an entry)
//          : Used when a study is loaded
//          : The IOR (IORName) of object created is returned
//============================================================================
char* GEOM_Gen_i::LocalPersistentIDToIOR(SALOMEDS::SObject_ptr theSObject,
					 const char* aLocalPersistentID,
					 CORBA::Boolean isMultiFile,
					 CORBA::Boolean isASCII) 
{ 
  SALOMEDS::Study_var myStudy = theSObject->GetStudy();
  GetCurrentStudy(myStudy->StudyId());
  Handle(TDocStd_Document) aDoc = Handle(TDocStd_Document)::DownCast(myStudyIDToDoc(myStudy->StudyId()));
  CORBA::String_var aPersRefString = aLocalPersistentID;

  /* For a GEOM::GEOM_Shape the pers_ref is the Entry in the OCAF document */
  TCollection_ExtendedString MainIOR;
  TDF_Label Lab;
  TDF_Tool::Label(aDoc->GetData(), aPersRefString, Lab );

  if (Lab.IsNull()) {
    MESSAGE("Can not find label "<<aPersRefString<<" for study "<<myStudy->StudyId());
    THROW_SALOME_CORBA_EXCEPTION("Incorrect GEOM data loaded",SALOME::BAD_PARAM);
    //return ""; empty IORs raise new problem: "display" in popup crashes
  }
  
  Handle(TNaming_NamedShape) NS;
  Lab.FindAttribute( TNaming_NamedShape::GetID(), NS );
  TopoDS_Shape S = TNaming_Tool::GetShape(NS);

  /* shapetype, index=0, topo, orb, shapetype, ismain=true and name are setted and modified later ? */
  GEOM::GEOM_Shape_var result = CreateObject(S);
  GEOMDS_Commands GC( aDoc->Main() ) ;
  
  if ( GC.HasIOR(Lab) ) { /* shape already created/loaded */
    return 0 ;
  }

  /******************* Dependent object (not a main shape) *********************/
  if( GC.IsDependentShape(Lab) ) {
    
    TDF_Label mainLabel ;
    Standard_Boolean mainShapeOk = GC.GetMainShapeLabel(Lab, mainLabel) ;
    
    /* Main object not yet loaded we load/create it */
    if( !GC.HasIOR(mainLabel) ) {
      
      TCollection_AsciiString entry;
      TDF_Tool::Entry(mainLabel,entry);
      CORBA::String_var ent = CORBA::string_dup(entry.ToCString());
      
      /* Create the main object recursively */
      MainIOR = LocalPersistentIDToIOR(theSObject, ent, isMultiFile, isASCII) ;
    } else {
      GC.ReturnNameIOR( mainLabel, MainIOR ); 
    }
    
    result->MainName( TCollection_AsciiString(MainIOR).ToCString() ) ;      
    result->IsMainShape(false) ;
    result->ShapeId(aPersRefString);
    
    Handle(TDF_Reference) aRef;
    Lab.FindAttribute( TDF_Reference::GetID(), aRef );
    TDF_Label myL = aRef->Get() ;
    Handle(TNaming_NamedShape) NN;
    myL.FindAttribute( TNaming_NamedShape::GetID(), NN );
    TopoDS_Shape mainTopo = TNaming_Tool::GetShape(NN);

    GEOM::GEOM_Shape::ListOfSubShapeID_var ListOfID = new GEOM::GEOM_Shape::ListOfSubShapeID;
    
    if(S.ShapeType() != TopAbs_COMPOUND) {
      /* to set the index of a unique sub shape (Explode All ONLY for the moment !) */
      ListOfID->length(1);
      int index = 1;
      TopTools_MapOfShape M;
      TopExp_Explorer Exp ;
      for( Exp.Init(mainTopo, TopAbs_ShapeEnum( result->ShapeType() )) ; Exp.More(); Exp.Next() )  {
	if ( M.Add(Exp.Current()) ) {
	  if(Exp.Current().IsSame(S) ) {
	    ListOfID[0] = index;
	    break;
	  }
	  index++ ;
	}	
      }
      result->Index(ListOfID) ;
      return result->Name(); 
    }
    else {
      /* Here is a TopAbs_COMPOUND : we set the list/index for a compound : containing two or more sub shapes  */
      /* Warning : the Corba shape has a shapetype Compound : in GEOMDS_Client we have to retrieve the kind of */
      /* subshapes contained in this compound !                                                                */
      TopTools_SequenceOfShape SS;
      TopoDS_Iterator it ;
      TopExp_Explorer exp ;
      TopAbs_ShapeEnum subType ;
      
      /* Set all sub shapes in a sequence of shapes  */
      for ( it.Initialize( S, true, true ) ; it.More(); it.Next() ) {
	subType = it.Value().ShapeType() ;
	SS.Append( it.Value() ) ;
      }
      
      ListOfID->length( SS.Length() ) ;
      int j, k ;  /* in TopTools_SequenceOfShape index start at 1 */
      
      for( k=1; k<=SS.Length(); k++ ) {
	j = 1 ;
	for( exp.Init( mainTopo, subType ); exp.More(); exp.Next() ) {	
	  if( exp.Current().IsSame( SS.Value(k) ) ) {
	    ListOfID[k-1] = j ;
	  }
	  j++ ;
	}
      }
      result->Index(ListOfID) ;
      return result->Name();
    }
    
  }
  /******************* Independent object (not a sub shape) *********************/
  else {
    result->IsMainShape(true) ;
    if( !GC.AddIORNameAttribute(Lab, result->Name() ) )  {
      MESSAGE("in LocalPersistentIDToIOR, NAME/IOR attribute already exist." << endl ) ;
    }
    Handle(TNaming_NamedShape) NamedShape ;  
    bool notTested = Lab.FindAttribute(TNaming_NamedShape::GetID(), NamedShape) ;
    result->ShapeId(aPersRefString);
    return result->Name(); 
  }
}

//============================================================================
// function : CanPublishInStudy
// purpose  : 
//============================================================================
bool GEOM_Gen_i::CanPublishInStudy(CORBA::Object_ptr theIOR)
{
  GEOM::GEOM_Shape_var aShape = GEOM::GEOM_Shape::_narrow(theIOR);
  return !(aShape->_is_nil());
}


//============================================================================
// function : PublishInStudy
// purpose  : 
//============================================================================
SALOMEDS::SObject_ptr GEOM_Gen_i::PublishInStudy(SALOMEDS::Study_ptr theStudy,
						 SALOMEDS::SObject_ptr theSObject,
						 CORBA::Object_ptr theObject,
						 const char* theName) throw (SALOME::SALOME_Exception)
{
  Unexpect aCatch(SALOME_SalomeException);
  SALOMEDS::SObject_var aResultSO;
  if(CORBA::is_nil(theObject)) return aResultSO;

  GEOM::GEOM_Shape_var aShape = GEOM::GEOM_Shape::_narrow(theObject);
  if(aShape->_is_nil()) return aResultSO;

  if(theStudy->_is_nil()) return aResultSO;

  SALOMEDS::GenericAttribute_var anAttr;
  SALOMEDS::StudyBuilder_var     aStudyBuilder = theStudy->NewBuilder(); 

  SALOMEDS::SComponent_var       aFather = theStudy->FindComponent("GEOM"); 
  if (aFather->_is_nil()) {
    aFather = aStudyBuilder->NewComponent("GEOM");
    anAttr = aStudyBuilder->FindOrCreateAttribute(aFather, "AttributeName");
    SALOMEDS::AttributeName_var aName = SALOMEDS::AttributeName::_narrow(anAttr);
    aName->SetValue("Geometry");
    anAttr = aStudyBuilder->FindOrCreateAttribute(aFather, "AttributePixMap");
    SALOMEDS::AttributePixMap::_narrow(anAttr)->SetPixMap("ICON_OBJBROWSER_Geometry");
    aStudyBuilder->DefineComponentInstance(aFather, GEOM_Gen::_this());
  }
  if (aFather->_is_nil()) return aResultSO;
  
  if (CORBA::is_nil(theSObject)) {
    aResultSO = aStudyBuilder->NewObject(aFather);
  } else {
    if (!theSObject->ReferencedObject(aResultSO)) 
      THROW_SALOME_CORBA_EXCEPTION("Publish in study supervision graph error",SALOME::BAD_PARAM);
  }
  anAttr = aStudyBuilder->FindOrCreateAttribute(aResultSO, "AttributeIOR");
  SALOMEDS::AttributeIOR_var anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
  anIOR->SetValue(aShape->Name());

  anAttr = aStudyBuilder->FindOrCreateAttribute(aResultSO, "AttributePixMap");
  SALOMEDS::AttributePixMap_var aPixmap = SALOMEDS::AttributePixMap::_narrow(anAttr);
  TCollection_AsciiString aShapeName("Shape_");  

  if ( aShape->ShapeType() == GEOM::COMPOUND ) {
    aPixmap->SetPixMap( "ICON_OBJBROWSER_COMPOUND" );
    aShapeName = "Compound_";
  } else if ( aShape->ShapeType() == GEOM::COMPSOLID ) {
    aPixmap->SetPixMap( "ICON_OBJBROWSER_COMPSOLID" );
    aShapeName = "Compsolid_";
  } else if ( aShape->ShapeType() == GEOM::SOLID ) {
    aPixmap->SetPixMap( "ICON_OBJBROWSER_SOLID" );
    aShapeName = "Solid_";
  } else if ( aShape->ShapeType() == GEOM::SHELL ) {
    aPixmap->SetPixMap( "ICON_OBJBROWSER_SHELL" );
    aShapeName = "Shell_";
  } else if ( aShape->ShapeType() == GEOM::FACE ) {
    aPixmap->SetPixMap( "ICON_OBJBROWSER_FACE" );
    aShapeName = "Face_";
  } else if ( aShape->ShapeType() == GEOM::WIRE ) {
    aPixmap->SetPixMap( "ICON_OBJBROWSER_WIRE" );
    aShapeName = "Wire_";
  } else if ( aShape->ShapeType() == GEOM::EDGE ) {
    aPixmap->SetPixMap( "ICON_OBJBROWSER_EDGE" );
    aShapeName = "Edge_";
  } else if ( aShape->ShapeType() == GEOM::VERTEX ) {
    aPixmap->SetPixMap( "ICON_OBJBROWSER_VERTEX" );
    aShapeName = "Vertex_";
  }                                          
  if (strlen(theName) == 0) aShapeName += TCollection_AsciiString(aResultSO->Tag());
  else aShapeName = TCollection_AsciiString((char*)theName);

  //Set a name of the added shape
  anAttr = aStudyBuilder->FindOrCreateAttribute(aResultSO, "AttributeName");
  SALOMEDS::AttributeName_var aNameAttrib = SALOMEDS::AttributeName::_narrow(anAttr);
  aNameAttrib->SetValue(aShapeName.ToCString());

  //Add a reference to published object
//    aStudyBuilder->Addreference(theObject, aResultSO);
  return aResultSO._retn();
}

 
//============================================================================
// function : Save()
// purpose  : save OCAF/Geom document
//============================================================================
SALOMEDS::TMPFile* GEOM_Gen_i::Save(SALOMEDS::SComponent_ptr theComponent,
				    const char* theURL,
				    bool isMultiFile) {
  SALOMEDS::TMPFile_var aStreamFile;
  // Get a temporary directory to store a file
  TCollection_AsciiString aTmpDir = (isMultiFile)?TCollection_AsciiString((char*)theURL):SALOMEDS_Tool::GetTmpDir();
  // Create a list to store names of created files
  SALOMEDS::ListOfFileNames_var aSeq = new SALOMEDS::ListOfFileNames;
  aSeq->length(1);
  // Prepare a file name to open
  TCollection_AsciiString aNameWithExt("");
  if (isMultiFile)
    aNameWithExt = TCollection_AsciiString(SALOMEDS_Tool::GetNameFromPath(theComponent->GetStudy()->URL()));
  aNameWithExt += TCollection_AsciiString("_GEOM.sgd");
  aSeq[0] = CORBA::string_dup(aNameWithExt.ToCString());
  // Build a full file name of temporary file
  TCollection_AsciiString aFullName = aTmpDir + aNameWithExt;
  // Save GEOM component in this file
  myOCAFApp->SaveAs(myCurrentOCAFDoc, aFullName);
  // Conver a file to the byte stream
  aStreamFile = SALOMEDS_Tool::PutFilesToStream(aTmpDir.ToCString(), aSeq.in(), isMultiFile);
  // Remove the created file and tmp directory
  if (!isMultiFile) SALOMEDS_Tool::RemoveTemporaryFiles(aTmpDir.ToCString(), aSeq.in(), true);
  // Return the created byte stream
  return aStreamFile._retn();
}

SALOMEDS::TMPFile* GEOM_Gen_i::SaveASCII(SALOMEDS::SComponent_ptr theComponent,
					 const char* theURL,
					 bool isMultiFile) {
  SALOMEDS::TMPFile_var aStreamFile = Save(theComponent, theURL, isMultiFile);
  return aStreamFile._retn();
}


CORBA::Boolean GEOM_Gen_i::Load(SALOMEDS::SComponent_ptr theComponent,
				const SALOMEDS::TMPFile& theStream,
				const char* theURL,
				bool isMultiFile) {

  if (theStream.length() <= 9) {
    MESSAGE("The TMPFile is too short : " << theStream.length() << " bytes ");
    return false;
  }

  // Get a temporary directory for a file
  TCollection_AsciiString aTmpDir = isMultiFile?TCollection_AsciiString((char*)theURL):SALOMEDS_Tool::GetTmpDir();
  // Conver the byte stream theStream to a file and place it in tmp directory
  SALOMEDS::ListOfFileNames_var aSeq = SALOMEDS_Tool::PutStreamToFiles(theStream,
								       aTmpDir.ToCString(),
								       isMultiFile);

  // Prepare a file name to open
  TCollection_AsciiString aNameWithExt("");
  if (isMultiFile)
    aNameWithExt = TCollection_AsciiString(SALOMEDS_Tool::GetNameFromPath(theComponent->GetStudy()->URL()));
  aNameWithExt += TCollection_AsciiString("_GEOM.sgd");
  TCollection_AsciiString aFullName = aTmpDir + aNameWithExt;

  // Open document
  if (myOCAFApp->Open(aFullName, myCurrentOCAFDoc) != CDF_RS_OK) return false;

  // Remove the created file and tmp directory
  if (!isMultiFile) SALOMEDS_Tool::RemoveTemporaryFiles(aTmpDir.ToCString(), aSeq.in(), true);

  SALOMEDS::Study_var Study = theComponent->GetStudy();
  TCollection_AsciiString name( Study->Name() );

  int StudyID = Study->StudyId();
  myStudyIDToDoc.Bind( StudyID, myCurrentOCAFDoc );  
  myStudyID = StudyID;
    
  /* We clear all IOR (nameIOR) attributes of all objects before reconstruction */
  /* This information will be setted when each object is reconstructed          */
  GEOMDS_Commands GC( myCurrentOCAFDoc->Main() ) ;
  GC.ClearAllIOR(myCurrentOCAFDoc->Main());

  return true;
}

CORBA::Boolean GEOM_Gen_i::LoadASCII(SALOMEDS::SComponent_ptr theComponent,
				     const SALOMEDS::TMPFile& theStream,
				     const char* theURL,
				     bool isMultiFile) {
  return Load(theComponent, theStream, theURL, isMultiFile);
}

//  //============================================================================
//  // function : Save()
//  // purpose  : save OCAF/Geom document
//  //============================================================================
//  void GEOM_Gen_i::Save(const char *IORSComponent, const char *aUrlOfFile) 
//  {

//    TCollection_ExtendedString path((char*)aUrlOfFile);
//    TCollection_ExtendedString pathWithExt = path + TCollection_ExtendedString(".sgd");
//    myOCAFApp->SaveAs(myCurrentOCAFDoc,pathWithExt);
//  }


//  //============================================================================
//  // function : Load()
//  // purpose  : Load OCAF/Geom document
//  //============================================================================
//  void GEOM_Gen_i::Load(const char *IORSComponent, const char *aUrlOfFile) 
//  {

//    TCollection_ExtendedString path((char*)aUrlOfFile);
//    TCollection_ExtendedString pathWithExt = path + TCollection_ExtendedString(".sgd");

//    myOCAFApp->Open(pathWithExt,myCurrentOCAFDoc);

//    SALOMEDS::SComponent_var SC = SALOMEDS::SComponent::_narrow(_orb->string_to_object(IORSComponent));
//    SALOMEDS::Study_var Study = SC->GetStudy();
//    TCollection_AsciiString name( Study->Name() );

//    int StudyID = Study->StudyId();
//    myStudyIDToDoc.Bind( StudyID, myCurrentOCAFDoc );  
//    myStudyID = StudyID;

//    /* We clear all IOR (nameIOR) attributes of all objects before reconstruction */
//    /* This information will be setted when each object is reconstructed          */
//    GEOMDS_Commands GC( myCurrentOCAFDoc->Main() ) ;
//    GC.ClearAllIOR(myCurrentOCAFDoc->Main());

//    return ;
//  }


//============================================================================
// function : Close()
// purpose  :
//============================================================================
void GEOM_Gen_i::Close(SALOMEDS::SComponent_ptr theComponent)
{
  int anID = theComponent->GetStudy()->StudyId();
  if (anID == myStudyID) GetCurrentStudy(0); // set default value of current study ID, if current is deleted
  if (myStudyIDToDoc.IsBound(anID)) {
    // close document in the application
//      Handle(TDocStd_Owner) anOwner;
    Handle(TDocStd_Document) aDoc = Handle(TDocStd_Document)::DownCast(myStudyIDToDoc.Find(anID));
//      Handle(TDocStd_Document) anEmptyDoc;
//      if (aDoc->Main().Root().FindAttribute(TDocStd_Owner::GetID(), anOwner)) {
//        anOwner->SetDocument(anEmptyDoc);
//        cout<<"********** Nullify owner of document"<<endl;
//      }
    myOCAFApp->Close(aDoc);
    myStudyIDToDoc.UnBind(anID); // remove document from GEOM documents data map
    }
}

//============================================================================
// function : CanCopy()
// purpose  :
//============================================================================
CORBA::Boolean GEOM_Gen_i::CanCopy(SALOMEDS::SObject_ptr theObject) {
  // Try to retrieve known by Geometry component GEOM_shape by given IOR
  SALOMEDS::GenericAttribute_var anAttr;
  if (!theObject->FindAttribute(anAttr, "AttributeIOR")) return false;
  GEOM::GEOM_Shape_var aShape = GetIORFromString(SALOMEDS::AttributeIOR::_narrow(anAttr)->Value());
  // If the object is null one it can't be copied: return false
  if (aShape->_is_nil()) return false;
  return true;
}

//============================================================================
// function : CopyFrom()
// purpose  :
//============================================================================
SALOMEDS::TMPFile* GEOM_Gen_i::CopyFrom(SALOMEDS::SObject_ptr theObject, CORBA::Long& theObjectID) {
  // Declare a sequence of the byte to store the copied object
  SALOMEDS::TMPFile_var aStreamFile = new SALOMEDS::TMPFile;

  // Try to get GEOM_Shape object by given SObject
  SALOMEDS::GenericAttribute_var anAttr;
  if (!theObject->FindAttribute(anAttr, "AttributeIOR")) return false;
  GEOM::GEOM_Shape_var aShape = GetIORFromString(SALOMEDS::AttributeIOR::_narrow(anAttr)->Value());
  // If the object is null one it can't be copied: return false
  if (aShape->_is_nil()) return aStreamFile._retn();
 
  GetCurrentStudy(theObject->GetStudy()->StudyId());

  // Convert a TopoDS_Shape to a stream of bytes
  TopoDS_Shape aTopology = GetTopoShape(aShape);
  if (aTopology.IsNull()) return aStreamFile._retn();
  ostrstream aStreamedShape;
  BRepTools::Write(aTopology, aStreamedShape);
  int aSize = aStreamedShape.pcount();
  char* aBuffer = new char[aSize];
  memcpy(aBuffer, aStreamedShape.str(), aSize);
  aStreamedShape.rdbuf()->freeze(0);

  aStreamFile = new SALOMEDS::TMPFile(aSize, aSize, (CORBA::Octet*)aBuffer, 1);
  
  // Assign an ID = 1 the the type GEOM_Shape
  theObjectID = 1;

  // Return created TMPFile
  return aStreamFile._retn();
}

//============================================================================
// function : CanPaste()
// purpose  :
//============================================================================
CORBA::Boolean GEOM_Gen_i::CanPaste(const char* theComponentName, CORBA::Long theObjectID) {
  // The Geometry component can paste only objects copied by Geometry component
  // and with the object type = 1
//    cout<<"********** GEOM_Gen_i::CanPaste ("<<theComponentName<<","<<theObjectID<<")"<<endl;
  if (strcmp(theComponentName, ComponentDataType()) != 0 || theObjectID != 1) return false;
  return true;
}

//============================================================================
// function : PasteInto()
// purpose  :
//============================================================================
SALOMEDS::SObject_ptr GEOM_Gen_i::PasteInto(const SALOMEDS::TMPFile& theStream,
					    CORBA::Long theObjectID,
					    SALOMEDS::SObject_ptr theObject) {
  // Find the current Study and StudyBuilder
  SALOMEDS::Study_var aStudy = theObject->GetStudy();
  SALOMEDS::StudyBuilder_var aStudyBuilder = aStudy->NewBuilder();

  // Retrieve a TopoDS_Shape from byte stream
  TopoDS_Shape aTopology;
  istrstream aStreamedBrep((char*) &theStream[0], theStream.length());
  BRep_Builder aBuilder;
  try {
    BRepTools::Read(aTopology, aStreamedBrep, aBuilder);
  } catch (Standard_Failure) {
//      cout<<"GEOM_Gen_i::PasteInto exception"<<endl;
    return false;
  }
  
  // Create new object in Geometry component using retrieved topology
  GEOM::GEOM_Shape_var aShape = CreateObject(aTopology);
  GetCurrentStudy(aStudy->StudyId());
  const char *anEntry = InsertInLabel(aTopology, aShape->Name(), myCurrentOCAFDoc) ;
  aShape->ShapeId(anEntry) ;

  // SObject of the created shape is theObject or new Child of Component if theObject == geom component
  SALOMEDS::SObject_var aNewSO;
  if (strcmp(theObject->GetFatherComponent()->GetID(),theObject->GetID()) == 0) {
    aNewSO = aStudyBuilder->NewObject(theObject);
  } else aNewSO = SALOMEDS::SObject::_duplicate(theObject);
  // Add IORAttribute to the Study and set IOR of the created GEOM_Shape to it
  SALOMEDS::GenericAttribute_var anAttr = aStudyBuilder->FindOrCreateAttribute(aNewSO, "AttributeIOR");
  SALOMEDS::AttributeIOR_var anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
  anIOR->SetValue(aShape->Name());

  // Return the created in the Study SObject
  return aNewSO._retn();
}

//============================================================================
// function : ComponentDataType()
// purpose  :
//============================================================================
char* GEOM_Gen_i::ComponentDataType()
{
  return CORBA::string_dup("GEOM");
}

//============================================================================
// function : register() 
// purpose  : register 'name' in 'name_service'
//============================================================================
void GEOM_Gen_i::register_name(char * name)
{
  GEOM::GEOM_Gen_ptr g = GEOM::GEOM_Gen::_narrow(POA_GEOM::GEOM_Gen::_this());
  name_service->Register(g, name); 
}



//================================================================================
// function : SequenceOfShapeFromListOfGeomShape()
// purpose  : Define a sequence of shapes from 'listShapes' and return its length.
//          : No control is made on shapes !
//================================================================================
int GEOM_Gen_i::SequenceOfShapeFromListOfGeomShape( const GEOM::GEOM_Gen::ListOfGeomShapes& listShapes,
						    TopTools_SequenceOfShape& SS )
{
  int nbShapes = listShapes.length() ;
  if( nbShapes < 1)
    return 0 ;
  
  for(int i=0; i<nbShapes; i++) {
    GEOM::GEOM_Shape_var aGeomShape = listShapes[i] ;
    TopoDS_Shape aShape = GetTopoShape(aGeomShape) ;
    SS.Append(aShape) ;
  }
  return nbShapes ;
}




//=================================================================================
// function : GetTopoShape()
// purpose  : Returns a TopoDS_Shape from a GEOM::GEOM_Shape_ptr in 'myCurrentOCAFDoc'
//          : A null shape is returned if not possible
//=================================================================================
TopoDS_Shape GEOM_Gen_i::GetTopoShape(GEOM::GEOM_Shape_ptr shape_ptr)
{ 
  TopoDS_Shape tds ;

  TDF_Label lab ;
  Handle(TDF_Data) D = myCurrentOCAFDoc->GetData() ;
  TDF_Tool::Label( D, shape_ptr->ShapeId(), lab, true ) ;
  Handle(TNaming_NamedShape) NamedShape ;  
  bool res = lab.FindAttribute(TNaming_NamedShape::GetID(), NamedShape) ;

  if( !res ) {
    return tds ; /* a null shape is returned */
  }
  else {
    return TNaming_Tool::GetShape(NamedShape) ;
  }
}



//=================================================================================
// function : GetStringFromIOR()
// purpose  : returns a string that represents  a 'GEOM::GEOM_Shape_var'
//=================================================================================
const char* GEOM_Gen_i::GetStringFromIOR(GEOM::GEOM_Shape_var shapeIOR) {
  const char * ret = _orb->object_to_string(shapeIOR) ;
  return ret ;
}



//=================================================================================
// function : GetIORFromString()
// purpose  : returns a 'GEOM::GEOM_Shape_var' from a string representing it
//=================================================================================
GEOM::GEOM_Shape_ptr GEOM_Gen_i::GetIORFromString(const char* stringIOR) {
  GEOM::GEOM_Shape_var shapeIOR;
  if(strcmp(stringIOR,"") != 0){
    CORBA::Object_var anObject = _orb->string_to_object(stringIOR);
    if(!CORBA::is_nil(anObject))
      shapeIOR =  GEOM::GEOM_Shape::_narrow(anObject.in()) ;
  }
  return shapeIOR._retn() ;
}



//==================================================================================
// function : InsertInLabel()
// purpose  : Insert S = Shape and mystr = name in a new Label of Geom/OCAF document
//          : and returns the corresponding OCAF entry
//==================================================================================
const char * GEOM_Gen_i::InsertInLabel(TopoDS_Shape S, const char *mystr, Handle(TDocStd_Document) OCAFDoc)
{
  GEOMDS_Commands GC(OCAFDoc->Main());
  /* add attributs S and mystr in a new label */
  TDF_Label Lab = GC.AddShape (S, (char*)mystr);

  TCollection_AsciiString entry;
  TDF_Tool::Entry(Lab,entry);
  const char *ent = entry.ToCString() ;
  return ent ;
}


//==================================================================================
// function : InsertInLabelDependentShape()
// purpose  : Insert S = Shape and its nameIor in a new Label of Geom/OCAF document
//          : insert also a reference attribute (a label) to the main shape 'mainshap_ptr'.
//          : and returns the corresponding OCAF entry of the new label.
//==================================================================================
const char * GEOM_Gen_i::InsertInLabelDependentShape( TopoDS_Shape S,
						      const char *nameIor,
						      GEOM::GEOM_Shape_ptr mainshape_ptr,
						      Handle(TDocStd_Document) OCAFDoc )
{
  GEOMDS_Commands GC(OCAFDoc->Main());
  /* add attributs S and nameIor in a new label */

  /* retrieve the label of the main shape in the document */
  TDF_Label mainRefLab;
  TDF_Tool::Label(OCAFDoc->GetData(), mainshape_ptr->ShapeId(), mainRefLab);

  /* add attributs : S, nameIor and ref to main */
  TDF_Label Lab = GC.AddDependentShape(S, (char*)nameIor, mainRefLab);

  TCollection_AsciiString entry;
  TDF_Tool::Entry(Lab, entry);
  const char *ent = entry.ToCString() ;
  return ent ;
}


//=================================================================================
// function : InsertInLabelOneArgument()
// purpose  :
//=================================================================================
void GEOM_Gen_i::InsertInLabelOneArgument(TopoDS_Shape main_topo,
					  GEOM::GEOM_Shape_ptr shape_ptr,
					  TopoDS_Shape result_topo,	
					  GEOM::GEOM_Shape_ptr result,	
					  Handle(TDocStd_Document) OCAFDoc)
{
  /* Put shape and name into geom/OCAF doc */
  GEOMDS_Commands GC(OCAFDoc->Main());
  /* Add attributs 'shape' and 'name_ior' in a new label */
  TDF_Label Lab = GC.Generated( main_topo, result_topo, result->Name() );
  TCollection_AsciiString entry;
  TDF_Tool::Entry(Lab, entry);
  result->ShapeId( entry.ToCString() ) ;
  
  /* Create a new label */
  TDF_Label NewLab = Lab.NewChild();
  TCollection_ExtendedString Value("Arguments");
  TDataStd_Name::Set(NewLab,Value);
  
  TDF_Label NewLab1 = NewLab.NewChild();
  TDF_Label RefLab;
  TDF_Tool::Label(OCAFDoc->GetData(), shape_ptr->ShapeId(), RefLab);
  TDF_Reference::Set(NewLab1, RefLab);
}


//=================================================================================
// function : InsertInLabelMoreArguments()
// purpose  :
//=================================================================================
void GEOM_Gen_i::InsertInLabelMoreArguments(TopoDS_Shape main_topo,
					    GEOM::GEOM_Shape_ptr result,
					    const GEOM::GEOM_Gen::ListOfIOR& ListShapes,				
					    Handle(TDocStd_Document) OCAFDoc)
{
  /* Put shape and name into geom/OCAF doc */
  GEOMDS_Commands GC(OCAFDoc->Main());
  /* Add attributs TopoDS and name_ior in a new label */
  TDF_Label Lab = GC.AddShape(main_topo, result->Name() );
  TCollection_AsciiString entry;
  TDF_Tool::Entry(Lab, entry);
  
  /* Create a new label */
  TDF_Label NewLab = Lab.NewChild();
  TCollection_ExtendedString Value("Arguments");
  TDataStd_Name::Set(NewLab, Value);

  for (unsigned int ind = 0; ind < ListShapes.length(); ind++) {
    
    TDF_Label NewLab1 = NewLab.NewChild();    
    GEOM::GEOM_Shape_var aShape = GetIORFromString( ListShapes[ind] );
    
    TDF_Label RefLab;
    TDF_Tool::Label(OCAFDoc->GetData(), aShape->ShapeId(), RefLab);
    TDF_Reference::Set(NewLab1, RefLab);
  }
  result->ShapeId(entry.ToCString());
}



//=================================================================================
// function: NbLabels()
// purpose : 
//=================================================================================
CORBA::Short GEOM_Gen_i::NbLabels()
{
  TDF_ChildIterator ChildIterator(myCurrentOCAFDoc->Main());
  unsigned int i = 1;
  
  while (ChildIterator.More()) {
    i++;
    ChildIterator.Next();
  }
  return i;
  //  return TDF_Tool::NbLabels( myCurrentOCAFDoc->Main() );
}



//=================================================================================
// function: GetCurrentStudy()
// purpose : Finds or creates the geom/OCAF document corresponding to the index
// 'StudyID'
//=================================================================================
void GEOM_Gen_i::GetCurrentStudy(CORBA::Long StudyID)
{
  /* If StudyID is known we link myCurrentOCAFDoc to it */
  if (myStudyIDToDoc.IsBound(StudyID)) {
    myCurrentOCAFDoc =  Handle(TDocStd_Document)::DownCast(myStudyIDToDoc(StudyID));
  }
  /* Create a new OCAFDoc and link it to 'StudyID' argument */
  else { 
    myOCAFApp->NewDocument("SALOME_GEOM",myCurrentOCAFDoc);
    myStudyIDToDoc.Bind(StudyID,myCurrentOCAFDoc);
  } 
  myStudyID = StudyID;
}


//================================================================================
// function : CreateObject() 
// purpose  : private function to create a complete CORBA object and return it
//================================================================================
GEOM::GEOM_Shape_ptr GEOM_Gen_i::CreateObject(TopoDS_Shape& tds)
{
//   if ( tds.ShapeType() == TopAbs_COMPOUND ) {
//     TopoDS_Iterator itr(tds);
//     TopoDS_Shape res;
//     int i = 0;
//     while (itr.More()) {
//       i++;
//       res = itr.Value();
//       itr.Next();
//     }
    
//     if ( i == 1 )
//       tds = res;
//   }

  GEOM::shape_type st = GEOM::shape_type(tds.ShapeType()) ; /* casting */
  
  /* Create the CORBA servant holding the TopoDS_Shape */
  GEOM::GEOM_Gen_ptr engine = POA_GEOM::GEOM_Gen::_this();
  GEOM::GEOM_Shape::ListOfSubShapeID_var index = new GEOM::GEOM_Shape::ListOfSubShapeID;
  index->length(0);  
  GEOM_Shape_i * shape_servant = new GEOM_Shape_i(tds, _orb, engine, index, st, true);
  GEOM::GEOM_Shape_var shape = GEOM::GEOM_Shape::_narrow(shape_servant->_this()); 
  
  /* Create and set the name (IOR of shape converted into a string) */
  string name_ior = _orb->object_to_string(shape) ;
  shape->Name( name_ior.c_str() );  
  shape->NameType( "" );
  return shape;
}

//=======================================================================
//function : CreateSubObject
//purpose  : 
//=======================================================================

GEOM::GEOM_Shape_ptr GEOM_Gen_i::CreateSubObject(const TopoDS_Shape& SubShape,
						 const GEOM::GEOM_Shape_ptr MainShape,
						 const GEOM::GEOM_Shape::ListOfSubShapeID& ListOfID)
{
  GEOM::shape_type st = GEOM::shape_type(SubShape.ShapeType()) ; /* casting */
  
  /* Create the CORBA servant holding the TopoDS_Shape */
  GEOM::GEOM_Gen_ptr engine = POA_GEOM::GEOM_Gen::_this();
  GEOM_Shape_i * shape_servant =
    new GEOM_Shape_i(SubShape, _orb, engine, ListOfID, st, false);
  GEOM::GEOM_Shape_var shape = GEOM::GEOM_Shape::_narrow(shape_servant->_this()); 
  
    /* Create and set the name (IOR of shape converted into a string) */
  string name_ior = _orb->object_to_string(shape) ;
  shape->Name( name_ior.c_str() );
    /* create and set the mainname (IOR of shape converted into a string) */
  const char *mainname_ior = _orb->object_to_string(MainShape) ;
  shape->MainName(mainname_ior);
    /* precaution : NameType will be set precisely in GUI */    
  shape->NameType( "" );
    /* add 'SubShape' its 'nameIOR' and a reference to the main shape thanks to method below  */
  const char *entry =
    InsertInLabelDependentShape(SubShape, shape->Name(), MainShape, myCurrentOCAFDoc) ;
  shape->ShapeId( entry ) ;
 
  return shape;
}

//=======================================================================
// function : SuppressFacesGlue()
// purpose  : Define a compound of shells after suppress of mapFaces in the 
//          : shape S and return the number of shells of the compound.
//=======================================================================
int GEOM_Gen_i::SuppressFacesGlue( const TopoDS_Shape& S,
				   const TopTools_MapOfShape& mapFaces,
				   TopoDS_Shape& aCompoundOfShells )
  throw (SALOME::SALOME_Exception)
{  
  Unexpect aCatch(SALOME_SalomeException);
  BRepTools_Quilt Glue;
  aCompoundOfShells.Nullify() ;
  
  for ( TopExp_Explorer exp( S, TopAbs_FACE); exp.More(); exp.Next() ) {
    const TopoDS_Face& F = TopoDS::Face(exp.Current());
    if ( !mapFaces.Contains(F) ) {
      /* this face must not to be suppressed */
      Glue.Add(F);
    }
  }
  
  /* Use specif method to calculate the compound of shells */
  aCompoundOfShells = Glue.Shells();
  
  if( aCompoundOfShells.ShapeType() != TopAbs_COMPOUND ) {
    THROW_SALOME_CORBA_EXCEPTION("In GEOM_Gen_i::SuppressFacesGlue() : not a GEOM::COMPOUND", SALOME::BAD_PARAM);
  }

  /* explore Compound for verification and return the number of shells */
  int numberOfShell = 0 ;
  for ( TopExp_Explorer exp1( aCompoundOfShells, TopAbs_SHELL); exp1.More(); exp1.Next() )
    numberOfShell++ ;

  return numberOfShell ;
}


//=====================================================================================
// function : GetIndexTopology()
// purpose  : return the index of a sub shape in a shape (index starts at 1)
//          : Return -1 if not found
//=====================================================================================
int GEOM_Gen_i::GetIndexTopology(const TopoDS_Shape& subshape, const TopoDS_Shape& mainShape) 
{ 
  if( mainShape.IsNull() || subshape.IsNull() ) 
    return -1 ; 

  int index = 1; 
  if (subshape.ShapeType() == TopAbs_COMPOUND) 
    { 
      TopoDS_Iterator it; 
      TopTools_ListOfShape CL; 
      CL.Append( mainShape ); 
      TopTools_ListIteratorOfListOfShape itC; 
      for (itC.Initialize( CL ); itC.More(); itC.Next()) 
	{ 
	  for (it.Initialize( itC.Value() );  it.More(); it.Next()) 
	    { 
	      if ( it.Value().ShapeType() == TopAbs_COMPOUND) 
		{
		  if (it.Value().IsSame(subshape)) 
		    return index; 
		  else 
		    index++; 
		  CL.Append( it.Value() ); 
		}
	    } 
	} 
    } 
  else 
    { 
      TopExp_Explorer Exp ( mainShape,  subshape.ShapeType() ); 
      TopTools_MapOfShape M; 
      while ( Exp.More() ) 
	{ 
	  if ( M.Add(Exp.Current()) ) 
	    { 
	      if ( Exp.Current().IsSame(subshape) ) 
		return index; 
	      index++; 
	    } 
	  Exp.Next(); 
	} 
    } 
  return -1; 
} 


//================================================================================
// function : IndexOfFacesOfSubShell()
// purpose  : Return a list of indices corresponding to the faces of a 'subShell'
//          : in the main shape 'S'
//================================================================================
GEOM::GEOM_Shape::ListOfSubShapeID* GEOM_Gen_i::IndexOfFacesOfSubShell( const TopoDS_Shape& S,
								  const TopoDS_Shape subShell )
  throw (SALOME::SALOME_Exception)
{

  Unexpect aCatch(SALOME_SalomeException);
  GEOM::GEOM_Shape::ListOfSubShapeID_var ListOfID = new GEOM::GEOM_Shape::ListOfSubShapeID;
  ListOfID->length(0) ;
  if( subShell.IsNull() || subShell.ShapeType() != TopAbs_SHELL ) {
    THROW_SALOME_CORBA_EXCEPTION("In GEOM_Gen_i::IndexOfFacesOfSubShell() : null shape or not a GEOM::SHELL", SALOME::BAD_PARAM);
  }

  /* put faces of subShell in a Map of faces */
  int j = 0 ;
  TopTools_MapOfShape mapFaces ;
  for( TopExp_Explorer Exp1( subShell, TopAbs_FACE );  Exp1.More(); Exp1.Next() ) {
    mapFaces.Add(Exp1.Current() ) ;
    j++ ;
  }

  if( j<1 )
    THROW_SALOME_CORBA_EXCEPTION("In GEOM_Gen_i::IndexOfFacesOfSubShell() : no faces in sub shell", SALOME::BAD_PARAM);
  
  /* Find index of each face of subshell in the main topology and put its index in ListOfID */
  int size = 0 ;
  for ( TopExp_Explorer Exp2(S, TopAbs_FACE); Exp2.More();  Exp2.Next() ) {
    
    const TopoDS_Face& F = TopoDS::Face( Exp2.Current() ) ;

    if( mapFaces.Contains(F) )  {
      int n = GetIndexTopology( F, S ) ;
      if( n<=0 ) {
	THROW_SALOME_CORBA_EXCEPTION("In GEOM_Gen_i::IndexOfFacesOfSubShell() : no index found", SALOME::BAD_PARAM);	
      }
      size++;
      ListOfID->length(size) ;
      ListOfID[size-1] = n ;
    }
  }

  return ListOfID._retn() ;
}



//================================================================================
// function : ListOfIDIntoMapOfShapes()
// purpose  : Define a MapOfShapes from a main topology 'S' a 'subShapeType'
//          : and a list of indices 'L'.
//          : Return true if 'aMap' is not empty
//================================================================================
bool GEOM_Gen_i::ListOfIDIntoMapOfShapes( const TopoDS_Shape& S,
					  const GEOM::GEOM_Shape::ListOfSubShapeID& L,
					  const int subShapeType,
					  TopTools_MapOfShape& aMap )
{
  if( L.length() < 1 || S.IsNull() ) {
    return false ;
  }
  
  aMap.Clear() ; 
  for( int k=0; k<L.length(); k++ ) {
    /* indices start at 1 in list L */
    int j = 1 ;
    TopExp_Explorer exp ;
    TopTools_MapOfShape M; 
    for(  exp.Init( S, TopAbs_ShapeEnum(subShapeType) ); exp.More(); exp.Next() ) {
      if ( M.Add(exp.Current()) ) 
	{ 
	  if( L[k] == j ) {
	    aMap.Add( exp.Current() ) ;
	  }
	  j++ ;
	}
    }
  }
  return true ;
}



//================================================================================
// function : ListOfIDIntoSequenceOfShapes()
// purpose  : Define 'aSequenceOfShapes' from a main topology 'S' a 'subShapeType'
//          : and a list of indices 'L'.
//          : Return true if 'aSequenceOfShapes' is not empty
//================================================================================
bool GEOM_Gen_i::ListOfIDIntoSequenceOfShapes( const TopoDS_Shape& S,
					       const GEOM::GEOM_Shape::ListOfSubShapeID& L,
					       const int subShapeType,
					       TopTools_SequenceOfShape& aSequenceOfShapes )
{
  if( L.length() < 1 || S.IsNull() ) {
    return false ;
  }
  
  aSequenceOfShapes.Clear() ; 
  for( int k=0; k<L.length(); k++ ) {
    /* indices start at 1 in list L */
    int j = 1 ;
    TopExp_Explorer exp ;
    for(  exp.Init( S, TopAbs_ShapeEnum(subShapeType) ); exp.More(); exp.Next() ) {
      if( L[k] == j ) {
	aSequenceOfShapes.Append( exp.Current() ) ;
      }
      j++ ;
    }
  }
  return true ;
}



//================================================================================
// function : SuppressFaces()
// purpose  : Suppress faces contained in ListOfID from 'shape'.
//          : Return a list of Geom shapes each one is a main shape GEOM::FACE or GEOM::SHELL
//================================================================================
GEOM::GEOM_Gen::ListOfGeomShapes* GEOM_Gen_i::SuppressFaces( GEOM::GEOM_Shape_ptr shape,
								const GEOM::GEOM_Shape::ListOfSubShapeID& ListOfID ) 
  throw (SALOME::SALOME_Exception)
{
  Unexpect aCatch(SALOME_SalomeException);
  GEOM::GEOM_Gen::ListOfGeomShapes_var listOfGeomShapes = new GEOM::GEOM_Gen::ListOfGeomShapes;
  listOfGeomShapes->length(0) ;

  TopoDS_Shape mainShape = GetTopoShape(shape);
  if( mainShape.IsNull() )
    THROW_SALOME_CORBA_EXCEPTION("In GEOM_Gen_i::SuppressFaces() : null argument shape", SALOME::BAD_PARAM);
  
  if( ListOfID.length() < 1 )
    THROW_SALOME_CORBA_EXCEPTION("In GEOM_Gen_i::SuppressFaces() : empty ListOfID", SALOME::BAD_PARAM);
  
  /* Define 'mapFaces' a map of faces to be suppressed in mainShape */
  TopTools_MapOfShape mapFaces ;
  if( !ListOfIDIntoMapOfShapes(mainShape, ListOfID, TopAbs_FACE, mapFaces ) ) {
    return listOfGeomShapes._retn();
  }
    
  /* Call algorithm to calculate a compound of shells resulting of face suppression */
  int numberOfShells = 0 ;
  TopoDS_Shape aCompoundOfShells ;
  numberOfShells = SuppressFacesGlue(mainShape, mapFaces, aCompoundOfShells) ;
  if(numberOfShells < 1) {
    THROW_SALOME_CORBA_EXCEPTION("In GEOM_Gen_i::Suppressfaces() : no shells", SALOME::BAD_PARAM);
  }
 
  /* Create a shell for each shell contained in 'aCompoundOfShells' and             */ 
  /* put it in the list of GeomShapes to be returned.                               */
  /* But if the shell is composed of only a face we create a face and not a shell   */
  int i = 0 ;
  for( TopExp_Explorer exp(aCompoundOfShells, TopAbs_SHELL); exp.More(); exp.Next() ) {

    const TopoDS_Shell& aShell = TopoDS::Shell( exp.Current() );    
    if( aShell.IsNull() ) {
      THROW_SALOME_CORBA_EXCEPTION("In GEOM_Gen_i::Suppressfaces() : null shell", SALOME::BAD_PARAM);
    }
    
    GEOM::GEOM_Shape::ListOfSubShapeID_var aList = new GEOM::GEOM_Shape::ListOfSubShapeID;
    aList = IndexOfFacesOfSubShell(mainShape, aShell) ;

     if( aList->length() < 1 ) {
      THROW_SALOME_CORBA_EXCEPTION("In GEOM_Gen_i::SuppressFaces() : aList is empty", SALOME::BAD_PARAM);
    }

    TopoDS_Shape aShellOrFace ;
    /* Only a face into the shell : we create a single face instead of a shell : 'aList' is unchanged  */
    if( aList->length() == 1 ) {
      TopExp_Explorer exp ;
      exp.Init( aShell, TopAbs_FACE ) ;
      exp.More() ;
      aShellOrFace = exp.Current() ;
    }
    else {
      aShellOrFace = aShell ;
    }
    
    /* Create CORBA object */
    GEOM::GEOM_Shape_var result = CreateObject(aShellOrFace) ;
    if( CORBA::is_nil(result) ) {
      THROW_SALOME_CORBA_EXCEPTION("Suppress Faces aborted : null result", SALOME::BAD_PARAM);
    }

    InsertInLabelOneArgument(mainShape, shape, aShellOrFace, result, myCurrentOCAFDoc) ;
    i++ ;
    listOfGeomShapes->length(i) ;
    listOfGeomShapes[i-1] = result ;
  }

  return listOfGeomShapes._retn() ;
}



//================================================================================
// function : IsShapeInSequence()
// purpose  : return true is aShape is in SS. The test method is 'IsSame()'
//================================================================================
bool GEOM_Gen_i::IsShapeInSequence(const TopTools_SequenceOfShape& SS, const TopoDS_Shape& aShape)
{
  if( aShape.IsNull() || SS.IsEmpty() ) 
    return false ;  
  for( int i=1; i<=SS.Length(); i++) {
    if( SS.Value(i).IsSame(aShape) )
      return true ;
  }
  return false ;
}


//================================================================================
// function : FreeEdgesFromMapOfFace()
// purpose  : Define MS a map of all edges of faces of 'MSfaces'
//          : All multiple edges are removed !
//================================================================================
void GEOM_Gen_i::FreeEdgesFromMapOfFace( const TopTools_MapOfShape& MSfaces,
					 TopTools_MapOfShape& MS )
{
  MS.Clear() ;
  TopTools_MapOfShape Multiple ;
  TopTools_MapIteratorOfMapOfShape it ;
  for( it.Initialize(MSfaces); it.More(); it.Next() ) {
    TopoDS_Shape aFace = it.Key() ;
    TopExp_Explorer exp ;
    for( exp.Init( aFace, TopAbs_EDGE); exp.More(); exp.Next() ) {
      if( !Multiple.Contains( exp.Current() ) && !MS.Add( exp.Current() ) ) {
	MS.Remove( exp.Current() ) ;
	Multiple.Add( exp.Current() ) ;
      }
    }
  }
  return ;
}


//================================================================================
// function : MapRemoveSequence()
// purpose  : In term of shapes ST = MS - SSRemove
//          :
//================================================================================
void GEOM_Gen_i::MapRemoveSequence( const TopTools_MapOfShape& MS,
				    const TopTools_SequenceOfShape& SSRemove,
				    TopTools_SequenceOfShape& ST ) 
{
  ST.Clear() ;
  TopTools_MapIteratorOfMapOfShape it ;
  for( it.Initialize(MS); it.More(); it.Next() ) {
    TopoDS_Shape aShape = it.Key() ;
    if( !IsShapeInSequence( SSRemove, aShape ) )
      ST.Append( aShape ) ;
  }
  return ;
}



//================================================================================
// function : SuppressHoleSubRoutine()
// purpose  : Define recursively 'MSfacesSuppress' a list of faces to suppress in a hole
//================================================================================
void GEOM_Gen_i::SuppressHoleSubRoutine( const TopoDS_Shape& mainShape,
					 const TopoDS_Face& aFace,
					 const TopTools_SequenceOfShape& SSedgesOfWire, 
					 const TopTools_IndexedDataMapOfShapeListOfShape& aMapEdgesFaces,
					 const TopTools_MapOfShape& MSfaces,					 
					 TopTools_MapOfShape& MSfacesSuppress,
					 const Standard_Boolean withEndFace,
					 const TopoDS_Face& endFace,
					 TopTools_MapOfShape& MSwireEndEdges )
  throw (SALOME::SALOME_Exception)
{  
  Unexpect aCatch(SALOME_SalomeException);
  TopTools_MapOfShape MS ;
  TopTools_SequenceOfShape SU ;
  FreeEdgesFromMapOfFace(MSfaces, MS) ;        /* MS = free edges of MSfaces */
  MapRemoveSequence(MS, SSedgesOfWire, SU) ;   /* SU = MS - SSedgesOfWire    */

  if( SU.IsEmpty() ) {
    return ;
  }

  /* Here SU contains new edges to find new faces to suppress                                          */
  /* Define the list of faces of SU edges that aren't in faces of MSfacesSuppress in order to add into */
  /* For each edge we have a map of all its faces : it's in 'aMapEdgesFaces'                           */
  TopTools_MapOfShape MSfacesTmp ;
  for( int v=1; v<=SU.Length(); v++ ) {
    TopoDS_Shape E = SU.Value(v) ;
    TopoDS_Shape F ;
    TopTools_ListOfShape LF ;
    int ind = aMapEdgesFaces.FindIndex(E) ;
    
    /* LF is the list of faces for an edge of SU : may be empty no matter */
    LF = aMapEdgesFaces.FindFromIndex(ind) ;
    
    TopTools_ListIteratorOfListOfShape it ;
    for( it.Initialize(LF); it.More(); it.Next() ) {
      F = it.Value() ;
      if( withEndFace == false ) {
	if( F.IsSame(aFace) )
	  THROW_SALOME_CORBA_EXCEPTION("in GEOM_Gen_i::SuppressHoleSubRoutine() : hole traversing or ?", SALOME::BAD_PARAM);
	if( !MSfacesSuppress.Contains(F) ) {
	  MSfacesSuppress.Add(F) ;
	  MSfacesTmp.Add(F) ; // Dont remove the 'if' !
	}
      }
      else { /* withEndFace == true */
	if( F.IsSame(aFace) && !F.IsSame(endFace) )
	  THROW_SALOME_CORBA_EXCEPTION("in GEOM_Gen_i::SuppressHoleSubRoutine() : hole traversing incoherent ?", SALOME::BAD_PARAM);
	
	if( F.IsSame(endFace) ) {
	  /* We have reached endFace if selection was correct so we add  */
	  /* edge in a map to find later the corresponding endWire (hole */
	  MSwireEndEdges.Add(E) ;
	}
	else {
	  if( !MSfacesSuppress.Contains(F) ) {
	    MSfacesSuppress.Add(F) ;
	    MSfacesTmp.Add(F) ;
	  }
	}
      }
    }
  }
  /* Call recursively this routine */
  SuppressHoleSubRoutine( mainShape, aFace, SSedgesOfWire, aMapEdgesFaces, MSfacesTmp, MSfacesSuppress, withEndFace, endFace, MSwireEndEdges ) ; 
}



//================================================================================
// function : GetShapeFromIndex()
// purpose  : Find 'tds' a sub shape of 'aShape' according to 'aList' that contains
//          : a unique index !
//          : Warning : index must be setted with the same exploration logic !
//          : So 'index' is calculated with no shape doublons !
//================================================================================
bool GEOM_Gen_i::GetShapeFromIndex( const TopoDS_Shape& aShape,
				    const TopAbs_ShapeEnum aType, 
				    const int index,
				    TopoDS_Shape& tds )

{
  if (aShape.IsNull() || index < 1) 
    return false ;
  /* Indices start at 1 */
  int j = 1 ;
  bool found = false ;
  TopExp_Explorer exp ;
  TopTools_MapOfShape M;
  for( exp.Init( aShape, aType ); exp.More(); exp.Next() ) {
    if( M.Add(exp.Current()) ) { /* if not a doublon : we compare */
      if( index == j ) {
	tds =  exp.Current() ;
	return true ;
      }
      j++ ;
    }
  }
  return false ;
}



//================================================================================
// function : SuppressHolesInFaceOrShell()  Main method.
// purpose  : Suppress holes identified by wires in a single face or shell
//
//================================================================================
GEOM::GEOM_Shape_ptr GEOM_Gen_i::SuppressHolesInFaceOrShell( GEOM::GEOM_Shape_ptr shapeFaceShell,
						       const GEOM::GEOM_Shape::ListOfSubShapeID& ListIdWires )
  throw (SALOME::SALOME_Exception)
{
  Unexpect aCatch(SALOME_SalomeException);
  GEOM::GEOM_Shape_var result;

  if( ListIdWires.length() < 1 )
    THROW_SALOME_CORBA_EXCEPTION("in GEOM_Gen_i::SuppressHolesInFace : no holes selected", SALOME::BAD_PARAM);
  
  const TopoDS_Shape tds = GetTopoShape(shapeFaceShell) ;
  if( tds.IsNull() || !IsValid(tds) )
    THROW_SALOME_CORBA_EXCEPTION("in GEOM_Gen_i::SuppressHolesInFace() : non valid main argument", SALOME::BAD_PARAM);
  
  /* Create a map of wires/holes to suppress */
  TopTools_MapOfShape MapHoles ;
  for ( int i = 0; i < ListIdWires.length(); i++ ) {
    TopoDS_Shape W ;    
    if( !GetShapeFromIndex( tds, TopAbs_WIRE, ListIdWires[i], W ) )
      THROW_SALOME_CORBA_EXCEPTION("in GEOM_Gen_i::SuppressHolesInFace() : bad index ?", SALOME::BAD_PARAM);
    MapHoles.Add( W ) ;
  }

  /* Test if argument is a face or shell */
  bool isFace ;
  if( tds.ShapeType() == TopAbs_FACE )
    isFace = true ;
  else if ( tds.ShapeType() == TopAbs_SHELL )
    isFace = false ;
  else
    THROW_SALOME_CORBA_EXCEPTION("in GEOM_Gen_i::SuppressHolesInFace() : not a face or a shell", SALOME::BAD_PARAM); 

  /* Define two maps : all faces and faces to that will be modified */
  TopTools_MapOfShape MapFacesToModify ;
  TopTools_MapOfShape MapFacesAll ;
  TopExp_Explorer expF ;
  for( expF.Init( tds, TopAbs_FACE);  expF.More(); expF.Next() ) {
    TopoDS_Face F = TopoDS::Face( expF.Current() ) ;
    MapFacesAll.Add(F) ;
    TopExp_Explorer expW ;
    for( expW.Init( F, TopAbs_WIRE);  expW.More(); expW.Next() ) {
      TopoDS_Wire W = TopoDS::Wire( expW.Current() ) ;
      if( MapHoles.Contains(W) ) {
	MapFacesToModify.Add(F) ;
      }
    }
  }

  /* Define faces not modified */
  TopTools_MapOfShape MapFacesNotModified ;
  TopTools_MapIteratorOfMapOfShape it ;
  for( it.Initialize(MapFacesAll); it.More(); it.Next() ) {
    TopoDS_Face FF = TopoDS::Face( it.Key() ) ;
    if( !MapFacesToModify.Contains(FF) )
      MapFacesNotModified.Add(FF) ;
  }

  if( MapFacesToModify.IsEmpty() )
    THROW_SALOME_CORBA_EXCEPTION("Error : empty map of faces", SALOME::BAD_PARAM); 
  
  if( isFace && MapFacesToModify.Extent() != 1 )
    THROW_SALOME_CORBA_EXCEPTION("Incoherent", SALOME::BAD_PARAM);
  
  /* Main argument is a face */
  if( isFace && MapFacesToModify.Extent() == 1 ) {
    TopoDS_Face resultFace ;
    if( !RebuildFaceRemovingHoles( TopoDS::Face(tds), MapHoles, resultFace ) )
      THROW_SALOME_CORBA_EXCEPTION(" Problem : !RebuildFaceRemovingHoles()", SALOME::BAD_PARAM);
    /* Creation of CORBA object : face topology */
    result = CreateObject(resultFace);
    InsertInLabelOneArgument(tds, shapeFaceShell, resultFace, result, myCurrentOCAFDoc) ;
    return result ;
  }

  /* Main argument is a shell : rebuild faces modified */
  TopTools_MapOfShape MapFacesModified ;
  for( it.Initialize(MapFacesToModify); it.More(); it.Next() ) {
    TopoDS_Face FF = TopoDS::Face( it.Key() ) ;
    TopoDS_Face resF ;
    if( !RebuildFaceRemovingHoles( FF, MapHoles, resF ) )
      THROW_SALOME_CORBA_EXCEPTION(" Problem shell : !RebuildFaceRemovingHoles()", SALOME::BAD_PARAM);
    MapFacesModified.Add(resF) ;
  }

  /* Rebuild the shell with faces modified and non modified */
  TopoDS_Shell resultShell ;
  BRepPrim_Builder B;
  B.MakeShell(resultShell) ;
  TopTools_MapIteratorOfMapOfShape it1 ;
  for( it1.Initialize(MapFacesModified); it1.More(); it1.Next() )
    B.AddShellFace( resultShell,TopoDS::Face( it1.Key() ) ) ;
  for( it1.Initialize(MapFacesNotModified); it1.More(); it1.Next() )
    B.AddShellFace( resultShell,TopoDS::Face( it1.Key() ) ) ;
  
  B.CompleteShell(resultShell) ;
  
  if( resultShell.IsNull() )
    THROW_SALOME_CORBA_EXCEPTION("Null or not valid result Shell", SALOME::BAD_PARAM) ;
  
  /* Creation of CORBA object : shell topology */
  result = CreateObject(resultShell);
  InsertInLabelOneArgument(tds, shapeFaceShell, resultShell, result, myCurrentOCAFDoc) ;
  return result ;
}


//================================================================================
// function : RebuildFaceRemovingHoles()
// purpose  : Rebuild a face removing holes that are in 'mapHoles'.
//          : NB : 'mapHoles' may content more holes than necessary
//================================================================================
bool GEOM_Gen_i::RebuildFaceRemovingHoles( const TopoDS_Face& aFace,
					   const TopTools_MapOfShape& mapHoles,
					   TopoDS_Shape& resultFace )
{
  /* Get the outer wire of the face 'aFace' */
  TopoDS_Wire outW = BRepTools::OuterWire( aFace ) ;
  if( outW.IsNull() || !IsValid(outW) )
    THROW_SALOME_CORBA_EXCEPTION("in GEOM_Gen_i::SuppressHolesInFace : bad outer wire of 'aFace'", SALOME::BAD_PARAM);
  
  /* Rebuild a face avoiding holes in the map 'mapHoles' */  
  Handle(Geom_Surface) Surface = BRep_Tool::Surface(aFace) ;
  TopoDS_Face F2 = BRepBuilderAPI_MakeFace( Surface, outW, true ) ;
  
  if( F2.Orientation() != aFace.Orientation() )
    F2.Orientation( aFace.Orientation() ) ;
  
  BRepBuilderAPI_MakeFace aBuilder( F2 ) ;
  bool foundAndKeepHoles = false ;
  TopExp_Explorer exp ;
  
  for( exp.Init( aFace, TopAbs_WIRE);  exp.More(); exp.Next() ) {
    TopoDS_Wire hole = TopoDS::Wire( exp.Current() ) ;
    if( !mapHoles.Contains(hole) && !exp.Current().IsEqual(outW) ) {
      aBuilder.Add( hole) ;      
      if( !aBuilder.IsDone() )
	THROW_SALOME_CORBA_EXCEPTION("in GEOM_Gen_i::SuppressHolesInFace : builder problem !", SALOME::BAD_PARAM);      
      
      resultFace = TopoDS::Face(aBuilder) ;
      foundAndKeepHoles = true ;
    }
  }
  
  if( !foundAndKeepHoles )
    resultFace = F2 ;
  else
    resultFace = TopoDS::Face(aBuilder) ;
  
  return true ;
}




//================================================================================
// function : SuppressHole() Main method.
// purpose  : Suppress an hole identified by a wire in a face of shape
//          : ListIdFace contains a unique index of face in shape
//          : ListIdWire contains a unique index of wire in face !!!
//          : ListIdEndFace is used only when hole traverse.
//================================================================================
GEOM::GEOM_Shape_ptr GEOM_Gen_i::SuppressHole( GEOM::GEOM_Shape_ptr shape,
					 const GEOM::GEOM_Shape::ListOfSubShapeID& ListIdFace,
					 const GEOM::GEOM_Shape::ListOfSubShapeID& ListIdWire,
					 const GEOM::GEOM_Shape::ListOfSubShapeID& ListIdEndFace )
  throw (SALOME::SALOME_Exception)
{
  Unexpect aCatch(SALOME_SalomeException);
  GEOM::GEOM_Shape_var result;
  TopoDS_Face aFace ;
  TopoDS_Wire aWire ;  
  TopoDS_Face endFace ;
  bool withEndFace ;
  TopoDS_Shape tmp ;
  
  /* Retrieve 'aShape' the initial main shape selection */
  const TopoDS_Shape aShape = GetTopoShape(shape);
  
  if( !IsValid(aShape) )
    THROW_SALOME_CORBA_EXCEPTION("in GEOM_Gen_i::SuppressHole() : non valid main shape", SALOME::BAD_PARAM);
  
  if( ListIdFace.length() != 1 || ListIdWire.length() != 1 )
    THROW_SALOME_CORBA_EXCEPTION("bad list", SALOME::BAD_PARAM);  
  
  /* Retrieve 'aFace' selection */
  if( !GetShapeFromIndex( aShape, TopAbs_FACE, ListIdFace[0], tmp ) ) {
    THROW_SALOME_CORBA_EXCEPTION("face not found", SALOME::BAD_PARAM);
  }
  else {
    aFace = TopoDS::Face(tmp) ;
  }
  if( !IsValid(aFace) )
    THROW_SALOME_CORBA_EXCEPTION("in GEOM_Gen_i::SuppressHole() : face shape not valid", SALOME::BAD_PARAM);  
  
   /* Retrieve 'aWire' selection : Warning : index of wire refers to the face ! */
  TopoDS_Shape aTmp ;
  if( !GetShapeFromIndex( aFace, TopAbs_WIRE, ListIdWire[0], aTmp ) ) {
    THROW_SALOME_CORBA_EXCEPTION("wire not found", SALOME::BAD_PARAM);
  }
  else {
    aWire = TopoDS::Wire(aTmp) ;
  }
  if( !IsValid(aWire) )
    THROW_SALOME_CORBA_EXCEPTION("in GEOM_Gen_i::SuppressHole() : bad wire" , SALOME::BAD_PARAM);

  /* Get the outer wire of aFace */
  TopoDS_Wire outerW = BRepTools::OuterWire( aFace ) ;
  if( outerW.IsNull() || !IsValid(outerW) ) 
    THROW_SALOME_CORBA_EXCEPTION("in GEOM_Gen_i::SuppressHole() : bad outer wire", SALOME::BAD_PARAM);
  
  /* Test bad user selection aWire */
  if( aWire.IsSame(outerW) )
    THROW_SALOME_CORBA_EXCEPTION("in GEOM_Gen_i::SuppressHole() : outerW = aWire", SALOME::BAD_PARAM);
   
  /* Test if 'endFace' is used  as argument and seems to be a valid one          */
  /* NB : 'endFace' is optional and used when hole to suppress traverse 'aShape' */
  if( ListIdEndFace.length() == 0 ) {
    withEndFace = false ;
  }
  else {
    TopoDS_Shape aTemp ;
    if( !GetShapeFromIndex( aShape, TopAbs_FACE, ListIdEndFace[0], aTemp ) || tmp.IsNull() || !IsValid(aTemp) )
      THROW_SALOME_CORBA_EXCEPTION("in GEOM_Gen_i::SuppressHole() : non valid endFace", SALOME::BAD_PARAM);

    /* Test if 'endFace' as at least one hole */    
    endFace = TopoDS::Face(aTemp) ;  

    TopExp_Explorer fExp ;
    int nbWires = 0 ;
    for( fExp.Init(endFace, TopAbs_WIRE);  fExp.More(); fExp.Next() ) {
      TopoDS_Wire W = TopoDS::Wire( fExp.Current() ) ;
      if( !W.IsNull() && IsValid(W) )
	nbWires++ ;
    }
    if(nbWires > 1)
      withEndFace = true ; /* at least 2 wires : outer wire plus an hole or more */
    else
      THROW_SALOME_CORBA_EXCEPTION("in GEOM_Gen_i::SuppressHole() : end face selection ?", SALOME::BAD_PARAM);
  }
  
  /* Find edges of aWire and test if degenerated */
  TopTools_SequenceOfShape SSedgesOfWire ;
  TopExp_Explorer wireExp ;  
  for( wireExp.Init(aWire, TopAbs_EDGE);  wireExp.More(); wireExp.Next() ) {
    TopoDS_Edge E = TopoDS::Edge( wireExp.Current() ) ;
    if( E.IsNull() || BRep_Tool::Degenerated(E) ) {
      THROW_SALOME_CORBA_EXCEPTION("in GEOM_Gen_i::SupressHole() : found bad edge", SALOME::BAD_PARAM);
    }
    else {
      SSedgesOfWire.Append( wireExp.Current() ) ;
    }
  }
  if( SSedgesOfWire.Length() < 1 )
    THROW_SALOME_CORBA_EXCEPTION("in GEOM_Gen_i::SupressHole() : no edge(s) for aWire", SALOME::BAD_PARAM); 
  
  /* Retrieve face ancestors of all edges of 'aWire' but avoiding 'aFace' */ 
  
  TopTools_IndexedDataMapOfShapeListOfShape aMapEdgesFaces;
  TopTools_MapIteratorOfMapOfShape anIt ;
  TopTools_MapOfShape MFSuppress ;
  TopTools_MapOfShape MFSuppressTmp ;
  bool wireOnFace = false ;
 
  TopExp::MapShapesAndAncestors(aShape, TopAbs_EDGE, TopAbs_FACE, aMapEdgesFaces) ;  
  for( int h=1; h<=SSedgesOfWire.Length(); h++ ) {
    
    TopoDS_Shape anEdgeOfWire = SSedgesOfWire.Value(h) ;
    int ind = aMapEdgesFaces.FindIndex(anEdgeOfWire) ;
    if(ind < 1)
      THROW_SALOME_CORBA_EXCEPTION("in GEOM_Gen_i::SupressHole() : index of edge", SALOME::BAD_PARAM);
    
    TopTools_ListOfShape LF;
    LF = aMapEdgesFaces.FindFromIndex(ind) ; /* Contains all faces ancestors of an edge of wire */
    if( LF.IsEmpty() )
      THROW_SALOME_CORBA_EXCEPTION("in GEOM_Gen_i::SupressHole() : no face for an edge", SALOME::BAD_PARAM);
    
    /* Filter faces avoiding 'aFace' */
    TopTools_ListIteratorOfListOfShape it ;
    for( it.Initialize(LF); it.More(); it.Next() ) {
      TopoDS_Face F = TopoDS::Face( it.Value() ) ;
      if( !F.IsSame(aFace) ) { 
	MFSuppressTmp.Add(F) ;
	MFSuppress.Add(F) ;
      }
      else {
	wireOnFace = true ;
      }
    }
  }
  
  if( !wireOnFace ) {
    THROW_SALOME_CORBA_EXCEPTION("in GEOM_Gen_i::SupressHole() : wire not on selected face", SALOME::BAD_PARAM);  
  }

  /* Call routine to define faces to suppress and and optional endWire on endFace */
  TopTools_MapOfShape MSwireEndEdges ; /* will contain edges of final wire (hole) */
  SuppressHoleSubRoutine( aShape, aFace, SSedgesOfWire, aMapEdgesFaces, MFSuppressTmp, MFSuppress, withEndFace, endFace, MSwireEndEdges ) ;

  TopoDS_Wire endWire ;
  if( withEndFace ) {
    
    if( MSwireEndEdges.Extent() < 1 )
      THROW_SALOME_CORBA_EXCEPTION("in GEOM_Gen_i::SupressHole() : MSwireEndEdges.Extent() < 1", SALOME::BAD_PARAM);

    if( !FindCompareWireHoleOnFace( endFace, MSwireEndEdges, endWire ) )           
      THROW_SALOME_CORBA_EXCEPTION("in GEOM_Gen_i::SupressHole() : no endWire found", SALOME::BAD_PARAM);    
   }
   
  /* Build 'resTds' : a shape containing a compound of faces */
  TopoDS_Shape resTds;
  if( !withEndFace && !BuildShapeHoleNotTraversing( aShape, aFace, aWire, MFSuppress, resTds ) )
    THROW_SALOME_CORBA_EXCEPTION("Rebuild result shape has aborted", SALOME::BAD_PARAM);
  
  if( withEndFace && !BuildShapeHoleTraversing( aShape, aFace, aWire, MFSuppress, endFace, endWire, resTds ) )
    THROW_SALOME_CORBA_EXCEPTION("Rebuild result shape has aborted (end hole)", SALOME::BAD_PARAM); 
  
  /* Reconstruction of final shape with 'resTds' : a compound of faces after hole suppressing */
  /* Actual limitation is : 'aShape' must not contain more than a solid or a shell !          */
  TopoDS_Shape finalShape ;
 
  TopExp_Explorer exp ;
  unsigned int nbSolid = 0 ;
  TopoDS_Solid aSolid ;
  for( exp.Init(aShape, TopAbs_SOLID); exp.More(); exp.Next() ) {
    aSolid = TopoDS::Solid( exp.Current() ) ;
    nbSolid++ ;
    if( nbSolid > 1 )
      THROW_SALOME_CORBA_EXCEPTION("Limitation : main shape contents more than one solid", SALOME::BAD_PARAM);  
  }
   
  unsigned int nbShell = 0 ;
  TopoDS_Shell aShell ;
  for( exp.Init(aShape, TopAbs_SHELL); exp.More(); exp.Next() ) {
    aShell = TopoDS::Shell( exp.Current() ) ;
    nbShell++ ;
    if( nbShell > 1 )
      THROW_SALOME_CORBA_EXCEPTION("Limitation : main shape contents more than one shell", SALOME::BAD_PARAM);
  }
  
  /* No shells and no solids : can send a compound even for a single face, see GUI ! */
  if( nbShell == 0 ) {
    finalShape = resTds ;
  }
  
  /* a shell */
  TopoDS_Shell shellResult ;
  if( nbShell == 1 ) {
    if ( !BuildShellWithFaceCompound( TopoDS::Compound(resTds), shellResult ) ) 
      THROW_SALOME_CORBA_EXCEPTION("Error after BuildShellWithFaceCompound()", SALOME::BAD_PARAM);
    finalShape = shellResult ;
  }
  
  /* a solid with a shell */
  if( nbSolid == 1 && nbShell == 1) {
    BRepBuilderAPI_MakeSolid B;
    B.Add(shellResult) ;
    if( !B.IsDone() )
      THROW_SALOME_CORBA_EXCEPTION("Error : !B.IsDone()", SALOME::BAD_PARAM);  
    finalShape = B.Solid() ;
  }

  result = CreateObject(finalShape);
  InsertInLabelOneArgument(aShape, shape, finalShape, result, myCurrentOCAFDoc) ;
  return result ;
}



//================================================================================
// function : BuildShellWithFaceCompound()
// purpose  : Build a shell with a compound of faces.
//================================================================================
bool GEOM_Gen_i::BuildShellWithFaceCompound( const TopoDS_Compound Comp,
					     TopoDS_Shell& resultShell )
{
  resultShell.Nullify() ;
  BRepPrim_Builder B ;
  B.MakeShell(resultShell) ;  
  TopExp_Explorer ex ;
  int i = 0 ;
  for( ex.Init( Comp, TopAbs_FACE); ex.More(); ex.Next() ) {
    TopoDS_Face F = TopoDS::Face( ex.Current() ) ;
    if( !IsValid(F) ) {
      return false ;
    }
    B.AddShellFace( resultShell, F ) ;
  }
  B.CompleteShell(resultShell) ;
  if( resultShell.IsNull() ) {
    return false ;
  }  
  return true ;
}


//================================================================================
// function : FindCompareWireHoleOnFace()
// purpose  : Try to find a wire on 'aFace' which edges are same than those
//          : into 'MSwireEdges' map. 'aFoundWire' is defined and 'true' returned.
//          : 'MSwireEdges' represents generally an hole an 'aFace'.
//          : The outer wire of 'aFace' is avoided !
//================================================================================
bool GEOM_Gen_i::FindCompareWireHoleOnFace( const TopoDS_Face& F,
					    const TopTools_MapOfShape& MSwireEdges,
					    TopoDS_Wire& aFoundWire )
{
  aFoundWire.Nullify() ;
  
  if( F.IsNull() )
    return false ;
  
  /* Get the outer wire of aFace */
  TopoDS_Wire outerW = BRepTools::OuterWire(F) ;
  if( outerW.IsNull() || !IsValid(outerW) ) {
    return false ;
  }
  
  int nbEdges = MSwireEdges.Extent() ;
  if( nbEdges < 1 ) {
    return false ;
  }
  
  TopExp_Explorer exp1 ;
  TopExp_Explorer exp2 ;
  for ( exp1.Init(F, TopAbs_WIRE) ; exp1.More(); exp1.Next() ) {
    TopoDS_Wire W = TopoDS::Wire( exp1.Current() ) ;
    int i = 0 ;
    if( !W.IsSame(outerW) ) {
      for ( exp2.Init( W, TopAbs_EDGE) ; exp2.More(); exp2.Next() ) {
	TopoDS_Edge E = TopoDS::Edge( exp2.Current() ) ;
	if( MSwireEdges.Contains(E) ) {
	  i++ ;
	  if( i == nbEdges ) {
	    aFoundWire = W ;
	    return true ;
	  }
	}
      }
    }
  }
  return false ;
}


//================================================================================
// function : BuildShapeHoleNotTraversing()
// purpose  : Define 'resultTds' a reconstruction of 'aShape' after modification
//          : on 'aFace' where 'aWire is removed' and suppression of faces 'MFSuppress'
//          : ( Used as a sub routine of SuppressHole() )
//================================================================================
bool GEOM_Gen_i::BuildShapeHoleNotTraversing( const TopoDS_Shape& aShape,
					      const TopoDS_Face& aFace,
					      const TopoDS_Wire& aWire,
					      const TopTools_MapOfShape& MFSuppress,
					      TopoDS_Shape& resultTds )
  throw (SALOME::SALOME_Exception)
{
  Unexpect aCatch(SALOME_SalomeException);
  BRep_Builder B;
  TopExp_Explorer exp ;
  TopoDS_Face newFace ;
  
  resultTds.Nullify() ;
  TopoDS_Compound Comp ;
  B.MakeCompound (Comp);
  
  try  {
    
    /* Explore all faces of 'aShape' to rebuild a compound */
    for ( exp.Init(aShape, TopAbs_FACE) ; exp.More(); exp.Next() ) {
      
      TopoDS_Face F1 = TopoDS::Face( exp.Current() );
      /* Rebuild face(s) not suppressed */
      if( !MFSuppress.Contains(F1) ) {
	
	if( F1.IsEqual( aFace ) ) {
	  TopTools_MapOfShape MSwire ;
	  MSwire.Add(aWire) ;
	  if( !RebuildFaceRemovingHoles(aFace, MSwire, newFace) ) {
	    return false ;
	  }
	  B.Add( Comp, newFace ) ;
	}
	else {
	  /* For any other face not suppressed */
	  B.Add( Comp, F1 ) ;
	}
      }
    }
  }
  catch(Standard_Failure) {
    THROW_SALOME_CORBA_EXCEPTION("in BuildShapeHoleNotTraversing() : Exception catched", SALOME::BAD_PARAM);
  }  
  resultTds = Comp ;
  return true ;
}



//================================================================================
// function : BuildShapeHoleTraversing()
// purpose  : Define 'resultTds' a reconstruction of 'aShape' after modifications.
//          : On 'aFace'    'aWire    is removed'
//          : On 'endFace'  'endWire' is removed.
//          : Faces of 'MFSuppress' are removed.
//          : ( Used as a sub routine of SuppressHole() )
//================================================================================
bool GEOM_Gen_i::BuildShapeHoleTraversing( const TopoDS_Shape& aShape,
					   const TopoDS_Face& aFace,
					   const TopoDS_Wire& aWire,
					   const TopTools_MapOfShape& MFSuppress,
					   const TopoDS_Face& endFace,
					   const TopoDS_Wire& endWire,
					   TopoDS_Shape& resultTds )
  throw (SALOME::SALOME_Exception)
{
  Unexpect aCatch(SALOME_SalomeException);
  BRep_Builder B;
  TopExp_Explorer exp ;
  TopoDS_Face newFace ;

  resultTds.Nullify() ;
  TopoDS_Compound Comp ;
  B.MakeCompound (Comp);
  
  /* Necessary to use general method */
  TopTools_MapOfShape MSwire1  ;
  MSwire1.Add(aWire) ; 
  TopTools_MapOfShape MSwire2 ;
  MSwire2.Add(endWire) ;

  try  {    
    
    /* Explore all faces of 'aShape' to rebuild a compound */
    for ( exp.Init(aShape, TopAbs_FACE) ; exp.More(); exp.Next() ) {
      TopoDS_Face F1 = TopoDS::Face( exp.Current() );      
      
      /* Rebuild face(s) not suppressed */
      if( !MFSuppress.Contains(F1) ) {
	
	/* Rebuild 'aFace' */
	if( F1.IsEqual( aFace ) && !F1.IsEqual( endFace ) ) {
	  if( !RebuildFaceRemovingHoles(aFace, MSwire1, newFace) ) {
	    return false ;
	  }
	  B.Add(Comp, newFace) ;
	}	
	
	/* Rebuild 'endFace' */
	if( !F1.IsEqual( aFace ) && F1.IsEqual( endFace ) ) {
	  if( !RebuildFaceRemovingHoles(endFace, MSwire2, newFace) ) {
	    return false ;
	  }
	  B.Add(Comp, newFace) ;
	}
	
	/* Hole in the same face : aFace = endFace */
	if( F1.IsEqual( aFace ) && F1.IsEqual( endFace ) ) {
	  TopoDS_Face FF ;
	  if( !RebuildFaceRemovingHoles(aFace, MSwire1, newFace) || !RebuildFaceRemovingHoles(newFace, MSwire2, FF) ) {
	    return false ;
	  }
	  B.Add( Comp, FF ) ;
	}
	
	/* For any other face not suppressed */
	if( !F1.IsEqual(aFace) && !F1.IsEqual( endFace ) ) {	  
	  B.Add( Comp, F1 ) ;
	}
	
      }
    }
  }
  catch(Standard_Failure) {
    THROW_SALOME_CORBA_EXCEPTION("in BuildShapeHoleTraversing() : Exception catched", SALOME::BAD_PARAM);
  }  
  resultTds = Comp ;
  return true ;
}

//=======================================================================
//function : SortShapes
//purpose  : 
//=======================================================================

static void SortShapes(TopTools_ListOfShape& SL)
{
  Standard_Integer MaxShapes = SL.Extent();
  TopTools_Array1OfShape  aShapes (1,MaxShapes);
  TColStd_Array1OfInteger OrderInd(1,MaxShapes);
  TColStd_Array1OfReal    MidXYZ  (1,MaxShapes); //X,Y,Z;
  TColStd_Array1OfReal    Length  (1,MaxShapes); //X,Y,Z;
  
  // Computing of CentreOfMass
  Standard_Integer Index;
  GProp_GProps GPr;
  gp_Pnt GPoint;
  TopTools_ListIteratorOfListOfShape it(SL);
  for (Index=1;  it.More();  Index++)
  {
    TopoDS_Shape S = it.Value();
    SL.Remove( it ); // == it.Next()
    aShapes(Index) = S;
    OrderInd.SetValue (Index, Index);
    if (S.ShapeType() == TopAbs_VERTEX)
    {
      GPoint = BRep_Tool::Pnt( TopoDS::Vertex( S ));
      Length.SetValue( Index, (Standard_Real) S.Orientation());
    }
    else
    {
      BRepGProp::LinearProperties (S, GPr);
      GPoint = GPr.CentreOfMass();
      Length.SetValue( Index, GPr.Mass() );
    }
    MidXYZ.SetValue(Index,
		    GPoint.X()*999 + GPoint.Y()*99 + GPoint.Z()*0.9);
  }
  // Sorting
  Standard_Integer aTemp;
  Standard_Boolean exchange, Sort = Standard_True;
  while (Sort)
  {
    Sort = Standard_False;
    for (Index=1; Index < MaxShapes; Index++)
    {
      if (MidXYZ(OrderInd(Index)) > MidXYZ(OrderInd(Index+1)))
	exchange = Standard_True;
      else if (MidXYZ(OrderInd(Index)) == MidXYZ(OrderInd(Index+1)) &&
	       Length(OrderInd(Index)) >  Length(OrderInd(Index+1)) )
	exchange = Standard_True;
      else
	exchange = Standard_False;
      if (exchange)
      {
        aTemp = OrderInd(Index);
        OrderInd(Index) = OrderInd(Index+1);
        OrderInd(Index+1) = aTemp;
        Sort = Standard_True;
      }
    }
  }
  for (Index=1; Index <= MaxShapes; Index++)
    SL.Append( aShapes( OrderInd(Index) ));
}

//================================================================================
// function : SubShape()
// purpose  : Method for GUI or TUI
//================================================================================

GEOM::GEOM_Shape_ptr GEOM_Gen_i::SubShape(GEOM::GEOM_Shape_ptr shape,
					  CORBA::Short ShapeType, 
					  const GEOM::GEOM_Shape::ListOfSubShapeID& ListOfID)
     throw (SALOME::SALOME_Exception)
{
  Unexpect aCatch(SALOME_SalomeException);
  return SubShapesOne(shape, (TopAbs_ShapeEnum) ShapeType, ListOfID);
}

//================================================================================
// function : SubShapeSorted()
// purpose  : Method for GUI or TUI
//================================================================================

GEOM::GEOM_Shape_ptr GEOM_Gen_i::SubShapeSorted(GEOM::GEOM_Shape_ptr shape,
						CORBA::Short ShapeType, 
						const GEOM::GEOM_Shape::ListOfSubShapeID& ListOfID)
     throw (SALOME::SALOME_Exception)
{
  Unexpect aCatch(SALOME_SalomeException);
  return SubShapesOne(shape, (TopAbs_ShapeEnum) ShapeType, ListOfID, Standard_True);
}

//================================================================================
// function : SubShapesOne()
// purpose  :
//================================================================================

GEOM::GEOM_Shape_ptr GEOM_Gen_i::SubShapesOne( GEOM::GEOM_Shape_ptr shape,
					      const TopAbs_ShapeEnum ShapeType, 
					      const GEOM::GEOM_Shape::ListOfSubShapeID& ListOfID,
					      const Standard_Boolean Sort)
  throw (SALOME::SALOME_Exception)
{
  Unexpect aCatch(SALOME_SalomeException);
  GEOM::GEOM_Shape_var result;
  TopoDS_Shape mainShape;
  TopoDS_Shape mainTopo = GetTopoShape(shape);

  bool main = false;
  while ( !main ) {
    if ( shape->IsMainShape() ) {
      mainShape = GetTopoShape(shape);
      main = true;
    } else
      shape = GetIORFromString( shape->MainName() );
  }
  
  if(ListOfID.length() < 1) {
    THROW_SALOME_CORBA_EXCEPTION("In GEOM_Gen_i::SubShape() : bad list of shapes",
				 SALOME::BAD_PARAM);
  }
  
  /* Create a sequence of all sub shapes */
  TopTools_ListOfShape listShape;

  TopTools_MapOfShape mapShape;
  TopExp_Explorer exp ;
  for ( exp.Init( mainShape, TopAbs_ShapeEnum(ShapeType)) ; exp.More(); exp.Next() ) {
    if ( mapShape.Add( exp.Current() ) )
      listShape.Append ( exp.Current() );
  }

  if (listShape.IsEmpty()) return shape;

  if (Sort)
    SortShapes(listShape);

  TopTools_SequenceOfShape SS;
  TopTools_ListIteratorOfListOfShape it (listShape);
  for (; it.More(); it.Next())
    SS.Append( it.Value() );

  TopoDS_Shape SubShape;
  if (ListOfID.length() == 1)
    SubShape = SS.Value(ListOfID[0]);
  else
  {
    BRep_Builder B;
    TopoDS_Compound Comp;
    B.MakeCompound (Comp);
    unsigned int ind;
    for ( ind = 0; ind < ListOfID.length(); ind++ )
      B.Add( Comp, SS.Value(ListOfID[ind]) );
    SubShape = Comp;
  }

  if ( !TNaming_Tool::HasLabel( myCurrentOCAFDoc->Main(), SubShape ) ) 
    result = CreateSubObject( SubShape, shape, ListOfID);
  else {
    int TransDef;
    TDF_Label Lab = TNaming_Tool::Label( myCurrentOCAFDoc->Main(), SubShape, TransDef );
    Handle(TDataStd_Name) Att;
    if ( Lab.FindAttribute( TDataStd_Name::GetID(), Att ) ) {
      TCollection_AsciiString nameIOR( Att->Get() );
      result = GEOM::GEOM_Shape::_narrow(_orb->string_to_object( nameIOR.ToCString() ));
      if ( strcmp( result->MainName(), _orb->object_to_string(shape) ) != 0 ) {
	result = CreateSubObject( SubShape, shape, ListOfID);
      }
    }
  }

  return result;
}


//================================================================================
// function : SubShapeAll()
// purpose  : Explode a shape in all sub shapes with a type (Method for TUI or GUI)
//================================================================================

GEOM::GEOM_Gen::ListOfGeomShapes* GEOM_Gen_i::SubShapeAll(GEOM::GEOM_Shape_ptr shape,
							  CORBA::Short ShapeType)
     throw (SALOME::SALOME_Exception)
{
  Unexpect aCatch(SALOME_SalomeException);
  return SubShapesAll(shape, (TopAbs_ShapeEnum) ShapeType);
}

//================================================================================
// function : SubShapeAllSorted()
// purpose  : Explode a shape in all sub shapes with a type (Method for TUI or GUI)
//================================================================================

GEOM::GEOM_Gen::ListOfGeomShapes* GEOM_Gen_i::SubShapeAllSorted(GEOM::GEOM_Shape_ptr shape,
								CORBA::Short ShapeType)
     throw (SALOME::SALOME_Exception)
{
  Unexpect aCatch(SALOME_SalomeException);
  return SubShapesAll(shape, (TopAbs_ShapeEnum) ShapeType, Standard_True);
}

//================================================================================
// function : SubShapeAllSorted()
// purpose  :
//================================================================================

GEOM::GEOM_Gen::ListOfGeomShapes* GEOM_Gen_i::SubShapesAll(GEOM::GEOM_Shape_ptr shape,
							   const TopAbs_ShapeEnum ShapeType,
							   const Standard_Boolean Sort)
  throw (SALOME::SALOME_Exception)
{
  /* List of sub shapes returned */
  Unexpect aCatch(SALOME_SalomeException);
  GEOM::GEOM_Gen::ListOfGeomShapes_var listOfGeomShapes = new GEOM::GEOM_Gen::ListOfGeomShapes;
  listOfGeomShapes->length(0) ;
  
  TopoDS_Shape mainTopo = GetTopoShape(shape);
  TopoDS_Shape mainShape;
  bool main = false;
  while ( !main ) {
    if ( shape->IsMainShape() ) {
      mainShape = GetTopoShape(shape);
      main = true;
    } else
      shape = GetIORFromString( shape->MainName() );
  }

  if( mainTopo.IsNull() ) {
    THROW_SALOME_CORBA_EXCEPTION("In GEOM_Gen_i::SubShapeAll() : null main shape",
				 SALOME::BAD_PARAM);
  }
  
  /* List/index : field set for any sub shape */
  GEOM::GEOM_Shape::ListOfSubShapeID_var ListOfID = new GEOM::GEOM_Shape::ListOfSubShapeID;
  ListOfID->length(1) ;

    // retrieve all subshapes
  TopTools_MapOfShape mapShape;
  TopTools_ListOfShape listShape;
    
  if (mainTopo.ShapeType()==TopAbs_COMPOUND  &&  ShapeType==TopAbs_SHAPE)
  {
    TopoDS_Iterator It(mainTopo,Standard_True,Standard_True );  
    for ( ; It.More(); It.Next() ) 
      if (mapShape.Add( It.Value() ))
	listShape.Append( It.Value() );
  }
  else
  {
    TopExp_Explorer exp ( mainTopo, ShapeType);
    for ( ; exp.More(); exp.Next() ) 
      if (mapShape.Add( exp.Current() ))
	listShape.Append( exp.Current() );
  }

  if (Sort)
    SortShapes(listShape);
    
  /* Create all sub shapes */
  int index;
  GEOM::GEOM_Shape_var result;
    
  TopTools_ListIteratorOfListOfShape itSub (listShape);
  for (index = 1; itSub.More(); itSub.Next(), ++index)
  {
    const TopoDS_Shape& SubShape = itSub.Value();
    // check if SubShape is already in OCAFDS and ...
    if ( TNaming_Tool::HasLabel( myCurrentOCAFDoc->Main(), SubShape ) )
    {
      int TransDef;
      TDF_Label Lab = TNaming_Tool::Label( myCurrentOCAFDoc->Main(), SubShape, TransDef );
      Handle(TDataStd_Name) Att;
      if ( Lab.FindAttribute( TDataStd_Name::GetID(), Att ) )
      {
        TCollection_AsciiString nameIOR( Att->Get() );
        result = GEOM::GEOM_Shape::_narrow(_orb->string_to_object( nameIOR.ToCString() ));
	// ... it is subshape of <shape>
        if ( strcmp( result->MainName(), _orb->object_to_string(shape) ) == 0 ) 
        {
          listOfGeomShapes->length(index) ;
          listOfGeomShapes[index-1] = result ;
	  continue;
        }
      }
    }

//      if (Sort)
//        ListOfID[0] = index;
//      else
    ListOfID[0] = GetIndexTopology( SubShape, mainShape ) ;
    result = CreateSubObject( SubShape, shape, ListOfID);
    /* Add each sub shape in the list returned */
    listOfGeomShapes->length(index) ;
    listOfGeomShapes[index-1] = result ;      
  }

  return listOfGeomShapes._retn() ;
}

//=================================================================================
// function : MakeBoolean()
// purpose  : Boolean operation according to the type 'operation'
//=================================================================================
GEOM::GEOM_Shape_ptr GEOM_Gen_i::MakeBoolean(GEOM::GEOM_Shape_ptr shape1,
					     GEOM::GEOM_Shape_ptr shape2,
					     CORBA::Long operation) 
  throw (SALOME::SALOME_Exception)
{
  Unexpect aCatch(SALOME_SalomeException);
  GEOM::GEOM_Shape_var result;
  TopoDS_Shape shape ;
  TopoDS_Shape aShape1  ;
  TopoDS_Shape aShape2  ;

  try {
    aShape1 = GetTopoShape(shape1) ;
    aShape2 = GetTopoShape(shape2) ;
    
    if( aShape1.IsNull() || aShape2.IsNull() ) {
      THROW_SALOME_CORBA_EXCEPTION("Boolean aborted : argument shape is null", SALOME::BAD_PARAM);
    }
    
    switch (operation)
      {
      case 1 :   /* Common */
	shape = BRepAlgoAPI_Common(aShape1, aShape2).Shape();
	break ;
      case 2 :   /* Cut */
	shape = BRepAlgoAPI_Cut(aShape1, aShape2).Shape();
	break ;
      case 3 :   /* Fuse */
	shape = BRepAlgoAPI_Fuse(aShape1, aShape2).Shape();
	break ;
      case 4 :   /* Section */
	shape = BRepAlgoAPI_Section(aShape1, aShape2).Shape();
	break ;
      default :
	MESSAGE("Boolean operation not known : " << operation ) ;
	return result ;
      }
  }
  catch(Standard_Failure) {
    THROW_SALOME_CORBA_EXCEPTION("Exception catched in boolean operation", SALOME::BAD_PARAM);
  }
    
  /* We test the validity of resulting shape */
  if( !IsValid(shape) ) {
    THROW_SALOME_CORBA_EXCEPTION("Boolean aborted : non valid shape result", SALOME::BAD_PARAM);
  }
	
  result = CreateObject(shape) ;
      
  /* put shape and name into geom/OCAF doc */
  GEOMDS_Commands GC(myCurrentOCAFDoc->Main());
  /* add attributs 'shape' and' name_io'r in a new label */
  TDF_Label Lab = GC.Generated(aShape1, shape, result->Name() );
  TCollection_AsciiString entry;
  TDF_Tool::Entry(Lab, entry);
  result->ShapeId( entry.ToCString() ) ;
      
  /* Create a new label */
  TDF_Label NewLab = Lab.NewChild();
  TCollection_ExtendedString Value("Arguments");
  TDataStd_Name::Set(NewLab,Value);
  
  TDF_Label NewLab1 = NewLab.NewChild();
  TDF_Label RefLab;
  TDF_Tool::Label(myCurrentOCAFDoc->GetData(), shape1->ShapeId(), RefLab);
  TDF_Reference::Set(NewLab1, RefLab);
  
  TDF_Label NewLab2 = NewLab.NewChild();
  TDF_Tool::Label(myCurrentOCAFDoc->GetData(), shape2->ShapeId(), RefLab);
  TDF_Reference::Set(NewLab2, RefLab);
  return result ;
}


//=================================================================================
// function : MakeFuse()
// purpose  : Special purpose !
//=================================================================================
GEOM::GEOM_Shape_ptr GEOM_Gen_i::MakeFuse(GEOM::GEOM_Shape_ptr shape1,
				    GEOM::GEOM_Shape_ptr shape2)
  throw (SALOME::SALOME_Exception)
{
  Unexpect aCatch(SALOME_SalomeException);
  GEOM::GEOM_Shape_var result;
  TopoDS_Shape aShape1 = GetTopoShape(shape1) ;
  TopoDS_Shape aShape2 = GetTopoShape(shape2) ;
  if( aShape1.IsNull() || aShape2.IsNull() ) {
    THROW_SALOME_CORBA_EXCEPTION("Fuse aborted : shape in argument is null", SALOME::BAD_PARAM);
  }
  TopoDS_Shape shape;
  try {
    shape  = BRepAlgoAPI_Fuse(aShape1, aShape2).Shape();
  }
  catch(Standard_Failure) {
    THROW_SALOME_CORBA_EXCEPTION("Exception catched in Fuse operation", SALOME::BAD_PARAM);
  }

  /* We test the validity of resulting shape */
  if( !IsValid(shape) ) {
    THROW_SALOME_CORBA_EXCEPTION("Fuse aborted : non valid shape result", SALOME::BAD_PARAM);
  }

  result = CreateObject(shape) ;

  /* put shape and name into geom/OCAF doc */
  GEOMDS_Commands GC(myCurrentOCAFDoc->Main());
  /* add attributs 'shape' and' name_io'r in a new label */
  TDF_Label Lab = GC.Generated(aShape1, shape, result->Name() );
  TCollection_AsciiString entry;
  TDF_Tool::Entry(Lab, entry);
  result->ShapeId( entry.ToCString() ) ;

  /* Create a new label */
  TDF_Label NewLab = Lab.NewChild();
  TCollection_ExtendedString Value("Arguments");
  TDataStd_Name::Set(NewLab,Value);

  TDF_Label NewLab1 = NewLab.NewChild();
  TDF_Label RefLab;
  TDF_Tool::Label(myCurrentOCAFDoc->GetData(), shape1->ShapeId(), RefLab);
  TDF_Reference::Set(NewLab1, RefLab);

  TDF_Label NewLab2 = NewLab.NewChild();
  TDF_Tool::Label(myCurrentOCAFDoc->GetData(), shape2->ShapeId(), RefLab);
  TDF_Reference::Set(NewLab2, RefLab);
  return result ;
}


//================================================================================
// function : MakeAxisStruct()
// purpose  : Create a structure GEOM::AxisStruct (see IDL file)
//================================================================================
GEOM::AxisStruct GEOM_Gen_i::MakeAxisStruct(CORBA::Double x,
				      CORBA::Double y,
				      CORBA::Double z,
				      CORBA::Double vx,
				      CORBA::Double vy,
				      CORBA::Double vz) 
{
  GEOM::AxisStruct A ;
  A.x  = x ;   A.y  = y ;   A.z  = z ;
  A.vx = vx ;  A.vy = vy ;  A.vz = vz ;
  return A ;
}


//================================================================================
// function : MakePointStruct()
// purpose  : Create a structure GEOM::PointStruct (see IDL file)
//================================================================================
GEOM::PointStruct GEOM_Gen_i::MakePointStruct(CORBA::Double x,
					CORBA::Double y,
					CORBA::Double z)
{
  beginService( "GEOM_Gen_i::MakePointStruct" );
  GEOM::PointStruct p ;
  p.x = x ;   p.y = y ;   p.z = z ;
  endService( "GEOM_Gen_i::MakePointStruct" );
  return p ;
}

//================================================================================
// function : MakeDirection()
// purpose  : Create a structure GEOM::DirStruct (see IDL file)
//================================================================================
GEOM::DirStruct GEOM_Gen_i::MakeDirection(const GEOM::PointStruct& p)
{ 
  GEOM::DirStruct d ;
  d.PS.x = p.x ;  d.PS.y = p.y ;  d.PS.z = p.z ;
  return d ;
}

//=================================================================================
// function : MakeBox()
// purpose  : Create a box topology.
//=================================================================================
GEOM::GEOM_Shape_ptr GEOM_Gen_i::MakeBox(CORBA::Double x1,
					 CORBA::Double y1,
					 CORBA::Double z1,
                                         CORBA::Double x2,
                                         CORBA::Double y2,
					 CORBA::Double z2) 
     throw (SALOME::SALOME_Exception)
{
  Unexpect aCatch(SALOME_SalomeException);
  gp_Pnt P1(x1,y1,z1);
  gp_Pnt P2(x2,y2,z2);
  GEOM::GEOM_Shape_var result ;
  TopoDS_Shape tds ;
  try {
    tds = BRepPrimAPI_MakeBox(P1,P2).Shape();
  }
  catch(Standard_Failure) {
    THROW_SALOME_CORBA_EXCEPTION("Exception catched in GEOM_Gen_i::MakeBox", SALOME::BAD_PARAM);
  }

  if (tds.IsNull()) {
    THROW_SALOME_CORBA_EXCEPTION("Make Box aborted : null shape", SALOME::BAD_PARAM);
  } 

  result = CreateObject(tds);
  const char *entry = InsertInLabel(tds, result->Name(), myCurrentOCAFDoc) ;
  result->ShapeId(entry) ;
  return result;  
}


//================================================================================
// function : MakeCylinder
// purpose  : Create a cylinder topology
//================================================================================
GEOM::GEOM_Shape_ptr  GEOM_Gen_i::MakeCylinder(const GEOM::PointStruct& pstruct,
					       const GEOM::DirStruct& dstruct,
					       CORBA::Double radius,
					       CORBA::Double height) 
  throw (SALOME::SALOME_Exception)
{
  Unexpect aCatch(SALOME_SalomeException);
  GEOM::GEOM_Shape_var result;
  TopoDS_Shape tds ;
  gp_Pnt p(pstruct.x, pstruct.y, pstruct.z) ;
  gp_Dir d(dstruct.PS.x, dstruct.PS.y, dstruct.PS.z) ;
  gp_Ax2 axis(p, d) ;

  try {
    tds = BRepPrimAPI_MakeCylinder(axis, radius, height).Shape();
  }
  catch(Standard_Failure) {
    THROW_SALOME_CORBA_EXCEPTION("Exception catched in GEOM_Gen_i::MakeCylinder", SALOME::BAD_PARAM);
  }
  
  if (tds.IsNull()) {
    THROW_SALOME_CORBA_EXCEPTION("Make Cylinder aborted", SALOME::BAD_PARAM);
  } 
  result = CreateObject(tds);
  const char *entry = InsertInLabel(tds, result->Name(), myCurrentOCAFDoc) ;
  result->ShapeId(entry);
  return result ;  
}

//================================================================================
// function : MakeSphere()
// purpose  : Make a sphere topology
//================================================================================
GEOM::GEOM_Shape_ptr GEOM_Gen_i::MakeSphere(CORBA::Double x1,
				      CORBA::Double y1,
				      CORBA::Double z1,
				      CORBA::Double radius) 
  throw (SALOME::SALOME_Exception)
{
  Unexpect aCatch(SALOME_SalomeException);
  GEOM::GEOM_Shape_var result ;
  TopoDS_Shape tds ;
  try {
    tds = BRepPrimAPI_MakeSphere(gp_Pnt(x1,y1,z1), radius).Shape();
  }
  catch(Standard_Failure) {
    THROW_SALOME_CORBA_EXCEPTION("Exception catched in GEOM_Gen_i::MakeSphere", SALOME::BAD_PARAM);
  }
  
  if (tds.IsNull()) {
    THROW_SALOME_CORBA_EXCEPTION("Make Sphere aborted", SALOME::BAD_PARAM);
  } 
  result = CreateObject(tds) ;  
  const char *entry = InsertInLabel(tds, result->Name(), myCurrentOCAFDoc) ;
  result->ShapeId(entry);
  return result;  
}

//================================================================================
// function : MakeTorus()
// purpose  : Create a torus topology
//================================================================================
GEOM::GEOM_Shape_ptr GEOM_Gen_i::MakeTorus( const GEOM::PointStruct& pstruct,
				      const GEOM::DirStruct& dstruct,
				      CORBA::Double major_radius,
				      CORBA::Double minor_radius )
  throw (SALOME::SALOME_Exception)
{
  Unexpect aCatch(SALOME_SalomeException);
  GEOM::GEOM_Shape_var result;
  TopoDS_Shape tds ;
  gp_Pnt p(pstruct.x, pstruct.y, pstruct.z) ;
  gp_Dir d(dstruct.PS.x, dstruct.PS.y, dstruct.PS.z) ;
  gp_Ax2 axis(p, d) ;

  try {
  tds = BRepPrimAPI_MakeTorus(axis, major_radius, minor_radius).Shape();
  }
  catch(Standard_Failure) {
    THROW_SALOME_CORBA_EXCEPTION("Exception catched in GEOM_Gen_i::MakeTorus", SALOME::BAD_PARAM);
  }

  if (tds.IsNull()) {
    THROW_SALOME_CORBA_EXCEPTION("Make torus aborted", SALOME::BAD_PARAM);
  }
  result = CreateObject(tds);
  const char *entry = InsertInLabel(tds, result->Name(), myCurrentOCAFDoc) ;
  result->ShapeId(entry);
  return result ;
}


//================================================================================
// function : MakeCone()
// purpose  : Create a cone topology
//================================================================================
GEOM::GEOM_Shape_ptr GEOM_Gen_i::MakeCone(const GEOM::PointStruct& pstruct,
				    const GEOM::DirStruct& dstruct,
				    CORBA::Double radius1,
				    CORBA::Double radius2,
				    CORBA::Double height)
  throw (SALOME::SALOME_Exception)
{
  Unexpect aCatch(SALOME_SalomeException);
  GEOM::GEOM_Shape_var result;
  TopoDS_Shape tds ;
  gp_Pnt p(pstruct.x, pstruct.y, pstruct.z) ;
  gp_Dir d(dstruct.PS.x, dstruct.PS.y, dstruct.PS.z) ;
  gp_Ax2 axis(p, d) ;
  
  try {
    /* Cone doesn't work if same radius */
    if( fabs(radius1-radius2) <= Precision::Confusion() ) {
      tds = BRepPrimAPI_MakeCylinder(axis, (radius1+radius2)/2.0, height).Shape();
    }
    else {
      tds = BRepPrimAPI_MakeCone(axis, radius1, radius2, height).Shape();
    }
  }
  catch(Standard_Failure) {
    THROW_SALOME_CORBA_EXCEPTION("Exception catched in GEOM_Gen_i::MakeCone", SALOME::BAD_PARAM);
  }

  if (tds.IsNull()) {
    THROW_SALOME_CORBA_EXCEPTION("Make Cone aborted", SALOME::BAD_PARAM);
  }
  result = CreateObject(tds);
  const char *entry = InsertInLabel(tds, result->Name(), myCurrentOCAFDoc) ;
  result->ShapeId(entry);
  return result ;
}



//==================================================================================
// function : ImportIGES()
// purpose  : Import shape from an IGES (IGS) file
//          : LPN modified 7 mai 2002
//==================================================================================
GEOM::GEOM_Shape_ptr GEOM_Gen_i::ImportIGES(const char* filename)
  throw (SALOME::SALOME_Exception)
{
  Unexpect aCatch(SALOME_SalomeException);
  GEOM::GEOM_Shape_var result ;
  //VRV: OCC 4.0 migration
  IGESControl_Reader aReader;  
  //VRV: OCC 4.0 migration
  try {
    IFSelect_ReturnStatus stat = aReader.ReadFile((char*)filename);
    if ( stat != IFSelect_RetDone ) {
      THROW_SALOME_CORBA_EXCEPTION("Error in reading import file", SALOME::BAD_PARAM);    }
    
    MESSAGE("ImportIGES : all Geometry Transfer" << endl ) ;
#if OCC_VERSION_MAJOR >= 5
    aReader.ClearShapes();
    aReader.TransferRoots();
#else
    aReader.Clear();
    aReader.TransferRoots(false);
#endif
    MESSAGE("ImportIGES : count of shapes produced = " << aReader.NbShapes() << endl );    
    TopoDS_Shape shape = aReader.OneShape();

    if ( !shape.IsNull() ) {      
      /* Final CORBA object creation */
      result = CreateObject(shape) ;
      const char *entry = InsertInLabel( shape, result->Name(), myCurrentOCAFDoc ) ;
      result->ShapeId(entry);
      return result ;
    }
  }
  catch(Standard_Failure) {
    THROW_SALOME_CORBA_EXCEPTION("Exception catched in GEOM_Gen_i::ImportIGES()", SALOME::BAD_PARAM);
  } 
  
  THROW_SALOME_CORBA_EXCEPTION("Import IGES aborted : internal error", SALOME::BAD_PARAM);
}



//==================================================================================
// function : ImportSTEP()
// purpose  : Import shape from an STEP (stp) file
//          : 'result' is a compound of shapes if file contains more entities.
//==================================================================================
GEOM::GEOM_Shape_ptr GEOM_Gen_i::ImportSTEP(const char* filename)
  throw (SALOME::SALOME_Exception)
{
  Unexpect aCatch(SALOME_SalomeException);
  GEOM::GEOM_Shape_var result ;
  //VRV: OCC 4.0 migration
  STEPControl_Reader aReader;
  //VRV: OCC 4.0 migration

  TopoDS_Compound compound;
  BRep_Builder B;
  B.MakeCompound( compound );
  
  try {
    IFSelect_ReturnStatus status = aReader.ReadFile((char*)filename);
    
    if (status == IFSelect_RetDone) {
      Standard_Boolean failsonly = Standard_False ;
      aReader.PrintCheckLoad (failsonly, IFSelect_ItemsByEntity);
      /* Root transfers */
      Standard_Integer nbr = aReader.NbRootsForTransfer();
      aReader.PrintCheckTransfer (failsonly, IFSelect_ItemsByEntity);
      
      for ( Standard_Integer n=1; n <= nbr; n++) {
	
	Standard_Boolean ok = aReader.TransferRoot(n);
	/* Collecting resulting entities */
	Standard_Integer nbs = aReader.NbShapes();
	if (nbs == 0)
	  THROW_SALOME_CORBA_EXCEPTION("Exception catched in GEOM_Gen_i::ImportStep", SALOME::BAD_PARAM) ;	
	
	for ( Standard_Integer i=1; i<=nbs; i++ ) {	  
	  TopoDS_Shape aShape = aReader.Shape(i);
	  if ( aShape.IsNull() )
	    THROW_SALOME_CORBA_EXCEPTION("Null shape in GEOM_Gen_i::ImportStep", SALOME::BAD_PARAM) ;	
	  
	  /* For a single entity */
	  if(nbr == 1 && nbs == 1) {	  
	    result = CreateObject(aShape) ;
	    const char *entry = InsertInLabel(aShape, result->Name(), myCurrentOCAFDoc) ;
	    result->ShapeId(entry);
	    return result ;
	  }
	  else {
	    B.Add( compound, aShape ) ;
	  }
	}
      }

     TopoDS_Shape tds = compound ; 
     result = CreateObject(tds) ;
     if( CORBA::is_nil(result) )
       THROW_SALOME_CORBA_EXCEPTION("Translation aborted : null result", SALOME::BAD_PARAM);
     const char *entry = InsertInLabel(tds, result->Name(), myCurrentOCAFDoc) ;
     result->ShapeId(entry);
     return result ;
    }
    
  }
  catch(Standard_Failure) {
    THROW_SALOME_CORBA_EXCEPTION("Exception catched in GEOM_Gen_i::ImportStep", SALOME::BAD_PARAM);
  }  
  return result ;
}


//==================================================================================
// function : Partition()
// purpose  :
//==================================================================================

GEOM::GEOM_Shape_ptr
  GEOM_Gen_i::Partition(const GEOM::GEOM_Gen::ListOfIOR& ListShapes, 
                        const GEOM::GEOM_Gen::ListOfIOR& ListTools,
                        const GEOM::GEOM_Gen::ListOfIOR& ListKeepInside,
                        const GEOM::GEOM_Gen::ListOfIOR& ListRemoveInside,
                        const CORBA::Short               Limit)
throw (SALOME::SALOME_Exception)
{
  Unexpect aCatch(SALOME_SalomeException);
  GEOM::GEOM_Shape_var aResult;  
  TopoDS_Shape tds ;
  //MESSAGE ("In Partition");
  try {

    unsigned int ind, nbshapes = 0;
    nbshapes += ListShapes.length() + ListTools.length();
    nbshapes += ListKeepInside.length() + ListRemoveInside.length();
    
    Partition_Spliter PS;
    TopTools_MapOfShape ShapesMap(nbshapes), ToolsMap(nbshapes);
    
    // add object shapes that are in ListShapes;
    for ( ind = 0; ind < ListShapes.length(); ind++) {
      
      GEOM::GEOM_Shape_var aShape = GetIORFromString( ListShapes[ind] );
      TopoDS_Shape Shape = GetTopoShape(aShape);
      if(Shape.IsNull() ) {
        //MESSAGE ( "In Partition a shape is null" );
	THROW_SALOME_CORBA_EXCEPTION("In Partition a shape is null", SALOME::BAD_PARAM);
      }
      if ( ShapesMap.Add( Shape ))
        PS.AddShape(Shape);
    }
    
    // add tool shapes that are in ListTools and not in ListShapes;
    for (ind = 0; ind < ListTools.length(); ind++) {
      
      GEOM::GEOM_Shape_var aShape = GetIORFromString( ListTools[ind] );
      TopoDS_Shape Shape = GetTopoShape(aShape);
      if(Shape.IsNull() ) {
	THROW_SALOME_CORBA_EXCEPTION("In Partition a shape is null", SALOME::BAD_PARAM);
      }
      if ( !ShapesMap.Contains( Shape ) && ToolsMap.Add( Shape ))
        PS.AddTool(Shape);
    }
    
    // add shapes that are in ListKeepInside, as object shapes;
    for (ind = 0; ind < ListKeepInside.length(); ind++) {
      
      GEOM::GEOM_Shape_var aShape = GetIORFromString( ListKeepInside[ind] );
      TopoDS_Shape Shape = GetTopoShape(aShape);
      if(Shape.IsNull() ) {
        //MESSAGE ( "In Partition a Keep Inside shape is null" );
	THROW_SALOME_CORBA_EXCEPTION("In Partition a shape is null", SALOME::BAD_PARAM);
      }
      if (!ToolsMap.Contains( Shape ) &&
          ShapesMap.Add( Shape ))
        PS.AddShape(Shape);
    }
    
    // add shapes that are in ListRemoveInside, as object shapes;
    for (ind = 0; ind < ListRemoveInside.length(); ind++) {
      
      GEOM::GEOM_Shape_var aShape = GetIORFromString( ListRemoveInside[ind] );
      TopoDS_Shape Shape = GetTopoShape(aShape);
      if(Shape.IsNull() ) {
        //MESSAGE ( "In Partition a Remove Inside shape is null" );
	THROW_SALOME_CORBA_EXCEPTION("In Partition a shape is null", SALOME::BAD_PARAM);
      }
      if (!ToolsMap.Contains( Shape ) &&
          ShapesMap.Add( Shape ) )
        PS.AddShape(Shape);
    }
    
    //MESSAGE ( "Partition::Compute() " );
    PS.Compute ((TopAbs_ShapeEnum) Limit);
    //MESSAGE ( "Partition::Compute() - END" );

    // suppress result outside of shapes in KInsideMap
    for (ind = 0; ind < ListKeepInside.length(); ind++) {
      GEOM::GEOM_Shape_var aShape = GetIORFromString( ListKeepInside[ind] );
      TopoDS_Shape Shape = GetTopoShape(aShape);
      PS.KeepShapesInside( Shape );
    }
    
    // suppress result inside of shapes in RInsideMap
    for (ind = 0; ind < ListRemoveInside.length(); ind++) {
      
      GEOM::GEOM_Shape_var aShape = GetIORFromString( ListRemoveInside[ind] );
      TopoDS_Shape Shape = GetTopoShape(aShape);
      PS.RemoveShapesInside( Shape );
    }
    
    tds = PS.Shape();
    
    if( !IsValid(tds) ) {
      //MESSAGE ( "In Partition: non valid shape result" );
      THROW_SALOME_CORBA_EXCEPTION("Partition aborted : non valid shape result", SALOME::BAD_PARAM);
    }
  }
  catch (Standard_Failure) {
    //MESSAGE ( "In Partition: Exception catched in GEOM_Gen_i::Partition()" );
    THROW_SALOME_CORBA_EXCEPTION("Exception catched in GEOM_Gen_i::Partition", SALOME::BAD_PARAM);
  }
  
  aResult = CreateObject(tds) ;

  /* add attributs S and mystr in a new label */
  GEOMDS_Commands GC(myCurrentOCAFDoc->Main());
  TDF_Label Lab = GC.AddShape(tds, aResult->Name() );
  TCollection_AsciiString entry;
  TDF_Tool::Entry(Lab,entry);
  aResult->ShapeId( entry.ToCString() ) ;
  
  // add arguments
  
  /* Create a new label */
  TDF_Label NewLab = Lab.NewChild();
  TCollection_ExtendedString Value("Arguments");
  TDataStd_Name::Set(NewLab,Value);

  // object shapes
  for (unsigned int ind = 0; ind < ListShapes.length(); ind++) {
    TDF_Label NewLab1 = NewLab.NewChild();
    
    GEOM::GEOM_Shape_var aShape = GetIORFromString( ListShapes[ind] );
    Standard_CString anEntry = aShape->ShapeId();
    TDF_Label RefLab;
    TDF_Tool::Label(myCurrentOCAFDoc->GetData(), anEntry, RefLab);
    TDF_Reference::Set(NewLab1,RefLab);
  }
  // tool shapes
  for (unsigned int ind = 0; ind < ListTools.length(); ind++) {
    TDF_Label NewLab1 = NewLab.NewChild();
    
    GEOM::GEOM_Shape_var aShape = GetIORFromString( ListTools[ind] );
    Standard_CString anEntry = aShape->ShapeId();
    TDF_Label RefLab;
    TDF_Tool::Label(myCurrentOCAFDoc->GetData(), anEntry, RefLab);
    TDF_Reference::Set(NewLab1,RefLab);
  }
  // limit shapes 1
  for (unsigned int ind = 0; ind < ListKeepInside.length(); ind++) {
    TDF_Label NewLab1 = NewLab.NewChild();
    
    GEOM::GEOM_Shape_var aShape = GetIORFromString( ListKeepInside[ind] );
    Standard_CString anEntry = aShape->ShapeId();
    TDF_Label RefLab;
    TDF_Tool::Label(myCurrentOCAFDoc->GetData(), anEntry, RefLab);
    TDF_Reference::Set(NewLab1,RefLab);
  }
  // limit shapes 2
  for (unsigned int ind = 0; ind < ListRemoveInside.length(); ind++) {
    TDF_Label NewLab1 = NewLab.NewChild();
    
    GEOM::GEOM_Shape_var aShape = GetIORFromString( ListRemoveInside[ind] );
    Standard_CString anEntry = aShape->ShapeId();
    TDF_Label RefLab;
    TDF_Tool::Label(myCurrentOCAFDoc->GetData(), anEntry, RefLab);
    TDF_Reference::Set(NewLab1,RefLab);
  }
  
  return aResult;    
}



//==================================================================================
// function : MakeFilling()
// purpose  : Create a surface from section curves filling
//==================================================================================
GEOM::GEOM_Shape_ptr GEOM_Gen_i::MakeFilling(GEOM::GEOM_Shape_ptr myShape,
				       CORBA::Short mindeg,
				       CORBA::Short maxdeg,
				       CORBA::Double tol3d,
				       CORBA::Double tol2d,
				       CORBA::Short nbiter)
  throw (SALOME::SALOME_Exception)
{
  Unexpect aCatch(SALOME_SalomeException);
  GEOM::GEOM_Shape_var result ;
  TopoDS_Face tds ;
  TopoDS_Shape aShape = GetTopoShape(myShape) ;
  if( aShape.IsNull() || aShape.ShapeType() != TopAbs_COMPOUND ) {
    THROW_SALOME_CORBA_EXCEPTION("MakeFilling aborted : null shape or not a compound", SALOME::BAD_PARAM);
  } 

  try {
    /* we verify the contents of the shape */
    TopExp_Explorer Ex ;
    TopoDS_Shape Scurrent ;	
    Standard_Real First, Last ;
    Handle(Geom_Curve) C ;
    GeomFill_SectionGenerator Section ;
    
    Standard_Integer i = 0 ;
    for(Ex.Init(aShape, TopAbs_EDGE); Ex.More(); Ex.Next()) {
      Scurrent = Ex.Current() ;
      if( Scurrent.IsNull() || Scurrent.ShapeType() != TopAbs_EDGE)  {
	THROW_SALOME_CORBA_EXCEPTION("Initial shape doesn't contain only edges !", SALOME::BAD_PARAM);
      }
      C = BRep_Tool::Curve(TopoDS::Edge(Scurrent), First, Last);
      if (C.IsNull()) continue;
      C = new Geom_TrimmedCurve(C, First, Last);
      Section.AddCurve(C) ;
      i++ ;
    }
    
    /* a 'tolerance' is used to compare 2 knots : see GeomFill_Generator.cdl */
    /* We set 'tolerance' = tol3d                                            */
    // Section.Perform( tol3d ) ; NRI */
    Section.Perform( Precision::Confusion() ) ;
    Handle(GeomFill_Line) Line = new GeomFill_Line(i) ;
    
    GeomFill_AppSurf App(mindeg, maxdeg, tol3d, tol2d, nbiter) ; /* user parameters */
    App.Perform(Line, Section) ;
    
    if (!App.IsDone()) {
      THROW_SALOME_CORBA_EXCEPTION("Filling aborted : non valid shape result", SALOME::BAD_PARAM);
    }
    Standard_Integer UDegree, VDegree, NbUPoles, NbVPoles, NbUKnots, NbVKnots;
    App.SurfShape(UDegree, VDegree, NbUPoles, NbVPoles, NbUKnots, NbVKnots);	
    Handle(Geom_BSplineSurface) GBS = new Geom_BSplineSurface(App.SurfPoles(),
							      App.SurfWeights(),
							      App.SurfUKnots(),
							      App.SurfVKnots(),
							      App.SurfUMults(),
							      App.SurfVMults(),
							      App.UDegree(),
							      App.VDegree());
    
    if( GBS.IsNull() )  {
      THROW_SALOME_CORBA_EXCEPTION("Make Filling aborted", SALOME::BAD_PARAM);
    }
    tds  = BRepBuilderAPI_MakeFace(GBS) ;    
  }
  catch(Standard_Failure) {
    THROW_SALOME_CORBA_EXCEPTION("Exception catched in GEOM_Gen_i::MakeFilling", SALOME::BAD_PARAM);
  } 
  
  /* We test the validity of resulting shape */
  if( !IsValid(tds) ) {
    THROW_SALOME_CORBA_EXCEPTION("Filling aborted : non valid shape result", SALOME::BAD_PARAM);
  } 
  else {
    result = CreateObject(tds) ;
    InsertInLabelOneArgument(aShape, myShape, tds, result, myCurrentOCAFDoc) ;
  }
 
  return result ;
}


//=================================================================================
// function : MakeGlueFaces()
// purpose  :
//=================================================================================

TopoDS_Face GEOM_Gen_i::FindSameFace(const TopoDS_Shape& aShape, 
				     const TopoDS_Face& F,
				     double tol3d)
{
  TopoDS_Face aFace;
  bool isSame = false;
  for (TopExp_Explorer exf(aShape,TopAbs_FACE); exf.More(); exf.Next())
    {
      //MESSAGE("--- test a face");
      int nbFound = 0;
      aFace = TopoDS::Face(exf.Current());
      TopTools_ListOfShape liste1;
      TopTools_ListOfShape liste2;
      for (TopExp_Explorer exp(aFace,TopAbs_VERTEX); exp.More(); exp.Next())
	{
	  const TopoDS_Vertex& V = TopoDS::Vertex(exp.Current());
	  liste1.Append(V);
	}
      for (TopExp_Explorer exp(F,TopAbs_VERTEX); exp.More(); exp.Next())
	{
	  const TopoDS_Vertex& V = TopoDS::Vertex(exp.Current());
	  liste2.Append(V);
	}
      isSame = false;
      if (liste1.Extent() == liste2.Extent())
	{
	  TopTools_ListIteratorOfListOfShape it1(liste1);
	  isSame = true;
	  for (; it1.More(); it1.Next())
	    {
	      bool foundSamePoint = false;
	      gp_Pnt P1 = BRep_Tool::Pnt(TopoDS::Vertex(it1.Value()));
	      TopTools_ListIteratorOfListOfShape it2(liste2);
	      for (it2; it2.More(); it2.Next())
		{
		  gp_Pnt P2 = BRep_Tool::Pnt(TopoDS::Vertex(it2.Value()));
		  double d = P1.Distance(P2);
		  if (d < tol3d)
		    {
		      nbFound++;
		      //MESSAGE("    found Same Point : "<<nbFound<<" - "<<P1.X()<<" "<<P1.Y()<<" "<<P1.Z());
		      foundSamePoint = true;
		      break;
		    }
		}
	      isSame = isSame && foundSamePoint;
	      if (! isSame) break; // a vertex does not correspond : not same face
	    }
	}
      if (isSame)
	{
	  //MESSAGE("    --- Found Same Face");
	  break; // a face corresponding to F is found
	}
    }
  if (! isSame) aFace.Nullify(); // return null face
  return aFace;
}

TopoDS_Edge GEOM_Gen_i::FindSameEdge(const TopoDS_Face& nf, 
				     TopoDS_Edge& Eold,
				     double tol3d)
{
  TopoDS_Face newFace = TopoDS::Face(nf.Oriented(TopAbs_REVERSED));
  TopoDS_Vertex VFirst, VLast;
  TopExp::Vertices(Eold, VFirst, VLast);
  gp_Pnt Pf = BRep_Tool::Pnt(VFirst);
  gp_Pnt Pl = BRep_Tool::Pnt(VLast);
  TopoDS_Edge Enew;
  for (TopExp_Explorer ee(newFace,TopAbs_EDGE); ee.More(); ee.Next())
    {
      const TopoDS_Edge& E = TopoDS::Edge(ee.Current());
      TopoDS_Vertex VFn, VLn;
      TopExp::Vertices(E, VFn, VLn);
      gp_Pnt Pfn = BRep_Tool::Pnt(VFn);
      gp_Pnt Pln = BRep_Tool::Pnt(VLn);
      double dff = Pf.Distance(Pfn);
      double dfl = Pf.Distance(Pln);
      double dlf = Pl.Distance(Pfn);
      double dll = Pl.Distance(Pln);
      if ((dff < tol3d) && (dll <tol3d))
	{
	  //MESSAGE("--- edge forward " <<Pf.X()<<" "<<Pf.Y()<<" "<<Pf.Z()<<" "<<Pl.X()<<" "<<Pl.Y()<<" "<<Pl.Z());
	  Enew = TopoDS::Edge(E.Oriented(TopAbs_FORWARD));
 	  Eold = TopoDS::Edge(Eold.Oriented(TopAbs_FORWARD));
	  break;
	} 
      if ((dfl < tol3d) && (dlf <tol3d))
	{
	  //MESSAGE("--- edge reversed " <<Pf.X()<<" "<<Pf.Y()<<" "<<Pf.Z()<<" "<<Pl.X()<<" "<<Pl.Y()<<" "<<Pl.Z());
	  Enew = TopoDS::Edge(E.Oriented(TopAbs_REVERSED));
 	  Eold = TopoDS::Edge(Eold.Oriented(TopAbs_FORWARD));
	  break;
	} 
    }
  return Enew;
}

GEOM::GEOM_Shape_ptr GEOM_Gen_i::MakeGlueFaces(GEOM::GEOM_Shape_ptr myShape,
					       double tol3d)
  throw (SALOME::SALOME_Exception)
{

  // prendre un premier shell dans la liste des shells
  // initialiser un compshell avec ce shell
  // tant qu'il reste des shells dans la liste
  //   chercher un shell qui a des faces en  commun avec le compshell
  //   creer un BRepTools_Quilt
  //   recenser les faces communes issues du compshell, les ajouter au quilt 
  //   recenser les faces restantes du shell a inclure, les ajouter au quilt
  //   recenser les edges en double, a remplacer
  //   pour chaque paire d'edge
  //     tester l'orientation relative des aretes
  //     bind dans le quilt de Eold.Forward et Enew.Forward (ou reverse)
  //   recuperer le nouveau shell
  // l'incorporer dans le compshell
  // appliquer BRepTools_SameParameter au compshell
  // (rendre parametres 2D des edges identiques aux parametres 3D)

  Unexpect aCatch(SALOME_SalomeException);
  GEOM::GEOM_Shape_var result ;
  TopoDS_Shape tds ;
  TopoDS_Shape aShape = GetTopoShape(myShape) ;
  TopoDS_Compound C;
  BRep_Builder bu;
  bu.MakeCompound(C); // empty compound;
  TopTools_ListOfShape shellList;
  for (TopExp_Explorer exp(aShape,TopAbs_SHELL); exp.More(); exp.Next())
    {
      const TopoDS_Shell& S = TopoDS::Shell(exp.Current());
      shellList.Append(S);
    }
  TopTools_ListIteratorOfListOfShape its(shellList);
  if ( ! its.More())
    {
      THROW_SALOME_CORBA_EXCEPTION("glue aborted : no shell in shape", SALOME::BAD_PARAM);
    }
  TopoDS_Shell S = TopoDS::Shell(its.Value());
  bu.Add(C, S); // add first shell to compound
  shellList.Remove(its);
  its.Initialize(shellList);
  bool shellAdded = true;
  while ((shellList.Extent() > 0) && shellAdded)
    {
      //MESSAGE("more shells : "<< shellList.Extent());
      shellAdded = false;
      its.Initialize(shellList);
      for(; its.More(); its.Next())
	{
	  //MESSAGE("one more shell to try");
	  TopTools_ListOfShape newFaces; // common faces from new compound
	  TopTools_ListOfShape oldFaces; // common faces from shell to add
	  TopTools_ListOfShape addFaces; // not common faces from shell to add
	  TopTools_ListOfShape newEdges; // common edges from new compound
	  TopTools_ListOfShape oldEdges; // common edges from face to add
	  TopoDS_Compound CFN;
	  TopoDS_Compound CFO;
	  bu.MakeCompound(CFN);       // empty compound for new faces
	  bu.MakeCompound(CFO);       // empty compound for old faces
	  S = TopoDS::Shell(its.Value());
	  for (TopExp_Explorer exp(S,TopAbs_FACE); exp.More(); exp.Next())
	    {
	      //MESSAGE("--- try to find corresponding face in new compound");
	      TopoDS_Face F = TopoDS::Face(exp.Current());
	      TopoDS_Face newFace = FindSameFace(C,F,tol3d);
	      if (! newFace.IsNull())
		{
		  //MESSAGE("--- face found");
		  newFaces.Append(newFace); 
		  bu.Add(CFN, newFace); // common faces from new compound
		  oldFaces.Append(F);
		  for (TopExp_Explorer ee(F,TopAbs_EDGE);ee.More();ee.Next())
		    {
		      //MESSAGE("--- find edge pair");
		      TopoDS_Edge Eold = TopoDS::Edge(ee.Current());
		      const TopoDS_Edge& Enew = FindSameEdge(newFace, Eold, tol3d);
		      oldEdges.Append(Eold);
		      newEdges.Append(Enew);
		    }
		}
	      else
		{
		  //MESSAGE("---");
		  addFaces.Append(F);
		  bu.Add(CFO, F); // not common faces from shell to add
		}
	    }
	  if ( !newFaces.IsEmpty())
	    {
	      //MESSAGE("--- some faces found ---");
	      shellAdded = true;
	      BRepTools_Quilt glue;
	      glue.Add(CFN);
	      TopTools_ListIteratorOfListOfShape ito(oldEdges);
	      TopTools_ListIteratorOfListOfShape itn(newEdges);
	      for (; ito.More(); ito.Next())
		{		  
		  //MESSAGE("--- bind");
		  glue.Bind(TopoDS::Edge(ito.Value()), TopoDS::Edge(itn.Value()));
		  itn.Next();
		}
	      glue.Add(CFO);
	      TopoDS_Compound newc = TopoDS::Compound(glue.Shells());
	      for (TopExp_Explorer exs(newc,TopAbs_SHELL); exs.More(); exs.Next())
		{
		  TopoDS_Shell NS = TopoDS::Shell(exs.Current());
		  bu.Add(C, NS);
		}
	      shellList.Remove(its);
	      //MESSAGE("--- remove shell from list");
	      break;
	    }
	}
    }
  //MESSAGE("---" << shellList.Extent() << " " << shellAdded);

  TopExp_Explorer  exp(C,TopAbs_SHELL);
  Standard_Integer ish=0;
  TopoDS_Compound  Res;
  TopoDS_Solid     Sol;
  BRep_Builder     B;
  B.MakeCompound(Res);
  TopoDS_Shape theShape;

  for (; exp.More(); exp.Next())
    {
      TopoDS_Shape Sh = exp.Current();
      B.MakeSolid(Sol);
      B.Add(Sol,Sh);
      BRepClass3d_SolidClassifier SC(Sol);
      SC.PerformInfinitePoint(1.E-6); // cf. BRepFill_Confusion() - BRepFill_Evolved.cxx
      if (SC.State() == TopAbs_IN)
	{
	  B.MakeSolid(Sol);
	  B.Add(Sol,Sh.Reversed());
	}
      B.Add(Res,Sol);
      ish++;
    }
  if (ish == 1) { theShape = Sol;}
  else          { theShape = Res;}

  BRepLib::SameParameter(theShape, 1.E-5, Standard_True);
  tds = theShape;
  result = CreateObject(tds);    
  InsertInLabelOneArgument(aShape, myShape, tds, result, myCurrentOCAFDoc) ;
  //MESSAGE("---");
  return result;
}

//=================================================================================
// function : MakeSewing()
// purpose  :
//=================================================================================
GEOM::GEOM_Shape_ptr GEOM_Gen_i::MakeSewing( const GEOM::GEOM_Gen::ListOfIOR& ListShapes,
				       CORBA::Double precision )
  throw (SALOME::SALOME_Exception)
{
  Unexpect aCatch(SALOME_SalomeException);
  GEOM::GEOM_Shape_var result ;
  TopoDS_Shape tds ;
  BRepOffsetAPI_Sewing aMethod ;

  try {
    /* default OCC is 1.0e-06 */
    aMethod.Init(precision, Standard_False);  
    for ( unsigned int i = 0; i < ListShapes.length(); i++) {
      GEOM::GEOM_Shape_var aShape = GetIORFromString( ListShapes[i] ); 
      TopoDS_Shape Shape = GetTopoShape(aShape) ;
      if( Shape.IsNull() ) {
	THROW_SALOME_CORBA_EXCEPTION("MakeSewing aborted : null shape during operation", SALOME::BAD_PARAM);
      } 
      aMethod.Add(Shape) ;  
    }  
    
    aMethod.Perform() ;
    tds = aMethod.SewedShape() ;
    if( !IsValid(tds) ) {
      THROW_SALOME_CORBA_EXCEPTION("Make Sewing aborted : non valid shape", SALOME::BAD_PARAM);
    }
    if( tds.IsNull() ) {
      THROW_SALOME_CORBA_EXCEPTION("Make Sewing aborted : null shape", SALOME::BAD_PARAM);
    }
  }
  catch (Standard_Failure) {
    THROW_SALOME_CORBA_EXCEPTION("Exception catched in GEOM_Gen_i::MakeSewing", SALOME::BAD_PARAM);
  }
  
  result = CreateObject(tds);
  InsertInLabelMoreArguments(tds, result, ListShapes, myCurrentOCAFDoc) ;
  return result;
}

GEOM::GEOM_Shape_ptr GEOM_Gen_i::MakeSewingShape( GEOM::GEOM_Shape_ptr aShape,
					    CORBA::Double precision )
  throw (SALOME::SALOME_Exception)
{
  Unexpect aCatch(SALOME_SalomeException);
  GEOM::GEOM_Shape_var result ;
  TopoDS_Shape tds, S ;
  BRepOffsetAPI_Sewing aMethod ;

  try {
    S = GetTopoShape(aShape) ;
    if(S.IsNull() ) {
      THROW_SALOME_CORBA_EXCEPTION("In Sewing a Shape is null", SALOME::BAD_PARAM);
    }

    /* default OCC is 1.0e-06 */
    aMethod.Init(precision, Standard_False);  
    for ( TopExp_Explorer exp( S, TopAbs_FACE); exp.More(); exp.Next() ) {
      const TopoDS_Face& F = TopoDS::Face(exp.Current());
      aMethod.Add(F) ;  
    }  
    
    aMethod.Perform() ;
    tds = aMethod.SewedShape() ;
    if( !IsValid(tds) ) {
      THROW_SALOME_CORBA_EXCEPTION("Make Sewing aborted : non valid shape", SALOME::BAD_PARAM);
    }
  }
  catch (Standard_Failure) {
    THROW_SALOME_CORBA_EXCEPTION("Exception catched in GEOM_Gen_i::MakeSewing", SALOME::BAD_PARAM);
  }
  
  result = CreateObject(tds);
  const char *entry = InsertInLabel(tds, result->Name(), myCurrentOCAFDoc) ;
  result->ShapeId(entry) ;
  return result;
}

//==================================================================================
// function : OrientationChange()
// purpose  : Change the orientation of a new shape
//          : TopAbs_FORWARD < -- > TopAbs_REVERSED
//
//          : WARNING : for the moment we make a new shape !
//==================================================================================
GEOM::GEOM_Shape_ptr GEOM_Gen_i::OrientationChange(GEOM::GEOM_Shape_ptr aShape)
  throw (SALOME::SALOME_Exception)
{
  Unexpect aCatch(SALOME_SalomeException);
  GEOM::GEOM_Shape_var result ;  
  BRep_Builder aBuilder;  

  TopoDS_Shape shape = GetTopoShape(aShape) ;
  if( shape.IsNull() ) {
    THROW_SALOME_CORBA_EXCEPTION("Shape is null", SALOME::BAD_PARAM);
  }

  BRepBuilderAPI_Copy Copy(shape);
  if( Copy.IsDone() ) {
    TopoDS_Shape tds = Copy.Shape();
    if( tds.IsNull() ) {
      THROW_SALOME_CORBA_EXCEPTION("Orientation aborted : null shape", SALOME::BAD_PARAM);
    } 
  
    if( tds.Orientation() == TopAbs_FORWARD)
      tds.Orientation(TopAbs_REVERSED) ;
    else
      tds.Orientation(TopAbs_FORWARD) ;
    
    result = CreateObject(tds); 
    InsertInLabelOneArgument(shape, aShape, tds, result, myCurrentOCAFDoc) ;
  }
  return result ;
}


//==================================================================================
// function : GetReferencedObjects()
// purpose  :
//==================================================================================
GEOM::GEOM_Gen::ListOfIOR* GEOM_Gen_i::GetReferencedObjects(GEOM::GEOM_Shape_ptr shape)
{
  GEOM::GEOM_Gen::ListOfIOR_var aList = new GEOM::GEOM_Gen::ListOfIOR;
  aList->length(0);

  if (shape->_is_nil()) return aList._retn();

  Standard_CString entry = shape->ShapeId();
  TDF_Label Lab;
  TDF_Tool::Label(myCurrentOCAFDoc->GetData(), entry, Lab);

  Handle(TDataStd_Name) Att;
  Lab.FindAttribute(TDataStd_Name::GetID(),Att);

  TDF_ChildIterator ChildIterator(Lab);
  if (ChildIterator.More()) {
    TDF_Label L = ChildIterator.Value();
    Handle(TDataStd_Name) Att;
    L.FindAttribute(TDataStd_Name::GetID(),Att);
    if (Att->Get().IsEqual(TCollection_ExtendedString("Arguments")) ) {

      TDF_ChildIterator ChildIterator1(L);
      unsigned int i = 0;

      while (ChildIterator1.More()) {
	TDF_Label L = ChildIterator1.Value();

	Handle(TDF_Reference) Ref;
	if (L.FindAttribute(TDF_Reference::GetID(),Ref)) {
	  i++;
	}
	ChildIterator1.Next();
      }
      aList->length(i);
      i = 0;
      TDF_ChildIterator ChildIterator2(L);
      while (ChildIterator2.More()) {    
	TDF_Label L = ChildIterator2.Value();
	Handle(TDF_Reference) Ref;
	if (L.FindAttribute(TDF_Reference::GetID(),Ref)) {
	  TDF_Label L = Ref->Get();
	  
	  Handle(TDataStd_Name) Att;
	  L.FindAttribute(TDataStd_Name::GetID(),Att);
	  TCollection_AsciiString nameIOR (Att->Get()) ;
	  aList[i] = CORBA::string_dup( nameIOR.ToCString() );
	  i++;
	}
   
	ChildIterator2.Next();
      }
    }
  }
  return aList._retn();
}

//==================================================================================
// function : GetObjects()
// purpose  :
//==================================================================================
GEOM::GEOM_Gen::ListOfIOR* GEOM_Gen_i::GetObjects(GEOM::GEOM_Shape_ptr shape)
{
  GEOM::GEOM_Gen::ListOfIOR_var aList = new GEOM::GEOM_Gen::ListOfIOR;
  aList->length(0);
  
  Standard_CString entry = shape->ShapeId();
  TDF_Label Lab;
  TDF_Tool::Label(myCurrentOCAFDoc->GetData(), entry, Lab);
  
  Handle(TDataStd_Name) Att;
  Lab.FindAttribute(TDataStd_Name::GetID(),Att);

  TDF_ChildIterator ChildIterator(Lab);
  unsigned int i = 0;
  while (ChildIterator.More()) {
    TDF_Label L = ChildIterator.Value();
    Handle(TDataStd_Name) Att;
    L.FindAttribute(TDataStd_Name::GetID(),Att);

    if (!Att->Get().IsEqual(TCollection_ExtendedString("Arguments")) ) {
      i++;
    }
    ChildIterator.Next();
  }

  aList->length(i);
  i = 0;
  TDF_ChildIterator ChildIterator1(Lab);
  while (ChildIterator1.More()) {
    TDF_Label L = ChildIterator1.Value();
    Handle(TDataStd_Name) Att;
    L.FindAttribute(TDataStd_Name::GetID(),Att);

    if (!Att->Get().IsEqual(TCollection_ExtendedString("Arguments")) ) {
      TCollection_AsciiString nameIOR (Att->Get());
      aList[i] = CORBA::string_dup( nameIOR.ToCString() );
      i++;
    }
    ChildIterator1.Next();
  }
  return aList._retn();
}


//==================================================================================
// function : Import
// purpose  : Import shape from a BREP file
//==================================================================================
GEOM::GEOM_Shape_ptr GEOM_Gen_i::ImportBREP(const char* filename)
  throw (SALOME::SALOME_Exception)
{
  Unexpect aCatch(SALOME_SalomeException);
  TopoDS_Shape tds ;
  GEOM::GEOM_Shape_var result ;
  
  try {
    BRep_Builder aBuilder;
    char* aCopyfilename = strdup(filename);
    BRepTools::Read(tds, aCopyfilename, aBuilder) ;
    free(aCopyfilename);
    if (tds.IsNull()) {
      THROW_SALOME_CORBA_EXCEPTION("Import BRep aborted", SALOME::BAD_PARAM);
    } 
  }
  catch(Standard_Failure) {
    THROW_SALOME_CORBA_EXCEPTION("Exception catched in GEOM_Gen_i::ImportBREP", SALOME::BAD_PARAM);
  }

  result = CreateObject(tds) ;
  const char *entry = InsertInLabel(tds, result->Name(), myCurrentOCAFDoc) ;
  result->ShapeId(entry);
  return result; 
}


//================================================================================
// function : MakePlane()
// purpose  : Make a plane topology (non infinite)
//================================================================================
GEOM::GEOM_Shape_ptr GEOM_Gen_i::MakePlane(const GEOM::PointStruct& pstruct,
				     const GEOM::DirStruct& dstruct,
				     CORBA::Double trimsize) 
  throw (SALOME::SALOME_Exception)
{
  Unexpect aCatch(SALOME_SalomeException);
  GEOM::GEOM_Shape_var result ;
  TopoDS_Shape tds ;

  try {
    gp_Pnt aPoint(pstruct.x, pstruct.y, pstruct.z) ;
    gp_Dir aDirection(dstruct.PS.x, dstruct.PS.y, dstruct.PS.z) ;
    /*  we make a trimmed plane */
    gp_Pln gplane(aPoint, aDirection) ;    
    tds = BRepBuilderAPI_MakeFace(gplane, -trimsize, +trimsize, -trimsize, +trimsize) ;
  }
  catch(Standard_Failure) {
    THROW_SALOME_CORBA_EXCEPTION("Exception catched in GEOM_Gen_i::MakePlane", SALOME::BAD_PARAM);
  }

  if (tds.IsNull()) {
    THROW_SALOME_CORBA_EXCEPTION("Make Plane aborted : null shape", SALOME::BAD_PARAM);
   } 
  
  result = CreateObject(tds) ;
  const char *entry = InsertInLabel(tds, result->Name(), myCurrentOCAFDoc) ;
  result->ShapeId(entry);
  return result ;
}

//=================================================================================
// function : MakeVertex()
// purpose  : Create a Vertex topology.
//=================================================================================
GEOM::GEOM_Shape_ptr GEOM_Gen_i::MakeVertex(CORBA::Double x,
				      CORBA::Double y,
				      CORBA::Double z) 
  throw (SALOME::SALOME_Exception)
{
  Unexpect aCatch(SALOME_SalomeException);
  GEOM::GEOM_Shape_var result ; 
  gp_Pnt P(x,y,z);
  TopoDS_Shape tds = BRepBuilderAPI_MakeVertex(P).Shape();
  if (tds.IsNull()) {
    THROW_SALOME_CORBA_EXCEPTION("Make Vertex/Point aborted", SALOME::BAD_PARAM);
  }
  tds.Infinite(true);
  result = CreateObject(tds) ;
  const char *entry = InsertInLabel(tds, result->Name(), myCurrentOCAFDoc) ;
  result->ShapeId(entry);
  return result ;
}


//=================================================================================
// function : MakeFace()
// purpose  : 
//=================================================================================
GEOM::GEOM_Shape_ptr GEOM_Gen_i::MakeFace( GEOM::GEOM_Shape_ptr wire,
				     CORBA::Boolean wantplanarface ) 
  throw (SALOME::SALOME_Exception) 
{
  Unexpect aCatch(SALOME_SalomeException);
  GEOM::GEOM_Shape_var result ;
  TopoDS_Shape aShape;
  TopoDS_Shape tds;

  try {
    aShape = GetTopoShape(wire) ;
    if( aShape.IsNull() || aShape.ShapeType() != TopAbs_WIRE ) {
      THROW_SALOME_CORBA_EXCEPTION("MakeFace aborted : null or inappropriate shape", SALOME::BAD_PARAM);
    }
    TopoDS_Wire W = TopoDS::Wire(aShape) ;
    tds = BRepBuilderAPI_MakeFace(W, wantplanarface).Shape() ;
    if( !tds.IsNull() ) {
      result = CreateObject(tds) ;
      InsertInLabelOneArgument(aShape, wire, tds, result, myCurrentOCAFDoc) ;
    }
    else {
      THROW_SALOME_CORBA_EXCEPTION("Null result in GEOM_Gen_i::MakeFace", SALOME::BAD_PARAM);
    }
  }
  catch (Standard_Failure) {
    THROW_SALOME_CORBA_EXCEPTION("Exception catched in GEOM_Gen_i::MakeFace", SALOME::BAD_PARAM);
  }  
  return result ;
}


//=================================================================================
// function : MakeFaces()
// purpose  : 
//=================================================================================
GEOM::GEOM_Shape_ptr GEOM_Gen_i::MakeFaces(const GEOM::GEOM_Gen::ListOfIOR& ListShapes,
					  CORBA::Boolean wantplanarface) 
  throw (SALOME::SALOME_Exception) 
{
  GEOM::GEOM_Shape_var result;

  try {
    GEOM::GEOM_Shape_var aShape = GetIORFromString(ListShapes[0]);    
    TopoDS_Shape Shape = GetTopoShape(aShape);
    if(Shape.IsNull() || Shape.ShapeType() != TopAbs_WIRE) {
      THROW_SALOME_CORBA_EXCEPTION("Shell aborted : null shape during operation", SALOME::BAD_PARAM);
    }
    TopoDS_Wire W = TopoDS::Wire(Shape);
    TopoDS_Shape FFace = BRepBuilderAPI_MakeFace(W, wantplanarface).Shape();
    if(!FFace.IsNull()) {
      if(ListShapes.length() == 1) {
	result = CreateObject(FFace);
	InsertInLabelMoreArguments(FFace, result, ListShapes, myCurrentOCAFDoc);
      }
      else if(ListShapes.length() >= 2) {
	TopoDS_Compound C;
	BRep_Builder aBuilder;
	aBuilder.MakeCompound(C);
	BRepAlgo_FaceRestrictor FR;

	TopAbs_Orientation OriF = FFace.Orientation();
	TopoDS_Shape aLocalS = FFace.Oriented(TopAbs_FORWARD);
	FR.Init(TopoDS::Face(aLocalS), Standard_False, Standard_True);

	for(unsigned int i = 0; i < ListShapes.length(); i++) {
	  GEOM::GEOM_Shape_var aShape = GetIORFromString(ListShapes[i]);    
	  TopoDS_Shape Shape = GetTopoShape(aShape);
	  if(Shape.IsNull()) {
	    THROW_SALOME_CORBA_EXCEPTION("Shell aborted : null shape during operation", SALOME::BAD_PARAM);
	  }
	  FR.Add(TopoDS::Wire(Shape));
	}

	FR.Perform();
    
	if(FR.IsDone()) {
	  int k = 0;
	  TopoDS_Shape aFace;
	  for(; FR.More(); FR.Next()) {
	    aFace = FR.Current().Oriented(OriF);
	    aBuilder.Add(C, aFace);
	    k++;
	  }
	  if(k == 1) {
	    result = CreateObject(aFace);
	    InsertInLabelMoreArguments(aFace, result, ListShapes, myCurrentOCAFDoc);
	  }
	  else {
	    result = CreateObject(C);
	    InsertInLabelMoreArguments(C, result, ListShapes, myCurrentOCAFDoc);
	  }
	}
      }
    }
    else {
      THROW_SALOME_CORBA_EXCEPTION("Null result in GEOM_Gen_i::MakeFace", SALOME::BAD_PARAM);
    }
  }
  catch (Standard_Failure) {
    THROW_SALOME_CORBA_EXCEPTION("Exception catched in GEOM_Gen_i::MakeFace", SALOME::BAD_PARAM);
  }
  return result;
}


//=================================================================================
// function : MakeShell()
// purpose  : Make a compound from a list containing one or more shapes
//=================================================================================
GEOM::GEOM_Shape_ptr GEOM_Gen_i::MakeShell( const GEOM::GEOM_Gen::ListOfIOR& ListShapes )
  throw (SALOME::SALOME_Exception)
{
  Unexpect aCatch(SALOME_SalomeException);
  GEOM::GEOM_Shape_var result ;
  BRepTools_Quilt Glue;
  TopoDS_Shape C;

  for ( unsigned int i = 0; i < ListShapes.length(); i++) {
    GEOM::GEOM_Shape_var aShape = GetIORFromString( ListShapes[i] );    
    TopoDS_Shape Shape = GetTopoShape(aShape) ;
    if( Shape.IsNull() ) {
       THROW_SALOME_CORBA_EXCEPTION("Shell aborted : null shape during operation", SALOME::BAD_PARAM);
    }
    Glue.Add(Shape) ;
  }

  TopExp_Explorer exp(Glue.Shells(), TopAbs_SHELL);
  Standard_Integer ish = 0; 
  for (; exp.More(); exp.Next()) {
    C = exp.Current(); 
    ish++;
  }

  if (ish != 1)
    C = Glue.Shells();
  
  if ( C.IsNull() ) {
    THROW_SALOME_CORBA_EXCEPTION("Null result : Shell operation aborted", SALOME::BAD_PARAM);
  }
  else {
    result = CreateObject(C) ;
    InsertInLabelMoreArguments(C, result, ListShapes, myCurrentOCAFDoc) ;
  }
  return result;
}


//=================================================================================
// function : MakeSolid()
// purpose  : Make a compound from a list containing one or more shapes
//=================================================================================
GEOM::GEOM_Shape_ptr GEOM_Gen_i::MakeSolid( const GEOM::GEOM_Gen::ListOfIOR& ListShapes )
  throw (SALOME::SALOME_Exception)
{
  Unexpect aCatch(SALOME_SalomeException);
  GEOM::GEOM_Shape_var result ;
  Standard_Integer ish = 0;
  TopoDS_Compound  Res;
  TopoDS_Solid     Sol;
  BRep_Builder     B;
  TopoDS_Shape     Shape;

  B.MakeCompound(Res);

  for ( unsigned int i = 0; i < ListShapes.length(); i++) {
    GEOM::GEOM_Shape_var aShape = GetIORFromString( ListShapes[i] );    
    TopoDS_Shape Sh = GetTopoShape(aShape) ;
    if( Sh.IsNull() ) {
       THROW_SALOME_CORBA_EXCEPTION("Solid aborted : null shape during operation", SALOME::BAD_PARAM);
    }
    B.MakeSolid(Sol);
    B.Add(Sol,Sh);
    BRepClass3d_SolidClassifier SC(Sol);
    SC.PerformInfinitePoint(Precision::Confusion());
    if (SC.State() == TopAbs_IN) {
      B.MakeSolid(Sol);
      B.Add(Sol,Sh.Reversed());
    }
    B.Add(Res,Sol);
    ish++;
  }
  if (ish == 1) { Shape = Sol;}
  else          { Shape = Res;} 
  
  if ( Shape.IsNull() ) {
    THROW_SALOME_CORBA_EXCEPTION("Null result : Solid operation aborted", SALOME::BAD_PARAM);
  }
  else {
    result = CreateObject(Shape) ;
    InsertInLabelMoreArguments(Shape, result, ListShapes, myCurrentOCAFDoc) ;
  }
  return result;
}


//================================================================================
// function : MakeLine
// purpose  : Make a Line topology
//================================================================================
GEOM::GEOM_Shape_ptr GEOM_Gen_i::MakeLine(const GEOM::PointStruct& pstruct,
				    const GEOM::DirStruct& dstruct)
  throw (SALOME::SALOME_Exception)
{
  Unexpect aCatch(SALOME_SalomeException);
  GEOM::GEOM_Shape_var result  ;
  gp_Pnt P1(pstruct.x, pstruct.y, pstruct.z);
  gp_Pnt P2(dstruct.PS.x, dstruct.PS.y, dstruct.PS.z) ;  
  TopoDS_Shape tds ;
  
  try {
    tds = BRepBuilderAPI_MakeEdge(P1, P2).Shape();
  }
  catch(Standard_Failure) {
    THROW_SALOME_CORBA_EXCEPTION("Exception catched in GEOM_Gen_i::MakeLine", SALOME::BAD_PARAM);
  }

  if ( tds.IsNull() ) {
    THROW_SALOME_CORBA_EXCEPTION("Make Line aborted : null shape", SALOME::BAD_PARAM);
  }
  else {
    tds.Infinite(true);
    result = CreateObject(tds) ;
    const char *entry = InsertInLabel(tds, result->Name(), myCurrentOCAFDoc) ;
    result->ShapeId(entry);
  }
  return result ;  
}


//================================================================================
// function : MakeVector()
// purpose  : Make a vector
//================================================================================
GEOM::GEOM_Shape_ptr GEOM_Gen_i::MakeVector(const GEOM::PointStruct& pstruct1,
				      const GEOM::PointStruct& pstruct2)
  throw (SALOME::SALOME_Exception)
{
  Unexpect aCatch(SALOME_SalomeException);
  GEOM::GEOM_Shape_var result  ;
  TopoDS_Shape tds ;
  
  try {
    gp_Pnt P1(pstruct1.x, pstruct1.y, pstruct1.z);
    gp_Pnt P2(pstruct2.x, pstruct2.y, pstruct2.z) ;
    tds = BRepBuilderAPI_MakeEdge(P1, P2).Shape();
  }
  catch(Standard_Failure) {
    THROW_SALOME_CORBA_EXCEPTION("Exception catched in GEOM_Gen_i::MakeVector", SALOME::BAD_PARAM);
  }

  if ( tds.IsNull() ) {
    THROW_SALOME_CORBA_EXCEPTION("Make Vector aborted : null shape", SALOME::BAD_PARAM);
  }
  else {
    result = CreateObject(tds) ;
    const char *entry = InsertInLabel(tds, result->Name(), myCurrentOCAFDoc) ;
    result->ShapeId(entry);
  }
  return result ;  
}


//================================================================================
// function : MakeCircle()
// purpose  : 
//================================================================================
GEOM::GEOM_Shape_ptr  GEOM_Gen_i::MakeCircle(const GEOM::PointStruct& pstruct,
				       const GEOM::DirStruct& dstruct,
				       CORBA::Double radius)
  throw (SALOME::SALOME_Exception) 
{
  Unexpect aCatch(SALOME_SalomeException);
  GEOM::GEOM_Shape_var result;
  TopoDS_Shape tds ;

  try {
    gp_Pnt p(pstruct.x, pstruct.y, pstruct.z) ;
    gp_Dir d(dstruct.PS.x, dstruct.PS.y, dstruct.PS.z) ;
    gp_Ax2 axis(p, d) ;
    gp_Circ circ( axis, radius);    
    BRepBuilderAPI_MakeEdge MakeEdge( circ );
    tds = MakeEdge.Edge();
  }
  catch(Standard_Failure) {
    THROW_SALOME_CORBA_EXCEPTION("Exception catched in GEOM_Gen_i::MakeCircle", SALOME::BAD_PARAM);
  }
  if (tds.IsNull()) {
    THROW_SALOME_CORBA_EXCEPTION("Make Circle aborted", SALOME::BAD_PARAM);
  } 
  result = CreateObject(tds);
  const char *entry = InsertInLabel(tds, result->Name(), myCurrentOCAFDoc) ;
  result->ShapeId(entry);
  return result ;  
}

//================================================================================
// function : MakeEllipse()
// purpose  : 
//================================================================================
GEOM::GEOM_Shape_ptr GEOM_Gen_i::MakeEllipse( const GEOM::PointStruct& pstruct,
					      const GEOM::DirStruct& dstruct,
					      CORBA::Double radius_major,
					      CORBA::Double radius_minor )
  throw (SALOME::SALOME_Exception) 
{
  Unexpect aCatch(SALOME_SalomeException);
  GEOM::GEOM_Shape_var result;
  TopoDS_Shape tds ;
  
  try {
    gp_Pnt p(pstruct.x, pstruct.y, pstruct.z) ;
    gp_Dir d(dstruct.PS.x, dstruct.PS.y, dstruct.PS.z) ;

    const gp_Ax2 axis(p, d) ;
    gp_Elips anEllipse( axis, radius_major, radius_minor ) ;
    BRepBuilderAPI_MakeEdge MakeEdge( anEllipse );
    tds = MakeEdge.Edge();

  }
  catch(Standard_Failure) {
    THROW_SALOME_CORBA_EXCEPTION("Exception catched in GEOM_Gen_i::MakeEllipse", SALOME::BAD_PARAM);
  }
  if (tds.IsNull()) {
    THROW_SALOME_CORBA_EXCEPTION("Make Ellipse aborted", SALOME::BAD_PARAM);
  } 
  result = CreateObject(tds);
  const char *entry = InsertInLabel(tds, result->Name(), myCurrentOCAFDoc) ;
  result->ShapeId(entry);
  return result ;  
}

//================================================================================
// function : MakeArc()
// purpose  : make an arc of circle from pInit to pEnd and passing on pCircle
//================================================================================
GEOM::GEOM_Shape_ptr  GEOM_Gen_i::MakeArc(const GEOM::PointStruct& pInit,
				    const GEOM::PointStruct& pCircle,
				    const GEOM::PointStruct& pEnd)
  throw (SALOME::SALOME_Exception) 
{
  Unexpect aCatch(SALOME_SalomeException);
  GEOM::GEOM_Shape_var result;
  try {
    gp_Pnt pI(pInit.x, pInit.y, pInit.z) ;
    gp_Pnt pC(pCircle.x, pCircle.y, pCircle.z) ;
    gp_Pnt pE(pEnd.x, pEnd.y, pEnd.z) ;
    
    GC_MakeArcOfCircle arc( pI, pC, pE ) ;
    if( !arc.IsDone() ) {
      THROW_SALOME_CORBA_EXCEPTION("Arc not done", SALOME::BAD_PARAM);
    }
    BRepBuilderAPI_MakeEdge MakeEdge( arc );
    TopoDS_Shape tds = MakeEdge.Edge();
    if (tds.IsNull()) {
      THROW_SALOME_CORBA_EXCEPTION("Null result : arc not done", SALOME::BAD_PARAM);
    } 
    else {
      result = CreateObject(tds);
      const char *entry = InsertInLabel(tds, result->Name(), myCurrentOCAFDoc) ;
      result->ShapeId(entry);
    }
  }
  catch(Standard_Failure) {
    THROW_SALOME_CORBA_EXCEPTION("Exception catched in GEOM_Gen_i::MakeArc", SALOME::BAD_PARAM);
  }
  return result ;
}

//=================================================================================
 // function : MakeSketcher()
 // purpose  : Make a wire from a list containing many points
 //=================================================================================
 GEOM::GEOM_Shape_ptr GEOM_Gen_i::MakeSketcher( const char *Cmd )
   throw (SALOME::SALOME_Exception)
 {
   GEOM::GEOM_Shape_var result ;
   TopoDS_Shape tds ;
   try {
     Sketcher_Profile aProfile (Cmd);
     if(aProfile.IsDone())
       tds = aProfile.GetShape();
   }
   catch(Standard_Failure) {
     THROW_SALOME_CORBA_EXCEPTION("Exception catched in GEOM_Gen_i::MakeSketcher", SALOME::BAD_PARAM);
   }
 
   if (tds.IsNull()) {
     THROW_SALOME_CORBA_EXCEPTION("MakeSketcher aborted : null shape", SALOME::BAD_PARAM);
   } 
   else {
     result = CreateObject(tds);
     const char *entry = InsertInLabel(tds, result->Name(), myCurrentOCAFDoc) ;
     result->ShapeId(entry) ;
   }
   return result;
 }

//=================================================================================
// function : MakeTranslation()
// purpose  : Translate a 3D shape
//=================================================================================
GEOM::GEOM_Shape_ptr GEOM_Gen_i::MakeTranslation( GEOM::GEOM_Shape_ptr myShape,
					    CORBA::Double x,
					    CORBA::Double y,
					    CORBA::Double z)
  throw (SALOME::SALOME_Exception)
{
  Unexpect aCatch(SALOME_SalomeException);
  GEOM::GEOM_Shape_var result ;
  TopoDS_Shape aShape = GetTopoShape(myShape) ;
  if( aShape.IsNull() ) {
    THROW_SALOME_CORBA_EXCEPTION("Translation aborted : null shape", SALOME::BAD_PARAM);
  }
  gp_Vec theVector(x,y,z) ;
  gp_Trsf theTransformation ;
  theTransformation.SetTranslation(theVector) ;
  BRepBuilderAPI_Transform myBRepTransformation(aShape, theTransformation, Standard_False) ;
  TopoDS_Shape tds = myBRepTransformation.Shape() ;

  result = CreateObject(tds) ;
  if( CORBA::is_nil(result) ) {
    THROW_SALOME_CORBA_EXCEPTION("Translation aborted : null result", SALOME::BAD_PARAM);
  }
  InsertInLabelOneArgument(aShape, myShape, tds, result, myCurrentOCAFDoc) ;
  return result;
}


//=================================================================================
// function : MakeMultiTranslation1D()
// purpose  : Multi-Translate a 3D shape
//=================================================================================
GEOM::GEOM_Shape_ptr GEOM_Gen_i::MakeMultiTranslation1D( GEOM::GEOM_Shape_ptr myShape,
						   const GEOM::DirStruct& dir,
						   CORBA::Double step,
						   CORBA::Short nbtimes )
  throw (SALOME::SALOME_Exception)
{
  Unexpect aCatch(SALOME_SalomeException);
  GEOM::GEOM_Shape_var result ;
  TopoDS_Shape tds ;

  TopoDS_Shape aShape = GetTopoShape(myShape) ;
  if( aShape.IsNull() )
    THROW_SALOME_CORBA_EXCEPTION("MakeMultiTranslation1D aborted : null shape", SALOME::BAD_PARAM);
  
  try {
    int i ;
    double DX, DY, DZ ;
    gp_Trsf theTransformation ;
    gp_Vec myVec ;
    gp_Vec Vec( dir.PS.x, dir.PS.y, dir.PS.z ) ;
    Vec.Normalize();
    TopoDS_Compound compound;
    BRep_Builder B;
    B.MakeCompound( compound );
    
    for ( i = 0; i < nbtimes; i++ ) {
      DX = i * step * Vec.X() ;
      DY = i * step * Vec.Y() ;
      DZ = i * step * Vec.Z() ;
      myVec.SetCoord( DX, DY, DZ ) ;
      theTransformation.SetTranslation(myVec) ;
      BRepBuilderAPI_Transform myBRepTransformation(aShape, theTransformation, Standard_False) ;
      B.Add( compound, myBRepTransformation.Shape() );
    }
    tds = compound ;
    result = CreateObject(tds) ;
  }
  catch (Standard_Failure) {
    THROW_SALOME_CORBA_EXCEPTION("Exception catched in GEOM_Gen_i::MakeMultiTranslation1D", SALOME::BAD_PARAM);
  }
    
  if( CORBA::is_nil(result) ) {
    THROW_SALOME_CORBA_EXCEPTION("MakeMultiTranslation1D aborted : null result", SALOME::BAD_PARAM);
  }
  InsertInLabelOneArgument(aShape, myShape, tds, result, myCurrentOCAFDoc) ;
  return result;
}


//=================================================================================
// function : MakeMultiTranslation2D()
// purpose  : Multi-Translate a 3D shape
//=================================================================================
GEOM::GEOM_Shape_ptr GEOM_Gen_i::MakeMultiTranslation2D( GEOM::GEOM_Shape_ptr myShape,
						   const GEOM::DirStruct& dir1,
						   CORBA::Double step1,
						   CORBA::Short nbtimes1,
						   const GEOM::DirStruct& dir2,
						   CORBA::Double step2,
						   CORBA::Short nbtimes2 )
  throw (SALOME::SALOME_Exception)
{
  Unexpect aCatch(SALOME_SalomeException);
  GEOM::GEOM_Shape_var result ;
  TopoDS_Shape tds ;
  
  TopoDS_Shape aShape = GetTopoShape(myShape) ;
  if( aShape.IsNull() ) {
    THROW_SALOME_CORBA_EXCEPTION("MakeMultiTranslation2D aborted : null shape", SALOME::BAD_PARAM);
  }
  
  try {
    int i, j ;
    double DX, DY, DZ ;
    gp_Trsf theTransformation ;
    gp_Vec myVec ;
    gp_Vec Vec1( dir1.PS.x, dir1.PS.y, dir1.PS.z ) ;
    Vec1.Normalize();
    gp_Vec Vec2( dir2.PS.x, dir2.PS.y, dir2.PS.z ) ;
    Vec2.Normalize();
    TopoDS_Compound compound;
    BRep_Builder B;
    B.MakeCompound( compound );
    
    for ( i = 0; i < nbtimes1; i++ ) {
      for ( j = 0; j < nbtimes2; j++ ) {
	DX = i * step1 * Vec1.X() + j * step2 * Vec2.X() ;
	DY = i * step1 * Vec1.Y() + j * step2 * Vec2.Y() ;
	DZ = i * step1 * Vec1.Z() + j * step2 * Vec2.Z() ;
	myVec.SetCoord( DX, DY, DZ ) ;
	theTransformation.SetTranslation(myVec) ;
	BRepBuilderAPI_Transform myBRepTransformation(aShape, theTransformation, Standard_False) ;
	B.Add( compound, myBRepTransformation.Shape() );
      }
    }
    tds = compound ;
    result = CreateObject(tds) ;
  }
  catch(Standard_Failure) {
    THROW_SALOME_CORBA_EXCEPTION("Exception catched in GEOM_Gen_i::MakeMultiTranslation2D", SALOME::BAD_PARAM);
  }
   
  if( CORBA::is_nil(result) ) {
    THROW_SALOME_CORBA_EXCEPTION("MakeMultiTranslation2D aborted : null result", SALOME::BAD_PARAM);
  }
  InsertInLabelOneArgument(aShape, myShape, tds, result, myCurrentOCAFDoc) ;
  return result;
}


//=================================================================================
// function : MakeMultiRotation1D()
// purpose  : Multi-Rotate a 3D shape
//=================================================================================
GEOM::GEOM_Shape_ptr GEOM_Gen_i::MakeMultiRotation1D( GEOM::GEOM_Shape_ptr myShape,
						const GEOM::DirStruct& dir,
						const GEOM::PointStruct& loc,
						CORBA::Short nbtimes)
  throw (SALOME::SALOME_Exception)
{
  Unexpect aCatch(SALOME_SalomeException);
  GEOM::GEOM_Shape_var result ;
  TopoDS_Shape tds ;
  TopoDS_Shape aShape = GetTopoShape(myShape) ;
  if( aShape.IsNull() ) {
    THROW_SALOME_CORBA_EXCEPTION("MakeMultiRotation1D aborted : null shape", SALOME::BAD_PARAM);
  }
 
  try {

    int i ;
    gp_Pnt P(loc.x, loc.y, loc.z) ;
    gp_Dir D(dir.PS.x, dir.PS.y, dir.PS.z) ;
    gp_Ax1 AX1(P, D) ;
    
    double angle = 360.0/nbtimes ;
    gp_Trsf theTransformation ;
    TopoDS_Compound compound;
    BRep_Builder B;
    B.MakeCompound( compound );
    
    for ( i = 0; i < nbtimes; i++ ) {
      theTransformation.SetRotation(AX1, i*angle*PI180) ;
      BRepBuilderAPI_Transform myBRepTransformation(aShape, theTransformation, Standard_False) ;
      B.Add( compound, myBRepTransformation.Shape() );
    }
    tds = compound ;
    result = CreateObject(tds) ;
  }
  catch(Standard_Failure) {
    THROW_SALOME_CORBA_EXCEPTION("Exception catched in GEOM_Gen_i::MakeMultiRotation1D", SALOME::BAD_PARAM);
  }
  
  if( CORBA::is_nil(result) ) {
    THROW_SALOME_CORBA_EXCEPTION("MakeMultiRotation1D aborted : null result", SALOME::BAD_PARAM);
  }
  InsertInLabelOneArgument(aShape, myShape, tds, result, myCurrentOCAFDoc) ;
  return result;
}


//=================================================================================
// function : MakeMultiRotation2D()
// purpose  : Multi-Rotate a 3D shape
//=================================================================================
GEOM::GEOM_Shape_ptr GEOM_Gen_i::MakeMultiRotation2D( GEOM::GEOM_Shape_ptr myShape,
						const GEOM::DirStruct& dir,
						const GEOM::PointStruct& loc,
						CORBA::Double ang,
						CORBA::Short nbtimes1,
						CORBA::Double step,
						CORBA::Short nbtimes2 )
  throw (SALOME::SALOME_Exception)
{
  Unexpect aCatch(SALOME_SalomeException);
  GEOM::GEOM_Shape_var result ;
  TopoDS_Shape tds ;
  TopoDS_Shape aShape = GetTopoShape(myShape) ;
  if( aShape.IsNull() ) {
    THROW_SALOME_CORBA_EXCEPTION("MakeMultiRotation2D aborted : null shape", SALOME::BAD_PARAM);
  }

  try {
   
    int i, j ;
    double DX, DY, DZ ;
    gp_Pnt P(loc.x, loc.y, loc.z) ;
    gp_Dir D(dir.PS.x, dir.PS.y, dir.PS.z) ;
    gp_Ax1 AX1(P, D) ;
    gp_Trsf theTransformation1 ;
    gp_Trsf theTransformation2 ;
    gp_Pnt P1 ;
    GProp_GProps System ;
    
    if ( aShape.ShapeType() == TopAbs_VERTEX) {
      P1 = BRep_Tool::Pnt(TopoDS::Vertex( aShape ));
    } 
    else if ( aShape.ShapeType() == TopAbs_EDGE || aShape.ShapeType() == TopAbs_WIRE ) {
      BRepGProp::LinearProperties(aShape, System);
      P1 = System.CentreOfMass() ;
    }
    else if ( aShape.ShapeType() == TopAbs_FACE || aShape.ShapeType() == TopAbs_SHELL ) {
      BRepGProp::SurfaceProperties(aShape, System);
      P1 = System.CentreOfMass() ;
    }
    else {
      BRepGProp::VolumeProperties(aShape, System);
      P1 = System.CentreOfMass() ;
    }
    
    Handle(Geom_Line) Line = new Geom_Line(AX1);
    gp_Pnt P2 = GeomAPI_ProjectPointOnCurve( P1, Line ) ;
    
    if ( P1.IsEqual(P2, Precision::Confusion() ) )
      THROW_SALOME_CORBA_EXCEPTION("Points are confused", SALOME::BAD_PARAM);
    
    gp_Vec Vec(P1.X()-P2.X(), P1.Y()-P2.Y(), P1.Z()-P2.Z()) ;
    Vec.Normalize();
    
    gp_Vec myVec ;
    TopoDS_Compound compound;
    BRep_Builder B;
    B.MakeCompound( compound );
    
    for ( i = 0; i < nbtimes2; i++ ) {
      for ( j = 0; j < nbtimes1; j++ ) {
	DX = i * step * Vec.X() ;
	DY = i * step * Vec.Y() ;
	DZ = i * step * Vec.Z() ;
	myVec.SetCoord( DX, DY, DZ ) ;
	theTransformation1.SetTranslation(myVec) ;
	theTransformation2.SetRotation(AX1, j*ang*PI180) ;
	BRepBuilderAPI_Transform myBRepTransformation1(aShape, theTransformation1, Standard_False) ;
	BRepBuilderAPI_Transform myBRepTransformation2(myBRepTransformation1.Shape(), theTransformation2, Standard_False) ;
	B.Add( compound, myBRepTransformation2.Shape() );
      }
    }
    tds = compound ;
    result = CreateObject(tds) ;  
  }
  catch(Standard_Failure) {
    THROW_SALOME_CORBA_EXCEPTION("Exception catched in GEOM_Gen_i::MakeMultiRotation2D", SALOME::BAD_PARAM);
  }
  
  if( CORBA::is_nil(result) ) {
    THROW_SALOME_CORBA_EXCEPTION("MakeMultiRotation2D aborted : null result", SALOME::BAD_PARAM);
  }
  InsertInLabelOneArgument(aShape, myShape, tds, result, myCurrentOCAFDoc) ;
  return result;
}


//=================================================================================
// function : MakeCopy()
// purpose  : Copy a 3D shape
//=================================================================================
GEOM::GEOM_Shape_ptr GEOM_Gen_i::MakeCopy( GEOM::GEOM_Shape_ptr Shape)
  throw (SALOME::SALOME_Exception) 
{
  Unexpect aCatch(SALOME_SalomeException);
  GEOM::GEOM_Shape_var result ;
  TopoDS_Shape tds ;
  TopoDS_Shape aShape = GetTopoShape(Shape) ;
  if(aShape.IsNull() ) {
    THROW_SALOME_CORBA_EXCEPTION("Copy aborted : null shape during operation", SALOME::BAD_PARAM);
  }  
  BRepBuilderAPI_Copy Copy(aShape);
  if( Copy.IsDone() ) {   
    tds = Copy.Shape();
    result = CreateObject(tds);    
    InsertInLabelOneArgument(aShape, Shape, tds, result, myCurrentOCAFDoc) ;
  }

  return result;
}


//=================================================================================
// function : MakeMirrorByPlane()
// purpose  : build a shape by symmetry of 'myShape' with 'shapePlane' in argument
//=================================================================================
GEOM::GEOM_Shape_ptr GEOM_Gen_i::MakeMirrorByPlane(GEOM::GEOM_Shape_ptr myShape,
					     GEOM::GEOM_Shape_ptr shapePlane) 
  throw (SALOME::SALOME_Exception)
{
  Unexpect aCatch(SALOME_SalomeException);
  GEOM::GEOM_Shape_var result ; 
  TopoDS_Shape tds ;
  TopoDS_Shape aShape      = GetTopoShape(myShape) ;
  TopoDS_Shape aShapePlane = GetTopoShape(shapePlane) ;
  if( aShape.IsNull()  || aShapePlane.IsNull() ) {
    THROW_SALOME_CORBA_EXCEPTION("Mirror aborted : null shape argument", SALOME::BAD_PARAM);
  }  
  
  try {
    Handle(Geom_Surface) surf = BRep_Tool::Surface(TopoDS::Face(aShapePlane)) ;	
    Handle(Geom_Plane) myPlane = Handle(Geom_Plane)::DownCast(surf) ;
    const gp_Ax3 pos = myPlane->Position() ;
    const gp_Pnt loc = pos.Location() ;  /* location of the plane */
    const gp_Dir dir = pos.Direction() ; /* Main direction of the plane (Z axis) */  
    
    /* plane used for mirroring */
    gp_Ax2 pln(loc, dir) ;
    gp_Trsf theTransformation ;
    theTransformation.SetMirror(pln) ;
    BRepBuilderAPI_Transform myBRepTransformation(aShape, theTransformation, Standard_False) ;

    tds = myBRepTransformation.Shape() ;
    if(tds.IsNull() ) {
      THROW_SALOME_CORBA_EXCEPTION("Mirror aborted", SALOME::BAD_PARAM);
    }
  }
  catch(Standard_Failure) {
    THROW_SALOME_CORBA_EXCEPTION("Exception catched in GEOM_Gen_i::MakeMirrorByPlane", SALOME::BAD_PARAM);    
  }
  
  result = CreateObject(tds) ;

  /* Insert arguments in ocaf */
  GEOM::GEOM_Gen::ListOfIOR_var ListShapes = new GEOM::GEOM_Gen::ListOfIOR;
  ListShapes->length(2);
  ListShapes[0] = GetStringFromIOR(GEOM::GEOM_Shape::_duplicate(myShape)) ;
  ListShapes[1] = GetStringFromIOR(GEOM::GEOM_Shape::_duplicate(shapePlane)) ;
  InsertInLabelMoreArguments(tds, result, ListShapes, myCurrentOCAFDoc) ;
  return result ;
}



//=================================================================================
// function : MakeRotation()
// purpose  : Rotation of a 3D shape around an axis
//=================================================================================
GEOM::GEOM_Shape_ptr GEOM_Gen_i::MakeRotation( GEOM::GEOM_Shape_ptr myShape,
					 const GEOM::AxisStruct& axis,
					 CORBA::Double angle)
  throw (SALOME::SALOME_Exception)
{
  Unexpect aCatch(SALOME_SalomeException);
  GEOM::GEOM_Shape_var result ;
  TopoDS_Shape tds ;
  TopoDS_Shape aShape = GetTopoShape(myShape) ;
  if( aShape.IsNull() ) {
    THROW_SALOME_CORBA_EXCEPTION("Rotation aborted : null shape during operation", SALOME::BAD_PARAM);
  }
  
  try {
    gp_Pnt P(axis.x, axis.y, axis.z) ;
    gp_Dir D(axis.vx, axis.vy, axis.vz) ;
    gp_Ax1 AX(P, D) ;
    
    gp_Trsf theTransformation ;
    theTransformation.SetRotation(AX, angle) ;
    BRepBuilderAPI_Transform myBRepTransformation(aShape, theTransformation, Standard_False) ;
    tds = myBRepTransformation.Shape() ;
  }
  catch(Standard_Failure) {
    THROW_SALOME_CORBA_EXCEPTION("Exception catched in GEOM_Gen_i::MakeRotation", SALOME::BAD_PARAM);
  }
  
  if ( !tds.IsNull() ) {
    result = CreateObject(tds) ;
    InsertInLabelOneArgument(aShape, myShape, tds, result, myCurrentOCAFDoc) ;
  }
  return result ;
}


//=================================================================================
// function : MakeScaleTransform()
// purpose  : Make a shape multipling another by a scale factor
//=================================================================================
GEOM::GEOM_Shape_ptr GEOM_Gen_i::MakeScaleTransform(GEOM::GEOM_Shape_ptr myShape,
					      const GEOM::PointStruct& theCenterOfScale,
					      CORBA::Double factor)
  throw (SALOME::SALOME_Exception)
{
  Unexpect aCatch(SALOME_SalomeException);
  GEOM::GEOM_Shape_var result ;
  TopoDS_Shape tds ;
  TopoDS_Shape aShape = GetTopoShape(myShape) ;
  if( aShape.IsNull() ) {
    THROW_SALOME_CORBA_EXCEPTION("Scale aborted : null shape during operation", SALOME::BAD_PARAM);
  }
  
  try {
    gp_Pnt Pcenter(theCenterOfScale.x, theCenterOfScale.y, theCenterOfScale.z) ;
    gp_Trsf theTransformation ;  
    theTransformation.SetScale(Pcenter, factor) ;
    BRepBuilderAPI_Transform myBRepTransformation(aShape, theTransformation, Standard_False) ;
    tds = myBRepTransformation.Shape() ;
  }
  catch(Standard_Failure) {
    THROW_SALOME_CORBA_EXCEPTION("Exception catched in GEOM_Gen_i::MakeScaleTransform", SALOME::BAD_PARAM);
  }
  
  if ( !tds.IsNull() ) {
    result = CreateObject(tds) ; 
    InsertInLabelOneArgument(aShape, myShape, tds, result, myCurrentOCAFDoc) ;
  }
  return result ;
}


//=================================================================================
// function : MakeCompound()
// purpose  : Make a compound from a list containing one or more shapes
//=================================================================================
GEOM::GEOM_Shape_ptr GEOM_Gen_i::MakeCompound( const GEOM::GEOM_Gen::ListOfIOR& ListShapes )
  throw (SALOME::SALOME_Exception)
{
  Unexpect aCatch(SALOME_SalomeException);
  GEOM::GEOM_Shape_var result ;
  TopoDS_Compound C;
  BRep_Builder aBuilder;
  aBuilder.MakeCompound(C) ;

  for ( unsigned int i = 0; i < ListShapes.length(); i++) {
    GEOM::GEOM_Shape_var aShape = GetIORFromString( ListShapes[i] );    
    TopoDS_Shape Shape = GetTopoShape(aShape) ;
    if( Shape.IsNull() ) {
       THROW_SALOME_CORBA_EXCEPTION("Compound aborted : null shape during operation", SALOME::BAD_PARAM);
    }
    aBuilder.Add(C, Shape) ;
  }
  
  if ( C.IsNull() ) {
    THROW_SALOME_CORBA_EXCEPTION("Null result : Compound operation aborted", SALOME::BAD_PARAM);
  }
  else {
    result = CreateObject(C) ;
    InsertInLabelMoreArguments(C, result, ListShapes, myCurrentOCAFDoc) ;
  }
  return result;
}


//================================================================================
// function : MakeEdge()
// purpose  : Make a linear edge with 2 points
//================================================================================
GEOM::GEOM_Shape_ptr GEOM_Gen_i::MakeEdge(const GEOM::PointStruct& pstruct1,
				    const GEOM::PointStruct& pstruct2)
  throw (SALOME::SALOME_Exception)
{
  Unexpect aCatch(SALOME_SalomeException);
  GEOM::GEOM_Shape_var result  ;
  TopoDS_Shape tds ;
  
  try {
    gp_Pnt P1(pstruct1.x, pstruct1.y, pstruct1.z);
    gp_Pnt P2(pstruct2.x, pstruct2.y, pstruct2.z) ;  
    tds = BRepBuilderAPI_MakeEdge(P1, P2).Shape();
    if ( tds.IsNull() )
      THROW_SALOME_CORBA_EXCEPTION("MakeEdge aborted : null result", SALOME::BAD_PARAM);
  }
  catch (Standard_Failure) {
    THROW_SALOME_CORBA_EXCEPTION("Exception catched in MakeEdge", SALOME::BAD_PARAM);
  }
  
  result = CreateObject(tds) ;
  const char *entry = InsertInLabel(tds, result->Name(), myCurrentOCAFDoc) ;
  result->ShapeId(entry);
  return result ;  
}



//=================================================================================
// function : MakeWire()
// purpose  : Make a wire from a list containing one or more edges or wires that can
// be connected
//=================================================================================
GEOM::GEOM_Shape_ptr GEOM_Gen_i::MakeWire( const GEOM::GEOM_Gen::ListOfIOR& ListShapes )
  throw (SALOME::SALOME_Exception)
{
  Unexpect aCatch(SALOME_SalomeException);
  GEOM::GEOM_Shape_var result ;
  BRepBuilderAPI_MakeWire MW ;
  TopoDS_Shape tds, Shape ; 
  
  try {
    for ( unsigned int i = 0; i < ListShapes.length(); i++) {
      GEOM::GEOM_Shape_var aShape = GetIORFromString( ListShapes[i] );    
      Shape = GetTopoShape(aShape) ;
      if( Shape.IsNull() ) {
	THROW_SALOME_CORBA_EXCEPTION("MakeWire aborted : null shape during operation", SALOME::BAD_PARAM);
      }
      if( Shape.ShapeType() == TopAbs_EDGE )
	MW.Add( TopoDS::Edge(Shape) ) ;
      if (Shape.ShapeType() == TopAbs_WIRE ) 
	MW.Add( TopoDS::Wire(Shape) ) ;
    }    
    tds = MW  ;

  }
  catch(Standard_Failure) {
    THROW_SALOME_CORBA_EXCEPTION("Exception catched in GEOM_Gen_i::MakeWire", SALOME::BAD_PARAM);
  }
  
  if( tds.IsNull() ) {
    THROW_SALOME_CORBA_EXCEPTION("Make Wire operation aborted : null result", SALOME::BAD_PARAM);
  }
  else {
    result = CreateObject(tds) ;
    InsertInLabelMoreArguments(tds, result, ListShapes, myCurrentOCAFDoc) ;   
  }
  return result;
}


//=================================================================================
// function : MakeRevolution()
// purpose  : 
//=================================================================================
GEOM::GEOM_Shape_ptr GEOM_Gen_i::MakeRevolution(GEOM::GEOM_Shape_ptr myShape,
					  const GEOM::AxisStruct& axis,
					  double angle)
  throw (SALOME::SALOME_Exception)
{ 
  Unexpect aCatch(SALOME_SalomeException);
  GEOM::GEOM_Shape_var result ;
  TopoDS_Shape tds ;
  TopoDS_Shape aShape = GetTopoShape(myShape) ;
  if( aShape.IsNull() ) {
    THROW_SALOME_CORBA_EXCEPTION("Revolution aborted : null shape", SALOME::BAD_PARAM);
  }
  try {
    gp_Pnt P(axis.x, axis.y, axis.z) ;
    gp_Dir D(axis.vx, axis.vy, axis.vz);
    gp_Ax1 AX(P,D);
    tds = BRepPrimAPI_MakeRevol(aShape, AX, angle);
  }
  catch(Standard_Failure) {
    THROW_SALOME_CORBA_EXCEPTION("Exception catched in GEOM_Gen_i::MakeRevolution", SALOME::BAD_PARAM);
  }
  
  if( tds.IsNull() ) {
    THROW_SALOME_CORBA_EXCEPTION("Revolution aborted", SALOME::BAD_PARAM);
  }
  result = CreateObject(tds) ;
  InsertInLabelOneArgument(aShape, myShape, tds, result, myCurrentOCAFDoc) ;
  return result ;     
}


//=================================================================================
// function : MakePipe()
// purpose  : Create a shape by sweeping a baseShape along a pathShape
//=================================================================================
GEOM::GEOM_Shape_ptr GEOM_Gen_i::MakePipe( GEOM::GEOM_Shape_ptr pathShape,
				     GEOM::GEOM_Shape_ptr baseShape )
  throw (SALOME::SALOME_Exception)
{
  Unexpect aCatch(SALOME_SalomeException);
  GEOM::GEOM_Shape_var result ;
  TopoDS_Shape tds ;
  TopoDS_Wire aWire ;
  TopoDS_Shape pathTds = GetTopoShape(pathShape) ;
  TopoDS_Shape baseTds = GetTopoShape(baseShape) ;

  if( baseTds.IsNull() || pathTds.IsNull() ) {
    THROW_SALOME_CORBA_EXCEPTION("MakePipe aborted : null shape argument", SALOME::BAD_PARAM);
  }

  if( pathTds.ShapeType() == TopAbs_WIRE ) {
    aWire = TopoDS::Wire(pathTds) ;
  }
  else {
    if ( pathTds.ShapeType() == TopAbs_EDGE ) {
      TopoDS_Edge aEdge = TopoDS::Edge(pathTds) ;
      aWire = BRepBuilderAPI_MakeWire(aEdge);
    }
    else {
      THROW_SALOME_CORBA_EXCEPTION("MakePipe aborted : bad shape type", SALOME::BAD_PARAM);
    }
  }

  try {
    tds = BRepOffsetAPI_MakePipe(aWire, baseTds) ;
  }
  catch(Standard_Failure) {
    THROW_SALOME_CORBA_EXCEPTION("Exception catched in GEOM_Gen_i::MakePipe", SALOME::BAD_PARAM);
  }
  
  if (  !IsValid(tds) ) {
    THROW_SALOME_CORBA_EXCEPTION("MakePipe aborted : non valid shape result", SALOME::BAD_PARAM);
  }
  else {
    result = CreateObject(tds) ;

    /* Insert arguments in ocaf */
    GEOM::GEOM_Gen::ListOfIOR_var ListShapes = new GEOM::GEOM_Gen::ListOfIOR;
    ListShapes->length(2);
    ListShapes[0] = GetStringFromIOR(GEOM::GEOM_Shape::_duplicate(pathShape)) ;
    ListShapes[1] = GetStringFromIOR(GEOM::GEOM_Shape::_duplicate(baseShape)) ;    
    InsertInLabelMoreArguments(tds, result, ListShapes, myCurrentOCAFDoc) ;
  }
  return result ;
}


//=================================================================================
// function : MakePrism()
// purpose  : uses myShape as base and the vector P1 to P2
//=================================================================================
GEOM::GEOM_Shape_ptr GEOM_Gen_i::MakePrism( GEOM::GEOM_Shape_ptr myShape,
					   const GEOM::PointStruct& P1,
					   const GEOM::PointStruct& P2 )
  throw (SALOME::SALOME_Exception)
{		
  Unexpect aCatch(SALOME_SalomeException);
  GEOM::GEOM_Shape_var result ;
  TopoDS_Shape tds ;
  TopoDS_Shape aShape = GetTopoShape(myShape) ;
  if( aShape.IsNull() ) {
    THROW_SALOME_CORBA_EXCEPTION("Prism aborted : null shape operation", SALOME::BAD_PARAM);
  }
  
  try {
    gp_Vec Vector (P2.x - P1.x, P2.y - P1.y, P2.z - P1.z) ;
    tds = BRepPrimAPI_MakePrism(aShape, Vector, Standard_False).Shape() ;
  }
  catch(Standard_Failure) {
    THROW_SALOME_CORBA_EXCEPTION("Exception catched in GEOM_Gen_i::MakePipe", SALOME::BAD_PARAM);
  }
  
  if ( tds.IsNull() ) {
    THROW_SALOME_CORBA_EXCEPTION("Prism aborted", SALOME::BAD_PARAM);
  }
  else {
    result = CreateObject(tds) ;     
    InsertInLabelOneArgument(aShape, myShape, tds, result, myCurrentOCAFDoc) ;
  }
  return result ;
}


//=================================================================================
// function : MakeCDG()
// purpose  : Create a CDG topology.
//=================================================================================
GEOM::GEOM_Shape_ptr GEOM_Gen_i::MakeCDG(GEOM::GEOM_Shape_ptr aShape) 
  throw (SALOME::SALOME_Exception)
{
  Unexpect aCatch(SALOME_SalomeException);
  GEOM::GEOM_Shape_var result ;
  TopoDS_Shape tds ;
  TopoDS_Shape shape = GetTopoShape(aShape) ;
  GProp_GProps System;
  gp_Pnt myCenterMass ;

  if( shape.IsNull() ) {
    THROW_SALOME_CORBA_EXCEPTION("MakeCDG aborted : null shape argument", SALOME::BAD_PARAM);
  }
  
  try {
    if ( shape.ShapeType() == TopAbs_VERTEX) {
      myCenterMass = BRep_Tool::Pnt(TopoDS::Vertex( shape ));
    } 
    else if ( shape.ShapeType() == TopAbs_EDGE || shape.ShapeType() == TopAbs_WIRE ) {
      BRepGProp::LinearProperties(shape, System);
      myCenterMass = System.CentreOfMass() ;
    }
    else if ( shape.ShapeType() == TopAbs_FACE || shape.ShapeType() == TopAbs_SHELL ) {
      BRepGProp::SurfaceProperties(shape, System);
      myCenterMass = System.CentreOfMass() ;
    }
    else {
      BRepGProp::VolumeProperties(shape, System);
      myCenterMass = System.CentreOfMass() ;
    }

    tds = BRepBuilderAPI_MakeVertex(myCenterMass).Shape() ;
  }
  catch(Standard_Failure) {
    THROW_SALOME_CORBA_EXCEPTION("Exception catched in GEOM_Gen_i::MakeCDG", SALOME::BAD_PARAM);
  }
  
  if ( tds.IsNull() ) {
    THROW_SALOME_CORBA_EXCEPTION("Make CDG aborted : null shape result", SALOME::BAD_PARAM);
  }
  else {
    result = CreateObject(tds) ;     
    InsertInLabelOneArgument(shape, aShape, tds, result, myCurrentOCAFDoc) ;
  }
  return result ; 
}


//=================================================================================
// function : Archimede()
// purpose  :
//=================================================================================
GEOM::GEOM_Shape_ptr GEOM_Gen_i::Archimede(GEOM::GEOM_Shape_ptr aShape,
				     CORBA::Double aWeight,
				     CORBA::Double aWaterDensity,
				     CORBA::Double aMeshingDeflection)
  throw (SALOME::SALOME_Exception)
{
  Unexpect aCatch(SALOME_SalomeException);
  GEOM::GEOM_Shape_var result;

  double cste = -1;
  if (aWaterDensity != 0.)
    cste = aWeight/aWaterDensity;
  else
    THROW_SALOME_CORBA_EXCEPTION("Water density is null", SALOME::BAD_PARAM);

  TopoDS_Shape shape = GetTopoShape(aShape) ;
  if( shape.IsNull() ) {
    THROW_SALOME_CORBA_EXCEPTION("Shape is null", SALOME::BAD_PARAM);
  }

  gp_Dir direct(0.0,0.0,1.0);
  gp_Pnt PosPlan(0.0,0.0,0.0);
  Geom_Plane PP (PosPlan,direct);
  Handle(Geom_Geometry) G = PP.Copy();
  Handle(Geom_Plane) P = Handle(Geom_Plane)::DownCast(G);

  gp_Dir Zdirection(0.0,0.0,1.0);
  VolumeSection VOL( shape, aMeshingDeflection);
  VOL.SetPlane(P);
  Handle (Geom_RectangularTrimmedSurface) SurfaceTrimmee;
  
  if(Zdirection.IsEqual(direct,Precision::Angular()) == Standard_False) { 
    VOL.MakeRotation(direct);
  }
  
  VOL.CenterOfGravity();
  SurfaceTrimmee = VOL.TrimSurf();
  Standard_Real Cote = VOL.Archimede( cste, aMeshingDeflection );
  
  if ( Cote == -1 ) {
    double Zmin,Zmax;
    VOL.getZ(Zmin,Zmax);
    double volume = VOL.CalculateVolume( Zmax ) * aWaterDensity;

    char msg[100]="";  
    sprintf(msg, "shape sinks to the bottom : Weigth max = %.1f", volume);

    THROW_SALOME_CORBA_EXCEPTION(msg, SALOME::BAD_PARAM);
  }
  
  SurfaceTrimmee=VOL.AjustePlan(SurfaceTrimmee,Cote,PosPlan);
  if(Zdirection.IsEqual(direct,Precision::Angular()) == Standard_False) { 
    SurfaceTrimmee=VOL.InvMakeRotation(direct,SurfaceTrimmee);
  }
  
  Standard_Real u1,u2,v1,v2;
  SurfaceTrimmee->Bounds(u1,u2,v1,v2);
  TopoDS_Face tirant = BRepBuilderAPI_MakeFace(SurfaceTrimmee, u1, u2, v1, v2);
  
  if (tirant.IsNull()) {
    THROW_SALOME_CORBA_EXCEPTION("Result is null", SALOME::BAD_PARAM);
  }

  result = CreateObject(tirant);
  InsertInLabelOneArgument(shape, aShape, tirant, result, myCurrentOCAFDoc) ;

  return result;  
}


//================================================================================
// function : MakeFillet()
// purpose  : Create a cylinder topology
//================================================================================
GEOM::GEOM_Shape_ptr  GEOM_Gen_i::MakeFillet( GEOM::GEOM_Shape_ptr shape,
				              CORBA::Double radius,
				              CORBA::Short ShapeType,
					      const GEOM::GEOM_Shape::ListOfSubShapeID& ListOfID ) 
  throw (SALOME::SALOME_Exception)
{
  Unexpect aCatch(SALOME_SalomeException);
  GEOM::GEOM_Shape_var result;
  TopoDS_Shape tds ;

  const TopoDS_Shape aShape = GetTopoShape(shape) ;
  if( aShape.IsNull() ) {
    THROW_SALOME_CORBA_EXCEPTION("Shape is null", SALOME::BAD_PARAM);
  }

  BRepFilletAPI_MakeFillet fill(aShape);

  try {
    /* case all */
    if(ListOfID.length() == 0) {
      TopExp_Explorer Exp ( aShape, TopAbs_EDGE );
      for (Exp; Exp.More(); Exp.Next()) {
	TopoDS_Edge E =TopoDS::Edge(Exp.Current());
	fill.Add(E);
      }
      for (int i = 1;i<=fill.NbContours();i++) {
#if OCC_VERSION_MAJOR >= 5
	fill.SetRadius(radius,i,i);
#else
	fill.SetRadius(radius,i);
#endif
      }
      tds = fill.Shape();
      
    } else {

      /* case selection */               
      for ( unsigned int ind = 0; ind < ListOfID.length(); ind++ ) {
	TopoDS_Shape ss ;
	if( GetShapeFromIndex( aShape, (TopAbs_ShapeEnum)ShapeType, ListOfID[ind], ss ) ) {
	  TopoDS_Edge E = TopoDS::Edge(ss) ;
	  fill.Add( E );
	}
      }
      for (int i = 1;i<=fill.NbContours();i++) {
#if OCC_VERSION_MAJOR >= 5
	fill.SetRadius(radius,i,i);
#else
	fill.SetRadius(radius,i);
#endif
      }
      tds = fill.Shape();
    }
  }
  catch(Standard_Failure) {
    THROW_SALOME_CORBA_EXCEPTION("Exception catched in GEOM_Gen_i::MakeFillet", SALOME::BAD_PARAM);
  }
  
  if (tds.IsNull()) {
    THROW_SALOME_CORBA_EXCEPTION("Make Fillet aborted", SALOME::BAD_PARAM);
  } 
  result = CreateObject(tds);
  InsertInLabelOneArgument(aShape, shape, tds, result, myCurrentOCAFDoc) ;

  return result ;  
}


//================================================================================
// function : MakeChamfer
// purpose  : Create a Chamfer topology
//================================================================================
GEOM::GEOM_Shape_ptr GEOM_Gen_i::MakeChamfer( GEOM::GEOM_Shape_ptr shape,
					      CORBA::Double d1,
					      CORBA::Double d2,
					      CORBA::Short ShapeType,
					      const GEOM::GEOM_Shape::ListOfSubShapeID& ListOfID ) 
  throw (SALOME::SALOME_Exception)
{
  Unexpect aCatch(SALOME_SalomeException);
  GEOM::GEOM_Shape_var result;
  TopoDS_Shape tds ;

  const TopoDS_Shape aShape = GetTopoShape(shape) ;
  if( aShape.IsNull() ) {
    THROW_SALOME_CORBA_EXCEPTION("Shape is null", SALOME::BAD_PARAM);
  }
  
  BRepFilletAPI_MakeChamfer MC(aShape);

  try {
    /* case all */
    TopTools_IndexedDataMapOfShapeListOfShape M;
    TopExp::MapShapesAndAncestors(aShape,TopAbs_EDGE,TopAbs_FACE,M);
    if(ListOfID.length() == 0) {
      for (int i = 1;i<=M.Extent();i++) {
	TopoDS_Edge E = TopoDS::Edge(M.FindKey(i));
	TopoDS_Face F = TopoDS::Face(M.FindFromIndex(i).First());
	if (!BRepTools::IsReallyClosed(E, F) && !BRep_Tool::Degenerated(E))
	  MC.Add(d1,d2,E,F);
      }
      tds = MC.Shape();

    } else {

      /* case selection */  
      for ( unsigned int ind = 0; ind < ListOfID.length(); ind++ ) {
	TopoDS_Shape ss ;
	if( GetShapeFromIndex( aShape, (TopAbs_ShapeEnum)ShapeType, ListOfID[ind], ss ) ) {
	  TopoDS_Edge E = TopoDS::Edge( ss ) ;
	  TopoDS_Face F = TopoDS::Face(M.FindFromKey(E).First());
	  if (!BRepTools::IsReallyClosed(E, F) && !BRep_Tool::Degenerated(E))
	    MC.Add(d1,d2,E,F);
	}
      }
      tds = MC.Shape();
    }
  }
  catch(Standard_Failure) {
    THROW_SALOME_CORBA_EXCEPTION("Exception catched in GEOM_Gen_i::MakeChamfer", SALOME::BAD_PARAM);
  }
  
  if (tds.IsNull()) {
    THROW_SALOME_CORBA_EXCEPTION("Make Chamfer aborted", SALOME::BAD_PARAM);
  } 
  result = CreateObject(tds);
  InsertInLabelOneArgument(aShape, shape, tds, result, myCurrentOCAFDoc) ;

  return result ;
}

//=================================================================================
// function : CheckShape()
// purpose  :
//=================================================================================
CORBA::Boolean GEOM_Gen_i::CheckShape(GEOM::GEOM_Shape_ptr shape) 
  throw (SALOME::SALOME_Exception)
{
  Unexpect aCatch(SALOME_SalomeException);
  TopoDS_Shape S = GetTopoShape(shape) ;
  if( S.IsNull() ) {
    THROW_SALOME_CORBA_EXCEPTION("Shape is null", SALOME::BAD_PARAM);
  }

  BRepCheck_Analyzer ana(S,false);
  if (ana.IsValid()) 
    return 1;

  return 0;
}

//=================================================================================
// function : MakePlacedBox()
// purpose  :
//=================================================================================
GEOM::GEOM_Shape_ptr GEOM_Gen_i::MakePlacedBox(CORBA::Double x1,  CORBA::Double y1,  CORBA::Double z1,
					 CORBA::Double delta1, CORBA::Double delta2, CORBA::Double delta3)
  throw (SALOME::SALOME_Exception)
{
  Unexpect aCatch(SALOME_SalomeException);
  GEOM::GEOM_Shape_var result ;
  TopoDS_Shape tds ;

  CORBA::Double x2, y2, z2 ;

  try {
    x2 = x1 + delta1 ;
    y2 = y1 + delta2 ;
    z2 = z1 + delta3 ;
    
    gp_Pnt P1(x1,y1,z1);
    gp_Pnt P2(x2,y2,z2);
    
    tds = BRepPrimAPI_MakeBox(P1,P2).Shape();
  }
  catch(Standard_Failure) {
    THROW_SALOME_CORBA_EXCEPTION("Exception catched in GEOM_Gen_i::MakeBox", SALOME::BAD_PARAM);
  }
  
  if (tds.IsNull()) {
    THROW_SALOME_CORBA_EXCEPTION("Make Box aborted : null shape", SALOME::BAD_PARAM);
  } 
  
  result = CreateObject(tds);
  const char *entry = InsertInLabel(tds, result->Name(), myCurrentOCAFDoc) ;
  result->ShapeId(entry) ;

  return result;  
}

//=================================================================================
// function : MakePanel()
// purpose  :
//=================================================================================
GEOM::GEOM_Shape_ptr GEOM_Gen_i::MakePanel(GEOM::GEOM_Shape_ptr shape,
				     CORBA::Short directiontype,
				     CORBA::Double delta)
  throw (SALOME::SALOME_Exception)
{
  Unexpect aCatch(SALOME_SalomeException);
  GEOM::GEOM_Shape_var result ;
  TopoDS_Shape tds ;
  TopoDS_Shape aShape = GetTopoShape(shape) ;
  Bnd_Box B ;
  Standard_Real axmin,aymin,azmin,axmax,aymax,azmax ;
  GEOM::PointStruct pstruct1, pstruct2, pstruct3, pstruct4 ;

  if(aShape.IsNull() ) {
    THROW_SALOME_CORBA_EXCEPTION("MakePanel aborted : null shape during operation", SALOME::BAD_PARAM);
  }  

  try {
    BRepBndLib::Add(aShape,B);
    B.Enlarge(10.);
    B.Get(axmin,aymin,azmin,axmax,aymax,azmax);
    
    switch (directiontype)
      {
      case 1 :   /* X */
	pstruct1 = MakePointStruct( delta, aymin, azmin ) ;
	pstruct2 = MakePointStruct( delta, aymin, azmax ) ;
	pstruct3 = MakePointStruct( delta, aymax, azmax ) ;
	pstruct4 = MakePointStruct( delta, aymax, azmin ) ;
	break ;
      case 2 :   /* Y */
	pstruct1 = MakePointStruct( axmin, delta, azmin ) ;
	pstruct2 = MakePointStruct( axmin, delta, azmax ) ;
	pstruct3 = MakePointStruct( axmax, delta, azmax ) ;
	pstruct4 = MakePointStruct( axmax, delta, azmin ) ;
	break ;
      case 3 :   /* Z */
	pstruct1 = MakePointStruct( axmin, aymin, delta ) ;
	pstruct2 = MakePointStruct( axmin, aymax, delta ) ;
	pstruct3 = MakePointStruct( axmax, aymax, delta ) ;
	pstruct4 = MakePointStruct( axmax, aymin, delta ) ;
	break ;
      default :
	return result ;
      }
    
    GEOM::GEOM_Shape_ptr Edge1 = MakeEdge(pstruct1, pstruct2);
    GEOM::GEOM_Shape_ptr Edge2 = MakeEdge(pstruct2, pstruct3);
    GEOM::GEOM_Shape_ptr Edge3 = MakeEdge(pstruct3, pstruct4);
    GEOM::GEOM_Shape_ptr Edge4 = MakeEdge(pstruct4, pstruct1);
    
    GEOM::GEOM_Gen::ListOfIOR_var aList = new GEOM::GEOM_Gen::ListOfIOR;
    aList->length(4);
    aList[0]=CORBA::string_dup(Edge1->Name());
    aList[1]=CORBA::string_dup(Edge2->Name());
    aList[2]=CORBA::string_dup(Edge3->Name());
    aList[3]=CORBA::string_dup(Edge4->Name());
    
    GEOM::GEOM_Shape_ptr aWire = MakeWire( aList );
    GEOM::GEOM_Shape_ptr aFace = MakeFace( aWire, true ) ;
    tds = GetTopoShape(aFace);
    
  }
  catch(Standard_Failure) {
    THROW_SALOME_CORBA_EXCEPTION("Exception catched in GEOM_Gen_i::MakePanel", SALOME::BAD_PARAM);
  }
  
  if (tds.IsNull()) {
    THROW_SALOME_CORBA_EXCEPTION("Make PanelsPartition aborted : null shape", SALOME::BAD_PARAM);
  } 
  
  result = CreateObject(tds); 
  const char *entry = InsertInLabel(tds, result->Name(), myCurrentOCAFDoc) ;
  result->ShapeId(entry) ;
  
  return result;
}


void GEOM_Gen_i::ExportIGES(const char* filename,GEOM::GEOM_Shape_ptr theShape) 
  throw (SALOME::SALOME_Exception)
{
  Unexpect aCatch(SALOME_SalomeException);
  if (theShape->_is_nil()) 
    {
      THROW_SALOME_CORBA_EXCEPTION("Export IGES aborted", SALOME::BAD_PARAM);
    } 
  TopoDS_Shape tds = GetTopoShape(theShape);
  if (tds.IsNull()) 
    {
      THROW_SALOME_CORBA_EXCEPTION("Export IGES aborted", SALOME::BAD_PARAM);
    } 
  try 
    {
      //VRV: OCC 4.0 migration
      IGESControl_Controller::Init();
      IGESControl_Writer ICW (Interface_Static::CVal("XSTEP.iges.unit"),
				   Interface_Static::IVal("XSTEP.iges.writebrep.mode"));
      //VRV: OCC 4.0 migration
	
      ICW.AddShape (tds);
      ICW.ComputeModel();
      char * aname = strdup(filename);
      Standard_Boolean result = ICW.Write( aname );
      free(aname);
    }
  catch(Standard_Failure) 
    {
      THROW_SALOME_CORBA_EXCEPTION("Exception catched in GEOM_Gen_i::ExportIGES", SALOME::BAD_PARAM);
    }
}

void GEOM_Gen_i::ExportBREP(const char* filename,GEOM::GEOM_Shape_ptr theShape)
  throw (SALOME::SALOME_Exception)
{
  Unexpect aCatch(SALOME_SalomeException);
  if (theShape->_is_nil()) 
    {
      THROW_SALOME_CORBA_EXCEPTION("Export BRep aborted", SALOME::BAD_PARAM);
    } 
  TopoDS_Shape tds = GetTopoShape(theShape);
  if (tds.IsNull()) 
    {
      THROW_SALOME_CORBA_EXCEPTION("Export BRep aborted", SALOME::BAD_PARAM);
    } 
  try 
    {
      char * aname = strdup(filename);
      Standard_Boolean result = BRepTools::Write(tds,aname);
      free(aname);
    }
  catch(Standard_Failure) 
    {
      THROW_SALOME_CORBA_EXCEPTION("Exception catched in GEOM_Gen_i::ExportBREP", SALOME::BAD_PARAM);
    }
}

void GEOM_Gen_i::ExportSTEP(const char* filename,GEOM::GEOM_Shape_ptr theShape) 
  throw (SALOME::SALOME_Exception)
{
  Unexpect aCatch(SALOME_SalomeException);
  if (theShape->_is_nil()) 
    {
      THROW_SALOME_CORBA_EXCEPTION("Export STEP aborted", SALOME::BAD_PARAM);
    } 
  TopoDS_Shape tds = GetTopoShape(theShape);
  if (tds.IsNull()) 
    {
      THROW_SALOME_CORBA_EXCEPTION("Export STEP aborted", SALOME::BAD_PARAM);
    } 
  try 
    {
      IFSelect_ReturnStatus status ;
      //VRV: OCC 4.0 migration
      STEPControl_Writer aWriter;
      status = aWriter.Transfer( tds, STEPControl_ManifoldSolidBrep ) ;
      //VRV: OCC 4.0 migration
      if ( status == IFSelect_RetDone ) 
	{
	  char * aname = strdup(filename);
	  status = aWriter.Write( aname ) ;
	  free(aname);
	}
    }
  catch(Standard_Failure) 
    {
      THROW_SALOME_CORBA_EXCEPTION("Exception catched in GEOM_Gen_i::ExportBREP", SALOME::BAD_PARAM);
    }
}


//=====================================================================================
// EXPORTED METHODS
//=====================================================================================
extern "C"
{
  PortableServer::ObjectId * GEOMEngine_factory(CORBA::ORB_ptr orb,
						PortableServer::POA_ptr poa, 
						PortableServer::ObjectId * contId,
						const char *instanceName, 
						const char * interfaceName)
  {
   GEOM_Gen_i * myGEOM_Gen_i = new GEOM_Gen_i(orb, poa, contId, instanceName, interfaceName);
   myGEOM_Gen_i->register_name("/myGEOM_Gen"); // NRI : 11/07/2002 : Add for Supervision example 
   return myGEOM_Gen_i->getId() ;
  }
}

