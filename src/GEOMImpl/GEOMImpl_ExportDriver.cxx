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

// internal includes
#include "GEOMImpl_ExportDriver.hxx"
#include "GEOMImpl_IImportExport.hxx"
#include "GEOMImpl_IECallBack.hxx"
#include "GEOMImpl_Types.hxx"

// GEOM includes
#include <GEOM_Function.hxx>

// OCC includes
#include <Standard_Stream.hxx>
#include <TopoDS_Shape.hxx>


//=======================================================================
//function : GetID
//purpose  :
//======================================================================= 
const Standard_GUID& GEOMImpl_ExportDriver::GetID()
{
  static Standard_GUID aExportDriver("FF1BBB58-5D14-4df2-980B-3A668264EA16");
  return aExportDriver; 
}


//=======================================================================
//function : GEOMImpl_ExportDriver
//purpose  : 
//=======================================================================
GEOMImpl_ExportDriver::GEOMImpl_ExportDriver() 
{
}

//=======================================================================
//function : Execute
//purpose  :
//======================================================================= 
Standard_Integer GEOMImpl_ExportDriver::Execute(LOGBOOK& log) const
{
  if (Label().IsNull()) return 0;    
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  GEOMImpl_IImportExport aCI (aFunction);

  // retrieve the being exported shape
  TopoDS_Shape aShape;
  Handle(GEOM_Function) aRefFunction = aCI.GetOriginal();
  if (aRefFunction.IsNull()) return 0;
  aShape = aRefFunction->GetValue();
  if (aShape.IsNull()) return 0;
  // !!! set the result of function to be used by next operations
  aFunction->SetValue(aShape);

  TDF_Label aLabel = aRefFunction->GetOwnerEntry();
  if (aLabel.IsRoot()) return 0;
  Handle(GEOM_Object) obj = GEOM_Object::GetObject( aLabel );
  if ( obj.IsNull() ) return 0;

  // retrieve the file and format names
  TCollection_AsciiString aFileName   = aCI.GetFileName();
  TCollection_AsciiString aFormatName = aCI.GetFormatName();
  TCollection_AsciiString aLibName    = aCI.GetPluginName();
  if (aFileName.IsEmpty() || aFormatName.IsEmpty() || aLibName.IsEmpty())
    return 0;

  if( !GEOMImpl_IECallBack::GetCallBack( aFormatName )->Export( GetDocID(), obj, aFileName, aFormatName ) );
    return 0;

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

bool GEOMImpl_ExportDriver::
GetCreationInformation(std::string&             theOperationName,
                       std::vector<GEOM_Param>& theParams)
{
  return false;
}

OCCT_IMPLEMENT_STANDARD_RTTIEXT (GEOMImpl_ExportDriver,GEOM_BaseDriver);
