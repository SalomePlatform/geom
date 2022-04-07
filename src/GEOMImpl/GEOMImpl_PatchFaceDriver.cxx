// Copyright (C) 2007-2021  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include <GEOMImpl_PatchFaceDriver.hxx>
#include <GEOMImpl_IPatchFace.hxx>
#include <GEOMImpl_Types.hxx>
#include <GEOM_Function.hxx>
#include <GEOMUtils.hxx>

#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepTools.hxx>

#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopAbs.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>

#include <StdFail_NotDone.hxx>

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& GEOMImpl_PatchFaceDriver::GetID()
{
  static Standard_GUID aPatchFaceDriver("6E0A4C17-9E56-4739-8C67-B37C01C47ED6");
  return aPatchFaceDriver;
}


//=======================================================================
//function : GEOMImpl_PatchFaceDriver
//purpose  :
//=======================================================================
GEOMImpl_PatchFaceDriver::GEOMImpl_PatchFaceDriver()
{
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer GEOMImpl_PatchFaceDriver::Execute(Handle(TFunction_Logbook)& log) const
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());
  GEOMImpl_IPatchFace aCI(aFunction);

  Handle(GEOM_Function) aRefShape = aCI.GetShape();
  if (aRefShape.IsNull()) return 0;

  TopoDS_Shape aFaceShape = aRefShape->GetValue();

  TopoDS_Compound aCompound;
  if (aFaceShape.ShapeType() == TopAbs_FACE)
  {
    if (aFaceShape.Orientation() == TopAbs_Orientation::TopAbs_REVERSED)
      aFaceShape.Reverse();

    // Colect all Wires from face
    TopTools_ListOfShape aWires;
    TopoDS_Wire anOuterWire = BRepTools::OuterWire(TopoDS::Face(aFaceShape));
    aWires.Append(anOuterWire);
    for (TopExp_Explorer anExpW(aFaceShape, TopAbs_WIRE); anExpW.More(); anExpW.Next())
    {
      TopoDS_Wire aWire = TopoDS::Wire(anExpW.Current());
      if (anOuterWire.IsSame(aWire))
        continue;

      aWire.Reverse();
      aWires.Append(aWire);
    }

    BRep_Builder aBrepBuilder;
    TopLoc_Location aLocation;
    Handle(Geom_Surface) aSurface = BRep_Tool::Surface(TopoDS::Face(aFaceShape), aLocation);

    // Create result compound from all faces
    aBrepBuilder.MakeCompound(aCompound);
    TopTools_ListIteratorOfListOfShape anIterWires(aWires);
    for (; anIterWires.More(); anIterWires.Next())
    {
      // Create face on surface from wire 
      TopoDS_Face aFace;
      aBrepBuilder.MakeFace(aFace, aSurface, aLocation, 1.e-7);
      aBrepBuilder.Add(aFace, anIterWires.Value());

      // Add created face to compound
      aBrepBuilder.Add(aCompound, aFace);
    }
  }
  else
  {
    Standard_ConstructionError::Raise("Wrong arguments: a face must be given");
  }

  if (aCompound.IsNull()) return 0;

  aFunction->SetValue(aCompound);
  log->SetTouched(Label());
  
  return 1;
}

//================================================================================
/*!
 * \brief Returns a name of creation operation and names and values of creation parameters
 */
 //================================================================================

bool GEOMImpl_PatchFaceDriver::
GetCreationInformation(std::string&             /*theOperationName*/,
                       std::vector<GEOM_Param>& /*theParams*/)
{
  return false;
}

IMPLEMENT_STANDARD_RTTIEXT(GEOMImpl_PatchFaceDriver, GEOM_BaseDriver)
