// Copyright (C) 2005  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public  
// License along with this library; if not, write to the Free Software 
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// File:	NMTTools_PaveFiller.cxx
// Created:	Fri Dec  5 14:58:54 2003
// Author:	Peter KURNEV
//		<pkv@irinox>
//
#include <NMTTools_PaveFiller.ixx>
//
#include <BOPTColStd_Failure.hxx>
#include <NMTDS_ShapesDataStructure.hxx>
#include <NMTTools_DEProcessor.hxx>

//
//=======================================================================
// function: NMTTools_PaveFiller::NMTTools_PaveFiller
// purpose: 
//=======================================================================
  NMTTools_PaveFiller::NMTTools_PaveFiller()
{
  myIntrPool=NULL;
  myDS=NULL;
  myIsDone=Standard_False;
  myNbSources=0;
  myNbEdges=0;
}
//=======================================================================
// function:NMTTools_PaveFiller::NMTTools_PaveFiller
// purpose: 
//=======================================================================
  NMTTools_PaveFiller::NMTTools_PaveFiller(const BOPTools_InterferencePool& aIP)
{
  SetInterferencePool(aIP);
}
//=======================================================================
// function: Destroy
// purpose: 
//=======================================================================
  void NMTTools_PaveFiller::Destroy()
{
}
//=======================================================================
// function: SetInterferencePool
// purpose: 
//=======================================================================
  void NMTTools_PaveFiller::SetInterferencePool(const BOPTools_InterferencePool& aIP)
{
  myIsDone=Standard_False;
  //
  myIntrPool=(BOPTools_InterferencePool*)&aIP;
  myDS=(NMTDS_ShapesDataStructure*)myIntrPool->DS();
  myNbSources=myDS->NumberOfShapesOfTheObject()+myDS->NumberOfShapesOfTheTool();
  myNbEdges=myDS->NbEdges();
}
//=======================================================================
// function:  InterfPool
// purpose: 
//=======================================================================
  BOPTools_PInterferencePool NMTTools_PaveFiller::InterfPool()
{
  return myIntrPool;
}
//=======================================================================
// function:IsDone
// purpose: 
//=======================================================================
  Standard_Boolean NMTTools_PaveFiller::IsDone() const
{
  return myIsDone;
}
//=======================================================================
// function:  DS
// purpose: 
//=======================================================================
  NMTDS_PShapesDataStructure NMTTools_PaveFiller::DS()
{
  return myDS;
}
//=======================================================================
// function: Context
// purpose: 
//=======================================================================
  const IntTools_Context& NMTTools_PaveFiller::Context() const
{
  return myContext;
}
//=======================================================================
// function: ChangeContext
// purpose: 
//=======================================================================
  IntTools_Context& NMTTools_PaveFiller::ChangeContext() 
{
  return myContext;
}
//=======================================================================
// function: PavePool
// purpose: 
//=======================================================================
  const BOPTools_PavePool& NMTTools_PaveFiller::PavePool() const
{
  return myPavePool;
}
//=======================================================================
// function: ChangePavePool
// purpose: 
//=======================================================================
  BOPTools_PavePool& NMTTools_PaveFiller::ChangePavePool()
{
  return myPavePool;
}
//=======================================================================
// function:  CommonBlockPool
// purpose: 
//=======================================================================
  const NMTTools_CommonBlockPool& NMTTools_PaveFiller::CommonBlockPool() const
{
  return myCommonBlockPool;
}
//=======================================================================
// function:  ChangeCommonBlockPool
// purpose: 
//=======================================================================
  NMTTools_CommonBlockPool& NMTTools_PaveFiller::ChangeCommonBlockPool()
{
  return myCommonBlockPool;
}
//=======================================================================
// function:  SplitShapesPool
// purpose: 
//=======================================================================
  const BOPTools_SplitShapesPool& NMTTools_PaveFiller::SplitShapesPool() const
{
  return mySplitShapesPool;
}

//=======================================================================
// function:  ChangeSplitShapesPool
// purpose: 
//=======================================================================
  BOPTools_SplitShapesPool& NMTTools_PaveFiller::ChangeSplitShapesPool()
{
  return mySplitShapesPool;
}
//=======================================================================
// function: Init
// purpose: 
//=======================================================================
  void NMTTools_PaveFiller::Init()
{
  myDSIt.SetDS(myDS);
  // Modified Thu Sep 14 14:35:18 2006 
  // Contribution of Samtech www.samcef.com BEGIN
  myDSIt.Prepare();
  // Contribution of Samtech www.samcef.com END 
}

//=======================================================================
// function: Perform
// purpose: 
//=======================================================================
  void NMTTools_PaveFiller::Perform()
{
  myIsDone=Standard_False;
  //
  try {
    // 0.
    // Modified Thu Sep 14 14:35:18 2006 
    // Contribution of Samtech www.samcef.com BEGIN
    Init();
    // Contribution of Samtech www.samcef.com END
    //1.VV
    //
    PerformVV();
    PerformNewVertices();
    //
    // 2.VE
    myPavePool.Resize (myNbEdges);
    
    PrepareEdges();

    PerformVE();
    //
    // 3.VF
    PerformVF();
    //
    // 4.EE
    myCommonBlockPool.Resize (myNbEdges);
    mySplitShapesPool.Resize (myNbEdges);
    myPavePoolNew    .Resize (myNbEdges);

    PreparePaveBlocks(TopAbs_VERTEX, TopAbs_EDGE);
    PreparePaveBlocks(TopAbs_EDGE, TopAbs_EDGE);
    //
    PerformEE();
    //
    RefinePavePool ();
    //
    myPavePoolNew.Destroy();
    myPavePoolNew.Resize (myNbEdges);
    //
    // 5.EF
    PreparePaveBlocks(TopAbs_EDGE, TopAbs_FACE);

    PerformEF();
    //
    RefinePavePool();
    //
    myPavePoolNew.Destroy();
    
    MakeSplitEdges();

    UpdateCommonBlocks();
    //
    // 6. FF
    PerformFF ();
    //
    MakeBlocks();
    //
    MakePCurves();
    //
    // 7.Postprocessing 
    UpdatePaveBlocks();
    //
    NMTTools_DEProcessor aDEP(*this);
    aDEP.Do();
    //
    // Modified to treat Alone Vertices Thu Sep 14 14:35:18 2006 
    // Contribution of Samtech www.samcef.com BEGIN
    MakeAloneVertices();
    // Contribution of Samtech www.samcef.com END
    //
    myIsDone=Standard_True;
  }
  catch (BOPTColStd_Failure& ) {
  }
}
