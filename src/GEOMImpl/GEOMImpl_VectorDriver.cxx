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

#include "GEOMImpl_VectorDriver.hxx"

#include "GEOMImpl_IVector.hxx"
#include "GEOMImpl_Types.hxx"
#include "GEOM_Function.hxx"
#include "GEOM_Object.hxx"

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRep_Tool.hxx>
#include <Geom_Curve.hxx>
#include <Precision.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

#include <Standard_ConstructionError.hxx>

namespace
{
  Handle(GEOM_Object) GetOwner( const TDF_Label& l )
  {
    TDF_Label label = l;
    // object is stored on a grandfather label of a driver label
    if ( !label.IsNull() )
      label = label.Father();
    if ( !label.IsNull() )
      label = label.Father();
    
    return GEOM_Object::GetObject( label );
  }
}

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& GEOMImpl_VectorDriver::GetID()
{
  static Standard_GUID aVectorDriver("FF1BBB04-5D14-4df2-980B-3A668264EA16");
  return aVectorDriver;
}


//=======================================================================
//function : GEOMImpl_VectorDriver
//purpose  :
//=======================================================================
GEOMImpl_VectorDriver::GEOMImpl_VectorDriver()
{
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer GEOMImpl_VectorDriver::Execute(LOGBOOK& log) const
{
  if (Label().IsNull())  return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  GEOMImpl_IVector aPI (aFunction);
  Standard_Integer aType = aFunction->GetType();
  if (aType != VECTOR_DX_DY_DZ && aType != VECTOR_TWO_PNT &&
      aType != VECTOR_TANGENT_CURVE_PAR && aType != VECTOR_REVERSE) return 0;

  TopoDS_Shape aShape;

  if (aType == VECTOR_DX_DY_DZ) {
    gp_Pnt P1 = gp::Origin();
    gp_Pnt P2 (aPI.GetDX(), aPI.GetDY(), aPI.GetDZ());
    if (P1.Distance(P2) < Precision::Confusion()) {
      TCollection_AsciiString aMsg ("Can not build vector with length, less than ");
      aMsg += TCollection_AsciiString(Precision::Confusion());
      Standard_ConstructionError::Raise(aMsg.ToCString());
    }
    aShape = BRepBuilderAPI_MakeEdge(P1, P2).Shape();
  }
  else if (aType == VECTOR_TWO_PNT) {
    Handle(GEOM_Function) aRefPnt1 = aPI.GetPoint1();
    Handle(GEOM_Function) aRefPnt2 = aPI.GetPoint2();
    TopoDS_Shape aShape1 = aRefPnt1->GetValue();
    TopoDS_Shape aShape2 = aRefPnt2->GetValue();
    if (aShape1.ShapeType() != TopAbs_VERTEX ||
        aShape2.ShapeType() != TopAbs_VERTEX) return 0;
    if (aShape1.IsSame(aShape2)) {
      Standard_ConstructionError::Raise("The end points must be different");
    }
    TopoDS_Vertex V1 = TopoDS::Vertex(aShape1);
    TopoDS_Vertex V2 = TopoDS::Vertex(aShape2);
    gp_Pnt P1 = BRep_Tool::Pnt(V1);
    gp_Pnt P2 = BRep_Tool::Pnt(V2);
    if (P1.Distance(P2) < Precision::Confusion()) {
      Standard_ConstructionError::Raise("The end points are too close");
    }
    aShape = BRepBuilderAPI_MakeEdge(V1, V2).Shape();
  } 
  else if (aType == VECTOR_TANGENT_CURVE_PAR) {
    Handle(GEOM_Function) aRefCurve = aPI.GetCurve();
    TopoDS_Shape aRefShape = aRefCurve->GetValue();
    if (aRefShape.ShapeType() != TopAbs_EDGE) {
      Standard_TypeMismatch::Raise
        ("Tangent On Curve creation aborted : curve shape is not an edge");
    }
    Standard_Real aFParam =0., aLParam =0., aParam =0.;
    Handle(Geom_Curve) aCurve = BRep_Tool::Curve(TopoDS::Edge(aRefShape), aFParam, aLParam);
    if(aCurve.IsNull()) {
      Standard_TypeMismatch::Raise
        ("Tangent On Curve creation aborted : curve is null");
    }

    aParam = aFParam + (aLParam - aFParam) * aPI.GetParameter();
    gp_Pnt aPoint1,aPoint2;
    gp_Vec aVec;
    aCurve->D1(aParam,aPoint1,aVec);
    if(aVec.Magnitude() < gp::Resolution())
      Standard_TypeMismatch::Raise
        ("Tangent On Curve creation aborted : invalid value of tangent");
    aPoint2.SetXYZ(aPoint1.XYZ() + aVec.XYZ());
    BRepBuilderAPI_MakeEdge aBuilder(aPoint1,aPoint2);
    if(aBuilder.IsDone())
      aShape = aBuilder.Shape();
  }
  else if (aType == VECTOR_REVERSE) {
    Handle(GEOM_Function) aRefVec = aPI.GetCurve();
    TopoDS_Shape aRefShape = aRefVec->GetValue();
    if (aRefShape.ShapeType() != TopAbs_EDGE) {
      Standard_TypeMismatch::Raise
        ("Reversed vector creation aborted : vector shape is not an edge");
    }
    TopoDS_Edge anE = TopoDS::Edge(aRefShape);
    TopoDS_Vertex V1, V2;
    TopExp::Vertices(anE, V1, V2, Standard_True);
    aShape = BRepBuilderAPI_MakeEdge(V2, V1).Shape();
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

bool GEOMImpl_VectorDriver::
GetCreationInformation(std::string&             theOperationName,
                       std::vector<GEOM_Param>& theParams)
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) function = GEOM_Function::GetFunction(Label());

  GEOMImpl_IVector aCI( function );
  Standard_Integer aType = function->GetType();

  switch ( aType ) {
  case VECTOR_DX_DY_DZ:
    theOperationName = "VECTOR";
    AddParam( theParams, "Dx", aCI.GetDX() );
    AddParam( theParams, "Dy", aCI.GetDY() );
    AddParam( theParams, "Dz", aCI.GetDZ() );
    break;
  case VECTOR_TWO_PNT: {
    Handle(GEOM_Object) obj = GetOwner( Label() );
    if ( !obj.IsNull() && obj->GetType() == GEOM_EDGE )
      theOperationName = "EDGE";
    else
      theOperationName = "VECTOR";
    AddParam( theParams, "Point 1", aCI.GetPoint1() );
    AddParam( theParams, "Point 2", aCI.GetPoint2() );
    break;
  }
  case VECTOR_TANGENT_CURVE_PAR:
    theOperationName = "MakeTangentOnCurve";
    AddParam( theParams, "Curve", aCI.GetCurve() );
    AddParam( theParams, "Parameter", aCI.GetParameter() );
    break;
  case VECTOR_REVERSE:
    theOperationName = "CHANGE_ORIENTATION";
    AddParam( theParams, "Vector", aCI.GetCurve() );
    break;
  default:
    return false;
  }

  return true;
}

OCCT_IMPLEMENT_STANDARD_RTTIEXT (GEOMImpl_VectorDriver,GEOM_BaseDriver);
