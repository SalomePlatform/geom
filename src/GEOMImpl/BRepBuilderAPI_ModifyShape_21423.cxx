// File:	BRepBuilderAPI_ModifyShape_21423.cxx
// Created:	Fri Dec  2 10:37:08 1994
// Author:	Jacques GOUSSARD
//		<jag@topsn2>

#include <BRepBuilderAPI_ModifyShape_21423.hxx>

#include <BRepTools_Modification.hxx>

#include <TopoDS_Shape.hxx>
#include <TopTools_ListOfShape.hxx>

#include <Standard_NullObject.hxx>
#include <Standard_NoSuchObject.hxx>

//=======================================================================
//function : BRepBuilderAPI_ModifyShape_21423
//purpose  : 
//=======================================================================

BRepBuilderAPI_ModifyShape_21423::BRepBuilderAPI_ModifyShape_21423 () {}

//=======================================================================
//function : BRepBuilderAPI_ModifyShape_21423
//purpose  : 
//=======================================================================

BRepBuilderAPI_ModifyShape_21423::BRepBuilderAPI_ModifyShape_21423 (const TopoDS_Shape& S):
   myModifier(S), myInitialShape(S)
{}


//=======================================================================
//function : BRepBuilderAPI_ModifyShape_21423
//purpose  : 
//=======================================================================

BRepBuilderAPI_ModifyShape_21423::BRepBuilderAPI_ModifyShape_21423 
    (const Handle(BRepTools_Modification)& M)
{
  myModification = M;
}


//=======================================================================
//function : BRepBuilderAPI_ModifyShape_21423
//purpose  : 
//=======================================================================

BRepBuilderAPI_ModifyShape_21423::BRepBuilderAPI_ModifyShape_21423 
  (const TopoDS_Shape& S,
   const Handle(BRepTools_Modification)& M): myModifier(S),myInitialShape(S)
{
  myModification = M;
  DoModif();
}


//=======================================================================
//function : DoModif
//purpose  : 
//=======================================================================

void BRepBuilderAPI_ModifyShape_21423::DoModif ()
{
  if (myInitialShape.IsNull() || myModification.IsNull()) {
    Standard_NullObject::Raise();
  }
  myModifier.Perform(myModification);
  if (myModifier.IsDone()) {
    Done();
    myShape = myModifier.ModifiedShape(myInitialShape);
  }
  else {
    NotDone();
  }
}


//=======================================================================
//function : DoModif
//purpose  : 
//=======================================================================

void BRepBuilderAPI_ModifyShape_21423::DoModif (const TopoDS_Shape& S)
{
  if (!S.IsEqual(myInitialShape) || !IsDone()) {
    myInitialShape = S;
    myModifier.Init(S);
    DoModif();
  }
}


//=======================================================================
//function : DoModif
//purpose  : 
//=======================================================================

void BRepBuilderAPI_ModifyShape_21423::DoModif (const Handle(BRepTools_Modification)& M)
{
  myModification = M;
  DoModif();
}


//=======================================================================
//function : DoModif
//purpose  : 
//=======================================================================

void BRepBuilderAPI_ModifyShape_21423::DoModif (const TopoDS_Shape& S,
				   const Handle(BRepTools_Modification)& M)
{
  myInitialShape = S;
  myModifier.Init(S);
  myModification = M;
  DoModif();
}


//=======================================================================
//function : ModifiedShape
//purpose  : 
//=======================================================================

const TopoDS_Shape& BRepBuilderAPI_ModifyShape_21423::ModifiedShape
  (const TopoDS_Shape& S) const
{
  return myModifier.ModifiedShape(S);
}

//=======================================================================
//function : Modified
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepBuilderAPI_ModifyShape_21423::Modified
  (const TopoDS_Shape& F)
{
  myGenerated.Clear();
  myGenerated.Append(myModifier.ModifiedShape(F));
  return myGenerated;
}
