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

#ifndef _GEOM_BaseObject_i_HeaderFile
#define _GEOM_BaseObject_i_HeaderFile

#include "GEOM_GEOM_I.hxx"

#include "GEOMImpl_Gen.hxx"

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(GEOM_Gen)

#include "SALOME_GenericObj_i.hh"

#include "GEOM_BaseObject.hxx"

class GEOM_I_EXPORT GEOM_BaseObject_i : public virtual POA_GEOM::GEOM_BaseObject, public virtual SALOME::GenericObj_i
{
 public:
  GEOM_BaseObject_i(PortableServer::POA_ptr thePOA, GEOM::GEOM_Gen_ptr theEngine, HANDLE_NAMESPACE(GEOM_BaseObject) theImpl);
  ~GEOM_BaseObject_i();

  virtual char* GetEntry();

  virtual CORBA::Long GetStudyID();

  virtual CORBA::Long GetType();

  virtual CORBA::Long GetTick();

  virtual void SetName(const char* theName);

  virtual char* GetName();

  virtual void SetStudyEntry(const char* theEntry);

  virtual char* GetStudyEntry();

  virtual GEOM::ListOfGBO* GetDependency();

  virtual GEOM::ListOfGBO* GetLastDependency();

  virtual bool IsSame(GEOM::GEOM_BaseObject_ptr other);

  virtual void SetParameters(const char* theParameters);

  virtual char* GetParameters();

  virtual GEOM::CreationInformationSeq* GetCreationInformation();

  HANDLE_NAMESPACE(GEOM_BaseObject) GetImpl() { return _impl; }

 protected:

  GEOM::GEOM_Gen_var      _engine;

 private:

  HANDLE_NAMESPACE(GEOM_BaseObject) _impl;
};

#endif
