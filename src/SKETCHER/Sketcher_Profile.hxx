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

//  File   : Sketcher_Profile.h
//  Author : Damien COQUERET


#ifndef _Sketcher_Profile_HXX_
#define _Sketcher_Profile_HXX_


#include "Sketcher.hxx"
#include <TopoDS_Shape.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_Array1OfAsciiString.hxx>

#include <string>

class SKETCHER_SALOME_EXPORT Sketcher_Profile
{
  class Functor;
  class ShapeFunctor;
  class DumpFunctor;

public:
  Sketcher_Profile();
  Sketcher_Profile( const char* );

  void                    SetCommand( const char* );

  TopoDS_Shape            GetShape( bool* = 0, double* = 0 );
  TCollection_AsciiString GetDump( bool* = 0 );

private:
  void parse( const TCollection_AsciiString&, Functor* );
  void badArgs();
  void findNextCommand( const TColStd_Array1OfAsciiString&, TColStd_Array1OfAsciiString&, int, int& );
  TCollection_AsciiString extractCommand( const TCollection_AsciiString& );

private:
  TCollection_AsciiString myCommand;
};

#endif
