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

#ifndef _GEOM_I3DPrimOperations_i_HeaderFile
#define _GEOM_I3DPrimOperations_i_HeaderFile

#include "GEOMImpl_Gen.hxx"

#include <SALOMEconfig.h>

#include CORBA_SERVER_HEADER(GEOM_Gen)
#include "GEOM_IOperations_i.hh"
#include "GEOM_Object_i.hh"

#include "GEOMImpl_I3DPrimOperations.hxx"

class GEOM_I_EXPORT GEOM_I3DPrimOperations_i :
    public virtual POA_GEOM::GEOM_I3DPrimOperations,
    public virtual GEOM_IOperations_i
{
 public:
  GEOM_I3DPrimOperations_i (PortableServer::POA_ptr thePOA, GEOM::GEOM_Gen_ptr theEngine,
			    ::GEOMImpl_I3DPrimOperations* theImpl);
  ~GEOM_I3DPrimOperations_i();

  GEOM::GEOM_Object_ptr MakeBoxDXDYDZ (CORBA::Double theDX,
				       CORBA::Double theDY,
				       CORBA::Double theDZ);

  GEOM::GEOM_Object_ptr MakeBoxTwoPnt (GEOM::GEOM_Object_ptr thePnt1,
				       GEOM::GEOM_Object_ptr thePnt2);
				    
  GEOM::GEOM_Object_ptr MakeFaceHW (CORBA::Double theH,
				    CORBA::Double theW,
				    CORBA::Short  theOrientation);

  GEOM::GEOM_Object_ptr MakeFaceObjHW (GEOM::GEOM_Object_ptr theObj,
				       CORBA::Double theH,
				       CORBA::Double theW);

  GEOM::GEOM_Object_ptr MakeDiskPntVecR (GEOM::GEOM_Object_ptr theCenter,
					 GEOM::GEOM_Object_ptr theVector,
					 double theR);

  GEOM::GEOM_Object_ptr MakeDiskThreePnt (GEOM::GEOM_Object_ptr thePnt1,
					  GEOM::GEOM_Object_ptr thePnt2,
					  GEOM::GEOM_Object_ptr thePnt3);

  GEOM::GEOM_Object_ptr MakeDiskR (CORBA::Double theR,
				   CORBA::Short  theOrientation);

  GEOM::GEOM_Object_ptr MakeCylinderRH (CORBA::Double theR,
					CORBA::Double theH);

  GEOM::GEOM_Object_ptr MakeCylinderPntVecRH (GEOM::GEOM_Object_ptr thePnt,
					      GEOM::GEOM_Object_ptr theVec,
					      CORBA::Double theR,
					      CORBA::Double theH);
					      
  GEOM::GEOM_Object_ptr MakeCylinderRHA (CORBA::Double theR,
					 CORBA::Double theH,
					 CORBA::Double theA);

  GEOM::GEOM_Object_ptr MakeCylinderPntVecRHA (GEOM::GEOM_Object_ptr thePnt,
					       GEOM::GEOM_Object_ptr theVec,
					       CORBA::Double theR,
					       CORBA::Double theH,
					       CORBA::Double theA);					      

  GEOM::GEOM_Object_ptr MakeConeR1R2H (CORBA::Double theR1,
				       CORBA::Double theR2,
				       CORBA::Double theH);

  GEOM::GEOM_Object_ptr MakeConePntVecR1R2H (GEOM::GEOM_Object_ptr thePnt,
					     GEOM::GEOM_Object_ptr theVec,
					     CORBA::Double theR1,
					     CORBA::Double theR2,
					     CORBA::Double theH);

  GEOM::GEOM_Object_ptr MakeSphereR (CORBA::Double theR);

  GEOM::GEOM_Object_ptr MakeSpherePntR (GEOM::GEOM_Object_ptr thePnt,
					CORBA::Double theR);

  GEOM::GEOM_Object_ptr MakeTorusRR (CORBA::Double theRMajor,
				     CORBA::Double theRMinor);

  GEOM::GEOM_Object_ptr MakeTorusPntVecRR (GEOM::GEOM_Object_ptr theCenter,
					   GEOM::GEOM_Object_ptr theVector,
					   CORBA::Double theRMajor,
					   CORBA::Double theRMinor);

  GEOM::GEOM_Object_ptr MakePrismVecH (GEOM::GEOM_Object_ptr theBase,
				       GEOM::GEOM_Object_ptr theVec,
				       CORBA::Double theH);

  GEOM::GEOM_Object_ptr MakePrismVecH2Ways (GEOM::GEOM_Object_ptr theBase,
					    GEOM::GEOM_Object_ptr theVec,
					    CORBA::Double theH);

  GEOM::GEOM_Object_ptr MakePrismVecHWithScaling (GEOM::GEOM_Object_ptr theBase,
                                                  GEOM::GEOM_Object_ptr theVec,
                                                  CORBA::Double theH,
                                                  CORBA::Double theScaleFactor);

  GEOM::GEOM_Object_ptr MakePrismTwoPnt (GEOM::GEOM_Object_ptr theBase,
					 GEOM::GEOM_Object_ptr thePoint1,
					 GEOM::GEOM_Object_ptr thePoint2);

  GEOM::GEOM_Object_ptr MakePrismTwoPnt2Ways (GEOM::GEOM_Object_ptr theBase,
					      GEOM::GEOM_Object_ptr thePoint1,
					      GEOM::GEOM_Object_ptr thePoint2);

  GEOM::GEOM_Object_ptr MakePrismTwoPntWithScaling (GEOM::GEOM_Object_ptr theBase,
                                                    GEOM::GEOM_Object_ptr thePoint1,
                                                    GEOM::GEOM_Object_ptr thePoint2,
                                                    CORBA::Double         theScaleFactor);

  GEOM::GEOM_Object_ptr MakePrismDXDYDZ (GEOM::GEOM_Object_ptr theBase,
				         CORBA::Double         theDX,
				         CORBA::Double         theDY,
				         CORBA::Double         theDZ);
  GEOM::GEOM_Object_ptr MakePrismDXDYDZ2Ways (GEOM::GEOM_Object_ptr theBase,
				              CORBA::Double         theDX,
				              CORBA::Double         theDY,
				              CORBA::Double         theDZ);
  GEOM::GEOM_Object_ptr MakePrismDXDYDZWithScaling (GEOM::GEOM_Object_ptr theBase,
                                                    CORBA::Double         theDX,
                                                    CORBA::Double         theDY,
                                                    CORBA::Double         theDZ,
                                                    CORBA::Double         theScaleFactor);
  
  GEOM::GEOM_Object_ptr MakeDraftPrism (GEOM::GEOM_Object_ptr theInitShape,
                                        GEOM::GEOM_Object_ptr theBase,
                                        CORBA::Double         theHeight,
                                        CORBA::Double         theAngle,
                                        CORBA::Boolean        theFuse,
                                        CORBA::Boolean        theInvert);

  GEOM::ListOfGO* MakePipe (GEOM::GEOM_Object_ptr theBase,
                            GEOM::GEOM_Object_ptr thePath,
                            CORBA::Boolean        IsGenerateGroups);

  GEOM::GEOM_Object_ptr MakeRevolutionAxisAngle (GEOM::GEOM_Object_ptr theBase,
						 GEOM::GEOM_Object_ptr theAxis,
						 CORBA::Double theAngle);

  GEOM::GEOM_Object_ptr MakeRevolutionAxisAngle2Ways (GEOM::GEOM_Object_ptr theBase,
						      GEOM::GEOM_Object_ptr theAxis,
						      CORBA::Double theAngle);

  GEOM::GEOM_Object_ptr MakeFilling(const GEOM::ListOfGO& theContours,
                                    CORBA::Long theMinDeg, CORBA::Long theMaxDeg,
                                    CORBA::Double theTol2D, CORBA::Double theTol3D,
                                    CORBA::Long theNbIter,
                                    GEOM::filling_oper_method theMethod,
                                    CORBA::Boolean theApprox);

  GEOM::GEOM_Object_ptr MakeThruSections(const GEOM::ListOfGO& theSeqSections,
					 CORBA::Boolean theModeSolid,
					 CORBA::Double thePreci,
					 CORBA::Boolean theRuled);

  GEOM::ListOfGO* MakePipeWithDifferentSections
                           (const GEOM::ListOfGO        &theBases,
                            const GEOM::ListOfGO        &theLocations,
                                  GEOM::GEOM_Object_ptr  thePath,
                                  CORBA::Boolean         theWithContact,
                                  CORBA::Boolean         theWithCorrections,
                                  CORBA::Boolean         IsBySteps,
                                  CORBA::Boolean         IsGenerateGroups);

  GEOM::ListOfGO* MakePipeWithShellSections
                           (const GEOM::ListOfGO        &theBases,
                            const GEOM::ListOfGO        &theSubBases,
                            const GEOM::ListOfGO        &theLocations,
                                  GEOM::GEOM_Object_ptr  thePath,
                                  CORBA::Boolean         theWithContact,
                                  CORBA::Boolean         theWithCorrections,
                                  CORBA::Boolean         IsGenerateGroups);

  GEOM::ListOfGO* MakePipeShellsWithoutPath
                           (const GEOM::ListOfGO &theBases,
                            const GEOM::ListOfGO &theLocations,
                                  CORBA::Boolean  IsGenerateGroups);

  GEOM::ListOfGO* MakePipeBiNormalAlongVector
                           (GEOM::GEOM_Object_ptr theBase,
                            GEOM::GEOM_Object_ptr thePath,
                            GEOM::GEOM_Object_ptr theVec,
                            CORBA::Boolean        IsGenerateGroups);

  GEOM::GEOM_Object_ptr MakeThickening (GEOM::GEOM_Object_ptr   theObject,
                                        const GEOM::ListOfLong &theFacesIDs,
                                        CORBA::Double           theOffset,
                                        CORBA::Boolean          isCopy,
                                        CORBA::Boolean          theInside);

  GEOM::GEOM_Object_ptr RestorePath (GEOM::GEOM_Object_ptr theShape,
                                     GEOM::GEOM_Object_ptr theBase1,
                                     GEOM::GEOM_Object_ptr theBase2);
  GEOM::GEOM_Object_ptr RestorePathEdges (GEOM::GEOM_Object_ptr theShape,
                                          const GEOM::ListOfGO& theBase1,
                                          const GEOM::ListOfGO& theBase2);

  ::GEOMImpl_I3DPrimOperations* GetOperations()
  { return (::GEOMImpl_I3DPrimOperations*)GetImpl(); }
};

#endif
