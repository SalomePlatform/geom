using namespace std;
//  File      : GEOMDS_Explorer.cxx
//  Created   :
//  Author    : Yves FRICAUD
//  Project   : SALOME
//  Module    : GEOM
//  Copyright : OPEN CASCADE
//  $Header$


#include "GEOMDS_Explorer.ixx"

#include <TNaming_NamedShape.hxx>
#include <TNaming_Tool.hxx>
#include <TDataStd_Name.hxx>


//=======================================================================
//function : GEOMDS_Explorer
//purpose  : 
//=======================================================================

GEOMDS_Explorer::GEOMDS_Explorer(const TDF_Label& Main) : myChildIterator(Main)
{
}


//=======================================================================
//function : const;
//purpose  : 
//=======================================================================

Standard_Boolean GEOMDS_Explorer::More() 
{
  if (!myChildIterator.More())
    return 0;
  Handle(TDataStd_Name) Att;
  Handle(TNaming_NamedShape) NS; 
  TDF_Label L = myChildIterator.Value();
  if (( L.FindAttribute(TDataStd_Name::GetID(),Att) ) || 
      (L.FindAttribute(TNaming_NamedShape::GetID(),NS))
      )
    return 1;
  //  myChildIterator.Next();
  //  More();
  return 0;
  /*
    if (!myChildIterator.More())
    return 0;
    TDF_Label L = myChildIterator.Value();
    Handle(TNaming_NamedShape) NS; 
    for (TDF_ChildIterator it2(L); it2.More(); it2.Next()) {
    TDF_Label L2 = it2.Value();
    if (L2.FindAttribute(TNaming_NamedShape::GetID(),NS)) {
    return 1;
    }
    myChildIterator.Next();
    }
    return 0;
  */
}


//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void GEOMDS_Explorer::Next() 
{
  myChildIterator.Next();
}


//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================

TopoDS_Shape GEOMDS_Explorer::Shape() const
{
  Handle(TNaming_NamedShape) NS; 
  TDF_Label L = myChildIterator.Value();
  L.FindAttribute(TNaming_NamedShape::GetID(),NS);
  TopoDS_Shape S = TNaming_Tool::GetShape(NS);
  return S;
}

//=======================================================================
//function : Name
//purpose  : 
//=======================================================================

TCollection_ExtendedString GEOMDS_Explorer::Name() const
{
 TDF_Label L = myChildIterator.Value();
 Handle(TDataStd_Name) Att;
 if ( L.FindAttribute(TDataStd_Name::GetID(),Att) )
   //L.FindAttribute(TDataStd_Name::GetID(),Att);
   return Att->Get();
 else
   return TCollection_ExtendedString();
}




