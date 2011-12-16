// Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include <NMTDS_ListNodeOfListOfPairBoolean.hxx>

#ifndef _Standard_Type_HeaderFile
#include <Standard_Type.hxx>
#endif

#ifndef _NMTDS_PairBoolean_HeaderFile
#include <NMTDS_PairBoolean.hxx>
#endif
#ifndef _NMTDS_ListOfPairBoolean_HeaderFile
#include <NMTDS_ListOfPairBoolean.hxx>
#endif
#ifndef _NMTDS_ListIteratorOfListOfPairBoolean_HeaderFile
#include <NMTDS_ListIteratorOfListOfPairBoolean.hxx>
#endif

 


IMPLEMENT_STANDARD_TYPE(NMTDS_ListNodeOfListOfPairBoolean)
IMPLEMENT_STANDARD_SUPERTYPE_ARRAY()
  STANDARD_TYPE(TCollection_MapNode),
  STANDARD_TYPE(MMgt_TShared),
  STANDARD_TYPE(Standard_Transient),

IMPLEMENT_STANDARD_SUPERTYPE_ARRAY_END()
IMPLEMENT_STANDARD_TYPE_END(NMTDS_ListNodeOfListOfPairBoolean)


IMPLEMENT_DOWNCAST(NMTDS_ListNodeOfListOfPairBoolean,Standard_Transient)
IMPLEMENT_STANDARD_RTTI(NMTDS_ListNodeOfListOfPairBoolean)


#define Item NMTDS_PairBoolean
#define Item_hxx <NMTDS_PairBoolean.hxx>
#define TCollection_ListNode NMTDS_ListNodeOfListOfPairBoolean
#define TCollection_ListNode_hxx <NMTDS_ListNodeOfListOfPairBoolean.hxx>
#define TCollection_ListIterator NMTDS_ListIteratorOfListOfPairBoolean
#define TCollection_ListIterator_hxx <NMTDS_ListIteratorOfListOfPairBoolean.hxx>
#define Handle_TCollection_ListNode Handle_NMTDS_ListNodeOfListOfPairBoolean
#define TCollection_ListNode_Type_() NMTDS_ListNodeOfListOfPairBoolean_Type_()
#define TCollection_List NMTDS_ListOfPairBoolean
#define TCollection_List_hxx <NMTDS_ListOfPairBoolean.hxx>
#include <TCollection_ListNode.gxx>

