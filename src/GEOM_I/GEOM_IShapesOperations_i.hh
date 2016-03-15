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

#ifndef _GEOM_IShapesOperations_i_HeaderFile
#define _GEOM_IShapesOperations_i_HeaderFile

#include "GEOM_GEOM_I.hxx"
#include "GEOMImpl_Gen.hxx"

#include <SALOMEconfig.h>

#include CORBA_SERVER_HEADER(GEOM_Gen)
#include "GEOM_IOperations_i.hh"
#include "GEOM_Object_i.hh"

#include "GEOMImpl_IShapesOperations.hxx"

class GEOM_I_EXPORT GEOM_IShapesOperations_i :
    public virtual POA_GEOM::GEOM_IShapesOperations,
    public virtual GEOM_IOperations_i
{
 public:
  GEOM_IShapesOperations_i (PortableServer::POA_ptr       thePOA,
                            GEOM::GEOM_Gen_ptr            theEngine,
                            ::GEOMImpl_IShapesOperations* theImpl);
  ~GEOM_IShapesOperations_i();

  GEOM::GEOM_Object_ptr MakeEdge (GEOM::GEOM_Object_ptr thePnt1,
                                  GEOM::GEOM_Object_ptr thePnt2);

  GEOM::GEOM_Object_ptr MakeEdgeOnCurveByLength (GEOM::GEOM_Object_ptr theCurve,
                                                 CORBA::Double         theLength,
                                                 GEOM::GEOM_Object_ptr theStartPoint);

  GEOM::GEOM_Object_ptr MakeEdgeWire (GEOM::GEOM_Object_ptr theWire,
				      const CORBA::Double theLinearTolerance,
				      const CORBA::Double theAngularTolerance);

  GEOM::GEOM_Object_ptr MakeWire (const GEOM::ListOfGO& theEdgesAndWires,
                                  const CORBA::Double   theTolerance);

  GEOM::GEOM_Object_ptr MakeFace (GEOM::GEOM_Object_ptr theWire,
                                  CORBA::Boolean  isPlanarWanted);

  GEOM::GEOM_Object_ptr MakeFaceWires (const GEOM::ListOfGO& theWires,
                                       CORBA::Boolean  isPlanarWanted);

  GEOM::GEOM_Object_ptr MakeFaceFromSurface(GEOM::GEOM_Object_ptr theFace,
                                            GEOM::GEOM_Object_ptr theWire);

  GEOM::GEOM_Object_ptr MakeFaceWithConstraints (const GEOM::ListOfGO& theConstraints);
  
  GEOM::GEOM_Object_ptr MakeShell (const GEOM::ListOfGO& theFacesAndShells);

  GEOM::GEOM_Object_ptr MakeSolidShell (GEOM::GEOM_Object_ptr theShell);

  GEOM::GEOM_Object_ptr MakeSolidShells (const GEOM::ListOfGO& theShells);

  GEOM::GEOM_Object_ptr MakeCompound (const GEOM::ListOfGO& theShapes);

  GEOM::GEOM_Object_ptr MakeSolidFromConnectedFaces (const GEOM::ListOfGO& theFacesOrShells,
                                                     CORBA::Boolean isIntersect);

  GEOM::GEOM_Object_ptr MakeGlueFaces (const GEOM::ListOfGO& theShape,
                                       CORBA::Double         theTolerance,
                                       CORBA::Boolean        doKeepNonSolids);

  GEOM::ListOfGO* GetGlueFaces (const GEOM::ListOfGO& theShape,
                                CORBA::Double         theTolerance);

  GEOM::GEOM_Object_ptr MakeGlueFacesByList (const GEOM::ListOfGO& theShape,
                                             CORBA::Double         theTolerance,
                                             const GEOM::ListOfGO& theFaces,
                                             CORBA::Boolean doKeepNonSolids,
                                             CORBA::Boolean doGlueAllEdges);

  GEOM::GEOM_Object_ptr MakeGlueEdges (const GEOM::ListOfGO& theShape,
                                       CORBA::Double         theTolerance);

  GEOM::ListOfGO* GetGlueEdges (const GEOM::ListOfGO& theShape,
                                CORBA::Double         theTolerance);

  GEOM::GEOM_Object_ptr MakeGlueEdgesByList (const GEOM::ListOfGO& theShape,
                                             CORBA::Double         theTolerance,
                                             const GEOM::ListOfGO& theEdges);

  GEOM::ListOfGO* GetExistingSubObjects (GEOM::GEOM_Object_ptr theShape,
                                         CORBA::Boolean        theGroupsOnly);

  // For old SubShapeAll()
  // Deprecated, use MakeAllSubShapes() instead
  GEOM::ListOfGO* MakeExplode (GEOM::GEOM_Object_ptr theShape,
                               CORBA::Long           theShapeType,
                               CORBA::Boolean        isSorted);

  GEOM::ListOfGO* MakeAllSubShapes (GEOM::GEOM_Object_ptr theShape,
                                    CORBA::Long           theShapeType,
                                    CORBA::Boolean        isSorted);

  GEOM::ListOfGO* ExtractSubShapes (GEOM::GEOM_Object_ptr theShape,
                                    CORBA::Long           theShapeType,
                                    CORBA::Boolean        isSorted);

  // Deprecated, use GetAllSubShapesIDs() instead
  GEOM::ListOfLong* SubShapeAllIDs (GEOM::GEOM_Object_ptr theShape,
                                    CORBA::Long           theShapeType,
                                    CORBA::Boolean        isSorted);

  GEOM::ListOfLong* GetAllSubShapesIDs (GEOM::GEOM_Object_ptr theShape,
                                        CORBA::Long           theShapeType,
                                        CORBA::Boolean        isSorted);

  GEOM::GEOM_Object_ptr GetSubShape (GEOM::GEOM_Object_ptr theMainShape,
                                     CORBA::Long           theID);

  GEOM::ListOfGO* MakeSubShapes (GEOM::GEOM_Object_ptr theMainShape,
                                 const GEOM::ListOfLong& theIndices);

  CORBA::Long GetSubShapeIndex (GEOM::GEOM_Object_ptr theMainShape,
                                GEOM::GEOM_Object_ptr theSubShape);
  
  GEOM::ListOfLong* GetSubShapesIndices (GEOM::GEOM_Object_ptr theMainShape,
                                         const GEOM::ListOfGO& theSubShapes);

  CORBA::Long GetTopologyIndex (GEOM::GEOM_Object_ptr theMainShape,
                                GEOM::GEOM_Object_ptr theSubShape);

  char* GetShapeTypeString (GEOM::GEOM_Object_ptr theShape);

  CORBA::Boolean IsSubShapeBelongsTo( GEOM::GEOM_Object_ptr theSubobject,
                                      const CORBA::Long theSubObjectIndex,
                                      GEOM::GEOM_Object_ptr theObject,
                                      const CORBA::Long theObjectIndex );

  CORBA::Long NumberOfFaces (GEOM::GEOM_Object_ptr theShape);
  CORBA::Long NumberOfEdges (GEOM::GEOM_Object_ptr theShape);
  CORBA::Long NumberOfSubShapes (GEOM::GEOM_Object_ptr theShape,
                                 const CORBA::Long     theShapeType);

  GEOM::GEOM_Object_ptr ChangeOrientation (GEOM::GEOM_Object_ptr theShape);

  GEOM::ListOfLong* GetFreeFacesIDs (GEOM::GEOM_Object_ptr theShape);

  GEOM::ListOfGO* GetSharedShapes (GEOM::GEOM_Object_ptr theShape1,
                                   GEOM::GEOM_Object_ptr theShape2,
                                   CORBA::Long           theShapeType);

  GEOM::ListOfGO* GetSharedShapesMulti (const GEOM::ListOfGO& theShapes,
                                        CORBA::Long           theShapeType,
                                        CORBA::Boolean        theMultiShare);

  GEOM::ListOfGO* GetShapesOnPlane (GEOM::GEOM_Object_ptr theShape,
                                    CORBA::Long           theShapeType,
                                    GEOM::GEOM_Object_ptr theAx1,
                                    GEOM::shape_state     theState);

  GEOM::ListOfGO* GetShapesOnPlaneWithLocation(GEOM::GEOM_Object_ptr theShape,
                                               CORBA::Long           theShapeType,
                                               GEOM::GEOM_Object_ptr theAx1,
                                               GEOM::GEOM_Object_ptr thePnt,
                                               GEOM::shape_state     theState);

  GEOM::ListOfGO* GetShapesOnCylinder (GEOM::GEOM_Object_ptr theShape,
                                       CORBA::Long           theShapeType,
                                       GEOM::GEOM_Object_ptr theAxis,
                                       CORBA::Double         theRadius,
                                       GEOM::shape_state     theState);

  GEOM::ListOfGO* GetShapesOnCylinderWithLocation (GEOM::GEOM_Object_ptr theShape,
                                                   CORBA::Long           theShapeType,
                                                   GEOM::GEOM_Object_ptr theAxis,
                                                   GEOM::GEOM_Object_ptr thePnt,
                                                   CORBA::Double         theRadius,
                                                   GEOM::shape_state     theState);

  GEOM::ListOfGO* GetShapesOnSphere (GEOM::GEOM_Object_ptr theShape,
                                     CORBA::Long           theShapeType,
                                     GEOM::GEOM_Object_ptr theCenter,
                                     CORBA::Double         theRadius,
                                     GEOM::shape_state     theState);

  GEOM::ListOfGO* GetShapesOnQuadrangle (GEOM::GEOM_Object_ptr theShape,
                                         CORBA::Long           theShapeType,
                                         GEOM::GEOM_Object_ptr theTopLeftPoint,
                                         GEOM::GEOM_Object_ptr theTopRigthPoint,
                                         GEOM::GEOM_Object_ptr theBottomLeftPoint,
                                         GEOM::GEOM_Object_ptr theBottomRigthPoint,
                                         GEOM::shape_state     theState);

  GEOM::ListOfLong* GetShapesOnPlaneIDs (GEOM::GEOM_Object_ptr theShape,
                                         CORBA::Long           theShapeType,
                                         GEOM::GEOM_Object_ptr theAx1,
                                         GEOM::shape_state     theState);

  GEOM::ListOfLong* GetShapesOnPlaneWithLocationIDs (GEOM::GEOM_Object_ptr theShape,
                                                     CORBA::Long           theShapeType,
                                                     GEOM::GEOM_Object_ptr theAx1,
                                                     GEOM::GEOM_Object_ptr thePnt,
                                                     GEOM::shape_state     theState);

  GEOM::ListOfLong* GetShapesOnCylinderIDs (GEOM::GEOM_Object_ptr theShape,
                                            CORBA::Long           theShapeType,
                                            GEOM::GEOM_Object_ptr theAxis,
                                            CORBA::Double         theRadius,
                                            GEOM::shape_state     theState);

  GEOM::ListOfLong* GetShapesOnCylinderWithLocationIDs (GEOM::GEOM_Object_ptr theShape,
                                                        CORBA::Long           theShapeType,
                                                        GEOM::GEOM_Object_ptr theAxis,
                                                        GEOM::GEOM_Object_ptr thePnt,
                                                        CORBA::Double         theRadius,
                                                        GEOM::shape_state     theState);

  GEOM::ListOfLong* GetShapesOnSphereIDs (GEOM::GEOM_Object_ptr theShape,
                                          CORBA::Long           theShapeType,
                                          GEOM::GEOM_Object_ptr theCenter,
                                          CORBA::Double         theRadius,
                                          GEOM::shape_state     theState);

  GEOM::ListOfLong* GetShapesOnQuadrangleIDs (GEOM::GEOM_Object_ptr theShape,
                                              CORBA::Long           theShapeType,
                                              GEOM::GEOM_Object_ptr theTopLeftPoint,
                                              GEOM::GEOM_Object_ptr theTopRigthPoint,
                                              GEOM::GEOM_Object_ptr theBottomLeftPoint,
                                              GEOM::GEOM_Object_ptr theBottomRigthPoint,
                                              GEOM::shape_state     theState);

  GEOM::ListOfGO* GetShapesOnBox (GEOM::GEOM_Object_ptr theBox,
                                  GEOM::GEOM_Object_ptr theShape,
                                  CORBA::Long           theShapeType,
                                  GEOM::shape_state     theState);

  GEOM::ListOfLong* GetShapesOnBoxIDs (GEOM::GEOM_Object_ptr theBox,
                                       GEOM::GEOM_Object_ptr theShape,
                                       CORBA::Long           theShapeType,
                                       GEOM::shape_state     theState);

  GEOM::ListOfGO* GetShapesOnShape (GEOM::GEOM_Object_ptr theSheckShape,
                                    GEOM::GEOM_Object_ptr theShape,
                                    CORBA::Short          theShapeType,
                                    GEOM::shape_state     theState);

  GEOM::GEOM_Object_ptr GetShapesOnShapeAsCompound
                                   (GEOM::GEOM_Object_ptr theSheckShape,
                                    GEOM::GEOM_Object_ptr theShape,
                                    CORBA::Short          theShapeType,
                                    GEOM::shape_state     theState);

  GEOM::ListOfLong* GetShapesOnShapeIDs (GEOM::GEOM_Object_ptr theCheckShape,
                                         GEOM::GEOM_Object_ptr theShape,
                                         CORBA::Short          theShapeType,
                                         GEOM::shape_state     theState);

  GEOM::GEOM_Object_ptr GetInPlace (GEOM::GEOM_Object_ptr theShapeWhere,
                                    GEOM::GEOM_Object_ptr theShapeWhat);

  GEOM::GEOM_Object_ptr GetInPlaceOld (GEOM::GEOM_Object_ptr theShapeWhere,
                                       GEOM::GEOM_Object_ptr theShapeWhat);

  GEOM::GEOM_Object_ptr GetInPlaceByHistory (GEOM::GEOM_Object_ptr theShapeWhere,
                                             GEOM::GEOM_Object_ptr theShapeWhat);

  GEOM::GEOM_Object_ptr GetSame (GEOM::GEOM_Object_ptr theShapeWhere,
                                 GEOM::GEOM_Object_ptr theShapeWhat);

  GEOM::ListOfLong* GetSameIDs  (GEOM::GEOM_Object_ptr theShapeWhere,
                                 GEOM::GEOM_Object_ptr theShapeWhat);

  GEOM::GEOM_Object_ptr ExtendEdge(GEOM::GEOM_Object_ptr theEdge,
                                   CORBA::Double         theMin,
                                   CORBA::Double         theMax);

  GEOM::GEOM_Object_ptr ExtendFace(GEOM::GEOM_Object_ptr theFace,
                                   CORBA::Double         theUMin,
                                   CORBA::Double         theUMax,
                                   CORBA::Double         theVMin,
                                   CORBA::Double         theVMax);

  GEOM::GEOM_Object_ptr MakeSurfaceFromFace(GEOM::GEOM_Object_ptr theFace);

  GEOM::ListOfGO* GetSubShapeEdgeSorted (GEOM::GEOM_Object_ptr theShape,
                                         GEOM::GEOM_Object_ptr theStartPoint);

  GEOM::ListOfGO* GetSubShapesWithTolerance
                     (GEOM::GEOM_Object_ptr      theShape,
                      CORBA::Short               theShapeType,
                      GEOM::comparison_condition theCondition,
                      CORBA::Double              theTolerance);

  GEOM::GEOM_Object_ptr MakeExtraction
         (GEOM::GEOM_Object_ptr                              theShape,
          const GEOM::ListOfLong                            &theSubShapeIDs,
          GEOM::GEOM_IShapesOperations::ExtractionStats_out  theStats);

  ::GEOMImpl_IShapesOperations* GetOperations()
  { return (::GEOMImpl_IShapesOperations*)GetImpl(); }
};

#endif
