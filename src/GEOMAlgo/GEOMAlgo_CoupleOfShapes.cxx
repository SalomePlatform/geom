// File:	GEOMAlgo_CoupleOfShapes.cxx
// Created:	Wed Dec 15 13:03:52 2004
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <GEOMAlgo_CoupleOfShapes.ixx>
//=======================================================================
//function : GEOMAlgo_CoupleOfShapes
//purpose  : 
//=======================================================================
GEOMAlgo_CoupleOfShapes::GEOMAlgo_CoupleOfShapes()
{}
//=======================================================================
//function : SetShapes
//purpose  : 
//=======================================================================
void GEOMAlgo_CoupleOfShapes::SetShapes(const TopoDS_Shape& aS1,
					const TopoDS_Shape& aS2)
{
  myShape1=aS1;
  myShape2=aS2;
}
//=======================================================================
//function : Shapes
//purpose  : 
//=======================================================================
void GEOMAlgo_CoupleOfShapes::Shapes(TopoDS_Shape& aS1,
				     TopoDS_Shape& aS2)const
{
  aS1=myShape1;
  aS2=myShape2;
}
//=======================================================================
//function : SetShape1
//purpose  : 
//=======================================================================
void GEOMAlgo_CoupleOfShapes::SetShape1(const TopoDS_Shape& aS1)
{
  myShape1=aS1;
}
//=======================================================================
//function : SetShape2
//purpose  : 
//=======================================================================
void GEOMAlgo_CoupleOfShapes::SetShape2(const TopoDS_Shape& aS2)
{
  myShape2=aS2;
}
//=======================================================================
//function : Shape1
//purpose  : 
//=======================================================================
const TopoDS_Shape& GEOMAlgo_CoupleOfShapes::Shape1()const
{
  return myShape1;
}
//=======================================================================
//function : Shape2
//purpose  : 
//=======================================================================
const TopoDS_Shape& GEOMAlgo_CoupleOfShapes::Shape2()const
{
  return myShape2;
}
