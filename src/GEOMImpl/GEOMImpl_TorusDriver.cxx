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

#include <Standard_Stream.hxx>

#include <GEOMImpl_TorusDriver.hxx>
#include <GEOMImpl_ITorus.hxx>
#include <GEOMImpl_Types.hxx>
#include <GEOM_Function.hxx>

#include <BRepPrimAPI_MakeTorus.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopAbs.hxx>
#include <TopExp.hxx>

#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <StdFail_NotDone.hxx>
#include <Standard_TypeMismatch.hxx>
#include <Standard_ConstructionError.hxx>

//=======================================================================
//function : GetID
//purpose  :
//======================================================================= 
const Standard_GUID& GEOMImpl_TorusDriver::GetID()
{
  static Standard_GUID aTorusDriver("FF1BBB12-5D14-4df2-980B-3A668264EA16");
  return aTorusDriver; 
}


//=======================================================================
//function : GEOMImpl_TorusDriver
//purpose  : 
//=======================================================================
GEOMImpl_TorusDriver::GEOMImpl_TorusDriver() 
{
}

//=======================================================================
//function : Execute
//purpose  :
//======================================================================= 
Standard_Integer GEOMImpl_TorusDriver::Execute(LOGBOOK& log) const
{
  if (Label().IsNull()) return 0;    
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  GEOMImpl_ITorus aCI (aFunction);
  Standard_Integer aType = aFunction->GetType();

  TopoDS_Shape aShape;

  if (aType == TORUS_RR) {
    aShape = BRepPrimAPI_MakeTorus(aCI.GetRMajor(), aCI.GetRMinor()).Shape();

  } else if (aType == TORUS_PNT_VEC_RR) {
    Handle(GEOM_Function) aRefPoint  = aCI.GetCenter();
    Handle(GEOM_Function) aRefVector = aCI.GetVector();
    TopoDS_Shape aShapePnt = aRefPoint->GetValue();
    TopoDS_Shape aShapeVec = aRefVector->GetValue();
    if (aShapePnt.ShapeType() != TopAbs_VERTEX) {
      Standard_TypeMismatch::Raise("Torus Center must be a vertex");
    }
    if (aShapeVec.ShapeType() != TopAbs_EDGE) {
      Standard_TypeMismatch::Raise("Torus Axis must be an edge");
    }

    gp_Pnt aP = BRep_Tool::Pnt(TopoDS::Vertex(aShapePnt));
    TopoDS_Edge anE = TopoDS::Edge(aShapeVec);
    TopoDS_Vertex V1, V2;
    TopExp::Vertices(anE, V1, V2, Standard_True);
    if (V1.IsNull() || V2.IsNull()) {
      Standard_ConstructionError::Raise("Bad edge for the Torus Axis given");
    }

    gp_Vec aV (BRep_Tool::Pnt(V1), BRep_Tool::Pnt(V2));
    if (aV.Magnitude() < Precision::Confusion()) {
      Standard_ConstructionError::Raise
        ("End vertices of edge, defining the Torus Axis, are too close");
    }

    gp_Ax2 anAxes (aP, aV);
    BRepPrimAPI_MakeTorus MT (anAxes, aCI.GetRMajor(), aCI.GetRMinor());
    if (!MT.IsDone()) MT.Build();
    if (!MT.IsDone()) StdFail_NotDone::Raise("Torus construction algorithm has failed");
    aShape = MT.Shape();
  } else {
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

bool GEOMImpl_TorusDriver::
GetCreationInformation(std::string&             theOperationName,
                       std::vector<GEOM_Param>& theParams)
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) function = GEOM_Function::GetFunction(Label());

  GEOMImpl_ITorus aCI( function );
  Standard_Integer aType = function->GetType();

  theOperationName = "TORUS";

  switch ( aType ) {
  case TORUS_RR:
    AddParam( theParams, "Radius 1", aCI.GetRMajor() );
    AddParam( theParams, "Radius 2", aCI.GetRMinor() );
    break;
  case TORUS_PNT_VEC_RR:
    AddParam( theParams, "Base Point", aCI.GetCenter() );
    AddParam( theParams, "Vector", aCI.GetVector() );
    AddParam( theParams, "Radius 1", aCI.GetRMajor() );
    AddParam( theParams, "Radius 2", aCI.GetRMinor() );
    break;
  default:
    return false;
  }

  return true;
}

OCCT_IMPLEMENT_STANDARD_RTTIEXT (GEOMImpl_TorusDriver,GEOM_BaseDriver);
