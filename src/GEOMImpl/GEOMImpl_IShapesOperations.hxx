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

#ifndef _GEOMImpl_IShapesOperations_HXX_
#define _GEOMImpl_IShapesOperations_HXX_

#include "GEOM_IOperations.hxx"

#include "GEOMAlgo_State.hxx"

#include <TopTools_ListOfShape.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <TColStd_HSequenceOfInteger.hxx>

#include <list>
#include <Handle_Geom_Surface.hxx>

class GEOM_Engine;
class Handle(GEOM_Object);
class Handle(TColStd_HArray1OfInteger);

class GEOMImpl_IShapesOperations : public GEOM_IOperations {
 public:
  Standard_EXPORT GEOMImpl_IShapesOperations(GEOM_Engine* theEngine, int theDocID);
  Standard_EXPORT ~GEOMImpl_IShapesOperations();

  Standard_EXPORT Handle(GEOM_Object) MakeEdge (Handle(GEOM_Object) thePoint1,
                                Handle(GEOM_Object) thePoint2);

  Standard_EXPORT Handle(GEOM_Object) MakeWire (list<Handle(GEOM_Object)> theEdgesAndWires);

  Standard_EXPORT Handle(GEOM_Object) MakeFace (Handle(GEOM_Object) theWire, const bool isPlanarWanted);

  Standard_EXPORT Handle(GEOM_Object) MakeFaceWires (list<Handle(GEOM_Object)> theWires,
                                     const bool isPlanarWanted);

  Standard_EXPORT Handle(GEOM_Object) MakeShell (list<Handle(GEOM_Object)> theShapes);

  Standard_EXPORT Handle(GEOM_Object) MakeSolidShell (Handle(GEOM_Object) theShell);

  Standard_EXPORT Handle(GEOM_Object) MakeSolidShells (list<Handle(GEOM_Object)> theShells);

  Standard_EXPORT Handle(GEOM_Object) MakeCompound (list<Handle(GEOM_Object)> theShapes);

  Standard_EXPORT Handle(GEOM_Object) MakeGlueFaces (Handle(GEOM_Object) theShape,
                                     const Standard_Real theTolerance);

  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) MakeExplode (Handle(GEOM_Object)    theShape,
                                                    const Standard_Integer theShapeType,
                                                    const Standard_Boolean isSorted);

  Standard_EXPORT Handle(TColStd_HSequenceOfInteger) SubShapeAllIDs (Handle(GEOM_Object)    theShape,
                                                     const Standard_Integer theShapeType,
                                                     const Standard_Boolean isSorted);

  Standard_EXPORT Handle(GEOM_Object) GetSubShape (Handle(GEOM_Object)    theMainShape,
                                   const Standard_Integer theID);

  Standard_EXPORT Standard_Integer NumberOfFaces (Handle(GEOM_Object) theShape);
  Standard_EXPORT Standard_Integer NumberOfEdges (Handle(GEOM_Object) theShape);

  Standard_EXPORT Handle(GEOM_Object) ReverseShape(Handle(GEOM_Object) theShapes);

  Standard_EXPORT Handle(TColStd_HSequenceOfInteger) GetFreeFacesIDs (Handle(GEOM_Object) theShape);

  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) GetSharedShapes (Handle(GEOM_Object)    theShape1,
                                                        Handle(GEOM_Object)    theShape2,
                                                        const Standard_Integer theShapeType);

  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) GetShapesOnPlane (const Handle(GEOM_Object)& theShape,
                                                         const Standard_Integer     theShapeType,
                                                         const Handle(GEOM_Object)& theAx1,
                                                         const GEOMAlgo_State       theState);

  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) GetShapesOnCylinder (const Handle(GEOM_Object)& theShape,
                                                            const Standard_Integer     theShapeType,
                                                            const Handle(GEOM_Object)& theAxis,
                                                            const Standard_Real        theRadius,
                                                            const GEOMAlgo_State       theState);

  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) GetShapesOnSphere (const Handle(GEOM_Object)& theShape,
                                                          const Standard_Integer     theShapeType,
                                                          const Handle(GEOM_Object)& theCenter,
                                                          const Standard_Real        theRadius,
                                                          const GEOMAlgo_State       theState);

  Standard_EXPORT Handle(TColStd_HSequenceOfInteger) GetShapesOnPlaneIDs (const Handle(GEOM_Object)& theShape,
                                                          const Standard_Integer     theShapeType,
                                                          const Handle(GEOM_Object)& theAx1,
                                                          const GEOMAlgo_State       theState);

  Standard_EXPORT Handle(TColStd_HSequenceOfInteger) GetShapesOnCylinderIDs (const Handle(GEOM_Object)& theShape,
                                                             const Standard_Integer     theShapeType,
                                                             const Handle(GEOM_Object)& theAxis,
                                                             const Standard_Real        theRadius,
                                                             const GEOMAlgo_State       theState);

  Standard_EXPORT Handle(TColStd_HSequenceOfInteger) GetShapesOnSphereIDs (const Handle(GEOM_Object)& theShape,
                                                           const Standard_Integer     theShapeType,
                                                           const Handle(GEOM_Object)& theCenter,
                                                           const Standard_Real        theRadius,
                                                           const GEOMAlgo_State       theState);

  /*!
   * \brief Find subshapes complying with given status about quadrangle
    * \param theShape - the shape to explore
    * \param theShapeType - type of subshape of theShape
    * \param theTopLeftPoint - top left quadrangle corner
    * \param theTopRigthPoint - top right quadrangle corner
    * \param theBottomLeftPoint - bottom left quadrangle corner
    * \param theBottomRigthPoint - bottom right quadrangle corner
    * \param theState - required state
    * \retval Handle(TColStd_HSequenceOfInteger) - IDs of found subshapes
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
   * \brief Find IDs of subshapes complying with given status about quadrangle
    * \param theShape - the shape to explore
    * \param theShapeType - type of subshape of theShape
    * \param theTopLeftPoint - top left quadrangle corner
    * \param theTopRigthPoint - top right quadrangle corner
    * \param theBottomLeftPoint - bottom left quadrangle corner
    * \param theBottomRigthPoint - bottom right quadrangle corner
    * \param theState - required state
    * \retval Handle(TColStd_HSequenceOfInteger) - IDs of found subshapes
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

  Standard_EXPORT static void SortShapes (TopTools_ListOfShape& SL);

 private:
  Handle(GEOM_Object) MakeShape (list<Handle(GEOM_Object)>      theShapes,
                                 const Standard_Integer         theObjectType,
                                 const Standard_Integer         theFunctionType,
                                 const TCollection_AsciiString& theMethodName);

// ----------------------------------------------------
  // methods common for all GetShapesOnXXX() functions
// ----------------------------------------------------

  bool CheckTriangulation (const TopoDS_Shape& aShape);

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
   * \brief Find IDs of subshapes complying with given status about surface
    * \param theSurface - the surface to check state of subshapes against
    * \param theShape - the shape to explore
    * \param theShapeType - type of subshape of theShape
    * \param theState - required state
    * \retval Handle(TColStd_HSequenceOfInteger) - IDs of found subshapes
   */
  Handle(TColStd_HSequenceOfInteger)
    getShapesOnSurfaceIDs(const Handle(Geom_Surface)& theSurface,
                          const TopoDS_Shape&         theShape,
                          TopAbs_ShapeEnum            theShapeType,
                          GEOMAlgo_State              theState);

  /*!
   * \brief Find subshapes complying with given status about surface
    * \param theSurface - the surface to check state of subshapes against
    * \param theShape - the shape to explore
    * \param theShapeType - type of subshape of theShape
    * \param theState - required state
    * \param theShapeEntries - outgoing entries like "entry1, entry2, ..."
    * \retval Handle(TColStd_HSequenceOfInteger) - IDs of found subshapes
   */
  Handle(TColStd_HSequenceOfTransient)
    getShapesOnSurface(const Handle(Geom_Surface)& theSurface,
                       const Handle(GEOM_Object)&  theShape,
                       TopAbs_ShapeEnum            theShapeType,
                       GEOMAlgo_State              theState,
                       TCollection_AsciiString &   theShapeEntries
                       );

  /*!
   * \brief Find IDs of subshapes complying with given status about quadrangle
    * \param theShape - the shape to explore
    * \param theShapeType - type of subshape of theShape
    * \param theTopLeftPoint - top left quadrangle corner
    * \param theTopRigthPoint - top right quadrangle corner
    * \param theBottomLeftPoint - bottom left quadrangle corner
    * \param theBottomRigthPoint - bottom right quadrangle corner
    * \param theState - required state
    * \retval Handle(TColStd_HSequenceOfInteger) - IDs of found subshapes
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
   * \brief Select the object created last
    * \param theObj1 - Object 1
    * \param theObj2 - Object 2
    * \retval Handle(GEOM_Object) - selected object
   */
  static Handle(GEOM_Object) getCreatedLast(const Handle(GEOM_Object)& theObj1,
                                            const Handle(GEOM_Object)& theObj2);
};

#endif
