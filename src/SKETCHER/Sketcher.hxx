// Copyright (C) 2007-2024  CEA, EDF, OPEN CASCADE
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

//  File   : Sketcher.hxx
//  Author : Sergey KHROMOV

#ifndef _SKETCHER_HXX_
#define _SKETCHER_HXX_

#if defined WIN32
#  if defined SKETCHER_SALOME_EXPORTS || defined SKETCHER_EXPORTS || defined GEOMSketcher_EXPORTS || defined GEOMSKETCHER_EXPORTS
#    define SKETCHER_SALOME_EXPORT _declspec( dllexport )
#  else
#    define SKETCHER_SALOME_EXPORT _declspec( dllimport )
#  endif
#else
#  define SKETCHER_SALOME_EXPORT
#endif

#endif // SKETCHER_HXX