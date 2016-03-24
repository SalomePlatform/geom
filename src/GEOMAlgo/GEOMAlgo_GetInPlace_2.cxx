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
// File:        GEOMAlgo_GetInPlace_2.cxx
// Created:
// Author:      Peter KURNEV

#include <GEOMAlgo_GetInPlace.hxx>

#include <TopAbs_ShapeEnum.hxx>

#include <gp_Pnt.hxx>

#include <TopoDS_Iterator.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>

#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>

#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#include <TopExp.hxx>

#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>


static
  Standard_Integer Dimension(const TopAbs_ShapeEnum aType);
static
  void PointProperties(const TopoDS_Shape& aS,
                       GProp_GProps& aGProps);

//=======================================================================
//function : CheckGProps
//purpose  :
//=======================================================================
void GEOMAlgo_GetInPlace::CheckGProps()
{
  myFound=Standard_True;
  CheckGProps(myArgument);
}
//=======================================================================
//function : CheckGProps
//purpose  :
//=======================================================================
Standard_Integer GEOMAlgo_GetInPlace::CheckGProps(const TopoDS_Shape& theS)
{
  TopAbs_ShapeEnum aType = theS.ShapeType();

  if (aType == TopAbs_COMPOUND) {
    TopoDS_Iterator anIt(theS);
    TopTools_MapOfShape aMapInc;

    for(; anIt.More(); anIt.Next()) {
      const TopoDS_Shape &aS1x = anIt.Value();

      aType = aS1x.ShapeType();

      if (aType != TopAbs_COMPOUND && myShapesInclusive.IsBound(aS1x)) {
        // This is a part of a whole.
        aMapInc.Add(aS1x);
      } else {
        // Check this subshape.
        const Standard_Integer iFound = CheckGProps(aS1x);
        
        if (!iFound) {
          UpdateChecked(theS, 0);
          myFound = Standard_False;
          return 0;
        }
      }
    }

    // Treat parts of a whole.
    while (!aMapInc.IsEmpty()) {
      TopTools_MapIteratorOfMapOfShape aMapIt(aMapInc);
      const TopoDS_Shape &aWhole = myShapesInclusive.Find(aMapIt.Key());

      if (!myImages.IsBound(aWhole)) {
        // Should not be.
        UpdateChecked(theS, 0);
        myFound = Standard_False;
        return 0;
      }

      const TopTools_ListOfShape& aLS1 = myImages.Find(aWhole);

      if (aLS1.IsEmpty()) {
        // Empty list of parts. Should not be.
        UpdateChecked(theS, 0);
        myFound = Standard_False;
        return 0;
      }

      TopTools_ListIteratorOfListOfShape aItS1x(aLS1);

      for (; aItS1x.More(); aItS1x.Next()) {
        const TopoDS_Shape &aS1x = aItS1x.Value();

        if (!aMapInc.Remove(aS1x)) {
          // There is no aS1x in theS. Should not be.
          UpdateChecked(theS, 0);
          myFound = Standard_False;
          return 0;
        }
      }

      // Compare a shape with an image.
      if (!CompareGProps(aWhole, aLS1)) {
        // Image doesn't correspond to the shape.
        UpdateChecked(theS, 0);
        myFound = Standard_False;
        return 0;
      }
    }

    return myFound ? 1 : 0;
  }

  // Check the simple shape.
  if (!myImages.IsBound(theS)) {
    // it should not be.
    UpdateChecked(theS, 0);
    myFound = Standard_False;
    return 0;
  }
  //
  const TopTools_ListOfShape &aLS2 = myImages.Find(theS);

  if (aLS2.IsEmpty()) {
    // it should not be.
    UpdateChecked(theS, 0);
    myFound = Standard_False;
    return 0;
  }

  // Compare a shape with an image.
  if (!CompareGProps(theS, aLS2)) {
    // Image doesn't correspond to the shape.
    UpdateChecked(theS, 0);
    myFound = Standard_False;
    return 0;
  }

  UpdateChecked(theS, 1);
  //
  return 1;
}

//=======================================================================
//function : CompareGProps
//purpose  : 
//=======================================================================
Standard_Boolean GEOMAlgo_GetInPlace::CompareGProps
                        (const TopoDS_Shape         &theShape1,
                         const TopTools_ListOfShape &theListShape2) const
{
  Standard_Boolean                   aResult = Standard_True;
  TopoDS_Compound                    aComp2;
  BRep_Builder                       aBuilder;
  TopTools_ListIteratorOfListOfShape anIt(theListShape2);

  aBuilder.MakeCompound(aComp2);

  for (; anIt.More(); anIt.Next()) {
    const TopoDS_Shape &aShape2 = anIt.Value();

    aBuilder.Add(aComp2, aShape2);
  }

  // Compute General Properties.
  GProp_GProps           aG1;
  GProp_GProps           aG2;
  const Standard_Real    aTolCG2     = myTolCG*myTolCG;
  Standard_Boolean       bOnlyClosed = Standard_False;
  const TopAbs_ShapeEnum aType       = theShape1.ShapeType();
  const Standard_Integer iDim        = Dimension(aType);

  if (iDim == 0) {
    PointProperties(theShape1, aG1);
    PointProperties(aComp2,    aG2);
  }
  else if (iDim == 1) {
    BRepGProp::LinearProperties(theShape1, aG1);
    BRepGProp::LinearProperties(aComp2,    aG2);
  }
  else if (iDim == 2) {
    BRepGProp::SurfaceProperties(theShape1, aG1);
    BRepGProp::SurfaceProperties(aComp2,    aG2);
  }
  else if (iDim == 3) {
    BRepGProp::VolumeProperties(theShape1, aG1, bOnlyClosed);
    BRepGProp::VolumeProperties(aComp2,    aG2, bOnlyClosed);
  } else {
    return Standard_False;
  }

  // Compare properties.
  const Standard_Real aMass1 = aG1.Mass();
  const Standard_Real aMass2 = aG2.Mass();
  const gp_Pnt        aCG1   = aG1.CentreOfMass();
  const gp_Pnt        aCG2   = aG2.CentreOfMass();
  Standard_Real       aDM    = fabs(aMass1 - aMass2);
  const Standard_Real aD2    = aCG1.SquareDistance(aCG2);

  if (aMass1 > myTolMass) {
    aDM /= aMass1;
  }

  if ((aDM > myTolMass) || (aD2 > aTolCG2)) {
    aResult = Standard_False;
  }

  return aResult;
}

//=======================================================================
//function : UpdateChecked
//purpose  : 
//=======================================================================
void GEOMAlgo_GetInPlace::UpdateChecked(const TopoDS_Shape& theS1,
                                        const Standard_Integer theFlag)
{
  if (myChecked.IsBound(theS1)) {
    Standard_Integer& iChecked=myChecked.ChangeFind(theS1);
    iChecked=theFlag;
  }
  else {
    myChecked.Bind(theS1, theFlag);
  }
}
//=======================================================================
//function : Dimension
//purpose  :
//=======================================================================
Standard_Integer Dimension(const TopAbs_ShapeEnum aType)
{
  Standard_Integer iDim;
  //
  iDim=-1;
  switch (aType) {
    case TopAbs_VERTEX:
      iDim=0;
      break;
    case TopAbs_EDGE:
    case TopAbs_WIRE:
      iDim=1;
      break;
    case TopAbs_FACE:
    case TopAbs_SHELL:
      iDim=2;
      break;
    case TopAbs_SOLID:
    case TopAbs_COMPSOLID:
      iDim=3;
      break;
    default:
      break;
  }
  return iDim;
}
//=======================================================================
//class : GEOMAlgo_GProps
//purpose  :
//=======================================================================
class GEOMAlgo_GProps : public GProp_GProps {
 public:
  GEOMAlgo_GProps() : GProp_GProps() {
  };
  //
  GEOMAlgo_GProps(const gp_Pnt& aPLoc) : GProp_GProps(aPLoc) {
  };
  //
  ~GEOMAlgo_GProps() {
  };
  //
  void SetMass(const Standard_Real aMass) {
    dim=aMass;
  }
  //
  void SetCG(const gp_Pnt& aPCG) {
    g=aPCG;
  }
};
//=======================================================================
//function : PointProperties
//purpose  :
//=======================================================================
void PointProperties(const TopoDS_Shape& aS, GProp_GProps& aGProps)
{
  Standard_Integer i, aNbS;
  Standard_Real aDensity;
  gp_Pnt aPX;
  TopTools_IndexedMapOfShape aMS;
  //
  aDensity=1.;
  //
  TopExp::MapShapes(aS, TopAbs_VERTEX, aMS);
  aNbS=aMS.Extent();
  for (i=1; i<=aNbS; ++i) {
    GEOMAlgo_GProps aGPropsX;
    //
    const TopoDS_Vertex& aVX=*((TopoDS_Vertex*)&aMS(i));
    aPX=BRep_Tool::Pnt(aVX);
    aGPropsX.SetMass(1.);
    aGPropsX.SetCG(aPX);
    aGProps.Add(aGPropsX, aDensity);
  }
}
