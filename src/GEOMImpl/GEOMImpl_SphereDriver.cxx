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

#include <GEOMImpl_SphereDriver.hxx>
#include <GEOMImpl_ISphere.hxx>
#include <GEOMImpl_Types.hxx>
#include <GEOM_Function.hxx>

#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRep_Tool.hxx>
#include <gp_Pnt.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopAbs.hxx>

//=======================================================================
//function : GetID
//purpose  :
//======================================================================= 
const Standard_GUID& GEOMImpl_SphereDriver::GetID()
{
  static Standard_GUID aSphereDriver("FF1BBB16-5D14-4df2-980B-3A668264EA16");
  return aSphereDriver; 
}


//=======================================================================
//function : GEOMImpl_SphereDriver
//purpose  : 
//=======================================================================
GEOMImpl_SphereDriver::GEOMImpl_SphereDriver() 
{
}

//=======================================================================
//function : Execute
//purpose  :
//======================================================================= 
Standard_Integer GEOMImpl_SphereDriver::Execute(LOGBOOK& log) const
{
  if (Label().IsNull()) return 0;    
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  GEOMImpl_ISphere aCI (aFunction);
  Standard_Integer aType = aFunction->GetType();

  TopoDS_Shape aShape;

  char aMsg[] = "Sphere creation aborted: radius value less than 1e-07 is not acceptable";

  if (aType == SPHERE_R) {
    double anR = aCI.GetR();
    if (anR < Precision::Confusion())
      Standard_ConstructionError::Raise(aMsg);

    aShape = BRepPrimAPI_MakeSphere(anR).Shape();
  }
  else if (aType == SPHERE_PNT_R) {
    double anR = aCI.GetR();
    if (anR < Precision::Confusion())
      Standard_ConstructionError::Raise(aMsg);

    Handle(GEOM_Function) aRefPoint  = aCI.GetPoint();
    TopoDS_Shape aShapePnt = aRefPoint->GetValue();
    if (aShapePnt.ShapeType() != TopAbs_VERTEX)
      Standard_ConstructionError::Raise("Invalid shape given for sphere center: it must be a point");
    gp_Pnt aP = BRep_Tool::Pnt(TopoDS::Vertex(aShapePnt));

    aShape = BRepPrimAPI_MakeSphere(aP, anR).Shape();
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

bool GEOMImpl_SphereDriver::
GetCreationInformation(std::string&             theOperationName,
                       std::vector<GEOM_Param>& theParams)
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) function = GEOM_Function::GetFunction(Label());

  GEOMImpl_ISphere aCI( function );
  Standard_Integer aType = function->GetType();

  theOperationName = "SPHERE";

  switch ( aType ) {
  case SPHERE_R:
    AddParam( theParams, "Radius", aCI.GetR() );
    break;
  case SPHERE_PNT_R:
    AddParam( theParams, "Center", aCI.GetPoint() );
    AddParam( theParams, "Radius", aCI.GetR() );
    break;
  default:
    return false;
  }
  
  return true;
}

OCCT_IMPLEMENT_STANDARD_RTTIEXT (GEOMImpl_SphereDriver,GEOM_BaseDriver);
