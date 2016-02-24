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

#include <GEOMImpl_ScaleDriver.hxx>
#include <GEOMImpl_IScale.hxx>
#include <GEOMImpl_Types.hxx>
#include <GEOMUtils.hxx>
#include <GEOM_Function.hxx>

#include <BRepBuilderAPI_Transform.hxx>
#include <BRepBuilderAPI_GTransform.hxx>
#include <BRep_Tool.hxx>

#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>

#include <Precision.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_GTrsf.hxx>

//=======================================================================
//function : GetID
//purpose  :
//======================================================================= 
const Standard_GUID& GEOMImpl_ScaleDriver::GetID()
{
  static Standard_GUID aScaleDriver("FF1BBB52-5D14-4df2-980B-3A668264EA16");
  return aScaleDriver; 
}


//=======================================================================
//function : GEOMImpl_ScaleDriver
//purpose  : 
//=======================================================================
GEOMImpl_ScaleDriver::GEOMImpl_ScaleDriver() 
{
}

//=======================================================================
//function : Execute
//purpose  :
//======================================================================= 
Standard_Integer GEOMImpl_ScaleDriver::Execute(LOGBOOK& log) const
{
  if (Label().IsNull()) return 0;    
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  GEOMImpl_IScale aCI (aFunction);
  Standard_Integer aType = aFunction->GetType();

  TopoDS_Shape aShape;

  if (aType == SCALE_SHAPE || aType == SCALE_SHAPE_COPY) {
    Handle(GEOM_Function) aRefShape = aCI.GetShape();
    TopoDS_Shape aShapeBase = aRefShape->GetValue();
    if (aShapeBase.IsNull()) return 0;

    gp_Pnt aP (0,0,0);
    Handle(GEOM_Function) aRefPoint = aCI.GetPoint();
    if (!aRefPoint.IsNull()) {
      TopoDS_Shape aShapePnt = aRefPoint->GetValue();
      if (aShapePnt.IsNull()) return 0;
      if (aShapePnt.ShapeType() != TopAbs_VERTEX) return 0;
      aP = BRep_Tool::Pnt(TopoDS::Vertex(aShapePnt));
    }

    // Bug 6839: Check for standalone (not included in faces) degenerated edges
    TopTools_IndexedDataMapOfShapeListOfShape aEFMap;
    TopExp::MapShapesAndAncestors(aShapeBase, TopAbs_EDGE, TopAbs_FACE, aEFMap);
    Standard_Integer i, nbE = aEFMap.Extent();
    for (i = 1; i <= nbE; i++) {
      TopoDS_Shape anEdgeSh = aEFMap.FindKey(i);
      if (BRep_Tool::Degenerated(TopoDS::Edge(anEdgeSh))) {
        const TopTools_ListOfShape& aFaces = aEFMap.FindFromIndex(i);
        if (aFaces.IsEmpty())
          Standard_ConstructionError::Raise
            ("Scaling aborted : cannot scale standalone degenerated edge");
      }
    }

    // Perform Scaling
    gp_Trsf aTrsf;
    aTrsf.SetScale(aP, aCI.GetFactor());
    BRepBuilderAPI_Transform aBRepTrsf (aShapeBase, aTrsf, Standard_False);
    aShape = aBRepTrsf.Shape();
  }
  else if (aType == SCALE_SHAPE_AXES || aType == SCALE_SHAPE_AXES_COPY) {
    Handle(GEOM_Function) aRefShape = aCI.GetShape();
    TopoDS_Shape aShapeBase = aRefShape->GetValue();
    if (aShapeBase.IsNull()) return 0;

    bool isP = false;
    gp_Pnt aP (0,0,0);
    Handle(GEOM_Function) aRefPoint = aCI.GetPoint();
    if (!aRefPoint.IsNull()) {
      TopoDS_Shape aShapePnt = aRefPoint->GetValue();
      if (aShapePnt.IsNull()) return 0;
      if (aShapePnt.ShapeType() != TopAbs_VERTEX) return 0;
      aP = BRep_Tool::Pnt(TopoDS::Vertex(aShapePnt));
      isP = true;
    }

    // Bug 6839: Check for standalone (not included in faces) degenerated edges
    TopTools_IndexedDataMapOfShapeListOfShape aEFMap;
    TopExp::MapShapesAndAncestors(aShapeBase, TopAbs_EDGE, TopAbs_FACE, aEFMap);
    Standard_Integer i, nbE = aEFMap.Extent();
    for (i = 1; i <= nbE; i++) {
      TopoDS_Shape anEdgeSh = aEFMap.FindKey(i);
      if (BRep_Tool::Degenerated(TopoDS::Edge(anEdgeSh))) {
        const TopTools_ListOfShape& aFaces = aEFMap.FindFromIndex(i);
        if (aFaces.IsEmpty())
          Standard_ConstructionError::Raise
            ("Scaling aborted : cannot scale standalone degenerated edge");
      }
    }

    // Perform Scaling
    gp_GTrsf aGTrsf;
    gp_Mat rot (aCI.GetFactorX(), 0, 0,
                0, aCI.GetFactorY(), 0,
                0, 0, aCI.GetFactorZ());
    aGTrsf.SetVectorialPart(rot);

    if (isP) {
      gp_Pnt anO (0,0,0);
      if (anO.Distance(aP) > Precision::Confusion()) {
        gp_GTrsf aGTrsfP0;
        aGTrsfP0.SetTranslationPart(anO.XYZ() - aP.XYZ());
        gp_GTrsf aGTrsf0P;
        aGTrsf0P.SetTranslationPart(aP.XYZ());
        //aGTrsf = aGTrsf0P * aGTrsf * aGTrsfP0;
        aGTrsf = aGTrsf0P.Multiplied(aGTrsf);
        aGTrsf = aGTrsf.Multiplied(aGTrsfP0);
      }
    }

    BRepBuilderAPI_GTransform aBRepGTrsf (aShapeBase, aGTrsf, Standard_False);
    if (!aBRepGTrsf.IsDone())
      Standard_ConstructionError::Raise("Scaling not done");
    aShape = aBRepGTrsf.Shape();
  }
  else {
  }

  if (aShape.IsNull()) return 0;

  if ( !GEOMUtils::CheckShape(aShape) && !GEOMUtils::FixShapeTolerance(aShape) )
    Standard_ConstructionError::Raise("Scaling aborted : algorithm has produced an invalid shape result");

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

bool GEOMImpl_ScaleDriver::
GetCreationInformation(std::string&             theOperationName,
                       std::vector<GEOM_Param>& theParams)
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) function = GEOM_Function::GetFunction(Label());

  GEOMImpl_IScale aCI( function );
  Standard_Integer aType = function->GetType();

  theOperationName = "SCALE";

  switch ( aType ) {
  case SCALE_SHAPE:
  case SCALE_SHAPE_COPY:
    AddParam( theParams, "Object", aCI.GetShape() );
    AddParam( theParams, "Central Point", aCI.GetPoint(), "origin" );
    AddParam( theParams, "Scale Factor", aCI.GetFactor() );
    break;
  case SCALE_SHAPE_AXES:
  case SCALE_SHAPE_AXES_COPY:
    AddParam( theParams, "Object", aCI.GetShape() );
    AddParam( theParams, "Central Point", aCI.GetPoint(), "origin" );
    AddParam( theParams, "Scale Factor X", aCI.GetFactorX() );
    AddParam( theParams, "Scale Factor Y", aCI.GetFactorY() );
    AddParam( theParams, "Scale Factor Z", aCI.GetFactorZ() );
    break;
  default:
    return false;
  }
  
  return true;
}

OCCT_IMPLEMENT_STANDARD_RTTIEXT (GEOMImpl_ScaleDriver,GEOM_BaseDriver);
