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
#include "BREPPlugin_ImportDriver.hxx"
#include "BREPPlugin_IImport.hxx"

// KERNEL includes
#include <utilities.h>
#include <Basics_Utils.hxx>

// GEOM includes
#include "GEOM_Function.hxx"
#include "GEOMImpl_Types.hxx"

// OOCT includes
#include <BRepTools.hxx>
#include <BRep_Builder.hxx>

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& BREPPlugin_ImportDriver::GetID()
{
  static Standard_GUID aGUID("7e1b28ec-8513-4217-b09c-7652fbe8149e");
  return aGUID;
}

//=======================================================================
//function : BREPPlugin_ImportDriver
//purpose  :
//=======================================================================
BREPPlugin_ImportDriver::BREPPlugin_ImportDriver()
{
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer BREPPlugin_ImportDriver::Execute(LOGBOOK& log) const
{
  if( Label().IsNull() ) return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction( Label() );

  BREPPlugin_IImport aData( aFunction );

  TCollection_AsciiString aFileName = aData.GetFileName().ToCString();

  MESSAGE("Import BREP from file " << aFileName);

  // Set "C" numeric locale to save numbers correctly
  Kernel_Utils::Localizer loc;

  TopoDS_Shape aShape;
  BRep_Builder B;
  BRepTools::Read(aShape, aFileName.ToCString(), B);
  if( aShape.IsNull() ) return 0;

  aFunction->SetValue( aShape );

#if OCC_VERSION_MAJOR < 7
  log.SetTouched(Label());
#else
  log->SetTouched(Label());
#endif

  return 1;
}

//================================================================================
/*!
 * \brief Returns a name of creation operation and names and values of creation parameters
 */
//================================================================================

bool BREPPlugin_ImportDriver::
GetCreationInformation( std::string&             theOperationName,
                        std::vector<GEOM_Param>& theParams )
{
  if( Label().IsNull() ) return 0;
  Handle(GEOM_Function) function = GEOM_Function::GetFunction( Label() );

  BREPPlugin_IImport aCI( function );
  Standard_Integer aType = function->GetType();

  theOperationName = "ImportBREP";

  switch ( aType ) {
  case IMPORT_SHAPE:
    AddParam( theParams, "File name", aCI.GetFileName() );
    break;
  default:
    return false;
  }
  return true;
}

OCCT_IMPLEMENT_STANDARD_RTTIEXT( BREPPlugin_ImportDriver, GEOM_BaseDriver );
