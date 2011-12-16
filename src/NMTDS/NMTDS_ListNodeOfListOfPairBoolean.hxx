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

#ifndef _NMTDS_ListNodeOfListOfPairBoolean_HeaderFile
#define _NMTDS_ListNodeOfListOfPairBoolean_HeaderFile

#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Standard_DefineHandle_HeaderFile
#include <Standard_DefineHandle.hxx>
#endif
#ifndef _Handle_NMTDS_ListNodeOfListOfPairBoolean_HeaderFile
#include <Handle_NMTDS_ListNodeOfListOfPairBoolean.hxx>
#endif

#ifndef _NMTDS_PairBoolean_HeaderFile
#include <NMTDS_PairBoolean.hxx>
#endif
#ifndef _TCollection_MapNode_HeaderFile
#include <TCollection_MapNode.hxx>
#endif
#ifndef _TCollection_MapNodePtr_HeaderFile
#include <TCollection_MapNodePtr.hxx>
#endif
class NMTDS_PairBoolean;
class NMTDS_ListOfPairBoolean;
class NMTDS_ListIteratorOfListOfPairBoolean;



class NMTDS_ListNodeOfListOfPairBoolean : public TCollection_MapNode {

public:

  
      NMTDS_ListNodeOfListOfPairBoolean(const NMTDS_PairBoolean& I,const TCollection_MapNodePtr& n);
  
        NMTDS_PairBoolean& Value() const;




  DEFINE_STANDARD_RTTI(NMTDS_ListNodeOfListOfPairBoolean)

protected:




private: 


NMTDS_PairBoolean myValue;


};

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
