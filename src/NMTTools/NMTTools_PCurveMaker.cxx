// File:	NMTTools_PCurveMaker.cxx
// Created:	
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <NMTTools_PCurveMaker.ixx>

#include <gp_Pnt2d.hxx>

#include <Precision.hxx>

#include <Geom_Curve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_HCurve.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>

#include <TopExp.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <BRepAdaptor_HSurface.hxx>

#include <NMTDS_ShapesDataStructure.hxx>
#include <NMTTools_DSFiller.hxx>

#include <IntTools_Curve.hxx>

#include <BOPTools_InterferencePool.hxx>
#include <BOPTools_CArray1OfSSInterference.hxx>
#include <BOPTools_SSInterference.hxx>
#include <BOPTools_ListOfPaveBlock.hxx>
#include <BOPTools_ListIteratorOfListOfPaveBlock.hxx>
#include <BOPTools_PaveBlock.hxx>
#include <BOPTools_Tools2D.hxx>
#include <NMTTools_PaveFiller.hxx>


//=======================================================================
// function: NMTTools_PCurveMaker
// purpose: 
//=======================================================================
  NMTTools_PCurveMaker::NMTTools_PCurveMaker(NMTTools_PDSFiller& pDSFiller)
:
  myIsDone(Standard_False)
{
  myDSFiller=pDSFiller;
}
//=======================================================================
// function: IsDone
// purpose: 
//=======================================================================
  Standard_Boolean NMTTools_PCurveMaker::IsDone() const
{
  return myIsDone;
}
//=======================================================================
// function: Do
// purpose: 
//=======================================================================
  void NMTTools_PCurveMaker::Do()
{
  Standard_Integer i, aNb,  nF1, nF2, nE;
  BOPTools_ListIteratorOfListOfPaveBlock anIt;
  TopoDS_Face aF1FWD, aF2FWD;
  //
  NMTTools_PaveFiller *pFiller=(NMTTools_PaveFiller*) &(myDSFiller->PaveFiller());
  NMTDS_ShapesDataStructure *pDS=pFiller->DS();
  //
  BOPTools_CArray1OfSSInterference& aFFs=(pFiller->InterfPool())->SSInterferences();
  //
  aNb=aFFs.Extent();
  for (i=1; i<=aNb; ++i) {
    BOPTools_SSInterference& aFF=aFFs(i);
    // Faces
    aFF.Indices(nF1, nF2);
    const TopoDS_Face& aF1=TopoDS::Face(pDS->Shape(nF1));
    const TopoDS_Face& aF2=TopoDS::Face(pDS->Shape(nF2));
    //
    // 1.Forwarding the Faces
    aF1FWD=aF1;
    aF1FWD.Orientation(TopAbs_FORWARD);
    aF2FWD=aF2;
    aF2FWD.Orientation(TopAbs_FORWARD);
    //
    // 2. In, On parts processing
    const BOPTools_ListOfPaveBlock& aLPBInOn=aFF.PaveBlocks();
    anIt.Initialize(aLPBInOn);
    for (; anIt.More(); anIt.Next()) {
      const BOPTools_PaveBlock& aPB=anIt.Value();
      const BOPTools_PaveBlock& aPBInOn=pFiller->RealPaveBlock(aPB);
      //
      nE=aPBInOn.Edge();
      const TopoDS_Edge& aE=TopoDS::Edge(pDS->Shape(nE));
      
      BOPTools_Tools2D::BuildPCurveForEdgeOnFace(aE, aF1FWD);
      BOPTools_Tools2D::BuildPCurveForEdgeOnFace(aE, aF2FWD);
    }
  } //  for (i=1; i<=aNb; i++)
  myIsDone=Standard_True;
}

