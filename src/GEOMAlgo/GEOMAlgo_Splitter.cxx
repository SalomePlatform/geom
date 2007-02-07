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
// File:	GEOMAlgo_Splitter.cxx
// Created:	
// Author:	Peter KURNEV
//		<pkv@irinox>

#include <GEOMAlgo_Splitter.ixx>

#include <TopAbs_ShapeEnum.hxx>

#include <TopoDS_Shape.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Iterator.hxx>

#include <TopExp.hxx>

#include <BRep_Builder.hxx>
#include <BRepLib.hxx>

#include <TopTools_MapOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#include <BOP_CorrectTolerances.hxx>

#include <NMTTools_DSFiller.hxx>

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
  GEOMAlgo_Splitter::GEOMAlgo_Splitter()
:
  GEOMAlgo_Builder()
{
  myLimit=TopAbs_SHAPE;
}
//=======================================================================
//function : ~
//purpose  : 
//=======================================================================
  GEOMAlgo_Splitter::~GEOMAlgo_Splitter()
{
}
//=======================================================================
//function : AddToolCompound
//purpose  : 
//=======================================================================
  void GEOMAlgo_Splitter::AddToolCompound(const TopoDS_Shape& theShape)
{
  TopoDS_Iterator aIt;
  //
  aIt.Initialize(theShape);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS=aIt.Value();
    AddTool(aS);
  }
}
//=======================================================================
//function : AddTool
//purpose  : 
//=======================================================================
  void GEOMAlgo_Splitter::AddTool(const TopoDS_Shape& theShape)
{
  if (myMapTools.Add(theShape)) {
    myTools.Append(theShape);
    //
    AddShape(theShape);
  }
}
//=======================================================================
//function : Tools
//purpose  : 
//=======================================================================
  const TopTools_ListOfShape& GEOMAlgo_Splitter::Tools()const
{
  return myTools;
}
//=======================================================================
//function : SetLimit
//purpose  : 
//=======================================================================
  void GEOMAlgo_Splitter::SetLimit(const TopAbs_ShapeEnum aLimit) 
{
  myLimit=aLimit;
}
//=======================================================================
//function : Limit
//purpose  : 
//=======================================================================
  TopAbs_ShapeEnum GEOMAlgo_Splitter::Limit()const
{
  return myLimit;
}
//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================
  void GEOMAlgo_Splitter::Clear()
{
  myTools.Clear();
  myMapTools.Clear();
  myLimit=TopAbs_SHAPE;
  GEOMAlgo_Builder::Clear();
}
//=======================================================================
//function : BuildResult
//purpose  : 
//=======================================================================
  void GEOMAlgo_Splitter::BuildResult(const TopAbs_ShapeEnum theType)
{
  myErrorStatus=0;
  //
  TopAbs_ShapeEnum aType;
  BRep_Builder aBB;
  TopTools_MapOfShape aM;
  TopTools_ListIteratorOfListOfShape aIt, aItIm;
  //
  aIt.Initialize(myShapes);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS=aIt.Value();
    aType=aS.ShapeType();
    if (aType==theType && !myMapTools.Contains(aS)) {
      if (myImages.HasImage(aS)) {
	const TopTools_ListOfShape& aLSIm=myImages.Image(aS);
	aItIm.Initialize(aLSIm);
	for (; aItIm.More(); aItIm.Next()) {
	  const TopoDS_Shape& aSIm=aItIm.Value();
	  if (aM.Add(aSIm)) {
	    aBB.Add(myShape, aSIm);
	  }
	}
      }
      else {
	if (aM.Add(aS)) {
	  aBB.Add(myShape, aS);
	}
      }
    }
  }
}
//=======================================================================
//function : PostTreat
//purpose  : 
//=======================================================================
  void GEOMAlgo_Splitter::PostTreat()
{
  if (myLimit!=TopAbs_SHAPE) {
    Standard_Integer i, aNbS;
    BRep_Builder aBB;
    TopoDS_Compound aC;
    TopTools_IndexedMapOfShape aM;
    //
    aBB.MakeCompound(aC);
    //
    TopExp::MapShapes(myShape, myLimit, aM);
    aNbS=aM.Extent();
    for (i=1; i<=aNbS; ++i) {
      const TopoDS_Shape& aS=aM(i);
      aBB.Add(aC, aS);
    }
    myShape=aC;
  }
  //
  GEOMAlgo_Builder::PostTreat();
  //
}
//
// myErrorStatus
// 
// 0  - Ok
// 1  - The object is just initialized
// 2  - DSFiller is failed
// 10 - No shapes to process
// 30 - SolidBuilder failed
