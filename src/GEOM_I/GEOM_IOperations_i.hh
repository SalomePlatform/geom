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

#ifndef _GEOM_IOperations_i_HeaderFile
#define _GEOM_IOperations_i_HeaderFile

#include "GEOMImpl_Gen.hxx"

#include <SALOMEconfig.h>

#include CORBA_SERVER_HEADER(GEOM_Gen)
#include "SALOME_GenericObj_i.hh" 
#include "GEOM_IOperations.hxx"
#include "GEOM_Object_i.hh" 
#include "GEOM_Object.hxx"

class GEOM_I_EXPORT GEOM_IOperations_i : public virtual POA_GEOM::GEOM_IOperations, public virtual SALOME::GenericObj_i
{
 public:
  GEOM_IOperations_i(PortableServer::POA_ptr thePOA, GEOM::GEOM_Gen_ptr theEngine, ::GEOM_IOperations* theImpl);
  ~GEOM_IOperations_i();
  
  virtual CORBA::Boolean IsDone();
  
  virtual void SetErrorCode(const  char* theErrorCode);
  
  virtual char* GetErrorCode();

  virtual CORBA::Long GetStudyID();    

  virtual GEOM::GEOM_BaseObject_ptr GetBaseObject(HANDLE_NAMESPACE(GEOM_BaseObject) theObject);
  virtual HANDLE_NAMESPACE(GEOM_BaseObject) GetBaseObjectImpl(GEOM::GEOM_BaseObject_ptr theObject);
  virtual GEOM::GEOM_Object_ptr GetObject(HANDLE_NAMESPACE(GEOM_Object) theObject);
  virtual HANDLE_NAMESPACE(GEOM_Object) GetObjectImpl(GEOM::GEOM_Object_ptr theObject);

  virtual void StartOperation();

  virtual void FinishOperation();

  virtual void AbortOperation();


   ::GEOM_IOperations* GetImpl() { return _impl; }

  virtual void UpdateGUIForObject(GEOM::GEOM_Object_ptr theObj);

protected:

  Handle(TColStd_HSequenceOfTransient)
               GetListOfObjectsImpl(const GEOM::ListOfGO& theObjects);
  bool GetListOfObjectsImpl(const GEOM::ListOfGO& theObjects,
                            std::list< HANDLE_NAMESPACE(GEOM_Object) >& theList);

  Handle(TColStd_HArray1OfExtendedString)
               ConvertStringArray(const GEOM::string_array &theInArray);

 private:
 
  ::GEOM_IOperations* _impl;
  GEOM::GEOM_Gen_var _engine;     
};

#endif
