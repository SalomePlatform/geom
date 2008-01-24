// Copyright (C) 2005  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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

#include "utilities.h"
#include "OpUtil.hxx"
#include "Utils_ExceptHandlers.hxx"

#include <Standard_Stream.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDF_Tool.hxx>

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
GEOM::GEOM_Object_ptr GEOM_IOperations_i::GetObject(Handle(GEOM_Object) theObject)
{
  if(theObject.IsNull()) return NULL;
  TCollection_AsciiString anEntry;
  TDF_Tool::Entry(theObject->GetEntry(), anEntry);
  GEOM::GEOM_Object_var GO = _engine->GetObject(theObject->GetDocID(), anEntry.ToCString());
  return GO._retn();
}
