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

#ifndef _STLPlugin_IExport_HXX_
#define _STLPlugin_IExport_HXX_

#include "GEOM_Function.hxx"

#define EXPORTSTL_ARG_ORIGINAL   1
#define EXPORTSTL_ARG_FILENAME   2
#define EXPORTSTL_ARG_DEFLECTION 3
#define EXPORTSTL_ARG_ISASCII    4
#define EXPORTSTL_ARG_ISRELATIVE 5

class STLPlugin_IExport
{
public:
  STLPlugin_IExport( Handle(GEOM_Function) theFunction )
    : _func(theFunction) {}

  void SetOriginal( Handle( GEOM_Function ) theOriginal)
    { _func->SetReference( EXPORTSTL_ARG_ORIGINAL, theOriginal ); }
  Handle( GEOM_Function ) GetOriginal()
    { return _func->GetReference( EXPORTSTL_ARG_ORIGINAL ); }

  void SetFileName( const TCollection_AsciiString& theFileName )
    { _func->SetString( EXPORTSTL_ARG_FILENAME, theFileName ); }
  TCollection_AsciiString GetFileName()
    { return _func->GetString( EXPORTSTL_ARG_FILENAME ); }
  
  void SetIsASCII( bool theIsASCII )
    { _func->SetInteger( EXPORTSTL_ARG_ISASCII, int( theIsASCII ) ); }
  bool GetIsASCII()
    { return bool( _func->GetInteger( EXPORTSTL_ARG_ISASCII ) ); }

  void SetIsRelative( bool theIsRelative )
    { _func->SetInteger( EXPORTSTL_ARG_ISRELATIVE, int( theIsRelative ) ); }
  bool GetIsRelative()
    { return bool( _func->GetInteger( EXPORTSTL_ARG_ISRELATIVE ) ); }

  void SetDeflection( double theDeflection )
    { _func->SetReal( EXPORTSTL_ARG_DEFLECTION, theDeflection ); }
  double GetDeflection()
    { return _func->GetReal( EXPORTSTL_ARG_DEFLECTION ); }
  
private:
  Handle(GEOM_Function) _func;
};

#endif // _STLPlugin_IExport_HXX_
