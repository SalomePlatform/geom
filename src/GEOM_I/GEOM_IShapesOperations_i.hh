
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
				  const CORBA::Boolean  isPlanarWanted);

  GEOM::GEOM_Object_ptr MakeFaceWires (const GEOM::ListOfGO& theWires,
				       const CORBA::Boolean  isPlanarWanted);

  GEOM::GEOM_Object_ptr MakeShell (const GEOM::ListOfGO& theFacesAndShells);

  GEOM::GEOM_Object_ptr MakeSolidShell (GEOM::GEOM_Object_ptr theShell);

  GEOM::GEOM_Object_ptr MakeSolidShells (const GEOM::ListOfGO& theShells);

  GEOM::GEOM_Object_ptr MakeCompound (const GEOM::ListOfGO& theShapes);

  GEOM::GEOM_Object_ptr MakeGlueFaces (GEOM::GEOM_Object_ptr theShape,
				       const CORBA::Double   theTolerance);

  GEOM::ListOfGO* MakeExplode (GEOM::GEOM_Object_ptr theShape,
			       const CORBA::Long     theShapeType,
			       const CORBA::Boolean  isSorted);

  GEOM::ListOfLong* SubShapeAllIDs (GEOM::GEOM_Object_ptr theShape,
				    const CORBA::Long     theShapeType,
				    const CORBA::Boolean  isSorted);

  GEOM::GEOM_Object_ptr GetSubShape (GEOM::GEOM_Object_ptr theMainShape,
				     const CORBA::Long     theID);

  CORBA::Long NumberOfFaces (GEOM::GEOM_Object_ptr theShape);
  CORBA::Long NumberOfEdges (GEOM::GEOM_Object_ptr theShape);

  GEOM::GEOM_Object_ptr ChangeOrientation (GEOM::GEOM_Object_ptr theShape);

  GEOM::ListOfLong* GetFreeFacesIDs (GEOM::GEOM_Object_ptr theShape);

  GEOM::ListOfGO* GetSharedShapes (GEOM::GEOM_Object_ptr theShape1,
				   GEOM::GEOM_Object_ptr theShape2,
				   const CORBA::Long     theShapeType);

  GEOM::GEOM_Object_ptr GetShapesOnPlane (GEOM::GEOM_Object_ptr theShape,
					  const CORBA::Long     theShapeType,
					  GEOM::GEOM_Object_ptr thePlane);

  GEOM::GEOM_Object_ptr GetShapesOnCylinder (GEOM::GEOM_Object_ptr theShape,
					     const CORBA::Long     theShapeType,
					     GEOM::GEOM_Object_ptr theAxis,
					     const CORBA::Double   theRadius);

  GEOM::GEOM_Object_ptr GetShapesOnSphere (GEOM::GEOM_Object_ptr theShape,
					   const CORBA::Long     theShapeType,
					   GEOM::GEOM_Object_ptr theCenter,
					   const CORBA::Double   theRadius);

  GEOM::GEOM_Object_ptr GetInPlace (GEOM::GEOM_Object_ptr theShapeWhere,
				    GEOM::GEOM_Object_ptr theShapeWhat);

  ::GEOMImpl_IShapesOperations* GetOperations()
  { return (::GEOMImpl_IShapesOperations*)GetImpl(); }
};

#endif
