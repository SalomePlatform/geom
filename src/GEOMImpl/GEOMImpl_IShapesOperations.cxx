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

#include "GEOMImpl_IShapesOperations.hxx"

#include "GEOMImpl_Types.hxx"

#include "GEOMImpl_VectorDriver.hxx"
#include "GEOMImpl_ShapeDriver.hxx"
#include "GEOMImpl_CopyDriver.hxx"
#include "GEOMImpl_GlueDriver.hxx"

#include "GEOMImpl_IVector.hxx"
#include "GEOMImpl_IShapes.hxx"
#include "GEOMImpl_IGlue.hxx"

#include "GEOMImpl_Block6Explorer.hxx"

#include "GEOM_Function.hxx"
#include "GEOM_PythonDump.hxx"

#include "GEOMAlgo_FinderShapeOn1.hxx"
#include "GEOMAlgo_FinderShapeOnQuad.hxx"

#include "utilities.h"
#include "OpUtil.hxx"
#include "Utils_ExceptHandlers.hxx"

#include <TFunction_DriverTable.hxx>
#include <TFunction_Driver.hxx>
#include <TFunction_Logbook.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_IntegerArray.hxx>
#include <TDF_Tool.hxx>

#include <BRepExtrema_ExtCF.hxx>

#include <BRep_Tool.hxx>
#include <BRepGProp.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepBndLib.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepMesh_IncrementalMesh.hxx>

#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_Array1OfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#include <Geom_Surface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <GeomAdaptor_Surface.hxx>

#include <Geom2d_Curve.hxx>

#include <Bnd_Box.hxx>
#include <GProp_GProps.hxx>
#include <gp_Pnt.hxx>
#include <gp_Lin.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HArray1OfInteger.hxx>

#include <vector>

#include <Standard_ErrorHandler.hxx> // CAREFUL ! position of this file is critic : see Lucien PIGNOLONI / OCC

//=============================================================================
/*!
 *   constructor:
 */
//=============================================================================
GEOMImpl_IShapesOperations::GEOMImpl_IShapesOperations (GEOM_Engine* theEngine, int theDocID)
: GEOM_IOperations(theEngine, theDocID)
{
  MESSAGE("GEOMImpl_IShapesOperations::GEOMImpl_IShapesOperations");
}

//=============================================================================
/*!
 *  destructor
 */
//=============================================================================
GEOMImpl_IShapesOperations::~GEOMImpl_IShapesOperations()
{
  MESSAGE("GEOMImpl_IShapesOperations::~GEOMImpl_IShapesOperations");
}


//=============================================================================
/*!
 *  MakeEdge
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IShapesOperations::MakeEdge
                     (Handle(GEOM_Object) thePnt1, Handle(GEOM_Object) thePnt2)
{
  SetErrorCode(KO);

  if (thePnt1.IsNull() || thePnt2.IsNull()) return NULL;

  //Add a new Edge object
  Handle(GEOM_Object) anEdge = GetEngine()->AddObject(GetDocID(), GEOM_EDGE);

  //Add a new Vector function
  Handle(GEOM_Function) aFunction =
    anEdge->AddFunction(GEOMImpl_VectorDriver::GetID(), VECTOR_TWO_PNT);

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_VectorDriver::GetID()) return NULL;

  GEOMImpl_IVector aPI (aFunction);

  Handle(GEOM_Function) aRef1 = thePnt1->GetLastFunction();
  Handle(GEOM_Function) aRef2 = thePnt2->GetLastFunction();
  if (aRef1.IsNull() || aRef2.IsNull()) return NULL;

  aPI.SetPoint1(aRef1);
  aPI.SetPoint2(aRef2);

  //Compute the Edge value
  try {
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Vector driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction) << anEdge << " = geompy.MakeEdge("
                               << thePnt1 << ", " << thePnt2 << ")";

  SetErrorCode(OK);
  return anEdge;
}

//=============================================================================
/*!
 *  MakeWire
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IShapesOperations::MakeWire
                             (list<Handle(GEOM_Object)> theShapes)
{
  return MakeShape(theShapes, GEOM_WIRE, WIRE_EDGES, "MakeWire");
}

//=============================================================================
/*!
 *  MakeFace
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IShapesOperations::MakeFace (Handle(GEOM_Object) theWire,
                                                          const bool isPlanarWanted)
{
  SetErrorCode(KO);

  if (theWire.IsNull()) return NULL;

  //Add a new Face object
  Handle(GEOM_Object) aFace = GetEngine()->AddObject(GetDocID(), GEOM_FACE);

  //Add a new Shape function for creation of a face from a wire
  Handle(GEOM_Function) aFunction =
    aFace->AddFunction(GEOMImpl_ShapeDriver::GetID(), FACE_WIRE);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_ShapeDriver::GetID()) return NULL;

  GEOMImpl_IShapes aCI (aFunction);

  Handle(GEOM_Function) aRefWire = theWire->GetLastFunction();

  if (aRefWire.IsNull()) return NULL;

  aCI.SetBase(aRefWire);
  aCI.SetIsPlanar(isPlanarWanted);

  //Compute the Face value
  try {
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Shape driver failed to compute a face");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction) << aFace << " = geompy.MakeFace("
    << theWire << ", " << (int)isPlanarWanted << ")";

  SetErrorCode(OK);
  return aFace;
}

//=============================================================================
/*!
 *  MakeFaceWires
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IShapesOperations::MakeFaceWires
                             (list<Handle(GEOM_Object)> theShapes,
                              const bool isPlanarWanted)
{
  SetErrorCode(KO);

  //Add a new object
  Handle(GEOM_Object) aShape = GetEngine()->AddObject(GetDocID(), GEOM_FACE);

  //Add a new function
  Handle(GEOM_Function) aFunction =
    aShape->AddFunction(GEOMImpl_ShapeDriver::GetID(), FACE_WIRES);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_ShapeDriver::GetID()) return NULL;

  GEOMImpl_IShapes aCI (aFunction);

  Handle(TColStd_HSequenceOfTransient) aShapesSeq = new TColStd_HSequenceOfTransient;

  // Shapes
  list<Handle(GEOM_Object)>::iterator it = theShapes.begin();
  for (; it != theShapes.end(); it++) {
    Handle(GEOM_Function) aRefSh = (*it)->GetLastFunction();
    if (aRefSh.IsNull()) {
      SetErrorCode("NULL argument shape for the face construction");
      return NULL;
    }
    aShapesSeq->Append(aRefSh);
  }
  aCI.SetShapes(aShapesSeq);

  aCI.SetIsPlanar(isPlanarWanted);

  //Compute the shape
  try {
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Shape driver failed");
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
  pd << aShape << " = geompy.MakeFaceWires([";

  // Shapes
  it = theShapes.begin();
  if (it != theShapes.end()) {
    pd << (*it++);
    while (it != theShapes.end()) {
      pd << ", " << (*it++);
    }
  }
  pd << "], " << (int)isPlanarWanted << ")";

  SetErrorCode(OK);
  return aShape;
}

//=============================================================================
/*!
 *  MakeShell
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IShapesOperations::MakeShell
                             (list<Handle(GEOM_Object)> theShapes)
{
  return MakeShape(theShapes, GEOM_SHELL, SHELL_FACES, "MakeShell");
}

//=============================================================================
/*!
 *  MakeSolidShells
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IShapesOperations::MakeSolidShells
                             (list<Handle(GEOM_Object)> theShapes)
{
  return MakeShape(theShapes, GEOM_SOLID, SOLID_SHELLS, "MakeSolid");
}

//=============================================================================
/*!
 *  MakeSolidShell
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IShapesOperations::MakeSolidShell (Handle(GEOM_Object) theShell)
{
  SetErrorCode(KO);

  if (theShell.IsNull()) return NULL;

  //Add a new Solid object
  Handle(GEOM_Object) aSolid = GetEngine()->AddObject(GetDocID(), GEOM_SOLID);

  //Add a new Solid function for creation of a solid from a shell
  Handle(GEOM_Function) aFunction =
    aSolid->AddFunction(GEOMImpl_ShapeDriver::GetID(), SOLID_SHELL);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_ShapeDriver::GetID()) return NULL;

  GEOMImpl_IShapes aCI (aFunction);

  Handle(GEOM_Function) aRefShell = theShell->GetLastFunction();

  if (aRefShell.IsNull()) return NULL;

  aCI.SetBase(aRefShell);

  //Compute the Solid value
  try {
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Solid driver failed");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction) << aSolid
    << " = geompy.MakeSolid(" << theShell << ")";

  SetErrorCode(OK);
  return aSolid;
}

//=============================================================================
/*!
 *  MakeCompound
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IShapesOperations::MakeCompound
                             (list<Handle(GEOM_Object)> theShapes)
{
  return MakeShape(theShapes, GEOM_COMPOUND, COMPOUND_SHAPES, "MakeCompound");
}

//=============================================================================
/*!
 *  MakeShape
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IShapesOperations::MakeShape
                             (list<Handle(GEOM_Object)>      theShapes,
                              const Standard_Integer         theObjectType,
                              const Standard_Integer         theFunctionType,
                              const TCollection_AsciiString& theMethodName)
{
  SetErrorCode(KO);

  //Add a new object
  Handle(GEOM_Object) aShape = GetEngine()->AddObject(GetDocID(), theObjectType);

  //Add a new function
  Handle(GEOM_Function) aFunction =
    aShape->AddFunction(GEOMImpl_ShapeDriver::GetID(), theFunctionType);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_ShapeDriver::GetID()) return NULL;

  GEOMImpl_IShapes aCI (aFunction);

  Handle(TColStd_HSequenceOfTransient) aShapesSeq = new TColStd_HSequenceOfTransient;

  // Shapes
  list<Handle(GEOM_Object)>::iterator it = theShapes.begin();
  for (; it != theShapes.end(); it++) {
    Handle(GEOM_Function) aRefSh = (*it)->GetLastFunction();
    if (aRefSh.IsNull()) {
      SetErrorCode("NULL argument shape for the shape construction");
      return NULL;
    }
    aShapesSeq->Append(aRefSh);
  }
  aCI.SetShapes(aShapesSeq);

  //Compute the shape
  try {
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Shape driver failed");
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
  pd << aShape << " = geompy." << theMethodName.ToCString() << "([";

  // Shapes
  it = theShapes.begin();
  if (it != theShapes.end()) {
    pd << (*it++);
    while (it != theShapes.end()) {
      pd << ", " << (*it++);
    }
  }
  pd << "])";

  SetErrorCode(OK);
  return aShape;
}

//=============================================================================
/*!
 *  MakeGlueFaces
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IShapesOperations::MakeGlueFaces
                                                (Handle(GEOM_Object) theShape,
                                                 const Standard_Real theTolerance)
{
  SetErrorCode(KO);

  if (theShape.IsNull()) return NULL;

  //Add a new Glued object
  Handle(GEOM_Object) aGlued = GetEngine()->AddObject(GetDocID(), GEOM_GLUED);

  //Add a new Glue function
  Handle(GEOM_Function) aFunction;
  aFunction = aGlued->AddFunction(GEOMImpl_GlueDriver::GetID(), GLUE_FACES);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_GlueDriver::GetID()) return NULL;

  GEOMImpl_IGlue aCI (aFunction);

  Handle(GEOM_Function) aRefShape = theShape->GetLastFunction();
  if (aRefShape.IsNull()) return NULL;

  aCI.SetBase(aRefShape);
  aCI.SetTolerance(theTolerance);

  //Compute the sub-shape value
  Standard_Boolean isWarning = Standard_False;
  try {
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Shape driver failed to glue faces");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    // to provide warning
    if (!aFunction->GetValue().IsNull()) {
      isWarning = Standard_True;
    } else {
      return NULL;
    }
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction) << aGlued << " = geompy.MakeGlueFaces("
    << theShape << ", " << theTolerance << ")";

  // to provide warning
  if (!isWarning) SetErrorCode(OK);
  return aGlued;
}

//=============================================================================
/*!
 *  MakeExplode
 */
//=============================================================================
Handle(TColStd_HSequenceOfTransient) GEOMImpl_IShapesOperations::MakeExplode
                                          (Handle(GEOM_Object)    theShape,
                                           const Standard_Integer theShapeType,
                                           const Standard_Boolean isSorted)
{
  SetErrorCode(KO);

  if (theShape.IsNull()) return NULL;
  TopoDS_Shape aShape = theShape->GetValue();
  if (aShape.IsNull()) return NULL;

  Handle(TColStd_HSequenceOfTransient) aSeq = new TColStd_HSequenceOfTransient;
  Handle(GEOM_Object) anObj;
  Handle(GEOM_Function) aFunction;
  TopTools_MapOfShape mapShape;
  TopTools_ListOfShape listShape;

  if (aShape.ShapeType() == TopAbs_COMPOUND &&
      (TopAbs_ShapeEnum(theShapeType) == TopAbs_SHAPE ||
       TopAbs_ShapeEnum(theShapeType) == TopAbs_COMPSOLID ||
       TopAbs_ShapeEnum(theShapeType) == TopAbs_COMPOUND)) {
    TopoDS_Iterator It (aShape, Standard_True, Standard_True);
    for (; It.More(); It.Next()) {
      if (mapShape.Add(It.Value())) {
        if (TopAbs_ShapeEnum(theShapeType) == TopAbs_SHAPE ||
            TopAbs_ShapeEnum(theShapeType) == It.Value().ShapeType()) {
          listShape.Append(It.Value());
        }
      }
    }
  } else {
    TopExp_Explorer exp (aShape, TopAbs_ShapeEnum(theShapeType));
    for (; exp.More(); exp.Next())
      if (mapShape.Add(exp.Current()))
	listShape.Append(exp.Current());
  }

  if (listShape.IsEmpty()) {
    SetErrorCode("The given shape has no sub-shapes of the requested type");
    return aSeq;
  }

  if (isSorted)
    SortShapes(listShape);

  TopTools_IndexedMapOfShape anIndices;
  TopExp::MapShapes(aShape, anIndices);
  Handle(TColStd_HArray1OfInteger) anArray;

  TopTools_ListIteratorOfListOfShape itSub (listShape);
  TCollection_AsciiString anAsciiList, anEntry;
  for (int index = 1; itSub.More(); itSub.Next(), ++index) {
    TopoDS_Shape aValue = itSub.Value();
    anArray = new TColStd_HArray1OfInteger(1,1);
    anArray->SetValue(1, anIndices.FindIndex(aValue));
    anObj = GetEngine()->AddSubShape(theShape, anArray);
    aSeq->Append(anObj);

    // for python command
    TDF_Tool::Entry(anObj->GetEntry(), anEntry);
    anAsciiList += anEntry;
    anAsciiList += ",";
  }

  //Make a Python command
  anAsciiList.Trunc(anAsciiList.Length() - 1);

  aFunction = theShape->GetLastFunction();

  GEOM::TPythonDump pd (aFunction, /*append=*/true);
  pd << "[" << anAsciiList.ToCString();
  pd << "] = geompy.SubShapeAll" << (isSorted ? "Sorted(" : "(");
  pd << theShape << ", " << TopAbs_ShapeEnum(theShapeType) << ")";

  SetErrorCode(OK);

  return aSeq;
}

//=============================================================================
/*!
 *  GetSubShapeAllIDs
 */
//=============================================================================
Handle(TColStd_HSequenceOfInteger) GEOMImpl_IShapesOperations::SubShapeAllIDs
                                          (Handle(GEOM_Object)    theShape,
                                           const Standard_Integer theShapeType,
                                           const Standard_Boolean isSorted)
{
  SetErrorCode(KO);

  if (theShape.IsNull()) return NULL;
  TopoDS_Shape aShape = theShape->GetValue();
  if (aShape.IsNull()) return NULL;

  Handle(TColStd_HSequenceOfInteger) aSeq = new TColStd_HSequenceOfInteger;
  TopTools_MapOfShape mapShape;
  TopTools_ListOfShape listShape;

  if (aShape.ShapeType() == TopAbs_COMPOUND &&
      (TopAbs_ShapeEnum(theShapeType) == TopAbs_SHAPE ||
       TopAbs_ShapeEnum(theShapeType) == TopAbs_COMPSOLID ||
       TopAbs_ShapeEnum(theShapeType) == TopAbs_COMPOUND)) {
    TopoDS_Iterator It (aShape, Standard_True, Standard_True);
    for (; It.More(); It.Next()) {
      if (mapShape.Add(It.Value())) {
        if (TopAbs_ShapeEnum(theShapeType) == TopAbs_SHAPE ||
            TopAbs_ShapeEnum(theShapeType) == It.Value().ShapeType()) {
          listShape.Append(It.Value());
        }
      }
    }
  } else {
    TopExp_Explorer exp (aShape, TopAbs_ShapeEnum(theShapeType));
    for (; exp.More(); exp.Next())
      if (mapShape.Add(exp.Current()))
	listShape.Append(exp.Current());
  }

  if (listShape.IsEmpty()) {
    SetErrorCode("The given shape has no sub-shapes of the requested type");
    return aSeq;
  }

  if (isSorted)
    SortShapes(listShape);

  TopTools_IndexedMapOfShape anIndices;
  TopExp::MapShapes(aShape, anIndices);
  Handle(TColStd_HArray1OfInteger) anArray;

  TopTools_ListIteratorOfListOfShape itSub (listShape);
  for (int index = 1; itSub.More(); itSub.Next(), ++index) {
    TopoDS_Shape aValue = itSub.Value();
    aSeq->Append(anIndices.FindIndex(aValue));
  }

  Handle(GEOM_Function) aFunction = theShape->GetLastFunction();

  //Make a Python command
  GEOM::TPythonDump pd (aFunction, /*append=*/true);
  pd << "listSubShapeIDs = geompy.SubShapeAll";
  pd << (isSorted ? "SortedIDs(" : "IDs(");
  pd << theShape << ", " << TopAbs_ShapeEnum(theShapeType) << ")";

  SetErrorCode(OK);
  return aSeq;
}

//=============================================================================
/*!
 *  GetSubShape
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IShapesOperations::GetSubShape
                                          (Handle(GEOM_Object)    theMainShape,
                                           const Standard_Integer theID)
{
  SetErrorCode(KO);

  if (theMainShape.IsNull()) return NULL;

  Handle(TColStd_HArray1OfInteger) anArray = new TColStd_HArray1OfInteger(1,1);
  anArray->SetValue(1, theID);
  Handle(GEOM_Object) anObj = GetEngine()->AddSubShape(theMainShape, anArray,true);
  if (anObj.IsNull()) {
    SetErrorCode("Can not get a sub-shape with the given ID");
    return NULL;
  }

  Handle(GEOM_Function) aFunction = anObj->GetLastFunction();

  //Make a Python command
  GEOM::TPythonDump(aFunction) << anObj << " = geompy.GetSubShape("
                               << theMainShape << ", [" << theID << "])";

  SetErrorCode(OK);
  return anObj;
}


//=============================================================================
/*!
 *  NumberOfFaces
 */
//=============================================================================
Standard_Integer GEOMImpl_IShapesOperations::NumberOfFaces (Handle(GEOM_Object) theShape)
{
  SetErrorCode(KO);

  Standard_Integer nb = 0;

  if (theShape.IsNull()) return -1;
  TopoDS_Shape aShape = theShape->GetValue();
  if (aShape.IsNull()) return -1;

  TopTools_MapOfShape mapShape;

  TopExp_Explorer exp (aShape, TopAbs_FACE);
  for (; exp.More(); exp.Next())
    if (mapShape.Add(exp.Current()))
      nb++;

  SetErrorCode(OK);
  return nb;
}

//=============================================================================
/*!
 *  NumberOfEdges
 */
//=============================================================================
Standard_Integer GEOMImpl_IShapesOperations::NumberOfEdges (Handle(GEOM_Object) theShape)
{
  SetErrorCode(KO);

  Standard_Integer nb = 0;

  if (theShape.IsNull()) return -1;
  TopoDS_Shape aShape = theShape->GetValue();
  if (aShape.IsNull()) return -1;

  TopTools_MapOfShape mapShape;

  TopExp_Explorer exp (aShape, TopAbs_EDGE);
  for (; exp.More(); exp.Next())
    if (mapShape.Add(exp.Current()))
      nb++;

  SetErrorCode(OK);
  return nb;
}

//=============================================================================
/*!
 *  ReverseShape
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IShapesOperations::ReverseShape(Handle(GEOM_Object) theShape)
{
  SetErrorCode(KO);

  if (theShape.IsNull()) return NULL;

  //Add a new reversed object
  Handle(GEOM_Object) aReversed = GetEngine()->AddObject(GetDocID(), theShape->GetType());

  //Add a new Revese function
  Handle(GEOM_Function) aFunction;
  aFunction = aReversed->AddFunction(GEOMImpl_ShapeDriver::GetID(), REVERSE_ORIENTATION);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_ShapeDriver::GetID()) return NULL;

  GEOMImpl_IShapes aSI (aFunction);

  Handle(GEOM_Function) aRefShape = theShape->GetLastFunction();
  if (aRefShape.IsNull()) return NULL;

  aSI.SetBase(aRefShape);

  //Compute the sub-shape value
  try {
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Shape driver failed to reverse shape");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction) << aReversed
    << " = geompy.ChangeOrientation(" << theShape << ")";

  SetErrorCode(OK);
  return aReversed;
}

//=============================================================================
/*!
 *  GetFreeFacesIDs
 */
//=============================================================================
Handle(TColStd_HSequenceOfInteger) GEOMImpl_IShapesOperations::GetFreeFacesIDs
                                                 (Handle(GEOM_Object) theShape)
{
  SetErrorCode(KO);

  if (theShape.IsNull()) return NULL;
  TopoDS_Shape aShape = theShape->GetValue();
  if (aShape.IsNull()) return NULL;

  Handle(TColStd_HSequenceOfInteger) aSeq = new TColStd_HSequenceOfInteger;

  TopTools_IndexedDataMapOfShapeListOfShape mapFaceBlocks;
  GEOMImpl_Block6Explorer::MapShapesAndAncestors
    (aShape, TopAbs_FACE, TopAbs_SOLID, mapFaceBlocks);

  Standard_Integer ind = 1, nbFaces = mapFaceBlocks.Extent();

  if (nbFaces == 0) {
    SetErrorCode("The given shape has no faces");
    return aSeq;
  }

  TopTools_IndexedMapOfShape anIndices;
  TopExp::MapShapes(aShape, anIndices);

  Standard_Integer id;
  for (; ind <= nbFaces; ind++) {
    if (mapFaceBlocks.FindFromIndex(ind).Extent() != 2) {
      id = anIndices.FindIndex(mapFaceBlocks.FindKey(ind));
      aSeq->Append(id);
    }
  }

  //The explode doesn't change object so no new function is required.
  Handle(GEOM_Function) aFunction = theShape->GetLastFunction();

  //Make a Python command
  GEOM::TPythonDump(aFunction, /*append=*/true)
    << "listFreeFacesIDs = geompy.GetFreeFacesIDs(" << theShape << ")";

  SetErrorCode(OK);
  return aSeq;
}

//=======================================================================
//function : GetSharedShapes
//purpose  : 
//=======================================================================

Handle(TColStd_HSequenceOfTransient) GEOMImpl_IShapesOperations::GetSharedShapes
                                                (Handle(GEOM_Object)    theShape1,
                                                 Handle(GEOM_Object)    theShape2,
                                                 const Standard_Integer theShapeType)
{
  SetErrorCode(KO);

  if (theShape1.IsNull() || theShape2.IsNull()) return NULL;

  TopoDS_Shape aShape1 = theShape1->GetValue();
  TopoDS_Shape aShape2 = theShape2->GetValue();

  if (aShape1.IsNull() || aShape2.IsNull()) return NULL;

  TopTools_IndexedMapOfShape anIndices;
  TopExp::MapShapes(aShape1, anIndices);
  Handle(TColStd_HArray1OfInteger) anArray;

  TopTools_IndexedMapOfShape mapShape1;
  TopExp::MapShapes(aShape1, TopAbs_ShapeEnum(theShapeType), mapShape1);

  Handle(GEOM_Object) anObj;
  Handle(TColStd_HSequenceOfTransient) aSeq = new TColStd_HSequenceOfTransient;
  TCollection_AsciiString anAsciiList, anEntry;

  TopTools_MapOfShape mapShape2;
  TopExp_Explorer exp (aShape2, TopAbs_ShapeEnum(theShapeType));
  for (; exp.More(); exp.Next()) {
    TopoDS_Shape aSS = exp.Current();
    if (mapShape2.Add(aSS) && mapShape1.Contains(aSS)) {
      anArray = new TColStd_HArray1OfInteger(1,1);
      anArray->SetValue(1, anIndices.FindIndex(aSS));
      anObj = GetEngine()->AddSubShape(theShape1, anArray);
      aSeq->Append(anObj);

      // for python command
      TDF_Tool::Entry(anObj->GetEntry(), anEntry);
      anAsciiList += anEntry;
      anAsciiList += ",";
    }
  }

  if (aSeq->IsEmpty()) {
    SetErrorCode("The given shapes have no shared sub-shapes of the requested type");
    return aSeq;
  }

  //Make a Python command
  anAsciiList.Trunc(anAsciiList.Length() - 1);

  Handle(GEOM_Function) aFunction = anObj->GetLastFunction();

  GEOM::TPythonDump(aFunction) << "[" << anAsciiList.ToCString()
    << "] = geompy.GetSharedShapes(" << theShape1 << ", "
      << theShape2 << ", " << TopAbs_ShapeEnum(theShapeType) << ")";

  SetErrorCode(OK);
  return aSeq;
}

//=============================================================================
/*!
 *  
 */
//=============================================================================
static GEOM::TPythonDump& operator<< (GEOM::TPythonDump&   theDump,
                                      const GEOMAlgo_State theState)
{
  switch (theState) {
  case GEOMAlgo_ST_IN:
    theDump << "geompy.GEOM.ST_IN";
    break;
  case GEOMAlgo_ST_OUT:
    theDump << "geompy.GEOM.ST_OUT";
    break;
  case GEOMAlgo_ST_ON:
    theDump << "geompy.GEOM.ST_ON";
    break;
  case GEOMAlgo_ST_ONIN:
    theDump << "geompy.GEOM.ST_ONIN";
    break;
  case GEOMAlgo_ST_ONOUT:
    theDump << "geompy.GEOM.ST_ONOUT";
    break;
  default:
    theDump << "geompy.GEOM.ST_UNKNOWN";
    break;
  }
  return theDump;
}

//=======================================================================
//function : checkTypeShapesOn
/*!
 * \brief Checks if theShapeType parameter of GetShapesOnXXX() is OK
 * \param theShapeType - the shape type to check
 * \retval bool  - result of the check
 */
//=======================================================================

bool GEOMImpl_IShapesOperations::checkTypeShapesOn(const Standard_Integer theShapeType)
{
  if (theShapeType != TopAbs_VERTEX &&
      theShapeType != TopAbs_EDGE &&
      theShapeType != TopAbs_FACE &&
      theShapeType != TopAbs_SOLID) {
    SetErrorCode("Only solids, vertices, edges or faces can be found by this method");
    return false;
  }
  return true;
}

//=======================================================================
//function : makePlane
  /*!
   * \brief Creates Geom_Plane
    * \param theAx1 - shape object defining plane parameters
    * \retval Handle(Geom_Surface) - resulting surface
   */
//=======================================================================

Handle(Geom_Surface) GEOMImpl_IShapesOperations::makePlane(const TopoDS_Shape& anAx1)
{
  if (anAx1.ShapeType() != TopAbs_EDGE) return NULL;
  TopoDS_Edge anEdge = TopoDS::Edge(anAx1);
  TopoDS_Vertex V1, V2;
  TopExp::Vertices(anEdge, V1, V2, Standard_True);
  if (V1.IsNull() || V2.IsNull()) {
    SetErrorCode("Bad edge given for the plane normal vector");
    return NULL;
  }
  gp_Pnt aLoc = BRep_Tool::Pnt(V1);
  gp_Vec aVec (aLoc, BRep_Tool::Pnt(V2));
  if (aVec.Magnitude() < Precision::Confusion()) {
    SetErrorCode("Vector with null magnitude given");
    return NULL;
  }
  return new Geom_Plane(aLoc, aVec);
}

//=======================================================================
//function : makeCylinder
  /*!
   * \brief Creates Geom_CylindricalSurface
    * \param theAx1 - edge defining cylinder axis
    * \param theRadius - cylinder radius
    * \retval Handle(Geom_Surface) - resulting surface
   */
//=======================================================================

Handle(Geom_Surface) GEOMImpl_IShapesOperations::makeCylinder(const TopoDS_Shape& anAxis,
                                                              const Standard_Real theRadius)
{
  //Axis of the cylinder
  if (anAxis.ShapeType() != TopAbs_EDGE) {
    SetErrorCode("Not an edge given for the axis");
    return NULL;
  }
  TopoDS_Edge anEdge = TopoDS::Edge(anAxis);
  TopoDS_Vertex V1, V2;
  TopExp::Vertices(anEdge, V1, V2, Standard_True);
  if (V1.IsNull() || V2.IsNull()) {
    SetErrorCode("Bad edge given for the axis");
    return NULL;
  }
  gp_Pnt aLoc = BRep_Tool::Pnt(V1);
  gp_Vec aVec (aLoc, BRep_Tool::Pnt(V2));
  if (aVec.Magnitude() < Precision::Confusion()) {
    SetErrorCode("Vector with null magnitude given");
    return NULL;
  }

  gp_Ax3 anAx3 (aLoc, aVec);
  return new Geom_CylindricalSurface(anAx3, theRadius);
}


//=======================================================================
//function : getShapesOnSurfaceIDs
  /*!
   * \brief Find IDs of subshapes complying with given status about surface
    * \param theSurface - the surface to check state of subshapes against
    * \param theShape - the shape to explore
    * \param theShapeType - type of subshape of theShape
    * \param theState - required state
    * \retval Handle(TColStd_HSequenceOfInteger) - IDs of found subshapes
   */
//=======================================================================

Handle(TColStd_HSequenceOfInteger)
  GEOMImpl_IShapesOperations::getShapesOnSurfaceIDs(const Handle(Geom_Surface)& theSurface,
                                                    const TopoDS_Shape&         theShape,
                                                    TopAbs_ShapeEnum            theShapeType,
                                                    GEOMAlgo_State              theState)
{
  Handle(TColStd_HSequenceOfInteger) aSeqOfIDs;

  // Check presence of triangulation, build if need
  if (!CheckTriangulation(theShape))
    return aSeqOfIDs;

  // Call algo
  GEOMAlgo_FinderShapeOn1 aFinder;
  Standard_Real aTol = 0.0001; // default value

  aFinder.SetShape(theShape);
  aFinder.SetTolerance(aTol);
  aFinder.SetSurface(theSurface);
  aFinder.SetShapeType(theShapeType);
  aFinder.SetState(theState);

  // Sets the minimal number of inner points for the faces that do not have own
  // inner points at all (for e.g. rectangular planar faces have just 2 triangles).
  // Default value=3
  aFinder.SetNbPntsMin(3);
  // Sets the maximal number of inner points for edges or faces.
  // It is usefull for the cases when this number is very big (e.g =2000) to improve
  // the performance. If this value =0, all inner points will be taken into account.
  // Default value=0
  aFinder.SetNbPntsMax(100);

  aFinder.Perform();

  // Interprete results
  Standard_Integer iErr = aFinder.ErrorStatus();
  // the detailed description of error codes is in GEOMAlgo_FinderShapeOn1.cxx
  if (iErr) {
    MESSAGE(" iErr : " << iErr);
    TCollection_AsciiString aMsg (" iErr : ");
    aMsg += TCollection_AsciiString(iErr);
    SetErrorCode(aMsg);
    return aSeqOfIDs;
  }
  Standard_Integer iWrn = aFinder.WarningStatus();
  // the detailed description of warning codes is in GEOMAlgo_FinderShapeOn1.cxx
  if (iWrn) {
    MESSAGE(" *** iWrn : " << iWrn);
  }

  const TopTools_ListOfShape& listSS = aFinder.Shapes(); // the result

  if (listSS.Extent() < 1) {
    SetErrorCode("Not a single sub-shape of the requested type found on the given surface");
    return aSeqOfIDs;
  }

  // Fill sequence of object IDs
  aSeqOfIDs = new TColStd_HSequenceOfInteger;

  TopTools_IndexedMapOfShape anIndices;
  TopExp::MapShapes(theShape, anIndices);

  TopTools_ListIteratorOfListOfShape itSub (listSS);
  for (int index = 1; itSub.More(); itSub.Next(), ++index) {
    int id = anIndices.FindIndex(itSub.Value());
    aSeqOfIDs->Append(id);
  }

  return aSeqOfIDs;
}

//=======================================================================
//function : getObjectsShapesOn
/*!
 * \brief Find shape objects and their entries by their ids
 * \param theShapeIDs - incoming shape ids
 * \param theShapeEntries - outgoing entries like "entry1, entry2, ..."
 * \retval Handle(TColStd_HSequenceOfTransient) - found shape objects
 */
//=======================================================================

Handle(TColStd_HSequenceOfTransient) GEOMImpl_IShapesOperations::
 getObjectsShapesOn(const Handle(GEOM_Object)&                theShape,
                    const Handle(TColStd_HSequenceOfInteger)& theShapeIDs,
                    TCollection_AsciiString &                 theShapeEntries)
{
  Handle(TColStd_HSequenceOfTransient) aSeq;

  if ( !theShapeIDs.IsNull() && theShapeIDs->Length() > 0 )
  {
    aSeq = new TColStd_HSequenceOfTransient;
    Handle(TColStd_HArray1OfInteger) anArray = new TColStd_HArray1OfInteger(1,1);
    TCollection_AsciiString anEntry;
    for ( int i = 1; i <= theShapeIDs->Length(); ++i )
    {
      anArray->SetValue(1, theShapeIDs->Value( i ));
      Handle(GEOM_Object) anObj = GetEngine()->AddSubShape(theShape, anArray);
      aSeq->Append( anObj );

      TDF_Tool::Entry(anObj->GetEntry(), anEntry);
      if ( i != 1 ) theShapeEntries += ",";
      theShapeEntries += anEntry;
    }
  }
  return aSeq;
}

//=======================================================================
//function : getShapesOnSurface
/*!
   * \brief Find subshapes complying with given status about surface
    * \param theSurface - the surface to check state of subshapes against
    * \param theShape - the shape to explore
    * \param theShapeType - type of subshape of theShape
    * \param theState - required state
    * \param theShapeEntries - outgoing entries like "entry1, entry2, ..."
    * \retval Handle(TColStd_HSequenceOfInteger) - IDs of found subshapes
 */
//=======================================================================

Handle(TColStd_HSequenceOfTransient)
    GEOMImpl_IShapesOperations::getShapesOnSurface(const Handle(Geom_Surface)& theSurface,
                                                   const Handle(GEOM_Object)&  theShape,
                                                   TopAbs_ShapeEnum            theShapeType,
                                                   GEOMAlgo_State              theState,
                                                   TCollection_AsciiString &   theShapeEntries)
{
  // Find subshapes ids
  Handle(TColStd_HSequenceOfInteger) aSeqOfIDs =
    getShapesOnSurfaceIDs (theSurface, theShape->GetValue(), theShapeType, theState);
  if ( aSeqOfIDs.IsNull()  || aSeqOfIDs->Length() == 0 )
    return NULL;

  return getObjectsShapesOn( theShape, aSeqOfIDs, theShapeEntries );
}

//=============================================================================
/*!
 *  GetShapesOnPlane
 */
//=============================================================================
Handle(TColStd_HSequenceOfTransient) GEOMImpl_IShapesOperations::GetShapesOnPlane
                                        (const Handle(GEOM_Object)& theShape,
                                         const Standard_Integer     theShapeType,
                                         const Handle(GEOM_Object)& theAx1,
                                         const GEOMAlgo_State       theState)
{
  SetErrorCode(KO);

  if (theShape.IsNull() || theAx1.IsNull()) return NULL;

  TopoDS_Shape aShape = theShape->GetValue();
  TopoDS_Shape anAx1  = theAx1->GetValue();

  if (aShape.IsNull() || anAx1.IsNull()) return NULL;

  TopAbs_ShapeEnum aShapeType = TopAbs_ShapeEnum(theShapeType);
  if ( !checkTypeShapesOn( theShapeType ))
    return NULL;

  // Create plane
  Handle(Geom_Surface) aPlane = makePlane( anAx1 );
  if ( aPlane.IsNull() )
    return NULL;

  // Find objects
  TCollection_AsciiString anAsciiList;
  Handle(TColStd_HSequenceOfTransient) aSeq;
  aSeq = getShapesOnSurface( aPlane, theShape, aShapeType, theState, anAsciiList );
  if ( aSeq.IsNull() || aSeq->Length() == 0 )
    return NULL;

  // Make a Python command

  Handle(GEOM_Object) anObj = Handle(GEOM_Object)::DownCast( aSeq->Value( 1 ));
  Handle(GEOM_Function) aFunction = anObj->GetLastFunction();

  GEOM::TPythonDump(aFunction) << "[" << anAsciiList.ToCString()
    << "] = geompy.GetShapesOnPlane(" << theShape << ", "
      << aShapeType << ", " << theAx1 << ", " << theState << ")";

  SetErrorCode(OK);
  return aSeq;
}

//=============================================================================
/*!
 *  GetShapesOnCylinder
 */
//=============================================================================
Handle(TColStd_HSequenceOfTransient) GEOMImpl_IShapesOperations::GetShapesOnCylinder
                                          (const Handle(GEOM_Object)& theShape,
                                           const Standard_Integer     theShapeType,
                                           const Handle(GEOM_Object)& theAxis,
                                           const Standard_Real        theRadius,
                                           const GEOMAlgo_State       theState)
{
  SetErrorCode(KO);

  if (theShape.IsNull() || theAxis.IsNull()) return NULL;

  TopoDS_Shape aShape = theShape->GetValue();
  TopoDS_Shape anAxis = theAxis->GetValue();

  if (aShape.IsNull() || anAxis.IsNull()) return NULL;

  TopAbs_ShapeEnum aShapeType = TopAbs_ShapeEnum(theShapeType);
  if ( !checkTypeShapesOn( aShapeType ))
    return NULL;

  // Create a cylinder surface
  Handle(Geom_Surface) aCylinder = makeCylinder( anAxis, theRadius );
  if ( aCylinder.IsNull() )
    return NULL;

  // Find objects
  TCollection_AsciiString anAsciiList;
  Handle(TColStd_HSequenceOfTransient) aSeq;
  aSeq = getShapesOnSurface( aCylinder, theShape, aShapeType, theState, anAsciiList );
  if ( aSeq.IsNull() || aSeq->Length() == 0 )
    return NULL;
  
  // Make a Python command

  Handle(GEOM_Object) anObj = Handle(GEOM_Object)::DownCast( aSeq->Value( 1 ));
  Handle(GEOM_Function) aFunction = anObj->GetLastFunction();

  GEOM::TPythonDump(aFunction) << "[" << anAsciiList.ToCString()
    << "] = geompy.GetShapesOnCylinder(" << theShape << ", " << aShapeType
      << ", " << theAxis << ", " << theRadius << ", " << theState << ")";

  SetErrorCode(OK);
  return aSeq;
}

//=============================================================================
/*!
 *  GetShapesOnSphere
 */
//=============================================================================
Handle(TColStd_HSequenceOfTransient) GEOMImpl_IShapesOperations::GetShapesOnSphere
                                          (const Handle(GEOM_Object)& theShape,
                                           const Standard_Integer     theShapeType,
                                           const Handle(GEOM_Object)& theCenter,
                                           const Standard_Real        theRadius,
                                           const GEOMAlgo_State       theState)
{
  SetErrorCode(KO);

  if (theShape.IsNull() || theCenter.IsNull()) return NULL;

  TopoDS_Shape aShape  = theShape->GetValue();
  TopoDS_Shape aCenter = theCenter->GetValue();

  if (aShape.IsNull() || aCenter.IsNull()) return NULL;

  TopAbs_ShapeEnum aShapeType = TopAbs_ShapeEnum(theShapeType);
  if ( !checkTypeShapesOn( aShapeType ))
    return NULL;

  // Center of the sphere
  if (aCenter.ShapeType() != TopAbs_VERTEX) return NULL;
  gp_Pnt aLoc = BRep_Tool::Pnt(TopoDS::Vertex(aCenter));

  gp_Ax3 anAx3 (aLoc, gp::DZ());
  Handle(Geom_SphericalSurface) aSphere =
    new Geom_SphericalSurface(anAx3, theRadius);

  // Find objects
  TCollection_AsciiString anAsciiList;
  Handle(TColStd_HSequenceOfTransient) aSeq;
  aSeq = getShapesOnSurface( aSphere, theShape, aShapeType, theState, anAsciiList );
  if ( aSeq.IsNull() || aSeq->Length() == 0 )
    return NULL;
    
  // Make a Python command

  Handle(GEOM_Object) anObj = Handle(GEOM_Object)::DownCast( aSeq->Value( 1 ));
  Handle(GEOM_Function) aFunction = anObj->GetLastFunction();

  GEOM::TPythonDump(aFunction) << "[" << anAsciiList.ToCString()
    << "] = geompy.GetShapesOnSphere(" << theShape << ", " << aShapeType
      << ", " << theCenter << ", " << theRadius << ", " << theState << ")";

  SetErrorCode(OK);
  return aSeq;
}

//=============================================================================
/*!
 *  GetShapesOnPlaneIDs
 */
//=============================================================================
Handle(TColStd_HSequenceOfInteger) GEOMImpl_IShapesOperations::GetShapesOnPlaneIDs
                                        (const Handle(GEOM_Object)& theShape,
                                         const Standard_Integer     theShapeType,
                                         const Handle(GEOM_Object)& theAx1,
                                         const GEOMAlgo_State       theState)
{
  SetErrorCode(KO);

  if (theShape.IsNull() || theAx1.IsNull()) return NULL;

  TopoDS_Shape aShape = theShape->GetValue();
  TopoDS_Shape anAx1  = theAx1->GetValue();

  if (aShape.IsNull() || anAx1.IsNull()) return NULL;

  TopAbs_ShapeEnum aShapeType = TopAbs_ShapeEnum(theShapeType);
  if ( !checkTypeShapesOn( aShapeType ))
    return NULL;

  // Create plane
  Handle(Geom_Surface) aPlane = makePlane( anAx1 );
  if ( aPlane.IsNull() )
    return NULL;

  // Find object IDs
  Handle(TColStd_HSequenceOfInteger) aSeq;
  aSeq = getShapesOnSurfaceIDs( aPlane, aShape, aShapeType, theState );

  // The GetShapesOnPlaneIDs() doesn't change object so no new function is required.
  Handle(GEOM_Function) aFunction = GEOM::GetCreatedLast(theShape,theAx1)->GetLastFunction();

  // Make a Python command
  GEOM::TPythonDump(aFunction, /*append=*/true)
    << "listShapesOnPlane = geompy.GetShapesOnPlaneIDs"
    << "(" << theShape << "," << aShapeType << "," << theAx1 << "," << theState << ")";

  SetErrorCode(OK);
  return aSeq;
}

//=============================================================================
/*!
 *  GetShapesOnCylinderIDs
 */
//=============================================================================
Handle(TColStd_HSequenceOfInteger) GEOMImpl_IShapesOperations::GetShapesOnCylinderIDs
                                          (const Handle(GEOM_Object)& theShape,
                                           const Standard_Integer     theShapeType,
                                           const Handle(GEOM_Object)& theAxis,
                                           const Standard_Real        theRadius,
                                           const GEOMAlgo_State       theState)
{
  SetErrorCode(KO);

  if (theShape.IsNull() || theAxis.IsNull()) return NULL;

  TopoDS_Shape aShape = theShape->GetValue();
  TopoDS_Shape anAxis = theAxis->GetValue();

  if (aShape.IsNull() || anAxis.IsNull()) return NULL;

  TopAbs_ShapeEnum aShapeType = TopAbs_ShapeEnum(theShapeType);
  if ( !checkTypeShapesOn( aShapeType ))
    return NULL;

  // Create a cylinder surface
  Handle(Geom_Surface) aCylinder = makeCylinder( anAxis, theRadius );
  if ( aCylinder.IsNull() )
    return NULL;
  
  // Find object IDs
  Handle(TColStd_HSequenceOfInteger) aSeq;
  aSeq = getShapesOnSurfaceIDs( aCylinder, aShape, aShapeType, theState );

  // The GetShapesOnCylinder() doesn't change object so no new function is required.
  Handle(GEOM_Function) aFunction = GEOM::GetCreatedLast(theShape,theAxis)->GetLastFunction();

  // Make a Python command
  GEOM::TPythonDump(aFunction, /*append=*/true)
    << "listShapesOnCylinder = geompy.GetShapesOnCylinderIDs"
    << "(" << theShape << ", " << aShapeType << ", " << theAxis << ", "
    << theRadius << ", " << theState << ")";

  SetErrorCode(OK);
  return aSeq;
}

//=============================================================================
/*!
 *  GetShapesOnSphereIDs
 */
//=============================================================================
Handle(TColStd_HSequenceOfInteger) GEOMImpl_IShapesOperations::GetShapesOnSphereIDs
                                          (const Handle(GEOM_Object)& theShape,
                                           const Standard_Integer     theShapeType,
                                           const Handle(GEOM_Object)& theCenter,
                                           const Standard_Real        theRadius,
                                           const GEOMAlgo_State       theState)
{
  SetErrorCode(KO);

  if (theShape.IsNull() || theCenter.IsNull()) return NULL;

  TopoDS_Shape aShape  = theShape->GetValue();
  TopoDS_Shape aCenter = theCenter->GetValue();

  if (aShape.IsNull() || aCenter.IsNull()) return NULL;

  TopAbs_ShapeEnum aShapeType = TopAbs_ShapeEnum(theShapeType);
  if ( !checkTypeShapesOn( aShapeType ))
    return NULL;

  // Center of the sphere
  if (aCenter.ShapeType() != TopAbs_VERTEX) return NULL;
  gp_Pnt aLoc = BRep_Tool::Pnt(TopoDS::Vertex(aCenter));

  gp_Ax3 anAx3 (aLoc, gp::DZ());
  Handle(Geom_SphericalSurface) aSphere =
    new Geom_SphericalSurface(anAx3, theRadius);

  // Find object IDs
  Handle(TColStd_HSequenceOfInteger) aSeq;
  aSeq = getShapesOnSurfaceIDs( aSphere, aShape, aShapeType, theState );
  
  // The GetShapesOnSphere() doesn't change object so no new function is required.
  Handle(GEOM_Function) aFunction = GEOM::GetCreatedLast(theShape,theCenter)->GetLastFunction();

  // Make a Python command
  GEOM::TPythonDump(aFunction, /*append=*/true)
    << "listShapesOnCylinder = geompy.GetShapesOnCylinderIDs"
    << "(" << theShape << ", " << aShapeType << ", " << theCenter << ", "
    << theRadius << ", " << theState << ")";

  SetErrorCode(OK);
  return aSeq;
}

//=======================================================================
//function : getShapesOnQuadrangleIDs
  /*!
   * \brief Find IDs of subshapes complying with given status about quadrangle
    * \param theShape - the shape to explore
    * \param theShapeType - type of subshape of theShape
    * \param theTopLeftPoint - top left quadrangle corner
    * \param theTopRigthPoint - top right quadrangle corner
    * \param theBottomLeftPoint - bottom left quadrangle corner
    * \param theBottomRigthPoint - bottom right quadrangle corner
    * \param theState - required state
    * \retval Handle(TColStd_HSequenceOfInteger) - IDs of found subshapes
   */
//=======================================================================

Handle(TColStd_HSequenceOfInteger)
  GEOMImpl_IShapesOperations::getShapesOnQuadrangleIDs (const Handle(GEOM_Object)& theShape,
                                                        const Standard_Integer     theShapeType,
                                                        const Handle(GEOM_Object)& theTopLeftPoint,
                                                        const Handle(GEOM_Object)& theTopRigthPoint,
                                                        const Handle(GEOM_Object)& theBottomLeftPoint,
                                                        const Handle(GEOM_Object)& theBottomRigthPoint,
                                                        const GEOMAlgo_State       theState)
{
  SetErrorCode(KO);

  if ( theShape.IsNull() ||
       theTopLeftPoint.IsNull() ||
       theTopRigthPoint.IsNull() ||
       theBottomLeftPoint.IsNull() ||
       theBottomRigthPoint.IsNull() )
    return NULL;

  TopoDS_Shape aShape = theShape->GetValue();
  TopoDS_Shape aTL = theTopLeftPoint->GetValue();
  TopoDS_Shape aTR = theTopRigthPoint->GetValue();
  TopoDS_Shape aBL = theBottomLeftPoint->GetValue();
  TopoDS_Shape aBR = theBottomRigthPoint->GetValue();

  if (aShape.IsNull() ||
      aTL.IsNull() || 
      aTR.IsNull() || 
      aBL.IsNull() || 
      aBR.IsNull() ||
      aTL.ShapeType() != TopAbs_VERTEX || 
      aTR.ShapeType() != TopAbs_VERTEX || 
      aBL.ShapeType() != TopAbs_VERTEX || 
      aBR.ShapeType() != TopAbs_VERTEX )
    return NULL;

  TopAbs_ShapeEnum aShapeType = TopAbs_ShapeEnum(theShapeType);
  if ( !checkTypeShapesOn( aShapeType ))
    return NULL;

  Handle(TColStd_HSequenceOfInteger) aSeqOfIDs;

  // Check presence of triangulation, build if need
  if (!CheckTriangulation(aShape))
    return aSeqOfIDs;

  // Call algo
  gp_Pnt aPntTL = BRep_Tool::Pnt(TopoDS::Vertex(aTL));
  gp_Pnt aPntTR = BRep_Tool::Pnt(TopoDS::Vertex(aTR));
  gp_Pnt aPntBL = BRep_Tool::Pnt(TopoDS::Vertex(aBL));
  gp_Pnt aPntBR = BRep_Tool::Pnt(TopoDS::Vertex(aBR));

  GEOMAlgo_FinderShapeOnQuad aFinder( aPntTL, aPntTR, aPntBL, aPntBR );
  Standard_Real aTol = 0.0001; // default value

  aFinder.SetShape(aShape);
  aFinder.SetTolerance(aTol);
  //aFinder.SetSurface(theSurface);
  aFinder.SetShapeType(aShapeType);
  aFinder.SetState(theState);

  // Sets the minimal number of inner points for the faces that do not have own
  // inner points at all (for e.g. rectangular planar faces have just 2 triangles).
  // Default value=3
  aFinder.SetNbPntsMin(3);
  // Sets the maximal number of inner points for edges or faces.
  // It is usefull for the cases when this number is very big (e.g =2000) to improve
  // the performance. If this value =0, all inner points will be taken into account.
  // Default value=0
  aFinder.SetNbPntsMax(100);

  aFinder.Perform();

  // Interprete results
  Standard_Integer iErr = aFinder.ErrorStatus();
  // the detailed description of error codes is in GEOMAlgo_FinderShapeOn1.cxx
  if (iErr) {
    MESSAGE(" iErr : " << iErr);
    TCollection_AsciiString aMsg (" iErr : ");
    aMsg += TCollection_AsciiString(iErr);
    SetErrorCode(aMsg);
    return aSeqOfIDs;
  }
  Standard_Integer iWrn = aFinder.WarningStatus();
  // the detailed description of warning codes is in GEOMAlgo_FinderShapeOn1.cxx
  if (iWrn) {
    MESSAGE(" *** iWrn : " << iWrn);
  }

  const TopTools_ListOfShape& listSS = aFinder.Shapes(); // the result

  if (listSS.Extent() < 1) {
    SetErrorCode("Not a single sub-shape of the requested type found on the given surface");
    return aSeqOfIDs;
  }

  // Fill sequence of object IDs
  aSeqOfIDs = new TColStd_HSequenceOfInteger;

  TopTools_IndexedMapOfShape anIndices;
  TopExp::MapShapes(aShape, anIndices);

  TopTools_ListIteratorOfListOfShape itSub (listSS);
  for (int index = 1; itSub.More(); itSub.Next(), ++index) {
    int id = anIndices.FindIndex(itSub.Value());
    aSeqOfIDs->Append(id);
  }
  return aSeqOfIDs;
}

//=======================================================================
//function : GetShapesOnQuadrangle
  /*!
   * \brief Find subshapes complying with given status about quadrangle
    * \param theShape - the shape to explore
    * \param theShapeType - type of subshape of theShape
    * \param theTopLeftPoint - top left quadrangle corner
    * \param theTopRigthPoint - top right quadrangle corner
    * \param theBottomLeftPoint - bottom left quadrangle corner
    * \param theBottomRigthPoint - bottom right quadrangle corner
    * \param theState - required state
    * \retval Handle(TColStd_HSequenceOfInteger) - IDs of found subshapes
   */
//=======================================================================

Handle(TColStd_HSequenceOfTransient)
    GEOMImpl_IShapesOperations::GetShapesOnQuadrangle (const Handle(GEOM_Object)& theShape,
                                                       const Standard_Integer     theShapeType,
                                                       const Handle(GEOM_Object)& theTopLeftPoint,
                                                       const Handle(GEOM_Object)& theTopRigthPoint,
                                                       const Handle(GEOM_Object)& theBottomLeftPoint,
                                                       const Handle(GEOM_Object)& theBottomRigthPoint,
                                                       const GEOMAlgo_State       theState)
{
  // Find indices
  Handle(TColStd_HSequenceOfInteger) aSeqOfIDs =
    getShapesOnQuadrangleIDs( theShape,
                              theShapeType,
                              theTopLeftPoint,
                              theTopRigthPoint,
                              theBottomLeftPoint,
                              theBottomRigthPoint,
                              theState);
  if ( aSeqOfIDs.IsNull() || aSeqOfIDs->IsEmpty() )
    return NULL;

  // Find objects by indices
  TCollection_AsciiString anAsciiList;
  Handle(TColStd_HSequenceOfTransient) aSeq;
  aSeq = getObjectsShapesOn( theShape, aSeqOfIDs, anAsciiList );
  if ( aSeq.IsNull() || aSeq->IsEmpty() )
    return NULL;

  // Make a Python command

  Handle(GEOM_Object) anObj = Handle(GEOM_Object)::DownCast( aSeq->Value( 1 ));
  Handle(GEOM_Function) aFunction = anObj->GetLastFunction();

  GEOM::TPythonDump(aFunction)
    << "[" << anAsciiList.ToCString() << "] = geompy.GetShapesOnQuadrangle("
    << theShape << ", "
    << TopAbs_ShapeEnum(theShapeType) << ", "
    << theTopLeftPoint << ", "
    << theTopRigthPoint << ", "
    << theBottomLeftPoint << ", "
    << theBottomRigthPoint << ", "
    << theState << ")";

  SetErrorCode(OK);
  return aSeq;
}

//=======================================================================
//function : GetShapesOnQuadrangleIDs
  /*!
   * \brief Find IDs of subshapes complying with given status about quadrangle
    * \param theShape - the shape to explore
    * \param theShapeType - type of subshape of theShape
    * \param theTopLeftPoint - top left quadrangle corner
    * \param theTopRigthPoint - top right quadrangle corner
    * \param theBottomLeftPoint - bottom left quadrangle corner
    * \param theBottomRigthPoint - bottom right quadrangle corner
    * \param theState - required state
    * \retval Handle(TColStd_HSequenceOfInteger) - IDs of found subshapes
   */
//=======================================================================

Handle(TColStd_HSequenceOfInteger)
  GEOMImpl_IShapesOperations::GetShapesOnQuadrangleIDs (const Handle(GEOM_Object)& theShape,
                                                        const Standard_Integer     theShapeType,
                                                        const Handle(GEOM_Object)& theTopLeftPoint,
                                                        const Handle(GEOM_Object)& theTopRigthPoint,
                                                        const Handle(GEOM_Object)& theBottomLeftPoint,
                                                        const Handle(GEOM_Object)& theBottomRigthPoint,
                                                        const GEOMAlgo_State       theState)
{
  // Find indices
  Handle(TColStd_HSequenceOfInteger) aSeqOfIDs =
    getShapesOnQuadrangleIDs( theShape,
                              theShapeType,
                              theTopLeftPoint,
                              theTopRigthPoint,
                              theBottomLeftPoint,
                              theBottomRigthPoint,
                              theState);
  if ( aSeqOfIDs.IsNull() || aSeqOfIDs->IsEmpty() )
    return NULL;

  // Make a Python command

  // The GetShapesOnCylinder() doesn't change object so no new function is required.
  Handle(GEOM_Object) lastObj = GEOM::GetCreatedLast(theShape,theTopLeftPoint);
  lastObj = GEOM::GetCreatedLast(lastObj,theTopRigthPoint);
  lastObj = GEOM::GetCreatedLast(lastObj,theBottomRigthPoint);
  lastObj = GEOM::GetCreatedLast(lastObj,theBottomLeftPoint);
  Handle(GEOM_Function) aFunction = lastObj->GetLastFunction();

  GEOM::TPythonDump(aFunction, /*append=*/true)
    << "listShapesOnQuadrangle = geompy.GetShapesOnQuadrangleIDs("
    << theShape << ", "
    << TopAbs_ShapeEnum(theShapeType) << ", "
    << theTopLeftPoint << ", "
    << theTopRigthPoint << ", "
    << theBottomLeftPoint << ", "
    << theBottomRigthPoint << ", "
    << theState << ")";

  SetErrorCode(OK);
  return aSeqOfIDs;
}


//=============================================================================
/*!
 *  GetInPlace
 */
//=============================================================================
static void SimplifyWhat (TopoDS_Shape& theWhat,
                          TopTools_IndexedMapOfShape& theArgumentIndices,
                          TColStd_ListOfInteger& theSimpleSubWhat)
{
  TopTools_MapOfShape mapShape;
  TopoDS_Iterator It (theWhat, Standard_True, Standard_True);
  for (; It.More(); It.Next()) {
    if (mapShape.Add(It.Value())) {
      TopoDS_Shape curSh = It.Value();
      if (curSh.ShapeType() == TopAbs_COMPOUND ||
          curSh.ShapeType() == TopAbs_COMPSOLID) {
        SimplifyWhat(curSh, theArgumentIndices, theSimpleSubWhat);
      } else {
        theSimpleSubWhat.Append(theArgumentIndices.FindIndex(curSh));
      }
    }
  }
}

static bool GetInPlaceOfCompound (Handle(GEOM_Function)& theWhereFunction,
                                  TopoDS_Shape& theWhat,
                                  TColStd_ListOfInteger& theModifiedArray)
{
  bool isFoundAny = false;
  TopTools_MapOfShape mapShape;
  TopoDS_Iterator It (theWhat, Standard_True, Standard_True);
  for (; It.More(); It.Next()) {
    if (mapShape.Add(It.Value())) {
      TopoDS_Shape curWhat = It.Value();
      if (curWhat.ShapeType() == TopAbs_COMPOUND ||
          curWhat.ShapeType() == TopAbs_COMPSOLID) {
        // Recursive call for compound or compsolid
        if (GetInPlaceOfCompound(theWhereFunction, curWhat, theModifiedArray))
          isFoundAny = true;
      } else {
        // Try to find for "simple" shape
        bool isFound = false;

        TDF_LabelSequence aLabelSeq;
        theWhereFunction->GetDependency(aLabelSeq);
        Standard_Integer nbArg = aLabelSeq.Length();

        for (Standard_Integer iarg = 1; iarg <= nbArg && !isFound; iarg++) {

          TDF_Label anArgumentRefLabel = aLabelSeq.Value(iarg);

          Handle(GEOM_Object) anArgumentObject = GEOM_Object::GetReferencedObject(anArgumentRefLabel);
          TopoDS_Shape anArgumentShape = anArgumentObject->GetValue();

          TopTools_IndexedMapOfShape anArgumentIndices;
          TopExp::MapShapes(anArgumentShape, anArgumentIndices);

          if (anArgumentIndices.Contains(curWhat)) {
            isFound = Standard_True;
            Standard_Integer aWhatIndex = anArgumentIndices.FindIndex(curWhat);

            // Find corresponding label in history
            TDF_Label anArgumentHistoryLabel =
              theWhereFunction->GetArgumentHistoryEntry(anArgumentRefLabel, Standard_False);
            if (!anArgumentHistoryLabel.IsNull()) {
              TDF_Label aWhatHistoryLabel = anArgumentHistoryLabel.FindChild(aWhatIndex, Standard_False);
              if (!aWhatHistoryLabel.IsNull()) {
                Handle(TDataStd_IntegerArray) anIntegerArray;
                if (aWhatHistoryLabel.FindAttribute(TDataStd_IntegerArray::GetID(), anIntegerArray)) {
                  Standard_Integer imod, aModifLen = anIntegerArray->Array()->Length();
                  for (imod = 1; imod <= aModifLen; imod++) {
                    theModifiedArray.Append(anIntegerArray->Array()->Value(imod));
                  }
                }
              }
            }
          }
        }
        if (isFound)
          isFoundAny = true;
      }
    }
  }
  return isFoundAny;
}

Handle(GEOM_Object) GEOMImpl_IShapesOperations::GetInPlace
                                          (Handle(GEOM_Object) theShapeWhere,
                                           Handle(GEOM_Object) theShapeWhat)
{
  SetErrorCode(KO);

  if (theShapeWhere.IsNull() || theShapeWhat.IsNull()) return NULL;

  TopoDS_Shape aWhere = theShapeWhere->GetValue();
  TopoDS_Shape aWhat  = theShapeWhat->GetValue();

  if (aWhere.IsNull() || aWhat.IsNull()) return NULL;

  //Fill array of indices
  Handle(TColStd_HArray1OfInteger) aModifiedArray;

  Handle(GEOM_Function) aWhereFunction = theShapeWhere->GetLastFunction();

  TopTools_IndexedMapOfShape aWhereIndices;
  TopExp::MapShapes(aWhere, aWhereIndices);

  if (aWhereIndices.Contains(aWhat)) {

    // entity was not changed by the operation
    Standard_Integer aWhatIndex = aWhereIndices.FindIndex(aWhat);
    aModifiedArray = new TColStd_HArray1OfInteger(1,1);
    aModifiedArray->SetValue(1, aWhatIndex);

  } else {

    TDF_Label aHistoryLabel = aWhereFunction->GetHistoryEntry(Standard_False);
    if (aHistoryLabel.IsNull()) {
      SetErrorCode("Modifications history does not exist for the shape under consideration.");
      return NULL;
    }

    // search in history for all argument shapes
    Standard_Boolean isFound = Standard_False;

    TDF_LabelSequence aLabelSeq;
    aWhereFunction->GetDependency(aLabelSeq);
    Standard_Integer nbArg = aLabelSeq.Length();

    for (Standard_Integer iarg = 1; iarg <= nbArg && !isFound; iarg++) {

      TDF_Label anArgumentRefLabel = aLabelSeq.Value(iarg);

      Handle(GEOM_Object) anArgumentObject = GEOM_Object::GetReferencedObject(anArgumentRefLabel);
      TopoDS_Shape anArgumentShape = anArgumentObject->GetValue();

      TopTools_IndexedMapOfShape anArgumentIndices;
      TopExp::MapShapes(anArgumentShape, anArgumentIndices);

      if (anArgumentIndices.Contains(aWhat)) {
        isFound = Standard_True;
        Standard_Integer aWhatIndex = anArgumentIndices.FindIndex(aWhat);

        // Find corresponding label in history
        TDF_Label anArgumentHistoryLabel =
          aWhereFunction->GetArgumentHistoryEntry(anArgumentRefLabel, Standard_False);
        if (anArgumentHistoryLabel.IsNull()) {
          // Lost History of operation argument. Possibly, all its entities was removed.
          SetErrorCode(OK);
          return NULL;
        }

        TDF_Label aWhatHistoryLabel = anArgumentHistoryLabel.FindChild(aWhatIndex, Standard_False);
        if (aWhatHistoryLabel.IsNull()) {
          // Check, if the sought shape is Compound or Compsolid.
          // In that case we will try to find history for its sub-shapes
          if (aWhat.ShapeType() == TopAbs_COMPOUND ||
              aWhat.ShapeType() == TopAbs_COMPSOLID) {
            TColStd_ListOfInteger aSimpleSubWhat, aModifiedList;
            SimplifyWhat(aWhat, anArgumentIndices, aSimpleSubWhat);
            TColStd_ListIteratorOfListOfInteger anIterSub (aSimpleSubWhat);
            for (; anIterSub.More(); anIterSub.Next()) {
              Standard_Integer aSubWhatIndex = anIterSub.Value();
              TDF_Label aSubWhatHistoryLabel =
                anArgumentHistoryLabel.FindChild(aSubWhatIndex, Standard_False);
              if (!aSubWhatHistoryLabel.IsNull()) {
                Handle(TDataStd_IntegerArray) anIntegerArray;
                if (aSubWhatHistoryLabel.FindAttribute(TDataStd_IntegerArray::GetID(), anIntegerArray)) {
                  for (Standard_Integer isub = 1; isub <= anIntegerArray->Length(); isub++) {
                    aModifiedList.Append(anIntegerArray->Value(isub));
                  }
                }
              }
            }
            if (aModifiedList.Extent() > 0) {
              Handle(TColStd_HArray1OfInteger) aModifiedArraySub =
                new TColStd_HArray1OfInteger (1, aModifiedList.Extent());
              TColStd_ListIteratorOfListOfInteger anIterModif (aModifiedList);
              for (Standard_Integer imod = 1; anIterModif.More(); anIterModif.Next(), imod++) {
                aModifiedArraySub->SetValue(imod, anIterModif.Value());
              }
              aModifiedArray = aModifiedArraySub;
            } else {
              // Removed entity
              SetErrorCode(OK);
              return NULL;
            }
          } else {
            // Removed entity
            SetErrorCode(OK);
            return NULL;
          }
        } else {
          Handle(TDataStd_IntegerArray) anIntegerArray;
          if (!aWhatHistoryLabel.FindAttribute(TDataStd_IntegerArray::GetID(), anIntegerArray)) {
            SetErrorCode("Error: Empty modifications history for the sought shape.");
            return NULL;
          }

          aModifiedArray = anIntegerArray->Array();
          if (aModifiedArray->Length() == 0) {
            SetErrorCode("Error: Empty modifications history for the sought shape.");
            return NULL;
          }
        }
      }
    }

    if (!isFound) {
      // try compound element by element
      if (aWhat.ShapeType() == TopAbs_COMPOUND ||
          aWhat.ShapeType() == TopAbs_COMPSOLID) {
        TColStd_ListOfInteger aModifiedList;
        isFound = GetInPlaceOfCompound(aWhereFunction, aWhat, aModifiedList);
        if (isFound) {
          aModifiedArray = new TColStd_HArray1OfInteger (1, aModifiedList.Extent());
          TColStd_ListIteratorOfListOfInteger anIterModif (aModifiedList);
          for (Standard_Integer imod = 1; anIterModif.More(); anIterModif.Next(), imod++) {
            aModifiedArray->SetValue(imod, anIterModif.Value());
          }
        }
      }
      if (!isFound) {
        SetErrorCode("The sought shape does not belong to any operation argument.");
        return NULL;
      }
    }
  }

  //Add a new object
  Handle(GEOM_Object) aResult = GetEngine()->AddSubShape(theShapeWhere, aModifiedArray);

  if (aModifiedArray->Length() > 1) {
    //Set a GROUP type
    aResult->SetType(GEOM_GROUP);

    //Set a sub shape type
    TDF_Label aFreeLabel = aResult->GetFreeLabel();
    TopAbs_ShapeEnum aShapeType = aWhat.ShapeType();
    TDataStd_Integer::Set(aFreeLabel, (Standard_Integer)aShapeType);
  }

  //Make a Python command
  Handle(GEOM_Function) aFunction = aResult->GetFunction(1);

  GEOM::TPythonDump(aFunction) << aResult << " = geompy.GetInPlace("
    << theShapeWhere << ", " << theShapeWhat << ")";

  SetErrorCode(OK);
  return aResult;
}

//=======================================================================
//function : SortShapes
//purpose  :
//=======================================================================
void GEOMImpl_IShapesOperations::SortShapes(TopTools_ListOfShape& SL)
{
  Standard_Integer MaxShapes = SL.Extent();
  TopTools_Array1OfShape  aShapes (1,MaxShapes);
  TColStd_Array1OfInteger OrderInd(1,MaxShapes);
  TColStd_Array1OfReal    MidXYZ  (1,MaxShapes); //X,Y,Z;
  TColStd_Array1OfReal    Length  (1,MaxShapes); //X,Y,Z;

  // Computing of CentreOfMass
  Standard_Integer Index;
  GProp_GProps GPr;
  gp_Pnt GPoint;
  TopTools_ListIteratorOfListOfShape it(SL);
  for (Index=1;  it.More();  Index++)
  {
    TopoDS_Shape S = it.Value();
    SL.Remove( it ); // == it.Next()
    aShapes(Index) = S;
    OrderInd.SetValue (Index, Index);
    if (S.ShapeType() == TopAbs_VERTEX)
    {
      GPoint = BRep_Tool::Pnt( TopoDS::Vertex( S ));
      Length.SetValue( Index, (Standard_Real) S.Orientation());
    }
    else
    {
      BRepGProp::LinearProperties (S, GPr);
      GPoint = GPr.CentreOfMass();
      Length.SetValue( Index, GPr.Mass() );
    }
    MidXYZ.SetValue(Index,
		    GPoint.X()*999 + GPoint.Y()*99 + GPoint.Z()*0.9);
  }

  // Sorting
  Standard_Integer aTemp;
  Standard_Boolean exchange, Sort = Standard_True;
  while (Sort)
  {
    Sort = Standard_False;
    for (Index=1; Index < MaxShapes; Index++)
    {
      if (MidXYZ(OrderInd(Index)) > MidXYZ(OrderInd(Index+1)))
	exchange = Standard_True;
      else if (MidXYZ(OrderInd(Index)) == MidXYZ(OrderInd(Index+1)) &&
	       Length(OrderInd(Index)) >  Length(OrderInd(Index+1)) )
	exchange = Standard_True;
      else
	exchange = Standard_False;
      if (exchange)
      {
        aTemp = OrderInd(Index);
        OrderInd(Index) = OrderInd(Index+1);
        OrderInd(Index+1) = aTemp;
        Sort = Standard_True;
      }
    }
  }
  for (Index=1; Index <= MaxShapes; Index++)
    SL.Append( aShapes( OrderInd(Index) ));
}

//=======================================================================
//function : CheckTriangulation
//purpose  :
//=======================================================================
bool GEOMImpl_IShapesOperations::CheckTriangulation (const TopoDS_Shape& aShape)
{
  TopExp_Explorer exp (aShape, TopAbs_FACE);
  if (!exp.More()) {
    SetErrorCode("Shape without faces given");
    return false;
  }

  TopLoc_Location aTopLoc;
  Handle(Poly_Triangulation) aTRF;
  aTRF = BRep_Tool::Triangulation(TopoDS::Face(exp.Current()), aTopLoc);
  if (aTRF.IsNull()) {
    // calculate deflection
    Standard_Real aDeviationCoefficient = 0.001;

    Bnd_Box B;
    BRepBndLib::Add(aShape, B);
    Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
    B.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

    Standard_Real dx = aXmax - aXmin, dy = aYmax - aYmin, dz = aZmax - aZmin;
    Standard_Real aDeflection = Max(Max(dx, dy), dz) * aDeviationCoefficient * 4;
    Standard_Real aHLRAngle = 0.349066;

    BRepMesh_IncrementalMesh Inc (aShape, aDeflection, Standard_False, aHLRAngle);
  }

  return true;
}
