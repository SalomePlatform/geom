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

#include <NMTDS_ListNodeOfListOfPair.hxx>

#ifndef _Standard_Type_HeaderFile
#include <Standard_Type.hxx>
#endif

#ifndef _NMTDS_Pair_HeaderFile
#include <NMTDS_Pair.hxx>
#endif
#ifndef _NMTDS_ListOfPair_HeaderFile
#include <NMTDS_ListOfPair.hxx>
#endif
#ifndef _NMTDS_ListIteratorOfListOfPair_HeaderFile
#include <NMTDS_ListIteratorOfListOfPair.hxx>
#endif

 


IMPLEMENT_STANDARD_TYPE(NMTDS_ListNodeOfListOfPair)
IMPLEMENT_STANDARD_SUPERTYPE_ARRAY()
  STANDARD_TYPE(TCollection_MapNode),
  STANDARD_TYPE(MMgt_TShared),
  STANDARD_TYPE(Standard_Transient),

IMPLEMENT_STANDARD_SUPERTYPE_ARRAY_END()
IMPLEMENT_STANDARD_TYPE_END(NMTDS_ListNodeOfListOfPair)


IMPLEMENT_DOWNCAST(NMTDS_ListNodeOfListOfPair,Standard_Transient)
IMPLEMENT_STANDARD_RTTI(NMTDS_ListNodeOfListOfPair)


#define Item NMTDS_Pair
#define Item_hxx <NMTDS_Pair.hxx>
#define TCollection_ListNode NMTDS_ListNodeOfListOfPair
#define TCollection_ListNode_hxx <NMTDS_ListNodeOfListOfPair.hxx>
#define TCollection_ListIterator NMTDS_ListIteratorOfListOfPair
#define TCollection_ListIterator_hxx <NMTDS_ListIteratorOfListOfPair.hxx>
#define Handle_TCollection_ListNode Handle_NMTDS_ListNodeOfListOfPair
#define TCollection_ListNode_Type_() NMTDS_ListNodeOfListOfPair_Type_()
#define TCollection_List NMTDS_ListOfPair
#define TCollection_List_hxx <NMTDS_ListOfPair.hxx>
#include <TCollection_ListNode.gxx>

