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

#include <GEOMImpl_MarkerDriver.hxx>
#include <GEOMImpl_IMarker.hxx>
#include <GEOMImpl_Types.hxx>

#include <GEOM_Function.hxx>

#include <GEOMUtils.hxx>

#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRep_Tool.hxx>

#include <TopAbs.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopExp.hxx>

#include <GC_MakePlane.hxx>
#include <Geom_Surface.hxx>

#include <Precision.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pln.hxx>
#include <gp_Vec.hxx>

#include <Standard_ConstructionError.hxx>

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& GEOMImpl_MarkerDriver::GetID()
{
  static Standard_GUID aMarkerDriver("FF1BBB07-5D14-4df2-980B-3A668264EA16");
  return aMarkerDriver;
}


//=======================================================================
//function : GEOMImpl_MarkerDriver
//purpose  :
//=======================================================================
GEOMImpl_MarkerDriver::GEOMImpl_MarkerDriver()
{
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer GEOMImpl_MarkerDriver::Execute(LOGBOOK& log) const
{
  if (Label().IsNull())  return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  GEOMImpl_IMarker aPI (aFunction);
  Standard_Integer aType = aFunction->GetType();

  TopoDS_Shape aShape;

  if (aType == MARKER_CS) {
    double OX, OY, OZ;
    double XDX, XDY, XDZ;
    double YDX, YDY, YDZ;
    aPI.GetOrigin(OX, OY, OZ);
    aPI.GetXDir(XDX, XDY, XDZ);
    aPI.GetYDir(YDX, YDY, YDZ);

    gp_Pnt aPO (OX, OY, OZ);
    gp_Vec aVX (XDX, XDY, XDZ);
    gp_Vec aVY (YDX, YDY, YDZ);
    Standard_Real aTol = Precision::Confusion();
    if (aVX.Magnitude() < aTol ||
        aVY.Magnitude() < aTol ||
        aVX.IsParallel(aVY, Precision::Angular())) {
      Standard_ConstructionError::Raise("Degenerated or parallel directions given");
    }

    gp_Vec aN = aVX ^ aVY;
    gp_Ax3 anA (aPO, aN, aVX);
    gp_Pln aPln (anA);

    double aTrimSize = 100.0;
    aShape = BRepBuilderAPI_MakeFace(aPln, -aTrimSize, +aTrimSize, -aTrimSize, +aTrimSize).Shape();
  } else if (aType == MARKER_SHAPE) {
    Handle(GEOM_Function) aRefShape = aPI.GetShape();
    TopoDS_Shape aSh = aRefShape->GetValue();
    gp_Ax3 anAx3 = GEOMUtils::GetPosition(aSh);
    gp_Pln aPln (anAx3);

    double aTrimSize = 100.0;
    aShape = BRepBuilderAPI_MakeFace(aPln, -aTrimSize, +aTrimSize, -aTrimSize, +aTrimSize).Shape();
  } else if (aType == MARKER_PNT2VEC) {
    Handle(GEOM_Function) aRefOrigin  = aPI.GetOrigin();
    Handle(GEOM_Function) aRefXVec = aPI.GetXVec();
    Handle(GEOM_Function) aRefYVec = aPI.GetYVec();
    TopoDS_Shape aShapeOrigin = aRefOrigin->GetValue();
    TopoDS_Shape aShapeXVec = aRefXVec->GetValue();
    TopoDS_Shape aShapeYVec = aRefYVec->GetValue();
    if (aShapeOrigin.ShapeType() != TopAbs_VERTEX || aShapeOrigin.IsNull()) return 0;
    if (aShapeXVec.ShapeType() != TopAbs_EDGE || aShapeXVec.IsNull()) return 0;
    if (aShapeYVec.ShapeType() != TopAbs_EDGE || aShapeYVec.IsNull()) return 0;

    gp_Pnt aPO = BRep_Tool::Pnt( TopoDS::Vertex( aShapeOrigin ) );

    gp_Pnt aPX1 = BRep_Tool::Pnt( TopExp::FirstVertex( TopoDS::Edge( aShapeXVec ) ) );
    gp_Pnt aPX2 = BRep_Tool::Pnt( TopExp::LastVertex( TopoDS::Edge( aShapeXVec ) ) );
    gp_Vec aVX( aPX1, aPX2 );

    gp_Pnt aPY1 = BRep_Tool::Pnt( TopExp::FirstVertex( TopoDS::Edge( aShapeYVec ) ) );
    gp_Pnt aPY2 = BRep_Tool::Pnt( TopExp::LastVertex( TopoDS::Edge( aShapeYVec ) ) );
    gp_Vec aVY( aPY1, aPY2 );

    if (aVX.Magnitude() < gp::Resolution() || aVY.Magnitude() < gp::Resolution())
        Standard_ConstructionError::Raise
          ("Local CS creation aborted: vector of zero length is given");

    if ( aVX.IsParallel(aVY, Precision::Angular()))
      Standard_ConstructionError::Raise("Parallel Vectors given");
    
    gp_Vec aN = aVX ^ aVY;
    gp_Ax3 anA (aPO, aN, aVX);
    gp_Pln aPln (anA);
    
    double aTrimSize = 100.0;
    aShape = BRepBuilderAPI_MakeFace(aPln, -aTrimSize, +aTrimSize, -aTrimSize, +aTrimSize).Shape();
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

bool GEOMImpl_MarkerDriver::
GetCreationInformation(std::string&             theOperationName,
                       std::vector<GEOM_Param>& theParams)
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) function = GEOM_Function::GetFunction(Label());

  GEOMImpl_IMarker aCI( function );
  Standard_Integer aType = function->GetType();

  theOperationName = "LOCAL_CS";

  switch ( aType ) {
  case MARKER_CS:
  {
    double OX,OY,OZ, XDX,XDY,XDZ, YDX,YDY,YDZ;
    aCI.GetOrigin(OX, OY, OZ);
    aCI.GetXDir(XDX, XDY, XDZ);
    aCI.GetYDir(YDX, YDY, YDZ);
    AddParam( theParams, "Coordinates of origin" ) << OX << ", " << OY << ", " << OZ;
    AddParam( theParams, "X axis direction" ) << XDX << ", " << XDY << ", " << XDZ;
    AddParam( theParams, "Y axis direction" ) << YDX << ", " << YDY << ", " << YDZ;
    break;
  }
  case MARKER_SHAPE:
    AddParam( theParams, "Object", aCI.GetShape() );
    break;
  case MARKER_PNT2VEC:
    AddParam( theParams, "Point", aCI.GetOrigin() );
    AddParam( theParams, "X axis direction", aCI.GetXVec() );
    AddParam( theParams, "Y axis direction", aCI.GetYVec() );
    break;
  default:
    return false;
  }
  
  return true;
}

OCCT_IMPLEMENT_STANDARD_RTTIEXT (GEOMImpl_MarkerDriver,GEOM_BaseDriver);
