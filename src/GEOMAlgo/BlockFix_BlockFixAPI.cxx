// File:	BlockFix_BlockFixAPI.cxx
// Created:	Tue Dec  7 11:59:05 2004
// Author:	Pavel DURANDIN
// Copyright:	Open CASCADE SA 2004


#include <BlockFix_BlockFixAPI.ixx>
#include <BlockFix.hxx>
#include <BlockFix_UnionFaces.hxx>
#include <BlockFix_UnionEdges.hxx>
#include <Precision.hxx>

//=======================================================================
//function : ShapeConvert_CanonicAPI
//purpose  : 
//=======================================================================

BlockFix_BlockFixAPI::BlockFix_BlockFixAPI()
{
  myTolerance = Precision::Confusion();
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void BlockFix_BlockFixAPI::Perform() 
{

  // processing spheres with degenerativities
  TopoDS_Shape aShape = Shape();
  myShape = BlockFix::RotateSphereSpace(aShape,myTolerance);
  
  // faces unification
  BlockFix_UnionFaces aFaceUnifier;
  aFaceUnifier.GetTolerance() = myTolerance;
  TopoDS_Shape aResult;
  aResult = aFaceUnifier.Perform(myShape);
  
  
  BlockFix_UnionEdges anEdgeUnifier;
  myShape = anEdgeUnifier.Perform(aResult,myTolerance);
  
  TopoDS_Shape aRes = BlockFix::FixRanges(myShape,myTolerance);
  myShape = aRes;
  
}
