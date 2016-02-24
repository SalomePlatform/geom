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

// File:        GEOMAlgo_Extractor.hxx
// Author:      Sergey KHROMOV

#ifndef _GEOMAlgo_Extractor_HeaderFile
#define _GEOMAlgo_Extractor_HeaderFile


#include <GEOMAlgo_Algo.hxx>

#include <NCollection_List.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>


/**
 * \brief This class encapsulates an algorithm of extraction of sub-shapes
 * from the main shape.
 */
class GEOMAlgo_Extractor : public GEOMAlgo_Algo
{
public:

  /**
   * \brief Empty constructor.
   */
  Standard_EXPORT GEOMAlgo_Extractor();

  /**
   * \brief Virtual destructor.
   */
  Standard_EXPORT virtual ~GEOMAlgo_Extractor();

  /**
   * \brief This method sets the main shape.
   *
   * \param theShape the main shape.
   */
  Standard_EXPORT void SetShape(const TopoDS_Shape &theShape);

  /**
   * \brief This method returns the main shape.
   *
   * \return the main shape.
   */
  const TopoDS_Shape &GetShape() const
  { return myShape; }

  /**
   * \brief This method sets the list of sub-shapes to be removed
   *  from the main shape.
   *
   * \param theSubShapes the sub-shapes to be removed.
   */
  Standard_EXPORT void SetShapesToRemove
      (const TopTools_ListOfShape &theSubShapes);

  /**
   * \brief This method returns the list of sub-shapes to be removed
   *  from the main shape.
   *
   * \return the list of sub-shapes to be removed.
   */
  const TopTools_ListOfShape &GetShapesToRemove() const
  { return mySubShapes; }

  /**
   * This method performs computation of the extracted shape.
   */
  Standard_EXPORT virtual void Perform();

  /**
   * This method returns the result of the algorithm.
   *
   * \return the result of the operation.
   */
  Standard_EXPORT const TopoDS_Shape &GetResult() const;

  /**
   * \brief This method returns the sub-shapes removed from the main shape.
   *
   * \return the list of removed sub-shapes.
   */
  const TopTools_ListOfShape &GetRemoved() const
  { return myRemoved; }

  /**
   * \brief This method returns the sub-shapes modified in the main shape.
   *
   * \return the list of modified sub-shapes.
   */
  const TopTools_ListOfShape &GetModified() const
  { return myModified; }

  /**
   * \brief This method returns the newly created sub-shapes in the result
   * shape.
   *
   * \return the list of new sub-shapes in result.
   */
  const TopTools_ListOfShape &GetNew() const
  { return myNew; }

private:

  /**
   * \brief This method reinitializes the shape.
   */
  void clear();

  /**
   * \brief This method checks the input data.
   */
  void checkData();

  /**
   * \brief This method fills the map of shapes and ancestors for the whole
   * sub-shapes of theShape. This method is recursively called up to the lowest
   * level of sub-shapes i.e. vertices.
   *
   * \param theShape the shape.
   */
  void makeMapShapeAncestors(const TopoDS_Shape &theShape);

  /**
   * \brief This method marks shapes to be removed and to be modified.
   */
  void markShapes();

  /**
   * \brief This method marks theShape to be removed. If it is required, it
   * recursively marks its sub-shapes to be removed.
   *
   * \param theShape the shape.
   */
  void markRemoved(const TopoDS_Shape &theShape);

  /**
   * \brief This method marks ancestors of theShape to be modified. It is
   * recursively called up to the level of main shape.
   *
   * \param theShape the shape.
   */
  void markAncestorsModified(const TopoDS_Shape &theShape);

  /**
   * \brief This method performs computation of modified shapes of
   *  the provided type.
   *
   * \param theType the processed shape type.
   */
  void processShapes(const TopAbs_ShapeEnum &theType);

  /**
   * \brief This method performs computation of a modified edge. 
   *
   * \param theEdge the modified edge (should be forward).
   */
  void processEdge(const TopoDS_Shape &theEdge);

  /**
   * \brief This method performs computation of a modified wire.
   *
   * \param theWire the modified wire (should be forward).
   */
  void processWire(const TopoDS_Shape &theWire);

  /**
   * \brief This method performs computation of a modified face or solid.
   *
   * \param theFOrSo the modified face or solid (should be forward).
   */
  void processFOrSo(const TopoDS_Shape &theFOrSo);

  /**
   * \brief This method performs computation of a modified shell or comp-solid.
   *
   * \param theShOrCS the modified shell or comp-solid (should be forward).
   */
  void processShOrCS(const TopoDS_Shape &theShOrCS);

  /**
   * \brief This method performs computation of a modified compound.
   *
   * \param theCompound the modified compound (should be forward).
   */
  void processCompound(const TopoDS_Shape &theCompound);

  /**
   * \brief This method removes hanging edges (faces) built for faces (solids)
   * if they lie on created faces (solids).
   *
   * \param theType the shape type. Should be either face or solid.
   */
  void removeBoundsOnFOrSo(const TopAbs_ShapeEnum theType);

  /**
   * \brief Returns theShape with an orientation composed with theContext's
   * orientation.
   *
   * \param theShape the shape to be re-oriented.
   * \param theContext the context shape.
   */
  TopoDS_Shape oriented(const TopoDS_Shape &theShape,
                        const TopoDS_Shape &theContext);

  /**
   * \brief This method makes a shape as an empty copy of theShape adding
   * subshapes to it.
   *
   * \param theShape the shape to be copied (should be forward).
   * \param theSubShapes the sub-shapes (should be oriented correctly).
   * \return the modified shape.
   */
  TopoDS_Shape makeShape(const TopoDS_Shape         &theShape,
                         const TopTools_ListOfShape &theSubShapes);

  /**
   * \brief This method returns the shape from the list of sub-shapes
   * if there is any shape created already with these sub-shapes.
   * If there is no such shape, null shape is returned.
   *
   * \param theShape the shape to be copied (should be forward).
   * \param theSubShapes the sub-shapes (should be oriented correctly).
   * \return the modified shape (or null if it is not found).
   */
  TopoDS_Shape getShapeFromSubShapes(const TopoDS_Shape         &theShape,
                                     const TopTools_ListOfShape &theSubShapes);

  /**
   * \brief This method makes the result for the given shape. If it is removed
   * the result is a compound of its modified sub-shapes (or a single
   * modified sub-shape if it in only one).
   *
   * \param theShape the shape.
   * \return the result.
   */
  TopoDS_Shape makeResult(const TopoDS_Shape &theShape);

  /**
   * \brief This method fills the lists of shapes myRemoved, myModified and
   * myNew with removed, modified and newly created shapes correspondingly.
   * This method is called recursively for sub-shapes of the shape.
   *
   * \param theShape the shape.
   * \param theMapFence the map of already treated shapes.
   */
  void makeHistory(const TopoDS_Shape        &theShape,
                         TopTools_MapOfShape &theMapFence);

  /**
   * \brief This method removes edges that are in theMapEdgesToRm from
   * theWire and re-creates one or more wires from the rest edges. theNewWires
   * contains the modified wire(s).
   *
   * \param theWire the input wire.
   * \param theMapEdgesToRm the map of edges to be extracted from theWire.
   * \param theNewWires is the list of new wires. Output parameter.
   * \return Standard_True if theWire is modified; Standard_False otherwise.
   */
  Standard_Boolean removeCommonEdges
                     (const TopoDS_Shape               &theWire,
                      const TopTools_IndexedMapOfShape &theMapEdgesToRm,
                            TopTools_ListOfShape       &theNewWires);

  /**
   * \brief This method removes faces that are in theMapFacesToRm from
   * theShell and re-creates one or more shells from the rest faces.
   * theNewShells contains the modified shell(s).
   *
   * \param theShell the input shell.
   * \param theMapFacesToRm the map of faces to be extracted from theShell.
   * \param theNewShells is the list of new shells. Output parameter.
   * \return Standard_True if theShell is modified; Standard_False otherwise.
   */
  Standard_Boolean removeCommonFaces
                     (const TopoDS_Shape               &theShell,
                      const TopTools_IndexedMapOfShape &theMapFacesToRm,
                            TopTools_ListOfShape       &theNewShells);

  /**
   * \brief This method creates wires from the list of list of edges.
   *
   * \param theWire the input wire.
   * \param theListListEdges the list of list of edges. Can be modified
   *        on output.
   * \param theWires the list of created wires. Output parameter.
   */
  void makeWires(const TopoDS_Shape                           &theWire,
                       NCollection_List<TopTools_ListOfShape> &theListListEdges,
                       TopTools_ListOfShape                   &theWires);

  /**
   * \brief This method collects the shapes in theShapes via common bounds.
   * This method is used to group faces into shells via common edges or
   * solids into compsolids via common faces. Collected lists of shapes
   * are used to create new shapes from theShape that are returned in
   * theNewShapes. theNewShapes is not cleared at first.
   *
   * \param theShape the original shape.
   * \param theSubShapes the list of shapes to be connected.
   * \param theNewShapes the list of newly created shapes. Output parameter.
   */
  void groupViaBounds(const TopoDS_Shape         &theShape,
                      const TopTools_ListOfShape &theSubShapes,
                            TopTools_ListOfShape &theNewShapes);

  /**
   * \brief This method returns the list of modified shapes obtained
   * from theShape. It performs recursive search in myMapModified.
   * theModifShapes is not cleared at first. If theShapeType filter is equal
   * to TopAbs_SHAPE (default value) all modified shapes will be returned,
   * otherwise shapes of particular type will only be returned.
   *
   * \param theShape the shape examined.
   * \param theModifShapes the list of modified shapes. Output parameter.
   * \param theShapeType the shape type filter.
   */
  void getModified(const TopoDS_Shape         &theShape,
                         TopTools_ListOfShape &theModifShapes,
                   const TopAbs_ShapeEnum      theShapeType = TopAbs_SHAPE);

protected:

  TopoDS_Shape                       myShape;
  TopoDS_Shape                       myResult;
  TopTools_ListOfShape               mySubShapes;
  TopTools_ListOfShape               myRemoved;
  TopTools_ListOfShape               myModified;
  TopTools_ListOfShape               myNew;
  TopTools_DataMapOfShapeListOfShape myMapShapeAnc;
  TopTools_MapOfShape                myMapRemoved;
  TopTools_DataMapOfShapeListOfShape myMapModified;
  TopTools_DataMapOfShapeListOfShape myMapNewShapeAnc;

};

#endif
