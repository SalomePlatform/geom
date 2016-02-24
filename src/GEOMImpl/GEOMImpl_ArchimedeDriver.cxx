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

#include "GEOMImpl_ArchimedeDriver.hxx"
#include "GEOMImpl_IArchimede.hxx"
#include "GEOMImpl_Types.hxx"

#include "Archimede_VolumeSection.hxx"

#include <Basics_OCCTVersion.hxx>

#include <stdio.h>

#include <BRepBuilderAPI_MakeFace.hxx>

#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>

#include <Geom_Plane.hxx>
#include <Geom_Geometry.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>

#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>

#include <StdFail_NotDone.hxx>


//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& GEOMImpl_ArchimedeDriver::GetID()
{
  static Standard_GUID aArchimedeDriver("FF1BBB59-5D14-4df2-980B-3A668264EA16");
  return aArchimedeDriver;
}


//=======================================================================
//function : GEOMImpl_ArchimedeDriver
//purpose  :
//=======================================================================

GEOMImpl_ArchimedeDriver::GEOMImpl_ArchimedeDriver()
{
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer GEOMImpl_ArchimedeDriver::Execute(LOGBOOK& log) const
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  Standard_Integer aType = aFunction->GetType();
  if (aType != ARCHIMEDE_TYPE) return 0;

  GEOMImpl_IArchimede IA (aFunction);

  Handle(GEOM_Function) aShapeFunction = IA.GetBasicShape();
  if (aShapeFunction.IsNull()) return 0;
  TopoDS_Shape shape = aShapeFunction->GetValue();
  if(shape.IsNull()) return 0;

  Standard_Real aWaterDensity = IA.GetDensity();
  Standard_Real aWeight = IA.GetWeight();
  Standard_Real aMeshingDeflection = IA.GetDeflection();

  double cste = -1;
  if (aWaterDensity != 0.)
    cste = aWeight/aWaterDensity;
  else
    return 0;

  gp_Dir direct (0.0, 0.0, 1.0);
  gp_Pnt PosPlan (0.0, 0.0, 0.0);
  Geom_Plane PP (PosPlan, direct);
  Handle(Geom_Plane) P = Handle(Geom_Plane)::DownCast(PP.Copy());

  gp_Dir Zdirection (0.0, 0.0, 1.0);
  VolumeSection VOL (shape, aMeshingDeflection);
  VOL.SetPlane(P);
  Handle(Geom_RectangularTrimmedSurface) SurfaceTrimmee;

  if (Zdirection.IsEqual(direct, Precision::Angular()) == Standard_False) {
    VOL.MakeRotation(direct);
  }

  VOL.CenterOfGravity();
  SurfaceTrimmee = VOL.TrimSurf();
  Standard_Real Cote = VOL.Archimede(cste, aMeshingDeflection);

  if (Cote == -1) {
    double Zmin,Zmax;
    VOL.getZ(Zmin,Zmax);
    double volume = VOL.CalculateVolume(Zmax) * aWaterDensity;

    char msg[100] = "";
    sprintf(msg, "shape sinks to the bottom : Weigth max = %.1f", volume);

    StdFail_NotDone::Raise(msg);
  }

  SurfaceTrimmee = VOL.AjustePlan(SurfaceTrimmee,Cote,PosPlan);
  if (Zdirection.IsEqual(direct,Precision::Angular()) == Standard_False) {
    SurfaceTrimmee = VOL.InvMakeRotation(direct,SurfaceTrimmee);
  }

  Standard_Real u1,u2,v1,v2;
  SurfaceTrimmee->Bounds(u1,u2,v1,v2);
  TopoDS_Face tirant = BRepBuilderAPI_MakeFace(SurfaceTrimmee, u1, u2, v1, v2, Precision::Confusion());

  if (tirant.IsNull()) {
    StdFail_NotDone::Raise("Failed to build secant face");
  }

  aFunction->SetValue(tirant);

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

bool GEOMImpl_ArchimedeDriver::
GetCreationInformation(std::string&             theOperationName,
                       std::vector<GEOM_Param>& theParams)
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) function = GEOM_Function::GetFunction(Label());

  GEOMImpl_IArchimede IA (function);
  Standard_Integer aType = function->GetType();
  if (aType != ARCHIMEDE_TYPE) return 0;

  theOperationName = "ARCHIMEDE";

  AddParam( theParams, "Objects", IA.GetBasicShape() );
  AddParam( theParams, "Weight", IA.GetWeight() );
  AddParam( theParams, "Water Density", IA.GetDensity() );
  AddParam( theParams, "Meshing Deflect.", IA.GetDeflection() );
  
  return true;
}

OCCT_IMPLEMENT_STANDARD_RTTIEXT (GEOMImpl_ArchimedeDriver,GEOM_BaseDriver);
