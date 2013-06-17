// Copyright (C) 2007-2013  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

#include "GEOMImpl_CopyDriver.hxx"
#include "GEOMImpl_ICopy.hxx"
#include "GEOMImpl_Types.hxx"
#include "GEOM_Function.hxx"
#include "GEOM_Object.hxx"

#include <BRep_Tool.hxx>
#include <gp_Pnt.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopAbs.hxx>
#include <TNaming_CopyShape.hxx>
#include <TColStd_IndexedDataMapOfTransientTransient.hxx>

//=======================================================================
//function : GetID
//purpose  :
//======================================================================= 
const Standard_GUID& GEOMImpl_CopyDriver::GetID()
{
  static Standard_GUID aCopyDriver("FF1BBB53-5D14-4df2-980B-3A668264EA16");
  return aCopyDriver; 
}


//=======================================================================
//function : GEOMImpl_CopyDriver
//purpose  : 
//=======================================================================
GEOMImpl_CopyDriver::GEOMImpl_CopyDriver() 
{
}

//=======================================================================
//function : Execute
//purpose  :
//======================================================================= 
Standard_Integer GEOMImpl_CopyDriver::Execute(TFunction_Logbook& log) const
{
  if (Label().IsNull()) return 0;    
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  Standard_Integer aType = aFunction->GetType();

  GEOMImpl_ICopy aCI (aFunction);
  TopoDS_Shape aCopy;
  
  if(aType == COPY_WITH_REF) {
  
    Handle(GEOM_Function) aRefFunction = aCI.GetOriginal();
    if (aRefFunction.IsNull()) return 0;
    TopoDS_Shape anOriginal = aRefFunction->GetValue();

    TColStd_IndexedDataMapOfTransientTransient aMap;
  
    TNaming_CopyShape::CopyTool(anOriginal, aMap, aCopy);
  }
  else if(aType == COPY_WITHOUT_REF) {
    aCopy = aFunction->GetValue();
  }
  
  if (aCopy.IsNull()) return 0;

  aFunction->SetValue(aCopy);

  log.SetTouched(Label()); 

  return 1;    
}

//================================================================================
/*!
 * \brief Returns a name of creation operation and names and values of creation parameters
 */
//================================================================================

bool GEOMImpl_CopyDriver::
GetCreationInformation(std::string&             theOperationName,
                       std::vector<GEOM_Param>& theParams)
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) function = GEOM_Function::GetFunction(Label());

  GEOMImpl_ICopy aCI( function );
  Standard_Integer aType = function->GetType();


  switch ( aType ) {
  case COPY_WITH_REF:
    theOperationName = "MakeCopy";
    AddParam( theParams, "Original", aCI.GetOriginal() );
    break;
  case COPY_WITHOUT_REF:
  {
    theOperationName = "RestoreShape";
    TDF_Label label = Label();
    Handle(GEOM_Object) obj = GEOM_Object::GetObject(label);
    if ( !obj.IsNull() && obj->GetType() == GEOM_FREE_BOUNDS )
      theOperationName = "CHECK_FREE_BNDS";
    break;
  }
  default:
    return false;
  }
  
  return true;
}

IMPLEMENT_STANDARD_HANDLE (GEOMImpl_CopyDriver,GEOM_BaseDriver);

IMPLEMENT_STANDARD_RTTIEXT (GEOMImpl_CopyDriver,GEOM_BaseDriver);
