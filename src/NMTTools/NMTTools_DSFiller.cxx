// File:	NMTTools_DSFiller.cxx
// Created:	Fri Dec  5 14:06:34 2003
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <NMTTools_DSFiller.ixx>
#include <NMTTools_PaveFiller.hxx>

//=======================================================================
// function: NMTTools_DSFiller::NMTTools_DSFiller
// purpose: 
//=======================================================================
  NMTTools_DSFiller::NMTTools_DSFiller()
{
  myPaveFiller=NULL;
  myInterfPool=NULL;
  myDS=NULL;
  myIsNewFiller=Standard_True;
  myIsDone=Standard_False;
}
//=======================================================================
// function:Destroy
// purpose:
//=======================================================================
  void NMTTools_DSFiller::Destroy() 
{
  Clear();
}
//=======================================================================
// function: Clear
// purpose: 
//=======================================================================
  void NMTTools_DSFiller::Clear()
{
  if (myPaveFiller!=NULL) {
    delete myPaveFiller;
  }
  if (myInterfPool!=NULL) {
    delete myInterfPool;
  }
  if (myDS!=NULL) {
    delete myDS;
  }
  myPaveFiller=NULL;
  myInterfPool=NULL;
  myDS=NULL;
}
//=======================================================================
// function: SetCompositeShape
// purpose: 
//=======================================================================
  void NMTTools_DSFiller::SetCompositeShape(const TopoDS_Shape& aS)
{
  myCompositeShape=aS;
}
//=======================================================================
// function: CompositeShape
// purpose: 
//=======================================================================
  const TopoDS_Shape& NMTTools_DSFiller::CompositeShape()const
{
  return myCompositeShape;
}
//=======================================================================
// function:  SetNewFiller
// purpose: 
//=======================================================================
  void NMTTools_DSFiller::SetNewFiller(const Standard_Boolean aFlag) const
{
  NMTTools_DSFiller* pDSFiller=(NMTTools_DSFiller*)this;
  pDSFiller->myIsNewFiller=aFlag;
}
//=======================================================================
// function: IsNewFiller 
// purpose: 
//=======================================================================
  Standard_Boolean NMTTools_DSFiller::IsNewFiller() const
{
  return myIsNewFiller;
}
//=======================================================================
// function: DS
// purpose: 
//=======================================================================
  const NMTDS_ShapesDataStructure& NMTTools_DSFiller::DS () const 
{
  return *myDS;
}
//=======================================================================
// function: InterfPool
// purpose: 
//=======================================================================
  const BOPTools_InterferencePool& NMTTools_DSFiller::InterfPool () const
{
  return *myInterfPool;
}
//=======================================================================
// function:  IsDone
// purpose: 
//=======================================================================
  Standard_Boolean NMTTools_DSFiller::IsDone()const 
{
  return myIsDone;
}
//=======================================================================
// function:  PaveFiller
// purpose: 
//=======================================================================
  const NMTTools_PaveFiller& NMTTools_DSFiller::PaveFiller()const 
{
  return *myPaveFiller;
}
//=======================================================================
// function:  ChangePaveFiller
// purpose: 
//=======================================================================
  NMTTools_PaveFiller& NMTTools_DSFiller::ChangePaveFiller() 
{
  return *myPaveFiller;
}
//=======================================================================
// function:  Perform
// purpose: 
//=======================================================================
  void NMTTools_DSFiller::Perform()
{
  myIsDone=Standard_False;
  //
  if (myCompositeShape.IsNull()) {
    return;
  }
  //
  //
  // Clear contents of the DS if it was before
  Clear();
  //
  myIsNewFiller=Standard_True;
  //
  myDS        = new NMTDS_ShapesDataStructure;
  myDS->SetCompositeShape(myCompositeShape);
  myDS->Init();
  //
  myInterfPool= new BOPTools_InterferencePool (*myDS);
  //
  myPaveFiller = new NMTTools_PaveFiller(*myInterfPool);
  myPaveFiller->Perform();
  myIsDone=myPaveFiller->IsDone();
}
