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
                      (GEOM::GEOM_Object_ptr theWire,
		       const CORBA::Boolean  isPlanarWanted)
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
					  const CORBA::Boolean  isPlanarWanted)
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

  //Perform the gluing
  Handle(GEOM_Object) anObject =
    GetOperations()->MakeGlueFaces(aShape, theTolerance);
  //if (!GetOperations()->IsDone() || anObject.IsNull())
  // to allow warning
  if (anObject.IsNull())
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
  if (theShape == NULL) return aSeq._retn();

  Handle(GEOM_Object) aShape = GetOperations()->GetEngine()->GetObject
    (theShape->GetStudyID(), theShape->GetEntry());

  Handle(TColStd_HSequenceOfTransient) aHSeq =
    GetOperations()->MakeExplode(aShape, theShapeType, isSorted);
  if (!GetOperations()->IsDone() || aHSeq.IsNull())
    return aSeq._retn();

  Standard_Integer aLength = aHSeq->Length();
  aSeq->length(aLength);
  for (Standard_Integer i = 1; i <= aLength; i++)
    aSeq[i-1] = GetObject(Handle(GEOM_Object)::DownCast(aHSeq->Value(i)));

  return aSeq._retn();
}

//=============================================================================
/*!
 *  SubShapeAllIDs
 */
//=============================================================================
GEOM::ListOfLong* GEOM_IShapesOperations_i::SubShapeAllIDs (GEOM::GEOM_Object_ptr theShape,
							    const CORBA::Long     theShapeType,
							    const CORBA::Boolean  isSorted)
{
  GEOM::ListOfLong_var aSeq = new GEOM::ListOfLong;
  if (theShape == NULL) return aSeq._retn();

  Handle(GEOM_Object) aShape = GetOperations()->GetEngine()->GetObject
    (theShape->GetStudyID(), theShape->GetEntry());

  Handle(TColStd_HSequenceOfInteger) aHSeq =
    GetOperations()->SubShapeAllIDs(aShape, theShapeType, isSorted);
  if (!GetOperations()->IsDone() || aHSeq.IsNull()) return aSeq._retn();

  Standard_Integer aLength = aHSeq->Length();
  aSeq->length(aLength);
  for (Standard_Integer i = 1; i <= aLength; i++)
    aSeq[i-1] = aHSeq->Value(i);

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
 *  ChangeOrientation
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

//=============================================================================
/*!
 *  GetFreeFacesIDs
 */
//=============================================================================
GEOM::ListOfLong* GEOM_IShapesOperations_i::GetFreeFacesIDs (GEOM::GEOM_Object_ptr theShape)
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  GEOM::ListOfLong_var aSeq = new GEOM::ListOfLong;
  if (theShape == NULL) return aSeq._retn();

  Handle(GEOM_Object) aShape = GetOperations()->GetEngine()->GetObject
    (theShape->GetStudyID(), theShape->GetEntry());

  Handle(TColStd_HSequenceOfInteger) aHSeq =
    GetOperations()->GetFreeFacesIDs(aShape);
  if (!GetOperations()->IsDone() || aHSeq.IsNull()) return aSeq._retn();

  Standard_Integer aLength = aHSeq->Length();
  aSeq->length(aLength);
  for (Standard_Integer i = 1; i <= aLength; i++)
    aSeq[i-1] = aHSeq->Value(i);

  return aSeq._retn();
}

//=============================================================================
/*!
 *  GetSharedShapes
 */
//=============================================================================
GEOM::ListOfGO* GEOM_IShapesOperations_i::GetSharedShapes
                                          (GEOM::GEOM_Object_ptr theShape1,
					   GEOM::GEOM_Object_ptr theShape2,
					   const CORBA::Long     theShapeType)
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  GEOM::ListOfGO_var aSeq = new GEOM::ListOfGO;
  if (theShape1 == NULL ||
      theShape2 == NULL) return aSeq._retn();

  Handle(GEOM_Object) aShape1 = GetOperations()->GetEngine()->GetObject
    (theShape1->GetStudyID(), theShape1->GetEntry());
  Handle(GEOM_Object) aShape2 = GetOperations()->GetEngine()->GetObject
    (theShape2->GetStudyID(), theShape2->GetEntry());

  if (aShape1.IsNull() ||
      aShape2.IsNull()) return aSeq._retn();

  Handle(TColStd_HSequenceOfTransient) aHSeq =
    GetOperations()->GetSharedShapes(aShape1, aShape2, theShapeType);
  if (!GetOperations()->IsDone() || aHSeq.IsNull())
    return aSeq._retn();

  Standard_Integer aLength = aHSeq->Length();
  aSeq->length(aLength);
  for (Standard_Integer i = 1; i <= aLength; i++)
    aSeq[i-1] = GetObject(Handle(GEOM_Object)::DownCast(aHSeq->Value(i)));

  return aSeq._retn();
}

static GEOMAlgo_State ShapeState (const GEOM::shape_state theState)
{
  GEOMAlgo_State aState = GEOMAlgo_ST_UNKNOWN;

  switch (theState) {
  case GEOM::ST_ON:
    aState = GEOMAlgo_ST_ON;
    break;
  case GEOM::ST_OUT:
    aState = GEOMAlgo_ST_OUT;
    break;
  case GEOM::ST_ONOUT:
    aState = GEOMAlgo_ST_ONOUT;
    break;
  case GEOM::ST_IN:
    aState = GEOMAlgo_ST_IN;
    break;
  case GEOM::ST_ONIN:
    aState = GEOMAlgo_ST_ONIN;
    break;
  default:
    break;
  }

  return aState;
}

//=============================================================================
/*!
 *  GetShapesOnPlane
 */
//=============================================================================
GEOM::ListOfGO* GEOM_IShapesOperations_i::GetShapesOnPlane
                                                (GEOM::GEOM_Object_ptr   theShape,
						 const CORBA::Long       theShapeType,
						 GEOM::GEOM_Object_ptr   theAx1,
						 const GEOM::shape_state theState)
{
  GEOM::ListOfGO_var aSeq = new GEOM::ListOfGO;

  //Set a not done flag
  GetOperations()->SetNotDone();

  if (theShape == NULL || theAx1 == NULL) return aSeq._retn();

  //Get the reference objects
  Handle(GEOM_Object) aShape = GetOperations()->GetEngine()->GetObject
    (theShape->GetStudyID(), theShape->GetEntry());
  Handle(GEOM_Object) anAx1 = GetOperations()->GetEngine()->GetObject
    (theAx1->GetStudyID(), theAx1->GetEntry());

  if (aShape.IsNull() || anAx1.IsNull()) return aSeq._retn();

  //Get Shapes On Plane
  Handle(TColStd_HSequenceOfTransient) aHSeq =
    GetOperations()->GetShapesOnPlane(aShape, theShapeType, anAx1, ShapeState(theState));
  if (!GetOperations()->IsDone() || aHSeq.IsNull())
    return aSeq._retn();

  Standard_Integer aLength = aHSeq->Length();
  aSeq->length(aLength);
  for (Standard_Integer i = 1; i <= aLength; i++)
    aSeq[i-1] = GetObject(Handle(GEOM_Object)::DownCast(aHSeq->Value(i)));

  return aSeq._retn();
}

//=============================================================================
/*!
 *  GetShapesOnCylinder
 */
//=============================================================================
GEOM::ListOfGO* GEOM_IShapesOperations_i::GetShapesOnCylinder
                                                (GEOM::GEOM_Object_ptr   theShape,
						 const CORBA::Long       theShapeType,
						 GEOM::GEOM_Object_ptr   theAxis,
						 const CORBA::Double     theRadius,
						 const GEOM::shape_state theState)
{
  GEOM::ListOfGO_var aSeq = new GEOM::ListOfGO;

  //Set a not done flag
  GetOperations()->SetNotDone();

  if (theShape == NULL || theAxis == NULL) return aSeq._retn();

  //Get the reference objects
  Handle(GEOM_Object) aShape = GetOperations()->GetEngine()->GetObject
    (theShape->GetStudyID(), theShape->GetEntry());
  Handle(GEOM_Object) anAxis = GetOperations()->GetEngine()->GetObject
    (theAxis->GetStudyID(), theAxis->GetEntry());

  if (aShape.IsNull() || anAxis.IsNull()) return aSeq._retn();

  //Get Shapes On Cylinder
  Handle(TColStd_HSequenceOfTransient) aHSeq = GetOperations()->GetShapesOnCylinder
    (aShape, theShapeType, anAxis, theRadius, ShapeState(theState));
  if (!GetOperations()->IsDone() || aHSeq.IsNull())
    return aSeq._retn();

  Standard_Integer aLength = aHSeq->Length();
  aSeq->length(aLength);
  for (Standard_Integer i = 1; i <= aLength; i++)
    aSeq[i-1] = GetObject(Handle(GEOM_Object)::DownCast(aHSeq->Value(i)));

  return aSeq._retn();
}

//=============================================================================
/*!
 *  GetShapesOnSphere
 */
//=============================================================================
GEOM::ListOfGO* GEOM_IShapesOperations_i::GetShapesOnSphere
                                                (GEOM::GEOM_Object_ptr   theShape,
						 const CORBA::Long       theShapeType,
						 GEOM::GEOM_Object_ptr   theCenter,
						 const CORBA::Double     theRadius,
						 const GEOM::shape_state theState)
{
  GEOM::ListOfGO_var aSeq = new GEOM::ListOfGO;

  //Set a not done flag
  GetOperations()->SetNotDone();

  if (theShape == NULL || theCenter == NULL) return aSeq._retn();

  //Get the reference objects
  Handle(GEOM_Object) aShape = GetOperations()->GetEngine()->GetObject
    (theShape->GetStudyID(), theShape->GetEntry());
  Handle(GEOM_Object) aCenter = GetOperations()->GetEngine()->GetObject
    (theCenter->GetStudyID(), theCenter->GetEntry());

  if (aShape.IsNull() || aCenter.IsNull()) return aSeq._retn();

  //Get Shapes On Sphere
  Handle(TColStd_HSequenceOfTransient) aHSeq = GetOperations()->GetShapesOnSphere
    (aShape, theShapeType, aCenter, theRadius, ShapeState(theState));
  if (!GetOperations()->IsDone() || aHSeq.IsNull())
    return aSeq._retn();

  Standard_Integer aLength = aHSeq->Length();
  aSeq->length(aLength);
  for (Standard_Integer i = 1; i <= aLength; i++)
    aSeq[i-1] = GetObject(Handle(GEOM_Object)::DownCast(aHSeq->Value(i)));

  return aSeq._retn();
}

//=============================================================================
/*!
 *  GetShapesOnQuadrangle
 */
//=============================================================================
GEOM::ListOfGO* GEOM_IShapesOperations_i::GetShapesOnQuadrangle
                                                (GEOM::GEOM_Object_ptr theShape,
                                                 CORBA::Long           theShapeType,
                                                 GEOM::GEOM_Object_ptr theTopLeftPoint,
                                                 GEOM::GEOM_Object_ptr theTopRigthPoint,
                                                 GEOM::GEOM_Object_ptr theBottomLeftPoint,
                                                 GEOM::GEOM_Object_ptr theBottomRigthPoint,
                                                 GEOM::shape_state     theState)
{
  GEOM::ListOfGO_var aSeq = new GEOM::ListOfGO;

  //Set a not done flag
  GetOperations()->SetNotDone();

  if (theShape            == NULL ||
      theTopLeftPoint     == NULL ||   
      theTopRigthPoint    == NULL || 
      theBottomLeftPoint  == NULL ||
      theBottomRigthPoint == NULL )
    return aSeq._retn();

  //Get the reference objects
  Handle(GEOM_Object) aShape = GetOperations()->GetEngine()->GetObject
    (theShape->GetStudyID(), theShape->GetEntry());
  Handle(GEOM_Object) aTopLeftPoint = GetOperations()->GetEngine()->GetObject
    (theShape->GetStudyID(), theTopLeftPoint->GetEntry());
  Handle(GEOM_Object) aTopRigthPoint = GetOperations()->GetEngine()->GetObject
    (theShape->GetStudyID(), theTopRigthPoint->GetEntry());
  Handle(GEOM_Object) aBottomLeftPoint = GetOperations()->GetEngine()->GetObject
    (theShape->GetStudyID(), theBottomLeftPoint->GetEntry());
  Handle(GEOM_Object) aBottomRigthPoint = GetOperations()->GetEngine()->GetObject
    (theShape->GetStudyID(), theBottomRigthPoint->GetEntry());

  if (aShape.IsNull() ||
      aTopLeftPoint.IsNull() ||
      aTopRigthPoint.IsNull() ||
      aBottomLeftPoint.IsNull() ||
      aBottomRigthPoint.IsNull() )
    return aSeq._retn();

  //Get Shapes On Quadrangle
  Handle(TColStd_HSequenceOfTransient) aHSeq = GetOperations()->GetShapesOnQuadrangle
    (aShape, theShapeType,
     aTopLeftPoint, aTopRigthPoint, aBottomLeftPoint, aBottomRigthPoint,
     ShapeState(theState));
  if (!GetOperations()->IsDone() || aHSeq.IsNull())
    return aSeq._retn();

  Standard_Integer aLength = aHSeq->Length();
  aSeq->length(aLength);
  for (Standard_Integer i = 1; i <= aLength; i++)
    aSeq[i-1] = GetObject(Handle(GEOM_Object)::DownCast(aHSeq->Value(i)));

  return aSeq._retn();
}

//=============================================================================
/*!
 *  GetShapesOnPlaneIDs
 */
//=============================================================================
GEOM::ListOfLong* GEOM_IShapesOperations_i::GetShapesOnPlaneIDs
                                                (GEOM::GEOM_Object_ptr   theShape,
						 const CORBA::Long       theShapeType,
						 GEOM::GEOM_Object_ptr   theAx1,
						 const GEOM::shape_state theState)
{
  GEOM::ListOfLong_var aSeq = new GEOM::ListOfLong;

  //Set a not done flag
  GetOperations()->SetNotDone();

  if (theShape == NULL || theAx1 == NULL) return aSeq._retn();

  //Get the reference objects
  Handle(GEOM_Object) aShape = GetOperations()->GetEngine()->GetObject
    (theShape->GetStudyID(), theShape->GetEntry());
  Handle(GEOM_Object) anAx1 = GetOperations()->GetEngine()->GetObject
    (theAx1->GetStudyID(), theAx1->GetEntry());

  if (aShape.IsNull() || anAx1.IsNull()) return aSeq._retn();

  //Get Shapes On Plane
  Handle(TColStd_HSequenceOfInteger) aHSeq =
    GetOperations()->GetShapesOnPlaneIDs(aShape, theShapeType, anAx1, ShapeState(theState));
  if (!GetOperations()->IsDone() || aHSeq.IsNull())
    return aSeq._retn();

  Standard_Integer aLength = aHSeq->Length();
  aSeq->length(aLength);
  for (Standard_Integer i = 1; i <= aLength; i++)
    aSeq[i-1] = aHSeq->Value(i);

  return aSeq._retn();
}

//=============================================================================
/*!
 *  GetShapesOnCylinderIDs
 */
//=============================================================================
GEOM::ListOfLong* GEOM_IShapesOperations_i::GetShapesOnCylinderIDs
                                                (GEOM::GEOM_Object_ptr   theShape,
						 const CORBA::Long       theShapeType,
						 GEOM::GEOM_Object_ptr   theAxis,
						 const CORBA::Double     theRadius,
						 const GEOM::shape_state theState)
{
  GEOM::ListOfLong_var aSeq = new GEOM::ListOfLong;

  //Set a not done flag
  GetOperations()->SetNotDone();

  if (theShape == NULL || theAxis == NULL) return aSeq._retn();

  //Get the reference objects
  Handle(GEOM_Object) aShape = GetOperations()->GetEngine()->GetObject
    (theShape->GetStudyID(), theShape->GetEntry());
  Handle(GEOM_Object) anAxis = GetOperations()->GetEngine()->GetObject
    (theAxis->GetStudyID(), theAxis->GetEntry());

  if (aShape.IsNull() || anAxis.IsNull()) return aSeq._retn();

  //Get Shapes On Cylinder
  Handle(TColStd_HSequenceOfInteger) aHSeq = GetOperations()->GetShapesOnCylinderIDs
    (aShape, theShapeType, anAxis, theRadius, ShapeState(theState));
  if (!GetOperations()->IsDone() || aHSeq.IsNull())
    return aSeq._retn();

  Standard_Integer aLength = aHSeq->Length();
  aSeq->length(aLength);
  for (Standard_Integer i = 1; i <= aLength; i++)
    aSeq[i-1] = aHSeq->Value(i);

  return aSeq._retn();
}

//=============================================================================
/*!
 *  GetShapesOnSphereIDs
 */
//=============================================================================
GEOM::ListOfLong* GEOM_IShapesOperations_i::GetShapesOnSphereIDs
                                                (GEOM::GEOM_Object_ptr   theShape,
						 const CORBA::Long       theShapeType,
						 GEOM::GEOM_Object_ptr   theCenter,
						 const CORBA::Double     theRadius,
						 const GEOM::shape_state theState)
{
  GEOM::ListOfLong_var aSeq = new GEOM::ListOfLong;

  //Set a not done flag
  GetOperations()->SetNotDone();

  if (theShape == NULL || theCenter == NULL) return aSeq._retn();

  //Get the reference objects
  Handle(GEOM_Object) aShape = GetOperations()->GetEngine()->GetObject
    (theShape->GetStudyID(), theShape->GetEntry());
  Handle(GEOM_Object) aCenter = GetOperations()->GetEngine()->GetObject
    (theCenter->GetStudyID(), theCenter->GetEntry());

  if (aShape.IsNull() || aCenter.IsNull()) return aSeq._retn();

  //Get Shapes On Sphere
  Handle(TColStd_HSequenceOfInteger) aHSeq = GetOperations()->GetShapesOnSphereIDs
    (aShape, theShapeType, aCenter, theRadius, ShapeState(theState));
  if (!GetOperations()->IsDone() || aHSeq.IsNull())
    return aSeq._retn();

  Standard_Integer aLength = aHSeq->Length();
  aSeq->length(aLength);
  for (Standard_Integer i = 1; i <= aLength; i++)
    aSeq[i-1] = aHSeq->Value(i);

  return aSeq._retn();
}

//=============================================================================
/*!
 *  GetShapesOnQuadrangleIDs
 */
//=============================================================================
GEOM::ListOfLong* GEOM_IShapesOperations_i::GetShapesOnQuadrangleIDs
                                                (GEOM::GEOM_Object_ptr theShape,
                                                 CORBA::Long           theShapeType,
                                                 GEOM::GEOM_Object_ptr theTopLeftPoint,
                                                 GEOM::GEOM_Object_ptr theTopRigthPoint,
                                                 GEOM::GEOM_Object_ptr theBottomLeftPoint,
                                                 GEOM::GEOM_Object_ptr theBottomRigthPoint,
                                                 GEOM::shape_state     theState)
{
  GEOM::ListOfLong_var aSeq = new GEOM::ListOfLong;

  //Set a not done flag
  GetOperations()->SetNotDone();

  if (theShape            == NULL ||
      theTopLeftPoint     == NULL ||   
      theTopRigthPoint    == NULL || 
      theBottomLeftPoint  == NULL ||
      theBottomRigthPoint == NULL )
    return aSeq._retn();

  //Get the reference objects
  Handle(GEOM_Object) aShape = GetOperations()->GetEngine()->GetObject
    (theShape->GetStudyID(), theShape->GetEntry());
  Handle(GEOM_Object) aTopLeftPoint = GetOperations()->GetEngine()->GetObject
    (theShape->GetStudyID(), theTopLeftPoint->GetEntry());
  Handle(GEOM_Object) aTopRigthPoint = GetOperations()->GetEngine()->GetObject
    (theShape->GetStudyID(), theTopRigthPoint->GetEntry());
  Handle(GEOM_Object) aBottomLeftPoint = GetOperations()->GetEngine()->GetObject
    (theShape->GetStudyID(), theBottomLeftPoint->GetEntry());
  Handle(GEOM_Object) aBottomRigthPoint = GetOperations()->GetEngine()->GetObject
    (theShape->GetStudyID(), theBottomRigthPoint->GetEntry());

  if (aShape.IsNull() ||
      aTopLeftPoint.IsNull() ||
      aTopRigthPoint.IsNull() ||
      aBottomLeftPoint.IsNull() ||
      aBottomRigthPoint.IsNull() )
    return aSeq._retn();

  //Get Shapes On Quadrangle
  Handle(TColStd_HSequenceOfInteger) aHSeq = GetOperations()->GetShapesOnQuadrangleIDs
    (aShape, theShapeType,
     aTopLeftPoint, aTopRigthPoint, aBottomLeftPoint, aBottomRigthPoint,
     ShapeState(theState));
  if (!GetOperations()->IsDone() || aHSeq.IsNull())
    return aSeq._retn();

  Standard_Integer aLength = aHSeq->Length();
  aSeq->length(aLength);
  for (Standard_Integer i = 1; i <= aLength; i++)
    aSeq[i-1] = aHSeq->Value(i);

  return aSeq._retn();
}

//=============================================================================
/*!
 *  GetInPlace
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IShapesOperations_i::GetInPlace
                                          (GEOM::GEOM_Object_ptr theShapeWhere,
					   GEOM::GEOM_Object_ptr theShapeWhat)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  if (theShapeWhere == NULL ||
      theShapeWhat == NULL) return aGEOMObject._retn();

  //Get the reference objects
  Handle(GEOM_Object) aShapeWhere = GetOperations()->GetEngine()->GetObject
    (theShapeWhere->GetStudyID(), theShapeWhere->GetEntry());
  Handle(GEOM_Object) aShapeWhat = GetOperations()->GetEngine()->GetObject
    (theShapeWhat->GetStudyID(), theShapeWhat->GetEntry());

  if (aShapeWhere.IsNull() ||
      aShapeWhat.IsNull()) return aGEOMObject._retn();

  //Get Shapes in place of aShapeWhat
  Handle(GEOM_Object) anObject =
    GetOperations()->GetInPlace(aShapeWhere, aShapeWhat);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}
