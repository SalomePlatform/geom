// File:	GEOMAlgo_ShapeAlgo.cxx
// Created:	Tue Dec  7 12:06:54 2004
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <GEOMAlgo_ShapeAlgo.ixx>

//=======================================================================
//function : GEOMAlgo_ShapeAlgo
//purpose  : 
//=======================================================================
GEOMAlgo_ShapeAlgo::GEOMAlgo_ShapeAlgo()
: 
  GEOMAlgo_Algo()
{
  myTolerance=0.0001;
}
       
//=======================================================================
//function : ~
//purpose  : 
//=======================================================================
GEOMAlgo_ShapeAlgo::~GEOMAlgo_ShapeAlgo()
{
}
//=======================================================================
//function : SetShape
//purpose  : 
//=======================================================================
void GEOMAlgo_ShapeAlgo::SetShape(const TopoDS_Shape& aS)
{
  myShape=aS;
}
//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================
const TopoDS_Shape& GEOMAlgo_ShapeAlgo::Shape()const
{
  return myShape;
}
//=======================================================================
//function : SetTolerance
//purpose  : 
//=======================================================================
void GEOMAlgo_ShapeAlgo::SetTolerance(const Standard_Real aT)
{
  myTolerance=aT;
}
//=======================================================================
//function : Tolerance
//purpose  : 
//=======================================================================
Standard_Real GEOMAlgo_ShapeAlgo::Tolerance()const
{
  return myTolerance;
}
//=======================================================================
//function : Result
//purpose  : 
//=======================================================================
const TopoDS_Shape& GEOMAlgo_ShapeAlgo::Result()const
{
  return myResult;
}
