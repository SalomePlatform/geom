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

#ifndef _GEOM_IMeasureOperations_i_HeaderFile
#define _GEOM_IMeasureOperations_i_HeaderFile

#include "GEOMImpl_Gen.hxx"

#include <SALOMEconfig.h>

#include CORBA_SERVER_HEADER(GEOM_Gen)
#include "GEOM_IOperations_i.hh"
#include "GEOM_Object_i.hh"

#include "GEOMImpl_IMeasureOperations.hxx"

class GEOM_I_EXPORT GEOM_IMeasureOperations_i :
    public virtual POA_GEOM::GEOM_IMeasureOperations,
    public virtual GEOM_IOperations_i
{
 public:
  GEOM_IMeasureOperations_i (PortableServer::POA_ptr thePOA, GEOM::GEOM_Gen_ptr theEngine,
                            ::GEOMImpl_IMeasureOperations* theImpl);
  ~GEOM_IMeasureOperations_i();

  GEOM::GEOM_IKindOfShape::shape_kind KindOfShape (GEOM::GEOM_Object_ptr  theShape,
                                                   GEOM::ListOfLong_out   theIntegers,
                                                   GEOM::ListOfDouble_out theDoubles);

  void GetPosition (GEOM::GEOM_Object_ptr theShape,
                    CORBA::Double& Ox, CORBA::Double& Oy, CORBA::Double& Oz,
                    CORBA::Double& Zx, CORBA::Double& Zy, CORBA::Double& Zz,
                    CORBA::Double& Xx, CORBA::Double& Xy, CORBA::Double& Xz);

  void GetBasicProperties (GEOM::GEOM_Object_ptr theShape,
                           CORBA::Double  theTolerance,
                           CORBA::Double& theLength,
                           CORBA::Double& theSurfArea,
                           CORBA::Double& theVolume);

  GEOM::GEOM_Object_ptr GetCentreOfMass (GEOM::GEOM_Object_ptr theShape);

  GEOM::GEOM_Object_ptr GetNormal (GEOM::GEOM_Object_ptr theFace,
                                   GEOM::GEOM_Object_ptr theOptionalPoint);

  GEOM::GEOM_Object_ptr GetVertexByIndex (GEOM::GEOM_Object_ptr theObject,
                                          CORBA::Long theIndex);

  void GetInertia (GEOM::GEOM_Object_ptr theShape,
                   CORBA::Double& I11, CORBA::Double& I12, CORBA::Double& I13,
                   CORBA::Double& I21, CORBA::Double& I22, CORBA::Double& I23,
                   CORBA::Double& I31, CORBA::Double& I32, CORBA::Double& I33,
                   CORBA::Double& Ix , CORBA::Double& Iy , CORBA::Double& Iz);

  void GetBoundingBox (GEOM::GEOM_Object_ptr theShape,
                       CORBA::Boolean precise,
                       CORBA::Double& Xmin, CORBA::Double& Xmax,
                       CORBA::Double& Ymin, CORBA::Double& Ymax,
                       CORBA::Double& Zmin, CORBA::Double& Zmax);

  GEOM::GEOM_Object_ptr MakeBoundingBox (GEOM::GEOM_Object_ptr theShape,
                                         CORBA::Boolean precise);

  void GetTolerance (GEOM::GEOM_Object_ptr theShape,
                     CORBA::Double& FaceMin, CORBA::Double& FaceMax,
                     CORBA::Double& EdgeMin, CORBA::Double& EdgeMax,
                     CORBA::Double& VertMin, CORBA::Double& VertMax);

  CORBA::Boolean CheckShape
             (GEOM::GEOM_Object_ptr                          theShape,
              GEOM::GEOM_IMeasureOperations::ShapeErrors_out theErrors);

  CORBA::Boolean CheckShapeWithGeometry
             (GEOM::GEOM_Object_ptr                          theShape,
              GEOM::GEOM_IMeasureOperations::ShapeErrors_out theErrors);

  char* PrintShapeErrors
             (      GEOM::GEOM_Object_ptr                       theShape,
              const GEOM::GEOM_IMeasureOperations::ShapeErrors &theErrors);

  CORBA::Boolean CheckSelfIntersections (GEOM::GEOM_Object_ptr theShape,
                                         CORBA::Long           theCheckLevel,
                                         GEOM::ListOfLong_out  theIntersections);

  CORBA::Boolean CheckSelfIntersectionsFast (GEOM::GEOM_Object_ptr theShape,
					     CORBA::Float          theDeflection,
					     CORBA::Double         theTolerance,
					     GEOM::ListOfLong_out  theIntersections);

  CORBA::Boolean CheckBOPArguments (GEOM::GEOM_Object_ptr theShape);

  CORBA::Boolean FastIntersect (GEOM::GEOM_Object_ptr theShape1,
                                GEOM::GEOM_Object_ptr theShape2,
                                CORBA::Double         theTolerance,
                                CORBA::Float         theDeflection,
                                GEOM::ListOfLong_out  theIntersections1,
                                GEOM::ListOfLong_out  theIntersections2);

  char* IsGoodForSolid (GEOM::GEOM_Object_ptr theShape);

  char* WhatIs (GEOM::GEOM_Object_ptr theShape);

  GEOM::ListOfBool* AreCoordsInside (GEOM::GEOM_Object_ptr theShape,
                                     const GEOM::ListOfDouble& theCoords,
                                     CORBA::Double theTolerance);

  CORBA::Double GetMinDistance (GEOM::GEOM_Object_ptr theShape1,
                                GEOM::GEOM_Object_ptr theShape2,
                                CORBA::Double& X1, CORBA::Double& Y1, CORBA::Double& Z1,
                                CORBA::Double& X2, CORBA::Double& Y2, CORBA::Double& Z2);

  CORBA::Long ClosestPoints (GEOM::GEOM_Object_ptr theShape1,
                             GEOM::GEOM_Object_ptr theShape2,
                             GEOM::ListOfDouble_out theCoords);

  void PointCoordinates (GEOM::GEOM_Object_ptr theShape,
                         CORBA::Double& X, CORBA::Double& Y, CORBA::Double& Z);

  CORBA::Double GetAngle (GEOM::GEOM_Object_ptr theShape1,
                          GEOM::GEOM_Object_ptr theShape2);

  CORBA::Double GetAngleBtwVectors (GEOM::GEOM_Object_ptr theShape1,
                                    GEOM::GEOM_Object_ptr theShape2);

  // Methods for recieving radiuses of curvature of curves and surfaces
  // in the given point
  CORBA::Double CurveCurvatureByParam (GEOM::GEOM_Object_ptr theCurve,
                                       CORBA::Double theParam);

  CORBA::Double CurveCurvatureByPoint (GEOM::GEOM_Object_ptr theCurve,
                                       GEOM::GEOM_Object_ptr thePoint);

  CORBA::Double MaxSurfaceCurvatureByParam (GEOM::GEOM_Object_ptr theSurf,
                                            CORBA::Double theUParam,
                                            CORBA::Double theVParam);

  CORBA::Double MaxSurfaceCurvatureByPoint (GEOM::GEOM_Object_ptr theSurf,
                                            GEOM::GEOM_Object_ptr thePoint);

  CORBA::Double MinSurfaceCurvatureByParam (GEOM::GEOM_Object_ptr theSurf,
                                            CORBA::Double theUParam,
                                            CORBA::Double theVParam);

  CORBA::Double MinSurfaceCurvatureByPoint (GEOM::GEOM_Object_ptr theSurf,
                                            GEOM::GEOM_Object_ptr thePoint);

  ::GEOMImpl_IMeasureOperations* GetOperations()
  { return (::GEOMImpl_IMeasureOperations*)GetImpl(); }
};

#endif
