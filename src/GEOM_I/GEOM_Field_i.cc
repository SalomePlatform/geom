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

#include "GEOM_Field_i.hh"

#include <utilities.h>

#include <TCollection_ExtendedString.hxx>
#include <TColStd_HArray1OfReal.hxx>

#ifdef WIN32
#pragma warning( disable:4786 )
#endif

//=============================================================================
/*!
 *   constructor:
 */
//=============================================================================

GEOM_Field_i::GEOM_Field_i (PortableServer::POA_ptr thePOA,
                            GEOM::GEOM_Gen_ptr      theEngine,
                            HANDLE_NAMESPACE(GEOM_Field)      theImpl):
  SALOME::GenericObj_i( thePOA ),
  GEOM_BaseObject_i( thePOA, theEngine, theImpl ),
  _impl( theImpl )
{
}

//=============================================================================
/*!
 *  destructor
 */
//=============================================================================

GEOM_Field_i::~GEOM_Field_i()
{
  MESSAGE("GEOM_Field_i::~GEOM_Field_i");
}

//================================================================================
/*!
 * \brief Returns the shape the field lies on
 */
//================================================================================

GEOM::GEOM_Object_ptr GEOM_Field_i::GetShape()
{
  GEOM::GEOM_Object_var shapeVar;
  HANDLE_NAMESPACE(GEOM_Object) shape = _impl->GetShape();
  if ( !shape.IsNull() )
  {
    GEOM::GEOM_BaseObject_var obj = _engine->GetObject( shape->GetDocID(),
                                                        shape->GetEntryString().ToCString());
    shapeVar = GEOM::GEOM_Object::_narrow( obj );
  }
  return shapeVar._retn();
}

//================================================================================
/*!
 * \brief Returns type of field data
 */
//================================================================================

GEOM::field_data_type GEOM_Field_i::GetDataType()
{
  return GEOM::field_data_type( _impl->GetDataType() );
}

//================================================================================
/*!
 * \brief Returns dimension of the shape the field lies on
 *  0 - VERTEX, 1 - EDGE, 2 - FACE, 3 - SOLID, -1 - whole shape
 */
//================================================================================

CORBA::Short GEOM_Field_i::GetDimension()
{
  return CORBA::Short( _impl->GetDimension() );
}

//================================================================================
/*!
 * \brief Returns names of components
 */
//================================================================================

GEOM::string_array* GEOM_Field_i::GetComponents()
{
  GEOM::string_array_var compArr = new GEOM::string_array;
  Handle(TColStd_HArray1OfExtendedString) comps = _impl->GetComponents();
  if ( !comps.IsNull() )
  {
    compArr->length( comps->Length() );
    std::string entry;
    int i, i0, nb; 
    for ( i = i0 = comps->Lower(), nb = comps->Upper(); i <= nb; ++i )
    {
      const TCollection_ExtendedString& anEntry = comps->Value( i );
      entry.resize( anEntry.LengthOfCString() );
      char* pstr = &entry[0];
      anEntry.ToUTF8CString( pstr );
      compArr[ i-i0 ] = CORBA::string_dup( entry.c_str() );
    }
  }
  return compArr._retn();
}

//================================================================================
/*!
 * \brief Adds a time step to the field
 */
//================================================================================

GEOM::GEOM_FieldStep_ptr GEOM_Field_i::AddStep(::CORBA::Long stepID, ::CORBA::Long stamp)
{
  GEOM::GEOM_FieldStep_var stepVar;
  HANDLE_NAMESPACE(GEOM_FieldStep) step = _impl->AddStep( stepID, stamp );
  if ( !step.IsNull() )
  {
    GEOM::GEOM_BaseObject_var obj = _engine->GetObject( step->GetDocID(),
                                                        step->GetEntryString().ToCString());
    stepVar = GEOM::GEOM_FieldStep::_narrow( obj );
  }
  return stepVar._retn();
}

//================================================================================
/*!
 * \brief Remove a time step from the field
 */
//================================================================================

void GEOM_Field_i::RemoveStep(::CORBA::Long stepID)
{
  _impl->RemoveStep( stepID );
}

//================================================================================
/*!
 * \brief Returns number of time steps in the field
 */
//================================================================================

CORBA::Long GEOM_Field_i::CountSteps()
{
  return (CORBA::Long) _impl->GetSteps().size();
}

//================================================================================
/*!
 * \brief Returns a list of time step IDs in the field
 */
//================================================================================

GEOM::ListOfLong* GEOM_Field_i::GetSteps()
{
  std::list< HANDLE_NAMESPACE(GEOM_FieldStep)> stepList = _impl->GetSteps();
  std::list< HANDLE_NAMESPACE(GEOM_FieldStep)>::iterator stp = stepList.begin();
  
  GEOM::ListOfLong_var stepIds = new GEOM::ListOfLong();
  stepIds->length( stepList.size() );
  int nbS = 0;
  for ( ; stp != stepList.end(); ++stp )
    if ( !stp->IsNull() )
      stepIds[ nbS++ ] = (*stp)->GetID();

  stepIds->length( nbS );
  return stepIds._retn();
}

//================================================================================
/*!
 * \brief Returns a time step by its ID
 */
//================================================================================

GEOM::GEOM_FieldStep_ptr GEOM_Field_i::GetStep(CORBA::Long stepID)
{
  GEOM::GEOM_FieldStep_var stepVar;
  HANDLE_NAMESPACE(GEOM_FieldStep) step = _impl->GetStep(stepID);
  if ( !step.IsNull() )
  {
    GEOM::GEOM_BaseObject_var obj = _engine->GetObject( step->GetDocID(),
                                                        step->GetEntryString().ToCString());
    stepVar = GEOM::GEOM_FieldStep::_narrow( obj );
  }
  return stepVar._retn();
}

//================================================================================
/*!
 *  \brief Returns a size of data array that is to be passed to
 *         GEOM_FieldStep.SetValues( dataArray ). This size depends on the
 *         number of sub-shapes of field dimension and the number of components
 */
//================================================================================

CORBA::Long GEOM_Field_i::GetArraySize()
{
  return (CORBA::Long) _impl->GetArrayLength();
}

//================================================================================
/*!
 * \brief Constructor of base step class
 */
//================================================================================

GEOM_FieldStep_i::GEOM_FieldStep_i(PortableServer::POA_ptr  thePOA,
                                   GEOM::GEOM_Gen_ptr       theEngine,
                                   HANDLE_NAMESPACE(GEOM_FieldStep)   theImpl):
  SALOME::GenericObj_i( thePOA ),
  GEOM_BaseObject_i( thePOA, theEngine, theImpl ),
  _impl( theImpl )
{
}

//================================================================================
/*!
 * \brief Destructor
 */
//================================================================================

GEOM_FieldStep_i::~GEOM_FieldStep_i()
{
}

//================================================================================
/*!
 * \brief Changes the stamp of the field step
 */
//================================================================================

void GEOM_FieldStep_i::SetStamp(::CORBA::Long stamp)
{
  _impl->SetStamp( stamp );
}

//================================================================================
/*!
 * \brief Returns the time of the field step
 */
//================================================================================

::CORBA::Long GEOM_FieldStep_i::GetStamp()
{
  return (CORBA::Long) _impl->GetStamp();
}

//================================================================================
/*!
 * \brief Returns the number of the field step
 */
//================================================================================

::CORBA::Long GEOM_FieldStep_i::GetID()
{
  return (CORBA::Long) _impl->GetID();
}

//================================================================================
/*!
 * \brief Returns the field the step belongs to
 */
//================================================================================

GEOM::GEOM_Field_ptr GEOM_FieldStep_i::GetField()
{
  GEOM::GEOM_Field_var fieldVar;
  HANDLE_NAMESPACE(GEOM_Field) field = _impl->GetField();
  if ( !field.IsNull() )
  {
    GEOM::GEOM_BaseObject_var obj = _engine->GetObject( field->GetDocID(),
                                                        field->GetEntryString().ToCString());
    fieldVar = GEOM::GEOM_Field::_narrow( obj );
  }
  return fieldVar._retn();
}

//================================================================================
/*!
 * \brief Constructor of GEOM_BoolFieldStep_i
 */
//================================================================================

GEOM_BoolFieldStep_i::GEOM_BoolFieldStep_i(PortableServer::POA_ptr thePOA, GEOM::GEOM_Gen_ptr theEngine, HANDLE_NAMESPACE(GEOM_FieldStep) theImpl):
  SALOME::GenericObj_i( thePOA ),
  GEOM_BaseObject_i( thePOA, theEngine, theImpl ),
  GEOM_FieldStep_i( thePOA, theEngine, theImpl )
{
}

//================================================================================
/*!
 * \brief Changes values of the field step. Returns false if number of values is wrong
 */
//================================================================================

::CORBA::Boolean GEOM_BoolFieldStep_i::SetValues(const ::GEOM::short_array& boolValues)
{
  Handle(TColStd_HArray1OfInteger) values =
    new TColStd_HArray1OfInteger( 0, boolValues.length()-1 );
  for ( size_t i = 0; i < boolValues.length(); ++ i )
    values->SetValue( i, boolValues[i] );

  return _impl->SetValues( values );
}

//================================================================================
/*!
 * \brief Returns values of the field step
 */
//================================================================================

GEOM::short_array* GEOM_BoolFieldStep_i::GetValues()
{
  GEOM::short_array_var resArray = new GEOM::short_array;

  Handle(TColStd_HArray1OfInteger) values = GEOM_FieldStep_i::_impl->GetIntValues();
  if ( !values.IsNull() )
  {
    resArray->length( values->Length() );
    int i, i0, nb; 
    for ( i = i0 = values->Lower(), nb = values->Upper(); i <= nb; ++i )
      resArray[ i-i0 ] = values->Value( i );
  }
  return resArray._retn();
}


//================================================================================
/*!
 * \brief Constructor of GEOM_IntFieldStep_i
 */
//================================================================================

GEOM_IntFieldStep_i::GEOM_IntFieldStep_i(PortableServer::POA_ptr thePOA, GEOM::GEOM_Gen_ptr theEngine, HANDLE_NAMESPACE(GEOM_FieldStep) theImpl):
  SALOME::GenericObj_i( thePOA ),
  GEOM_BaseObject_i( thePOA, theEngine, theImpl ),
  GEOM_FieldStep_i( thePOA, theEngine, theImpl )
{
}

//================================================================================
/*!
 * \brief Changes values of the field step. Returns false if number of values is wrong
 */
//================================================================================

::CORBA::Boolean GEOM_IntFieldStep_i::SetValues(const ::GEOM::ListOfLong& intValues)
{
  Handle(TColStd_HArray1OfInteger) values =
    new TColStd_HArray1OfInteger( 0, intValues.length()-1 );
  for ( size_t i = 0; i < intValues.length(); ++ i )
    values->SetValue( i, intValues[i] );

  return _impl->SetValues( values );
}

//================================================================================
/*!
 * \brief Returns values of the field step
 */
//================================================================================

GEOM::ListOfLong* GEOM_IntFieldStep_i::GetValues()
{
  GEOM::ListOfLong_var resArray = new GEOM::ListOfLong;

  Handle(TColStd_HArray1OfInteger) values = GEOM_FieldStep_i::_impl->GetIntValues();
  if ( !values.IsNull() )
  {
    resArray->length( values->Length() );
    int i, i0, nb; 
    for ( i = i0 = values->Lower(), nb = values->Upper(); i <= nb; ++i )
      resArray[ i-i0 ] = values->Value( i );
  }
  return resArray._retn();
}


//================================================================================
/*!
 * \brief Constructor of GEOM_DoubleFieldStep_i
 */
//================================================================================

GEOM_DoubleFieldStep_i::GEOM_DoubleFieldStep_i(PortableServer::POA_ptr thePOA, GEOM::GEOM_Gen_ptr theEngine, HANDLE_NAMESPACE(GEOM_FieldStep) theImpl):
  SALOME::GenericObj_i( thePOA ),
  GEOM_BaseObject_i( thePOA, theEngine, theImpl ),
  GEOM_FieldStep_i( thePOA, theEngine, theImpl )
{
}

//================================================================================
/*!
 * \brief Changes values of the field step. Returns false if number of values is wrong
 */
//================================================================================

::CORBA::Boolean GEOM_DoubleFieldStep_i::SetValues(const ::GEOM::ListOfDouble& dblValues)
{
  Handle(TColStd_HArray1OfReal) values =
    new TColStd_HArray1OfReal( 0, dblValues.length()-1 );
  for ( size_t i = 0; i < dblValues.length(); ++ i )
    values->SetValue( i, dblValues[i] );

  return _impl->SetValues( values );
}

//================================================================================
/*!
 * \brief Returns values of the field step
 */
//================================================================================

GEOM::ListOfDouble* GEOM_DoubleFieldStep_i::GetValues()
{
  GEOM::ListOfDouble_var resArray = new GEOM::ListOfDouble;

  Handle(TColStd_HArray1OfReal) values = GEOM_FieldStep_i::_impl->GetDoubleValues();
  if ( !values.IsNull() )
  {
    resArray->length( values->Length() );
    int i, i0, nb; 
    for ( i = i0 = values->Lower(), nb = values->Upper(); i <= nb; ++i )
      resArray[ i-i0 ] = values->Value( i );
  }
  return resArray._retn();
}


//================================================================================
/*!
 * \brief Constructor of GEOM_StringFieldStep_i
 */
//================================================================================

GEOM_StringFieldStep_i::GEOM_StringFieldStep_i(PortableServer::POA_ptr thePOA, GEOM::GEOM_Gen_ptr theEngine, HANDLE_NAMESPACE(GEOM_FieldStep) theImpl):
  SALOME::GenericObj_i( thePOA ),
  GEOM_BaseObject_i( thePOA, theEngine, theImpl ),
  GEOM_FieldStep_i( thePOA, theEngine, theImpl )
{
}

//================================================================================
/*!
 * \brief Changes values of the field step. Returns false if number of values is wrong
 */
//================================================================================

::CORBA::Boolean GEOM_StringFieldStep_i::SetValues(const ::GEOM::string_array& strValues)
{
  Handle(TColStd_HArray1OfExtendedString) values =
    new TColStd_HArray1OfExtendedString( 0, strValues.length()-1 );
  for ( size_t i = 0; i < strValues.length(); ++ i )
    values->SetValue( i, strValues[i].in() );

  return _impl->SetValues( values );
}

//================================================================================
/*!
 * \brief Returns values of the field step
 */
//================================================================================

GEOM::string_array* GEOM_StringFieldStep_i::GetValues()
{
  GEOM::string_array_var resArray = new GEOM::string_array;

  Handle(TColStd_HArray1OfExtendedString) values = GEOM_FieldStep_i::_impl->GetStringValues();
  if ( !values.IsNull() )
  {
    resArray->length( values->Length() );
    std::string entry;
    int i, i0, nb; 
    for ( i = i0 = values->Lower(), nb = values->Upper(); i <= nb; ++i )
    {
      const TCollection_ExtendedString& anEntry = values->Value( i );
      entry.resize( anEntry.LengthOfCString() );
      char* pstr = &entry[0];
      anEntry.ToUTF8CString( pstr );
      resArray[ i-i0 ] = CORBA::string_dup( entry.c_str() );
    }
  }
  return resArray._retn();
}

