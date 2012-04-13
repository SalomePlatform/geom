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

#ifndef _NMTDS_ListNodeOfListOfPair_HeaderFile
#define _NMTDS_ListNodeOfListOfPair_HeaderFile

#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Standard_DefineHandle_HeaderFile
#include <Standard_DefineHandle.hxx>
#endif
#ifndef _Handle_NMTDS_ListNodeOfListOfPair_HeaderFile
#include <Handle_NMTDS_ListNodeOfListOfPair.hxx>
#endif

#ifndef _NMTDS_Pair_HeaderFile
#include <NMTDS_Pair.hxx>
#endif
#ifndef _TCollection_MapNode_HeaderFile
#include <TCollection_MapNode.hxx>
#endif
#ifndef _TCollection_MapNodePtr_HeaderFile
#include <TCollection_MapNodePtr.hxx>
#endif
class NMTDS_Pair;
class NMTDS_ListOfPair;
class NMTDS_ListIteratorOfListOfPair;



class NMTDS_ListNodeOfListOfPair : public TCollection_MapNode {

public:

  
      NMTDS_ListNodeOfListOfPair(const NMTDS_Pair& I,const TCollection_MapNodePtr& n);
  
        NMTDS_Pair& Value() const;




  DEFINE_STANDARD_RTTI(NMTDS_ListNodeOfListOfPair)

protected:




private: 


NMTDS_Pair myValue;


};

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

#include <TCollection_ListNode.lxx>

#undef Item
#undef Item_hxx
#undef TCollection_ListNode
#undef TCollection_ListNode_hxx
#undef TCollection_ListIterator
#undef TCollection_ListIterator_hxx
#undef Handle_TCollection_ListNode
#undef TCollection_ListNode_Type_
#undef TCollection_List
#undef TCollection_List_hxx


// other Inline functions and methods (like "C++: function call" methods)


#endif
