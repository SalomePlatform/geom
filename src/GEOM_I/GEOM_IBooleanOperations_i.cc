// Copyright (C) 2007-2013  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

#include "GEOM_IBooleanOperations_i.hh"

#include "utilities.h"
#include "OpUtil.hxx"

#include "GEOM_Engine.hxx"
#include "GEOM_Object.hxx"

#include <TColStd_HArray1OfInteger.hxx>

//=============================================================================
/*!
 *   constructor:
 */
//=============================================================================
GEOM_IBooleanOperations_i::GEOM_IBooleanOperations_i (PortableServer::POA_ptr thePOA,
                                                      GEOM::GEOM_Gen_ptr theEngine,
                                                      ::GEOMImpl_IBooleanOperations* theImpl)
:GEOM_IOperations_i(thePOA, theEngine, theImpl)
{
  MESSAGE("GEOM_IBooleanOperations_i::GEOM_IBooleanOperations_i");
}

//=============================================================================
/*!
 *  destructor
 */
//=============================================================================
GEOM_IBooleanOperations_i::~GEOM_IBooleanOperations_i()
{
  MESSAGE("GEOM_IBooleanOperations_i::~GEOM_IBooleanOperations_i");
}


//=============================================================================
/*!
 *  MakeBoolean
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBooleanOperations_i::MakeBoolean
                                                 (GEOM::GEOM_Object_ptr theShape1,
                                                  GEOM::GEOM_Object_ptr theShape2,
                                                  CORBA::Long           theOp)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference shapes
  Handle(GEOM_Object) aSh1 = GetObjectImpl(theShape1);
  Handle(GEOM_Object) aSh2 = GetObjectImpl(theShape2);

  if (aSh1.IsNull() || aSh2.IsNull()) return aGEOMObject._retn();

  // Make Boolean
  Handle(GEOM_Object) anObject = GetOperations()->MakeBoolean(aSh1, aSh2, theOp);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakePartition
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBooleanOperations_i::MakePartition
                                      (const GEOM::ListOfGO&   theShapes,
                                       const GEOM::ListOfGO&   theTools,
                                       const GEOM::ListOfGO&   theKeepIns,
                                       const GEOM::ListOfGO&   theRemoveIns,
                                       CORBA::Short            theLimit,
                                       CORBA::Boolean          theRemoveWebs,
                                       const GEOM::ListOfLong& theMaterials,
                                       CORBA::Short theKeepNonlimitShapes)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  int ind, aLen;
  Handle(TColStd_HSequenceOfTransient) aShapes  = new TColStd_HSequenceOfTransient;
  Handle(TColStd_HSequenceOfTransient) aTools   = new TColStd_HSequenceOfTransient;
  Handle(TColStd_HSequenceOfTransient) aKeepIns = new TColStd_HSequenceOfTransient;
  Handle(TColStd_HSequenceOfTransient) aRemIns  = new TColStd_HSequenceOfTransient;
  Handle(TColStd_HArray1OfInteger) aMaterials;

  //Get the shapes
  aLen = theShapes.length();
  for (ind = 0; ind < aLen; ind++) {
    Handle(GEOM_Object) aSh = GetObjectImpl(theShapes[ind]);
    if (aSh.IsNull()) return aGEOMObject._retn();
    aShapes->Append(aSh);
  }

  //Get the tools
  aLen = theTools.length();
  for (ind = 0; ind < aLen; ind++) {
    Handle(GEOM_Object) aSh = GetObjectImpl(theTools[ind]);
    if (aSh.IsNull()) return aGEOMObject._retn();
    aTools->Append(aSh);
  }

  //Get the keep inside shapes
  aLen = theKeepIns.length();
  for (ind = 0; ind < aLen; ind++) {
    Handle(GEOM_Object) aSh = GetObjectImpl(theKeepIns[ind]);
    if (aSh.IsNull()) return aGEOMObject._retn();
    aKeepIns->Append(aSh);
  }

  //Get the remove inside shapes
  aLen = theRemoveIns.length();
  for (ind = 0; ind < aLen; ind++) {
    Handle(GEOM_Object) aSh = GetObjectImpl(theRemoveIns[ind]);
    if (aSh.IsNull()) return aGEOMObject._retn();
    aRemIns->Append(aSh);
  }

  //Get the materials
  aLen = theMaterials.length();
  if ( aLen ) {
    aMaterials = new TColStd_HArray1OfInteger (1, aLen);
    for (ind = 0; ind < aLen; ind++) {
      aMaterials->SetValue(ind+1, theMaterials[ind]);
    }
  }

  // Make Partition
  Handle(GEOM_Object) anObject =
    GetOperations()->MakePartition(aShapes, aTools, aKeepIns, aRemIns,
                                   theLimit, theRemoveWebs, aMaterials,
                                   theKeepNonlimitShapes,
                                   /*PerformSelfIntersections*/Standard_True);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakePartitionNonSelfIntersectedShape
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBooleanOperations_i::MakePartitionNonSelfIntersectedShape
                                      (const GEOM::ListOfGO&   theShapes,
                                       const GEOM::ListOfGO&   theTools,
                                       const GEOM::ListOfGO&   theKeepIns,
                                       const GEOM::ListOfGO&   theRemoveIns,
                                       CORBA::Short            theLimit,
                                       CORBA::Boolean          theRemoveWebs,
                                       const GEOM::ListOfLong& theMaterials,
                                       CORBA::Short theKeepNonlimitShapes)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  int ind, aLen;
  Handle(TColStd_HSequenceOfTransient) aShapes  = new TColStd_HSequenceOfTransient;
  Handle(TColStd_HSequenceOfTransient) aTools   = new TColStd_HSequenceOfTransient;
  Handle(TColStd_HSequenceOfTransient) aKeepIns = new TColStd_HSequenceOfTransient;
  Handle(TColStd_HSequenceOfTransient) aRemIns  = new TColStd_HSequenceOfTransient;
  Handle(TColStd_HArray1OfInteger) aMaterials;

  //Get the shapes
  aLen = theShapes.length();
  for (ind = 0; ind < aLen; ind++) {
    Handle(GEOM_Object) aSh = GetObjectImpl(theShapes[ind]);
    if (aSh.IsNull()) return aGEOMObject._retn();
    aShapes->Append(aSh);
  }

  //Get the tools
  aLen = theTools.length();
  for (ind = 0; ind < aLen; ind++) {
    Handle(GEOM_Object) aSh = GetObjectImpl(theTools[ind]);
    if (aSh.IsNull()) return aGEOMObject._retn();
    aTools->Append(aSh);
  }

  //Get the keep inside shapes
  aLen = theKeepIns.length();
  for (ind = 0; ind < aLen; ind++) {
    Handle(GEOM_Object) aSh = GetObjectImpl(theKeepIns[ind]);
    if (aSh.IsNull()) return aGEOMObject._retn();
    aKeepIns->Append(aSh);
  }

  //Get the remove inside shapes
  aLen = theRemoveIns.length();
  for (ind = 0; ind < aLen; ind++) {
    Handle(GEOM_Object) aSh = GetObjectImpl(theRemoveIns[ind]);
    if (aSh.IsNull()) return aGEOMObject._retn();
    aRemIns->Append(aSh);
  }

  //Get the materials
  aLen = theMaterials.length();
  if ( aLen ) {
    aMaterials = new TColStd_HArray1OfInteger (1, aLen);
    for (ind = 0; ind < aLen; ind++) {
      aMaterials->SetValue(ind+1, theMaterials[ind]);
    }
  }

  // Make Partition
  Handle(GEOM_Object) anObject =
    GetOperations()->MakePartition(aShapes, aTools, aKeepIns, aRemIns,
                                   theLimit, theRemoveWebs, aMaterials,
                                   theKeepNonlimitShapes,
                                   /*PerformSelfIntersections*/Standard_False);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeHalfPartition
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBooleanOperations_i::MakeHalfPartition
                                                 (GEOM::GEOM_Object_ptr theShape,
                                                  GEOM::GEOM_Object_ptr thePlane)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference shapes
  Handle(GEOM_Object) aSh = GetObjectImpl(theShape);
  Handle(GEOM_Object) aPl = GetObjectImpl(thePlane);

  if (aSh.IsNull() || aPl.IsNull()) return aGEOMObject._retn();

  // Make Half Partition
  Handle(GEOM_Object) anObject = GetOperations()->MakeHalfPartition(aSh, aPl);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}
