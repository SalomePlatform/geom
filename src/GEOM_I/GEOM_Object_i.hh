
#ifndef _GEOM_Object_i_HeaderFile
#define _GEOM_Object_i_HeaderFile


#include <SALOMEconfig.h>

#include CORBA_SERVER_HEADER(SALOMEDS)
#include CORBA_SERVER_HEADER(GEOM_Gen)

#include "SALOME_GenericObj_i.hh"
#include "GEOM_Object.hxx"
#include <TopoDS_Shape.hxx>

class GEOM_Object_i : public virtual POA_GEOM::GEOM_Object, public virtual SALOME::GenericObj_i
{
 public:
   GEOM_Object_i(PortableServer::POA_ptr thePOA, GEOM::GEOM_Gen_ptr theEngine, Handle(GEOM_Object) theImpl);
   ~GEOM_Object_i();
  
  virtual char* GetEntry();
  
  virtual CORBA::Long GetStudyID();
  
  virtual CORBA::Long GetType();

  virtual GEOM::shape_type GetShapeType(); 

  virtual void SetName(const char* theName);

  virtual char* GetName();    

  virtual void SetStudyEntry(const char* theEntry);

  virtual char* GetStudyEntry();    

  virtual GEOM::ListOfGO* GetDependency();

  virtual GEOM::ListOfGO* GetLastDependency();

  virtual SALOMEDS::TMPFile* GetShapeStream();

  long getShape();

  virtual bool IsMainShape() { return _impl->IsMainShape(); }

  virtual GEOM::ListOfLong* GetSubShapeIndices(); 
 
  virtual GEOM::GEOM_Object_ptr GetMainShape();

  virtual bool IsShape();   

  Handle(GEOM_Object) GetImpl() { return _impl; }

 private:

  GEOM::GEOM_Gen_ptr _engine; 
  Handle(GEOM_Object) _impl;     
  TopoDS_Shape _geom;
};

#endif
