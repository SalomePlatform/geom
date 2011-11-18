// File:	BRepBuilderAPI_NurbsConvert_21423.cxx
// Created:	Fri Dec  9 09:14:55 1994
// Author:	Jacques GOUSSARD
//		<jag@topsn2>

#include <BRepBuilderAPI_NurbsConvert_21423.hxx>

#include <TopoDS_Shape.hxx>

#include <BRepLib.hxx>
#include <TopAbs.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopExp_Explorer.hxx>
#include <BRep_Builder.hxx>

#include <BRepTools_NurbsConvertModification_21423.hxx>

//=======================================================================
//function : BRepBuilderAPI_NurbsConvert_21423
//purpose  : 
//=======================================================================

BRepBuilderAPI_NurbsConvert_21423::BRepBuilderAPI_NurbsConvert_21423 () 
     
{
  myModification = new BRepTools_NurbsConvertModification_21423();
}

//=======================================================================
//function : BRepBuilderAPI_NurbsConvert_21423
//purpose  : 
//=======================================================================

BRepBuilderAPI_NurbsConvert_21423::BRepBuilderAPI_NurbsConvert_21423 (const TopoDS_Shape& S,
					    const Standard_Boolean Copy) 
     
{
  myModification = new BRepTools_NurbsConvertModification_21423();
  Perform(S,Copy);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void BRepBuilderAPI_NurbsConvert_21423::Perform(const TopoDS_Shape& S,
				   const Standard_Boolean Copy)
{
  Handle(BRepTools_NurbsConvertModification_21423) theModif = 
    Handle(BRepTools_NurbsConvertModification_21423)::DownCast(myModification);
  DoModif(S,myModification);
}



