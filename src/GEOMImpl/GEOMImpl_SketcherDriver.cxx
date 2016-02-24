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

#include <GEOMImpl_SketcherDriver.hxx>
#include <GEOMImpl_ISketcher.hxx>
#include <GEOMImpl_Types.hxx>
#include <GEOM_Function.hxx>
#include <GEOMUtils.hxx>
#include <Sketcher_Profile.hxx>

#include <Basics_Utils.hxx>

// OCCT Includes
#include <BRepBuilderAPI_Transform.hxx>
#include <BRep_Tool.hxx>
#include <Geom_Plane.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <gp_Pln.hxx>
#include <Standard_ConstructionError.hxx>

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& GEOMImpl_SketcherDriver::GetID()
{
  static Standard_GUID aSketcherDriver("FF1BBB64-5D14-4df2-980B-3A668264EA16");
  return aSketcherDriver;
}


//=======================================================================
//function : GEOMImpl_SketcherDriver
//purpose  :
//=======================================================================
GEOMImpl_SketcherDriver::GEOMImpl_SketcherDriver()
{
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer GEOMImpl_SketcherDriver::Execute(LOGBOOK& log) const
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  GEOMImpl_ISketcher aCI (aFunction);
  //Standard_Integer aType = aFunction->GetType();

  // retrieve the command
  TCollection_AsciiString aCommand = aCI.GetCommand();
  if (aCommand.IsEmpty())
    return 0;

  // Set "C" numeric locale to save numbers correctly
  Kernel_Utils::Localizer loc;

  // create sketcher
  Sketcher_Profile aProfile( aCommand.ToCString() );
  bool isDone = false;
  TopoDS_Shape aShape = aProfile.GetShape( &isDone );

  if ( !isDone ) {
    Standard_ConstructionError::Raise("Sketcher creation failed");
  }

  if (aShape.IsNull())
    return 0;

  gp_Ax3 aWPlane;
  if ( aFunction->GetType() == SKETCHER_NINE_DOUBLS )
  {
    gp_Pnt aOrigin =
      gp_Pnt(aCI.GetWorkingPlane(1), aCI.GetWorkingPlane(2), aCI.GetWorkingPlane(3));
    gp_Dir aDirZ =
      gp_Dir(aCI.GetWorkingPlane(4), aCI.GetWorkingPlane(5), aCI.GetWorkingPlane(6));
    gp_Dir aDirX =
      gp_Dir(aCI.GetWorkingPlane(7), aCI.GetWorkingPlane(8), aCI.GetWorkingPlane(9));
    aWPlane = gp_Ax3(aOrigin, aDirZ, aDirX);
  }
  else
  {
    Handle(GEOM_Function) aRefFace = aCI.GetWorkingPlane();
    TopoDS_Shape aShape = aRefFace->GetValue();
    //if ( aShape.IsNull() || aShape.ShapeType() != TopAbs_FACE )
    //  return 0;
    //Handle(Geom_Surface) aGS = BRep_Tool::Surface( TopoDS::Face( aShape ));
    //if ( aGS.IsNull() || !aGS->IsKind( STANDARD_TYPE( Geom_Plane )))
    //  return 0;
    //Handle(Geom_Plane) aGPlane = Handle(Geom_Plane)::DownCast( aGS );
    //aWPlane = aGPlane->Pln().Position();
    aWPlane = GEOMUtils::GetPosition(aShape);
  }
  gp_Trsf aTrans;
  aTrans.SetTransformation(aWPlane);
  aTrans.Invert();
  BRepBuilderAPI_Transform aTransformation (aShape, aTrans, Standard_False);
  aShape = aTransformation.Shape();

  if (aShape.IsNull())
    return 0;

  // set the function result
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

bool GEOMImpl_SketcherDriver::
GetCreationInformation(std::string&             theOperationName,
                       std::vector<GEOM_Param>& theParams)
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) function = GEOM_Function::GetFunction(Label());

  GEOMImpl_ISketcher aCI( function );
  Standard_Integer aType = function->GetType();

  theOperationName = "SKETCH";

  switch ( aType ) {
  case SKETCHER_NINE_DOUBLS:
    AddParam( theParams, "Command", aCI.GetCommand() );
    AddParam( theParams, "Origin")
      << aCI.GetWorkingPlane(1) << " "
      << aCI.GetWorkingPlane(2) << " "
      << aCI.GetWorkingPlane(3);
    AddParam( theParams, "OZ")
      << aCI.GetWorkingPlane(4) << " "
      << aCI.GetWorkingPlane(5) << " "
      << aCI.GetWorkingPlane(6);
    AddParam( theParams, "OX")
      << aCI.GetWorkingPlane(7) << " "
      << aCI.GetWorkingPlane(8) << " "
      << aCI.GetWorkingPlane(9);
    break;
  case SKETCHER_PLANE:
    AddParam( theParams, "Command", aCI.GetCommand() );
    AddParam( theParams, "Working plane", aCI.GetWorkingPlane(), "XOY" );
    break;
  default:
    return false;
  }

  return true;
}

OCCT_IMPLEMENT_STANDARD_RTTIEXT (GEOMImpl_SketcherDriver,GEOM_BaseDriver);
