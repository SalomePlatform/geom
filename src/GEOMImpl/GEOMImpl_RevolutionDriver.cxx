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

#include <GEOMImpl_RevolutionDriver.hxx>

#include <GEOMImpl_IRevolution.hxx>
#include <GEOMImpl_Types.hxx>

#include <GEOM_Function.hxx>

#include <GEOMUtils.hxx>

#include <BRepPrimAPI_MakeRevol.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <gp_Trsf.hxx>
#include <gp_Pnt.hxx>
#include <gp_Lin.hxx>
#include <gp_Dir.hxx>
#include <Precision.hxx>
#include <StdFail_NotDone.hxx>
#include <Standard_TypeMismatch.hxx>
#include <Standard_ConstructionError.hxx>

//=======================================================================
//function : GetID
//purpose  :
//======================================================================= 
const Standard_GUID& GEOMImpl_RevolutionDriver::GetID()
{
  static Standard_GUID aRevolutionDriver("FF1BBB18-5D14-4df2-980B-3A668264EA16");
  return aRevolutionDriver; 
}


//=======================================================================
//function : GEOMImpl_RevolutionDriver
//purpose  : 
//=======================================================================
GEOMImpl_RevolutionDriver::GEOMImpl_RevolutionDriver() 
{
}

//=======================================================================
//function : Execute
//purpose  :
//======================================================================= 
Standard_Integer GEOMImpl_RevolutionDriver::Execute(LOGBOOK& log) const
{
  if (Label().IsNull()) return 0;    
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  GEOMImpl_IRevolution aCI (aFunction);
  Standard_Integer aType = aFunction->GetType();

  TopoDS_Shape aShape;

  if (aType == REVOLUTION_BASE_AXIS_ANGLE || aType == REVOLUTION_BASE_AXIS_ANGLE_2WAYS) {
    Handle(GEOM_Function) aRefBase = aCI.GetBase();
    Handle(GEOM_Function) aRefAxis = aCI.GetAxis();
    TopoDS_Shape aShapeBase = aRefBase->GetValue();
    TopoDS_Shape aShapeAxis = aRefAxis->GetValue();
    if (aShapeAxis.ShapeType() != TopAbs_EDGE) {
      Standard_TypeMismatch::Raise("Revolution Axis must be an edge");
    }

    TopoDS_Edge anE = TopoDS::Edge(aShapeAxis);
    TopoDS_Vertex V1, V2;
    TopExp::Vertices(anE, V1, V2, Standard_True);
    if (V1.IsNull() || V2.IsNull()) {
      Standard_ConstructionError::Raise("Bad edge for the Revolution Axis given");
    }

    gp_Vec aV (BRep_Tool::Pnt(V1), BRep_Tool::Pnt(V2));
    if (aV.Magnitude() < Precision::Confusion()) {
      Standard_ConstructionError::Raise
        ("End vertices of edge, defining the Revolution Axis, are too close");
    }

    if (aShapeBase.ShapeType() == TopAbs_VERTEX) {
      gp_Lin aL(BRep_Tool::Pnt(V1), gp_Dir(aV));
      Standard_Real d = aL.Distance(BRep_Tool::Pnt(TopoDS::Vertex(aShapeBase)));
      if (d < Precision::Confusion()) {
        Standard_ConstructionError::Raise("Vertex to be rotated is too close to Revolution Axis");
      }
    }
    double anAngle = aCI.GetAngle();
    gp_Ax1 anAxis (BRep_Tool::Pnt(V1), aV);
    if (aType == REVOLUTION_BASE_AXIS_ANGLE_2WAYS)
      {
        gp_Trsf aTrsf;
        aTrsf.SetRotation(anAxis, ( -anAngle ));
        BRepBuilderAPI_Transform aTransformation(aShapeBase, aTrsf, Standard_False);
        aShapeBase = aTransformation.Shape();
        anAngle = anAngle * 2;
      }
    BRepPrimAPI_MakeRevol MR (aShapeBase, anAxis, anAngle, Standard_False);
    if (!MR.IsDone()) MR.Build();
    if (!MR.IsDone()) StdFail_NotDone::Raise("Revolution algorithm has failed");
    aShape = MR.Shape();
  } else {
  }

  if (aShape.IsNull()) return 0;

  TopoDS_Shape aRes = GEOMUtils::CompsolidToCompound(aShape);
  aFunction->SetValue(aRes);

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

bool GEOMImpl_RevolutionDriver::
GetCreationInformation(std::string&             theOperationName,
                       std::vector<GEOM_Param>& theParams)
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) function = GEOM_Function::GetFunction(Label());

  GEOMImpl_IRevolution aCI( function );
  Standard_Integer aType = function->GetType();

  theOperationName = "REVOLUTION";

  switch ( aType ) {
  case REVOLUTION_BASE_AXIS_ANGLE:
  case REVOLUTION_BASE_AXIS_ANGLE_2WAYS:
    AddParam( theParams, "Object", aCI.GetBase() );
    AddParam( theParams, "Axis", aCI.GetAxis() );
    AddParam( theParams, "Angle", aCI.GetAngle() );
    AddParam( theParams, "Both Directions", aType == REVOLUTION_BASE_AXIS_ANGLE_2WAYS );
    break;
  default:
    return false;
  }
  
  return true;
}

OCCT_IMPLEMENT_STANDARD_RTTIEXT (GEOMImpl_RevolutionDriver,GEOM_BaseDriver);
