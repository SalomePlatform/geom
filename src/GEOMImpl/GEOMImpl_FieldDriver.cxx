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

#include "GEOMImpl_FieldDriver.hxx"

#include "GEOM_Field.hxx"
#include "GEOM_Function.hxx"
#include "GEOM_IField.hxx"

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& GEOMImpl_FieldDriver::GetID()
{
  return GEOM_Field::GetFieldID();
}

//=======================================================================
//function : GEOMImpl_FieldDriver
//purpose  :
//=======================================================================
GEOMImpl_FieldDriver::GEOMImpl_FieldDriver()
{
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer GEOMImpl_FieldDriver::Execute(LOGBOOK& log) const
{
  return 0;
}

//================================================================================
/*!
 * \brief Returns a name of creation operation and names and values of creation parameters
 */
//================================================================================

bool GEOMImpl_FieldDriver::
GetCreationInformation(std::string&             theOperationName,
                       std::vector<GEOM_Param>& theParams)
{
  if (Label().IsNull()) return false;
  Handle(GEOM_Function) function = GEOM_Function::GetFunction(Label());

  GEOM_IField data( function );
  Standard_Integer funType = function->GetType();

  if ( funType == GEOM_Field::FUN_ADD_FIELD )
  {
    theOperationName = "FIELD_CREATE";
    // geompy.CreateField(shape, name, type, dimension, componentNames)
    Handle(GEOM_Field) f = GEOM_Field::GetField( function->GetOwnerEntry() );
    AddParam( theParams, "Shape", data.GetShape() );
    AddParam( theParams, "Name", f->GetName() );
    AddParam( theParams, "Type", f->GetDataTypeString( data.GetDataType() ));
    AddParam( theParams, "Dimension", data.GetDimension() );
    GEOM_Param comps = AddParam( theParams, "Components", f->GetComponentsForPython() );
  }
  else if ( funType == GEOM_Field::FUN_ADD_STEP )
  {
    theOperationName = "ADD_FIELD_STEP";
    // field.addStep(step, stamp, values)
    AddParam( theParams, "Step", data.GetStepID() );
    AddParam( theParams, "Stamp", data.GetStepStamp() );
  }
  else if ( funType == GEOM_Field::FUN_CHANGE_COMP_NAMES )
  {
    theOperationName = "Change component names";
  }
  else if ( funType == GEOM_Field::FUN_CHANGE_STEP_STAMP )
  {
    theOperationName = "Change stamp";
    AddParam( theParams, "Stamp", data.GetStepStamp() );
  }
  else if ( funType == GEOM_Field::FUN_CHANGE_VALUE )
  {
    theOperationName = "FIELD_EDIT";//"Change values";
    AddParam( theParams, "Values", "..." );
  }
  else
  {
    return false;
  }
  
  return true;
}

OCCT_IMPLEMENT_STANDARD_RTTIEXT (GEOMImpl_FieldDriver,GEOM_BaseDriver);
