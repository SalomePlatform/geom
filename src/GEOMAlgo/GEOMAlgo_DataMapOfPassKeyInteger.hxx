// Copyright (C) 2007-2024  CEA, EDF, OPEN CASCADE
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
// File:        GEOMAlgo_DataMapOfPassKeyInteger.hxx
// Created:     Wed Feb 22 10:55:55 2012
// Author:
//              <pkv@BDEURI37616>


#ifndef GEOMAlgo_DataMapOfPassKeyInteger_HeaderFile
#define GEOMAlgo_DataMapOfPassKeyInteger_HeaderFile

#include <Basics_OCCTVersion.hxx>

#include <GEOMAlgo_PassKey.hxx>
#include <Standard_Integer.hxx>

#if OCC_VERSION_LARGE < 0x07080000
#include <GEOMAlgo_PassKeyMapHasher.hxx>

#define _NCollection_MapHasher
#include <NCollection_DataMap.hxx>

typedef NCollection_DataMap<GEOMAlgo_PassKey, Standard_Integer, GEOMAlgo_PassKeyMapHasher> GEOMAlgo_DataMapOfPassKeyInteger;

#else

#include <NCollection_DataMap.hxx>

typedef NCollection_DataMap<GEOMAlgo_PassKey, Standard_Integer> GEOMAlgo_DataMapOfPassKeyInteger;

#endif // OCC_VERSION_LARGE < 0x07080000


typedef GEOMAlgo_DataMapOfPassKeyInteger::Iterator GEOMAlgo_DataMapIteratorOfDataMapOfPassKeyInteger;

#undef _NCollection_MapHasher


#endif
