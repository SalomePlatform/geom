using namespace std;

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

#include "utilities.h"
#include "OpUtil.hxx"
#include "Utils_ExceptHandlers.hxx"

#include <TFunction_DriverTable.hxx>
#include <TFunction_Driver.hxx>
#include <TFunction_Logbook.hxx>
#include <TDataStd_Integer.hxx>
#include <TDF_Tool.hxx>

#include <BRepExtrema_ExtCF.hxx>

#include <BRep_Tool.hxx>
#include <BRepGProp.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>

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

#include <GProp_GProps.hxx>
#include <gp_Pnt.hxx>
#include <gp_Lin.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HArray1OfInteger.hxx>

//#include <OSD_Timer.hxx>

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
  TCollection_AsciiString anEntry, aDescr;
  TDF_Tool::Entry(anEdge->GetEntry(), anEntry);
  aDescr += (anEntry+" = IShapesOperations.MakeEdge(");
  TDF_Tool::Entry(thePnt1->GetEntry(), anEntry);
  aDescr += (anEntry+", ");
  TDF_Tool::Entry(thePnt2->GetEntry(), anEntry);
  aDescr += (anEntry+")");

  aFunction->SetDescription(aDescr);

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
  TCollection_AsciiString anEntry, aDescr;
  TDF_Tool::Entry(aFace->GetEntry(), anEntry);
  aDescr += anEntry;
  aDescr += " = IShapesOperations.MakeFace(";
  TDF_Tool::Entry(theWire->GetEntry(), anEntry);
  aDescr += anEntry;
  if (isPlanarWanted)
    aDescr += ", 1)";

  else
    aDescr += ", 0)";

  aFunction->SetDescription(aDescr);

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
  TCollection_AsciiString anEntry, aDescr;
  TDF_Tool::Entry(aShape->GetEntry(), anEntry);
  aDescr += (anEntry + " = IShapesOperations.MakeFaceWires([");
  // Shapes
  it = theShapes.begin();
  if (it != theShapes.end()) {
    TDF_Tool::Entry((*it)->GetEntry(), anEntry);
    it++;
    aDescr += (anEntry+", ");
    for (; it != theShapes.end(); it++) {
      aDescr += ", ";
      TDF_Tool::Entry((*it)->GetEntry(), anEntry);
      aDescr += anEntry;
    }
  }
  if (isPlanarWanted)
    aDescr += "], 1)";

  else
    aDescr += "], 0)";

  aFunction->SetDescription(aDescr);

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
  return MakeShape(theShapes, GEOM_SOLID, SOLID_SHELLS, "MakeSolidShells");
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
  TCollection_AsciiString anEntry, aDescr("");
  TDF_Tool::Entry(aSolid->GetEntry(), anEntry);
  aDescr += anEntry;
  aDescr += " = IShapesOperations.MakeSolidShell(";
  TDF_Tool::Entry(theShell->GetEntry(), anEntry);
  aDescr += (anEntry+")");

  aFunction->SetDescription(aDescr);

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
                             (list<Handle(GEOM_Object)>     theShapes,
                              const Standard_Integer        theObjectType,
                              const Standard_Integer        theFunctionType,
                              const TCollection_AsciiString theMethodName)
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
  TCollection_AsciiString anEntry, aDescr("");
  TDF_Tool::Entry(aShape->GetEntry(), anEntry);
  aDescr += (anEntry + " = IShapesOperations.");
  aDescr += (theMethodName + "([");
  // Shapes
  it = theShapes.begin();
  if (it != theShapes.end()) {
    TDF_Tool::Entry((*it)->GetEntry(), anEntry);
    it++;
    aDescr += (anEntry+", ");
    for (; it != theShapes.end(); it++) {
      aDescr += ", ";
      TDF_Tool::Entry((*it)->GetEntry(), anEntry);
      aDescr += anEntry;
    }
  }
  aDescr += "])";

  aFunction->SetDescription(aDescr);

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
  TCollection_AsciiString anEntry, aDescr;
  TDF_Tool::Entry(aGlued->GetEntry(), anEntry);
  aDescr += anEntry;
  aDescr += " = IShapesOperations.MakeGlueFaces(";
  TDF_Tool::Entry(theShape->GetEntry(), anEntry);
  aDescr += anEntry + ", ";
  aDescr += TCollection_AsciiString(theTolerance) + ")";

  aFunction->SetDescription(aDescr);

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
//  OSD_Timer timer1, timer2, timer3, timer4;
//  timer1.Start();

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

//  timer1.Stop();
//  timer2.Start();

  if (isSorted)
    SortShapes(listShape);

//  timer2.Stop();
//  timer3.Start();

  TopTools_IndexedMapOfShape anIndices;
  TopExp::MapShapes(aShape, anIndices);
  Handle(TColStd_HArray1OfInteger) anArray;

  TopTools_ListIteratorOfListOfShape itSub (listShape);
  TCollection_AsciiString anAsciiList = "[", anEntry;
  for (int index = 1; itSub.More(); itSub.Next(), ++index) {
    TopoDS_Shape aValue = itSub.Value();
    anArray = new TColStd_HArray1OfInteger(1,1);
    anArray->SetValue(1, anIndices.FindIndex(aValue));
    anObj = GetEngine()->AddSubShape(theShape, anArray);
    aSeq->Append(anObj);

    TDF_Tool::Entry(anObj->GetEntry(), anEntry);
    anAsciiList += anEntry;
    anAsciiList += ",";
  }

//  timer3.Stop();
//  timer4.Start();

  anAsciiList.Trunc(anAsciiList.Length() - 1);
  anAsciiList += "]";

  anAsciiList = TCollection_AsciiString("\n") + anAsciiList;

  //The explode doesn't change object so no new function is requiered.
  aFunction = theShape->GetLastFunction();

  //Make a Python command
  TCollection_AsciiString aDescr(anAsciiList);
  TDF_Tool::Entry(theShape->GetEntry(), anEntry);
  aDescr += " = IShapesOperations.MakeExplode(";
  aDescr += (anEntry + ",");
  if (isSorted)
    aDescr += (TCollection_AsciiString(theShapeType) + ", 1)");
  else
    aDescr += (TCollection_AsciiString(theShapeType) + ", 0)");

  TCollection_AsciiString anOldDescr = aFunction->GetDescription();
  anOldDescr = anOldDescr + aDescr;
  aFunction->SetDescription(anOldDescr);

  SetErrorCode(OK);

//  timer4.Stop();

//  cout << "Explosure takes:" << endl;
//  timer1.Show();
//  cout << "Sorting takes:" << endl;
//  timer2.Show();
//  cout << "Sub-shapes addition takes:" << endl;
//  timer3.Show();
//  cout << "Update Description takes:" << endl;
//  timer4.Show();

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

  //The explode doesn't change object so no new function is required.
  Handle(GEOM_Function) aFunction = theShape->GetLastFunction();

  //Make a Python command
  TCollection_AsciiString aDescr
    ("\nlistSubShapeAllIDs = IShapesOperations.SubShapeAllIDs(");
  TCollection_AsciiString anEntry;
  TDF_Tool::Entry(theShape->GetEntry(), anEntry);
  aDescr += (anEntry + ",");
  if (isSorted)
    aDescr += (TCollection_AsciiString(theShapeType) + ", 1)");
  else
    aDescr += (TCollection_AsciiString(theShapeType) + ", 0)");

  TCollection_AsciiString anOldDescr = aFunction->GetDescription();
  anOldDescr = anOldDescr + aDescr;
  aFunction->SetDescription(anOldDescr);

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
  Handle(GEOM_Object) anObj = GetEngine()->AddSubShape(theMainShape, anArray);
  if (anObj.IsNull()) {
    SetErrorCode("Can not get a sub-shape with the given ID");
    return NULL;
  }

  //The GetSubShape() doesn't change object so no new function is requiered.
  Handle(GEOM_Function) aFunction = theMainShape->GetLastFunction();

  //Make a Python command
  TCollection_AsciiString aDescr ("\n");
  TCollection_AsciiString anEntry;
  TDF_Tool::Entry(anObj->GetEntry(), anEntry);
  aDescr += anEntry + " = IShapesOperations.GetSubShape(";
  TDF_Tool::Entry(theMainShape->GetEntry(), anEntry);
  aDescr += anEntry + ", ";
  aDescr += TCollection_AsciiString(theID) + ")";

  TCollection_AsciiString anOldDescr = aFunction->GetDescription();
  anOldDescr = anOldDescr + aDescr;
  aFunction->SetDescription(anOldDescr);

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
  TCollection_AsciiString anEntry, aDescr;
  TDF_Tool::Entry(aReversed->GetEntry(), anEntry);
  aDescr += anEntry;
  aDescr += " = IShapesOperations.ReverseShape(";
  TDF_Tool::Entry(theShape->GetEntry(), anEntry);
  aDescr += anEntry + ")";

  aFunction->SetDescription(aDescr);

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
  TCollection_AsciiString aDescr ("\nlistFreeFacesIDs = IShapesOperations.GetFreeFacesIDs(");
  TCollection_AsciiString anEntry;
  TDF_Tool::Entry(theShape->GetEntry(), anEntry);
  aDescr += (anEntry + ")");

  TCollection_AsciiString anOldDescr = aFunction->GetDescription();
  anOldDescr = anOldDescr + aDescr;
  aFunction->SetDescription(anOldDescr);

  SetErrorCode(OK);
  return aSeq;
}

//=============================================================================
/*!
 *  GetSharedShapes
 */
//=============================================================================
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

  TopTools_MapOfShape mapShape2;
  TopExp_Explorer exp (aShape2, TopAbs_ShapeEnum(theShapeType));
  for (; exp.More(); exp.Next()) {
    TopoDS_Shape aSS = exp.Current();
    if (mapShape2.Add(aSS) && mapShape1.Contains(aSS)) {
      anArray = new TColStd_HArray1OfInteger(1,1);
      anArray->SetValue(1, anIndices.FindIndex(aSS));
      anObj = GetEngine()->AddSubShape(theShape1, anArray);
      aSeq->Append(anObj);
    }
  }

  if (aSeq->IsEmpty()) {
    SetErrorCode("The given shapes have no shared sub-shapes of the requested type");
    return aSeq;
  }

  //The explode doesn't change object so no new function is required.
  Handle(GEOM_Function) aFunction = theShape1->GetLastFunction();

  //Make a Python command
  TCollection_AsciiString aDescr
    ("\nlistSharedShapes = IShapesOperations.GetSharedShapes(");
  TCollection_AsciiString anEntry;
  TDF_Tool::Entry(theShape1->GetEntry(), anEntry);
  aDescr += (anEntry + ",");
  TDF_Tool::Entry(theShape2->GetEntry(), anEntry);
  aDescr += (anEntry + ",");
  aDescr += TCollection_AsciiString(theShapeType) + ")";

  TCollection_AsciiString anOldDescr = aFunction->GetDescription();
  anOldDescr = anOldDescr + aDescr;
  aFunction->SetDescription(anOldDescr);

  SetErrorCode(OK);
  return aSeq;
}

//=============================================================================
/*!
 *  GetShapesOnPlane
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IShapesOperations::GetShapesOnPlane
                                          (Handle(GEOM_Object)    theShape,
                                           const Standard_Integer theShapeType,
                                           Handle(GEOM_Object)    thePlane)
{
  SetErrorCode(KO);

  if (theShape.IsNull() || thePlane.IsNull()) return NULL;

  TopoDS_Shape aShape = theShape->GetValue();
  TopoDS_Shape aPlane = thePlane->GetValue();

  if (aShape.IsNull() || aPlane.IsNull()) return NULL;

  TopAbs_ShapeEnum aShapeType = TopAbs_ShapeEnum(theShapeType);
  if (aShapeType != TopAbs_VERTEX &&
      aShapeType != TopAbs_EDGE &&
      aShapeType != TopAbs_FACE) {
    SetErrorCode("Not implemented for the given sub-shape type");
    return NULL;
  }

  //Get plane parameters
  if (aPlane.IsNull() || aPlane.ShapeType() != TopAbs_FACE) return NULL;
  TopoDS_Face aFace = TopoDS::Face(aPlane);
  Handle(Geom_Surface) surf = BRep_Tool::Surface(aFace);
  Handle(Geom_Plane) pln = Handle(Geom_Plane)::DownCast(surf);
  if (pln.IsNull()) {
    SetErrorCode("Not planar face given");
    return NULL;
  }
  const gp_Ax3 pos = pln->Position();
  const gp_Pnt loc = pos.Location();
  const gp_Dir dir = pos.Direction();

  //Find sub-shapes on the plane
  TopTools_ListOfShape listSS;
  TopTools_MapOfShape mapShapes;
  TopExp_Explorer exp (aShape, aShapeType);
  for (; exp.More(); exp.Next()) {
    TopoDS_Shape aSS = exp.Current();
    if (mapShapes.Add(aSS)) {
      switch (aShapeType) {
      case TopAbs_VERTEX:
        {
          TopoDS_Vertex aV = TopoDS::Vertex(aSS);
          gp_Pnt aP = BRep_Tool::Pnt(aV);
          gp_Vec vecToLoc (aP, loc);
          if (vecToLoc.IsNormal(dir, Precision::Angular())) {
            listSS.Append(aSS);
          }
        }
        break;
      case TopAbs_EDGE:
        {
          TopoDS_Edge anEdge = TopoDS::Edge(aSS);
          Standard_Real f, l;
          Handle(Geom2d_Curve) PC;
          Handle(Geom_Surface) cur_surf;
          TopLoc_Location L;
          Standard_Integer i = 0;

          // iterate on the surfaces of the edge
          while (Standard_True) {
            i++;
            BRep_Tool::CurveOnSurface(anEdge, PC , cur_surf, L, f, l, i);
            if (cur_surf.IsNull()) break;

            Handle(Geom_Plane) cur_pln = Handle(Geom_Plane)::DownCast(cur_surf);
            if (!cur_pln.IsNull()) {
              const gp_Ax3 cur_pos = cur_pln->Position();
              const gp_Pnt cur_loc = cur_pos.Location();
              const gp_Dir cur_dir = cur_pos.Direction();
              gp_Vec vecToLoc (cur_loc, loc);
              if (vecToLoc.IsNormal(dir, Precision::Angular()) &&
                  cur_dir.IsParallel(dir, Precision::Angular())) {
                listSS.Append(aSS);
              }
            }
          }
        }
        break;
      case TopAbs_FACE:
        {
          TopoDS_Face aF = TopoDS::Face(aSS);
          Handle(Geom_Surface) cur_surf = BRep_Tool::Surface(aF);
          Handle(Geom_Plane) cur_pln = Handle(Geom_Plane)::DownCast(cur_surf);
          if (!cur_pln.IsNull()) {
            const gp_Ax3 cur_pos = cur_pln->Position();
            const gp_Pnt cur_loc = cur_pos.Location();
            const gp_Dir cur_dir = cur_pos.Direction();
            gp_Vec vecToLoc (cur_loc, loc);
            if (vecToLoc.IsNormal(dir, Precision::Angular()) &&
                cur_dir.IsParallel(dir, Precision::Angular())) {
              listSS.Append(aSS);
            }
          }
        }
        break;
      default:
        break;
      }
    }
  }

  if (listSS.Extent() < 1) {
    SetErrorCode("Not a single sub-shape of the requested type found on the given plane");
    return NULL;
  }

  //Fill array of indices
  TopTools_IndexedMapOfShape anIndices;
  TopExp::MapShapes(aShape, anIndices);

  Handle(TColStd_HArray1OfInteger) anArray =
    new TColStd_HArray1OfInteger (1, listSS.Extent());
  TopTools_ListIteratorOfListOfShape itSub (listSS);
  for (int index = 1; itSub.More(); itSub.Next(), ++index) {
    int id = anIndices.FindIndex(itSub.Value());
    anArray->SetValue(index, id);
  }
  
  //Add a new group object
  Handle(GEOM_Object) aGroup = GetEngine()->AddSubShape(theShape, anArray);

  //Set a GROUP type
  aGroup->SetType(GEOM_GROUP);

  //Set a sub shape type
  TDF_Label aFreeLabel = aGroup->GetFreeLabel();
  TDataStd_Integer::Set(aFreeLabel, (Standard_Integer)theShapeType);
 
  //Make a Python command
  TCollection_AsciiString anEntry, aDescr;
  TDF_Tool::Entry(aGroup->GetEntry(), anEntry);
  aDescr += anEntry;
  aDescr += " = IShapesOperations.GetShapesOnPlane(";
  TDF_Tool::Entry(theShape->GetEntry(), anEntry);
  aDescr += anEntry + TCollection_AsciiString(theShapeType) + ",";
  TDF_Tool::Entry(thePlane->GetEntry(), anEntry);
  aDescr += anEntry + ")";

  Handle(GEOM_Function) aFunction = aGroup->GetFunction(1);
  aFunction->SetDescription(aDescr);

  SetErrorCode(OK);
  return aGroup;
}

//=============================================================================
/*!
 *  GetShapesOnCylinder
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IShapesOperations::GetShapesOnCylinder
                                          (Handle(GEOM_Object)    theShape,
                                           const Standard_Integer theShapeType,
                                           Handle(GEOM_Object)    theAxis,
                                           const Standard_Real    theRadius)
{
  SetErrorCode(KO);

  if (theShape.IsNull() || theAxis.IsNull()) return NULL;

  TopoDS_Shape aShape = theShape->GetValue();
  TopoDS_Shape anAxis = theAxis->GetValue();

  if (aShape.IsNull() || anAxis.IsNull()) return NULL;

  TopAbs_ShapeEnum aShapeType = TopAbs_ShapeEnum(theShapeType);
  if (aShapeType != TopAbs_VERTEX &&
      aShapeType != TopAbs_EDGE &&
      aShapeType != TopAbs_FACE) {
    SetErrorCode("Not implemented for the given sub-shape type");
    return NULL;
  }

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
  gp_Pnt loc = BRep_Tool::Pnt(V1);
  gp_Vec aVec (loc, BRep_Tool::Pnt(V2));
  gp_Dir dir (aVec);
  gp_Lin aLin (loc, aVec);

  //Find sub-shapes on the cylinder
  TopTools_ListOfShape listSS;
  TopTools_MapOfShape mapShapes;
  TopExp_Explorer exp (aShape, aShapeType);
  for (; exp.More(); exp.Next()) {
    TopoDS_Shape aSS = exp.Current();
    if (mapShapes.Add(aSS)) {
      switch (aShapeType) {
      case TopAbs_VERTEX:
        {
          TopoDS_Vertex aV = TopoDS::Vertex(aSS);
          gp_Pnt aP = BRep_Tool::Pnt(aV);
          if (Abs(aLin.Distance(aP) - theRadius) < Precision::Confusion()) {
            listSS.Append(aSS);
          }
        }
        break;
      case TopAbs_EDGE:
        {
          TopoDS_Edge anEdge = TopoDS::Edge(aSS);
          Standard_Real f, l;
          Handle(Geom2d_Curve) PC;
          Handle(Geom_Surface) cur_surf;
          TopLoc_Location L;
          Standard_Integer i = 0;

          // iterate on the surfaces of the edge
          while (Standard_True) {
            i++;
            BRep_Tool::CurveOnSurface(anEdge, PC , cur_surf, L, f, l, i);
            if (cur_surf.IsNull()) break;

            Handle(Geom_CylindricalSurface) cur_cyl =
              Handle(Geom_CylindricalSurface)::DownCast(cur_surf);
            if (!cur_cyl.IsNull()) {
              const gp_Ax3 cur_pos = cur_cyl->Position();
              const gp_Pnt cur_loc = cur_pos.Location();
              const gp_Dir cur_dir = cur_pos.Direction();
              const Standard_Real cur_rad = cur_cyl->Radius();
              gp_Vec vecToLoc (cur_loc, loc);
              if (vecToLoc.IsParallel(dir, Precision::Angular()) &&
                  cur_dir.IsParallel(dir, Precision::Angular()) &&
                  Abs(cur_rad - theRadius) < Precision::Confusion()) {
                listSS.Append(aSS);
              }
            }
          }
        }
        break;
      case TopAbs_FACE:
        {
          TopoDS_Face aF = TopoDS::Face(aSS);
          Handle(Geom_Surface) cur_surf = BRep_Tool::Surface(aF);
          Handle(Geom_CylindricalSurface) cur_cyl =
            Handle(Geom_CylindricalSurface)::DownCast(cur_surf);
          if (!cur_cyl.IsNull()) {
            const gp_Ax3 cur_pos = cur_cyl->Position();
            const gp_Pnt cur_loc = cur_pos.Location();
            const gp_Dir cur_dir = cur_pos.Direction();
            const Standard_Real cur_rad = cur_cyl->Radius();
            gp_Vec vecToLoc (cur_loc, loc);
            if (vecToLoc.IsParallel(dir, Precision::Angular()) &&
                cur_dir.IsParallel(dir, Precision::Angular()) &&
                Abs(cur_rad - theRadius) < Precision::Confusion()) {
              listSS.Append(aSS);
            }
          }
        }
        break;
      default:
        break;
      }
    }
  }

  if (listSS.Extent() < 1) {
    SetErrorCode("Not a single sub-shape of the requested type found on the given cylinder");
    return NULL;
  }

  //Fill array of indices
  TopTools_IndexedMapOfShape anIndices;
  TopExp::MapShapes(aShape, anIndices);

  Handle(TColStd_HArray1OfInteger) anArray =
    new TColStd_HArray1OfInteger (1, listSS.Extent());
  TopTools_ListIteratorOfListOfShape itSub (listSS);
  for (int index = 1; itSub.More(); itSub.Next(), ++index) {
    int id = anIndices.FindIndex(itSub.Value());
    anArray->SetValue(index, id);
  }
  
  //Add a new group object
  Handle(GEOM_Object) aGroup = GetEngine()->AddSubShape(theShape, anArray);

  //Set a GROUP type
  aGroup->SetType(GEOM_GROUP);

  //Set a sub shape type
  TDF_Label aFreeLabel = aGroup->GetFreeLabel();
  TDataStd_Integer::Set(aFreeLabel, (Standard_Integer)theShapeType);
 
  //Make a Python command
  TCollection_AsciiString anEntry, aDescr;
  TDF_Tool::Entry(aGroup->GetEntry(), anEntry);
  aDescr += anEntry;
  aDescr += " = IShapesOperations.GetShapesOnCylinder(";
  TDF_Tool::Entry(theShape->GetEntry(), anEntry);
  aDescr += anEntry + TCollection_AsciiString(theShapeType) + ",";
  TDF_Tool::Entry(theAxis->GetEntry(), anEntry);
  aDescr += anEntry + TCollection_AsciiString(theRadius) + ")";

  Handle(GEOM_Function) aFunction = aGroup->GetFunction(1);
  aFunction->SetDescription(aDescr);

  SetErrorCode(OK);
  return aGroup;
}

//=============================================================================
/*!
 *  GetShapesOnSphere
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IShapesOperations::GetShapesOnSphere
                                          (Handle(GEOM_Object)    theShape,
                                           const Standard_Integer theShapeType,
                                           Handle(GEOM_Object)    theCenter,
                                           const Standard_Real    theRadius)
{
  SetErrorCode(KO);

  if (theShape.IsNull() || theCenter.IsNull()) return NULL;

  TopoDS_Shape aShape  = theShape->GetValue();
  TopoDS_Shape aCenter = theCenter->GetValue();

  if (aShape.IsNull() || aCenter.IsNull()) return NULL;

  TopAbs_ShapeEnum aShapeType = TopAbs_ShapeEnum(theShapeType);
  if (aShapeType != TopAbs_VERTEX &&
      aShapeType != TopAbs_EDGE &&
      aShapeType != TopAbs_FACE) {
    SetErrorCode("Not implemented for the given sub-shape type");
    return NULL;
  }

  //Center of the sphere
  if (aCenter.ShapeType() != TopAbs_VERTEX) return NULL;
  gp_Pnt aC = BRep_Tool::Pnt(TopoDS::Vertex(aCenter));

  //Find sub-shapes on the sphere
  TopTools_ListOfShape listSS;
  TopTools_MapOfShape mapShapes;
  TopExp_Explorer exp (aShape, aShapeType);
  for (; exp.More(); exp.Next()) {
    TopoDS_Shape aSS = exp.Current();
    if (mapShapes.Add(aSS)) {
      switch (aShapeType) {
      case TopAbs_VERTEX:
        {
          TopoDS_Vertex aV = TopoDS::Vertex(aSS);
          gp_Pnt aP = BRep_Tool::Pnt(aV);
          if (Abs(aP.Distance(aC) - theRadius) < Precision::Confusion()) {
            listSS.Append(aSS);
          }
        }
        break;
      case TopAbs_EDGE:
        {
          TopoDS_Edge anEdge = TopoDS::Edge(aSS);
          Standard_Real f, l;
          Handle(Geom2d_Curve) PC;
          Handle(Geom_Surface) cur_surf;
          TopLoc_Location L;
          Standard_Integer i = 0;

          // iterate on the surfaces of the edge
          while (Standard_True) {
            i++;
            BRep_Tool::CurveOnSurface(anEdge, PC , cur_surf, L, f, l, i);
            if (cur_surf.IsNull()) break;

            Handle(Geom_SphericalSurface) cur_sph =
              Handle(Geom_SphericalSurface)::DownCast(cur_surf);
            if (!cur_sph.IsNull()) {
              const gp_Ax3 cur_pos = cur_sph->Position();
              const gp_Pnt cur_loc = cur_pos.Location();
              const Standard_Real cur_rad = cur_sph->Radius();
              if (cur_loc.Distance(aC) < Precision::Confusion() &&
                  Abs(cur_rad - theRadius) < Precision::Confusion()) {
                listSS.Append(aSS);
              }
            }
          }
        }
        break;
      case TopAbs_FACE:
        {
          TopoDS_Face aF = TopoDS::Face(aSS);
          Handle(Geom_Surface) cur_surf = BRep_Tool::Surface(aF);
          Handle(Geom_SphericalSurface) cur_sph =
            Handle(Geom_SphericalSurface)::DownCast(cur_surf);
          if (!cur_sph.IsNull()) {
            const gp_Ax3 cur_pos = cur_sph->Position();
            const gp_Pnt cur_loc = cur_pos.Location();
            const Standard_Real cur_rad = cur_sph->Radius();
            if (cur_loc.Distance(aC) < Precision::Confusion() &&
                Abs(cur_rad - theRadius) < Precision::Confusion()) {
              listSS.Append(aSS);
            }
          }
        }
        break;
      default:
        break;
      }
    }
  }

  if (listSS.Extent() < 1) {
    SetErrorCode("Not a single sub-shape of the requested type found on the given sphere");
    return NULL;
  }

  //Fill array of indices
  TopTools_IndexedMapOfShape anIndices;
  TopExp::MapShapes(aShape, anIndices);

  Handle(TColStd_HArray1OfInteger) anArray =
    new TColStd_HArray1OfInteger (1, listSS.Extent());
  TopTools_ListIteratorOfListOfShape itSub (listSS);
  for (int index = 1; itSub.More(); itSub.Next(), ++index) {
    int id = anIndices.FindIndex(itSub.Value());
    anArray->SetValue(index, id);
  }
  
  //Add a new group object
  Handle(GEOM_Object) aGroup = GetEngine()->AddSubShape(theShape, anArray);

  //Set a GROUP type
  aGroup->SetType(GEOM_GROUP);

  //Set a sub shape type
  TDF_Label aFreeLabel = aGroup->GetFreeLabel();
  TDataStd_Integer::Set(aFreeLabel, (Standard_Integer)theShapeType);
 
  //Make a Python command
  TCollection_AsciiString anEntry, aDescr;
  TDF_Tool::Entry(aGroup->GetEntry(), anEntry);
  aDescr += anEntry;
  aDescr += " = IShapesOperations.GetShapesOnSphere(";
  TDF_Tool::Entry(theShape->GetEntry(), anEntry);
  aDescr += anEntry + TCollection_AsciiString(theShapeType) + ",";
  TDF_Tool::Entry(theCenter->GetEntry(), anEntry);
  aDescr += anEntry + TCollection_AsciiString(theRadius) + ")";

  Handle(GEOM_Function) aFunction = aGroup->GetFunction(1);
  aFunction->SetDescription(aDescr);

  SetErrorCode(OK);
  return aGroup;
}

//=============================================================================
/*!
 *  GetInPlace
 */
//=============================================================================
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
  TopTools_IndexedMapOfShape anIndices;
  TopExp::MapShapes(aWhere, anIndices);

//  Handle(TColStd_HArray1OfInteger) anArray =
//    new TColStd_HArray1OfInteger (1, listSS.Extent());
//  TopTools_ListIteratorOfListOfShape itSub (listSS);
//  for (int index = 1; itSub.More(); itSub.Next(), ++index) {
//    int id = anIndices.FindIndex(itSub.Value());
//    anArray->SetValue(index, id);
//  }
//  
//  //Add a new group object
//  Handle(GEOM_Object) aGroup = GetEngine()->AddSubShape(theShape, anArray);
//
//  //Set a GROUP type
//  aGroup->SetType(GEOM_GROUP);
//
//  //Set a sub shape type
//  TDF_Label aFreeLabel = aGroup->GetFreeLabel();
//  TDataStd_Integer::Set(aFreeLabel, (Standard_Integer)theShapeType);
// 
//  //Make a Python command
//  TCollection_AsciiString anEntry, aDescr;
//  TDF_Tool::Entry(aGroup->GetEntry(), anEntry);
//  aDescr += anEntry;
//  aDescr += " = IShapesOperations.GetInPlace(";
//  TDF_Tool::Entry(theShapeWhere->GetEntry(), anEntry);
//  aDescr += anEntry + ",";
//  TDF_Tool::Entry(theShapeWhat->GetEntry(), anEntry);
//  aDescr += anEntry + ")";
//
//  Handle(GEOM_Function) aFunction = aGroup->GetFunction(1);
//  aFunction->SetDescription(aDescr);

//  SetErrorCode(OK);
//  return aGroup;
  SetErrorCode("Not implemented");
  return NULL;
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
