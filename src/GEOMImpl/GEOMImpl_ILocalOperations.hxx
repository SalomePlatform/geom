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

#ifndef _GEOMImpl_ILocalOperations_HXX_
#define _GEOMImpl_ILocalOperations_HXX_

#include "Utils_SALOME_Exception.hxx"
#include "GEOM_IOperations.hxx"
#include "GEOM_Engine.hxx"
#include "GEOM_Object.hxx"
#include <TDocStd_Document.hxx>

#include <list>

class GEOMImpl_ILocalOperations : public GEOM_IOperations {
 public:
  Standard_EXPORT GEOMImpl_ILocalOperations(GEOM_Engine* theEngine, int theDocID);
  Standard_EXPORT ~GEOMImpl_ILocalOperations();

  Standard_EXPORT Handle(GEOM_Object) MakeFilletAll   (Handle(GEOM_Object) theShape, double theR);
  Standard_EXPORT Handle(GEOM_Object) MakeFilletEdges (Handle(GEOM_Object) theShape, double theR,
                                       list<int> theEdges);
  Standard_EXPORT Handle(GEOM_Object) MakeFilletFaces (Handle(GEOM_Object) theShape, double theR,
                                       list<int> theFaces);

  Standard_EXPORT Handle(GEOM_Object) MakeChamferAll   (Handle(GEOM_Object) theShape, double theD);
  Standard_EXPORT Handle(GEOM_Object) MakeChamferEdge  (Handle(GEOM_Object) theShape,
                                        double theD1, double theD2,
                                        int theFace1, int theFace2);
  Standard_EXPORT Handle(GEOM_Object) MakeChamferFaces (Handle(GEOM_Object) theShape,
                                        double theD1, double theD2,
                                        list<int> theFaces);

  Standard_EXPORT Handle(GEOM_Object) MakeArchimede (Handle(GEOM_Object) theShape,
                                     double theWeight, double theWaterDensity,
                                     double theMeshingDeflection);

  Standard_EXPORT Standard_Integer GetSubShapeIndex (Handle(GEOM_Object) theShape,
                                     Handle(GEOM_Object) theSubShape);

  Standard_EXPORT static bool GetSubShape (const TopoDS_Shape& theShape,
                           const int theIndex,
                           TopoDS_Shape& theSubShape);
};

#endif
