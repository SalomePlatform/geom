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

#include <GEOMImpl_MirrorDriver.hxx>

#include <GEOMImpl_IMirror.hxx>
#include <GEOMImpl_Types.hxx>
#include <GEOM_Function.hxx>

#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepClass_FaceClassifier.hxx>
#include <BRepTools.hxx>

#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>

#include <Geom_Plane.hxx>

#include <gp_Trsf.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

//=======================================================================
//function : GetID
//purpose  :
//======================================================================= 
const Standard_GUID& GEOMImpl_MirrorDriver::GetID()
{
  static Standard_GUID aMirrorDriver("FF1BBB57-5D14-4df2-980B-3A668264EA16");
  return aMirrorDriver; 
}


//=======================================================================
//function : GEOMImpl_MirrorDriver
//purpose  : 
//=======================================================================

GEOMImpl_MirrorDriver::GEOMImpl_MirrorDriver() 
{
}

//=======================================================================
//function : Execute
//purpose  :
//======================================================================= 
Standard_Integer GEOMImpl_MirrorDriver::Execute(LOGBOOK& log) const
{
  if (Label().IsNull())  return 0;    
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  if (aFunction.IsNull()) return 0;

  TopoDS_Shape aShape;
  gp_Trsf aTrsf;

  GEOMImpl_IMirror TI (aFunction);
  Standard_Integer aType = aFunction->GetType();

  Handle(GEOM_Function) anOriginalFunction = TI.GetOriginal();
  if (anOriginalFunction.IsNull()) return 0;

  TopoDS_Shape anOriginal = anOriginalFunction->GetValue();
  if (anOriginal.IsNull()) return 0;

  // Bug 12158: Check for standalone (not included in faces) degenerated edges
  TopTools_IndexedDataMapOfShapeListOfShape aEFMap;
  TopExp::MapShapesAndAncestors(anOriginal, TopAbs_EDGE, TopAbs_FACE, aEFMap);
  Standard_Integer i, nbE = aEFMap.Extent();
  for (i = 1; i <= nbE; i++) {
    TopoDS_Shape anEdgeSh = aEFMap.FindKey(i);
    if (BRep_Tool::Degenerated(TopoDS::Edge(anEdgeSh))) {
      const TopTools_ListOfShape& aFaces = aEFMap.FindFromIndex(i);
      if (aFaces.IsEmpty())
        Standard_ConstructionError::Raise
          ("Mirror aborted : cannot process standalone degenerated edge");
    }
  }

  // Perform Mirror
  if (aType == MIRROR_PLANE || aType == MIRROR_PLANE_COPY) {
    Handle(GEOM_Function) aPlane = TI.GetPlane();
    if (aPlane.IsNull()) return 0;
    TopoDS_Shape aFaceShape = aPlane->GetValue();
    if (aFaceShape.IsNull() || aFaceShape.ShapeType() != TopAbs_FACE) return 0;
    TopoDS_Face aFace = TopoDS::Face(aFaceShape);

    Handle(Geom_Surface) surf = BRep_Tool::Surface(aFace);
    Handle(Geom_Plane) myPlane = Handle(Geom_Plane)::DownCast(surf);
    const gp_Ax3 pos = myPlane->Position();
    const gp_Pnt loc = pos.Location();  /* location of the plane */
    const gp_Dir dir = pos.Direction(); /* Main direction of the plane (Z axis) */
    gp_Ax2 aPln (loc, dir);
    aTrsf.SetMirror(aPln);
  }
  else if (aType == MIRROR_AXIS || aType == MIRROR_AXIS_COPY) {
    Handle(GEOM_Function) anAxis = TI.GetAxis();
    if (anAxis.IsNull()) return 0;
    TopoDS_Shape anAxisShape = anAxis->GetValue();
    if (anAxisShape.IsNull() || anAxisShape.ShapeType() != TopAbs_EDGE) return 0;
    TopoDS_Edge anEdge = TopoDS::Edge(anAxisShape);

    gp_Pnt aP1 = BRep_Tool::Pnt(TopExp::FirstVertex(anEdge));
    gp_Pnt aP2 = BRep_Tool::Pnt(TopExp::LastVertex (anEdge));
    gp_Vec aV (aP1, aP2);
    gp_Ax1 anAx1 (aP1, aV);
    aTrsf.SetMirror(anAx1);
  }
  else if (aType == MIRROR_POINT || aType == MIRROR_POINT_COPY) {
    Handle(GEOM_Function) aPoint = TI.GetPoint();
    if (aPoint.IsNull()) return 0;
    TopoDS_Shape aVertexShape = aPoint->GetValue();
    if (aVertexShape.IsNull() || aVertexShape.ShapeType() != TopAbs_VERTEX) return 0;
    TopoDS_Vertex aVertex = TopoDS::Vertex(aVertexShape);

    gp_Pnt aP = BRep_Tool::Pnt(aVertex);
    aTrsf.SetMirror(aP);
  }
  else {
    return 0;
  }

  BRepBuilderAPI_Transform aTransformation (anOriginal, aTrsf, Standard_False);
  aShape = aTransformation.Shape();

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

bool GEOMImpl_MirrorDriver::
GetCreationInformation(std::string&             theOperationName,
                       std::vector<GEOM_Param>& theParams)
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) function = GEOM_Function::GetFunction(Label());

  GEOMImpl_IMirror aCI( function );
  Standard_Integer aType = function->GetType();

  theOperationName = "MIRROR";

  switch ( aType ) {
  case MIRROR_PLANE:
  case MIRROR_PLANE_COPY:
    AddParam( theParams, "Object", aCI.GetOriginal() );
    AddParam( theParams, "Plane Mirror", aCI.GetPlane() );
    break;
  case MIRROR_AXIS:
  case MIRROR_AXIS_COPY:
    AddParam( theParams, "Object", aCI.GetOriginal() );
    AddParam( theParams, "Axis Mirror", aCI.GetAxis() );
    break;
  case MIRROR_POINT:
  case MIRROR_POINT_COPY:
    AddParam( theParams, "Object", aCI.GetOriginal() );
    AddParam( theParams, "Point Mirror", aCI.GetPoint() );
    break;
  default:
    return false;
  }
  
  return true;
}

OCCT_IMPLEMENT_STANDARD_RTTIEXT (GEOMImpl_MirrorDriver,GEOM_BaseDriver);
