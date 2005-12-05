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

#include "GEOM_IBasicOperations_i.hh"

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
GEOM_IBasicOperations_i::GEOM_IBasicOperations_i (PortableServer::POA_ptr thePOA,
						  GEOM::GEOM_Gen_ptr theEngine,
						  ::GEOMImpl_IBasicOperations* theImpl)
     :GEOM_IOperations_i(thePOA, theEngine, theImpl)
{
  MESSAGE("GEOM_IBasicOperations_i::GEOM_IBasicOperations_i");
}

//=============================================================================
/*!
 *  destructor
 */
//=============================================================================
GEOM_IBasicOperations_i::~GEOM_IBasicOperations_i()
{
  MESSAGE("GEOM_IBasicOperations_i::~GEOM_IBasicOperations_i");
}


//=============================================================================
/*!
 *  MakePointXYZ
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBasicOperations_i::MakePointXYZ
  (CORBA::Double theX, CORBA::Double theY, CORBA::Double theZ)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

   //Create the point

  Handle(GEOM_Object) anObject = GetOperations()->MakePointXYZ(theX, theY, theZ);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakePointWithReference
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBasicOperations_i::MakePointWithReference
  (GEOM::GEOM_Object_ptr theReference, CORBA::Double theX, CORBA::Double theY, CORBA::Double theZ)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  if(theReference == NULL) return aGEOMObject._retn();

  //Get the reference point

  Handle(GEOM_Object) aRefernce = GetOperations()->GetEngine()->GetObject
    (theReference->GetStudyID(), theReference->GetEntry());
  if (aRefernce.IsNull()) return aGEOMObject._retn();

  //Create the point

  Handle(GEOM_Object) anObject =
    GetOperations()->MakePointWithReference(aRefernce, theX, theY, theZ);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakePointOnCurve
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBasicOperations_i::MakePointOnCurve
                  (GEOM::GEOM_Object_ptr theCurve,  CORBA::Double theParameter)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  if (theCurve == NULL) return aGEOMObject._retn();

  //Get the reference curve

  Handle(GEOM_Object) aRefernce = GetOperations()->GetEngine()->GetObject
    (theCurve->GetStudyID(), theCurve->GetEntry());
  if (aRefernce.IsNull()) return aGEOMObject._retn();

  //Create the point

  Handle(GEOM_Object) anObject =
    GetOperations()->MakePointOnCurve(aRefernce, theParameter);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}


//=============================================================================
/*!
 *  MakeVectorDXDYDZ
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBasicOperations_i::MakeVectorDXDYDZ
  (CORBA::Double theDX, CORBA::Double theDY, CORBA::Double theDZ)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Create the Vector

  Handle(GEOM_Object) anObject = GetOperations()->MakeVectorDXDYDZ(theDX, theDY, theDZ);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeVectorTwoPnt
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBasicOperations_i::MakeVectorTwoPnt
                 (GEOM::GEOM_Object_ptr thePnt1, GEOM::GEOM_Object_ptr thePnt2)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  if (thePnt1 == NULL || thePnt2 == NULL) return aGEOMObject._retn();

  //Get the reference points

  Handle(GEOM_Object) aRef1 = GetOperations()->GetEngine()->GetObject
    (thePnt1->GetStudyID(), thePnt1->GetEntry());
  Handle(GEOM_Object) aRef2 = GetOperations()->GetEngine()->GetObject
    (thePnt2->GetStudyID(), thePnt2->GetEntry());
  if (aRef1.IsNull() || aRef2.IsNull()) return aGEOMObject._retn();

  //Create the vector

  Handle(GEOM_Object) anObject =
    GetOperations()->MakeVectorTwoPnt(aRef1, aRef2);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}


//=============================================================================
/*!
 *  MakeLine
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBasicOperations_i::MakeLine
                   (GEOM::GEOM_Object_ptr thePnt, GEOM::GEOM_Object_ptr theDir)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  if (thePnt == NULL || theDir == NULL) return aGEOMObject._retn();

  //Get the reference objects

  Handle(GEOM_Object) aRef1 = GetOperations()->GetEngine()->GetObject
    (thePnt->GetStudyID(), thePnt->GetEntry());
  Handle(GEOM_Object) aRef2 = GetOperations()->GetEngine()->GetObject
    (theDir->GetStudyID(), theDir->GetEntry());
  if (aRef1.IsNull() || aRef2.IsNull()) return aGEOMObject._retn();

  //Create the Line

  Handle(GEOM_Object) anObject =
    GetOperations()->MakeLine(aRef1, aRef2);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeLineTwoPnt
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBasicOperations_i::MakeLineTwoPnt
                 (GEOM::GEOM_Object_ptr thePnt1, GEOM::GEOM_Object_ptr thePnt2)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  if (thePnt1 == NULL || thePnt2 == NULL) return aGEOMObject._retn();

  //Get the reference points

  Handle(GEOM_Object) aRef1 = GetOperations()->GetEngine()->GetObject
    (thePnt1->GetStudyID(), thePnt1->GetEntry());
  Handle(GEOM_Object) aRef2 = GetOperations()->GetEngine()->GetObject
    (thePnt2->GetStudyID(), thePnt2->GetEntry());
  if (aRef1.IsNull() || aRef2.IsNull()) return aGEOMObject._retn();

  //Create the Line

  Handle(GEOM_Object) anObject =
    GetOperations()->MakeLineTwoPnt(aRef1, aRef2);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}


//=============================================================================
/*!
 *  MakePlanePntVec
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBasicOperations_i::MakePlanePntVec
                 (GEOM::GEOM_Object_ptr thePnt, GEOM::GEOM_Object_ptr theVec,
		  CORBA::Double theTrimSize)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  if (thePnt == NULL || theVec == NULL) return aGEOMObject._retn();

  //Get the references

  Handle(GEOM_Object) aRef1 = GetOperations()->GetEngine()->GetObject
    (thePnt->GetStudyID(), thePnt->GetEntry());
  Handle(GEOM_Object) aRef2 = GetOperations()->GetEngine()->GetObject
    (theVec->GetStudyID(), theVec->GetEntry());
  if (aRef1.IsNull() || aRef2.IsNull()) return aGEOMObject._retn();

  //Create the plane

  Handle(GEOM_Object) anObject =
    GetOperations()->MakePlanePntVec(aRef1, aRef2, theTrimSize);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakePlaneThreePnt
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBasicOperations_i::MakePlaneThreePnt
                 (GEOM::GEOM_Object_ptr thePnt1, GEOM::GEOM_Object_ptr thePnt2,
		  GEOM::GEOM_Object_ptr thePnt3, CORBA::Double theTrimSize)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  if (thePnt1 == NULL || thePnt2 == NULL || thePnt3 == NULL)
    return aGEOMObject._retn();

  //Get the reference points

  Handle(GEOM_Object) aRef1 = GetOperations()->GetEngine()->GetObject
    (thePnt1->GetStudyID(), thePnt1->GetEntry());
  Handle(GEOM_Object) aRef2 = GetOperations()->GetEngine()->GetObject
    (thePnt2->GetStudyID(), thePnt2->GetEntry());
  Handle(GEOM_Object) aRef3 = GetOperations()->GetEngine()->GetObject
    (thePnt3->GetStudyID(), thePnt3->GetEntry());
  if (aRef1.IsNull() || aRef2.IsNull() || aRef3.IsNull())
    return aGEOMObject._retn();

  //Create the plane

  Handle(GEOM_Object) anObject =
    GetOperations()->MakePlaneThreePnt(aRef1, aRef2, aRef3, theTrimSize);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakePlaneFace
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBasicOperations_i::MakePlaneFace
                     (GEOM::GEOM_Object_ptr theFace, CORBA::Double theTrimSize)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  if (theFace == NULL) return aGEOMObject._retn();

  //Get the reference face

  Handle(GEOM_Object) aRef = GetOperations()->GetEngine()->GetObject
    (theFace->GetStudyID(), theFace->GetEntry());
  if (aRef.IsNull()) return aGEOMObject._retn();

  //Create the plane

  Handle(GEOM_Object) anObject =
    GetOperations()->MakePlaneFace(aRef, theTrimSize);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}


//=============================================================================
/*!
 *  MakeMarker
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IBasicOperations_i::MakeMarker
  (CORBA::Double theOX , CORBA::Double theOY , CORBA::Double theOZ,
   CORBA::Double theXDX, CORBA::Double theXDY, CORBA::Double theXDZ,
   CORBA::Double theYDX, CORBA::Double theYDY, CORBA::Double theYDZ)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Create the point
  Handle(GEOM_Object) anObject = GetOperations()->MakeMarker(theOX , theOY , theOZ,
							     theXDX, theXDY, theXDZ,
							     theYDX, theYDY, theYDZ);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}
