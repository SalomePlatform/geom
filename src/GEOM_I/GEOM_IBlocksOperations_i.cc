using namespace std;

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

  if (theEdge1 == NULL || theEdge2 == NULL ||
      theEdge3 == NULL || theEdge4 == NULL) return aGEOMObject._retn();

  //Get the reference edges
  Handle(GEOM_Object) anEdge1 = GetOperations()->GetEngine()->GetObject
    (theEdge1->GetStudyID(), theEdge1->GetEntry());
  Handle(GEOM_Object) anEdge2 = GetOperations()->GetEngine()->GetObject
    (theEdge2->GetStudyID(), theEdge2->GetEntry());
  Handle(GEOM_Object) anEdge3 = GetOperations()->GetEngine()->GetObject
    (theEdge3->GetStudyID(), theEdge3->GetEntry());
  Handle(GEOM_Object) anEdge4 = GetOperations()->GetEngine()->GetObject
    (theEdge4->GetStudyID(), theEdge4->GetEntry());

  if (anEdge1.IsNull() || anEdge2.IsNull() ||
      anEdge3.IsNull() || anEdge4.IsNull()) return aGEOMObject._retn();

  //Create the Face
  Handle(GEOM_Object) anObject =
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

  if (theEdge1 == NULL || theEdge2 == NULL) return aGEOMObject._retn();

  //Get the reference edges
  Handle(GEOM_Object) anEdge1 = GetOperations()->GetEngine()->GetObject
    (theEdge1->GetStudyID(), theEdge1->GetEntry());
  Handle(GEOM_Object) anEdge2 = GetOperations()->GetEngine()->GetObject
    (theEdge2->GetStudyID(), theEdge2->GetEntry());

  if (anEdge1.IsNull() || anEdge2.IsNull()) return aGEOMObject._retn();

  //Create the Face
  Handle(GEOM_Object) anObject =
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

  if (thePnt1 == NULL || thePnt2 == NULL ||
      thePnt3 == NULL || thePnt4 == NULL) return aGEOMObject._retn();

  //Get the reference points
  Handle(GEOM_Object) aPnt1 = GetOperations()->GetEngine()->GetObject
    (thePnt1->GetStudyID(), thePnt1->GetEntry());
  Handle(GEOM_Object) aPnt2 = GetOperations()->GetEngine()->GetObject
    (thePnt2->GetStudyID(), thePnt2->GetEntry());
  Handle(GEOM_Object) aPnt3 = GetOperations()->GetEngine()->GetObject
    (thePnt3->GetStudyID(), thePnt3->GetEntry());
  Handle(GEOM_Object) aPnt4 = GetOperations()->GetEngine()->GetObject
    (thePnt4->GetStudyID(), thePnt4->GetEntry());

  if (aPnt1.IsNull() || aPnt2.IsNull() ||
      aPnt3.IsNull() || aPnt4.IsNull()) return aGEOMObject._retn();

  //Create the Face
  Handle(GEOM_Object) anObject =
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

  if (theFace1 == NULL || theFace2 == NULL ||
      theFace3 == NULL || theFace4 == NULL ||
      theFace5 == NULL || theFace6 == NULL) return aGEOMObject._retn();

  //Get the reference Faces
  Handle(GEOM_Object) anFace1 = GetOperations()->GetEngine()->GetObject
    (theFace1->GetStudyID(), theFace1->GetEntry());
  Handle(GEOM_Object) anFace2 = GetOperations()->GetEngine()->GetObject
    (theFace2->GetStudyID(), theFace2->GetEntry());
  Handle(GEOM_Object) anFace3 = GetOperations()->GetEngine()->GetObject
    (theFace3->GetStudyID(), theFace3->GetEntry());
  Handle(GEOM_Object) anFace4 = GetOperations()->GetEngine()->GetObject
    (theFace4->GetStudyID(), theFace4->GetEntry());
  Handle(GEOM_Object) anFace5 = GetOperations()->GetEngine()->GetObject
    (theFace5->GetStudyID(), theFace5->GetEntry());
  Handle(GEOM_Object) anFace6 = GetOperations()->GetEngine()->GetObject
    (theFace6->GetStudyID(), theFace6->GetEntry());

  if (anFace1.IsNull() || anFace2.IsNull() ||
      anFace3.IsNull() || anFace4.IsNull() ||
      anFace5.IsNull() || anFace6.IsNull()) return aGEOMObject._retn();

  //Create the Face
  Handle(GEOM_Object) anObject = GetOperations()->MakeHexa
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

  if (theFace1 == NULL || theFace2 == NULL) return aGEOMObject._retn();

  //Get the reference Faces
  Handle(GEOM_Object) anFace1 = GetOperations()->GetEngine()->GetObject
    (theFace1->GetStudyID(), theFace1->GetEntry());
  Handle(GEOM_Object) anFace2 = GetOperations()->GetEngine()->GetObject
    (theFace2->GetStudyID(), theFace2->GetEntry());

  if (anFace1.IsNull() || anFace2.IsNull()) return aGEOMObject._retn();

  //Create the Face
  Handle(GEOM_Object) anObject =
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

  if (theCompound == NULL) return aGEOMObject._retn();

  //Get the reference Compound
  Handle(GEOM_Object) aCompound = GetOperations()->GetEngine()->GetObject
    (theCompound->GetStudyID(), theCompound->GetEntry());

  if (aCompound.IsNull()) return aGEOMObject._retn();

  //Create the Blocks Compound
  Handle(GEOM_Object) anObject =
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

  if (theShape == NULL) return aGEOMObject._retn();

  //Get the reference Objects
  Handle(GEOM_Object) aShape = GetOperations()->GetEngine()->GetObject
    (theShape->GetStudyID(), theShape->GetEntry());

  if (aShape.IsNull()) return aGEOMObject._retn();

  //Create the Point
  Handle(GEOM_Object) anObject =
    GetOperations()->GetPoint(aShape, theX, theY, theZ, theEpsilon);
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

  if (theShape == NULL ||
      thePoint1 == NULL || thePoint2 == NULL) return aGEOMObject._retn();

  //Get the reference Objects
  Handle(GEOM_Object) aShape = GetOperations()->GetEngine()->GetObject
    (theShape->GetStudyID(), theShape->GetEntry());
  Handle(GEOM_Object) aPoint1 = GetOperations()->GetEngine()->GetObject
    (thePoint1->GetStudyID(), thePoint1->GetEntry());
  Handle(GEOM_Object) aPoint2 = GetOperations()->GetEngine()->GetObject
    (thePoint2->GetStudyID(), thePoint2->GetEntry());

  if (aShape.IsNull() ||
      aPoint1.IsNull() || aPoint2.IsNull()) return aGEOMObject._retn();

  //Create the Point
  Handle(GEOM_Object) anObject =
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

  if (theShape == NULL || thePoint == NULL) return aGEOMObject._retn();

  //Get the reference Objects
  Handle(GEOM_Object) aShape = GetOperations()->GetEngine()->GetObject
    (theShape->GetStudyID(), theShape->GetEntry());
  Handle(GEOM_Object) aPoint = GetOperations()->GetEngine()->GetObject
    (thePoint->GetStudyID(), thePoint->GetEntry());

  if (aShape.IsNull() || aPoint.IsNull()) return aGEOMObject._retn();

  //Create the Edge
  Handle(GEOM_Object) anObject =
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

  if (theShape == NULL ||
      thePoint1 == NULL || thePoint2 == NULL ||
      thePoint3 == NULL || thePoint4 == NULL) return aGEOMObject._retn();

  //Get the reference Objects
  Handle(GEOM_Object) aShape = GetOperations()->GetEngine()->GetObject
    (theShape->GetStudyID(), theShape->GetEntry());
  Handle(GEOM_Object) aPoint1 = GetOperations()->GetEngine()->GetObject
    (thePoint1->GetStudyID(), thePoint1->GetEntry());
  Handle(GEOM_Object) aPoint2 = GetOperations()->GetEngine()->GetObject
    (thePoint2->GetStudyID(), thePoint2->GetEntry());
  Handle(GEOM_Object) aPoint3 = GetOperations()->GetEngine()->GetObject
    (thePoint3->GetStudyID(), thePoint3->GetEntry());
  Handle(GEOM_Object) aPoint4 = GetOperations()->GetEngine()->GetObject
    (thePoint4->GetStudyID(), thePoint4->GetEntry());

  if (aShape.IsNull() ||
      aPoint1.IsNull() || aPoint2.IsNull() ||
      aPoint3.IsNull() || aPoint4.IsNull()) return aGEOMObject._retn();

  //Create the Face
  Handle(GEOM_Object) anObject =
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

  if (theShape == NULL ||
      theEdge1 == NULL || theEdge2 == NULL) return aGEOMObject._retn();

  //Get the reference Objects
  Handle(GEOM_Object) aShape = GetOperations()->GetEngine()->GetObject
    (theShape->GetStudyID(), theShape->GetEntry());
  Handle(GEOM_Object) anEdge1 = GetOperations()->GetEngine()->GetObject
    (theEdge1->GetStudyID(), theEdge1->GetEntry());
  Handle(GEOM_Object) anEdge2 = GetOperations()->GetEngine()->GetObject
    (theEdge2->GetStudyID(), theEdge2->GetEntry());

  if (aShape.IsNull() ||
      anEdge1.IsNull() || anEdge2.IsNull()) return aGEOMObject._retn();

  //Create the Face
  Handle(GEOM_Object) anObject =
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

  if (theShape == NULL || theFace == NULL) return aGEOMObject._retn();

  //Get the reference Objects
  Handle(GEOM_Object) aShape = GetOperations()->GetEngine()->GetObject
    (theShape->GetStudyID(), theShape->GetEntry());
  Handle(GEOM_Object) aFace = GetOperations()->GetEngine()->GetObject
    (theFace->GetStudyID(), theFace->GetEntry());

  if (aShape.IsNull() || aFace.IsNull()) return aGEOMObject._retn();

  //Create the Face
  Handle(GEOM_Object) anObject =
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

  if (theShape == NULL || thePoint == NULL) return aGEOMObject._retn();

  //Get the reference Objects
  Handle(GEOM_Object) aShape = GetOperations()->GetEngine()->GetObject
    (theShape->GetStudyID(), theShape->GetEntry());
  Handle(GEOM_Object) aPoint = GetOperations()->GetEngine()->GetObject
    (thePoint->GetStudyID(), thePoint->GetEntry());

  if (aShape.IsNull() || aPoint.IsNull()) return aGEOMObject._retn();

  //Create the Face
  Handle(GEOM_Object) anObject =
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

  if (theShape == NULL || theVector == NULL) return aGEOMObject._retn();

  //Get the reference Objects
  Handle(GEOM_Object) aShape = GetOperations()->GetEngine()->GetObject
    (theShape->GetStudyID(), theShape->GetEntry());
  Handle(GEOM_Object) aVector = GetOperations()->GetEngine()->GetObject
    (theVector->GetStudyID(), theVector->GetEntry());

  if (aShape.IsNull() || aVector.IsNull()) return aGEOMObject._retn();

  //Create the Face
  Handle(GEOM_Object) anObject =
    GetOperations()->GetFaceByNormale(aShape, aVector);
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

  if (theCompound == NULL) return aSeq._retn();

  //Get the reference Compound
  Handle(GEOM_Object) aCompound = GetOperations()->GetEngine()->GetObject
    (theCompound->GetStudyID(), theCompound->GetEntry());

  if (aCompound.IsNull()) return aSeq._retn();

  //Explode
  Handle(TColStd_HSequenceOfTransient) aHSeq =
    GetOperations()->ExplodeCompoundOfBlocks(aCompound, theMinNbFaces, theMaxNbFaces);
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

  if (theCompound == NULL) return isComp;

  //Get the reference Compound
  Handle(GEOM_Object) aCompound = GetOperations()->GetEngine()->GetObject
    (theCompound->GetStudyID(), theCompound->GetEntry());

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
					   GEOM::GEOM_IBlocksOperations::BCErrors_out theErrors)
{
  CORBA::Boolean isComp = false;

  //Set a not done flag
  GetOperations()->SetNotDone();

  if (theCompound == NULL) return isComp;

  //Get the reference Compound
  Handle(GEOM_Object) aCompound = GetOperations()->GetEngine()->GetObject
    (theCompound->GetStudyID(), theCompound->GetEntry());

  if (aCompound.IsNull()) return isComp;

  //Check
  list<GEOMImpl_IBlocksOperations::BCError> errList;
  isComp = GetOperations()->CheckCompoundOfBlocks(aCompound, errList);
  if (!GetOperations()->IsDone())
    return isComp;

  const int nbErr = errList.size();
  GEOM::GEOM_IBlocksOperations::BCErrors_var anErrArray =
    new GEOM::GEOM_IBlocksOperations::BCErrors();
  anErrArray->length(nbErr);

  // fill the local CORBA array with values from lists
  list<GEOMImpl_IBlocksOperations::BCError>::iterator errIt = errList.begin();
  int i = 0;
  for (; errIt != errList.end(); i++, errIt++) {
    GEOM::GEOM_IBlocksOperations::BCError_var anError =
      new GEOM::GEOM_IBlocksOperations::BCError();

    GEOMImpl_IBlocksOperations::BCError errStruct = *errIt;

    switch (errStruct.error) {
    case GEOMImpl_IBlocksOperations::NOT_BLOCK:
      anError->error = GEOM::GEOM_IBlocksOperations::NOT_BLOCK;
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

    list<int> sshList = errStruct.incriminated;
    GEOM::ListOfLong_var anIncrims = new GEOM::ListOfLong();
    anIncrims->length(sshList.size());

    list<int>::iterator sshIt = sshList.begin();
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
  if (theCompound == NULL) return NULL;
  Handle(GEOM_Object) aCompound = GetOperations()->GetEngine()->GetObject
    (theCompound->GetStudyID(), theCompound->GetEntry());
  if (aCompound.IsNull()) return NULL;

  // Convert the errors sequence
  list<GEOMImpl_IBlocksOperations::BCError> anErrors;
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
 *  GetBlockNearPoint
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBlocksOperations_i::GetBlockNearPoint (GEOM::GEOM_Object_ptr theCompound,
								   GEOM::GEOM_Object_ptr thePoint)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  if (theCompound == NULL || thePoint == NULL) return aGEOMObject._retn();

  //Get the reference Objects
  Handle(GEOM_Object) aCompound = GetOperations()->GetEngine()->GetObject
    (theCompound->GetStudyID(), theCompound->GetEntry());
  Handle(GEOM_Object) aPoint = GetOperations()->GetEngine()->GetObject
    (thePoint->GetStudyID(), thePoint->GetEntry());

  if (aCompound.IsNull() || aPoint.IsNull()) return aGEOMObject._retn();

  //Get the block
  Handle(GEOM_Object) anObject =
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

  if (theCompound == NULL) return aGEOMObject._retn();

  //Get the reference Compound
  Handle(GEOM_Object) aCompound = GetOperations()->GetEngine()->GetObject
    (theCompound->GetStudyID(), theCompound->GetEntry());

  if (aCompound.IsNull()) return aGEOMObject._retn();

  //Get the parts
  int ind, aLen;
  Handle(TColStd_HSequenceOfTransient) aParts = new TColStd_HSequenceOfTransient;

  aLen = theParts.length();
  for (ind = 0; ind < aLen; ind++) {
    if (theParts[ind] == NULL) return aGEOMObject._retn();
    Handle(GEOM_Object) aSh = GetOperations()->GetEngine()->GetObject
      (theParts[ind]->GetStudyID(), theParts[ind]->GetEntry());
    if (aSh.IsNull()) return aGEOMObject._retn();
    aParts->Append(aSh);
  }

  //Get the Block
  Handle(GEOM_Object) anObject =
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

  if (theCompound == NULL) return aSeq._retn();

  //Get the reference Compound
  Handle(GEOM_Object) aCompound = GetOperations()->GetEngine()->GetObject
    (theCompound->GetStudyID(), theCompound->GetEntry());

  if (aCompound.IsNull()) return aSeq._retn();

  //Get the parts
  int ind, aLen;
  Handle(TColStd_HSequenceOfTransient) aParts = new TColStd_HSequenceOfTransient;

  aLen = theParts.length();
  for (ind = 0; ind < aLen; ind++) {
    if (theParts[ind] == NULL) return aSeq._retn();
    Handle(GEOM_Object) aSh = GetOperations()->GetEngine()->GetObject
      (theParts[ind]->GetStudyID(), theParts[ind]->GetEntry());
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
    aSeq[i-1] = GetObject(Handle(GEOM_Object)::DownCast(aHSeq->Value(i)));

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

  if (theBlock == NULL) return aGEOMObject._retn();

   //Get the object itself and the vector of translation
   Handle(GEOM_Object) aBasicObject = GetOperations()->GetEngine()->GetObject
     (theBlock->GetStudyID(), theBlock->GetEntry());

   if (aBasicObject.IsNull()) return aGEOMObject._retn();

   //Perform the transformation
   Handle(GEOM_Object) anObject = GetOperations()->MakeMultiTransformation1D
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

  if (theBlock == NULL) return aGEOMObject._retn();

  //Get the object itself
  Handle(GEOM_Object) aBasicObject = GetOperations()->GetEngine()->GetObject
    (theBlock->GetStudyID(), theBlock->GetEntry());

  if (aBasicObject.IsNull()) return aGEOMObject._retn();

  //Perform the transformation
  Handle(GEOM_Object) anObject = GetOperations()->MakeMultiTransformation2D
    (aBasicObject,
     theDirFace1U, theDirFace2U, theNbTimesU,
     theDirFace1V, theDirFace2V, theNbTimesV);
  if (!GetOperations()->IsDone() || anObject.IsNull()) return aGEOMObject._retn();

  return GetObject(anObject);
}
