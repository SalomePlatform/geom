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

#ifndef __GEOM_GEN_I_H__
#define __GEOM_GEN_I_H__

#include "GEOM_GEOM_I.hxx"

#include "GEOMImpl_Gen.hxx"

// IDL headers
#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(GEOM_Gen)

#include CORBA_CLIENT_HEADER(SALOMEDS)
#include CORBA_CLIENT_HEADER(SALOMEDS_Attributes)


#include "SALOME_Component_i.hxx"
#include "SALOME_NamingService.hxx"

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
#include "GEOM_IFieldOperations_i.hh"
#include "GEOMUtils.hxx"

#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_SequenceOfShape.hxx>

#include <map>
#include <set>
#include <list>
#include <string>

//#include <Standard_ErrorHandler.hxx> // CAREFUL ! position of this file is critic : see Lucien PIGNOLONI / OCC

//=====================================================================
// Generic operations creator (for plugins mechanism)
//=====================================================================
class GEOM_I_EXPORT GEOM_GenericOperationsCreator
{
public:
  // Create operations
  virtual GEOM_IOperations_i* Create (PortableServer::POA_ptr thePOA,
                                      int                     theStudyId,
                                      GEOM::GEOM_Gen_ptr      theEngine,
                                      ::GEOMImpl_Gen*         theGenImpl) = 0;
  // return the name of IDL module
  //virtual std::string GetModuleName() = 0;
};

//=====================================================================
// GEOM_Gen_i : class definition
//=====================================================================
class GEOM_I_EXPORT GEOM_Gen_i: virtual public POA_GEOM::GEOM_Gen, virtual public Engines_Component_i
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

  // Get ORB object
  CORBA::ORB_ptr GetORB() { return CORBA::ORB::_duplicate(_orb); }

  // Get Naming Service object
  SALOME_NamingService* GetNS() { return name_service; }

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

  GEOM::ListOfGO* PublishNamedShapesInStudy(SALOMEDS::Study_ptr theStudy,
                                            CORBA::Object_ptr theObject);

  CORBA::Boolean CanCopy(SALOMEDS::SObject_ptr theObject);
  SALOMEDS::TMPFile* CopyFrom(SALOMEDS::SObject_ptr theObject, CORBA::Long& theObjectID);
  CORBA::Boolean CanPaste(const char* theComponentName, CORBA::Long theObjectID);
  SALOMEDS::SObject_ptr PasteInto(const SALOMEDS::TMPFile& theStream,
                                  CORBA::Long theObjectID,
                                  SALOMEDS::SObject_ptr theObject);

  /*! \brief Adds theObject in the study with a name = theName, if
   *         theFather is not null the object is placed under theFather
   */
  SALOMEDS::SObject_ptr AddInStudy (SALOMEDS::Study_ptr       theStudy,
                                    GEOM::GEOM_BaseObject_ptr theObject,
                                    const char*               theName,
                                    GEOM::GEOM_BaseObject_ptr theFather);

  /*! \brief Publish sub-shapes, standing for arguments and sub-shapes of arguments.
   *         To be used from python scripts out of geompy.addToStudy (non-default usage)
   */
  GEOM::ListOfGO* RestoreSubShapesO (SALOMEDS::Study_ptr     theStudy,
                                     GEOM::GEOM_Object_ptr   theObject,
                                     const GEOM::ListOfGO&   theArgs,
                                     GEOM::find_shape_method theFindMethod,
                                     CORBA::Boolean          theInheritFirstArg,
                                     CORBA::Boolean          theAddPrefix);

  /*! \brief Publish sub-shapes, standing for given in \a theArgs arguments and sub-shapes.
   *         To be used from python scripts, generated by Dump Python.
   */
  GEOM::ListOfGO* RestoreGivenSubShapesO (SALOMEDS::Study_ptr     theStudy,
                                          GEOM::GEOM_Object_ptr   theObject,
                                          const GEOM::ListOfGO&   theArgs,
                                          GEOM::find_shape_method theFindMethod,
                                          CORBA::Boolean          theInheritFirstArg,
                                          CORBA::Boolean          theAddPrefix);

  /*! \brief Publish sub-shapes, standing for arguments and sub-shapes of arguments.
   *         To be used from GUI and from geompy.addToStudy
   */
  GEOM::ListOfGO* RestoreSubShapesSO (SALOMEDS::Study_ptr     theStudy,
                                      SALOMEDS::SObject_ptr   theSObject,
                                      const GEOM::ListOfGO&   theArgs,
                                      GEOM::find_shape_method theFindMethod,
                                      CORBA::Boolean          theInheritFirstArg,
                                      CORBA::Boolean          theAddPrefix);

  //Collects dependencies of the given objects from other ones
  SALOMEDS::TMPFile* GetDependencyTree(SALOMEDS::Study_ptr theStudy,
                                       const GEOM::string_array& theObjectEntries);

  //-----------------------------------------------------------------------//
  // Transaction methods                                                   //
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

  //Returns a pointer to FiedlOperations interface
  virtual GEOM::GEOM_IFieldOperations_ptr GetIFieldOperations (CORBA::Long theStudyID)
    throw (SALOME::SALOME_Exception);

  //Returns a pointer to corresponding plugin operations interface
  virtual GEOM::GEOM_IOperations_ptr GetPluginOperations (CORBA::Long theStudyID,
                                                          const char* theLibName)
    throw (SALOME::SALOME_Exception);

  //Adds a new sub-shape
  virtual GEOM::GEOM_Object_ptr AddSubShape (GEOM::GEOM_Object_ptr theMainShape,
                                             const GEOM::ListOfLong& theIndices);

  virtual void RemoveObject(GEOM::GEOM_BaseObject_ptr theObject);

  virtual  char* GetStringFromIOR(GEOM::GEOM_Object_ptr theObject);

  virtual GEOM::GEOM_Object_ptr GetIORFromString(const char* stringIOR);

  virtual Engines::TMPFile* DumpPython(CORBA::Object_ptr theStudy,
                                       CORBA::Boolean isPublished,
                                       CORBA::Boolean isMultiFile,
                                       CORBA::Boolean& isValidScript);

  char* GetDumpName (const char* theStudyEntry);

  GEOM::string_array* GetAllDumpNames();

  // Object information
  virtual bool hasObjectInfo();
  virtual char* getObjectInfo(CORBA::Long studyId, const char* entry);

  // Version information
  virtual char* getVersion();

  // Create a new folder object
  SALOMEDS::SObject_ptr CreateFolder(const char* theName,
                                     SALOMEDS::SObject_ptr theFather);

  // Move GEOM object to the specified folder
  void MoveToFolder(GEOM::GEOM_Object_ptr theObject,
                    SALOMEDS::SObject_ptr theFolder);

  // Move list of GEOM objects to the specified folder
  void MoveListToFolder (const GEOM::ListOfGO& theListOfGO,
                         SALOMEDS::SObject_ptr theFolder);

  // Move objects to the specified position
  void Move( const GEOM::object_list& what,
             SALOMEDS::SObject_ptr where,
             CORBA::Long row );

  /*! \brief Fills 3 lists that is used to clean study of redundant objects.
   *         To be used from GUI.
   */
  void GetEntriesToReduceStudy(SALOMEDS::Study_ptr theStudy,
                               GEOM::string_array& theSelectedEntries,
                               GEOM::string_array& theParentEntries,
                               GEOM::string_array& theSubEntries,
                               GEOM::string_array& theOtherEntries);

  //-----------------------------------------------------------------------//
  // Internal methods                                                      //
  //-----------------------------------------------------------------------//

  virtual GEOM::GEOM_BaseObject_ptr GetObject(CORBA::Long theStudyID, const char* theEntry);

 private:
  GEOM::ListOfGO* RestoreSubShapes (SALOMEDS::Study_ptr     theStudy,
                                    GEOM::GEOM_Object_ptr   theObject,
                                    SALOMEDS::SObject_ptr   theSObject,
                                    const GEOM::ListOfGO&   theArgs,
                                    GEOM::find_shape_method theFindMethod,
                                    CORBA::Boolean          theInheritFirstArg,
                                    CORBA::Boolean          theAddPrefix);

  GEOM::ListOfGO* RestoreSubShapesOneLevel (SALOMEDS::Study_ptr     theStudy,
                                            SALOMEDS::SObject_ptr   theOldSO,
                                            SALOMEDS::SObject_ptr   theNewSO,
                                            GEOM::GEOM_Object_ptr   theNewO,
                                            GEOM::ListOfGO&         theOutArgs,
                                            GEOM::find_shape_method theFindMethod,
                                            CORBA::Boolean          theAddPrefix);

  GEOM::ListOfGO* RestoreGivenSubShapes (SALOMEDS::Study_ptr     theStudy,
                                         GEOM::GEOM_Object_ptr   theObject,
                                         SALOMEDS::SObject_ptr   theSObject,
                                         const GEOM::ListOfGO&   theArgs,
                                         GEOM::find_shape_method theFindMethod,
                                         CORBA::Boolean          theInheritFirstArg,
                                         CORBA::Boolean          theAddPrefix);

  GEOM::ListOfGO* RestoreGivenSubShapesOneLevel (SALOMEDS::Study_ptr     theStudy,
                                                 SALOMEDS::SObject_ptr   theOldSO,
                                                 SALOMEDS::SObject_ptr   theNewSO,
                                                 GEOM::GEOM_Object_ptr   theNewO,
                                                 std::set<std::string>   theArgs,
                                                 GEOM::find_shape_method theFindMethod,
                                                 CORBA::Boolean          theAddPrefix);

  // auxilary for PublishNamedShapesInStudy
  void CreateAndPublishGroup(SALOMEDS::Study_ptr theStudy,
                             GEOM::GEOM_Object_var theMainShape,
                             const TopTools_IndexedMapOfShape& anIndices,
                             const TopTools_SequenceOfShape& SeqS,
                             const TColStd_SequenceOfAsciiString& SeqN,
                             const Standard_CString& GrName,
                             GEOM::ListOfGO_var aResList);

  void getUpwardDependency( GEOM::GEOM_BaseObject_ptr gbo,
                            GEOMUtils::LevelsList &upLevelList,
                            std::map< std::string, std::set<std::string> > &passedEntries,
                            int level = 0 );

  void getDownwardDependency( GEOM::GEOM_BaseObject_ptr gbo,
                              GEOMUtils::LevelsList &downLevelList,
                              std::map< std::string, std::set<std::string> > &passedEntries,
                              int level = 0 );

  void includeParentDependencies(GEOM::GEOM_BaseObject_ptr gbo,
                                 std::set<std::string>& aSelected,
                                 std::set<std::string>& aParents,
                                 std::set<std::string>& aChildren,
                                 std::set<std::string>& anOthers);

  void includeSubObjects(SALOMEDS::Study_ptr theStudy,
                         const std::string& aSelectedEntry,
                         std::set<std::string>& aSelected,
                         std::set<std::string>& aParents,
                         std::set<std::string>& aChildren,
                         std::set<std::string>& anOthers);

  void LoadPlugin(const std::string& theLibName);

 private:

   ::GEOMImpl_Gen* _impl;
   SALOME_NamingService * name_service;
   char * _name;

   // plugin operations managing
   std::map<std::string, GEOM_GenericOperationsCreator*> myOpCreatorMap;
};

#endif
