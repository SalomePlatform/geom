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

// File:	GEOMAlgo_IndexedDataMapOfShapeBndSphere.hxx
// Created:	Mon Feb 20 09:17:01 2012
// Author:	
//		<pkv@BDEURI37616>


#ifndef GEOMAlgo_IndexedDataMapOfShapeBndSphere_HeaderFile
#define GEOMAlgo_IndexedDataMapOfShapeBndSphere_HeaderFile

#include <TopoDS_Shape.hxx>  
#include <TopTools_ShapeMapHasher.hxx>
#include <GEOMAlgo_BndSphere.hxx>

#define _NCollection_MapHasher
#include <NCollection_IndexedDataMap.hxx>



typedef NCollection_IndexedDataMap<TopoDS_Shape, GEOMAlgo_BndSphere, TopTools_ShapeMapHasher> GEOMAlgo_IndexedDataMapOfShapeBndSphere; 
 
#undef _NCollection_MapHasher




#endif
