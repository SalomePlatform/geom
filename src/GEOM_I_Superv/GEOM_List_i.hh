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

#ifndef __GEOM_LIST_I_H__
#define __GEOM_LIST_I_H__

#include "GEOM_I_Superv.hxx"

// IDL headers
#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(GEOM_Superv)

#include "utilities.h"

template <class Type>
class GEOM_I_SUPERV_EXPORT GEOM_List_i : 
  public virtual POA_GEOM::GEOM_List,
  public virtual PortableServer::RefCountServantBase
{
public:
  GEOM_List_i();
  GEOM_List_i(const Type& theListToCopy);
  ~GEOM_List_i();

  const Type& GetList();

  template <class ElemType>
  void AddObject(ElemType theObject);

private:
  Type myGOList;
};

//=============================================================================
//  constructor:
//=============================================================================
template <class Type>
GEOM_List_i<Type>::GEOM_List_i()
{
  MESSAGE("GEOM_List_i::GEOM_List_i");
  //create an empty list
  myGOList.length(0);
}

//=============================================================================
//  constructor:
//=============================================================================
template <class Type>
GEOM_List_i<Type>::GEOM_List_i(const Type& theListToCopy)
{
  int aLength = theListToCopy.length();
  myGOList.length(aLength);
  for (int i = 0; i < aLength; i++)
    myGOList[i] = theListToCopy[i];
}

//=============================================================================
//  destructor:
//=============================================================================
template <class Type>
GEOM_List_i<Type>::~GEOM_List_i()
{
  MESSAGE("GEOM_List_i::~GEOM_List_i");
}

//=============================================================================
//  GetList:
//=============================================================================
template <class Type>
const Type& GEOM_List_i<Type>::GetList()
{
  MESSAGE("GEOM_List_i::GetList()");
  return myGOList;
}

//=============================================================================
//  AddObject:
//=============================================================================
template <class Type>
template <class ElemType>
void GEOM_List_i<Type>::AddObject(ElemType theObject)
{
  MESSAGE("GEOM_List_i::AddObject(...)");
  int aLength = myGOList.length();
  myGOList.length(aLength+1);
  myGOList[aLength] = theObject;
}

#endif
