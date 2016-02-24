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
#include "BREPPlugin_ExportDriver.hxx"
#include "BREPPlugin_IExport.hxx"

// KERNEL includes
#include <utilities.h>
#include <Basics_Utils.hxx>

// GEOM includes
#include "GEOM_Function.hxx"

// OOCT includes
#include <BRepTools.hxx>
#include <TopoDS_Shape.hxx>

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& BREPPlugin_ExportDriver::GetID()
{
  static Standard_GUID aGUID("4c1fd92e-bcf3-4695-89b7-a8353038174f");
  return aGUID;
}

//=======================================================================
//function : BREPPlugin_ExportDriver
//purpose  :
//=======================================================================
BREPPlugin_ExportDriver::BREPPlugin_ExportDriver()
{
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer BREPPlugin_ExportDriver::Execute(LOGBOOK& log) const
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction( Label() );

  BREPPlugin_IExport aData (aFunction);

  // retrieve the being exported shape
  TopoDS_Shape aShape;
  Handle(GEOM_Function) aRefFunction = aData.GetOriginal();
  if( aRefFunction.IsNull() ) return 0;
  aShape = aRefFunction->GetValue();
  if( aShape.IsNull() ) return 0;
  // set the result of function to be used by next operations
  aFunction->SetValue( aShape );

  TCollection_AsciiString aFileName = aData.GetFileName();

  MESSAGE("Export BREP into file " << aFileName.ToCString());

  // Set "C" numeric locale to save numbers correctly
  Kernel_Utils::Localizer loc;

  if ( !BRepTools::Write( aShape, aFileName.ToCString() ) )
    return 0;

  return 1;
}

//================================================================================
/*!
 * \brief Returns a name of creation operation and names and values of creation parameters
 */
//================================================================================
bool BREPPlugin_ExportDriver::
GetCreationInformation( std::string&             theOperationName,
                        std::vector<GEOM_Param>& theParams )
{
  return false;
}

OCCT_IMPLEMENT_STANDARD_RTTIEXT( BREPPlugin_ExportDriver,GEOM_BaseDriver );
