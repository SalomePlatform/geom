#ifndef __GEOM_SUPERV_I_H__
#define __GEOM_SUPERV_I_H__

// IDL headers
#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(GEOM_Superv)

#include "GEOM_Gen_i.hh"

#include "QAD_Study.h"
#include "QAD_Application.h"
#include "QAD_Desktop.h"

class GEOM_Superv_i : public virtual POA_GEOM::GEOM_Superv,
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

  //-----------------------------------------------------------------------//
  // Set current stydy ID                                                  //
  //-----------------------------------------------------------------------//
  void SetStudyID( CORBA::Long theId );      

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
  GEOM::GEOM_Object_ptr MakeVectorDXDYDZ (CORBA::Double theDX,
					  CORBA::Double theDY,
					  CORBA::Double theDZ);
  GEOM::GEOM_Object_ptr MakeVectorTwoPnt (GEOM::GEOM_Object_ptr thePnt1,
					  GEOM::GEOM_Object_ptr thePnt2);
  GEOM::GEOM_Object_ptr MakeLineTwoPnt (GEOM::GEOM_Object_ptr thePnt1,
					GEOM::GEOM_Object_ptr thePnt2);
  GEOM::GEOM_Object_ptr MakePlaneThreePnt (GEOM::GEOM_Object_ptr thePnt1,
					   GEOM::GEOM_Object_ptr thePnt2,
					   GEOM::GEOM_Object_ptr thePnt3,
					   CORBA::Double theTrimSize);
  GEOM::GEOM_Object_ptr MakePlanePntVec (GEOM::GEOM_Object_ptr thePnt,
					 GEOM::GEOM_Object_ptr theVec,
					 CORBA::Double theTrimSize);
  GEOM::GEOM_Object_ptr MakePlaneFace (GEOM::GEOM_Object_ptr theFace,
				       CORBA::Double theTrimSize);
  GEOM::GEOM_Object_ptr MakeMarker (CORBA::Double theOX , CORBA::Double theOY , CORBA::Double theOZ,
				    CORBA::Double theXDX, CORBA::Double theXDY, CORBA::Double theXDZ,
				    CORBA::Double theYDX, CORBA::Double theYDY, CORBA::Double theYDZ);

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
  GEOM::GEOM_Object_ptr MakeCylinderPntVecRH (GEOM::GEOM_Object_ptr thePnt,
					      GEOM::GEOM_Object_ptr theAxis,
					      CORBA::Double theRadius,
					      CORBA::Double theHeight);
  GEOM::GEOM_Object_ptr MakeCylinderRH (CORBA::Double theR, 
					CORBA::Double theH);
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
  GEOM::GEOM_Object_ptr MakePrismTwoPnt (GEOM::GEOM_Object_ptr theBase,
					 GEOM::GEOM_Object_ptr thePoint1,
					 GEOM::GEOM_Object_ptr thePoint2);
  GEOM::GEOM_Object_ptr MakePipe (GEOM::GEOM_Object_ptr theBase, 
				  GEOM::GEOM_Object_ptr thePath);
  GEOM::GEOM_Object_ptr MakeRevolutionAxisAngle (GEOM::GEOM_Object_ptr theBase,
						 GEOM::GEOM_Object_ptr theAxis,
						 CORBA::Double theAngle);
  GEOM::GEOM_Object_ptr MakeFilling (GEOM::GEOM_Object_ptr theShape,
				     CORBA::Long theMinDeg, CORBA::Long theMaxDeg,
				     CORBA::Double theTol2D, CORBA::Double theTol3D,
				     CORBA::Long theNbIter);
  
  //-----------------------------------------------------------//
  // BooleanOperations                                         //
  //-----------------------------------------------------------//
  GEOM::GEOM_Object_ptr MakeBoolean (GEOM::GEOM_Object_ptr theShape1,
				     GEOM::GEOM_Object_ptr theShape2,
				     CORBA::Long theOperation);
  GEOM::GEOM_Object_ptr MakeFuse (GEOM::GEOM_Object_ptr theShape1,
				  GEOM::GEOM_Object_ptr theShape2);
  GEOM::GEOM_Object_ptr MakePartition (const GEOM::ListOfGO&   theShapes,
				       const GEOM::ListOfGO&   theTools,
				       const GEOM::ListOfGO&   theKeepInside,
				       const GEOM::ListOfGO&   theRemoveInside,
				       const CORBA::Short      theLimit,
				       const CORBA::Boolean    theRemoveWebs,
				       const GEOM::ListOfLong& theMaterials);
  GEOM::GEOM_Object_ptr MakeHalfPartition (GEOM::GEOM_Object_ptr theShape,
					   GEOM::GEOM_Object_ptr thePlane);

  //-----------------------------------------------------------//
  // InsertOperations                                          //
  //-----------------------------------------------------------//
  GEOM::GEOM_Object_ptr MakeCopy (GEOM::GEOM_Object_ptr theOriginal);
  void Export (GEOM::GEOM_Object_ptr theObject, 
	       const char*           theFileName, 
	       const char*           theFormatName);
  GEOM::GEOM_Object_ptr Import (const char* theFileName, 
				const char* theFormatName);
  void ImportTranslators (GEOM::string_array_out theFormats,
			  GEOM::string_array_out thePatterns);
  void ExportTranslators (GEOM::string_array_out theFormats,
			  GEOM::string_array_out thePatterns);

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
  GEOM::GEOM_Object_ptr PositionShape (GEOM::GEOM_Object_ptr theObject,
				       GEOM::GEOM_Object_ptr theStartLCS,
				       GEOM::GEOM_Object_ptr theEndLCS);
  GEOM::GEOM_Object_ptr PositionShapeCopy (GEOM::GEOM_Object_ptr theObject,
					   GEOM::GEOM_Object_ptr theStartLCS,
					   GEOM::GEOM_Object_ptr theEndLCS);

  //-----------------------------------------------------------//
  // ShapesOperations                                          //
  //-----------------------------------------------------------//
  GEOM::GEOM_Object_ptr MakeEdge (GEOM::GEOM_Object_ptr thePnt1,
				  GEOM::GEOM_Object_ptr thePnt2);
  GEOM::GEOM_Object_ptr MakeWire (const GEOM::ListOfGO& theEdgesAndWires);
  GEOM::GEOM_Object_ptr MakeFace (GEOM::GEOM_Object_ptr theWire,
				  CORBA::Boolean isPlanarWanted);
  GEOM::GEOM_Object_ptr MakeFaceWires (const GEOM::ListOfGO& theWires,
				       CORBA::Boolean isPlanarWanted);
  GEOM::GEOM_Object_ptr MakeShell (const GEOM::ListOfGO& theFacesAndShells);
  GEOM::GEOM_Object_ptr MakeSolidShell (GEOM::GEOM_Object_ptr theShell);
  GEOM::GEOM_Object_ptr MakeSolidShells (const GEOM::ListOfGO& theShells);
  GEOM::GEOM_Object_ptr MakeCompound (const GEOM::ListOfGO& theShapes);
  GEOM::GEOM_Object_ptr MakeGlueFaces (GEOM::GEOM_Object_ptr theShape,
				       const CORBA::Double   theTolerance);
  GEOM::ListOfGO* MakeExplode (GEOM::GEOM_Object_ptr theShape,
			       const CORBA::Long theShapeType,
			       const CORBA::Boolean isSorted);
  CORBA::Long NumberOfFaces (GEOM::GEOM_Object_ptr theShape);
  CORBA::Long NumberOfEdges (GEOM::GEOM_Object_ptr theShape);
  GEOM::GEOM_Object_ptr ChangeOrientation (GEOM::GEOM_Object_ptr theShape);

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
				  const CORBA::Double   theX,
				  const CORBA::Double   theY,
				  const CORBA::Double   theZ,
				  const CORBA::Double   theEpsilon);
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
				     const CORBA::Long     theMinNbFaces,
				     const CORBA::Long     theMaxNbFaces,
				     CORBA::Long&          theNbBlocks);
  CORBA::Boolean CheckCompoundOfBlocks (GEOM::GEOM_Object_ptr theCompound,
					GEOM::GEOM_IBlocksOperations::BCErrors_out theErrors);
  char* PrintBCErrors (GEOM::GEOM_Object_ptr theCompound,
		       const GEOM::GEOM_IBlocksOperations::BCErrors& theErrors);
  GEOM::ListOfGO* ExplodeCompoundOfBlocks (GEOM::GEOM_Object_ptr theCompound,
					   const CORBA::Long     theMinNbFaces,
					   const CORBA::Long     theMaxNbFaces);
  GEOM::GEOM_Object_ptr GetBlockNearPoint (GEOM::GEOM_Object_ptr theCompound,
					   GEOM::GEOM_Object_ptr thePoint);
  GEOM::GEOM_Object_ptr GetBlockByParts (GEOM::GEOM_Object_ptr theCompound,
					 const GEOM::ListOfGO& theParts);
  GEOM::ListOfGO* GetBlocksByParts (GEOM::GEOM_Object_ptr theCompound,
				    const GEOM::ListOfGO& theParts);
  GEOM::GEOM_Object_ptr MakeMultiTransformation1D (GEOM::GEOM_Object_ptr theBlock,
						   const CORBA::Long     theDirFace1,
						   const CORBA::Long     theDirFace2,
						   const CORBA::Long     theNbTimes);
  GEOM::GEOM_Object_ptr MakeMultiTransformation2D (GEOM::GEOM_Object_ptr theBlock,
						   const CORBA::Long     theDirFace1U,
						   const CORBA::Long     theDirFace2U,
						   const CORBA::Long     theNbTimesU,
						   const CORBA::Long     theDirFace1V,
						   const CORBA::Long     theDirFace2V,
						   const CORBA::Long     theNbTimesV);

  //-----------------------------------------------------------//
  // CurvesOperations                                          //
  //-----------------------------------------------------------//
  GEOM::GEOM_Object_ptr MakeCirclePntVecR (GEOM::GEOM_Object_ptr theCenter,
					   GEOM::GEOM_Object_ptr theVector,
					   CORBA::Double theR);
  GEOM::GEOM_Object_ptr MakeCircleThreePnt (GEOM::GEOM_Object_ptr thePnt1,
					    GEOM::GEOM_Object_ptr thePnt2,
					    GEOM::GEOM_Object_ptr thePnt3);
  GEOM::GEOM_Object_ptr MakeEllipse (GEOM::GEOM_Object_ptr theCenter,
				     GEOM::GEOM_Object_ptr theVector,
				     CORBA::Double theRMajor, CORBA::Double theRMinor);
  GEOM::GEOM_Object_ptr MakeArc (GEOM::GEOM_Object_ptr thePnt1,
				 GEOM::GEOM_Object_ptr thePnt2,
				 GEOM::GEOM_Object_ptr thePnt3);
  GEOM::GEOM_Object_ptr MakePolyline (const GEOM::ListOfGO& thePoints);
  GEOM::GEOM_Object_ptr MakeSplineBezier (const GEOM::ListOfGO& thePoints);
  GEOM::GEOM_Object_ptr MakeSplineInterpolation (const GEOM::ListOfGO& thePoints);
  GEOM::GEOM_Object_ptr MakeSketcher (const char* theCommand, 
				      const GEOM::ListOfDouble& theWorkingPlane);

  //-----------------------------------------------------------//
  // LocalOperations                                           //
  //-----------------------------------------------------------//
  GEOM::GEOM_Object_ptr MakeFilletAll (GEOM::GEOM_Object_ptr theShape,
				       CORBA::Double theR);
  GEOM::GEOM_Object_ptr MakeFilletEdges (GEOM::GEOM_Object_ptr theShape, CORBA::Double theR,
					 const GEOM::ListOfLong& theEdges);
  GEOM::GEOM_Object_ptr MakeFilletFaces (GEOM::GEOM_Object_ptr theShape, CORBA::Double theR,
					 const GEOM::ListOfLong& theFaces);
  GEOM::GEOM_Object_ptr MakeChamferAll (GEOM::GEOM_Object_ptr theShape, CORBA::Double theD);
  GEOM::GEOM_Object_ptr MakeChamferEdge (GEOM::GEOM_Object_ptr theShape,
					 CORBA::Double theD1, CORBA::Double theD2,
					 CORBA::Long theFace1, CORBA::Long theFace2);
  GEOM::GEOM_Object_ptr MakeChamferFaces (GEOM::GEOM_Object_ptr theShape,
					  CORBA::Double theD1, CORBA::Double theD2,
					  const GEOM::ListOfLong& theFaces);
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
  GEOM::ListOfLong* GetObjects (GEOM::GEOM_Object_ptr theGroup);
  
private:
  SALOME_NamingService *  name_service; 
  GEOM::GEOM_Gen_ptr      myGeomEngine;
  CORBA::Long             myStudyID;
  
  GEOM::GEOM_IBasicOperations_ptr     myBasicOp;
  GEOM::GEOM_I3DPrimOperations_ptr    my3DPrimOp;
  GEOM::GEOM_IBooleanOperations_ptr   myBoolOp;
  GEOM::GEOM_IInsertOperations_ptr    myInsOp;
  GEOM::GEOM_ITransformOperations_ptr myTransfOp;
  GEOM::GEOM_IShapesOperations_ptr    myShapesOp;
  GEOM::GEOM_IBlocksOperations_ptr    myBlocksOp;
  GEOM::GEOM_ICurvesOperations_ptr    myCurvesOp;
  GEOM::GEOM_ILocalOperations_ptr     myLocalOp;
  GEOM::GEOM_IGroupOperations_ptr     myGroupOp;

};

#endif 
