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

#ifndef _GEOM_ITransformOperations_i_HeaderFile
#define _GEOM_ITransformOperations_i_HeaderFile

#include "GEOMImpl_Gen.hxx"

#include <SALOMEconfig.h>

#include CORBA_SERVER_HEADER(GEOM_Gen)
#include "GEOM_IOperations_i.hh"
#include "GEOM_Object_i.hh"

#include "GEOMImpl_ITransformOperations.hxx"

class GEOM_I_EXPORT GEOM_ITransformOperations_i :
    public virtual POA_GEOM::GEOM_ITransformOperations,
    public virtual GEOM_IOperations_i
{
 public:
  GEOM_ITransformOperations_i (PortableServer::POA_ptr thePOA,
                               GEOM::GEOM_Gen_ptr theEngine,
                               ::GEOMImpl_ITransformOperations* theImpl);
  ~GEOM_ITransformOperations_i();

  GEOM::GEOM_Object_ptr TranslateTwoPoints (GEOM::GEOM_Object_ptr theObject,
                                            GEOM::GEOM_Object_ptr thePoint1,
                                            GEOM::GEOM_Object_ptr thePoint2);

  GEOM::GEOM_Object_ptr TranslateTwoPointsCopy (GEOM::GEOM_Object_ptr theObject,
                                                GEOM::GEOM_Object_ptr thePoint1,
                                                GEOM::GEOM_Object_ptr thePoint2);

  GEOM::GEOM_Object_ptr TranslateDXDYDZ (GEOM::GEOM_Object_ptr theObject,
                                         CORBA::Double theDX, CORBA::Double theDY, CORBA::Double theDZ);


  GEOM::GEOM_Object_ptr TranslateDXDYDZCopy (GEOM::GEOM_Object_ptr theObject,
                                             CORBA::Double theDX, CORBA::Double theDY, CORBA::Double theDZ);

  GEOM::GEOM_Object_ptr TranslateVector (GEOM::GEOM_Object_ptr theObject,
                                         GEOM::GEOM_Object_ptr theVector);

  GEOM::GEOM_Object_ptr TranslateVectorCopy (GEOM::GEOM_Object_ptr theObject,
                                             GEOM::GEOM_Object_ptr theVector);

  GEOM::GEOM_Object_ptr TranslateVectorDistance (GEOM::GEOM_Object_ptr theObject,
                                                 GEOM::GEOM_Object_ptr theVector,
                                                 CORBA::Double theDistance,
                                                 CORBA::Boolean theCopy);

  GEOM::GEOM_Object_ptr MultiTranslate1D (GEOM::GEOM_Object_ptr theObject,
                                          GEOM::GEOM_Object_ptr theVector,
                                          CORBA::Double theStep, CORBA::Long theNbTimes);

  GEOM::GEOM_Object_ptr MultiTranslate2D (GEOM::GEOM_Object_ptr theObject,
                                          GEOM::GEOM_Object_ptr theVector1,
                                          CORBA::Double theStep1, CORBA::Long theNbTimes1,
                                          GEOM::GEOM_Object_ptr theVector2,
                                          CORBA::Double theStep2, CORBA::Long theNbTimes2);

  GEOM::GEOM_Object_ptr Rotate (GEOM::GEOM_Object_ptr theObject,
                                GEOM::GEOM_Object_ptr theAxis,
                                CORBA::Double theAngle);

  GEOM::GEOM_Object_ptr MultiRotate1D (GEOM::GEOM_Object_ptr theObject,
                                       GEOM::GEOM_Object_ptr theVector,
                                       CORBA::Long theNbTimes);

  GEOM::GEOM_Object_ptr MultiRotate1DByStep (GEOM::GEOM_Object_ptr theObject,
                                             GEOM::GEOM_Object_ptr theAxis,
                                             CORBA::Double theAngleStep,
                                             CORBA::Long theNbSteps);

  GEOM::GEOM_Object_ptr MultiRotate2DNbTimes (GEOM::GEOM_Object_ptr theObject,
                                              GEOM::GEOM_Object_ptr theAxis,
                                              CORBA::Long theNbObjects,
                                              CORBA::Double theRadialStep,
                                              CORBA::Long theNbSteps);

  GEOM::GEOM_Object_ptr MultiRotate2DByStep (GEOM::GEOM_Object_ptr theObject,
                                             GEOM::GEOM_Object_ptr theVector,
                                             CORBA::Double theAngle,
                                             CORBA::Long theNbTimes1,
                                             CORBA::Double theStep,
                                             CORBA::Long theNbTimes2);

  GEOM::GEOM_Object_ptr MultiRotate2D (GEOM::GEOM_Object_ptr theObject,
                                       GEOM::GEOM_Object_ptr theVector,
                                       CORBA::Double theAngle,
                                       CORBA::Long theNbTimes1,
                                       CORBA::Double theStep,
                                       CORBA::Long theNbTimes2);

  GEOM::GEOM_Object_ptr RotateCopy (GEOM::GEOM_Object_ptr theObject,
                                    GEOM::GEOM_Object_ptr theAxis,
                                    CORBA::Double theAngle);

  GEOM::GEOM_Object_ptr MirrorPlane (GEOM::GEOM_Object_ptr theObject,
                                     GEOM::GEOM_Object_ptr thePlane);

  GEOM::GEOM_Object_ptr MirrorPlaneCopy (GEOM::GEOM_Object_ptr theObject,
                                         GEOM::GEOM_Object_ptr thePlane);

  GEOM::GEOM_Object_ptr MirrorAxis (GEOM::GEOM_Object_ptr theObject,
                                    GEOM::GEOM_Object_ptr theAxis);

  GEOM::GEOM_Object_ptr MirrorAxisCopy (GEOM::GEOM_Object_ptr theObject,
                                        GEOM::GEOM_Object_ptr theAxis);

  GEOM::GEOM_Object_ptr MirrorPoint (GEOM::GEOM_Object_ptr theObject,
                                     GEOM::GEOM_Object_ptr thePoint);

  GEOM::GEOM_Object_ptr MirrorPointCopy (GEOM::GEOM_Object_ptr theObject,
                                         GEOM::GEOM_Object_ptr thePoint);

  GEOM::GEOM_Object_ptr OffsetShape (GEOM::GEOM_Object_ptr theObject,
                                     CORBA::Double theOffset);

  GEOM::GEOM_Object_ptr OffsetShapeCopy (GEOM::GEOM_Object_ptr theObject,
                                         CORBA::Double theOffset);

  GEOM::GEOM_Object_ptr ProjectShapeCopy (GEOM::GEOM_Object_ptr theSource,
                                          GEOM::GEOM_Object_ptr theTarget);

  CORBA::Double ProjectPointOnWire (GEOM::GEOM_Object_ptr thePoint,
                                    GEOM::GEOM_Object_ptr theWire,
                                    GEOM::GEOM_Object_out thePointOnEdge,
                                    CORBA::Long&          theEdgeInWireIndex);

  GEOM::GEOM_Object_ptr ScaleShape (GEOM::GEOM_Object_ptr theObject,
                                    GEOM::GEOM_Object_ptr thePoint,
                                    CORBA::Double theFactor);

  GEOM::GEOM_Object_ptr ScaleShapeCopy (GEOM::GEOM_Object_ptr theObject,
                                        GEOM::GEOM_Object_ptr thePoint,
                                        CORBA::Double theFactor);

  GEOM::GEOM_Object_ptr ScaleShapeAlongAxes (GEOM::GEOM_Object_ptr theObject,
                                             GEOM::GEOM_Object_ptr thePoint,
                                             CORBA::Double theFactorX,
                                             CORBA::Double theFactorY,
                                             CORBA::Double theFactorZ);

  GEOM::GEOM_Object_ptr ScaleShapeAlongAxesCopy (GEOM::GEOM_Object_ptr theObject,
                                                 GEOM::GEOM_Object_ptr thePoint,
                                                 CORBA::Double theFactorX,
                                                 CORBA::Double theFactorY,
                                                 CORBA::Double theFactorZ);

  GEOM::GEOM_Object_ptr PositionShape (GEOM::GEOM_Object_ptr theObject,
                                       GEOM::GEOM_Object_ptr theStartLCS,
                                       GEOM::GEOM_Object_ptr theEndLCS);

  GEOM::GEOM_Object_ptr PositionShapeCopy (GEOM::GEOM_Object_ptr theObject,
                                           GEOM::GEOM_Object_ptr theStartLCS,
                                           GEOM::GEOM_Object_ptr theEndLCS);

  GEOM::GEOM_Object_ptr PositionAlongPath (GEOM::GEOM_Object_ptr theObject,
                                           GEOM::GEOM_Object_ptr thePath,
                                           CORBA::Double         theDistance,
                                           CORBA::Boolean        theCopy,
                                           CORBA::Boolean        theReverse);

  GEOM::GEOM_Object_ptr RotateThreePoints (GEOM::GEOM_Object_ptr theObject,
                                           GEOM::GEOM_Object_ptr theCentPoint,
                                           GEOM::GEOM_Object_ptr thePoint1,
                                           GEOM::GEOM_Object_ptr thePoint2);

  GEOM::GEOM_Object_ptr RotateThreePointsCopy (GEOM::GEOM_Object_ptr theObject,
                                               GEOM::GEOM_Object_ptr theCentPoint,
                                               GEOM::GEOM_Object_ptr thePoint1,
                                               GEOM::GEOM_Object_ptr thePoint2);

  GEOM::GEOM_Object_ptr TransformLikeOtherCopy (GEOM::GEOM_Object_ptr theObject,
                                                GEOM::GEOM_Object_ptr theSample);

  GEOM::GEOM_Object_ptr RecomputeObject (GEOM::GEOM_Object_ptr theObject);

  GEOM::GEOM_Object_ptr MakeProjectionOnCylinder
                                   (GEOM::GEOM_Object_ptr theObject,
                                    CORBA::Double         theRadius,
                                    CORBA::Double         theStartAngle,
                                    CORBA::Double         theAngleLength,
                                    CORBA::Double         theAngleRotation);

  ::GEOMImpl_ITransformOperations* GetOperations() { return (::GEOMImpl_ITransformOperations*)GetImpl(); }
};

#endif
