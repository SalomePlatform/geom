
#ifndef _GEOM_IGroupOperations_i_HeaderFile
#define _GEOM_IGroupOperations_i_HeaderFile


#include <SALOMEconfig.h>

#include CORBA_SERVER_HEADER(GEOM_Gen)
#include "GEOM_IOperations_i.hh"
#include "GEOM_Object_i.hh"

#include "GEOMImpl_IGroupOperations.hxx"

class GEOM_IGroupOperations_i : 
    public virtual POA_GEOM::GEOM_IGroupOperations,
    public virtual GEOM_IOperations_i
{
 public:
  GEOM_IGroupOperations_i (PortableServer::POA_ptr thePOA, GEOM::GEOM_Gen_ptr theEngine,
			   ::GEOMImpl_IGroupOperations* theImpl);
  ~GEOM_IGroupOperations_i();

  GEOM::GEOM_Object_ptr CreateGroup  (GEOM::GEOM_Object_ptr theMainShape, CORBA::Long theShapeType);
 
  void AddObject (GEOM::GEOM_Object_ptr theGroup, CORBA::Long theSubShapeId);

  void RemoveObject (GEOM::GEOM_Object_ptr theGroup, CORBA::Long theSubShapeId);

  CORBA::Long GetType (GEOM::GEOM_Object_ptr theGroup);
  
  GEOM::GEOM_Object_ptr GetMainShape (GEOM::GEOM_Object_ptr theGroup);

  GEOM::ListOfLong* GetObjects (GEOM::GEOM_Object_ptr theGroup);

  ::GEOMImpl_IGroupOperations* GetOperations()
  { return (::GEOMImpl_IGroupOperations*)GetImpl(); }
};

#endif
