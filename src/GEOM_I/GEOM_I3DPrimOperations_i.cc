using namespace std;

#include "GEOM_I3DPrimOperations_i.hh"

#include "utilities.h"
#include "OpUtil.hxx"
#include "Utils_ExceptHandlers.hxx"

#include "GEOM_Engine.hxx"
#include "GEOM_Object.hxx"

//=============================================================================
/*!
 *   constructor:
 */
//=============================================================================
GEOM_I3DPrimOperations_i::GEOM_I3DPrimOperations_i(PortableServer::POA_ptr thePOA, GEOM::GEOM_Gen_ptr theEngine, ::GEOMImpl_I3DPrimOperations* theImpl)
:GEOM_IOperations_i(thePOA, theEngine, theImpl)
{
  MESSAGE("GEOM_I3DPrimOperations_i::GEOM_I3DPrimOperations_i");
}

//=============================================================================
/*!
 *  destructor
 */
//=============================================================================
GEOM_I3DPrimOperations_i::~GEOM_I3DPrimOperations_i()
{
  MESSAGE("GEOM_I3DPrimOperations_i::~GEOM_I3DPrimOperations_i");
}


//=============================================================================
/*!
 *  MakeBoxDXDYDZ
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_I3DPrimOperations_i::MakeBoxDXDYDZ (CORBA::Double theDX,
							       CORBA::Double theDY,
							       CORBA::Double theDZ)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Create the Box
  Handle(GEOM_Object) anObject = GetOperations()->MakeBoxDXDYDZ(theDX, theDY, theDZ);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeBoxTwoPnt
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_I3DPrimOperations_i::MakeBoxTwoPnt
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

  //Create the Box
  Handle(GEOM_Object) anObject = GetOperations()->MakeBoxTwoPnt(aPnt1, aPnt2);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeCylinderRH
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_I3DPrimOperations_i::MakeCylinderRH (CORBA::Double theR,
								CORBA::Double theH)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Create the Cylinder
  Handle(GEOM_Object) anObject = GetOperations()->MakeCylinderRH(theR, theH);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeCylinderPntVecRH
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_I3DPrimOperations_i::MakeCylinderPntVecRH
                      (GEOM::GEOM_Object_ptr thePnt, GEOM::GEOM_Object_ptr theVec,
		       CORBA::Double theR, CORBA::Double theH)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  if (thePnt == NULL || theVec == NULL) return aGEOMObject._retn();

  //Get the reference points
  Handle(GEOM_Object) aPnt = GetOperations()->GetEngine()->GetObject
    (thePnt->GetStudyID(), thePnt->GetEntry());
  Handle(GEOM_Object) aVec = GetOperations()->GetEngine()->GetObject
    (theVec->GetStudyID(), theVec->GetEntry());

  if (aPnt.IsNull() || aVec.IsNull()) return aGEOMObject._retn();

  //Create the Cylinder
  Handle(GEOM_Object) anObject = GetOperations()->MakeCylinderPntVecRH(aPnt, aVec, theR, theH);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeConeR1R2H
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_I3DPrimOperations_i::MakeConeR1R2H (CORBA::Double theR1,
							       CORBA::Double theR2,
							       CORBA::Double theH)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Create the Cone
  Handle(GEOM_Object) anObject = GetOperations()->MakeConeR1R2H(theR1, theR2, theH);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeConePntVecR1R2H
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_I3DPrimOperations_i::MakeConePntVecR1R2H
                      (GEOM::GEOM_Object_ptr thePnt, GEOM::GEOM_Object_ptr theVec,
		       CORBA::Double theR1, CORBA::Double theR2, CORBA::Double theH)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  if (thePnt == NULL || theVec == NULL) return aGEOMObject._retn();

  //Get the reference points
  Handle(GEOM_Object) aPnt = GetOperations()->GetEngine()->GetObject
    (thePnt->GetStudyID(), thePnt->GetEntry());
  Handle(GEOM_Object) aVec = GetOperations()->GetEngine()->GetObject
    (theVec->GetStudyID(), theVec->GetEntry());

  if (aPnt.IsNull() || aVec.IsNull()) return aGEOMObject._retn();

  //Create the Cone
  Handle(GEOM_Object) anObject =
      GetOperations()->MakeConePntVecR1R2H(aPnt, aVec, theR1, theR2, theH);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeSphereR
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_I3DPrimOperations_i::MakeSphereR (CORBA::Double theR)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Create the Cone
  Handle(GEOM_Object) anObject = GetOperations()->MakeSphereR(theR);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeSpherePntR
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_I3DPrimOperations_i::MakeSpherePntR
                      (GEOM::GEOM_Object_ptr thePnt, CORBA::Double theR)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  if (thePnt == NULL) return aGEOMObject._retn();

  //Get the reference point
  Handle(GEOM_Object) aPnt = GetOperations()->GetEngine()->GetObject
    (thePnt->GetStudyID(), thePnt->GetEntry());

  if (aPnt.IsNull()) return aGEOMObject._retn();

  //Create the Sphere
  Handle(GEOM_Object) anObject =
      GetOperations()->MakeSpherePntR(aPnt, theR);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeTorusRR
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_I3DPrimOperations_i::MakeTorusRR
                      (CORBA::Double theRMajor, CORBA::Double theRMinor)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  // Make Torus
  Handle(GEOM_Object) anObject =
    GetOperations()->MakeTorusRR(theRMajor, theRMinor);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeTorusPntVecRR
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_I3DPrimOperations_i::MakeTorusPntVecRR
                      (GEOM::GEOM_Object_ptr thePnt, GEOM::GEOM_Object_ptr theVec,
		       CORBA::Double theRMajor, CORBA::Double theRMinor)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  if (thePnt == NULL || theVec == NULL) return aGEOMObject._retn();

  //Get the reference points
  Handle(GEOM_Object) aPnt = GetOperations()->GetEngine()->GetObject
    (thePnt->GetStudyID(), thePnt->GetEntry());
  Handle(GEOM_Object) aVec = GetOperations()->GetEngine()->GetObject
    (theVec->GetStudyID(), theVec->GetEntry());

  if (aPnt.IsNull() || aVec.IsNull()) return aGEOMObject._retn();

  // Make Torus
  Handle(GEOM_Object) anObject =
    GetOperations()->MakeTorusPntVecRR(aPnt, aVec, theRMajor, theRMinor);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakePrismVecH
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_I3DPrimOperations_i::MakePrismVecH
                      (GEOM::GEOM_Object_ptr theBase, GEOM::GEOM_Object_ptr theVec,
		       CORBA::Double theH)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  if (theBase == NULL || theVec == NULL) return aGEOMObject._retn();

  //Get the reference objects
  Handle(GEOM_Object) aBase = GetOperations()->GetEngine()->GetObject
    (theBase->GetStudyID(), theBase->GetEntry());
  Handle(GEOM_Object) aVec = GetOperations()->GetEngine()->GetObject
    (theVec->GetStudyID(), theVec->GetEntry());

  if (aBase.IsNull() || aVec.IsNull()) return aGEOMObject._retn();

  //Create the Prism
  Handle(GEOM_Object) anObject =
      GetOperations()->MakePrismVecH(aBase, aVec, theH);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakePrismTwoPnt
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_I3DPrimOperations_i::MakePrismTwoPnt
                                             (GEOM::GEOM_Object_ptr theBase,
					      GEOM::GEOM_Object_ptr thePoint1,
					      GEOM::GEOM_Object_ptr thePoint2)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  if (theBase == NULL || thePoint1 == NULL || thePoint2 == NULL)
    return aGEOMObject._retn();

  //Get the reference objects
  Handle(GEOM_Object) aBase = GetOperations()->GetEngine()->GetObject
    (theBase->GetStudyID(), theBase->GetEntry());
  Handle(GEOM_Object) aPoint1 = GetOperations()->GetEngine()->GetObject
    (thePoint1->GetStudyID(), thePoint1->GetEntry());
  Handle(GEOM_Object) aPoint2 = GetOperations()->GetEngine()->GetObject
    (thePoint2->GetStudyID(), thePoint2->GetEntry());

  if (aBase.IsNull() || aPoint1.IsNull() || aPoint2.IsNull())
    return aGEOMObject._retn();

  //Create the Prism
  Handle(GEOM_Object) anObject =
    GetOperations()->MakePrismTwoPnt(aBase, aPoint1, aPoint2);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakePipe
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_I3DPrimOperations_i::MakePipe
                 (GEOM::GEOM_Object_ptr theBase, GEOM::GEOM_Object_ptr thePath)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  if (theBase == NULL || thePath == NULL) return aGEOMObject._retn();

  //Get the reference objects
  Handle(GEOM_Object) aBase = GetOperations()->GetEngine()->GetObject
    (theBase->GetStudyID(), theBase->GetEntry());
  Handle(GEOM_Object) aPath = GetOperations()->GetEngine()->GetObject
    (thePath->GetStudyID(), thePath->GetEntry());

  if (aBase.IsNull() || aPath.IsNull()) return aGEOMObject._retn();

  //Create the Pipe
  Handle(GEOM_Object) anObject =
    GetOperations()->MakePipe(aBase, aPath);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeRevolutionAxisAngle
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_I3DPrimOperations_i::MakeRevolutionAxisAngle
                      (GEOM::GEOM_Object_ptr theBase, GEOM::GEOM_Object_ptr theAxis,
		       CORBA::Double theAngle)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  if (theBase == NULL || theAxis == NULL) return aGEOMObject._retn();

  //Get the reference objects
  Handle(GEOM_Object) aBase = GetOperations()->GetEngine()->GetObject
    (theBase->GetStudyID(), theBase->GetEntry());
  Handle(GEOM_Object) anAxis = GetOperations()->GetEngine()->GetObject
    (theAxis->GetStudyID(), theAxis->GetEntry());

  if (aBase.IsNull() || anAxis.IsNull()) return aGEOMObject._retn();

  //Create the Revolution
  Handle(GEOM_Object) anObject =
      GetOperations()->MakeRevolutionAxisAngle(aBase, anAxis, theAngle);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeFilling
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_I3DPrimOperations_i::MakeFilling(GEOM::GEOM_Object_ptr theShape,
							    CORBA::Long theMinDeg,
							    CORBA::Long theMaxDeg,
							    CORBA::Double theTol2D,
							    CORBA::Double theTol3D,
							    CORBA::Long theNbIter)
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
  Handle(GEOM_Object) anObject = GetOperations()->MakeFilling(aShape, theMinDeg, theMaxDeg, theTol2D, theTol3D, theNbIter);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}
