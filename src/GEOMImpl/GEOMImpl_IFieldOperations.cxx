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

#include "GEOMImpl_IFieldOperations.hxx"

#include "GEOMImpl_Types.hxx"

#include "GEOM_Function.hxx"
#include "GEOM_IField.hxx"
#include "GEOM_Object.hxx"
#include "GEOM_PythonDump.hxx"

#include <TDataStd_ListIteratorOfListOfExtendedString.hxx>

#include <utilities.h>


//=============================================================================
/*!
 *   constructor:
 */
//=============================================================================
GEOMImpl_IFieldOperations::GEOMImpl_IFieldOperations (GEOM_Engine* theEngine, int theDocID)
: GEOM_IOperations(theEngine, theDocID)
{
  MESSAGE("GEOMImpl_IFieldOperations::GEOMImpl_IFieldOperations");
}

//=============================================================================
/*!
 *  destructor
 */
//=============================================================================
GEOMImpl_IFieldOperations::~GEOMImpl_IFieldOperations()
{
  MESSAGE("GEOMImpl_IFieldOperations::~GEOMImpl_IFieldOperations");
}


//=============================================================================
/*!
 *  CreateField
 */
//=============================================================================
Handle(GEOM_Field) GEOMImpl_IFieldOperations::
CreateField( const Handle(GEOM_Object)&                     theShape,
             const char*                                    theName,
             const int                                      theType,
             const int                                      theDimension,
             const Handle(TColStd_HArray1OfExtendedString)& theComponentNames)
{
  SetErrorCode(KO);

  // check input data
  if ( theShape.IsNull() ) {
    SetErrorCode( "Error: NULL shape" );
    return NULL;
  }
  if ( !theName || !theName[0] ) {
    SetErrorCode( "Error: empty field name" );
    return NULL;
  }
  if ( theType < 0 || theType > 3) {
    SetErrorCode( "Error: invalid field type."
                  "Valid types are: 0 - boolean, 1 - integer, 2 - double, 3 - string");
    return NULL;
  }
  if ( theDimension < -1 || theDimension > 3) {
    SetErrorCode( "Error: invalid shape dimension."
                  "Valid types are: 0 - VERTEX, 1 - EDGE, 2 - FACE, 3 - SOLID, -1 - whole shape");
    return NULL;
  }
  if ( theComponentNames.IsNull() || theComponentNames->Length() < 1 ) {
    SetErrorCode( "Error: no component names provided");
    return NULL;
  }

  // make a field
  Handle(GEOM_Field) aField = Handle(GEOM_Field)::DownCast
    ( GetEngine()->AddBaseObject( GetDocID(), GEOM_FIELD ));

  // set field data
  aField->Init( theShape, theName, theType, theDimension, theComponentNames );

  // remember aField as a sub-object of theShape
  Handle(GEOM_Function) aFieldFun = aField->GetFunction(1);
  Handle(GEOM_Function) aShapeFun = theShape->GetFunction(1);
  aShapeFun->AddSubShapeReference(aFieldFun);

  //make a Python command
  GEOM::TPythonDump(aFieldFun) << aField << " = geompy.CreateField( "
                               << theShape << ", '"
                               << theName << "', "
                               << "GEOM.FDT_" << aField->GetDataTypeString( theType ) << ", "
                               << theDimension << ", "
                               << aField->GetComponentsForPython() << ")";
  SetErrorCode(OK);
  return aField;
}

//=======================================================================
//function : CountFields
//purpose  : Returns number of fields on a shape
//=======================================================================

int GEOMImpl_IFieldOperations::CountFields( const Handle(GEOM_Object)& theShape )
{
  SetErrorCode(OK);

  if ( theShape.IsNull() )
    return 0;

  Handle(GEOM_Function) aShapeFun = theShape->GetFunction(1);
  if ( aShapeFun.IsNull() || !aShapeFun->HasSubShapeReferences() )
    return 0;

  const TDataStd_ListOfExtendedString& aListEntries = aShapeFun->GetSubShapeReferences();
  if (aListEntries.IsEmpty()) return 0;

  int nbFields = 0;
  char entry[200], *pentry = entry; // the entry can't be too long
  TDataStd_ListIteratorOfListOfExtendedString anIt (aListEntries);
  for (; anIt.More(); anIt.Next()) {
    TCollection_ExtendedString& anEntry = anIt.Value();
    anEntry.ToUTF8CString( (Standard_PCharacter&) pentry );
    Handle(GEOM_BaseObject) anObj = GetEngine()->GetObject(GetDocID(), entry, false);
    nbFields += ( !anObj.IsNull() && anObj->IsKind(STANDARD_TYPE(GEOM_Field)) );
  }

  return nbFields;
}

//=======================================================================
//function : GetFields
//purpose  : Returns all fields on a shape
//=======================================================================

Handle(TColStd_HSequenceOfTransient)
GEOMImpl_IFieldOperations::GetFields( const Handle(GEOM_Object)& theShape )
{
  SetErrorCode(KO);

  Handle(TColStd_HSequenceOfTransient) fields = new TColStd_HSequenceOfTransient;
  if ( theShape.IsNull() ) {
    SetErrorCode( "Error: NULL shape" );
    return fields;
  }
  SetErrorCode(OK);

  Handle(GEOM_Function) aShapeFun = theShape->GetFunction(1);
  if ( aShapeFun.IsNull() || !aShapeFun->HasSubShapeReferences() )
    return fields;

  const TDataStd_ListOfExtendedString& aListEntries = aShapeFun->GetSubShapeReferences();
  if (aListEntries.IsEmpty()) return fields;

  char entry[200], *pentry = entry; // the entry can't be too long
  TDataStd_ListIteratorOfListOfExtendedString anIt (aListEntries);
  for (; anIt.More(); anIt.Next()) {
    TCollection_ExtendedString& anEntry = anIt.Value();
    anEntry.ToUTF8CString( (Standard_PCharacter&) pentry );
    Handle(GEOM_BaseObject) anObj = GetEngine()->GetObject(GetDocID(), entry, false);
    if ( !anObj.IsNull() && anObj->IsKind(STANDARD_TYPE(GEOM_Field)) )
    {
        Handle(GEOM_Field) field = Handle(GEOM_Field)::DownCast( anObj );
        if ( !field.IsNull() )
          fields->Append( field );
    }
  }

  return fields;
}

//=======================================================================
//function : GetField
//purpose  : Returns a field on a shape by its name
//=======================================================================

Handle(GEOM_Field)
GEOMImpl_IFieldOperations::GetField( const Handle(GEOM_Object)& theShape,
                                     const char*                theName )
{
  SetErrorCode(OK);

  Handle(GEOM_Field) field;
  if ( theShape.IsNull() ) {
    //SetErrorCode( "Error: NULL shape" );
    return field;
  }
  Handle(GEOM_Function) aShapeFun = theShape->GetFunction(1);
  if ( aShapeFun.IsNull() || !aShapeFun->HasSubShapeReferences() ) {
    //SetErrorCode( "Error: No fields on the shape at all" );
    return field;
  }
  const TDataStd_ListOfExtendedString& aListEntries = aShapeFun->GetSubShapeReferences();
  if (aListEntries.IsEmpty())  {
    //SetErrorCode( "Error: No fields on the shape at all" );
    return field;
  }

  char entry[200], *pentry = entry; // the entry can't be too long
  TDataStd_ListIteratorOfListOfExtendedString anIt (aListEntries);
  for (; anIt.More(); anIt.Next()) {
    TCollection_ExtendedString& anEntry = anIt.Value();
    anEntry.ToUTF8CString( (Standard_PCharacter&) pentry );
    field = Handle(GEOM_Field)::DownCast( GetEngine()->GetObject( GetDocID(), entry, false ));
    if ( !field.IsNull() && field->GetName() == theName ) {
      SetErrorCode(OK);
      break;
    }
  }

  return field;
}
