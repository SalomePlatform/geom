// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifdef WIN32
#pragma warning( disable:4786 )
#endif

#include <Standard_Stream.hxx>

#include "GEOM_Gen_i.hh"
#include "GEOM_Object_i.hh"
#include "GEOM_Field_i.hh"
#include "GEOM_version.h"

#include "Utils_CorbaException.hxx"
#include "OpUtil.hxx"
#include "Utils_ExceptHandlers.hxx"
#include "utilities.h"

#include "GEOM_Object.hxx"
#include "GEOM_Function.hxx"
#include "GEOM_ISubShape.hxx"
#include "GEOM_PythonDump.hxx"
#include "GEOMImpl_Types.hxx"
#include "GEOMImpl_CopyDriver.hxx"
#include "GEOMImpl_IInsertOperations.hxx"
#include "GEOM_wrap.hxx"
#include "GEOMUtils_XmlHandler.hxx"

// Cascade headers
#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <TDF_Label.hxx>
#include <TDF_Tool.hxx>
#include <TDF_ChildIDIterator.hxx>
#include <TNaming_NamedShape.hxx>
#include <TDataStd_Name.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp.hxx>
#include <OSD.hxx>
#include <TDataStd_ChildNodeIterator.hxx>
#include <TDocStd_Owner.hxx>
#include <TDataStd_ListIteratorOfListOfExtendedString.hxx>

#include <SALOMEDS_Tool.hxx>
#include <SALOMEDS_wrap.hxx>
#include <Basics_DirUtils.hxx>

#include <set>
#include <sstream>

#ifdef WIN32
 #include <windows.h>
 #include <process.h>
#else
 #include <dlfcn.h>
#endif

#ifdef WIN32
 #define LibHandle HMODULE
 #define LoadLib( name ) LoadLibrary( name )
 #define GetProc GetProcAddress
 #define UnLoadLib( handle ) FreeLibrary( handle );
#else
 #define LibHandle void*
 #define LoadLib( name ) dlopen( name, RTLD_LAZY | RTLD_GLOBAL )
 #define GetProc dlsym
 #define UnLoadLib( handle ) dlclose( handle );
#endif

//============================================================================
// function : GEOM_Gen_i()
// purpose  : constructor to be called for servant creation.
//============================================================================
GEOM_Gen_i::GEOM_Gen_i(CORBA::ORB_ptr            orb,
                       PortableServer::POA_ptr   poa,
                       PortableServer::ObjectId* contId,
                       const char*               instanceName,
                       const char*               interfaceName) :
  Engines_Component_i(orb, poa, contId, instanceName, interfaceName)
{
  _thisObj = this;
  _id = _poa->activate_object(_thisObj);
  name_service = new SALOME_NamingService(_orb);

  _impl = new ::GEOMImpl_Gen;

  //PAL10867: disable signals catching with "noexcepthandler" option
  char* envNoCatchSignals = getenv("NOT_INTERCEPT_SIGNALS");
  if (!envNoCatchSignals || !atoi(envNoCatchSignals))
  {
    //work around PAL12004, PAL12628
    //OSD::SetSignal( true );
    bool raiseFPE = false;
#if defined(_DEBUG_) | defined(_DEBUG) //the Last for WIN32 default settings
    char* envEnableFPE = getenv("ENABLE_FPE");
    if (envEnableFPE && atoi(envEnableFPE))
      raiseFPE = true;
#endif
    OSD::SetSignal( raiseFPE );
  }

  GEOMUtils::PluginInfo plugins = GEOMUtils::ReadPluginInfo();
  GEOMUtils::PluginInfo::const_iterator it;
  for (it = plugins.begin(); it != plugins.end(); ++it)
  {
    try {
      LoadPlugin((*it).serverLib);
    }
    catch (...)  {
      MESSAGE("Warning: can't load plugin library " << (*it).serverLib);
    }
  }
}

//============================================================================
// function : ~GEOM_Gen_i()
// purpose  : destructor
//============================================================================
GEOM_Gen_i::~GEOM_Gen_i() {
  delete name_service;
  delete _impl;
  std::map<std::string, GEOM_GenericOperationsCreator*>::const_iterator it;
  for ( it = myOpCreatorMap.begin(); it != myOpCreatorMap.end(); ++it)
    delete (*it).second;
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
  GEOM::GEOM_BaseObject_var anObject =
    GEOM::GEOM_BaseObject::_narrow(_orb->string_to_object(IORString));
  if (!CORBA::is_nil(anObject)) {
    return CORBA::string_dup(anObject->GetEntry());
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

  HANDLE_NAMESPACE(GEOM_BaseObject) anObject =
    _impl->GetObject(aStudy->StudyId(), aLocalPersistentID);
  if ( !anObject.IsNull() )
  {
    TCollection_AsciiString anEntry;
    TDF_Tool::Entry(anObject->GetEntry(), anEntry);
    GEOM::GEOM_BaseObject_var obj = GetObject(anObject->GetDocID(), anEntry.ToCString());

    CORBA::String_var aPersRefString = _orb->object_to_string(obj);
    return CORBA::string_dup(aPersRefString);
  }
  else
  {
    return CORBA::string_dup("");
  }
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
SALOMEDS::SObject_ptr GEOM_Gen_i::PublishInStudy(SALOMEDS::Study_ptr   theStudy,
                                                 SALOMEDS::SObject_ptr theSObject,
                                                 CORBA::Object_ptr     theObject,
                                                 const char*           theName)
  throw (SALOME::SALOME_Exception)
{
  Unexpect aCatch(SALOME_SalomeException);
  SALOMEDS::SObject_var aResultSO;
  if(CORBA::is_nil(theObject) || theStudy->_is_nil()) return aResultSO;
  GEOM::GEOM_BaseObject_var aBaseObj = GEOM::GEOM_BaseObject::_narrow(theObject);
  GEOM::GEOM_Object_var       aShape = GEOM::GEOM_Object::_narrow(theObject);
  if(aBaseObj->_is_nil()) return aResultSO;

  SALOMEDS::GenericAttribute_var anAttr;
  SALOMEDS::StudyBuilder_var     aStudyBuilder = theStudy->NewBuilder();
  SALOMEDS::UseCaseBuilder_wrap  useCaseBuilder = theStudy->GetUseCaseBuilder();

  SALOMEDS::SComponent_var       aFather = theStudy->FindComponent("GEOM");
  if (aFather->_is_nil()) {
    aFather = aStudyBuilder->NewComponent("GEOM");
    anAttr = aStudyBuilder->FindOrCreateAttribute(aFather, "AttributeName");
    SALOMEDS::AttributeName_var aName = SALOMEDS::AttributeName::_narrow(anAttr);
    aName->SetValue("Geometry");
    aName->UnRegister();
    anAttr = aStudyBuilder->FindOrCreateAttribute(aFather, "AttributePixMap");
    SALOMEDS::AttributePixMap_var aPixMap=SALOMEDS::AttributePixMap::_narrow(anAttr);
    aPixMap->SetPixMap("ICON_OBJBROWSER_Geometry");
    aPixMap->UnRegister();
    aStudyBuilder->DefineComponentInstance(aFather, (GEOM::GEOM_Gen_var)GEOM_Gen::_this());
    // add component to the use case tree
    // (to support tree representation customization and drag-n-drop)
    useCaseBuilder->SetRootCurrent();
    useCaseBuilder->Append( aFather ); // component object is added as the top level item
  }
  if (aFather->_is_nil()) return aResultSO;

  if (CORBA::is_nil(theSObject)) {
    aResultSO = aStudyBuilder->NewObject(aFather);
  } else {
    if (!theSObject->ReferencedObject(aResultSO))
      aResultSO = SALOMEDS::SObject::_duplicate(theSObject); //SRN: Added Aug 24,2004 : for  the method AddInStudy with theFather argumenet != NULL
    //THROW_SALOME_CORBA_EXCEPTION("Publish in study supervision graph error",SALOME::BAD_PARAM);
  }
  CORBA::String_var aGeomObjIOR = _orb->object_to_string(theObject);
  aResultSO->SetAttrString("AttributeIOR",aGeomObjIOR);

  TCollection_AsciiString anObjectName, aNamePrefix("Shape_");
  CORBA::Long mytype=aBaseObj->GetType();

  // BEGIN: try to find existed name for current shape
  if ( !aShape->_is_nil() && mytype != GEOM_GROUP)
  {
    // recieve current TopoDS shape
    CORBA::String_var entry = aShape->GetEntry();
    HANDLE_NAMESPACE(GEOM_Object) aGShape = HANDLE_NAMESPACE(GEOM_Object)::DownCast
      ( _impl->GetObject( aShape->GetStudyID(), entry ));
    TopoDS_Shape TopoSh = aGShape->GetValue();
    // find label of main shape
    GEOM::GEOM_Object_var aMainSh = aShape;
    while( !aMainSh->IsMainShape() ) {
      aMainSh = aMainSh->GetMainShape();
    }
    entry = aMainSh->GetEntry();
    HANDLE_NAMESPACE(GEOM_BaseObject) anObj = _impl->GetObject( aMainSh->GetStudyID(), entry );
    TDF_Label aMainLbl = anObj->GetFunction(1)->GetNamingEntry();

    // check all named shapes using iterator
    TDF_ChildIDIterator anIt (aMainLbl, TNaming_NamedShape::GetID(), Standard_True);

    for (; anIt.More() && anObjectName.IsEmpty(); anIt.Next()) {
      Handle(TNaming_NamedShape) anAttr =
        Handle(TNaming_NamedShape)::DownCast(anIt.Value());
      if (anAttr.IsNull()) continue;
      TopoDS_Shape S = anAttr->Get();
      if (S.IsEqual(TopoSh)) {
        TDF_Label L = anAttr->Label();
        Handle(TDataStd_Name) aName;
        if (L.FindAttribute(TDataStd_Name::GetID(), aName))
          anObjectName = aName->Get();
      }
    }
  }
  // END: try to find existed name for current shape

  if ( mytype == GEOM_GROUP ) {
    GEOM::GEOM_IGroupOperations_var anOp = GetIGroupOperations( theStudy->StudyId() );
    switch ( (TopAbs_ShapeEnum)anOp->GetType( aShape )) {
    case TopAbs_VERTEX:
      aResultSO->SetAttrString("AttributePixMap","ICON_OBJBROWSER_GROUP_PNT" );
      aNamePrefix = "Group_Of_Vertices_";
      break;
    case TopAbs_EDGE:
      aResultSO->SetAttrString("AttributePixMap","ICON_OBJBROWSER_GROUP_EDGE");
      aNamePrefix = "Group_Of_Edges_";
      break;
    case TopAbs_FACE:
      aResultSO->SetAttrString("AttributePixMap","ICON_OBJBROWSER_GROUP_FACE");
      aNamePrefix = "Group_Of_Faces_";
      break;
    case TopAbs_SOLID:
      aResultSO->SetAttrString("AttributePixMap","ICON_OBJBROWSER_GROUP_SOLID");
      aNamePrefix = "Group_Of_Solids_";
      break;
    }
  } else if ( mytype == GEOM_MARKER ) {
    aResultSO->SetAttrString("AttributePixMap","ICON_OBJBROWSER_LCS");
    aNamePrefix = "LocalCS_";
  }  else if ( mytype >= USER_TYPE_EX ) {
    char buf[20];
    sprintf( buf, "%d", aBaseObj->GetType() );
    GEOM::CreationInformationSeq_var infoSeq = aBaseObj->GetCreationInformation();
    std::string plgId;
    for ( size_t j = 0; j < infoSeq->length(); ++j )
      for ( size_t i = 0; i < infoSeq[j].params.length(); ++i ) {
        std::string param_name  = infoSeq[j].params[i].name.in();
        std::string param_value = infoSeq[j].params[i].value.in();
        if( param_name == PLUGIN_NAME) {
          plgId = param_value;
          break;
        }
      }
    if(plgId.length() > 0 ) {
      plgId += "::";
    }
    plgId +="ICON_OBJBROWSER_";
    plgId += buf;
    aResultSO->SetAttrString("AttributePixMap",plgId.c_str());
  } else if ( mytype > USER_TYPE ) {
    char buf[20];
    sprintf( buf, "%d", aBaseObj->GetType() );
    std::string advId = "ICON_OBJBROWSER_ADVANCED_"; advId += buf;
    aResultSO->SetAttrString("AttributePixMap",advId.c_str());
    aNamePrefix = "Advanced_";
  } else if ( mytype == GEOM_FIELD ) {
    aNamePrefix = "Field_";
    GEOM::GEOM_Field_var aField = GEOM::GEOM_Field::_narrow(theObject);
    if ( !aField->_is_nil() )
      switch( aField->GetDimension() ) {
      case 0:
        aResultSO->SetAttrString("AttributePixMap","ICON_OBJBROWSER_FIELD_PNT" ); break;
      case 1:
        aResultSO->SetAttrString("AttributePixMap","ICON_OBJBROWSER_FIELD_EDGE"); break;
      case 2:
        aResultSO->SetAttrString("AttributePixMap","ICON_OBJBROWSER_FIELD_FACE"); break;
      case 3:
        aResultSO->SetAttrString("AttributePixMap","ICON_OBJBROWSER_FIELD_SOLID"); break;
      default:
        aResultSO->SetAttrString("AttributePixMap","ICON_OBJBROWSER_FIELD_SOLID");
      }
  } else if ( mytype == GEOM_FIELD_STEP ) {
    aNamePrefix = "Step_";
  } else if ( !aShape->_is_nil() ) {
    GEOM::shape_type myshapetype=aShape->GetShapeType();
    if ( myshapetype == GEOM::COMPOUND ) {
      aResultSO->SetAttrString("AttributePixMap","ICON_OBJBROWSER_COMPOUND" );
      aNamePrefix = "Compound_";
    } else if ( myshapetype == GEOM::COMPSOLID ) {
      aResultSO->SetAttrString("AttributePixMap","ICON_OBJBROWSER_COMPSOLID");
      aNamePrefix = "Compsolid_";
    } else if ( myshapetype == GEOM::SOLID ) {
      aResultSO->SetAttrString("AttributePixMap","ICON_OBJBROWSER_SOLID");
      aNamePrefix = "Solid_";
    } else if ( myshapetype == GEOM::SHELL ) {
      aResultSO->SetAttrString("AttributePixMap","ICON_OBJBROWSER_SHELL");
      aNamePrefix = "Shell_";
    } else if ( myshapetype == GEOM::FACE ) {
      aResultSO->SetAttrString("AttributePixMap","ICON_OBJBROWSER_FACE");
      aNamePrefix = "Face_";
    } else if ( myshapetype == GEOM::WIRE ) {
      aResultSO->SetAttrString("AttributePixMap","ICON_OBJBROWSER_WIRE");
      aNamePrefix = "Wire_";
    } else if ( myshapetype == GEOM::EDGE ) {
      aResultSO->SetAttrString("AttributePixMap", "ICON_OBJBROWSER_EDGE");
      aNamePrefix = "Edge_";
    } else if ( myshapetype == GEOM::VERTEX ) {
      aResultSO->SetAttrString("AttributePixMap","ICON_OBJBROWSER_VERTEX" );
      aNamePrefix = "Vertex_";
    }
  }
  if ( anObjectName.IsEmpty() )
  {
    //if (strlen(theName) == 0) aNamePrefix += TCollection_AsciiString(aResultSO->Tag());
    //else anObjectName = TCollection_AsciiString(CORBA::string_dup(theName));

    // asv : 11.11.04 Introducing a more sofisticated method of name creation, just as
    //       it is done in GUI in GEOMBase::GetDefaultName() - not just add a Tag() == number
    //       of objects in the study, but compute a number of objects with the same prefix
    //       and build a new name as Prefix_N+1
    if ( strlen( theName ) == 0 ) { // MOST PROBABLY CALLED FROM BATCHMODE OR SUPERVISOR
      int i = 0;                    // (WITH EMPTY NEW NAME)
      SALOMEDS::SObject_var obj;
      do {
        anObjectName = aNamePrefix + TCollection_AsciiString(++i);
        obj = theStudy->FindObject( anObjectName.ToCString() );
      }
      while ( !obj->_is_nil() );
    }
    else { // MOST PROBABLY CALLED FROM GEOM GUI (ALREADY WITH VALID NAME)
      anObjectName = theName;
    }
  }

  //Set the study entry as a name of  the published GEOM_Object
  CORBA::String_var anID = aResultSO->GetID();
  aBaseObj->SetStudyEntry(anID.in());

  //Set a name of the added shape
  aResultSO->SetAttrString("AttributeName",anObjectName.ToCString());

  //Set NoteBook variables used in the object creation
  TCollection_AsciiString aVars;
  CORBA::String_var aString=aBaseObj->GetParameters();
  SALOMEDS::ListOfListOfStrings_var aSections = theStudy->ParseVariables(aString);
  for(int i = 0, n = aSections->length(); i < n; i++) {
    SALOMEDS::ListOfStrings aListOfVars = aSections[i];
    for(int j = 0, m = aListOfVars.length(); j < m; j++) {
      if(theStudy->IsVariable(aListOfVars[j].in()))
        aVars += aListOfVars[j].in();
      if(j != m-1)
        aVars += ":";
    }
    if(i != n-1)
      aVars += "|";
  }
  aResultSO->SetAttrString("AttributeString",aVars.ToCString());

  aFather->UnRegister();

  //Set a name of the GEOM object
  aBaseObj->SetName(anObjectName.ToCString());

  // add object to the use case tree
  // (to support tree representation customization and drag-n-drop)
  useCaseBuilder->AppendTo( aResultSO->GetFather(), aResultSO );

  return aResultSO._retn();
}

//============================================================================
// function : CreateAndPublishGroup
// purpose  : auxilary for PublishNamedShapesInStudy
//============================================================================
void GEOM_Gen_i::CreateAndPublishGroup(SALOMEDS::Study_ptr theStudy,
                                       GEOM::GEOM_Object_var theMainShape,
                                       const TopTools_IndexedMapOfShape& anIndices,
                                       const TopTools_SequenceOfShape& SeqS,
                                       const TColStd_SequenceOfAsciiString& SeqN,
                                       const Standard_CString& GrName,
                                       GEOM::ListOfGO_var aResList)
{
  CORBA::String_var entry = theMainShape->GetEntry();
  //HANDLE_NAMESPACE(GEOM_Object) aMainShape = _impl->GetObject(theMainShape->GetStudyID(), entry);
  Handle(TColStd_HArray1OfInteger) anArray;
  if(SeqS.Length()>0) {
    // create a group
    GEOM::GEOM_IGroupOperations_var GOp = GetIGroupOperations(theStudy->StudyId());
    GEOM::GEOM_Object_wrap GrObj = GOp->CreateGroup( theMainShape, SeqS(1).ShapeType() );
    AddInStudy(theStudy, GrObj, GrName, theMainShape._retn());
    //CORBA::String_var GrEntry = GrObj->GetEntry();
    //HANDLE_NAMESPACE(GEOM_Object) HGrObj = _impl->GetObject(GrObj->GetStudyID(), GrEntry);
    // add named objects
    //HANDLE_NAMESPACE(GEOM_Object) anObj;
    for(int i=1; i<=SeqS.Length(); i++) {
      TopoDS_Shape aValue = SeqS.Value(i);
      //anArray = new TColStd_HArray1OfInteger(1,1);
      Standard_Integer anIndex = anIndices.FindIndex(aValue);
      //anArray->SetValue(1, anIndex);
      GOp->AddObject(GrObj,anIndex);
      //anObj = GEOM_Engine::GetEngine()->AddObject(aMainShape->GetDocID(), GEOM_SUBSHAPE);
      //if (anObj.IsNull()) continue;
      //HANDLE_NAMESPACE(GEOM_Function) aFunction = anObj->AddFunction(GEOM_Object::GetSubShapeID(), 1);
      //if (aFunction.IsNull()) continue;
      //GEOM_ISubShape aSSI(aFunction);
      //aSSI.SetMainShape(aMainShape->GetLastFunction());
      //aSSI.SetIndices(anArray);
      //aFunction->SetValue(aValue);
      //GOp->UnionIDs(GrObj, anIndex);
      //SALOMEDS::SObject_var aResultSO;
      //TCollection_AsciiString anEntry;
      //TDF_Tool::Entry(anObj->GetEntry(),anEntry);
      //GEOM::GEOM_Object_var aGObj = GetObject(anObj->GetDocID(), anEntry.ToCString());
      //AddInStudy(theStudy, aGObj._retn(), SeqN.Value(i).ToCString(), GrObj);
    }
  }
}


//============================================================================
// function : PublishNamedShapesInStudy
// purpose  :
//============================================================================
GEOM::ListOfGO* GEOM_Gen_i::
            PublishNamedShapesInStudy(SALOMEDS::Study_ptr theStudy,
                                      //SALOMEDS::SObject_ptr theSObject,
                                      CORBA::Object_ptr theObject)
{
  //Unexpect aCatch(SALOME_SalomeException);
  GEOM::ListOfGO_var aResList = new GEOM::ListOfGO;

  //CORBA::Object_var theObject = theSObject->GetObject();
  GEOM::GEOM_Object_var theMainShape = GEOM::GEOM_Object::_narrow(theObject);
  if(theMainShape->_is_nil()) return aResList._retn();

  CORBA::String_var entry = theMainShape->GetEntry();
  HANDLE_NAMESPACE(GEOM_Object) aMainShape = HANDLE_NAMESPACE(GEOM_Object)::DownCast
    ( _impl->GetObject( theMainShape->GetStudyID(), entry ));
  if (aMainShape.IsNull()) return aResList._retn();
  TopoDS_Shape MainSh = aMainShape->GetValue();

  TDF_Label aMainLbl = aMainShape->GetEntry();
  TopTools_SequenceOfShape SolidSeqS, FaceSeqS, EdgeSeqS, VertSeqS;
  TColStd_SequenceOfAsciiString SolidSeqN, FaceSeqN, EdgeSeqN, VertSeqN;
  TDF_ChildIDIterator anIt(aMainLbl, TNaming_NamedShape::GetID(), Standard_True);
  for(; anIt.More(); anIt.Next()) {
    Handle(TNaming_NamedShape) anAttr =
      Handle(TNaming_NamedShape)::DownCast(anIt.Value());
    if(anAttr.IsNull()) continue;
    TopoDS_Shape S = anAttr->Get();
    TDF_Label L = anAttr->Label();
    //if(S.IsEqual(MainSh)) continue;
    Handle(TDataStd_Name) aName;
    if(L.FindAttribute(TDataStd_Name::GetID(),aName)) {
      TCollection_ExtendedString EName = aName->Get();
      if(S.ShapeType()==TopAbs_SOLID) {
        SolidSeqS.Append(S);
        SolidSeqN.Append(aName->Get());
      }
      else if(S.ShapeType()==TopAbs_FACE) {
        FaceSeqS.Append(S);
        FaceSeqN.Append(aName->Get());
      }
      else if(S.ShapeType()==TopAbs_EDGE) {
        EdgeSeqS.Append(S);
        EdgeSeqN.Append(aName->Get());
      }
      else if(S.ShapeType()==TopAbs_VERTEX) {
        VertSeqS.Append(S);
        VertSeqN.Append(aName->Get());
      }
    }
  }

  TopTools_IndexedMapOfShape anIndices;
  TopExp::MapShapes(MainSh, anIndices);

  CreateAndPublishGroup(theStudy, theMainShape, anIndices, SolidSeqS, SolidSeqN,
                        "Group_Of_Named_Solids", aResList);

  CreateAndPublishGroup(theStudy, theMainShape, anIndices, FaceSeqS, FaceSeqN,
                        "Group_Of_Named_Faces", aResList);

  CreateAndPublishGroup(theStudy, theMainShape, anIndices, EdgeSeqS, EdgeSeqN,
                        "Group_Of_Named_Edges", aResList);

  CreateAndPublishGroup(theStudy, theMainShape, anIndices, VertSeqS, VertSeqN,
                        "Group_Of_Named_Vertices", aResList);

  return aResList._retn();
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

  // OCCT BUG: cannot save a document (in current folder)
  // if directory name is empty
  if (aTmpDir.size() == 0) {
#ifdef WIN32
    aTmpDir = ".\\";
#else
    aTmpDir = "./";
#endif
  }

  // Create a list to store names of created files
  SALOMEDS::ListOfFileNames_var aSeq = new SALOMEDS::ListOfFileNames;
  aSeq->length(1);
  // Prepare a file name to open
  TCollection_AsciiString aNameWithExt("");
  if (isMultiFile)
    aNameWithExt = TCollection_AsciiString((char*)(SALOMEDS_Tool::GetNameFromPath
                                                   (theComponent->GetStudy()->URL())).c_str());
#if OCC_VERSION_MAJOR > 6
  aNameWithExt += TCollection_AsciiString("_GEOM.cbf");
#else
  aNameWithExt += TCollection_AsciiString("_GEOM.sgd");
#endif
  aSeq[0] = CORBA::string_dup(aNameWithExt.ToCString());
  // Build a full file name of temporary file
  TCollection_AsciiString aFullName = TCollection_AsciiString((char*)aTmpDir.c_str()) + aNameWithExt;
  // Save GEOM component in this file
  _impl->Save(theComponent->GetStudy()->StudyId(),(char*) aFullName.ToCString());
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

  // OCCT BUG: cannot load a document (from current folder)
  // if directory name is empty
  if (aTmpDir.size() == 0) {
#ifdef WIN32
    aTmpDir = ".\\";
#else
    aTmpDir = "./";
#endif
  }

  // Conver the byte stream theStream to a file and place it in tmp directory
  SALOMEDS::ListOfFileNames_var aSeq =
    SALOMEDS_Tool::PutStreamToFiles(theStream, aTmpDir.c_str(), isMultiFile);

  // Prepare a file name to open
  TCollection_AsciiString aNameWithExt("");
  SALOMEDS::Study_var study = theComponent->GetStudy();

#if OCC_VERSION_MAJOR > 6
  // Get the file name.
  int         i;
  int         aLength  = aSeq->length();
  const char *aGeomSgd = "_GEOM.sgd";
  const char *aGeomcbf = "_GEOM.cbf";

  for(i = 0; i < aLength; i++) {
    std::string aName(aSeq[i]);

    if (aName.rfind(aGeomSgd) != std::string::npos ||
        aName.rfind(aGeomcbf) != std::string::npos) {
      aNameWithExt = aName.c_str();
      break;
    }
  }
#else
  if (isMultiFile) {
    CORBA::String_var url = study->URL();
    aNameWithExt = (char*)SALOMEDS_Tool::GetNameFromPath(url.in()).c_str();
  }

  aNameWithExt += "_GEOM.sgd";
#endif

  TCollection_AsciiString aFullName = (TCollection_AsciiString((char*)aTmpDir.c_str()) + aNameWithExt);

  // Open document
  if (!_impl->Load(study->StudyId(),(char*) aFullName.ToCString())) return false;

  // Remove the created file and tmp directory
  if (!isMultiFile) SALOMEDS_Tool::RemoveTemporaryFiles(aTmpDir.c_str(), aSeq.in(), true);

  // creation of tree nodes for all data objects in the study
  // to support tree representation customization and drag-n-drop:
  SALOMEDS::UseCaseBuilder_wrap useCaseBuilder = study->GetUseCaseBuilder();
  if ( !useCaseBuilder->IsUseCaseNode( theComponent ) ) {
    useCaseBuilder->SetRootCurrent();
    useCaseBuilder->Append( theComponent ); // component object is added as the top level item
  }

  SALOMEDS::ChildIterator_wrap it = study->NewChildIterator( theComponent );
  for ( it->InitEx(true); it->More(); it->Next() ) {
    if ( !useCaseBuilder->IsUseCaseNode( it->Value() ) ) {
      useCaseBuilder->AppendTo( it->Value()->GetFather(), it->Value() );
    }
  }

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
  SALOMEDS::Study_var aStudy= theComponent->GetStudy();
  _impl->Close(aStudy->StudyId());
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

  CORBA::String_var aString=anIOR->Value();
  anIOR->UnRegister();
  CORBA::Object_var anObj = _orb->string_to_object(aString);
  GEOM::GEOM_Object_var anObject = GEOM::GEOM_Object::_narrow(anObj);
  // If the object is null one it can't be copied: return false
  if (anObject->_is_nil()) return false;
  return true;
}

//============================================================================
// function : CopyFrom()
// purpose  :
//============================================================================
SALOMEDS::TMPFile* GEOM_Gen_i::CopyFrom(SALOMEDS::SObject_ptr theObject, CORBA::Long& theObjectID)
{
  // Declare a sequence of the byte to store the copied object
  SALOMEDS::TMPFile_var aStreamFile = new SALOMEDS::TMPFile;

  // Try to get GEOM_Object object by given SObject
  SALOMEDS::GenericAttribute_var anAttr;
  if (!theObject->FindAttribute(anAttr, "AttributeIOR")) return aStreamFile._retn();
  GEOM::GEOM_Object_var anObject = GEOM::GEOM_Object::_narrow
    (_orb->string_to_object(SALOMEDS::AttributeIOR::_narrow(anAttr)->Value()));
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
  SALOMEDS::UseCaseBuilder_var anUseCaseBuilder = aStudy->GetUseCaseBuilder();
  SALOMEDS::SObject_var aNewSO;
  // Retrieve a TopoDS_Shape from byte stream
  TopoDS_Shape aTopology;
  std::istringstream aStreamedBrep((char*) &theStream[0]);
  BRep_Builder aBuilder;
  try {
    BRepTools::Read(aTopology, aStreamedBrep, aBuilder);
  } catch (Standard_Failure) {
    return aNewSO._retn();
  }

  // SObject of the created shape is theObject or new Child of Component if theObject == geom component
  if (strcmp(theObject->GetFatherComponent()->GetID(),theObject->GetID()) == 0) {
    aNewSO = aStudyBuilder->NewObject(theObject);
  } else aNewSO = SALOMEDS::SObject::_duplicate(theObject);


  //Create a new GEOM_Object
  HANDLE_NAMESPACE(GEOM_Object) anObj = _impl->AddObject(aNewSO->GetStudy()->StudyId(), theObjectID);
  HANDLE_NAMESPACE(GEOM_Function) aFunction = anObj->AddFunction(GEOMImpl_CopyDriver::GetID(), COPY_WITHOUT_REF);
  aFunction->SetValue(aTopology);

  TCollection_AsciiString anEntry;
  TDF_Tool::Entry(anObj->GetEntry(), anEntry);
  GEOM::GEOM_BaseObject_var obj = GetObject(anObj->GetDocID(), anEntry.ToCString());

  //Set the study entry of the published GEOM_Object
  obj->SetStudyEntry(aNewSO->GetID());

  // Add IORAttribute to the Study and set IOR of the created GEOM_Object to it
  SALOMEDS::GenericAttribute_var anAttr = aStudyBuilder->FindOrCreateAttribute(aNewSO, "AttributeIOR");
  SALOMEDS::AttributeIOR_var anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
  CORBA::String_var objStr = _orb->object_to_string(obj);
  anIOR->SetValue(objStr.in());
  anIOR->UnRegister();

  // add object to the use case tree
  // (to support tree representation customization and drag-n-drop)
  anUseCaseBuilder->AppendTo( aNewSO->GetFather(), aNewSO );

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
// function : AddInStudy
// purpose  :
//============================================================================
SALOMEDS::SObject_ptr GEOM_Gen_i::AddInStudy (SALOMEDS::Study_ptr       theStudy,
                                              GEOM::GEOM_BaseObject_ptr theObject,
                                              const char*               theName,
                                              GEOM::GEOM_BaseObject_ptr theFather)
{
  SALOMEDS::SObject_var aResultSO;
  if(theObject->_is_nil() || theStudy->_is_nil()) return aResultSO;

  SALOMEDS::StudyBuilder_var aStudyBuilder = theStudy->NewBuilder();
  CORBA::String_var IOR;

  if(!theFather->_is_nil()) {
    IOR = _orb->object_to_string(theFather);
    SALOMEDS::SObject_wrap aFatherSO = theStudy->FindObjectIOR(IOR.in());
    if(aFatherSO->_is_nil()) return aResultSO._retn();
    aResultSO = aStudyBuilder->NewObject(aFatherSO);
    //aStudyBuilder->Addreference(aResultSO, aResultSO);
  }

  aResultSO = PublishInStudy(theStudy, aResultSO, theObject, theName);
  if(aResultSO->_is_nil()) return aResultSO._retn();

  GEOM::ListOfGBO_var aList = theObject->GetDependency();
  Standard_Integer aLength = aList->length();
  if(aLength < 1) return aResultSO._retn();

  //Publish the arguments
  TCollection_AsciiString aPrevID; // to avoid multiple references to same object
  for(Standard_Integer i = 0; i< aLength; i++) {
    GEOM::GEOM_BaseObject_var anObject = aList[i];
    if(anObject->_is_nil()) continue;
    IOR = _orb->object_to_string(anObject);
    SALOMEDS::SObject_wrap aSO =  theStudy->FindObjectIOR(IOR.in());
    if(aSO->_is_nil()) continue;
    CORBA::String_var anID = aSO->GetID();
    if ( aPrevID == anID.in() ) continue;
    aPrevID = anID.in();
    SALOMEDS::SObject_wrap aSubSO = aStudyBuilder->NewObject(aResultSO);
    aStudyBuilder->Addreference(aSubSO, aSO);
    theStudy->GetUseCaseBuilder()->AppendTo( aResultSO, aSubSO );
  }

  return aResultSO._retn();
}

//============================================================================
// function : RestoreSubShapesO
// purpose  : Publish sub-shapes, standing for arguments and sub-shapes of arguments.
//            To be used from python scripts out of geompy.addToStudy (non-default usage)
//============================================================================
GEOM::ListOfGO* GEOM_Gen_i::RestoreSubShapesO (SALOMEDS::Study_ptr     theStudy,
                                               GEOM::GEOM_Object_ptr   theObject,
                                               const GEOM::ListOfGO&   theArgs,
                                               GEOM::find_shape_method theFindMethod,
                                               CORBA::Boolean          theInheritFirstArg,
                                               CORBA::Boolean          theAddPrefix)
{
  GEOM::ListOfGO_var aParts = new GEOM::ListOfGO;
  if (CORBA::is_nil(theStudy) || CORBA::is_nil(theObject))
    return aParts._retn();

  // find SObject in the study if it is already published
  CORBA::String_var anIORo = _orb->object_to_string(theObject);
  SALOMEDS::SObject_var aSO = theStudy->FindObjectIOR(anIORo.in());
  //PTv, IMP 0020001, The salome object <aSO>
  // is not obligatory in case of invokation from script
  // if (CORBA::is_nil(aSO))
  //  return aParts._retn();

  aParts = RestoreSubShapes(theStudy, theObject, aSO, theArgs,
                            theFindMethod, theInheritFirstArg, theAddPrefix);
  if (!CORBA::is_nil(aSO)) aSO->UnRegister();
  return aParts._retn();
}

//============================================================================
// function : RestoreGivenSubShapesO
// purpose  : Publish sub-shapes, standing for arguments and sub-shapes of arguments.
//            To be used from python scripts, generated by Dump Python.
//============================================================================
GEOM::ListOfGO* GEOM_Gen_i::RestoreGivenSubShapesO (SALOMEDS::Study_ptr     theStudy,
                                                    GEOM::GEOM_Object_ptr   theObject,
                                                    const GEOM::ListOfGO&   theArgs,
                                                    GEOM::find_shape_method theFindMethod,
                                                    CORBA::Boolean          theInheritFirstArg,
                                                    CORBA::Boolean          theAddPrefix)
{
  GEOM::ListOfGO_var aParts = new GEOM::ListOfGO;
  if (CORBA::is_nil(theStudy) || CORBA::is_nil(theObject))
    return aParts._retn();

  // find SObject in the study if it is already published
  CORBA::String_var anIORo = _orb->object_to_string(theObject);
  SALOMEDS::SObject_var aSO = theStudy->FindObjectIOR(anIORo.in());
  //PTv, IMP 0020001, The salome object <aSO>
  // is not obligatory in case of invokation from script
  // if (CORBA::is_nil(aSO))
  //  return aParts._retn();

  aParts = RestoreGivenSubShapes(theStudy, theObject, aSO, theArgs,
                                 theFindMethod, theInheritFirstArg, theAddPrefix);
  if (!CORBA::is_nil(aSO)) aSO->UnRegister();
  return aParts._retn();
}

//============================================================================
// function : RestoreSubShapesSO
// purpose  : Publish sub-shapes, standing for arguments and sub-shapes of arguments.
//            To be used from GUI and from geompy.addToStudy
//============================================================================
GEOM::ListOfGO* GEOM_Gen_i::RestoreSubShapesSO (SALOMEDS::Study_ptr     theStudy,
                                                SALOMEDS::SObject_ptr   theSObject,
                                                const GEOM::ListOfGO&   theArgs,
                                                GEOM::find_shape_method theFindMethod,
                                                CORBA::Boolean          theInheritFirstArg,
                                                CORBA::Boolean          theAddPrefix)
{
  GEOM::ListOfGO_var aParts = new GEOM::ListOfGO;
  if (CORBA::is_nil(theStudy) || CORBA::is_nil(theSObject))
    return aParts._retn();

  SALOMEDS::GenericAttribute_var anAttr;
  if (!theSObject->FindAttribute(anAttr, "AttributeIOR"))
    return aParts._retn();

  SALOMEDS::AttributeIOR_var anAttrIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
  CORBA::String_var anIORso = anAttrIOR->Value();

  // get Object from SObject
  GEOM::GEOM_Object_var anO = GEOM::GEOM_Object::_narrow(_orb->string_to_object(anIORso));
  if (CORBA::is_nil(anO))
    return aParts._retn();

  aParts = RestoreSubShapes(theStudy, anO, theSObject, theArgs,
                            theFindMethod, theInheritFirstArg, theAddPrefix);
  return aParts._retn();
}

//============================================================================
// function : addToListOfGO
// purpose  : static local function
//============================================================================
static void addToListOfGO( GEOM::GEOM_Object_ptr theObject,
                           GEOM::ListOfGO& theList )
{
  const int oldLen = theList.length();
  theList.length(oldLen + 1);
  theList[ oldLen ] = GEOM::GEOM_Object::_duplicate( theObject );
}

//============================================================================
// function : addToListOfGO
// purpose  : static local function
//============================================================================
static void addToListOfGO( const GEOM::ListOfGO& theSrcList,
                           GEOM::ListOfGO& theTrgList )
{
  const int oldLen = theTrgList.length();
  const int srcLen = theSrcList.length();
  theTrgList.length(oldLen + srcLen);
  for( int i = 0; i < srcLen; i++ )
    theTrgList[ oldLen + i ] = GEOM::GEOM_Object::_duplicate( theSrcList[ i ] );
}

//============================================================================
// function : RestoreSubShapes
// purpose  : Private method. Works only if both theObject and theSObject
//            are defined, and does not check, if they correspond to each other.
//============================================================================
GEOM::ListOfGO* GEOM_Gen_i::RestoreSubShapes(SALOMEDS::Study_ptr     theStudy,
                                             GEOM::GEOM_Object_ptr   theObject,
                                             SALOMEDS::SObject_ptr   theSObject,
                                             const GEOM::ListOfGO&   theArgs,
                                             GEOM::find_shape_method theFindMethod,
                                             CORBA::Boolean          theInheritFirstArg,
                                             CORBA::Boolean          theAddPrefix)
{
  GEOM::ListOfGO_var aParts = new GEOM::ListOfGO;
  //PTv, IMP 0020001, The salome object <theSObject>
  //     is not obligatory in case of invokation from script
  if (CORBA::is_nil(theStudy) || CORBA::is_nil(theObject) /*|| CORBA::is_nil(theSObject)*/)
    return aParts._retn();

  // For Dump Python (mantis issue 0020768)
  GEOM::ListOfGO_var anOutArgs = new GEOM::ListOfGO;

  // Arguments to be published
  GEOM::ListOfGO_var aList;

  // If theArgs list is empty, we try to publish all arguments,
  // otherwise publish only passed args
  Standard_Integer nbArgsActual = -1; // -1 means unknown
  Standard_Integer aLength = theArgs.length();
  if (aLength > 0) {
    aList = new GEOM::ListOfGO;
    aList->length(aLength);
    for (int i = 0; i < aLength; i++) {
      aList[i] = GEOM::GEOM_Object::_duplicate( theArgs[i] );
    }
  }
  else {
    // Get all arguments
    GEOM::ListOfGBO_var boList = theObject->GetDependency();
    aLength = boList->length();
    aList = new GEOM::ListOfGO;
    aList->length(aLength);
    for (int i = 0; i < aLength; i++)
      aList[i] = GEOM::GEOM_Object::_narrow( boList[i] );
    nbArgsActual = aLength;
  }

  if (aLength < 1)
    return aParts._retn();

  if (theInheritFirstArg || (nbArgsActual == 1)) {
    // Do not publish argument's reflection,
    // but only reconstruct its published sub-shapes

    CORBA::String_var anIOR = _orb->object_to_string(aList[0]);
    SALOMEDS::SObject_var anArgSO = theStudy->FindObjectIOR(anIOR.in());

    // remember restored objects for Python Dump
    addToListOfGO(aList[0], anOutArgs);

    aParts = RestoreSubShapesOneLevel(theStudy, anArgSO, theSObject, theObject,
                                      anOutArgs, theFindMethod, theAddPrefix);

    // set the color of the transformed shape to the color of initial shape
    theObject->SetColor(aList[0]->GetColor());
    // set the texture
    if (theObject->GetShapeType() == GEOM::VERTEX) {
      theObject->SetMarkerStd(aList[0]->GetMarkerType(), aList[0]->GetMarkerSize());
      if (aList[0]->GetMarkerType() == GEOM::MT_USER)
        theObject->SetMarkerTexture(aList[0]->GetMarkerTexture());
    }

    anArgSO->UnRegister();
  }
  else {
    // Get interface, containing method, which we will use to reconstruct sub-shapes
    GEOM::GEOM_IShapesOperations_var  aShapesOp = GetIShapesOperations(theStudy->StudyId());
    GEOM::GEOM_IGroupOperations_var    aGroupOp = GetIGroupOperations(theStudy->StudyId());
    GEOM::GEOM_ITransformOperations_var aTrsfOp = GetITransformOperations(theStudy->StudyId());

    PortableServer::Servant aServant = _poa->reference_to_servant(aTrsfOp.in());
    GEOM_ITransformOperations_i*      aTrsfOpSv = dynamic_cast<GEOM_ITransformOperations_i*>(aServant);

    // Reconstruct arguments and tree of sub-shapes of the arguments
    CORBA::String_var anIOR;
    SALOMEDS::StudyBuilder_var aStudyBuilder = theStudy->NewBuilder();
    for (Standard_Integer i = 0; i < aLength; i++)
    {
      GEOM::GEOM_Object_var anArgO = aList[i];
      if (!CORBA::is_nil(anArgO)) {
        anIOR = _orb->object_to_string(anArgO);
        SALOMEDS::SObject_var anArgSO = theStudy->FindObjectIOR(anIOR.in());
        TCollection_AsciiString anArgName;
        if (CORBA::is_nil(anArgSO)) {
          anArgName = "arg_";
          anArgName += TCollection_AsciiString(i);
        }
        else {
          anArgName = anArgSO->GetName();
        }

        // Find a sub-shape of theObject in place of the argument
        GEOM::GEOM_Object_var aSubO;
        switch (theFindMethod) {
        case GEOM::FSM_GetInPlace:
          {
            // Use GetInPlace
            aSubO = aShapesOp->GetInPlace(theObject, anArgO);
          }
          break;
        case GEOM::FSM_MultiTransformed:
          {
            // Only for Multi-transformations
            GEOM::GEOM_Object_var anArgOTrsf = aTrsfOpSv->TransformLikeOtherCopy(anArgO, theObject);
            if (!CORBA::is_nil(anArgOTrsf)) {
              CORBA::String_var anArgOTrsfEntry = anArgOTrsf->GetEntry();
              HANDLE_NAMESPACE(GEOM_BaseObject) anArgOTrsfImpl = _impl->GetObject(anArgOTrsf->GetStudyID(), anArgOTrsfEntry);
              HANDLE_NAMESPACE(GEOM_Function) anArgOTrsfFun = anArgOTrsfImpl->GetLastFunction();
              anArgOTrsfFun->SetDescription("");
              aSubO = aShapesOp->GetInPlace(theObject, anArgOTrsf);
            }
            /*
            HANDLE_NAMESPACE(GEOM_Function) anOFun = theObject->GetLastFunction();
            if (!anOFun.IsNull()) {
              CORBA::String_var entryArg = anArgO->GetEntry();
              HANDLE_NAMESPACE(GEOM_Object) anArgOImpl = _impl->GetObject(anArgO->GetStudyID(), entryArg);
              if (!anArgOImpl.IsNull()) {
                TopoDS_Shape anArgOShape = anArgOImpl->GetValue();
                TopoDS_Shape aMultiArgShape;
                //GEOM::GEOM_Object_var anArgOMulti; // ???
                switch (anOFun->GetType()) {
                case TRANSLATE_1D:
                  {
                    GEOMImpl_ITranslate aTI (anOFun);
                    aMultiArgShape = GEOMImpl_ITransformOperations::TranslateShape1D(anArgOShape, &aTI);
                    //anArgOMulti = aTrsfOpSv->Translate1D(anArgO, , , );
                  }
                  break;
                case TRANSLATE_2D:
                  {
                    GEOMImpl_ITranslate aTI (anOFun);
                    aMultiArgShape = GEOMImpl_ITransformOperations::TranslateShape2D(anArgOShape, &aTI);
                  }
                  break;
                case ROTATE_1D:
                  {
                    GEOMImpl_IRotate aTI (anOFun);
                    //aMultiArgShape = GEOMImpl_ITransformOperations::TranslateShape2D(anArgOShape, &aTI);
                  }
                  break;
                case ROTATE_2D:
                  {
                    GEOMImpl_IRotate aTI (anOFun);
                    //aMultiArgShape = GEOMImpl_ITransformOperations::TranslateShape2D(anArgOShape, &aTI);
                  }
                  break;
                default:
                  {}
                }
                GEOM::GEOM_Object_var anArgOMulti = (aMultiArgShape); // TODO
                HANDLE_NAMESPACE(GEOM_Function) anArgOMultiFun = anArgOMulti->GetLastFunction();
                anArgOMultiFun->SetDescription("");
                aSubO = aShapesOp->GetInPlace(theObject, anArgOMulti);
              }
            }
            */
          }
          break;
        case GEOM::FSM_Transformed:
          {
            // transformation, cannot use GetInPlace, operate with indices
            GEOM::ListOfLong_var anIDs = anArgO->GetSubShapeIndices();
            if (anIDs->length() > 1) {
              // group
              aSubO = aGroupOp->CreateGroup(theObject, aGroupOp->GetType(anArgO));
              if (!CORBA::is_nil(aSubO))
                aGroupOp->UnionIDs(aSubO, anIDs);
            }
            else if (anIDs->length() > 0) {
              // single sub-shape
              aSubO = aShapesOp->GetSubShape(theObject, anIDs[0]);
            }
          }
          break;
        case GEOM::FSM_GetSame:
          {
            // Use GetSame
            aSubO = aShapesOp->GetSame(theObject, anArgO);
          }
          break;
        case GEOM::FSM_GetShapesOnShape:
          {
            // Use GetShapesOnShape. Can work only on solids, so it has sense to search only solids
            aSubO = aShapesOp->GetShapesOnShapeAsCompound(anArgO, theObject,
              (short)GEOM::SOLID, GEOM::ST_ONIN);
          }
          break;
        case GEOM::FSM_GetInPlaceByHistory:
          {
            // Use GetInPlaceByHistory
            aSubO = aShapesOp->GetInPlaceByHistory(theObject, anArgO);
          }
          break;
        default:
          {}
        }

        if (!CORBA::is_nil(aSubO)) {
          // remember restored objects for Python Dump
          addToListOfGO(anArgO, anOutArgs);

          // add to parts list
          addToListOfGO( aSubO, aParts );

          // Publish the sub-shape
          SALOMEDS::SObject_var aSubSO;
          if (!CORBA::is_nil(theSObject)) {
            TCollection_AsciiString aSubName;
            if (theAddPrefix) {
              aSubName = "from_";
            }
            aSubName += anArgName;
            aSubSO = aStudyBuilder->NewObject(theSObject);
            aSubSO = PublishInStudy(theStudy, aSubSO, aSubO, aSubName.ToCString());
            // Restore color
            aSubO->SetColor(anArgO->GetColor());
            // set the texture
            if (aSubO->GetShapeType() == GEOM::VERTEX) {
              aSubO->SetMarkerStd(anArgO->GetMarkerType(), anArgO->GetMarkerSize());
              if (anArgO->GetMarkerType() == GEOM::MT_USER)
                aSubO->SetMarkerTexture(anArgO->GetMarkerTexture());
            }
          }

          if (!CORBA::is_nil(anArgSO)) {
            // Restore published sub-shapes of the argument
            GEOM::ListOfGO_var aSubParts;
            if (theFindMethod == GEOM::FSM_GetInPlaceByHistory)
              // pass theObject, because only it has the history
              aSubParts = RestoreSubShapesOneLevel(theStudy, anArgSO, aSubSO,
                                                   theObject, anOutArgs, theFindMethod, theAddPrefix);
            else
              aSubParts = RestoreSubShapesOneLevel(theStudy, anArgSO, aSubSO,
                                                   aSubO, anOutArgs, theFindMethod, theAddPrefix);
            // add to parts list
            addToListOfGO( aSubParts, aParts );
          }
        }
        else { // GetInPlace failed, try to build from published parts
          if (!CORBA::is_nil(anArgSO)) {
            SALOMEDS::SObject_var aSubSO;
            if (!CORBA::is_nil(theSObject))
              aSubSO = aStudyBuilder->NewObject(theSObject);

            // Restore published sub-shapes of the argument
            GEOM::ListOfGO_var aSubParts =
              RestoreSubShapesOneLevel(theStudy, anArgSO, aSubSO,
                                       theObject, anOutArgs, theFindMethod, theAddPrefix);

            // add to parts list
            addToListOfGO( aSubParts, aParts );

            if (aSubParts->length() > 0) {
              // remember restored objects for Python Dump
              addToListOfGO(anArgO, anOutArgs);

              // try to build an argument from a set of its sub-shapes,
              // that published and will be reconstructed
              if (aSubParts->length() > 1) {
                aSubO = aShapesOp->MakeCompound(aSubParts);
                // add to parts list
                addToListOfGO( aSubO, aParts );
              }
              else {
                aSubO = aSubParts[0];
              }
              if (!CORBA::is_nil(aSubO) && !CORBA::is_nil(aSubSO)) {
                // Publish the sub-shape
                TCollection_AsciiString aSubName;
                if (theAddPrefix) {
                  aSubName = "from_parts_of_";
                }
                aSubName += anArgName;
                aSubSO = PublishInStudy(theStudy, aSubSO, aSubO, aSubName.ToCString());
                // Restore color
                aSubO->SetColor(anArgO->GetColor());
                // set the texture
                if (aSubO->GetShapeType() == GEOM::VERTEX) {
                  aSubO->SetMarkerStd(anArgO->GetMarkerType(), anArgO->GetMarkerSize());
                  if (anArgO->GetMarkerType() == GEOM::MT_USER)
                    aSubO->SetMarkerTexture(anArgO->GetMarkerTexture());
                }
              }
            }
            else if (!CORBA::is_nil(aSubSO)) {
              // remove created aSubSO, because no parts have been found
              aStudyBuilder->RemoveObject(aSubSO);
            }
          }
        } // try to build from published parts
        anArgSO->UnRegister();
      }
    } // process arguments
  }
  std::set<std::string> anObjEntryMap;
  GEOM::ListOfGO_var aResParts = new GEOM::ListOfGO;
  int nbRes = 0;
  int nb = aParts->length();
  aResParts->length(nb);
  if (nb > 0)
  {
    HANDLE_NAMESPACE(GEOM_BaseObject) aMainObj = _impl->GetObject(theObject->GetStudyID(), theObject->GetEntry());
    HANDLE_NAMESPACE(GEOM_Function) aFunction = aMainObj->GetLastFunction();
    GEOM::TPythonDump pd (aFunction, true);
    pd <<"[";
    int i = 0, j = 0;
    for ( ; i < nb; i++ )
    {
      GEOM::GEOM_Object_var anObj = GEOM::GEOM_Object::_duplicate( aParts[ i ] );
      if (CORBA::is_nil(anObj))
        continue;
      char* anEntry = anObj->GetEntry();
      if (anObjEntryMap.count(anEntry))
        continue; // already treated
      anObjEntryMap.insert(anEntry);
      aResParts[nbRes++] = anObj;
      // clear python dump of object
      HANDLE_NAMESPACE(GEOM_BaseObject) aGeomObj = _impl->GetObject(anObj->GetStudyID(), anEntry);
      HANDLE_NAMESPACE(GEOM_Function)   anObjFun = aGeomObj->GetLastFunction();
      if ( !anObjFun.IsNull() )
        anObjFun->SetDescription( "" );
      if ( j > 0 )
        pd << ", ";
      pd << aGeomObj;
      j++;
    }
    pd <<"]" << " = geompy.RestoreGivenSubShapes(" << aMainObj << ", " << "[";
    //i = 0; nb = theArgs.length(); j = 0;
    i = 0; nb = anOutArgs->length(); j = 0;
    for ( ; i < nb; i++ )
    {
      //GEOM::GEOM_Object_var anObj = theArgs[ i ];
      GEOM::GEOM_Object_var anObj = anOutArgs[ i ];
      if (CORBA::is_nil(anObj))
        continue;
      HANDLE_NAMESPACE(GEOM_BaseObject) aGeomObj = _impl->GetObject(anObj->GetStudyID(), anObj->GetEntry());
      if ( j > 0 )
        pd << ", ";
      pd << aGeomObj;
      j++;
    }
    pd <<"]" << ", " <<"GEOM.";
    switch (theFindMethod) {
    case GEOM::FSM_GetInPlace:
      pd << "FSM_GetInPlace"; break;
    case GEOM::FSM_MultiTransformed:
      pd << "FSM_MultiTransformed"; break;
    case GEOM::FSM_Transformed:
      pd << "FSM_Transformed"; break;
    case GEOM::FSM_GetSame:
      pd << "FSM_GetSame"; break;
    case GEOM::FSM_GetShapesOnShape:
      pd << "FSM_GetShapesOnShape"; break;
    case GEOM::FSM_GetInPlaceByHistory:
    default:
      pd << "FSM_GetInPlaceByHistory"; break;
    }
    pd << ", " << theInheritFirstArg << ", " << theAddPrefix << ")";
  }
  aResParts->length(nbRes);
  return aResParts._retn();
}

//============================================================================
// function : RestoreSubShapesOneLevel
// purpose  : Private method
//============================================================================
GEOM::ListOfGO* GEOM_Gen_i::RestoreSubShapesOneLevel (SALOMEDS::Study_ptr     theStudy,
                                                      SALOMEDS::SObject_ptr   theOldSO,
                                                      SALOMEDS::SObject_ptr   theNewSO,
                                                      GEOM::GEOM_Object_ptr   theNewO,
                                                      GEOM::ListOfGO&         theOutArgs,
                                                      GEOM::find_shape_method theFindMethod,
                                                      CORBA::Boolean          theAddPrefix)
{
  int i = 0;
  GEOM::ListOfGO_var aParts = new GEOM::ListOfGO;
  GEOM::ListOfGO_var aNewParts = new GEOM::ListOfGO;
  if (CORBA::is_nil(theStudy) || CORBA::is_nil(theOldSO) ||
      CORBA::is_nil(theNewO) /*|| CORBA::is_nil(theNewSO)*/)
    return aParts._retn();

  SALOMEDS::StudyBuilder_var aStudyBuilder = theStudy->NewBuilder();

  // Get interface, containing method, which we will use to reconstruct sub-shapes
  GEOM::GEOM_IShapesOperations_var  aShapesOp = GetIShapesOperations(theStudy->StudyId());
  GEOM::GEOM_IGroupOperations_var    aGroupOp = GetIGroupOperations(theStudy->StudyId());
  GEOM::GEOM_ITransformOperations_var aTrsfOp = GetITransformOperations(theStudy->StudyId());

  PortableServer::Servant aServant = _poa->reference_to_servant(aTrsfOp.in());
  GEOM_ITransformOperations_i*      aTrsfOpSv = dynamic_cast<GEOM_ITransformOperations_i*>(aServant);

  // Reconstruct published sub-shapes
  SALOMEDS::ChildIterator_var it = theStudy->NewChildIterator(theOldSO);

  int aLen = 0;
  for (it->Init(); it->More(); it->Next()) {
    aLen++;
  }
  aParts->length(aLen);

  for (it->Init(); it->More(); it->Next()) {
    SALOMEDS::SObject_var anOldSubSO = it->Value();

    TCollection_AsciiString anArgName = anOldSubSO->GetName();

    SALOMEDS::GenericAttribute_var anAttr;
    if (anOldSubSO->FindAttribute(anAttr, "AttributeIOR")) {
      SALOMEDS::AttributeIOR_var anAttrIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
      GEOM::GEOM_Object_var anOldSubO =
        GEOM::GEOM_Object::_narrow(_orb->string_to_object(anAttrIOR->Value()));
      if (!CORBA::is_nil(anOldSubO)) {
        // Find a sub-shape of theNewO in place of anOldSubO
        GEOM::GEOM_Object_var aNewSubO;
        switch (theFindMethod) {
        case GEOM::FSM_GetInPlace:
          {
            // Use GetInPlace
            aNewSubO = aShapesOp->GetInPlace(theNewO, anOldSubO);
          }
          break;
        case GEOM::FSM_MultiTransformed:
          {
            // Only for Multi-transformations
            GEOM::GEOM_Object_var anArgOTrsf = aTrsfOpSv->TransformLikeOtherCopy(anOldSubO, theNewO);
            if (!CORBA::is_nil(anArgOTrsf)) {
              CORBA::String_var anArgOTrsfEntry = anArgOTrsf->GetEntry();
              HANDLE_NAMESPACE(GEOM_BaseObject) anArgOTrsfImpl = _impl->GetObject(anArgOTrsf->GetStudyID(), anArgOTrsfEntry);
              HANDLE_NAMESPACE(GEOM_Function) anArgOTrsfFun = anArgOTrsfImpl->GetLastFunction();
              anArgOTrsfFun->SetDescription("");
              aNewSubO = aShapesOp->GetInPlace(theNewO, anArgOTrsf);
            }
          }
          break;
        case GEOM::FSM_Transformed:
          {
            // transformation, cannot use GetInPlace, operate with indices
            GEOM::ListOfLong_var anIDs = anOldSubO->GetSubShapeIndices();
            if (anIDs->length() > 1) {
              // group
              aNewSubO = aGroupOp->CreateGroup(theNewO, aGroupOp->GetType(anOldSubO));
              if (!CORBA::is_nil(aNewSubO))
                aGroupOp->UnionIDs(aNewSubO, anIDs);
            }
            else {
              // single sub-shape
              aNewSubO = aShapesOp->GetSubShape(theNewO, anIDs[0]);
            }
          }
          break;
        case GEOM::FSM_GetSame:
          {
            // Use GetSame
            aNewSubO = aShapesOp->GetSame(theNewO, anOldSubO);
          }
          break;
        case GEOM::FSM_GetShapesOnShape:
          {
            // Use GetShapesOnShape. Can work only on solids, so it has sense to search only solids
            aNewSubO = aShapesOp->GetShapesOnShapeAsCompound(anOldSubO, theNewO,
                                                             (short)GEOM::SOLID, GEOM::ST_ONIN);
          }
          break;
        case GEOM::FSM_GetInPlaceByHistory:
          {
            // Use GetInPlaceByHistory
            aNewSubO = aShapesOp->GetInPlaceByHistory(theNewO, anOldSubO);
          }
          break;
        default:
          {}
        }

        if (!CORBA::is_nil(aNewSubO)) {
          // remember restored objects for Python Dump
          addToListOfGO(anOldSubO, theOutArgs);

          // add the part to the list
          aParts[i] = aNewSubO;
          i++;
          // add to parts list
          addToListOfGO( aNewSubO, aNewParts );

          SALOMEDS::SObject_var aNewSubSO;
          if (!CORBA::is_nil(theNewSO)) {
              // Publish the sub-shape
            TCollection_AsciiString aSubName;
            if (theAddPrefix) {
              aSubName = "from_";
            }
            aSubName += anArgName;
            aNewSubSO = aStudyBuilder->NewObject(theNewSO);
            aNewSubSO = PublishInStudy(theStudy, aNewSubSO, aNewSubO, aSubName.ToCString());
            // Restore color
            aNewSubO->SetColor(anOldSubO->GetColor());
            // set the texture
            if (aNewSubO->GetShapeType() == GEOM::VERTEX) {
              aNewSubO->SetMarkerStd(anOldSubO->GetMarkerType(), anOldSubO->GetMarkerSize());
              if (anOldSubO->GetMarkerType() == GEOM::MT_USER)
                aNewSubO->SetMarkerTexture(anOldSubO->GetMarkerTexture());
            }
          }
          // Restore published sub-shapes of the argument
          GEOM::ListOfGO_var aSubParts;
          if (theFindMethod == GEOM::FSM_GetInPlaceByHistory)
            // pass the main shape as Object, because only it has the history
            aSubParts = RestoreSubShapesOneLevel(theStudy, anOldSubSO, aNewSubSO,
                                                 theNewO, theOutArgs, theFindMethod, theAddPrefix);
          else
            aSubParts = RestoreSubShapesOneLevel(theStudy, anOldSubSO, aNewSubSO,
                                                 aNewSubO, theOutArgs, theFindMethod, theAddPrefix);
          // add to parts list
          addToListOfGO( aSubParts, aNewParts );
        }
        else { // GetInPlace failed, try to build from published parts
          SALOMEDS::SObject_var aNewSubSO;
          if (!CORBA::is_nil(theNewSO))
            aNewSubSO = aStudyBuilder->NewObject(theNewSO);

          // Restore published sub-shapes of the argument
          GEOM::ListOfGO_var aSubParts =
            RestoreSubShapesOneLevel(theStudy, anOldSubSO, aNewSubSO,
                                     theNewO, theOutArgs, theFindMethod, theAddPrefix);
          // add to parts list
          addToListOfGO( aSubParts, aNewParts );

          if (aSubParts->length() > 0) {
            // remember restored objects for Python Dump
            addToListOfGO(anOldSubO, theOutArgs);

            // try to build an object from a set of its sub-shapes,
            // that published and will be reconstructed
            if (aSubParts->length() > 1) {
              aNewSubO = aShapesOp->MakeCompound(aSubParts);
              // add to parts list
              addToListOfGO( aNewSubO, aNewParts );
            }
            else {
              aNewSubO = aSubParts[0];
            }

            if (!CORBA::is_nil(aNewSubO)) {
              // add the part to the list
              aSubParts[i] = aNewSubO;
              i++;

              // Publish the sub-shape
              if (!CORBA::is_nil(aNewSubSO)) {
                TCollection_AsciiString aSubName;
                if (theAddPrefix) {
                  aSubName = "from_parts_of_";
                }
                aSubName += anArgName;
                aNewSubSO = PublishInStudy(theStudy, aNewSubSO, aNewSubO, aSubName.ToCString());
                // Restore color
                aNewSubO->SetColor(anOldSubO->GetColor());
                // set the texture
                if (aNewSubO->GetShapeType() == GEOM::VERTEX) {
                  aNewSubO->SetMarkerStd(anOldSubO->GetMarkerType(), anOldSubO->GetMarkerSize());
                  if (anOldSubO->GetMarkerType() == GEOM::MT_USER)
                    aNewSubO->SetMarkerTexture(anOldSubO->GetMarkerTexture());
                }
              }
            }
          }
          else if (!CORBA::is_nil(aNewSubSO)) {
            // remove created aSubSO, because no parts have been found
            aStudyBuilder->RemoveObject(aNewSubSO);
          }
        } // try to build from published parts
      }
    }
  } // iterate on published sub-shapes

  aParts->length(i);
  // add to parts list
  addToListOfGO( aNewParts, aParts );
  return aParts._retn();
}

//============================================================================
// function : RestoreGivenSubShapes
// purpose  : Private method. Works only if both theObject and theSObject
//            are defined, and does not check, if they correspond to each other.
//            List theArgs in this case contains not only operation arguments,
//            but also all subshapes, which must be published.
//============================================================================
GEOM::ListOfGO* GEOM_Gen_i::RestoreGivenSubShapes(SALOMEDS::Study_ptr     theStudy,
                                                  GEOM::GEOM_Object_ptr   theObject,
                                                  SALOMEDS::SObject_ptr   theSObject,
                                                  const GEOM::ListOfGO&   theArgs,
                                                  GEOM::find_shape_method theFindMethod,
                                                  CORBA::Boolean          theInheritFirstArg,
                                                  CORBA::Boolean          theAddPrefix)
{
  GEOM::ListOfGO_var aParts = new GEOM::ListOfGO;
  //PTv, IMP 0020001, The salome object <theSObject>
  //     is not obligatory in case of invokation from script
  if (CORBA::is_nil(theStudy) || CORBA::is_nil(theObject) /*|| CORBA::is_nil(theSObject)*/)
    return aParts._retn();

  // If theArgs list is empty, nothing to do
  Standard_Integer aLength = theArgs.length();
  if (aLength == 0)
    return aParts._retn();

  // Get all arguments
  GEOM::ListOfGBO_var anOpArgsList = theObject->GetDependency();
  Standard_Integer    nbArgsActual = anOpArgsList->length();

  // If anOpArgsList list is empty, nothing to do
  if (nbArgsActual == 0)
    return aParts._retn();

  // Entries of arguments and subshapes
  std::set<std::string> anArgs;
  for (int i = 0; i < aLength; i++) {
    CORBA::String_var anEntry = theArgs[i]->GetEntry();
    anArgs.insert(anEntry.in());
  }

  // Arguments to be published
  // We try to publish all arguments, that are in theArgs list
  GEOM::ListOfGO_var aList = new GEOM::ListOfGO;
  aList->length(nbArgsActual);

  int k = 0;
  for (int j = 0; j < nbArgsActual; j++) {
    CORBA::String_var anEntry = anOpArgsList[j]->GetEntry();
    if (anArgs.count(anEntry.in())) {
      aList[k] = GEOM::GEOM_Object::_narrow(anOpArgsList[j]);
      k++;
    }
  }
  nbArgsActual = k;
  //aList->length(nbArgsActual);

  if (nbArgsActual < 1)
    return aParts._retn();

  if (theInheritFirstArg || (nbArgsActual == 1)) {
    // Do not publish argument's reflection,
    // but only reconstruct its published sub-shapes

    CORBA::String_var anIOR = _orb->object_to_string(aList[0]);
    SALOMEDS::SObject_var anArgSO = theStudy->FindObjectIOR(anIOR.in());

    aParts = RestoreGivenSubShapesOneLevel(theStudy, anArgSO, theSObject, theObject,
                                           anArgs, theFindMethod, theAddPrefix);

    // set the color of the transformed shape to the color of initial shape
    theObject->SetColor(aList[0]->GetColor());
    // set the texture
    if (theObject->GetShapeType() == GEOM::VERTEX) {
      theObject->SetMarkerStd(aList[0]->GetMarkerType(), aList[0]->GetMarkerSize());
      if (aList[0]->GetMarkerType() == GEOM::MT_USER)
        theObject->SetMarkerTexture(aList[0]->GetMarkerTexture());
    }

    anArgSO->UnRegister();
  }
  else {
    // Get interface, containing method, which we will use to reconstruct sub-shapes
    GEOM::GEOM_IShapesOperations_var  aShapesOp = GetIShapesOperations(theStudy->StudyId());
    GEOM::GEOM_IGroupOperations_var    aGroupOp = GetIGroupOperations(theStudy->StudyId());
    GEOM::GEOM_ITransformOperations_var aTrsfOp = GetITransformOperations(theStudy->StudyId());

    PortableServer::Servant aServant = _poa->reference_to_servant(aTrsfOp.in());
    GEOM_ITransformOperations_i*      aTrsfOpSv = dynamic_cast<GEOM_ITransformOperations_i*>(aServant);

    // Reconstruct arguments and tree of sub-shapes of the arguments
    CORBA::String_var anIOR;
    SALOMEDS::StudyBuilder_var aStudyBuilder = theStudy->NewBuilder();
    for (Standard_Integer i = 0; i < nbArgsActual; i++)
    {
      GEOM::GEOM_Object_var anArgO = aList[i];
      if (!CORBA::is_nil(anArgO)) {
        anIOR = _orb->object_to_string(anArgO);
        SALOMEDS::SObject_var anArgSO = theStudy->FindObjectIOR(anIOR.in());
        TCollection_AsciiString anArgName;
        if (CORBA::is_nil(anArgSO)) {
          anArgName = "arg_";
          anArgName += TCollection_AsciiString(i);
        }
        else {
          anArgName = anArgSO->GetName();
        }

        // Find a sub-shape of theObject in place of the argument
        GEOM::GEOM_Object_var aSubO;
        switch (theFindMethod) {
        case GEOM::FSM_GetInPlace:
          {
            // Use GetInPlace
            aSubO = aShapesOp->GetInPlace(theObject, anArgO);
          }
          break;
        case GEOM::FSM_MultiTransformed:
          {
            // Only for Multi-transformations
            GEOM::GEOM_Object_var anArgOTrsf = aTrsfOpSv->TransformLikeOtherCopy(anArgO, theObject);
            if (!CORBA::is_nil(anArgOTrsf)) {
              CORBA::String_var anArgOTrsfEntry = anArgOTrsf->GetEntry();
              HANDLE_NAMESPACE(GEOM_BaseObject) anArgOTrsfImpl = _impl->GetObject(anArgOTrsf->GetStudyID(), anArgOTrsfEntry);
              HANDLE_NAMESPACE(GEOM_Function) anArgOTrsfFun = anArgOTrsfImpl->GetLastFunction();
              anArgOTrsfFun->SetDescription("");
              aSubO = aShapesOp->GetInPlace(theObject, anArgOTrsf);
            }
          }
          break;
        case GEOM::FSM_Transformed:
          {
            // transformation, cannot use GetInPlace, operate with indices
            GEOM::ListOfLong_var anIDs = anArgO->GetSubShapeIndices();
            if (anIDs->length() > 1) {
              // group
              aSubO = aGroupOp->CreateGroup(theObject, aGroupOp->GetType(anArgO));
              if (!CORBA::is_nil(aSubO))
                aGroupOp->UnionIDs(aSubO, anIDs);
            }
            else if (anIDs->length() > 0) {
              // single sub-shape
              aSubO = aShapesOp->GetSubShape(theObject, anIDs[0]);
            }
          }
          break;
        case GEOM::FSM_GetSame:
          {
            // Use GetSame
            aSubO = aShapesOp->GetSame(theObject, anArgO);
          }
          break;
        case GEOM::FSM_GetShapesOnShape:
          {
            // Use GetShapesOnShape. Can work only on solids, so it has sense to search only solids
            aSubO = aShapesOp->GetShapesOnShapeAsCompound(anArgO, theObject,
              (short)GEOM::SOLID, GEOM::ST_ONIN);
          }
          break;
        case GEOM::FSM_GetInPlaceByHistory:
          {
            // Use GetInPlaceByHistory
            aSubO = aShapesOp->GetInPlaceByHistory(theObject, anArgO);
          }
          break;
        default:
          {}
        }

        if (!CORBA::is_nil(aSubO)) {
          // add to parts list
          addToListOfGO( aSubO, aParts );

          // Publish the sub-shape
          SALOMEDS::SObject_var aSubSO;
          if (!CORBA::is_nil(theSObject)) {
            TCollection_AsciiString aSubName;
            if (theAddPrefix) {
              aSubName = "from_";
            }
            aSubName += anArgName;
            aSubSO = aStudyBuilder->NewObject(theSObject);
            aSubSO = PublishInStudy(theStudy, aSubSO, aSubO, aSubName.ToCString());
            // Restore color
            aSubO->SetColor(anArgO->GetColor());
            // set the texture
            if (aSubO->GetShapeType() == GEOM::VERTEX) {
              aSubO->SetMarkerStd(anArgO->GetMarkerType(), anArgO->GetMarkerSize());
              if (anArgO->GetMarkerType() == GEOM::MT_USER)
                aSubO->SetMarkerTexture(anArgO->GetMarkerTexture());
            }
          }

          if (!CORBA::is_nil(anArgSO)) {
            // Restore published sub-shapes of the argument
            GEOM::ListOfGO_var aSubParts;
            if (theFindMethod == GEOM::FSM_GetInPlaceByHistory)
              // pass theObject, because only it has the history
              aSubParts = RestoreGivenSubShapesOneLevel(theStudy, anArgSO, aSubSO,
                                                        theObject, anArgs, theFindMethod, theAddPrefix);
            else
              aSubParts = RestoreGivenSubShapesOneLevel(theStudy, anArgSO, aSubSO,
                                                        aSubO, anArgs, theFindMethod, theAddPrefix);
            // add to parts list
            addToListOfGO( aSubParts, aParts );
          }
        }
        else { // GetInPlace failed, try to build from published parts
          if (!CORBA::is_nil(anArgSO)) {
            SALOMEDS::SObject_var aSubSO;
            if (!CORBA::is_nil(theSObject))
              aSubSO = aStudyBuilder->NewObject(theSObject);

            // Restore published sub-shapes of the argument
            GEOM::ListOfGO_var aSubParts =
              RestoreGivenSubShapesOneLevel(theStudy, anArgSO, aSubSO,
                                            theObject, anArgs, theFindMethod, theAddPrefix);

            // add to parts list
            addToListOfGO( aSubParts, aParts );

            if (aSubParts->length() > 0) {
              // try to build an argument from a set of its sub-shapes,
              // that published and will be reconstructed
              if (aSubParts->length() > 1) {
                aSubO = aShapesOp->MakeCompound(aSubParts);
                // add to parts list
                addToListOfGO( aSubO, aParts );
              }
              else {
                aSubO = aSubParts[0];
              }
              if (!CORBA::is_nil(aSubO) && !CORBA::is_nil(aSubSO)) {
                // Publish the sub-shape
                TCollection_AsciiString aSubName;
                if (theAddPrefix) {
                  aSubName = "from_parts_of_";
                }
                aSubName += anArgName;
                aSubSO = PublishInStudy(theStudy, aSubSO, aSubO, aSubName.ToCString());
                // Restore color
                aSubO->SetColor(anArgO->GetColor());
                // set the texture
                if (aSubO->GetShapeType() == GEOM::VERTEX) {
                  aSubO->SetMarkerStd(anArgO->GetMarkerType(), anArgO->GetMarkerSize());
                  if (anArgO->GetMarkerType() == GEOM::MT_USER)
                    aSubO->SetMarkerTexture(anArgO->GetMarkerTexture());
                }
              }
            }
            else if (!CORBA::is_nil(aSubSO)) {
              // remove created aSubSO, because no parts have been found
              aStudyBuilder->RemoveObject(aSubSO);
            }
          }
        } // try to build from published parts
        anArgSO->UnRegister();
      }
    } // process arguments
  }
  std::set<std::string> anObjEntryMap;
  GEOM::ListOfGO_var aResParts = new GEOM::ListOfGO;
  int nbRes = 0;
  int nb = aParts->length();
  aResParts->length(nb);
  if (nb > 0)
  {
    HANDLE_NAMESPACE(GEOM_BaseObject) aMainObj = _impl->GetObject(theObject->GetStudyID(), theObject->GetEntry());
    HANDLE_NAMESPACE(GEOM_Function) aFunction = aMainObj->GetLastFunction();
    GEOM::TPythonDump pd (aFunction, true);
    pd <<"[";
    int i = 0, j = 0;
    for ( ; i < nb; i++ )
    {
      GEOM::GEOM_Object_var anObj = GEOM::GEOM_Object::_duplicate( aParts[ i ] );
      if (CORBA::is_nil(anObj))
        continue;
      char* anEntry = anObj->GetEntry();
      if (anObjEntryMap.count(anEntry))
        continue; // already treated
      anObjEntryMap.insert(anEntry);
      aResParts[nbRes++] = anObj;
      // clear python dump of object
      HANDLE_NAMESPACE(GEOM_BaseObject) aGeomObj = _impl->GetObject(anObj->GetStudyID(), anEntry);
      HANDLE_NAMESPACE(GEOM_Function)   anObjFun = aGeomObj->GetLastFunction();
      if ( !anObjFun.IsNull() )
        anObjFun->SetDescription( "" );
      if ( j > 0 )
        pd << ", ";
      pd << aGeomObj;
      j++;
    }
    pd <<"]" << " = geompy.RestoreGivenSubShapes(" << aMainObj << ", " << "[";
    i = 0; nb = theArgs.length(); j = 0;
    for ( ; i < nb; i++ )
    {
      GEOM::GEOM_Object_var anObj = theArgs[ i ];
      if (CORBA::is_nil(anObj))
        continue;
      HANDLE_NAMESPACE(GEOM_BaseObject) aGeomObj = _impl->GetObject(anObj->GetStudyID(), anObj->GetEntry());
      if ( j > 0 )
        pd << ", ";
      pd << aGeomObj;
      j++;
    }
    pd <<"]" << ", " <<"GEOM.";
    switch (theFindMethod) {
    case GEOM::FSM_GetInPlace:
      pd << "FSM_GetInPlace"; break;
    case GEOM::FSM_MultiTransformed:
      pd << "FSM_MultiTransformed"; break;
    case GEOM::FSM_Transformed:
      pd << "FSM_Transformed"; break;
    case GEOM::FSM_GetSame:
      pd << "FSM_GetSame"; break;
    case GEOM::FSM_GetShapesOnShape:
      pd << "FSM_GetShapesOnShape"; break;
    case GEOM::FSM_GetInPlaceByHistory:
    default:
      pd << "FSM_GetInPlaceByHistory"; break;
    }
    pd << ", " << theInheritFirstArg << ", " << theAddPrefix << ")";
  }
  aResParts->length(nbRes);
  return aResParts._retn();
}

//============================================================================
// function : RestoreGivenSubShapesOneLevel
// purpose  : Private method
//============================================================================
GEOM::ListOfGO* GEOM_Gen_i::RestoreGivenSubShapesOneLevel (SALOMEDS::Study_ptr     theStudy,
                                                           SALOMEDS::SObject_ptr   theOldSO,
                                                           SALOMEDS::SObject_ptr   theNewSO,
                                                           GEOM::GEOM_Object_ptr   theNewO,
                                                           std::set<std::string>   theArgs,
                                                           GEOM::find_shape_method theFindMethod,
                                                           CORBA::Boolean          theAddPrefix)
{
  int i = 0;
  GEOM::ListOfGO_var aParts = new GEOM::ListOfGO;
  GEOM::ListOfGO_var aNewParts = new GEOM::ListOfGO;
  if (CORBA::is_nil(theStudy) || CORBA::is_nil(theOldSO) ||
      CORBA::is_nil(theNewO) /*|| CORBA::is_nil(theNewSO)*/)
    return aParts._retn();

  SALOMEDS::StudyBuilder_var aStudyBuilder = theStudy->NewBuilder();

  // Get interface, containing method, which we will use to reconstruct sub-shapes
  GEOM::GEOM_IShapesOperations_var  aShapesOp = GetIShapesOperations(theStudy->StudyId());
  GEOM::GEOM_IGroupOperations_var    aGroupOp = GetIGroupOperations(theStudy->StudyId());
  GEOM::GEOM_ITransformOperations_var aTrsfOp = GetITransformOperations(theStudy->StudyId());

  PortableServer::Servant aServant = _poa->reference_to_servant(aTrsfOp.in());
  GEOM_ITransformOperations_i*      aTrsfOpSv = dynamic_cast<GEOM_ITransformOperations_i*>(aServant);

  // Reconstruct published sub-shapes
  SALOMEDS::ChildIterator_var it = theStudy->NewChildIterator(theOldSO);

  int aLen = 0;
  for (it->Init(); it->More(); it->Next()) {
    aLen++;
  }
  aParts->length(aLen);

  for (it->Init(); it->More(); it->Next()) {
    SALOMEDS::SObject_var anOldSubSO = it->Value();

    TCollection_AsciiString anArgName = anOldSubSO->GetName();

    SALOMEDS::GenericAttribute_var anAttr;
    if (anOldSubSO->FindAttribute(anAttr, "AttributeIOR")) {
      SALOMEDS::AttributeIOR_var anAttrIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
      GEOM::GEOM_Object_var anOldSubO =
        GEOM::GEOM_Object::_narrow(_orb->string_to_object(anAttrIOR->Value()));

      bool okToContinue = false;

      if (!CORBA::is_nil(anOldSubO)) {
        CORBA::String_var anEntry = anOldSubO->GetEntry();
        okToContinue = theArgs.count(anEntry.in());
      }

      if (okToContinue) {
        // Find a sub-shape of theNewO in place of anOldSubO
        GEOM::GEOM_Object_var aNewSubO;
        switch (theFindMethod) {
        case GEOM::FSM_GetInPlace:
          {
            // Use GetInPlace
            aNewSubO = aShapesOp->GetInPlace(theNewO, anOldSubO);
          }
          break;
        case GEOM::FSM_MultiTransformed:
          {
            // Only for Multi-transformations
            GEOM::GEOM_Object_var anArgOTrsf = aTrsfOpSv->TransformLikeOtherCopy(anOldSubO, theNewO);
            if (!CORBA::is_nil(anArgOTrsf)) {
              CORBA::String_var anArgOTrsfEntry = anArgOTrsf->GetEntry();
              HANDLE_NAMESPACE(GEOM_BaseObject) anArgOTrsfImpl = _impl->GetObject(anArgOTrsf->GetStudyID(), anArgOTrsfEntry);
              HANDLE_NAMESPACE(GEOM_Function) anArgOTrsfFun = anArgOTrsfImpl->GetLastFunction();
              anArgOTrsfFun->SetDescription("");
              aNewSubO = aShapesOp->GetInPlace(theNewO, anArgOTrsf);
            }
          }
          break;
        case GEOM::FSM_Transformed:
          {
            // transformation, cannot use GetInPlace, operate with indices
            GEOM::ListOfLong_var anIDs = anOldSubO->GetSubShapeIndices();
            if (anIDs->length() > 1) {
              // group
              aNewSubO = aGroupOp->CreateGroup(theNewO, aGroupOp->GetType(anOldSubO));
              if (!CORBA::is_nil(aNewSubO))
                aGroupOp->UnionIDs(aNewSubO, anIDs);
            }
            else {
              // single sub-shape
              aNewSubO = aShapesOp->GetSubShape(theNewO, anIDs[0]);
            }
          }
          break;
        case GEOM::FSM_GetSame:
          {
            // Use GetSame
            aNewSubO = aShapesOp->GetSame(theNewO, anOldSubO);
          }
          break;
        case GEOM::FSM_GetShapesOnShape:
          {
            // Use GetShapesOnShape. Can work only on solids, so it has sense to search only solids
            aNewSubO = aShapesOp->GetShapesOnShapeAsCompound(anOldSubO, theNewO,
                                                             (short)GEOM::SOLID, GEOM::ST_ONIN);
          }
          break;
        case GEOM::FSM_GetInPlaceByHistory:
          {
            // Use GetInPlaceByHistory
            aNewSubO = aShapesOp->GetInPlaceByHistory(theNewO, anOldSubO);
          }
          break;
        default:
          {}
        }

        if (!CORBA::is_nil(aNewSubO)) {
          // add the part to the list
          aParts[i] = aNewSubO;
          i++;
          // add to parts list
          addToListOfGO( aNewSubO, aNewParts );

          SALOMEDS::SObject_var aNewSubSO;
          if (!CORBA::is_nil(theNewSO)) {
              // Publish the sub-shape
            TCollection_AsciiString aSubName;
            if (theAddPrefix) {
              aSubName = "from_";
            }
            aSubName += anArgName;
            aNewSubSO = aStudyBuilder->NewObject(theNewSO);
            aNewSubSO = PublishInStudy(theStudy, aNewSubSO, aNewSubO, aSubName.ToCString());
            // Restore color
            aNewSubO->SetColor(anOldSubO->GetColor());
            // set the texture
            if (aNewSubO->GetShapeType() == GEOM::VERTEX) {
              aNewSubO->SetMarkerStd(anOldSubO->GetMarkerType(), anOldSubO->GetMarkerSize());
              if (anOldSubO->GetMarkerType() == GEOM::MT_USER)
                aNewSubO->SetMarkerTexture(anOldSubO->GetMarkerTexture());
            }
          }
          // Restore published sub-shapes of the argument
          GEOM::ListOfGO_var aSubParts;
          if (theFindMethod == GEOM::FSM_GetInPlaceByHistory)
            // pass the main shape as Object, because only it has the history
            aSubParts = RestoreGivenSubShapesOneLevel(theStudy, anOldSubSO, aNewSubSO,
                                                      theNewO, theArgs, theFindMethod, theAddPrefix);
          else
            aSubParts = RestoreGivenSubShapesOneLevel(theStudy, anOldSubSO, aNewSubSO,
                                                      aNewSubO, theArgs, theFindMethod, theAddPrefix);
          // add to parts list
          addToListOfGO( aSubParts, aNewParts );
        }
        else { // GetInPlace failed, try to build from published parts
          SALOMEDS::SObject_var aNewSubSO;
          if (!CORBA::is_nil(theNewSO))
            aNewSubSO = aStudyBuilder->NewObject(theNewSO);

          // Restore published sub-shapes of the argument
          GEOM::ListOfGO_var aSubParts =
            RestoreGivenSubShapesOneLevel(theStudy, anOldSubSO, aNewSubSO,
                                          theNewO, theArgs, theFindMethod, theAddPrefix);
          // add to parts list
          addToListOfGO( aSubParts, aNewParts );

          if (aSubParts->length() > 0) {
            // try to build an object from a set of its sub-shapes,
            // that published and will be reconstructed
            if (aSubParts->length() > 1) {
              aNewSubO = aShapesOp->MakeCompound(aSubParts);
              // add to parts list
              addToListOfGO( aNewSubO, aNewParts );
            }
            else {
              aNewSubO = aSubParts[0];
            }

            if (!CORBA::is_nil(aNewSubO)) {
              // add the part to the list
              aSubParts[i] = aNewSubO;
              i++;

              // Publish the sub-shape
              if (!CORBA::is_nil(aNewSubSO)) {
                TCollection_AsciiString aSubName;
                if (theAddPrefix) {
                  aSubName = "from_parts_of_";
                }
                aSubName += anArgName;
                aNewSubSO = PublishInStudy(theStudy, aNewSubSO, aNewSubO, aSubName.ToCString());
                // Restore color
                aNewSubO->SetColor(anOldSubO->GetColor());
                // set the texture
                if (aNewSubO->GetShapeType() == GEOM::VERTEX) {
                  aNewSubO->SetMarkerStd(anOldSubO->GetMarkerType(), anOldSubO->GetMarkerSize());
                  if (anOldSubO->GetMarkerType() == GEOM::MT_USER)
                    aNewSubO->SetMarkerTexture(anOldSubO->GetMarkerTexture());
                }
              }
            }
          }
          else if (!CORBA::is_nil(aNewSubSO)) {
            // remove created aSubSO, because no parts have been found
            aStudyBuilder->RemoveObject(aNewSubSO);
          }
        } // try to build from published parts
      }
    }
  } // iterate on published sub-shapes

  aParts->length(i);
  // add to parts list
  addToListOfGO( aNewParts, aParts );
  return aParts._retn();
}

//============================================================================
// function : register()
// purpose  : register 'name' in 'name_service'
//============================================================================
void GEOM_Gen_i::register_name(char * name)
{
  GEOM::GEOM_Gen_var g = _this();
  name_service->Register(g, name);
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

  GEOM::GEOM_Gen_ptr engine = _this();

  //transfer reference on engine
  GEOM_IBasicOperations_i* aServant =
    new GEOM_IBasicOperations_i(_poa, engine, _impl->GetIBasicOperations(theStudyID));

  PortableServer::ObjectId_var id = _poa->activate_object(aServant);
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

  GEOM::GEOM_Gen_ptr engine = _this();

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

  GEOM::GEOM_Gen_ptr engine = _this();

  GEOM_I3DPrimOperations_i* aServant =
    new GEOM_I3DPrimOperations_i(_poa, engine, _impl->GetI3DPrimOperations(theStudyID));
  PortableServer::ObjectId_var id = _poa->activate_object(aServant);

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

  GEOM::GEOM_Gen_ptr engine = _this();

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

  GEOM::GEOM_Gen_ptr engine = _this();

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

  GEOM::GEOM_Gen_ptr engine = _this();

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

  GEOM::GEOM_Gen_ptr engine = _this();

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

  GEOM::GEOM_Gen_ptr engine = _this();

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

  GEOM::GEOM_Gen_ptr engine = _this();

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

  GEOM::GEOM_Gen_ptr engine = _this();

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

  GEOM::GEOM_Gen_ptr engine = _this();

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

  GEOM::GEOM_Gen_ptr engine = _this();

  GEOM_IGroupOperations_i* aServant =
    new GEOM_IGroupOperations_i(_poa, engine, _impl->GetIGroupOperations(theStudyID));

  // activate the CORBA servant
  GEOM::GEOM_IGroupOperations_var operations = aServant->_this();
  return operations._retn();
}

//============================================================================
// function : GetIFieldOperations
// purpose  :
//============================================================================
GEOM::GEOM_IFieldOperations_ptr GEOM_Gen_i::GetIFieldOperations(CORBA::Long theStudyID)
     throw ( SALOME::SALOME_Exception )
{
  Unexpect aCatch(SALOME_SalomeException);
  MESSAGE( "GEOM_Gen_i::GetIFieldOperations" );

  GEOM::GEOM_Gen_ptr engine = _this();

  GEOM_IFieldOperations_i* aServant =
    new GEOM_IFieldOperations_i(_poa, engine, _impl->GetIFieldOperations(theStudyID));

  // activate the CORBA servant
  GEOM::GEOM_IFieldOperations_var operations = aServant->_this();
  return operations._retn();
}

//============================================================================
// function : GetPluginOperations
// purpose  :
//============================================================================
GEOM::GEOM_IOperations_ptr GEOM_Gen_i::GetPluginOperations(CORBA::Long theStudyID,
                                                           const char* theLibName)
     throw ( SALOME::SALOME_Exception )
{
  Unexpect aCatch(SALOME_SalomeException);
  MESSAGE( "GEOM_Gen_i::GetPluginOperations" );

  GEOM::GEOM_Gen_ptr engine = _this();

  GEOM::GEOM_IOperations_var operations;

  std::string aLibName = theLibName;

  try {
    // load plugin library
    LoadPlugin(aLibName);
    // create a new operations object, store its ref. in engine
    if ( myOpCreatorMap.find(aLibName) != myOpCreatorMap.end() ) {
      GEOM_IOperations_i* aServant = 0;
      aServant = myOpCreatorMap[aLibName]->Create(_poa, theStudyID, engine, _impl);
      // activate the CORBA servant
      if (aServant)
        operations = aServant->_this();
    }
  }
  catch (SALOME_Exception& S_ex) {
    THROW_SALOME_CORBA_EXCEPTION(S_ex.what(), SALOME::BAD_PARAM);
  }

  return operations._retn();
}

//============================================================================
// function : LoadPlugin
// purpose  : load plugin library and retrieve an instance of operations creator
//============================================================================
void GEOM_Gen_i::LoadPlugin(const std::string& theLibName)
{
  std::string aPlatformLibName;
#ifdef WIN32
  aPlatformLibName = theLibName;
  aPlatformLibName += ".dll" ;
#else
  aPlatformLibName = "lib";
  aPlatformLibName += theLibName;
  aPlatformLibName += ".so";
#endif

  // check, if corresponding operations are already created
  if (myOpCreatorMap.find(theLibName) == myOpCreatorMap.end()) {
    // load plugin library
    LibHandle libHandle = LoadLib( aPlatformLibName.c_str() );
    if (!libHandle) {
      // report any error, if occured
#ifndef WIN32
      throw(SALOME_Exception(dlerror()));
#else
      throw(SALOME_Exception(LOCALIZED( "Can't load server geometry plugin library" )));
#endif
    }

    // get method, returning operations creator
    typedef GEOM_GenericOperationsCreator* (*GetOperationsCreator)();
    GetOperationsCreator procHandle =
      (GetOperationsCreator)GetProc( libHandle, "GetOperationsCreator" );
    if (!procHandle) {
      UnLoadLib(libHandle);
      throw(SALOME_Exception(LOCALIZED("bad geometry plugin library")));
    }

    // get operations creator
    GEOM_GenericOperationsCreator* aCreator = procHandle();
    if (aCreator) {
      // map operations creator to a plugin name
      myOpCreatorMap[theLibName] = aCreator;
    }
    else {
      throw(SALOME_Exception(LOCALIZED("bad geometry plugin library implementation")));
    }
  }
}

//=============================================================================
/*!
 *  AddSubShape
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Gen_i::AddSubShape (GEOM::GEOM_Object_ptr   theMainShape,
                                               const GEOM::ListOfLong& theIndices)
{
  if (CORBA::is_nil(theMainShape) || theIndices.length() < 1)
    return GEOM::GEOM_Object::_nil();
  CORBA::String_var entry = theMainShape->GetEntry();
  HANDLE_NAMESPACE(GEOM_Object) aMainShape = HANDLE_NAMESPACE(GEOM_Object)::DownCast
    ( _impl->GetObject( theMainShape->GetStudyID(), entry ));
  if (aMainShape.IsNull()) return GEOM::GEOM_Object::_nil();

  Handle(TColStd_HArray1OfInteger) anArray = new TColStd_HArray1OfInteger(1, theIndices.length());
  for(Standard_Integer i = 0; i<theIndices.length(); i++) anArray->SetValue(i+1, theIndices[i]);

  HANDLE_NAMESPACE(GEOM_Object) anObject = _impl->AddSubShape(aMainShape, anArray, true);
  if(anObject.IsNull()) return GEOM::GEOM_Object::_nil();

  TCollection_AsciiString anEntry;
  TDF_Tool::Entry(anObject->GetEntry(), anEntry);
  return GEOM::GEOM_Object::_narrow( GetObject(anObject->GetDocID(), anEntry.ToCString()));
}

//=============================================================================
/*!
 *  RemoveObject
 */
//=============================================================================
void GEOM_Gen_i::RemoveObject(GEOM::GEOM_BaseObject_ptr theObject)
{
  CORBA::String_var anEntry = theObject->GetEntry();
  HANDLE_NAMESPACE(GEOM_BaseObject) anObject = _impl->GetObject(theObject->GetStudyID(), anEntry, false);
  if (!anObject.IsNull())
    _impl->RemoveObject(anObject);
}

//=================================================================================
// function : GetStringFromIOR()
// purpose  : returns a string that represents  a 'GEOM::GEOM_Object_var'
//=================================================================================
char* GEOM_Gen_i::GetStringFromIOR(GEOM::GEOM_Object_ptr theObject)
{
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
      aGeomObject =  GEOM::GEOM_Object::_narrow(anObject.in());
  }
  return aGeomObject._retn();
}

//=================================================================================
// function : GetObject()
// purpose  :
//=================================================================================
GEOM::GEOM_BaseObject_ptr GEOM_Gen_i::GetObject (CORBA::Long theStudyID, const char* theEntry)
{
  GEOM::GEOM_BaseObject_var obj;
  HANDLE_NAMESPACE(GEOM_BaseObject) handle_object = _impl->GetObject(theStudyID, (char*)theEntry);
  if (handle_object.IsNull()) return obj._retn();

  TCollection_AsciiString stringIOR = handle_object->GetIOR();
  if (stringIOR.Length() > 1) {
    CORBA::Object_var corba_object = _orb->string_to_object(stringIOR.ToCString());
    if (!CORBA::is_nil(corba_object)) obj = GEOM::GEOM_BaseObject::_narrow(corba_object);
    return obj._retn();
  }

  GEOM::GEOM_Gen_ptr engine = _this();
  //transfer the reference to GEOM_Object_i
  GEOM_BaseObject_i* servant = 0;
  switch( handle_object->GetType() ) {
  case GEOM_FIELD: {
    servant = new GEOM_Field_i (_poa, engine, HANDLE_NAMESPACE(GEOM_Field)::DownCast( handle_object ));
    break;
  }
  case GEOM_FIELD_STEP: {
    HANDLE_NAMESPACE(GEOM_FieldStep) step = HANDLE_NAMESPACE(GEOM_FieldStep)::DownCast( handle_object );
    HANDLE_NAMESPACE(GEOM_Field)    field = step->GetField();
    int type = ( !field.IsNull() ? field->GetDataType() : 0 );
    switch( type ) {
    case GEOM::FDT_Bool:
      servant = new GEOM_BoolFieldStep_i (_poa, engine, step );
      break;
    case GEOM::FDT_Int:
      servant = new GEOM_IntFieldStep_i (_poa, engine, step );
      break;
    case GEOM::FDT_Double:
      servant = new GEOM_DoubleFieldStep_i (_poa, engine, step );
      break;
    default:
      servant = new GEOM_StringFieldStep_i (_poa, engine, step );
    }
    break;
  }
  default:
    servant = new GEOM_Object_i (_poa, engine, HANDLE_NAMESPACE(GEOM_Object)::DownCast( handle_object ));
  }
  PortableServer::ObjectId_var id = _poa->activate_object(servant);

  obj = servant->_this();
  CORBA::String_var objStr = _orb->object_to_string(obj);
  TCollection_AsciiString anAscii( (char *)objStr.in() );
  handle_object->SetIOR( anAscii );
  return obj._retn();
}

//=================================================================================
// function : hasObjectInfo()
// purpose  : shows if module provides information for its objects
//=================================================================================
bool GEOM_Gen_i::hasObjectInfo()
{
  return true;
}

//=================================================================================
// function : getObjectInfo()
// purpose  : returns an information for a given object by its entry
//=================================================================================
char* GEOM_Gen_i::getObjectInfo(CORBA::Long studyId, const char* entry)
{
  GEOM::GEOM_Object_var aGeomObject;

  CORBA::Object_var aSMObject = name_service->Resolve( "/myStudyManager" );
  SALOMEDS::StudyManager_var aStudyManager = SALOMEDS::StudyManager::_narrow( aSMObject );
  SALOMEDS::Study_var aStudy = aStudyManager->GetStudyByID( studyId );
  SALOMEDS::SObject_var aSObj = aStudy->FindObjectID( entry );
  SALOMEDS::SObject_var aResultSObj;
  if (aSObj->ReferencedObject(aResultSObj))
    aSObj = aResultSObj;

  SALOMEDS::GenericAttribute_var anAttr;
  if (!aSObj->_is_nil() && aSObj->FindAttribute(anAttr, "AttributeIOR")) {
    SALOMEDS::AttributeIOR_var anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
    CORBA::String_var aVal = anIOR->Value();
    anIOR->UnRegister();
    CORBA::Object_var anObject = aStudy->ConvertIORToObject(aVal);
    aGeomObject = GEOM::GEOM_Object::_narrow(anObject);
  }
  if (!aSObj->_is_nil() )
    aSObj->UnRegister();

  const char* aTypeInfo = "Object";
  if ( !aGeomObject->_is_nil() ) {
    GEOM::GEOM_IKindOfShape::shape_kind aKind;
    GEOM::ListOfLong_var anInts;
    GEOM::ListOfDouble_var aDbls;

    GEOM::GEOM_IMeasureOperations_var anOp = GetIMeasureOperations( studyId );
    aKind = anOp->KindOfShape( aGeomObject, anInts, aDbls );

    if ( anOp->IsDone() ) {
      switch ( aKind ) {
      case GEOM::GEOM_IKindOfShape::COMPOUND:
        aTypeInfo = "Compound";
        break;
      case GEOM::GEOM_IKindOfShape::COMPSOLID:
        aTypeInfo = "CompSolid";
        break;
      case GEOM::GEOM_IKindOfShape::SHELL:
        aTypeInfo = "Shell";
        break;
      case GEOM::GEOM_IKindOfShape::WIRE:
        if ( anInts[0] == 1 )
          aTypeInfo = "Closed Wire";
        else if ( anInts[0] == 2 )
          aTypeInfo = "Opened Wire";
        else
          aTypeInfo = "Wire";
        break;
        // SOLIDs
      case GEOM::GEOM_IKindOfShape::SPHERE:
        aTypeInfo = "Sphere";
        break;
      case GEOM::GEOM_IKindOfShape::CYLINDER:
        aTypeInfo = "Cylinder";
        break;
      case GEOM::GEOM_IKindOfShape::BOX:
      case GEOM::GEOM_IKindOfShape::ROTATED_BOX:
        aTypeInfo = "Box";
        break;
      case GEOM::GEOM_IKindOfShape::TORUS:
        aTypeInfo = "Torus";
        break;
      case GEOM::GEOM_IKindOfShape::CONE:
        aTypeInfo = "Cone";
        break;
      case GEOM::GEOM_IKindOfShape::POLYHEDRON:
        aTypeInfo = "Polyhedron";
        break;
      case GEOM::GEOM_IKindOfShape::SOLID:
        aTypeInfo = "Solid";
        break;
        // FACEs
      case GEOM::GEOM_IKindOfShape::SPHERE2D:
        aTypeInfo = "Spherical Face";
        break;
      case GEOM::GEOM_IKindOfShape::CYLINDER2D:
        aTypeInfo = "Cylindrical Face";
        break;
      case GEOM::GEOM_IKindOfShape::TORUS2D:
        aTypeInfo = "Toroidal Face";
        break;
      case GEOM::GEOM_IKindOfShape::CONE2D:
        aTypeInfo = "Conical Face";
        break;
      case GEOM::GEOM_IKindOfShape::DISK_CIRCLE:
        aTypeInfo = "Disk";
        break;
      case GEOM::GEOM_IKindOfShape::DISK_ELLIPSE:
        aTypeInfo = "Elliptical Face";
        break;
      case GEOM::GEOM_IKindOfShape::POLYGON:
        aTypeInfo = "Polygon";
        break;
      case GEOM::GEOM_IKindOfShape::PLANE:
        aTypeInfo = "Plane";
        break;
      case GEOM::GEOM_IKindOfShape::PLANAR:
        aTypeInfo = "Planar Face";
        break;
      case GEOM::GEOM_IKindOfShape::FACE:
        aTypeInfo = "Face";
        break;
        // EDGEs
      case GEOM::GEOM_IKindOfShape::CIRCLE:
        aTypeInfo = "Circle";
        break;
      case GEOM::GEOM_IKindOfShape::ARC_CIRCLE:
        aTypeInfo = "Arc Circle";
        break;
      case GEOM::GEOM_IKindOfShape::ELLIPSE:
        aTypeInfo = "Ellipse";
        break;
      case GEOM::GEOM_IKindOfShape::ARC_ELLIPSE:
        aTypeInfo = "Arc Ellipse";
        break;
      case GEOM::GEOM_IKindOfShape::LINE:
        aTypeInfo = "Line";
        break;
      case GEOM::GEOM_IKindOfShape::SEGMENT:
        aTypeInfo = "Segment";
        break;
      case GEOM::GEOM_IKindOfShape::EDGE:
        aTypeInfo = "Edge";
        break;
      case GEOM::GEOM_IKindOfShape::VERTEX:
        aTypeInfo = "Vertex";
        break;
      default:
        break;
      }
    }
  }

  CORBA::String_var compType = ComponentDataType();
  char* anInfo = new char[strlen("Module ") + strlen(compType.in()) + strlen(", ") + strlen(aTypeInfo) + 3];
  sprintf(anInfo, "Module %s, %s", compType.in(), aTypeInfo);

  char* ret = CORBA::string_dup(anInfo);
  delete [] anInfo;
  return ret;
}

// Version information
char* GEOM_Gen_i::getVersion()
{
#if GEOM_DEVELOPMENT
  return CORBA::string_dup(GEOM_VERSION_STR"dev");
#else
  return CORBA::string_dup(GEOM_VERSION_STR);
#endif
}

//=================================================================================
// function : CreateFolder()
// purpose  : Creates and returns a new folder object
//=================================================================================
SALOMEDS::SObject_ptr GEOM_Gen_i::CreateFolder(const char* theName,
                                               SALOMEDS::SObject_ptr theFather)
{
  SALOMEDS::SObject_var aFolderSO;

  if ( CORBA::is_nil(theFather) ) return aFolderSO._retn();

  SALOMEDS::GenericAttribute_var anAttr;
  if ( strcmp(theFather->GetFatherComponent()->GetID(), theFather->GetID()) != 0 ) {
    // not a GEOM component object was selected
    if ( !theFather->FindAttribute(anAttr, "AttributeLocalID") ) return aFolderSO._retn();
    SALOMEDS::AttributeLocalID_var aLocalID = SALOMEDS::AttributeLocalID::_narrow(anAttr);
    if( aLocalID->Value() != 999 ) {
      // not a Folder object was selected
      GEOM::GEOM_Object_var aGeomObject = GEOM::GEOM_Object::_narrow(theFather);
      if ( CORBA::is_nil(aGeomObject) ) return aFolderSO._retn();
      // another GEOM object was selected, so get GEOM component as father object
      theFather = theFather->GetFatherComponent();
    }
    aLocalID->UnRegister();
  }

  SALOMEDS::Study_var aStudy = theFather->GetStudy();
  SALOMEDS::StudyBuilder_var aStudyBuilder( aStudy->NewBuilder() );
  aFolderSO = aStudyBuilder->NewObject( theFather );

  anAttr = aStudyBuilder->FindOrCreateAttribute(aFolderSO, "AttributeLocalID");
  SALOMEDS::AttributeLocalID_var aLocalID = SALOMEDS::AttributeLocalID::_narrow(anAttr);
  aLocalID->SetValue( 999 ); // mark of the "Folder" object
  aLocalID->UnRegister();

  anAttr = aStudyBuilder->FindOrCreateAttribute(aFolderSO, "AttributeName");
  SALOMEDS::AttributeName_var aName = SALOMEDS::AttributeName::_narrow(anAttr);
  aName->SetValue( theName );
  aName->UnRegister();

  anAttr = aStudyBuilder->FindOrCreateAttribute(aFolderSO, "AttributePixMap");
  SALOMEDS::AttributePixMap_var aPixMap = SALOMEDS::AttributePixMap::_narrow(anAttr);
  aPixMap->SetPixMap("ICON_FOLDER");
  aPixMap->UnRegister();

  // add object to the use case tree
  // (to support tree representation customization and drag-n-drop)
  SALOMEDS::UseCaseBuilder_var useCaseBuilder = aStudy->GetUseCaseBuilder();
  useCaseBuilder->AppendTo( theFather, aFolderSO );

  return aFolderSO._retn();
}

//=================================================================================
// function : MoveToFolder()
// purpose  : Moves GEOM object to the specified folder
//=================================================================================
void GEOM_Gen_i::MoveToFolder(GEOM::GEOM_Object_ptr theObject,
                              SALOMEDS::SObject_ptr theFolder) {
  GEOM::object_list_var objects = new GEOM::object_list();
  objects->length( 1 );
  SALOMEDS::SObject_var aSO = theFolder->GetStudy()->FindObjectID( theObject->GetStudyEntry() );
  objects[0] = aSO;
  Move( objects, theFolder, -1 );
}

//=================================================================================
// function : MoveListToFolder()
// purpose  : Moves list of GEOM objects to the specified folder
//=================================================================================
void GEOM_Gen_i::MoveListToFolder (const GEOM::ListOfGO& theListOfGO,
                                   SALOMEDS::SObject_ptr theFolder) {
  int aLen = theListOfGO.length();
  GEOM::object_list_var objects = new GEOM::object_list();
  objects->length( aLen );
  GEOM::GEOM_Object_var aGO;
  SALOMEDS::SObject_var aSO;
  for (int i = 0; i < aLen; i++) {
    aGO = GEOM::GEOM_Object::_duplicate( theListOfGO[i] );
    aSO = theFolder->GetStudy()->FindObjectID( aGO->GetStudyEntry() );
    objects[i] = aSO;
  }
  if ( objects->length() > 0 )
    Move( objects, theFolder, -1 );
}

//=================================================================================
// function : Move()
// purpose  : Moves objects to the specified position.
//            Is used in the drag-n-drop functionality.
//=================================================================================
void GEOM_Gen_i::Move( const GEOM::object_list& what,
                       SALOMEDS::SObject_ptr where,
                       CORBA::Long row )
{
  if ( CORBA::is_nil( where ) ) return;

  SALOMEDS::Study_var study = where->GetStudy();
  SALOMEDS::StudyBuilder_var studyBuilder = study->NewBuilder();
  SALOMEDS::UseCaseBuilder_var useCaseBuilder = study->GetUseCaseBuilder();
  SALOMEDS::SComponent_var father = where->GetFatherComponent();
  std::string dataType = father->ComponentDataType();
  if ( dataType != "GEOM" ) return; // not a GEOM component

  SALOMEDS::SObject_var objAfter;
  if ( row >= 0 && useCaseBuilder->HasChildren( where ) ) {
    // insert at given row -> find insertion position
    SALOMEDS::UseCaseIterator_var useCaseIt = useCaseBuilder->GetUseCaseIterator( where );
    int i;
    for ( i = 0; i < row && useCaseIt->More(); i++, useCaseIt->Next() );
    if ( i == row && useCaseIt->More() ) {
      objAfter = useCaseIt->Value();
    }
  }

  for ( int i = 0; i < what.length(); i++ ) {
    SALOMEDS::SObject_var sobj = what[i];
    if ( CORBA::is_nil( sobj ) ) continue; // skip bad object
    // insert the object to the use case tree
    if ( !CORBA::is_nil( objAfter ) )
      useCaseBuilder->InsertBefore( sobj, objAfter ); // insert at given row
    else
      useCaseBuilder->AppendTo( where, sobj );        // append to the end of list
  }
}

//=======================================================================
// function : GetDependencyTree
// purpose  : Collects dependencies of the given objects from other ones
//=======================================================================
SALOMEDS::TMPFile* GEOM_Gen_i::GetDependencyTree( SALOMEDS::Study_ptr theStudy,
                                                  const GEOM::string_array& theObjectEntries ) {
  // fill in the tree structure
  GEOMUtils::TreeModel tree;

  std::string entry;
  for ( int i = 0; i < theObjectEntries.length(); i++ ) {
    // process objects one-by-one
    entry = theObjectEntries[i].in();
    GEOM::GEOM_BaseObject_var anObj = GetObject( theStudy->StudyId(), entry.c_str() );
    if ( anObj->_is_nil() )
      continue;
    std::map< std::string, std::set<std::string> > passedEntries;
    GEOMUtils::LevelsList upLevelList;
    // get objects from which current one depends on recursively
    getUpwardDependency( anObj, upLevelList, passedEntries );
    GEOMUtils::LevelsList downLevelList;
    // get objects that depends on current one recursively
    getDownwardDependency( anObj, downLevelList, passedEntries );
    tree.insert( std::pair<std::string, std::pair<GEOMUtils::LevelsList,GEOMUtils::LevelsList> >(entry, std::pair<GEOMUtils::LevelsList,GEOMUtils::LevelsList>( upLevelList, downLevelList ) ) );
  }

  // translation the tree into string
  std::string treeStr;
  GEOMUtils::ConvertTreeToString( tree, treeStr );

  // put string into stream
  char* aBuffer = (char*)CORBA::string_dup(treeStr.c_str());
  int aBufferSize = strlen((char*)aBuffer);

  CORBA::Octet* anOctetBuf =  (CORBA::Octet*)aBuffer;

  SALOMEDS::TMPFile_var aStream = new SALOMEDS::TMPFile(aBufferSize, aBufferSize, anOctetBuf, 1);

  return aStream._retn();
}

//=======================================================================
// function : getUpwardDependency
// purpose  : Collects the entries of objects on that the given one depends
//=======================================================================
void GEOM_Gen_i::getUpwardDependency( GEOM::GEOM_BaseObject_ptr gbo,
                                      GEOMUtils::LevelsList &upLevelList,
                                      std::map< std::string, std::set<std::string> > &passedEntries,
                                      int level ) {
  std::string aGboEntry = gbo->GetEntry();
  GEOMUtils::NodeLinks anEntries;
  GEOMUtils::LevelInfo aLevelMap;
  if ( level > 0 ) {
    if ( level-1 >= upLevelList.size() ) {
      // create a new map
      upLevelList.push_back( aLevelMap );
    } else {
      // get the existent map
      aLevelMap = upLevelList.at(level-1);
      if ( aLevelMap.count( aGboEntry ) > 0 ) {
        anEntries = aLevelMap[ aGboEntry ];
      }
    }
  }
  // get objects on that the current one depends
  GEOM::ListOfGBO_var depList = gbo->GetDependency();
  std::string aDepEntry;
  for( int j = 0; j < depList->length(); j++ ) {
    if ( depList[j]->_is_nil() )
      continue;
    aDepEntry = depList[j]->GetEntry();
    if ( passedEntries.count( aGboEntry ) > 0 &&
         passedEntries[aGboEntry].count( aDepEntry ) > 0 ) {
      //avoid checking the passed objects
      continue;
    }
    passedEntries[aGboEntry].insert( aDepEntry );
    if ( level > 0 ) {
      anEntries.push_back( aDepEntry );
    }
    // get dependencies recursively
    getUpwardDependency(depList[j], upLevelList, passedEntries, level+1);
  }
  if ( level > 0 ) {
    aLevelMap.insert( std::pair<std::string, GEOMUtils::NodeLinks>(aGboEntry, anEntries) );
    upLevelList[level-1] = aLevelMap;
  }
}

//=======================================================================
// function : getDownwardDependency
// purpose  : Collects the entries of objects that depends on the given one
//=======================================================================
void GEOM_Gen_i::getDownwardDependency( GEOM::GEOM_BaseObject_ptr gbo,
                                        GEOMUtils::LevelsList &downLevelList,
                                        std::map< std::string, std::set<std::string> > &passedEntries,
                                        int level ) {
  std::string aGboEntry = gbo->GetEntry();
  Handle(TDocStd_Document) aDoc = GEOM_Engine::GetEngine()->GetDocument(gbo->GetStudyID());
  Handle(TDataStd_TreeNode) aNode, aRoot;
  HANDLE_NAMESPACE(GEOM_Function) aFunction;
  if (aDoc->Main().FindAttribute(GEOM_Function::GetFunctionTreeID(), aRoot)) {
    // go through the whole OCAF tree
    TDataStd_ChildNodeIterator Itr( aRoot );
    for (; Itr.More(); Itr.Next()) {
      aNode = Itr.Value();
      aFunction = GEOM_Function::GetFunction(aNode->Label());
      if (aFunction.IsNull()) {
        continue;
      }
      TDF_Label aLabel  = aFunction->GetOwnerEntry();
      if(aLabel.IsNull()) continue;
      TCollection_AsciiString anEntry;
      TDF_Tool::Entry(aLabel, anEntry);
      GEOM::GEOM_BaseObject_var geomObj = GetObject( gbo->GetStudyID(), anEntry.ToCString() );
      if( CORBA::is_nil( geomObj ) )
        continue;
      // get dependencies for current object in the tree
      GEOM::ListOfGBO_var depList = geomObj->GetDependency();
      if( depList->length() == 0 )
        continue;
      std::string aGoEntry = geomObj->GetEntry();
      // go through dependencies of current object to check whether it depends on the given object
      for( int i = 0; i < depList->length(); i++ ) {
        if ( depList[i]->_is_nil() )
          continue;
        if ( depList[i]->_is_equivalent( gbo ) ) {
          // yes, the current object depends on the given object
          if ( passedEntries.count( aGoEntry ) > 0 &&
               passedEntries[aGoEntry].count( aGboEntry ) > 0 ) {
            //avoid checking the passed objects
            continue;
          }
          passedEntries[aGoEntry].insert( aGboEntry );
          GEOMUtils::NodeLinks anEntries;
          GEOMUtils::LevelInfo aLevelMap;
          anEntries.push_back( aGboEntry );
          if ( level >= downLevelList.size() ) {
            downLevelList.push_back( aLevelMap );
          } else {
            aLevelMap = downLevelList.at(level);
            if ( aLevelMap.count( aGoEntry ) > 0 ) {
              anEntries = aLevelMap[ aGoEntry ];
            }
          }
          aLevelMap.insert( std::pair<std::string, GEOMUtils::NodeLinks>(aGoEntry, anEntries) );
          downLevelList[level] = aLevelMap;
          // get dependencies of the current object recursively
          getDownwardDependency(geomObj, downLevelList, passedEntries, level+1);
          break;
        }
      }
    }
  }
}

//==============================================================================
// function : GetEntriesToReduceStudy
// purpose  : Fills 3 lists that is used to clean study of redundant objects
//==============================================================================
void GEOM_Gen_i::GetEntriesToReduceStudy(SALOMEDS::Study_ptr theStudy,
                                        GEOM::string_array& theSelectedEntries,
                                        GEOM::string_array& theParentEntries,
                                        GEOM::string_array& theSubEntries,
                                        GEOM::string_array& theOtherEntries)
{
  std::set<std::string> aSelected, aParents, aChildren, anOthers;
  for ( int i = 0; i < theSelectedEntries.length(); i++ ) {
    aSelected.insert( CORBA::string_dup( theSelectedEntries[i] ) );
  }

  Handle(TDocStd_Document) aDoc = GEOM_Engine::GetEngine()->GetDocument(theStudy->StudyId());
  Handle(TDataStd_TreeNode) aNode, aRoot;
  HANDLE_NAMESPACE(GEOM_Function) aFunction;
  if (aDoc->Main().FindAttribute(GEOM_Function::GetFunctionTreeID(), aRoot)) {
    // go through the whole OCAF tree
    TDF_Label aLabel;
    std::string anEntry;
    TCollection_AsciiString anAsciiEntry;
    TDataStd_ChildNodeIterator Itr( aRoot );
    for (; Itr.More(); Itr.Next()) {
      aNode = Itr.Value();
      aFunction = GEOM_Function::GetFunction(aNode->Label());
      if (aFunction.IsNull()) {
        continue;
      }
      aLabel = aFunction->GetOwnerEntry();
      if(aLabel.IsNull())
        continue;
      TDF_Tool::Entry(aLabel, anAsciiEntry);
      anEntry = anAsciiEntry.ToCString();
      GEOM::GEOM_BaseObject_var geomObj = GetObject( theStudy->StudyId(), anEntry.c_str() );
      if( CORBA::is_nil( geomObj ) )
        continue;

      if ( aSelected.count( anEntry ) > 0 &&
           aParents.count( anEntry ) == 0 ) {
        includeParentDependencies( geomObj, aSelected, aParents, aChildren, anOthers );
      } else if ( aParents.count( anEntry ) == 0 &&
                  aChildren.count( anEntry ) == 0 ) {
        anOthers.insert( geomObj->GetEntry() );
      }
    }

    std::set<std::string>::iterator it;
    std::set<std::string>::iterator foundIt;
    TCollection_AsciiString stringIOR;
    GEOM::GEOM_Object_var geomObj;

    // filling list of sub-objects
    for ( it = aSelected.begin(); it != aSelected.end(); ++it ) {
      includeSubObjects( theStudy, *it, aSelected, aParents, aChildren, anOthers );
    }

    // if some selected object is not a main shape,
    // we move it's main shapes into 'selected' list,
    // because they could not be modified anyhow.
    std::set<std::string> aToBeInSelected;
    for ( it = aSelected.begin(); it != aSelected.end(); ++it ) {
      HANDLE_NAMESPACE(GEOM_BaseObject) handle_object = _impl->GetObject( theStudy->StudyId(), (*it).c_str(), false);
      if ( handle_object.IsNull() )
        continue;

      stringIOR = handle_object->GetIOR();
      if ( !stringIOR.Length() > 1 )
        continue;

      geomObj = GetIORFromString( stringIOR.ToCString() );
      while ( !geomObj->IsMainShape() ) {
        geomObj = geomObj->GetMainShape();
        anEntry = geomObj->GetEntry();

        foundIt = aParents.find( anEntry );
        if ( foundIt != aParents.end() )
          aParents.erase( foundIt );

        foundIt = aChildren.find( anEntry );
        if ( foundIt != aChildren.end() )
          aChildren.erase( foundIt );

        foundIt = anOthers.find( anEntry );
        if ( foundIt != anOthers.end() )
          anOthers.erase( foundIt );

        aToBeInSelected.insert( anEntry );
      }
    }
    aSelected.insert( aToBeInSelected.begin(), aToBeInSelected.end() );

    // fill the CORBA arrays with values from sets
    int i;
    theSelectedEntries.length( aSelected.size() );
    for ( i = 0, it = aSelected.begin(); it != aSelected.end(); ++it, i++ )
      theSelectedEntries[i]  = CORBA::string_dup( (*it).c_str() );
    theParentEntries.length( aParents.size() );
    for ( i = 0, it = aParents.begin(); it != aParents.end(); ++it, i++ )
      theParentEntries[i]  = CORBA::string_dup( (*it).c_str() );
    theSubEntries.length( aChildren.size() );
    for ( i = 0, it = aChildren.begin(); it != aChildren.end(); ++it, i++ )
      theSubEntries[i]  = CORBA::string_dup( (*it).c_str() );
    theOtherEntries.length( anOthers.size() );
    for ( i = 0, it = anOthers.begin(); it != anOthers.end(); ++it, i++ )
      theOtherEntries[i]  = CORBA::string_dup( (*it).c_str() );
  }
}

//==============================================================================
// function : includeParentDependencies
// purpose  :
//==============================================================================
void GEOM_Gen_i::includeParentDependencies(GEOM::GEOM_BaseObject_ptr geomObj,
                                           std::set<std::string>& aSelected,
                                           std::set<std::string>& aParents,
                                           std::set<std::string>& aChildren,
                                           std::set<std::string>& anOthers)
{
  std::string anEntry = geomObj->GetEntry();
  if ( aSelected.count( anEntry ) == 0 ) {
    aParents.insert( anEntry );
    std::set<std::string>::iterator it;
    it = aChildren.find( anEntry );
    if ( it != aChildren.end() )
      aChildren.erase( it );
    it = anOthers.find( anEntry );
    if ( it != anOthers.end() )
      anOthers.erase( it );
  }
  // get dependencies for current object in the tree
  GEOM::ListOfGBO_var depList = geomObj->GetDependency();
  if( depList->length() == 0 )
    return;
  // go through dependencies of current object to check whether it depends on the given object
  std::string aDepEntry;
  for( int i = 0; i < depList->length(); i++ ) {
    aDepEntry = depList[i]->GetEntry();
    if ( depList[i]->_is_nil() ||
         aDepEntry == anEntry ||             // skip self-depending
         aSelected.count( aDepEntry ) > 0 || // skip selected objects
         aParents.count( aDepEntry ) > 0     // skip already processed objects
         )
      continue;
    includeParentDependencies( depList[i], aSelected, aParents, aChildren, anOthers );
  }
}

//==============================================================================
// function : includeSubObjects
// purpose  :
//==============================================================================
void GEOM_Gen_i::includeSubObjects(SALOMEDS::Study_ptr theStudy,
                                   const std::string& aSelectedEntry,
                                   std::set<std::string>& aSelected,
                                   std::set<std::string>& aParents,
                                   std::set<std::string>& aChildren,
                                   std::set<std::string>& anOthers)
{
  std::set<std::string>::iterator foundIt;
  HANDLE_NAMESPACE(GEOM_BaseObject) handle_object = _impl->GetObject( theStudy->StudyId(), aSelectedEntry.c_str(), false);
  if ( handle_object.IsNull() )
    return;

  HANDLE_NAMESPACE(GEOM_Function) aShapeFun = handle_object->GetFunction(1);
  if ( aShapeFun.IsNull() || !aShapeFun->HasSubShapeReferences() )
    return;

  const TDataStd_ListOfExtendedString& aListEntries = aShapeFun->GetSubShapeReferences();
  if ( aListEntries.IsEmpty() )
    return;

  TDataStd_ListIteratorOfListOfExtendedString anIt (aListEntries);
  for ( ; anIt.More(); anIt.Next() ) {
    TCollection_ExtendedString aSubEntry = anIt.Value();
    Standard_Integer aStrLen = aSubEntry.LengthOfCString();
    char* aSubEntryStr = new char[aStrLen+1];
    aSubEntry.ToUTF8CString( aSubEntryStr );
    foundIt = aParents.find( aSubEntryStr );
    if ( foundIt == aParents.end() ) { // add to sub-objects if it is not in parents list
      foundIt = aSelected.find( aSubEntryStr );
      if ( foundIt == aSelected.end() ) { // add to sub-objects if it is not in selected list
            aChildren.insert( aSubEntryStr );
            foundIt = anOthers.find( aSubEntryStr );
            if ( foundIt != anOthers.end() )
              anOthers.erase( foundIt );
      }
    }
    includeSubObjects( theStudy, aSubEntryStr, aSelected, aParents, aChildren, anOthers );
  }
}
//=====================================================================================
// EXPORTED METHODS
//=====================================================================================
extern "C"
{
  /*
  GEOM_I_EXPORT
  PortableServer::ObjectId* GEOMEngine_factory(CORBA::ORB*, PortableServer::POA*, PortableServer::ObjectId*, const char*, const char*);
  */

  GEOM_I_EXPORT
  PortableServer::ObjectId* GEOMEngine_factory(CORBA::ORB_ptr            orb,
                                               PortableServer::POA_ptr   poa,
                                               PortableServer::ObjectId* contId,
                                               const char*               instanceName,
                                               const char*               interfaceName)
  {
    GEOM_Gen_i* myGEOM_Gen_i = new GEOM_Gen_i(orb, poa, contId, instanceName, interfaceName);
    return myGEOM_Gen_i->getId();
  }
}
