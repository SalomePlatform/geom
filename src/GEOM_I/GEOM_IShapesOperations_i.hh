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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef _GEOM_IShapesOperations_i_HeaderFile
#define _GEOM_IShapesOperations_i_HeaderFile


#include <SALOMEconfig.h>

#include CORBA_SERVER_HEADER(GEOM_Gen)
#include "GEOM_IOperations_i.hh"
#include "GEOM_Object_i.hh"

#include "GEOMImpl_IShapesOperations.hxx"

class GEOM_IShapesOperations_i :
    public virtual POA_GEOM::GEOM_IShapesOperations,
    public virtual GEOM_IOperations_i
{
 public:
  GEOM_IShapesOperations_i (PortableServer::POA_ptr       thePOA,
			    GEOM::GEOM_Gen_ptr            theEngine,
			    ::GEOMImpl_IShapesOperations* theImpl);
  ~GEOM_IShapesOperations_i();

  GEOM::GEOM_Object_ptr MakeEdge (GEOM::GEOM_Object_ptr thePnt1,
				  GEOM::GEOM_Object_ptr thePnt2);

  GEOM::GEOM_Object_ptr MakeWire (const GEOM::ListOfGO& theEdgesAndWires);

  GEOM::GEOM_Object_ptr MakeFace (GEOM::GEOM_Object_ptr theWire,
				  CORBA::Boolean  isPlanarWanted);

  GEOM::GEOM_Object_ptr MakeFaceWires (const GEOM::ListOfGO& theWires,
				       CORBA::Boolean  isPlanarWanted);

  GEOM::GEOM_Object_ptr MakeShell (const GEOM::ListOfGO& theFacesAndShells);

  GEOM::GEOM_Object_ptr MakeSolidShell (GEOM::GEOM_Object_ptr theShell);

  GEOM::GEOM_Object_ptr MakeSolidShells (const GEOM::ListOfGO& theShells);

  GEOM::GEOM_Object_ptr MakeCompound (const GEOM::ListOfGO& theShapes);

  GEOM::GEOM_Object_ptr MakeGlueFaces (GEOM::GEOM_Object_ptr theShape,
				       CORBA::Double         theTolerance);

  GEOM::ListOfGO* MakeExplode (GEOM::GEOM_Object_ptr theShape,
			       CORBA::Long           theShapeType,
			       CORBA::Boolean  isSorted);

  GEOM::ListOfLong* SubShapeAllIDs (GEOM::GEOM_Object_ptr theShape,
				    CORBA::Long           theShapeType,
				    CORBA::Boolean  isSorted);

  GEOM::GEOM_Object_ptr GetSubShape (GEOM::GEOM_Object_ptr theMainShape,
				     CORBA::Long           theID);

  CORBA::Long GetSubShapeIndex (GEOM::GEOM_Object_ptr theMainShape,
				GEOM::GEOM_Object_ptr theSubShape);

  CORBA::Long GetTopologyIndex (GEOM::GEOM_Object_ptr theMainShape,
				GEOM::GEOM_Object_ptr theSubShape);

  char* GetShapeTypeString (GEOM::GEOM_Object_ptr theShape);

  CORBA::Long NumberOfFaces (GEOM::GEOM_Object_ptr theShape);
  CORBA::Long NumberOfEdges (GEOM::GEOM_Object_ptr theShape);

  GEOM::GEOM_Object_ptr ChangeOrientation (GEOM::GEOM_Object_ptr theShape);

  GEOM::ListOfLong* GetFreeFacesIDs (GEOM::GEOM_Object_ptr theShape);

  GEOM::ListOfGO* GetSharedShapes (GEOM::GEOM_Object_ptr theShape1,
				   GEOM::GEOM_Object_ptr theShape2,
				   CORBA::Long           theShapeType);

  GEOM::ListOfGO* GetShapesOnPlane (GEOM::GEOM_Object_ptr theShape,
				    CORBA::Long           theShapeType,
				    GEOM::GEOM_Object_ptr theAx1,
				    GEOM::shape_state     theState);

  GEOM::ListOfGO* GetShapesOnCylinder (GEOM::GEOM_Object_ptr theShape,
				       CORBA::Long           theShapeType,
				       GEOM::GEOM_Object_ptr theAxis,
				       CORBA::Double         theRadius,
				       GEOM::shape_state     theState);

  GEOM::ListOfGO* GetShapesOnSphere (GEOM::GEOM_Object_ptr theShape,
				     CORBA::Long           theShapeType,
				     GEOM::GEOM_Object_ptr theCenter,
				     CORBA::Double         theRadius,
				     GEOM::shape_state     theState);

  GEOM::ListOfGO* GetShapesOnQuadrangle (GEOM::GEOM_Object_ptr theShape,
                                         CORBA::Long           theShapeType,
                                         GEOM::GEOM_Object_ptr theTopLeftPoint,
                                         GEOM::GEOM_Object_ptr theTopRigthPoint,
                                         GEOM::GEOM_Object_ptr theBottomLeftPoint,
                                         GEOM::GEOM_Object_ptr theBottomRigthPoint,
                                         GEOM::shape_state     theState);

  GEOM::ListOfLong* GetShapesOnPlaneIDs (GEOM::GEOM_Object_ptr theShape,
					 CORBA::Long           theShapeType,
					 GEOM::GEOM_Object_ptr theAx1,
					 GEOM::shape_state     theState);

  GEOM::ListOfLong* GetShapesOnCylinderIDs (GEOM::GEOM_Object_ptr theShape,
					    CORBA::Long           theShapeType,
					    GEOM::GEOM_Object_ptr theAxis,
					    CORBA::Double         theRadius,
					    GEOM::shape_state     theState);

  GEOM::ListOfLong* GetShapesOnSphereIDs (GEOM::GEOM_Object_ptr theShape,
					  CORBA::Long           theShapeType,
					  GEOM::GEOM_Object_ptr theCenter,
					  CORBA::Double         theRadius,
					  GEOM::shape_state     theState);

  GEOM::ListOfLong* GetShapesOnQuadrangleIDs (GEOM::GEOM_Object_ptr theShape,
                                              CORBA::Long           theShapeType,
                                              GEOM::GEOM_Object_ptr theTopLeftPoint,
                                              GEOM::GEOM_Object_ptr theTopRigthPoint,
                                              GEOM::GEOM_Object_ptr theBottomLeftPoint,
                                              GEOM::GEOM_Object_ptr theBottomRigthPoint,
                                              GEOM::shape_state     theState);

  GEOM::GEOM_Object_ptr GetInPlace (GEOM::GEOM_Object_ptr theShapeWhere,
				    GEOM::GEOM_Object_ptr theShapeWhat);

  ::GEOMImpl_IShapesOperations* GetOperations()
  { return (::GEOMImpl_IShapesOperations*)GetImpl(); }
};

#endif
