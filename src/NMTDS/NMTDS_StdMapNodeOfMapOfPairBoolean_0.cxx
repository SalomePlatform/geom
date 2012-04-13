// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

#include <NMTDS_StdMapNodeOfMapOfPairBoolean.hxx>

#ifndef _Standard_Type_HeaderFile
#include <Standard_Type.hxx>
#endif

#ifndef _NMTDS_PairBoolean_HeaderFile
#include <NMTDS_PairBoolean.hxx>
#endif
#ifndef _NMTDS_PairMapHasher_HeaderFile
#include <NMTDS_PairMapHasher.hxx>
#endif
#ifndef _NMTDS_MapOfPairBoolean_HeaderFile
#include <NMTDS_MapOfPairBoolean.hxx>
#endif
#ifndef _NMTDS_MapIteratorOfMapOfPairBoolean_HeaderFile
#include <NMTDS_MapIteratorOfMapOfPairBoolean.hxx>
#endif

 


IMPLEMENT_STANDARD_TYPE(NMTDS_StdMapNodeOfMapOfPairBoolean)
IMPLEMENT_STANDARD_SUPERTYPE_ARRAY()
  STANDARD_TYPE(TCollection_MapNode),
  STANDARD_TYPE(MMgt_TShared),
  STANDARD_TYPE(Standard_Transient),

IMPLEMENT_STANDARD_SUPERTYPE_ARRAY_END()
IMPLEMENT_STANDARD_TYPE_END(NMTDS_StdMapNodeOfMapOfPairBoolean)


IMPLEMENT_DOWNCAST(NMTDS_StdMapNodeOfMapOfPairBoolean,Standard_Transient)
IMPLEMENT_STANDARD_RTTI(NMTDS_StdMapNodeOfMapOfPairBoolean)


#define TheKey NMTDS_PairBoolean
#define TheKey_hxx <NMTDS_PairBoolean.hxx>
#define Hasher NMTDS_PairMapHasher
#define Hasher_hxx <NMTDS_PairMapHasher.hxx>
#define TCollection_StdMapNode NMTDS_StdMapNodeOfMapOfPairBoolean
#define TCollection_StdMapNode_hxx <NMTDS_StdMapNodeOfMapOfPairBoolean.hxx>
#define TCollection_MapIterator NMTDS_MapIteratorOfMapOfPairBoolean
#define TCollection_MapIterator_hxx <NMTDS_MapIteratorOfMapOfPairBoolean.hxx>
#define Handle_TCollection_StdMapNode Handle_NMTDS_StdMapNodeOfMapOfPairBoolean
#define TCollection_StdMapNode_Type_() NMTDS_StdMapNodeOfMapOfPairBoolean_Type_()
#define TCollection_Map NMTDS_MapOfPairBoolean
#define TCollection_Map_hxx <NMTDS_MapOfPairBoolean.hxx>
#include <TCollection_StdMapNode.gxx>

