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

//NOTE: This is an intreface to a function for the Field creation.
//
#include "GEOM_Function.hxx"

#include <TColStd_HArray1OfExtendedString.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>

class GEOM_Field;

class GEOM_IField
{
  enum {
    // field
    FIELD_SHAPE      = 1, // ref
    FIELD_DATATYPE   = 1, // int
    FIELD_DIMENSION  = 2, // int
    FIELD_COMPONENTS = 1, // string array
    // field step
    STEP_FIELD       = 1, // ref
    STEP_ID          = 1, // int
    STEP_STAMP       = 2, // int
    STEP_VALUES      = 2  // an array of any type
  };

public:

  GEOM_IField(Handle(GEOM_Function) theFunction): _func(theFunction) {}

  Standard_EXPORT Handle(GEOM_Function) GetShape()
  { return _func->GetReference( FIELD_SHAPE ); }
  Standard_EXPORT int GetDataType()  { return _func->GetInteger( FIELD_DATATYPE ); }
  Standard_EXPORT int GetDimension() { return _func->GetInteger( FIELD_DIMENSION ); }
  Standard_EXPORT Handle(TColStd_HArray1OfExtendedString) GetComponents()
  { return _func->GetStringArray( FIELD_COMPONENTS ); }

  Standard_EXPORT Handle(GEOM_Function) GetField()
  { return _func->GetReference( STEP_FIELD ); }
  Standard_EXPORT int GetStepID() { return _func->GetInteger( STEP_ID ); }
  Standard_EXPORT int GetStepStamp() { return _func->GetInteger( STEP_STAMP ); }
  Standard_EXPORT Handle(TColStd_HArray1OfInteger) GetIntValues()
  { return _func->GetIntegerArray( STEP_VALUES ); }
  Standard_EXPORT Handle(TColStd_HArray1OfReal) GetDoubleValues()
  { return _func->GetRealArray( STEP_VALUES ); }
  Standard_EXPORT Handle(TColStd_HArray1OfExtendedString) GetStringValues()
  { return _func->GetStringArray( STEP_VALUES ); }

 private:

  void SetShape(Handle(GEOM_Function) theS) { _func->SetReference( FIELD_SHAPE, theS ); }
  void SetDataType( int type ) { _func->SetInteger( FIELD_DATATYPE, type ); }
  void SetDimension( int dim ) { _func->SetInteger( FIELD_DIMENSION, dim ); }
  void SetComponents( const Handle(TColStd_HArray1OfExtendedString)& compNames )
  { _func->SetStringArray( FIELD_COMPONENTS, compNames ); }

  friend class GEOM_Field;

  void SetField(Handle(GEOM_Function) theF) { _func->SetReference( STEP_FIELD, theF ); }
  void SetStepStamp( int stamp ) { _func->SetInteger( STEP_STAMP, stamp ); }
  void SetStepID( int step ) { _func->SetInteger( STEP_ID, step ); }
  // void SetStepIDs( Handle(TColStd_HArray1OfInteger) steps )
  // { _func->SetRealArray( FIELD_STEP_IDS, steps ); }
  void SetValues( const Handle(TColStd_HArray1OfInteger)& values )
  { _func->SetIntegerArray( STEP_VALUES, values ); }
  void SetValues( const Handle(TColStd_HArray1OfReal)& values )
  { _func->SetRealArray( STEP_VALUES, values ); }
  void SetValues( const Handle(TColStd_HArray1OfExtendedString)& values )
  { _func->SetStringArray( STEP_VALUES, values ); }

  friend class GEOM_FieldStep;

  Handle(GEOM_Function) _func;
};
