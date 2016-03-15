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
// File : BREPPlugin_IOperations.hxx

#ifndef _BREPPlugin_IOperations_HXX_
#define _BREPPlugin_IOperations_HXX_

// internal includes
#include "BREPPlugin_Engine.hxx"

// GEOM includes
#include "GEOMImpl_IBaseIEOperations.hxx"

class BREPPLUGINENGINE_EXPORT BREPPlugin_IOperations: public GEOMImpl_IBaseIEOperations
{
public:
  BREPPlugin_IOperations( GEOM_Engine*, int );
  ~BREPPlugin_IOperations();

  void ExportBREP( const Handle(GEOM_Object),
		   const TCollection_AsciiString& );
  
  Handle(TColStd_HSequenceOfTransient) ImportBREP( const TCollection_AsciiString& );
};

#endif
