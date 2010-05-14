//  Copyright (C) 2007-2010  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include <Standard_Stream.hxx>

#include "GEOM_IGroupOperations_i.hh"

#include "utilities.h"
#include "OpUtil.hxx"
#include "Utils_ExceptHandlers.hxx"

#include "GEOM_Engine.hxx"
#include "GEOM_Object.hxx"

#include <TColStd_HArray1OfInteger.hxx>
#include <TopAbs.hxx>

//=============================================================================
/*!
 *   constructor:
 */
//=============================================================================
GEOM_IGroupOperations_i::GEOM_IGroupOperations_i (PortableServer::POA_ptr thePOA,
                                                  GEOM::GEOM_Gen_ptr theEngine,
                                                  ::GEOMImpl_IGroupOperations* theImpl)
     :GEOM_IOperations_i(thePOA, theEngine, theImpl)
{
  MESSAGE("GEOM_IGroupOperations_i::GEOM_IGroupOperations_i");
}

//=============================================================================
/*!
 *  destructor
 */
//=============================================================================
GEOM_IGroupOperations_i::~GEOM_IGroupOperations_i()
{
  MESSAGE("GEOM_IGroupOperations_i::~GEOM_IGroupOperations_i");
}


//=============================================================================
/*!
 *  CreateGroup
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IGroupOperations_i::CreateGroup(GEOM::GEOM_Object_ptr theMainShape,
                                                           CORBA::Long theShapeType)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  if (theShapeType < 0) return aGEOMObject._retn();

  //Get the reference shape
  Handle(GEOM_Object) aShapeRef = GetObjectImpl(theMainShape);
  if (aShapeRef.IsNull()) return aGEOMObject._retn();

  //Create the Fillet
  Handle(GEOM_Object) anObject = GetOperations()->CreateGroup(aShapeRef, (TopAbs_ShapeEnum)theShapeType);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  AddObject
 */
//=============================================================================
void GEOM_IGroupOperations_i::AddObject(GEOM::GEOM_Object_ptr theGroup, CORBA::Long theSubShapeId)
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference group
  Handle(GEOM_Object) aGroupRef = GetObjectImpl(theGroup);
  if (aGroupRef.IsNull()) return;

  GetOperations()->AddObject(aGroupRef, theSubShapeId);
}

//=============================================================================
/*!
 *  RemoveObject
 */
//=============================================================================
void GEOM_IGroupOperations_i::RemoveObject(GEOM::GEOM_Object_ptr theGroup, CORBA::Long theSubShapeId)
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference group
  Handle(GEOM_Object) aGroupRef = GetObjectImpl(theGroup);
  if (aGroupRef.IsNull()) return;

  GetOperations()->RemoveObject(aGroupRef, theSubShapeId);
}

//=============================================================================
/*!
 *  UnionList
 */
//=============================================================================
void GEOM_IGroupOperations_i::UnionList (GEOM::GEOM_Object_ptr theGroup,
                                         const GEOM::ListOfGO& theSubShapes)
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference group
  Handle(GEOM_Object) aGroupRef = GetObjectImpl(theGroup);
  if (aGroupRef.IsNull()) return;

  //Get sub-shape to add
  Handle(TColStd_HSequenceOfTransient) aSubShapes = new TColStd_HSequenceOfTransient;

  int ind, aLen = theSubShapes.length();
  for (ind = 0; ind < aLen; ind++) {
    Handle(GEOM_Object) aSh = GetObjectImpl(theSubShapes[ind]);
    if (aSh.IsNull()) return;
    aSubShapes->Append(aSh);
  }

  //Perform the operation
  GetOperations()->UnionList(aGroupRef, aSubShapes);
}

//=============================================================================
/*!
 *  DifferenceList
 */
//=============================================================================
void GEOM_IGroupOperations_i::DifferenceList (GEOM::GEOM_Object_ptr theGroup,
                                              const GEOM::ListOfGO& theSubShapes)
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference group
  Handle(GEOM_Object) aGroupRef = GetObjectImpl(theGroup);
  if (aGroupRef.IsNull()) return;

  //Get sub-shape to remove
  Handle(TColStd_HSequenceOfTransient) aSubShapes = new TColStd_HSequenceOfTransient;

  int ind, aLen = theSubShapes.length();
  for (ind = 0; ind < aLen; ind++) {
    Handle(GEOM_Object) aSh = GetObjectImpl(theSubShapes[ind]);
    if (aSh.IsNull()) return;
    aSubShapes->Append(aSh);
  }

  //Perform the operation
  GetOperations()->DifferenceList(aGroupRef, aSubShapes);
}

//=============================================================================
/*!
 *  UnionIDs
 */
//=============================================================================
void GEOM_IGroupOperations_i::UnionIDs (GEOM::GEOM_Object_ptr   theGroup,
                                        const GEOM::ListOfLong& theSubShapes)
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference group
  Handle(GEOM_Object) aGroupRef = GetObjectImpl(theGroup);
  if (aGroupRef.IsNull()) return;

  //Get sub-shape to add
  Handle(TColStd_HSequenceOfInteger) aSubShapes = new TColStd_HSequenceOfInteger;

  int ind, aLen = theSubShapes.length();
  for (ind = 0; ind < aLen; ind++) {
    aSubShapes->Append(theSubShapes[ind]);
  }

  //Perform the operation
  GetOperations()->UnionIDs(aGroupRef, aSubShapes);
  return;
}

//=============================================================================
/*!
 *  DifferenceIDs
 */
//=============================================================================
void GEOM_IGroupOperations_i::DifferenceIDs (GEOM::GEOM_Object_ptr   theGroup,
                                             const GEOM::ListOfLong& theSubShapes)
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference group
  Handle(GEOM_Object) aGroupRef = GetObjectImpl(theGroup);
  if (aGroupRef.IsNull()) return;

  //Get sub-shape to remove
  Handle(TColStd_HSequenceOfInteger) aSubShapes = new TColStd_HSequenceOfInteger;

  int ind, aLen = theSubShapes.length();
  for (ind = 0; ind < aLen; ind++) {
    aSubShapes->Append(theSubShapes[ind]);
  }

  //Perform the operation
  GetOperations()->DifferenceIDs(aGroupRef, aSubShapes);
  return;
}

//=============================================================================
/*!
 *  GetType
 */
//=============================================================================
CORBA::Long GEOM_IGroupOperations_i::GetType(GEOM::GEOM_Object_ptr theGroup)
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference group
  Handle(GEOM_Object) aGroupRef = GetObjectImpl(theGroup);
  if (aGroupRef.IsNull()) return -1;

  return GetOperations()->GetType(aGroupRef);
}

//=============================================================================
/*!
 *  GetMainShape
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IGroupOperations_i::GetMainShape(GEOM::GEOM_Object_ptr theGroup)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference group
  Handle(GEOM_Object) aGroupRef = GetObjectImpl(theGroup);
  if (aGroupRef.IsNull()) return aGEOMObject._retn();

  Handle(GEOM_Object) anObject = GetOperations()->GetMainShape(aGroupRef);
  if (!GetOperations()->IsDone() || anObject.IsNull()) return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  GetObjects
 */
//=============================================================================
GEOM::ListOfLong* GEOM_IGroupOperations_i::GetObjects(GEOM::GEOM_Object_ptr theGroup)
{
  GEOM::ListOfLong_var aList;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference group
  Handle(GEOM_Object) aGroupRef = GetObjectImpl(theGroup);
  if (aGroupRef.IsNull()) return aList._retn();

  aList = new GEOM::ListOfLong;

  Handle(TColStd_HArray1OfInteger) aSeq = GetOperations()->GetObjects(aGroupRef);
  if (!GetOperations()->IsDone() || aSeq.IsNull()) return aList._retn();

  aList->length(aSeq->Length());
  for(int i = 1; i<=aSeq->Length(); i++) aList[i-1] = aSeq->Value(i);

  return aList._retn();
}
