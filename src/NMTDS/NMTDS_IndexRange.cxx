// File:	NMTDS_IndexRange.cxx
// Created:	Fri Nov 28 10:34:59 2003
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <NMTDS_IndexRange.ixx>
//=======================================================================
//function :NMTDS_IndexRange::NMTDS_IndexRange
//purpose  : 
//=======================================================================
  NMTDS_IndexRange::NMTDS_IndexRange()
:
  myFirst(0),
  myLast(0)
{
}
//=======================================================================
//function :SetFirst
//purpose  : 
//=======================================================================
  void NMTDS_IndexRange::SetFirst(const Standard_Integer aFirst)
{
  myFirst=aFirst;
}
//=======================================================================
//function :First
//purpose  : 
//=======================================================================
  Standard_Integer NMTDS_IndexRange::First()const
{
  return myFirst;
}
//=======================================================================
//function :SetLast
//purpose  : 
//=======================================================================
  void NMTDS_IndexRange::SetLast(const Standard_Integer aLast)
{
  myLast=aLast;
}
//=======================================================================
//function :Last
//purpose  : 
//=======================================================================
  Standard_Integer NMTDS_IndexRange::Last()const
{
  return myLast;
}
//=======================================================================
//function :IsInRange
//purpose  : 
//=======================================================================
  Standard_Boolean NMTDS_IndexRange::IsInRange(const Standard_Integer aIndex)const
{
  return (Standard_Boolean)(aIndex>=myFirst && aIndex<=myLast);
}
