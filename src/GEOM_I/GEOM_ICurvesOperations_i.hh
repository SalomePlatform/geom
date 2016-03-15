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

#ifndef _GEOM_ICurvesOperations_i_HeaderFile
#define _GEOM_ICurvesOperations_i_HeaderFile

#include "GEOMImpl_Gen.hxx"

#include <SALOMEconfig.h>

#include CORBA_SERVER_HEADER(GEOM_Gen)
#include "GEOM_IOperations_i.hh"
#include "GEOM_Object_i.hh"

#include "GEOMImpl_ICurvesOperations.hxx"

class GEOM_I_EXPORT GEOM_ICurvesOperations_i :
    public virtual POA_GEOM::GEOM_ICurvesOperations,
    public virtual GEOM_IOperations_i
{
 public:
  GEOM_ICurvesOperations_i (PortableServer::POA_ptr thePOA, GEOM::GEOM_Gen_ptr theEngine,
                            ::GEOMImpl_ICurvesOperations* theImpl);
  ~GEOM_ICurvesOperations_i();

  GEOM::GEOM_Object_ptr MakeCirclePntVecR (GEOM::GEOM_Object_ptr theCenter,
                                           GEOM::GEOM_Object_ptr theVector,
                                           double theR);

  GEOM::GEOM_Object_ptr MakeCircleThreePnt (GEOM::GEOM_Object_ptr thePnt1,
                                            GEOM::GEOM_Object_ptr thePnt2,
                                            GEOM::GEOM_Object_ptr thePnt3);

  GEOM::GEOM_Object_ptr MakeCircleCenter2Pnt (GEOM::GEOM_Object_ptr thePnt1,
                                              GEOM::GEOM_Object_ptr thePnt2,
                                              GEOM::GEOM_Object_ptr thePnt3);

  GEOM::GEOM_Object_ptr MakeEllipse (GEOM::GEOM_Object_ptr theCenter,
                                     GEOM::GEOM_Object_ptr theVector,
                                     double theRMajor, double theRMinor);

  GEOM::GEOM_Object_ptr MakeEllipseVec (GEOM::GEOM_Object_ptr theCenter,
                                        GEOM::GEOM_Object_ptr theVector,
                                        double theRMajor, double theRMinor,
                                        GEOM::GEOM_Object_ptr theVectorMajor);

  GEOM::GEOM_Object_ptr MakeArc (GEOM::GEOM_Object_ptr thePnt1,
                                 GEOM::GEOM_Object_ptr thePnt2,
                                 GEOM::GEOM_Object_ptr thePnt3);

  GEOM::GEOM_Object_ptr MakeArcCenter (GEOM::GEOM_Object_ptr thePnt1,
                                       GEOM::GEOM_Object_ptr thePnt2,
                                       GEOM::GEOM_Object_ptr thePnt3,
                                       bool theSense);

  GEOM::GEOM_Object_ptr MakeArcOfEllipse (GEOM::GEOM_Object_ptr thePnt1,
                                          GEOM::GEOM_Object_ptr thePnt2,
                                          GEOM::GEOM_Object_ptr thePnt3);

  GEOM::GEOM_Object_ptr MakePolyline (const GEOM::ListOfGO& thePoints,
                                      CORBA::Boolean        theIsClosed);

  GEOM::GEOM_Object_ptr MakeSplineBezier (const GEOM::ListOfGO& thePoints,
                                          CORBA::Boolean        theIsClosed);

  GEOM::GEOM_Object_ptr MakeSplineInterpolation (const GEOM::ListOfGO& thePoints,
                                                 CORBA::Boolean        theIsClosed,
                                                 CORBA::Boolean        theDoReordering);

  GEOM::GEOM_Object_ptr MakeSplineInterpolWithTangents (const GEOM::ListOfGO& thePoints,
                                                        GEOM::GEOM_Object_ptr theFirstVec,
                                                        GEOM::GEOM_Object_ptr theLastVec);

  GEOM::GEOM_Object_ptr MakeCurveParametric
    (const char* thexExpr, const char* theyExpr, const char* thezExpr,
     double theParamMin, double theParamMax, double theParamStep,
     GEOM::curve_type theCurveType);

  GEOM::GEOM_Object_ptr MakeCurveParametricNew
    (const char* thexExpr, const char* theyExpr, const char* thezExpr,
     double theParamMin, double theParamMax, CORBA::Long theParamNbStep,
     GEOM::curve_type theCurveType);

  GEOM::GEOM_Object_ptr MakeIsoline (GEOM::GEOM_Object_ptr theFace,
                                     CORBA::Boolean        IsUIsoline,
                                     double                theParameter);

  GEOM::GEOM_Object_ptr MakeSketcher (const char* theCommand, const GEOM::ListOfDouble& theWorkingPlane);

  GEOM::GEOM_Object_ptr MakeSketcherOnPlane (const char* theCommand, GEOM::GEOM_Object_ptr theWorkingPlane);

  GEOM::GEOM_Object_ptr Make3DSketcherCommand (const char* theCommand);

  GEOM::GEOM_Object_ptr Make3DSketcher (const GEOM::ListOfDouble& theCoordinates);

  GEOM::GEOM_Object_ptr MakePolyline2D
                (const GEOM::ListOfListOfDouble &theCoordsList,
                 const GEOM::string_array       &theNamesList,
                 const GEOM::short_array        &theTypesList,
                 const GEOM::ListOfBool         &theClosedList,
                 const GEOM::ListOfDouble       &theWorkingPlane);

  GEOM::GEOM_Object_ptr MakePolyline2DOnPlane
                (const GEOM::ListOfListOfDouble &theCoordsList,
                 const GEOM::string_array       &theNamesList,
                 const GEOM::short_array        &theTypesList,
                 const GEOM::ListOfBool         &theClosedList,
                 GEOM::GEOM_Object_ptr           theWorkingPlane);

  ::GEOMImpl_ICurvesOperations* GetOperations()
  { return (::GEOMImpl_ICurvesOperations*)GetImpl(); }

private:

  Handle(TColStd_HArray1OfByte)
        ConvertEnumArray(const GEOM::short_array &theInArray);

  Handle(TColStd_HArray1OfByte)
        ConvertBoolArray(const GEOM::ListOfBool &theInArray);

  void ConvertListListDouble
            (const GEOM::ListOfListOfDouble        &theCoordsList,
                   std::list <std::list <double> > &theCoords);

};

#endif
