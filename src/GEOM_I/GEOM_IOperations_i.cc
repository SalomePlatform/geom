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

#include "GEOM_IOperations_i.hh"

#include "GEOM_Engine.hxx"
#include "GEOM_Gen_i.hh"
#include <SALOME_NamingService.hxx>

#include "utilities.h"
#include "OpUtil.hxx"
#include "Utils_ExceptHandlers.hxx"

#include <Standard_Stream.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDF_Tool.hxx>

#include CORBA_SERVER_HEADER(SALOME_Session)

//=============================================================================
/*!
 *  default constructor:
 */
//=============================================================================

GEOM_IOperations_i::GEOM_IOperations_i(PortableServer::POA_ptr thePOA, GEOM::GEOM_Gen_ptr theEngine, ::GEOM_IOperations* theImpl)
:SALOME::GenericObj_i( thePOA ), _impl(theImpl), _engine(theEngine)
{
}

//=============================================================================
/*!
 *  destructor
 */
//=============================================================================

GEOM_IOperations_i::~GEOM_IOperations_i()
{}


//=============================================================================
/*!
 *  IsDone
 */
//=============================================================================
CORBA::Boolean GEOM_IOperations_i::IsDone()
{
  return _impl->IsDone();
}


//=============================================================================
/*!
 *  SetErrorCode
 */
//=============================================================================
void GEOM_IOperations_i::SetErrorCode(const char* theErrorCode)
{
  _impl->SetErrorCode((char*)theErrorCode);
}

//=============================================================================
/*!
 *  GetErrorCode
 */
//=============================================================================
char* GEOM_IOperations_i::GetErrorCode()
{
  return CORBA::string_dup(_impl->GetErrorCode());
}

//=============================================================================
/*!
 *  GetStudyID
 */
//=============================================================================
CORBA::Long GEOM_IOperations_i::GetStudyID()
{
  return _impl->GetDocID();
}

//=============================================================================
/*!
 *  StartOperation
 */
//=============================================================================
void GEOM_IOperations_i::StartOperation()
{
  _impl->StartOperation();
}

//=============================================================================
/*!
 *  FinishOperation
 */
//=============================================================================
void GEOM_IOperations_i::FinishOperation()
{
  _impl->FinishOperation();
}


//=============================================================================
/*!
 *  AbortOperation
 */
//=============================================================================
void GEOM_IOperations_i::AbortOperation()
{
  _impl->AbortOperation();
}

//=============================================================================
/*!
 *  GetObject
 */
//=============================================================================
GEOM::GEOM_BaseObject_ptr
GEOM_IOperations_i::GetBaseObject(HANDLE_NAMESPACE(GEOM_BaseObject) theObject)
{
  GEOM::GEOM_BaseObject_var GO;
  if (theObject.IsNull()) return GO._retn();
  TCollection_AsciiString anEntry;
  TDF_Tool::Entry(theObject->GetEntry(), anEntry);
  GO = _engine->GetObject(theObject->GetDocID(), (char*) anEntry.ToCString());
  return GO._retn();
}

//=============================================================================
/*!
 *  GetObjectImpl
 */
//=============================================================================
HANDLE_NAMESPACE(GEOM_BaseObject)
GEOM_IOperations_i::GetBaseObjectImpl(GEOM::GEOM_BaseObject_ptr theObject)
{
  HANDLE_NAMESPACE(GEOM_BaseObject) anImpl;
  if (!CORBA::is_nil(theObject)) {
    CORBA::String_var anEntry = theObject->GetEntry();
    anImpl = GetImpl()->GetEngine()->GetObject( theObject->GetStudyID(), anEntry );
  }
  return anImpl;
}

//=============================================================================
/*!
 *  GetObject
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IOperations_i::GetObject(HANDLE_NAMESPACE(GEOM_Object) theObject)
{
  return GEOM::GEOM_Object::_narrow( GetBaseObject( theObject ));
}

//=============================================================================
/*!
 *  GetObjectImpl
 */
//=============================================================================
HANDLE_NAMESPACE(GEOM_Object) GEOM_IOperations_i::GetObjectImpl(GEOM::GEOM_Object_ptr theObject)
{
  return HANDLE_NAMESPACE(GEOM_Object)::DownCast( GetBaseObjectImpl( theObject ));
}

//=============================================================================
/*!
 *  GetListOfObjectsImpl
 */
//=============================================================================
Handle(TColStd_HSequenceOfTransient) GEOM_IOperations_i::GetListOfObjectsImpl
              (const GEOM::ListOfGO& theObjects)
{
  Handle(TColStd_HSequenceOfTransient) aResult =
    new TColStd_HSequenceOfTransient;

  int i;
  int aNbObj = theObjects.length();

  for (i = 0; i < aNbObj; i++) {
    HANDLE_NAMESPACE(GEOM_Object) anObj = GetObjectImpl(theObjects[i]);

    if (anObj.IsNull()) {
      aResult.Nullify();
      break;
    }

    aResult->Append(anObj);
  }

  return aResult;
}

//=======================================================================
//function : GetListOfObjectsImpl
//purpose  : 
//=======================================================================

bool GEOM_IOperations_i::GetListOfObjectsImpl(const GEOM::ListOfGO&             theObjects,
                                              std::list< HANDLE_NAMESPACE(GEOM_Object) >& theList)
{
  int i;
  int aNbObj = theObjects.length();

  for (i = 0; i < aNbObj; i++) {
    HANDLE_NAMESPACE(GEOM_Object) anObj = GetObjectImpl(theObjects[i]);

    if (anObj.IsNull())
      return false;

    theList.push_back(anObj);
  }

  return true;
}

//=============================================================================
/*!
 *  UpdateGUIForObject
 */
//=============================================================================
void GEOM_IOperations_i::UpdateGUIForObject(GEOM::GEOM_Object_ptr theObj)
{
  if (!CORBA::is_nil (theObj)) {
    // Cast _engine to GEOM_Gen_i type.
    PortableServer::Servant aServant = myPOA->reference_to_servant(_engine.in());
    GEOM_Gen_i *anEngine = dynamic_cast<GEOM_Gen_i *>(aServant);

    if (anEngine) {
      SALOME_NamingService *aNameService = anEngine->GetNS();
      CORBA::Object_var aSessionObj = aNameService->Resolve("/Kernel/Session");
      SALOME::Session_var aSession = SALOME::Session::_narrow(aSessionObj);
  
      if (!aSession->_is_nil())
      {
        std::string aMsg("GEOM/modified/");
        CORBA::String_var anIOR = anEngine->GetORB()->object_to_string(theObj);
  
        aMsg += anIOR.in();
        aSession->emitMessageOneWay(aMsg.c_str());
      }
    }
  }
}

//=============================================================================
/*!
 *  ConvertStringArray
 */
//=============================================================================
Handle(TColStd_HArray1OfExtendedString) GEOM_IOperations_i::ConvertStringArray
        (const GEOM::string_array &theInArray)
{
  Handle(TColStd_HArray1OfExtendedString) anOutArray;
  const int                               n = theInArray.length();
  int                                     i;

  if (n <= 0) {
    return anOutArray;
  }

  anOutArray = new TColStd_HArray1OfExtendedString( 1, n );

  for (i = 0; i < n; i++) {
    anOutArray->SetValue(i + 1, TCollection_ExtendedString(theInArray[i].in()));
  }

  return anOutArray;
}
