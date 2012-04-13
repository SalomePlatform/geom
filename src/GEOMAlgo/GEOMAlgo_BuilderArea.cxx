// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

// File:        GEOMAlgo_BuilderArea.cxx
// Author:      Peter KURNEV

#include <GEOMAlgo_BuilderArea.ixx>

#include <Basics_OCCTVersion.hxx>

#include <TopTools_ListIteratorOfListOfShape.hxx>

#include <TopoDS_Shape.hxx>

#include <IntTools_Context.hxx>

//=======================================================================
//function :
//purpose  :
//=======================================================================
GEOMAlgo_BuilderArea::GEOMAlgo_BuilderArea()
  : GEOMAlgo_Algo()
{
#if OCC_VERSION_LARGE <= 0x06050200
  myContext=NULL;
#endif
}
//=======================================================================
//function : ~
//purpose  :
//=======================================================================
GEOMAlgo_BuilderArea::~GEOMAlgo_BuilderArea()
{
}
//=======================================================================
//function : SetContext
//purpose  :
//=======================================================================
#if OCC_VERSION_LARGE > 0x06050200
void GEOMAlgo_BuilderArea::SetContext(const Handle(IntTools_Context)& theContext)
#else
void GEOMAlgo_BuilderArea::SetContext(const IntTools_Context& theContext)
#endif
{
#if OCC_VERSION_LARGE > 0x06050200
  myContext=theContext;
#else
  myContext=(IntTools_Context*)&theContext;
#endif
}

#if OCC_VERSION_LARGE > 0x06050200
//=======================================================================
//function : Context
//purpose  :
//=======================================================================
const Handle(IntTools_Context)& GEOMAlgo_BuilderArea::Context()const
{
  return myContext;
}
//=======================================================================
//function : Perform
//purpose  :
//=======================================================================
void GEOMAlgo_BuilderArea::Perform()
{
  if (myContext.IsNull()) {
    myContext=new IntTools_Context;
  }
}
#endif

//=======================================================================
//function : SetShapes
//purpose  :
//=======================================================================
void GEOMAlgo_BuilderArea::SetShapes(const TopTools_ListOfShape& theLF)
{
  TopTools_ListIteratorOfListOfShape aIt;
  //
  myShapes.Clear();
  aIt.Initialize(theLF);
  for(; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aF=aIt.Value();
    myShapes.Append(aF);
  }
}
//=======================================================================
//function : Shapes
//purpose  :
//=======================================================================
const TopTools_ListOfShape& GEOMAlgo_BuilderArea::Shapes()const
{
  return myShapes;
}
//=======================================================================
//function : Loops
//purpose  :
//=======================================================================
const TopTools_ListOfShape& GEOMAlgo_BuilderArea::Loops()const
{
  return myLoops;
}
//=======================================================================
//function : Areas
//purpose  :
//=======================================================================
const TopTools_ListOfShape& GEOMAlgo_BuilderArea::Areas()const
{
  return myAreas;
}
//=======================================================================
//function :PerformShapesToAvoid
//purpose  :
//=======================================================================
void GEOMAlgo_BuilderArea::PerformShapesToAvoid()
{
}
//=======================================================================
//function : PerformLoops
//purpose  :
//=======================================================================
void GEOMAlgo_BuilderArea::PerformLoops()
{
}
//=======================================================================
//function : PerformAreas
//purpose  :
//=======================================================================
void GEOMAlgo_BuilderArea::PerformAreas()
{
}
//=======================================================================
//function : PerformInternalShapes
//purpose  :
//=======================================================================
void GEOMAlgo_BuilderArea::PerformInternalShapes()
{
}
