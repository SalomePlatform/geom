//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#ifndef _GEOM_Object_i_HeaderFile
#define _GEOM_Object_i_HeaderFile

#include "GEOM_GEOM_I.hxx"

#include "GEOMImpl_Gen.hxx"

#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(SALOMEDS)
#include CORBA_SERVER_HEADER(GEOM_Gen)

#include "SALOME_GenericObj_i.hh"

#include "GEOM_Object.hxx"

#include <TopoDS_Shape.hxx>

class GEOM_I_EXPORT GEOM_Object_i : public virtual POA_GEOM::GEOM_Object, public virtual SALOME::GenericObj_i
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

  virtual void SetColor(const SALOMEDS::Color& theColor);

  virtual SALOMEDS::Color GetColor();

  virtual void SetAutoColor(CORBA::Boolean theAutoColor);

  virtual CORBA::Boolean GetAutoColor();

  virtual void SetStudyEntry(const char* theEntry);

  virtual char* GetStudyEntry();

  virtual GEOM::ListOfGO* GetDependency();

  virtual GEOM::ListOfGO* GetLastDependency();

  virtual SALOMEDS::TMPFile* GetShapeStream();

  virtual CORBA::LongLong getShape();

  virtual bool IsMainShape() { return _impl->IsMainShape(); }

  virtual GEOM::ListOfLong* GetSubShapeIndices();

  virtual GEOM::GEOM_Object_ptr GetMainShape();

  virtual bool IsShape();

  virtual void SetParameters(const char* theParameters);

  virtual char* GetParameters();

  Handle(GEOM_Object) GetImpl() { return _impl; }

 private:

  GEOM::GEOM_Gen_var _engine;
  Handle(GEOM_Object) _impl;
  TopoDS_Shape _geom;
};

#endif
