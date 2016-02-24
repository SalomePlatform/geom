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

#include "GEOM_IFieldOperations_i.hh"

#include "utilities.h"
#include "OpUtil.hxx"
#include "Utils_ExceptHandlers.hxx"

#include "GEOM_Engine.hxx"
#include "GEOM_Object.hxx"
#include "GEOM_Field.hxx"

//=============================================================================
/*
 *   constructor:
 */
//=============================================================================
GEOM_IFieldOperations_i::GEOM_IFieldOperations_i (PortableServer::POA_ptr thePOA,
                                                  GEOM::GEOM_Gen_ptr theEngine,
                                                  ::GEOMImpl_IFieldOperations* theImpl)
  :GEOM_IOperations_i(thePOA, theEngine, theImpl)
{
  MESSAGE("GEOM_IFieldOperations_i::GEOM_IFieldOperations_i");
}

//=============================================================================
/*
 *  destructor
 */
//=============================================================================
GEOM_IFieldOperations_i::~GEOM_IFieldOperations_i()
{
  MESSAGE("GEOM_IFieldOperations_i::~GEOM_IFieldOperations_i");
}


//=============================================================================
/*
 *  \brief Creates a field
 */
//=============================================================================

GEOM::GEOM_Field_ptr
GEOM_IFieldOperations_i::CreateField(GEOM::GEOM_Object_ptr     theShape,
                                     const char*               theName,
                                     GEOM::field_data_type     theType,
                                     CORBA::Short              theDimension,
                                     const GEOM::string_array& theComponentNames)
{
  GEOM::GEOM_Field_var aField;

  Handle(TColStd_HArray1OfExtendedString) compNames;
  if ( theComponentNames.length() > 0 )
  {
    compNames = new TColStd_HArray1OfExtendedString( 1, theComponentNames.length() );
    for ( size_t i = 0; i < theComponentNames.length(); ++i )
      compNames->SetValue( i+1, theComponentNames[ i ].in() );
  }
  HANDLE_NAMESPACE(GEOM_Object) aShapeRef = GetObjectImpl( theShape );
  HANDLE_NAMESPACE(GEOM_Field)  aFieldRef = GetOperations()->CreateField( aShapeRef,
                                                                theName,
                                                                (int) theType,
                                                                theDimension,
                                                                compNames );
  if ( !aFieldRef.IsNull() )
    aField = GEOM::GEOM_Field::_narrow( GetBaseObject( aFieldRef ));

  return aField._retn();
}

//=============================================================================
/*
 *  \brief Returns number of fields on a shape
 */
//=============================================================================

CORBA::Long GEOM_IFieldOperations_i::CountFields( GEOM::GEOM_Object_ptr shape)
{
  HANDLE_NAMESPACE(GEOM_Object) go = GetObjectImpl( shape );

  return ( CORBA::Long) GetOperations()->CountFields( go );
}

//=============================================================================
/*
 *  \brief Returns all fields on a shape
 */
//=============================================================================

GEOM::ListOfFields* GEOM_IFieldOperations_i::GetFields( GEOM::GEOM_Object_ptr shape )
{
  GEOM::ListOfFields_var fields = new GEOM::ListOfFields;

  HANDLE_NAMESPACE(GEOM_Object) go = GetObjectImpl( shape );

  Handle(TColStd_HSequenceOfTransient) seq = GetOperations()->GetFields( go );
  if ( !seq.IsNull() )
  {
    fields->length( seq->Length() );
    int nbF = 0;
    for ( int i = 1; i <= seq->Length(); ++i )
    {
      HANDLE_NAMESPACE(GEOM_Field)      f = HANDLE_NAMESPACE(GEOM_Field)::DownCast( seq->Value( i ));
      GEOM::GEOM_Field_var fvar = GEOM::GEOM_Field::_narrow( GetBaseObject( f ));
      if ( !fvar->_is_nil() )
        fields[ nbF++ ] = fvar._retn();
    }
    fields->length( nbF );
  }
  return fields._retn();
}

//=============================================================================
/*
 *  \brief Returns all fields on a shape
 */
//=============================================================================

GEOM::GEOM_Field_ptr GEOM_IFieldOperations_i::GetField( GEOM::GEOM_Object_ptr shape,
                                                        const char*           name)
{
  GEOM::GEOM_Field_var field;

  HANDLE_NAMESPACE(GEOM_Object) go = GetObjectImpl( shape );

  HANDLE_NAMESPACE(GEOM_Field) f = GetOperations()->GetField( go, name );
  field = GEOM::GEOM_Field::_narrow( GetBaseObject( f ));

  return field._retn();
}

//=============================================================================
/*
 *  \brief Returns number of sub-shapes of given dimension
 */
//=============================================================================

::CORBA::Long GEOM_IFieldOperations_i::GetNbSubShapes(::GEOM::GEOM_Object_ptr shape,
                                                      ::CORBA::Short          fieldDim)
{
  HANDLE_NAMESPACE(GEOM_Object) go = GetObjectImpl( shape );
  return GEOM_Field::GetNbSubShapes( go, fieldDim );
}
