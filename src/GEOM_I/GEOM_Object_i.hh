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

#ifndef _GEOM_Object_i_HeaderFile
#define _GEOM_Object_i_HeaderFile

#include "GEOMImpl_Gen.hxx"
#include "GEOM_BaseObject_i.hh"
#include "GEOM_Object.hxx"

#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(SALOMEDS)
#include CORBA_SERVER_HEADER(GEOM_Gen)

#include <TopoDS_Shape.hxx>

class GEOM_I_EXPORT GEOM_Object_i : public virtual POA_GEOM::GEOM_Object, public virtual GEOM_BaseObject_i
{
 public:
   GEOM_Object_i(PortableServer::POA_ptr thePOA, GEOM::GEOM_Gen_ptr theEngine, HANDLE_NAMESPACE(GEOM_Object) theImpl);
   ~GEOM_Object_i();

  virtual GEOM::shape_type GetShapeType();

  virtual GEOM::shape_type GetTopologyType();

  virtual GEOM::shape_type GetMinShapeType();

  virtual GEOM::shape_type GetMaxShapeType();

  virtual char* GetSubShapeName(CORBA::Long subID);

  virtual void SetColor(const SALOMEDS::Color& theColor);

  virtual SALOMEDS::Color GetColor();

  virtual void SetAutoColor(CORBA::Boolean theAutoColor);

  virtual CORBA::Boolean GetAutoColor();

  virtual void SetMarkerStd(GEOM::marker_type theType, GEOM::marker_size theSize);

  virtual void SetMarkerTexture(CORBA::Long theTextureId);

  virtual GEOM::marker_type GetMarkerType();

  virtual GEOM::marker_size GetMarkerSize();

  virtual CORBA::Long GetMarkerTexture();

  virtual SALOMEDS::TMPFile* GetShapeStream();

  virtual CORBA::LongLong getShape();

  virtual bool IsMainShape() { return _impl->IsMainShape(); }

  virtual GEOM::ListOfLong* GetSubShapeIndices();

  virtual GEOM::GEOM_Object_ptr GetMainShape();

  virtual bool IsSame(GEOM::GEOM_BaseObject_ptr other);

  virtual bool IsShape();

  HANDLE_NAMESPACE(GEOM_Object) GetImpl() { return _impl; }

 private:

  HANDLE_NAMESPACE(GEOM_Object) _impl;
  TopoDS_Shape _geom;
};

#endif
