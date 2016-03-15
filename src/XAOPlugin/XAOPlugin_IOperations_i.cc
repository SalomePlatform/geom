// Copyright (C) 2014-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

// internal includes
#include "XAOPlugin_IOperations_i.hh"
#include "XAOPlugin_IOperations.hxx"

// KERNEL includes
#include <utilities.h>

//=============================================================================
/*!
 *   constructor:
 */
//=============================================================================
XAOPlugin_IOperations_i::XAOPlugin_IOperations_i( PortableServer::POA_ptr thePOA,
						  GEOM::GEOM_Gen_ptr theEngine,
						  XAOPlugin_IOperations* theImpl )
:GEOM_IOperations_i( thePOA, theEngine, theImpl )
{
  MESSAGE( "XAOPlugin_IOperations_i::XAOPlugin_IOperations_i" );
}

//=============================================================================
/*!
 *  destructor
 */
//=============================================================================
XAOPlugin_IOperations_i::~XAOPlugin_IOperations_i()
{
  MESSAGE( "XAOPlugin_IOperations_i::~XAOPlugin_IOperations_i" );
}

//=============================================================================
/*!
 *  Export a shape to XAO format
 *  \param shape The shape to export
 *  \param groups The list of groups to export
 *  \param fields The list of fields to export
 *  \param author The author of the export
 *  \param fileName The name of the exported file
 *  \return boolean indicating if export was succeful.
 */
//=============================================================================
CORBA::Boolean XAOPlugin_IOperations_i::ExportXAO( GEOM::GEOM_Object_ptr shape,
						   const GEOM::ListOfGO& groups,
						   const GEOM::ListOfFields& fields,
						   const char* author,
						   const char* fileName)
{
  bool isGood = false;
  // Set a not done flag
  GetOperations()->SetNotDone();

  // Get the reference shape
  Handle(GEOM_Object) reference = GetObjectImpl( shape );

  // Get the reference groups
  int ind = 0;
  std::list<Handle(GEOM_Object)> groupsObj;
  for (; ind < groups.length(); ind++)
  {
    Handle(GEOM_Object) gobj = GetObjectImpl( groups[ind] );
    if (gobj.IsNull()) return false;
    groupsObj.push_back(gobj);
  }

  // Get the reference fields
  ind = 0;
  std::list<Handle(GEOM_Field)> fieldsObj;
  for( ; ind < fields.length(); ind++ )
  {
    Handle(GEOM_Field) fobj = Handle(GEOM_Field)::DownCast( GetBaseObjectImpl( fields[ind] ) );
    if( fobj.IsNull() ) return false;
    fieldsObj.push_back(fobj);
  }

  if( !reference.IsNull() )
  {
    // Export XAO
    isGood = GetOperations()->ExportXAO( reference, groupsObj, fieldsObj, author, fileName );
  }

  return isGood;
}

//=============================================================================
/*!
 *  Import a shape from XAO format
 *  \param fileName The name of the file to import
 *  \param shape The imported shape
 *  \param subShapes The list of imported subShapes
 *  \param groups The list of imported groups
 *  \param fields The list of imported fields
 *  \return boolean indicating if import was succeful.
 */
//=============================================================================
CORBA::Boolean XAOPlugin_IOperations_i::ImportXAO( const char* fileName,
						   GEOM::GEOM_Object_out shape,
						   GEOM::ListOfGO_out subShapes,
						   GEOM::ListOfGO_out groups,
						   GEOM::ListOfFields_out fields)
{
  GEOM::GEOM_Object_var vshape;
  shape = vshape._retn();

  subShapes = new GEOM::ListOfGO;
  groups = new GEOM::ListOfGO;
  fields = new GEOM::ListOfFields;

  // Set a not done flag
  GetOperations()->SetNotDone();

  Handle(TColStd_HSequenceOfTransient) importedSubShapes = new TColStd_HSequenceOfTransient();
  Handle(TColStd_HSequenceOfTransient) importedGroups = new TColStd_HSequenceOfTransient();
  Handle(TColStd_HSequenceOfTransient) importedFields = new TColStd_HSequenceOfTransient();
  Handle(GEOM_Object) hshape;
  bool res = GetOperations()->ImportXAO( fileName, hshape, importedSubShapes, importedGroups, importedFields );

  if( !GetOperations()->IsDone() || !res )
    return false;

  // parse fields
  int n = importedSubShapes->Length();
  subShapes->length(n);
  for( int i = 1; i <= n; i++ )
  {
    (*subShapes)[i - 1] = GetObject( Handle(GEOM_Object)::DownCast( importedSubShapes->Value(i) ) );
  }

  // parse groups
  n = importedGroups->Length();
  groups->length(n);
  for( int i = 1; i <= n; i++ )
  {
    (*groups)[i - 1] = GetObject( Handle(GEOM_Object)::DownCast( importedGroups->Value(i) ) );
  }

  // parse fields
  n = importedFields->Length();
  fields->length(n);
  for( int i = 1; i <= n; i++ )
  {
    (*fields)[i - 1] = GEOM::GEOM_Field::_narrow(
      GetBaseObject( Handle(GEOM_Field)::DownCast( importedFields->Value(i) ) ) );
  }

  shape = GetObject( hshape );

  return res;
}

XAOPlugin_IOperations* XAOPlugin_IOperations_i::GetOperations()
{
  return (XAOPlugin_IOperations*)GetImpl();
}
