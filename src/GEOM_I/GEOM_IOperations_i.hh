
#ifndef _GEOM_IOperations_i_HeaderFile
#define _GEOM_IOperations_i_HeaderFile


#include <SALOMEconfig.h>

#include CORBA_SERVER_HEADER(GEOM_Gen)
#include "SALOME_GenericObj_i.hh" 
#include "GEOM_IOperations.hxx"
#include "GEOM_Object_i.hh" 
#include "GEOM_Object.hxx"

class GEOM_IOperations_i : public virtual POA_GEOM::GEOM_IOperations, public virtual SALOME::GenericObj_i
{
 public:
  GEOM_IOperations_i(PortableServer::POA_ptr thePOA, GEOM::GEOM_Gen_ptr theEngine, ::GEOM_IOperations* theImpl);
  ~GEOM_IOperations_i();
  
  virtual CORBA::Boolean IsDone();
  
  virtual void SetErrorCode(const  char* theErrorCode);
  
  virtual char* GetErrorCode();

  virtual CORBA::Long GetStudyID();    

  virtual GEOM::GEOM_Object_ptr GetObject(Handle(GEOM_Object) theObject);

  virtual void StartOperation();

  virtual void FinishOperation();

  virtual void AbortOperation();


   ::GEOM_IOperations* GetImpl() { return _impl; }

 private:
 
  ::GEOM_IOperations* _impl;
  GEOM::GEOM_Gen_ptr _engine;     
};

#endif
