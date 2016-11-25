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

#include "GEOM_BaseObject_i.hh"

#include "GEOMImpl_Types.hxx"
#include "GEOM_BaseDriver.hxx"

#include <utilities.h>
//#include <OpUtil.hxx>
//#include <Utils_ExceptHandlers.hxx>

#include <TCollection_AsciiString.hxx>
#include <TDF_Label.hxx>
#include <TDF_Tool.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>

#ifdef _DEBUG_
#include <typeinfo>
#endif

#ifdef WIN32
#pragma warning( disable:4786 )
#endif

//=============================================================================
/*!
 *   constructor:
 */
//=============================================================================

GEOM_BaseObject_i::GEOM_BaseObject_i (PortableServer::POA_ptr thePOA,
                                      GEOM::GEOM_Gen_ptr      theEngine,
                                      HANDLE_NAMESPACE(GEOM_BaseObject) theImpl)
  : SALOME::GenericObj_i( thePOA ), _engine(theEngine), _impl(theImpl)
{
}

//=============================================================================
/*!
 *  destructor
 */
//=============================================================================

GEOM_BaseObject_i::~GEOM_BaseObject_i()
{
  //MESSAGE("GEOM_BaseObject_i::~GEOM_BaseObject_i");
  GEOM_Engine::GetEngine()->RemoveObject(_impl);
}

//=============================================================================
/*!
 *  GetEntry
 */
//=============================================================================

char* GEOM_BaseObject_i::GetEntry()
{
  const TDF_Label& aLabel = _impl->GetEntry();
  TCollection_AsciiString anEntry;
  TDF_Tool::Entry(aLabel, anEntry);
  const char* anEntstr = anEntry.ToCString();
  return CORBA::string_dup(anEntstr);
}

//=============================================================================
/*!
 *  GetStudyID
 */
//=============================================================================

CORBA::Long GEOM_BaseObject_i::GetStudyID()
{
   return _impl->GetDocID();
}

//=============================================================================
/*!
 *  GetType
 */
//=============================================================================

CORBA::Long GEOM_BaseObject_i::GetType()
{
  return _impl->GetType();
}

//=============================================================================
/*!
 *  GetTick
 */
//=============================================================================

CORBA::Long GEOM_BaseObject_i::GetTick()
{
  return _impl->GetTic();
}

//=============================================================================
/*!
 *  SetName
 */
//=============================================================================
void GEOM_BaseObject_i::SetName(const char* theName)
{
  _impl->SetName(theName);
}

//=============================================================================
/*!
 *  GetName
 */
//=============================================================================

char* GEOM_BaseObject_i::GetName()
{
  TCollection_AsciiString aName = _impl->GetName();
  return CORBA::string_dup( aName.ToCString() );
}

//=============================================================================
/*!
 *  SetStudyEntry
 */
//=============================================================================

void GEOM_BaseObject_i::SetStudyEntry(const char* theEntry)
{
  _impl->SetAuxData(theEntry);
}

//=============================================================================
/*!
 *  GetStudyEntry
 */
//=============================================================================

char* GEOM_BaseObject_i::GetStudyEntry()
{
  TCollection_AsciiString anEntry = _impl->GetAuxData();
  return CORBA::string_dup(anEntry.ToCString());
}

//=============================================================================
/*!
 *  GetDependency
 */
//=============================================================================
GEOM::ListOfGBO* GEOM_BaseObject_i::GetDependency()
{
  GEOM::ListOfGBO_var aList = new GEOM::ListOfGBO();
  aList->length(0);

  Handle(TColStd_HSequenceOfTransient) aSeq = _impl->GetAllDependency();
  if (aSeq.IsNull()) return aList._retn();
  int aLength = aSeq->Length();
  if (aLength == 0) return aList._retn();

  aList->length(aLength);

  TCollection_AsciiString anEntry;

  for (int i = 1; i<=aLength; i++) {
    HANDLE_NAMESPACE(GEOM_BaseObject) anObj = HANDLE_NAMESPACE(GEOM_BaseObject)::DownCast(aSeq->Value(i));
    if (anObj.IsNull()) continue;
    TDF_Tool::Entry(anObj->GetEntry(), anEntry);
    GEOM::GEOM_BaseObject_var obj = _engine->GetObject(anObj->GetDocID(), (char*) anEntry.ToCString());
    aList[i-1] = obj;
  }

  return aList._retn();
}

//=============================================================================
/*!
 * GetLastDependency
 */
//=============================================================================
GEOM::ListOfGBO* GEOM_BaseObject_i::GetLastDependency()
{
  GEOM::ListOfGBO_var aList = new GEOM::ListOfGBO();
  aList->length(0);

  Handle(TColStd_HSequenceOfTransient) aSeq = _impl->GetLastDependency();
  if (aSeq.IsNull()) return aList._retn();
  int aLength = aSeq->Length();
  if (aLength == 0) return aList._retn();

  aList->length(aLength);

  TCollection_AsciiString anEntry;

  for (int i = 1; i<=aLength; i++) {
     HANDLE_NAMESPACE(GEOM_BaseObject) anObj = HANDLE_NAMESPACE(GEOM_BaseObject)::DownCast(aSeq->Value(i));
     if (anObj.IsNull()) continue;
     GEOM::GEOM_BaseObject_var obj = _engine->GetObject(anObj->GetDocID(),
                                                        anObj->GetEntryString().ToCString());
     aList[i-1] = GEOM::GEOM_BaseObject::_duplicate( obj );
  }

  return aList._retn();
}

//================================================================================
/*!
 * \brief 
 */
//================================================================================

bool GEOM_BaseObject_i::IsSame(GEOM::GEOM_BaseObject_ptr other)
{
  if ( CORBA::is_nil( other ))
    return false;

  PortableServer::Servant aServant = myPOA->reference_to_servant( other );
  GEOM_BaseObject_i * other_i = dynamic_cast<GEOM_BaseObject_i*>(aServant);
  if ( !other_i )
    return false;

  return _impl->GetEntry() == other_i->_impl->GetEntry();
}

//================================================================================
/*!
 * \brief 
 */
//================================================================================

void GEOM_BaseObject_i::SetParameters(const char* theParameters)
{
  _impl->SetParameters((char*)theParameters);
}

//================================================================================
/*!
 * \brief 
 */
//================================================================================

char* GEOM_BaseObject_i::GetParameters()
{
  return CORBA::string_dup(_impl->GetParameters().ToCString());
}

//================================================================================
/*!
 * \brief 
 */
//================================================================================

GEOM::CreationInformationSeq* GEOM_BaseObject_i::GetCreationInformation()
{
  GEOM::CreationInformationSeq_var info = new GEOM::CreationInformationSeq();

  int nbFun = _impl->GetNbFunctions();
  info->length( nbFun );
  int nbI = 0;
  for ( int i = 1; i <= nbFun; ++i )
  {
    HANDLE_NAMESPACE(GEOM_BaseDriver) driver =
      HANDLE_NAMESPACE(GEOM_BaseDriver)::DownCast( _impl->GetCreationDriver( i ));
    if ( !driver.IsNull() )
    {
      std::vector<GEOM_Param> params;
      std::string             operationName;
      try
      {
        OCC_CATCH_SIGNALS;
        if ( driver->GetCreationInformation( operationName, params ))
        {
          info[nbI].operationName = operationName.c_str();
          info[nbI].params.length( params.size() );
          for ( size_t i = 0; i < params.size(); ++i )
          {
            info[nbI].params[i].name  = params[i].name.c_str();
            info[nbI].params[i].value = params[i].value.c_str();
          }
          nbI++;
        }
#ifdef _DEBUG_
        if ( operationName.empty() )
        {
          cout << endl << endl << endl << "Warning: " << endl << "Dear developer!!!" << endl
               << "  Consider implementing "
               <<    typeid(*(driver.operator->())).name() << "::GetCreationInformation() " << endl
               << "  for the case of operation which has created '" << GetName() << "' object" << endl
               << "PLEEEEEEEASE" << endl
               << "\tPLEEEEEEEASE" << endl
               << "\t\tPLEEEEEEEASE" << endl
               << "\t\t\tPLEEEEEEEASE" << endl
               << "\t\t\t\tPLEEEEEEEASE" << endl;
        }
#endif
      }
      catch(...)
      {
#ifdef _DEBUG_
        cout << "Ecxeption in GEOM_BaseObject_i::GetCreationInformation()" << endl;
#endif
      }
    }
  }
  info->length( nbI );

  return info._retn();
}
