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

#include <Standard_Stream.hxx>

#include <GEOMImpl_IInsertOperations.hxx>

#include <GEOMImpl_CopyDriver.hxx>
#include <GEOMImpl_ExportDriver.hxx>
#include <GEOMImpl_ImportDriver.hxx>
#include <GEOMImpl_ICopy.hxx>
#include <GEOMImpl_IImportExport.hxx>
#include <GEOMImpl_ITransferData.hxx>
#include <GEOMImpl_Types.hxx>
#include "GEOMImpl_IShapesOperations.hxx"
#include "GEOMImpl_IGroupOperations.hxx"
#include "GEOMImpl_IFieldOperations.hxx"
#include "GEOMImpl_IECallBack.hxx"

#include <GEOM_Function.hxx>
#include <GEOM_PythonDump.hxx>
#include "GEOM_ISubShape.hxx"

#include <Basics_OCCTVersion.hxx>

#include "utilities.h"
#include <OpUtil.hxx>
#include <Utils_ExceptHandlers.hxx>

#include <TFunction_DriverTable.hxx>
#include <TFunction_Driver.hxx>
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

#include <TColStd_HArray1OfReal.hxx>

#include <Standard_Failure.hxx>
#include <Standard_ErrorHandler.hxx> // CAREFUL ! position of this file is critic : see Lucien PIGNOLONI / OCC

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
    OCC_CATCH_SIGNALS;
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

  if ( !GEOMImpl_IECallBack::GetCallBack( theFormatName )->Export( GetDocID(), theOriginal, theFileName, theFormatName ) )
    return;
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

  Handle(TColStd_HSequenceOfTransient) aSeq =
    GEOMImpl_IECallBack::GetCallBack( theFormatName )->Import( GetDocID(), theFormatName, theFileName );
  SetErrorCode(OK);
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
  TCollection_AsciiString aValue;
  if (theFileName.IsEmpty() || theFormatName.IsEmpty() || theParameterName.IsEmpty()) return aValue;

  aValue = GEOMImpl_IECallBack::GetCallBack( theFormatName )->ReadValue( GetDocID(), theFileName, theFormatName, theParameterName );

  SetErrorCode(OK);
  return aValue;
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

  Handle(TColStd_HArray1OfByte) aTexture;

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

  aTexture = new TColStd_HArray1OfByte (1, lines.size()*lenbytes);

  std::list<unsigned char>::iterator bdit;
  int i;
  for (i = 1, bdit = bytedata.begin(); bdit != bytedata.end(); ++bdit, ++i)
    aTexture->SetValue(i, (Standard_Byte)(*bdit));

  int aTextureId = GetEngine()->addTexture(GetDocID(), lenbytes*8, lines.size(), aTexture, theTextureFile);
  if (aTextureId > 0) SetErrorCode(OK);
  return aTextureId;
}
  
int GEOMImpl_IInsertOperations::AddTexture(int theWidth, int theHeight, 
                                           const Handle(TColStd_HArray1OfByte)& theTexture)
{
  SetErrorCode(KO);
  int aTextureId = GetEngine()->addTexture(GetDocID(), theWidth, theHeight, theTexture);
  if (aTextureId > 0) SetErrorCode(OK);
  return aTextureId;
}

Handle(TColStd_HArray1OfByte) GEOMImpl_IInsertOperations::GetTexture(int theTextureId,
                                                                     int& theWidth, int& theHeight)
{
  SetErrorCode(KO);
  
  Handle(TColStd_HArray1OfByte) aTexture;

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

//=============================================================================
/*!
 *  TransferData
 */
//=============================================================================
bool GEOMImpl_IInsertOperations::TransferData
                          (const Handle(GEOM_Object)      &theObjectFrom,
                           const Handle(GEOM_Object)      &theObjectTo,
                           const int                       theFindMethod,
                                 std::list<TransferDatum> &theResult)
{
  SetErrorCode(KO);

  if (theObjectFrom.IsNull() || theObjectTo.IsNull()) {
    return false;
  }

  //Add a new Transfer Data object object
  Handle(GEOM_Object) aTDObj =
    GetEngine()->AddObject(GetDocID(), GEOM_TRANSFER_DATA);

  //Add a Transfer Data function for created object
  Handle(GEOM_Function) aFunction =
    aTDObj->AddFunction(GEOMImpl_CopyDriver::GetID(), TRANSFER_DATA);

  //Check if the function is set correctly
  if(aFunction->GetDriverGUID() != GEOMImpl_CopyDriver::GetID()) {
    return false;
  }

  Handle(GEOM_Function) aFunctionFrom = theObjectFrom->GetLastFunction();
  Handle(GEOM_Function) aFunctionTo   = theObjectTo->GetLastFunction();

  if (aFunctionFrom.IsNull() || aFunctionTo.IsNull()) {
    return false;
  }

  GEOMImpl_ITransferData aTD(aFunction);

  aTD.SetRef1(aFunctionFrom);
  aTD.SetRef2(aFunctionTo);
  aTD.SetFindMethod(theFindMethod);

  // Transfer data
  try {
    OCC_CATCH_SIGNALS;
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Transfer data failed");
      return false;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return false;
  }

  // Fill result list of data.
  theResult.clear();

  Handle(TColStd_HArray1OfExtendedString) aDatumName   = aTD.GetDatumName();
  Handle(TColStd_HArray1OfInteger)        aDatumMaxVal = aTD.GetDatumMaxVal();
  Handle(TColStd_HArray1OfInteger)        aDatumVal    = aTD.GetDatumVal();

  if (!aDatumName.IsNull() && !aDatumMaxVal.IsNull() && !aDatumVal.IsNull()) {
    Standard_Integer i;
    Standard_Integer aNbDatum = aDatumName->Length();

    for (i = 1; i <= aNbDatum; ++i) {
      if (aDatumMaxVal->Value(i) > 0) {
        TransferDatum aDatum;

        aDatum.myName      = TCollection_AsciiString(aDatumName->Value(i));
        aDatum.myNumber    = aDatumVal->Value(i);
        aDatum.myMaxNumber = aDatumMaxVal->Value(i);
        theResult.push_back(aDatum);
      }
    }
  }

  //Make a Python command
  GEOM::TPythonDump pd (aFunction);
  pd << "geompy.TransferData(" << theObjectFrom << ", " << theObjectTo;
  pd << ", GEOM.";

  switch (theFindMethod) {
  case TD_GET_IN_PLACE:
    pd << "FSM_GetInPlace";
    break;
  case TD_GET_IN_PLACE_OLD:
    pd << "FSM_GetInPlace_Old";
    break;
  case TD_GET_IN_PLACE_BY_HISTORY:
  default:
    pd << "FSM_GetInPlaceByHistory";
    break;
  }
  pd << ")";

  SetErrorCode(OK);

  return true;
}
