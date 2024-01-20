// Copyright (C) 2014-2024  CEA, EDF, OPEN CASCADE
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

#ifndef _STEPPlugin_IECallBack_HXX_
#define _STEPPlugin_IECallBack_HXX_

// internal includes
#include "STEPPlugin_Engine.hxx"

// GEOM includes
#include "GEOM_Object.hxx"
#include "GEOMImpl_IECallBack.hxx"

// OCC includes
#include <TCollection_AsciiString.hxx>

class STEPPLUGINENGINE_EXPORT STEPPlugin_IECallBack : public GEOMImpl_IECallBack
{
public:
  STEPPlugin_IECallBack();
  ~STEPPlugin_IECallBack();
  
  bool Export( const Handle(GEOM_Object)      theOriginal,
		       const TCollection_AsciiString& theFileName,
		       const TCollection_AsciiString& theFormatName );

  Handle(TColStd_HSequenceOfTransient) Import( const TCollection_AsciiString& theFormatName,
                                               const TCollection_AsciiString& theFileName );

  TCollection_AsciiString  ReadValue( const TCollection_AsciiString& theFileName,
                                      const TCollection_AsciiString& theFormatName,
                                      const TCollection_AsciiString& theParameterName );
};

#endif
