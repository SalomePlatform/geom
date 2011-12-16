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

#ifndef _NMTDS_StdMapNodeOfMapOfPairBoolean_HeaderFile
#define _NMTDS_StdMapNodeOfMapOfPairBoolean_HeaderFile

#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Standard_DefineHandle_HeaderFile
#include <Standard_DefineHandle.hxx>
#endif
#ifndef _Handle_NMTDS_StdMapNodeOfMapOfPairBoolean_HeaderFile
#include <Handle_NMTDS_StdMapNodeOfMapOfPairBoolean.hxx>
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
class NMTDS_PairMapHasher;
class NMTDS_MapOfPairBoolean;
class NMTDS_MapIteratorOfMapOfPairBoolean;



class NMTDS_StdMapNodeOfMapOfPairBoolean : public TCollection_MapNode {

public:

  
      NMTDS_StdMapNodeOfMapOfPairBoolean(const NMTDS_PairBoolean& K,const TCollection_MapNodePtr& n);
  
        NMTDS_PairBoolean& Key() const;




  DEFINE_STANDARD_RTTI(NMTDS_StdMapNodeOfMapOfPairBoolean)

protected:




private: 


NMTDS_PairBoolean myKey;


};

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

#include <TCollection_StdMapNode.lxx>

#undef TheKey
#undef TheKey_hxx
#undef Hasher
#undef Hasher_hxx
#undef TCollection_StdMapNode
#undef TCollection_StdMapNode_hxx
#undef TCollection_MapIterator
#undef TCollection_MapIterator_hxx
#undef Handle_TCollection_StdMapNode
#undef TCollection_StdMapNode_Type_
#undef TCollection_Map
#undef TCollection_Map_hxx


// other Inline functions and methods (like "C++: function call" methods)


#endif
