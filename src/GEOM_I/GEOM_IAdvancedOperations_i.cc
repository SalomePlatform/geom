//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
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
//  File   : GEOM_IAdvancedOperations.cc
//  Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//

#include <Standard_Stream.hxx>

#include "GEOM_IAdvancedOperations_i.hh"

#include <utilities.h>
#include <OpUtil.hxx>
#include <Utils_ExceptHandlers.hxx>

#include "GEOM_Engine.hxx"
#include "GEOM_Object.hxx"

//=============================================================================
/*!
 *   constructor:
 */
//=============================================================================
GEOM_IAdvancedOperations_i::GEOM_IAdvancedOperations_i(PortableServer::POA_ptr thePOA, GEOM::GEOM_Gen_ptr theEngine, ::GEOMImpl_IAdvancedOperations* theImpl)
:GEOM_IOperations_i(thePOA, theEngine, theImpl)
{
  MESSAGE("GEOM_IAdvancedOperations_i::GEOM_IAdvancedOperations_i");
}

//=============================================================================
/*!
 *  destructor
 */
//=============================================================================
GEOM_IAdvancedOperations_i::~GEOM_IAdvancedOperations_i()
{
  MESSAGE("GEOM_IAdvancedOperations_i::~GEOM_IAdvancedOperations_i");
}

//=============================================================================
/*!
 *  Create a T-shape object with specified caracteristics for the main and the
 *  incident pipes (radius, width, half-length).
 *  Center of the shape is (0,0,0). The main plane of the T-shape is XOY.
 *  \param theR1 Internal radius of main pipe
 *  \param theW1 Width of main pipe
 *  \param theL1 Half-length of main pipe
 *  \param theR2 Internal radius of incident pipe (R2 < R1)
 *  \param theW2 Width of incident pipe (R2+W2 < R1+W1)
 *  \param theL2 Half-length of incident pipe
 *  \param theHexMesh false = no partition, true = with partition
 *  \return List of GEOM_Objects, containing the created shape and propagation groups.
 */
//=============================================================================
GEOM::ListOfGO* GEOM_IAdvancedOperations_i::MakePipeTShape (CORBA::Double theR1, CORBA::Double theW1, CORBA::Double theL1, 
							      CORBA::Double theR2, CORBA::Double theW2, CORBA::Double theL2, CORBA::Boolean theHexMesh)
{
  GEOM::ListOfGO_var aSeq = new GEOM::ListOfGO;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Create the TShape
  Handle(TColStd_HSequenceOfTransient) aHSeq = GetOperations()->MakePipeTShape(theR1, theW1, theL1, theR2, theW2, theL2, theHexMesh);
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
 *  Create a T-shape object with specified caracteristics for the main and the
 *  incident pipes (radius, width, half-length).
 *  The T-shape is placed at junction points P1, P2 and P3.
 *  \param theR1 Internal radius of main pipe
 *  \param theW1 Width of main pipe
 *  \param theL1 Half-length of main pipe
 *  \param theR2 Internal radius of incident pipe (R2 < R1)
 *  \param theW2 Width of incident pipe (R2+W2 < R1+W1)
 *  \param theL2 Half-length of incident pipe
 *  \param theHexMesh false = no partition, true = with partition
 *  \return List of GEOM_Objects, containing the created shape and propagation groups.
 */
//=============================================================================
GEOM::ListOfGO* GEOM_IAdvancedOperations_i::MakePipeTShapeWithPosition (CORBA::Double theR1, CORBA::Double theW1, CORBA::Double theL1, 
									      CORBA::Double theR2, CORBA::Double theW2, CORBA::Double theL2, CORBA::Boolean theHexMesh,
									      GEOM::GEOM_Object_ptr theP1, GEOM::GEOM_Object_ptr theP2, GEOM::GEOM_Object_ptr theP3)
{
  GEOM::ListOfGO_var aSeq = new GEOM::ListOfGO;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference vertices
  Handle(GEOM_Object) aP1 = GetObjectImpl(theP1);
  Handle(GEOM_Object) aP2 = GetObjectImpl(theP2);
  Handle(GEOM_Object) aP3 = GetObjectImpl(theP3);
  if (aP1.IsNull() || aP2.IsNull() || aP3.IsNull())return aSeq._retn();

  //Create the TShape
  Handle(TColStd_HSequenceOfTransient) aHSeq = GetOperations()->MakePipeTShapeWithPosition(theR1, theW1, theL1, theR2, theW2, theL2, theHexMesh, aP1, aP2, aP3);
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
 *  Create a T-shape object with specified caracteristics for the main and the
 *  incident pipes (radius, width, half-length). A chamfer is created on
 *  the junction of the pipes.
 *  Center of the shape is (0,0,0). The main plane of the T-shape is XOY.
 *  \param theR1 Internal radius of main pipe
 *  \param theW1 Width of main pipe
 *  \param theL1 Half-length of main pipe
 *  \param theR2 Internal radius of incident pipe (R2 < R1)
 *  \param theW2 Width of incident pipe (R2+W2 < R1+W1)
 *  \param theL2 Half-length of incident pipe
 *  \param theH Height of the chamfer.
 *  \param theW Width of the chamfer.
 *  \param theHexMesh false = no partition, true = with partition
 *  \return List of GEOM_Objects, containing the created shape and propagation groups.
 */
//=============================================================================
GEOM::ListOfGO* GEOM_IAdvancedOperations_i::MakePipeTShapeChamfer(CORBA::Double theR1, CORBA::Double theW1, CORBA::Double theL1, 
									CORBA::Double theR2, CORBA::Double theW2, CORBA::Double theL2, 
									CORBA::Double theH, CORBA::Double theW, CORBA::Boolean theHexMesh)
{
  GEOM::ListOfGO_var aSeq = new GEOM::ListOfGO;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Create the TShape
  Handle(TColStd_HSequenceOfTransient) aHSeq = GetOperations()->MakePipeTShapeChamfer(theR1, theW1, theL1, theR2, theW2, theL2, theH, theW, theHexMesh);
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
 *  Create a T-shape object with specified caracteristics for the main and the
 *  incident pipes (radius, width, half-length). A chamfer is created on
 *  the junction of the pipes.
 *  The T-shape is placed at junction points P1, P2 and P3.
 *  \param theR1 Internal radius of main pipe
 *  \param theW1 Width of main pipe
 *  \param theL1 Half-length of main pipe
 *  \param theR2 Internal radius of incident pipe (R2 < R1)
 *  \param theW2 Width of incident pipe (R2+W2 < R1+W1)
 *  \param theL2 Half-length of incident pipe
 *  \param theH Height of the chamfer.
 *  \param theW Width of the chamfer.
 *  \param theHexMesh false = no partition, true = with partition
 *  \return List of GEOM_Objects, containing the created shape and propagation groups.
 */
//=============================================================================
GEOM::ListOfGO* GEOM_IAdvancedOperations_i::MakePipeTShapeChamferWithPosition (CORBA::Double theR1, CORBA::Double theW1, CORBA::Double theL1, 
										     CORBA::Double theR2, CORBA::Double theW2, CORBA::Double theL2, 
										     CORBA::Double theH, CORBA::Double theW, CORBA::Boolean theHexMesh,
										     GEOM::GEOM_Object_ptr theP1, GEOM::GEOM_Object_ptr theP2, GEOM::GEOM_Object_ptr theP3)
{
  GEOM::ListOfGO_var aSeq = new GEOM::ListOfGO;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference vertices
  Handle(GEOM_Object) aP1 = GetObjectImpl(theP1);
  Handle(GEOM_Object) aP2 = GetObjectImpl(theP2);
  Handle(GEOM_Object) aP3 = GetObjectImpl(theP3);

  //Create the TShape
  Handle(TColStd_HSequenceOfTransient) aHSeq = GetOperations()->MakePipeTShapeChamferWithPosition(theR1, theW1, theL1, theR2, theW2, theL2, theH, theW,
										    theHexMesh, aP1, aP2, aP3);
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
 *  Create a T-shape object with specified caracteristics for the main and the
 *  incident pipes (radius, width, half-length). A fillet is created on
 *  the junction of the pipes.
 *  Center of the shape is (0,0,0). The main plane of the T-shape is XOY.
 *  \param theR1 Internal radius of main pipe
 *  \param theW1 Width of main pipe
 *  \param theL1 Half-length of main pipe
 *  \param theR2 Internal radius of incident pipe (R2 < R1)
 *  \param theW2 Width of incident pipe (R2+W2 < R1+W1)
 *  \param theL2 Half-length of incident pipe
 *  \param theRF Radius of curvature of fillet.
 *  \param theHexMesh false = no partition, true = with partition
 *  \return List of GEOM_Objects, containing the created shape and propagation groups.
 */
//=============================================================================
GEOM::ListOfGO* GEOM_IAdvancedOperations_i::MakePipeTShapeFillet (CORBA::Double theR1, CORBA::Double theW1, CORBA::Double theL1,
									CORBA::Double theR2, CORBA::Double theW2, CORBA::Double theL2, 
									CORBA::Double theRF, CORBA::Boolean theHexMesh)
{
  GEOM::ListOfGO_var aSeq = new GEOM::ListOfGO;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Create the TShape
  Handle(TColStd_HSequenceOfTransient) aHSeq = GetOperations()->MakePipeTShapeFillet(theR1, theW1, theL1, theR2, theW2, theL2, theRF, theHexMesh);
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
 *  Create a T-shape object with specified caracteristics for the main and the
 *  incident pipes (radius, width, half-length). A fillet is created on
 *  the junction of the pipes.
 *  The T-shape is placed at junction points P1, P2 and P3.
 *  \param theR1 Internal radius of main pipe
 *  \param theW1 Width of main pipe
 *  \param theL1 Half-length of main pipe
 *  \param theR2 Internal radius of incident pipe (R2 < R1)
 *  \param theW2 Width of incident pipe (R2+W2 < R1+W1)
 *  \param theL2 Half-length of incident pipe
 *  \param theRF Radius of curvature of fillet.
 *  \param theHexMesh false = no partition, true = with partition
 *  \return List of GEOM_Objects, containing the created shape and propagation groups.
 */
//=============================================================================
GEOM::ListOfGO* GEOM_IAdvancedOperations_i::MakePipeTShapeFilletWithPosition (CORBA::Double theR1, CORBA::Double theW1, CORBA::Double theL1,
										    CORBA::Double theR2, CORBA::Double theW2, CORBA::Double theL2, 
										    CORBA::Double theRF, CORBA::Boolean theHexMesh,
										    GEOM::GEOM_Object_ptr theP1, GEOM::GEOM_Object_ptr theP2, GEOM::GEOM_Object_ptr theP3)
{
  GEOM::ListOfGO_var aSeq = new GEOM::ListOfGO;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference vertices
  Handle(GEOM_Object) aP1 = GetObjectImpl(theP1);
  Handle(GEOM_Object) aP2 = GetObjectImpl(theP2);
  Handle(GEOM_Object) aP3 = GetObjectImpl(theP3);

  //Create the TShape
  Handle(TColStd_HSequenceOfTransient) aHSeq = GetOperations()->MakePipeTShapeFilletWithPosition(theR1, theW1, theL1, theR2, theW2, theL2, theRF, 
										   theHexMesh, aP1, aP2, aP3);
  if (!GetOperations()->IsDone() || aHSeq.IsNull())
    return aSeq._retn();
  
  Standard_Integer aLength = aHSeq->Length();
  aSeq->length(aLength);
  for (Standard_Integer i = 1; i <= aLength; i++)
    aSeq[i-1] = GetObject(Handle(GEOM_Object)::DownCast(aHSeq->Value(i)));

  return aSeq._retn();
}

/*@@ insert new functions before this line @@*/
