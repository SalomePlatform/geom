using namespace std;

#include "GEOMImpl_IInsertOperations.hxx"

#include "utilities.h"
#include "OpUtil.hxx"
#include "Utils_ExceptHandlers.hxx"

#include <TFunction_DriverTable.hxx>
#include <TFunction_Driver.hxx>
#include <TFunction_Logbook.hxx>
#include <TDF_Tool.hxx>

#include "GEOM_Function.hxx"

#include "GEOMImpl_CopyDriver.hxx"
#include "GEOMImpl_ExportDriver.hxx"
#include "GEOMImpl_ImportDriver.hxx"

#include "GEOMImpl_ICopy.hxx"
#include "GEOMImpl_IImportExport.hxx"

#include "GEOMImpl_Types.hxx"

#include <Standard_ErrorHandler.hxx> // CAREFUL ! position of this file is critic : see Lucien PIGNOLONI / OCC

//=============================================================================
/*!
 *   constructor:
 */
//=============================================================================

GEOMImpl_IInsertOperations::GEOMImpl_IInsertOperations(GEOM_Engine* theEngine, int theDocID)
: GEOM_IOperations(theEngine, theDocID)
{
  MESSAGE("GEOMImpl_IInsertOperations::GEOMImpl_IInsertOperations");
}

//=============================================================================
/*!
 *  destructor
 */
//=============================================================================

GEOMImpl_IInsertOperations::~GEOMImpl_IInsertOperations()
{
  MESSAGE("GEOMImpl_IInsertOperations::~GEOMImpl_IInsertOperations");
}



//=============================================================================
/*!
 *  MakeCopy
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IInsertOperations::MakeCopy(Handle(GEOM_Object) theOriginal)
{
  SetErrorCode(KO);

  if (theOriginal.IsNull()) return NULL;

  //Add a new Copy object
  Handle(GEOM_Object) aCopy = GetEngine()->AddObject(GetDocID(), GEOM_COPY);

  //Add a Copy function for creation a copy object
  Handle(GEOM_Function) aFunction = aCopy->AddFunction(GEOMImpl_CopyDriver::GetID(), COPY_WITH_REF);

  //Check if the function is set correctly
  if(aFunction->GetDriverGUID() != GEOMImpl_CopyDriver::GetID()) return NULL;

  GEOMImpl_ICopy aCI(aFunction);

  Handle(GEOM_Function) aRefFunction = theOriginal->GetLastFunction();
  if (aRefFunction.IsNull()) return NULL;

  aCI.SetOriginal(aRefFunction);

  //Compute the Copy value
  try {
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Copy driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  TCollection_AsciiString anEntry, aDescr;
  TDF_Tool::Entry(aCopy->GetEntry(), anEntry);
  aDescr += (anEntry+" = IInsertOperations.MakeCopy(");
  TDF_Tool::Entry(theOriginal->GetEntry(), anEntry);
  aDescr += (anEntry+")");

  aFunction->SetDescription(aDescr);

  SetErrorCode(OK);
  return aCopy;
}

//=============================================================================
/*!
 *  Export
 */
//=============================================================================
void GEOMImpl_IInsertOperations::Export
                     (const Handle(GEOM_Object) theOriginal,
		      const char*               theFileName,
		      const char*               theFormatName)
{
  SetErrorCode(KO);

  if (theOriginal.IsNull()) return;

  Handle(GEOM_Function) aRefFunction = theOriginal->GetLastFunction();
  if (aRefFunction.IsNull()) return;  //There is no function which creates an object to be exported

  //Add an Export function
  Handle(GEOM_Function) aFunction = theOriginal->AddFunction(GEOMImpl_ExportDriver::GetID(), EXPORT_SHAPE);
  if (aFunction.IsNull()) return;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_ExportDriver::GetID()) return;

  //Set parameters
  GEOMImpl_IImportExport aCI (aFunction);
  aCI.SetOriginal(aRefFunction);
  char* aFileName = (char*)theFileName;
  aCI.SetFileName(aFileName);

  char* aFormatName = (char*)theFormatName;
  Handle(TCollection_HAsciiString) aHLibName;
  if (!IsSupported(Standard_False, aFormatName, aHLibName)) {
    return;
  }
  TCollection_AsciiString aLibName = aHLibName->String();
  aCI.SetPluginName(aLibName);

  //Perform the Export
  try {
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Export driver failed");
      return;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return;
  }

  //Make a Python command
  TCollection_AsciiString anEntry, aDescr;
  aDescr = "IInsertOperations.Export(";
  TDF_Tool::Entry(theOriginal->GetEntry(), anEntry);
  aDescr += (anEntry + ", ");
  aDescr += (TCollection_AsciiString(aFileName) + ", ");
  aDescr += (TCollection_AsciiString(aFormatName) + ")");

  aFunction->SetDescription(aDescr);

  SetErrorCode(OK);
}

//=============================================================================
/*!
 *  Import
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IInsertOperations::Import
                     (const char* theFileName,
		      const char* theFormatName)
{
  SetErrorCode(KO);

  if (!theFileName || !theFormatName) return NULL;

  //Add a new result object
  Handle(GEOM_Object) result = GetEngine()->AddObject(GetDocID(), GEOM_IMPORT);

  //Add an Import function
  Handle(GEOM_Function) aFunction = result->AddFunction(GEOMImpl_ImportDriver::GetID(), IMPORT_SHAPE);
  if (aFunction.IsNull()) return result;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_ImportDriver::GetID()) return result;

  //Set parameters
  GEOMImpl_IImportExport aCI (aFunction);
  char* aFileName = (char*)theFileName;
  aCI.SetFileName(aFileName);

  char* aFormatName = (char*)theFormatName;
  Handle(TCollection_HAsciiString) aHLibName;
  if (!IsSupported(Standard_True, aFormatName, aHLibName)) {
    return result;
  }
  TCollection_AsciiString aLibName = aHLibName->String();
  aCI.SetPluginName(aLibName);

  //Perform the Import
  try {
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Import driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  TCollection_AsciiString anEntry, aDescr;
  TDF_Tool::Entry(result->GetEntry(), anEntry);
  aDescr += (anEntry + " = IInsertOperations.Import(");
  aDescr += (TCollection_AsciiString(aFileName) + ", ");
  aDescr += (TCollection_AsciiString(aFormatName) + ")");

  aFunction->SetDescription(aDescr);

  SetErrorCode(OK);
  return result;
}

//=============================================================================
/*!
 *  ImportTranslators
 */
//=============================================================================
Standard_Boolean GEOMImpl_IInsertOperations::ImportTranslators
                     (Handle(TColStd_HSequenceOfAsciiString)& theFormats,
		      Handle(TColStd_HSequenceOfAsciiString)& thePatterns)
{
  if (theFormats.IsNull())
    theFormats = new TColStd_HSequenceOfAsciiString;
  else
    theFormats->Clear();

  if (thePatterns.IsNull())
    thePatterns = new TColStd_HSequenceOfAsciiString;
  else
    thePatterns->Clear();

  if (!InitResMgr()) return Standard_False;

  // Read Import formats list
  if (myResMgr->Find("Import")) {
    TCollection_AsciiString aFormats (myResMgr->Value("Import"));
    TCollection_AsciiString aToken = aFormats.Token("| \t", 1);
    int i = 1;
    for (; !aToken.IsEmpty(); aToken = aFormats.Token("| \t", ++i)) {
      theFormats->Append(aToken);
    }
  }

  // Read Patterns for each supported format
  int j = 1, len = theFormats->Length();
  for (; j <= len; j++) {
    TCollection_AsciiString aPattern;
    TCollection_AsciiString aKey (theFormats->Value(j));
    aKey += ".Pattern";
    if (myResMgr->Find(aKey.ToCString()))
      aPattern = myResMgr->Value(aKey.ToCString());
    else {
      aPattern = theFormats->Value(j);
      aPattern += " Files ( *.* )";
    }
    thePatterns->Append(aPattern);
  }

  return (!theFormats->IsEmpty());
}

//=============================================================================
/*!
 *  ExportTranslators
 */
//=============================================================================
Standard_Boolean GEOMImpl_IInsertOperations::ExportTranslators
                     (Handle(TColStd_HSequenceOfAsciiString)& theFormats,
		      Handle(TColStd_HSequenceOfAsciiString)& thePatterns)
{
  if (theFormats.IsNull())
    theFormats = new TColStd_HSequenceOfAsciiString;
  else
    theFormats->Clear();

  if (thePatterns.IsNull())
    thePatterns = new TColStd_HSequenceOfAsciiString;
  else
    thePatterns->Clear();

  if (!InitResMgr()) return Standard_False;

  // Read Export formats list
  if (myResMgr->Find("Export")) {
    TCollection_AsciiString aFormats (myResMgr->Value("Export"));
    TCollection_AsciiString aToken = aFormats.Token("| \t", 1);
    int i = 1;
    for (; !aToken.IsEmpty(); aToken = aFormats.Token("| \t", ++i)) {
      theFormats->Append(aToken);
    }
  }

  // Read Patterns for each supported format
  int j = 1, len = theFormats->Length();
  for (; j <= len; j++) {
    TCollection_AsciiString aPattern;
    TCollection_AsciiString aKey (theFormats->Value(j));
    aKey += ".Pattern";
    if (myResMgr->Find(aKey.ToCString()))
      aPattern = myResMgr->Value(aKey.ToCString());
    else {
      aPattern = theFormats->Value(j);
      aPattern += " Files ( *.* )";
    }
    thePatterns->Append(aPattern);
  }

  return (!theFormats->IsEmpty());
}

//=============================================================================
/*!
 *  IsSupported
 */
//=============================================================================
Standard_Boolean GEOMImpl_IInsertOperations::IsSupported
                            (const Standard_Boolean isImport,
                             const TCollection_AsciiString theFormat,
                             Handle(TCollection_HAsciiString)& theLibName)
{
  if (!InitResMgr()) return Standard_False;

  // Import/Export mode
  Standard_CString aMode;
  if (isImport) aMode = "Import";
  else aMode = "Export";

  // Read supported formats for the certain mode
  if (myResMgr->Find(aMode)) {
    TCollection_AsciiString aFormats (myResMgr->Value(aMode));
    if (aFormats.Search(theFormat) > -1) {
      // Read library name for the supported format
      TCollection_AsciiString aKey (theFormat);
      aKey += ".";
      aKey += aMode;
      if (myResMgr->Find(aKey.ToCString())) {
        TCollection_AsciiString aLibName (myResMgr->Value(aKey.ToCString()));
        theLibName = new TCollection_HAsciiString (aLibName);
        return Standard_True;
      }
    }
  }

  return Standard_False;
}

//=============================================================================
/*!
 *  InitResMgr
 */
//=============================================================================
Standard_Boolean GEOMImpl_IInsertOperations::InitResMgr()
{
  if (myResMgr.IsNull()) {
    // Initialize the Resource Manager
    TCollection_AsciiString aResDir (getenv("GEOM_ROOT_DIR"));
#ifdef WNT
    aResDir += "\\share\\salome\\resources";
#else
    aResDir += "/share/salome/resources";
#endif
    TCollection_AsciiString aUserResDir (getenv("HOME"));
#ifdef WNT
    aUserResDir += "\\.salome\\resources";
#else
    aUserResDir += "/.salome/resources";
#endif
    myResMgr = new Resource_Manager ("ImportExport", aResDir, aUserResDir, Standard_True);
  }

  if (myResMgr->Find("Import") || myResMgr->Find("Export"))
    return Standard_True;

  return Standard_False;
}
