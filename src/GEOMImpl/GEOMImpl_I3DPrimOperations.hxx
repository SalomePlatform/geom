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

#ifndef _GEOMImpl_I3DPrimOperations_HXX_
#define _GEOMImpl_I3DPrimOperations_HXX_

#include "Utils_SALOME_Exception.hxx"
#include "GEOM_IOperations.hxx"
#include "GEOM_Engine.hxx"
#include "GEOM_Object.hxx"
#include <TDocStd_Document.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

class GEOMImpl_IGroupOperations;
class GEOMImpl_IPipe;

class GEOMImpl_I3DPrimOperations : public GEOM_IOperations {
 public:
  Standard_EXPORT GEOMImpl_I3DPrimOperations(GEOM_Engine* theEngine);
  Standard_EXPORT ~GEOMImpl_I3DPrimOperations();

  Standard_EXPORT Handle(GEOM_Object) MakeBoxDXDYDZ (double theDX, double theDY, double theDZ);
  Standard_EXPORT Handle(GEOM_Object) MakeBoxTwoPnt (Handle(GEOM_Object) thePnt1,
                                                     Handle(GEOM_Object) thePnt2);
  Standard_EXPORT Handle(GEOM_Object) MakeFaceHW (double theH, double theW, int theOrientation);
  Standard_EXPORT Handle(GEOM_Object) MakeFaceObjHW (Handle(GEOM_Object) theObj,
                                                     double theH, double theW);
  Standard_EXPORT Handle(GEOM_Object) MakeDiskThreePnt (Handle(GEOM_Object) thePnt1,
                                                        Handle(GEOM_Object) thePnt2,
                                                        Handle(GEOM_Object) thePnt3);
  Standard_EXPORT Handle(GEOM_Object) MakeDiskPntVecR (Handle(GEOM_Object) thePnt1,
                                                       Handle(GEOM_Object) theVec,
                                                       double theR);
  Standard_EXPORT Handle(GEOM_Object) MakeDiskR (double theR, int theOrientation);
  Standard_EXPORT Handle(GEOM_Object) MakeCylinderRH       (double theR, double theH);
  Standard_EXPORT Handle(GEOM_Object) MakeCylinderPntVecRH (Handle(GEOM_Object) thePnt,
                                                            Handle(GEOM_Object) theVec,
                                                            double theR, double theH);
  Standard_EXPORT Handle(GEOM_Object) MakeCylinderRHA       (double theR, double theH, double theA);
  Standard_EXPORT Handle(GEOM_Object) MakeCylinderPntVecRHA (Handle(GEOM_Object) thePnt,
                                                             Handle(GEOM_Object) theVec,
                                                             double theR, double theH, double theA);                                        

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
                                                     Handle(GEOM_Object) theVec,
                                                     double theH, 
                                                     double theScaleFactor = -1.0);

  Standard_EXPORT Handle(GEOM_Object) MakePrismVecH2Ways (Handle(GEOM_Object) theBase,
                                                          Handle(GEOM_Object) theVec, double theH);

  Standard_EXPORT Handle(GEOM_Object) MakePrismTwoPnt (Handle(GEOM_Object) theBase,
                                                       Handle(GEOM_Object) thePoint1,
                                                       Handle(GEOM_Object) thePoint2,
                                                       double theScaleFactor = -1.0);

  Standard_EXPORT Handle(GEOM_Object) MakePrismTwoPnt2Ways (Handle(GEOM_Object) theBase,
                                                            Handle(GEOM_Object) thePoint1,
                                                            Handle(GEOM_Object) thePoint2);

  Standard_EXPORT Handle(GEOM_Object) MakePrismDXDYDZ (Handle(GEOM_Object) theBase,
                                                       double theDX, double theDY, double theDZ,
                                                       double theScaleFactor = -1.0);

  Standard_EXPORT Handle(GEOM_Object) MakePrismDXDYDZ2Ways (Handle(GEOM_Object) theBase,
                                                            double theDX, double theDY, double theDZ);
  
  Standard_EXPORT Handle(GEOM_Object) MakeDraftPrism  (Handle(GEOM_Object) theInitShape, Handle(GEOM_Object) theBase,
                                                       double theHeight, double theAngle, bool theFuse, bool theInvert = false );
  
  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) MakePipe
                            (const Handle(GEOM_Object) &theBase,
                             const Handle(GEOM_Object) &thePath,
                             const bool                 IsGenerateGroups);

  Standard_EXPORT Handle(GEOM_Object) MakeRevolutionAxisAngle (Handle(GEOM_Object) theBase,
                                                               Handle(GEOM_Object) theAxis,
                                                               double theAngle);

  Standard_EXPORT Handle(GEOM_Object) MakeRevolutionAxisAngle2Ways (Handle(GEOM_Object) theBase,
                                                                    Handle(GEOM_Object) theAxis,
                                                                    double theAngle);

  Standard_EXPORT Handle(GEOM_Object) MakeFilling (std::list< Handle(GEOM_Object)> & theContours,
                                                   int theMinDeg, int theMaxDeg,
                                                   double theTol2D, double theTol3D,
                                                   int theNbIter, int theMethod,
                                                   bool isApprox);

  Standard_EXPORT Handle(GEOM_Object) MakeThruSections
                                      (const Handle(TColStd_HSequenceOfTransient)& theSeqSections,
                                       bool theModeSolid,
                                       double thePreci,
                                       bool theRuled);

  Standard_EXPORT Handle(TColStd_HSequenceOfTransient)
      MakePipeWithDifferentSections
              (const Handle(TColStd_HSequenceOfTransient) &theBases,
               const Handle(TColStd_HSequenceOfTransient) &theLocations,
               const Handle(GEOM_Object)                  &thePath,
               const bool                                  theWithContact,
               const bool                                  theWithCorrections,
               const bool                                  IsBySteps,
               const bool                                  IsGenerateGroups);

  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) MakePipeWithShellSections
              (const Handle(TColStd_HSequenceOfTransient) &theBases,
               const Handle(TColStd_HSequenceOfTransient) &theSubBases,
               const Handle(TColStd_HSequenceOfTransient) &theLocations,
               const Handle(GEOM_Object)                  &thePath,
               const bool                                  theWithContact,
               const bool                                  theWithCorrections,
               const bool                                  IsGenerateGroups);

  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) MakePipeShellsWithoutPath
              (const Handle(TColStd_HSequenceOfTransient) &theBases,
               const Handle(TColStd_HSequenceOfTransient) &theLocations,
               const bool                                  IsGenerateGroups);

  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) MakePipeBiNormalAlongVector
                (const Handle(GEOM_Object) &theBase,
                 const Handle(GEOM_Object) &thePath,
                 const Handle(GEOM_Object) &theVec,
                 const bool                 IsGenerateGroups);
  
  Standard_EXPORT Handle(GEOM_Object) MakeThickening
                (Handle(GEOM_Object)                     theObject,
                 const Handle(TColStd_HArray1OfInteger) &theFacesIDs,
                 double                                  theOffset,
                 bool                                    isCopy,
                 bool                                    theInside = false);

  Standard_EXPORT Handle(GEOM_Object) RestorePath (Handle(GEOM_Object) theShape,
                                                   Handle(GEOM_Object) theBase1,
                                                   Handle(GEOM_Object) theBase2);

  Standard_EXPORT Handle(GEOM_Object) RestorePath (Handle(GEOM_Object) theShape,
                                                   const Handle(TColStd_HSequenceOfTransient)& theBase1,
                                                   const Handle(TColStd_HSequenceOfTransient)& theBase2);

private:

  Handle(GEOM_Object) createGroup
                  (const Handle(GEOM_Object)              &theBaseObject,
                   const Handle(TColStd_HArray1OfInteger) &theGroupIDs,
                   const TCollection_AsciiString          &theName,
                   const TopTools_IndexedMapOfShape       &theIndices);

  void createGroups(const Handle(GEOM_Object)                  &theBaseObject,
                          GEOMImpl_IPipe                       *thePipe,
                          Handle(TColStd_HSequenceOfTransient) &theSequence);

private:

  GEOMImpl_IGroupOperations *myGroupOperations;

};

#endif
