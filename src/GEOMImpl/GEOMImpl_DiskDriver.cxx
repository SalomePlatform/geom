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

#include <Standard_Stream.hxx>

#include <GEOMImpl_DiskDriver.hxx>
#include <GEOMImpl_IDisk.hxx>
#include <GEOMImpl_Types.hxx>
#include <GEOM_Function.hxx>

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopAbs.hxx>
#include <TopExp.hxx>

#include <GC_MakeCircle.hxx>
#include <Geom_Circle.hxx>

#include <Standard_ConstructionError.hxx>
#include <Precision.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Circ.hxx>

//=======================================================================
//function : GetID
//purpose  :
//======================================================================= 
const Standard_GUID& GEOMImpl_DiskDriver::GetID()
{
  static Standard_GUID aDiskDriver("C1FEEF9D-1C6D-41ce-9507-F10D75430CE1");
  return aDiskDriver; 
}


//=======================================================================
//function : GEOMImpl_DiskDriver
//purpose  : 
//=======================================================================
GEOMImpl_DiskDriver::GEOMImpl_DiskDriver() 
{
}

//=======================================================================
//function : Execute
//purpose  :
//======================================================================= 
Standard_Integer GEOMImpl_DiskDriver::Execute(LOGBOOK& log) const
{
  if (Label().IsNull()) return 0;    
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  GEOMImpl_IDisk aCI (aFunction);
  Standard_Integer aType = aFunction->GetType();

  TopoDS_Shape aShape;

  if (aType == DISK_PNT_VEC_R) {
    Handle(GEOM_Function) aRefPoint  = aCI.GetCenter();
    Handle(GEOM_Function) aRefVector = aCI.GetVector();
    TopoDS_Shape aShapePnt = aRefPoint->GetValue();
    TopoDS_Shape aShapeVec = aRefVector->GetValue();
    if (aShapePnt.ShapeType() == TopAbs_VERTEX &&
        aShapeVec.ShapeType() == TopAbs_EDGE) {
      gp_Pnt aP = BRep_Tool::Pnt(TopoDS::Vertex(aShapePnt));
      TopoDS_Edge anE = TopoDS::Edge(aShapeVec);
      TopoDS_Vertex V1, V2;
      TopExp::Vertices(anE, V1, V2, Standard_True);
      if (!V1.IsNull() && !V2.IsNull()) {
        gp_Vec aV (BRep_Tool::Pnt(V1), BRep_Tool::Pnt(V2));
        gp_Ax2 anAxes (aP, aV);
        gp_Ax3 anAxes3(anAxes);
        gp_Pln aPln(anAxes3);
        gp_Ax2 anAxes1(aP, -aV);
        gp_Circ aCirc (anAxes1, aCI.GetRadius());
        TopoDS_Shape aCircle = BRepBuilderAPI_MakeEdge(aCirc).Edge();
        BRepBuilderAPI_MakeWire MW;
        MW.Add(TopoDS::Edge(aCircle));
        BRepBuilderAPI_MakeFace MF (aPln, MW);
        aShape = MF.Shape();
      }
    }
  }
  else if (aType == DISK_THREE_PNT) {
    Handle(GEOM_Function) aRefPoint1 = aCI.GetPoint1();
    Handle(GEOM_Function) aRefPoint2 = aCI.GetPoint2();
    Handle(GEOM_Function) aRefPoint3 = aCI.GetPoint3();
    TopoDS_Shape aShapePnt1 = aRefPoint1->GetValue();
    TopoDS_Shape aShapePnt2 = aRefPoint2->GetValue();
    TopoDS_Shape aShapePnt3 = aRefPoint3->GetValue();
    if (aShapePnt1.ShapeType() == TopAbs_VERTEX &&
        aShapePnt2.ShapeType() == TopAbs_VERTEX &&
        aShapePnt3.ShapeType() == TopAbs_VERTEX) {
      gp_Pnt aP1 = BRep_Tool::Pnt(TopoDS::Vertex(aShapePnt1));
      gp_Pnt aP2 = BRep_Tool::Pnt(TopoDS::Vertex(aShapePnt2));
      gp_Pnt aP3 = BRep_Tool::Pnt(TopoDS::Vertex(aShapePnt3));
      if (aP1.Distance(aP2) < gp::Resolution() ||
          aP1.Distance(aP3) < gp::Resolution() ||
          aP2.Distance(aP3) < gp::Resolution())
        Standard_ConstructionError::Raise("Disk creation aborted: coincident points given");
      if (gp_Vec(aP1, aP2).IsParallel(gp_Vec(aP1, aP3), Precision::Angular()))
        Standard_ConstructionError::Raise("Disk creation aborted: points lay on one line");
      Handle(Geom_Circle) aCirc = GC_MakeCircle(aP3, aP2, aP1).Value();
      gp_Circ aGpCirc = aCirc->Circ();
      gp_Ax2 anAxes = aGpCirc.Position();
      gp_Ax3 anAxes3(anAxes.Location(), -anAxes.Direction());
      gp_Pln aPln(anAxes3);
      TopoDS_Shape aCircle = BRepBuilderAPI_MakeEdge(aCirc).Edge();
      BRepBuilderAPI_MakeWire MW;
      MW.Add(TopoDS::Edge(aCircle));
      BRepBuilderAPI_MakeFace MF (aPln, MW);
      aShape = MF.Shape();
    }  
  }
  else if (aType == DISK_R) {
    int anOrient = aCI.GetOrientation();
    gp_Pnt aP = gp::Origin();
    gp_Vec aV;
    if (anOrient == 1)
      aV = gp::DZ();
    else if (anOrient == 2)
      aV = gp::DX();
    else if (anOrient == 3)
      aV = gp::DY();

    gp_Ax2 anAxes (aP, aV);
    gp_Ax2 anAxes1(aP, -aV);
    gp_Ax3 anAxes3(anAxes);
    gp_Pln aPln(anAxes3);
    gp_Circ aCirc (anAxes1, aCI.GetRadius());
    TopoDS_Shape aCircle = BRepBuilderAPI_MakeEdge(aCirc).Edge();
    BRepBuilderAPI_MakeWire MW;
    MW.Add(TopoDS::Edge(aCircle));
    BRepBuilderAPI_MakeFace MF (aPln, MW);
    aShape = MF.Shape();
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

bool GEOMImpl_DiskDriver::
GetCreationInformation(std::string&             theOperationName,
                       std::vector<GEOM_Param>& theParams)
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) function = GEOM_Function::GetFunction(Label());

  GEOMImpl_IDisk aCI( function );
  Standard_Integer aType = function->GetType();

  theOperationName = "DISK";

  switch ( aType ) {
  case DISK_PNT_VEC_R:
    AddParam( theParams, "Center Point", aCI.GetCenter() );
    AddParam( theParams, "Vector", aCI.GetVector() );
    AddParam( theParams, "Radius", aCI.GetRadius() );
    break;
  case DISK_THREE_PNT:
    AddParam( theParams, "Point 1", aCI.GetPoint1() );
    AddParam( theParams, "Point 2", aCI.GetPoint2() );
    AddParam( theParams, "Point 3", aCI.GetPoint3() );
    break;
  case DISK_R:
    AddParam( theParams, "Radius", aCI.GetRadius() );
    AddParam( theParams, "Orientation", aCI.GetOrientation() );
    break;
  default:
    return false;
  }
  
  return true;
}

OCCT_IMPLEMENT_STANDARD_RTTIEXT (GEOMImpl_DiskDriver,GEOM_BaseDriver);
