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
// See http://www.salome-platform.org/
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
  MESSAGE("GEOM_Superv_i::MakePointXYZ");
  getBasicOp();
  // make vertex and return
  return myBasicOp->MakePointXYZ(theX, theY, theZ);
}

//=============================================================================
//  MakePointWithReference:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakePointWithReference (GEOM::GEOM_Object_ptr theReference,
							     CORBA::Double theX, 
							     CORBA::Double theY, 
							     CORBA::Double theZ)
{
  MESSAGE("GEOM_Superv_i::MakePointWithReference");
  getBasicOp();
  return myBasicOp->MakePointWithReference(theReference, theX, theY, theZ);
}

//=============================================================================
//  MakePointOnCurve:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakePointOnCurve (GEOM::GEOM_Object_ptr theRefCurve,
						       CORBA::Double theParameter)
{
  MESSAGE("GEOM_Superv_i::MakePointOnCurve");
  getBasicOp();
  return myBasicOp->MakePointOnCurve(theRefCurve, theParameter);
}

//=============================================================================
//  MakeVectorDXDYDZ:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeVectorDXDYDZ (CORBA::Double theDX,
						       CORBA::Double theDY,
						       CORBA::Double theDZ)
{
  MESSAGE("GEOM_Superv_i::MakeVectorDXDYDZ");
  getBasicOp();
  return myBasicOp->MakeVectorDXDYDZ(theDX, theDY, theDZ);
}

//=============================================================================
//  MakeVectorTwoPnt:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeVectorTwoPnt (GEOM::GEOM_Object_ptr thePnt1,
						       GEOM::GEOM_Object_ptr thePnt2)
{
  MESSAGE("GEOM_Superv_i::MakeVector");
  getBasicOp();
  return myBasicOp->MakeVectorTwoPnt(thePnt1, thePnt2);
}

//=============================================================================
//  MakeLineTwoPnt:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeLineTwoPnt (GEOM::GEOM_Object_ptr thePnt1,
						     GEOM::GEOM_Object_ptr thePnt2)
{
  MESSAGE("GEOM_Superv_i::MakeLineTwoPnt");
  getBasicOp();
  return myBasicOp->MakeLineTwoPnt(thePnt1, thePnt2);
}

//=============================================================================
//  MakePlaneThreePnt:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakePlaneThreePnt (GEOM::GEOM_Object_ptr thePnt1,
							GEOM::GEOM_Object_ptr thePnt2,
							GEOM::GEOM_Object_ptr thePnt3,
							CORBA::Double theTrimSize)
{
  MESSAGE("GEOM_Superv_i::MakePlaneThreePnt");
  getBasicOp();
  return myBasicOp->MakePlaneThreePnt(thePnt1, thePnt2, thePnt3, theTrimSize);
}

//=============================================================================
//  MakePlanePntVec:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakePlanePntVec (GEOM::GEOM_Object_ptr thePnt,
						      GEOM::GEOM_Object_ptr theVec,
						      CORBA::Double theTrimSize)
{
  MESSAGE("GEOM_Superv_i::MakePlanePntVec");
  getBasicOp();
  return myBasicOp->MakePlanePntVec(thePnt, theVec, theTrimSize);
}

//=============================================================================
//  MakePlaneFace:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakePlaneFace (GEOM::GEOM_Object_ptr theFace,
						    CORBA::Double theTrimSize)
{
  MESSAGE("GEOM_Superv_i::MakePlaneFace");
  getBasicOp();
  return myBasicOp->MakePlaneFace(theFace, theTrimSize);
}

//=============================================================================
//  MakeMarker:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeMarker 
(CORBA::Double theOX , CORBA::Double theOY , CORBA::Double theOZ,
 CORBA::Double theXDX, CORBA::Double theXDY, CORBA::Double theXDZ,
 CORBA::Double theYDX, CORBA::Double theYDY, CORBA::Double theYDZ)
{
  MESSAGE("GEOM_Superv_i::MakeMarker");
  getBasicOp();
  return myBasicOp->MakeMarker(theOX, theOY, theOZ, theXDX, theXDY, theXDZ, theYDX, theYDY, theYDZ);
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
  MESSAGE("GEOM_Superv_i::MakeBox");
  getBasicOp();
  get3DPrimOp();
  return my3DPrimOp->MakeBoxTwoPnt(myBasicOp->MakePointXYZ(theX1, theY1, theZ1),
				   myBasicOp->MakePointXYZ(theX2, theY2, theZ2));
}

//=============================================================================
//  MakeBoxDXDYDZ:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeBoxDXDYDZ (CORBA::Double theDX, 
						    CORBA::Double theDY, 
						    CORBA::Double theDZ)
{
  MESSAGE("GEOM_Superv_i::MakeBoxDXDYDZ");
  get3DPrimOp();
  return my3DPrimOp->MakeBoxDXDYDZ(theDX, theDY, theDZ);
}

//=============================================================================
//  MakeBoxTwoPnt:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeBoxTwoPnt (GEOM::GEOM_Object_ptr thePnt1, 
						    GEOM::GEOM_Object_ptr thePnt2)
{
  MESSAGE("GEOM_Superv_i::MakeBoxTwoPnt");
  get3DPrimOp();
  return my3DPrimOp->MakeBoxTwoPnt(thePnt1, thePnt2);
}

//=============================================================================
//  MakeCylinderPntVecRH:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeCylinderPntVecRH (GEOM::GEOM_Object_ptr thePnt,
							   GEOM::GEOM_Object_ptr theAxis,
							   CORBA::Double theRadius,
							   CORBA::Double theHeight)
{
  MESSAGE("GEOM_Superv_i::MakeCylinderPntVecRH");
  get3DPrimOp();
  return my3DPrimOp->MakeCylinderPntVecRH(thePnt, theAxis, theRadius, theHeight); 
}

//=============================================================================
//  MakeCylinderRH:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeCylinderRH (CORBA::Double theR, 
						     CORBA::Double theH)
{
  MESSAGE("GEOM_Superv_i::MakeCylinderRH");
  get3DPrimOp();
  return my3DPrimOp->MakeCylinderRH(theR, theH); 
}

//=============================================================================
//  MakeSphere:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeSphere  (CORBA::Double theX,
						  CORBA::Double theY,
						  CORBA::Double theZ,
						  CORBA::Double theRadius)
{
  MESSAGE("GEOM_Superv_i::MakeSphepe");
  getBasicOp();
  get3DPrimOp();
  return my3DPrimOp->MakeSpherePntR(myBasicOp->MakePointXYZ(theX, theY, theZ), theRadius);
}

//=============================================================================
//  MakeSphereR:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeSphereR (CORBA::Double theR)
{
  MESSAGE("GEOM_Superv_i::MakeSphereR");
  get3DPrimOp();
  return my3DPrimOp->MakeSphereR(theR);
}

//=============================================================================
//  MakeSpherePntR:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeSpherePntR (GEOM::GEOM_Object_ptr thePnt, 
						     CORBA::Double theR)
{
  MESSAGE("GEOM_Superv_i::MakeSpherePntR");
  get3DPrimOp();
  return my3DPrimOp->MakeSpherePntR(thePnt, theR);
}

//=============================================================================
//  MakeTorusPntVecRR:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeTorusPntVecRR (GEOM::GEOM_Object_ptr thePnt,
							GEOM::GEOM_Object_ptr theVec,
							CORBA::Double theRMajor,
							CORBA::Double theRMinor)
{
  MESSAGE("GEOM_Superv_i::MakeTorusPntVecRR");
  get3DPrimOp();
  return my3DPrimOp->MakeTorusPntVecRR(thePnt, theVec, theRMajor, theRMinor);
}

//=============================================================================
//  MakeTorusRR:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeTorusRR (CORBA::Double theRMajor,
						  CORBA::Double theRMinor)
{
  MESSAGE("GEOM_Superv_i::MakeTorusRR");
  get3DPrimOp();
  return my3DPrimOp->MakeTorusRR(theRMajor, theRMinor);
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
  MESSAGE("GEOM_Superv_i::MakeConePntVecR1R2H");
  get3DPrimOp();
  return my3DPrimOp->MakeConePntVecR1R2H(thePnt, theAxis, theR1, theR2, theHeight);
}

//=============================================================================
//  MakeConeR1R2H:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeConeR1R2H (CORBA::Double theR1, 
						    CORBA::Double theR2, 
						    CORBA::Double theHeight)
{
  MESSAGE("GEOM_Superv_i::MakeConeR1R2H");
  get3DPrimOp();
  return my3DPrimOp->MakeConeR1R2H(theR1, theR2, theHeight);
}

//=============================================================================
//  MakePrismVecH:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakePrismVecH (GEOM::GEOM_Object_ptr theBase,
						    GEOM::GEOM_Object_ptr theVec,
						    CORBA::Double         theH)
{
  MESSAGE("GEOM_Superv_i::MakePrismVecH");
  get3DPrimOp();
  return my3DPrimOp->MakePrismVecH(theBase, theVec, theH);
}


//=============================================================================
//  MakePrismTwoPnt:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakePrismTwoPnt (GEOM::GEOM_Object_ptr theBase,
						      GEOM::GEOM_Object_ptr thePoint1,
						      GEOM::GEOM_Object_ptr thePoint2)
{
  MESSAGE("GEOM_Superv_i::MakePrismTwoPnt");
  get3DPrimOp();
  return my3DPrimOp->MakePrismTwoPnt(theBase, thePoint1, thePoint2);
}

//=============================================================================
//  MakePipe:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakePipe (GEOM::GEOM_Object_ptr theBase, 
					       GEOM::GEOM_Object_ptr thePath)
{
  MESSAGE("GEOM_Superv_i::MakePipe");
  get3DPrimOp();
  return my3DPrimOp->MakePipe(theBase, thePath);
}

//=============================================================================
//  MakeRevolutionAxisAngle:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeRevolutionAxisAngle (GEOM::GEOM_Object_ptr theBase,
							      GEOM::GEOM_Object_ptr theAxis,
							      CORBA::Double theAngle)
{
  MESSAGE("GEOM_Superv_i::MakeRevolutionAxisAngle");
  get3DPrimOp();
  return my3DPrimOp->MakeRevolutionAxisAngle(theBase, theAxis, theAngle);
}

//=============================================================================
//  MakeFilling:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeFilling (GEOM::GEOM_Object_ptr theShape,
						  CORBA::Long theMinDeg, CORBA::Long theMaxDeg,
						  CORBA::Double theTol2D, CORBA::Double theTol3D,
						  CORBA::Long theNbIter)
{
  MESSAGE("GEOM_Superv_i::MakeFilling");
  get3DPrimOp();
  return my3DPrimOp->MakeFilling(theShape, theMinDeg, theMaxDeg, theTol2D, theTol3D, theNbIter);
}

//============================= BooleanOperations =============================
//=============================================================================
//  MakeBoolean:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeBoolean (GEOM::GEOM_Object_ptr theShape1,
						  GEOM::GEOM_Object_ptr theShape2,
						  CORBA::Long theOperation)
{
  // theOperation indicates the operation to be done:
  // 1 - Common, 2 - Cut, 3 - Fuse, 4 - Section
  MESSAGE("GEOM_Superv_i::MakeBoolean");
  getBoolOp();
  return myBoolOp->MakeBoolean(theShape1, theShape2, theOperation);
}

//=============================================================================
//  MakeFuse:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeFuse (GEOM::GEOM_Object_ptr theShape1,
					       GEOM::GEOM_Object_ptr theShape2)
{
  MESSAGE("GEOM_Superv_i::MakeFuse");
  getBoolOp();
  return myBoolOp->MakeBoolean(theShape1, theShape2, 3);
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
    return myBoolOp->MakePartition(aListImplS->GetList(), aListImplT->GetList(), 
				   aListImplKI->GetList(), aListImplRI->GetList(),
				   theLimit, theRemoveWebs, aListImplM->GetList());
  }
  return NULL;
}

//=============================================================================
//  MakeHalfPartition:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeHalfPartition (GEOM::GEOM_Object_ptr theShape,
							GEOM::GEOM_Object_ptr thePlane)
{
  MESSAGE("GEOM_Superv_i::MakeHalfPartition");
  getBoolOp();
  return myBoolOp->MakeHalfPartition(theShape, thePlane);
}

//============================== InsertOperations =============================
//=============================================================================
//  MakeCopy:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeCopy (GEOM::GEOM_Object_ptr theOriginal)
{
  MESSAGE("GEOM_Superv_i::MakeCopy");
  getInsOp();
  return myInsOp->MakeCopy(theOriginal);
}

//=============================================================================
//  Export:
//=============================================================================
void GEOM_Superv_i::Export (GEOM::GEOM_Object_ptr theObject, 
			    const char*           theFileName, 
			    const char*           theFormatName)
{
  MESSAGE("GEOM_Superv_i::Export");
  getInsOp();
  myInsOp->Export(theObject, theFileName, theFormatName);
}

//=============================================================================
//  Import:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::Import (const char* theFileName, 
					     const char* theFormatName)
{
  MESSAGE("GEOM_Superv_i::Import");
  getInsOp();
  return myInsOp->Import(theFileName, theFormatName);
}

//=============================================================================
//  ImportTranslators:
//=============================================================================
void GEOM_Superv_i::ImportTranslators (GEOM::string_array_out theFormats,
				       GEOM::string_array_out thePatterns)
{
  MESSAGE("GEOM_Superv_i::ImportTranslators");
  getInsOp();
  myInsOp->ImportTranslators(theFormats, thePatterns);
}

//=============================================================================
//  ExportTranslators:
//=============================================================================
void GEOM_Superv_i::ExportTranslators (GEOM::string_array_out theFormats,
				       GEOM::string_array_out thePatterns)
{
  MESSAGE("GEOM_Superv_i::ExportTranslators");
  getInsOp();
  myInsOp->ExportTranslators(theFormats, thePatterns);
}

//============================= TransformOperations ===========================
//=============================================================================
//  TranslateTwoPoints:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::TranslateTwoPoints (GEOM::GEOM_Object_ptr theObject,
							 GEOM::GEOM_Object_ptr thePoint1,
							 GEOM::GEOM_Object_ptr thePoint2)
{
  MESSAGE("GEOM_Superv_i::TranslateTwoPoints");
  getTransfOp();
  return myTransfOp->TranslateTwoPoints(theObject, thePoint1, thePoint2);
}

//=============================================================================
//  TranslateTwoPointsCopy:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::TranslateTwoPointsCopy (GEOM::GEOM_Object_ptr theObject,
							     GEOM::GEOM_Object_ptr thePoint1,
							     GEOM::GEOM_Object_ptr thePoint2)
{
  MESSAGE("GEOM_Superv_i::TranslateTwoPointsCopy");
  getTransfOp();
  return myTransfOp->TranslateTwoPointsCopy(theObject, thePoint1, thePoint2);
}

//=============================================================================
//  TranslateDXDYDZ:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::TranslateDXDYDZ (GEOM::GEOM_Object_ptr theObject,
						      CORBA::Double theDX, 
						      CORBA::Double theDY, 
						      CORBA::Double theDZ)
{
  MESSAGE("GEOM_Superv_i::TranslateDXDYDZ");
  getTransfOp();
  return myTransfOp->TranslateDXDYDZ(theObject, theDX, theDY, theDZ);
}

//=============================================================================
//  TranslateDXDYDZCopy:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::TranslateDXDYDZCopy (GEOM::GEOM_Object_ptr theObject,
							  CORBA::Double theDX, 
							  CORBA::Double theDY, 
							  CORBA::Double theDZ)
{
  MESSAGE("GEOM_Superv_i::TranslateDXDYDZCopy");
  getTransfOp();
  return myTransfOp->TranslateDXDYDZCopy(theObject, theDX, theDY, theDZ);
}

//=============================================================================
//  TranslateVector:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::TranslateVector (GEOM::GEOM_Object_ptr theObject,
						      GEOM::GEOM_Object_ptr theVector)
{
  MESSAGE("GEOM_Superv_i::TranslateVector");
  getTransfOp();
  return myTransfOp->TranslateVector(theObject, theVector);
}

//=============================================================================
//  TranslateVectorCopy:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::TranslateVectorCopy (GEOM::GEOM_Object_ptr theObject,
							  GEOM::GEOM_Object_ptr theVector)
{
  MESSAGE("GEOM_Superv_i::TranslateVectorCopy");
  getTransfOp();
  return myTransfOp->TranslateVectorCopy(theObject, theVector);
}

//=============================================================================
//  MultiTranslate1D:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MultiTranslate1D (GEOM::GEOM_Object_ptr theObject,
						       GEOM::GEOM_Object_ptr theVector,
						       CORBA::Double theStep,
						       CORBA::Long theNbTimes)
{
  MESSAGE("GEOM_Superv_i::MultiTranslate1D");
  getTransfOp();
  return myTransfOp->MultiTranslate1D(theObject, theVector, theStep, theNbTimes);
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
  MESSAGE("GEOM_Superv_i::MultiTranslate2D");
  getTransfOp();
  return myTransfOp->MultiTranslate2D(theObject, theVector1, theStep1, theNbTimes1,
				      theVector2, theStep2, theNbTimes2);
}

//=============================================================================
//  Rotate:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::Rotate (GEOM::GEOM_Object_ptr theObject,
					     GEOM::GEOM_Object_ptr theAxis,
					     CORBA::Double theAngle)
{
  MESSAGE("GEOM_Superv_i::Rotate");
  getTransfOp();
  return myTransfOp->Rotate(theObject, theAxis, theAngle);
}

//=============================================================================
//  RotateCopy:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::RotateCopy (GEOM::GEOM_Object_ptr theObject,
						 GEOM::GEOM_Object_ptr theAxis,
						 CORBA::Double theAngle)
{
  MESSAGE("GEOM_Superv_i::RotateCopy");
  getTransfOp();
  return myTransfOp->RotateCopy(theObject, theAxis, theAngle);
}

//=============================================================================
//  MultiRotate1D:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MultiRotate1D (GEOM::GEOM_Object_ptr theObject,
						    GEOM::GEOM_Object_ptr theAxis,
						    CORBA::Long theNbTimes)
{
  MESSAGE("GEOM_Superv_i::MultiRotate1D");
  getTransfOp();
  return myTransfOp->MultiRotate1D(theObject, theAxis, theNbTimes);
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
  MESSAGE("GEOM_Superv_i::MultiRotate2D");
  getTransfOp();
  return myTransfOp->MultiRotate2D(theObject, theAxis, theAngle, theNbTimes1, theStep, theNbTimes2);
}

//=============================================================================
//  MirrorPlane:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MirrorPlane (GEOM::GEOM_Object_ptr theObject, 
						  GEOM::GEOM_Object_ptr thePlane)
{
  MESSAGE("GEOM_Superv_i::MirrorPlane");
  getTransfOp();
  return myTransfOp->MirrorPlane(theObject, thePlane);
}

//=============================================================================
//  MirrorPlaneCopy:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MirrorPlaneCopy (GEOM::GEOM_Object_ptr theObject, 
						      GEOM::GEOM_Object_ptr thePlane)
{
  MESSAGE("GEOM_Superv_i::MirrorPlaneCopy");
  getTransfOp();
  return myTransfOp->MirrorPlaneCopy(theObject, thePlane);
}

//=============================================================================
//  MirrorAxis:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MirrorAxis (GEOM::GEOM_Object_ptr theObject, 
						 GEOM::GEOM_Object_ptr theAxis)
{
  MESSAGE("GEOM_Superv_i::MirrorAxis");
  getTransfOp();
  return myTransfOp->MirrorAxis(theObject, theAxis);
}

//=============================================================================
//  MirrorAxisCopy:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MirrorAxisCopy (GEOM::GEOM_Object_ptr theObject, 
						     GEOM::GEOM_Object_ptr theAxis)
{
  MESSAGE("GEOM_Superv_i::MirrorAxisCopy");
  getTransfOp();
  return myTransfOp->MirrorAxisCopy(theObject, theAxis);
}

//=============================================================================
//  MirrorPoint:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MirrorPoint (GEOM::GEOM_Object_ptr theObject, 
						  GEOM::GEOM_Object_ptr thePoint)
{
  MESSAGE("GEOM_Superv_i::MirrorPoint");
  getTransfOp();
  return myTransfOp->MirrorPoint(theObject, thePoint);
}

//=============================================================================
//  MirrorPointCopy:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MirrorPointCopy (GEOM::GEOM_Object_ptr theObject, 
						      GEOM::GEOM_Object_ptr thePoint)
{
  MESSAGE("GEOM_Superv_i::MirrorPointCopy");
  getTransfOp();
  return myTransfOp->MirrorPointCopy(theObject, thePoint);
}

//=============================================================================
//  OffsetShape:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::OffsetShape (GEOM::GEOM_Object_ptr theObject, 
						  CORBA::Double theOffset)
{
  MESSAGE("GEOM_Superv_i::OffsetShape");
  getTransfOp();
  return myTransfOp->OffsetShape(theObject, theOffset);
}

//=============================================================================
//  OffsetShapeCopy:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::OffsetShapeCopy (GEOM::GEOM_Object_ptr theObject, 
						      CORBA::Double theOffset)
{
  MESSAGE("GEOM_Superv_i::OffsetShapeCopy");
  getTransfOp();
  return myTransfOp->OffsetShapeCopy(theObject, theOffset);
}

//=============================================================================
//  ScaleShape:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::ScaleShape (GEOM::GEOM_Object_ptr theObject, 
						 GEOM::GEOM_Object_ptr thePoint,
						 CORBA::Double theFactor)
{
  MESSAGE("GEOM_Superv_i::ScaleShape");
  getTransfOp();
  return myTransfOp->ScaleShape(theObject, thePoint, theFactor);
}

//=============================================================================
//  ScaleShapeCopy:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::ScaleShapeCopy (GEOM::GEOM_Object_ptr theObject, 
						     GEOM::GEOM_Object_ptr thePoint,
						     CORBA::Double theFactor)
{
  MESSAGE("GEOM_Superv_i::ScaleShapeCopy");
  getTransfOp();
  return myTransfOp->ScaleShapeCopy(theObject, thePoint, theFactor);
}

//=============================================================================
//  PositionShape:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::PositionShape (GEOM::GEOM_Object_ptr theObject,
						    GEOM::GEOM_Object_ptr theStartLCS,
						    GEOM::GEOM_Object_ptr theEndLCS)
{
  MESSAGE("GEOM_Superv_i::PositionShape");
  getTransfOp();
  return myTransfOp->PositionShape(theObject, theStartLCS, theEndLCS);
}

//=============================================================================
//  PositionShapeCopy:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::PositionShapeCopy (GEOM::GEOM_Object_ptr theObject,
							GEOM::GEOM_Object_ptr theStartLCS,
							GEOM::GEOM_Object_ptr theEndLCS)
{
  MESSAGE("GEOM_Superv_i::PositionShapeCopy");
  getTransfOp();
  return myTransfOp->PositionShapeCopy(theObject, theStartLCS, theEndLCS);
}

//=============================== ShapesOperations ============================
//=============================================================================
//  Make:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeEdge (GEOM::GEOM_Object_ptr thePnt1,
					       GEOM::GEOM_Object_ptr thePnt2)
{
  MESSAGE("GEOM_Superv_i::MakeEdge");
  getShapesOp();
  return myShapesOp->MakeEdge(thePnt1, thePnt2);
}

//=============================================================================
//  MakeWire:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeWire (GEOM::GEOM_List_ptr theEdgesAndWires)
{
  MESSAGE("GEOM_Superv_i::MakeWire");
  if (GEOM_List_i<GEOM::ListOfGO>* aListImplEW = 
      dynamic_cast<GEOM_List_i<GEOM::ListOfGO>*>(GetServant(theEdgesAndWires, myPOA).in())) {
    getShapesOp();
    return myShapesOp->MakeWire(aListImplEW->GetList());
  }
  return NULL;
}

//=============================================================================
//  MakeFace:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeFace (GEOM::GEOM_Object_ptr theWire,
					       CORBA::Boolean isPlanarWanted)
{
  MESSAGE("GEOM_Superv_i::MakeFace");
  getShapesOp();
  return myShapesOp->MakeFace(theWire, isPlanarWanted);
}

//=============================================================================
//  MakeFaceWires:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeFaceWires (GEOM::GEOM_List_ptr theWires,
						    CORBA::Boolean isPlanarWanted)
{
  MESSAGE("GEOM_Superv_i::MakeFaceWires");
  if (GEOM_List_i<GEOM::ListOfGO>* aListImplW = 
      dynamic_cast<GEOM_List_i<GEOM::ListOfGO>*>(GetServant(theWires, myPOA).in())) {
    getShapesOp();
    return myShapesOp->MakeFaceWires(aListImplW->GetList(), isPlanarWanted);
  }
  return NULL;
}

//=============================================================================
//  MakeShell:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeShell (GEOM::GEOM_List_ptr theFacesAndShells)
{
  MESSAGE("GEOM_Superv_i::MakeShell");
  if (GEOM_List_i<GEOM::ListOfGO>* aListImplFS = 
      dynamic_cast<GEOM_List_i<GEOM::ListOfGO>*>(GetServant(theFacesAndShells, myPOA).in())) {
    getShapesOp();
    return myShapesOp->MakeShell(aListImplFS->GetList());
  }
  return NULL;
}

//=============================================================================
//  MakeSolidShell:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeSolidShell (GEOM::GEOM_Object_ptr theShell)
{
  MESSAGE("GEOM_Superv_i::MakeSolidShell");
  getShapesOp();
  return myShapesOp->MakeSolidShell(theShell);
}

//=============================================================================
//  MakeSolidShells:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeSolidShells (GEOM::GEOM_List_ptr theShells)
{
  MESSAGE("GEOM_Superv_i::MakeSolidShells");
  if (GEOM_List_i<GEOM::ListOfGO>* aListImplS = 
      dynamic_cast<GEOM_List_i<GEOM::ListOfGO>*>(GetServant(theShells, myPOA).in())) {
    getShapesOp();
    return myShapesOp->MakeSolidShells(aListImplS->GetList());
  }
  return NULL;
}

//=============================================================================
//  MakeCompound:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeCompound (GEOM::GEOM_List_ptr theShapes)
{
  MESSAGE("GEOM_Superv_i::MakeCompound");
  if (GEOM_List_i<GEOM::ListOfGO>* aListImpl = 
      dynamic_cast<GEOM_List_i<GEOM::ListOfGO>*>(GetServant(theShapes, myPOA).in())) {
    getShapesOp();
    return myShapesOp->MakeCompound(aListImpl->GetList());
  }
  return NULL;
}

//=============================================================================
//  MakeGlueFaces:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeGlueFaces (GEOM::GEOM_Object_ptr theShape,
								CORBA::Double   theTolerance)
{
  MESSAGE("GEOM_Superv_i::MakeGlueFaces");
  getShapesOp();
  return myShapesOp->MakeGlueFaces(theShape, theTolerance);
}

//=============================================================================
//  MakeExplode:
//=============================================================================
GEOM::GEOM_List_ptr GEOM_Superv_i::MakeExplode (GEOM::GEOM_Object_ptr theShape,
						    CORBA::Long theShapeType,
						    CORBA::Boolean isSorted)
{
  MESSAGE("GEOM_Superv_i::MakeExplode");
  getShapesOp();

  GEOM::ListOfGO* aList = myShapesOp->MakeExplode(theShape, theShapeType, isSorted);
  GEOM_List_i<GEOM::ListOfGO>* aListPtr = new GEOM_List_i<GEOM::ListOfGO>(*(aList));
  MESSAGE(" List of "<<aListPtr->GetList().length()<<" element(s)");
  return aListPtr->_this();
}

//=============================================================================
//  NumberOfFaces:
//=============================================================================
CORBA::Long GEOM_Superv_i::NumberOfFaces (GEOM::GEOM_Object_ptr theShape)
{
  MESSAGE("GEOM_Superv_i::NumberOfFaces");
  getShapesOp();
  return myShapesOp->NumberOfFaces(theShape);
}

//=============================================================================
//  NumberOfEdges:
//=============================================================================
CORBA::Long GEOM_Superv_i::NumberOfEdges (GEOM::GEOM_Object_ptr theShape)
{
  MESSAGE("GEOM_Superv_i::NumberOfEdges");
  getShapesOp();
  return myShapesOp->NumberOfEdges(theShape);
}

//=============================================================================
//  ChangeOrientation:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::ChangeOrientation (GEOM::GEOM_Object_ptr theShape)
{
  MESSAGE("GEOM_Superv_i::ChangeOrientation");
  getShapesOp();
  return myShapesOp->ChangeOrientation(theShape);
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
  MESSAGE("GEOM_Superv_i::MakeQuad4Vertices");
  getBlocksOp();
  return myBlocksOp->MakeQuad4Vertices(thePnt1, thePnt2, thePnt3, thePnt4);
}

//=============================================================================
//  MakeQuad:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeQuad (GEOM::GEOM_Object_ptr theEdge1,
					       GEOM::GEOM_Object_ptr theEdge2,
					       GEOM::GEOM_Object_ptr theEdge3,
					       GEOM::GEOM_Object_ptr theEdge4)
{
  MESSAGE("GEOM_Superv_i::MakeQuad");
  getBlocksOp();
  return myBlocksOp->MakeQuad(theEdge1, theEdge2, theEdge3, theEdge4);
}

//=============================================================================
//  MakeQuad2Edges:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeQuad2Edges (GEOM::GEOM_Object_ptr theEdge1,
						     GEOM::GEOM_Object_ptr theEdge2)
{
  MESSAGE("GEOM_Superv_i::MakeQuad2Edges");
  getBlocksOp();
  return myBlocksOp->MakeQuad2Edges(theEdge1, theEdge2);
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
  MESSAGE("GEOM_Superv_i::MakeHexa");
  getBlocksOp();
  return myBlocksOp->MakeHexa(theFace1, theFace2, theFace3, theFace4, theFace5, theFace6);
}

//=============================================================================
//  MakeHexa2Faces:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeHexa2Faces (GEOM::GEOM_Object_ptr theFace1,
						     GEOM::GEOM_Object_ptr theFace2)
{
  MESSAGE("GEOM_Superv_i::MakeHexa2Faces");
  getBlocksOp();
  return myBlocksOp->MakeHexa2Faces(theFace1, theFace2);
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
  MESSAGE("GEOM_Superv_i::GetPoint");
  getBlocksOp();
  return myBlocksOp->GetPoint(theShape, theX, theY, theZ, theEpsilon);
}

//=============================================================================
//  GetEdge:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::GetEdge (GEOM::GEOM_Object_ptr theShape,
					      GEOM::GEOM_Object_ptr thePoint1,
					      GEOM::GEOM_Object_ptr thePoint2)
{
  MESSAGE("GEOM_Superv_i::GetEdge");
  getBlocksOp();
  return myBlocksOp->GetEdge(theShape, thePoint1, thePoint2);
}

//=============================================================================
//  GetEdgeNearPoint:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::GetEdgeNearPoint (GEOM::GEOM_Object_ptr theShape,
						       GEOM::GEOM_Object_ptr thePoint)
{
  MESSAGE("GEOM_Superv_i::GetEdgeNearPoint");
  getBlocksOp();
  return myBlocksOp->GetEdgeNearPoint(theShape, thePoint);
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
  MESSAGE("GEOM_Superv_i::GetFaceByPoints");
  getBlocksOp();
  return myBlocksOp->GetFaceByPoints(theShape, thePoint1, thePoint2, thePoint3, thePoint4);
}

//=============================================================================
//  GetFaceByEdges:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::GetFaceByEdges (GEOM::GEOM_Object_ptr theShape,
						     GEOM::GEOM_Object_ptr theEdge1,
						     GEOM::GEOM_Object_ptr theEdge2)
{
  MESSAGE("GEOM_Superv_i::GetFaceByEdges");
  getBlocksOp();
  return myBlocksOp->GetFaceByEdges(theShape, theEdge1, theEdge2);
}

//=============================================================================
//  GetOppositeFace:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::GetOppositeFace (GEOM::GEOM_Object_ptr theBlock,
						      GEOM::GEOM_Object_ptr theFace)
{
  MESSAGE("GEOM_Superv_i::GetOppositeFace");
  getBlocksOp();
  return myBlocksOp->GetOppositeFace(theBlock, theFace);
}

//=============================================================================
//  GetFaceNearPoint:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::GetFaceNearPoint (GEOM::GEOM_Object_ptr theShape,
						       GEOM::GEOM_Object_ptr thePoint)
{
  MESSAGE("GEOM_Superv_i::GetFaceNearPoint");
  getBlocksOp();
  return myBlocksOp->GetFaceNearPoint(theShape, thePoint);
}

//=============================================================================
//  GetFaceByNormale:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::GetFaceByNormale (GEOM::GEOM_Object_ptr theBlock,
						       GEOM::GEOM_Object_ptr theVector)
{
  MESSAGE("GEOM_Superv_i::GetFaceByNormale");
  getBlocksOp();
  return myBlocksOp->GetFaceByNormale(theBlock, theVector);
}

//=============================================================================
//  IsCompoundOfBlocks:
//=============================================================================
CORBA::Boolean GEOM_Superv_i::IsCompoundOfBlocks (GEOM::GEOM_Object_ptr theCompound,
						  CORBA::Long     theMinNbFaces,
						  CORBA::Long     theMaxNbFaces,
						  CORBA::Long&          theNbBlocks)
{
  MESSAGE("GEOM_Superv_i::IsCompoundOfBlocks");
  getBlocksOp();
  return myBlocksOp->IsCompoundOfBlocks(theCompound, theMinNbFaces, theMaxNbFaces, theNbBlocks);
}

//=============================================================================
//  CheckCompoundOfBlocks:
//=============================================================================
CORBA::Boolean GEOM_Superv_i::CheckCompoundOfBlocks 
(GEOM::GEOM_Object_ptr theCompound,
 GEOM::GEOM_IBlocksOperations::BCErrors_out theErrors)
{
  MESSAGE("GEOM_Superv_i::CheckCompoundOfBlocks");
  getBlocksOp();
  return myBlocksOp->CheckCompoundOfBlocks(theCompound, theErrors);
}

//=============================================================================
//  PrintBCErrors:
//=============================================================================
char* GEOM_Superv_i::PrintBCErrors (GEOM::GEOM_Object_ptr theCompound,
				    const GEOM::GEOM_IBlocksOperations::BCErrors& theErrors)
{
  MESSAGE("GEOM_Superv_i::PrintBCErrors");
  getBlocksOp();
  return myBlocksOp->PrintBCErrors(theCompound, theErrors);
}

//=============================================================================
//  ExplodeCompoundOfBlocks:
//=============================================================================
GEOM::GEOM_List_ptr GEOM_Superv_i::ExplodeCompoundOfBlocks (GEOM::GEOM_Object_ptr theCompound,
								CORBA::Long     theMinNbFaces,
								CORBA::Long     theMaxNbFaces)
{
  MESSAGE("GEOM_Superv_i::ExplodeCompoundOfBlocks");
  getBlocksOp();
  GEOM::ListOfGO* aBlocks = myBlocksOp->ExplodeCompoundOfBlocks(theCompound, theMinNbFaces, theMaxNbFaces);
  GEOM_List_i<GEOM::ListOfGO>* aListPtr = new GEOM_List_i<GEOM::ListOfGO>(*(aBlocks));
  return aListPtr->_this();
}

//=============================================================================
//  GetBlockNearPoint:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::GetBlockNearPoint (GEOM::GEOM_Object_ptr theCompound,
							GEOM::GEOM_Object_ptr thePoint)
{
  MESSAGE("GEOM_Superv_i::GetBlockNearPoint");
  getBlocksOp();
  return myBlocksOp->GetBlockNearPoint(theCompound, thePoint);
}

//=============================================================================
//  GetBlockByParts:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::GetBlockByParts (GEOM::GEOM_Object_ptr theCompound,
						      GEOM::GEOM_List_ptr theParts)
{
  MESSAGE("GEOM_Superv_i::GetBlockByParts");
  if (GEOM_List_i<GEOM::ListOfGO>* aListImplP = 
      dynamic_cast<GEOM_List_i<GEOM::ListOfGO>*>(GetServant(theParts, myPOA).in())) {
    getBlocksOp();
    return myBlocksOp->GetBlockByParts(theCompound, aListImplP->GetList());
  }
  return NULL;
}

//=============================================================================
//  GetBlocksByParts:
//=============================================================================
GEOM::GEOM_List_ptr GEOM_Superv_i::GetBlocksByParts (GEOM::GEOM_Object_ptr theCompound,
							 GEOM::GEOM_List_ptr theParts)
{
  MESSAGE("GEOM_Superv_i::GetBlocksByParts");
  if (GEOM_List_i<GEOM::ListOfGO>* aListImplP = 
      dynamic_cast<GEOM_List_i<GEOM::ListOfGO>*>(GetServant(theParts, myPOA).in())) {
    getBlocksOp();
    
    GEOM::ListOfGO* aBlocks = myBlocksOp->GetBlocksByParts(theCompound, aListImplP->GetList());
    GEOM_List_i<GEOM::ListOfGO>* aListPtr = new GEOM_List_i<GEOM::ListOfGO>(*(aBlocks));
    return aListPtr->_this();
  }
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
  MESSAGE("GEOM_Superv_i::MakeMultiTransformation1D");
  getBlocksOp();
  return myBlocksOp->MakeMultiTransformation1D(theBlock, theDirFace1, theDirFace2, theNbTimes);
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
  MESSAGE("GEOM_Superv_i::MakeMultiTransformation2D");
  getBlocksOp();
  return myBlocksOp->MakeMultiTransformation2D(theBlock, 
					       theDirFace1U, theDirFace2U, theNbTimesU,
					       theDirFace1V, theDirFace2V, theNbTimesV);
}

//=============================== CurvesOperations ============================
//=============================================================================
//  MakeCirclePntVecR:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeCirclePntVecR (GEOM::GEOM_Object_ptr theCenter,
							GEOM::GEOM_Object_ptr theVector,
							CORBA::Double theR)
{
  MESSAGE("GEOM_Superv_i::MakeCirclePntVecR");
  getCurvesOp();
  return myCurvesOp->MakeCirclePntVecR(theCenter, theVector, theR);
}

//=============================================================================
//  MakeCircleThreePnt:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeCircleThreePnt (GEOM::GEOM_Object_ptr thePnt1,
							 GEOM::GEOM_Object_ptr thePnt2,
							 GEOM::GEOM_Object_ptr thePnt3)
{
  MESSAGE("GEOM_Superv_i::MakeCircleThreePnt");
  getCurvesOp();
  return myCurvesOp->MakeCircleThreePnt(thePnt1, thePnt2, thePnt3);
}

//=============================================================================
//  MakeEllipse:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeEllipse (GEOM::GEOM_Object_ptr theCenter,
						  GEOM::GEOM_Object_ptr theVector,
						  CORBA::Double theRMajor, 
						  CORBA::Double theRMinor)
{
  MESSAGE("GEOM_Superv_i::MakeEllipse");
  getCurvesOp();
  return myCurvesOp->MakeEllipse(theCenter, theVector, theRMajor, theRMinor);
}

//=============================================================================
//  MakeArc:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeArc (GEOM::GEOM_Object_ptr thePnt1,
					      GEOM::GEOM_Object_ptr thePnt2,
					      GEOM::GEOM_Object_ptr thePnt3)
{
  MESSAGE("GEOM_Superv_i::MakeArc");
  getCurvesOp();
  return myCurvesOp->MakeArc(thePnt1, thePnt2, thePnt3);
}

//=============================================================================
//  MakePolyline:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakePolyline (GEOM::GEOM_List_ptr thePoints)
{
  MESSAGE("GEOM_Superv_i::MakePolyline");
  if (GEOM_List_i<GEOM::ListOfGO>* aListImplP = 
      dynamic_cast<GEOM_List_i<GEOM::ListOfGO>*>(GetServant(thePoints, myPOA).in())) {
    getCurvesOp();
    return myCurvesOp->MakePolyline(aListImplP->GetList());
  }
  return NULL;
}

//=============================================================================
//  MakeSplineBezier:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeSplineBezier (GEOM::GEOM_List_ptr thePoints)
{
  MESSAGE("GEOM_Superv_i::MakeSplineBezier");
  if (GEOM_List_i<GEOM::ListOfGO>* aListImplP = 
      dynamic_cast<GEOM_List_i<GEOM::ListOfGO>*>(GetServant(thePoints, myPOA).in())) {
    getCurvesOp();
    return myCurvesOp->MakeSplineBezier(aListImplP->GetList());
  }
  return NULL;
}

//=============================================================================
//  MakeSplineInterpolation:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeSplineInterpolation (GEOM::GEOM_List_ptr thePoints)
{
  MESSAGE("GEOM_Superv_i::MakeSplineInterpolation");
  if (GEOM_List_i<GEOM::ListOfGO>* aListImplP = 
      dynamic_cast<GEOM_List_i<GEOM::ListOfGO>*>(GetServant(thePoints, myPOA).in())) {
    getCurvesOp();
    return myCurvesOp->MakeSplineInterpolation(aListImplP->GetList());
  }
  return NULL;
}

//=============================================================================
//  MakeSketcher:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeSketcher (const char* theCommand, 
						   GEOM::GEOM_List_ptr theWorkingPlane)
{
  MESSAGE("GEOM_Superv_i::MakeSketcher");
  if (GEOM_List_i<GEOM::ListOfDouble>* aListImplWP = 
      dynamic_cast<GEOM_List_i<GEOM::ListOfDouble>*>(GetServant(theWorkingPlane, myPOA).in())) {
    getCurvesOp();
    return myCurvesOp->MakeSketcher(theCommand, aListImplWP->GetList());
  }
  return NULL;
}

//=============================== LocalOperations =============================
//=============================================================================
//  MakeFilletAll:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeFilletAll (GEOM::GEOM_Object_ptr theShape,
						    CORBA::Double theR)
{
  MESSAGE("GEOM_Superv_i::MakeFilletAllMakeSketcher");
  getLocalOp();
  return myLocalOp->MakeFilletAll(theShape, theR);
}

//=============================================================================
//  MakeFilletEdges:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeFilletEdges (GEOM::GEOM_Object_ptr theShape, 
						      CORBA::Double theR,
						      GEOM::GEOM_List_ptr theEdges)
{
  MESSAGE("GEOM_Superv_i::MakeFilletEdges");
  if (GEOM_List_i<GEOM::ListOfLong>* aListImplE = 
      dynamic_cast<GEOM_List_i<GEOM::ListOfLong>*>(GetServant(theEdges, myPOA).in())) {
    getLocalOp();
    return myLocalOp->MakeFilletEdges(theShape, theR, aListImplE->GetList());
  }
  return NULL;
}

//=============================================================================
//  MakeFilletFaces:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeFilletFaces (GEOM::GEOM_Object_ptr theShape, 
						      CORBA::Double theR,
						      GEOM::GEOM_List_ptr theFaces)
{
  MESSAGE("GEOM_Superv_i::MakeFilletFaces");
  if (GEOM_List_i<GEOM::ListOfLong>* aListImplF = 
      dynamic_cast<GEOM_List_i<GEOM::ListOfLong>*>(GetServant(theFaces, myPOA).in())) {
    getLocalOp();
    return myLocalOp->MakeFilletFaces(theShape, theR, aListImplF->GetList());
  }
  return NULL;
}

//=============================================================================
//  MakeChamferAll:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeChamferAll (GEOM::GEOM_Object_ptr theShape, CORBA::Double theD)
{
  MESSAGE("GEOM_Superv_i::MakeChamferAll");
  getLocalOp();
  return myLocalOp->MakeChamferAll(theShape, theD);
}
  
//=============================================================================
//  MakeChamferEdge:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeChamferEdge (GEOM::GEOM_Object_ptr theShape,
						      CORBA::Double theD1, CORBA::Double theD2,
						      CORBA::Long theFace1, CORBA::Long theFace2)
{
  MESSAGE("GEOM_Superv_i::MakeChamferEdge");
  getLocalOp();
  return myLocalOp->MakeChamferEdge(theShape, theD1, theD2, theFace1, theFace2);
}

//=============================================================================
//  MakeChamferFaces:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::MakeChamferFaces (GEOM::GEOM_Object_ptr theShape,
						       CORBA::Double theD1, CORBA::Double theD2,
						       GEOM::GEOM_List_ptr theFaces)
{
  MESSAGE("GEOM_Superv_i::MakeChamferFaces");
  if (GEOM_List_i<GEOM::ListOfLong>* aListImplF = 
      dynamic_cast<GEOM_List_i<GEOM::ListOfLong>*>(GetServant(theFaces, myPOA).in())) {
    getLocalOp();
    return myLocalOp->MakeChamferFaces(theShape, theD1, theD2, aListImplF->GetList());
  }
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
  MESSAGE("GEOM_Superv_i::MakeArchimede");
  getLocalOp();
  return myLocalOp->MakeArchimede(theShape, theWeight, theWaterDensity, theMeshingDeflection);
}

//=============================================================================
//  GetSubShapeIndexMakeFilletAll:
//=============================================================================
CORBA::Long GEOM_Superv_i::GetSubShapeIndex (GEOM::GEOM_Object_ptr theShape,
					     GEOM::GEOM_Object_ptr theSubShape)
{
  MESSAGE("GEOM_Superv_i::GetSubShapeIndexMakeArchimede");
  getLocalOp();
  return myLocalOp->GetSubShapeIndex(theShape, theSubShape);
}

//=============================== GroupOperations =============================
//=============================================================================
//  CreateGroup:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::CreateGroup (GEOM::GEOM_Object_ptr theMainShape, 
						  CORBA::Long theShapeType)
{
  MESSAGE("GEOM_Superv_i::CreateGroup");
  getGroupOp();
  return myGroupOp->CreateGroup(theMainShape, theShapeType);
}

//=============================================================================
//  AddObject:
//=============================================================================
void GEOM_Superv_i::AddObject (GEOM::GEOM_Object_ptr theGroup, 
			       CORBA::Long theSubShapeId)
{
  MESSAGE("GEOM_Superv_i::AddObject");
  getGroupOp();
  myGroupOp->AddObject(theGroup, theSubShapeId);
}

//=============================================================================
//  RemoveObject:
//=============================================================================
void GEOM_Superv_i::RemoveObject (GEOM::GEOM_Object_ptr theGroup, 
				  CORBA::Long theSubShapeId)
{
  MESSAGE("GEOM_Superv_i::RemoveObject");
  getGroupOp();
  myGroupOp->RemoveObject(theGroup, theSubShapeId);
}

//=============================================================================
//  GetType:
//=============================================================================
CORBA::Long GEOM_Superv_i::GetType (GEOM::GEOM_Object_ptr theGroup)
{
  MESSAGE("GEOM_Superv_i::GetType");
  getGroupOp();
  return myGroupOp->GetType(theGroup);
}

//=============================================================================
//  GetMainShape:
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_Superv_i::GetMainShape (GEOM::GEOM_Object_ptr theGroup)
{
  MESSAGE("GEOM_Superv_i::GetMainShape");
  getGroupOp();
  return myGroupOp->GetMainShape(theGroup);
}

//=============================================================================
//  GetObjects:
//=============================================================================
GEOM::GEOM_List_ptr GEOM_Superv_i::GetObjects (GEOM::GEOM_Object_ptr theGroup)
{
  MESSAGE("GEOM_Superv_i::GetObjects");
  getGroupOp();

  GEOM::ListOfLong* aList = myGroupOp->GetObjects(theGroup);
  GEOM_List_i<GEOM::ListOfLong>* aListPtr = new GEOM_List_i<GEOM::ListOfLong>(*(aList));
  MESSAGE(" List of "<<aListPtr->GetList().length()<<" element(s)");
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
