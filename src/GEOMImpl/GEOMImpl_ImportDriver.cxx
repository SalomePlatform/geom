
using namespace std;
#include "GEOMImpl_ImportDriver.hxx"
#include "GEOMImpl_IImportExport.hxx"
#include "GEOMImpl_Types.hxx"
#include "GEOM_Function.hxx"

#include "utilities.h"

#include <TopoDS_Shape.hxx>

#include <Standard_ConstructionError.hxx>
#include <StdFail_NotDone.hxx>

#ifdef WNT
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#ifdef WNT
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

typedef TopoDS_Shape (*funcPoint)(const TCollection_AsciiString&, TCollection_AsciiString&);

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
  TCollection_AsciiString aFileName = aCI.GetFileName();
  TCollection_AsciiString aLibName  = aCI.GetPluginName();
  if (aFileName.IsEmpty() || aLibName.IsEmpty())
    return 0;

  // load plugin library
  LibHandle anImportLib = LoadLib( aLibName.ToCString() );
  funcPoint fp = 0;
  if ( anImportLib )
    fp = (funcPoint)GetProc( anImportLib, "Import" );

  if ( !fp )
    return 0;

  // perform the import
  TCollection_AsciiString anError;
  TopoDS_Shape aShape = fp( aFileName, anError );

  // unload plugin library
  UnLoadLib( anImportLib );

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
//function :  GEOMImpl_ImportDriver_Type_
//purpose  :
//=======================================================================
Standard_EXPORT Handle_Standard_Type& GEOMImpl_ImportDriver_Type_()
{

  static Handle_Standard_Type aType1 = STANDARD_TYPE(TFunction_Driver);
  if ( aType1.IsNull()) aType1 = STANDARD_TYPE(TFunction_Driver);
  static Handle_Standard_Type aType2 = STANDARD_TYPE(MMgt_TShared);
  if ( aType2.IsNull()) aType2 = STANDARD_TYPE(MMgt_TShared);
  static Handle_Standard_Type aType3 = STANDARD_TYPE(Standard_Transient);
  if ( aType3.IsNull()) aType3 = STANDARD_TYPE(Standard_Transient);


  static Handle_Standard_Transient _Ancestors[]= {aType1,aType2,aType3,NULL};
  static Handle_Standard_Type _aType = new Standard_Type("GEOMImpl_ImportDriver",
			                                 sizeof(GEOMImpl_ImportDriver),
			                                 1,
			                                 (Standard_Address)_Ancestors,
			                                 (Standard_Address)NULL);

  return _aType;
}

//=======================================================================
//function : DownCast
//purpose  :
//=======================================================================
const Handle(GEOMImpl_ImportDriver) Handle(GEOMImpl_ImportDriver)::DownCast(const Handle(Standard_Transient)& AnObject)
{
  Handle(GEOMImpl_ImportDriver) _anOtherObject;

  if (!AnObject.IsNull()) {
     if (AnObject->IsKind(STANDARD_TYPE(GEOMImpl_ImportDriver))) {
       _anOtherObject = Handle(GEOMImpl_ImportDriver)((Handle(GEOMImpl_ImportDriver)&)AnObject);
     }
  }

  return _anOtherObject ;
}
