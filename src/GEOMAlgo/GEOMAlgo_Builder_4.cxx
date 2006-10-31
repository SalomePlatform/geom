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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// File:	GEOMAlgo_Builder_4.cxx
// Created:	
// Author:	Peter KURNEV 

#include <GEOMAlgo_Builder.hxx>

#include <TopoDS_Iterator.hxx>

#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>

#include <IntTools_Context.hxx>

#include <NMTDS_ShapesDataStructure.hxx>

#include <NMTTools_PaveFiller.hxx>
#include <NMTTools_DSFiller.hxx>

#include <GEOMAlgo_Tools3D.hxx>


static
  void MapShapes(const TopoDS_Shape& aS,
		 TopTools_MapOfShape& aM);

//=======================================================================
//function : Generated
//purpose  : 
//=======================================================================
  const TopTools_ListOfShape& GEOMAlgo_Builder::Generated(const TopoDS_Shape& theS)
{
  const NMTDS_ShapesDataStructure& aDS=myDSFiller->DS();
  NMTTools_PaveFiller* pPF=(NMTTools_PaveFiller*)&(myDSFiller->PaveFiller());
  IntTools_Context& aCtx=pPF->ChangeContext();
  //
  Standard_Boolean bHasImage, bToReverse;
  TopAbs_ShapeEnum aType;
  TopTools_ListIteratorOfListOfShape aIt;
  //
  myHistShapes.Clear();
  //
  if (theS.IsNull()) {
    return myHistShapes;
  }
  //
  bHasImage=myImages.HasImage(theS);
  if (!bHasImage) {
    return myHistShapes;
  }
  //
  aType=theS.ShapeType();
  //
  if (aType==TopAbs_EDGE   || aType==TopAbs_FACE || 
      aType==TopAbs_VERTEX || aType==TopAbs_SOLID) { 
    const TopTools_ListOfShape& aLSp=myImages.Image(theS);
    aIt.Initialize(aLSp);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aSp=aIt.Value();
      if (mySameDomainShapes.Contains(aSp)) {
	if (myMapShape.Contains(aSp)) {
	  TopoDS_Shape aSpR=mySameDomainShapes.FindFromKey(aSp);
	  //
	  if (aType==TopAbs_VERTEX || aType==TopAbs_SOLID) {
	    aSpR.Orientation(theS.Orientation());
	  }
	  else {
	    bToReverse=GEOMAlgo_Tools3D::IsSplitToReverse(aSpR, theS, aCtx);
	    if (bToReverse) {
	      aSpR.Reverse();
	    }
	  }
	  //
	  myHistShapes.Append(aSpR);
	}
      }
    }
  }
  //
  return myHistShapes;
}
//=======================================================================
//function : Modified
//purpose  : 
//=======================================================================
  const TopTools_ListOfShape& GEOMAlgo_Builder::Modified(const TopoDS_Shape& theS)
{
  const NMTDS_ShapesDataStructure& aDS=myDSFiller->DS();
  NMTTools_PaveFiller* pPF=(NMTTools_PaveFiller*)&(myDSFiller->PaveFiller());
  IntTools_Context& aCtx=pPF->ChangeContext();
  //
  Standard_Boolean bHasImage, bToReverse;
  TopAbs_ShapeEnum aType;
  TopTools_ListIteratorOfListOfShape aIt;
  //
  myHistShapes.Clear();
  //
  if (theS.IsNull()) {
    return myHistShapes;
  }
  //
  bHasImage=myImages.HasImage(theS);
  if (!bHasImage) {
    return myHistShapes;
  }
  //
  aType=theS.ShapeType();
  //
  if (aType==TopAbs_EDGE   || aType==TopAbs_FACE || 
      aType==TopAbs_VERTEX || aType==TopAbs_SOLID) { 
    const TopTools_ListOfShape& aLSp=myImages.Image(theS);
    aIt.Initialize(aLSp);
    for (; aIt.More(); aIt.Next()) {
      TopoDS_Shape aSp=aIt.Value();
      if (!mySameDomainShapes.Contains(aSp)) {
	if (myMapShape.Contains(aSp)) {
	  //
	  if (aType==TopAbs_VERTEX || aType==TopAbs_SOLID) {
	    aSp.Orientation(theS.Orientation());
	  }
	  else {
	    bToReverse=GEOMAlgo_Tools3D::IsSplitToReverse(aSp, theS, aCtx);
	    if (bToReverse) {
	      aSp.Reverse();
	    }
	  }
	  //
	  myHistShapes.Append(aSp);
	}
      }
    }
  }
  //
  return myHistShapes;
}
//=======================================================================
//function : IsDeleted
//purpose  : 
//=======================================================================
  Standard_Boolean GEOMAlgo_Builder::IsDeleted(const TopoDS_Shape& theS)
{
  Standard_Boolean bRet, bHasImage, bContains;
  TopAbs_ShapeEnum aType;
  TopTools_ListIteratorOfListOfShape aIt;
  //
  bRet=Standard_False;
  //
  if (theS.IsNull()) {
    return !bRet; //true
  }
  //
  bContains=myMapShape.Contains(theS);
  if (bContains) {
    return bRet; //false
  }
  //
  bHasImage=myImages.HasImage(theS);
  if (!bHasImage) {
    return !bRet; //true
  }
  //
  aType=theS.ShapeType();
  if (aType==TopAbs_EDGE   || aType==TopAbs_FACE || 
      aType==TopAbs_VERTEX || aType==TopAbs_SOLID) { 
    const TopTools_ListOfShape& aLSp=myImages.Image(theS);
    aIt.Initialize(aLSp);
    for (; aIt.More(); aIt.Next()) {
      TopoDS_Shape aSp=aIt.Value();
      //
      if (!mySameDomainShapes.Contains(aSp)) {
	if (myMapShape.Contains(aSp)) {
	  return bRet; //false
	}
      }
      else {
	TopoDS_Shape aSpR=mySameDomainShapes.FindFromKey(aSp);
	if (myMapShape.Contains(aSpR)) {
	  return bRet; //false
	}
      }
    }
  }
  return !bRet; // true
}
//=======================================================================
//function : PrepareHistory
//purpose  : 
//=======================================================================
  void GEOMAlgo_Builder::PrepareHistory()
{
  myHistShapes.Clear();
  myMapShape.Clear();
  myHasDeleted=Standard_False;
  myHasGenerated=Standard_False;
  myHasModified=Standard_False;
  //
  if(myShape.IsNull()) {
    return;
  }
  //
  MapShapes(myShape, myMapShape);
  //
  Standard_Boolean bHasImage, bContainsSD;
  TopAbs_ShapeEnum aType;
  TopTools_MapOfShape aMS;
  TopTools_ListIteratorOfListOfShape aIt;
  TopTools_MapIteratorOfMapOfShape aItM;
  //
  aIt.Initialize(myShapes);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS=aIt.Value();
    MapShapes(aS, aMS);
  }
  //
  aItM.Initialize(aMS);
  for (; aItM.More(); aItM.Next()) {
    const TopoDS_Shape& aSx=aItM.Key();
    aType=aSx.ShapeType();
    //
    if (!myHasDeleted) {
      myHasDeleted=IsDeleted(aSx);//xx
    }
    //
    if (!myHasGenerated || !myHasModified) {
      if (aType==TopAbs_EDGE   || aType==TopAbs_FACE || 
	  aType==TopAbs_VERTEX || aType==TopAbs_SOLID) {
	bHasImage=myImages.HasImage(aSx);
	if (bHasImage) {
	  const TopTools_ListOfShape& aLSp=myImages.Image(aSx);
	  aIt.Initialize(aLSp);
	  for (; aIt.More(); aIt.Next()) {
	    const TopoDS_Shape& aSp=aIt.Value();
	    //
	    if (myMapShape.Contains(aSp)) {
	      bContainsSD=mySameDomainShapes.Contains(aSp);
	      //
	      if (!myHasGenerated) {
		if (bContainsSD) {
		  myHasGenerated=Standard_True;
		}
	      }
	      if (!myHasModified) {
		if (!bContainsSD) {
		  myHasModified=Standard_True;
		}
	      }
	    } // if (myMapShape.Contains(aSp))
	  }
	}
      } 
    }
  }
  
}
//=======================================================================
//function : MapShapes
//purpose  : 
//=======================================================================
  void MapShapes(const TopoDS_Shape& theS,
		 TopTools_MapOfShape& theM)
{
  theM.Add(theS);
  TopoDS_Iterator anIt;
  anIt.Initialize(theS);
  for (; anIt.More(); anIt.Next()) {
    const TopoDS_Shape& aSx=anIt.Value();
    MapShapes(aSx, theM);
  }
}
