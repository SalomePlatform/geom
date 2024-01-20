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

#ifndef _STLPlugin_IImport_HXX_
#define _STLPlugin_IImport_HXX_

#include "GEOM_Function.hxx"

#define IMPORTSTL_ARG_FILENAME 1

class STLPlugin_IImport
{
public:
  STLPlugin_IImport( Handle(GEOM_Function) theFunction)
    : _func(theFunction) {}

  void SetFileName( const TCollection_AsciiString& theFileName )
    { _func->SetString( IMPORTSTL_ARG_FILENAME, theFileName ); }
  TCollection_AsciiString GetFileName()
    { return _func->GetString( IMPORTSTL_ARG_FILENAME ); }
  
private:
  Handle(GEOM_Function) _func;
};

#endif // _STLPlugin_IImport_HXX_
