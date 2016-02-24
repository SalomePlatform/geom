// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "GEOM_IBlocksOperations_i.hh"

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
GEOM_IBlocksOperations_i::GEOM_IBlocksOperations_i (PortableServer::POA_ptr thePOA,
                                                    GEOM::GEOM_Gen_ptr theEngine,
                                                    ::GEOMImpl_IBlocksOperations* theImpl)
:GEOM_IOperations_i(thePOA, theEngine, theImpl)
{
  MESSAGE("GEOM_IBlocksOperations_i::GEOM_IBlocksOperations_i");
}

//=============================================================================
/*!
 *  destructor
 */
//=============================================================================
GEOM_IBlocksOperations_i::~GEOM_IBlocksOperations_i()
{
  MESSAGE("GEOM_IBlocksOperations_i::~GEOM_IBlocksOperations_i");
}


//=============================================================================
/*!
 *  MakeQuad
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBlocksOperations_i::MakeQuad
               (GEOM::GEOM_Object_ptr theEdge1, GEOM::GEOM_Object_ptr theEdge2,
                GEOM::GEOM_Object_ptr theEdge3, GEOM::GEOM_Object_ptr theEdge4)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference edges
  HANDLE_NAMESPACE(GEOM_Object) anEdge1 = GetObjectImpl(theEdge1);
  HANDLE_NAMESPACE(GEOM_Object) anEdge2 = GetObjectImpl(theEdge2);
  HANDLE_NAMESPACE(GEOM_Object) anEdge3 = GetObjectImpl(theEdge3);
  HANDLE_NAMESPACE(GEOM_Object) anEdge4 = GetObjectImpl(theEdge4);

  if (anEdge1.IsNull() || anEdge2.IsNull() ||
      anEdge3.IsNull() || anEdge4.IsNull()) return aGEOMObject._retn();

  //Create the Face
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->MakeQuad(anEdge1, anEdge2, anEdge3, anEdge4);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeQuad2Edges
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBlocksOperations_i::MakeQuad2Edges
               (GEOM::GEOM_Object_ptr theEdge1, GEOM::GEOM_Object_ptr theEdge2)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference edges
  HANDLE_NAMESPACE(GEOM_Object) anEdge1 = GetObjectImpl(theEdge1);
  HANDLE_NAMESPACE(GEOM_Object) anEdge2 = GetObjectImpl(theEdge2);

  if (anEdge1.IsNull() || anEdge2.IsNull()) return aGEOMObject._retn();

  //Create the Face
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->MakeQuad2Edges(anEdge1, anEdge2);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeQuad4Vertices
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBlocksOperations_i::MakeQuad4Vertices
                      (GEOM::GEOM_Object_ptr thePnt1, GEOM::GEOM_Object_ptr thePnt2,
                       GEOM::GEOM_Object_ptr thePnt3, GEOM::GEOM_Object_ptr thePnt4)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference points
  HANDLE_NAMESPACE(GEOM_Object) aPnt1 = GetObjectImpl(thePnt1);
  HANDLE_NAMESPACE(GEOM_Object) aPnt2 = GetObjectImpl(thePnt2);
  HANDLE_NAMESPACE(GEOM_Object) aPnt3 = GetObjectImpl(thePnt3);
  HANDLE_NAMESPACE(GEOM_Object) aPnt4 = GetObjectImpl(thePnt4);

  if (aPnt1.IsNull() || aPnt2.IsNull() ||
      aPnt3.IsNull() || aPnt4.IsNull()) return aGEOMObject._retn();

  //Create the Face
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->MakeQuad4Vertices(aPnt1, aPnt2, aPnt3, aPnt4);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeHexa
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBlocksOperations_i::MakeHexa
               (GEOM::GEOM_Object_ptr theFace1, GEOM::GEOM_Object_ptr theFace2,
                GEOM::GEOM_Object_ptr theFace3, GEOM::GEOM_Object_ptr theFace4,
                GEOM::GEOM_Object_ptr theFace5, GEOM::GEOM_Object_ptr theFace6)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference Faces
  HANDLE_NAMESPACE(GEOM_Object) anFace1 = GetObjectImpl(theFace1);
  HANDLE_NAMESPACE(GEOM_Object) anFace2 = GetObjectImpl(theFace2);
  HANDLE_NAMESPACE(GEOM_Object) anFace3 = GetObjectImpl(theFace3);
  HANDLE_NAMESPACE(GEOM_Object) anFace4 = GetObjectImpl(theFace4);
  HANDLE_NAMESPACE(GEOM_Object) anFace5 = GetObjectImpl(theFace5);
  HANDLE_NAMESPACE(GEOM_Object) anFace6 = GetObjectImpl(theFace6);

  if (anFace1.IsNull() || anFace2.IsNull() ||
      anFace3.IsNull() || anFace4.IsNull() ||
      anFace5.IsNull() || anFace6.IsNull()) return aGEOMObject._retn();

  //Create the Face
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetOperations()->MakeHexa
    (anFace1, anFace2, anFace3, anFace4, anFace5, anFace6);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeHexa2Faces
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBlocksOperations_i::MakeHexa2Faces
               (GEOM::GEOM_Object_ptr theFace1, GEOM::GEOM_Object_ptr theFace2)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference Faces
  HANDLE_NAMESPACE(GEOM_Object) anFace1 = GetObjectImpl(theFace1);
  HANDLE_NAMESPACE(GEOM_Object) anFace2 = GetObjectImpl(theFace2);

  if (anFace1.IsNull() || anFace2.IsNull()) return aGEOMObject._retn();

  //Create the Face
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->MakeHexa2Faces(anFace1, anFace2);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeBlockCompound
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBlocksOperations_i::MakeBlockCompound
                                            (GEOM::GEOM_Object_ptr theCompound)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference Compound
  HANDLE_NAMESPACE(GEOM_Object) aCompound = GetObjectImpl(theCompound);
  if (aCompound.IsNull()) return aGEOMObject._retn();

  //Create the Blocks Compound
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->MakeBlockCompound(aCompound);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  GetPoint
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBlocksOperations_i::GetPoint (GEOM::GEOM_Object_ptr theShape,
                                                          const CORBA::Double   theX,
                                                          const CORBA::Double   theY,
                                                          const CORBA::Double   theZ,
                                                          const CORBA::Double   theEpsilon)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference Objects
  HANDLE_NAMESPACE(GEOM_Object) aShape = GetObjectImpl(theShape);
  if (aShape.IsNull()) return aGEOMObject._retn();

  //Create the Point
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->GetPoint(aShape, theX, theY, theZ, theEpsilon);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  GetVertexNearPoint
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBlocksOperations_i::GetVertexNearPoint
                                                (GEOM::GEOM_Object_ptr theShape,
                                                 GEOM::GEOM_Object_ptr thePoint)
{
  GEOM::GEOM_Object_var aGEOMObject;

  // Set a not done flag
  GetOperations()->SetNotDone();

  // Get the reference Objects
  HANDLE_NAMESPACE(GEOM_Object) aShape = GetObjectImpl(theShape);
  HANDLE_NAMESPACE(GEOM_Object) aPoint = GetObjectImpl(thePoint);
  if (aShape.IsNull() || aPoint.IsNull()) return aGEOMObject._retn();

  // Create the Point
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->GetVertexNearPoint(aShape, aPoint);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  GetEdge
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBlocksOperations_i::GetEdge (GEOM::GEOM_Object_ptr theShape,
                                                         GEOM::GEOM_Object_ptr thePoint1,
                                                         GEOM::GEOM_Object_ptr thePoint2)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference Objects
  HANDLE_NAMESPACE(GEOM_Object) aShape = GetObjectImpl(theShape);
  HANDLE_NAMESPACE(GEOM_Object) aPoint1 = GetObjectImpl(thePoint1);
  HANDLE_NAMESPACE(GEOM_Object) aPoint2 = GetObjectImpl(thePoint2);

  if (aShape.IsNull() ||
      aPoint1.IsNull() || aPoint2.IsNull()) return aGEOMObject._retn();

  //Create the Point
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->GetEdge(aShape, aPoint1, aPoint2);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  GetEdgeNearPoint
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBlocksOperations_i::GetEdgeNearPoint (GEOM::GEOM_Object_ptr theShape,
                                                                  GEOM::GEOM_Object_ptr thePoint)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference Objects
  HANDLE_NAMESPACE(GEOM_Object) aShape = GetObjectImpl(theShape);
  HANDLE_NAMESPACE(GEOM_Object) aPoint = GetObjectImpl(thePoint);

  if (aShape.IsNull() || aPoint.IsNull()) return aGEOMObject._retn();

  //Create the Edge
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->GetEdgeNearPoint(aShape, aPoint);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  GetFaceByPoints
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBlocksOperations_i::GetFaceByPoints
                                              (GEOM::GEOM_Object_ptr theShape,
                                               GEOM::GEOM_Object_ptr thePoint1,
                                               GEOM::GEOM_Object_ptr thePoint2,
                                               GEOM::GEOM_Object_ptr thePoint3,
                                               GEOM::GEOM_Object_ptr thePoint4)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference Objects
  HANDLE_NAMESPACE(GEOM_Object) aShape = GetObjectImpl(theShape);
  HANDLE_NAMESPACE(GEOM_Object) aPoint1 = GetObjectImpl(thePoint1);
  HANDLE_NAMESPACE(GEOM_Object) aPoint2 = GetObjectImpl(thePoint2);
  HANDLE_NAMESPACE(GEOM_Object) aPoint3 = GetObjectImpl(thePoint3);
  HANDLE_NAMESPACE(GEOM_Object) aPoint4 = GetObjectImpl(thePoint4);

  if (aShape.IsNull() ||
      aPoint1.IsNull() || aPoint2.IsNull() ||
      aPoint3.IsNull() || aPoint4.IsNull()) return aGEOMObject._retn();

  //Create the Face
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->GetFaceByPoints(aShape, aPoint1, aPoint2, aPoint3, aPoint4);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  GetFaceByEdges
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBlocksOperations_i::GetFaceByEdges
                                               (GEOM::GEOM_Object_ptr theShape,
                                                GEOM::GEOM_Object_ptr theEdge1,
                                                GEOM::GEOM_Object_ptr theEdge2)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference Objects
  HANDLE_NAMESPACE(GEOM_Object) aShape = GetObjectImpl(theShape);
  HANDLE_NAMESPACE(GEOM_Object) anEdge1 = GetObjectImpl(theEdge1);
  HANDLE_NAMESPACE(GEOM_Object) anEdge2 = GetObjectImpl(theEdge2);

  if (aShape.IsNull() ||
      anEdge1.IsNull() || anEdge2.IsNull()) return aGEOMObject._retn();

  //Create the Face
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->GetFaceByEdges(aShape, anEdge1, anEdge2);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  GetOppositeFace
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBlocksOperations_i::GetOppositeFace (GEOM::GEOM_Object_ptr theShape,
                                                                 GEOM::GEOM_Object_ptr theFace)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference Objects
  HANDLE_NAMESPACE(GEOM_Object) aShape = GetObjectImpl(theShape);
  HANDLE_NAMESPACE(GEOM_Object) aFace = GetObjectImpl(theFace);

  if (aShape.IsNull() || aFace.IsNull()) return aGEOMObject._retn();

  //Create the Face
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->GetOppositeFace(aShape, aFace);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  GetFaceNearPoint
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBlocksOperations_i::GetFaceNearPoint (GEOM::GEOM_Object_ptr theShape,
                                                                  GEOM::GEOM_Object_ptr thePoint)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference Objects
  HANDLE_NAMESPACE(GEOM_Object) aShape = GetObjectImpl(theShape);
  HANDLE_NAMESPACE(GEOM_Object) aPoint = GetObjectImpl(thePoint);

  if (aShape.IsNull() || aPoint.IsNull()) return aGEOMObject._retn();

  //Create the Face
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->GetFaceNearPoint(aShape, aPoint);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  GetFaceByNormale
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBlocksOperations_i::GetFaceByNormale (GEOM::GEOM_Object_ptr theShape,
                                                                  GEOM::GEOM_Object_ptr theVector)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference Objects
  HANDLE_NAMESPACE(GEOM_Object) aShape = GetObjectImpl(theShape);
  HANDLE_NAMESPACE(GEOM_Object) aVector = GetObjectImpl(theVector);

  if (aShape.IsNull() || aVector.IsNull()) return aGEOMObject._retn();

  //Create the Face
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->GetFaceByNormale(aShape, aVector);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  GetShapesNearPoint
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBlocksOperations_i::GetShapesNearPoint
                      (GEOM::GEOM_Object_ptr theShape,
                       GEOM::GEOM_Object_ptr thePoint,
                       CORBA::Long           theShapeType,
                       CORBA::Double         theTolerance)
{
  GEOM::GEOM_Object_var aGEOMObject;

  // Set a not done flag
  GetOperations()->SetNotDone();

  // Get the reference Objects
  HANDLE_NAMESPACE(GEOM_Object) aShape = GetObjectImpl(theShape);
  HANDLE_NAMESPACE(GEOM_Object) aPoint = GetObjectImpl(thePoint);

  if (aShape.IsNull() || aPoint.IsNull()) return aGEOMObject._retn();

  // Create the Shape
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->GetShapesNearPoint(aShape, aPoint, theShapeType, theTolerance);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  ExplodeCompoundOfBlocks
 */
//=============================================================================
GEOM::ListOfGO* GEOM_IBlocksOperations_i::ExplodeCompoundOfBlocks
                                          (GEOM::GEOM_Object_ptr theCompound,
                                           const CORBA::Long     theMinNbFaces,
                                           const CORBA::Long     theMaxNbFaces)
{
  GEOM::ListOfGO_var aSeq = new GEOM::ListOfGO;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference Compound
  HANDLE_NAMESPACE(GEOM_Object) aCompound = GetObjectImpl(theCompound);
  if (aCompound.IsNull()) return aSeq._retn();

  //Explode
  Handle(TColStd_HSequenceOfTransient) aHSeq =
    GetOperations()->ExplodeCompoundOfBlocks(aCompound, theMinNbFaces, theMaxNbFaces);
  if (!GetOperations()->IsDone() || aHSeq.IsNull())
    return aSeq._retn();

  Standard_Integer aLength = aHSeq->Length();
  aSeq->length(aLength);
  for (Standard_Integer i = 1; i <= aLength; i++)
    aSeq[i-1] = GetObject(HANDLE_NAMESPACE(GEOM_Object)::DownCast(aHSeq->Value(i)));

  return aSeq._retn();
}

//=============================================================================
/*!
 *  IsCompoundOfBlocks
 */
//=============================================================================
CORBA::Boolean GEOM_IBlocksOperations_i::IsCompoundOfBlocks
                                          (GEOM::GEOM_Object_ptr theCompound,
                                           const CORBA::Long     theMinNbFaces,
                                           const CORBA::Long     theMaxNbFaces,
                                           CORBA::Long&          theNbBlocks)
{
  theNbBlocks = 0;
  CORBA::Boolean isComp = false;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference Compound
  HANDLE_NAMESPACE(GEOM_Object) aCompound = GetObjectImpl(theCompound);
  if (aCompound.IsNull()) return isComp;

  //Check
  Standard_Integer nbBlocks = 0;
  isComp = GetOperations()->IsCompoundOfBlocks(aCompound,
                                               theMinNbFaces,
                                               theMaxNbFaces,
                                               nbBlocks);
  if (!GetOperations()->IsDone())
    return isComp;

  theNbBlocks = nbBlocks;
  return isComp;
}

//=============================================================================
/*!
 *  CheckCompoundOfBlocks
 */
//=============================================================================
CORBA::Boolean GEOM_IBlocksOperations_i::CheckCompoundOfBlocks
                                          (GEOM::GEOM_Object_ptr theCompound,
                                           const CORBA::Double   theToleranceC1,
                                           GEOM::GEOM_IBlocksOperations::BCErrors_out theErrors)
{
  CORBA::Boolean isComp = false;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference Compound
  HANDLE_NAMESPACE(GEOM_Object) aCompound = GetObjectImpl(theCompound);
  if (aCompound.IsNull()) return isComp;

  //Check
  std::list<GEOMImpl_IBlocksOperations::BCError> errList;
  isComp = GetOperations()->CheckCompoundOfBlocks
    (aCompound, theToleranceC1, errList);
  if (!GetOperations()->IsDone())
    return isComp;

  const int nbErr = errList.size();
  GEOM::GEOM_IBlocksOperations::BCErrors_var anErrArray =
    new GEOM::GEOM_IBlocksOperations::BCErrors();
  anErrArray->length(nbErr);

  // fill the local CORBA array with values from lists
  std::list<GEOMImpl_IBlocksOperations::BCError>::iterator errIt = errList.begin();
  int i = 0;
  for (; errIt != errList.end(); i++, errIt++) {
    GEOM::GEOM_IBlocksOperations::BCError_var anError =
      new GEOM::GEOM_IBlocksOperations::BCError();

    GEOMImpl_IBlocksOperations::BCError errStruct = *errIt;

    switch (errStruct.error) {
    case GEOMImpl_IBlocksOperations::NOT_BLOCK:
      anError->error = GEOM::GEOM_IBlocksOperations::NOT_BLOCK;
      break;
    case GEOMImpl_IBlocksOperations::EXTRA_EDGE:
      anError->error = GEOM::GEOM_IBlocksOperations::EXTRA_EDGE;
      break;
    case GEOMImpl_IBlocksOperations::INVALID_CONNECTION:
      anError->error = GEOM::GEOM_IBlocksOperations::INVALID_CONNECTION;
      break;
    case GEOMImpl_IBlocksOperations::NOT_CONNECTED:
      anError->error = GEOM::GEOM_IBlocksOperations::NOT_CONNECTED;
      break;
    case GEOMImpl_IBlocksOperations::NOT_GLUED:
      anError->error = GEOM::GEOM_IBlocksOperations::NOT_GLUED;
      break;
    default:
      break;
    }

    std::list<int> sshList = errStruct.incriminated;
    GEOM::ListOfLong_var anIncrims = new GEOM::ListOfLong();
    anIncrims->length(sshList.size());

    std::list<int>::iterator sshIt = sshList.begin();
    int jj = 0;
    for (; sshIt != sshList.end(); jj++, sshIt++) {
      anIncrims[jj] = *sshIt;
    }

    anError->incriminated = anIncrims;

    anErrArray[i] = anError;
  }

  // initialize out-parameter with local array
  theErrors = anErrArray._retn();

  return isComp;
}

//=============================================================================
/*!
 *  PrintBCErrors
 */
//=============================================================================
char* GEOM_IBlocksOperations_i::PrintBCErrors
                      (GEOM::GEOM_Object_ptr theCompound,
                       const GEOM::GEOM_IBlocksOperations::BCErrors& theErrors)
{
  //Get the reference Compound
  HANDLE_NAMESPACE(GEOM_Object) aCompound = GetObjectImpl(theCompound);
  if (aCompound.IsNull()) return NULL;

  // Convert the errors sequence
  std::list<GEOMImpl_IBlocksOperations::BCError> anErrors;
  int nbErr = theErrors.length();
  int ie = 0;
  for (; ie < nbErr; ie++) {
    const GEOM::GEOM_IBlocksOperations::BCError err = theErrors[ie];
    const GEOM::GEOM_IBlocksOperations::BCErrorType typ = err.error;
    const GEOM::ListOfLong incrims = err.incriminated;

    GEOMImpl_IBlocksOperations::BCError errStruct;
    switch (typ) {
    case GEOM::GEOM_IBlocksOperations::NOT_BLOCK:
      errStruct.error = GEOMImpl_IBlocksOperations::NOT_BLOCK;
      break;
    case GEOM::GEOM_IBlocksOperations::EXTRA_EDGE:
      errStruct.error = GEOMImpl_IBlocksOperations::EXTRA_EDGE;
      break;
    case GEOM::GEOM_IBlocksOperations::INVALID_CONNECTION:
      errStruct.error = GEOMImpl_IBlocksOperations::INVALID_CONNECTION;
      break;
    case GEOM::GEOM_IBlocksOperations::NOT_CONNECTED:
      errStruct.error = GEOMImpl_IBlocksOperations::NOT_CONNECTED;
      break;
    case GEOM::GEOM_IBlocksOperations::NOT_GLUED:
      errStruct.error = GEOMImpl_IBlocksOperations::NOT_GLUED;
      break;
    default:
      break;
    }

    int ii = 0;
    int aLen = incrims.length();
    for (; ii < aLen; ii++) {
      errStruct.incriminated.push_back(incrims[ii]);
    }

    anErrors.push_back(errStruct);
  }

  TCollection_AsciiString aDescr = GetOperations()->PrintBCErrors(aCompound, anErrors);
  return CORBA::string_dup(aDescr.ToCString());
}

//=============================================================================
/*!
 *  GetNonBlocks
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBlocksOperations_i::GetNonBlocks
                                      (GEOM::GEOM_Object_ptr theShape,
                                       const CORBA::Double   theToleranceC1,
                                       GEOM::GEOM_Object_out theNonQuads)
{
  GEOM::GEOM_Object_var aGEOMObject;
  GEOM::GEOM_Object_var aNonQuads;

  theNonQuads = aNonQuads._retn();

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference Objects
  HANDLE_NAMESPACE(GEOM_Object) aShape = GetObjectImpl(theShape);
  if (aShape.IsNull()) return aGEOMObject._retn();

  //Get the result
  HANDLE_NAMESPACE(GEOM_Object) aFaces;
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->GetNonBlocks(aShape, theToleranceC1, aFaces);
  if (!GetOperations()->IsDone())
    return aGEOMObject._retn();

  if (!aFaces.IsNull())
    theNonQuads = GetObject(aFaces);

  if (anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  RemoveExtraEdges
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBlocksOperations_i::RemoveExtraEdges
                                      (GEOM::GEOM_Object_ptr theShape,
                                       CORBA::Long           theOptimumNbFaces)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference Objects
  HANDLE_NAMESPACE(GEOM_Object) aShape = GetObjectImpl(theShape);
  if (aShape.IsNull()) return aGEOMObject._retn();

  //Get the result
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetOperations()->RemoveExtraEdges(aShape, theOptimumNbFaces);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  UnionFaces
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBlocksOperations_i::UnionFaces
                                      (GEOM::GEOM_Object_ptr theShape)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference Objects
  HANDLE_NAMESPACE(GEOM_Object) aShape = GetObjectImpl(theShape);
  if (aShape.IsNull()) return aGEOMObject._retn();

  //Get the result
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetOperations()->UnionFaces(aShape);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  CheckAndImprove
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBlocksOperations_i::CheckAndImprove (GEOM::GEOM_Object_ptr theCompound)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference Objects
  HANDLE_NAMESPACE(GEOM_Object) aCompound = GetObjectImpl(theCompound);
  if (aCompound.IsNull()) return aGEOMObject._retn();

  //Get the result
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->CheckAndImprove(aCompound);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  GetBlockNearPoint
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBlocksOperations_i::GetBlockNearPoint (GEOM::GEOM_Object_ptr theCompound,
                                                                   GEOM::GEOM_Object_ptr thePoint)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference Objects
  HANDLE_NAMESPACE(GEOM_Object) aCompound = GetObjectImpl(theCompound);
  HANDLE_NAMESPACE(GEOM_Object) aPoint = GetObjectImpl(thePoint);

  if (aCompound.IsNull() || aPoint.IsNull()) return aGEOMObject._retn();

  //Get the block
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->GetBlockNearPoint(aCompound, aPoint);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  GetBlockByParts
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBlocksOperations_i::GetBlockByParts (GEOM::GEOM_Object_ptr theCompound,
                                                                 const GEOM::ListOfGO& theParts)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference Compound
  HANDLE_NAMESPACE(GEOM_Object) aCompound = GetObjectImpl(theCompound);
  if (aCompound.IsNull()) return aGEOMObject._retn();

  //Get the parts
  int ind, aLen;
  Handle(TColStd_HSequenceOfTransient) aParts = new TColStd_HSequenceOfTransient;

  aLen = theParts.length();
  for (ind = 0; ind < aLen; ind++) {
    HANDLE_NAMESPACE(GEOM_Object) aSh = GetObjectImpl(theParts[ind]);
    if (aSh.IsNull()) return aGEOMObject._retn();
    aParts->Append(aSh);
  }

  //Get the Block
  HANDLE_NAMESPACE(GEOM_Object) anObject =
    GetOperations()->GetBlockByParts(aCompound, aParts);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  GetBlocksByParts
 */
//=============================================================================
GEOM::ListOfGO* GEOM_IBlocksOperations_i::GetBlocksByParts (GEOM::GEOM_Object_ptr theCompound,
                                                            const GEOM::ListOfGO& theParts)
{
  GEOM::ListOfGO_var aSeq = new GEOM::ListOfGO;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference Compound
  HANDLE_NAMESPACE(GEOM_Object) aCompound = GetObjectImpl(theCompound);
  if (aCompound.IsNull()) return aSeq._retn();

  //Get the parts
  int ind, aLen;
  Handle(TColStd_HSequenceOfTransient) aParts = new TColStd_HSequenceOfTransient;

  aLen = theParts.length();
  for (ind = 0; ind < aLen; ind++) {
    HANDLE_NAMESPACE(GEOM_Object) aSh = GetObjectImpl(theParts[ind]);
    if (aSh.IsNull()) return aSeq._retn();
    aParts->Append(aSh);
  }

  //Get the Block
  Handle(TColStd_HSequenceOfTransient) aHSeq =
    GetOperations()->GetBlocksByParts(aCompound, aParts);
  if (!GetOperations()->IsDone() || aHSeq.IsNull())
    return aSeq._retn();

  Standard_Integer aLength = aHSeq->Length();
  aSeq->length(aLength);
  for (Standard_Integer i = 1; i <= aLength; i++)
    aSeq[i-1] = GetObject(HANDLE_NAMESPACE(GEOM_Object)::DownCast(aHSeq->Value(i)));

  return aSeq._retn();
}

//=============================================================================
/*!
 *  MakeMultiTransformation1D
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBlocksOperations_i::MakeMultiTransformation1D
                                              (GEOM::GEOM_Object_ptr theBlock,
                                               const CORBA::Long     theDirFace1,
                                               const CORBA::Long     theDirFace2,
                                               const CORBA::Long     theNbTimes)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the object itself and the vector of translation
  HANDLE_NAMESPACE(GEOM_Object) aBasicObject = GetObjectImpl(theBlock);
  if (aBasicObject.IsNull()) return aGEOMObject._retn();

  //Perform the transformation
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetOperations()->MakeMultiTransformation1D
    (aBasicObject, theDirFace1, theDirFace2, theNbTimes);
  if (!GetOperations()->IsDone() || anObject.IsNull())  return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeMultiTransformation2D
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBlocksOperations_i::MakeMultiTransformation2D
                                           (GEOM::GEOM_Object_ptr theBlock,
                                            const CORBA::Long     theDirFace1U,
                                            const CORBA::Long     theDirFace2U,
                                            const CORBA::Long     theNbTimesU,
                                            const CORBA::Long     theDirFace1V,
                                            const CORBA::Long     theDirFace2V,
                                            const CORBA::Long     theNbTimesV)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the object itself
  HANDLE_NAMESPACE(GEOM_Object) aBasicObject = GetObjectImpl(theBlock);
  if (aBasicObject.IsNull()) return aGEOMObject._retn();

  //Perform the transformation
  HANDLE_NAMESPACE(GEOM_Object) anObject = GetOperations()->MakeMultiTransformation2D
    (aBasicObject,
     theDirFace1U, theDirFace2U, theNbTimesU,
     theDirFace1V, theDirFace2V, theNbTimesV);
  if (!GetOperations()->IsDone() || anObject.IsNull()) return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  Propagate
 */
//=============================================================================
GEOM::ListOfGO* GEOM_IBlocksOperations_i::Propagate (GEOM::GEOM_Object_ptr theShape)
{
  GEOM::ListOfGO_var aSeq = new GEOM::ListOfGO;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference Shape
  HANDLE_NAMESPACE(GEOM_Object) aShape = GetObjectImpl(theShape);
  if (aShape.IsNull()) return aSeq._retn();

  //Get the Propagation chains
  Handle(TColStd_HSequenceOfTransient) aHSeq =
    GetOperations()->Propagate(aShape);
  if (!GetOperations()->IsDone() || aHSeq.IsNull())
    return aSeq._retn();

  Standard_Integer aLength = aHSeq->Length();
  aSeq->length(aLength);
  for (Standard_Integer i = 1; i <= aLength; i++)
    aSeq[i-1] = GetObject(HANDLE_NAMESPACE(GEOM_Object)::DownCast(aHSeq->Value(i)));

  return aSeq._retn();
}
