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

//=============================================================================
// File      : GEOMImpl_IShapesOperations.hxx
// Created   :
// Author    : modified by Lioka RAZAFINDRAZAKA (CEA) 22/06/2007
// Project   : SALOME
//=============================================================================

#ifndef _GEOMImpl_IShapesOperations_HXX_
#define _GEOMImpl_IShapesOperations_HXX_

#include "GEOM_IOperations.hxx"

#include "GEOMAlgo_State.hxx"
#include "GEOMUtils.hxx"

#include <TColStd_HSequenceOfTransient.hxx>
#include <TColStd_HSequenceOfInteger.hxx>
#include <TColStd_HArray1OfInteger.hxx>

#include <Geom_Surface.hxx>

#include <list>

class GEOM_Engine;
class GEOM_Object;
class TopoDS_Shape;

class GEOMImpl_IShapesOperations : public GEOM_IOperations
{
 public:

  enum SubShapeType {
    None      = 0x00,
    Groups    = 0x01,
    Fields    = 0x02,
    SubShapes = 0x04,
    All       = Groups | Fields | SubShapes,
  };

  /**
   * This enumeration represents an extraction statistics type.
   */
  enum ExtractionStatType
  {
    EST_Removed,  ///< Removed sub-shapes
    EST_Modified, ///< Modified sub-shapes
    EST_Added     ///< Newly created sub-shapes
  };

  /*!
   * This structure defines a format of extraction statistics.
   */
  struct ExtractionStat
  {
    ExtractionStatType          type;    ///< Type of extraction statistics.
    std::list<Standard_Integer> indices; ///< Shape indices touched by this type of modification.
  };

  Standard_EXPORT GEOMImpl_IShapesOperations(GEOM_Engine* theEngine, int theDocID);
  Standard_EXPORT ~GEOMImpl_IShapesOperations();

  Standard_EXPORT Handle(GEOM_Object) MakeEdge (Handle(GEOM_Object) thePoint1,
                                                Handle(GEOM_Object) thePoint2);

  Standard_EXPORT Handle(GEOM_Object) MakeEdgeOnCurveByLength (Handle(GEOM_Object) theCurve,
                                                               const Standard_Real theLength,
                                                               Handle(GEOM_Object) theStartPoint);

  Standard_EXPORT Handle(GEOM_Object) MakeEdgeWire (Handle(GEOM_Object) theWire,
                                                    const Standard_Real theLinearTolerance,
                                                    const Standard_Real theAngularTolerance);

  Standard_EXPORT Handle(GEOM_Object) MakeWire (std::list<Handle(GEOM_Object)> theEdgesAndWires,
                                                const Standard_Real            theTolerance);

  Standard_EXPORT Handle(GEOM_Object) MakeFace (Handle(GEOM_Object) theWire, const bool isPlanarWanted);

  Standard_EXPORT Handle(GEOM_Object) MakeFaceWires (std::list<Handle(GEOM_Object)> theWires,
                                                     const bool isPlanarWanted);

  Standard_EXPORT Handle(GEOM_Object) MakeFaceFromSurface
                                              (Handle(GEOM_Object) theFace,
                                               Handle(GEOM_Object) theWire);
                                               
  Standard_EXPORT Handle(GEOM_Object) MakeFaceWithConstraints (std::list<Handle(GEOM_Object)> theConstraints);

  Standard_EXPORT Handle(GEOM_Object) MakeShell (std::list<Handle(GEOM_Object)> theShapes);

  Standard_EXPORT Handle(GEOM_Object) MakeSolidShells (std::list<Handle(GEOM_Object)> theShells);

  Standard_EXPORT Handle(GEOM_Object) MakeCompound (std::list<Handle(GEOM_Object)> theShapes);

  Standard_EXPORT Handle(GEOM_Object) MakeSolidFromConnectedFaces (std::list<Handle(GEOM_Object)> theFacesOrShells,
                                                                   const Standard_Boolean isIntersect);

  Standard_EXPORT Handle(GEOM_Object) MakeGlueFaces (std::list< Handle(GEOM_Object) >& theShapes,
                                                     const Standard_Real theTolerance,
                                                     const Standard_Boolean doKeepNonSolids);

  Standard_EXPORT Handle(GEOM_Object) MakeGlueFacesByList (std::list< Handle(GEOM_Object) >& theShapes,
                                                           const Standard_Real theTolerance,
                                                           std::list<Handle(GEOM_Object)>& theFaces,
                                                           const Standard_Boolean doKeepNonSolids,
                                                           const Standard_Boolean doGlueAllEdges);

  Standard_EXPORT Handle(GEOM_Object) MakeGlueEdges (std::list< Handle(GEOM_Object) >& theShapes,
                                                     const Standard_Real theTolerance);

  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) GetGlueShapes (std::list< Handle(GEOM_Object) >& theShapes,
                                                                      const Standard_Real theTolerance,
                                                                      const TopAbs_ShapeEnum theType);

  Standard_EXPORT Handle(GEOM_Object) MakeGlueEdgesByList (std::list< Handle(GEOM_Object) >& theShapes,
                                                           const Standard_Real theTolerance,
                                                           std::list<Handle(GEOM_Object)> & theEdges);

  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) GetExistingSubObjects(Handle(GEOM_Object)    theShape,
                                                                             const Standard_Boolean theGroupsOnly);
  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) GetExistingSubObjects(Handle(GEOM_Object)    theShape,
                                                                             const Standard_Integer theTypes = All);
  
  enum ExplodeType {
    EXPLODE_OLD_INCLUDE_MAIN,
    EXPLODE_NEW_INCLUDE_MAIN,
    EXPLODE_NEW_EXCLUDE_MAIN
  };

  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) MakeExplode
    (Handle(GEOM_Object)    theShape,
     const Standard_Integer theShapeType,
     const Standard_Boolean isSorted,
     const ExplodeType      theExplodeType = EXPLODE_NEW_INCLUDE_MAIN);

  Standard_EXPORT Handle(TColStd_HSequenceOfInteger) SubShapeAllIDs
    (Handle(GEOM_Object)    theShape,
     const Standard_Integer theShapeType,
     const Standard_Boolean isSorted,
     const ExplodeType      theExplodeType = EXPLODE_NEW_INCLUDE_MAIN);

  Standard_EXPORT Handle(GEOM_Object) GetSubShape (Handle(GEOM_Object)    theMainShape,
                                                   const Standard_Integer theID);

  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) MakeSubShapes
    (Handle(GEOM_Object)              theMainShape,
     Handle(TColStd_HArray1OfInteger) theIndices);

  Standard_EXPORT Standard_Integer GetSubShapeIndex (Handle(GEOM_Object) theMainShape,
                                                     Handle(GEOM_Object) theSubShape);
  
  Standard_EXPORT Handle(TColStd_HSequenceOfInteger) GetSubShapesIndices (Handle(GEOM_Object) theMainShape,
                                                                          std::list<Handle(GEOM_Object)> theSubShapes);

  Standard_EXPORT Standard_Integer GetTopologyIndex (Handle(GEOM_Object) theMainShape,
                                                     Handle(GEOM_Object) theSubShape);

  Standard_EXPORT TCollection_AsciiString GetShapeTypeString (Handle(GEOM_Object) theShape);

  Standard_EXPORT Standard_Boolean IsSubShapeBelongsTo(Handle(GEOM_Object) theSubObject,
                                                       const Standard_Integer theSubObjectIndex,
                                                       Handle(GEOM_Object) theObject,
                                                       const Standard_Integer theObjectIndex);

  Standard_EXPORT Standard_Integer NumberOfSubShapes (Handle(GEOM_Object)    theShape,
                                                      const Standard_Integer theShapeType);

  Standard_EXPORT Handle(GEOM_Object) ReverseShape(Handle(GEOM_Object) theShapes);

  Standard_EXPORT Handle(TColStd_HSequenceOfInteger) GetFreeFacesIDs (Handle(GEOM_Object) theShape);

  Standard_EXPORT Handle(TColStd_HSequenceOfTransient)
    GetSharedShapes (Handle(GEOM_Object)    theShape1,
                     Handle(GEOM_Object)    theShape2,
                     const Standard_Integer theShapeType);

  Standard_EXPORT Handle(TColStd_HSequenceOfTransient)
    GetSharedShapes (std::list<Handle(GEOM_Object)>& theShapes,
                     const Standard_Integer          theShapeType,
                     const bool                      theMultiShare = true);

  Standard_EXPORT Handle(TColStd_HSequenceOfTransient)
    GetShapesOnPlane (const Handle(GEOM_Object)& theShape,
                      const Standard_Integer     theShapeType,
                      const Handle(GEOM_Object)& theAx1,
                      const GEOMAlgo_State       theState);

  Standard_EXPORT Handle(TColStd_HSequenceOfTransient)
    GetShapesOnPlaneWithLocation (const Handle(GEOM_Object)& theShape,
                                  const Standard_Integer     theShapeType,
                                  const Handle(GEOM_Object)& theAx1,
                                  const Handle(GEOM_Object)& thePnt,
                                  const GEOMAlgo_State       theState);


  Standard_EXPORT Handle(TColStd_HSequenceOfTransient)
    GetShapesOnCylinder (const Handle(GEOM_Object)& theShape,
                         const Standard_Integer     theShapeType,
                         const Handle(GEOM_Object)& theAxis,
                         const Standard_Real        theRadius,
                         const GEOMAlgo_State       theState);

  Standard_EXPORT Handle(TColStd_HSequenceOfTransient)
    GetShapesOnCylinderWithLocation (const Handle(GEOM_Object)& theShape,
                                     const Standard_Integer     theShapeType,
                                     const Handle(GEOM_Object)& theAxis,
                                     const Handle(GEOM_Object)& thePnt,
                                     const Standard_Real        theRadius,
                                     const GEOMAlgo_State       theState);

  Standard_EXPORT Handle(TColStd_HSequenceOfTransient)
    GetShapesOnSphere (const Handle(GEOM_Object)& theShape,
                       const Standard_Integer     theShapeType,
                       const Handle(GEOM_Object)& theCenter,
                       const Standard_Real        theRadius,
                       const GEOMAlgo_State       theState);

  Standard_EXPORT Handle(TColStd_HSequenceOfInteger)
    GetShapesOnPlaneIDs (const Handle(GEOM_Object)& theShape,
                         const Standard_Integer     theShapeType,
                         const Handle(GEOM_Object)& theAx1,
                         const GEOMAlgo_State       theState);

  Standard_EXPORT Handle(TColStd_HSequenceOfInteger)
    GetShapesOnPlaneWithLocationIDs (const Handle(GEOM_Object)& theShape,
                                     const Standard_Integer     theShapeType,
                                     const Handle(GEOM_Object)& theAx1,
                                     const Handle(GEOM_Object)& thePnt,
                                     const GEOMAlgo_State       theState);

  Standard_EXPORT Handle(TColStd_HSequenceOfInteger)
    GetShapesOnCylinderIDs (const Handle(GEOM_Object)& theShape,
                            const Standard_Integer     theShapeType,
                            const Handle(GEOM_Object)& theAxis,
                            const Standard_Real        theRadius,
                            const GEOMAlgo_State       theState);

  Standard_EXPORT Handle(TColStd_HSequenceOfInteger)
    GetShapesOnCylinderWithLocationIDs (const Handle(GEOM_Object)& theShape,
                                        const Standard_Integer     theShapeType,
                                        const Handle(GEOM_Object)& theAxis,
                                        const Handle(GEOM_Object)& thePnt,
                                        const Standard_Real        theRadius,
                                        const GEOMAlgo_State       theState);

  Standard_EXPORT Handle(TColStd_HSequenceOfInteger)
    GetShapesOnSphereIDs (const Handle(GEOM_Object)& theShape,
                          const Standard_Integer     theShapeType,
                          const Handle(GEOM_Object)& theCenter,
                          const Standard_Real        theRadius,
                          const GEOMAlgo_State       theState);

  /*!
   * \brief Find sub-shapes complying with given status about quadrangle
    * \param theShape - the shape to explore
    * \param theShapeType - type of sub-shape of theShape
    * \param theTopLeftPoint - top left quadrangle corner
    * \param theTopRigthPoint - top right quadrangle corner
    * \param theBottomLeftPoint - bottom left quadrangle corner
    * \param theBottomRigthPoint - bottom right quadrangle corner
    * \param theState - required state
    * \retval Handle(TColStd_HSequenceOfInteger) - IDs of found sub-shapes
   */
  Standard_EXPORT Handle(TColStd_HSequenceOfTransient)
    GetShapesOnQuadrangle (const Handle(GEOM_Object)& theShape,
                           const Standard_Integer     theShapeType,
                           const Handle(GEOM_Object)& theTopLeftPoint,
                           const Handle(GEOM_Object)& theTopRigthPoint,
                           const Handle(GEOM_Object)& theBottomLeftPoint,
                           const Handle(GEOM_Object)& theBottomRigthPoint,
                           const GEOMAlgo_State       theState);

  /*!
   * \brief Find IDs of sub-shapes complying with given status about quadrangle
    * \param theShape - the shape to explore
    * \param theShapeType - type of sub-shape of theShape
    * \param theTopLeftPoint - top left quadrangle corner
    * \param theTopRigthPoint - top right quadrangle corner
    * \param theBottomLeftPoint - bottom left quadrangle corner
    * \param theBottomRigthPoint - bottom right quadrangle corner
    * \param theState - required state
    * \retval Handle(TColStd_HSequenceOfInteger) - IDs of found sub-shapes
   */
  Standard_EXPORT Handle(TColStd_HSequenceOfInteger)
    GetShapesOnQuadrangleIDs (const Handle(GEOM_Object)& theShape,
                              const Standard_Integer     theShapeType,
                              const Handle(GEOM_Object)& theTopLeftPoint,
                              const Handle(GEOM_Object)& theTopRigthPoint,
                              const Handle(GEOM_Object)& theBottomLeftPoint,
                              const Handle(GEOM_Object)& theBottomRigthPoint,
                              const GEOMAlgo_State       theState);

  Standard_EXPORT Handle(GEOM_Object) GetShapesOnCylinderOld (Handle(GEOM_Object)    theShape,
                                                              const Standard_Integer theShapeType,
                                                              Handle(GEOM_Object)    theAxis,
                                                              const Standard_Real    theRadius);

  Standard_EXPORT Handle(GEOM_Object) GetShapesOnSphereOld (Handle(GEOM_Object)    theShape,
                                                            const Standard_Integer theShapeType,
                                                            Handle(GEOM_Object)    theCenter,
                                                            const Standard_Real    theRadius);

  Standard_EXPORT Handle(GEOM_Object) GetInPlace (Handle(GEOM_Object) theShapeWhere,
                                                  Handle(GEOM_Object) theShapeWhat);

  Standard_EXPORT Handle(GEOM_Object) GetInPlaceOld (Handle(GEOM_Object) theShapeWhere,
                                                     Handle(GEOM_Object) theShapeWhat);

  Standard_EXPORT Handle(GEOM_Object) GetInPlaceByHistory (Handle(GEOM_Object) theShapeWhere,
                                                           Handle(GEOM_Object) theShapeWhat);

  /*!
   * \brief Searches a shape equal to theWhat in the context of theWhere
   * \param theShapeWhere - a context shap
   * \param theShapeWhat - a sample shape
   * \retval Handle(GEOM_Object) - found shape
   */
  Standard_EXPORT Handle(GEOM_Object) GetSame(const Handle(GEOM_Object)& theShapeWhere,
                                              const Handle(GEOM_Object)& theShapeWhat);

  /*!
   * \brief Searches a shape equal to theWhat in the context of theWhere
   * \param theShapeWhere - a context shap
   * \param theShapeWhat - a sample shape
   * \retval Handle(TColStd_HSequenceOfInteger) - IDs of found sub-shapes
   */
  Standard_EXPORT Handle(TColStd_HSequenceOfInteger) GetSameIDs(const Handle(GEOM_Object)& theShapeWhere,
                                                                 const Handle(GEOM_Object)& theShapeWhat);

  /*!
   * \brief Find IDs of sub-shapes complying with given status about surface
    * \param theBox - the box to check state of sub-shapes against
    * \param theShape - the shape to explore
    * \param theShapeType - type of sub-shape of theShape
    * \param theState - required state
    * \retval Handle(TColStd_HSequenceOfInteger) - IDs of found sub-shapes
   */
  Standard_EXPORT Handle(TColStd_HSequenceOfInteger)
                              GetShapesOnBoxIDs(const Handle(GEOM_Object)& theBox,
                                                const Handle(GEOM_Object)& theShape,
                                                const Standard_Integer theShapeType,
                                                GEOMAlgo_State theState);

  /*!
   * \brief Find sub-shapes complying with given status about surface
   * \param theBox - the box to check state of sub-shapes against
   * \param theShape - the shape to explore
   * \param theShapeType - type of sub-shape of theShape
   * \param theState - required state
   * \retval Handle(TColStd_HSequenceOfTransient) - found shape objects
   */
  Standard_EXPORT Handle(TColStd_HSequenceOfTransient)
                             GetShapesOnBox(const Handle(GEOM_Object)& theBox,
                                            const Handle(GEOM_Object)& theShape,
                                            const Standard_Integer theShapeType,
                                            GEOMAlgo_State theState);

  /*!
   * \brief Find IDs of sub-shapes complying with given status about surface
   * \param theCheckShape - the shape to check state of sub-shapes against
   * \param theShape - the shape to explore
   * \param theShapeType - type of sub-shape of theShape
   * \param theState - required state
   * \retval Handle(TColStd_HSequenceOfInteger) - IDs of found sub-shapes
   */
  Standard_EXPORT Handle(TColStd_HSequenceOfInteger)
                   GetShapesOnShapeIDs(const Handle(GEOM_Object)& theCheckShape,
                                       const Handle(GEOM_Object)& theShape,
                                       const Standard_Integer theShapeType,
                                       GEOMAlgo_State theState);

  /*!
   * \brief Find sub-shapes complying with given status about surface
   * \param theCheckShape - the shape to check state of sub-shapes against
   * \param theShape - the shape to explore
   * \param theShapeType - type of sub-shape of theShape
   * \param theState - required state
   * \retval Handle(TColStd_HSequenceOfTransient) - found shape objects
   */
  Standard_EXPORT Handle(TColStd_HSequenceOfTransient)
                      GetShapesOnShape(const Handle(GEOM_Object)& theCheckShape,
                                       const Handle(GEOM_Object)& theShape,
                                       const Standard_Integer theShapeType,
                                       GEOMAlgo_State theState);
  Standard_EXPORT Handle(GEOM_Object)
            GetShapesOnShapeAsCompound(const Handle(GEOM_Object)& theCheckShape,
                                       const Handle(GEOM_Object)& theShape,
                                       const Standard_Integer theShapeType,
                                       GEOMAlgo_State theState);

  /*!
   * \brief Resize the input edge with the new Min and Max parameters.
   * The input edge parameters range is [0, 1]. If theMin parameter is
   * negative, the input edge is extended, otherwise it is shrinked by
   * theMin parameter. If theMax is greater than 1, the edge is extended,
   * otherwise it is shrinked by theMax parameter
   * \param theEdge the input edge to be resized
   * \param theMin the minimal parameter value
   * \param theMax the maximal parameter value
   * \retval Handle(GEOM_Object) - newly created edge
   */
  Standard_EXPORT Handle(GEOM_Object)
                            ExtendEdge(const Handle(GEOM_Object)& theEdge,
                                       const Standard_Real        theMin,
                                       const Standard_Real        theMax);

  /*!
   * \brief Resize the input face with the new UMin, UMax, VMin and VMax
   * parameters. The input face U and V parameters range is [0, 1]. If
   * theUMin parameter is negative, the input face is extended, otherwise
   * it is shrinked along U direction by theUMin parameter. If theUMax is
   * greater than 1, the face is extended, otherwise it is shrinked along
   * U direction by theUMax parameter. So as for theVMin, theVMax and
   * V direction of the input face.
   * \param theFace the input face to be resized
   * \param theUMin the minimal U parameter value
   * \param theUMax the maximal U parameter value
   * \param theVMin the minimal V parameter value
   * \param theVMax the maximal V parameter value
   * \retval Handle(GEOM_Object) - newly created face
   */
  Standard_EXPORT Handle(GEOM_Object)
                            ExtendFace(const Handle(GEOM_Object)& theFace,
                                       const Standard_Real        theUMin,
                                       const Standard_Real        theUMax,
                                       const Standard_Real        theVMin,
                                       const Standard_Real        theVMax);
  
  /*!
   * \brief Make a surface from a face. This function takes some face as
   * input parameter and creates new GEOM_Object, i.e. topological shape
   * by extracting underlying surface of the source face and limiting it
   * by the Umin, Umax, Vmin, Vmax parameters of the source face (in the
   * parametrical space).
   * \param theFace the input face
   * \retval Handle(GEOM_Object) - newly created face
   */
  Standard_EXPORT Handle(GEOM_Object)
                   MakeSurfaceFromFace(const Handle(GEOM_Object) &theFace);

  /*!
   * \brief Explode a shape into edges sorted in a row from a starting point.
   * \param theShape - the shape to be exploded on edges.
   * \param theStartPoint - the starting point.
   * \return Ordered list of edges sorted in a row from a starting point.
   */
  Standard_EXPORT Handle(TColStd_HSequenceOfTransient)
    GetSubShapeEdgeSorted (const Handle(GEOM_Object) &theShape,
                           const Handle(GEOM_Object) &theStartPoint);

  /*!
   * \brief Return the list of subshapes that satisfies a certain tolerance
   * criterion. The user defines the type of shapes to be returned, the
   * condition and the tolerance value. The operation is defined for
   * faces, edges and vertices only. E.g. for theShapeType FACE, theCondition
   * CC_GT and theTolerance 1.e-7 this method returns all faces of theShape
   * that have tolerances greater then 1.e7.
   *
   * \param theShape the shape to be exploded
   * \param theShapeType the type of shapes to be returned. Can have the
   *        values FACE, EDGE and VERTEX only.
   * \param theCondition the condition type.
   * \param theTolerance the tolerance filter.
   * \return the list of shapes that satisfy the conditions.
   */
  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) GetSubShapesWithTolerance
                     (const Handle(GEOM_Object)            &theShape,
                      const Standard_Integer                theShapeType,
                      const GEOMUtils::ComparisonCondition  theCondition,
                      const Standard_Real                   theTolerance);

  /*!
   * \brief Return the shape that is constructed from theShape without
   * extracted sub-shapes from the input list.
   *
   * \param theShape the original shape.
   * \param theSubShapeIDs the list of sub-shape IDs to be extracted from
   *        the original shape.
   * \param theStats the operation statistics. Output parameter.
   * \return the shape without extracted sub-shapes.
   */
  Standard_EXPORT Handle(GEOM_Object) MakeExtraction
                     (const Handle(GEOM_Object)              &theShape,
                      const Handle(TColStd_HArray1OfInteger) &theSubShapeIDs,
                      std::list<ExtractionStat>              &theStats);

 private:
  Handle(GEOM_Object) MakeShape (std::list<Handle(GEOM_Object)>      theShapes,
                                 const Standard_Integer         theObjectType,
                                 const Standard_Integer         theFunctionType,
                                 const TCollection_AsciiString& theMethodName);

// ----------------------------------------------------
// methods common for all GetShapesOnXXX() functions
// ----------------------------------------------------

  /*!
   * \brief Checks if theShapeType parameter of GetShapesOnXXX() is OK
   * \param theShapeType - the shape type to check
   * \retval bool  - result of the check
   */
  bool checkTypeShapesOn(const Standard_Integer theShapeType);

  /*!
   * \brief Creates Geom_Plane
   * \param theAx1 - edge defining plane normal
   * \retval Handle(Geom_Surface) - resulting surface
   */
  Handle(Geom_Surface) makePlane(const TopoDS_Shape& theAx1);

  /*!
   * \brief Creates Geom_CylindricalSurface
   * \param theAx1 - edge defining cylinder axis
   * \param theRadius - cylinder radius
   * \retval Handle(Geom_Surface) - resulting surface
   */
  Handle(Geom_Surface) makeCylinder(const TopoDS_Shape& theAx1,
                                    const Standard_Real theRadius);

  /*!
   * \brief Find IDs of sub-shapes complying with given status about surface
   * \param theSurface - the surface to check state of sub-shapes against
   * \param theShape - the shape to explore
   * \param theShapeType - type of sub-shape of theShape
   * \param theState - required state
   * \retval Handle(TColStd_HSequenceOfInteger) - IDs of found sub-shapes
   */
  Handle(TColStd_HSequenceOfInteger)
    getShapesOnSurfaceIDs(const Handle(Geom_Surface)& theSurface,
                          const TopoDS_Shape&         theShape,
                          TopAbs_ShapeEnum            theShapeType,
                          GEOMAlgo_State              theState);

  /*!
   * \brief Find sub-shapes complying with given status about surface
   * \param theSurface - the surface to check state of sub-shapes against
   * \param theShape - the shape to explore
   * \param theShapeType - type of sub-shape of theShape
   * \param theState - required state
   * \param theShapeEntries - outgoing entries like "entry1, entry2, ..."
   * \retval Handle(TColStd_HSequenceOfTransient) - found shape objects
   */
  Handle(TColStd_HSequenceOfTransient)
    getShapesOnSurface(const Handle(Geom_Surface)& theSurface,
                       const Handle(GEOM_Object)&  theShape,
                       TopAbs_ShapeEnum            theShapeType,
                       GEOMAlgo_State              theState,
                       TCollection_AsciiString &   theShapeEntries);

  /*!
   * \brief Find IDs of sub-shapes complying with given status about quadrangle
   * \param theShape - the shape to explore
   * \param theShapeType - type of sub-shape of theShape
   * \param theTopLeftPoint - top left quadrangle corner
   * \param theTopRigthPoint - top right quadrangle corner
   * \param theBottomLeftPoint - bottom left quadrangle corner
   * \param theBottomRigthPoint - bottom right quadrangle corner
   * \param theState - required state
   * \retval Handle(TColStd_HSequenceOfInteger) - IDs of found sub-shapes
   */
  Handle(TColStd_HSequenceOfInteger)
    getShapesOnQuadrangleIDs (const Handle(GEOM_Object)& theShape,
                              const Standard_Integer     theShapeType,
                              const Handle(GEOM_Object)& theTopLeftPoint,
                              const Handle(GEOM_Object)& theTopRigthPoint,
                              const Handle(GEOM_Object)& theBottomLeftPoint,
                              const Handle(GEOM_Object)& theBottomRigthPoint,
                              const GEOMAlgo_State       theState);

  /*!
   * \brief Find IDs of sub-shapes complying with given status about surface
   * \param theBox - the box to check state of sub-shapes against
   * \param theShape - the shape to explore
   * \param theShapeType - type of sub-shape of theShape
   * \param theState - required state
   * \retval Handle(TColStd_HSequenceOfInteger) - IDs of found sub-shapes
   */
  Handle(TColStd_HSequenceOfInteger) getShapesOnBoxIDs(const Handle(GEOM_Object)& theBox,
                                                       const Handle(GEOM_Object)& theShape,
                                                       const Standard_Integer theShapeType,
                                                       GEOMAlgo_State theState);

  /*!
   * \brief Find IDs of sub-shapes complying with given status about surface
   * \param theCheckShape - the shape to check state of sub-shapes against
   * \param theShape - the shape to explore
   * \param theShapeType - type of sub-shape of theShape
   * \param theState - required state
   * \retval Handle(TColStd_HSequenceOfInteger) - IDs of found sub-shapes
   */
  Handle(TColStd_HSequenceOfInteger) getShapesOnShapeIDs
                                      (const Handle(GEOM_Object)& theCheckShape,
                                       const Handle(GEOM_Object)& theShape,
                                       const Standard_Integer theShapeType,
                                       GEOMAlgo_State theState);

  /*!
   * \brief Find shape objects and their entries by their ids
   * \param theShape - the main shape
   * \param theShapeIDs - theShapeIDs - incoming shape ids
   * \param theShapeEntries - outgoing entries like "entry1, entry2, ..."
   * \retval Handle(TColStd_HSequenceOfTransient) - found shape objects
   */
  Handle(TColStd_HSequenceOfTransient)
    getObjectsShapesOn(const Handle(GEOM_Object)&                theShape,
                       const Handle(TColStd_HSequenceOfInteger)& theShapeIDs,
                       TCollection_AsciiString &                 theShapeEntries);

  /*!
   * \brief Explode a shape into edges sorted in a row from a starting point.
   * \param theShape - the shape to be exploded on edges.
   * \param theStartPoint - the starting point.
   * \return Ordered list of edges sorted in a row from a starting point.
   */
  Handle(TColStd_HSequenceOfInteger) getSubShapeEdgeSortedIDs
                                (const Handle(GEOM_Object) &theShape,
                                 const Handle(GEOM_Object) &theStartPoint);

  /*!
   * \brief Select the object created last
   * \param theObj1 - Object 1
   * \param theObj2 - Object 2
   * \retval Handle(GEOM_Object) - selected object
   */
  static Handle(GEOM_Object) getCreatedLast(const Handle(GEOM_Object)& theObj1,
                                            const Handle(GEOM_Object)& theObj2);
};

#endif
