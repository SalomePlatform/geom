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

//  File   : GEOM_GEOM_I.hxx
//  Author : Alexander A. BORODIN
//  Module : GEOM
//
#ifndef _GEOM_GEOM_I_HXX_
#define _GEOM_GEOM_I_HXX_

#ifdef WIN32
  #if defined GEOM_I_EXPORTS || defined GEOMEngine_EXPORTS
    #define GEOM_I_EXPORT __declspec( dllexport )
  #else
    #define GEOM_I_EXPORT __declspec( dllimport )
  #endif
#else
   #define GEOM_I_EXPORT
#endif

#include <Basics_OCCTVersion.hxx>
#if OCC_VERSION_MAJOR < 7
  #define HANDLE_NAMESPACE(CLS) Handle(CLS)
#else
  #define HANDLE_NAMESPACE(CLS) Handle(::CLS)
#endif

#endif //_GEOM_GEOM_I_HXX_
