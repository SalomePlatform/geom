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

#include <GEOMImpl_FilletDriver.hxx>
#include <GEOMImpl_IFillet.hxx>
#include <GEOMImpl_Types.hxx>
#include <GEOMImpl_ILocalOperations.hxx>
#include <GEOMUtils.hxx>
#include <GEOM_Function.hxx>

#include <BRepFilletAPI_MakeFillet.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopAbs.hxx>
#include <TopExp_Explorer.hxx>
#include <StdFail_NotDone.hxx>

// Debug PipeTShape function: uncomment the macro below to correct tolerance
// of resulting face after fillet creation
// VSR 30/12/2014: macro disabled
//#define FIX_FACE_TOLERANCE
// Debug PipeTShape function: uncomment the macro below to correct tolerance
// of resulting curves after fillet creation
// VSR 30/12/2014: macro disabled
//#define FIX_CURVES_TOLERANCES

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& GEOMImpl_FilletDriver::GetID()
{
  static Standard_GUID aFilletDriver("FF1BBB41-5D14-4df2-980B-3A668264EA16");
  return aFilletDriver;
}


//=======================================================================
//function : GEOMImpl_FilletDriver
//purpose  :
//=======================================================================
GEOMImpl_FilletDriver::GEOMImpl_FilletDriver()
{
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer GEOMImpl_FilletDriver::Execute(LOGBOOK& log) const
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  GEOMImpl_IFillet aCI (aFunction);
  Standard_Integer aType = aFunction->GetType();

  TopoDS_Shape aShape;

  Handle(GEOM_Function) aRefShape = aCI.GetShape();
  TopoDS_Shape aShapeBase = aRefShape->GetValue();
  BRepFilletAPI_MakeFillet fill (aShapeBase);

  if (aType == FILLET_SHAPE_ALL) {
    TopExp_Explorer Exp (aShapeBase, TopAbs_EDGE);
    for (; Exp.More(); Exp.Next()) {
      TopoDS_Edge E = TopoDS::Edge(Exp.Current());
      fill.Add(E);
    }
  }
  else if (aType == FILLET_SHAPE_EDGES || aType == FILLET_SHAPE_EDGES_2R) {
    int aLen = aCI.GetLength();
    int ind = 1;
    for (; ind <= aLen; ind++) {
      TopoDS_Shape aShapeEdge;
      if (GEOMImpl_ILocalOperations::GetSubShape
          (aShapeBase, aCI.GetEdge(ind), aShapeEdge)) {
          fill.Add(TopoDS::Edge(aShapeEdge));
      }
    }
  }
  else if (aType == FILLET_SHAPE_FACES || aType == FILLET_SHAPE_FACES_2R) {
    int aLen = aCI.GetLength();
    int ind = 1;
    for (; ind <= aLen; ind++) {
      TopoDS_Shape aShapeFace;
      if (GEOMImpl_ILocalOperations::GetSubShape
          (aShapeBase, aCI.GetFace(ind), aShapeFace)) {
        TopExp_Explorer Exp (aShapeFace, TopAbs_EDGE);
        for (; Exp.More(); Exp.Next()) {
          fill.Add(TopoDS::Edge(Exp.Current()));
        }
      }
    }
  }
  else {
  }

  if (aType == FILLET_SHAPE_FACES || aType == FILLET_SHAPE_EDGES || aType == FILLET_SHAPE_ALL) {
    for (int i = 1; i <= fill.NbContours(); i++)
      fill.SetRadius(aCI.GetR(), i, 1);
  }
  else if (aType == FILLET_SHAPE_FACES_2R || aType == FILLET_SHAPE_EDGES_2R) {
    for (int i = 1; i <= fill.NbContours(); i++) {
      fill.SetRadius(aCI.GetR1(), aCI.GetR2(), i, 1);
    }
  }

  fill.Build();
  if (!fill.IsDone()) {
    StdFail_NotDone::Raise("Fillet can't be computed on the given shape with the given radius");
  }
  aShape = GEOMUtils::ReduceCompound( fill.Shape() );

  if (aShape.IsNull()) return 0;

#if defined(FIX_CURVES_TOLERANCES)
  bool isOk = GEOMUtils::FixShapeCurves(aShape);
#elif defined(FIX_FACE_TOLERANCE)
  bool isOk = GEOMUtils::FixShapeTolerance(aShape, TopAbs_FACE);
#else
  // 08.07.2008 added by skl during fixing bug 19761 from Mantis
  bool isOk = GEOMUtils::CheckShape(aShape) || GEOMUtils::FixShapeTolerance(aShape);
#endif
  if ( !isOk )
    StdFail_NotDone::Raise("Fillet algorithm have produced an invalid shape result");

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

bool GEOMImpl_FilletDriver::
GetCreationInformation(std::string&             theOperationName,
                       std::vector<GEOM_Param>& theParams)
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) function = GEOM_Function::GetFunction(Label());

  GEOMImpl_IFillet aCI( function );
  Standard_Integer aType = function->GetType();

  theOperationName = "FILLET";

  switch ( aType ) {
  case FILLET_SHAPE_ALL:
    AddParam( theParams, "Main Object", aCI.GetShape() );
    AddParam( theParams, "Selected edges", "all" );
    break;
  case FILLET_SHAPE_EDGES:
  case FILLET_SHAPE_EDGES_2R:
    AddParam( theParams, "Main Object", aCI.GetShape() );
    AddParam( theParams, "Selected edges" );
    if ( aCI.GetLength() > 1 )
      theParams[1] << aCI.GetLength() << " edges: ";
    for (int i = 1; i <= aCI.GetLength(); ++i )
      theParams[1] << aCI.GetEdge( i ) << " ";
    if ( aType == FILLET_SHAPE_EDGES ) {
      AddParam( theParams, "Radius", aCI.GetR() );
    }
    else {
      AddParam( theParams, "R1", aCI.GetR1() );
      AddParam( theParams, "R2", aCI.GetR2() );
    }
    break;
  case FILLET_SHAPE_FACES:
  case FILLET_SHAPE_FACES_2R:
    AddParam( theParams, "Main Object", aCI.GetShape() );
    AddParam( theParams, "Selected faces" );
    if ( aCI.GetLength() > 1 )
      theParams[1] << aCI.GetLength() << " faces: ";
    for (int i = 1; i <= aCI.GetLength(); ++i )
      theParams[1] << aCI.GetFace( i ) << " ";
    if ( aType == FILLET_SHAPE_FACES ) {
      AddParam( theParams, "Radius", aCI.GetR() );
    }
    else {
      AddParam( theParams, "R1", aCI.GetR1() );
      AddParam( theParams, "R2", aCI.GetR2() );
    }
    break;
  default:
    return false;
  }
  
  return true;
}

OCCT_IMPLEMENT_STANDARD_RTTIEXT (GEOMImpl_FilletDriver,GEOM_BaseDriver);
