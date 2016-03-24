// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

//  File   : GEOM_IAdvancedOperations.cc
//  Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

#include <Standard_Stream.hxx>

#include "AdvancedEngine_IOperations_i.hh"
#include "AdvancedEngine_IOperations.hxx"

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
AdvancedEngine_IOperations_i::AdvancedEngine_IOperations_i(PortableServer::POA_ptr thePOA, GEOM::GEOM_Gen_ptr theEngine, AdvancedEngine_IOperations* theImpl)
:GEOM_IOperations_i(thePOA, theEngine, theImpl)
{
  MESSAGE("AdvancedEngine_IOperations_i::AdvancedEngine_IOperations_i");
}

//=============================================================================
/*!
 *  destructor
 */
//=============================================================================
AdvancedEngine_IOperations_i::~AdvancedEngine_IOperations_i()
{
  MESSAGE("AdvancedEngine_IOperations_i::~AdvancedEngine_IOperations_i");
}

//=============================================================================
/*!
 *  MakePipeTShape
 *  Create a T-shape object with specified caracteristics for the main and
 *  the incident pipes (radius, width, half-length).
 *  Center of the shape is (0,0,0). The main plane of the T-shape is XOY.
 *  \param theR1 Internal radius of main pipe
 *  \param theW1 Width of main pipe
 *  \param theL1 Half-length of main pipe
 *  \param theR2 Internal radius of incident pipe (R2 < R1)
 *  \param theW2 Width of incident pipe (R2+W2 < R1+W1)
 *  \param theL2 Half-length of incident pipe
 *  \param theHexMesh Boolean indicating if shape is prepared for hex mesh
 *  \return List of GEOM_Objects, containing the created shape and propagation groups.
 */
//=============================================================================
GEOM::ListOfGO* AdvancedEngine_IOperations_i::MakePipeTShape
                     (CORBA::Double theR1, CORBA::Double theW1, CORBA::Double theL1,
                      CORBA::Double theR2, CORBA::Double theW2, CORBA::Double theL2,
                      CORBA::Boolean theHexMesh)
{
  GEOM::ListOfGO_var aSeq = new GEOM::ListOfGO;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Create the TShape
  Handle(TColStd_HSequenceOfTransient) aHSeq =
    GetOperations()->MakePipeTShape(theR1, theW1, theL1, theR2, theW2, theL2,
                                    0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
                                    theHexMesh);
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
 *  MakePipeTShapeWithPosition
 *  Create a T-shape object with specified caracteristics for the main and
 *  the incident pipes (radius, width, half-length).
 *  The extremities of the main pipe are located on junctions points P1 and P2.
 *  The extremity of the incident pipe is located on junction point P3.
 *  \param theR1 Internal radius of main pipe
 *  \param theW1 Width of main pipe
 *  \param theL1 Half-length of main pipe
 *  \param theR2 Internal radius of incident pipe (R2 < R1)
 *  \param theW2 Width of incident pipe (R2+W2 < R1+W1)
 *  \param theL2 Half-length of incident pipe
 *  \param theHexMesh Boolean indicating if shape is prepared for hex mesh
 *  \param theP1 1st junction point of main pipe
 *  \param theP2 2nd junction point of main pipe
 *  \param theP3 Junction point of incident pipe
 *  \return List of GEOM_Objects, containing the created shape and propagation groups.
 */
//=============================================================================
GEOM::ListOfGO* AdvancedEngine_IOperations_i::MakePipeTShapeWithPosition
                     (CORBA::Double theR1, CORBA::Double theW1, CORBA::Double theL1,
                      CORBA::Double theR2, CORBA::Double theW2, CORBA::Double theL2,
                      CORBA::Boolean theHexMesh,
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
  Handle(TColStd_HSequenceOfTransient) aHSeq =
    GetOperations()->MakePipeTShapeWithPosition(theR1, theW1, theL1, theR2, theW2, theL2,
                                                0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
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
 *  MakePipeTShapeChamfer
 *  Create a T-shape object with specified caracteristics for the main and
 *  the incident pipes (radius, width, half-length). A chamfer is created
 *  on the junction of the pipes.
 *  Center of the shape is (0,0,0). The main plane of the T-shape is XOY.
 *  \param theR1 Internal radius of main pipe
 *  \param theW1 Width of main pipe
 *  \param theL1 Half-length of main pipe
 *  \param theR2 Internal radius of incident pipe (R2 < R1)
 *  \param theW2 Width of incident pipe (R2+W2 < R1+W1)
 *  \param theL2 Half-length of incident pipe
 *  \param theH Height of chamfer.
 *  \param theW Width of chamfer.
 *  \param theHexMesh Boolean indicating if shape is prepared for hex mesh
 *  \return List of GEOM_Objects, containing the created shape and propagation groups.
 */
//=============================================================================
GEOM::ListOfGO* AdvancedEngine_IOperations_i::MakePipeTShapeChamfer
                     (CORBA::Double theR1, CORBA::Double theW1, CORBA::Double theL1,
                      CORBA::Double theR2, CORBA::Double theW2, CORBA::Double theL2,
                      CORBA::Double theH, CORBA::Double theW, CORBA::Boolean theHexMesh)
{
  GEOM::ListOfGO_var aSeq = new GEOM::ListOfGO;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Create the TShape
  Handle(TColStd_HSequenceOfTransient) aHSeq =
    GetOperations()->MakePipeTShapeChamfer(theR1, theW1, theL1, theR2, theW2, theL2,
                                           0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
                                           theH, theW, theHexMesh);
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
 *  MakePipeTShapeChamferWithPosition
 *  Create a T-shape object with specified caracteristics for the main and
 *  the incident pipes (radius, width, half-length). A chamfer is created
 *  on the junction of the pipes.
 *  The extremities of the main pipe are located on junctions points P1 and P2.
 *  The extremity of the incident pipe is located on junction point P3.
 *  \param theR1 Internal radius of main pipe
 *  \param theW1 Width of main pipe
 *  \param theL1 Half-length of main pipe
 *  \param theR2 Internal radius of incident pipe (R2 < R1)
 *  \param theW2 Width of incident pipe (R2+W2 < R1+W1)
 *  \param theL2 Half-length of incident pipe
 *  \param theH Height of the chamfer.
 *  \param theW Width of the chamfer.
 *  \param theHexMesh Boolean indicating if shape is prepared for hex mesh
 *  \param theP1 1st junction point of main pipe
 *  \param theP2 2nd junction point of main pipe
 *  \param theP3 Junction point of incident pipe
 *  \return List of GEOM_Objects, containing the created shape and propagation groups.
 */
//=============================================================================
GEOM::ListOfGO* AdvancedEngine_IOperations_i::MakePipeTShapeChamferWithPosition
                     (CORBA::Double theR1, CORBA::Double theW1, CORBA::Double theL1,
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
  Handle(TColStd_HSequenceOfTransient) aHSeq =
    GetOperations()->MakePipeTShapeChamferWithPosition(theR1, theW1, theL1, theR2, theW2, theL2,
                                                       0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
                                                       theH, theW, theHexMesh, aP1, aP2, aP3);
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
 *  MakePipeTShapeFillet
 *  Create a T-shape object with specified caracteristics for the main and
 *  the incident pipes (radius, width, half-length). A fillet is created
 *  on the junction of the pipes.
 *  Center of the shape is (0,0,0). The main plane of the T-shape is XOY.
 *  \param theR1 Internal radius of main pipe
 *  \param theW1 Width of main pipe
 *  \param theL1 Half-length of main pipe
 *  \param theR2 Internal radius of incident pipe (R2 < R1)
 *  \param theW2 Width of incident pipe (R2+W2 < R1+W1)
 *  \param theL2 Half-length of incident pipe
 *  \param theRF Radius of curvature of fillet.
 *  \param theHexMesh Boolean indicating if shape is prepared for hex mesh
 *  \return List of GEOM_Objects, containing the created shape and propagation groups.
 */
//=============================================================================
GEOM::ListOfGO* AdvancedEngine_IOperations_i::MakePipeTShapeFillet
                     (CORBA::Double theR1, CORBA::Double theW1, CORBA::Double theL1,
                      CORBA::Double theR2, CORBA::Double theW2, CORBA::Double theL2,
                      CORBA::Double theRF, CORBA::Boolean theHexMesh)
{
  GEOM::ListOfGO_var aSeq = new GEOM::ListOfGO;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Create the TShape
  Handle(TColStd_HSequenceOfTransient) aHSeq =
    GetOperations()->MakePipeTShapeFillet(theR1, theW1, theL1, theR2, theW2, theL2,
                                          0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
                                          theRF, theHexMesh);
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
 *  MakePipeTShapeFilletWithPosition
 *  Create a T-shape object with specified caracteristics for the main and
 *  the incident pipes (radius, width, half-length). A fillet is created
 *  on the junction of the pipes.
 *  The extremities of the main pipe are located on junctions points P1 and P2.
 *  The extremity of the incident pipe is located on junction point P3.
 *  \param theR1 Internal radius of main pipe
 *  \param theW1 Width of main pipe
 *  \param theL1 Half-length of main pipe
 *  \param theR2 Internal radius of incident pipe (R2 < R1)
 *  \param theW2 Width of incident pipe (R2+W2 < R1+W1)
 *  \param theL2 Half-length of incident pipe
 *  \param theRF Radius of curvature of fillet
 *  \param theHexMesh Boolean indicating if shape is prepared for hex mesh
 *  \param theP1 1st junction point of main pipe
 *  \param theP2 2nd junction point of main pipe
 *  \param theP3 Junction point of incident pipe
 *  \return List of GEOM_Objects, containing the created shape and propagation groups.
 */
//=============================================================================
GEOM::ListOfGO* AdvancedEngine_IOperations_i::MakePipeTShapeFilletWithPosition
                     (CORBA::Double theR1, CORBA::Double theW1, CORBA::Double theL1,
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
  Handle(TColStd_HSequenceOfTransient) aHSeq =
    GetOperations()->MakePipeTShapeFilletWithPosition(theR1, theW1, theL1, theR2, theW2, theL2,
                                                      0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
                                                      theRF, theHexMesh, aP1, aP2, aP3);
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
 *  MakePipeTShapeTR
 *  Create a T-shape object with specified caracteristics for the main and
 *  the incident pipes (radius, width, half-length).
 *  Center of the shape is (0,0,0). The main plane of the T-shape is XOY.
 *  \param theR1 Internal radius of main pipe
 *  \param theW1 Width of main pipe
 *  \param theL1 Half-length of main pipe
 *  \param theR2 Internal radius of incident pipe (R2 < R1)
 *  \param theW2 Width of incident pipe (R2+W2 < R1+W1)
 *  \param theL2 Half-length of incident pipe
 *  \param theHexMesh Boolean indicating if shape is prepared for hex mesh
 *  \return List of GEOM_Objects, containing the created shape and propagation groups.
 */
//=============================================================================
GEOM::ListOfGO* AdvancedEngine_IOperations_i::MakePipeTShapeTR
                     (CORBA::Double theR1, CORBA::Double theW1, CORBA::Double theL1,
                      CORBA::Double theR2, CORBA::Double theW2, CORBA::Double theL2,
                      CORBA::Double theRL, CORBA::Double theWL, CORBA::Double theLtransL, CORBA::Double theLthinL,
                      CORBA::Double theRR, CORBA::Double theWR, CORBA::Double theLtransR, CORBA::Double theLthinR,
                      CORBA::Double theRI, CORBA::Double theWI, CORBA::Double theLtransI, CORBA::Double theLthinI,
                      CORBA::Boolean theHexMesh)
{
  GEOM::ListOfGO_var aSeq = new GEOM::ListOfGO;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Create the TShape
  Handle(TColStd_HSequenceOfTransient) aHSeq =
    GetOperations()->MakePipeTShape(theR1, theW1, theL1, theR2, theW2, theL2,
                                    theRL, theWL, theLtransL, theLthinL,
                                    theRR, theWR, theLtransR, theLthinR,
                                    theRI, theWI, theLtransI, theLthinI,
                                    theHexMesh);
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
 *  MakePipeTShapeTRWithPosition
 *  Create a T-shape object with specified caracteristics for the main and
 *  the incident pipes (radius, width, half-length).
 *  The extremities of the main pipe are located on junctions points P1 and P2.
 *  The extremity of the incident pipe is located on junction point P3.
 *  \param theR1 Internal radius of main pipe
 *  \param theW1 Width of main pipe
 *  \param theL1 Half-length of main pipe
 *  \param theR2 Internal radius of incident pipe (R2 < R1)
 *  \param theW2 Width of incident pipe (R2+W2 < R1+W1)
 *  \param theL2 Half-length of incident pipe
 *  \param theHexMesh Boolean indicating if shape is prepared for hex mesh
 *  \param theP1 1st junction point of main pipe
 *  \param theP2 2nd junction point of main pipe
 *  \param theP3 Junction point of incident pipe
 *  \return List of GEOM_Objects, containing the created shape and propagation groups.
 */
//=============================================================================
GEOM::ListOfGO* AdvancedEngine_IOperations_i::MakePipeTShapeTRWithPosition
                     (CORBA::Double theR1, CORBA::Double theW1, CORBA::Double theL1,
                      CORBA::Double theR2, CORBA::Double theW2, CORBA::Double theL2,
                      CORBA::Double theRL, CORBA::Double theWL, CORBA::Double theLtransL, CORBA::Double theLthinL,
                      CORBA::Double theRR, CORBA::Double theWR, CORBA::Double theLtransR, CORBA::Double theLthinR,
                      CORBA::Double theRI, CORBA::Double theWI, CORBA::Double theLtransI, CORBA::Double theLthinI,
                      CORBA::Boolean theHexMesh,
                      GEOM::GEOM_Object_ptr theP1, GEOM::GEOM_Object_ptr theP2, GEOM::GEOM_Object_ptr theP3)
{
  GEOM::ListOfGO_var aSeq = new GEOM::ListOfGO;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference vertices
  Handle(GEOM_Object) aP1 = GetObjectImpl(theP1);
  Handle(GEOM_Object) aP2 = GetObjectImpl(theP2);
  Handle(GEOM_Object) aP3 = GetObjectImpl(theP3);
  if (aP1.IsNull() || aP2.IsNull() || aP3.IsNull()) return aSeq._retn();

  //Create the TShape
  Handle(TColStd_HSequenceOfTransient) aHSeq =
    GetOperations()->MakePipeTShapeWithPosition(theR1, theW1, theL1, theR2, theW2, theL2,
                                                theRL, theWL, theLtransL, theLthinL,
                                                theRR, theWR, theLtransR, theLthinR,
                                                theRI, theWI, theLtransI, theLthinI,
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
 *  MakePipeTShapeTRChamfer
 *  Create a T-shape object with specified caracteristics for the main and
 *  the incident pipes (radius, width, half-length). A chamfer is created
 *  on the junction of the pipes.
 *  Center of the shape is (0,0,0). The main plane of the T-shape is XOY.
 *  \param theR1 Internal radius of main pipe
 *  \param theW1 Width of main pipe
 *  \param theL1 Half-length of main pipe
 *  \param theR2 Internal radius of incident pipe (R2 < R1)
 *  \param theW2 Width of incident pipe (R2+W2 < R1+W1)
 *  \param theL2 Half-length of incident pipe
 *  \param theH Height of chamfer.
 *  \param theW Width of chamfer.
 *  \param theHexMesh Boolean indicating if shape is prepared for hex mesh
 *  \return List of GEOM_Objects, containing the created shape and propagation groups.
 */
//=============================================================================
GEOM::ListOfGO* AdvancedEngine_IOperations_i::MakePipeTShapeTRChamfer
                     (CORBA::Double theR1, CORBA::Double theW1, CORBA::Double theL1,
                      CORBA::Double theR2, CORBA::Double theW2, CORBA::Double theL2,
                      CORBA::Double theRL, CORBA::Double theWL, CORBA::Double theLtransL, CORBA::Double theLthinL,
                      CORBA::Double theRR, CORBA::Double theWR, CORBA::Double theLtransR, CORBA::Double theLthinR,
                      CORBA::Double theRI, CORBA::Double theWI, CORBA::Double theLtransI, CORBA::Double theLthinI,
                      CORBA::Double theH, CORBA::Double theW, CORBA::Boolean theHexMesh)
{
  GEOM::ListOfGO_var aSeq = new GEOM::ListOfGO;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Create the TShape
  Handle(TColStd_HSequenceOfTransient) aHSeq =
    GetOperations()->MakePipeTShapeChamfer(theR1, theW1, theL1, theR2, theW2, theL2,
                                           theRL, theWL, theLtransL, theLthinL,
                                           theRR, theWR, theLtransR, theLthinR,
                                           theRI, theWI, theLtransI, theLthinI,
                                           theH, theW, theHexMesh);
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
 *  MakePipeTShapeTRChamferWithPosition
 *  Create a T-shape object with specified caracteristics for the main and
 *  the incident pipes (radius, width, half-length). A chamfer is created
 *  on the junction of the pipes.
 *  The extremities of the main pipe are located on junctions points P1 and P2.
 *  The extremity of the incident pipe is located on junction point P3.
 *  \param theR1 Internal radius of main pipe
 *  \param theW1 Width of main pipe
 *  \param theL1 Half-length of main pipe
 *  \param theR2 Internal radius of incident pipe (R2 < R1)
 *  \param theW2 Width of incident pipe (R2+W2 < R1+W1)
 *  \param theL2 Half-length of incident pipe
 *  \param theH Height of the chamfer.
 *  \param theW Width of the chamfer.
 *  \param theHexMesh Boolean indicating if shape is prepared for hex mesh
 *  \param theP1 1st junction point of main pipe
 *  \param theP2 2nd junction point of main pipe
 *  \param theP3 Junction point of incident pipe
 *  \return List of GEOM_Objects, containing the created shape and propagation groups.
 */
//=============================================================================
GEOM::ListOfGO* AdvancedEngine_IOperations_i::MakePipeTShapeTRChamferWithPosition
                     (CORBA::Double theR1, CORBA::Double theW1, CORBA::Double theL1,
                      CORBA::Double theR2, CORBA::Double theW2, CORBA::Double theL2,
                      CORBA::Double theRL, CORBA::Double theWL, CORBA::Double theLtransL, CORBA::Double theLthinL,
                      CORBA::Double theRR, CORBA::Double theWR, CORBA::Double theLtransR, CORBA::Double theLthinR,
                      CORBA::Double theRI, CORBA::Double theWI, CORBA::Double theLtransI, CORBA::Double theLthinI,
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
  Handle(TColStd_HSequenceOfTransient) aHSeq =
    GetOperations()->MakePipeTShapeChamferWithPosition(theR1, theW1, theL1, theR2, theW2, theL2,
                                                       theRL, theWL, theLtransL, theLthinL,
                                                       theRR, theWR, theLtransR, theLthinR,
                                                       theRI, theWI, theLtransI, theLthinI,
                                                       theH, theW, theHexMesh, aP1, aP2, aP3);
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
 *  MakePipeTShapeTRFillet
 *  Create a T-shape object with specified caracteristics for the main and
 *  the incident pipes (radius, width, half-length). A fillet is created
 *  on the junction of the pipes.
 *  Center of the shape is (0,0,0). The main plane of the T-shape is XOY.
 *  \param theR1 Internal radius of main pipe
 *  \param theW1 Width of main pipe
 *  \param theL1 Half-length of main pipe
 *  \param theR2 Internal radius of incident pipe (R2 < R1)
 *  \param theW2 Width of incident pipe (R2+W2 < R1+W1)
 *  \param theL2 Half-length of incident pipe
 *  \param theRF Radius of curvature of fillet.
 *  \param theHexMesh Boolean indicating if shape is prepared for hex mesh
 *  \return List of GEOM_Objects, containing the created shape and propagation groups.
 */
//=============================================================================
GEOM::ListOfGO* AdvancedEngine_IOperations_i::MakePipeTShapeTRFillet
                     (CORBA::Double theR1, CORBA::Double theW1, CORBA::Double theL1,
                      CORBA::Double theR2, CORBA::Double theW2, CORBA::Double theL2,
                      CORBA::Double theRL, CORBA::Double theWL, CORBA::Double theLtransL, CORBA::Double theLthinL,
                      CORBA::Double theRR, CORBA::Double theWR, CORBA::Double theLtransR, CORBA::Double theLthinR,
                      CORBA::Double theRI, CORBA::Double theWI, CORBA::Double theLtransI, CORBA::Double theLthinI,
                      CORBA::Double theRF, CORBA::Boolean theHexMesh)
{
  GEOM::ListOfGO_var aSeq = new GEOM::ListOfGO;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Create the TShape
  Handle(TColStd_HSequenceOfTransient) aHSeq =
    GetOperations()->MakePipeTShapeFillet(theR1, theW1, theL1, theR2, theW2, theL2,
                                          theRL, theWL, theLtransL, theLthinL,
                                          theRR, theWR, theLtransR, theLthinR,
                                          theRI, theWI, theLtransI, theLthinI,
                                          theRF, theHexMesh);
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
 *  MakePipeTShapeTRFilletWithPosition
 *  Create a T-shape object with specified caracteristics for the main and
 *  the incident pipes (radius, width, half-length). A fillet is created
 *  on the junction of the pipes.
 *  The extremities of the main pipe are located on junctions points P1 and P2.
 *  The extremity of the incident pipe is located on junction point P3.
 *  \param theR1 Internal radius of main pipe
 *  \param theW1 Width of main pipe
 *  \param theL1 Half-length of main pipe
 *  \param theR2 Internal radius of incident pipe (R2 < R1)
 *  \param theW2 Width of incident pipe (R2+W2 < R1+W1)
 *  \param theL2 Half-length of incident pipe
 *  \param theRF Radius of curvature of fillet
 *  \param theHexMesh Boolean indicating if shape is prepared for hex mesh
 *  \param theP1 1st junction point of main pipe
 *  \param theP2 2nd junction point of main pipe
 *  \param theP3 Junction point of incident pipe
 *  \return List of GEOM_Objects, containing the created shape and propagation groups.
 */
//=============================================================================
GEOM::ListOfGO* AdvancedEngine_IOperations_i::MakePipeTShapeTRFilletWithPosition
                     (CORBA::Double theR1, CORBA::Double theW1, CORBA::Double theL1,
                      CORBA::Double theR2, CORBA::Double theW2, CORBA::Double theL2,
                      CORBA::Double theRL, CORBA::Double theWL, CORBA::Double theLtransL, CORBA::Double theLthinL,
                      CORBA::Double theRR, CORBA::Double theWR, CORBA::Double theLtransR, CORBA::Double theLthinR,
                      CORBA::Double theRI, CORBA::Double theWI, CORBA::Double theLtransI, CORBA::Double theLthinI,
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
  Handle(TColStd_HSequenceOfTransient) aHSeq =
    GetOperations()->MakePipeTShapeFilletWithPosition(theR1, theW1, theL1, theR2, theW2, theL2,
                                                      theRL, theWL, theLtransL, theLthinL,
                                                      theRR, theWR, theLtransR, theLthinR,
                                                      theRI, theWI, theLtransI, theLthinI,
                                                      theRF, theHexMesh, aP1, aP2, aP3);
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
 *  This function allows to create a disk already divided into blocks. It can be
 *  use to create divided pipes for later meshing in hexaedra.
 *  \param theR Radius of the disk
 *  \param theRatio Relative size of the central square diagonal against the disk diameter
 *  \return New GEOM_Object, containing the created shape.
 */
//=============================================================================
GEOM::GEOM_Object_ptr AdvancedEngine_IOperations_i::MakeDividedDisk (CORBA::Double theR,
								     CORBA::Double theRatio,
								     CORBA::Short theOrientation,
								     GEOM::pattern thePattern)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Create the DividedDisk
  Handle(GEOM_Object) anObject = GetOperations()->MakeDividedDisk(theR, theRatio, theOrientation, thePattern);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeDividedDiskPntVecR
 */
//=============================================================================
GEOM::GEOM_Object_ptr AdvancedEngine_IOperations_i::MakeDividedDiskPntVecR (GEOM::GEOM_Object_ptr thePnt,
									    GEOM::GEOM_Object_ptr theVec,
									    CORBA::Double theR,
									    CORBA::Double theRatio,
									    GEOM::pattern thePattern)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference points
  Handle(GEOM_Object) aPnt = GetObjectImpl(thePnt);
  Handle(GEOM_Object) aVec = GetObjectImpl(theVec);

  if (aPnt.IsNull() || aVec.IsNull()) return aGEOMObject._retn();

  // Make DividedDisk
  Handle(GEOM_Object) anObject =
    GetOperations()->MakeDividedDiskPntVecR(aPnt, aVec, theR, theRatio, thePattern);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  Builds a cylinder prepared for hexa meshes
 *  \param theR Radius of the cylinder
 *  \param theH Height of the cylinder
 *  \return New GEOM_Object, containing the created shape.
 */
//=============================================================================
GEOM::GEOM_Object_ptr AdvancedEngine_IOperations_i::MakeDividedCylinder (CORBA::Double theR,
									 CORBA::Double theH,
									 GEOM::pattern thePattern)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Create the DividedCylinder
  Handle(GEOM_Object) anObject = GetOperations()->MakeDividedCylinder(theR, theH, thePattern);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  
 *  \param thelPoints list of  points
 *  \param theNbMax maximum number of Bezier pieces in the resulting surface.
 *  \param theDegMax maximum degree of the resulting BSpline surface
 *  \param theDMax specifies maximum value of the GeomPlate_PlateG0Criterion criterion.
 *  \return New GEOM_Object, containing the created shape.
 */
//=============================================================================
GEOM::GEOM_Object_ptr AdvancedEngine_IOperations_i::MakeSmoothingSurface (const GEOM::ListOfGO& thelPoints,
									  CORBA::Long           theNbMax,
									  CORBA::Long           theDegMax,
									  CORBA::Double         theDMax)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference point
  int ind = 0;
  int aLen = thelPoints.length();
  std::list<Handle(GEOM_Object)> aPoints;
  for (; ind < aLen; ind++) {
    Handle(GEOM_Object) aPnt = GetObjectImpl(thelPoints[ind]);
    if (aPnt.IsNull()) return aGEOMObject._retn();
    aPoints.push_back(aPnt);
  }
  //Create the SmoothingSurface
  Handle(GEOM_Object) anObject = GetOperations()->MakeSmoothingSurface
        (aPoints, theNbMax, theDegMax, theDMax);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

AdvancedEngine_IOperations* AdvancedEngine_IOperations_i::GetOperations()
{
  return (AdvancedEngine_IOperations*)GetImpl();
}

