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
//  File   : GEOM_GEN_i.h file
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$ 

#ifndef __GEOM_GEN_I_H__
#define __GEOM_GEN_I_H__

// standard C++ headers
#include <TDocStd_Document.hxx>
#include "GEOMDS_DataMapOfIntegerTransient.hxx"
#include "GEOMDS_Application.hxx"
#include <TopTools_MapOfShape.hxx>
#include <TopTools_SequenceOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Face.hxx>

// IDL headers
#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(GEOM_Gen)
#include CORBA_SERVER_HEADER(GEOM_Shape)
#include CORBA_SERVER_HEADER(SALOMEDS)

#include "SALOME_Component_i.hxx"
#include "GEOM_Shape_i.hh"

#include "SALOME_NamingService.hxx"
#include <iostream.h>

#include <Standard_ErrorHandler.hxx> // CAREFUL ! position of this file is critic : see Lucien PIGNOLONI / OCC


//=====================================================================
// GEOM_Gen_i : class definition
//=====================================================================
class GEOM_Gen_i: public POA_GEOM::GEOM_Gen,
  public Engines_Component_i
{
 private:

  SALOME_NamingService * name_service;
  char * _name;
  Handle(GEOMDS_Application)        myOCAFApp;          /* geom/OCAF Application             */
  Handle(TDocStd_Document)          myCurrentOCAFDoc;   /* Current geom/OCAF Document        */
  GEOMDS_DataMapOfIntegerTransient  myStudyIDToDoc;     /* Map to bind a Study to a Document */

  int myStudyID;

  GEOM::GEOM_Shape_ptr CreateObject(TopoDS_Shape& tds) ;

  GEOM::GEOM_Shape_ptr CreateSubObject(const TopoDS_Shape& SubShape,
				       const GEOM::GEOM_Shape_ptr MainShape,
				       const GEOM::GEOM_Shape::ListOfSubShapeID& ListOfID);
  // Create and insert(!) SubShape of MainShape

  GEOM::GEOM_Shape_ptr GEOM_Gen_i::SubShapesOne( GEOM::GEOM_Shape_ptr shape, 
						const TopAbs_ShapeEnum ShapeType, 
						const GEOM::GEOM_Shape::ListOfSubShapeID& ListOfID,
						const Standard_Boolean sorted=Standard_False)
    throw (SALOME::SALOME_Exception);
  // return all listed subshapes as one shape
  
  GEOM::GEOM_Gen::ListOfGeomShapes* SubShapesAll(GEOM::GEOM_Shape_ptr shape,
						 const TopAbs_ShapeEnum type,
						 const Standard_Boolean sorted=Standard_False)
    throw (SALOME::SALOME_Exception);
  // return all subshapes by type

  TopoDS_Face FindSameFace(const TopoDS_Shape& aShape, 
			   const TopoDS_Face& F,
			   double tol3d);
  TopoDS_Edge FindSameEdge(const TopoDS_Face& newFace, 
			   TopoDS_Edge& Eold,
			   double tol3d);

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
  // Studies Management                                                    //
  //-----------------------------------------------------------------------//
  void GetCurrentStudy (CORBA::Long StudyID);

  CORBA::Short NbLabels();

  // inherited methods from SALOMEDS::Driver
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

  //-----------------------------------------------------------------------//
  // Shapes Management                                                     //
  //-----------------------------------------------------------------------//
  const char* InsertInLabel(TopoDS_Shape S,
			    const char *nameIor,
			    Handle(TDocStd_Document) OCAFDoc) ;

  const char* InsertInLabelDependentShape(TopoDS_Shape S,
					  const char *nameIor,
					  GEOM::GEOM_Shape_ptr mainshape_ptr,
                                          Handle(TDocStd_Document) OCAFDoc) ;
  
  void InsertInLabelOneArgument(TopoDS_Shape main_topo,
				GEOM::GEOM_Shape_ptr shape_ptr,
				TopoDS_Shape result_topo,				
				GEOM::GEOM_Shape_ptr result,	
				Handle(TDocStd_Document) OCAFDoc) ;

  void InsertInLabelMoreArguments(TopoDS_Shape main_topo,
				  GEOM::GEOM_Shape_ptr result,
				  const GEOM::GEOM_Gen::ListOfIOR& ListShapes,				
				  Handle(TDocStd_Document) OCAFDoc) ;

  // Methods used by SuppressFaces
  int SuppressFacesGlue( const TopoDS_Shape& S,
			const TopTools_MapOfShape& mapFaces,
			TopoDS_Shape& aCompoundOfShells ) throw (SALOME::SALOME_Exception) ;

  // various services
  int GetIndexTopology( const TopoDS_Shape& subshape,
		       const TopoDS_Shape& mainShape ) ;

  bool GetShapeFromIndex( const TopoDS_Shape& aShape,
			 const TopAbs_ShapeEnum aType, 
			 const int index,
			 TopoDS_Shape& tds) ; 

  GEOM::GEOM_Shape::ListOfSubShapeID* IndexOfFacesOfSubShell( const TopoDS_Shape& S,
							     const TopoDS_Shape subShell )
    throw (SALOME::SALOME_Exception) ;

  bool ListOfIDIntoMapOfShapes( const TopoDS_Shape& S,
			       const GEOM::GEOM_Shape::ListOfSubShapeID& L, 
			       const int subShapeType,
			       TopTools_MapOfShape& aMap ) ;

  bool ListOfIDIntoSequenceOfShapes( const TopoDS_Shape& S,
				    const GEOM::GEOM_Shape::ListOfSubShapeID& L,
				    const int subShapeType,
				    TopTools_SequenceOfShape& aSequenceOfShapes ) ;

  // Returns a  TopoDS_Shape from a GEOM::GEOM_Shape_ptr
  TopoDS_Shape GetTopoShape(GEOM::GEOM_Shape_ptr shape_ptr) ;

  // Define a sequence of shapes from 'listShapes' and return its length
  int SequenceOfShapeFromListOfGeomShape( const GEOM::GEOM_Gen::ListOfGeomShapes& listShapes,
					 TopTools_SequenceOfShape& SS ) ;

  // Get a string representing a shape ref IOR
  const char* GetStringFromIOR(GEOM::GEOM_Shape_var shapeIOR);

  // Return the shape ref represented by a string of IOR
  GEOM::GEOM_Shape_ptr GetIORFromString(const char* stringIOR);

  GEOM::GEOM_Gen::ListOfIOR* GetReferencedObjects(GEOM::GEOM_Shape_ptr shape);
  GEOM::GEOM_Gen::ListOfIOR* GetObjects(GEOM::GEOM_Shape_ptr shape);


  //-----------------------------------------------------------------------//
  // Internal structure of shapes                                          //
  //-----------------------------------------------------------------------//
  // Explode SubShape
  GEOM::GEOM_Shape_ptr SubShape(GEOM::GEOM_Shape_ptr shape,
				CORBA::Short ShapeType, 
				const GEOM::GEOM_Shape::ListOfSubShapeID& ListOfID)
    throw (SALOME::SALOME_Exception) ;

  // Explode SubShape in predictable order
  GEOM::GEOM_Shape_ptr SubShapeSorted(GEOM::GEOM_Shape_ptr shape,
				      CORBA::Short ShapeType, 
				      const GEOM::GEOM_Shape::ListOfSubShapeID& ListOfID)
    throw (SALOME::SALOME_Exception) ;
  
  // Explode SubShape
  GEOM::GEOM_Gen::ListOfGeomShapes*  SubShapeAll(GEOM::GEOM_Shape_ptr shape,
						 CORBA::Short ShapeType)
    throw (SALOME::SALOME_Exception) ;
  
  // Explode SubShape in predictable order for TUI or GUI
  GEOM::GEOM_Gen::ListOfGeomShapes*  SubShapeAllSorted(GEOM::GEOM_Shape_ptr shape,
						       CORBA::Short ShapeType)
    throw (SALOME::SALOME_Exception) ;

  // Suppress faces in a shape 
  GEOM::GEOM_Gen::ListOfGeomShapes* SuppressFaces( GEOM::GEOM_Shape_ptr shape,
						  const GEOM::GEOM_Shape::ListOfSubShapeID& ListOfID )
    throw (SALOME::SALOME_Exception) ;

  // Suppress one or more holes in a face or shell independant topology
  GEOM::GEOM_Shape_ptr SuppressHolesInFaceOrShell( GEOM::GEOM_Shape_ptr shapeFaceShell,
						  const GEOM::GEOM_Shape::ListOfSubShapeID& ListOfIdWires)
    throw (SALOME::SALOME_Exception) ;

  // Suppress a single hole in a topology (face) shell or solid with/without hole traversing
  GEOM::GEOM_Shape_ptr SuppressHole( GEOM::GEOM_Shape_ptr shape,
				    const GEOM::GEOM_Shape::ListOfSubShapeID& ListIdFace,
				    const GEOM::GEOM_Shape::ListOfSubShapeID& ListIdWire,
				    const GEOM::GEOM_Shape::ListOfSubShapeID& ListIdEndFace )
    throw (SALOME::SALOME_Exception) ;

  bool RebuildFaceRemovingHoles( const TopoDS_Face& aFace,
				const TopTools_MapOfShape& mapHoles,
				TopoDS_Shape& resultFace ) ;

  void SuppressHoleSubRoutine( const TopoDS_Shape& mainShape,
			       const TopoDS_Face& aFace,
			       const TopTools_SequenceOfShape& SSedgesOfWire,
			       const TopTools_IndexedDataMapOfShapeListOfShape& aMapEdgesFaces,
			       const TopTools_MapOfShape& MSfaces,
			       TopTools_MapOfShape& MSfacesSuppress,
			       const Standard_Boolean withEndFace,
			       const TopoDS_Face& endFace,
			       TopTools_MapOfShape& MSwireEndEdges )
    throw (SALOME::SALOME_Exception ) ;  
  
  bool BuildShapeHoleNotTraversing( const TopoDS_Shape& aShape,
				   const TopoDS_Face& aFace,
				   const TopoDS_Wire& aWire,
				   const TopTools_MapOfShape& MFSuppress,
				   TopoDS_Shape& resultTds )
    throw (SALOME::SALOME_Exception) ;

  bool BuildShapeHoleTraversing( const TopoDS_Shape& aShape,
				const TopoDS_Face& aFace,
				const TopoDS_Wire& aWire,
                                const TopTools_MapOfShape& MFSuppress,
				 const TopoDS_Face& endFace,
				const TopoDS_Wire& endWire,
				TopoDS_Shape& resultTds )
    throw (SALOME::SALOME_Exception) ;

  bool FindCompareWireHoleOnFace( const TopoDS_Face& F,
				 const TopTools_MapOfShape& MSwireEdges,
				 TopoDS_Wire& aFoundWire ) ;

  bool IsShapeInSequence( const TopTools_SequenceOfShape& SS, const TopoDS_Shape& aShape ) ;

  void FreeEdgesFromMapOfFace(const TopTools_MapOfShape& MSfaces, TopTools_MapOfShape& MS ) ;

  void MapRemoveSequence( const TopTools_MapOfShape& MS,
			 const TopTools_SequenceOfShape& SSRemove,
			 TopTools_SequenceOfShape& ST) ;

  bool BuildShellWithFaceCompound( const TopoDS_Compound Comp,
				  TopoDS_Shell& resultShell ) ;

  //-----------------------------------------------------------------------//
  // Basic structures                                                      //
  //-----------------------------------------------------------------------//
  GEOM::PointStruct MakePointStruct(CORBA::Double x,
					  CORBA::Double y,
					  CORBA::Double z) ;
  GEOM::DirStruct   MakeDirection(const GEOM::PointStruct& p) ;

  GEOM::AxisStruct  MakeAxisStruct(CORBA::Double x,
					 CORBA::Double y,
					 CORBA::Double z,
					 CORBA::Double vx,
					 CORBA::Double vy,
					 CORBA::Double vz) ;

  //----------------------------------------------------------------------//
  // Boolean Operations                                                   //
  //----------------------------------------------------------------------//
  GEOM::GEOM_Shape_ptr MakeBoolean(GEOM::GEOM_Shape_ptr shape1,
				   GEOM::GEOM_Shape_ptr shape2,
				   CORBA::Long operation)
    throw (SALOME::SALOME_Exception) ;
  GEOM::GEOM_Shape_ptr MakeFuse(GEOM::GEOM_Shape_ptr shape1,
				GEOM::GEOM_Shape_ptr shape2)
    throw (SALOME::SALOME_Exception) ;

  //----------------------------------------------------------------------//
  // Advanced Operations                                                  //
  //----------------------------------------------------------------------//
  GEOM::GEOM_Shape_ptr Partition(const GEOM::GEOM_Gen::ListOfIOR& ListShapes, 
				 const GEOM::GEOM_Gen::ListOfIOR& ListTools,
                                 const GEOM::GEOM_Gen::ListOfIOR& ListKeepInside,
                                 const GEOM::GEOM_Gen::ListOfIOR& ListRemoveInside,
                                 const CORBA::Short               Limit)
    throw (SALOME::SALOME_Exception) ;

  // Filling a surface with section curves
  GEOM::GEOM_Shape_ptr MakeFilling(GEOM::GEOM_Shape_ptr shape,
                                   CORBA::Short mindeg,
				   CORBA::Short maxdeg,
				   CORBA::Double tol3d,
				   CORBA::Double tol2d,
				   CORBA::Short nbiter)
    throw (SALOME::SALOME_Exception) ;
  // Sewing of shapes
  GEOM::GEOM_Shape_ptr MakeSewing(const GEOM::GEOM_Gen::ListOfIOR& ListShapes,
				  CORBA::Double precision)
    throw (SALOME::SALOME_Exception) ;

  GEOM::GEOM_Shape_ptr MakeSewingShape( GEOM::GEOM_Shape_ptr aShape,
				       CORBA::Double precision )
    throw (SALOME::SALOME_Exception);

  GEOM::GEOM_Shape_ptr MakeGlueFaces(GEOM::GEOM_Shape_ptr myShape,
				     double tol3d)
    throw (SALOME::SALOME_Exception);

  // Change the orientation of a (new) shape
  GEOM::GEOM_Shape_ptr OrientationChange(GEOM::GEOM_Shape_ptr shape)
    throw (SALOME::SALOME_Exception) ;

  GEOM::GEOM_Shape_ptr MakePlacedBox(CORBA::Double x1,  CORBA::Double y1,  CORBA::Double z1,
				     CORBA::Double delta1, CORBA::Double delta2, CORBA::Double delta3)
    throw (SALOME::SALOME_Exception) ;

  GEOM::GEOM_Shape_ptr MakePanel(GEOM::GEOM_Shape_ptr shape,
				 CORBA::Short directiontype,
				 CORBA::Double delta)
    throw (SALOME::SALOME_Exception) ;

  //---------------------------------------------------------------------//
  // Transformations Operations                                          //
  //---------------------------------------------------------------------//
  // Copy 
  GEOM::GEOM_Shape_ptr MakeCopy( GEOM::GEOM_Shape_ptr shape)
    throw (SALOME::SALOME_Exception) ;

  // Translation
  GEOM::GEOM_Shape_ptr MakeTranslation( GEOM::GEOM_Shape_ptr shape,
				       CORBA::Double x,
				       CORBA::Double y,
				       CORBA::Double z)
    throw (SALOME::SALOME_Exception) ;
  // Rotation
  GEOM::GEOM_Shape_ptr MakeRotation( GEOM::GEOM_Shape_ptr shape,
				    const GEOM::AxisStruct& axis,
				    CORBA::Double angle)
    throw (SALOME::SALOME_Exception) ;
  // Create a shape using a scale factor
  GEOM::GEOM_Shape_ptr MakeScaleTransform(GEOM::GEOM_Shape_ptr shape,
					  const GEOM::PointStruct& theCenterOfScale,
					  CORBA::Double factor)
    throw (SALOME::SALOME_Exception) ;
  // Mirror of a shape by a  plane
  GEOM::GEOM_Shape_ptr MakeMirrorByPlane(GEOM::GEOM_Shape_ptr shape,
					 GEOM::GEOM_Shape_ptr shapePlane)
    throw (SALOME::SALOME_Exception) ;  

  // Shape by revolution of another around an axis
  GEOM::GEOM_Shape_ptr MakeRevolution(GEOM::GEOM_Shape_ptr shape,
				      const GEOM::AxisStruct& axis,
				      CORBA::Double angle)
    throw (SALOME::SALOME_Exception) ;

  // Create a prism with a base shape along a vector P1 to P2 
  GEOM::GEOM_Shape_ptr MakePrism(GEOM::GEOM_Shape_ptr baseShape,
				 const GEOM::PointStruct& P1,
				 const GEOM::PointStruct& P2)
    throw (SALOME::SALOME_Exception) ;
  // Create a shape by sweeping a baseShape along a pathShape
  GEOM::GEOM_Shape_ptr MakePipe(GEOM::GEOM_Shape_ptr pathShape,
				GEOM::GEOM_Shape_ptr baseShape)
    throw (SALOME::SALOME_Exception) ;

  //---------------------------------------------------------------------//
  // Patterns Operations                                                 //
  //---------------------------------------------------------------------//

  // Multi Translation 1D
  GEOM::GEOM_Shape_ptr MakeMultiTranslation1D( GEOM::GEOM_Shape_ptr shape,
					      const GEOM::DirStruct& dir,
					      CORBA::Double step,
					      CORBA::Short nbtimes)
    throw (SALOME::SALOME_Exception) ;

  // Multi Translation 2D
  GEOM::GEOM_Shape_ptr MakeMultiTranslation2D( GEOM::GEOM_Shape_ptr shape,
					      const GEOM::DirStruct& dir1,
					      CORBA::Double step1,
					      CORBA::Short nbtimes1,
					      const GEOM::DirStruct& dir2,
					      CORBA::Double step2,
					      CORBA::Short nbtimes2)
    throw (SALOME::SALOME_Exception) ;

  // Multi Rotation 1D
  GEOM::GEOM_Shape_ptr MakeMultiRotation1D( GEOM::GEOM_Shape_ptr shape,
					   const GEOM::DirStruct& dir,
					   const GEOM::PointStruct& loc,
					   CORBA::Short nbtimes)
    throw (SALOME::SALOME_Exception) ;

  // Multi Rotation 2D
  GEOM::GEOM_Shape_ptr MakeMultiRotation2D( GEOM::GEOM_Shape_ptr shape,
					   const GEOM::DirStruct& dir,
					   const GEOM::PointStruct& loc,
					   CORBA::Double ang,
					   CORBA::Short nbtimes1,
					   CORBA::Double step,
					   CORBA::Short nbtimes2)
    throw (SALOME::SALOME_Exception) ;

  //--------------------------------------------------------------------//
  // Primitives Construction                                            //
  //--------------------------------------------------------------------//
  GEOM::GEOM_Shape_ptr MakeBox(CORBA::Double x1,
			       CORBA::Double y1,
			       CORBA::Double z1,
			       CORBA::Double x2,
			       CORBA::Double y2,
			       CORBA::Double z2)
    throw (SALOME::SALOME_Exception) ;
  GEOM::GEOM_Shape_ptr MakeSphere(CORBA::Double x1,
				  CORBA::Double y1,
				  CORBA::Double z1,
				  CORBA::Double radius)
    throw (SALOME::SALOME_Exception) ;
  GEOM::GEOM_Shape_ptr MakeCylinder(const  GEOM::PointStruct& pstruct,
				    const GEOM::DirStruct& dstruct,
				    CORBA::Double radius,
				    CORBA::Double height)
    throw (SALOME::SALOME_Exception) ;
  GEOM::GEOM_Shape_ptr MakeTorus(const GEOM::PointStruct& pstruct,
				 const GEOM::DirStruct& dstruct,
				 CORBA::Double major_radius,
				 CORBA::Double minor_radius)
    throw (SALOME::SALOME_Exception) ;
  GEOM::GEOM_Shape_ptr MakeCone(const GEOM::PointStruct& pstruct,
				const GEOM::DirStruct& dstruct,
				CORBA::Double radius1,
				CORBA::Double radius2,
				CORBA::Double height)
    throw (SALOME::SALOME_Exception) ;

  //-------------------------------------------------------------------//
  // Import/Export                                                     //
  //-------------------------------------------------------------------//
  GEOM::GEOM_Shape_ptr ImportIGES(const char* filename) throw (SALOME::SALOME_Exception) ;
  GEOM::GEOM_Shape_ptr ImportBREP(const char* filename) throw (SALOME::SALOME_Exception) ;
  GEOM::GEOM_Shape_ptr ImportSTEP(const char* filename) throw (SALOME::SALOME_Exception) ;

  void ExportIGES(const char* filename,GEOM::GEOM_Shape_ptr theShape) throw (SALOME::SALOME_Exception) ;
  void ExportBREP(const char* filename,GEOM::GEOM_Shape_ptr theShape) throw (SALOME::SALOME_Exception) ;
  void ExportSTEP(const char* filename,GEOM::GEOM_Shape_ptr theShape) throw (SALOME::SALOME_Exception) ;
  //-------------------------------------------------------------------//
  // Fillet and Chamfer construction                                   //
  //-------------------------------------------------------------------//
  GEOM::GEOM_Shape_ptr MakeFillet (GEOM::GEOM_Shape_ptr shape,
                                   CORBA::Double radius,
			     CORBA::Short ShapeType,
				   const GEOM::GEOM_Shape::ListOfSubShapeID& ListOfID)
    throw (SALOME::SALOME_Exception) ;

  GEOM::GEOM_Shape_ptr MakeChamfer(GEOM::GEOM_Shape_ptr shape,
				   CORBA::Double d1,
				   CORBA::Double d2,
				   CORBA::Short ShapeType,
				   const GEOM::GEOM_Shape::ListOfSubShapeID& ListOfID)
    throw (SALOME::SALOME_Exception) ;

  //-------------------------------------------------------------------//
  // Mesures Construction                                              //
  //-------------------------------------------------------------------//
  GEOM::GEOM_Shape_ptr MakeCDG(GEOM::GEOM_Shape_ptr shape)
    throw (SALOME::SALOME_Exception) ;

  //-------------------------------------------------------------------//
  // Check Shape                                                       //
  //-------------------------------------------------------------------//
  CORBA::Boolean CheckShape(GEOM::GEOM_Shape_ptr shape)
    throw (SALOME::SALOME_Exception) ;

  //-------------------------------------------------------------------//
  // Primitives Construction                                           //
  //-------------------------------------------------------------------//
  GEOM::GEOM_Shape_ptr MakeVertex(CORBA::Double x,
				  CORBA::Double y,
				  CORBA::Double z)
    throw (SALOME::SALOME_Exception) ;
  GEOM::GEOM_Shape_ptr MakeVector(const GEOM::PointStruct& pstruct1,
				  const GEOM::PointStruct& pstruct2)
    throw (SALOME::SALOME_Exception) ;
  GEOM::GEOM_Shape_ptr MakeLine  (const GEOM::PointStruct& pstruct,
				  const GEOM::DirStruct& dstruc)
    throw (SALOME::SALOME_Exception) ;
  GEOM::GEOM_Shape_ptr MakePlane (const GEOM::PointStruct& pstruct,
				  const GEOM::DirStruct& dstruc,
				  CORBA::Double trimsize)
    throw (SALOME::SALOME_Exception) ;
  GEOM::GEOM_Shape_ptr MakeCircle(const  GEOM::PointStruct& pstruct,
				  const GEOM::DirStruct& dstruct,
				  CORBA::Double radius)
    throw (SALOME::SALOME_Exception) ;
  GEOM::GEOM_Shape_ptr MakeEllipse(const GEOM::PointStruct& pstruct,
				   const GEOM::DirStruct& dstruct,
				   CORBA::Double radius_major,
				   CORBA::Double radiusminus )
    throw (SALOME::SALOME_Exception) ;
  GEOM::GEOM_Shape_ptr MakeArc   (const  GEOM::PointStruct& pInit,
				  const  GEOM::PointStruct& pCircle,
				  const  GEOM::PointStruct& pEnd)
    throw (SALOME::SALOME_Exception) ;

  GEOM::GEOM_Shape_ptr MakeCompound (const GEOM::GEOM_Gen::ListOfIOR& ListShapes)
    throw (SALOME::SALOME_Exception) ;
  GEOM::GEOM_Shape_ptr MakeWire     (const GEOM::GEOM_Gen::ListOfIOR& ListShapes)
    throw (SALOME::SALOME_Exception) ;
  GEOM::GEOM_Shape_ptr MakeEdge     (const GEOM::PointStruct& pstruct1,
				     const GEOM::PointStruct& pstruct2)
    throw (SALOME::SALOME_Exception) ;
  GEOM::GEOM_Shape_ptr MakeFace     (GEOM::GEOM_Shape_ptr wire, CORBA::Boolean wantplanarface)
    throw (SALOME::SALOME_Exception) ;
  GEOM::GEOM_Shape_ptr MakeShell    (const GEOM::GEOM_Gen::ListOfIOR& ListShapes)
    throw (SALOME::SALOME_Exception) ;
  GEOM::GEOM_Shape_ptr MakeSolid    (const GEOM::GEOM_Gen::ListOfIOR& ListShapes)
    throw (SALOME::SALOME_Exception) ;


  //-------------------------------------------------------------------//
  // Speciic method Archimede                                          //
  //-------------------------------------------------------------------//
  GEOM::GEOM_Shape_ptr Archimede(GEOM::GEOM_Shape_ptr aShape,
                                 CORBA::Double aWeight,
                                 CORBA::Double aWaterDensity,
                                 CORBA::Double aMeshingDeflection)
    throw (SALOME::SALOME_Exception) ;

};

#endif 
