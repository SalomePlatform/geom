// File:	NMTTools_CoupleOfShape.cxx
// Created:	Wed Jan 28 15:06:11 2004
// Author:	Peter KURNEV
//		<pkv@irinox>

#include <NMTTools_CoupleOfShape.ixx>

//=======================================================================
// function: NMTTools_CoupleOfShape::NMTTools_CoupleOfShape
// purpose: 
//=======================================================================
  NMTTools_CoupleOfShape::NMTTools_CoupleOfShape()
{}
//=======================================================================
// function: SetShape1
// purpose: 
//=======================================================================
  void NMTTools_CoupleOfShape::SetShape1(const TopoDS_Shape& aS)
{
  myShape1=aS;
}
//=======================================================================
// function: SetShape2
// purpose: 
//=======================================================================
  void NMTTools_CoupleOfShape::SetShape2(const TopoDS_Shape& aS)
{
  myShape2=aS;
}
//=======================================================================
// function: Shape1
// purpose: 
//=======================================================================
  const TopoDS_Shape& NMTTools_CoupleOfShape::Shape1()const
{
  return myShape1;
}
//=======================================================================
// function: Shape2
// purpose: 
//=======================================================================
  const TopoDS_Shape& NMTTools_CoupleOfShape::Shape2()const
{
  return myShape2;
}
