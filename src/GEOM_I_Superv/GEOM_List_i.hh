#ifndef __GEOM_LIST_I_H__
#define __GEOM_LIST_I_H__

// IDL headers
#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(GEOM_Superv)

#include "GEOM_Gen_i.hh"
#include "utilities.h"

template <class Type>
class GEOM_List_i : 
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
