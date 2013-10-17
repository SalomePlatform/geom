// Copyright (C) 2007-2013  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

#include <Standard_Stream.hxx>

#include <GEOMImpl_ImportDriver.hxx>
#include <GEOMImpl_IImportExport.hxx>
#include <GEOMImpl_Types.hxx>
#include <GEOM_Function.hxx>

#include <TopoDS_Shape.hxx>

#include <TCollection_HAsciiString.hxx>

#include "utilities.h"

#include <Standard_Failure.hxx>
#include <StdFail_NotDone.hxx>

#ifdef WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#ifdef WIN32
#define LibHandle HMODULE
#define LoadLib( name ) LoadLibrary( name )
#define GetProc GetProcAddress
#define UnLoadLib( handle ) FreeLibrary( handle );
#else
#define LibHandle void*
#define LoadLib( name ) dlopen( name, RTLD_LAZY )
#define GetProc dlsym
#define UnLoadLib( handle ) dlclose( handle );
#endif

typedef TopoDS_Shape (*funcPoint)(const TCollection_AsciiString&,
                                  const TCollection_AsciiString&,
                                  TCollection_AsciiString&,
                                  const TDF_Label&);

typedef Handle(TCollection_HAsciiString) (*pGetValue)(const TCollection_AsciiString&,
                                                      const TCollection_AsciiString&,
                                                      TCollection_AsciiString&);

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
Standard_Integer GEOMImpl_ImportDriver::Execute(TFunction_Logbook& log) const
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  GEOMImpl_IImportExport aCI (aFunction);
  //Standard_Integer aType = aFunction->GetType();

  // retrieve the file and plugin library names
  TCollection_AsciiString aFileName   = aCI.GetFileName();
  TCollection_AsciiString aFormatName = aCI.GetFormatName();
  TCollection_AsciiString aLibName    = aCI.GetPluginName();
  if (aFileName.IsEmpty() || aFormatName.IsEmpty() || aLibName.IsEmpty())
    return 0;

  // load plugin library
  LibHandle anImportLib = LoadLib( aLibName.ToCString() ); //This is workaround of BUG OCC13051

  // Get Import method
  funcPoint fp = 0;
  if ( anImportLib )
    fp = (funcPoint)GetProc( anImportLib, "Import" );

  if ( !fp ) {
    TCollection_AsciiString aMsg = aFormatName.SubString(1,4);
    aMsg += " plugin was not installed";
    Standard_Failure::Raise(aMsg.ToCString());
  }

  // perform the import
  TCollection_AsciiString anError;
  TopoDS_Shape aShape = fp(aFileName, aFormatName, anError, aFunction->GetNamingEntry());

  // unload plugin library
  // commented by enk:
  // the bug was occured: using ACIS Import/Export plugin
  //UnLoadLib( anImportLib ); //This is workaround of BUG OCC13051

  if ( aShape.IsNull() ) {
    StdFail_NotDone::Raise(anError.ToCString());
    return 0;
  }

  // set the function result
  aFunction->SetValue(aShape);

  log.SetTouched(Label());

  return 1;
}

//=======================================================================
//function : ReadValue
//purpose  :
//=======================================================================
TCollection_AsciiString GEOMImpl_ImportDriver::ReadValue(const TCollection_AsciiString& theFileName,
                                                         const TCollection_AsciiString& theLibName,
                                                         const TCollection_AsciiString& theParameterName,
                                                         TCollection_AsciiString& theError)
{
  TCollection_AsciiString aValue;

  if (theFileName.IsEmpty() || theLibName.IsEmpty() || theParameterName.IsEmpty())
    return aValue;

  // load plugin library
  LibHandle anImportLib = LoadLib(theLibName.ToCString()); //This is workaround of BUG OCC13051
  if (!anImportLib) {
    theError = theLibName + " library was not installed";
    return aValue;
  }

  // Get GetValue method
  pGetValue pGV = (pGetValue)GetProc(anImportLib, "GetValue");

  if (!pGV) {
    theError = theLibName + " library doesn't support GetValue method";
    return aValue;
  }

  Handle(TCollection_HAsciiString) aHValue = pGV(theFileName, theParameterName, theError);

  if (aHValue.IsNull()) {
    if (theError.IsEmpty())
      theError = theFileName + " doesn't contain requested parameter";
    return aValue;
  }

  aValue = aHValue->String();

  // unload plugin library
  // commented by enk:
  // the bug was occured: using ACIS Import/Export plugin
  //UnLoadLib( anImportLib ); //This is workaround of BUG OCC13051

  return aValue;
}

//================================================================================
/*!
 * \brief Returns a name of creation operation and names and values of creation parameters
 */
//================================================================================

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

IMPLEMENT_STANDARD_HANDLE (GEOMImpl_ImportDriver,GEOM_BaseDriver);
IMPLEMENT_STANDARD_RTTIEXT (GEOMImpl_ImportDriver,GEOM_BaseDriver);
