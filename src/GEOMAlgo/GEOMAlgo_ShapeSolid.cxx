// File:	GEOMAlgo_ShapeSolid.cxx
// Created:	Thu Jan 13 12:54:48 2005
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <GEOMAlgo_ShapeSolid.ixx>

#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>

//=======================================================================
//function : GEOMAlgo_ShapeSolid
//purpose  : 
//=======================================================================
GEOMAlgo_ShapeSolid::GEOMAlgo_ShapeSolid()
:
  GEOMAlgo_Algo(),
  myRank(0),
  myDSFiller(NULL)
{
}
//=======================================================================
//function : ~
//purpose  : 
//=======================================================================
GEOMAlgo_ShapeSolid::~GEOMAlgo_ShapeSolid()
{
}
//=======================================================================
//function : SetFiller
//purpose  : 
//=======================================================================
void GEOMAlgo_ShapeSolid::SetFiller(const BOPTools_DSFiller& aDSFiller)
{
  myDSFiller=(BOPTools_DSFiller*) &aDSFiller;
}
//=======================================================================
// function: Shapes
// purpose: 
//=======================================================================
const TopTools_ListOfShape& GEOMAlgo_ShapeSolid::Shapes(const TopAbs_State aState) const
{
  const TopTools_ListOfShape *pL;
  //
  switch (aState) {
    case TopAbs_IN:
      pL=&myLSIN;
      break;
    case TopAbs_OUT:
      pL=&myLSOUT;
      break;
    case TopAbs_ON:
      pL=&myLSON;
      break;  
    default:
      pL=&myLSON;
      break; 
  }
  return *pL;
}
