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

#include <GEOMImpl_PositionDriver.hxx>

#include <GEOMImpl_IPosition.hxx>
#include <GEOMImpl_Types.hxx>

#include <GEOM_Function.hxx>

#include <GEOMUtils.hxx>

// OCCT Includes
#include <BRepBuilderAPI_Transform.hxx>
#include <ShHealOper_EdgeDivide.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <BRepFill_LocationLaw.hxx>
#include <BRepFill_Edge3DLaw.hxx>
#include <BRepFill_SectionPlacement.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <gp_Pln.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Curve.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <BRepGProp.hxx>
#include <ShapeFix_Wire.hxx>

#include <GeomFill_TrihedronLaw.hxx>
#include <GeomFill_CurveAndTrihedron.hxx>
#include <GeomFill_CorrectedFrenet.hxx>

#include <Precision.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <TopExp.hxx>

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& GEOMImpl_PositionDriver::GetID()
{
  static Standard_GUID aPositionDriver("FF1BBB69-5D14-4df2-980B-3A668264EA16");
  return aPositionDriver;
}


//=======================================================================
//function : GEOMImpl_PositionDriver
//purpose  :
//=======================================================================
GEOMImpl_PositionDriver::GEOMImpl_PositionDriver()
{
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer GEOMImpl_PositionDriver::Execute(LOGBOOK& log) const
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  GEOMImpl_IPosition aCI (aFunction);
  Standard_Integer aType = aFunction->GetType();

  TopoDS_Shape aShape;

  if (aType == POSITION_SHAPE || aType == POSITION_SHAPE_COPY) {
    Handle(GEOM_Function) aRefShape = aCI.GetShape();
    Handle(GEOM_Function) aRefStartLCS = aCI.GetStartLCS();
    Handle(GEOM_Function) aRefEndLCS = aCI.GetEndLCS();

    TopoDS_Shape aShapeBase = aRefShape->GetValue();
    TopoDS_Shape aShapeStartLCS = aRefStartLCS->GetValue();
    TopoDS_Shape aShapeEndLCS = aRefEndLCS->GetValue();

    if (aShapeBase.IsNull() || aShapeStartLCS.IsNull() ||
        aShapeEndLCS.IsNull() || aShapeEndLCS.ShapeType() != TopAbs_FACE)
      return 0;

    gp_Trsf aTrsf;
    gp_Ax3 aStartAx3, aDestAx3;

    // End LCS
    aDestAx3 = GEOMUtils::GetPosition(aShapeEndLCS);

    // Start LCS
    aStartAx3 = GEOMUtils::GetPosition(aShapeStartLCS);

    // Set transformation
    aTrsf.SetDisplacement(aStartAx3, aDestAx3);

    // Perform transformation
    BRepBuilderAPI_Transform aBRepTrsf (aShapeBase, aTrsf, Standard_False);
    aShape = aBRepTrsf.Shape();
  }
  else if (aType == POSITION_SHAPE_FROM_GLOBAL ||
           aType == POSITION_SHAPE_FROM_GLOBAL_COPY) {
    Handle(GEOM_Function) aRefShape = aCI.GetShape();
    Handle(GEOM_Function) aRefEndLCS = aCI.GetEndLCS();

    TopoDS_Shape aShapeBase = aRefShape->GetValue();
    TopoDS_Shape aShapeEndLCS = aRefEndLCS->GetValue();

    if (aShapeBase.IsNull() || aShapeEndLCS.IsNull() ||
        aShapeEndLCS.ShapeType() != TopAbs_FACE)
      return 0;

    gp_Trsf aTrsf;
    gp_Ax3 aStartAx3, aDestAx3;

    // End LCS
    aDestAx3 = GEOMUtils::GetPosition(aShapeEndLCS);

    // Set transformation
    aTrsf.SetDisplacement(aStartAx3, aDestAx3);

    // Perform transformation
    BRepBuilderAPI_Transform aBRepTrsf (aShapeBase, aTrsf, Standard_False);
    aShape = aBRepTrsf.Shape();
  }
  else if (aType == POSITION_ALONG_PATH) {
    Handle(GEOM_Function) aRefShape = aCI.GetShape();
    Handle(GEOM_Function) aPathShape = aCI.GetPath();
    Standard_Real aParameter = aCI.GetDistance();
    bool aReversed = aCI.GetReverse();
    if (aReversed)
      aParameter = 1 - aParameter;

    TopoDS_Shape aShapeBase = aRefShape->GetValue();
    TopoDS_Shape aPath = aPathShape->GetValue();
    TopoDS_Wire aWire;

    if (aShapeBase.IsNull() || aPath.IsNull())
      return 0;

    if ( aPath.ShapeType() == TopAbs_EDGE ) {
      TopoDS_Edge anEdge = TopoDS::Edge(aPath);
      aWire = BRepBuilderAPI_MakeWire(anEdge); 
    }
    else if ( aPath.ShapeType() == TopAbs_WIRE)
      aWire = TopoDS::Wire(aPath);
    else
      return 0;

    Handle(GeomFill_TrihedronLaw) TLaw = new GeomFill_CorrectedFrenet();
    Handle(GeomFill_CurveAndTrihedron) aLocationLaw = new GeomFill_CurveAndTrihedron( TLaw );
    Handle(BRepFill_LocationLaw) aLocation = new BRepFill_Edge3DLaw(aWire, aLocationLaw);

    aLocation->TransformInCompatibleLaw( 0.01 );

    //Calculate a Parameter
    Standard_Real aFirstParam1 = 0, aLastParam1 = 0; // Parameters of the First edge
    Standard_Real aFirstParam2 = 0, aLastParam2 = 0; // Parameters of the Last edge
    aLocation->CurvilinearBounds(aLocation->NbLaw(), aFirstParam2, aLastParam2);

    if ( aLocation->NbLaw() > 1)
      aLocation->CurvilinearBounds(1, aFirstParam1, aLastParam1);
    else if ( aLocation->NbLaw() == 1 )
      aFirstParam1 = aFirstParam2;
    else
      return 0;

    Standard_Real aParam = (aFirstParam1 + (aLastParam2 - aFirstParam1)*aParameter );

    TopoDS_Shape CopyShape = aShapeBase;
    BRepFill_SectionPlacement Place( aLocation, aShapeBase );
    TopLoc_Location Loc2(Place.Transformation()), Loc1;
    Loc1 = CopyShape.Location();
    CopyShape.Location(Loc2.Multiplied(Loc1));

    aLocation->D0( aParam, CopyShape );
    aShape = CopyShape;
  }
  else
    return 0;

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

bool GEOMImpl_PositionDriver::
GetCreationInformation(std::string&             theOperationName,
                       std::vector<GEOM_Param>& theParams)
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) function = GEOM_Function::GetFunction(Label());

  GEOMImpl_IPosition aCI( function );
  Standard_Integer aType = function->GetType();

  theOperationName = "MODIFY_LOCATION";

  switch ( aType ) {
  case POSITION_SHAPE:
  case POSITION_SHAPE_COPY:
    AddParam( theParams, "Object", aCI.GetShape() );
    AddParam( theParams, "Start LCS", aCI.GetStartLCS() );
    AddParam( theParams, "End LCS", aCI.GetEndLCS() );
    break;
  case POSITION_SHAPE_FROM_GLOBAL:
  case POSITION_SHAPE_FROM_GLOBAL_COPY:
    AddParam( theParams, "Object", aCI.GetShape() );
    AddParam( theParams, "End LCS", aCI.GetEndLCS() );
    break;
  case POSITION_ALONG_PATH:
    AddParam( theParams, "Object", aCI.GetShape() );
    AddParam( theParams, "Path", aCI.GetPath() );
    AddParam( theParams, "Distance", aCI.GetDistance() );
    AddParam( theParams, "Reverse Direction", aCI.GetReverse() );
    break;
  default:
    return false;
  }
  
  return true;
}

OCCT_IMPLEMENT_STANDARD_RTTIEXT (GEOMImpl_PositionDriver,GEOM_BaseDriver);
