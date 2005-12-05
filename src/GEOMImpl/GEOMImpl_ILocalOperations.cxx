// Copyright (C) 2005  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public  
// License along with this library; if not, write to the Free Software 
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/
//
#include <Standard_Stream.hxx>

#include <GEOMImpl_ILocalOperations.hxx>

#include <GEOM_Function.hxx>
#include <GEOM_PythonDump.hxx>

#include <GEOMImpl_Types.hxx>

#include <GEOMImpl_FilletDriver.hxx>
#include <GEOMImpl_ChamferDriver.hxx>

#include <GEOMImpl_IFillet.hxx>
#include <GEOMImpl_IChamfer.hxx>

#include <GEOMImpl_IArchimede.hxx>
#include <GEOMImpl_ArchimedeDriver.hxx>

#include "utilities.h"
#include <OpUtil.hxx>
#include <Utils_ExceptHandlers.hxx>

#include <TFunction_DriverTable.hxx>
#include <TFunction_Driver.hxx>
#include <TFunction_Logbook.hxx>
#include <TDF_Tool.hxx>

#include <TopExp.hxx>
#include <TopoDS_TShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#include <Standard_ErrorHandler.hxx> // CAREFUL ! position of this file is critic : see Lucien PIGNOLONI / OCC

//=============================================================================
/*!
 *   constructor:
 */
//=============================================================================
GEOMImpl_ILocalOperations::GEOMImpl_ILocalOperations (GEOM_Engine* theEngine, int theDocID)
: GEOM_IOperations(theEngine, theDocID)
{
  MESSAGE("GEOMImpl_ILocalOperations::GEOMImpl_ILocalOperations");
}

//=============================================================================
/*!
 *  destructor
 */
//=============================================================================
GEOMImpl_ILocalOperations::~GEOMImpl_ILocalOperations()
{
  MESSAGE("GEOMImpl_ILocalOperations::~GEOMImpl_ILocalOperations");
}


//=============================================================================
/*!
 *  MakeFilletAll
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_ILocalOperations::MakeFilletAll
                                    (Handle(GEOM_Object) theShape, double theR)
{
  SetErrorCode(KO);

  //Add a new Fillet object
  Handle(GEOM_Object) aFillet = GetEngine()->AddObject(GetDocID(), GEOM_FILLET);

  //Add a new Fillet function
  Handle(GEOM_Function) aFunction =
    aFillet->AddFunction(GEOMImpl_FilletDriver::GetID(), FILLET_SHAPE_ALL);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_FilletDriver::GetID()) return NULL;

  GEOMImpl_IFillet aCI (aFunction);

  Handle(GEOM_Function) aRefShape = theShape->GetLastFunction();
  if (aRefShape.IsNull()) return NULL;

  aCI.SetShape(aRefShape);
  aCI.SetR(theR);

  //Compute the Fillet value
  try {
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Fillet driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction) << aFillet << " = geompy.MakeFilletAll("
                               << theShape << ", " << theR << ")";

  SetErrorCode(OK);
  return aFillet;
}

//=============================================================================
/*!
 *  MakeFilletEdges
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_ILocalOperations::MakeFilletEdges
       (Handle(GEOM_Object) theShape, double theR, list<int> theEdges)
{
  SetErrorCode(KO);

  //Add a new Fillet object
  Handle(GEOM_Object) aFillet = GetEngine()->AddObject(GetDocID(), GEOM_FILLET);

  //Add a new Fillet function
  Handle(GEOM_Function) aFunction =
    aFillet->AddFunction(GEOMImpl_FilletDriver::GetID(), FILLET_SHAPE_EDGES);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_FilletDriver::GetID()) return NULL;

  GEOMImpl_IFillet aCI (aFunction);

  Handle(GEOM_Function) aRefShape = theShape->GetLastFunction();
  if (aRefShape.IsNull()) return NULL;

  aCI.SetShape(aRefShape);
  aCI.SetR(theR);
  int aLen = theEdges.size();
  aCI.SetLength(aLen);

  int ind = 1;
  list<int>::iterator it = theEdges.begin();
  for (; it != theEdges.end(); it++, ind++) {
    aCI.SetEdge(ind, (*it));
  }

  //Compute the Fillet value
  try {
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Fillet driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump pd (aFunction);
  pd << aFillet << " = geompy.MakeFillet(" << theShape
    << ", " << theR << ", geompy.ShapeType[\"EDGE\"], [";

  it = theEdges.begin();
  pd << (*it++);
  while (it != theEdges.end()) {
    pd << ", " << (*it++);
  }
  pd << "])";

  SetErrorCode(OK);
  return aFillet;
}

//=============================================================================
/*!
 *  MakeFilletFaces
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_ILocalOperations::MakeFilletFaces
       (Handle(GEOM_Object) theShape, double theR, list<int> theFaces)
{
  SetErrorCode(KO);

  //Add a new Fillet object
  Handle(GEOM_Object) aFillet = GetEngine()->AddObject(GetDocID(), GEOM_FILLET);

  //Add a new Fillet function
  Handle(GEOM_Function) aFunction =
    aFillet->AddFunction(GEOMImpl_FilletDriver::GetID(), FILLET_SHAPE_FACES);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_FilletDriver::GetID()) return NULL;

  GEOMImpl_IFillet aCI (aFunction);

  Handle(GEOM_Function) aRefShape = theShape->GetLastFunction();
  if (aRefShape.IsNull()) return NULL;

  aCI.SetShape(aRefShape);
  aCI.SetR(theR);
  int aLen = theFaces.size();
  aCI.SetLength(aLen);

  int ind = 1;
  list<int>::iterator it = theFaces.begin();
  for (; it != theFaces.end(); it++, ind++) {
    aCI.SetFace(ind, (*it));
  }

  //Compute the Fillet value
  try {
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Fillet driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump pd (aFunction);
  pd << aFillet << " = geompy.MakeFillet(" << theShape
    << ", " << theR << ", geompy.ShapeType[\"FACE\"], [";

  it = theFaces.begin();
  pd << (*it++);
  while (it != theFaces.end()) {
    pd << ", " << (*it++);
  }
  pd << "])";

  SetErrorCode(OK);
  return aFillet;
}

//=============================================================================
/*!
 *  MakeChamferAll
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_ILocalOperations::MakeChamferAll (Handle(GEOM_Object) theShape, double theD)
{
  SetErrorCode(KO);

  //Add a new Chamfer object
  Handle(GEOM_Object) aChamfer = GetEngine()->AddObject(GetDocID(), GEOM_CHAMFER);

  //Add a new Chamfer function
  Handle(GEOM_Function) aFunction =
    aChamfer->AddFunction(GEOMImpl_ChamferDriver::GetID(), CHAMFER_SHAPE_ALL);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_ChamferDriver::GetID()) return NULL;

  GEOMImpl_IChamfer aCI (aFunction);

  Handle(GEOM_Function) aRefShape = theShape->GetLastFunction();
  if (aRefShape.IsNull()) return NULL;

  aCI.SetShape(aRefShape);
  aCI.SetD(theD);

  //Compute the Chamfer value
  try {
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Chamfer driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction) << aChamfer << " = geompy.MakeChamferAll("
                               << theShape << ", " << theD << ")";

  SetErrorCode(OK);
  return aChamfer;
}

//=============================================================================
/*!
 *  MakeChamferEdge
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_ILocalOperations::MakeChamferEdge
                            (Handle(GEOM_Object) theShape, double theD1, double theD2,
                             int theFace1, int theFace2)
{
  SetErrorCode(KO);

  //Add a new Chamfer object
  Handle(GEOM_Object) aChamfer = GetEngine()->AddObject(GetDocID(), GEOM_CHAMFER);

  //Add a new Chamfer function
  Handle(GEOM_Function) aFunction =
    aChamfer->AddFunction(GEOMImpl_ChamferDriver::GetID(), CHAMFER_SHAPE_EDGE);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_ChamferDriver::GetID()) return NULL;

  GEOMImpl_IChamfer aCI (aFunction);

  Handle(GEOM_Function) aRefShape = theShape->GetLastFunction();
  if (aRefShape.IsNull()) return NULL;

  aCI.SetShape(aRefShape);
  aCI.SetD1(theD1);
  aCI.SetD2(theD2);
  aCI.SetFace1(theFace1);
  aCI.SetFace2(theFace2);

  //Compute the Chamfer value
  try {
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Chamfer driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction) << aChamfer
    << " = geompy.MakeChamferEdge(" << theShape << ", " << theD1
      << ", " << theD2 << ", " << theFace1 << ", " << theFace2 << ")";

  SetErrorCode(OK);
  return aChamfer;
}

//=============================================================================
/*!
 *  MakeChamferFaces
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_ILocalOperations::MakeChamferFaces
                            (Handle(GEOM_Object) theShape, double theD1, double theD2,
                             list<int> theFaces)
{
  SetErrorCode(KO);

  //Add a new Chamfer object
  Handle(GEOM_Object) aChamfer = GetEngine()->AddObject(GetDocID(), GEOM_CHAMFER);

  //Add a new Chamfer function
  Handle(GEOM_Function) aFunction =
    aChamfer->AddFunction(GEOMImpl_ChamferDriver::GetID(), CHAMFER_SHAPE_FACES);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_ChamferDriver::GetID()) return NULL;

  GEOMImpl_IChamfer aCI (aFunction);

  Handle(GEOM_Function) aRefShape = theShape->GetLastFunction();
  if (aRefShape.IsNull()) return NULL;

  aCI.SetShape(aRefShape);
  aCI.SetD1(theD1);
  aCI.SetD2(theD2);
  int aLen = theFaces.size();
  aCI.SetLength(aLen);

  int ind = 1;
  list<int>::iterator it = theFaces.begin();
  for (; it != theFaces.end(); it++, ind++) {
    aCI.SetFace(ind, (*it));
  }

  //Compute the Chamfer value
  try {
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Chamfer driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump pd (aFunction);
  pd << aChamfer << " = geompy.MakeChamferFaces(" << theShape
    << ", " << theD1 << ", " << theD2 << ", [";

  it = theFaces.begin();
  pd << (*it++);
  while (it != theFaces.end()) {
    pd << ", " << (*it++);
  }
  pd << "])";

  SetErrorCode(OK);
  return aChamfer;
}

//=============================================================================
/*!
 *  Archimede
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_ILocalOperations::MakeArchimede (Handle(GEOM_Object) theShape,
                                                              double theWeight,
                                                              double theWaterDensity,
                                                              double theMeshingDeflection)
{
  SetErrorCode(KO);

  //Add a new Archimede object
  Handle(GEOM_Object) aChamfer = GetEngine()->AddObject(GetDocID(), GEOM_ARCHIMEDE);

  //Add a new Archimede function
  Handle(GEOM_Function) aFunction = aChamfer->AddFunction(GEOMImpl_ArchimedeDriver::GetID(), ARCHIMEDE_TYPE);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_ArchimedeDriver::GetID()) return NULL;

  GEOMImpl_IArchimede aAI (aFunction);

  Handle(GEOM_Function) aRefShape = theShape->GetLastFunction();
  if (aRefShape.IsNull()) return NULL;

  aAI.SetBasicShape(aRefShape);
  aAI.SetWeight(theWeight);
  aAI.SetDensity(theWaterDensity);
  aAI.SetDeflection(theMeshingDeflection);

  //Compute the Archimede value
  try {
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Archimede driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction) << aChamfer
    << " = geompy.Archimede(" << theShape << ", " << theWeight << ", "
      << theWaterDensity << ", " << theMeshingDeflection << ")";

  SetErrorCode(OK);
  return aChamfer;
}

//=============================================================================
/*!
 *  GetSubShape
 */
//=============================================================================
bool GEOMImpl_ILocalOperations::GetSubShape (const TopoDS_Shape& theShape, const int theIndex,
                                             TopoDS_Shape& theSubShape)
{
  if (theShape.IsNull() || theIndex < 1)
    return false;

  TopTools_IndexedMapOfShape anIndices;
  TopExp::MapShapes(theShape, anIndices);
  if (theIndex > anIndices.Extent()) return false;
  theSubShape = anIndices.FindKey(theIndex);

  return true;
}

//=============================================================================
/*!
 *  GetSubShapeIndex
 */
//=============================================================================
Standard_Integer GEOMImpl_ILocalOperations::GetSubShapeIndex (Handle(GEOM_Object) theShape,
                                                              Handle(GEOM_Object) theSubShape)
{
  SetErrorCode(KO);

  TopoDS_Shape aShape = theShape->GetValue();
  TopoDS_Shape aSubShape = theSubShape->GetValue();

  if (aShape.IsNull() || aSubShape.IsNull()) return -1;

  TopTools_IndexedMapOfShape anIndices;
  TopExp::MapShapes(aShape, anIndices);
  if (anIndices.Contains(aSubShape)) {
    SetErrorCode(OK);
    return anIndices.FindIndex(aSubShape);
  }

  return -1;
}
