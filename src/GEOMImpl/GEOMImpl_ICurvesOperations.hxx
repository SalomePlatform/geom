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
using namespace std;

#ifndef _GEOMImpl_ICurvesOperations_HXX_
#define _GEOMImpl_ICurvesOperations_HXX_

#include "GEOM_IOperations.hxx"

#include <TCollection_AsciiString.hxx>

#include <list>

class GEOM_Engine;
class Handle(GEOM_Object);

class GEOMImpl_ICurvesOperations : public GEOM_IOperations {
 public:
  Standard_EXPORT GEOMImpl_ICurvesOperations(GEOM_Engine* theEngine, int theDocID);
  Standard_EXPORT ~GEOMImpl_ICurvesOperations();

  Standard_EXPORT Handle(GEOM_Object) MakePolyline (list<Handle(GEOM_Object)> thePoints);

  Standard_EXPORT Handle(GEOM_Object) MakeCircleThreePnt (Handle(GEOM_Object) thePnt1,
                                                          Handle(GEOM_Object) thePnt2,
                                                          Handle(GEOM_Object) thePnt3);
  Standard_EXPORT Handle(GEOM_Object) MakeCirclePntVecR  (Handle(GEOM_Object) thePnt,
                                                          Handle(GEOM_Object) theVec, double theR);

  Standard_EXPORT Handle(GEOM_Object) MakeEllipse (Handle(GEOM_Object) thePnt,
                                                   Handle(GEOM_Object) theVec,
                                                   double theRMajor, double theRMinor);

  Standard_EXPORT Handle(GEOM_Object) MakeArc (Handle(GEOM_Object) thePnt1,
                                               Handle(GEOM_Object) thePnt2,
                                               Handle(GEOM_Object) thePnt3);

  Standard_EXPORT Handle(GEOM_Object) MakeSplineBezier        (list<Handle(GEOM_Object)> thePoints);
  Standard_EXPORT Handle(GEOM_Object) MakeSplineInterpolation (list<Handle(GEOM_Object)> thePoints);

  Standard_EXPORT Handle(GEOM_Object) MakeSketcher (const TCollection_AsciiString& theCommand,
                                                    list<double> theWorkingPlane);
  Standard_EXPORT Handle(GEOM_Object) MakeSketcherOnPlane (const TCollection_AsciiString& theCommand,
                                                           Handle(GEOM_Object) theWorkingPlane);
};

#endif
