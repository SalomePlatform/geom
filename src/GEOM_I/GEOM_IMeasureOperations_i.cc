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
 *  KindOfShape
 */
//=============================================================================
GEOM::GEOM_IKindOfShape::shape_kind GEOM_IMeasureOperations_i::KindOfShape
                                   (GEOM::GEOM_Object_ptr  theShape,
                                    GEOM::ListOfLong_out   theIntegers,
                                    GEOM::ListOfDouble_out theDoubles)
{
  GEOMImpl_IMeasureOperations::ShapeKind aKind = GEOMImpl_IMeasureOperations::SK_NO_SHAPE;

  // allocate the CORBA arrays
  GEOM::ListOfLong_var anIntegersArray = new GEOM::ListOfLong();
  GEOM::ListOfDouble_var aDoublesArray = new GEOM::ListOfDouble();

  //Get the reference shape
  Handle(GEOM_Object) aShape = GetObjectImpl(theShape);

  if (!aShape.IsNull()) {
    Handle(TColStd_HSequenceOfInteger) anIntegers = new TColStd_HSequenceOfInteger;
    Handle(TColStd_HSequenceOfReal)    aDoubles   = new TColStd_HSequenceOfReal;

    // Detect kind of shape and parameters
    aKind = GetOperations()->KindOfShape(aShape, anIntegers, aDoubles);

    int nbInts = anIntegers->Length();
    int nbDbls = aDoubles->Length();

    anIntegersArray->length(nbInts);
    aDoublesArray->length(nbDbls);

    for (int ii = 0; ii < nbInts; ii++) {
      anIntegersArray[ii] = anIntegers->Value(ii + 1);
    }
    for (int id = 0; id < nbDbls; id++) {
      aDoublesArray[id] = aDoubles->Value(id + 1);
    }
  }

  // initialize out-parameters with local arrays
  theIntegers = anIntegersArray._retn();
  theDoubles  = aDoublesArray._retn();
  return (GEOM::GEOM_IKindOfShape::shape_kind)aKind;
}

//=============================================================================
/*!
 *  GetPosition
 */
//=============================================================================
void GEOM_IMeasureOperations_i::GetPosition
                 (GEOM::GEOM_Object_ptr theShape,
                  CORBA::Double& Ox, CORBA::Double& Oy, CORBA::Double& Oz,
                  CORBA::Double& Zx, CORBA::Double& Zy, CORBA::Double& Zz,
                  CORBA::Double& Xx, CORBA::Double& Xy, CORBA::Double& Xz)
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  //Set default values: global CS
  Ox = Oy = Oz = Zx = Zy = Xy = Xz = 0.;
  Zz = Xx = 1.;

  //Get the reference shape
  Handle(GEOM_Object) aShape = GetObjectImpl(theShape);
  if (aShape.IsNull()) return;

  // Get shape parameters
  GetOperations()->GetPosition(aShape, Ox,Oy,Oz, Zx,Zy,Zz, Xx,Xy,Xz);
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

  //Get the reference shape
  Handle(GEOM_Object) aShape = GetObjectImpl(theShape);
  if (aShape.IsNull()) return aGEOMObject._retn();

  // Make Point - centre of mass of theShape
  Handle(GEOM_Object) anObject = GetOperations()->GetCentreOfMass(aShape);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  GetVertexByIndex
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IMeasureOperations_i::GetVertexByIndex
  (GEOM::GEOM_Object_ptr theShape, CORBA::Long theIndex)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference shape
  Handle(GEOM_Object) aShape = GetObjectImpl(theShape);
  if ( aShape.IsNull() ) return aGEOMObject._retn();

  // Get vertex by index
  Handle(GEOM_Object) anObject = GetOperations()->GetVertexByIndex(aShape, theIndex);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  GetNormal
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IMeasureOperations_i::GetNormal
                                       (GEOM::GEOM_Object_ptr theFace,
                                        GEOM::GEOM_Object_ptr theOptionalPoint)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference shape
  Handle(GEOM_Object) aFace = GetObjectImpl(theFace);
  if (aFace.IsNull()) return aGEOMObject._retn();

  // Get the OptionalPoint (can be not defined)
  Handle(GEOM_Object) anOptionalPoint = GetObjectImpl(theOptionalPoint);

  // Make Vector - normal to theFace (in point theOptionalPoint if the face is not planar)
  Handle(GEOM_Object) anObject = GetOperations()->GetNormal(aFace, anOptionalPoint);
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

  //Get the reference shape
  Handle(GEOM_Object) aShape = GetObjectImpl(theShape);
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

  //Get the reference shape
  Handle(GEOM_Object) aShape = GetObjectImpl(theShape);
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

  //Get the reference shape
  Handle(GEOM_Object) aShape = GetObjectImpl(theShape);
  if (aShape.IsNull()) return;

  // Get shape parameters
  GetOperations()->GetBoundingBox(aShape, Xmin, Xmax, Ymin, Ymax, Zmin, Zmax);
}

//=============================================================================
/*!
 *  MakeBoundingBox
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IMeasureOperations_i::MakeBoundingBox
                                              (GEOM::GEOM_Object_ptr theShape)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference shape
  Handle(GEOM_Object) aShape = GetObjectImpl(theShape);
  if (aShape.IsNull()) return aGEOMObject._retn();

  // Make Box - bounding box of theShape
  Handle(GEOM_Object) anObject = GetOperations()->GetBoundingBox(aShape);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
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

  //Get the reference shape
  Handle(GEOM_Object) aShape = GetObjectImpl(theShape);
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

  if (CORBA::is_nil(theShape))
  {
    theDescription = CORBA::string_dup("null");
    return 0;
  }

  //Get the reference shape
  Handle(GEOM_Object) aShape = GetObjectImpl(theShape);

  if (aShape.IsNull())
  {
    theDescription = CORBA::string_dup("null2");
    return 0;
  }

  // Get shape parameters
  TCollection_AsciiString aDump;
  if (GetOperations()->CheckShape(aShape, /*check_geom = */false, aDump))
  {
    theDescription = CORBA::string_dup("OK");
    return 1;
  }
  theDescription = CORBA::string_dup(aDump.ToCString());
  return 0;
}

CORBA::Boolean GEOM_IMeasureOperations_i::CheckShapeWithGeometry (GEOM::GEOM_Object_ptr theShape,
                                                                  CORBA::String_out     theDescription)
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  if (CORBA::is_nil(theShape))
  {
    theDescription = CORBA::string_dup("null");
    return 0;
  }

  //Get the reference shape
  Handle(GEOM_Object) aShape = GetObjectImpl(theShape);

  if (aShape.IsNull())
  {
    theDescription = CORBA::string_dup("null2");
    return 0;
  }

  // Get shape parameters
  TCollection_AsciiString aDump;
  if (GetOperations()->CheckShape(aShape, /*check_geom = */true, aDump))
  {
    theDescription = CORBA::string_dup("OK");
    return 1;
  }
  theDescription = CORBA::string_dup(aDump.ToCString());
  return 0;
}

//=============================================================================
/*!
 *  CheckSelfIntersections
 */
//=============================================================================
CORBA::Boolean GEOM_IMeasureOperations_i::CheckSelfIntersections (GEOM::GEOM_Object_ptr theShape,
                                                                  GEOM::ListOfLong_out  theIntersections)
{
  // Set a not done flag
  GetOperations()->SetNotDone();

  bool isGood = false;

  // Allocate the CORBA arrays
  GEOM::ListOfLong_var anIntegersArray = new GEOM::ListOfLong();

  // Get the reference shape
  Handle(GEOM_Object) aShape = GetObjectImpl(theShape);

  if (!aShape.IsNull()) {
    Handle(TColStd_HSequenceOfInteger) anIntegers = new TColStd_HSequenceOfInteger;

    // Detect self-intersections
    isGood = GetOperations()->CheckSelfIntersections(aShape, anIntegers);

    int nbInts = anIntegers->Length();

    anIntegersArray->length(nbInts);

    for (int ii = 0; ii < nbInts; ii++) {
      anIntegersArray[ii] = anIntegers->Value(ii + 1);
    }
  }

  // Initialize out-parameters with local arrays
  theIntersections = anIntegersArray._retn();
  return isGood;
}

//=============================================================================
/*!
 *  IsGoodForSolid
 */
//=============================================================================
char* GEOM_IMeasureOperations_i::IsGoodForSolid (GEOM::GEOM_Object_ptr theShape)
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference shape
  Handle(GEOM_Object) aShape = GetObjectImpl(theShape);
  if (aShape.IsNull()) return CORBA::string_dup("WRN_NULL_OBJECT_OR_SHAPE");

  // Get shape parameters
  TCollection_AsciiString aDescription = GetOperations()->IsGoodForSolid(aShape);
  return CORBA::string_dup(aDescription.ToCString());
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

  //Get the reference shape
  Handle(GEOM_Object) aShape = GetObjectImpl(theShape);
  if (aShape.IsNull()) return NULL;

  // Get shape parameters
  TCollection_AsciiString aDescription = GetOperations()->WhatIs(aShape);
  return CORBA::string_dup(aDescription.ToCString());
}

//=============================================================================
/*!
 *  AreCoordsInside
 */
//=============================================================================
GEOM::ListOfBool* GEOM_IMeasureOperations_i::AreCoordsInside (GEOM::GEOM_Object_ptr theShape,
                                                              const GEOM::ListOfDouble& theCoords,
                                                              CORBA::Double tolerance)
{
  //Set a not done flag
  GetOperations()->SetNotDone();
  
  unsigned int nb_points = theCoords.length()/3;

  GEOM::ListOfBool_var aResults = new GEOM::ListOfBool;
  aResults->length(nb_points);
  
  Handle(GEOM_Object) aShape = GetObjectImpl(theShape);
  
  std::vector<double> tmp(3*nb_points);
  for (int i = 0; i < 3*nb_points; i++)
    tmp[i] = theCoords[i];
  std::vector<bool> res = GetOperations()->AreCoordsInside(aShape, tmp, tolerance);
  for (int i = 0; i < nb_points; i++)
    aResults[i] = i < res.size() ? res[i] : false;
  return aResults._retn();
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

  //Get the reference shape
  Handle(GEOM_Object) aShape1 = GetObjectImpl(theShape1);
  Handle(GEOM_Object) aShape2 = GetObjectImpl(theShape2);
  if (aShape1.IsNull() || aShape2.IsNull()) return -1.0;

  // Get shape parameters
  return GetOperations()->GetMinDistance(aShape1, aShape2, X1, Y1, Z1, X2, Y2, Z2);
}

//=============================================================================
/*!
 *  ClosestPoints
 */
//=============================================================================
CORBA::Long GEOM_IMeasureOperations_i::ClosestPoints
  (GEOM::GEOM_Object_ptr theShape1, GEOM::GEOM_Object_ptr theShape2,
   GEOM::ListOfDouble_out theCoords)
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  // allocate the CORBA array
  int nbSols = 0;
  GEOM::ListOfDouble_var aDoublesArray = new GEOM::ListOfDouble();

  //Get the reference shape
  Handle(GEOM_Object) aShape1 = GetObjectImpl(theShape1);
  Handle(GEOM_Object) aShape2 = GetObjectImpl(theShape2);

  if (!aShape1.IsNull() && !aShape2.IsNull()) {
    Handle(TColStd_HSequenceOfReal) aDoubles = new TColStd_HSequenceOfReal;
    // Get shape parameters
    nbSols = GetOperations()->ClosestPoints(aShape1, aShape2, aDoubles);
    int nbDbls = aDoubles->Length();
    aDoublesArray->length(nbDbls);
    for (int id = 0; id < nbDbls; id++) {
      aDoublesArray[id] = aDoubles->Value(id + 1);
    }
  }

  theCoords = aDoublesArray._retn();
  return nbSols;
}

//=============================================================================
/*!
 *  PointCoordinates
 */
//=============================================================================
void GEOM_IMeasureOperations_i::PointCoordinates (GEOM::GEOM_Object_ptr theShape,
                                                  CORBA::Double& X, CORBA::Double& Y, CORBA::Double& Z)

{
  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference shape
  Handle(GEOM_Object) aShape = GetObjectImpl(theShape);
  if (aShape.IsNull())
    return;

  // Get shape parameters
  GetOperations()->PointCoordinates( aShape, X, Y, Z );
}

//=============================================================================
/*!
 *  GetAngle
 */
//=============================================================================
CORBA::Double GEOM_IMeasureOperations_i::GetAngle (GEOM::GEOM_Object_ptr theShape1,
                                                   GEOM::GEOM_Object_ptr theShape2)
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference shapes
  Handle(GEOM_Object) aShape1 = GetObjectImpl(theShape1);
  Handle(GEOM_Object) aShape2 = GetObjectImpl(theShape2);
  if (aShape1.IsNull() || aShape2.IsNull()) return -1.0;

  // Get the angle
  return GetOperations()->GetAngle(aShape1, aShape2);
}

//=============================================================================
/*!
 *  GetAngle
 */
//=============================================================================
CORBA::Double GEOM_IMeasureOperations_i::GetAngleBtwVectors (GEOM::GEOM_Object_ptr theShape1,
                                                             GEOM::GEOM_Object_ptr theShape2)
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference shapes
  Handle(GEOM_Object) aShape1 = GetObjectImpl(theShape1);
  Handle(GEOM_Object) aShape2 = GetObjectImpl(theShape2);
  if (aShape1.IsNull() || aShape2.IsNull()) return -1.0;

  // Get the angle
  return GetOperations()->GetAngleBtwVectors(aShape1, aShape2);
}


//=============================================================================
/*!
 *  CurveCurvatureByParam
 */
//=============================================================================
CORBA::Double GEOM_IMeasureOperations_i::CurveCurvatureByParam
                       (GEOM::GEOM_Object_ptr theCurve, CORBA::Double theParam)
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference shape
  Handle(GEOM_Object) aShape = GetObjectImpl(theCurve);
  if(aShape.IsNull()) return -1.0;

  return GetOperations()->CurveCurvatureByParam(aShape,theParam);
}

//=============================================================================
/*!
 *  CurveCurvatureByPoint
 */
//=============================================================================
CORBA::Double GEOM_IMeasureOperations_i::CurveCurvatureByPoint
               (GEOM::GEOM_Object_ptr theCurve, GEOM::GEOM_Object_ptr thePoint)
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference shape
  Handle(GEOM_Object) aShape = GetObjectImpl(theCurve);
  Handle(GEOM_Object) aPoint = GetObjectImpl(thePoint);
  if( aShape.IsNull() || aPoint.IsNull() ) return -1.0;

  return GetOperations()->CurveCurvatureByPoint(aShape,aPoint);
}


//=============================================================================
/*!
 *  MaxSurfaceCurvatureByParam
 */
//=============================================================================
CORBA::Double GEOM_IMeasureOperations_i::MaxSurfaceCurvatureByParam
                                                (GEOM::GEOM_Object_ptr theSurf,
                                                 CORBA::Double theUParam,
                                                 CORBA::Double theVParam)
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference shape
  Handle(GEOM_Object) aShape = GetObjectImpl(theSurf);
  if(aShape.IsNull()) return -1.0;

  return GetOperations()->MaxSurfaceCurvatureByParam(aShape,theUParam,theVParam);
}

//=============================================================================
/*!
 *  MaxSurfaceCurvatureByPoint
 */
//=============================================================================
CORBA::Double GEOM_IMeasureOperations_i::MaxSurfaceCurvatureByPoint
                (GEOM::GEOM_Object_ptr theSurf, GEOM::GEOM_Object_ptr thePoint)
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference shape
  Handle(GEOM_Object) aShape = GetObjectImpl(theSurf);
  Handle(GEOM_Object) aPoint = GetObjectImpl(thePoint);
  if( aShape.IsNull() || aPoint.IsNull() ) return -1.0;

  return GetOperations()->MaxSurfaceCurvatureByPoint(aShape,aPoint);
}

//=============================================================================
/*!
 *  MinSurfaceCurvatureByParam
 */
//=============================================================================
CORBA::Double GEOM_IMeasureOperations_i::MinSurfaceCurvatureByParam
                                                (GEOM::GEOM_Object_ptr theSurf,
                                                 CORBA::Double theUParam,
                                                 CORBA::Double theVParam)
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference shape
  Handle(GEOM_Object) aShape = GetObjectImpl(theSurf);
  if (aShape.IsNull()) return -1.0;

  return GetOperations()->MinSurfaceCurvatureByParam(aShape,theUParam,theVParam);
}

//=============================================================================
/*!
 *  MinSurfaceCurvatureByPoint
 */
//=============================================================================
CORBA::Double GEOM_IMeasureOperations_i::MinSurfaceCurvatureByPoint
                (GEOM::GEOM_Object_ptr theSurf, GEOM::GEOM_Object_ptr thePoint)
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference shape
  Handle(GEOM_Object) aShape = GetObjectImpl(theSurf);
  Handle(GEOM_Object) aPoint = GetObjectImpl(thePoint);
  if (aShape.IsNull() || aPoint.IsNull()) return -1.0;

  return GetOperations()->MinSurfaceCurvatureByPoint(aShape,aPoint);
}
