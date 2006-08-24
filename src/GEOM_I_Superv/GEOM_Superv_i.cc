// Copyright (C) 2005  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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

#include "GEOM_Superv_i.hh"
#include "SALOME_LifeCycleCORBA.hxx"

#include CORBA_SERVER_HEADER(SALOME_Session)
#include "SALOMEDSClient_ClientFactory.hxx"

#define isNewStudy(a,b) (a > 0 && a != b)

using namespace std;
//=============================================================================
//  constructor:
//=============================================================================
GEOM_Superv_i::GEOM_Superv_i(CORBA::ORB_ptr orb,
			     PortableServer::POA_ptr poa,
			     PortableServer::ObjectId * contId, 
			     const char *instanceName, 
			     const char *interfaceName) :
  Engines_Component_i(orb, poa, contId, instanceName, interfaceName)
{
  MESSAGE("GEOM_Superv_i::GEOM_Superv_i");

  _thisObj = this ;
  _id = _poa->activate_object(_thisObj);
  name_service = new SALOME_NamingService(_orb);
  //get RootPOA (the default)
  //myPOA = PortableServer::RefCountServantBase::_default_POA();
  CORBA::Object_var anObj = _orb->resolve_initial_references("RootPOA");
  myPOA = PortableServer::POA::_narrow(anObj);

  setGeomEngine();

  myStudyID = -1;
  myLastStudyID = -1;

  myBasicOp = GEOM::GEOM_IBasicOperations::_nil();
  my3DPrimOp = GEOM::GEOM_I3DPrimOperations::_nil();
  myBoolOp = GEOM::GEOM_IBooleanOperations::_nil();
  myInsOp = GEOM::GEOM_IInsertOperations::_nil();
  myTransfOp = GEOM::GEOM_ITransformOperations::_nil();
  myShapesOp = GEOM::GEOM_IShapesOperations::_nil();
  myBlocksOp = GEOM::GEOM_IBlocksOperations::_nil();
  myCurvesOp = GEOM::GEOM_ICurvesOperations::_nil();
  myLocalOp = GEOM::GEOM_ILocalOperations::_nil();
  myGroupOp = GEOM::GEOM_IGroupOperations::_nil();
}

//=============================================================================
//  destructor
//=============================================================================
GEOM_Superv_i::~GEOM_Superv_i()
{
  MESSAGE("GEOM_Superv_i::~GEOM_Superv_i");
  delete name_service;
}

//============================================================================
// function : register() 
// purpose  : register 'name' in 'name_service'
//============================================================================
void GEOM_Superv_i::register_name(char * name)
{
  GEOM::GEOM_Superv_ptr g = GEOM::GEOM_Superv::_narrow(POA_GEOM::GEOM_Superv::_this());
  name_service->Register(g, strdup(name)); 
}

//=============================================================================
//  setGEOMEngine:
//=============================================================================
void GEOM_Superv_i::setGeomEngine()
{
  // get GEOM_Gen engine
  SALOME_LifeCycleCORBA* lcc = new SALOME_LifeCycleCORBA( name_service );
  Engines::Component_var comp = lcc->FindOrLoad_Component( "FactoryServer", "GEOM" );

  myGeomEngine = GEOM::GEOM_Gen::_narrow(comp);
}

//=============================================================================
//  SetStudyID:
//=============================================================================
void GEOM_Superv_i::SetStudyID( CORBA::Long theId )
{
  // mkr : PAL10770 -->
  myLastStudyID = myStudyID;

  CORBA::Object_ptr anObject = name_service->Resolve("/Kernel/Session");
  if ( !CORBA::is_nil(anObject) ) {
    SALOME::Session_var aSession = SALOME::Session::_narrow(anObject);
    if ( !CORBA::is_nil(aSession) ) {
      int aStudyID = aSession->GetActiveStudyId();
      if ( theId != aStudyID && aStudyID > 0) { // mkr : IPAL12128
	MESSAGE("Warning : given study ID theId="<<theId<<" is wrong and will be replaced by the value "<<aStudyID);
	myStudyID = aStudyID;
      }
      else
	myStudyID = theId; // mkr : IPAL12128
    }
  }
  
  if ( isNewStudy(myLastStudyID,myStudyID) ) {
    if (CORBA::is_nil(myGeomEngine)) setGeomEngine();
    string anEngine = _orb->object_to_string( myGeomEngine );
    
    CORBA::Object_var anObj = name_service->Resolve("/myStudyManager");
    if ( !CORBA::is_nil(anObj) ) {
      SALOMEDS::StudyManager_var aStudyManager = SALOMEDS::StudyManager::_narrow(anObj);
      if ( !CORBA::is_nil(aStudyManager) ) {
	_PTR(Study) aDSStudy = ClientFactory::Study(aStudyManager->GetStudyByID(myStudyID));
	if ( aDSStudy ) {
	  _PTR(SComponent) aSCO = aDSStudy->FindComponent(myGeomEngine->ComponentDataType());
	  if ( aSCO ) {
	    _PTR(StudyBuilder) aBuilder = aDSStudy->NewBuilder();
	    if ( aBuilder ) aBuilder->LoadWith( aSCO, anEngine );
	  }
	}
      }
    }
  }
  // mkr : PAL10770 <--
}

//=============================================================================
//  CreateListOfGO:
//=============================================================================
GEOM::GEOM_List_ptr GEOM_Superv_i::CreateListOfGO()
{
  MESSAGE("GEOM_Superv_i::CreateListOfGO()");
  GEOM_List_i<GEOM::ListOfGO>* aListPtr = new GEOM_List_i<GEOM::ListOfGO>();
  return aListPtr->_this();
}

//=============================================================================
//  AddItemToListOfGO:
//=============================================================================
void GEOM_Superv_i::AddItemToListOfGO(GEOM::GEOM_List_ptr& theList, 
				      GEOM::GEOM_Object_ptr    theObject)
{
  MESSAGE("GEOM_Superv_i::AddItemToListOfGO(...)");
  if (GEOM_List_i<GEOM::ListOfGO>* aList = 
      dynamic_cast<GEOM_List_i<GEOM::ListOfGO>*>(GetServant(theList, myPOA).in())) {
    aList->AddObject(theObject);
    MESSAGE(" NewLength = "<<aList->GetList().length());
  }
}

//=============================================================================
//  CreateListOfLong:
//=============================================================================
GEOM::GEOM_List_ptr GEOM_Superv_i::CreateListOfLong()
{
  MESSAGE("GEOM_Superv_i::CreateListOfLong()");
  GEOM_List_i<GEOM::ListOfLong>* aListPtr = new GEOM_List_i<GEOM::ListOfLong>();
  return aListPtr->_this();
}

//=============================================================================
//  AddItemToListOfLong:
//=============================================================================
void GEOM_Superv_i::AddItemToListOfLong(GEOM::GEOM_List_ptr& theList, 
					CORBA::Long theObject)
{
  MESSAGE("GEOM_Superv_i::AddItemToListOfLong(...)");
  if (GEOM_List_i<GEOM::ListOfLong>* aList = 
      dynamic_cast<GEOM_List_i<GEOM::ListOfLong>*>(GetServant(theList, myPOA).in())) {
    aList->AddObject(theObject);
    MESSAGE(" NewLength = "<<aList->GetList().length());
  }
}

//=============================================================================
//  CreateListOfDouble:
//=============================================================================
GEOM::GEOM_List_ptr GEOM_Superv_i::CreateListOfDouble()
{
  MESSAGE("GEOM_Superv_i::CreateListOfDouble()");
  GEOM_List_i<GEOM::ListOfDouble>* aListPtr = new GEOM_List_i<GEOM::ListOfDouble>();
  return aListPtr->_this();
}

//=============================================================================
//  AddItemToListOfDouble:
//=============================================================================
void GEOM_Superv_i::AddItemToListOfDouble(GEOM::GEOM_List_ptr& theList, 
					  CORBA::Double theObject)
{
  MESSAGE("GEOM_Superv_i::AddItemToListOfDouble(...)");
  if (GEOM_List_i<GEOM::ListOfDouble>* aList = 
      dynamic_cast<GEOM_List_i<GEOM::ListOfDouble>*>(GetServant(theList, myPOA).in())) {
    aList->AddObject(theObject);
    MESSAGE(" NewLength = "<<aList->GetList().length());
  }
}

//=============================================================================
//  getBasicOp:
//=============================================================================
void GEOM_Superv_i::getBasicOp()
{
  if (CORBA::is_nil(myGeomEngine))
    setGeomEngine();
  // get GEOM_IBasicOperations interface
  if (CORBA::is_nil(myBasicOp) || isNewStudy(myLastStudyID,myStudyID))
    myBasicOp = myGeomEngine->GetIBasicOperations(myStudyID);
}

//=============================================================================
//  get3DPrimOp:
//=============================================================================
void GEOM_Superv_i::get3DPrimOp()
{
  if (CORBA::is_nil(myGeomEngine))
    setGeomEngine();
  // get GEOM_I3DPrimOperations interface
  if (CORBA::is_nil(my3DPrimOp) || isNewStudy(myLastStudyID,myStudyID))
    my3DPrimOp = myGeomEngine->GetI3DPrimOperations(myStudyID);
}

//=============================================================================
//  getBoolOp:
//=============================================================================
void GEOM_Superv_i::getBoolOp()
{
  if (CORBA::is_nil(myGeomEngine))
    setGeomEngine();
  // get GEOM_IBooleanOperations interface
  if (CORBA::is_nil(myBoolOp) || isNewStudy(myLastStudyID,myStudyID))
    myBoolOp = myGeomEngine->GetIBooleanOperations(myStudyID);
}

//=============================================================================
//  getInsOp:
//=============================================================================
void GEOM_Superv_i::getInsOp()
{
  if (CORBA::is_nil(myGeomEngine))
    setGeomEngine();
  // get GEOM_IInsertOperations interface
  if (CORBA::is_nil(myInsOp) || isNewStudy(myLastStudyID,myStudyID))
    myInsOp = myGeomEngine->GetIInsertOperations(myStudyID);
}

//=============================================================================
//  getTransfOp:
//=============================================================================
void GEOM_Superv_i::getTransfOp()
{
  if (CORBA::is_nil(myGeomEngine))
    setGeomEngine();
  // get GEOM_ITransformOperations interface
  if (CORBA::is_nil(myTransfOp) || isNewStudy(myLastStudyID,myStudyID))
    myTransfOp = myGeomEngine->GetITransformOperations(myStudyID);
}

//=============================================================================
//  getShapesOp:
//=============================================================================
void GEOM_Superv_i::getShapesOp()
{
  if (CORBA::is_nil(myGeomEngine))
    setGeomEngine();
  // get GEOM_IShapesOperations interface
  if (CORBA::is_nil(myShapesOp) || isNewStudy(myLastStudyID,myStudyID))
    myShapesOp = myGeomEngine->GetIShapesOperations(myStudyID);
}

//=============================================================================
//  getBlocksOp:
//=============================================================================
void GEOM_Superv_i::getBlocksOp()
{
  if (CORBA::is_nil(myGeomEngine))
    setGeomEngine();
  // get GEOM_IBlocksOperations interface
  if (CORBA::is_nil(myBlocksOp) || isNewStudy(myLastStudyID,myStudyID))
    myBlocksOp = myGeomEngine->GetIBlocksOperations(myStudyID);
}

//=============================================================================
//  getCurvesOp:
//=============================================================================
void GEOM_Superv_i::getCurvesOp()
{
  if (CORBA::is_nil(myGeomEngine))
    setGeomEngine();
  // get GEOM_ICurvesOperations interface
  if (CORBA::is_nil(myCurvesOp) || isNewStudy(myLastStudyID,myStudyID))
    myCurvesOp = myGeomEngine->GetICurvesOperations(myStudyID);
}

//=============================================================================
//  getLocalOp:
//=============================================================================
void GEOM_Superv_i::getLocalOp()
{
  if (CORBA::is_nil(myGeomEngine))
    setGeomEngine();
  // get GEOM_ILocalOperations interface
  if (CORBA::is_nil(myLocalOp) || isNewStudy(myLastStudyID,myStudyID))
    myLocalOp = myGeomEngine->GetILocalOperations(myStudyID);
}

//=============================================================================
//  getGroupOp:
//=============================================================================
void GEOM_Superv_i::getGroupOp()
{
  if (CORBA::is_nil(myGeomEngine))
    setGeomEngine();
  // get GEOM_IGroupOperations interface
  if (CORBA::is_nil(myGroupOp) || isNewStudy(myLastStudyID,myStudyID))
    myGroupOp = myGeomEngine->GetIGroupOperations(myStudyID);
}

//=============================================================================
//  GetServant:
//=============================================================================
PortableServer::ServantBase_var GEOM_Superv_i::GetServant(CORBA::Object_ptr       theObject,
							  PortableServer::POA_ptr thePOA)
{
  if(CORBA::is_nil(theObject))  return NULL;
  PortableServer::Servant aServant = thePOA->reference_to_servant(theObject);
  return aServant;
}

//============================================================================
// function : Save()
// purpose  : save OCAF/Geom document
//============================================================================
SALOMEDS::TMPFile* GEOM_Superv_i::Save(SALOMEDS::SComponent_ptr theComponent,
				       const char* theURL,
				       CORBA::Boolean isMultiFile)
{
  SALOMEDS::TMPFile_var aStreamFile;
  return aStreamFile._retn();
}

//============================================================================
// function : SaveASCII()
// purpose  :
//============================================================================ 
SALOMEDS::TMPFile* GEOM_Superv_i::SaveASCII(SALOMEDS::SComponent_ptr theComponent,
					    const char* theURL,
					    CORBA::Boolean isMultiFile)
{
  SALOMEDS::TMPFile_var aStreamFile;
  return aStreamFile._retn();
}

//============================================================================
// function : Load()
// purpose  :
//============================================================================ 
CORBA::Boolean GEOM_Superv_i::Load(SALOMEDS::SComponent_ptr theComponent,
				   const SALOMEDS::TMPFile& theStream,
				   const char* theURL,
				   CORBA::Boolean isMultiFile)
{
  return false;
}

//============================================================================
// function : LoadASCII()
// purpose  :
//============================================================================ 
CORBA::Boolean GEOM_Superv_i::LoadASCII(SALOMEDS::SComponent_ptr theComponent,
					const SALOMEDS::TMPFile& theStream,
					const char* theURL,
					CORBA::Boolean isMultiFile)
{
  return false;
}

//============================================================================
// function : Close()
// purpose  :
//============================================================================
void GEOM_Superv_i::Close(SALOMEDS::SComponent_ptr theComponent)
{
}

//============================================================================
// function : ComponentDataType()
// purpose  :
//============================================================================
char* GEOM_Superv_i::ComponentDataType()
{
  return 0;
}

//============================================================================
// function : IORToLocalPersistentID()
// purpose  :
//============================================================================
char* GEOM_Superv_i::IORToLocalPersistentID(SALOMEDS::SObject_ptr theSObject,
					    const char* IORString,
					    CORBA::Boolean isMultiFile,
					    CORBA::Boolean isASCII)
{
  return 0;
}

//============================================================================
// function : LocalPersistentIDToIOR()
// purpose  : Create/Load CORBA object from a persistent ref (an entry)
//          : Used when a study is loaded
//          : The IOR (IORName) of object created is returned
//============================================================================
char* GEOM_Superv_i::LocalPersistentIDToIOR(SALOMEDS::SObject_ptr theSObject,
					    const char* aLocalPersistentID,
					    CORBA::Boolean isMultiFile,
					    CORBA::Boolean isASCII)
{
  return 0;
}

//============================================================================
// function : CanPublishInStudy
// purpose  : 
//============================================================================
CORBA::Boolean GEOM_Superv_i::CanPublishInStudy(CORBA::Object_ptr theIOR)
{
  if (CORBA::is_nil(myGeomEngine))
    setGeomEngine();
  return myGeomEngine->CanPublishInStudy(theIOR);
}

//============================================================================
// function : PublishInStudy
// purpose  : 
//============================================================================
SALOMEDS::SObject_ptr GEOM_Superv_i::PublishInStudy(SALOMEDS::Study_ptr theStudy,
						    SALOMEDS::SObject_ptr theSObject,
						    CORBA::Object_ptr theObject,
						    const char* theName) throw (SALOME::SALOME_Exception)
{
  if (CORBA::is_nil(myGeomEngine))
    setGeomEngine();
  return myGeomEngine->PublishInStudy(theStudy, theSObject, theObject, theName);
}

//============================================================================
// function : CanCopy()
// purpose  :
//============================================================================
CORBA::Boolean GEOM_Superv_i::CanCopy(SALOMEDS::SObject_ptr theObject)
{
  return false;
}

//============================================================================
// function : CopyFrom()
// purpose  :
//============================================================================
SALOMEDS::TMPFile* GEOM_Superv_i::CopyFrom(SALOMEDS::SObject_ptr theObject, CORBA::Long& theObjectID)
{
  SALOMEDS::TMPFile_var aStreamFile;
  return aStreamFile._retn();
}

//============================================================================
// function : CanPaste()
// purpose  :
//============================================================================
CORBA::Boolean GEOM_Superv_i::CanPaste(const char* theComponentName, CORBA::Long theObjectID)
{
  return false;
}

//============================================================================
// function : PasteInto()
// purpose  :
//============================================================================
SALOMEDS::SObject_ptr GEOM_Superv_i::PasteInto(const SALOMEDS::TMPFile& theStream,
					       CORBA::Long theObjectID,
					       SALOMEDS::SObject_ptr theObject)
{
  SALOMEDS::SObject_var aNewSO;
  return aNewSO._retn();
}

//================= Primitives Construction : BasicOperations =================
//=============================================================================
//  MakePointXYZ:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakePointXYZ(CORBA::Double theX,
						  CORBA::Double theY,
						  CORBA::Double theZ)
{
  beginService( " GEOM_Superv_i::MakePointXYZ" );
  MESSAGE("GEOM_Superv_i::MakePointXYZ");
  getBasicOp();
  // make vertex and return
  GEOM::GEOM_Object_ptr anObj = myBasicOp->MakePointXYZ(theX, theY, theZ);
  endService( " GEOM_Superv_i::MakePointXYZ" );
  return anObj;
}

//=============================================================================
//  MakePointWithReference:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakePointWithReference (GEOM::GEOM_Object_ptr theReference,
							     CORBA::Double theX, 
							     CORBA::Double theY, 
							     CORBA::Double theZ)
{
  beginService( " GEOM_Superv_i::MakePointWithReference" );
  MESSAGE("GEOM_Superv_i::MakePointWithReference");
  getBasicOp();
  GEOM::GEOM_Object_ptr anObj = myBasicOp->MakePointWithReference(theReference, theX, theY, theZ);
  endService( " GEOM_Superv_i::MakePointWithReference" );
  return anObj;
}

//=============================================================================
//  MakePointOnCurve:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakePointOnCurve (GEOM::GEOM_Object_ptr theRefCurve,
						       CORBA::Double theParameter)
{
  beginService( " GEOM_Superv_i::MakePointOnCurve" );
  MESSAGE("GEOM_Superv_i::MakePointOnCurve");
  getBasicOp();
  GEOM::GEOM_Object_ptr anObj = myBasicOp->MakePointOnCurve(theRefCurve, theParameter);
  endService( " GEOM_Superv_i::MakePointOnCurve" );
  return anObj;
}

//=============================================================================
//  MakeTangentOnCurve:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeTangentOnCurve (GEOM::GEOM_Object_ptr theRefCurve,
							 CORBA::Double theParameter)
{
  beginService( " GEOM_Superv_i::MakeTangentOnCurve" );
  MESSAGE("GEOM_Superv_i::MakeTangentOnCurve");
  getBasicOp();
  GEOM::GEOM_Object_ptr anObj = myBasicOp->MakeTangentOnCurve(theRefCurve, theParameter);
  endService( " GEOM_Superv_i::MakeTangentOnCurve" );
  return anObj;
}

//=============================================================================
//  MakeVectorDXDYDZ:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeVectorDXDYDZ (CORBA::Double theDX,
						       CORBA::Double theDY,
						       CORBA::Double theDZ)
{
  beginService( " GEOM_Superv_i::MakeVectorDXDYDZ" );
  MESSAGE("GEOM_Superv_i::MakeVectorDXDYDZ");
  getBasicOp();
  GEOM::GEOM_Object_ptr anObj = myBasicOp->MakeVectorDXDYDZ(theDX, theDY, theDZ);
  endService( " GEOM_Superv_i::MakeVectorDXDYDZ" );
  return anObj;
}

//=============================================================================
//  MakeVectorTwoPnt:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeVectorTwoPnt (GEOM::GEOM_Object_ptr thePnt1,
						       GEOM::GEOM_Object_ptr thePnt2)
{
  beginService( " GEOM_Superv_i::MakeVectorTwoPnt" );
  MESSAGE("GEOM_Superv_i::MakeVector");
  getBasicOp();
  GEOM::GEOM_Object_ptr anObj = myBasicOp->MakeVectorTwoPnt(thePnt1, thePnt2);
  endService( " GEOM_Superv_i::MakeVectorTwoPnt" );
  return anObj;
}

//=============================================================================
//  MakeLineTwoPnt:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeLineTwoPnt (GEOM::GEOM_Object_ptr thePnt1,
						     GEOM::GEOM_Object_ptr thePnt2)
{
  beginService( " GEOM_Superv_i::MakeLineTwoPnt");
  MESSAGE("GEOM_Superv_i::MakeLineTwoPnt");
  getBasicOp();
  GEOM::GEOM_Object_ptr anObj = myBasicOp->MakeLineTwoPnt(thePnt1, thePnt2);
  endService( " GEOM_Superv_i::MakeLineTwoPnt");
  return anObj;
}

//=============================================================================
//  MakePlaneThreePnt:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakePlaneThreePnt (GEOM::GEOM_Object_ptr thePnt1,
							GEOM::GEOM_Object_ptr thePnt2,
							GEOM::GEOM_Object_ptr thePnt3,
							CORBA::Double theTrimSize)
{
  beginService( " GEOM_Superv_i::MakePlaneThreePnt");
  MESSAGE("GEOM_Superv_i::MakePlaneThreePnt");
  getBasicOp();
  GEOM::GEOM_Object_ptr anObj = myBasicOp->MakePlaneThreePnt(thePnt1, thePnt2, thePnt3, theTrimSize);
  endService( " GEOM_Superv_i::MakePlaneThreePnt");
  return anObj;
}

//=============================================================================
//  MakePlanePntVec:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakePlanePntVec (GEOM::GEOM_Object_ptr thePnt,
						      GEOM::GEOM_Object_ptr theVec,
						      CORBA::Double theTrimSize)
{ 
  beginService( " GEOM_Superv_i::MakePlanePntVec" );
  MESSAGE("GEOM_Superv_i::MakePlanePntVec");
  getBasicOp();
  GEOM::GEOM_Object_ptr anObj = myBasicOp->MakePlanePntVec(thePnt, theVec, theTrimSize);
  endService( " GEOM_Superv_i::MakePlanePntVec" );
  return anObj;
}

//=============================================================================
//  MakePlaneFace:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakePlaneFace (GEOM::GEOM_Object_ptr theFace,
						    CORBA::Double theTrimSize)
{
  beginService( " GEOM_Superv_i::MakePlaneFace" );
  MESSAGE("GEOM_Superv_i::MakePlaneFace");
  getBasicOp();
  GEOM::GEOM_Object_ptr anObj = myBasicOp->MakePlaneFace(theFace, theTrimSize);
  endService( " GEOM_Superv_i::MakePlaneFace" );
  return anObj;
}

//=============================================================================
//  MakeMarker:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeMarker 
(CORBA::Double theOX , CORBA::Double theOY , CORBA::Double theOZ,
 CORBA::Double theXDX, CORBA::Double theXDY, CORBA::Double theXDZ,
 CORBA::Double theYDX, CORBA::Double theYDY, CORBA::Double theYDZ)
{
  beginService( " GEOM_Superv_i::MakeMarker" );
  MESSAGE("GEOM_Superv_i::MakeMarker");
  getBasicOp();
  GEOM::GEOM_Object_ptr anObj = myBasicOp->MakeMarker(theOX, theOY, theOZ, theXDX, theXDY, theXDZ, theYDX, theYDY, theYDZ);
  endService( " GEOM_Superv_i::MakeMarker" );
  return anObj;
}

//=============================================================================
//  MakeTangentPlaneOnFace:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeTangentPlaneOnFace (GEOM::GEOM_Object_ptr theFace, 
						 CORBA::Double theParameterU,
						 CORBA::Double theParameterV,
						 CORBA::Double theTrimSize)
{
  beginService( " GEOM_Superv_i::MakeTangentPlaneOnFace" );
  MESSAGE("GEOM_Superv_i::MakeTangentPlaneOnFace");
  getBasicOp();
  GEOM::GEOM_Object_ptr anObj = myBasicOp->MakeTangentPlaneOnFace(theFace, theParameterU,theParameterV,theTrimSize);
  endService( " GEOM_Superv_i::MakeTangentPlaneOnFace" );
  return anObj;
}

//================= Primitives Construction : 3DPrimOperations ================
//=============================================================================
//  MakeBox:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeBox (CORBA::Double theX1,
					      CORBA::Double theY1,
					      CORBA::Double theZ1,
					      CORBA::Double theX2,
					      CORBA::Double theY2,
					      CORBA::Double theZ2)
{
  beginService( " GEOM_Superv_i::MakeBox" );
  MESSAGE("GEOM_Superv_i::MakeBox");
  getBasicOp();
  get3DPrimOp();
  GEOM::GEOM_Object_ptr anObj = my3DPrimOp->MakeBoxTwoPnt(myBasicOp->MakePointXYZ(theX1, theY1, theZ1),
							  myBasicOp->MakePointXYZ(theX2, theY2, theZ2));
  endService( " GEOM_Superv_i::MakeBox" );
  return anObj;
}

//=============================================================================
//  MakeBoxDXDYDZ:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeBoxDXDYDZ (CORBA::Double theDX, 
						    CORBA::Double theDY, 
						    CORBA::Double theDZ)
{
  beginService( " GEOM_Superv_i::MakeBoxDXDYDZ" );
  MESSAGE("GEOM_Superv_i::MakeBoxDXDYDZ");
  get3DPrimOp();
  GEOM::GEOM_Object_ptr anObj = my3DPrimOp->MakeBoxDXDYDZ(theDX, theDY, theDZ);
  endService( " GEOM_Superv_i::MakeBoxDXDYDZ" );
  return anObj;
}

//=============================================================================
//  MakeBoxTwoPnt:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeBoxTwoPnt (GEOM::GEOM_Object_ptr thePnt1, 
						    GEOM::GEOM_Object_ptr thePnt2)
{
  beginService( " GEOM_Superv_i::MakeBoxTwoPnt" );
  MESSAGE("GEOM_Superv_i::MakeBoxTwoPnt");
  get3DPrimOp();
  GEOM::GEOM_Object_ptr anObj = my3DPrimOp->MakeBoxTwoPnt(thePnt1, thePnt2);
  endService( " GEOM_Superv_i::MakeBoxTwoPnt" );
  return anObj;
}

//=============================================================================
//  MakeCylinderPntVecRH:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeCylinderPntVecRH (GEOM::GEOM_Object_ptr thePnt,
							   GEOM::GEOM_Object_ptr theAxis,
							   CORBA::Double theRadius,
							   CORBA::Double theHeight)
{
  beginService( " GEOM_Superv_i::MakeCylinderPntVecRH" );
  MESSAGE("GEOM_Superv_i::MakeCylinderPntVecRH");
  get3DPrimOp();
  GEOM::GEOM_Object_ptr anObj = my3DPrimOp->MakeCylinderPntVecRH(thePnt, theAxis, theRadius, theHeight);
  endService( " GEOM_Superv_i::MakeCylinderPntVecRH" );
  return anObj;
}

//=============================================================================
//  MakeCylinderRH:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeCylinderRH (CORBA::Double theR, 
						     CORBA::Double theH)
{
  beginService( " GEOM_Superv_i::MakeCylinderRH" );
  MESSAGE("GEOM_Superv_i::MakeCylinderRH");
  get3DPrimOp();
  GEOM::GEOM_Object_ptr anObj = my3DPrimOp->MakeCylinderRH(theR, theH);
  endService( " GEOM_Superv_i::MakeCylinderRH" );
  return anObj; 
}

//=============================================================================
//  MakeSphere:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeSphere  (CORBA::Double theX,
						  CORBA::Double theY,
						  CORBA::Double theZ,
						  CORBA::Double theRadius)
{
  beginService( " GEOM_Superv_i::MakeSphepe" );
  MESSAGE("GEOM_Superv_i::MakeSphepe");
  getBasicOp();
  get3DPrimOp();
  GEOM::GEOM_Object_ptr anObj = my3DPrimOp->MakeSpherePntR(myBasicOp->MakePointXYZ(theX, theY, theZ), theRadius);
  endService( " GEOM_Superv_i::MakeSphepe" );
  return anObj;
}

//=============================================================================
//  MakeSphereR:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeSphereR (CORBA::Double theR)
{
  beginService( " GEOM_Superv_i::MakeSphereR" );
  MESSAGE("GEOM_Superv_i::MakeSphereR");
  get3DPrimOp();
  GEOM::GEOM_Object_ptr anObj = my3DPrimOp->MakeSphereR(theR);
  endService( " GEOM_Superv_i::MakeSphereR" );
  return anObj;
}

//=============================================================================
//  MakeSpherePntR:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeSpherePntR (GEOM::GEOM_Object_ptr thePnt, 
						     CORBA::Double theR)
{
  beginService( " GEOM_Superv_i::MakeSpherePntR" );
  MESSAGE("GEOM_Superv_i::MakeSpherePntR");
  get3DPrimOp();
  GEOM::GEOM_Object_ptr anObj = my3DPrimOp->MakeSpherePntR(thePnt, theR);
  endService( " GEOM_Superv_i::MakeSpherePntR" );
  return anObj;
}

//=============================================================================
//  MakeTorusPntVecRR:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeTorusPntVecRR (GEOM::GEOM_Object_ptr thePnt,
							GEOM::GEOM_Object_ptr theVec,
							CORBA::Double theRMajor,
							CORBA::Double theRMinor)
{
  beginService( " GEOM_Superv_i::MakeTorusPntVecRR" );
  MESSAGE("GEOM_Superv_i::MakeTorusPntVecRR");
  get3DPrimOp();
  GEOM::GEOM_Object_ptr anObj = my3DPrimOp->MakeTorusPntVecRR(thePnt, theVec, theRMajor, theRMinor);
  endService( " GEOM_Superv_i::MakeTorusPntVecRR" );
  return anObj;
}

//=============================================================================
//  MakeTorusRR:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeTorusRR (CORBA::Double theRMajor,
						  CORBA::Double theRMinor)
{
  beginService( " GEOM_Superv_i::MakeTorusRR" );
  MESSAGE("GEOM_Superv_i::MakeTorusRR");
  get3DPrimOp();
  GEOM::GEOM_Object_ptr anObj = my3DPrimOp->MakeTorusRR(theRMajor, theRMinor);
  endService( " GEOM_Superv_i::MakeTorusRR" );
  return anObj;
}

//=============================================================================
//  MakeConePntVecR1R2H:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeConePntVecR1R2H (GEOM::GEOM_Object_ptr thePnt,
							  GEOM::GEOM_Object_ptr theAxis,
							  CORBA::Double theR1,
							  CORBA::Double theR2,
							  CORBA::Double theHeight)
{
  beginService( " GEOM_Superv_i::MakeConePntVecR1R2H" );
  MESSAGE("GEOM_Superv_i::MakeConePntVecR1R2H");
  get3DPrimOp();
  GEOM::GEOM_Object_ptr anObj = my3DPrimOp->MakeConePntVecR1R2H(thePnt, theAxis, theR1, theR2, theHeight);
  endService( " GEOM_Superv_i::MakeConePntVecR1R2H" );
  return anObj;
}

//=============================================================================
//  MakeConeR1R2H:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeConeR1R2H (CORBA::Double theR1, 
						    CORBA::Double theR2, 
						    CORBA::Double theHeight)
{
  beginService( " GEOM_Superv_i::MakeConeR1R2H" );
  MESSAGE("GEOM_Superv_i::MakeConeR1R2H");
  get3DPrimOp();
  GEOM::GEOM_Object_ptr anObj = my3DPrimOp->MakeConeR1R2H(theR1, theR2, theHeight);
  endService( " GEOM_Superv_i::MakeConeR1R2H" );
  return anObj;
}

//=============================================================================
//  MakePrismVecH:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakePrismVecH (GEOM::GEOM_Object_ptr theBase,
						    GEOM::GEOM_Object_ptr theVec,
						    CORBA::Double         theH)
{
  beginService( " GEOM_Superv_i::MakePrismVecH" );
  MESSAGE("GEOM_Superv_i::MakePrismVecH");
  get3DPrimOp();
  GEOM::GEOM_Object_ptr anObj = my3DPrimOp->MakePrismVecH(theBase, theVec, theH);
  endService( " GEOM_Superv_i::MakePrismVecH" );
  return anObj;
}


//=============================================================================
//  MakePrismTwoPnt:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakePrismTwoPnt (GEOM::GEOM_Object_ptr theBase,
						      GEOM::GEOM_Object_ptr thePoint1,
						      GEOM::GEOM_Object_ptr thePoint2)
{
  beginService( " GEOM_Superv_i::MakePrismTwoPnt" );
  MESSAGE("GEOM_Superv_i::MakePrismTwoPnt");
  get3DPrimOp();
  GEOM::GEOM_Object_ptr anObj = my3DPrimOp->MakePrismTwoPnt(theBase, thePoint1, thePoint2);
  endService( " GEOM_Superv_i::MakePrismTwoPnt" );
  return anObj;
}

//=============================================================================
//  MakePipe:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakePipe (GEOM::GEOM_Object_ptr theBase, 
					       GEOM::GEOM_Object_ptr thePath)
{
  beginService( " GEOM_Superv_i::MakePipe" );
  MESSAGE("GEOM_Superv_i::MakePipe");
  get3DPrimOp();
  GEOM::GEOM_Object_ptr anObj = my3DPrimOp->MakePipe(theBase, thePath);
  endService( " GEOM_Superv_i::MakePipe" );
  return anObj;
}

//=============================================================================
//  MakeRevolutionAxisAngle:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeRevolutionAxisAngle (GEOM::GEOM_Object_ptr theBase,
							      GEOM::GEOM_Object_ptr theAxis,
							      CORBA::Double theAngle)
{
  beginService( " GEOM_Superv_i::MakeRevolutionAxisAngle" );
  MESSAGE("GEOM_Superv_i::MakeRevolutionAxisAngle");
  get3DPrimOp();
  GEOM::GEOM_Object_ptr anObj = my3DPrimOp->MakeRevolutionAxisAngle(theBase, theAxis, theAngle);
  endService( " GEOM_Superv_i::MakeRevolutionAxisAngle" );
  return anObj;
}

//=============================================================================
//  MakeFilling:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeFilling (GEOM::GEOM_Object_ptr theShape,
						  CORBA::Long theMinDeg, CORBA::Long theMaxDeg,
						  CORBA::Double theTol2D, CORBA::Double theTol3D,
						  CORBA::Long theNbIter)
{
  beginService( " GEOM_Superv_i::MakeFilling" );
  MESSAGE("GEOM_Superv_i::MakeFilling");
  get3DPrimOp();
  GEOM::GEOM_Object_ptr anObj = my3DPrimOp->MakeFilling(theShape, theMinDeg, theMaxDeg, theTol2D, theTol3D, theNbIter);
  endService( " GEOM_Superv_i::MakeFilling" );
  return anObj;
}

//============================= BooleanOperations =============================
//=============================================================================
//  MakeBoolean:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeBoolean (GEOM::GEOM_Object_ptr theShape1,
						  GEOM::GEOM_Object_ptr theShape2,
						  CORBA::Long theOperation)
{
  beginService( " GEOM_Superv_i::MakeBoolean" );
  // theOperation indicates the operation to be done:
  // 1 - Common, 2 - Cut, 3 - Fuse, 4 - Section
  MESSAGE("GEOM_Superv_i::MakeBoolean");
  getBoolOp();
  GEOM::GEOM_Object_ptr anObj = myBoolOp->MakeBoolean(theShape1, theShape2, theOperation);
  endService( " GEOM_Superv_i::MakeBoolean" );
  return anObj;
}

//=============================================================================
//  MakeThruSections:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeThruSections(const GEOM::ListOfGO& theSeqSections,
					 CORBA::Boolean theModeSolid,
					 CORBA::Double thePreci,
					 CORBA::Boolean theRuled)
{
  beginService( " GEOM_Superv_i::MakeThruSections" );
  MESSAGE("GEOM_Superv_i::MakeThruSections");
  get3DPrimOp();
  GEOM::GEOM_Object_ptr anObj = my3DPrimOp->MakeThruSections(theSeqSections, theModeSolid,thePreci,theRuled);
  endService( " GEOM_Superv_i::MakeThruSections" );
  return anObj;
}

//=============================================================================
//  MakePipe:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakePipeWithDifferentSections(const GEOM::ListOfGO& theBases,
						      const GEOM::ListOfGO& theLocations,
						      GEOM::GEOM_Object_ptr thePath,
						      CORBA::Boolean theWithContact,
						      CORBA::Boolean theWithCorrections)
{
  beginService( " GEOM_Superv_i::MakePipeWithDifferentSections" );
  MESSAGE("GEOM_Superv_i::MakePipeWithDifferentSections");
  get3DPrimOp();
  GEOM::GEOM_Object_ptr anObj = my3DPrimOp->MakePipeWithDifferentSections(theBases,theLocations, thePath,theWithContact,theWithCorrections);
  endService( " GEOM_Superv_i::MakePipeWithDifferentSections" );
  return anObj;
}
//=============================================================================
//  MakeFuse:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeFuse (GEOM::GEOM_Object_ptr theShape1,
					       GEOM::GEOM_Object_ptr theShape2)
{
  beginService( " GEOM_Superv_i::MakeFuse" );
  MESSAGE("GEOM_Superv_i::MakeFuse");
  getBoolOp();
  GEOM::GEOM_Object_ptr anObj = myBoolOp->MakeBoolean(theShape1, theShape2, 3);
  endService( " GEOM_Superv_i::MakeFuse" );
  return anObj;
}

//=============================================================================
//  MakePartition:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakePartition (GEOM::GEOM_List_ptr   theShapes,
						    GEOM::GEOM_List_ptr   theTools,
						    GEOM::GEOM_List_ptr   theKeepInside,
						    GEOM::GEOM_List_ptr   theRemoveInside,
						    CORBA::Short      theLimit,
						    CORBA::Boolean    theRemoveWebs,
						    GEOM::GEOM_List_ptr theMaterials)
{
  beginService( " GEOM_Superv_i::MakePartition" );
  MESSAGE("GEOM_Superv_i::MakePartition");
  GEOM_List_i<GEOM::ListOfGO>* aListImplS = 
    dynamic_cast<GEOM_List_i<GEOM::ListOfGO>*>(GetServant(theShapes, myPOA).in());
  GEOM_List_i<GEOM::ListOfGO>* aListImplT = 
    dynamic_cast<GEOM_List_i<GEOM::ListOfGO>*>(GetServant(theTools, myPOA).in());
  GEOM_List_i<GEOM::ListOfGO>* aListImplKI = 
    dynamic_cast<GEOM_List_i<GEOM::ListOfGO>*>(GetServant(theKeepInside, myPOA).in());
  GEOM_List_i<GEOM::ListOfGO>* aListImplRI = 
    dynamic_cast<GEOM_List_i<GEOM::ListOfGO>*>(GetServant(theRemoveInside, myPOA).in());
  GEOM_List_i<GEOM::ListOfLong>* aListImplM = 
    dynamic_cast<GEOM_List_i<GEOM::ListOfLong>*>(GetServant(theMaterials, myPOA).in());
  if (aListImplS && aListImplT && aListImplKI && aListImplRI && aListImplM) {
    getBoolOp();
    GEOM::GEOM_Object_ptr anObj = myBoolOp->MakePartition(aListImplS->GetList(), aListImplT->GetList(), 
							  aListImplKI->GetList(), aListImplRI->GetList(),
							  theLimit, theRemoveWebs, aListImplM->GetList());
    endService( " GEOM_Superv_i::MakePartition" );
    return anObj;
  }
  endService( " GEOM_Superv_i::MakePartition" );
  return NULL;
}

//=============================================================================
//  MakeHalfPartition:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeHalfPartition (GEOM::GEOM_Object_ptr theShape,
							GEOM::GEOM_Object_ptr thePlane)
{
  beginService( " GEOM_Superv_i::MakeHalfPartition" );
  MESSAGE("GEOM_Superv_i::MakeHalfPartition");
  getBoolOp();
  GEOM::GEOM_Object_ptr anObj = myBoolOp->MakeHalfPartition(theShape, thePlane);
  endService( " GEOM_Superv_i::MakeHalfPartition" );
  return anObj;
}

//============================== InsertOperations =============================
//=============================================================================
//  MakeCopy:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeCopy (GEOM::GEOM_Object_ptr theOriginal)
{
  beginService( " GEOM_Superv_i::MakeCopy" );
  MESSAGE("GEOM_Superv_i::MakeCopy");
  getInsOp();
  GEOM::GEOM_Object_ptr anObj = myInsOp->MakeCopy(theOriginal);
  endService( " GEOM_Superv_i::MakeCopy" );
  return anObj;
}

//=============================================================================
//  Export:
//=============================================================================
void GEOM_Superv_i::Export (GEOM::GEOM_Object_ptr theObject, 
			    const char*           theFileName, 
			    const char*           theFormatName)
{
  beginService( " GEOM_Superv_i::Export" );
  MESSAGE("GEOM_Superv_i::Export");
  getInsOp();
  myInsOp->Export(theObject, theFileName, theFormatName);
  endService( " GEOM_Superv_i::Export" );
}

//=============================================================================
//  Import:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::Import (const char* theFileName, 
					     const char* theFormatName)
{
  beginService( " GEOM_Superv_i::Import" );
  MESSAGE("GEOM_Superv_i::Import");
  getInsOp();
  GEOM::GEOM_Object_ptr anObj = myInsOp->Import(theFileName, theFormatName);
  endService( " GEOM_Superv_i::Import" );
  return anObj;
}

//=============================================================================
//  ImportTranslators:
//=============================================================================
void GEOM_Superv_i::ImportTranslators (GEOM::string_array_out theFormats,
				       GEOM::string_array_out thePatterns)
{
  beginService( " GEOM_Superv_i::ImportTranslators" );
  MESSAGE("GEOM_Superv_i::ImportTranslators");
  getInsOp();
  myInsOp->ImportTranslators(theFormats, thePatterns);
  endService( " GEOM_Superv_i::ImportTranslators" );
}

//=============================================================================
//  ExportTranslators:
//=============================================================================
void GEOM_Superv_i::ExportTranslators (GEOM::string_array_out theFormats,
				       GEOM::string_array_out thePatterns)
{
  beginService( " GEOM_Superv_i::ExportTranslators" );
  MESSAGE("GEOM_Superv_i::ExportTranslators");
  getInsOp();
  myInsOp->ExportTranslators(theFormats, thePatterns);
  endService( " GEOM_Superv_i::ExportTranslators" );
}

//============================= TransformOperations ===========================
//=============================================================================
//  TranslateTwoPoints:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::TranslateTwoPoints (GEOM::GEOM_Object_ptr theObject,
							 GEOM::GEOM_Object_ptr thePoint1,
							 GEOM::GEOM_Object_ptr thePoint2)
{
  beginService( " GEOM_Superv_i::TranslateTwoPoints" );
  MESSAGE("GEOM_Superv_i::TranslateTwoPoints");
  getTransfOp();
  GEOM::GEOM_Object_ptr anObj = myTransfOp->TranslateTwoPoints(theObject, thePoint1, thePoint2);
  endService( " GEOM_Superv_i::TranslateTwoPoints" );
  return anObj;
}

//=============================================================================
//  TranslateTwoPointsCopy:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::TranslateTwoPointsCopy (GEOM::GEOM_Object_ptr theObject,
							     GEOM::GEOM_Object_ptr thePoint1,
							     GEOM::GEOM_Object_ptr thePoint2)
{
  beginService( " GEOM_Superv_i::TranslateTwoPointsCopy" );
  MESSAGE("GEOM_Superv_i::TranslateTwoPointsCopy");
  getTransfOp();
  GEOM::GEOM_Object_ptr anObj = myTransfOp->TranslateTwoPointsCopy(theObject, thePoint1, thePoint2);
  endService( " GEOM_Superv_i::TranslateTwoPointsCopy" );
  return anObj;
}

//=============================================================================
//  TranslateDXDYDZ:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::TranslateDXDYDZ (GEOM::GEOM_Object_ptr theObject,
						      CORBA::Double theDX, 
						      CORBA::Double theDY, 
						      CORBA::Double theDZ)
{
  beginService( " GEOM_Superv_i::TranslateDXDYDZ" );
  MESSAGE("GEOM_Superv_i::TranslateDXDYDZ");
  getTransfOp();
  GEOM::GEOM_Object_ptr anObj = myTransfOp->TranslateDXDYDZ(theObject, theDX, theDY, theDZ);
  endService( " GEOM_Superv_i::TranslateDXDYDZ" );
  return anObj;
}

//=============================================================================
//  TranslateDXDYDZCopy:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::TranslateDXDYDZCopy (GEOM::GEOM_Object_ptr theObject,
							  CORBA::Double theDX, 
							  CORBA::Double theDY, 
							  CORBA::Double theDZ)
{
  beginService( " GEOM_Superv_i::TranslateDXDYDZCopy" );
  MESSAGE("GEOM_Superv_i::TranslateDXDYDZCopy");
  getTransfOp();
  GEOM::GEOM_Object_ptr anObj = myTransfOp->TranslateDXDYDZCopy(theObject, theDX, theDY, theDZ);
  endService( " GEOM_Superv_i::TranslateDXDYDZCopy" );
  return anObj;
}

//=============================================================================
//  TranslateVector:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::TranslateVector (GEOM::GEOM_Object_ptr theObject,
						      GEOM::GEOM_Object_ptr theVector)
{
  beginService( " GEOM_Superv_i::TranslateVector" );
  MESSAGE("GEOM_Superv_i::TranslateVector");
  getTransfOp();
  GEOM::GEOM_Object_ptr anObj = myTransfOp->TranslateVector(theObject, theVector);
  endService( " GEOM_Superv_i::TranslateVector" );
  return anObj;
}

//=============================================================================
//  TranslateVectorCopy:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::TranslateVectorCopy (GEOM::GEOM_Object_ptr theObject,
							  GEOM::GEOM_Object_ptr theVector)
{
  beginService( " GEOM_Superv_i::TranslateVectorCopy" );
  MESSAGE("GEOM_Superv_i::TranslateVectorCopy");
  getTransfOp();
  GEOM::GEOM_Object_ptr anObj = myTransfOp->TranslateVectorCopy(theObject, theVector);
  endService( " GEOM_Superv_i::TranslateVectorCopy" );
  return anObj;
}

//=============================================================================
//  MultiTranslate1D:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MultiTranslate1D (GEOM::GEOM_Object_ptr theObject,
						       GEOM::GEOM_Object_ptr theVector,
						       CORBA::Double theStep,
						       CORBA::Long theNbTimes)
{
  beginService( " GEOM_Superv_i::MultiTranslate1D" );
  MESSAGE("GEOM_Superv_i::MultiTranslate1D");
  getTransfOp();
  GEOM::GEOM_Object_ptr anObj = myTransfOp->MultiTranslate1D(theObject, theVector, theStep, theNbTimes);
  endService( " GEOM_Superv_i::MultiTranslate1D" );
  return anObj;
}

//=============================================================================
//  MultiTranslate2D:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MultiTranslate2D (GEOM::GEOM_Object_ptr theObject,
						       GEOM::GEOM_Object_ptr theVector1,
						       CORBA::Double theStep1,
						       CORBA::Long theNbTimes1,
						       GEOM::GEOM_Object_ptr theVector2,
						       CORBA::Double theStep2,
						       CORBA::Long theNbTimes2)
{
  beginService( " GEOM_Superv_i::MultiTranslate2D" );
  MESSAGE("GEOM_Superv_i::MultiTranslate2D");
  getTransfOp();
  GEOM::GEOM_Object_ptr anObj = myTransfOp->MultiTranslate2D(theObject, theVector1, theStep1, theNbTimes1,
							     theVector2, theStep2, theNbTimes2);
  endService( " GEOM_Superv_i::MultiTranslate2D" );
  return anObj;
}

//=============================================================================
//  Rotate:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::Rotate (GEOM::GEOM_Object_ptr theObject,
					     GEOM::GEOM_Object_ptr theAxis,
					     CORBA::Double theAngle)
{
  beginService( " GEOM_Superv_i::Rotate" );
  MESSAGE("GEOM_Superv_i::Rotate");
  getTransfOp();
  GEOM::GEOM_Object_ptr anObj = myTransfOp->Rotate(theObject, theAxis, theAngle);
  endService( " GEOM_Superv_i::Rotate" );
  return anObj;
}

//=============================================================================
//  RotateCopy:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::RotateCopy (GEOM::GEOM_Object_ptr theObject,
						 GEOM::GEOM_Object_ptr theAxis,
						 CORBA::Double theAngle)
{
  beginService( " GEOM_Superv_i::RotateCopy" );
  MESSAGE("GEOM_Superv_i::RotateCopy");
  getTransfOp();
  GEOM::GEOM_Object_ptr anObj = myTransfOp->RotateCopy(theObject, theAxis, theAngle);
  endService( " GEOM_Superv_i::RotateCopy" );
  return anObj;
}

//=============================================================================
//  MultiRotate1D:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MultiRotate1D (GEOM::GEOM_Object_ptr theObject,
						    GEOM::GEOM_Object_ptr theAxis,
						    CORBA::Long theNbTimes)
{
  beginService( " GEOM_Superv_i::MultiRotate1D" );
  MESSAGE("GEOM_Superv_i::MultiRotate1D");
  getTransfOp();
  GEOM::GEOM_Object_ptr anObj = myTransfOp->MultiRotate1D(theObject, theAxis, theNbTimes);
  endService( " GEOM_Superv_i::MultiRotate1D" );
  return anObj;
}

//=============================================================================
//  MultiRotate2D:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MultiRotate2D (GEOM::GEOM_Object_ptr theObject,
						    GEOM::GEOM_Object_ptr theAxis,
						    CORBA::Double theAngle,
						    CORBA::Long theNbTimes1,
						    CORBA::Double theStep,
						    CORBA::Long theNbTimes2)
{
  beginService( " GEOM_Superv_i::MultiRotate2D" );
  MESSAGE("GEOM_Superv_i::MultiRotate2D");
  getTransfOp();
  GEOM::GEOM_Object_ptr anObj = myTransfOp->MultiRotate2D(theObject, theAxis, theAngle, theNbTimes1, theStep, theNbTimes2);
  endService( " GEOM_Superv_i::MultiRotate2D" );
  return anObj;
}

//=============================================================================
//  MirrorPlane:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MirrorPlane (GEOM::GEOM_Object_ptr theObject, 
						  GEOM::GEOM_Object_ptr thePlane)
{
  beginService( " GEOM_Superv_i::MirrorPlane" );
  MESSAGE("GEOM_Superv_i::MirrorPlane");
  getTransfOp();
  GEOM::GEOM_Object_ptr anObj = myTransfOp->MirrorPlane(theObject, thePlane);
  endService( " GEOM_Superv_i::MirrorPlane" );
  return anObj;
}

//=============================================================================
//  MirrorPlaneCopy:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MirrorPlaneCopy (GEOM::GEOM_Object_ptr theObject, 
						      GEOM::GEOM_Object_ptr thePlane)
{
  beginService( " GEOM_Superv_i::MirrorPlaneCopy" );
  MESSAGE("GEOM_Superv_i::MirrorPlaneCopy");
  getTransfOp();
  GEOM::GEOM_Object_ptr anObj = myTransfOp->MirrorPlaneCopy(theObject, thePlane);
  endService( " GEOM_Superv_i::MirrorPlaneCopy" );
  return anObj;
}

//=============================================================================
//  MirrorAxis:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MirrorAxis (GEOM::GEOM_Object_ptr theObject, 
						 GEOM::GEOM_Object_ptr theAxis)
{
  beginService( " GEOM_Superv_i::MirrorAxis" );
  MESSAGE("GEOM_Superv_i::MirrorAxis");
  getTransfOp();
  GEOM::GEOM_Object_ptr anObj = myTransfOp->MirrorAxis(theObject, theAxis);
  endService( " GEOM_Superv_i::MirrorAxis" );
  return anObj;
}

//=============================================================================
//  MirrorAxisCopy:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MirrorAxisCopy (GEOM::GEOM_Object_ptr theObject, 
						     GEOM::GEOM_Object_ptr theAxis)
{
  beginService( " GEOM_Superv_i::MirrorAxisCopy" );
  MESSAGE("GEOM_Superv_i::MirrorAxisCopy");
  getTransfOp();
  GEOM::GEOM_Object_ptr anObj = myTransfOp->MirrorAxisCopy(theObject, theAxis);
  endService( " GEOM_Superv_i::MirrorAxisCopy" );
  return anObj;
}

//=============================================================================
//  MirrorPoint:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MirrorPoint (GEOM::GEOM_Object_ptr theObject, 
						  GEOM::GEOM_Object_ptr thePoint)
{
  beginService( " GEOM_Superv_i::MirrorPoint" );
  MESSAGE("GEOM_Superv_i::MirrorPoint");
  getTransfOp();
  GEOM::GEOM_Object_ptr anObj = myTransfOp->MirrorPoint(theObject, thePoint);
  endService( " GEOM_Superv_i::MirrorPoint" );
  return anObj;
}

//=============================================================================
//  MirrorPointCopy:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MirrorPointCopy (GEOM::GEOM_Object_ptr theObject, 
						      GEOM::GEOM_Object_ptr thePoint)
{
  beginService( " GEOM_Superv_i::MirrorPoint" );
  MESSAGE("GEOM_Superv_i::MirrorPointCopy");
  getTransfOp();
  GEOM::GEOM_Object_ptr anObj = myTransfOp->MirrorPointCopy(theObject, thePoint);
  endService( " GEOM_Superv_i::MirrorPoint" );
  return anObj;
}

//=============================================================================
//  OffsetShape:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::OffsetShape (GEOM::GEOM_Object_ptr theObject, 
						  CORBA::Double theOffset)
{
  beginService( " GEOM_Superv_i::OffsetShape" );
  MESSAGE("GEOM_Superv_i::OffsetShape");
  getTransfOp();
  GEOM::GEOM_Object_ptr anObj = myTransfOp->OffsetShape(theObject, theOffset);
  endService( " GEOM_Superv_i::OffsetShape" );
  return anObj;
}

//=============================================================================
//  OffsetShapeCopy:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::OffsetShapeCopy (GEOM::GEOM_Object_ptr theObject, 
						      CORBA::Double theOffset)
{
  beginService( " GEOM_Superv_i::OffsetShapeCopy" );
  MESSAGE("GEOM_Superv_i::OffsetShapeCopy");
  getTransfOp();
  GEOM::GEOM_Object_ptr anObj = myTransfOp->OffsetShapeCopy(theObject, theOffset);
  endService( " GEOM_Superv_i::OffsetShapeCopy" );
  return anObj;
}

//=============================================================================
//  ScaleShape:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::ScaleShape (GEOM::GEOM_Object_ptr theObject, 
						 GEOM::GEOM_Object_ptr thePoint,
						 CORBA::Double theFactor)
{
  beginService( " GEOM_Superv_i::ScaleShape" );
  MESSAGE("GEOM_Superv_i::ScaleShape");
  getTransfOp();
  GEOM::GEOM_Object_ptr anObj = myTransfOp->ScaleShape(theObject, thePoint, theFactor);
  endService( " GEOM_Superv_i::ScaleShape" );
  return anObj;
}

//=============================================================================
//  ScaleShapeCopy:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::ScaleShapeCopy (GEOM::GEOM_Object_ptr theObject, 
						     GEOM::GEOM_Object_ptr thePoint,
						     CORBA::Double theFactor)
{
  beginService( " GEOM_Superv_i::ScaleShapeCopy" );
  MESSAGE("GEOM_Superv_i::ScaleShapeCopy");
  getTransfOp();
  GEOM::GEOM_Object_ptr anObj = myTransfOp->ScaleShapeCopy(theObject, thePoint, theFactor);
  endService( " GEOM_Superv_i::ScaleShapeCopy" );
  return anObj;
}

//=============================================================================
//  PositionShape:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::PositionShape (GEOM::GEOM_Object_ptr theObject,
						    GEOM::GEOM_Object_ptr theStartLCS,
						    GEOM::GEOM_Object_ptr theEndLCS)
{
  beginService( " GEOM_Superv_i::PositionShape" );
  MESSAGE("GEOM_Superv_i::PositionShape");
  getTransfOp();
  GEOM::GEOM_Object_ptr anObj = myTransfOp->PositionShape(theObject, theStartLCS, theEndLCS);
  endService( " GEOM_Superv_i::PositionShape" );
  return anObj;
}

//=============================================================================
//  PositionShapeCopy:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::PositionShapeCopy (GEOM::GEOM_Object_ptr theObject,
							GEOM::GEOM_Object_ptr theStartLCS,
							GEOM::GEOM_Object_ptr theEndLCS)
{
  beginService( " GEOM_Superv_i::PositionShapeCopy" );
  MESSAGE("GEOM_Superv_i::PositionShapeCopy");
  getTransfOp();
  GEOM::GEOM_Object_ptr anObj = myTransfOp->PositionShapeCopy(theObject, theStartLCS, theEndLCS);
  endService( " GEOM_Superv_i::PositionShapeCopy" );
  return anObj;
}

//=============================== ShapesOperations ============================
//=============================================================================
//  Make:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeEdge (GEOM::GEOM_Object_ptr thePnt1,
					       GEOM::GEOM_Object_ptr thePnt2)
{
  beginService( " GEOM_Superv_i::MakeEdge" );
  MESSAGE("GEOM_Superv_i::MakeEdge");
  getShapesOp();
  GEOM::GEOM_Object_ptr anObj = myShapesOp->MakeEdge(thePnt1, thePnt2);
  endService( " GEOM_Superv_i::MakeEdge" );
  return anObj;
}

//=============================================================================
//  MakeWire:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeWire (GEOM::GEOM_List_ptr theEdgesAndWires)
{
  beginService( " GEOM_Superv_i::MakeWire" );
  MESSAGE("GEOM_Superv_i::MakeWire");
  if (GEOM_List_i<GEOM::ListOfGO>* aListImplEW = 
      dynamic_cast<GEOM_List_i<GEOM::ListOfGO>*>(GetServant(theEdgesAndWires, myPOA).in())) {
    getShapesOp();
    GEOM::GEOM_Object_ptr anObj = myShapesOp->MakeWire(aListImplEW->GetList());
    endService( " GEOM_Superv_i::MakeWire" );
    return anObj;
  }
  endService( " GEOM_Superv_i::MakeWire" );
  return NULL;
}

//=============================================================================
//  MakeFace:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeFace (GEOM::GEOM_Object_ptr theWire,
					       CORBA::Boolean isPlanarWanted)
{
  beginService( " GEOM_Superv_i::MakeFace" );
  MESSAGE("GEOM_Superv_i::MakeFace");
  getShapesOp();
  GEOM::GEOM_Object_ptr anObj = myShapesOp->MakeFace(theWire, isPlanarWanted);
  endService( " GEOM_Superv_i::MakeFace" );
  return anObj;
}

//=============================================================================
//  MakeFaceWires:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeFaceWires (GEOM::GEOM_List_ptr theWires,
						    CORBA::Boolean isPlanarWanted)
{
  beginService( " GEOM_Superv_i::MakeFaceWires" );
  MESSAGE("GEOM_Superv_i::MakeFaceWires");
  if (GEOM_List_i<GEOM::ListOfGO>* aListImplW = 
      dynamic_cast<GEOM_List_i<GEOM::ListOfGO>*>(GetServant(theWires, myPOA).in())) {
    getShapesOp();
    GEOM::GEOM_Object_ptr anObj = myShapesOp->MakeFaceWires(aListImplW->GetList(), isPlanarWanted);
    endService( " GEOM_Superv_i::MakeFaceWires" );
    return anObj;
  }
  endService( " GEOM_Superv_i::MakeFaceWires" );
  return NULL;
}

//=============================================================================
//  MakeShell:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeShell (GEOM::GEOM_List_ptr theFacesAndShells)
{
  beginService( " GEOM_Superv_i::MakeShell" );
  MESSAGE("GEOM_Superv_i::MakeShell");
  if (GEOM_List_i<GEOM::ListOfGO>* aListImplFS = 
      dynamic_cast<GEOM_List_i<GEOM::ListOfGO>*>(GetServant(theFacesAndShells, myPOA).in())) {
    getShapesOp();
    GEOM::GEOM_Object_ptr anObj = myShapesOp->MakeShell(aListImplFS->GetList());
    endService( " GEOM_Superv_i::MakeShell" );
    return anObj;
  }
  endService( " GEOM_Superv_i::MakeShell" );
  return NULL;
}

//=============================================================================
//  MakeSolidShell:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeSolidShell (GEOM::GEOM_Object_ptr theShell)
{
  beginService( " GEOM_Superv_i::MakeSolidShell" );
  MESSAGE("GEOM_Superv_i::MakeSolidShell");
  getShapesOp();
  GEOM::GEOM_Object_ptr anObj = myShapesOp->MakeSolidShell(theShell);
  endService( " GEOM_Superv_i::MakeSolidShell" );
  return anObj;
}

//=============================================================================
//  MakeSolidShells:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeSolidShells (GEOM::GEOM_List_ptr theShells)
{
  beginService( " GEOM_Superv_i::MakeSolidShells" );
  MESSAGE("GEOM_Superv_i::MakeSolidShells");
  if (GEOM_List_i<GEOM::ListOfGO>* aListImplS = 
      dynamic_cast<GEOM_List_i<GEOM::ListOfGO>*>(GetServant(theShells, myPOA).in())) {
    getShapesOp();
    GEOM::GEOM_Object_ptr anObj = myShapesOp->MakeSolidShells(aListImplS->GetList());
    endService( " GEOM_Superv_i::MakeSolidShells" );
    return anObj;
  }
  endService( " GEOM_Superv_i::MakeSolidShells" );
  return NULL;
}

//=============================================================================
//  MakeCompound:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeCompound (GEOM::GEOM_List_ptr theShapes)
{
  beginService( " GEOM_Superv_i::MakeCompound" );
  MESSAGE("GEOM_Superv_i::MakeCompound");
  if (GEOM_List_i<GEOM::ListOfGO>* aListImpl = 
      dynamic_cast<GEOM_List_i<GEOM::ListOfGO>*>(GetServant(theShapes, myPOA).in())) {
    getShapesOp();
    GEOM::GEOM_Object_ptr anObj = myShapesOp->MakeCompound(aListImpl->GetList());
    endService( " GEOM_Superv_i::MakeCompound" );
    return anObj;
  }
  endService( " GEOM_Superv_i::MakeCompound" );
  return NULL;
}

//=============================================================================
//  MakeGlueFaces:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeGlueFaces (GEOM::GEOM_Object_ptr theShape,
								CORBA::Double   theTolerance)
{
  beginService( " GEOM_Superv_i::MakeGlueFaces" );
  MESSAGE("GEOM_Superv_i::MakeGlueFaces");
  getShapesOp();
  GEOM::GEOM_Object_ptr anObj = myShapesOp->MakeGlueFaces(theShape, theTolerance);
  endService( " GEOM_Superv_i::MakeGlueFaces" );
  return anObj;
}

//=============================================================================
//  MakeExplode:
//=============================================================================
GEOM::GEOM_List_ptr GEOM_Superv_i::MakeExplode (GEOM::GEOM_Object_ptr theShape,
						    CORBA::Long theShapeType,
						    CORBA::Boolean isSorted)
{
  beginService( " GEOM_Superv_i::MakeExplode" );
  MESSAGE("GEOM_Superv_i::MakeExplode");
  getShapesOp();

  GEOM::ListOfGO* aList = myShapesOp->MakeExplode(theShape, theShapeType, isSorted);
  GEOM_List_i<GEOM::ListOfGO>* aListPtr = new GEOM_List_i<GEOM::ListOfGO>(*(aList));
  MESSAGE(" List of "<<aListPtr->GetList().length()<<" element(s)");
  endService( " GEOM_Superv_i::MakeExplode" );
  return aListPtr->_this();
}

//=============================================================================
//  NumberOfFaces:
//=============================================================================
CORBA::Long GEOM_Superv_i::NumberOfFaces (GEOM::GEOM_Object_ptr theShape)
{
  beginService( " GEOM_Superv_i::NumberOfFaces" );
  MESSAGE("GEOM_Superv_i::NumberOfFaces");
  getShapesOp();
  CORBA::Long aRes = myShapesOp->NumberOfFaces(theShape);
  endService( " GEOM_Superv_i::NumberOfFaces" );
  return aRes;
}

//=============================================================================
//  NumberOfEdges:
//=============================================================================
CORBA::Long GEOM_Superv_i::NumberOfEdges (GEOM::GEOM_Object_ptr theShape)
{
  beginService( " GEOM_Superv_i::NumberOfEdges" );
  MESSAGE("GEOM_Superv_i::NumberOfEdges");
  getShapesOp();
  CORBA::Long aRes = myShapesOp->NumberOfEdges(theShape);
  endService( " GEOM_Superv_i::NumberOfEdges" );
  return aRes;
}

//=============================================================================
//  ChangeOrientation:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::ChangeOrientation (GEOM::GEOM_Object_ptr theShape)
{
  beginService( " GEOM_Superv_i::ChangeOrientation" );
  MESSAGE("GEOM_Superv_i::ChangeOrientation");
  getShapesOp();
  GEOM::GEOM_Object_ptr anObj = myShapesOp->ChangeOrientation(theShape);
  endService( " GEOM_Superv_i::ChangeOrientation" );
  return anObj;
}


//=============================== BlocksOperations ============================
//=============================================================================
//  MakeQuad4Vertices:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeQuad4Vertices (GEOM::GEOM_Object_ptr thePnt1,
							GEOM::GEOM_Object_ptr thePnt2,
							GEOM::GEOM_Object_ptr thePnt3,
							GEOM::GEOM_Object_ptr thePnt4)
{
  beginService( " GEOM_Superv_i::MakeQuad4Vertices" );
  MESSAGE("GEOM_Superv_i::MakeQuad4Vertices");
  getBlocksOp();
  GEOM::GEOM_Object_ptr anObj = myBlocksOp->MakeQuad4Vertices(thePnt1, thePnt2, thePnt3, thePnt4);
  endService( " GEOM_Superv_i::MakeQuad4Vertices" );
  return anObj;
}

//=============================================================================
//  MakeQuad:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeQuad (GEOM::GEOM_Object_ptr theEdge1,
					       GEOM::GEOM_Object_ptr theEdge2,
					       GEOM::GEOM_Object_ptr theEdge3,
					       GEOM::GEOM_Object_ptr theEdge4)
{
  beginService( " GEOM_Superv_i::MakeQuad" );
  MESSAGE("GEOM_Superv_i::MakeQuad");
  getBlocksOp();
  GEOM::GEOM_Object_ptr anObj = myBlocksOp->MakeQuad(theEdge1, theEdge2, theEdge3, theEdge4);
  endService( " GEOM_Superv_i::MakeQuad" );
  return anObj;
}

//=============================================================================
//  MakeQuad2Edges:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeQuad2Edges (GEOM::GEOM_Object_ptr theEdge1,
						     GEOM::GEOM_Object_ptr theEdge2)
{
  beginService( " GEOM_Superv_i::MakeQuad2Edges" );
  MESSAGE("GEOM_Superv_i::MakeQuad2Edges");
  getBlocksOp();
  GEOM::GEOM_Object_ptr anObj = myBlocksOp->MakeQuad2Edges(theEdge1, theEdge2);
  endService( " GEOM_Superv_i::MakeQuad2Edges" );
  return anObj;
}

//=============================================================================
//  MakeHexa:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeHexa (GEOM::GEOM_Object_ptr theFace1,
					       GEOM::GEOM_Object_ptr theFace2,
					       GEOM::GEOM_Object_ptr theFace3,
					       GEOM::GEOM_Object_ptr theFace4,
					       GEOM::GEOM_Object_ptr theFace5,
					       GEOM::GEOM_Object_ptr theFace6)
{
  beginService( " GEOM_Superv_i::MakeHexa" );
  MESSAGE("GEOM_Superv_i::MakeHexa");
  getBlocksOp();
  GEOM::GEOM_Object_ptr anObj = myBlocksOp->MakeHexa(theFace1, theFace2, theFace3, theFace4, theFace5, theFace6);
  endService( " GEOM_Superv_i::MakeHexa" );
  return anObj;
}

//=============================================================================
//  MakeHexa2Faces:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeHexa2Faces (GEOM::GEOM_Object_ptr theFace1,
						     GEOM::GEOM_Object_ptr theFace2)
{
  beginService( " GEOM_Superv_i::MakeHexa2Faces" );
  MESSAGE("GEOM_Superv_i::MakeHexa2Faces");
  getBlocksOp();
  GEOM::GEOM_Object_ptr anObj = myBlocksOp->MakeHexa2Faces(theFace1, theFace2);
  endService( " GEOM_Superv_i::MakeHexa2Faces" );
  return anObj;
}

//=============================================================================
//  GetPoint:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::GetPoint (GEOM::GEOM_Object_ptr theShape,
					       CORBA::Double   theX,
					       CORBA::Double   theY,
					       CORBA::Double   theZ,
					       CORBA::Double   theEpsilon)
{
  beginService( " GEOM_Superv_i::GetPoint" );
  MESSAGE("GEOM_Superv_i::GetPoint");
  getBlocksOp();
  GEOM::GEOM_Object_ptr anObj = myBlocksOp->GetPoint(theShape, theX, theY, theZ, theEpsilon);
  endService( " GEOM_Superv_i::GetPoint" );
  return anObj;
}

//=============================================================================
//  GetEdge:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::GetEdge (GEOM::GEOM_Object_ptr theShape,
					      GEOM::GEOM_Object_ptr thePoint1,
					      GEOM::GEOM_Object_ptr thePoint2)
{
  beginService( " GEOM_Superv_i::GetEdge" );
  MESSAGE("GEOM_Superv_i::GetEdge");
  getBlocksOp();
  GEOM::GEOM_Object_ptr anObj = myBlocksOp->GetEdge(theShape, thePoint1, thePoint2);
  endService( " GEOM_Superv_i::GetEdge" );
  return anObj;
}

//=============================================================================
//  GetEdgeNearPoint:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::GetEdgeNearPoint (GEOM::GEOM_Object_ptr theShape,
						       GEOM::GEOM_Object_ptr thePoint)
{
  beginService( " GEOM_Superv_i::GetEdgeNearPoint" );
  MESSAGE("GEOM_Superv_i::GetEdgeNearPoint");
  getBlocksOp();
  GEOM::GEOM_Object_ptr anObj = myBlocksOp->GetEdgeNearPoint(theShape, thePoint);
  endService( " GEOM_Superv_i::GetEdgeNearPoint" );
  return anObj;
}

//=============================================================================
//  GetFaceByPoints:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::GetFaceByPoints (GEOM::GEOM_Object_ptr theShape,
						      GEOM::GEOM_Object_ptr thePoint1,
						      GEOM::GEOM_Object_ptr thePoint2,
						      GEOM::GEOM_Object_ptr thePoint3,
						      GEOM::GEOM_Object_ptr thePoint4)
{
  beginService( " GEOM_Superv_i::GetFaceByPoints" );
  MESSAGE("GEOM_Superv_i::GetFaceByPoints");
  getBlocksOp();
  GEOM::GEOM_Object_ptr anObj = myBlocksOp->GetFaceByPoints(theShape, thePoint1, thePoint2, thePoint3, thePoint4);
  endService( " GEOM_Superv_i::GetFaceByPoints" );
  return anObj;
}

//=============================================================================
//  GetFaceByEdges:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::GetFaceByEdges (GEOM::GEOM_Object_ptr theShape,
						     GEOM::GEOM_Object_ptr theEdge1,
						     GEOM::GEOM_Object_ptr theEdge2)
{
  beginService( " GEOM_Superv_i::GetFaceByEdges" );
  MESSAGE("GEOM_Superv_i::GetFaceByEdges");
  getBlocksOp();
  GEOM::GEOM_Object_ptr anObj = myBlocksOp->GetFaceByEdges(theShape, theEdge1, theEdge2);
  endService( " GEOM_Superv_i::GetFaceByEdges" );
  return anObj;
}

//=============================================================================
//  GetOppositeFace:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::GetOppositeFace (GEOM::GEOM_Object_ptr theBlock,
						      GEOM::GEOM_Object_ptr theFace)
{
  beginService( " GEOM_Superv_i::GetOppositeFace" );
  MESSAGE("GEOM_Superv_i::GetOppositeFace");
  getBlocksOp();
  GEOM::GEOM_Object_ptr anObj = myBlocksOp->GetOppositeFace(theBlock, theFace);
  endService( " GEOM_Superv_i::GetOppositeFace" );
  return anObj;
}

//=============================================================================
//  GetFaceNearPoint:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::GetFaceNearPoint (GEOM::GEOM_Object_ptr theShape,
						       GEOM::GEOM_Object_ptr thePoint)
{
  beginService( " GEOM_Superv_i::GetFaceNearPoint" );
  MESSAGE("GEOM_Superv_i::GetFaceNearPoint");
  getBlocksOp();
  GEOM::GEOM_Object_ptr anObj = myBlocksOp->GetFaceNearPoint(theShape, thePoint);
  endService( " GEOM_Superv_i::GetFaceNearPoint" );
  return anObj;
}

//=============================================================================
//  GetFaceByNormale:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::GetFaceByNormale (GEOM::GEOM_Object_ptr theBlock,
						       GEOM::GEOM_Object_ptr theVector)
{
  beginService( " GEOM_Superv_i::GetFaceByNormale" );
  MESSAGE("GEOM_Superv_i::GetFaceByNormale");
  getBlocksOp();
  GEOM::GEOM_Object_ptr anObj = myBlocksOp->GetFaceByNormale(theBlock, theVector);
  endService( " GEOM_Superv_i::GetFaceByNormale" );
  return anObj;
}

//=============================================================================
//  IsCompoundOfBlocks:
//=============================================================================
CORBA::Boolean GEOM_Superv_i::IsCompoundOfBlocks (GEOM::GEOM_Object_ptr theCompound,
						  CORBA::Long     theMinNbFaces,
						  CORBA::Long     theMaxNbFaces,
						  CORBA::Long&          theNbBlocks)
{
  beginService( " GEOM_Superv_i::IsCompoundOfBlocks" );
  MESSAGE("GEOM_Superv_i::IsCompoundOfBlocks");
  getBlocksOp();
  CORBA::Boolean aRes = myBlocksOp->IsCompoundOfBlocks(theCompound, theMinNbFaces, theMaxNbFaces, theNbBlocks);
  endService( " GEOM_Superv_i::IsCompoundOfBlocks" );
  return aRes;
}

//=============================================================================
//  CheckCompoundOfBlocks:
//=============================================================================
CORBA::Boolean GEOM_Superv_i::CheckCompoundOfBlocks 
(GEOM::GEOM_Object_ptr theCompound,
 GEOM::GEOM_IBlocksOperations::BCErrors_out theErrors)
{
  beginService( " GEOM_Superv_i::CheckCompoundOfBlocks" );
  MESSAGE("GEOM_Superv_i::CheckCompoundOfBlocks");
  getBlocksOp();
  CORBA::Boolean aRes = myBlocksOp->CheckCompoundOfBlocks(theCompound, theErrors);
  endService( " GEOM_Superv_i::CheckCompoundOfBlocks" );
  return aRes;
}

//=============================================================================
//  PrintBCErrors:
//=============================================================================
char* GEOM_Superv_i::PrintBCErrors (GEOM::GEOM_Object_ptr theCompound,
				    const GEOM::GEOM_IBlocksOperations::BCErrors& theErrors)
{
  beginService( " GEOM_Superv_i::PrintBCErrors" );
  MESSAGE("GEOM_Superv_i::PrintBCErrors");
  getBlocksOp();
  char* anErrors = myBlocksOp->PrintBCErrors(theCompound, theErrors);
  endService( " GEOM_Superv_i::PrintBCErrors" );
  return anErrors;
}

//=============================================================================
//  ExplodeCompoundOfBlocks:
//=============================================================================
GEOM::GEOM_List_ptr GEOM_Superv_i::ExplodeCompoundOfBlocks (GEOM::GEOM_Object_ptr theCompound,
								CORBA::Long     theMinNbFaces,
								CORBA::Long     theMaxNbFaces)
{
  beginService( " GEOM_Superv_i::ExplodeCompoundOfBlocks" );
  MESSAGE("GEOM_Superv_i::ExplodeCompoundOfBlocks");
  getBlocksOp();
  GEOM::ListOfGO* aBlocks = myBlocksOp->ExplodeCompoundOfBlocks(theCompound, theMinNbFaces, theMaxNbFaces);
  GEOM_List_i<GEOM::ListOfGO>* aListPtr = new GEOM_List_i<GEOM::ListOfGO>(*(aBlocks));
  endService( " GEOM_Superv_i::ExplodeCompoundOfBlocks" );
  return aListPtr->_this();
}

//=============================================================================
//  GetBlockNearPoint:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::GetBlockNearPoint (GEOM::GEOM_Object_ptr theCompound,
							GEOM::GEOM_Object_ptr thePoint)
{
  beginService( " GEOM_Superv_i::GetBlockNearPoint" );
  MESSAGE("GEOM_Superv_i::GetBlockNearPoint");
  getBlocksOp();
  GEOM::GEOM_Object_ptr anObj = myBlocksOp->GetBlockNearPoint(theCompound, thePoint);
  endService( " GEOM_Superv_i::GetBlockNearPoint" );
  return anObj;
}

//=============================================================================
//  GetBlockByParts:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::GetBlockByParts (GEOM::GEOM_Object_ptr theCompound,
						      GEOM::GEOM_List_ptr theParts)
{
  beginService( " GEOM_Superv_i::GetBlockByParts" );
  MESSAGE("GEOM_Superv_i::GetBlockByParts");
  if (GEOM_List_i<GEOM::ListOfGO>* aListImplP = 
      dynamic_cast<GEOM_List_i<GEOM::ListOfGO>*>(GetServant(theParts, myPOA).in())) {
    getBlocksOp();
    GEOM::GEOM_Object_ptr anObj = myBlocksOp->GetBlockByParts(theCompound, aListImplP->GetList());
    endService( " GEOM_Superv_i::GetBlockByParts" );
    return anObj;
  }
  endService( " GEOM_Superv_i::GetBlockByParts" );
  return NULL;
}

//=============================================================================
//  GetBlocksByParts:
//=============================================================================
GEOM::GEOM_List_ptr GEOM_Superv_i::GetBlocksByParts (GEOM::GEOM_Object_ptr theCompound,
							 GEOM::GEOM_List_ptr theParts)
{
  beginService( " GEOM_Superv_i::GetBlocksByParts" );
  MESSAGE("GEOM_Superv_i::GetBlocksByParts");
  if (GEOM_List_i<GEOM::ListOfGO>* aListImplP = 
      dynamic_cast<GEOM_List_i<GEOM::ListOfGO>*>(GetServant(theParts, myPOA).in())) {
    getBlocksOp();
    
    GEOM::ListOfGO* aBlocks = myBlocksOp->GetBlocksByParts(theCompound, aListImplP->GetList());
    GEOM_List_i<GEOM::ListOfGO>* aListPtr = new GEOM_List_i<GEOM::ListOfGO>(*(aBlocks));
    endService( " GEOM_Superv_i::GetBlocksByParts" );
    return aListPtr->_this();
  }
  endService( " GEOM_Superv_i::GetBlocksByParts" );
  return NULL;
}

//=============================================================================
//  MakeMultiTransformation1D:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeMultiTransformation1D (GEOM::GEOM_Object_ptr theBlock,
								CORBA::Long     theDirFace1,
								CORBA::Long     theDirFace2,
								CORBA::Long     theNbTimes)
{
  beginService( " GEOM_Superv_i::MakeMultiTransformation1D" );
  MESSAGE("GEOM_Superv_i::MakeMultiTransformation1D");
  getBlocksOp();
  GEOM::GEOM_Object_ptr anObj = myBlocksOp->MakeMultiTransformation1D(theBlock, theDirFace1, theDirFace2, theNbTimes);
  endService( " GEOM_Superv_i::MakeMultiTransformation1D" );
  return anObj;
}

//=============================================================================
//  MakeMultiTransformation2D:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeMultiTransformation2D 
(GEOM::GEOM_Object_ptr theBlock,
 CORBA::Long     theDirFace1U,
 CORBA::Long     theDirFace2U,
 CORBA::Long     theNbTimesU,
 CORBA::Long     theDirFace1V,
 CORBA::Long     theDirFace2V,
 CORBA::Long     theNbTimesV)
{
  beginService( " GEOM_Superv_i::MakeMultiTransformation2D" );
  MESSAGE("GEOM_Superv_i::MakeMultiTransformation2D");
  getBlocksOp();
  GEOM::GEOM_Object_ptr anObj = myBlocksOp->MakeMultiTransformation2D(theBlock, 
								      theDirFace1U, theDirFace2U, theNbTimesU,
								      theDirFace1V, theDirFace2V, theNbTimesV);
  endService( " GEOM_Superv_i::MakeMultiTransformation2D" );
  return anObj;
}

//=============================== CurvesOperations ============================
//=============================================================================
//  MakeCirclePntVecR:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeCirclePntVecR (GEOM::GEOM_Object_ptr theCenter,
							GEOM::GEOM_Object_ptr theVector,
							CORBA::Double theR)
{
  beginService( " GEOM_Superv_i::MakeCirclePntVecR" );
  MESSAGE("GEOM_Superv_i::MakeCirclePntVecR");
  getCurvesOp();
  GEOM::GEOM_Object_ptr anObj = myCurvesOp->MakeCirclePntVecR(theCenter, theVector, theR);
  endService( " GEOM_Superv_i::MakeCirclePntVecR" );
  return anObj;
}

//=============================================================================
//  MakeCircleThreePnt:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeCircleThreePnt (GEOM::GEOM_Object_ptr thePnt1,
							 GEOM::GEOM_Object_ptr thePnt2,
							 GEOM::GEOM_Object_ptr thePnt3)
{
  beginService( " GEOM_Superv_i::MakeCircleThreePnt" );
  MESSAGE("GEOM_Superv_i::MakeCircleThreePnt");
  getCurvesOp();
  GEOM::GEOM_Object_ptr anObj = myCurvesOp->MakeCircleThreePnt(thePnt1, thePnt2, thePnt3);
  endService( " GEOM_Superv_i::MakeCircleThreePnt" );
  return anObj;
}

//=============================================================================
//  MakeEllipse:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeEllipse (GEOM::GEOM_Object_ptr theCenter,
						  GEOM::GEOM_Object_ptr theVector,
						  CORBA::Double theRMajor, 
						  CORBA::Double theRMinor)
{
  beginService( " GEOM_Superv_i::MakeEllipse" );
  MESSAGE("GEOM_Superv_i::MakeEllipse");
  getCurvesOp();
  GEOM::GEOM_Object_ptr anObj = myCurvesOp->MakeEllipse(theCenter, theVector, theRMajor, theRMinor);
  endService( " GEOM_Superv_i::MakeEllipse" );
  return anObj;
}

//=============================================================================
//  MakeArc:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeArc (GEOM::GEOM_Object_ptr thePnt1,
					      GEOM::GEOM_Object_ptr thePnt2,
					      GEOM::GEOM_Object_ptr thePnt3)
{
  beginService( " GEOM_Superv_i::MakeArc" );
  MESSAGE("GEOM_Superv_i::MakeArc");
  getCurvesOp();
  GEOM::GEOM_Object_ptr anObj = myCurvesOp->MakeArc(thePnt1, thePnt2, thePnt3);
  endService( " GEOM_Superv_i::MakeArc" );
  return anObj;
}

//=============================================================================
//  MakePolyline:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakePolyline (GEOM::GEOM_List_ptr thePoints)
{
  beginService( " GEOM_Superv_i::MakePolyline" );
  MESSAGE("GEOM_Superv_i::MakePolyline");
  if (GEOM_List_i<GEOM::ListOfGO>* aListImplP = 
      dynamic_cast<GEOM_List_i<GEOM::ListOfGO>*>(GetServant(thePoints, myPOA).in())) {
    getCurvesOp();
    GEOM::GEOM_Object_ptr anObj = myCurvesOp->MakePolyline(aListImplP->GetList());
    endService( " GEOM_Superv_i::MakePolyline" );
    return anObj;
  }
  endService( " GEOM_Superv_i::MakePolyline" );
  return NULL;
}

//=============================================================================
//  MakeSplineBezier:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeSplineBezier (GEOM::GEOM_List_ptr thePoints)
{
  beginService( " GEOM_Superv_i::MakeSplineBezier" );
  MESSAGE("GEOM_Superv_i::MakeSplineBezier");
  if (GEOM_List_i<GEOM::ListOfGO>* aListImplP = 
      dynamic_cast<GEOM_List_i<GEOM::ListOfGO>*>(GetServant(thePoints, myPOA).in())) {
    getCurvesOp();
    GEOM::GEOM_Object_ptr anObj = myCurvesOp->MakeSplineBezier(aListImplP->GetList());
    endService( " GEOM_Superv_i::MakeSplineBezier" );
    return anObj;
  }
  endService( " GEOM_Superv_i::MakeSplineBezier" );
  return NULL;
}

//=============================================================================
//  MakeSplineInterpolation:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeSplineInterpolation (GEOM::GEOM_List_ptr thePoints)
{
  beginService( " GEOM_Superv_i::MakeSplineInterpolation" );
  MESSAGE("GEOM_Superv_i::MakeSplineInterpolation");
  if (GEOM_List_i<GEOM::ListOfGO>* aListImplP = 
      dynamic_cast<GEOM_List_i<GEOM::ListOfGO>*>(GetServant(thePoints, myPOA).in())) {
    getCurvesOp();
    GEOM::GEOM_Object_ptr anObj = myCurvesOp->MakeSplineInterpolation(aListImplP->GetList());
    endService( " GEOM_Superv_i::MakeSplineInterpolation" );
    return anObj;
  }
  endService( " GEOM_Superv_i::MakeSplineInterpolation" );
  return NULL;
}

//=============================================================================
//  MakeSketcher:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeSketcher (const char* theCommand, 
						   GEOM::GEOM_List_ptr theWorkingPlane)
{
  beginService( " GEOM_Superv_i::MakeSketcher" );
  MESSAGE("GEOM_Superv_i::MakeSketcher");
  if (GEOM_List_i<GEOM::ListOfDouble>* aListImplWP = 
      dynamic_cast<GEOM_List_i<GEOM::ListOfDouble>*>(GetServant(theWorkingPlane, myPOA).in())) {
    getCurvesOp();
    GEOM::GEOM_Object_ptr anObj = myCurvesOp->MakeSketcher(theCommand, aListImplWP->GetList());
    endService( " GEOM_Superv_i::MakeSketcher" );
    return anObj;
  }
  endService( " GEOM_Superv_i::MakeSketcher" );
  return NULL;
}

//=============================== LocalOperations =============================
//=============================================================================
//  MakeFilletAll:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeFilletAll (GEOM::GEOM_Object_ptr theShape,
						    CORBA::Double theR)
{
  beginService( " GEOM_Superv_i::MakeFilletAll" );
  MESSAGE("GEOM_Superv_i::MakeFilletAllMakeSketcher");
  getLocalOp();
  GEOM::GEOM_Object_ptr anObj = myLocalOp->MakeFilletAll(theShape, theR);
  endService( " GEOM_Superv_i::MakeFilletAll" );
  return anObj;
}

//=============================================================================
//  MakeFilletEdges:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeFilletEdges (GEOM::GEOM_Object_ptr theShape, 
						      CORBA::Double theR,
						      GEOM::GEOM_List_ptr theEdges)
{
  beginService( " GEOM_Superv_i::MakeFilletEdges" );
  MESSAGE("GEOM_Superv_i::MakeFilletEdges");
  if (GEOM_List_i<GEOM::ListOfLong>* aListImplE = 
      dynamic_cast<GEOM_List_i<GEOM::ListOfLong>*>(GetServant(theEdges, myPOA).in())) {
    getLocalOp();
    GEOM::GEOM_Object_ptr anObj = myLocalOp->MakeFilletEdges(theShape, theR, aListImplE->GetList());
    endService( " GEOM_Superv_i::MakeFilletEdges" );
    return anObj;
  }
  endService( " GEOM_Superv_i::MakeFilletEdges" );
  return NULL;
}

//=============================================================================
//  MakeFilletFaces:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeFilletFaces (GEOM::GEOM_Object_ptr theShape, 
						      CORBA::Double theR,
						      GEOM::GEOM_List_ptr theFaces)
{
  beginService( " GEOM_Superv_i::MakeFilletFaces" );
  MESSAGE("GEOM_Superv_i::MakeFilletFaces");
  if (GEOM_List_i<GEOM::ListOfLong>* aListImplF = 
      dynamic_cast<GEOM_List_i<GEOM::ListOfLong>*>(GetServant(theFaces, myPOA).in())) {
    getLocalOp();
    GEOM::GEOM_Object_ptr anObj = myLocalOp->MakeFilletFaces(theShape, theR, aListImplF->GetList());
    endService( " GEOM_Superv_i::MakeFilletFaces" );
    return anObj;
  }
  endService( " GEOM_Superv_i::MakeFilletFaces" );
  return NULL;
}

//=============================================================================
//  MakeChamferAll:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeChamferAll (GEOM::GEOM_Object_ptr theShape, CORBA::Double theD)
{
  beginService( " GEOM_Superv_i::MakeChamferAll" );
  MESSAGE("GEOM_Superv_i::MakeChamferAll");
  getLocalOp();
  GEOM::GEOM_Object_ptr anObj = myLocalOp->MakeChamferAll(theShape, theD);
  endService( " GEOM_Superv_i::MakeChamferAll" );
  return anObj;
}
  
//=============================================================================
//  MakeChamferEdge:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeChamferEdge (GEOM::GEOM_Object_ptr theShape,
						      CORBA::Double theD1, CORBA::Double theD2,
						      CORBA::Long theFace1, CORBA::Long theFace2)
{
  beginService( " GEOM_Superv_i::MakeChamferEdge" );
  MESSAGE("GEOM_Superv_i::MakeChamferEdge");
  getLocalOp();
  GEOM::GEOM_Object_ptr anObj = myLocalOp->MakeChamferEdge(theShape, theD1, theD2, theFace1, theFace2);
  endService( " GEOM_Superv_i::MakeChamferEdge" );
  return anObj;
}

//=============================================================================
//  MakeChamferFaces:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeChamferFaces (GEOM::GEOM_Object_ptr theShape,
						       CORBA::Double theD1, CORBA::Double theD2,
						       GEOM::GEOM_List_ptr theFaces)
{
  beginService( " GEOM_Superv_i::MakeChamferFaces" );
  MESSAGE("GEOM_Superv_i::MakeChamferFaces");
  if (GEOM_List_i<GEOM::ListOfLong>* aListImplF = 
      dynamic_cast<GEOM_List_i<GEOM::ListOfLong>*>(GetServant(theFaces, myPOA).in())) {
    getLocalOp();
    GEOM::GEOM_Object_ptr anObj = myLocalOp->MakeChamferFaces(theShape, theD1, theD2, aListImplF->GetList());
    endService( " GEOM_Superv_i::MakeChamferFaces" );
    return anObj;
  }
  endService( " GEOM_Superv_i::MakeChamferFaces" );
  return NULL;
}

//=============================================================================
//  MakeArchimede:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeArchimede (GEOM::GEOM_Object_ptr theShape,
						    CORBA::Double theWeight,
						    CORBA::Double theWaterDensity,
						    CORBA::Double theMeshingDeflection)
{
  beginService( " GEOM_Superv_i::MakeArchimede" );
  MESSAGE("GEOM_Superv_i::MakeArchimede");
  getLocalOp();
  GEOM::GEOM_Object_ptr anObj = myLocalOp->MakeArchimede(theShape, theWeight, theWaterDensity, theMeshingDeflection);
  endService( " GEOM_Superv_i::MakeArchimede" );
  return anObj;
}

//=============================================================================
//  GetSubShapeIndexMakeFilletAll:
//=============================================================================
CORBA::Long GEOM_Superv_i::GetSubShapeIndex (GEOM::GEOM_Object_ptr theShape,
					     GEOM::GEOM_Object_ptr theSubShape)
{
  beginService( " GEOM_Superv_i::GetSubShapeIndex" );
  MESSAGE("GEOM_Superv_i::GetSubShapeIndexMakeArchimede");
  getLocalOp();
  CORBA::Long aRes = myLocalOp->GetSubShapeIndex(theShape, theSubShape);
  endService( " GEOM_Superv_i::GetSubShapeIndex" );
  return aRes;
}

//=============================== GroupOperations =============================
//=============================================================================
//  CreateGroup:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::CreateGroup (GEOM::GEOM_Object_ptr theMainShape, 
						  CORBA::Long theShapeType)
{
  beginService( " GEOM_Superv_i::CreateGroup" );
  MESSAGE("GEOM_Superv_i::CreateGroup");
  getGroupOp();
  GEOM::GEOM_Object_ptr anObj = myGroupOp->CreateGroup(theMainShape, theShapeType);
  endService( " GEOM_Superv_i::CreateGroup" );
  return anObj;
}

//=============================================================================
//  AddObject:
//=============================================================================
void GEOM_Superv_i::AddObject (GEOM::GEOM_Object_ptr theGroup, 
			       CORBA::Long theSubShapeId)
{
  beginService( " GEOM_Superv_i::AddObject" );
  MESSAGE("GEOM_Superv_i::AddObject");
  getGroupOp();
  myGroupOp->AddObject(theGroup, theSubShapeId);
  endService( " GEOM_Superv_i::AddObject" );
}

//=============================================================================
//  RemoveObject:
//=============================================================================
void GEOM_Superv_i::RemoveObject (GEOM::GEOM_Object_ptr theGroup, 
				  CORBA::Long theSubShapeId)
{
  beginService( " GEOM_Superv_i::RemoveObject" );
  MESSAGE("GEOM_Superv_i::RemoveObject");
  getGroupOp();
  myGroupOp->RemoveObject(theGroup, theSubShapeId);
  endService( " GEOM_Superv_i::RemoveObject" );
}

//=============================================================================
//  GetType:
//=============================================================================
CORBA::Long GEOM_Superv_i::GetType (GEOM::GEOM_Object_ptr theGroup)
{
  beginService( " GEOM_Superv_i::GetType" );
  MESSAGE("GEOM_Superv_i::GetType");
  getGroupOp();
  CORBA::Long aResult = myGroupOp->GetType(theGroup);
  endService( " GEOM_Superv_i::GetType" );
  return aResult;
}

//=============================================================================
//  GetMainShape:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::GetMainShape (GEOM::GEOM_Object_ptr theGroup)
{
  beginService( " GEOM_Superv_i::GetMainShape" );
  MESSAGE("GEOM_Superv_i::GetMainShape");
  getGroupOp();
  GEOM::GEOM_Object_ptr anObj = myGroupOp->GetMainShape(theGroup);
  endService( " GEOM_Superv_i::GetMainShape" );
  return anObj;
}

//=============================================================================
//  GetObjects:
//=============================================================================
GEOM::GEOM_List_ptr GEOM_Superv_i::GetObjects (GEOM::GEOM_Object_ptr theGroup)
{
  beginService( " GEOM_Superv_i::GetObjects" );
  MESSAGE("GEOM_Superv_i::GetObjects");
  getGroupOp();

  GEOM::ListOfLong* aList = myGroupOp->GetObjects(theGroup);
  GEOM_List_i<GEOM::ListOfLong>* aListPtr = new GEOM_List_i<GEOM::ListOfLong>(*(aList));
  MESSAGE(" List of "<<aListPtr->GetList().length()<<" element(s)");
  endService( " GEOM_Superv_i::GetObjects" );
  return aListPtr->_this();
}

//=====================================================================================
// EXPORTED METHODS
//=====================================================================================
extern "C"
{
  PortableServer::ObjectId * GEOM_SupervEngine_factory(CORBA::ORB_ptr orb,
						      PortableServer::POA_ptr poa, 
						      PortableServer::ObjectId * contId,
						      const char *instanceName, 
						      const char * interfaceName)
  {
    GEOM_Superv_i * myGEOM_Superv_i = new GEOM_Superv_i(orb, poa, contId, instanceName, interfaceName);
    myGEOM_Superv_i->register_name("/myGEOM_Superv");
    return myGEOM_Superv_i->getId() ;
  }
}
