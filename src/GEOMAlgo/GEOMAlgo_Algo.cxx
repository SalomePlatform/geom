// File:	GEOMAlgo_Algo.cxx
// Created:	Sat Dec 04 12:39:47 2004
// Author:	Peter KURNEV
//		<peter@PREFEX>


#include <GEOMAlgo_Algo.ixx>

//=======================================================================
// function: 
// purpose: 
//=======================================================================
  GEOMAlgo_Algo::GEOMAlgo_Algo()
:
  myErrorStatus(1),
  myWarningStatus(0)
{}
//=======================================================================
// function: ~
// purpose: 
//=======================================================================
  GEOMAlgo_Algo::~GEOMAlgo_Algo()
{
}
//=======================================================================
// function: ErrorStatus
// purpose: 
//=======================================================================
  Standard_Integer GEOMAlgo_Algo::ErrorStatus()const
{
  return myErrorStatus;
}
//=======================================================================
// function: WarningStatus
// purpose: 
//=======================================================================
  Standard_Integer GEOMAlgo_Algo::WarningStatus()const
{
  return myWarningStatus;
}
//  myErrorStatus
//
// 1 - object is just initialized
