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

#ifndef _GEOMImpl_I3DPrimOperations_HXX_
#define _GEOMImpl_I3DPrimOperations_HXX_

#include "Utils_SALOME_Exception.hxx"
#include "GEOM_IOperations.hxx"
#include "GEOM_Engine.hxx"
#include "GEOM_Object.hxx"
#include <TDocStd_Document.hxx>

class GEOMImpl_I3DPrimOperations : public GEOM_IOperations {
 public:
  Standard_EXPORT GEOMImpl_I3DPrimOperations(GEOM_Engine* theEngine, int theDocID);
  Standard_EXPORT ~GEOMImpl_I3DPrimOperations();

  Standard_EXPORT Handle(GEOM_Object) MakeBoxDXDYDZ (double theDX, double theDY, double theDZ);
  Standard_EXPORT Handle(GEOM_Object) MakeBoxTwoPnt (Handle(GEOM_Object) thePnt1,
                                     Handle(GEOM_Object) thePnt2);

  Standard_EXPORT Handle(GEOM_Object) MakeCylinderRH       (double theR, double theH);
  Standard_EXPORT Handle(GEOM_Object) MakeCylinderPntVecRH (Handle(GEOM_Object) thePnt,
                                            Handle(GEOM_Object) theVec,
                                            double theR, double theH);

  Standard_EXPORT Handle(GEOM_Object) MakeConeR1R2H       (double theR1, double theR2, double theH);
  Standard_EXPORT Handle(GEOM_Object) MakeConePntVecR1R2H (Handle(GEOM_Object) thePnt,
                                           Handle(GEOM_Object) theVec,
                                           double theR1, double theR2, double theH);

  Standard_EXPORT Handle(GEOM_Object) MakeSphereR    (double theR);
  Standard_EXPORT Handle(GEOM_Object) MakeSpherePntR (Handle(GEOM_Object) thePnt, double theR);

  Standard_EXPORT Handle(GEOM_Object) MakeTorusRR (double theRMajor, double theRMinor);

  Standard_EXPORT Handle(GEOM_Object) MakeTorusPntVecRR (Handle(GEOM_Object) thePnt,
                                         Handle(GEOM_Object) theVec,
                                         double theRMajor, double theRMinor);

  Standard_EXPORT Handle(GEOM_Object) MakePrismVecH (Handle(GEOM_Object) theBase,
                                     Handle(GEOM_Object) theVec, double theH);

  Standard_EXPORT Handle(GEOM_Object) MakePrismTwoPnt (Handle(GEOM_Object) theBase,
                                       Handle(GEOM_Object) thePoint1,
                                       Handle(GEOM_Object) thePoint2);

  Standard_EXPORT Handle(GEOM_Object) MakePipe (Handle(GEOM_Object) theBase,
                                Handle(GEOM_Object) thePath);

  Standard_EXPORT Handle(GEOM_Object) MakeRevolutionAxisAngle (Handle(GEOM_Object) theBase,
                                               Handle(GEOM_Object) theAxis,
                                               double theAngle);

  Standard_EXPORT Handle(GEOM_Object) MakeSolidShell (Handle(GEOM_Object) theShell);

  Standard_EXPORT Handle(GEOM_Object) MakeFilling (Handle(GEOM_Object) theShape, int theMinDeg, int theMaxDeg, double theTol2D, double theTol3D, int theNbIter);

};

#endif
