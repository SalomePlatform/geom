// Copyright (C) 2014-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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
// File : XAOPlugin_IOperations.hxx

#ifndef _XAOPlugin_IOperations_HXX_
#define _XAOPlugin_IOperations_HXX_

// internal includes
#include "XAOPlugin_Engine.hxx"

// GEOM includes
#include "GEOMImpl_IBaseIEOperations.hxx"
#include "GEOM_Object.hxx"
#include "GEOM_Field.hxx"

#include <list>

namespace XAO {
  class Geometry;
  class BrepGeometry;
  class Xao;
}

class XAOPLUGINENGINE_EXPORT XAOPlugin_IOperations: public GEOMImpl_IBaseIEOperations
{
public:
  XAOPlugin_IOperations( GEOM_Engine*, int );
  ~XAOPlugin_IOperations();

  bool ExportXAO( Handle(GEOM_Object) shape,
                  std::list<Handle(GEOM_Object)> groupList,
                  std::list<Handle(GEOM_Field)> fieldList,
                  const char* author,
                  const char* fileName );
  
  bool ImportXAO( const char* fileName,
                  Handle(GEOM_Object)& shape,
                  Handle(TColStd_HSequenceOfTransient)& subShapes,
                  Handle(TColStd_HSequenceOfTransient)& groups,
                  Handle(TColStd_HSequenceOfTransient)& fields );

private:
  void importSubShapes( XAO::Geometry* xaoGeometry,
                        Handle(GEOM_Function) function,
                        int shapeType,
                        int dim,
                        Handle(TColStd_HSequenceOfTransient)& subshapeList );
  void exportSubshapes( const Handle(GEOM_Object)& shape,
                        XAO::BrepGeometry* geometry );
  void exportFields( std::list<Handle(GEOM_Field)> fieldList,
                     XAO::Xao* xaoObject,
                     XAO::BrepGeometry* geometry );
  bool exportGroups( std::list<Handle(GEOM_Object)> groupList,
                     XAO::Xao* xaoObject,
                     XAO::BrepGeometry* geometry );
};

#endif
