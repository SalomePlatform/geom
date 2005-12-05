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
