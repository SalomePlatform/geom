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

#include "GEOM_SubShapeDriver.hxx"

#include "GEOM_ISubShape.hxx"
#include "GEOM_Function.hxx"
#include "GEOM_Object.hxx"

#include <BRep_Builder.hxx>
#include <TopExp.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Shape.hxx>
#include <TDataStd_Integer.hxx>

#include <Standard_NullObject.hxx>

//=======================================================================
//function : GEOM_SubShapeDriver
//purpose  :
//=======================================================================
GEOM_SubShapeDriver::GEOM_SubShapeDriver()
{
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer GEOM_SubShapeDriver::Execute(LOGBOOK& log) const
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  GEOM_ISubShape aCI (aFunction);

  TDF_Label aLabel = aCI.GetMainShape()->GetOwnerEntry();
  if (aLabel.IsRoot()) return 0;
  Handle(GEOM_Object) anObj = GEOM_Object::GetObject(aLabel);
  if (anObj.IsNull()) return 0;
  TopoDS_Shape aMainShape = anObj->GetValue();
  if (aMainShape.IsNull()) return 0;

  Handle(TColStd_HArray1OfInteger) anIndices = aCI.GetIndices();
  if (anIndices.IsNull() || anIndices->Length() <= 0) return 0;

  BRep_Builder B;
  TopoDS_Compound aCompound;
  TopoDS_Shape aShape;

  if (anIndices->Length() == 1 && anIndices->Value(1) == -1) { //The empty sub-shape
    B.MakeCompound(aCompound);
    aShape = aCompound;
  }
  else {
    TopTools_IndexedMapOfShape aMapOfShapes;
    TopExp::MapShapes(aMainShape, aMapOfShapes);

    if (anIndices->Length() > 1) {
      B.MakeCompound(aCompound);

      for (int i = anIndices->Lower(); i <= anIndices->Upper(); i++) {
        if (aMapOfShapes.Extent() < anIndices->Value(i))
          Standard_NullObject::Raise("GEOM_SubShapeDriver::Execute: Index is out of range");
        TopoDS_Shape aSubShape = aMapOfShapes.FindKey(anIndices->Value(i));
        if (aSubShape.IsNull()) continue;
        B.Add(aCompound,aSubShape);
      }

      aShape = aCompound;
    }
    else {
      int i = anIndices->Lower();
      if (aMapOfShapes.Extent() < anIndices->Value(i))
        Standard_NullObject::Raise("GEOM_SubShapeDriver::Execute: Index is out of range");
      aShape = aMapOfShapes.FindKey(anIndices->Value(i));
    }
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

bool GEOM_SubShapeDriver::
GetCreationInformation(std::string&             theOperationName,
                       std::vector<GEOM_Param>& theParams)
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) function = GEOM_Function::GetFunction(Label());
  GEOM_ISubShape aCI( function );

  enum { GEOM_SUBSHAPE = 28, GEOM_GROUP = 37 };

  TDF_Label aLabel = function->GetOwnerEntry();
  if (aLabel.IsRoot()) return false;
  Handle(GEOM_Object) obj = GEOM_Object::GetObject( aLabel );
  if ( obj.IsNull() ) return false;

  switch ( obj->GetType() ) {
  case GEOM_SUBSHAPE:
    theOperationName = "EXPLODE";
    AddParam( theParams, "Main Object", aCI.GetMainShape() );
    AddParam( theParams, "Index", aCI.GetIndices() );
    break;
  case GEOM_GROUP:
  {
    theOperationName = "GROUP_CREATE";
    TopAbs_ShapeEnum type = TopAbs_SHAPE;
    {
      TDF_Label aFreeLabel = obj->GetFreeLabel();
      Handle(TDataStd_Integer) anAttrib;
      if(aFreeLabel.FindAttribute(TDataStd_Integer::GetID(), anAttrib))
        type = (TopAbs_ShapeEnum) anAttrib->Get();
    }
    AddParam( theParams, "Shape Type", type );
    AddParam( theParams, "Main Shape", aCI.GetMainShape() );
    AddParam( theParams, "Indices", aCI.GetIndices() );
    break;
  }
  default:
    return false;
  }
  
  return true;
}

OCCT_IMPLEMENT_STANDARD_RTTIEXT (GEOM_SubShapeDriver,GEOM_BaseDriver);
