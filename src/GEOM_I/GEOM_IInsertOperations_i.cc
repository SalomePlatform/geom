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

#ifdef WIN32
#pragma warning( disable:4786 )
#endif

#include <Standard_Stream.hxx>

#include "GEOM_IInsertOperations_i.hh"

#include "utilities.h"
#include "OpUtil.hxx"
#include "Utils_ExceptHandlers.hxx"

#include "GEOM_Engine.hxx"
#include "GEOM_BaseObject.hxx"
#include "GEOMImpl_Types.hxx"

#include <Basics_OCCTVersion.hxx>

#include <TColStd_HSequenceOfAsciiString.hxx>

#include <TColStd_HArray1OfByte.hxx>

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
  HANDLE_NAMESPACE(GEOM_Object) anOriginal = GetObjectImpl(theOriginal);
  if (anOriginal.IsNull()) return aGEOMObject._retn();

  //Create the copy
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetOperations()->MakeCopy(anOriginal);

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
  HANDLE_NAMESPACE(GEOM_Object) anOriginal = GetObjectImpl(theOriginal);
  if (anOriginal.IsNull()) return;

  //Export the shape to the file
  GetOperations()->Export(anOriginal, theFileName, theFormatName);
}

//=============================================================================
/*!
 *  ImportFile
 */
//=============================================================================
GEOM::ListOfGBO* GEOM_IInsertOperations_i::ImportFile
                   (const char* theFileName,
                    const char* theFormatName)
{
  GEOM::ListOfGBO_var aSeq = new GEOM::ListOfGBO;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Import the shape from the file
  Handle(TColStd_HSequenceOfTransient) aHSeq = GetOperations()->Import(theFileName, theFormatName);

  if (!GetOperations()->IsDone() || aHSeq.IsNull()) {
    return aSeq._retn();
  }

  // Copy created objects.
  Standard_Integer aLength = aHSeq->Length();

  aSeq->length(aLength);
  for (Standard_Integer i = 1; i <= aLength; i++)
    aSeq[i-1] = GetBaseObject(HANDLE_NAMESPACE(GEOM_BaseObject)::DownCast(aHSeq->Value(i)));

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

  TCollection_AsciiString aUnits = GetOperations()->ReadValue
    (theFileName, theFormatName, theParameterName);

  return CORBA::string_dup(aUnits.ToCString());
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

  HANDLE_NAMESPACE(GEOM_Object) anObject = GetOperations()->RestoreShape(aStream);
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

  Handle(TColStd_HArray1OfByte) aTexture;

  if ( theTexture.length() > 0 ) {
    aTexture = new TColStd_HArray1OfByte (1, theTexture.length());

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
  Handle(TColStd_HArray1OfByte) aTextureImpl =
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
 *  TransferData
 */
//=============================================================================
CORBA::Boolean GEOM_IInsertOperations_i::TransferData
        (GEOM::GEOM_Object_ptr                                 theObjectFrom,
         GEOM::GEOM_Object_ptr                                 theObjectTo,
         GEOM::find_shape_method                               theFindMethod,
         GEOM::GEOM_IInsertOperations::ListOfTransferDatum_out theResult)
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  if (CORBA::is_nil(theObjectFrom) || CORBA::is_nil(theObjectTo))
  {
    return false;
  }

  //Get the reference shape
  HANDLE_NAMESPACE(GEOM_Object) aShapeFrom = GetObjectImpl(theObjectFrom);
  HANDLE_NAMESPACE(GEOM_Object) aShapeTo   = GetObjectImpl(theObjectTo);

  if (aShapeFrom.IsNull() || aShapeTo.IsNull())
  {
    return false;
  }

  bool isOk = false;
  std::list<GEOMImpl_IInsertOperations::TransferDatum> aData;
  int aFindMethod = -1;

  switch (theFindMethod) {
  case GEOM::FSM_GetInPlace:
    aFindMethod = TD_GET_IN_PLACE;
    break;
  case GEOM::FSM_GetInPlaceByHistory:
    aFindMethod = TD_GET_IN_PLACE_BY_HISTORY;
    break;
  case GEOM::FSM_GetInPlace_Old:
    aFindMethod = TD_GET_IN_PLACE_OLD;
    break;
  default:
    break;
  }

  // Transfer data.
  if (aFindMethod > 0) {
    isOk = GetOperations()->TransferData
      (aShapeFrom, aShapeTo, aFindMethod, aData);
  }

  if (isOk) {
    // Copy results.
    const int aNbDatum = aData.size();
    GEOM::GEOM_IInsertOperations::ListOfTransferDatum_var aResult =
      new GEOM::GEOM_IInsertOperations::ListOfTransferDatum;

    aResult->length(aNbDatum);

    // fill the local CORBA array with values from lists
    std::list<GEOMImpl_IInsertOperations::TransferDatum>::const_iterator
      anIt = aData.begin();
    int i = 0;

    for (; anIt != aData.end(); i++, anIt++) {
      GEOM::GEOM_IInsertOperations::TransferDatum_var aDatum =
        new GEOM::GEOM_IInsertOperations::TransferDatum;

      aDatum->myName      = CORBA::string_dup(anIt->myName.ToCString());
      aDatum->myNumber    = anIt->myNumber;
      aDatum->myMaxNumber = anIt->myMaxNumber;
      aResult[i]          = aDatum;
    }

    theResult = aResult._retn();
  }

  return isOk;
}
