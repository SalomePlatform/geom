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
#include <TColStd_HArray1OfReal.hxx>

#include "GEOM_IMeasureOperations_i.hh"

#include "utilities.h"
#include "OpUtil.hxx"

#include "GEOM_Engine.hxx"
#include "GEOM_Object.hxx"

#define WRN_SHAPE_IS_NULL   "WRN_SHAPE_IS_NULL"

/**
 * This function converts shape errors from theErrorsFrom to theErrorsTo.
 * Note that theErrorsTo is not cleared at first.
 *
 * \param theErrorsFrom errors to be converted.
 * \param theErrorsTo result errors.
 */
static void ConvertShapeError
    (const GEOM::GEOM_IMeasureOperations::ShapeErrors         &theErrorsFrom,
           std::list<GEOMImpl_IMeasureOperations::ShapeError> &theErrorsTo)
{
  int aNbErr = theErrorsFrom.length();
  int i = 0;

  for (; i < aNbErr; i++) {
    const GEOM::GEOM_IMeasureOperations::ShapeError anErr = theErrorsFrom[i];
    const GEOM::GEOM_IMeasureOperations::ShapeErrorType aType = anErr.error;
    const GEOM::ListOfLong anIncrims = anErr.incriminated;
    GEOMImpl_IMeasureOperations::ShapeError anErrStruct;

    switch (aType) {
    case GEOM::GEOM_IMeasureOperations::InvalidPointOnCurve:
      anErrStruct.error = BRepCheck_InvalidPointOnCurve;
      break;
    case GEOM::GEOM_IMeasureOperations::InvalidPointOnCurveOnSurface:
      anErrStruct.error = BRepCheck_InvalidPointOnCurveOnSurface;
      break;
    case GEOM::GEOM_IMeasureOperations::InvalidPointOnSurface:
      anErrStruct.error = BRepCheck_InvalidPointOnSurface;
      break;
    case GEOM::GEOM_IMeasureOperations::No3DCurve:
      anErrStruct.error = BRepCheck_No3DCurve;
      break;
    case GEOM::GEOM_IMeasureOperations::Multiple3DCurve:
      anErrStruct.error = BRepCheck_Multiple3DCurve;
      break;
    case GEOM::GEOM_IMeasureOperations::Invalid3DCurve:
      anErrStruct.error = BRepCheck_Invalid3DCurve;
      break;
    case GEOM::GEOM_IMeasureOperations::NoCurveOnSurface:
      anErrStruct.error = BRepCheck_NoCurveOnSurface;
      break;
    case GEOM::GEOM_IMeasureOperations::InvalidCurveOnSurface:
      anErrStruct.error = BRepCheck_InvalidCurveOnSurface;
      break;
    case GEOM::GEOM_IMeasureOperations::InvalidCurveOnClosedSurface:
      anErrStruct.error = BRepCheck_InvalidCurveOnClosedSurface;
      break;
    case GEOM::GEOM_IMeasureOperations::InvalidSameRangeFlag:
      anErrStruct.error = BRepCheck_InvalidSameRangeFlag;
      break;
    case GEOM::GEOM_IMeasureOperations::InvalidSameParameterFlag:
      anErrStruct.error = BRepCheck_InvalidSameParameterFlag;
      break;
    case GEOM::GEOM_IMeasureOperations::InvalidDegeneratedFlag:
      anErrStruct.error = BRepCheck_InvalidDegeneratedFlag;
      break;
    case GEOM::GEOM_IMeasureOperations::FreeEdge:
      anErrStruct.error = BRepCheck_FreeEdge;
      break;
    case GEOM::GEOM_IMeasureOperations::InvalidMultiConnexity:
      anErrStruct.error = BRepCheck_InvalidMultiConnexity;
      break;
    case GEOM::GEOM_IMeasureOperations::InvalidRange:
      anErrStruct.error = BRepCheck_InvalidRange;
      break;
    case GEOM::GEOM_IMeasureOperations::EmptyWire:
      anErrStruct.error = BRepCheck_EmptyWire;
      break;
    case GEOM::GEOM_IMeasureOperations::RedundantEdge:
      anErrStruct.error = BRepCheck_RedundantEdge;
      break;
    case GEOM::GEOM_IMeasureOperations::SelfIntersectingWire:
      anErrStruct.error = BRepCheck_SelfIntersectingWire;
      break;
    case GEOM::GEOM_IMeasureOperations::NoSurface:
      anErrStruct.error = BRepCheck_NoSurface;
      break;
    case GEOM::GEOM_IMeasureOperations::InvalidWire:
      anErrStruct.error = BRepCheck_InvalidWire;
      break;
    case GEOM::GEOM_IMeasureOperations::RedundantWire:
      anErrStruct.error = BRepCheck_RedundantWire;
      break;
    case GEOM::GEOM_IMeasureOperations::IntersectingWires:
      anErrStruct.error = BRepCheck_IntersectingWires;
      break;
    case GEOM::GEOM_IMeasureOperations::InvalidImbricationOfWires:
      anErrStruct.error = BRepCheck_InvalidImbricationOfWires;
      break;
    case GEOM::GEOM_IMeasureOperations::EmptyShell:
      anErrStruct.error = BRepCheck_EmptyShell;
      break;
    case GEOM::GEOM_IMeasureOperations::RedundantFace:
      anErrStruct.error = BRepCheck_RedundantFace;
      break;
    case GEOM::GEOM_IMeasureOperations::UnorientableShape:
      anErrStruct.error = BRepCheck_UnorientableShape;
      break;
    case GEOM::GEOM_IMeasureOperations::NotClosed:
      anErrStruct.error = BRepCheck_NotClosed;
      break;
    case GEOM::GEOM_IMeasureOperations::NotConnected:
      anErrStruct.error = BRepCheck_NotConnected;
      break;
    case GEOM::GEOM_IMeasureOperations::SubshapeNotInShape:
      anErrStruct.error = BRepCheck_SubshapeNotInShape;
      break;
    case GEOM::GEOM_IMeasureOperations::BadOrientation:
      anErrStruct.error = BRepCheck_BadOrientation;
      break;
    case GEOM::GEOM_IMeasureOperations::BadOrientationOfSubshape:
      anErrStruct.error = BRepCheck_BadOrientationOfSubshape;
      break;
    case GEOM::GEOM_IMeasureOperations::InvalidToleranceValue:
      anErrStruct.error = BRepCheck_InvalidToleranceValue;
      break;
    case GEOM::GEOM_IMeasureOperations::CheckFail:
      anErrStruct.error = BRepCheck_CheckFail;
      break;
    default:
      break;
    }

    int ii = 0;
    int aLen = anIncrims.length();

    for (; ii < aLen; ii++) {
      anErrStruct.incriminated.push_back(anIncrims[ii]);
    }

    theErrorsTo.push_back(anErrStruct);
  }
}

/**
 * This function converts shape errors from theErrorsFrom to theErrorsTo.
 * Note that theErrorsTo is not cleared at first.
 *
 * \param theErrorsFrom errors to be converted.
 * \param theErrorsTo result errors.
 */
static void ConvertShapeError
    (const std::list<GEOMImpl_IMeasureOperations::ShapeError> &theErrorsFrom,
           GEOM::GEOM_IMeasureOperations::ShapeErrors_out     &theErrorsTo)
{
  const int aNbErr = theErrorsFrom.size();
  GEOM::GEOM_IMeasureOperations::ShapeErrors_var anErrArray =
    new GEOM::GEOM_IMeasureOperations::ShapeErrors;

  anErrArray->length(aNbErr);

  // fill the local CORBA array with values from lists
  std::list<GEOMImpl_IMeasureOperations::ShapeError>::const_iterator
    anIt = theErrorsFrom.begin();
  int i = 0;

  for (; anIt != theErrorsFrom.end(); i++, anIt++) {
    GEOM::GEOM_IMeasureOperations::ShapeError_var anErrStruct =
      new GEOM::GEOM_IMeasureOperations::ShapeError;

    switch (anIt->error) {
    case BRepCheck_InvalidPointOnCurve:
      anErrStruct->error = GEOM::GEOM_IMeasureOperations::InvalidPointOnCurve;
      break;
    case BRepCheck_InvalidPointOnCurveOnSurface:
      anErrStruct->error = GEOM::GEOM_IMeasureOperations::InvalidPointOnCurveOnSurface;
      break;
    case BRepCheck_InvalidPointOnSurface:
      anErrStruct->error = GEOM::GEOM_IMeasureOperations::InvalidPointOnSurface;
      break;
    case BRepCheck_No3DCurve:
      anErrStruct->error = GEOM::GEOM_IMeasureOperations::No3DCurve;
      break;
    case BRepCheck_Multiple3DCurve:
      anErrStruct->error = GEOM::GEOM_IMeasureOperations::Multiple3DCurve;
      break;
    case BRepCheck_Invalid3DCurve:
      anErrStruct->error = GEOM::GEOM_IMeasureOperations::Invalid3DCurve;
      break;
    case BRepCheck_NoCurveOnSurface:
      anErrStruct->error = GEOM::GEOM_IMeasureOperations::NoCurveOnSurface;
      break;
    case BRepCheck_InvalidCurveOnSurface:
      anErrStruct->error = GEOM::GEOM_IMeasureOperations::InvalidCurveOnSurface;
      break;
    case BRepCheck_InvalidCurveOnClosedSurface:
      anErrStruct->error = GEOM::GEOM_IMeasureOperations::InvalidCurveOnClosedSurface;
      break;
    case BRepCheck_InvalidSameRangeFlag:
      anErrStruct->error = GEOM::GEOM_IMeasureOperations::InvalidSameRangeFlag;
      break;
    case BRepCheck_InvalidSameParameterFlag:
      anErrStruct->error = GEOM::GEOM_IMeasureOperations::InvalidSameParameterFlag;
      break;
    case BRepCheck_InvalidDegeneratedFlag:
      anErrStruct->error = GEOM::GEOM_IMeasureOperations::InvalidDegeneratedFlag;
      break;
    case BRepCheck_FreeEdge:
      anErrStruct->error = GEOM::GEOM_IMeasureOperations::FreeEdge;
      break;
    case BRepCheck_InvalidMultiConnexity:
      anErrStruct->error = GEOM::GEOM_IMeasureOperations::InvalidMultiConnexity;
      break;
    case BRepCheck_InvalidRange:
      anErrStruct->error = GEOM::GEOM_IMeasureOperations::InvalidRange;
      break;
    case BRepCheck_EmptyWire:
      anErrStruct->error = GEOM::GEOM_IMeasureOperations::EmptyWire;
      break;
    case BRepCheck_RedundantEdge:
      anErrStruct->error = GEOM::GEOM_IMeasureOperations::RedundantEdge;
      break;
    case BRepCheck_SelfIntersectingWire:
      anErrStruct->error = GEOM::GEOM_IMeasureOperations::SelfIntersectingWire;
      break;
    case BRepCheck_NoSurface:
      anErrStruct->error = GEOM::GEOM_IMeasureOperations::NoSurface;
      break;
    case BRepCheck_InvalidWire:
      anErrStruct->error = GEOM::GEOM_IMeasureOperations::InvalidWire;
      break;
    case BRepCheck_RedundantWire:
      anErrStruct->error = GEOM::GEOM_IMeasureOperations::RedundantWire;
      break;
    case BRepCheck_IntersectingWires:
      anErrStruct->error = GEOM::GEOM_IMeasureOperations::IntersectingWires;
      break;
    case BRepCheck_InvalidImbricationOfWires:
      anErrStruct->error = GEOM::GEOM_IMeasureOperations::InvalidImbricationOfWires;
      break;
    case BRepCheck_EmptyShell:
      anErrStruct->error = GEOM::GEOM_IMeasureOperations::EmptyShell;
      break;
    case BRepCheck_RedundantFace:
      anErrStruct->error = GEOM::GEOM_IMeasureOperations::RedundantFace;
      break;
    case BRepCheck_UnorientableShape:
      anErrStruct->error = GEOM::GEOM_IMeasureOperations::UnorientableShape;
      break;
    case BRepCheck_NotClosed:
      anErrStruct->error = GEOM::GEOM_IMeasureOperations::NotClosed;
      break;
    case BRepCheck_NotConnected:
      anErrStruct->error = GEOM::GEOM_IMeasureOperations::NotConnected;
      break;
    case BRepCheck_SubshapeNotInShape:
      anErrStruct->error = GEOM::GEOM_IMeasureOperations::SubshapeNotInShape;
      break;
    case BRepCheck_BadOrientation:
      anErrStruct->error = GEOM::GEOM_IMeasureOperations::BadOrientation;
      break;
    case BRepCheck_BadOrientationOfSubshape:
      anErrStruct->error = GEOM::GEOM_IMeasureOperations::BadOrientationOfSubshape;
      break;
    case BRepCheck_InvalidToleranceValue:
      anErrStruct->error = GEOM::GEOM_IMeasureOperations::InvalidToleranceValue;
      break;
    case BRepCheck_CheckFail:
      anErrStruct->error = GEOM::GEOM_IMeasureOperations::CheckFail;
      break;
    default:
      break;
    }

    std::list<int> aIndList = anIt->incriminated;
    GEOM::ListOfLong_var anIncrims = new GEOM::ListOfLong();
    anIncrims->length(aIndList.size());

    std::list<int>::iterator anIndIt = aIndList.begin();
    int jj = 0;

    for (; anIndIt != aIndList.end(); jj++, anIndIt++) {
      anIncrims[jj] = *anIndIt;
    }

    anErrStruct->incriminated = anIncrims;

    anErrArray[i] = anErrStruct;
  }

  // initialize out-parameter with local array
  theErrorsTo = anErrArray._retn();
}

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
  Handle(::GEOM_Object) aShape = GetObjectImpl(theShape);

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
  Handle(::GEOM_Object) aShape = GetObjectImpl(theShape);
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
  Handle(::GEOM_Object) aShape = GetObjectImpl(theShape);
  if (aShape.IsNull()) return aGEOMObject._retn();

  // Make Point - centre of mass of theShape
  Handle(::GEOM_Object) anObject = GetOperations()->GetCentreOfMass(aShape);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  GetVertexByIndex
 */
//=============================================================================
GEOM::GEOM_Object_ptr
GEOM_IMeasureOperations_i::GetVertexByIndex( GEOM::GEOM_Object_ptr theShape,
                                             CORBA::Long           theIndex,
                                             CORBA::Boolean        theUseOri )
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference shape
  Handle(::GEOM_Object) aShape = GetObjectImpl(theShape);
  if ( aShape.IsNull() ) return aGEOMObject._retn();

  // Get vertex by index
  Handle(::GEOM_Object) anObject = GetOperations()->GetVertexByIndex(aShape, theIndex, theUseOri);
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
  Handle(::GEOM_Object) aFace = GetObjectImpl(theFace);
  if (aFace.IsNull()) return aGEOMObject._retn();

  // Get the OptionalPoint (can be not defined)
  Handle(::GEOM_Object) anOptionalPoint = GetObjectImpl(theOptionalPoint);

  // Make Vector - normal to theFace (in point theOptionalPoint if the face is not planar)
  Handle(::GEOM_Object) anObject = GetOperations()->GetNormal(aFace, anOptionalPoint);
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
                                                    CORBA::Double  theTolerance,
                                                    CORBA::Double& theLength,
                                                    CORBA::Double& theSurfArea,
                                                    CORBA::Double& theVolume)
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference shape
  Handle(::GEOM_Object) aShape = GetObjectImpl(theShape);
  if (aShape.IsNull()) return;

  // Get shape parameters
  GetOperations()->GetBasicProperties(aShape, theTolerance, theLength,
                                      theSurfArea, theVolume);
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
  Handle(::GEOM_Object) aShape = GetObjectImpl(theShape);
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
                                                CORBA::Boolean precise,
                                                CORBA::Double& Xmin, CORBA::Double& Xmax,
                                                CORBA::Double& Ymin, CORBA::Double& Ymax,
                                                CORBA::Double& Zmin, CORBA::Double& Zmax)
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference shape
  Handle(::GEOM_Object) aShape = GetObjectImpl(theShape);
  if (aShape.IsNull()) return;

  // Get shape parameters
  GetOperations()->GetBoundingBox
    (aShape, precise, Xmin, Xmax, Ymin, Ymax, Zmin, Zmax);
}

//=============================================================================
/*!
 *  MakeBoundingBox
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IMeasureOperations_i::MakeBoundingBox
                                              (GEOM::GEOM_Object_ptr theShape,
                                               CORBA::Boolean precise)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference shape
  Handle(::GEOM_Object) aShape = GetObjectImpl(theShape);
  if (aShape.IsNull()) return aGEOMObject._retn();

  // Make Box - bounding box of theShape
  Handle(::GEOM_Object) anObject =
    GetOperations()->GetBoundingBox(aShape, precise);
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
  Handle(::GEOM_Object) aShape = GetObjectImpl(theShape);
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
CORBA::Boolean GEOM_IMeasureOperations_i::CheckShape
             (GEOM::GEOM_Object_ptr                          theShape,
              GEOM::GEOM_IMeasureOperations::ShapeErrors_out theErrors)
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  if (CORBA::is_nil(theShape))
  {
    return 0;
  }

  //Get the reference shape
  Handle(::GEOM_Object) aShape = GetObjectImpl(theShape);

  if (aShape.IsNull())
  {
    return 0;
  }

  std::list<GEOMImpl_IMeasureOperations::ShapeError> anErrList;
  bool isOk = GetOperations()->CheckShape(aShape, false, anErrList);

  ConvertShapeError(anErrList, theErrors);

  return isOk;
}

CORBA::Boolean GEOM_IMeasureOperations_i::CheckShapeWithGeometry
             (GEOM::GEOM_Object_ptr                          theShape,
              GEOM::GEOM_IMeasureOperations::ShapeErrors_out theErrors)
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  if (CORBA::is_nil(theShape))
  {
    return 0;
  }

  //Get the reference shape
  Handle(::GEOM_Object) aShape = GetObjectImpl(theShape);

  if (aShape.IsNull())
  {
    return 0;
  }

  std::list<GEOMImpl_IMeasureOperations::ShapeError> anErrList;
  bool isOk = GetOperations()->CheckShape(aShape, true, anErrList);

  ConvertShapeError(anErrList, theErrors);

  return isOk;
}

//=============================================================================
/*!
 *  PrintShapeErrors
 */
//=============================================================================
char* GEOM_IMeasureOperations_i::PrintShapeErrors
             (      GEOM::GEOM_Object_ptr                       theShape,
              const GEOM::GEOM_IMeasureOperations::ShapeErrors &theErrors)
{
  //Get the reference shape
  Handle(::GEOM_Object) aShape = GetObjectImpl(theShape);

  if (aShape.IsNull())
  {
    GetOperations()->SetErrorCode(WRN_SHAPE_IS_NULL);
    return CORBA::string_dup("");
  }

  // Convert the errors sequence
  std::list<GEOMImpl_IMeasureOperations::ShapeError> anErrList;

  ConvertShapeError(theErrors, anErrList);

  TCollection_AsciiString aDescr =
    GetOperations()->PrintShapeErrors(aShape, anErrList);

  return CORBA::string_dup(aDescr.ToCString());
}

//=============================================================================
/*!
 *  CheckSelfIntersections
 */
//=============================================================================
CORBA::Boolean GEOM_IMeasureOperations_i::CheckSelfIntersections (GEOM::GEOM_Object_ptr theShape,
                                                                  CORBA::Long           theCheckLevel,
                                                                  GEOM::ListOfLong_out  theIntersections)
{
  // Set a not done flag
  GetOperations()->SetNotDone();

  bool isGood = false;

  // Allocate the CORBA arrays
  GEOM::ListOfLong_var anIntegersArray = new GEOM::ListOfLong();

  // Get the reference shape
  Handle(::GEOM_Object) aShape = GetObjectImpl(theShape);

  if (!aShape.IsNull()) {
    GEOMImpl_IMeasureOperations::SICheckLevel aCheckLevel;

    switch(theCheckLevel) {
    case GEOM::SI_V_V:
      aCheckLevel = GEOMImpl_IMeasureOperations::SI_V_V;
      break;
    case GEOM::SI_V_E:
      aCheckLevel = GEOMImpl_IMeasureOperations::SI_V_E;
      break;
    case GEOM::SI_E_E:
      aCheckLevel = GEOMImpl_IMeasureOperations::SI_E_E;
      break;
    case GEOM::SI_V_F:
      aCheckLevel = GEOMImpl_IMeasureOperations::SI_V_F;
      break;
    case GEOM::SI_E_F:
      aCheckLevel = GEOMImpl_IMeasureOperations::SI_E_F;
      break;
    case GEOM::SI_ALL:
    default:
      aCheckLevel = GEOMImpl_IMeasureOperations::SI_ALL;
      break;
    }

    Handle(TColStd_HSequenceOfInteger) anIntegers = new TColStd_HSequenceOfInteger;

    // Detect self-intersections
    isGood = GetOperations()->CheckSelfIntersections
      (aShape, aCheckLevel, anIntegers);

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
 *  CheckSelfIntersectionsFast
 */
//=============================================================================
CORBA::Boolean GEOM_IMeasureOperations_i::CheckSelfIntersectionsFast 
  (GEOM::GEOM_Object_ptr theShape,
   CORBA::Float          theDeflection,
   CORBA::Double         theTolerance,
   GEOM::ListOfLong_out  theIntersections)
{
  // Set a not done flag
  GetOperations()->SetNotDone();

  bool isGood = false;

  // Allocate the CORBA arrays
  GEOM::ListOfLong_var anIntegersArray = new GEOM::ListOfLong();

  // Get the reference shape
  Handle(::GEOM_Object) aShape = GetObjectImpl(theShape);

  if (!aShape.IsNull()) {
    Handle(TColStd_HSequenceOfInteger) anIntegers = new TColStd_HSequenceOfInteger;

    // Detect self-intersections
    isGood = GetOperations()->CheckSelfIntersectionsFast
      (aShape, theDeflection, theTolerance, anIntegers);

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
 *  CheckBOPArguments
 */
//=============================================================================
CORBA::Boolean GEOM_IMeasureOperations_i::CheckBOPArguments
                                      (GEOM::GEOM_Object_ptr theShape)
{
  // Set a not done flag
  GetOperations()->SetNotDone();

  // Get the reference shape
  Handle(::GEOM_Object) aShape = GetObjectImpl(theShape);
  bool isGood = false;

  if (!aShape.IsNull()) {
    // Check BOP arguments
    isGood = GetOperations()->CheckBOPArguments(aShape);
  }

  return isGood;
}

//=============================================================================
/*!
 *  FastIntersect
 */
//=============================================================================
CORBA::Boolean GEOM_IMeasureOperations_i::FastIntersect (GEOM::GEOM_Object_ptr theShape1,
                                                         GEOM::GEOM_Object_ptr theShape2,
                                                         CORBA::Double         theTolerance,
                                                         CORBA::Float          theDeflection,
                                                         GEOM::ListOfLong_out  theIntersections1,
                                                         GEOM::ListOfLong_out  theIntersections2)
{
  // Set a not done flag
  GetOperations()->SetNotDone();

  bool isGood = false;

  // Allocate the CORBA arrays
  GEOM::ListOfLong_var anIntegersArray1 = new GEOM::ListOfLong();
  GEOM::ListOfLong_var anIntegersArray2 = new GEOM::ListOfLong();

  // Get the reference shape
  Handle(::GEOM_Object) aShape1 = GetObjectImpl(theShape1);
  Handle(::GEOM_Object) aShape2 = GetObjectImpl(theShape2);
  
  if (!aShape1.IsNull() && !aShape2.IsNull()) {
    Handle(TColStd_HSequenceOfInteger) anIntegers1 = new TColStd_HSequenceOfInteger;
    Handle(TColStd_HSequenceOfInteger) anIntegers2 = new TColStd_HSequenceOfInteger;

    // Detect intersections
    isGood = GetOperations()->FastIntersect
      (aShape1, aShape2, theTolerance, theDeflection, anIntegers1, anIntegers2);

    int nbInts1 = anIntegers1->Length();
    int nbInts2 = anIntegers2->Length();

    anIntegersArray1->length(nbInts1);
    anIntegersArray2->length(nbInts2);

    for (int ii = 0; ii < nbInts1; ii++) {
      anIntegersArray1[ii] = anIntegers1->Value(ii + 1);
    }
    for (int ii = 0; ii < nbInts2; ii++) {
      anIntegersArray2[ii] = anIntegers2->Value(ii + 1);
    }
  }

  // Initialize out-parameters with local arrays
  theIntersections1 = anIntegersArray1._retn();
  theIntersections2 = anIntegersArray2._retn();
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
  Handle(::GEOM_Object) aShape = GetObjectImpl(theShape);
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
  Handle(::GEOM_Object) aShape = GetObjectImpl(theShape);
  if (aShape.IsNull())
  {
    //Assume the "WhatIs" operation as being done
    GetOperations()->SetErrorCode(OK);
    return CORBA::string_dup(WRN_SHAPE_IS_NULL);
  }

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
  
  int nb_points = theCoords.length()/3;

  GEOM::ListOfBool_var aResults = new GEOM::ListOfBool;
  aResults->length(nb_points);
  
  Handle(::GEOM_Object) aShape = GetObjectImpl(theShape);
  
  std::vector<double> tmp(3*nb_points);
  for (int i = 0; i < 3*nb_points; i++)
    tmp[i] = theCoords[i];
  std::vector<bool> res = GetOperations()->AreCoordsInside(aShape, tmp, tolerance);
  for (int i = 0, resSize = res.size();  i < nb_points; i++)
    aResults[i] = i < resSize ? res[i] : false;
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
  Handle(::GEOM_Object) aShape1 = GetObjectImpl(theShape1);
  Handle(::GEOM_Object) aShape2 = GetObjectImpl(theShape2);
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
  Handle(::GEOM_Object) aShape1 = GetObjectImpl(theShape1);
  Handle(::GEOM_Object) aShape2 = GetObjectImpl(theShape2);

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
  Handle(::GEOM_Object) aShape = GetObjectImpl(theShape);
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
  Handle(::GEOM_Object) aShape1 = GetObjectImpl(theShape1);
  Handle(::GEOM_Object) aShape2 = GetObjectImpl(theShape2);
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
  Handle(::GEOM_Object) aShape1 = GetObjectImpl(theShape1);
  Handle(::GEOM_Object) aShape2 = GetObjectImpl(theShape2);
  if (aShape1.IsNull() || aShape2.IsNull()) return -1.0;

  // Get the angle
  return GetOperations()->GetAngleBtwVectors(aShape1, aShape2);
}

//=============================================================================
/*!
*  PatchFace
*/
//=============================================================================
GEOM::ListOfGO* GEOM_IMeasureOperations_i::PatchFace(GEOM::GEOM_Object_ptr theShape)
{
  GEOM::ListOfGO_var aSeq = new GEOM::ListOfGO;

  //Get the reference shape
  Handle(::GEOM_Object) aShapeRef = GetObjectImpl(theShape);
  if (aShapeRef.IsNull()) return aSeq._retn();

  // Perform patch face operation
  Handle(TColStd_HSequenceOfTransient) aHSeq =
    GetOperations()->PatchFace(aShapeRef);
  if (!GetOperations()->IsDone() || aHSeq.IsNull())
    return aSeq._retn();

  Standard_Integer aLength = aHSeq->Length();
  aSeq->length(aLength);
  for (Standard_Integer i = 1; i <= aLength; i++)
    aSeq[i - 1] = GetObject(Handle(::GEOM_Object)::DownCast(aHSeq->Value(i)));

  return aSeq._retn();
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
  Handle(::GEOM_Object) aShape = GetObjectImpl(theCurve);
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
  Handle(::GEOM_Object) aShape = GetObjectImpl(theCurve);
  Handle(::GEOM_Object) aPoint = GetObjectImpl(thePoint);
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
  Handle(::GEOM_Object) aShape = GetObjectImpl(theSurf);
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
  Handle(::GEOM_Object) aShape = GetObjectImpl(theSurf);
  Handle(::GEOM_Object) aPoint = GetObjectImpl(thePoint);
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
  Handle(::GEOM_Object) aShape = GetObjectImpl(theSurf);
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
  Handle(::GEOM_Object) aShape = GetObjectImpl(theSurf);
  Handle(::GEOM_Object) aPoint = GetObjectImpl(thePoint);
  if (aShape.IsNull() || aPoint.IsNull()) return -1.0;

  return GetOperations()->MinSurfaceCurvatureByPoint(aShape,aPoint);
}

//=============================================================================
/*!
 *  SurfaceCurvatureByPointAndDirection
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IMeasureOperations_i::SurfaceCurvatureByPointAndDirection
                                          (GEOM::GEOM_Object_ptr theSurf,
                                           GEOM::GEOM_Object_ptr thePoint,
                                           GEOM::GEOM_Object_ptr theDirection)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference shape
  Handle(::GEOM_Object) aShape = GetObjectImpl(theSurf);
  Handle(::GEOM_Object) aPoint = GetObjectImpl(thePoint);
  Handle(::GEOM_Object) aDirection = GetObjectImpl(theDirection);
  if (aShape.IsNull() || aPoint.IsNull() || aDirection.IsNull()) return aGEOMObject._retn();

  Handle(::GEOM_Object) anObject =
    GetOperations()->SurfaceCurvatureByPointAndDirection(aShape,aPoint,aDirection);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  XYZtoUV
 */
//=============================================================================
GEOM::ListOfDouble* GEOM_IMeasureOperations_i::XYZtoUV
                                        (GEOM::GEOM_Object_ptr     theSurf,
                                         const GEOM::ListOfDouble& theXYZlist,
                                         CORBA::Boolean            theIsNormalized)
{
  GEOM::ListOfDouble_var resUV = new GEOM::ListOfDouble;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference shape
  Handle(::GEOM_Object) aShape = GetObjectImpl(theSurf);
  if (aShape.IsNull()) return resUV._retn();

  //Get input XYZ list
  Handle(TColStd_HArray1OfReal) aXYZlist =
    new TColStd_HArray1OfReal (0, theXYZlist.length() - 1);
  {
    size_t nb = theXYZlist.length(); 
    for (size_t i = 0; i < nb; ++i)
      aXYZlist->SetValue(i, theXYZlist[i]);
  }

  //Call implementation
  Handle(TColStd_HArray1OfReal) aUVlist =
    GetOperations()->XYZtoUV(aShape, aXYZlist, theIsNormalized);

  if (GetOperations()->IsDone()) {
    resUV->length(aUVlist->Length());
    int i0 = aUVlist->Lower();
    int nb = aUVlist->Upper(); 
    for (int i = i0; i <= nb; ++i)
      resUV[ i-i0 ] = aUVlist->Value(i);
  }

  return resUV._retn();
}

//=============================================================================
/*!
 *  UVtoXYZ
 */
//=============================================================================
GEOM::ListOfDouble* GEOM_IMeasureOperations_i::UVtoXYZ
                                        (GEOM::GEOM_Object_ptr     theSurf,
                                         const GEOM::ListOfDouble& theUVlist,
                                         CORBA::Boolean            theIsNormalized)
{
  GEOM::ListOfDouble_var resXYZ = new GEOM::ListOfDouble;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference shape
  Handle(::GEOM_Object) aShape = GetObjectImpl(theSurf);
  if (aShape.IsNull()) return resXYZ._retn();

  //Get input UV list
  Handle(TColStd_HArray1OfReal) aUVlist =
    new TColStd_HArray1OfReal (0, theUVlist.length() - 1);
  {
    size_t nb = theUVlist.length(); 
    for (size_t i = 0; i < nb; ++i)
      aUVlist->SetValue(i, theUVlist[i]);
  }

  //Call implementation
  Handle(TColStd_HArray1OfReal) aXYZlist =
    GetOperations()->UVtoXYZ(aShape, aUVlist, theIsNormalized);

  if (GetOperations()->IsDone()) {
    resXYZ->length(aXYZlist->Length());
    int i0 = aXYZlist->Lower();
    int nb = aXYZlist->Upper(); 
    for (int i = i0; i <= nb; ++i)
      resXYZ[ i-i0 ] = aXYZlist->Value(i);
  }

  return resXYZ._retn();
}

//=============================================================================
/*!
 *  SelfIntersected2D
 *  Find all self-intersected 2D curves.
 *  \param theShape Shape for check.
 */
 //=============================================================================
GEOM::GEOM_IMeasureOperations::SequenceOfPairOfShape* GEOM_IMeasureOperations_i::SelfIntersected2D(
                                          const GEOM::GEOM_IMeasureOperations::CheckResults& theResuts)
{
  GEOM::GEOM_IMeasureOperations::SequenceOfPairOfShape_var aSeq =
    new GEOM::GEOM_IMeasureOperations::SequenceOfPairOfShape;

  // Perform patch face operation
  std::list<GEOMImpl_IMeasureOperations::FailedChecks> aResults;
  ConvertToList(theResuts, aResults);

  std::list<GEOMImpl_IMeasureOperations::CoupleOfObjects> aSelfInters = GetOperations()->SelfIntersected2D(aResults);
  if (!GetOperations()->IsDone() || aSelfInters.empty())
    return aSeq._retn();

  Standard_Integer aLength = aSelfInters.size();
  aSeq->length(aLength);
  std::list<GEOMImpl_IMeasureOperations::CoupleOfObjects>::iterator anIter(aSelfInters.begin());
  for (Standard_Integer i = 0; i < aLength; i++, ++anIter)
  {
    aSeq[i].first = GetObject(Handle(::GEOM_Object)::DownCast((*anIter).first));
    aSeq[i].second = GetObject(Handle(::GEOM_Object)::DownCast((*anIter).second));
  }

  return aSeq._retn();
}

//=============================================================================
/*!
 *  InterferingSubshapes
 *  Find pairs of interfering sub-shapes, by default all pairs of interfering shapes are returned.
 *  \param theShape Shape for check.
 *  \param theShapeType1 Type of shape.
 *  \param theShapeType2 Type of shape.
 */
 //=============================================================================
GEOM::GEOM_IMeasureOperations::SequenceOfPairOfShape* GEOM_IMeasureOperations_i::InterferingSubshapes(
    const GEOM::GEOM_IMeasureOperations::CheckResults& theResuts,
    const CORBA::Long theShapeType1,
    const CORBA::Long theShapeType2)
{
  GEOM::GEOM_IMeasureOperations::SequenceOfPairOfShape_var aSeq =
    new GEOM::GEOM_IMeasureOperations::SequenceOfPairOfShape;

  // Perform patch face operation
  std::list<GEOMImpl_IMeasureOperations::FailedChecks> aResults;
  ConvertToList(theResuts, aResults);

  std::list<GEOMImpl_IMeasureOperations::CoupleOfObjects> aSelfInterf =
    GetOperations()->InterferingSubshapes(aResults, theShapeType1, theShapeType2);
  if (!GetOperations()->IsDone() || aSelfInterf.empty())
    return aSeq._retn();

  Standard_Integer aLength = aSelfInterf.size();
  aSeq->length(aLength);
  std::list<GEOMImpl_IMeasureOperations::CoupleOfObjects>::iterator anIter(aSelfInterf.begin());
  for (Standard_Integer i = 0; i < aLength; i++, ++anIter)
  {
    aSeq[i].first = GetObject(Handle(::GEOM_Object)::DownCast((*anIter).first));
    aSeq[i].second = GetObject(Handle(::GEOM_Object)::DownCast((*anIter).second));
  }

  return aSeq._retn();
}

//=============================================================================
/*!
 *  SmallEdges
 *  Find edges, which are fully covered by tolerances of vertices.
 *  \param theShape Shape for check.
 */
 //=============================================================================
GEOM::ListOfGO* GEOM_IMeasureOperations_i::SmallEdges(const GEOM::GEOM_IMeasureOperations::CheckResults& theResuts)
{
  GEOM::ListOfGO_var aSeq = new GEOM::ListOfGO;

  std::list<GEOMImpl_IMeasureOperations::FailedChecks> aResults;
  ConvertToList(theResuts, aResults);

  //Get the reference shape
  Handle(TColStd_HSequenceOfTransient) aSmallEdges = GetOperations()->SmallEdges(aResults);
  if (!GetOperations()->IsDone() || aSmallEdges.IsNull())
    return aSeq._retn();

  Standard_Integer aLength = aSmallEdges->Length();
  aSeq->length(aLength);
  for (Standard_Integer i = 1; i <= aLength; i++)
    aSeq[i - 1] = GetObject(Handle(::GEOM_Object)::DownCast(aSmallEdges->Value(i)));

  return aSeq._retn();
}

//=============================================================================
/*!
 *  DistantShapes
 *  find remote objects (sub-shape on a shape).
 *  \param theShape Shape for check.
 *  \param theShapeType Type of shape.
 *  \param theSubShapeType Type of sub-shape.
 *  \param theTolerance tolerance.
 */
 //=============================================================================
GEOM::GEOM_IMeasureOperations::SequenceOfPairOfShape* GEOM_IMeasureOperations_i::DistantShapes(
    const GEOM::GEOM_IMeasureOperations::CheckResults& theResuts,
    const CORBA::Long theShapeType,
    const CORBA::Long theSubShapeType,
    const CORBA::Double theTolerance)
{
  GEOM::GEOM_IMeasureOperations::SequenceOfPairOfShape_var aSeq =
    new GEOM::GEOM_IMeasureOperations::SequenceOfPairOfShape;

  std::list<GEOMImpl_IMeasureOperations::FailedChecks> aResults;
  ConvertToList(theResuts, aResults);

  // Perform patch face operation
  std::list<GEOMImpl_IMeasureOperations::CoupleOfObjects> aDistantS =
      GetOperations()->DistantShapes(aResults, theShapeType, theSubShapeType, theTolerance);
  if (!GetOperations()->IsDone() || aDistantS.empty())
    return aSeq._retn();

  Standard_Integer aLength = aDistantS.size();
  aSeq->length(aLength);
  std::list<GEOMImpl_IMeasureOperations::CoupleOfObjects>::iterator anIter(aDistantS.begin());
  for (Standard_Integer i = 0; i < aLength; i++, ++anIter)
  {
    aSeq[i].first = GetObject(Handle(::GEOM_Object)::DownCast((*anIter).first));
    aSeq[i].second = GetObject(Handle(::GEOM_Object)::DownCast((*anIter).second));
  }

  return aSeq._retn();
}

//=============================================================================
/*!
 *  CheckConformityShape
 *  Perform all required checks
 */
 //=============================================================================
GEOM::GEOM_IMeasureOperations::CheckResults* GEOM_IMeasureOperations_i::CheckConformityShape(
    GEOM::GEOM_Object_ptr theShape)
{
  GEOM::GEOM_IMeasureOperations::CheckResults_var aRes = new GEOM::GEOM_IMeasureOperations::CheckResults;

  //Get the reference shape
  Handle(::GEOM_Object) aShapeRef = GetObjectImpl(theShape);
  if (aShapeRef.IsNull()) return nullptr;

  std::list<GEOMImpl_IMeasureOperations::FailedChecks> aChecks;
  GetOperations()->CheckConformityShape(aShapeRef, aChecks);

  Standard_Integer aLength = aChecks.size();
  aRes->length(aLength);
  std::list<GEOMImpl_IMeasureOperations::FailedChecks>::const_iterator anIntIt = aChecks.cbegin();
  for (Standard_Integer i = 0; i < aLength; i++, ++anIntIt)
  {
    aRes[i].type = (*anIntIt).TypeOfCheck;
    aRes[i].failedShapes.first  = GetObject(Handle(::GEOM_Object)::DownCast((*anIntIt).FailedShapes.first));
    aRes[i].failedShapes.second = GetObject(Handle(::GEOM_Object)::DownCast((*anIntIt).FailedShapes.second));
  }

  return aRes._retn();
}

//=============================================================================
/*!
 *  UpdateTolerance
 *  Compute possible tolerance for the shape, minimize tolerance of shape as well
 *  as tolerance of sub-shapes as much as possible
 *  \param theShape Shape for check.
 */
 //=============================================================================
CORBA::Double GEOM_IMeasureOperations_i::UpdateTolerance(GEOM::GEOM_Object_ptr theShape)
{
  //Get the reference shape
  Handle(::GEOM_Object) aShapeRef = GetObjectImpl(theShape);
  if (aShapeRef.IsNull())
    return false;

  return GetOperations()->UpdateTolerance(aShapeRef);
}

void GEOM_IMeasureOperations_i::ConvertToList(const GEOM::GEOM_IMeasureOperations::CheckResults& theResuts,
                                              std::list<GEOMImpl_IMeasureOperations::FailedChecks>& theListOfResults)
{
  for (Standard_Integer i = 0; i < theResuts.length(); ++i)
  {
    GEOMImpl_IMeasureOperations::FailedChecks aCheck;
    aCheck.TypeOfCheck = theResuts[i].type;
    aCheck.FailedShapes.first = GetObjectImpl(theResuts[i].failedShapes.first);
    aCheck.FailedShapes.second = GetObjectImpl(theResuts[i].failedShapes.second);
    theListOfResults.push_back(aCheck);
  }
}

//=============================================================================
/*!
 *  ShapeProximityCalculator
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IMeasureOperations_i::ShapeProximityCalculator(GEOM::GEOM_Object_ptr theShape1,
                                                                          GEOM::GEOM_Object_ptr theShape2)
{
  GEOM::GEOM_Object_var anEmptyCalc;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference shape
  Handle(::GEOM_Object) aShape1 = GetObjectImpl(theShape1);
  Handle(::GEOM_Object) aShape2 = GetObjectImpl(theShape2);
  if (aShape1.IsNull() || aShape2.IsNull())
    return anEmptyCalc._retn();

  Handle(::GEOM_Object) aCalculator = GetOperations()->ShapeProximityCalculator(aShape1, aShape2);
  if (!GetOperations()->IsDone() || aCalculator.IsNull())
    return anEmptyCalc._retn();

  return GetObject(aCalculator);
}

//=============================================================================
/*!
 *  SetShapeSampling
 */
 //=============================================================================
void GEOM_IMeasureOperations_i::SetShapeSampling(GEOM::GEOM_Object_ptr theCalculator,
                                                 GEOM::GEOM_Object_ptr theShape,
                                                 CORBA::Long theNbSamples)
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the proximity calculator
  Handle(::GEOM_Object) aCalc = GetObjectImpl(theCalculator);
  if (aCalc.IsNull())
    return ;
  //Get the reference shape
  Handle(::GEOM_Object) aShape = GetObjectImpl(theShape);
  if (aShape.IsNull())
    return ;

  GetOperations()->SetShapeSampling(aCalc, aShape, theNbSamples);
}

//=============================================================================
/*!
 *  GetCoarseProximity
 */
 //=============================================================================
CORBA::Double GEOM_IMeasureOperations_i::GetCoarseProximity(GEOM::GEOM_Object_ptr theCalculator)
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference shape
  Handle(::GEOM_Object) aCalc = GetObjectImpl(theCalculator);
  if (aCalc.IsNull())
    return -1.0;

  Standard_Real aProximity = GetOperations()->GetCoarseProximity(aCalc);
  if (!GetOperations()->IsDone())
    return -1.0;

  return aProximity;
}

//=============================================================================
/*!
 *  GetPreciseProximity
 */
 //=============================================================================
CORBA::Double GEOM_IMeasureOperations_i::GetPreciseProximity(GEOM::GEOM_Object_ptr theCalculator)
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference shape
  Handle(::GEOM_Object) aCalc = GetObjectImpl(theCalculator);
  if (aCalc.IsNull())
    return -1.0;

  Standard_Real aProximity = GetOperations()->GetPreciseProximity(aCalc);
  if (!GetOperations()->IsDone())
    return -1.0;

  return aProximity;
}
