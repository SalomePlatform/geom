// Copyright (C) 2007-2014  CEA/DEN, EDF R&D, OPEN CASCADE
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

#ifdef WIN32
#pragma warning( disable:4786 )
#endif

#include <Standard_Stream.hxx>

#include "GEOM_IInsertOperations_i.hh"

#include "utilities.h"
#include "OpUtil.hxx"
#include "Utils_ExceptHandlers.hxx"

#include "GEOM_Engine.hxx"
#include "GEOM_Object.hxx"

#include <Basics_OCCTVersion.hxx>

#include <TColStd_HSequenceOfAsciiString.hxx>

#if OCC_VERSION_LARGE > 0x06040000 // Porting to OCCT6.5.1
#include <TColStd_HArray1OfByte.hxx>
#else
#include <TDataStd_HArray1OfByte.hxx>
#endif

//=============================================================================
/*!
 *  constructor
 */
//=============================================================================
GEOM_IInsertOperations_i::GEOM_IInsertOperations_i (PortableServer::POA_ptr thePOA,
                                                    GEOM::GEOM_Gen_ptr theEngine,
                                                    ::GEOMImpl_IInsertOperations* theImpl)
     :GEOM_IOperations_i(thePOA, theEngine, theImpl)
{
  MESSAGE("GEOM_IInsertOperations_i::GEOM_IInsertOperations_i");
}

//=============================================================================
/*!
 *  destructor
 */
//=============================================================================
GEOM_IInsertOperations_i::~GEOM_IInsertOperations_i()
{
  MESSAGE("GEOM_IInsertOperations_i::~GEOM_IInsertOperations_i");
}


//=============================================================================
/*!
 *  MakeCopy
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IInsertOperations_i::MakeCopy(GEOM::GEOM_Object_ptr theOriginal)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference shape
  Handle(GEOM_Object) anOriginal = GetObjectImpl(theOriginal);
  if (anOriginal.IsNull()) return aGEOMObject._retn();

  //Create the copy
  Handle(GEOM_Object) anObject = GetOperations()->MakeCopy(anOriginal);

  if (!GetOperations()->IsDone() || anObject.IsNull()) return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  Export
 */
//=============================================================================
void GEOM_IInsertOperations_i::Export
                   (GEOM::GEOM_Object_ptr theOriginal,
                    const char*           theFileName,
                    const char*           theFormatName)
{
  GEOM::GEOM_Object_var aGEOMObject = GEOM::GEOM_Object::_duplicate(theOriginal);

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference shape
  Handle(GEOM_Object) anOriginal = GetObjectImpl(theOriginal);
  if (anOriginal.IsNull()) return;

  //Export the shape to the file
  char* aFileName   = strdup(theFileName);
  char* aFormatName = strdup(theFormatName);
  GetOperations()->Export(anOriginal, aFileName, aFormatName);
  free(aFileName);
  free(aFormatName);
}

//=============================================================================
/*!
 *  ImportFile
 */
//=============================================================================
GEOM::ListOfGO* GEOM_IInsertOperations_i::ImportFile
                   (const char* theFileName,
                    const char* theFormatName)
{
  GEOM::ListOfGO_var aSeq = new GEOM::ListOfGO;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Import the shape from the file
  char* aFileName   = strdup(theFileName);
  char* aFormatName = strdup(theFormatName);
  Handle(TColStd_HSequenceOfTransient) aHSeq = GetOperations()->Import(aFileName, aFormatName);

  if( strcmp(aFormatName,"IGES_UNIT")==0 && !aHSeq.IsNull() ) {
    free(aFileName);
    free(aFormatName);
    return aSeq._retn();
  }

  free(aFileName);
  free(aFormatName);

  if (!GetOperations()->IsDone() || aHSeq.IsNull()) {
    return aSeq._retn();
  }

  // Copy created objects.
  Standard_Integer aLength = aHSeq->Length();

  aSeq->length(aLength);

  for (Standard_Integer i = 1; i <= aLength; i++) {
    aSeq[i-1] = GetObject(Handle(GEOM_Object)::DownCast(aHSeq->Value(i)));
  }

  return aSeq._retn();
}

//=============================================================================
/*!
 *  ReadValue
 */
//=============================================================================
char* GEOM_IInsertOperations_i::ReadValue(const char* theFileName,
                                          const char* theFormatName,
                                          const char* theParameterName)
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  char* aFileName      = strdup(theFileName);
  char* aFormatName    = strdup(theFormatName);
  char* aParameterName = strdup(theParameterName);

  TCollection_AsciiString aUnits = GetOperations()->ReadValue
    (aFileName, aFormatName, aParameterName);

  free(aFileName);
  free(aFormatName);
  free(aParameterName);

  return CORBA::string_dup(aUnits.ToCString());
}

//=============================================================================
/*!
 *  ImportTranslators
 */
//=============================================================================
void GEOM_IInsertOperations_i::ImportTranslators
  (GEOM::string_array_out theFormats, GEOM::string_array_out thePatterns)
{
  // allocate the CORBA arrays
  GEOM::string_array_var aFormatsArray  = new GEOM::string_array();
  GEOM::string_array_var aPatternsArray = new GEOM::string_array();

  // Get sequences of available formats
  Handle(TColStd_HSequenceOfAsciiString) aFormats  = new TColStd_HSequenceOfAsciiString;
  Handle(TColStd_HSequenceOfAsciiString) aPatterns = new TColStd_HSequenceOfAsciiString;
  if (GetOperations()->ImportTranslators(aFormats, aPatterns)) {
    const int formSize = aFormats->Length();
    if (formSize == aPatterns->Length()) {
      aFormatsArray->length(formSize);
      aPatternsArray->length(formSize);

      // fill the local CORBA arrays with values from sequences
      CORBA::Long i = 1;
      for (; i <= formSize; i++) {
        aFormatsArray[i-1]  = CORBA::string_dup(aFormats->Value(i).ToCString());
        aPatternsArray[i-1] = CORBA::string_dup(aPatterns->Value(i).ToCString());
      }
    }
  }

  // initialize out-parameters with local arrays
  theFormats  = aFormatsArray._retn();
  thePatterns = aPatternsArray._retn();
}

//=============================================================================
/*!
 *  ExportTranslators
 */
//=============================================================================
void GEOM_IInsertOperations_i::ExportTranslators
  (GEOM::string_array_out theFormats, GEOM::string_array_out thePatterns)
{
  // allocate the CORBA arrays
  GEOM::string_array_var aFormatsArray  = new GEOM::string_array();
  GEOM::string_array_var aPatternsArray = new GEOM::string_array();

  // Get sequences of available formats
  Handle(TColStd_HSequenceOfAsciiString) aFormats  = new TColStd_HSequenceOfAsciiString;
  Handle(TColStd_HSequenceOfAsciiString) aPatterns = new TColStd_HSequenceOfAsciiString;
  if (GetOperations()->ExportTranslators(aFormats, aPatterns)) {
    const int formSize = aFormats->Length();
    if (formSize == aPatterns->Length()) {
      aFormatsArray->length(formSize);
      aPatternsArray->length(formSize);

      // fill the local CORBA arrays with values from sequences
      CORBA::Long i = 1;
      for (; i <= formSize; i++) {
        aFormatsArray[i-1]  = CORBA::string_dup(aFormats->Value(i).ToCString());
        aPatternsArray[i-1] = CORBA::string_dup(aPatterns->Value(i).ToCString());
      }
    }
  }

  // initialize out-parameters with local arrays
  theFormats  = aFormatsArray._retn();
  thePatterns = aPatternsArray._retn();
}

//=============================================================================
/*!
 *  RestoreShape
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IInsertOperations_i::RestoreShape (const SALOMEDS::TMPFile& theStream)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  if (theStream.length() < 1)
    return aGEOMObject._retn();

  char* buf = (char*)theStream.NP_data();
  std::istringstream aStream (buf);

  Handle(GEOM_Object) anObject = GetOperations()->RestoreShape(aStream);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  LoadTexture
 */
//=============================================================================
CORBA::Long GEOM_IInsertOperations_i::LoadTexture(const char* theTextureFile)
{
  GetOperations()->SetNotDone();
  return GetOperations()->LoadTexture( theTextureFile );
}

//=============================================================================
/*!
 *  AddTexture
 */
//=============================================================================
CORBA::Long GEOM_IInsertOperations_i::AddTexture(CORBA::Long theWidth, CORBA::Long theHeight,
                                                 const SALOMEDS::TMPFile& theTexture)
{
  GetOperations()->SetNotDone();

#if OCC_VERSION_LARGE > 0x06040000 // Porting to OCCT6.5.1
  Handle(TColStd_HArray1OfByte) aTexture;
#else
  Handle(TDataStd_HArray1OfByte) aTexture;
#endif

  if ( theTexture.length() > 0 ) {
#if OCC_VERSION_LARGE > 0x06040000 // Porting to OCCT6.5.1
    aTexture = new TColStd_HArray1OfByte (1, theTexture.length());
#else
    aTexture = new TDataStd_HArray1OfByte (1, theTexture.length());
#endif

    for ( int i = 0; i < theTexture.length(); i++ )
      aTexture->SetValue( i+1, (Standard_Byte)theTexture[i] );
  }
  return GetOperations()->AddTexture( theWidth, theHeight, aTexture );
}

//=============================================================================
/*!
 *  GetTexture
 */
//=============================================================================
SALOMEDS::TMPFile* GEOM_IInsertOperations_i::GetTexture(CORBA::Long theID,
                                                        CORBA::Long& theWidth,
                                                        CORBA::Long& theHeight)
{
  int aWidth, aHeight;
#if OCC_VERSION_LARGE > 0x06040000 // Porting to OCCT6.5.1
  Handle(TColStd_HArray1OfByte) aTextureImpl =
#else
  Handle(TDataStd_HArray1OfByte) aTextureImpl =
#endif
    GetOperations()->GetTexture(theID, aWidth, aHeight);
  theWidth  = aWidth;
  theHeight = aHeight;
  SALOMEDS::TMPFile_var aTexture;
  if ( !aTextureImpl.IsNull() ) {
    aTexture = new SALOMEDS::TMPFile;
    aTexture->length( aTextureImpl->Length() );
    for ( int i = aTextureImpl->Lower(); i <= aTextureImpl->Upper(); i++ )
      aTexture[i-aTextureImpl->Lower()] = aTextureImpl->Value( i );
  }
  return aTexture._retn();
}

//=============================================================================
/*!
 *  GetAllTextures
 */
//=============================================================================
GEOM::ListOfLong* GEOM_IInsertOperations_i::GetAllTextures()
{
  std::list<int> localIDs = GetOperations()->GetAllTextures();
  GEOM::ListOfLong_var anIDs = new GEOM::ListOfLong(localIDs.size());
  anIDs->length(localIDs.size());
  std::list<int>::const_iterator anIt;
  int i = 0;
  for( anIt = localIDs.begin(); anIt != localIDs.end(); ++anIt, i++)
    anIDs[i] = *anIt;
  return anIDs._retn();
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
CORBA::Boolean GEOM_IInsertOperations_i::ExportXAO(GEOM::GEOM_Object_ptr shape,
                                                   const GEOM::ListOfGO& groups,
                                                   const GEOM::ListOfFields&  fields,
                                                   const char* author,
                                                   const char* fileName)
{
  bool isGood = false;
  // Set a not done flag
  GetOperations()->SetNotDone();
  
  // Get the reference shape
  Handle(GEOM_Object) reference = GetObjectImpl(shape);
  
  // Get the reference groups
  int ind = 0;
  std::list<Handle(GEOM_Object)> groupsObj;
  for (; ind < groups.length(); ind++)
  {
    Handle(GEOM_Object) gobj = GetObjectImpl(groups[ind]);
    if (gobj.IsNull()) return false;
    groupsObj.push_back(gobj);
  }

  // Get the reference fields
  ind = 0;
  std::list<Handle(GEOM_Field)> fieldsObj;
  for (; ind < fields.length(); ind++)
  {
    Handle(GEOM_Field) fobj = Handle(GEOM_Field)::DownCast(GetBaseObjectImpl(fields[ind]));
    if (fobj.IsNull()) return false;
    fieldsObj.push_back(fobj);
  }
  
  if (!reference.IsNull())
  {
    // Export XAO
    isGood = GetOperations()->ExportXAO(reference, groupsObj, fieldsObj, author, fileName);
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
CORBA::Boolean GEOM_IInsertOperations_i::ImportXAO(const char* fileName,
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
  bool res = GetOperations()->ImportXAO(fileName, hshape, importedSubShapes, importedGroups, importedFields);
  
  if (!GetOperations()->IsDone() || !res)
    return false;
  
  // parse fields
  int n = importedSubShapes->Length();
  subShapes->length(n);
  for (int i = 1; i <= n; i++)
  {
    (*subShapes)[i - 1] = GetObject(Handle(GEOM_Object)::DownCast(importedSubShapes->Value(i)));
  }
  
  // parse groups
  n = importedGroups->Length();
  groups->length(n);
  for (int i = 1; i <= n; i++)
  {
    (*groups)[i - 1] = GetObject(Handle(GEOM_Object)::DownCast(importedGroups->Value(i)));
  }
  
  // parse fields
  n = importedFields->Length();
  fields->length(n);
  for (int i = 1; i <= n; i++)
  {
        (*fields)[i - 1] = GEOM::GEOM_Field::_narrow(
                GetBaseObject(Handle(GEOM_Field)::DownCast(importedFields->Value(i))));
  }
  
  shape = GetObject(hshape);
  
  return res;
}
