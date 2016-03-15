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

// File:      ShHealOper_ChangeOrientation.cxx
// Created:   11.07.06 11:46:45
// Author:    Sergey KUUL

#include <ShHealOper_ChangeOrientation.hxx>

#include <BRepBuilderAPI_Copy.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRep_Builder.hxx>
#include <Geom_Curve.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Iterator.hxx>
#include <Geom_TrimmedCurve.hxx>

//=======================================================================
//function : ShHealOper_ChangeOrientation()
//purpose  : Constructor
//=======================================================================

ShHealOper_ChangeOrientation::ShHealOper_ChangeOrientation ( const TopoDS_Shape& theShape )
{
  Init(theShape);
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void ShHealOper_ChangeOrientation::Init(const TopoDS_Shape& theShape)
{
  ShHealOper_Tool::Init(theShape);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

Standard_Boolean ShHealOper_ChangeOrientation::Perform()
{
  BRep_Builder B;
  if (myInitShape.ShapeType() == TopAbs_SHELL)
  {
    myResultShape = myInitShape.EmptyCopied();
    TopoDS_Iterator itr (myInitShape);
    while (itr.More()) {
      B.Add(myResultShape,itr.Value().Reversed());
      itr.Next();
      myStatistics.AddModif("Face reversed");
    }
  }
  else if (myInitShape.ShapeType() == TopAbs_FACE)
  {
    myResultShape = myInitShape.EmptyCopied();
    TopoDS_Iterator itr (myInitShape);
    while (itr.More()) {
      B.Add(myResultShape,itr.Value());
      itr.Next();
      myStatistics.AddModif("Wire reversed");
    }
    myResultShape.Reverse();
  }
  else if ( myInitShape.ShapeType() == TopAbs_WIRE || myInitShape.ShapeType() == TopAbs_EDGE)
  {
    TopTools_ListOfShape reversedEdges;
    for ( TopExp_Explorer edgeIt( myInitShape, TopAbs_EDGE ); edgeIt.More(); edgeIt.Next() )
    {
      const TopoDS_Edge& edge = TopoDS::Edge( edgeIt.Current() );

      double f,l;
      Handle(Geom_Curve) curve = BRep_Tool::Curve( edge, f,l );
      Handle(Geom_TrimmedCurve) tc = Handle(Geom_TrimmedCurve)::DownCast(curve);
      if ( !tc.IsNull() ) curve = tc->BasisCurve();

      f = curve->ReversedParameter( f );
      l = curve->ReversedParameter( l );
      curve = curve->Reversed();
      reversedEdges.Prepend( BRepBuilderAPI_MakeEdge( curve, Min( f, l ), Max( f, l )));
    }
    if ( myInitShape.ShapeType() == TopAbs_EDGE )
    {
      myResultShape = reversedEdges.First();
      myStatistics.AddModif("Edge reversed");
    }
    else
    {
      BRepBuilderAPI_MakeWire wire;
      wire.Add( reversedEdges );
      myResultShape = wire;
      myStatistics.AddModif("Wire reversed");
    }
    // myResultShape = myInitShape.EmptyCopied();
    // TopoDS_Iterator itr (myInitShape);
    // while (itr.More()) {
    //   B.Add(myResultShape,itr.Value());
    //   itr.Next();
    // }
    // myResultShape.Reverse();
  }
  else
  {
    BRepBuilderAPI_Copy Copy (myInitShape);
    if (!Copy.IsDone()) return false;

    myResultShape = Copy.Shape();
    if (myResultShape.IsNull()) return false;

    if (myResultShape.Orientation() == TopAbs_FORWARD)
      myResultShape.Orientation(TopAbs_REVERSED);
    else
      myResultShape.Orientation(TopAbs_FORWARD);

    switch( myResultShape.ShapeType() ) {
    case TopAbs_SOLID    : myStatistics.AddModif("Solid reversed"); break;
    case TopAbs_COMPSOLID: myStatistics.AddModif("Compsolid reversed"); break;
    case TopAbs_COMPOUND : myStatistics.AddModif("Compound reversed"); break;
    default:;
    }
  }

  return true;
}
