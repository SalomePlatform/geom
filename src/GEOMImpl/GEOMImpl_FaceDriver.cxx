// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include <GEOMImpl_FaceDriver.hxx>

#include <GEOMImpl_IFace.hxx>
#include <GEOMImpl_Types.hxx>

#include <GEOM_Function.hxx>

#include <GEOMUtils.hxx>

#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRep_Tool.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pln.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopAbs.hxx>
#include <TopExp.hxx>

#include <StdFail_NotDone.hxx>

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& GEOMImpl_FaceDriver::GetID()
{
  static Standard_GUID aFaceDriver("F7953CC1-FF8B-4628-BF5E-9D3510DE4629");
  return aFaceDriver;
}


//=======================================================================
//function : GEOMImpl_FaceDriver
//purpose  :
//=======================================================================
GEOMImpl_FaceDriver::GEOMImpl_FaceDriver()
{
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer GEOMImpl_FaceDriver::Execute(LOGBOOK& log) const
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  GEOMImpl_IFace aFI (aFunction);
  Standard_Integer aType = aFunction->GetType();

  TopoDS_Shape aShape;

  if (aType == FACE_OBJ_H_W) {
    Handle(GEOM_Function) aRefFunct = aFI.GetRef1();
    TopoDS_Shape aRefShape = aRefFunct->GetValue();
    if (aRefShape.ShapeType() == TopAbs_EDGE) {
      TopoDS_Edge anEdge = TopoDS::Edge(aRefShape);
      double aH = aFI.GetH() / 2.0;
      double aW = aFI.GetW() / 2.0;
      TopoDS_Vertex V1, V2;
      TopExp::Vertices(anEdge, V1, V2, Standard_True);
      gp_Pnt aP (BRep_Tool::Pnt(V1));
      gp_Vec aV (BRep_Tool::Pnt(V1), BRep_Tool::Pnt(V2));
      gp_Pln aPlane (aP, aV);
      aShape = BRepBuilderAPI_MakeFace(aPlane, -aH, +aH, -aW, +aW).Shape();
    } else if (aRefShape.ShapeType() == TopAbs_FACE) {
      double aH = aFI.GetH() / 2.0;
      double aW = aFI.GetW() / 2.0;
      gp_Ax3 anAx = GEOMUtils::GetPosition(aRefShape);
      gp_Pln aPln (anAx);
      aShape = BRepBuilderAPI_MakeFace(aPln, -aH, +aH, -aW, +aW).Shape();
    }
  }
  else if (aType == FACE_H_W) {
    double aH = aFI.GetH() / 2.0;
    double aW = aFI.GetW() / 2.0;
    TopoDS_Vertex V1, V2;
    int anOrient = aFI.GetOrientation();
    gp_Pnt aP = gp::Origin();
    gp_Vec aV;
    if (anOrient == 1)
      aV = gp::DZ();
    else if (anOrient == 2)
      aV = gp::DX();
    else if (anOrient == 3)
      aV = gp::DY();

    gp_Pln aPlane (aP, aV);
    aShape = BRepBuilderAPI_MakeFace(aPlane, -aH, +aH, -aW, +aW).Shape();
  }
  else {
  }

  if (aShape.IsNull()) return 0;

  aFunction->SetValue(aShape);

#if OCC_VERSION_MAJOR < 7
  log.SetTouched(Label());
#else
  log->SetTouched(Label());
#endif

  return 1;
}

//================================================================================
/*!
 * \brief Returns a name of creation operation and names and values of creation parameters
 */
//================================================================================

bool GEOMImpl_FaceDriver::
GetCreationInformation(std::string&             theOperationName,
                       std::vector<GEOM_Param>& theParams)
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) function = GEOM_Function::GetFunction(Label());

  GEOMImpl_IFace aCI( function );
  Standard_Integer aType = function->GetType();

  theOperationName = "RECTANGLE";

  switch ( aType ) {
  case FACE_OBJ_H_W:
  {
    Handle(GEOM_Function) aRefFunct = aCI.GetRef1();
    TopoDS_Shape aRefShape = aRefFunct->GetValue();
    if (aRefShape.ShapeType() == TopAbs_EDGE)
      AddParam( theParams, "Edge", aRefFunct );
    else
      AddParam( theParams, "Face", aRefFunct );
    AddParam( theParams, "Height", aCI.GetH() );
    AddParam( theParams, "Width", aCI.GetW() );
    break;
  }
  case FACE_H_W:
    AddParam( theParams, "Height", aCI.GetH() );
    AddParam( theParams, "Width", aCI.GetW() );
    AddParam( theParams, "Orientation", aCI.GetOrientation() );
    break;
  default:
    return false;
  }
  
  return true;
}

OCCT_IMPLEMENT_STANDARD_RTTIEXT (GEOMImpl_FaceDriver,GEOM_BaseDriver);
