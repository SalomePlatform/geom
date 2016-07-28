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

#ifndef __GEOM_SUPERV_I_H__
#define __GEOM_SUPERV_I_H__

#include "GEOM_I_Superv.hxx"

// IDL headers
#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(GEOM_Gen)
#include CORBA_CLIENT_HEADER(AdvancedGEOM)
#include CORBA_CLIENT_HEADER(STLPlugin)
#include CORBA_CLIENT_HEADER(BREPPlugin)
#include CORBA_CLIENT_HEADER(STEPPlugin)
#include CORBA_CLIENT_HEADER(IGESPlugin)
#include CORBA_CLIENT_HEADER(XAOPlugin)
#ifdef WITH_VTK
#include CORBA_CLIENT_HEADER(VTKPlugin)
#endif
#include CORBA_SERVER_HEADER(GEOM_Superv)

#include "SALOME_Component_i.hxx"
#include "SALOME_NamingService.hxx"

#include "GEOM_List_i.hh"

class GEOM_I_SUPERV_EXPORT GEOM_Superv_i : public virtual POA_GEOM::GEOM_Superv,
                                           public Engines_Component_i
{
public:
  GEOM_Superv_i(CORBA::ORB_ptr orb,
                PortableServer::POA_ptr poa,
                PortableServer::ObjectId * contId,
                const char *instanceName,
                const char *interfaceName);
  ~GEOM_Superv_i();

  // generic method to be put in a super class
  void register_name(char * name);
  void setGeomEngine();

  void getBasicOp();
  void get3DPrimOp();
  void getBoolOp();
  void getInsOp();
  void getTransfOp();
  void getShapesOp();
  void getBlocksOp();
  void getCurvesOp();
  void getLocalOp();
  void getGroupOp();
  void getAdvancedOp();
  void getSTLPluginOp();
  void getBREPPluginOp();
  void getSTEPPluginOp();
  void getIGESPluginOp();
  void getXAOPluginOp();
#ifdef WITH_VTK
  void getVTKPluginOp();
#endif
  PortableServer::ServantBase_var GetServant(CORBA::Object_ptr       theObject,
                                             PortableServer::POA_ptr thePOA);

  //-----------------------------------------------------------------------//
  // Set current stydy ID                                                  //
  //-----------------------------------------------------------------------//
  void SetStudyID( CORBA::Long theId );

  //-----------------------------------------------------------//
  // Create ListOfGO and add items to it                       //
  //-----------------------------------------------------------//
  GEOM::GEOM_List_ptr CreateListOfGO();
  void AddItemToListOfGO(GEOM::GEOM_List_ptr& theList,
                         GEOM::GEOM_Object_ptr theObject);

  //-----------------------------------------------------------//
  // Create ListOfLong and add items to it                     //
  //-----------------------------------------------------------//
  GEOM::GEOM_List_ptr CreateListOfLong();
  void AddItemToListOfLong(GEOM::GEOM_List_ptr& theList,
                           CORBA::Long theObject);

  //-----------------------------------------------------------//
  // Create ListOfDouble and add items to it                   //
  //-----------------------------------------------------------//
  GEOM::GEOM_List_ptr CreateListOfDouble();
  void AddItemToListOfDouble(GEOM::GEOM_List_ptr& theList,
                             CORBA::Double theObject);

  //-----------------------------------------------------------------------//
  // Inherited methods from SALOMEDS::Driver                               //
  //-----------------------------------------------------------------------//

  SALOMEDS::TMPFile* Save(SALOMEDS::SComponent_ptr theComponent,
                          const char* theURL,
                          CORBA::Boolean isMultiFile);

  SALOMEDS::TMPFile* SaveASCII(SALOMEDS::SComponent_ptr theComponent,
                               const char* theURL,
                               CORBA::Boolean isMultiFile);

  CORBA::Boolean Load(SALOMEDS::SComponent_ptr theComponent,
                      const SALOMEDS::TMPFile& theStream,
                      const char* theURL,
                      CORBA::Boolean isMultiFile);

  CORBA::Boolean LoadASCII(SALOMEDS::SComponent_ptr theComponent,
                           const SALOMEDS::TMPFile& theStream,
                           const char* theURL,
                           CORBA::Boolean isMultiFile);

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

  CORBA::Boolean CanPublishInStudy(CORBA::Object_ptr theIOR);
  SALOMEDS::SObject_ptr PublishInStudy(SALOMEDS::Study_ptr theStudy,
                                       SALOMEDS::SObject_ptr theSObject,
                                       CORBA::Object_ptr theObject,
                                       const char* theName) throw (SALOME::SALOME_Exception) ;

  GEOM::ListOfGO* PublishNamedShapesInStudy(SALOMEDS::Study_ptr theStudy,
                                            //SALOMEDS::SObject_ptr theSObject,
                                            CORBA::Object_ptr theObject);

  CORBA::Boolean CanCopy(SALOMEDS::SObject_ptr theObject);
  SALOMEDS::TMPFile* CopyFrom(SALOMEDS::SObject_ptr theObject, CORBA::Long& theObjectID);
  CORBA::Boolean CanPaste(const char* theComponentName, CORBA::Long theObjectID);
  SALOMEDS::SObject_ptr PasteInto(const SALOMEDS::TMPFile& theStream,
                                  CORBA::Long theObjectID,
                                  SALOMEDS::SObject_ptr theObject);

  //-----------------------------------------------------------//
  // Primitives Construction : BasicOperations                 //
  //-----------------------------------------------------------//
  GEOM::GEOM_Object_ptr MakePointXYZ (CORBA::Double theX,
                                      CORBA::Double theY,
                                      CORBA::Double theZ);
  GEOM::GEOM_Object_ptr MakePointWithReference (GEOM::GEOM_Object_ptr theReference,
                                                CORBA::Double theX,
                                                CORBA::Double theY,
                                                CORBA::Double theZ);
  GEOM::GEOM_Object_ptr MakePointOnCurve (GEOM::GEOM_Object_ptr theRefCurve,
                                          CORBA::Double theParameter);
  GEOM::GEOM_Object_ptr MakePointOnCurveByLength (GEOM::GEOM_Object_ptr theRefCurve,
                                                  CORBA::Double theLength,
                                                  GEOM::GEOM_Object_ptr theStartPoint);
  GEOM::GEOM_Object_ptr MakePointOnCurveByCoord (GEOM::GEOM_Object_ptr theRefCurve,
                                                 CORBA::Double theXParameter,
                                                 CORBA::Double theYParameter,
                                                 CORBA::Double theZParameter);
  GEOM::GEOM_Object_ptr MakePointOnSurface (GEOM::GEOM_Object_ptr theRefSurface,
                                            CORBA::Double theUParameter,
                                            CORBA::Double theVParameter);
  GEOM::GEOM_Object_ptr MakePointOnSurfaceByCoord (GEOM::GEOM_Object_ptr theRefSurface,
                                                   CORBA::Double theXParameter,
                                                   CORBA::Double theYParameter,
                                                   CORBA::Double theZParameter);
  GEOM::GEOM_Object_ptr MakePointOnLinesIntersection (GEOM::GEOM_Object_ptr theRefLine1,
                                                      GEOM::GEOM_Object_ptr theRefLine2);
  GEOM::GEOM_Object_ptr MakeTangentOnCurve (GEOM::GEOM_Object_ptr theRefCurve,
                                             CORBA::Double theParameter);
  GEOM::GEOM_Object_ptr MakeVectorDXDYDZ (CORBA::Double theDX,
                                          CORBA::Double theDY,
                                          CORBA::Double theDZ);
  GEOM::GEOM_Object_ptr MakeVectorTwoPnt (GEOM::GEOM_Object_ptr thePnt1,
                                          GEOM::GEOM_Object_ptr thePnt2);
  GEOM::GEOM_Object_ptr MakeLineTwoPnt (GEOM::GEOM_Object_ptr thePnt1,
                                        GEOM::GEOM_Object_ptr thePnt2);
  GEOM::GEOM_Object_ptr MakeLineTwoFaces (GEOM::GEOM_Object_ptr theFace1,
                                          GEOM::GEOM_Object_ptr theFace2);
  GEOM::GEOM_Object_ptr MakePlaneThreePnt (GEOM::GEOM_Object_ptr thePnt1,
                                           GEOM::GEOM_Object_ptr thePnt2,
                                           GEOM::GEOM_Object_ptr thePnt3,
                                           CORBA::Double theTrimSize);
  GEOM::GEOM_Object_ptr MakePlanePntVec (GEOM::GEOM_Object_ptr thePnt,
                                         GEOM::GEOM_Object_ptr theVec,
                                         CORBA::Double theTrimSize);
  GEOM::GEOM_Object_ptr MakePlaneFace (GEOM::GEOM_Object_ptr theFace,
                                       CORBA::Double theTrimSize);
  GEOM::GEOM_Object_ptr MakePlane2Vec (GEOM::GEOM_Object_ptr theVec1,
                                       GEOM::GEOM_Object_ptr theVec2,
                                       CORBA::Double theTrimSize);
  GEOM::GEOM_Object_ptr MakePlaneLCS (GEOM::GEOM_Object_ptr theLCS,
                                      CORBA::Double theTrimSize,
                                      CORBA::Double theOrientation);
  GEOM::GEOM_Object_ptr MakeMarker (CORBA::Double theOX , CORBA::Double theOY , CORBA::Double theOZ,
                                    CORBA::Double theXDX, CORBA::Double theXDY, CORBA::Double theXDZ,
                                    CORBA::Double theYDX, CORBA::Double theYDY, CORBA::Double theYDZ);

  GEOM::GEOM_Object_ptr MakeMarkerFromShape (GEOM::GEOM_Object_ptr theShape);

  GEOM::GEOM_Object_ptr MakeMarkerPntTwoVec (GEOM::GEOM_Object_ptr theOrigin,
                                             GEOM::GEOM_Object_ptr theXVec,
                                             GEOM::GEOM_Object_ptr theYVec);

  GEOM::GEOM_Object_ptr MakeTangentPlaneOnFace (GEOM::GEOM_Object_ptr theFace,
                                                 CORBA::Double theParameterU,
                                                 CORBA::Double theParameterV,
                                                 CORBA::Double theTrimSize);

  //-----------------------------------------------------------//
  // Primitives Construction : 3DPrimOperations                //
  //-----------------------------------------------------------//
  GEOM::GEOM_Object_ptr MakeBox (CORBA::Double theX1,
                                 CORBA::Double theY1,
                                 CORBA::Double theZ1,
                                 CORBA::Double theX2,
                                 CORBA::Double theY2,
                                 CORBA::Double theZ2);
  GEOM::GEOM_Object_ptr MakeBoxDXDYDZ (CORBA::Double theDX,
                                       CORBA::Double theDY,
                                       CORBA::Double theDZ);
  GEOM::GEOM_Object_ptr MakeBoxTwoPnt (GEOM::GEOM_Object_ptr thePnt1,
                                       GEOM::GEOM_Object_ptr thePnt2);
  GEOM::GEOM_Object_ptr MakeFaceHW (CORBA::Double theH,
                                    CORBA::Double theW,
                                    CORBA::Short  theOrientation);
  GEOM::GEOM_Object_ptr MakeFaceObjHW (GEOM::GEOM_Object_ptr theObj,
                                       CORBA::Double theH,
                                       CORBA::Double theW);
  GEOM::GEOM_Object_ptr MakeDiskPntVecR (GEOM::GEOM_Object_ptr theCenter,
                                         GEOM::GEOM_Object_ptr theVector,
                                         CORBA::Double theR);
  GEOM::GEOM_Object_ptr MakeDiskThreePnt (GEOM::GEOM_Object_ptr thePnt1,
                                          GEOM::GEOM_Object_ptr thePnt2,
                                          GEOM::GEOM_Object_ptr thePnt3);
  GEOM::GEOM_Object_ptr MakeDiskR (CORBA::Double theR,
                                   CORBA::Short  theOrientation);
  GEOM::GEOM_Object_ptr MakeCylinderPntVecRH (GEOM::GEOM_Object_ptr thePnt,
                                              GEOM::GEOM_Object_ptr theAxis,
                                              CORBA::Double theRadius,
                                              CORBA::Double theHeight);
  GEOM::GEOM_Object_ptr MakeCylinderRH (CORBA::Double theR,
                                        CORBA::Double theH);
  GEOM::GEOM_Object_ptr MakeCylinderPntVecRHA (GEOM::GEOM_Object_ptr thePnt,
					       GEOM::GEOM_Object_ptr theAxis,
					       CORBA::Double theRadius,
					       CORBA::Double theHeight,
					       CORBA::Double theAngle);
  GEOM::GEOM_Object_ptr MakeCylinderRHA (CORBA::Double theR,
					 CORBA::Double theH,
					 CORBA::Double theA);					
  GEOM::GEOM_Object_ptr MakeSphere (CORBA::Double theX,
                                    CORBA::Double theY,
                                    CORBA::Double theZ,
                                    CORBA::Double theRadius);
  GEOM::GEOM_Object_ptr MakeSphereR (CORBA::Double theR);
  GEOM::GEOM_Object_ptr MakeSpherePntR (GEOM::GEOM_Object_ptr thePnt,
                                        CORBA::Double theR);
  GEOM::GEOM_Object_ptr MakeTorusPntVecRR (GEOM::GEOM_Object_ptr thePnt,
                                           GEOM::GEOM_Object_ptr theVec,
                                           CORBA::Double theRMajor,
                                           CORBA::Double theRMinor);
  GEOM::GEOM_Object_ptr MakeTorusRR (CORBA::Double theRMajor,
                                     CORBA::Double theRMinor);
  GEOM::GEOM_Object_ptr MakeConePntVecR1R2H (GEOM::GEOM_Object_ptr thePnt,
                                             GEOM::GEOM_Object_ptr theAxis,
                                             CORBA::Double theR1,
                                             CORBA::Double theR2,
                                             CORBA::Double theHeight);
  GEOM::GEOM_Object_ptr MakeConeR1R2H (CORBA::Double theR1,
                                       CORBA::Double theR2,
                                       CORBA::Double theHeight);
  GEOM::GEOM_Object_ptr MakePrismVecH (GEOM::GEOM_Object_ptr theBase,
                                       GEOM::GEOM_Object_ptr theVec,
                                       CORBA::Double         theH);
  GEOM::GEOM_Object_ptr MakePrismVecH2Ways (GEOM::GEOM_Object_ptr theBase,
                                            GEOM::GEOM_Object_ptr theVec,
                                            CORBA::Double         theH);
  GEOM::GEOM_Object_ptr MakePrismTwoPnt (GEOM::GEOM_Object_ptr theBase,
                                         GEOM::GEOM_Object_ptr thePoint1,
                                         GEOM::GEOM_Object_ptr thePoint2);
  GEOM::GEOM_Object_ptr MakePrismTwoPnt2Ways (GEOM::GEOM_Object_ptr theBase,
                                              GEOM::GEOM_Object_ptr thePoint1,
                                              GEOM::GEOM_Object_ptr thePoint2);
  GEOM::GEOM_Object_ptr MakePrismDXDYDZ (GEOM::GEOM_Object_ptr theBase,
                                         CORBA::Double         theDX,
                                         CORBA::Double         theDY,
                                         CORBA::Double         theDZ);
  GEOM::GEOM_Object_ptr MakePrismDXDYDZ2Ways (GEOM::GEOM_Object_ptr theBase,
                                              CORBA::Double         theDX,
                                              CORBA::Double         theDY,
                                              CORBA::Double         theDZ);
  GEOM::GEOM_Object_ptr MakePipe (GEOM::GEOM_Object_ptr theBase,
                                  GEOM::GEOM_Object_ptr thePath);
  GEOM::GEOM_Object_ptr MakeRevolutionAxisAngle (GEOM::GEOM_Object_ptr theBase,
                                                 GEOM::GEOM_Object_ptr theAxis,
                                                 CORBA::Double theAngle);
  GEOM::GEOM_Object_ptr MakeRevolutionAxisAngle2Ways (GEOM::GEOM_Object_ptr theBase,
                                                      GEOM::GEOM_Object_ptr theAxis,
                                                      CORBA::Double theAngle);
  GEOM::GEOM_Object_ptr MakeFilling (GEOM::GEOM_Object_ptr theShape,
                                     CORBA::Long theMinDeg, CORBA::Long theMaxDeg,
                                     CORBA::Double theTol2D, CORBA::Double theTol3D,
                                     CORBA::Long theNbIter,
                                     GEOM::filling_oper_method theMethod,
                                     CORBA::Boolean theApprox);

  GEOM::GEOM_Object_ptr MakeThruSections(const GEOM::ListOfGO& theSeqSections,
                                         CORBA::Boolean theModeSolid,
                                         CORBA::Double thePreci,
                                         CORBA::Boolean theRuled);

  GEOM::GEOM_Object_ptr MakePipeWithDifferentSections(const GEOM::ListOfGO& theBases,
                                                      const GEOM::ListOfGO& theLocations,
                                                      GEOM::GEOM_Object_ptr thePath,
                                                      CORBA::Boolean theWithContact,
                                                      CORBA::Boolean theWithCorrections);

  GEOM::GEOM_Object_ptr MakePipeWithShellSections(const GEOM::ListOfGO& theBases,
                                                  const GEOM::ListOfGO& theSubBases,
                                                  const GEOM::ListOfGO& theLocations,
                                                  GEOM::GEOM_Object_ptr thePath,
                                                  CORBA::Boolean theWithContact,
                                                  CORBA::Boolean theWithCorrections);

  GEOM::GEOM_Object_ptr MakePipeShellsWithoutPath(const GEOM::ListOfGO& theBases,
                                                  const GEOM::ListOfGO& theLocations);

  GEOM::GEOM_Object_ptr MakePipeBiNormalAlongVector (GEOM::GEOM_Object_ptr theBase,
                                                     GEOM::GEOM_Object_ptr thePath,
                                                     GEOM::GEOM_Object_ptr theVec);

  //-----------------------------------------------------------//
  // BooleanOperations                                         //
  //-----------------------------------------------------------//
  GEOM::GEOM_Object_ptr MakeBoolean (GEOM::GEOM_Object_ptr theShape1,
                                     GEOM::GEOM_Object_ptr theShape2,
                                     CORBA::Long           theOperation);
  GEOM::GEOM_Object_ptr MakeFuse (GEOM::GEOM_Object_ptr theShape1,
                                  GEOM::GEOM_Object_ptr theShape2);
  GEOM::GEOM_Object_ptr MakeCommon (GEOM::GEOM_Object_ptr theShape1,
				    GEOM::GEOM_Object_ptr theShape2);
  GEOM::GEOM_Object_ptr MakeCut (GEOM::GEOM_Object_ptr theShape1,
				 GEOM::GEOM_Object_ptr theShape2);
  GEOM::GEOM_Object_ptr MakeSection (GEOM::GEOM_Object_ptr theShape1,
				     GEOM::GEOM_Object_ptr theShape2);
  GEOM::GEOM_Object_ptr MakePartition (GEOM::GEOM_List_ptr   theShapes,
                                       GEOM::GEOM_List_ptr   theTools,
                                       GEOM::GEOM_List_ptr   theKeepInside,
                                       GEOM::GEOM_List_ptr   theRemoveInside,
                                       CORBA::Short      theLimit,
                                       CORBA::Boolean    theRemoveWebs,
                                       GEOM::GEOM_List_ptr theMaterials,
                                       CORBA::Short theKeepNonlimitShapes);
  GEOM::GEOM_Object_ptr MakeHalfPartition (GEOM::GEOM_Object_ptr theShape,
                                           GEOM::GEOM_Object_ptr thePlane);

  //-----------------------------------------------------------//
  // InsertOperations                                          //
  //-----------------------------------------------------------//
  GEOM::GEOM_Object_ptr MakeCopy (GEOM::GEOM_Object_ptr theOriginal);
  void Export (GEOM::GEOM_Object_ptr theObject,
               const char*           theFileName,
               const char*           theFormatName);
  GEOM::GEOM_Object_ptr ImportFile (const char* theFileName,
                                    const char* theFormatName);

  //-----------------------------------------------------------//
  // TransformOperations                                       //
  //-----------------------------------------------------------//
  GEOM::GEOM_Object_ptr TranslateTwoPoints (GEOM::GEOM_Object_ptr theObject,
                                            GEOM::GEOM_Object_ptr thePoint1,
                                            GEOM::GEOM_Object_ptr thePoint2);
  GEOM::GEOM_Object_ptr TranslateTwoPointsCopy (GEOM::GEOM_Object_ptr theObject,
                                                GEOM::GEOM_Object_ptr thePoint1,
                                                GEOM::GEOM_Object_ptr thePoint2);
  GEOM::GEOM_Object_ptr TranslateDXDYDZ (GEOM::GEOM_Object_ptr theObject,
                                         CORBA::Double theDX,
                                         CORBA::Double theDY,
                                         CORBA::Double theDZ);
  GEOM::GEOM_Object_ptr TranslateDXDYDZCopy (GEOM::GEOM_Object_ptr theObject,
                                             CORBA::Double theDX,
                                             CORBA::Double theDY,
                                             CORBA::Double theDZ);
  GEOM::GEOM_Object_ptr TranslateVector (GEOM::GEOM_Object_ptr theObject,
                                         GEOM::GEOM_Object_ptr theVector);
  GEOM::GEOM_Object_ptr TranslateVectorCopy (GEOM::GEOM_Object_ptr theObject,
                                             GEOM::GEOM_Object_ptr theVector);
  GEOM::GEOM_Object_ptr TranslateVectorDistance (GEOM::GEOM_Object_ptr theObject,
                                                 GEOM::GEOM_Object_ptr theVector,
                                                 CORBA::Double theDistance,
                                                 CORBA::Boolean theCopy);
  GEOM::GEOM_Object_ptr MultiTranslate1D (GEOM::GEOM_Object_ptr theObject,
                                          GEOM::GEOM_Object_ptr theVector,
                                          CORBA::Double theStep,
                                          CORBA::Long theNbTimes);
  GEOM::GEOM_Object_ptr MultiTranslate2D (GEOM::GEOM_Object_ptr theObject,
                                          GEOM::GEOM_Object_ptr theVector1,
                                          CORBA::Double theStep1,
                                          CORBA::Long theNbTimes1,
                                          GEOM::GEOM_Object_ptr theVector2,
                                          CORBA::Double theStep2,
                                          CORBA::Long theNbTimes2);
  GEOM::GEOM_Object_ptr Rotate (GEOM::GEOM_Object_ptr theObject,
                                GEOM::GEOM_Object_ptr theAxis,
                                CORBA::Double theAngle);
  GEOM::GEOM_Object_ptr RotateCopy (GEOM::GEOM_Object_ptr theObject,
                                    GEOM::GEOM_Object_ptr theAxis,
                                    CORBA::Double theAngle);

  GEOM::GEOM_Object_ptr RotateThreePoints (GEOM::GEOM_Object_ptr theObject,
                                           GEOM::GEOM_Object_ptr theCentPoint,
                                           GEOM::GEOM_Object_ptr thePoint1,
                                           GEOM::GEOM_Object_ptr thePoint2);

  GEOM::GEOM_Object_ptr RotateThreePointsCopy (GEOM::GEOM_Object_ptr theObject,
                                               GEOM::GEOM_Object_ptr theCentPoint,
                                               GEOM::GEOM_Object_ptr thePoint1,
                                               GEOM::GEOM_Object_ptr thePoint2);

  GEOM::GEOM_Object_ptr MultiRotate1D (GEOM::GEOM_Object_ptr theObject,
                                       GEOM::GEOM_Object_ptr theAxis,
                                       CORBA::Long theNbTimes);
  GEOM::GEOM_Object_ptr MultiRotate2D (GEOM::GEOM_Object_ptr theObject,
                                       GEOM::GEOM_Object_ptr theAxis,
                                       CORBA::Double theAngle,
                                       CORBA::Long theNbTimes1,
                                       CORBA::Double theStep,
                                       CORBA::Long theNbTimes2);
  GEOM::GEOM_Object_ptr MirrorPlane (GEOM::GEOM_Object_ptr theObject,
                                     GEOM::GEOM_Object_ptr thePlane);
  GEOM::GEOM_Object_ptr MirrorPlaneCopy (GEOM::GEOM_Object_ptr theObject,
                                         GEOM::GEOM_Object_ptr thePlane);
  GEOM::GEOM_Object_ptr MirrorAxis (GEOM::GEOM_Object_ptr theObject,
                                    GEOM::GEOM_Object_ptr theAxis);
  GEOM::GEOM_Object_ptr MirrorAxisCopy (GEOM::GEOM_Object_ptr theObject,
                                        GEOM::GEOM_Object_ptr theAxis);
  GEOM::GEOM_Object_ptr MirrorPoint (GEOM::GEOM_Object_ptr theObject,
                                     GEOM::GEOM_Object_ptr thePoint);
  GEOM::GEOM_Object_ptr MirrorPointCopy (GEOM::GEOM_Object_ptr theObject,
                                         GEOM::GEOM_Object_ptr thePoint);
  GEOM::GEOM_Object_ptr OffsetShape (GEOM::GEOM_Object_ptr theObject,
                                     CORBA::Double theOffset);
  GEOM::GEOM_Object_ptr OffsetShapeCopy (GEOM::GEOM_Object_ptr theObject,
                                         CORBA::Double theOffset);
  GEOM::GEOM_Object_ptr ScaleShape (GEOM::GEOM_Object_ptr theObject,
                                    GEOM::GEOM_Object_ptr thePoint,
                                    CORBA::Double theFactor);
  GEOM::GEOM_Object_ptr ScaleShapeCopy (GEOM::GEOM_Object_ptr theObject,
                                        GEOM::GEOM_Object_ptr thePoint,
                                        CORBA::Double theFactor);
  GEOM::GEOM_Object_ptr ScaleShapeAlongAxes (GEOM::GEOM_Object_ptr theObject,
                                             GEOM::GEOM_Object_ptr thePoint,
                                             CORBA::Double theFactorX,
                                             CORBA::Double theFactorY,
                                             CORBA::Double theFactorZ);
  GEOM::GEOM_Object_ptr ScaleShapeAlongAxesCopy (GEOM::GEOM_Object_ptr theObject,
                                                 GEOM::GEOM_Object_ptr thePoint,
                                                 CORBA::Double theFactorX,
                                                 CORBA::Double theFactorY,
                                                 CORBA::Double theFactorZ);
  GEOM::GEOM_Object_ptr PositionShape (GEOM::GEOM_Object_ptr theObject,
                                       GEOM::GEOM_Object_ptr theStartLCS,
                                       GEOM::GEOM_Object_ptr theEndLCS);
  GEOM::GEOM_Object_ptr PositionShapeCopy (GEOM::GEOM_Object_ptr theObject,
                                           GEOM::GEOM_Object_ptr theStartLCS,
                                           GEOM::GEOM_Object_ptr theEndLCS);
  GEOM::GEOM_Object_ptr PositionAlongPath (GEOM::GEOM_Object_ptr theObject,
                                           GEOM::GEOM_Object_ptr thePath,
                                           CORBA::Double         theDistance,
                                           CORBA::Boolean        theCopy,
                                           CORBA::Boolean        theReverse);

  //-----------------------------------------------------------//
  // ShapesOperations                                          //
  //-----------------------------------------------------------//
  GEOM::GEOM_Object_ptr MakeEdge (GEOM::GEOM_Object_ptr thePnt1,
                                  GEOM::GEOM_Object_ptr thePnt2);
  GEOM::GEOM_Object_ptr MakeEdgeOnCurveByLength (GEOM::GEOM_Object_ptr theRefCurve,
                                                 CORBA::Double theLength,
                                                 GEOM::GEOM_Object_ptr theStartPoint);
  GEOM::GEOM_Object_ptr MakeWire (GEOM::GEOM_List_ptr theEdgesAndWires,
                                  CORBA::Double       theTolerance);
  GEOM::GEOM_Object_ptr MakeFace (GEOM::GEOM_Object_ptr theWire,
                                  CORBA::Boolean isPlanarWanted);
  GEOM::GEOM_Object_ptr MakeFaceWires (GEOM::GEOM_List_ptr theWires,
                                       CORBA::Boolean isPlanarWanted);
  GEOM::GEOM_Object_ptr MakeFaceWithConstraints (GEOM::GEOM_List_ptr theConstraints);
  GEOM::GEOM_Object_ptr MakeShell (GEOM::GEOM_List_ptr theFacesAndShells);
  GEOM::GEOM_Object_ptr MakeSolidShell (GEOM::GEOM_Object_ptr theShell);
  GEOM::GEOM_Object_ptr MakeSolidShells (GEOM::GEOM_List_ptr theShells);
  GEOM::GEOM_Object_ptr MakeCompound (GEOM::GEOM_List_ptr theShapes);
  GEOM::GEOM_Object_ptr MakeSolidFromConnectedFaces (GEOM::GEOM_List_ptr theFacesOrShells,
                                                     CORBA::Boolean isIntersect);
  GEOM::GEOM_Object_ptr MakeGlueFaces (GEOM::GEOM_Object_ptr theShape,
                                       CORBA::Double theTolerance,
                                       CORBA::Boolean doKeepNonSolids);
  GEOM::GEOM_List_ptr GetGlueFaces (GEOM::GEOM_Object_ptr theShape,
                                    CORBA::Double theTolerance);
  GEOM::GEOM_Object_ptr MakeGlueFacesByList (GEOM::GEOM_Object_ptr theShape,
                                             CORBA::Double theTolerance,
                                             const GEOM::ListOfGO& theFaces,
                                             CORBA::Boolean doKeepNonSolids,
                                             CORBA::Boolean doGlueAllEdges);
  GEOM::GEOM_List_ptr MakeExplode (GEOM::GEOM_Object_ptr theShape,
                                       CORBA::Long theShapeType,
                                       CORBA::Boolean isSorted);
  CORBA::Long NumberOfFaces (GEOM::GEOM_Object_ptr theShape);
  CORBA::Long NumberOfEdges (GEOM::GEOM_Object_ptr theShape);
  GEOM::GEOM_Object_ptr ChangeOrientation (GEOM::GEOM_Object_ptr theShape);

  GEOM::GEOM_List_ptr GetShapesOnShape (GEOM::GEOM_Object_ptr theCheckShape,
                                          GEOM::GEOM_Object_ptr theShape,
                                          CORBA::Short theShapeType,
                                          GEOM::shape_state theState);
  GEOM::GEOM_Object_ptr GetShapesOnShapeAsCompound
                                       (GEOM::GEOM_Object_ptr theCheckShape,
                                          GEOM::GEOM_Object_ptr theShape,
                                          CORBA::Short theShapeType,
                                          GEOM::shape_state theState);

  //-----------------------------------------------------------//
  // BlocksOperations                                          //
  //-----------------------------------------------------------//
  GEOM::GEOM_Object_ptr MakeQuad4Vertices (GEOM::GEOM_Object_ptr thePnt1,
                                           GEOM::GEOM_Object_ptr thePnt2,
                                           GEOM::GEOM_Object_ptr thePnt3,
                                           GEOM::GEOM_Object_ptr thePnt4);
  GEOM::GEOM_Object_ptr MakeQuad (GEOM::GEOM_Object_ptr theEdge1,
                                  GEOM::GEOM_Object_ptr theEdge2,
                                  GEOM::GEOM_Object_ptr theEdge3,
                                  GEOM::GEOM_Object_ptr theEdge4);
  GEOM::GEOM_Object_ptr MakeQuad2Edges (GEOM::GEOM_Object_ptr theEdge1,
                                        GEOM::GEOM_Object_ptr theEdge2);
  GEOM::GEOM_Object_ptr MakeHexa (GEOM::GEOM_Object_ptr theFace1,
                                  GEOM::GEOM_Object_ptr theFace2,
                                  GEOM::GEOM_Object_ptr theFace3,
                                  GEOM::GEOM_Object_ptr theFace4,
                                  GEOM::GEOM_Object_ptr theFace5,
                                  GEOM::GEOM_Object_ptr theFace6);
  GEOM::GEOM_Object_ptr MakeHexa2Faces (GEOM::GEOM_Object_ptr theFace1,
                                        GEOM::GEOM_Object_ptr theFace2);
  GEOM::GEOM_Object_ptr GetPoint (GEOM::GEOM_Object_ptr theShape,
                                  CORBA::Double   theX,
                                  CORBA::Double   theY,
                                  CORBA::Double   theZ,
                                  CORBA::Double   theEpsilon);
  GEOM::GEOM_Object_ptr GetEdge (GEOM::GEOM_Object_ptr theShape,
                                 GEOM::GEOM_Object_ptr thePoint1,
                                 GEOM::GEOM_Object_ptr thePoint2);
  GEOM::GEOM_Object_ptr GetEdgeNearPoint (GEOM::GEOM_Object_ptr theShape,
                                          GEOM::GEOM_Object_ptr thePoint);
  GEOM::GEOM_Object_ptr GetFaceByPoints (GEOM::GEOM_Object_ptr theShape,
                                         GEOM::GEOM_Object_ptr thePoint1,
                                         GEOM::GEOM_Object_ptr thePoint2,
                                         GEOM::GEOM_Object_ptr thePoint3,
                                         GEOM::GEOM_Object_ptr thePoint4);
  GEOM::GEOM_Object_ptr GetFaceByEdges (GEOM::GEOM_Object_ptr theShape,
                                        GEOM::GEOM_Object_ptr theEdge1,
                                        GEOM::GEOM_Object_ptr theEdge2);
  GEOM::GEOM_Object_ptr GetOppositeFace (GEOM::GEOM_Object_ptr theBlock,
                                         GEOM::GEOM_Object_ptr theFace);
  GEOM::GEOM_Object_ptr GetFaceNearPoint (GEOM::GEOM_Object_ptr theShape,
                                          GEOM::GEOM_Object_ptr thePoint);
  GEOM::GEOM_Object_ptr GetFaceByNormale (GEOM::GEOM_Object_ptr theBlock,
                                          GEOM::GEOM_Object_ptr theVector);
  CORBA::Boolean IsCompoundOfBlocks (GEOM::GEOM_Object_ptr theCompound,
                                     CORBA::Long     theMinNbFaces,
                                     CORBA::Long     theMaxNbFaces,
                                     CORBA::Long&          theNbBlocks);
  CORBA::Boolean CheckCompoundOfBlocks (GEOM::GEOM_Object_ptr theCompound,
                                        GEOM::GEOM_IBlocksOperations::BCErrors_out theErrors);
  char* PrintBCErrors (GEOM::GEOM_Object_ptr theCompound,
                       const GEOM::GEOM_IBlocksOperations::BCErrors& theErrors);
  GEOM::GEOM_List_ptr ExplodeCompoundOfBlocks (GEOM::GEOM_Object_ptr theCompound,
                                                   CORBA::Long     theMinNbFaces,
                                                   CORBA::Long     theMaxNbFaces);
  GEOM::GEOM_Object_ptr GetBlockNearPoint (GEOM::GEOM_Object_ptr theCompound,
                                           GEOM::GEOM_Object_ptr thePoint);
  GEOM::GEOM_Object_ptr GetBlockByParts (GEOM::GEOM_Object_ptr theCompound,
                                         GEOM::GEOM_List_ptr theParts);
  GEOM::GEOM_List_ptr GetBlocksByParts (GEOM::GEOM_Object_ptr theCompound,
                                            GEOM::GEOM_List_ptr theParts);
  GEOM::GEOM_Object_ptr MakeMultiTransformation1D (GEOM::GEOM_Object_ptr theBlock,
                                                   CORBA::Long     theDirFace1,
                                                   CORBA::Long     theDirFace2,
                                                   CORBA::Long     theNbTimes);
  GEOM::GEOM_Object_ptr MakeMultiTransformation2D (GEOM::GEOM_Object_ptr theBlock,
                                                   CORBA::Long     theDirFace1U,
                                                   CORBA::Long     theDirFace2U,
                                                   CORBA::Long     theNbTimesU,
                                                   CORBA::Long     theDirFace1V,
                                                   CORBA::Long     theDirFace2V,
                                                   CORBA::Long     theNbTimesV);

  //-----------------------------------------------------------//
  // CurvesOperations                                          //
  //-----------------------------------------------------------//
  GEOM::GEOM_Object_ptr MakeCirclePntVecR (GEOM::GEOM_Object_ptr theCenter,
                                           GEOM::GEOM_Object_ptr theVector,
                                           CORBA::Double theR);
  GEOM::GEOM_Object_ptr MakeCircleThreePnt (GEOM::GEOM_Object_ptr thePnt1,
                                            GEOM::GEOM_Object_ptr thePnt2,
                                            GEOM::GEOM_Object_ptr thePnt3);
  GEOM::GEOM_Object_ptr MakeCircleCenter2Pnt (GEOM::GEOM_Object_ptr thePnt1,
                                              GEOM::GEOM_Object_ptr thePnt2,
                                              GEOM::GEOM_Object_ptr thePnt3);
  GEOM::GEOM_Object_ptr MakeEllipse (GEOM::GEOM_Object_ptr theCenter,
                                     GEOM::GEOM_Object_ptr theVector,
                                     CORBA::Double theRMajor, CORBA::Double theRMinor);
  GEOM::GEOM_Object_ptr MakeEllipseVec (GEOM::GEOM_Object_ptr theCenter,
                                        GEOM::GEOM_Object_ptr theVector,
                                        CORBA::Double theRMajor, CORBA::Double theRMinor,
                                        GEOM::GEOM_Object_ptr theVectorMajor);
  GEOM::GEOM_Object_ptr MakeArc (GEOM::GEOM_Object_ptr thePnt1,
                                 GEOM::GEOM_Object_ptr thePnt2,
                                 GEOM::GEOM_Object_ptr thePnt3);
  GEOM::GEOM_Object_ptr MakeArcCenter (GEOM::GEOM_Object_ptr theCenter,
                                       GEOM::GEOM_Object_ptr thePnt1,
                                       GEOM::GEOM_Object_ptr thePnt2,
                                       CORBA::Boolean theSense);
  GEOM::GEOM_Object_ptr MakeArcOfEllipse (GEOM::GEOM_Object_ptr thePnt1,
                                          GEOM::GEOM_Object_ptr thePnt2,
                                          GEOM::GEOM_Object_ptr thePnt3);
  GEOM::GEOM_Object_ptr MakePolyline (GEOM::GEOM_List_ptr thePoints,
                                      CORBA::Boolean      theIsClosed);
  GEOM::GEOM_Object_ptr MakeSplineBezier (GEOM::GEOM_List_ptr thePoints,
                                          CORBA::Boolean      theIsClosed);
  GEOM::GEOM_Object_ptr MakeSplineInterpolation (GEOM::GEOM_List_ptr thePoints,
                                                 CORBA::Boolean      theIsClosed,
                                                 CORBA::Boolean      theDoReordering);
  GEOM::GEOM_Object_ptr MakeSketcher (const char* theCommand,
                                                GEOM::GEOM_List_ptr theWorkingPlane);
  GEOM::GEOM_Object_ptr Make3DSketcher (GEOM::GEOM_List_ptr theCoordinates);

  //-----------------------------------------------------------//
  // LocalOperations                                           //
  //-----------------------------------------------------------//
  GEOM::GEOM_Object_ptr MakeFilletAll (GEOM::GEOM_Object_ptr theShape,
                                       CORBA::Double theR);
  GEOM::GEOM_Object_ptr MakeFilletEdges (GEOM::GEOM_Object_ptr theShape, CORBA::Double theR,
                                         GEOM::GEOM_List_ptr theEdges);
  GEOM::GEOM_Object_ptr MakeFilletEdgesR1R2 (GEOM::GEOM_Object_ptr theShape, CORBA::Double theR1,
                                             CORBA::Double theR2, GEOM::GEOM_List_ptr theEdges);
  GEOM::GEOM_Object_ptr MakeFilletFaces (GEOM::GEOM_Object_ptr theShape, CORBA::Double theR,
                                         GEOM::GEOM_List_ptr theFaces);
  GEOM::GEOM_Object_ptr MakeFilletFacesR1R2 (GEOM::GEOM_Object_ptr theShape, CORBA::Double theR1,
                                             CORBA::Double theR2, GEOM::GEOM_List_ptr theFaces);
  GEOM::GEOM_Object_ptr MakeFillet2D (GEOM::GEOM_Object_ptr theShape, CORBA::Double theR,
                                      GEOM::GEOM_List_ptr theVertexes);
  GEOM::GEOM_Object_ptr MakeFillet1D (GEOM::GEOM_Object_ptr theShape, CORBA::Double theR,
                                      GEOM::GEOM_List_ptr theVertexes, CORBA::Boolean doIgnoreSecantVertices);
  GEOM::GEOM_Object_ptr MakeChamferAll (GEOM::GEOM_Object_ptr theShape, CORBA::Double theD);
  GEOM::GEOM_Object_ptr MakeChamferEdge (GEOM::GEOM_Object_ptr theShape,
                                         CORBA::Double theD1, CORBA::Double theD2,
                                         CORBA::Long theFace1, CORBA::Long theFace2);
  GEOM::GEOM_Object_ptr MakeChamferEdgeAD (GEOM::GEOM_Object_ptr theShape,
                                            CORBA::Double theD, CORBA::Double theAngle,
                                           CORBA::Long theFace1, CORBA::Long theFace2);
  GEOM::GEOM_Object_ptr MakeChamferFaces (GEOM::GEOM_Object_ptr theShape,
                                          CORBA::Double theD1, CORBA::Double theD2,
                                          GEOM::GEOM_List_ptr theFaces);
  GEOM::GEOM_Object_ptr MakeChamferFacesAD (GEOM::GEOM_Object_ptr theShape,
                                            CORBA::Double theD, CORBA::Double theAngle,
                                            GEOM::GEOM_List_ptr theFaces);
  GEOM::GEOM_Object_ptr MakeChamferEdges (GEOM::GEOM_Object_ptr theShape,
                                          CORBA::Double theD1, CORBA::Double theD2,
                                          GEOM::GEOM_List_ptr theEdges);
  GEOM::GEOM_Object_ptr MakeChamferEdgesAD (GEOM::GEOM_Object_ptr theShape,
                                            CORBA::Double theD, CORBA::Double theAngle,
                                               GEOM::GEOM_List_ptr theEdges);
  GEOM::GEOM_Object_ptr MakeArchimede (GEOM::GEOM_Object_ptr theShape,
                                       CORBA::Double theWeight,
                                       CORBA::Double theWaterDensity,
                                       CORBA::Double theMeshingDeflection);
  CORBA::Long GetSubShapeIndex (GEOM::GEOM_Object_ptr theShape,
                                GEOM::GEOM_Object_ptr theSubShape);

  //-----------------------------------------------------------//
  // GroupOperations                                           //
  //-----------------------------------------------------------//
  GEOM::GEOM_Object_ptr CreateGroup (GEOM::GEOM_Object_ptr theMainShape,
                                     CORBA::Long theShapeType);
  void AddObject (GEOM::GEOM_Object_ptr theGroup,
                  CORBA::Long theSubShapeId);
  void RemoveObject (GEOM::GEOM_Object_ptr theGroup,
                     CORBA::Long theSubShapeId);
  CORBA::Long GetType (GEOM::GEOM_Object_ptr theGroup);
  GEOM::GEOM_Object_ptr GetMainShape (GEOM::GEOM_Object_ptr theGroup);
  GEOM::GEOM_List_ptr GetObjects (GEOM::GEOM_Object_ptr theGroup);

  //-----------------------------------------------------------//
  // ImportExport Operations                                   //
  //-----------------------------------------------------------//

  void ExportSTL( GEOM::GEOM_Object_ptr theObject,
                  const char*           theFileName,
                  const bool            theIsASCII,
                  CORBA::Double         theDeflection,
                  const bool            theIsRelative );

  GEOM::GEOM_Object_ptr ImportSTL( const char* theFileName );

  void ExportBREP( GEOM::GEOM_Object_ptr theObject,
                   const char*           theFileName );

  GEOM::GEOM_Object_ptr ImportBREP( const char* theFileName );

  void ExportSTEP( GEOM::GEOM_Object_ptr theObject,
                   const char*           theFileName );

  GEOM::GEOM_Object_ptr ImportSTEP( const char* theFileName,
                                    const bool  theIsIgnoreUnits );

  void ExportIGES( GEOM::GEOM_Object_ptr theObject,
                   const char*           theFileName,
                   const char*           theVersion );

  GEOM::GEOM_Object_ptr ImportIGES( const char* theFileName,
                                    const bool  theIsIgnoreUnits );

  CORBA::Boolean ExportXAO( GEOM::GEOM_Object_ptr shape,
                            const GEOM::ListOfGO& groups,
                            const GEOM::ListOfFields& fields,
                            const char* author,
                            const char* fileName);
  CORBA::Boolean ImportXAO( const char* fileName,
                            GEOM::GEOM_Object_out shape,
                            GEOM::ListOfGO_out subShapes,
                            GEOM::ListOfGO_out groups,
                            GEOM::ListOfFields_out fields );

  void ExportVTK( GEOM::GEOM_Object_ptr theObject,
                  const char*           theFileName,
                  CORBA::Double         theDeflection );

  //-----------------------------------------------------------//
  // Advanced Operations                                       //
  //-----------------------------------------------------------//
  GEOM::GEOM_List_ptr MakePipeTShape (CORBA::Double theR1, CORBA::Double theW1, CORBA::Double theL1,
                                      CORBA::Double theR2, CORBA::Double theW2, CORBA::Double theL2,
                                      CORBA::Boolean theHexMesh);
  GEOM::GEOM_List_ptr MakePipeTShapeWithPosition (CORBA::Double theR1, CORBA::Double theW1, CORBA::Double theL1,
                                                  CORBA::Double theR2, CORBA::Double theW2, CORBA::Double theL2,
                                                  CORBA::Boolean theHexMesh,
                                                  GEOM::GEOM_Object_ptr theP1, GEOM::GEOM_Object_ptr theP2, GEOM::GEOM_Object_ptr theP3);
  GEOM::GEOM_List_ptr MakePipeTShapeChamfer (CORBA::Double theR1, CORBA::Double theW1, CORBA::Double theL1,
                                             CORBA::Double theR2, CORBA::Double theW2, CORBA::Double theL2,
                                             CORBA::Double theH, CORBA::Double theW, CORBA::Boolean theHexMesh);
  GEOM::GEOM_List_ptr MakePipeTShapeChamferWithPosition (CORBA::Double theR1, CORBA::Double theW1, CORBA::Double theL1,
                                                         CORBA::Double theR2, CORBA::Double theW2, CORBA::Double theL2,
                                                         CORBA::Double theH, CORBA::Double theW, CORBA::Boolean theHexMesh,
                                                         GEOM::GEOM_Object_ptr theP1, GEOM::GEOM_Object_ptr theP2, GEOM::GEOM_Object_ptr theP3);
  GEOM::GEOM_List_ptr MakePipeTShapeFillet (CORBA::Double theR1, CORBA::Double theW1, CORBA::Double theL1,
                                            CORBA::Double theR2, CORBA::Double theW2, CORBA::Double theL2,
                                            CORBA::Double theRF, CORBA::Boolean theHexMesh);
  GEOM::GEOM_List_ptr MakePipeTShapeFilletWithPosition (CORBA::Double theR1, CORBA::Double theW1, CORBA::Double theL1,
                                                        CORBA::Double theR2, CORBA::Double theW2, CORBA::Double theL2,
                                                        CORBA::Double theRF, CORBA::Boolean theHexMesh,
                                                        GEOM::GEOM_Object_ptr theP1, GEOM::GEOM_Object_ptr theP2, GEOM::GEOM_Object_ptr theP3);
  GEOM::GEOM_Object_ptr MakeDividedDisk (CORBA::Double theR, CORBA::Double theRatio,
                                         CORBA::Short theOrientation, GEOM::pattern thePattern);
  GEOM::GEOM_Object_ptr MakeDividedCylinder (CORBA::Double theR, 
                                             CORBA::Double theH,
                                             GEOM::pattern thePattern);
  GEOM::GEOM_Object_ptr MakeSmoothingSurface (GEOM::GEOM_List_ptr thelPoints);
  /*@@ insert new functions before this line @@ do not remove this line @@*/

private:
  SALOME_NamingService *  name_service;
  GEOM::GEOM_Gen_var      myGeomEngine;
  CORBA::Long             myStudyID;
  CORBA::Long             myLastStudyID; // mkr : PAL10770
  PortableServer::POA_var myPOA;

  GEOM::GEOM_IBasicOperations_var     myBasicOp;
  GEOM::GEOM_I3DPrimOperations_var    my3DPrimOp;
  GEOM::GEOM_IBooleanOperations_var   myBoolOp;
  GEOM::GEOM_IInsertOperations_var    myInsOp;
  GEOM::GEOM_ITransformOperations_var myTransfOp;
  GEOM::GEOM_IShapesOperations_var    myShapesOp;
  GEOM::GEOM_IBlocksOperations_var    myBlocksOp;
  GEOM::GEOM_ICurvesOperations_var    myCurvesOp;
  GEOM::GEOM_ILocalOperations_var     myLocalOp;
  GEOM::GEOM_IGroupOperations_var     myGroupOp;
  GEOM::IAdvancedOperations_var       myAdvancedOp;
  GEOM::ISTLOperations_var            mySTLOp;
  GEOM::IBREPOperations_var           myBREPOp;
  GEOM::ISTEPOperations_var           mySTEPOp;
  GEOM::IIGESOperations_var           myIGESOp;
  GEOM::IXAOOperations_var            myXAOOp;
#ifdef WITH_VTK
  GEOM::IVTKOperations_var            myVTKOp;
#endif
};

#endif
