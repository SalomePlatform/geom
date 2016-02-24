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
#include "GEOMImpl_ImportDriver.hxx"
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
const Standard_GUID& GEOMImpl_ImportDriver::GetID()
{
  static Standard_GUID aImportDriver("FF1BBB60-5D14-4df2-980B-3A668264EA16");
  return aImportDriver;
}

//=======================================================================
//function : GEOMImpl_ImportDriver
//purpose  :
//=======================================================================
GEOMImpl_ImportDriver::GEOMImpl_ImportDriver()
{
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer GEOMImpl_ImportDriver::Execute(LOGBOOK& log) const
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  GEOMImpl_IImportExport aCI (aFunction);

  // retrieve the file and plugin library names
  TCollection_AsciiString aFileName   = aCI.GetFileName();
  TCollection_AsciiString aFormatName = aCI.GetFormatName();
  if (aFileName.IsEmpty() || aFormatName.IsEmpty() )
    return 0;

  Handle(TColStd_HSequenceOfTransient) aSeq =
    GEOMImpl_IECallBack::GetCallBack( aFormatName )->Import( GetDocID(), aFormatName, aFileName );
  if( aSeq.IsNull() )
    return 0;

  Handle(GEOM_Object) anImported = Handle(GEOM_Object)::DownCast( aSeq->Value(1) );
  TopoDS_Shape aShape = anImported->GetValue();
  aFunction->SetValue( aShape );
#if OCC_VERSION_MAJOR < 7
  log.SetTouched(Label());
#else
  log->SetTouched(Label());
#endif

  return 1;
}

bool GEOMImpl_ImportDriver::
GetCreationInformation(std::string&             theOperationName,
                       std::vector<GEOM_Param>& theParams)
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) function = GEOM_Function::GetFunction(Label());

  GEOMImpl_IImportExport aCI( function );
  Standard_Integer aType = function->GetType();

  theOperationName = "IMPORT";

  switch ( aType ) {
  case IMPORT_SHAPE:
    AddParam( theParams, "File name", aCI.GetFileName() );
    AddParam( theParams, "Format", aCI.GetFormatName() );
    AddParam( theParams, "Plugin name", aCI.GetPluginName() );
    break;
  default:
    return false;
  }
  
  return true;
}

OCCT_IMPLEMENT_STANDARD_RTTIEXT (GEOMImpl_ImportDriver,GEOM_BaseDriver);
