#ifndef __GEOM_GEN_I_H__
#define __GEOM_GEN_I_H__

#include "GEOMImpl_Gen.hxx"

// IDL headers
#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(GEOM_Gen)
#include CORBA_SERVER_HEADER(SALOMEDS)
#include CORBA_SERVER_HEADER(SALOMEDS_Attributes)

#include "SALOME_Component_i.hxx"

#include "SALOME_NamingService.hxx"

#include <Standard_ErrorHandler.hxx> // CAREFUL ! position of this file is critic : see Lucien PIGNOLONI / OCC

#include "GEOM_IBasicOperations_i.hh"
#include "GEOM_ITransformOperations_i.hh"
#include "GEOM_I3DPrimOperations_i.hh"
#include "GEOM_IShapesOperations_i.hh"
#include "GEOM_IBlocksOperations_i.hh"
#include "GEOM_IBooleanOperations_i.hh"
#include "GEOM_ICurvesOperations_i.hh"
#include "GEOM_ILocalOperations_i.hh"
#include "GEOM_IHealingOperations_i.hh"
#include "GEOM_IInsertOperations_i.hh"
#include "GEOM_IMeasureOperations_i.hh"
#include "GEOM_IGroupOperations_i.hh"

//=====================================================================
// GEOM_Gen_i : class definition
//=====================================================================
class GEOM_Gen_i: public POA_GEOM::GEOM_Gen, public Engines_Component_i
{
 public:

  //-----------------------------------------------------------------------//
  // Constructor / Destructor                                              //
  //-----------------------------------------------------------------------//
  // constructor to be called for servant creation. 
  GEOM_Gen_i();
  GEOM_Gen_i(CORBA::ORB_ptr orb,
	     PortableServer::POA_ptr poa,
	     PortableServer::ObjectId * contId, 
	     const char *instanceName, 
	     const char *interfaceName);

  // destructor, doing nothing (for now)
  virtual ~GEOM_Gen_i();

  // generic method to be put in a super class
  void register_name(char * name);


  //-----------------------------------------------------------------------//
  // Inherited methods from SALOMEDS::Driver                               //
  //-----------------------------------------------------------------------//    

  SALOMEDS::TMPFile* Save(SALOMEDS::SComponent_ptr theComponent,
			  const char* theURL,
			  bool isMultiFile);

  SALOMEDS::TMPFile* SaveASCII(SALOMEDS::SComponent_ptr theComponent,
			       const char* theURL,
			       bool isMultiFile);
  
  CORBA::Boolean Load(SALOMEDS::SComponent_ptr theComponent,
		      const SALOMEDS::TMPFile& theStream,
		      const char* theURL,
		      bool isMultiFile);

  CORBA::Boolean LoadASCII(SALOMEDS::SComponent_ptr theComponent,
			   const SALOMEDS::TMPFile& theStream,
			   const char* theURL,
			   bool isMultiFile);

  void Close(SALOMEDS::SComponent_ptr theComponent);
  char* ComponentDataType();


  char* IORToLocalPersistentID(SALOMEDS::SObject_ptr theSObject,
			       const char* IORString,
			       CORBA::Boolean isMultiFile,
			       CORBA::Boolean isASCII);
  char* LocalPersistentIDToIOR(SALOMEDS::SObject_ptr theSObject,
			       const char* aLocalPersistentID,
			       CORBA::Boolean isMultiFile,
			       CORBA::Boolean isASCII);

  bool CanPublishInStudy(CORBA::Object_ptr theIOR);
  SALOMEDS::SObject_ptr PublishInStudy(SALOMEDS::Study_ptr theStudy,
				       SALOMEDS::SObject_ptr theSObject,
				       CORBA::Object_ptr theObject,
				       const char* theName) throw (SALOME::SALOME_Exception) ;

  CORBA::Boolean CanCopy(SALOMEDS::SObject_ptr theObject);
  SALOMEDS::TMPFile* CopyFrom(SALOMEDS::SObject_ptr theObject, CORBA::Long& theObjectID);
  CORBA::Boolean CanPaste(const char* theComponentName, CORBA::Long theObjectID);
  SALOMEDS::SObject_ptr PasteInto(const SALOMEDS::TMPFile& theStream,
				  CORBA::Long theObjectID,
				  SALOMEDS::SObject_ptr theObject);

  //Adds theObject in the study with a name = theName, if theFather is not null the object is placed under theFather
  SALOMEDS::SObject_ptr AddInStudy(SALOMEDS::Study_ptr theStudy, GEOM::GEOM_Object_ptr theObject, const char* theName, GEOM::GEOM_Object_ptr theFather);

  //-----------------------------------------------------------------------//
  // Transaction methods                                                    //
  //-----------------------------------------------------------------------// 
 
  //Undos one transaction in the document associated with theStudyID
  virtual void Undo(CORBA::Long theStudyID);

  //Redos one transaction in the document associated with theStudyID
  virtual void Redo(CORBA::Long theStudyID);

  //-----------------------------------------------------------------------//
  // Operations methods                                                    //
  //-----------------------------------------------------------------------// 

  //Returns a pointer to BasicOperations interface
  virtual GEOM::GEOM_IBasicOperations_ptr GetIBasicOperations (CORBA::Long theStudyID)
    throw (SALOME::SALOME_Exception);
  
  //Returns a pointer to TransformOperations interface
  virtual GEOM::GEOM_ITransformOperations_ptr GetITransformOperations (CORBA::Long theStudyID)
    throw (SALOME::SALOME_Exception);

  //Returns a pointer to 3DPrimOperations interface
  virtual GEOM::GEOM_I3DPrimOperations_ptr GetI3DPrimOperations (CORBA::Long theStudyID)
    throw (SALOME::SALOME_Exception);

  //Returns a pointer to ShapesOperations interface
  virtual GEOM::GEOM_IShapesOperations_ptr GetIShapesOperations (CORBA::Long theStudyID)
    throw (SALOME::SALOME_Exception);

  //Returns a pointer to BlocksOperations interface
  virtual GEOM::GEOM_IBlocksOperations_ptr GetIBlocksOperations (CORBA::Long theStudyID)
    throw (SALOME::SALOME_Exception);

  //Returns a pointer to BooleanOperations interface
  virtual GEOM::GEOM_IBooleanOperations_ptr GetIBooleanOperations (CORBA::Long theStudyID)
    throw (SALOME::SALOME_Exception);

  //Returns a pointer to CurvesOperations interface
  virtual GEOM::GEOM_ICurvesOperations_ptr GetICurvesOperations (CORBA::Long theStudyID)
    throw (SALOME::SALOME_Exception);

  //Returns a pointer to LocalOperations interface
  virtual GEOM::GEOM_ILocalOperations_ptr GetILocalOperations (CORBA::Long theStudyID)
    throw (SALOME::SALOME_Exception);

  //Returns a pointer to HealingOperations interface
  virtual GEOM::GEOM_IHealingOperations_ptr GetIHealingOperations (CORBA::Long theStudyID)
    throw (SALOME::SALOME_Exception);

  //Returns a pointer to InsertOperations interface
  virtual GEOM::GEOM_IInsertOperations_ptr GetIInsertOperations (CORBA::Long theStudyID)
    throw (SALOME::SALOME_Exception);

  //Returns a pointer to MeasureOperations interface
  virtual GEOM::GEOM_IMeasureOperations_ptr GetIMeasureOperations (CORBA::Long theStudyID)
    throw (SALOME::SALOME_Exception);

  //Returns a pointer to GroupOperations interface
  virtual GEOM::GEOM_IGroupOperations_ptr GetIGroupOperations (CORBA::Long theStudyID)
    throw (SALOME::SALOME_Exception);

  //Adds a new sub shape
  virtual GEOM::GEOM_Object_ptr AddSubShape(GEOM::GEOM_Object_ptr theMainShape, const GEOM::ListOfLong& theIndices);

  virtual void RemoveObject(GEOM::GEOM_Object_ptr theObject);

  virtual  char* GetStringFromIOR(GEOM::GEOM_Object_ptr theObject);

  virtual GEOM::GEOM_Object_ptr GetIORFromString(const char* stringIOR);

  //********************************************************************************************************//
  //     Internal methods
  //********************************************************************************************************//
  virtual GEOM::GEOM_Object_ptr GetObject(CORBA::Long theStudyID, const char* theEntry);

 private:
 
   ::GEOMImpl_Gen* _impl;
   SALOME_NamingService * name_service;
   char * _name; 

};

#endif 
