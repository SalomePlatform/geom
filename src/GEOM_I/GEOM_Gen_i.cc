using namespace std;

#include "GEOM_Gen_i.hh"
#include "GEOM_Object_i.hh"
#include "SALOMEDS_Tool.hxx"

#include "Utils_CorbaException.hxx"
#include "OpUtil.hxx"
#include "Utils_ExceptHandlers.hxx"
#include "utilities.h"

#include "GEOM_Object_i.hh"
#include "GEOM_Object.hxx"
#include "GEOM_Function.hxx"
#include "GEOMImpl_Types.hxx"
#include "GEOMImpl_CopyDriver.hxx"

// Cascade headers
#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <TDF_Label.hxx>
#include <TDF_Tool.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TopAbs_ShapeEnum.hxx>

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
  name_service = new SALOME_NamingService(_orb);

  _impl = new ::GEOMImpl_Gen;

}



//============================================================================
// function : ~GEOM_Gen_i()
// purpose  : destructor
//============================================================================
GEOM_Gen_i::~GEOM_Gen_i() {
  delete name_service;
  delete _impl;
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
  GEOM::GEOM_Object_var anObject = GEOM::GEOM_Object::_narrow(_orb->string_to_object(IORString));
  if (!CORBA::is_nil(anObject)) {
    return strdup(anObject->GetEntry());
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
  SALOMEDS::Study_var aStudy = theSObject->GetStudy();

  Handle(GEOM_Object) anObject = _impl->GetObject(aStudy->StudyId(), const_cast<char*>(aLocalPersistentID));
  TCollection_AsciiString anEntry;  
  TDF_Tool::Entry(anObject->GetEntry(), anEntry);  
  GEOM::GEOM_Object_var obj = GetObject(anObject->GetDocID(), anEntry.ToCString());

  CORBA::String_var aPersRefString = _orb->object_to_string(obj);
  return strdup(aPersRefString);
}

//============================================================================
// function : CanPublishInStudy
// purpose  : 
//============================================================================
bool GEOM_Gen_i::CanPublishInStudy(CORBA::Object_ptr theIOR)
{
  GEOM::GEOM_Object_var anObject = GEOM::GEOM_Object::_narrow(theIOR);
  return !(anObject->_is_nil());
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
  if(CORBA::is_nil(theObject) || theStudy->_is_nil()) return aResultSO;

  GEOM::GEOM_Object_var aShape = GEOM::GEOM_Object::_narrow(theObject);
  if(aShape->_is_nil()) return aResultSO;

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
      aResultSO = SALOMEDS::SObject::_duplicate(theSObject); //SRN: Added Aug 24,2004 : for  the method AddInStudy with theFather argumenet != NULL
      //THROW_SALOME_CORBA_EXCEPTION("Publish in study supervision graph error",SALOME::BAD_PARAM);
  }
  anAttr = aStudyBuilder->FindOrCreateAttribute(aResultSO, "AttributeIOR");
  SALOMEDS::AttributeIOR_var anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
  char *aGeomObjIOR = _orb->object_to_string(theObject);
  anIOR->SetValue(strdup(aGeomObjIOR));

  anAttr = aStudyBuilder->FindOrCreateAttribute(aResultSO, "AttributePixMap");
  SALOMEDS::AttributePixMap_var aPixmap = SALOMEDS::AttributePixMap::_narrow(anAttr);
  TCollection_AsciiString aShapeName("Shape_");  

  if ( aShape->GetType() == GEOM_GROUP ) {
    GEOM::GEOM_IGroupOperations_var anOp = GetIGroupOperations( theStudy->StudyId() );
    switch ( (TopAbs_ShapeEnum)anOp->GetType( aShape ) ) {
    case TopAbs_VERTEX:
      aPixmap->SetPixMap( "ICON_OBJBROWSER_GROUP_PNT" );
      aShapeName = "Group_Of_Vertices_";
      break;
    case TopAbs_EDGE:
      aPixmap->SetPixMap( "ICON_OBJBROWSER_GROUP_EDGE" );
      aShapeName = "Group_Of_Edges_";
      break;
    case TopAbs_FACE:
      aPixmap->SetPixMap( "ICON_OBJBROWSER_GROUP_FACE" );
      aShapeName = "Group_Of_Faces_";
      break;
    case TopAbs_SOLID:
      aPixmap->SetPixMap( "ICON_OBJBROWSER_GROUP_SOLID" );
      aShapeName = "Group_Of_Solids_";
      break;
    }
  } else if ( aShape->GetType() == GEOM_MARKER ) {
    aPixmap->SetPixMap( "ICON_OBJBROWSER_LCS" );
    aShapeName = "LocalCS_";
  } else if ( aShape->GetShapeType() == GEOM::COMPOUND ) {
    aPixmap->SetPixMap( "ICON_OBJBROWSER_COMPOUND" );
    aShapeName = "Compound_";
  } else if ( aShape->GetShapeType() == GEOM::COMPSOLID ) {
    aPixmap->SetPixMap( "ICON_OBJBROWSER_COMPSOLID" );
    aShapeName = "Compsolid_";
  } else if ( aShape->GetShapeType() == GEOM::SOLID ) {
    aPixmap->SetPixMap( "ICON_OBJBROWSER_SOLID" );
    aShapeName = "Solid_";
  } else if ( aShape->GetShapeType() == GEOM::SHELL ) {
    aPixmap->SetPixMap( "ICON_OBJBROWSER_SHELL" );
    aShapeName = "Shell_";
  } else if ( aShape->GetShapeType() == GEOM::FACE ) {
    aPixmap->SetPixMap( "ICON_OBJBROWSER_FACE" );
    aShapeName = "Face_";
  } else if ( aShape->GetShapeType() == GEOM::WIRE ) {
    aPixmap->SetPixMap( "ICON_OBJBROWSER_WIRE" );
    aShapeName = "Wire_";
  } else if ( aShape->GetShapeType() == GEOM::EDGE ) {
    aPixmap->SetPixMap( "ICON_OBJBROWSER_EDGE" );
    aShapeName = "Edge_";
  } else if ( aShape->GetShapeType() == GEOM::VERTEX ) {
    aPixmap->SetPixMap( "ICON_OBJBROWSER_VERTEX" );
    aShapeName = "Vertex_";
  }                                          
  //if (strlen(theName) == 0) aShapeName += TCollection_AsciiString(aResultSO->Tag());
  //else aShapeName = TCollection_AsciiString(strdup(theName));

  // asv : 11.11.04 Introducing a more sofisticated method of name creation, just as
  //       it is done in GUI in GEOMBase::GetDefaultName() - not just add a Tag() == number
  //       of objects in the study, but compute a number of objects with the same prefix
  //       and build a new name as Prefix_N+1
  if ( strlen( theName ) == 0 ) { // MOST PROBABLY CALLED FROM BATCHMODE OR SUPERVISOR
    int i = 0;                    // (WITH EMPTY NEW NAME)
    SALOMEDS::SObject_var obj;
    TCollection_AsciiString aNewShapeName;
    do {
      aNewShapeName = aShapeName + TCollection_AsciiString(++i);
      obj = theStudy->FindObject( aNewShapeName.ToCString() );
    } 
    while ( !obj->_is_nil() );
    aShapeName = aNewShapeName;
  }
  else // MOST PROBABLY CALLED FROM GEOM GUI (ALREADY WITH VALID NAME)
    aShapeName = TCollection_AsciiString(strdup(theName));

  //Set the study entry as a name of  the published GEOM_Object
  aShape->SetStudyEntry(aResultSO->GetID());

  //Set a name of the added shape
  anAttr = aStudyBuilder->FindOrCreateAttribute(aResultSO, "AttributeName");
  SALOMEDS::AttributeName_var aNameAttrib = SALOMEDS::AttributeName::_narrow(anAttr);
  aNameAttrib->SetValue(aShapeName.ToCString());

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
  std::string aTmpDir = (isMultiFile)?theURL:SALOMEDS_Tool::GetTmpDir();
  // Create a list to store names of created files
  SALOMEDS::ListOfFileNames_var aSeq = new SALOMEDS::ListOfFileNames;
  aSeq->length(1);
  // Prepare a file name to open
  TCollection_AsciiString aNameWithExt("");
  if (isMultiFile)
    aNameWithExt = TCollection_AsciiString((char*)(SALOMEDS_Tool::GetNameFromPath(theComponent->GetStudy()->URL())).c_str());
  aNameWithExt += TCollection_AsciiString("_GEOM.sgd");
  aSeq[0] = CORBA::string_dup(aNameWithExt.ToCString());
  // Build a full file name of temporary file
  TCollection_AsciiString aFullName = TCollection_AsciiString((char*)aTmpDir.c_str()) + aNameWithExt;
  // Save GEOM component in this file
  _impl->Save(theComponent->GetStudy()->StudyId(), aFullName.ToCString());
  // Conver a file to the byte stream
  aStreamFile = SALOMEDS_Tool::PutFilesToStream(aTmpDir.c_str(), aSeq.in(), isMultiFile);
  // Remove the created file and tmp directory
  if (!isMultiFile) SALOMEDS_Tool::RemoveTemporaryFiles(aTmpDir.c_str(), aSeq.in(), true);

  // Return the created byte stream
  return aStreamFile._retn();
}


//============================================================================
// function : SaveASCII()
// purpose  :
//============================================================================ 
SALOMEDS::TMPFile* GEOM_Gen_i::SaveASCII(SALOMEDS::SComponent_ptr theComponent,
					 const char* theURL,
					 bool isMultiFile) {
  SALOMEDS::TMPFile_var aStreamFile = Save(theComponent, theURL, isMultiFile);
  return aStreamFile._retn();
}


//============================================================================
// function : Load()
// purpose  :
//============================================================================ 
CORBA::Boolean GEOM_Gen_i::Load(SALOMEDS::SComponent_ptr theComponent,
				const SALOMEDS::TMPFile& theStream,
				const char* theURL,
				bool isMultiFile) {

  if (theStream.length() <= 9) {
    MESSAGE("The TMPFile is too short : " << theStream.length() << " bytes ");
    return false;
  }

  // Get a temporary directory for a file
  std::string aTmpDir = isMultiFile?theURL:SALOMEDS_Tool::GetTmpDir();
  // Conver the byte stream theStream to a file and place it in tmp directory
  SALOMEDS::ListOfFileNames_var aSeq = SALOMEDS_Tool::PutStreamToFiles(theStream, aTmpDir.c_str(), isMultiFile);

  // Prepare a file name to open
  TCollection_AsciiString aNameWithExt("");
  if (isMultiFile)
    aNameWithExt = TCollection_AsciiString((char*)(SALOMEDS_Tool::GetNameFromPath(theComponent->GetStudy()->URL())).c_str());
  aNameWithExt += TCollection_AsciiString("_GEOM.sgd");
  TCollection_AsciiString aFullName = (TCollection_AsciiString((char*)aTmpDir.c_str()) + aNameWithExt);

  // Open document
  if (!_impl->Load(theComponent->GetStudy()->StudyId(), aFullName.ToCString())) return false;

  // Remove the created file and tmp directory
  if (!isMultiFile) SALOMEDS_Tool::RemoveTemporaryFiles(aTmpDir.c_str(), aSeq.in(), true);

  SALOMEDS::Study_var Study = theComponent->GetStudy();
  TCollection_AsciiString name( strdup(Study->Name()) );

  return true;
}


//============================================================================
// function : LoadASCII()
// purpose  :
//============================================================================ 
CORBA::Boolean GEOM_Gen_i::LoadASCII(SALOMEDS::SComponent_ptr theComponent,
				     const SALOMEDS::TMPFile& theStream,
				     const char* theURL,
				     bool isMultiFile) {
  return Load(theComponent, theStream, theURL, isMultiFile);
}


//============================================================================
// function : Close()
// purpose  :
//============================================================================
void GEOM_Gen_i::Close(SALOMEDS::SComponent_ptr theComponent)
{
  _impl->Close(theComponent->GetStudy()->StudyId());
}

//============================================================================
// function : CanCopy()
// purpose  :
//============================================================================
CORBA::Boolean GEOM_Gen_i::CanCopy(SALOMEDS::SObject_ptr theObject) {
  // Try to retrieve known by Geometry component GEOM_Object by given IOR
  SALOMEDS::GenericAttribute_var anAttr;
  if (!theObject->FindAttribute(anAttr, "AttributeIOR")) return false;

  SALOMEDS::AttributeIOR_var anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);

  GEOM::GEOM_Object_var anObject = GEOM::GEOM_Object::_narrow(_orb->string_to_object(anIOR->Value()));
  // If the object is null one it can't be copied: return false
  if (anObject->_is_nil()) return false;
  return true;
}

//============================================================================
// function : CopyFrom()
// purpose  :
//============================================================================
SALOMEDS::TMPFile* GEOM_Gen_i::CopyFrom(SALOMEDS::SObject_ptr theObject, CORBA::Long& theObjectID) {
  // Declare a sequence of the byte to store the copied object
  SALOMEDS::TMPFile_var aStreamFile = new SALOMEDS::TMPFile;

  // Try to get GEOM_Object object by given SObject
  SALOMEDS::GenericAttribute_var anAttr;
  if (!theObject->FindAttribute(anAttr, "AttributeIOR")) return aStreamFile._retn();
  GEOM::GEOM_Object_var anObject =  GEOM::GEOM_Object::_narrow(_orb->string_to_object(SALOMEDS::AttributeIOR::_narrow(anAttr)->Value()));
  if (anObject->_is_nil()) return aStreamFile._retn();

  aStreamFile = anObject->GetShapeStream();
  
  // Assign an ID  the type of  GEOM_Object
  theObjectID = anObject->GetType();

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
  if (strcmp(theComponentName, ComponentDataType()) != 0) return false;
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
    return false;
  }
  
  // SObject of the created shape is theObject or new Child of Component if theObject == geom component
  SALOMEDS::SObject_var aNewSO;
  if (strcmp(theObject->GetFatherComponent()->GetID(),theObject->GetID()) == 0) {
    aNewSO = aStudyBuilder->NewObject(theObject);
  } else aNewSO = SALOMEDS::SObject::_duplicate(theObject);

  
  //Create a new GEOM_Object 
  Handle(GEOM_Object) anObj = _impl->AddObject(aNewSO->GetStudy()->StudyId(), theObjectID);  
  Handle(GEOM_Function) aFunction = anObj->AddFunction(GEOMImpl_CopyDriver::GetID(), COPY_WITHOUT_REF);  
  aFunction->SetValue(aTopology);

  TCollection_AsciiString anEntry;  
  TDF_Tool::Entry(anObj->GetEntry(), anEntry);  
  GEOM::GEOM_Object_var obj = GetObject(anObj->GetDocID(), anEntry.ToCString());

  // Add IORAttribute to the Study and set IOR of the created GEOM_Object to it
  SALOMEDS::GenericAttribute_var anAttr = aStudyBuilder->FindOrCreateAttribute(aNewSO, "AttributeIOR");
  SALOMEDS::AttributeIOR_var anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
  anIOR->SetValue(_orb->object_to_string(obj));

  // Return the created in the Study SObject
  return aNewSO._retn();
}

//============================================================================
// function : ComponentDataType()
// purpose  :
//============================================================================
char* GEOM_Gen_i::ComponentDataType()
{
  return strdup("GEOM");
}

//============================================================================
// function : AddInStudy
// purpose  :
//============================================================================
SALOMEDS::SObject_ptr GEOM_Gen_i::AddInStudy(SALOMEDS::Study_ptr theStudy, GEOM::GEOM_Object_ptr theObject, const char* theName, GEOM::GEOM_Object_ptr theFather)
{
  SALOMEDS::SObject_var aResultSO;
  if(theObject->_is_nil() || theStudy->_is_nil()) return aResultSO;
    
  SALOMEDS::StudyBuilder_var aStudyBuilder = theStudy->NewBuilder(); 
  char* IOR;

  if(!theFather->_is_nil()) {
    IOR = _orb->object_to_string(theFather); 
    SALOMEDS::SObject_var aFatherSO = theStudy->FindObjectIOR(IOR);
    if(aFatherSO->_is_nil()) return aResultSO._retn();
    aResultSO = aStudyBuilder->NewObject(aFatherSO);
    //aStudyBuilder->Addreference(aResultSO, aResultSO);
  }   

  aResultSO = PublishInStudy(theStudy, aResultSO, theObject, theName);
  if(aResultSO->_is_nil()) return aResultSO._retn();

  GEOM::ListOfGO_var aList = theObject->GetDependency();
  Standard_Integer aLength = aList->length(); 
  if(aLength < 1) return aResultSO._retn();

  //Publish the arguments  
  for(Standard_Integer i = 0; i< aLength; i++) {
    GEOM::GEOM_Object_var anObject = aList[i];
    if(anObject->_is_nil()) continue;
    IOR = _orb->object_to_string(anObject);
    SALOMEDS::SObject_var aSO =  theStudy->FindObjectIOR(IOR);
    if(aSO->_is_nil()) continue;
    SALOMEDS::SObject_var aSubSO = aStudyBuilder->NewObject(aResultSO);
    aStudyBuilder->Addreference(aSubSO, aSO);
  }
  
  return aResultSO._retn();
}

//============================================================================
// function : register() 
// purpose  : register 'name' in 'name_service'
//============================================================================
void GEOM_Gen_i::register_name(char * name)
{
  GEOM::GEOM_Gen_ptr g = GEOM::GEOM_Gen::_narrow(POA_GEOM::GEOM_Gen::_this());
  name_service->Register(g, strdup(name)); 
}

//============================================================================
// function : Undo
// purpose  :
//============================================================================
void GEOM_Gen_i::Undo(CORBA::Long theStudyID)
{
  _impl->Undo(theStudyID);
}

//============================================================================
// function : Redo
// purpose  :
//============================================================================
void GEOM_Gen_i::Redo(CORBA::Long theStudyID)
{
  _impl->Redo(theStudyID);
}

//============================================================================
// function : GetIBasicOperations
// purpose  : 
//============================================================================
GEOM::GEOM_IBasicOperations_ptr GEOM_Gen_i::GetIBasicOperations(CORBA::Long theStudyID)
     throw ( SALOME::SALOME_Exception )
{
  Unexpect aCatch(SALOME_SalomeException);
  MESSAGE( "GEOM_Gen_i::GetIBasicOperations" );

  GEOM::GEOM_Gen_ptr engine = POA_GEOM::GEOM_Gen::_this(); 

  GEOM_IBasicOperations_i* aServant =
    new GEOM_IBasicOperations_i(_poa, engine, _impl->GetIBasicOperations(theStudyID));

  // activate the CORBA servant 
  GEOM::GEOM_IBasicOperations_var operations = aServant->_this();
  return operations._retn();
}

//============================================================================
// function : GetITransformOperations
// purpose  : 
//============================================================================
GEOM::GEOM_ITransformOperations_ptr GEOM_Gen_i::GetITransformOperations(CORBA::Long theStudyID)
     throw ( SALOME::SALOME_Exception )
{
  Unexpect aCatch(SALOME_SalomeException);
  MESSAGE( "GEOM_Gen_i::GetITransformOperations" );

  GEOM::GEOM_Gen_ptr engine = POA_GEOM::GEOM_Gen::_this(); 

  GEOM_ITransformOperations_i* aServant =
    new GEOM_ITransformOperations_i(_poa, engine, _impl->GetITransformOperations(theStudyID));

  // activate the CORBA servant 
  GEOM::GEOM_ITransformOperations_var operations = aServant->_this();
  return operations._retn();
}

//============================================================================
// function : GetI3DPrimOperations
// purpose  : 
//============================================================================
GEOM::GEOM_I3DPrimOperations_ptr GEOM_Gen_i::GetI3DPrimOperations(CORBA::Long theStudyID)
     throw ( SALOME::SALOME_Exception )
{
  Unexpect aCatch(SALOME_SalomeException);
  MESSAGE( "GEOM_Gen_i::GetI3DPrimOperations" );

  GEOM::GEOM_Gen_ptr engine = POA_GEOM::GEOM_Gen::_this(); 

  GEOM_I3DPrimOperations_i* aServant =
    new GEOM_I3DPrimOperations_i(_poa, engine, _impl->GetI3DPrimOperations(theStudyID));

  // activate the CORBA servant 
  GEOM::GEOM_I3DPrimOperations_var operations = aServant->_this();
  return operations._retn();
}

//============================================================================
// function : GetIShapesOperations
// purpose  : 
//============================================================================
GEOM::GEOM_IShapesOperations_ptr GEOM_Gen_i::GetIShapesOperations(CORBA::Long theStudyID)
     throw ( SALOME::SALOME_Exception )
{
  Unexpect aCatch(SALOME_SalomeException);
  MESSAGE( "GEOM_Gen_i::GetIShapesOperations" );

  GEOM::GEOM_Gen_ptr engine = POA_GEOM::GEOM_Gen::_this(); 

  GEOM_IShapesOperations_i* aServant =
    new GEOM_IShapesOperations_i(_poa, engine, _impl->GetIShapesOperations(theStudyID));

  // activate the CORBA servant 
  GEOM::GEOM_IShapesOperations_var operations = aServant->_this();
  return operations._retn();
}

//============================================================================
// function : GetIBlocksOperations
// purpose  : 
//============================================================================
GEOM::GEOM_IBlocksOperations_ptr GEOM_Gen_i::GetIBlocksOperations(CORBA::Long theStudyID)
     throw ( SALOME::SALOME_Exception )
{
  Unexpect aCatch(SALOME_SalomeException);
  MESSAGE( "GEOM_Gen_i::GetIBlocksOperations" );

  GEOM::GEOM_Gen_ptr engine = POA_GEOM::GEOM_Gen::_this(); 

  GEOM_IBlocksOperations_i* aServant =
    new GEOM_IBlocksOperations_i(_poa, engine, _impl->GetIBlocksOperations(theStudyID));

  // activate the CORBA servant 
  GEOM::GEOM_IBlocksOperations_var operations = aServant->_this();
  return operations._retn();
}

//============================================================================
// function : GetIBooleanOperations
// purpose  : 
//============================================================================
GEOM::GEOM_IBooleanOperations_ptr GEOM_Gen_i::GetIBooleanOperations(CORBA::Long theStudyID)
     throw ( SALOME::SALOME_Exception )
{
  Unexpect aCatch(SALOME_SalomeException);
  MESSAGE( "GEOM_Gen_i::GetIBooleanOperations" );

  GEOM::GEOM_Gen_ptr engine = POA_GEOM::GEOM_Gen::_this(); 

  GEOM_IBooleanOperations_i* aServant =
    new GEOM_IBooleanOperations_i(_poa, engine, _impl->GetIBooleanOperations(theStudyID));

  // activate the CORBA servant 
  GEOM::GEOM_IBooleanOperations_var operations = aServant->_this();
  return operations._retn();
}

//============================================================================
// function : GetICurvesOperations
// purpose  : 
//============================================================================
GEOM::GEOM_ICurvesOperations_ptr GEOM_Gen_i::GetICurvesOperations(CORBA::Long theStudyID)
     throw ( SALOME::SALOME_Exception )
{
  Unexpect aCatch(SALOME_SalomeException);
  MESSAGE( "GEOM_Gen_i::GetICurvesOperations" );

  GEOM::GEOM_Gen_ptr engine = POA_GEOM::GEOM_Gen::_this(); 

  GEOM_ICurvesOperations_i* aServant =
    new GEOM_ICurvesOperations_i(_poa, engine, _impl->GetICurvesOperations(theStudyID));

  // activate the CORBA servant 
  GEOM::GEOM_ICurvesOperations_var operations = aServant->_this();
  return operations._retn();
}

//============================================================================
// function : GetILocalOperations
// purpose  : 
//============================================================================
GEOM::GEOM_ILocalOperations_ptr GEOM_Gen_i::GetILocalOperations(CORBA::Long theStudyID)
     throw ( SALOME::SALOME_Exception )
{
  Unexpect aCatch(SALOME_SalomeException);
  MESSAGE( "GEOM_Gen_i::GetILocalOperations" );

  GEOM::GEOM_Gen_ptr engine = POA_GEOM::GEOM_Gen::_this(); 

  GEOM_ILocalOperations_i* aServant =
    new GEOM_ILocalOperations_i(_poa, engine, _impl->GetILocalOperations(theStudyID));

  // activate the CORBA servant 
  GEOM::GEOM_ILocalOperations_var operations = aServant->_this();
  return operations._retn();
}

//============================================================================
// function : GetIHealingOperations
// purpose  : 
//============================================================================
GEOM::GEOM_IHealingOperations_ptr GEOM_Gen_i::GetIHealingOperations(CORBA::Long theStudyID)
     throw ( SALOME::SALOME_Exception )
{
  Unexpect aCatch(SALOME_SalomeException);
  MESSAGE( "GEOM_Gen_i::IHealingOperations" );

  GEOM::GEOM_Gen_ptr engine = POA_GEOM::GEOM_Gen::_this(); 

  GEOM_IHealingOperations_i* aServant =
    new GEOM_IHealingOperations_i(_poa, engine, _impl->GetIHealingOperations(theStudyID));

  // activate the CORBA servant 
  GEOM::GEOM_IHealingOperations_var operations = aServant->_this();
  return operations._retn();
}

//============================================================================
// function : GetIInsertOperations
// purpose  : 
//============================================================================
GEOM::GEOM_IInsertOperations_ptr GEOM_Gen_i::GetIInsertOperations(CORBA::Long theStudyID)
     throw ( SALOME::SALOME_Exception )
{
  Unexpect aCatch(SALOME_SalomeException);
  MESSAGE( "GEOM_Gen_i::GetIInsertOperations" );

  GEOM::GEOM_Gen_ptr engine = POA_GEOM::GEOM_Gen::_this(); 

  GEOM_IInsertOperations_i* aServant =
    new GEOM_IInsertOperations_i(_poa, engine, _impl->GetIInsertOperations(theStudyID));

  // activate the CORBA servant 
  GEOM::GEOM_IInsertOperations_var operations = aServant->_this();
  return operations._retn();
}

//============================================================================
// function : GetIMeasureOperations
// purpose  : 
//============================================================================
GEOM::GEOM_IMeasureOperations_ptr GEOM_Gen_i::GetIMeasureOperations(CORBA::Long theStudyID)
     throw ( SALOME::SALOME_Exception )
{
  Unexpect aCatch(SALOME_SalomeException);
  MESSAGE( "GEOM_Gen_i::GetIMeasureOperations" );

  GEOM::GEOM_Gen_ptr engine = POA_GEOM::GEOM_Gen::_this(); 

  GEOM_IMeasureOperations_i* aServant =
    new GEOM_IMeasureOperations_i(_poa, engine, _impl->GetIMeasureOperations(theStudyID));

  // activate the CORBA servant 
  GEOM::GEOM_IMeasureOperations_var operations = aServant->_this();
  return operations._retn();
}

//============================================================================
// function : GetIGroupOperations
// purpose  : 
//============================================================================
GEOM::GEOM_IGroupOperations_ptr GEOM_Gen_i::GetIGroupOperations(CORBA::Long theStudyID)
     throw ( SALOME::SALOME_Exception )
{
  Unexpect aCatch(SALOME_SalomeException);
  MESSAGE( "GEOM_Gen_i::GetIGroupOperations" );

  GEOM::GEOM_Gen_ptr engine = POA_GEOM::GEOM_Gen::_this(); 

  GEOM_IGroupOperations_i* aServant =
    new GEOM_IGroupOperations_i(_poa, engine, _impl->GetIGroupOperations(theStudyID));

  // activate the CORBA servant 
  GEOM::GEOM_IGroupOperations_var operations = aServant->_this();
  return operations._retn();
}

//=============================================================================
/*!
 *  AddSubShape
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Gen_i::AddSubShape(GEOM::GEOM_Object_ptr theMainShape, const GEOM::ListOfLong& theIndices)
{
  if(theMainShape == NULL || theIndices.length() < 1) return GEOM::GEOM_Object::_nil();
  Handle(GEOM_Object) aMainsShape = _impl->GetObject(theMainShape->GetStudyID(), theMainShape->GetEntry());
  if (aMainsShape.IsNull()) return GEOM::GEOM_Object::_nil();
  
  Handle(TColStd_HArray1OfInteger) anArray = new TColStd_HArray1OfInteger(1, theIndices.length());
  for(Standard_Integer i = 0; i<theIndices.length(); i++) anArray->SetValue(i+1, theIndices[i]);

  Handle(GEOM_Object) anObject = _impl->AddSubShape(aMainsShape, anArray);
  if(anObject.IsNull()) return GEOM::GEOM_Object::_nil();

  TCollection_AsciiString anEntry;
  TDF_Tool::Entry(anObject->GetEntry(), anEntry);
  return GetObject(anObject->GetDocID(), anEntry.ToCString());
}

//=============================================================================
/*!
 *  RemoveObject
 */
//=============================================================================
void GEOM_Gen_i::RemoveObject(GEOM::GEOM_Object_ptr theObject)         
{
  Handle(GEOM_Object) anObject = _impl->GetObject(theObject->GetStudyID(), theObject->GetEntry());
  if (anObject.IsNull()) return;
  _impl->RemoveObject(anObject);
  return; 
}  


//=================================================================================
// function : GetStringFromIOR()
// purpose  : returns a string that represents  a 'GEOM::GEOM_Object_var'
//=================================================================================
char* GEOM_Gen_i::GetStringFromIOR(GEOM::GEOM_Object_ptr theObject) {
  return _orb->object_to_string(theObject);
}


//=================================================================================
// function : GetIORFromString()
// purpose  : returns a 'GEOM::GEOM_Object_var' from a string representing it
//=================================================================================
GEOM::GEOM_Object_ptr GEOM_Gen_i::GetIORFromString(const char* stringIOR) {
  GEOM::GEOM_Object_var aGeomObject;
  if(strcmp(stringIOR,"") != 0){
    CORBA::Object_var anObject = _orb->string_to_object(stringIOR);
    if(!CORBA::is_nil(anObject))
      aGeomObject =  GEOM::GEOM_Object::_narrow(anObject.in()) ;
  }
  return aGeomObject._retn() ;
}

//=================================================================================
// function : GetObject()
// purpose  : 
//=================================================================================
GEOM::GEOM_Object_ptr GEOM_Gen_i::GetObject(CORBA::Long theStudyID, const char* theEntry)
{
  GEOM::GEOM_Object_var obj;
  Handle(GEOM_Object) handle_object = _impl->GetObject(theStudyID, (char*)theEntry);
  if(handle_object.IsNull()) return NULL;
  TCollection_AsciiString stringIOR = handle_object->GetIOR();
  if(stringIOR.Length() > 1) {
    CORBA::Object_var corba_object = _orb->string_to_object(stringIOR.ToCString());
    if(!CORBA::is_nil(corba_object)) obj = GEOM::GEOM_Object::_narrow(corba_object);
    return obj._retn();  
   }

  GEOM::GEOM_Gen_ptr engine = POA_GEOM::GEOM_Gen::_this(); 
  GEOM_Object_i* servant = new GEOM_Object_i(_poa, engine, handle_object);
  
  obj = servant->_this();
  stringIOR =  _orb->object_to_string(obj);
  handle_object->SetIOR(stringIOR);
  return obj._retn(); 
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

