// Copyright (C) 2007-2014  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include <Standard_Stream.hxx>

#include <GEOMImpl_IInsertOperations.hxx>

#include <GEOMImpl_CopyDriver.hxx>
#include <GEOMImpl_ExportDriver.hxx>
#include <GEOMImpl_ImportDriver.hxx>
#include <GEOMImpl_ICopy.hxx>
#include <GEOMImpl_IImportExport.hxx>
#include <GEOMImpl_Types.hxx>
#include "GEOMImpl_IShapesOperations.hxx"
#include "GEOMImpl_IGroupOperations.hxx"
#include "GEOMImpl_IFieldOperations.hxx"
#include "GEOMImpl_XAODriver.hxx"
#include "GEOMImpl_IImportExportXAO.hxx"

#include <GEOM_Function.hxx>
#include <GEOM_PythonDump.hxx>
#include "GEOM_ISubShape.hxx"

#include <XAO_Xao.hxx>
#include <XAO_Geometry.hxx>
#include <XAO_BrepGeometry.hxx>
#include <XAO_Group.hxx>
#include <XAO_Field.hxx>
#include <XAO_XaoUtils.hxx>
#include <XAO_BooleanField.hxx>
#include <XAO_IntegerField.hxx>
#include <XAO_DoubleField.hxx>
#include <XAO_StringField.hxx>
#include <XAO_BooleanStep.hxx>
#include <XAO_IntegerStep.hxx>
#include <XAO_DoubleStep.hxx>
#include <XAO_StringStep.hxx>

#include <Basics_OCCTVersion.hxx>

#include "utilities.h"
#include <OpUtil.hxx>
#include <Utils_ExceptHandlers.hxx>

#include <TFunction_DriverTable.hxx>
#include <TFunction_Driver.hxx>
#include <TFunction_Logbook.hxx>
#include <TDF_ChildIDIterator.hxx>
#include <TDF_Tool.hxx>
#include <TDataStd_Integer.hxx>
#include <TNaming_NamedShape.hxx>
#include <TDataStd_Comment.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <gp_Pnt.hxx>

#if OCC_VERSION_LARGE > 0x06040000 // Porting to OCCT6.5.1
#include <TColStd_HArray1OfByte.hxx>
#include <TColStd_HArray1OfReal.hxx>
#else
#include <TDataStd_HArray1OfByte.hxx>
#endif

#include <Standard_Failure.hxx>
#include <Standard_ErrorHandler.hxx> // CAREFUL ! position of this file is critic : see Lucien PIGNOLONI / OCC

/**
 * This function returns the input format name from the original format name.
 */
static TCollection_AsciiString GetImportFormatName
        (const TCollection_AsciiString& theFormatName)
{
  return theFormatName.Token("_");
}

//=============================================================================
/*!
 *  constructor
 */
//=============================================================================
GEOMImpl_IInsertOperations::GEOMImpl_IInsertOperations(GEOM_Engine* theEngine, int theDocID)
: GEOM_IOperations(theEngine, theDocID)
{
  MESSAGE("GEOMImpl_IInsertOperations::GEOMImpl_IInsertOperations");
  myShapesOperations = new GEOMImpl_IShapesOperations(GetEngine(), GetDocID());
  myGroupOperations = new GEOMImpl_IGroupOperations(GetEngine(), GetDocID());
  myFieldOperations = new GEOMImpl_IFieldOperations(GetEngine(), GetDocID());
}

//=============================================================================
/*!
 *  destructor
 */
//=============================================================================
GEOMImpl_IInsertOperations::~GEOMImpl_IInsertOperations()
{
  MESSAGE("GEOMImpl_IInsertOperations::~GEOMImpl_IInsertOperations");
  delete myShapesOperations;
  delete myGroupOperations;
  delete myFieldOperations;
}

//=============================================================================
/*!
 *  MakeCopy
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IInsertOperations::MakeCopy (Handle(GEOM_Object) theOriginal)
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
#if OCC_VERSION_LARGE > 0x06010000
    OCC_CATCH_SIGNALS;
#endif
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
  GEOM::TPythonDump(aFunction) << aCopy << " = geompy.MakeCopy(" << theOriginal << ")";

  SetErrorCode(OK);
  return aCopy;
}

//=============================================================================
/*!
 *  Export
 */
//=============================================================================
void GEOMImpl_IInsertOperations::Export
                     (const Handle(GEOM_Object)      theOriginal,
                      const TCollection_AsciiString& theFileName,
                      const TCollection_AsciiString& theFormatName)
{
  SetErrorCode(KO);

  if (theOriginal.IsNull()) return;

  Handle(GEOM_Function) aRefFunction = theOriginal->GetLastFunction();
  if (aRefFunction.IsNull()) return;  //There is no function which creates an object to be exported

  //Add a new result object
  Handle(GEOM_Object) result = GetEngine()->AddObject(GetDocID(), GEOM_IMPORT);

  //Add an Export function
  Handle(GEOM_Function) aFunction = result->AddFunction(GEOMImpl_ExportDriver::GetID(), EXPORT_SHAPE);
  if (aFunction.IsNull()) return;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_ExportDriver::GetID()) return;

  Handle(TCollection_HAsciiString) aHLibName;
  if (!IsSupported(Standard_False, theFormatName, aHLibName)) {
    return;
  }
  TCollection_AsciiString aLibName = aHLibName->String();

  //Set parameters
  GEOMImpl_IImportExport aCI (aFunction);
  aCI.SetOriginal(aRefFunction);
  aCI.SetFileName(theFileName);
  aCI.SetFormatName(theFormatName);
  aCI.SetPluginName(aLibName);

  //Perform the Export
  try {
#if OCC_VERSION_LARGE > 0x06010000
    OCC_CATCH_SIGNALS;
#endif
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Not enough space on disk, or you haven't permissions to write this directory");
      return;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return;
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction) << "geompy.Export(" << theOriginal << ", \""
    << theFileName.ToCString() << "\", \"" << theFormatName.ToCString() << "\")";

  SetErrorCode(OK);
}

//=============================================================================
/*!
 *  Import
 */
//=============================================================================
Handle(TColStd_HSequenceOfTransient) GEOMImpl_IInsertOperations::Import
                                 (const TCollection_AsciiString& theFileName,
                                  const TCollection_AsciiString& theFormatName)
{
  SetErrorCode(KO);

  if (theFileName.IsEmpty() || theFormatName.IsEmpty()) return NULL;

  //Add a new result object
  Handle(GEOM_Object) anImported = GetEngine()->AddObject(GetDocID(), GEOM_IMPORT);

  //Add an Import function
  Handle(GEOM_Function) aFunction =
    anImported->AddFunction(GEOMImpl_ImportDriver::GetID(), IMPORT_SHAPE);

  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_ImportDriver::GetID()) return NULL;

  Handle(TCollection_HAsciiString) aHLibName;
  if (!IsSupported
          (Standard_True, GetImportFormatName(theFormatName), aHLibName)) {
    return NULL;
  }
  TCollection_AsciiString aLibName = aHLibName->String();

  //Set parameters
  GEOMImpl_IImportExport aCI (aFunction);
  aCI.SetFileName(theFileName);
  aCI.SetFormatName(theFormatName);
  aCI.SetPluginName(aLibName);

  //Perform the Import
  Handle(TColStd_HSequenceOfTransient) aSeq = new TColStd_HSequenceOfTransient;

  try {
#if OCC_VERSION_LARGE > 0x06010000
    OCC_CATCH_SIGNALS;
#endif
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Import driver failed");
      return NULL;
    }

    aSeq->Append(anImported);

    // Greate material groups.
    MakeMaterialGroups(anImported, aSeq);
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  if (theFormatName != "IGES_UNIT") {
    GEOM::TPythonDump pd (aFunction);
    if (theFormatName == "BREP")
      pd << anImported << " = geompy.ImportBREP(\"" << theFileName.ToCString() << "\")";
    else if (theFormatName == "IGES")
      pd << anImported << " = geompy.ImportIGES(\"" << theFileName.ToCString() << "\")";
    else if (theFormatName == "IGES_SCALE")
      pd << anImported << " = geompy.ImportIGES(\"" << theFileName.ToCString() << "\", True)";
    else if (theFormatName == "STEP")
      pd << anImported << " = geompy.ImportSTEP(\"" << theFileName.ToCString() << "\")";
    else if (theFormatName == "STEP_SCALE")
      pd << anImported << " = geompy.ImportSTEP(\"" << theFileName.ToCString() << "\", True)";
    else {
      pd << anImported << " = geompy.ImportFile(\""
         << theFileName.ToCString() << "\", \"" << theFormatName.ToCString() << "\")";
    }
  }

  SetErrorCode(OK);

  // OLD CODE: begin
  if (theFormatName == "IGES_UNIT") {
    TopoDS_Shape S = aFunction->GetValue();
    TopoDS_Vertex V = TopoDS::Vertex(S);
    gp_Pnt P = BRep_Tool::Pnt(V);
    double scale = P.X();
    TCollection_AsciiString aUnitName = "UNIT_M";
    if (fabs(scale-0.01) < 1.e-6)
      aUnitName = "UNIT_CM";
    else if (fabs(scale-0.001) < 1.e-6)
      aUnitName = "UNIT_MM";
    //cout<<"IIO: aUnitName = "<<aUnitName.ToCString()<<endl;
    SetErrorCode(aUnitName);
  }
  // OLD CODE: end

  return aSeq;
}

//=============================================================================
/*!
 *  ReadValue
 */
//=============================================================================
TCollection_AsciiString GEOMImpl_IInsertOperations::ReadValue
                                 (const TCollection_AsciiString& theFileName,
                                  const TCollection_AsciiString& theFormatName,
                                  const TCollection_AsciiString& theParameterName)
{
  SetErrorCode(KO);

  TCollection_AsciiString aValue, anError;

  if (theFileName.IsEmpty() || theFormatName.IsEmpty() || theParameterName.IsEmpty()) return aValue;

  Handle(TCollection_HAsciiString) aHLibName;
  if (!IsSupported
          (Standard_True, GetImportFormatName(theFormatName), aHLibName)) {
    return aValue;
  }
  TCollection_AsciiString aLibName = aHLibName->String();

  aValue = GEOMImpl_ImportDriver::ReadValue(theFileName, aLibName, theParameterName, anError);
  if (anError.IsEmpty())
    SetErrorCode(OK);
  else
    SetErrorCode(anError.ToCString());

  return aValue;
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

  // Read Import formats list from install directory
  if (myResMgr->Find("Import")) {
    TCollection_AsciiString aFormats (myResMgr->Value("Import"));
    TCollection_AsciiString aToken = aFormats.Token("| \t", 1);
    int i = 1;
    for (; !aToken.IsEmpty(); aToken = aFormats.Token("| \t", ++i)) {
      theFormats->Append(aToken);
    }
  }

  // Read Import formats from user directory
  if (myResMgrUser->Find("Import")) {
    TCollection_AsciiString aFormats (myResMgrUser->Value("Import"));
    TCollection_AsciiString aToken = aFormats.Token("| \t", 1);
    int i = 1;
    for (; !aToken.IsEmpty(); aToken = aFormats.Token("| \t", ++i)) {
      int aLenFormats = theFormats->Length();
      bool isFound = false;
      for(int aInd=1;aInd<=aLenFormats;aInd++){
        if( theFormats->Value(aInd) == aToken){
          isFound = true;
          break;
        }
      }
      if(!isFound)
        theFormats->Append(aToken);
    }
  }

  // Read Patterns for each supported format
  int j = 1, len = theFormats->Length();
  for (; j <= len; j++) {
    TCollection_AsciiString aKey, aPattern;
    aKey = theFormats->Value(j) + ".ImportPattern";
    if (myResMgr->Find(aKey.ToCString()))
      aPattern = myResMgr->Value(aKey.ToCString());
    else if(myResMgrUser->Find(aKey.ToCString()))
      aPattern = myResMgrUser->Value(aKey.ToCString());
    else {
      aKey = theFormats->Value(j) + ".Pattern";
      if (myResMgr->Find(aKey.ToCString()))
        aPattern = myResMgr->Value(aKey.ToCString());
      else if(myResMgrUser->Find(aKey.ToCString()))
        aPattern = myResMgrUser->Value(aKey.ToCString());
      else {
        aPattern = theFormats->Value(j);
        aPattern += " Files ( *.* )";
      }
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

  // Read Export formats list from install directory
  if (myResMgr->Find("Export")) {
    TCollection_AsciiString aFormats (myResMgr->Value("Export"));
    TCollection_AsciiString aToken = aFormats.Token("| \t", 1);
    int i = 1;
    for (; !aToken.IsEmpty(); aToken = aFormats.Token("| \t", ++i)) {
      theFormats->Append(aToken);
    }
  }

  // Read Export formats list from user directory
  if (myResMgrUser->Find("Export")) {
    TCollection_AsciiString aFormats (myResMgrUser->Value("Export"));
    TCollection_AsciiString aToken = aFormats.Token("| \t", 1);
    int i = 1;
    for (; !aToken.IsEmpty(); aToken = aFormats.Token("| \t", ++i)) {
      int aLenFormats = theFormats->Length();
      bool isFound = false;
      for(int aInd=1;aInd<=aLenFormats;aInd++){
        if( theFormats->Value(aInd) == aToken){
          isFound = true;
          break;
        }
      }
      if(!isFound)
        theFormats->Append(aToken);
    }
  }

  // Read Patterns for each supported format
  int j = 1, len = theFormats->Length();
  for (; j <= len; j++) {
    TCollection_AsciiString aKey, aPattern;
    aKey = theFormats->Value(j) + ".ExportPattern";
    if (myResMgr->Find(aKey.ToCString()))
      aPattern = myResMgr->Value(aKey.ToCString());
    else if (myResMgrUser->Find(aKey.ToCString()))
      aPattern = myResMgrUser->Value(aKey.ToCString());
    else {
      aKey = theFormats->Value(j) + ".Pattern";
      if (myResMgr->Find(aKey.ToCString()))
        aPattern = myResMgr->Value(aKey.ToCString());
      else if (myResMgrUser->Find(aKey.ToCString()))
        aPattern = myResMgrUser->Value(aKey.ToCString());
      else {
        aPattern = theFormats->Value(j);
        aPattern += " Files ( *.* )";
      }
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
                             const TCollection_AsciiString& theFormat,
                             Handle(TCollection_HAsciiString)& theLibName)
{
  if (!InitResMgr()) return Standard_False;

  // Import/Export mode
  TCollection_AsciiString aMode;
  //Standard_CString aMode;
  if (isImport) aMode = "Import";
  else aMode = "Export";

  // Read supported formats for the certain mode from install directory
  if (myResMgr->Find(aMode.ToCString())) {
    TCollection_AsciiString aFormats (myResMgr->Value(aMode.ToCString()));
    if (aFormats.Search(theFormat) > -1) {
      // Read library name for the supported format
      TCollection_AsciiString aKey (theFormat);
      aKey += ".";
      aKey += aMode;
      if (myResMgr->Find(aKey.ToCString())) {
        TCollection_AsciiString aLibName (myResMgr->Value(aKey.ToCString()));        
#ifndef WIN32
	if ( aLibName.Length() > 3 && aLibName.SubString(1,3) != "lib" )
	  aLibName.Prepend("lib");
        aLibName += ".so";
#else
        aLibName += ".dll";
#endif
        theLibName = new TCollection_HAsciiString (aLibName);
        return Standard_True;
      }
    }
  }
  
  // Read supported formats for the certain mode from user directory
  if (myResMgrUser->Find(aMode.ToCString())) {
    TCollection_AsciiString aFormats (myResMgrUser->Value(aMode.ToCString()));
    if (aFormats.Search(theFormat) > -1) {
      // Read library name for the supported format
      TCollection_AsciiString aKey (theFormat);
      aKey += ".";
      aKey += aMode;
      if (myResMgrUser->Find(aKey.ToCString())) {
        TCollection_AsciiString aLibName (myResMgrUser->Value(aKey.ToCString()));
#ifndef WIN32
	if ( aLibName.Length() > 3 && aLibName.SubString(1,3) != "lib" )
	  aLibName.Prepend("lib");
        aLibName += ".so";
#else
        aLibName += ".dll";
#endif
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
  bool isResourceFound     = false;
  bool isResourceFoundUser = false;
  TCollection_AsciiString aUserResDir,aResDir;
  
  if (myResMgr.IsNull()) {
    // Initialize the Resource Manager
    TCollection_AsciiString aNull;
    aResDir = TCollection_AsciiString(getenv("GEOM_ROOT_DIR"));
#ifdef WIN32
    aResDir += "\\share\\salome\\resources\\geom";
#else
    aResDir += "/share/salome/resources/geom";
#endif
    
    myResMgr = new Resource_Manager ("ImportExport", aResDir, aNull, Standard_False);

    isResourceFound = true;
    if (!myResMgr->Find("Import") && !myResMgr->Find("Export")) {
      // instead of complains in Resource_Manager
      isResourceFound = false;
      INFOS("No valid file \"ImportExport\" found in " << aResDir.ToCString());
    }
  } else
    isResourceFound = true;

  if (myResMgrUser.IsNull()) {
    char * dir = getenv("GEOM_ENGINE_RESOURCES_DIR");
    TCollection_AsciiString aNull;
    if ( dir )
    {
      aUserResDir = dir;
    }
    else
    {
      aUserResDir = getenv("HOME");
#ifdef WIN32
      aUserResDir += "\\.salome\\resources";
#else
      aUserResDir += "/.salome/resources";
#endif
    }

    myResMgrUser = new Resource_Manager ("ImportExport", aNull, aUserResDir, Standard_False);

    isResourceFoundUser = true;
    
    if (!myResMgrUser->Find("Import") && !myResMgrUser->Find("Export")) {
      // instead of complains in Resource_Manager
      isResourceFoundUser = false;
    }
      
  } else
    isResourceFoundUser = true;
    
  if(!isResourceFound && !isResourceFoundUser){
    INFOS("No valid file \"ImportExport\" found in " << aResDir.ToCString());
    INFOS("No valid file \"ImportExport\" found in " << aUserResDir.ToCString() );
  }

  return ( myResMgr->Find("Import") || myResMgr->Find("Export") ||
           myResMgrUser->Find("Import") || myResMgrUser->Find("Export"));
}

//=============================================================================
/*!
 *  RestoreShape
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IInsertOperations::RestoreShape (std::istringstream& theStream)
{
  SetErrorCode(KO);

  //Add a new result object
  Handle(GEOM_Object) result = GetEngine()->AddObject(GetDocID(), GEOM_COPY);

  //Add a Copy function
  Handle(GEOM_Function) aFunction = result->AddFunction(GEOMImpl_CopyDriver::GetID(), COPY_WITHOUT_REF);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_CopyDriver::GetID()) return NULL;

  //Read a shape from the stream
  TopoDS_Shape aShape;
  BRep_Builder B;
  BRepTools::Read(aShape, theStream, B);
  if (aShape.IsNull()) {
    SetErrorCode("RestoreShape error: BREP reading failed");
  }

  //Set function value
  aFunction->SetValue(aShape);

  //Special dump to avoid restored shapes publication.
  //See correcponding code in GEOM_Engine.cxx (method ProcessFunction)
  //GEOM::TPythonDump(aFunction) << "#";

  GEOM::TPythonDump(aFunction) << result
    << " = geompy.RestoreShape(\"\") # the shape string has not been dump for performance reason";

  SetErrorCode(OK);

  return result;
}

int GEOMImpl_IInsertOperations::LoadTexture(const TCollection_AsciiString& theTextureFile)
{
  SetErrorCode(KO);

  if (theTextureFile.IsEmpty()) return 0;

#if OCC_VERSION_LARGE > 0x06040000 // Porting to OCCT6.5.1
  Handle(TColStd_HArray1OfByte) aTexture;
#else
  Handle(TDataStd_HArray1OfByte) aTexture;
#endif

  FILE* fp = fopen(theTextureFile.ToCString(), "r");
  if (!fp) return 0;

  std::list<std::string> lines;
  char buffer[4096];
  int maxlen = 0;
  while (!feof(fp)) {
    if ((fgets(buffer, 4096, fp)) == NULL) break;
    int aLen = strlen(buffer);
    if (buffer[aLen-1] == '\n') buffer[aLen-1] = '\0';
    lines.push_back(buffer);
    maxlen = std::max(maxlen, (int)strlen(buffer));
  }

  fclose(fp);

  int lenbytes = maxlen/8;
  if (maxlen%8) lenbytes++;

  if (lenbytes == 0 || lines.empty())
    return 0;

  std::list<unsigned char> bytedata;
  std::list<std::string>::const_iterator it;
  for (it = lines.begin(); it != lines.end(); ++it) {
    std::string line = *it;
    int lenline = (line.size()/8 + (line.size()%8 ? 1 : 0)) * 8;
    for (int i = 0; i < lenline/8; i++) {
      unsigned char byte = 0;
      for (int j = 0; j < 8; j++)
        byte = (byte << 1) + ( i*8+j < line.size() && line[i*8+j] != '0' ? 1 : 0 );
      bytedata.push_back(byte);
    }
    for (int i = lenline/8; i < lenbytes; i++)
      bytedata.push_back((unsigned char)0);
  }

  if (bytedata.empty() || bytedata.size() != lines.size()*lenbytes)
    return 0;

#if OCC_VERSION_LARGE > 0x06040000 // Porting to OCCT6.5.1
  aTexture = new TColStd_HArray1OfByte (1, lines.size()*lenbytes);
#else
  aTexture = new TDataStd_HArray1OfByte (1, lines.size()*lenbytes);
#endif

  std::list<unsigned char>::iterator bdit;
  int i;
  for (i = 1, bdit = bytedata.begin(); bdit != bytedata.end(); ++bdit, ++i)
    aTexture->SetValue(i, (Standard_Byte)(*bdit));

  int aTextureId = GetEngine()->addTexture(GetDocID(), lenbytes*8, lines.size(), aTexture, theTextureFile);
  if (aTextureId > 0) SetErrorCode(OK);
  return aTextureId;
}
  
int GEOMImpl_IInsertOperations::AddTexture(int theWidth, int theHeight, 
#if OCC_VERSION_LARGE > 0x06040000 // Porting to OCCT6.5.1
                                           const Handle(TColStd_HArray1OfByte)& theTexture)
#else
                                           const Handle(TDataStd_HArray1OfByte)& theTexture)
#endif
{
  SetErrorCode(KO);
  int aTextureId = GetEngine()->addTexture(GetDocID(), theWidth, theHeight, theTexture);
  if (aTextureId > 0) SetErrorCode(OK);
  return aTextureId;
}

#if OCC_VERSION_LARGE > 0x06040000 // Porting to OCCT6.5.1
Handle(TColStd_HArray1OfByte) GEOMImpl_IInsertOperations::GetTexture(int theTextureId,
#else
Handle(TDataStd_HArray1OfByte) GEOMImpl_IInsertOperations::GetTexture(int theTextureId,
#endif
                                                                      int& theWidth, int& theHeight)
{
  SetErrorCode(KO);
  
#if OCC_VERSION_LARGE > 0x06040000 // Porting to OCCT6.5.1
  Handle(TColStd_HArray1OfByte) aTexture;
#else
  Handle(TDataStd_HArray1OfByte) aTexture;
#endif

  theWidth = theHeight = 0;
  TCollection_AsciiString aFileName;

  if (theTextureId <= 0)
    return aTexture;

  aTexture = GetEngine()->getTexture(GetDocID(), theTextureId, theWidth, theHeight, aFileName);

  if (theWidth > 0 && theHeight > 0 && aTexture->Length() > 0) SetErrorCode(OK);

  return aTexture;
}

std::list<int> GEOMImpl_IInsertOperations::GetAllTextures()
{
  SetErrorCode(KO);
  std::list<int> id_list = GetEngine()->getAllTextures(GetDocID());
  SetErrorCode(OK);
  return id_list;
}

TopAbs_ShapeEnum getGroupDimension(XAO::Group* group)
{
  XAO::Dimension dim = group->getDimension();
  TopAbs_ShapeEnum rdim;
  switch ( dim )
  {
  case XAO::VERTEX:
    rdim = TopAbs_VERTEX; break;
  case XAO::EDGE:
    rdim = TopAbs_EDGE; break;
  case XAO::FACE:
    rdim = TopAbs_FACE; break;
  case XAO::SOLID:
    rdim = TopAbs_SOLID; break;
  default:
    rdim = TopAbs_COMPOUND; break;
  }
  return rdim;
}

XAO::Dimension shapeEnumToDimension(const TopAbs_ShapeEnum& shape)
{
  XAO::Dimension dim;
  switch( shape ) {
  case TopAbs_VERTEX:
    dim = XAO::VERTEX; break;
  case TopAbs_EDGE:
    dim = XAO::EDGE; break;
  case TopAbs_FACE:
    dim = XAO::FACE; break;
  case TopAbs_SOLID:
    dim = XAO::SOLID; break;
  default:
    throw SALOME_Exception("Bad type"); // TODO
  }
  return dim;
}

void GEOMImpl_IInsertOperations::exportGroups(std::list<Handle(GEOM_Object)> groupList,
                                              XAO::Xao* xaoObject,
                                              XAO::BrepGeometry* geometry)
{
  // add the groups
  std::list<Handle(GEOM_Object)>::iterator groupIterator = groupList.begin();
  while (groupIterator != groupList.end())
  {
    Handle(GEOM_Object) currGroup = (*groupIterator++);
    Handle(TColStd_HArray1OfInteger) groupIds = myGroupOperations->GetObjects(currGroup);
    
    TopAbs_ShapeEnum shapeGroup = myGroupOperations->GetType(currGroup);
    XAO::Dimension dim = shapeEnumToDimension(shapeGroup);
    XAO::Group* group = xaoObject->addGroup(dim, currGroup->GetName().ToCString());
    
    switch (shapeGroup)
    {
    case TopAbs_VERTEX:
      for (int i = 1; i <= groupIds->Length(); i++)
      {
        std::string ref = XAO::XaoUtils::intToString(groupIds->Value(i));
        int index = geometry->getVertexIndexByReference(ref);
        group->add(index);
      }
      break;
    case TopAbs_EDGE:
      for (int i = 1; i <= groupIds->Length(); i++)
      {
        std::string ref = XAO::XaoUtils::intToString(groupIds->Value(i));
        int index = geometry->getEdgeIndexByReference(ref);
        group->add(index);
      }
      break;
    case TopAbs_FACE:
      for (int i = 1; i <= groupIds->Length(); i++)
      {
        std::string ref = XAO::XaoUtils::intToString(groupIds->Value(i));
        int index = geometry->getFaceIndexByReference(ref);
        group->add(index);
      }
      break;
    case TopAbs_SOLID:
      for (int i = 1; i <= groupIds->Length(); i++)
      {
        std::string ref = XAO::XaoUtils::intToString(groupIds->Value(i));
        int index = geometry->getSolidIndexByReference(ref);
        group->add(index);
      }
      break;
    }
  }
}

void GEOMImpl_IInsertOperations::exportFields(std::list<Handle(GEOM_Field)> fieldList,
                                              XAO::Xao* xaoObject,
                                              XAO::BrepGeometry* geometry)
{
    std::list<Handle(GEOM_Field)>::iterator fieldIterator = fieldList.begin();
    while (fieldIterator != fieldList.end())
    {
        Handle(GEOM_Field) currField = (*fieldIterator++);

        int fdim = currField->GetDimension();
        int ftype = currField->GetDataType();
        int nbComponents = currField->GetNbComponents();
        std::string name = currField->GetName().ToCString();

        XAO::Field* field = xaoObject->addField((XAO::Type)ftype, (XAO::Dimension)fdim, nbComponents, name);

        Handle(TColStd_HArray1OfExtendedString) components = currField->GetComponents();
        for (int i = components->Lower(), j = 0; i <= components->Upper(); ++i, ++j)
        {
            field->setComponentName(j, TCollection_AsciiString(components->Value(i)).ToCString());
        }

        std::list< Handle(GEOM_FieldStep)> steps = currField->GetSteps();
        std::list<Handle(GEOM_FieldStep)>::iterator stepIterator = steps.begin();
        while (stepIterator != steps.end())
        {
            Handle(GEOM_FieldStep) currStep = (*stepIterator++);

            XAO::Step* step = field->addNewStep(currStep->GetID());
            step->setStamp(currStep->GetStamp());

            switch (ftype)
            {
                case 0: // bool
                {
                    XAO::BooleanStep* bs = (XAO::BooleanStep*)step;
                    Handle(TColStd_HArray1OfInteger) bvalues = currStep->GetIntValues();
                    std::vector<bool> bv;
                    bv.reserve(bvalues->Upper());
                    for ( int i = bvalues->Lower(), nb = bvalues->Upper(); i <= nb; ++i )
                    {
                        bv.push_back(bvalues->Value(i) != 0);
                    }
                    bs->setValues(bv);
                    break;
                }
                case 1: // integer
                {
                    XAO::IntegerStep* is = (XAO::IntegerStep*)step;
                    Handle(TColStd_HArray1OfInteger) ivalues = currStep->GetIntValues();
                    std::vector<int> iv;
                    iv.reserve(ivalues->Upper());
                    for ( int i = ivalues->Lower(), nb = ivalues->Upper(); i <= nb; ++i )
                    {
                        iv.push_back(ivalues->Value(i));
                    }
                    is->setValues(iv);
                    break;
                }
                case 2: // double
                {
                    XAO::DoubleStep* ds = (XAO::DoubleStep*)step;
                    Handle(TColStd_HArray1OfReal) dvalues = currStep->GetDoubleValues();
                    std::vector<double> dv;
                    dv.reserve(dvalues->Upper());
                    for ( int i = dvalues->Lower(), nb = dvalues->Upper(); i <= nb; ++i )
                    {
                        dv.push_back(dvalues->Value(i));
                    }
                    ds->setValues(dv);
                    break;
                }
                case 3: // string
                {
                    XAO::StringStep* ss = (XAO::StringStep*)step;
                    Handle(TColStd_HArray1OfExtendedString) svalues = currStep->GetStringValues();
                    std::vector<std::string> sv;
                    sv.reserve(svalues->Upper());
                    for ( int i = svalues->Lower(), nb = svalues->Upper(); i <= nb; ++i )
                    {
                        sv.push_back(TCollection_AsciiString(svalues->Value(i)).ToCString());
                    }
                    ss->setValues(sv);
                    break;
                }
            }
        }
    }
}

void GEOMImpl_IInsertOperations::exportSubshapes(const Handle(GEOM_Object)& shape, XAO::BrepGeometry* geometry)
{
  Handle(TColStd_HSequenceOfTransient) subObjects = myShapesOperations->GetExistingSubObjects(shape, false);
  int nbSubObjects = subObjects->Length();
  // set the names of the sub shapes
  for (int i = 1; i <= nbSubObjects; i++)
  {
    Handle(Standard_Transient) transientSubObject = subObjects->Value(i);
    if (transientSubObject.IsNull())
      continue;
    
    Handle(GEOM_Object) subObject = Handle(GEOM_Object)::DownCast(transientSubObject);
    if (subObject->GetType() != GEOM_GROUP)
    {
      int subIndex = myShapesOperations->GetSubShapeIndex(shape, subObject);
      switch (subObject->GetValue().ShapeType())
      {
      case TopAbs_VERTEX:
        geometry->changeVertexName(subIndex, subObject->GetName().ToCString());
        break;
      case TopAbs_EDGE:
        geometry->changeEdgeName(subIndex, subObject->GetName().ToCString());
        break;
      case TopAbs_FACE:
        geometry->changeFaceName(subIndex, subObject->GetName().ToCString());
        break;
      case TopAbs_SOLID:
        geometry->changeSolidName(subIndex, subObject->GetName().ToCString());
        break;
      }
    }
  }
}

//=============================================================================
/*!
 *  Export a shape to XAO format
 *  \param shape The shape to export
 *  \param groups The list of groups to export
 *  \param fields The list of fields to export
 *  \param fileName The name of the file to exported
 *  \return boolean indicating if export was succeful.
 */
//=============================================================================
bool GEOMImpl_IInsertOperations::ExportXAO(Handle(GEOM_Object) shape,
                                           std::list<Handle(GEOM_Object)> groupList,
                                           std::list<Handle(GEOM_Field)> fieldList,
                                           const char* author,
                                           const char* fileName)
{
  SetErrorCode(KO);

  if (shape.IsNull()) return false;
  
  // add a new shape function with parameters
  Handle(GEOM_Function) lastFunction = shape->GetLastFunction();
  if (lastFunction.IsNull()) return false;
  
  // add a new result object
  Handle(GEOM_Object) result = GetEngine()->AddObject(GetDocID(), GEOM_IMPORT);
  
  // add an Export function
  Handle(GEOM_Function) exportFunction = result->AddFunction(GEOMImpl_XAODriver::GetID(), IMPORTEXPORT_EXPORTXAO);
  if (exportFunction.IsNull()) return false;
  if (exportFunction->GetDriverGUID() != GEOMImpl_XAODriver::GetID()) return false;
  
  // create the XAO object
  XAO::Xao* xaoObject = new XAO::Xao();
  xaoObject->setAuthor(author);
  
  // add the geometry
  XAO::BrepGeometry* geometry = (XAO::BrepGeometry*)XAO::Geometry::createGeometry(XAO::BREP);
  TopoDS_Shape topoShape = shape->GetValue();
  exportFunction->SetValue(topoShape);
  XAO::BrepGeometry* brep = (XAO::BrepGeometry*)geometry;
  brep->setTopoDS_Shape(topoShape);
  
  geometry->setName(shape->GetName().ToCString());
  exportSubshapes(shape, geometry);
  xaoObject->setGeometry(geometry);
  
  exportGroups(groupList, xaoObject, geometry);
  exportFields(fieldList, xaoObject, geometry);
  
  // export the XAO to the file
  xaoObject->exportXAO(fileName);
  
  // make a Python command
  GEOM::TPythonDump pd(exportFunction);
  pd << "exported = geompy.ExportXAO(" << shape;
  
  // list of groups
  pd << ", [";
  if (groupList.size() > 0)
  {
    std::list<Handle(GEOM_Object)>::iterator itGroup = groupList.begin();
    pd << (*itGroup++);
    while (itGroup != groupList.end())
    {
      pd << ", " << (*itGroup++);
    }
  }

  // list of fields
  pd << "], [";
  if (fieldList.size() > 0)
  {
    std::list<Handle(GEOM_Field)>::iterator itField = fieldList.begin();
    pd << (*itField++);
    while (itField != fieldList.end())
    {
      pd << ", " << (*itField++);
    }
  }
  pd << "], ";
  pd << "\"" << author << "\", \"" << fileName << "\")";
  
  SetErrorCode(OK);
  delete xaoObject;
  
  return true;
}

void GEOMImpl_IInsertOperations::importSubShapes(XAO::Geometry* xaoGeometry,
                                                 Handle(GEOM_Function) function, int shapeType, int dim,
                                                 Handle(TColStd_HSequenceOfTransient)& subShapeList)
{
  Handle(GEOM_Object) subShape;
  Handle(GEOM_Function) aFunction;
  Handle(TColStd_HArray1OfInteger) anArray;
  
  XAO::GeometricElementList::iterator elementIterator = xaoGeometry->begin((XAO::Dimension)dim);
  for (; elementIterator != xaoGeometry->end((XAO::Dimension)dim); elementIterator++)
  {
    XAO::GeometricElement element = elementIterator->second;
    if (!element.hasName())
      continue;
    
    std::string name = element.getName();
    std::string ref = element.getReference();
    int iref = XAO::XaoUtils::stringToInt(ref);
    
    anArray = new TColStd_HArray1OfInteger(1, 1);
    anArray->SetValue(1, iref);
    
    subShape = GetEngine()->AddObject(GetDocID(), GEOM_SUBSHAPE);
    Handle(GEOM_Function) aFunction = subShape->AddFunction(GEOM_Object::GetSubShapeID(), 1);
    if (aFunction.IsNull())
      return;
    
    subShape->SetName(name.c_str());
    subShape->SetType(shapeType);
    
    GEOM_ISubShape aSSI(aFunction);
    aSSI.SetMainShape(function);
    aSSI.SetIndices(anArray);
    
    //aFunction->SetValue(aValue);
    subShapeList->Append(subShape);
    
    // Put this subshape in the list of sub-shapes of theMainShape
    function->AddSubShapeReference(aFunction);
  }
}

//=============================================================================
/*!
 *  Import a shape from XAO format
 *  \param fileName The name of the file to import
 *  \param shape The imported shape
 *  \param subShapes The list of imported groups
 *  \param groups The list of imported groups
 *  \param fields The list of imported fields
 *  \return boolean indicating if import was succeful.
 */
//=============================================================================
bool GEOMImpl_IInsertOperations::ImportXAO(const char* fileName,
                                           Handle(GEOM_Object)& shape,
                                           Handle(TColStd_HSequenceOfTransient)& subShapes,
                                           Handle(TColStd_HSequenceOfTransient)& groups,
                                           Handle(TColStd_HSequenceOfTransient)& fields)
{
  SetErrorCode(KO);
  
  if (fileName == NULL || groups.IsNull() || fields.IsNull())
    return false;
  
  // Read the XAO
  XAO::Xao* xaoObject = new XAO::Xao();
  try
  {
    xaoObject->importXAO(fileName);
  }
  catch (XAO::XAO_Exception& exc)
  {
    delete xaoObject;
    SetErrorCode(exc.what());
    return false;
  }
  
  XAO::Geometry* xaoGeometry = xaoObject->getGeometry();
  if (xaoGeometry == NULL)
  {
    delete xaoObject;
    SetErrorCode("Cannot import XAO: geometry format not supported.");
    return false;
  }
  
  // create the shape
  shape = GetEngine()->AddObject(GetDocID(), GEOM_IMPORT);
  Handle(GEOM_Function) function = shape->AddFunction(GEOMImpl_XAODriver::GetID(), IMPORTEXPORT_EXPORTXAO);
  if (function.IsNull()) return false;
  if (function->GetDriverGUID() != GEOMImpl_XAODriver::GetID()) return false;
  
  // set the geometry
  if (xaoGeometry->getFormat() == XAO::BREP)
  {
    XAO::BrepGeometry* brep = (XAO::BrepGeometry*)xaoGeometry;
    TopoDS_Shape geomShape = brep->getTopoDS_Shape();
    function->SetValue(geomShape);
    shape->SetName(xaoGeometry->getName().c_str());
  }
  else
  {
    delete xaoObject;
    SetErrorCode("Cannot import XAO: geometry format not supported.");
    return false;
  }
  
  // create sub shapes with names
  importSubShapes(xaoGeometry, function, GEOM_POINT, XAO::VERTEX, subShapes);
  importSubShapes(xaoGeometry, function, GEOM_EDGE, XAO::EDGE, subShapes);
  importSubShapes(xaoGeometry, function, GEOM_FACE, XAO::FACE, subShapes);
  importSubShapes(xaoGeometry, function, GEOM_SOLID, XAO::SOLID, subShapes);
  
  // create groups
  int nbGroups = xaoObject->countGroups();
  for (int i = 0; i < nbGroups; ++i)
  {
    XAO::Group* xaoGroup = xaoObject->getGroup(i);
    
    // build an array with the indexes of the sub shapes
    int nbElt = xaoGroup->count();
    Handle(TColStd_HArray1OfInteger) array = new TColStd_HArray1OfInteger(1, nbElt);
    int j = 0;
    for (std::set<int>::iterator it = xaoGroup->begin(); it != xaoGroup->end(); ++it)
    {
      int index = (*it);
      std::string ref = xaoGeometry->getElementReference(xaoGroup->getDimension(), index);
      array->SetValue(++j, XAO::XaoUtils::stringToInt(ref));
    }
    
    // create the group with the array of sub shapes indexes
    Handle(GEOM_Object) group = GetEngine()->AddSubShape(shape, array);
    group->SetType(GEOM_GROUP);
    group->SetName(xaoGroup->getName().c_str());
    
    // Set a sub-shape type
    TDF_Label freeLabel = group->GetFreeLabel();
    TDataStd_Integer::Set(freeLabel, (Standard_Integer) getGroupDimension(xaoGroup));
    groups->Append(group);
    
    function = group->GetLastFunction();
  }
  
  // create the fields
  int nbFields = xaoObject->countFields();
  for (int i = 0; i < nbFields; ++i)
  {
    XAO::Field* xaoField = xaoObject->getField(i);

    Handle(TColStd_HArray1OfExtendedString) components = new TColStd_HArray1OfExtendedString(0, xaoField->countComponents()-1);
    for (int j = 0; j < xaoField->countComponents(); ++j)
    {
        components->SetValue(j, (TCollection_ExtendedString)xaoField->getComponentName(j).c_str());
    }

    Handle(GEOM_Field) field = myFieldOperations->CreateField(shape,
                 xaoField->getName().c_str(),
                 (int)xaoField->getType(),
                 (int)xaoField->getDimension(),
                 components);

    switch (xaoField->getType())
    {
        case XAO::BOOLEAN:
        {
            XAO::BooleanField* bfield = (XAO::BooleanField*)xaoField;
            for (int j = 0; j < xaoField->countSteps(); ++j)
            {
                XAO::BooleanStep* bstep = bfield->getStep(j);
                Handle(GEOM_FieldStep) step = field->AddStep(bstep->getStep(), bstep->getStamp());

                Handle(TColStd_HArray1OfInteger) values = new TColStd_HArray1OfInteger(0, bstep->countValues()-1);
                std::vector<bool> bvalues = bstep->getValues();
                for (int k = 0; k < bstep->countValues(); ++k)
                {
                    values->SetValue(k, bvalues[k] ? 1 : 0);
                }
                step->SetValues(values);
            }
            break;
        }
        case XAO::INTEGER:
        {
            XAO::IntegerField* ifield = (XAO::IntegerField*)xaoField;
            for (int j = 0; j < xaoField->countSteps(); ++j)
            {
                XAO::IntegerStep* istep = ifield->getStep(j);
                Handle(GEOM_FieldStep) step = field->AddStep(istep->getStep(), istep->getStamp());

                Handle(TColStd_HArray1OfInteger) values = new TColStd_HArray1OfInteger(0, istep->countValues()-1);
                std::vector<int> ivalues = istep->getValues();
                for (int k = 0; k < istep->countValues(); ++k)
                {
                    values->SetValue(k, ivalues[k]);
                }
                step->SetValues(values);
            }
            break;
        }
        case XAO::DOUBLE:
        {
            XAO::DoubleField* dfield = (XAO::DoubleField*)xaoField;
            for (int j = 0; j < xaoField->countSteps(); ++j)
            {
                XAO::DoubleStep* dstep = dfield->getStep(j);
                Handle(GEOM_FieldStep) step = field->AddStep(dstep->getStep(), dstep->getStamp());

                Handle(TColStd_HArray1OfReal) values = new TColStd_HArray1OfReal(0, dstep->countValues()-1);
                std::vector<double> dvalues = dstep->getValues();
                for (int k = 0; k < dstep->countValues(); ++k)
                {
                    values->SetValue(k, dvalues[k]);
                }
                step->SetValues(values);
            }
            break;
        }
        case XAO::STRING:
        {
            XAO::StringField* sfield = (XAO::StringField*)xaoField;
            for (int j = 0; j < xaoField->countSteps(); ++j)
            {
                XAO::StringStep* sstep = sfield->getStep(j);
                Handle(GEOM_FieldStep) step = field->AddStep(sstep->getStep(), sstep->getStamp());

                Handle(TColStd_HArray1OfExtendedString) values = new TColStd_HArray1OfExtendedString(0, sstep->countValues()-1);
                std::vector<std::string> svalues = sstep->getValues();
                for (int k = 0; k < sstep->countValues(); ++k)
                {
                    values->SetValue(k, TCollection_ExtendedString(svalues[k].c_str()));
                }
                step->SetValues(values);
            }
            break;
        }
    }

    fields->Append(field);
  }
  
  // make a Python command
  GEOM::TPythonDump pd(function);
  pd << "(imported, " << shape << ", ";
  
  // list of sub shapes
  pd << "[";
  int nbSubshapes = subShapes->Length();
  if (nbSubshapes > 0)
  {
    for (int i = 1; i <= nbSubshapes; i++)
    {
      Handle(GEOM_Object) obj = Handle(GEOM_Object)::DownCast(subShapes->Value(i));
      pd << obj << ((i < nbSubshapes) ? ", " : "");
    }
  }
  pd << "], [";
  
  // list of groups
  if (nbGroups > 0)
  {
    for (int i = 1; i <= nbGroups; i++)
    {
      Handle(GEOM_Object) obj = Handle(GEOM_Object)::DownCast(groups->Value(i));
      pd << obj << ((i < nbGroups) ? ", " : "");
    }
  }
  
  pd << "], [";
  
  // list of fields
  if (nbFields > 0)
  {
    for (int i = 1; i <= nbFields; i++)
    {
      Handle(GEOM_Field) obj = Handle(GEOM_Field)::DownCast(fields->Value(i));
      pd << obj << ((i < nbFields) ? ", " : "");
    }
  }
  pd << "]";
  pd << ") = geompy.ImportXAO(\"" << fileName << "\")";
  
  delete xaoObject;
  SetErrorCode(OK);
  
  return true;
}

//=============================================================================
/*!
 *  This method creates material groups for an imported object.
 *  \param theObject the imported object.
 */
//=============================================================================
void GEOMImpl_IInsertOperations::MakeMaterialGroups
                        (const Handle(GEOM_Object) &theObject,
                         const Handle(TColStd_HSequenceOfTransient) &theSeq)
{
  TopoDS_Shape aResShape = theObject->GetValue();

  if (aResShape.IsNull() == Standard_False) {
    // Group shapes by material names.
    Handle(GEOM_Function)      aFunction = theObject->GetLastFunction();
    DataMapOfStringListOfShape aMapMaterialShapes;

    // check all named shapes using iterator
    TDF_ChildIDIterator anIt (aFunction->GetNamingEntry(),
        TNaming_NamedShape::GetID(), Standard_True);

    for (; anIt.More(); anIt.Next()) {
      Handle(TNaming_NamedShape) anAttr =
          Handle(TNaming_NamedShape)::DownCast(anIt.Value());

      if (anAttr.IsNull() == Standard_False) {
        TDF_Label                aLabel = anAttr->Label();
        Handle(TDataStd_Comment) aComment;

        if (aLabel.FindAttribute(TDataStd_Comment::GetID(), aComment)) {
          TCollection_ExtendedString aMatName = aComment->Get();
          TopoDS_Shape               aShape   = anAttr->Get();

          if (aMapMaterialShapes.IsBound(aMatName) == Standard_False) {
            NCollection_List<TopoDS_Shape> anEmptyList;

            aMapMaterialShapes.Bind(aMatName, anEmptyList);
          }

          aMapMaterialShapes(aMatName).Append(aShape);
        }
      }
    }

    if (aMapMaterialShapes.IsEmpty() == Standard_False) {
      // Construct groups.
      TopAbs_ShapeEnum aType = aResShape.ShapeType();
      Standard_Integer i;
      DataMapOfStringListOfShape::Iterator aMapIter;

      // Check each shape type.
      for(i = aType; i <= TopAbs_VERTEX; i++) {
        DataMapOfStringListOfShape::Iterator aMapIter(aMapMaterialShapes);

        for (; aMapIter.More(); aMapIter.Next()) {
          NCollection_List<TopoDS_Shape> &aShList = aMapIter.ChangeValue();
          NCollection_List<TopoDS_Shape>::Iterator aShIter(aShList);
          NCollection_List<TopoDS_Shape>  aShListSameType;

          while (aShIter.More()) {
            const TopoDS_Shape &aShape = aShIter.Value();

            if (i == aShape.ShapeType()) {
              // Treat this element.
              aShListSameType.Append(aShape);
              aShList.Remove(aShIter);
            } else {
              // Go to the next element.
              aShIter.Next();
            }
          }

          if (aShListSameType.IsEmpty() == Standard_False) {
            // Construct a group.
            Handle(GEOM_Object) aGroup =
              MakeGroup(theObject, aMapIter.Key(), aShListSameType);

            if (aGroup.IsNull() == Standard_False) {
              theSeq->Append(aGroup);
            }
          }
        }
      }
    }
  }
}


//=============================================================================
/*!
 *  This method creates a group of shapes of certain type.
 *  \param theObject the imported object.
 *  \param theName the material name.
 *  \param theShapes the list of shapes to be added to this group.
 *  \return the created group.
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IInsertOperations::MakeGroup
                  (const Handle(GEOM_Object)            &theObject,
                   const TCollection_ExtendedString     &theName,
                   const NCollection_List<TopoDS_Shape> &theShapes)
{
  Handle(GEOM_Object)                aGroup;
  TopTools_IndexedMapOfShape         anIndices;
  Handle(TColStd_HSequenceOfInteger) aSeqIDs = new TColStd_HSequenceOfInteger;
  NCollection_List<TopoDS_Shape>::Iterator anIter(theShapes);

  TopExp::MapShapes(theObject->GetValue(), anIndices);

  // Compose shape IDs.
  for (; anIter.More(); anIter.Next()) {
    const TopoDS_Shape &aShape = anIter.Value();
    const Standard_Integer anIndex = anIndices.FindIndex(aShape);

    if (anIndex > 0) {
      aSeqIDs->Append(anIndex);
    }
  }

  if (aSeqIDs->IsEmpty() == Standard_False) {
    // Create a group.
    const TopAbs_ShapeEnum aType  = theShapes.First().ShapeType();

    aGroup = myGroupOperations->CreateGroup(theObject, aType);

    if (aGroup.IsNull() == Standard_False) {
      aGroup->GetLastFunction()->SetDescription("");
      myGroupOperations->UnionIDs(aGroup, aSeqIDs);
      aGroup->GetLastFunction()->SetDescription("");

      // Compose the group name.
      TCollection_AsciiString aGroupName(theName);

      switch(aType) {
        case TopAbs_VERTEX:
          aGroupName += "_VERTEX";
          break;
        case TopAbs_EDGE:
          aGroupName += "_EDGE";
          break;
        case TopAbs_WIRE:
          aGroupName += "_WIRE";
          break;
        case TopAbs_FACE:
          aGroupName += "_FACE";
          break;
        case TopAbs_SHELL:
          aGroupName += "_SHELL";
          break;
        case TopAbs_SOLID:
          aGroupName += "_SOLID";
          break;
        case TopAbs_COMPSOLID:
          aGroupName += "_COMPSOLID";
          break;
        case TopAbs_COMPOUND:
          aGroupName += "_COMPOUND";
          break;
        default:
          aGroupName += "_SHAPE";
          break;
      }

      aGroup->SetName(aGroupName.ToCString());
    }
  }

  return aGroup;
}
