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

#ifndef _GEOMImpl_IECallBack_HXX_
#define _GEOMImpl_IECallBack_HXX_

// GEOM includes
#include "GEOM_Object.hxx"

// OCC includes
#include <TCollection_AsciiString.hxx>

// C++ includes
#include <map>

class GEOMImpl_Gen;

class GEOMImpl_IECallBack
{
 public:
  Standard_EXPORT GEOMImpl_IECallBack();
  Standard_EXPORT ~GEOMImpl_IECallBack();
  
  Standard_EXPORT virtual bool Export( int                            theDocId,
				       const Handle(GEOM_Object)      theOriginal,
                                       const TCollection_AsciiString& theFileName,
                                       const TCollection_AsciiString& theFormatName );

  Standard_EXPORT virtual
  Handle(TColStd_HSequenceOfTransient) Import( int                            theDocId,
					       const TCollection_AsciiString& theFormatName,
					       const TCollection_AsciiString& theFileName );

  Standard_EXPORT virtual
  TCollection_AsciiString  ReadValue( int                            theDocId,
				      const TCollection_AsciiString& theFileName,
				      const TCollection_AsciiString& theFormatName,
				      const TCollection_AsciiString& theParameterName );

  Standard_EXPORT static void Register( const TCollection_AsciiString& theFormatName, GEOMImpl_IECallBack* theCallBack );

  static GEOMImpl_IECallBack* GetCallBack( const TCollection_AsciiString& theFormatName );

protected:
  Standard_EXPORT GEOMImpl_Gen* GetEngine();

private:
  static std::map<TCollection_AsciiString, GEOMImpl_IECallBack*> myCallBacks;
};

#endif
