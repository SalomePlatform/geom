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
const TopTools_ListOfShape& GEOMAlgo_ShapeSolid::Shapes(const GEOMAlgo_State aState) const
{
  TopTools_ListIteratorOfListOfShape aIt;
  //
  TopTools_ListOfShape* pLS=(TopTools_ListOfShape*)&myLS;
  //
  pLS->Clear();
  //
  switch (aState) {
    case GEOMAlgo_ST_IN: {
      aIt.Initialize(myLSIN);
      for (; aIt.More(); aIt.Next()) {
	const TopoDS_Shape& aS=aIt.Value(); 
	pLS->Append(aS);
      }
    }
      break;
    //  
    case GEOMAlgo_ST_OUT: {
      aIt.Initialize(myLSOUT);
      for (; aIt.More(); aIt.Next()) {
	const TopoDS_Shape& aS=aIt.Value(); 
	pLS->Append(aS);
      }
    }
      break;
    //  
    case GEOMAlgo_ST_ON: {
      aIt.Initialize(myLSON);
      for (; aIt.More(); aIt.Next()) {
	const TopoDS_Shape& aS=aIt.Value(); 
	pLS->Append(aS);
      }
    }
      break;
    //  
    case GEOMAlgo_ST_ONIN: {
      aIt.Initialize(myLSON);
      for (; aIt.More(); aIt.Next()) {
	const TopoDS_Shape& aS=aIt.Value(); 
	pLS->Append(aS);
      }
      aIt.Initialize(myLSIN);
      for (; aIt.More(); aIt.Next()) {
	const TopoDS_Shape& aS=aIt.Value(); 
	pLS->Append(aS);
      }
    }
      break;
    //  
    case GEOMAlgo_ST_ONOUT: {
      aIt.Initialize(myLSON);
      for (; aIt.More(); aIt.Next()) {
	const TopoDS_Shape& aS=aIt.Value(); 
	pLS->Append(aS);
      }
      aIt.Initialize(myLSOUT);
      for (; aIt.More(); aIt.Next()) {
	const TopoDS_Shape& aS=aIt.Value(); 
	pLS->Append(aS);
      }
    }
      break;
    //
    case GEOMAlgo_ST_UNKNOWN:
    case GEOMAlgo_ST_INOUT:
    default:
      break;
  }
  return myLS;
}

