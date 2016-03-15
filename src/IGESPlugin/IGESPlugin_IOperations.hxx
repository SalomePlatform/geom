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
// File : IGESPlugin_IOperations.hxx

#ifndef _IGESPlugin_IOperations_HXX_
#define _IGESPlugin_IOperations_HXX_

// internal includes
#include "IGESPlugin_Engine.hxx"

// GEOM includes
#include "GEOMImpl_IBaseIEOperations.hxx"

class IGESPLUGINENGINE_EXPORT IGESPlugin_IOperations: public GEOMImpl_IBaseIEOperations
{
public:
  IGESPlugin_IOperations( GEOM_Engine*, int );
  ~IGESPlugin_IOperations();

  void ExportIGES( const Handle(GEOM_Object),
		   const TCollection_AsciiString&,
		   const TCollection_AsciiString& );
  
  Handle(TColStd_HSequenceOfTransient) ImportIGES( const TCollection_AsciiString&,
						   const bool );
  
  TCollection_AsciiString ReadValue( const TCollection_AsciiString&,
				     const TCollection_AsciiString& );
};

#endif
