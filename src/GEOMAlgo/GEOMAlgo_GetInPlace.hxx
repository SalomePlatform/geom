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
// File:        GEOMAlgo_GetInPlace.hxx
// Created:
// Author:      Peter KURNEV

#ifndef _GEOMAlgo_GetInPlace_HeaderFile
#define _GEOMAlgo_GetInPlace_HeaderFile

#include <Standard.hxx>
#include <Standard_Macro.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>

#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS_Shape.hxx>

#include <GEOMAlgo_CoupleOfShapes.hxx>
#include <GEOMAlgo_ListOfCoupleOfShapes.hxx>
#include <GEOMAlgo_ListIteratorOfListOfCoupleOfShapes.hxx>

#include <GEOMAlgo_DataMapOfShapeMapOfShape.hxx>
#include <GEOMAlgo_GluerAlgo.hxx>
#include <GEOMAlgo_Algo.hxx>
#include <GEOMAlgo_DataMapOfShapePnt.hxx>
#include <TopTools_DataMapOfShapeInteger.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>


//=======================================================================
/**
 * The implementation of iterator of intersected  shapes
 * for Get In Place Algorithm.
 * The intersection is in terms of 3D bounding boxes.
 */
//=======================================================================
//class    : GEOMAlgo_GetInPlaceIterator
//purpose  :
//=======================================================================
class GEOMAlgo_GetInPlaceIterator  {
 public:
  /**
   * Constructor.
   */
  //Standard_EXPORT
    GEOMAlgo_GetInPlaceIterator();

  /**
   * Destructor.
   */
  //Standard_EXPORT
    virtual ~GEOMAlgo_GetInPlaceIterator();

  /**
   * Clear the internal content.
   */
  //Standard_EXPORT
    void Clear() ;

  /**
   * Append the pair of intersected shapes.
   * @param theCS
   *   The pair of intersected shapes.
   */
  //Standard_EXPORT
    void AppendPair(const GEOMAlgo_CoupleOfShapes& theCS) ;

  /**
   * Initialize the iterator.
   * @param theT1
   *   The type of (sub)shape What.
   * @param theT2
   *   The type of (sub)shape Where.
   */
  //Standard_EXPORT
    void Initialize(const TopAbs_ShapeEnum theT1,
                    const TopAbs_ShapeEnum theT2) ;
  /**
   * Check the existence of pairs to iterare.
   * @return
   *   Standard_True if there are pairs to iterare.
   */
  //Standard_EXPORT
    Standard_Boolean More() const;

  /**
   * Shift to the next pair.
   */
  //Standard_EXPORT
    void Next() ;

  /**
   * Returns the pair of intersected shapes.
   * @return
   *   The pair of intersected shapes.
   */
  //Standard_EXPORT
    const GEOMAlgo_CoupleOfShapes& Value() const;

protected:
  Standard_Integer myDim;
  GEOMAlgo_ListOfCoupleOfShapes myLists[10];
  GEOMAlgo_ListOfCoupleOfShapes myEmptyList;
  GEOMAlgo_ListIteratorOfListOfCoupleOfShapes myIterator;

private:
};


//=======================================================================
/**
 * The implementation of Get In Place Algorithm.
 * The algorithm provides the search the argument [What]
 * in the shape [Where].
 */
//=======================================================================
//class    : GEOMAlgo_GetInPlace
//purpose  :
//=======================================================================
class GEOMAlgo_GetInPlace  : public GEOMAlgo_GluerAlgo,
                             public GEOMAlgo_Algo
{
 public:
  /**
   * Constructor.
   */
  Standard_EXPORT
    GEOMAlgo_GetInPlace();
  /**
   * Destructor.
   */
  Standard_EXPORT
    virtual ~GEOMAlgo_GetInPlace();
  /**
   * Modifier. Sets the shape where the search is intended.
   * @param theShape
   *   The shape where the search is intended.
   */
  Standard_EXPORT
    virtual void SetShapeWhere(const TopoDS_Shape& theShape) ;

  /**
   * Selector. Returns the shape where the search is intended.
   * @return
   *   The shape where the search is intended.
   */
  Standard_EXPORT
    const TopoDS_Shape& ShapeWhere() const;

  /**
   * Modifier. Sets the tolerance of mass.
   * @param theTol
   *   The value tolerance of mass.
   */
  Standard_EXPORT
    void SetTolMass(const Standard_Real theTol) ;

  /**
   * Selector. Returns the value tolerance of mass.
   * @return
   *   The value tolerance of mass.
   */
  Standard_EXPORT
    Standard_Real TolMass() const;

  /**
   * Modifier. Sets the tolerance of center of gravily.
   * @param theTol
   *   The value tolerance of center of gravily.
   */
  Standard_EXPORT
    void SetTolCG(const Standard_Real theTol) ;

  /**
   * Selector. Returns the tolerance of center of gravily.
   * @return
   *   The value tolerance of center of gravily.
   */
  Standard_EXPORT
    Standard_Real TolCG() const;

  /**
   * Perform the algorithm.
   */
  Standard_EXPORT
    virtual  void Perform() ;

  /**
   * Returns state of the search.
   * @return
   *   Standard_True if the whole argument is found.
   */
  Standard_EXPORT
    Standard_Boolean IsFound() const;

  /**
   * Checks data
   */
  Standard_EXPORT
    virtual  void CheckData() ;

  /**
   * Clear the internal content.
   */
  Standard_EXPORT
    virtual  void Clear() ;

  /**
   * Returns the map of shapes IN.
   * @return
   ** Returns the map of shapes IN.
   * The Key - the (sub)shape of the argument [What].
   * The Item- the (sub)shapes of the shape [Where] that have
   *           the state IN in respect of [What].
  */
  Standard_EXPORT
    const GEOMAlgo_DataMapOfShapeMapOfShape& ShapesIn() const;

  /**
   * Returns the map of shapes ON.
   * @return
   * Returns the map of shapes ON.
   * The Key - the (sub)shape of the argument [What].
   * The Item- the (sub)shapes of the shape [Where] that have
   *           the state ON in respect of [What].
   */
  Standard_EXPORT
    const GEOMAlgo_DataMapOfShapeMapOfShape& ShapesOn() const;
  //
  /**
   * Returns the result that includes parts of the whole and whole from parts.
   *
   * The Result is the parts (taken from myShapeWhere) that correspond to
   * myArgument (the whole one or its part). To check if the whole myArgument
   * is found in myShapeWhere it is possible to use the method IsFound.
   *
   * The Result is a compound.
   *
   * @return
   *  - the compound of the parts from myShapeWhere 
   *    that correspond to the myArgument if success.
   *  - NULL shape otherwise
   */
  Standard_EXPORT
    const TopoDS_Shape &Result();
    
protected:
  Standard_EXPORT
    void Intersect() ;

  Standard_EXPORT
    void PerformVV() ;

  Standard_EXPORT
    void PerformVE() ;

  Standard_EXPORT
    void PerformEE() ;

    void PerformEE(const TopoDS_Shape &theE1, const TopoDS_Shape &theE2);

  Standard_EXPORT
    void PerformVF() ;

  Standard_EXPORT
    void PerformEF() ;

  Standard_EXPORT
    void PerformFF() ;

    void PerformFF(const TopoDS_Shape &theF1, const TopoDS_Shape &theF2);

  Standard_EXPORT
    void FillEdgesOn(const TopoDS_Shape &theShape);

  Standard_EXPORT
    void FillFacesOn(const TopoDS_Shape &theShape);

  Standard_EXPORT
    void FillSolidsOn(const TopoDS_Shape &theShape);

  Standard_EXPORT
    void PerformZF() ;

  Standard_EXPORT
    void PerformZZ() ;

    void PerformZZ(const TopoDS_Shape &theSo1, const TopoDS_Shape &theSo2);

  Standard_EXPORT
    void FillImages(const TopoDS_Shape &theShape,
                    const Standard_Boolean IsWhere);

    void FillImgSimple(const TopoDS_Shape     &theShape,
                       const TopAbs_ShapeEnum  theSubShapeType,
                       const Standard_Boolean  IsWhere);

    void FillImgComplex(const TopoDS_Shape     &theShape,
                        const Standard_Boolean  IsWhere);

    void FillImgComplex(const TopoDS_Shape     &theShape,
                        const TopAbs_ShapeEnum  theSubShapeType,
                        const Standard_Boolean  IsWhere);

  Standard_EXPORT
    void CheckGProps() ;

  Standard_EXPORT
    void FillShapesIn(const TopoDS_Shape& theS1,
                      const TopoDS_Shape& theS2) ;

  Standard_EXPORT
    void FillShapesOn(const TopoDS_Shape& theS1,
                      const TopoDS_Shape& theS2) ;

  Standard_EXPORT
    Standard_Boolean CheckCoincidence(const TopoDS_Shape& theS1,
                                      const TopoDS_Shape& theS2);
  
  Standard_EXPORT
    Standard_Integer CheckGProps(const TopoDS_Shape& theS);

  Standard_Boolean CompareGProps
                  (const TopoDS_Shape         &theShape1,
                   const TopTools_ListOfShape &theListShape2) const;

  Standard_EXPORT
    void UpdateChecked(const TopoDS_Shape& theS1,
                       const Standard_Integer theFlag);
  //
  TopoDS_Shape myShapeWhere;
  GEOMAlgo_GetInPlaceIterator myIterator;
  GEOMAlgo_DataMapOfShapeMapOfShape myShapesIn;
  GEOMAlgo_DataMapOfShapeMapOfShape myShapesOn;
  TopTools_DataMapOfShapeShape myShapesInclusive;
  Standard_Real myTolMass;
  Standard_Real myTolCG;
  Standard_Boolean myFound;
  GEOMAlgo_DataMapOfShapePnt myMapShapePnt;
  TopTools_DataMapOfShapeInteger myChecked;
  //
  TopoDS_Shape myResult;

private:
};



#endif
