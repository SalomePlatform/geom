// Copyright (C) 2007-2021  CEA/DEN, EDF R&D, OPEN CASCADE
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

//  File:    BlockFix_BlockFixAPI.cxx
//  Created: Tue Dec  7 11:59:05 2004
//  Author:  Pavel DURANDIN

#include <BlockFix_BlockFixAPI.hxx>

#include <BlockFix.hxx>
#include <BlockFix_UnionFaces.hxx>
#include <BlockFix_UnionEdges.hxx>

#include <ShapeUpgrade_RemoveLocations.hxx>
#include <TopoDS_Edge.hxx>
#include <ShapeUpgrade_UnifySameDomain.hxx>

#include <Precision.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BlockFix_BlockFixAPI, Standard_Transient)

//=======================================================================
//function : BlockFix_BlockFixAPI
//purpose  :
//=======================================================================
BlockFix_BlockFixAPI::BlockFix_BlockFixAPI()
{
  myTolerance = Precision::Confusion();
  myOptimumNbFaces = 6;
}

//=======================================================================
//function : ~BlockFix_BlockFixAPI
//purpose  :
//=======================================================================
BlockFix_BlockFixAPI::~BlockFix_BlockFixAPI() {}

//=======================================================================
//function : Perform
//purpose  :
//=======================================================================
void BlockFix_BlockFixAPI::Perform()
{
  // processing spheres with degenerativities
  TopoDS_Shape aShape = Shape();
  myShape = BlockFix::RotateSphereSpace(aShape,myTolerance);

  // try to approximate non-canonic surfaces
  // with singularities on boundaries by filling
  myShape = BlockFix::RefillProblemFaces(myShape);

  // faces unification
  TopoDS_Shape aResult = myShape;
  if (myOptimumNbFaces > 1) {
    // use old algo BlockFix_UnionFaces for exactly given resulting number of faces
    BlockFix_UnionFaces aFaceUnifier;
    aFaceUnifier.GetTolerance() = myTolerance;
    aFaceUnifier.GetOptimumNbFaces() = myOptimumNbFaces;
    aResult = aFaceUnifier.Perform(aResult);
  }
  else if (myOptimumNbFaces != -1) {
    // use OCCT algo ShapeUpgrade_UnifySameDomain
    ShapeUpgrade_UnifySameDomain Unifier;
    //only faces
    Standard_Boolean isUnifyEdges = Standard_False;
    Standard_Boolean isUnifyFaces = Standard_True;
    Standard_Boolean isConcatBSplines = Standard_True;
    Unifier.Initialize(myShape, isUnifyEdges, isUnifyFaces, isConcatBSplines);
    //Unifier.SetLinearTolerance(myTolerance);
    Unifier.SetLinearTolerance(Precision::Confusion());
    Unifier.SetAngularTolerance(Precision::Confusion());
    Unifier.Build();
    aResult = Unifier.Shape();
  }
  else {
    // myOptimumNbFaces == -1 means do not union faces
  }

  // avoid problem with degenerated edges appearance
  // due to shape quality regress
  ShapeUpgrade_RemoveLocations RemLoc;
  RemLoc.Remove(aResult);
  aResult = RemLoc.GetResult();

  // edges unification
  //BlockFix_UnionEdges anEdgeUnifier;
  //myShape = anEdgeUnifier.Perform(aResult,myTolerance);
  ShapeUpgrade_UnifySameDomain Unifier;
  Standard_Boolean isUnifyEdges = Standard_True;
  Standard_Boolean isUnifyFaces = Standard_False; //only edges
  Standard_Boolean isConcatBSplines = Standard_True;
  Unifier.Initialize(aResult, isUnifyEdges, isUnifyFaces, isConcatBSplines);
  Unifier.SetLinearTolerance(myTolerance);
  Unifier.Build();
  myShape = Unifier.Shape();

  TopoDS_Shape aRes = BlockFix::FixRanges(myShape,myTolerance);
  myShape = aRes;
}
