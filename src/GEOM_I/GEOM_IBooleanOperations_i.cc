// Copyright (C) 2007-2024  CEA, EDF, OPEN CASCADE
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
                                     CORBA::Long           theOp,
                                     CORBA::Boolean        IsCheckSelfInte)
{
  return MakeBooleanWithFuzzy(theShape1, theShape2, theOp, IsCheckSelfInte, -1.0);
}

//=============================================================================
/*!
 *  MakeFuse
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBooleanOperations_i::MakeFuse
                                    (GEOM::GEOM_Object_ptr theShape1,
                                     GEOM::GEOM_Object_ptr theShape2,
                                     CORBA::Boolean        IsCheckSelfInte,
                                     CORBA::Boolean        IsRmExtraEdges)
{
  return MakeFuseWithFuzzy(theShape1, theShape2, IsCheckSelfInte, IsRmExtraEdges, -1.0);
}

//=============================================================================
/*!
 *  MakeFuseList
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBooleanOperations_i::MakeFuseList
                                    (const GEOM::ListOfGO& theShapes,
                                     CORBA::Boolean        IsCheckSelfInte,
                                     CORBA::Boolean        IsRmExtraEdges)
{
  return MakeFuseListWithFuzzy(theShapes, IsCheckSelfInte, IsRmExtraEdges, -1.0);
}

//=============================================================================
/*!
 *  MakeCommonList
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBooleanOperations_i::MakeCommonList
                                    (const GEOM::ListOfGO& theShapes,
                                     CORBA::Boolean        IsCheckSelfInte)
{
  return MakeCommonListWithFuzzy(theShapes, IsCheckSelfInte, -1.0);
}

//=============================================================================
/*!
 *  MakeCutList
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBooleanOperations_i::MakeCutList
                                    (GEOM::GEOM_Object_ptr theMainShape,
                                     const GEOM::ListOfGO& theShapes,
                                     CORBA::Boolean        IsCheckSelfInte)
{
  return MakeCutListWithFuzzy(theMainShape, theShapes, IsCheckSelfInte, -1.0);
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
                                       CORBA::Short            theKeepNonlimitShapes)
{
  return MakePartitionWithFuzzy(theShapes, theTools, theKeepIns, theRemoveIns, theLimit,
                                theRemoveWebs, theMaterials, theKeepNonlimitShapes, -1.0);
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
                                       CORBA::Short            theKeepNonlimitShapes,
                                       CORBA::Boolean          IsCheckSelfInte)
{
  return MakePartitionNonSelfIntersectedShapeWithFuzzy(theShapes, theTools, theKeepIns, theRemoveIns,
                                                       theLimit, theRemoveWebs, theMaterials,
                                                       theKeepNonlimitShapes, IsCheckSelfInte, -1.0);
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
  return MakeHalfPartitionWithFuzzy(theShape, thePlane, -1.0);
}



//=============================================================================
/*!
 *  MakeBooleanWithFuzzy
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBooleanOperations_i::MakeBooleanWithFuzzy
                                    (GEOM::GEOM_Object_ptr theShape1,
                                     GEOM::GEOM_Object_ptr theShape2,
                                     CORBA::Long           theOp,
                                     CORBA::Boolean        IsCheckSelfInte,
                                     CORBA::Double         theFuzzyParam)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference shapes
  Handle(::GEOM_Object) aSh1 = GetObjectImpl(theShape1);
  Handle(::GEOM_Object) aSh2 = GetObjectImpl(theShape2);

  if (aSh1.IsNull() || aSh2.IsNull()) return aGEOMObject._retn();

  // Make Boolean
  Handle(::GEOM_Object) anObject =
    GetOperations()->MakeBoolean(aSh1, aSh2, theOp, IsCheckSelfInte, theFuzzyParam);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeFuseWithFuzzy
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBooleanOperations_i::MakeFuseWithFuzzy
                                    (GEOM::GEOM_Object_ptr theShape1,
                                     GEOM::GEOM_Object_ptr theShape2,
                                     CORBA::Boolean        IsCheckSelfInte,
                                     CORBA::Boolean        IsRmExtraEdges,
                                     CORBA::Double         theFuzzyParam)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference shapes
  Handle(::GEOM_Object) aSh1 = GetObjectImpl(theShape1);
  Handle(::GEOM_Object) aSh2 = GetObjectImpl(theShape2);

  if (aSh1.IsNull() || aSh2.IsNull()) return aGEOMObject._retn();

  // Make Boolean
  Handle(::GEOM_Object) anObject = GetOperations()->MakeFuse
    (aSh1, aSh2, IsCheckSelfInte, IsRmExtraEdges, theFuzzyParam);

  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeFuseListWithFuzzy
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBooleanOperations_i::MakeFuseListWithFuzzy
                                    (const GEOM::ListOfGO& theShapes,
                                     CORBA::Boolean        IsCheckSelfInte,
                                     CORBA::Boolean        IsRmExtraEdges,
                                     CORBA::Double         theFuzzyParam)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  Handle(TColStd_HSequenceOfTransient) aShapes =
    GetListOfObjectsImpl(theShapes);

  if (aShapes.IsNull()) {
    return aGEOMObject._retn();
  }

  // Make fusion
  Handle(::GEOM_Object) anObject =
    GetOperations()->MakeFuseList(aShapes, IsCheckSelfInte, IsRmExtraEdges, theFuzzyParam);

  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeCommonListWithFuzzy
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBooleanOperations_i::MakeCommonListWithFuzzy
                                    (const GEOM::ListOfGO& theShapes,
                                     CORBA::Boolean        IsCheckSelfInte,
                                     CORBA::Double         theFuzzyParam)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  Handle(TColStd_HSequenceOfTransient) aShapes =
    GetListOfObjectsImpl(theShapes);

  if (aShapes.IsNull()) {
    return aGEOMObject._retn();
  }

  // Make fusion
  Handle(::GEOM_Object) anObject =
    GetOperations()->MakeCommonList(aShapes, IsCheckSelfInte, theFuzzyParam);

  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeCutListWithFuzzy
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBooleanOperations_i::MakeCutListWithFuzzy
                                    (GEOM::GEOM_Object_ptr theMainShape,
                                     const GEOM::ListOfGO& theShapes,
                                     CORBA::Boolean        IsCheckSelfInte,
                                     CORBA::Double         theFuzzyParam)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  Handle(::GEOM_Object) aMainShape = GetObjectImpl(theMainShape);

  if (aMainShape.IsNull()) {
    return aGEOMObject._retn();
  }

  Handle(TColStd_HSequenceOfTransient) aShapes =
    GetListOfObjectsImpl(theShapes);

  if (aShapes.IsNull()) {
    return aGEOMObject._retn();
  }

  // Make fusion
  Handle(::GEOM_Object) anObject =
    GetOperations()->MakeCutList(aMainShape, aShapes, IsCheckSelfInte, theFuzzyParam);

  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakePartitionWithFuzzy
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBooleanOperations_i::MakePartitionWithFuzzy
                                      (const GEOM::ListOfGO&   theShapes,
                                       const GEOM::ListOfGO&   theTools,
                                       const GEOM::ListOfGO&   theKeepIns,
                                       const GEOM::ListOfGO&   theRemoveIns,
                                       CORBA::Short            theLimit,
                                       CORBA::Boolean          theRemoveWebs,
                                       const GEOM::ListOfLong& theMaterials,
                                       CORBA::Short            theKeepNonlimitShapes,
                                       CORBA::Double           theFuzzyParam)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  Handle(TColStd_HSequenceOfTransient) aShapes  = GetListOfObjectsImpl(theShapes);
  Handle(TColStd_HSequenceOfTransient) aTools   = GetListOfObjectsImpl(theTools);
  Handle(TColStd_HSequenceOfTransient) aKeepIns = GetListOfObjectsImpl(theKeepIns);
  Handle(TColStd_HSequenceOfTransient) aRemIns  = GetListOfObjectsImpl(theRemoveIns);
  Handle(TColStd_HArray1OfInteger) aMaterials;

  if (aShapes.IsNull() || aTools.IsNull() ||
      aKeepIns.IsNull() || aRemIns.IsNull()) {
    return aGEOMObject._retn();
  }

  //Get the materials
  int ind;
  int aLen = theMaterials.length();
  if ( aLen ) {
    aMaterials = new TColStd_HArray1OfInteger (1, aLen);
    for (ind = 0; ind < aLen; ind++) {
      aMaterials->SetValue(ind+1, theMaterials[ind]);
    }
  }

  // Make Partition
  Handle(::GEOM_Object) anObject =
    GetOperations()->MakePartition(aShapes, aTools, aKeepIns, aRemIns,
                                   theLimit, theRemoveWebs, aMaterials,
                                   theKeepNonlimitShapes,
                                   /*PerformSelfIntersections*/Standard_True,
                                   /*IsCheckSelfInte*/Standard_False,
                                   theFuzzyParam);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakePartitionNonSelfIntersectedShapeWithFuzzy
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBooleanOperations_i::MakePartitionNonSelfIntersectedShapeWithFuzzy
                                      (const GEOM::ListOfGO&   theShapes,
                                       const GEOM::ListOfGO&   theTools,
                                       const GEOM::ListOfGO&   theKeepIns,
                                       const GEOM::ListOfGO&   theRemoveIns,
                                       CORBA::Short            theLimit,
                                       CORBA::Boolean          theRemoveWebs,
                                       const GEOM::ListOfLong& theMaterials,
                                       CORBA::Short theKeepNonlimitShapes,
                                       CORBA::Boolean          IsCheckSelfInte,
                                       CORBA::Double           theFuzzyParam)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  Handle(TColStd_HSequenceOfTransient) aShapes  = GetListOfObjectsImpl(theShapes);
  Handle(TColStd_HSequenceOfTransient) aTools   = GetListOfObjectsImpl(theTools);
  Handle(TColStd_HSequenceOfTransient) aKeepIns = GetListOfObjectsImpl(theKeepIns);
  Handle(TColStd_HSequenceOfTransient) aRemIns  = GetListOfObjectsImpl(theRemoveIns);
  Handle(TColStd_HArray1OfInteger) aMaterials;

  if (aShapes.IsNull() || aTools.IsNull() ||
      aKeepIns.IsNull() || aRemIns.IsNull()) {
    return aGEOMObject._retn();
  }

  //Get the materials
  int ind;
  int aLen = theMaterials.length();
  if ( aLen ) {
    aMaterials = new TColStd_HArray1OfInteger (1, aLen);
    for (ind = 0; ind < aLen; ind++) {
      aMaterials->SetValue(ind+1, theMaterials[ind]);
    }
  }

  // Make Partition
  Handle(::GEOM_Object) anObject =
    GetOperations()->MakePartition(aShapes, aTools, aKeepIns, aRemIns,
                                   theLimit, theRemoveWebs, aMaterials,
                                   theKeepNonlimitShapes,
                                   /*PerformSelfIntersections*/Standard_False,
                                   IsCheckSelfInte,
                                   theFuzzyParam);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeHalfPartitionWithFuzzy
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBooleanOperations_i::MakeHalfPartitionWithFuzzy
                                                 (GEOM::GEOM_Object_ptr theShape,
                                                  GEOM::GEOM_Object_ptr thePlane,
                                                  CORBA::Double         theFuzzyParam)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference shapes
  Handle(::GEOM_Object) aSh = GetObjectImpl(theShape);
  Handle(::GEOM_Object) aPl = GetObjectImpl(thePlane);

  if (aSh.IsNull() || aPl.IsNull()) return aGEOMObject._retn();

  // Make Half Partition
  Handle(::GEOM_Object) anObject =
    GetOperations()->MakeHalfPartition(aSh, aPl, theFuzzyParam);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}
