// Copyright (C) 2014-2024  CEA, EDF, OPEN CASCADE
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
 *  Export a shape to XAO format file
 *  \param shape The shape to export
 *  \param groups The list of groups to export
 *  \param fields The list of fields to export
 *  \param author The author of the export
 *  \param fileName The name of the exported file
 *  \param shapeFileName If not empty, save the BREP shape to this external file
 *  \return boolean indicating if export was successful.
 */
//=============================================================================
CORBA::Boolean XAOPlugin_IOperations_i::ExportXAO( GEOM::GEOM_Object_ptr shape,
                                                   const GEOM::ListOfGO& groups,
                                                   const GEOM::ListOfFields& fields,
                                                   const char* author,
                                                   const char* fileName,
                                                   const char* shapeFileName)
{
  exportXAO( shape, groups, fields, author, true, fileName, shapeFileName );
  return IsDone();
}

//=============================================================================
/*!
 *  Export a shape to XAO format buffer
 *  \param shape The shape to export
 *  \param groups The list of groups to export
 *  \param fields The list of fields to export
 *  \param author The author of the export
 *  \return The output buffer
 */
//=============================================================================
SALOMEDS::TMPFile* XAOPlugin_IOperations_i::ExportXAOMem( GEOM::GEOM_Object_ptr shape,
                                                          const GEOM::ListOfGO& groups,
                                                          const GEOM::ListOfFields& fields,
                                                          const char* author )
{
  std::string anXMLBuf = exportXAO( shape, groups, fields, author, false, "", "" );

  int size = anXMLBuf.size();
  CORBA::Octet* OctetBuf = SALOMEDS::TMPFile::allocbuf(size);
  memcpy(OctetBuf, anXMLBuf.c_str(), size);
  SALOMEDS::TMPFile_var SeqFile = new SALOMEDS::TMPFile (size,size,OctetBuf,1);
  return SeqFile._retn();
}

//=============================================================================
/*!
 *  Export a shape to XAO format
 *  \param shape The shape to export
 *  \param groups The list of groups to export
 *  \param fields The list of fields to export
 *  \param author The author of the export
 *  \param toFile Export to file if true, otherwise export to memory buffer (the returned string)
 *  \param fileName The name of the exported file
 *  \param shapeFileName If not empty, save the BREP shape to this external file
 *  \return string The exported buffer if toFile=false, otherwise an empty string
 */
//=============================================================================
std::string XAOPlugin_IOperations_i::exportXAO( GEOM::GEOM_Object_ptr shape,
                                                const GEOM::ListOfGO& groups,
                                                const GEOM::ListOfFields& fields,
                                                const char* author,
                                                const bool  toFile,
                                                const char* fileName,
                                                const char* shapeFileName)
{
  std::string anXMLBuff;
  // Set a not done flag
  GetOperations()->SetNotDone();

  // Get the reference shape
  Handle(GEOM_Object) reference = GetObjectImpl( shape );
  if( reference.IsNull() )
    return anXMLBuff;

  // Get the reference groups
  CORBA::ULong ind = 0;
  std::list<Handle(GEOM_Object)> groupsObj;
  for (; ind < groups.length(); ind++)
  {
    Handle(GEOM_Object) gobj = GetObjectImpl( groups[ind] );
    if (gobj.IsNull()) return anXMLBuff;
    groupsObj.push_back(gobj);
  }

  // Get the reference fields
  ind = 0;
  std::list<Handle(GEOM_Field)> fieldsObj;
  for( ; ind < fields.length(); ind++ )
  {
    Handle(GEOM_Field) fobj = Handle(GEOM_Field)::DownCast( GetBaseObjectImpl( fields[ind] ) );
    if( fobj.IsNull() ) return anXMLBuff;
    fieldsObj.push_back(fobj);
  }

  if ( toFile )
  {
    // Export XAO
    GetOperations()->ExportXAO( reference, groupsObj, fieldsObj, author, fileName, shapeFileName );
  }
  else
  {
    anXMLBuff = GetOperations()->ExportXAOMem( reference, groupsObj, fieldsObj, author );
  }

  return anXMLBuff;
}

//=============================================================================
/*!
 *  Import a shape from XAO format
 *  \param fileName The name of the file to import
 *  \param shape The imported shape
 *  \param subShapes The list of imported subShapes
 *  \param groups The list of imported groups
 *  \param fields The list of imported fields
 *  \return boolean indicating if import was successful.
 */
//=============================================================================
CORBA::Boolean XAOPlugin_IOperations_i::ImportXAO( const char* fileName,
						   GEOM::GEOM_Object_out shape,
						   GEOM::ListOfGO_out subShapes,
						   GEOM::ListOfGO_out groups,
						   GEOM::ListOfFields_out fields)
{
  SALOMEDS::TMPFile_var aBuff;
  importXAO( true, fileName, aBuff, shape, subShapes, groups, fields);
  return IsDone();
}

//=============================================================================
/*!
 *  Import a shape from XAO format memory buffer
 *  \param fileName The name of the file to import
 *  \param shape The imported shape
 *  \param subShapes The list of imported subShapes
 *  \param groups The list of imported groups
 *  \param fields The list of imported fields
 *  \return boolean indicating if import was successful.
 */
//=============================================================================
CORBA::Boolean XAOPlugin_IOperations_i::ImportXAOMem( const SALOMEDS::TMPFile& theBuff,
                                                      GEOM::GEOM_Object_out shape,
                                                      GEOM::ListOfGO_out subShapes,
                                                      GEOM::ListOfGO_out groups,
                                                      GEOM::ListOfFields_out fields)
{
  importXAO( false, NULL, theBuff, shape, subShapes, groups, fields);
  return IsDone();
}

//=============================================================================
/*!
 *  Import a shape from XAO format
 *  \param isFile Boolean flag to switch between import from file or from buffer
 *  \param fileName The name of the file to import from if isFile=true
 *  \param theBuff The buffer to import from if isFile=false
 *  \param shape The imported shape
 *  \param subShapes The list of imported subShapes
 *  \param groups The list of imported groups
 *  \param fields The list of imported fields
 *  \return boolean indicating if import was successful.
 */
//=============================================================================
CORBA::Boolean XAOPlugin_IOperations_i::importXAO( const bool isFile,
                                                   const char* fileName,
                                                   const SALOMEDS::TMPFile& theBuff,
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

  if (isFile) {
    GetOperations()->ImportXAO( fileName, hshape, importedSubShapes, importedGroups, importedFields );
  }
  else {
    if (theBuff.length() < 1)
      return false;

    char* buf = (char*)theBuff.NP_data();
    //std::string anXMLBuff (buf); // works wrongly
    std::string anXMLBuff (buf, theBuff.length());
    GetOperations()->ImportXAOMem( anXMLBuff, hshape, importedSubShapes, importedGroups, importedFields );
  }

  if( !GetOperations()->IsDone() )
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

  return IsDone();
}

XAOPlugin_IOperations* XAOPlugin_IOperations_i::GetOperations()
{
  return (XAOPlugin_IOperations*)GetImpl();
}
