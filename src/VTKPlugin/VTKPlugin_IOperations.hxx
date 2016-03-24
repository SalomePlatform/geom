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
// File : VTKPlugin_IOperations.hxx

#ifndef _VTKPlugin_IOperations_HXX_
#define _VTKPlugin_IOperations_HXX_

// internal includes
#include "VTKPlugin_Engine.hxx"

// GEOM includes
#include "GEOMImpl_IBaseIEOperations.hxx"

class VTKPLUGINENGINE_EXPORT VTKPlugin_IOperations: public GEOMImpl_IBaseIEOperations
{
public:
  VTKPlugin_IOperations( GEOM_Engine*, int );
  ~VTKPlugin_IOperations();

  void ExportVTK( const Handle(GEOM_Object),
		  const TCollection_AsciiString&,
		  const double );
};

#endif
