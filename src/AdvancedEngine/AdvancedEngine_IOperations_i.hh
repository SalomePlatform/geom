// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

//  File   : AdvancedEngine_IOperations_i.hh
//  Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

#ifndef _AdvancedEngine_IOperations_i_HeaderFile
#define _AdvancedEngine_IOperations_i_HeaderFile

#include "AdvancedEngine.hxx"

#include "GEOM_IOperations_i.hh"
#include "GEOM_Object_i.hh"

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(GEOM_Gen)
#include CORBA_SERVER_HEADER(AdvancedGEOM)

class AdvancedEngine_IOperations;

class ADVANCEDENGINE_EXPORT AdvancedEngine_IOperations_i :
    public virtual POA_GEOM::IAdvancedOperations,
    public virtual GEOM_IOperations_i
{
 public:
  AdvancedEngine_IOperations_i (PortableServer::POA_ptr thePOA, GEOM::GEOM_Gen_ptr theEngine,
				AdvancedEngine_IOperations* theImpl);
  ~AdvancedEngine_IOperations_i();

  // PipeTShape without thickness reduction
  GEOM::ListOfGO* MakePipeTShape (CORBA::Double theR1, CORBA::Double theW1, CORBA::Double theL1,
                                  CORBA::Double theR2, CORBA::Double theW2, CORBA::Double theL2,
                                  CORBA::Boolean theHexMesh);
  GEOM::ListOfGO* MakePipeTShapeWithPosition (CORBA::Double theR1, CORBA::Double theW1, CORBA::Double theL1,
                                              CORBA::Double theR2, CORBA::Double theW2, CORBA::Double theL2,
                                              CORBA::Boolean theHexMesh,
                                              GEOM::GEOM_Object_ptr theP1, GEOM::GEOM_Object_ptr theP2, GEOM::GEOM_Object_ptr theP3);
  GEOM::ListOfGO* MakePipeTShapeChamfer (CORBA::Double theR1, CORBA::Double theW1, CORBA::Double theL1,
                                         CORBA::Double theR2, CORBA::Double theW2, CORBA::Double theL2,
                                         CORBA::Double theH, CORBA::Double theW, CORBA::Boolean theHexMesh);
  GEOM::ListOfGO* MakePipeTShapeChamferWithPosition (CORBA::Double theR1, CORBA::Double theW1, CORBA::Double theL1,
                                                     CORBA::Double theR2, CORBA::Double theW2, CORBA::Double theL2,
                                                     CORBA::Double theH, CORBA::Double theW, CORBA::Boolean theHexMesh,
                                                     GEOM::GEOM_Object_ptr theP1, GEOM::GEOM_Object_ptr theP2, GEOM::GEOM_Object_ptr theP3);
  GEOM::ListOfGO* MakePipeTShapeFillet (CORBA::Double theR1, CORBA::Double theW1, CORBA::Double theL1,
                                        CORBA::Double theR2, CORBA::Double theW2, CORBA::Double theL2,
                                        CORBA::Double theRF, CORBA::Boolean theHexMesh);
  GEOM::ListOfGO* MakePipeTShapeFilletWithPosition (CORBA::Double theR1, CORBA::Double theW1, CORBA::Double theL1,
                                                    CORBA::Double theR2, CORBA::Double theW2, CORBA::Double theL2,
                                                    CORBA::Double theRF, CORBA::Boolean theHexMesh,
                                                    GEOM::GEOM_Object_ptr theP1, GEOM::GEOM_Object_ptr theP2, GEOM::GEOM_Object_ptr theP3);

  // PipeTShape with thickness reduction
  GEOM::ListOfGO* MakePipeTShapeTR
    (CORBA::Double theR1, CORBA::Double theW1, CORBA::Double theL1,
     CORBA::Double theR2, CORBA::Double theW2, CORBA::Double theL2,
     CORBA::Double theRL, CORBA::Double theWL, CORBA::Double theLtransL, CORBA::Double theLthinL,
     CORBA::Double theRR, CORBA::Double theWR, CORBA::Double theLtransR, CORBA::Double theLthinR,
     CORBA::Double theRI, CORBA::Double theWI, CORBA::Double theLtransI, CORBA::Double theLthinI,
     CORBA::Boolean theHexMesh);
  GEOM::ListOfGO* MakePipeTShapeTRWithPosition
    (CORBA::Double theR1, CORBA::Double theW1, CORBA::Double theL1,
     CORBA::Double theR2, CORBA::Double theW2, CORBA::Double theL2,
     CORBA::Double theRL, CORBA::Double theWL, CORBA::Double theLtransL, CORBA::Double theLthinL,
     CORBA::Double theRR, CORBA::Double theWR, CORBA::Double theLtransR, CORBA::Double theLthinR,
     CORBA::Double theRI, CORBA::Double theWI, CORBA::Double theLtransI, CORBA::Double theLthinI,
     CORBA::Boolean theHexMesh,
     GEOM::GEOM_Object_ptr theP1, GEOM::GEOM_Object_ptr theP2, GEOM::GEOM_Object_ptr theP3);
  GEOM::ListOfGO* MakePipeTShapeTRChamfer
    (CORBA::Double theR1, CORBA::Double theW1, CORBA::Double theL1,
     CORBA::Double theR2, CORBA::Double theW2, CORBA::Double theL2,
     CORBA::Double theRL, CORBA::Double theWL, CORBA::Double theLtransL, CORBA::Double theLthinL,
     CORBA::Double theRR, CORBA::Double theWR, CORBA::Double theLtransR, CORBA::Double theLthinR,
     CORBA::Double theRI, CORBA::Double theWI, CORBA::Double theLtransI, CORBA::Double theLthinI,
     CORBA::Double theH, CORBA::Double theW, CORBA::Boolean theHexMesh);
  GEOM::ListOfGO* MakePipeTShapeTRChamferWithPosition
    (CORBA::Double theR1, CORBA::Double theW1, CORBA::Double theL1,
     CORBA::Double theR2, CORBA::Double theW2, CORBA::Double theL2,
     CORBA::Double theRL, CORBA::Double theWL, CORBA::Double theLtransL, CORBA::Double theLthinL,
     CORBA::Double theRR, CORBA::Double theWR, CORBA::Double theLtransR, CORBA::Double theLthinR,
     CORBA::Double theRI, CORBA::Double theWI, CORBA::Double theLtransI, CORBA::Double theLthinI,
     CORBA::Double theH, CORBA::Double theW, CORBA::Boolean theHexMesh,
     GEOM::GEOM_Object_ptr theP1, GEOM::GEOM_Object_ptr theP2, GEOM::GEOM_Object_ptr theP3);
  GEOM::ListOfGO* MakePipeTShapeTRFillet
    (CORBA::Double theR1, CORBA::Double theW1, CORBA::Double theL1,
     CORBA::Double theR2, CORBA::Double theW2, CORBA::Double theL2,
     CORBA::Double theRL, CORBA::Double theWL, CORBA::Double theLtransL, CORBA::Double theLthinL,
     CORBA::Double theRR, CORBA::Double theWR, CORBA::Double theLtransR, CORBA::Double theLthinR,
     CORBA::Double theRI, CORBA::Double theWI, CORBA::Double theLtransI, CORBA::Double theLthinI,
     CORBA::Double theRF, CORBA::Boolean theHexMesh);
  GEOM::ListOfGO* MakePipeTShapeTRFilletWithPosition
    (CORBA::Double theR1, CORBA::Double theW1, CORBA::Double theL1,
     CORBA::Double theR2, CORBA::Double theW2, CORBA::Double theL2,
     CORBA::Double theRL, CORBA::Double theWL, CORBA::Double theLtransL, CORBA::Double theLthinL,
     CORBA::Double theRR, CORBA::Double theWR, CORBA::Double theLtransR, CORBA::Double theLthinR,
     CORBA::Double theRI, CORBA::Double theWI, CORBA::Double theLtransI, CORBA::Double theLthinI,
     CORBA::Double theRF, CORBA::Boolean theHexMesh,
     GEOM::GEOM_Object_ptr theP1, GEOM::GEOM_Object_ptr theP2, GEOM::GEOM_Object_ptr theP3);

  GEOM::GEOM_Object_ptr MakeDividedDisk        (CORBA::Double theR,
                                                CORBA::Double theRatio,
                                                CORBA::Short theOrientation,
                                                GEOM::pattern thePattern);

  GEOM::GEOM_Object_ptr MakeDividedDiskPntVecR (GEOM::GEOM_Object_ptr theCenter,
                                                GEOM::GEOM_Object_ptr theVector,
                                                CORBA::Double theR,
                                                CORBA::Double theRatio,
                                                GEOM::pattern thePattern);

  GEOM::GEOM_Object_ptr MakeDividedCylinder (CORBA::Double theR,
                                             CORBA::Double theH,
                                             GEOM::pattern thePattern);
  GEOM::GEOM_Object_ptr MakeSmoothingSurface (const GEOM::ListOfGO& thelPoints,
                                              CORBA::Long           theNbMax,
                                              CORBA::Long           theDegMax,
                                              CORBA::Double         theDMax);

  AdvancedEngine_IOperations* GetOperations();
};

#endif
