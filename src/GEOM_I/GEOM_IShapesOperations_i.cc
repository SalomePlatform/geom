using namespace std;

#include "GEOM_IShapesOperations_i.hh"

#include "utilities.h"
#include "OpUtil.hxx"
#include "Utils_ExceptHandlers.hxx"

#include "GEOM_Engine.hxx"
#include "GEOM_Object.hxx"

#include <TColStd_HSequenceOfTransient.hxx>
#include <TColStd_HArray1OfInteger.hxx>

//=============================================================================
/*!
 *   constructor:
 */
//=============================================================================
GEOM_IShapesOperations_i::GEOM_IShapesOperations_i (PortableServer::POA_ptr thePOA,
						    GEOM::GEOM_Gen_ptr theEngine,
						    ::GEOMImpl_IShapesOperations* theImpl)
:GEOM_IOperations_i(thePOA, theEngine, theImpl)
{
  MESSAGE("GEOM_IShapesOperations_i::GEOM_IShapesOperations_i");
}

//=============================================================================
/*!
 *  destructor
 */
//=============================================================================
GEOM_IShapesOperations_i::~GEOM_IShapesOperations_i()
{
  MESSAGE("GEOM_IShapesOperations_i::~GEOM_IShapesOperations_i");
}


//=============================================================================
/*!
 *  MakeEdge
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IShapesOperations_i::MakeEdge
                      (GEOM::GEOM_Object_ptr thePnt1, GEOM::GEOM_Object_ptr thePnt2)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  if (thePnt1 == NULL || thePnt2 == NULL) return aGEOMObject._retn();

  //Get the reference points
  Handle(GEOM_Object) aPnt1 = GetOperations()->GetEngine()->GetObject
    (thePnt1->GetStudyID(), thePnt1->GetEntry());
  Handle(GEOM_Object) aPnt2 = GetOperations()->GetEngine()->GetObject
    (thePnt2->GetStudyID(), thePnt2->GetEntry());

  if (aPnt1.IsNull() || aPnt2.IsNull()) return aGEOMObject._retn();

  //Create the Edge
  Handle(GEOM_Object) anObject = GetOperations()->MakeEdge(aPnt1, aPnt2);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeWire
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IShapesOperations_i::MakeWire
                                      (const GEOM::ListOfGO& theEdgesAndWires)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  int ind, aLen;
  list<Handle(GEOM_Object)> aShapes;

  //Get the shapes
  aLen = theEdgesAndWires.length();
  for (ind = 0; ind < aLen; ind++) {
    if (theEdgesAndWires[ind] == NULL) return aGEOMObject._retn();
    Handle(GEOM_Object) aSh = GetOperations()->GetEngine()->GetObject
      (theEdgesAndWires[ind]->GetStudyID(), theEdgesAndWires[ind]->GetEntry());
    if (aSh.IsNull()) return aGEOMObject._retn();
    aShapes.push_back(aSh);
  }

  // Make Solid
  Handle(GEOM_Object) anObject =
    GetOperations()->MakeWire(aShapes);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeFace
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IShapesOperations_i::MakeFace
                      (GEOM::GEOM_Object_ptr theWire, CORBA::Boolean isPlanarWanted)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  if (theWire == NULL) return aGEOMObject._retn();

  //Get the reference wire
  Handle(GEOM_Object) aWire = GetOperations()->GetEngine()->GetObject
    (theWire->GetStudyID(), theWire->GetEntry());

  if (aWire.IsNull()) return aGEOMObject._retn();

  //Create the Face
  Handle(GEOM_Object) anObject = GetOperations()->MakeFace(aWire, isPlanarWanted);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeFaceWires
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IShapesOperations_i::MakeFaceWires
                                         (const GEOM::ListOfGO& theWires,
					  CORBA::Boolean        isPlanarWanted)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  int ind, aLen;
  list<Handle(GEOM_Object)> aShapes;

  //Get the shapes
  aLen = theWires.length();
  for (ind = 0; ind < aLen; ind++) {
    if (theWires[ind] == NULL) return aGEOMObject._retn();
    Handle(GEOM_Object) aSh = GetOperations()->GetEngine()->GetObject
      (theWires[ind]->GetStudyID(), theWires[ind]->GetEntry());
    if (aSh.IsNull()) return aGEOMObject._retn();
    aShapes.push_back(aSh);
  }

  // Make Face
  Handle(GEOM_Object) anObject =
    GetOperations()->MakeFaceWires(aShapes, isPlanarWanted);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeShell
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IShapesOperations_i::MakeShell
                                      (const GEOM::ListOfGO& theFacesAndShells)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  int ind, aLen;
  list<Handle(GEOM_Object)> aShapes;

  //Get the shapes
  aLen = theFacesAndShells.length();
  for (ind = 0; ind < aLen; ind++) {
    if (theFacesAndShells[ind] == NULL) return aGEOMObject._retn();
    Handle(GEOM_Object) aSh = GetOperations()->GetEngine()->GetObject
      (theFacesAndShells[ind]->GetStudyID(), theFacesAndShells[ind]->GetEntry());
    if (aSh.IsNull()) return aGEOMObject._retn();
    aShapes.push_back(aSh);
  }

  // Make Solid
  Handle(GEOM_Object) anObject =
    GetOperations()->MakeShell(aShapes);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeSolidShell
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IShapesOperations_i::MakeSolidShell
                                                (GEOM::GEOM_Object_ptr theShell)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  if (theShell == NULL) return aGEOMObject._retn();

  //Get the reference objects
  Handle(GEOM_Object) aShell = GetOperations()->GetEngine()->GetObject
    (theShell->GetStudyID(), theShell->GetEntry());

  if (aShell.IsNull()) return aGEOMObject._retn();

  //Create the Solid
  Handle(GEOM_Object) anObject = GetOperations()->MakeSolidShell(aShell);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeSolidShells
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IShapesOperations_i::MakeSolidShells
                                      (const GEOM::ListOfGO& theShells)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  int ind, aLen;
  list<Handle(GEOM_Object)> aShapes;

  //Get the shapes
  aLen = theShells.length();
  for (ind = 0; ind < aLen; ind++) {
    if (theShells[ind] == NULL) return aGEOMObject._retn();
    Handle(GEOM_Object) aSh = GetOperations()->GetEngine()->GetObject
      (theShells[ind]->GetStudyID(), theShells[ind]->GetEntry());
    if (aSh.IsNull()) return aGEOMObject._retn();
    aShapes.push_back(aSh);
  }

  // Make Solid
  Handle(GEOM_Object) anObject =
    GetOperations()->MakeSolidShells(aShapes);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeCompound
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IShapesOperations_i::MakeCompound
                                      (const GEOM::ListOfGO& theShapes)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  int ind, aLen;
  list<Handle(GEOM_Object)> aShapes;

  //Get the shapes
  aLen = theShapes.length();
  for (ind = 0; ind < aLen; ind++) {
    if (theShapes[ind] == NULL) return aGEOMObject._retn();
    Handle(GEOM_Object) aSh = GetOperations()->GetEngine()->GetObject
      (theShapes[ind]->GetStudyID(), theShapes[ind]->GetEntry());
    if (aSh.IsNull()) return aGEOMObject._retn();
    aShapes.push_back(aSh);
  }

  // Make Solid
  Handle(GEOM_Object) anObject =
    GetOperations()->MakeCompound(aShapes);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeGlueFaces
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IShapesOperations_i::MakeGlueFaces
                                           (GEOM::GEOM_Object_ptr theShape,
					    const CORBA::Double   theTolerance)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  if (theShape == NULL) return aGEOMObject._retn();

  //Get the reference objects
  Handle(GEOM_Object) aShape = GetOperations()->GetEngine()->GetObject
    (theShape->GetStudyID(), theShape->GetEntry());

  if (aShape.IsNull()) return aGEOMObject._retn();

  //Perform the glueing
  Handle(GEOM_Object) anObject =
    GetOperations()->MakeGlueFaces(aShape, theTolerance);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeExplode
 */
//=============================================================================
GEOM::ListOfGO* GEOM_IShapesOperations_i::MakeExplode (GEOM::GEOM_Object_ptr theShape,
						       const CORBA::Long     theShapeType,
						       const CORBA::Boolean  isSorted)
{
  GEOM::ListOfGO_var aSeq = new GEOM::ListOfGO;
  if (theShape == NULL)  return aSeq._retn();
  Handle(GEOM_Object) aShape =   GetOperations()->GetEngine()->GetObject(theShape->GetStudyID(), theShape->GetEntry());
  Handle(TColStd_HSequenceOfTransient) aHSeq = GetOperations()->MakeExplode(aShape, theShapeType, isSorted);
  if (!GetOperations()->IsDone() || aHSeq.IsNull()) return aSeq._retn();

  Standard_Integer aLength = aHSeq->Length();
  aSeq->length(aLength);
  for (Standard_Integer i = 1; i <= aLength; i++)
    aSeq[i-1] = GetObject(Handle(GEOM_Object)::DownCast(aHSeq->Value(i)));

  return aSeq._retn();
}

//=============================================================================
/*!
 *  GetSubShape
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IShapesOperations_i::GetSubShape
                                           (GEOM::GEOM_Object_ptr theMainShape,
					    const CORBA::Long     theID)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  if (theMainShape == NULL) return aGEOMObject._retn();

  //Get the reference objects
  Handle(GEOM_Object) aShape = GetOperations()->GetEngine()->GetObject
    (theMainShape->GetStudyID(), theMainShape->GetEntry());

  if (aShape.IsNull()) return aGEOMObject._retn();

  Handle(GEOM_Object) anObject = GetOperations()->GetSubShape(aShape, theID);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  NumberOfFaces
 */
//=============================================================================
CORBA::Long GEOM_IShapesOperations_i::NumberOfFaces (GEOM::GEOM_Object_ptr theShape)
{
  if (theShape == NULL) return -1;

  Handle(GEOM_Object) aShape = GetOperations()->GetEngine()->GetObject
    (theShape->GetStudyID(), theShape->GetEntry());

  CORBA::Long aNb = GetOperations()->NumberOfFaces(aShape);
  if (!GetOperations()->IsDone()) return -1;

  return aNb;
}

//=============================================================================
/*!
 *  NumberOfEdges
 */
//=============================================================================
CORBA::Long GEOM_IShapesOperations_i::NumberOfEdges (GEOM::GEOM_Object_ptr theShape)
{
  if (theShape == NULL) return -1;

  Handle(GEOM_Object) aShape = GetOperations()->GetEngine()->GetObject
    (theShape->GetStudyID(), theShape->GetEntry());

  CORBA::Long aNb = GetOperations()->NumberOfEdges(aShape);
  if (!GetOperations()->IsDone()) return -1;

  return aNb;
}

//=============================================================================
/*!
 *  ReverseOrientation
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IShapesOperations_i::ChangeOrientation
                                                (GEOM::GEOM_Object_ptr theShape)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  if (theShape == NULL) return aGEOMObject._retn();

  //Get the reference objects
  Handle(GEOM_Object) aShape = GetOperations()->GetEngine()->GetObject
    (theShape->GetStudyID(), theShape->GetEntry());

  if (aShape.IsNull()) return aGEOMObject._retn();

  //Create the Solid
  Handle(GEOM_Object) anObject = GetOperations()->ReverseShape(aShape);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

