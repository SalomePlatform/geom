// File:	GEOMAlgo_StateCollector.cxx
// Created:	Thu Mar 10 09:42:11 2005
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <GEOMAlgo_StateCollector.ixx>

//=======================================================================
//function : GEOMAlgo_StateCollector
//purpose  : 
//=======================================================================
GEOMAlgo_StateCollector::GEOMAlgo_StateCollector() 
{
  myCounter[0]=0;
  myCounter[1]=0;
  myCounter[2]=0;
}
//=======================================================================
//function : AppendState
//purpose  : 
//=======================================================================
Standard_Boolean GEOMAlgo_StateCollector::AppendState(const TopAbs_State aSt) 
{
  Standard_Boolean bIsToBreak;
  //
  switch(aSt) {
    case TopAbs_IN:
      ++myCounter[0];
      break;
    case TopAbs_OUT:
      ++myCounter[1];
      break; 
    case TopAbs_ON:
      ++myCounter[2];
      break;
    default:
      break;
  }
  bIsToBreak=(myCounter[0] && myCounter[1]);
  //
  return bIsToBreak;
}
//=======================================================================
//function : State
//purpose  : 
//=======================================================================
TopAbs_State GEOMAlgo_StateCollector::State()const 
{
  TopAbs_State aSt;
  //
  aSt=TopAbs_UNKNOWN;
  //
  if (myCounter[0] && myCounter[1]) {
    return aSt;
  }
  //
  else if (myCounter[0] && myCounter[2]>=0) {
    aSt=TopAbs_IN;
  }
  //
  else if (myCounter[1] && myCounter[2]>=0) {
    aSt=TopAbs_OUT;
  }
  //
  else if (!myCounter[0] && !myCounter[1] && myCounter[2]) {
    aSt=TopAbs_ON;
  }
  //
  return aSt;
}
