using namespace std;

#include "GEOM_IMeasureOperations_i.hh"

#include "utilities.h"
#include "OpUtil.hxx"

#include "GEOM_Engine.hxx"
#include "GEOM_Object.hxx"

//=============================================================================
/*!
 *   constructor:
 */
//=============================================================================
GEOM_IMeasureOperations_i::GEOM_IMeasureOperations_i (PortableServer::POA_ptr thePOA,
						    GEOM::GEOM_Gen_ptr theEngine,
						    ::GEOMImpl_IMeasureOperations* theImpl)
:GEOM_IOperations_i(thePOA, theEngine, theImpl)
{
  MESSAGE("GEOM_IMeasureOperations_i::GEOM_IMeasureOperations_i");
}

//=============================================================================
/*!
 *  destructor
 */
//=============================================================================
GEOM_IMeasureOperations_i::~GEOM_IMeasureOperations_i()
{
  MESSAGE("GEOM_IMeasureOperations_i::~GEOM_IMeasureOperations_i");
}


//=============================================================================
/*!
 *  GetCentreOfMass
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IMeasureOperations_i::GetCentreOfMass
                                              (GEOM::GEOM_Object_ptr theShape)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  if (theShape == NULL) return aGEOMObject._retn();

  //Get the reference shape
  Handle(GEOM_Object) aShape = GetOperations()->GetEngine()->GetObject
    (theShape->GetStudyID(), theShape->GetEntry());

  if (aShape.IsNull()) return aGEOMObject._retn();

  // Make Point - centre of mass of theShape
  Handle(GEOM_Object) anObject = GetOperations()->GetCentreOfMass(aShape);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  GetBasicProperties
 */
//=============================================================================
void GEOM_IMeasureOperations_i::GetBasicProperties (GEOM::GEOM_Object_ptr theShape,
						    CORBA::Double& theLength,
						    CORBA::Double& theSurfArea,
						    CORBA::Double& theVolume)
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  if (theShape == NULL) return;

  //Get the reference shape
  Handle(GEOM_Object) aShape = GetOperations()->GetEngine()->GetObject
    (theShape->GetStudyID(), theShape->GetEntry());

  if (aShape.IsNull()) return;

  // Get shape parameters
  GetOperations()->GetBasicProperties(aShape, theLength, theSurfArea, theVolume);
}

//=============================================================================
/*!
 *  GetInertia
 */
//=============================================================================
void GEOM_IMeasureOperations_i::GetInertia
  (GEOM::GEOM_Object_ptr theShape,
   CORBA::Double& I11, CORBA::Double& I12, CORBA::Double& I13,
   CORBA::Double& I21, CORBA::Double& I22, CORBA::Double& I23,
   CORBA::Double& I31, CORBA::Double& I32, CORBA::Double& I33,
   CORBA::Double& Ix , CORBA::Double& Iy , CORBA::Double& Iz)
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  if (theShape == NULL) return;

  //Get the reference shape
  Handle(GEOM_Object) aShape = GetOperations()->GetEngine()->GetObject
    (theShape->GetStudyID(), theShape->GetEntry());

  if (aShape.IsNull()) return;

  // Get shape parameters
  GetOperations()->GetInertia(aShape,
			      I11, I12, I13,
			      I21, I22, I23,
			      I31, I32, I33,
			      Ix , Iy , Iz);
}

//=============================================================================
/*!
 *  GetBoundingBox
 */
//=============================================================================
void GEOM_IMeasureOperations_i::GetBoundingBox (GEOM::GEOM_Object_ptr theShape,
						CORBA::Double& Xmin, CORBA::Double& Xmax,
						CORBA::Double& Ymin, CORBA::Double& Ymax,
						CORBA::Double& Zmin, CORBA::Double& Zmax)
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  if (theShape == NULL) return;

  //Get the reference shape
  Handle(GEOM_Object) aShape = GetOperations()->GetEngine()->GetObject
    (theShape->GetStudyID(), theShape->GetEntry());

  if (aShape.IsNull()) return;

  // Get shape parameters
  GetOperations()->GetBoundingBox(aShape, Xmin, Xmax, Ymin, Ymax, Zmin, Zmax);
}

//=============================================================================
/*!
 *  GetTolerance
 */
//=============================================================================
void GEOM_IMeasureOperations_i::GetTolerance
                                (GEOM::GEOM_Object_ptr theShape,
				 CORBA::Double& FaceMin, CORBA::Double& FaceMax,
				 CORBA::Double& EdgeMin, CORBA::Double& EdgeMax,
				 CORBA::Double& VertMin, CORBA::Double& VertMax)
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  if (theShape == NULL) return;

  //Get the reference shape
  Handle(GEOM_Object) aShape = GetOperations()->GetEngine()->GetObject
    (theShape->GetStudyID(), theShape->GetEntry());

  if (aShape.IsNull()) return;

  // Get shape parameters
  GetOperations()->GetTolerance(aShape,
				FaceMin, FaceMax,
				EdgeMin, EdgeMax,
				VertMin, VertMax);
}

//=============================================================================
/*!
 *  CheckShape
 */
//=============================================================================
CORBA::Boolean GEOM_IMeasureOperations_i::CheckShape (GEOM::GEOM_Object_ptr theShape,
						      CORBA::String_out     theDescription)
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  if (theShape == NULL) return 0;

  //Get the reference shape
  Handle(GEOM_Object) aShape = GetOperations()->GetEngine()->GetObject
    (theShape->GetStudyID(), theShape->GetEntry());

  if (aShape.IsNull()) return 0;

  // Get shape parameters
  TCollection_AsciiString aDump;
  if (GetOperations()->CheckShape(aShape, aDump)) return 1;

  theDescription = CORBA::string_dup(aDump.ToCString());
  return 0;
}

//=============================================================================
/*!
 *  WhatIs
 */
//=============================================================================
char* GEOM_IMeasureOperations_i::WhatIs (GEOM::GEOM_Object_ptr theShape)
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  if (theShape == NULL) return NULL;

  //Get the reference shape
  Handle(GEOM_Object) aShape = GetOperations()->GetEngine()->GetObject
    (theShape->GetStudyID(), theShape->GetEntry());

  if (aShape.IsNull()) return NULL;

  // Get shape parameters
  TCollection_AsciiString aDescription = GetOperations()->WhatIs(aShape);
  return strdup(aDescription.ToCString());
}

//=============================================================================
/*!
 *  GetMinDistance
 */
//=============================================================================
CORBA::Double GEOM_IMeasureOperations_i::GetMinDistance
  (GEOM::GEOM_Object_ptr theShape1, GEOM::GEOM_Object_ptr theShape2,
   CORBA::Double& X1, CORBA::Double& Y1, CORBA::Double& Z1,
   CORBA::Double& X2, CORBA::Double& Y2, CORBA::Double& Z2)
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  if (theShape1 == NULL || theShape2 == NULL) return -1.0;

  //Get the reference shape
  Handle(GEOM_Object) aShape1 = GetOperations()->GetEngine()->GetObject
    (theShape1->GetStudyID(), theShape1->GetEntry());
  Handle(GEOM_Object) aShape2 = GetOperations()->GetEngine()->GetObject
    (theShape2->GetStudyID(), theShape2->GetEntry());

  if (aShape1.IsNull() || aShape2.IsNull()) return -1.0;

  // Get shape parameters
  return GetOperations()->GetMinDistance(aShape1, aShape2, X1, Y1, Z1, X2, Y2, Z2);
}

//=============================================================================
/*!
 *  PointCoordinates
 */
//=============================================================================
void GEOM_IMeasureOperations_i::PointCoordinates(
  GEOM::GEOM_Object_ptr theShape, CORBA::Double& X, CORBA::Double& Y, CORBA::Double& Z )

{
  //Set a not done flag
  GetOperations()->SetNotDone();

  if ( theShape->_is_nil() )
    return;

  //Get the reference shape
  Handle(GEOM_Object) aShape = GetOperations()->GetEngine()->GetObject(
    theShape->GetStudyID(), theShape->GetEntry() );

  if ( aShape.IsNull() )
    return;

  // Get shape parameters
  GetOperations()->PointCoordinates( aShape, X, Y, Z );
}
