// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
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

#include <GEOMAlgo_ShapeInfoFiller.hxx>

#include <Precision.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>

#include <gp_Lin.hxx>
#include <gp_XYZ.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <gp_Vec.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>

#include <ElCLib.hxx>

#include <GeomAdaptor_Surface.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>

#include <BRep_Tool.hxx>

#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

#include <TopTools_MapOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#include <BRepTools_WireExplorer.hxx>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>

#include <GEOMAlgo_ShapeInfo.hxx>

static
  Standard_Boolean IsEqual(const gp_Sphere& aSp1,
                           const gp_Sphere& aSp2,
                           const Standard_Real aTolDst);

//=======================================================================
//function : FillDetails
//purpose  :
//=======================================================================
void GEOMAlgo_ShapeInfoFiller::FillDetails(const TopoDS_Solid& aSd)
{
  Standard_Boolean bIsStepSphere;
  Standard_Integer i, aNbF, aNbCyl, aNbCon, aNbPgn, aNbRct;
  Standard_Integer aNbShells, aNbCrc, aNbX;
  TopoDS_Shape aFCyl, aFCon;
  TopTools_IndexedMapOfShape aMF;
  GEOMAlgo_KindOfName aKNF;
  GEOMAlgo_KindOfDef aKD;
  //
  GEOMAlgo_ShapeInfo& aInfo=myMapInfo.ChangeFromKey(aSd);
  aInfo.SetKindOfName(GEOMAlgo_KN_UNKNOWN);
  //
  TopExp::MapShapes(aSd, TopAbs_FACE, aMF);
  //
  aNbF=aMF.Extent();
  if (!aNbF) {
    return;
  }
  //
  //modified by NIZNHY-PKV Tue Jun 09 08:35:23 2015f
  if (aNbF==2) {
    // case requested by the customer
    // specific solid that should be treated as a sphere
    bIsStepSphere=TreatStepSphere(aSd);
    if (bIsStepSphere) {
      return;
    }
  }
  //modified by NIZNHY-PKV Tue Jun 09 08:35:28 2015t
  //
  aKD=GEOMAlgo_KD_SPECIFIED;
  for (i=1; i<=aNbF && aKD==GEOMAlgo_KD_SPECIFIED; ++i) {
    const TopoDS_Shape& aF=aMF(i);
    GEOMAlgo_ShapeInfo& aInfoF=myMapInfo.ChangeFromKey(aF);
    aKD=aInfoF.KindOfDef();
  }
  if (aKD!=GEOMAlgo_KD_SPECIFIED) {
    aInfo.SetKindOfName(GEOMAlgo_KN_SOLID); 
    return;
  }
  //
  aNbShells=GEOMAlgo_ShapeInfoFiller::NbShells(aSd);
  if (aNbShells>1) {
    aInfo.SetKindOfName(GEOMAlgo_KN_SOLID); 
    return;
  }
  //
  //
  if (aNbF==1) {
    // mb: sphere, torus
    const TopoDS_Shape& aF=aMF(1);
    GEOMAlgo_ShapeInfo& aInfoF=myMapInfo.ChangeFromKey(aF);
    aKNF=aInfoF.KindOfName(); // mb: sphere, torus
    if (aKNF==GEOMAlgo_KN_SPHERE ||
        aKNF==GEOMAlgo_KN_TORUS) {
      aInfo.SetKindOfName(aKNF);
      aInfo.SetLocation(aInfoF.Location());
      aInfo.SetPosition(aInfoF.Position());
      aInfo.SetRadius1(aInfoF.Radius1());
      if(aKNF==GEOMAlgo_KN_TORUS) {
        aInfo.SetRadius2(aInfoF.Radius2());
      }
      return;
    }
  }
  //modified by NIZNHY-PKV Tue Jun 09 08:36:08 2015f
  /*
  else if (aNbF==2) {
    // specific solid that should be treated as a sphere
    bIsStepSphere=TreatStepSphere(aSd);
    if (bIsStepSphere) {
      return;
    }
  }
  */
  //modified by NIZNHY-PKV Tue Jun 09 08:36:12 2015t
  //
  aNbCyl=0;
  aNbCon=0;
  aNbPgn=0;
  aNbRct=0;
  aNbCrc=0;
  for (i=1; i<=aNbF; ++i) {
    const TopoDS_Shape& aF=aMF(i);
    GEOMAlgo_ShapeInfo& aInfoF=myMapInfo.ChangeFromKey(aF);
    aKNF=aInfoF.KindOfName();
    if (aKNF==GEOMAlgo_KN_CYLINDER) {
      aFCyl=aF;
      ++aNbCyl;
    }
    else if (aKNF==GEOMAlgo_KN_CONE) {
      aFCon=aF;
      ++aNbCon;
    }
    else if (aKNF==GEOMAlgo_KN_DISKCIRCLE) {
      ++aNbCrc;
    }
    else if (aKNF==GEOMAlgo_KN_POLYGON ||
             aKNF==GEOMAlgo_KN_TRIANGLE ||
             aKNF==GEOMAlgo_KN_QUADRANGLE) {
      ++aNbPgn;
    }
    else if (aKNF==GEOMAlgo_KN_RECTANGLE) {
      ++aNbPgn;
      ++aNbRct;
    }
  }
  //
  aNbX=aNbCyl+aNbCrc;
  if (aNbCyl==1 && aNbCrc==2 && aNbX==aNbF) {
    // cylinder (as they understand it)
    GEOMAlgo_ShapeInfo& aInfoF=myMapInfo.ChangeFromKey(aFCyl);
    aKNF=aInfoF.KindOfName();
    aInfo.SetKindOfName(aKNF);
    aInfo.SetLocation(aInfoF.Location());
    aInfo.SetPosition(aInfoF.Position());
    aInfo.SetRadius1(aInfoF.Radius1());
    aInfo.SetHeight(aInfoF.Height());
    return;
  }
  //
  aNbX=aNbCon+aNbCrc;
  if (aNbCon==1 && (aNbCrc==1 || aNbCrc==2) && aNbX==aNbF) {
    // cone
    GEOMAlgo_ShapeInfo& aInfoF=myMapInfo.ChangeFromKey(aFCon);
    aKNF=aInfoF.KindOfName();
    aInfo.SetKindOfName(aKNF);
    aInfo.SetLocation(aInfoF.Location());
    aInfo.SetPosition(aInfoF.Position());
    aInfo.SetRadius1(aInfoF.Radius1());
    aInfo.SetRadius2(aInfoF.Radius2());
    aInfo.SetHeight(aInfoF.Height());
    return;
  }
  //
  if (aNbF!=aNbPgn) {
    return;// -> GEOMAlgo_KN_UNKNOWN
  }
  if (aNbPgn!=6) {
    aInfo.SetKindOfName(GEOMAlgo_KN_POLYHEDRON);
    return;
  }
  // aNbPgn==6
  if (aNbPgn!=aNbRct) {
    aInfo.SetKindOfName(GEOMAlgo_KN_POLYHEDRON);
    return;
  }
  //===================================================
  // aNbRct=6;
  // box
  Standard_Integer j, aNbFi, aNbV, iMax, iMin, iMid;
  Standard_Real aDot, aLength, aWidth, aHeight, aDist[3];
  Standard_Real aDistMin, aDistMax;
  gp_Pnt aPi, aPc;
  gp_Dir aDir[3];
  gp_XYZ aXYZc;
  TColStd_IndexedMapOfInteger aMp;
  TopTools_IndexedMapOfShape aMV, aMFi;
  //
  // barycenter aPc
  TopExp::MapShapes(aSd, TopAbs_VERTEX, aMV);
  aNbV=aMV.Extent();
  if (aNbV!=8) {
    return;
  }
  //
  aXYZc.SetCoord(0.,0.,0.);
  for (i=1; i<=aNbV; ++i) {
    const TopoDS_Vertex& aVi=TopoDS::Vertex(aMV(i));
    aPi=BRep_Tool::Pnt(aVi);
    const gp_XYZ& aXYZ=aPi.XYZ();
    aXYZc=aXYZc+aXYZ;
  }
  //
  aXYZc.Divide(aNbV);
  aPc.SetXYZ(aXYZc);
  //
  // 3 faces
  for (i=1; i<=aNbF; ++i) {
    if (aMp.Contains(i)) {
      continue;
    }
    //
    const TopoDS_Shape& aFi=aMF(i);
    const GEOMAlgo_ShapeInfo& aIFi=myMapInfo.FindFromKey(aFi);
    const gp_Dir& aDNi=aIFi.Position().Direction();
    //
    for (j=i+1; j<=aNbF; ++j) {
      if (aMp.Contains(j)) {
        continue;
      }
      //
      const TopoDS_Shape& aFj=aMF(j);
      const GEOMAlgo_ShapeInfo& aIFj=myMapInfo.FindFromKey(aFj);
      const gp_Dir& aDNj=aIFj.Position().Direction();
      //
      aDot=aDNi*aDNj;
      if (aDot<0.) {
        aDot=-aDot;
      }
      if (fabs(1.-aDot)<0.0001) {
        aMp.Add(i);
        aMp.Add(j);
        aMFi.Add(aFi);
        break;
      }
      //
    }
  }
  aNbFi=aMFi.Extent();
  if (aNbFi!=3) {
    return;
  }
  //
  iMin=-1;
  iMax=-1;
  aDistMin=1.e15;
  aDistMax=-aDistMin;
  for (i=0; i<aNbFi; ++i) {
    const TopoDS_Shape& aFi=aMFi(i+1);
    const GEOMAlgo_ShapeInfo& aIFi=myMapInfo.FindFromKey(aFi);
    aPi=aIFi.Location();
    aDist[i]=aPc.Distance(aPi);
    if (aDist[i]>aDistMax) {
      aDistMax=aDist[i];
      iMax=i;
    }
    if (aDist[i]<aDistMin) {
      aDistMin=aDist[i];
      iMin=i;
    }
    gp_Vec aVi(aPc, aPi);
    gp_Dir aDi(aVi);
    aDir[i]=aDi;
  }
  //
  if (iMax==iMin) {
    iMax=0;
    iMin=1;
  }
  iMid=3-iMax-iMin;
  //
  aLength=2.*aDist[iMax];
  aWidth=2.*aDist[iMid];
  aHeight=2.*aDist[iMin];
  //
  gp_Ax2 aAx2(aPc, aDir[iMin], aDir[iMax]);
  gp_Ax3 aAx3(aAx2);
  //
  aInfo.SetKindOfName(GEOMAlgo_KN_BOX);
  aInfo.SetLocation(aPc);
  aInfo.SetLength(aLength);
  aInfo.SetWidth(aWidth);
  aInfo.SetHeight(aHeight);
  aInfo.SetPosition(aAx3);
}
//=======================================================================
//function : FillDetails
//purpose  :
//=======================================================================
void GEOMAlgo_ShapeInfoFiller::FillDetails(const TopoDS_Face& aF,
                                           const gp_Pln& aPln)
{
  Standard_Integer aNbV, aNbE, i, j;
  Standard_Real aDot, aD0, aD1, aLength, aWidth;
  gp_Dir aDx[4], aDX;
  gp_Pnt aPx[4], aP, aPc;
  gp_XYZ aXYZc;
  TopExp_Explorer aExp;
  TopoDS_Shape aE;
  TopoDS_Wire aW;
  TopoDS_Edge aEx;
  TopoDS_Iterator aIt;
  TopTools_IndexedMapOfShape aMV;
  BRepTools_WireExplorer aWExp;
  GEOMAlgo_KindOfName aKN, aKNE;
  //
  GEOMAlgo_ShapeInfo& aInfo=myMapInfo.ChangeFromKey(aF);
  //
  aInfo.SetKindOfDef(GEOMAlgo_KD_ARBITRARY);
  //
  aNbV=aInfo.NbSubShapes(TopAbs_VERTEX);
  aNbE=aInfo.NbSubShapes(TopAbs_EDGE);
  //
  // 1. may be it is circle/ellipse
  if (aNbV==1 && aNbE==1) {
    aExp.Init(aF, TopAbs_EDGE);
    if (aExp.More()) {
      aE=aExp.Current();
    }
    //
    const GEOMAlgo_ShapeInfo& aInfoE=myMapInfo.FindFromKey(aE);
    aKNE=aInfoE.KindOfName();
    if (aKNE==GEOMAlgo_KN_CIRCLE) {
      aKN=GEOMAlgo_KN_DISKCIRCLE;
      aInfo.SetKindOfName(aKN);
      aInfo.SetRadius1(aInfoE.Radius1());
      aInfo.SetLocation(aInfoE.Location());
      aInfo.SetPosition(aInfoE.Position());
      aInfo.SetKindOfDef(GEOMAlgo_KD_SPECIFIED);
    }
    if (aKNE==GEOMAlgo_KN_ELLIPSE) {
      aKN=GEOMAlgo_KN_DISKELLIPSE;
      aInfo.SetKindOfName(aKN);
      aInfo.SetRadius1(aInfoE.Radius1());
      aInfo.SetRadius2(aInfoE.Radius2());
      aInfo.SetLocation(aInfoE.Location());
      aInfo.SetPosition(aInfoE.Position());
      aInfo.SetKindOfDef(GEOMAlgo_KD_SPECIFIED);
    }
    return;
  }// if (aNbV==1 && aNbE==1) {
  //
  //
  Standard_Boolean bSegment;
  //
  bSegment=Standard_True;
  aExp.Init(aF, TopAbs_EDGE);
  for (; aExp.More() && bSegment; aExp.Next()) {
    aE=aExp.Current();
    const GEOMAlgo_ShapeInfo& aInfoE=myMapInfo.FindFromKey(aE);
    aKNE=aInfoE.KindOfName();
    if (aKNE!=GEOMAlgo_KN_SEGMENT) {
      bSegment=!bSegment;
    }
  }
  //
  if (bSegment) {
  // 2. may be it is TRIANGLE, POLYGON, QUADRANGLE, RECTANGLE
    aInfo.SetKindOfDef(GEOMAlgo_KD_SPECIFIED);
    aInfo.SetKindOfName(GEOMAlgo_KN_POLYGON); 
    //
    if (aNbV==3 && aNbE==3) {
      aInfo.SetKindOfName(GEOMAlgo_KN_TRIANGLE);
      //
      aXYZc.SetCoord(0.,0.,0.);
      TopExp::MapShapes(aF, TopAbs_VERTEX, aMV);
      for (i=1; i<=aNbV; ++i) {
        const TopoDS_Vertex& aV=TopoDS::Vertex(aMV(i));
        aP=BRep_Tool::Pnt(aV);
        const gp_XYZ& aXYZ=aP.XYZ();
        aXYZc=aXYZc+aXYZ;
        aPx[i-1]=aP;
      }
      aXYZc.Divide(3.);
      //
      aPc.SetXYZ(aXYZc);
      gp_Vec aVX(aPc, aPx[0]);
      aVX.Normalize();
      aDX.SetXYZ(aVX.XYZ());
      const gp_Dir& aDZ=aPln.Axis().Direction();
      //
      gp_Ax2 aAx2(aPc, aDZ, aDX);
      gp_Ax3 aAx3(aAx2);
      //
      aInfo.SetLocation(aPc);
      aInfo.SetPosition(aAx3);
    } // if (aNbV==3 && aNbE==3) {
    //
    if (aNbV==4 && aNbE==4) {
      aIt.Initialize(aF);
      if (aIt.More()) {
        aW=*((TopoDS_Wire*)&aIt.Value());
      }
      //
      aWExp.Init(aW, aF);
      for (i=0; aWExp.More(); aWExp.Next(), ++i) {
        aEx=aWExp.Current();
        const GEOMAlgo_ShapeInfo& aInfoEx=myMapInfo.FindFromKey(aEx);
        aDx[i]=aInfoEx.Direction();
        aPx[i]=aInfoEx.Location();
      }
      //
      Standard_Boolean isRectangle = Standard_True;
      for (i=0; i<4; ++i) {
        j=(i==3) ? 0 : i+1;
        aDot=aDx[i]*aDx[j];
        if (fabs (aDot) > myTolerance) {
          isRectangle = Standard_False;
          break;
        }
      }
      //
      // rectangle
      // shift location to the center
      aXYZc.SetCoord(0.,0.,0.);
      TopExp::MapShapes(aF, TopAbs_VERTEX, aMV);
      for (i=1; i<=aNbV; ++i) {
        const TopoDS_Vertex& aV=TopoDS::Vertex(aMV(i));
        aP=BRep_Tool::Pnt(aV);
        const gp_XYZ& aXYZ=aP.XYZ();
        aXYZc=aXYZc+aXYZ;
      }
      //
      // Location : aPc in center of rectangle
      // Position : 0z is plane normal
      //            0x is along the first edge (quadrangle) or
      //            along length (rectangle)
      //
      aXYZc.Divide(4.);
      aPc.SetXYZ(aXYZc);
      aDX=aDx[0];
      aInfo.SetLocation(aPc);

      if (isRectangle) {
      // Calculate sizes
        gp_Lin aL0(aPx[0], aDx[0]);
        gp_Lin aL1(aPx[1], aDx[1]);
        //
        aD0=aL0.Distance(aPc);
        aD1=aL1.Distance(aPc);
        //
        aLength=aD1;
        aWidth =aD0;
        
        if (aD0>aD1) {
          aLength=aD0;
          aWidth =aD1;
          aDX=aDx[1];
        }
        //
        aLength=2.*aLength;
        aWidth =2.*aWidth;
        //
        aInfo.SetLength(aLength);
        aInfo.SetWidth(aWidth);
        aInfo.SetKindOfName(GEOMAlgo_KN_RECTANGLE);
      } else {
        aInfo.SetKindOfName(GEOMAlgo_KN_QUADRANGLE);
      }
      //
      const gp_Dir& aDZ=aPln.Axis().Direction();
      gp_Ax2 aAx2(aPc, aDZ, aDX);
      gp_Ax3 aAx3(aAx2);
      aInfo.SetPosition(aAx3);
      //
    }// if (aNbV==4 && aNbE==4) {
    return;
  }// if (bSegment) {
  //
  //aInfo.SetKindOfName(GEOMAlgo_KN_PLANE);
}
//=======================================================================
//function : FillDetails
//purpose  :
//=======================================================================
void GEOMAlgo_ShapeInfoFiller::FillDetails(const TopoDS_Face& aF,
                                           const gp_Sphere& )//aSph)
{
  
  Standard_Integer aNbV, aNbE, aNbSE, aNbDE;
  TopoDS_Edge aE;
  TopExp_Explorer aExp;
  TopTools_MapOfShape aM;
  GEOMAlgo_KindOfShape aKSE;//, aKSE;
  //
  GEOMAlgo_ShapeInfo& aInfo=myMapInfo.ChangeFromKey(aF);
  // 
  aInfo.SetKindOfDef(GEOMAlgo_KD_ARBITRARY);
  aNbV=aInfo.NbSubShapes(TopAbs_VERTEX);
  aNbE=aInfo.NbSubShapes(TopAbs_EDGE);
  if (aNbV==2 && aNbE==3) {
    aNbSE=0;
    aNbDE=0;
    aExp.Init(aF, TopAbs_EDGE);
    for (; aExp.More(); aExp.Next()) {
      aE=TopoDS::Edge(aExp.Current());
      if(aM.Add(aE)) {
        const GEOMAlgo_ShapeInfo& aInfoE=myMapInfo.FindFromKey(aE);
        aKSE=aInfoE.KindOfShape();
        //
        if (BRep_Tool::IsClosed(aE, aF)) {
          ++aNbSE;
        }
        else if (aKSE==GEOMAlgo_KS_DEGENERATED) {
          ++aNbDE;
        }
      }
    }
    //
    if (aNbSE==1 && aNbDE==2) {
      aInfo.SetKindOfDef(GEOMAlgo_KD_SPECIFIED);
    }
  }
}
//=======================================================================
//function : FillDetails
//purpose  :
//=======================================================================
void GEOMAlgo_ShapeInfoFiller::FillDetails(const TopoDS_Face& aF,
                                           const gp_Cylinder& aCyl)
     
{
  Standard_Integer i, aNbV, aNbE, aNbCE, aNbSE;
  Standard_Real aT0, aT1, aHeight;
  gp_Pnt aPC[3], aPc;
  TopoDS_Edge aE;
  TopExp_Explorer aExp;
  TopTools_MapOfShape aM;
  GEOMAlgo_KindOfName aKNE;
  GEOMAlgo_KindOfClosed aKCE;
  //
  GEOMAlgo_ShapeInfo& aInfo=myMapInfo.ChangeFromKey(aF);
  //
  aInfo.SetKindOfDef(GEOMAlgo_KD_ARBITRARY);
  aNbV=aInfo.NbSubShapes(TopAbs_VERTEX);
  aNbE=aInfo.NbSubShapes(TopAbs_EDGE);
  if (aNbV==2 && aNbE==3) {
    const gp_Ax1& aAx1=aCyl.Axis();
    const gp_Dir& aDir=aAx1.Direction();
    const gp_Pnt& aPLoc=aAx1.Location();
    //
    i=0;
    aNbCE=0;
    aNbSE=0;
    aExp.Init(aF, TopAbs_EDGE);
    for (; aExp.More(); aExp.Next()) {
      aE=TopoDS::Edge(aExp.Current());
      if(aM.Add(aE)) {
        const GEOMAlgo_ShapeInfo& aInfoE=myMapInfo.FindFromKey(aE);
        aKNE=aInfoE.KindOfName();
        aKCE=aInfoE.KindOfClosed();
        if (aKNE==GEOMAlgo_KN_CIRCLE && aKCE==GEOMAlgo_KC_CLOSED) {
          aPC[aNbCE]=aInfoE.Location();
          ++aNbCE;
        }
        else if (aKNE==GEOMAlgo_KN_SEGMENT) {
          if (BRep_Tool::IsClosed(aE, aF)) {
            ++aNbSE;
          }
        }
      }
    }
    //
    if (aNbCE==2 && aNbSE==1) {
      gp_Lin aLin(aPLoc, aDir);
      //
      aT0=ElCLib::Parameter(aLin, aPC[0]);
      aT1=ElCLib::Parameter(aLin, aPC[1]);
      //
      aPc=aPC[0];
      if (aT0>aT1) {
        aPc=aPC[1];
      }
      aHeight=aPC[0].Distance(aPC[1]);
      //
      gp_Ax3 aAx3=aCyl.Position();
      aAx3.SetLocation(aPc);
      //
      aInfo.SetPosition(aAx3);
      aInfo.SetLocation(aPc);
      aInfo.SetHeight(aHeight);
      //
      aInfo.SetKindOfDef(GEOMAlgo_KD_SPECIFIED);
      return; // conventional cylinder
    }//if (aNbCE==2 && aNbSE==1) {
  }//if (aNbV==2 && aNbE==3) {
}
//=======================================================================
//function : FillDetails
//purpose  :
//=======================================================================
void GEOMAlgo_ShapeInfoFiller::FillDetails(const TopoDS_Face& aF,
                                           const gp_Cone& aCone)
{
  Standard_Integer aNbV, aNbE, aNbCE, aNbSE, aNbDE, i;
  Standard_Real aR[3], aHeight, aRmin, aRmax;
  gp_Pnt aPC[3], aPD, aPc, aPX[3];
  TopoDS_Vertex aVD;
  TopoDS_Edge aE;
  TopoDS_Iterator aIt;
  TopExp_Explorer aExp;
  TopTools_MapOfShape aM;
  GEOMAlgo_KindOfShape aKSE;
  GEOMAlgo_KindOfName aKNE;
  GEOMAlgo_KindOfClosed aKCE;
  //
  GEOMAlgo_ShapeInfo& aInfo=myMapInfo.ChangeFromKey(aF);
  //
  aInfo.SetKindOfDef(GEOMAlgo_KD_ARBITRARY);
  //
  aNbV=aInfo.NbSubShapes(TopAbs_VERTEX);
  aNbE=aInfo.NbSubShapes(TopAbs_EDGE);
  if (aNbV==2 && aNbE==3) {
    i=0;
    aNbCE=0;
    aNbSE=0;
    aNbDE=0;
    aExp.Init(aF, TopAbs_EDGE);
    for (; aExp.More(); aExp.Next()) {
      aE=TopoDS::Edge(aExp.Current());
      if(aM.Add(aE)) {
        const GEOMAlgo_ShapeInfo& aInfoE=myMapInfo.FindFromKey(aE);
        aKNE=aInfoE.KindOfName();
        aKCE=aInfoE.KindOfClosed();
        aKSE=aInfoE.KindOfShape();
        if (aKNE==GEOMAlgo_KN_CIRCLE && aKCE==GEOMAlgo_KC_CLOSED) {
          aPC[i]=aInfoE.Location();
          aR[i]=aInfoE.Radius1();
          //
          aIt.Initialize(aE);
          if (aIt.More()) {
            aVD=*((TopoDS_Vertex*)&aIt.Value());
          }
          aPX[i]=BRep_Tool::Pnt(aVD);
          //
          ++i;
          ++aNbCE;
        }
        else if (aKNE==GEOMAlgo_KN_SEGMENT) {
          if (BRep_Tool::IsClosed(aE, aF)) {
            ++aNbSE;
          }
        }
        else if (aKSE==GEOMAlgo_KS_DEGENERATED) {
          aIt.Initialize(aE);
          if (aIt.More()) {
            aVD=*((TopoDS_Vertex*)&aIt.Value());
          }
          //
          aPD=BRep_Tool::Pnt(aVD);
          //
          ++aNbDE;
        }
      }
    }
    //
    if ((aNbCE==2 || (aNbCE==1 && aNbDE==1)) && aNbSE==1) {
      if (aNbDE==1) {
        aPC[1]=aPD;
        aR[1]=0.;
      }
      //
      aHeight=aPC[0].Distance(aPC[1]);
      //
      
      gp_Ax2 aAx2new;
      //
      if (aR[0]>aR[1]) {
        aRmin=aR[1];
        aRmax=aR[0];
        aPc=aPC[0];
        gp_Vec aVz(aPC[0], aPC[1]);
        gp_Vec aVx(aPC[0], aPX[0]);
        gp_Dir aDz(aVz);
        gp_Dir aDx(aVx);
        gp_Ax2 aAx2(aPc, aDz, aDx);
        aAx2new=aAx2;
      }
      else {
        aRmin=aR[0];
        aRmax=aR[1];
        aPc=aPC[1];
        gp_Vec aVz(aPC[1], aPC[0]);
        gp_Vec aVx(aPC[1], aPX[1]);
        gp_Dir aDz(aVz);
        gp_Dir aDx(aVx);
        gp_Ax2 aAx2(aPc, aDz, aDx);
        aAx2new=aAx2;
      }
      //
      gp_Ax3 aAx3(aAx2new);
      aInfo.SetLocation(aPc);
      aInfo.SetPosition(aAx3);
      aInfo.SetRadius1(aRmax);
      aInfo.SetRadius2(aRmin);
      aInfo.SetHeight(aHeight);
      //
      aInfo.SetKindOfDef(GEOMAlgo_KD_SPECIFIED);
      return;
    }//if ((aNbCE==2 || (aNbCE==1 && aNbDE==1)) && aNbSE==1) {
  }//if (aNbV==2 && aNbE==3) {
  //
  aInfo.SetRadius1 (aCone.RefRadius());
  //
  aRmin=0.;   // ZZ
  aInfo.SetRadius2(aRmin);
}
//=======================================================================
//function : FillDetails
//purpose  :
//=======================================================================
void GEOMAlgo_ShapeInfoFiller::FillDetails(const TopoDS_Face& aF,
                                           const gp_Torus& )
{
  
  Standard_Integer aNbV, aNbE, aNbSE;
  TopoDS_Edge aE;
  TopExp_Explorer aExp;
  TopTools_MapOfShape aM;
  GEOMAlgo_KindOfShape aKS;
  //
  GEOMAlgo_ShapeInfo& aInfo=myMapInfo.ChangeFromKey(aF);
  aInfo.SetKindOfDef(GEOMAlgo_KD_ARBITRARY);
  //
  aKS=aInfo.KindOfShape();
  if (aKS!=GEOMAlgo_KS_TORUS) {
    return;
  }
  
  aNbV=aInfo.NbSubShapes(TopAbs_VERTEX);
  aNbE=aInfo.NbSubShapes(TopAbs_EDGE); 
  
  if (aNbV==1 && aNbE==2) {
    aNbSE=0;
    aExp.Init(aF, TopAbs_EDGE);
    for (; aExp.More(); aExp.Next()) {
      aE=TopoDS::Edge(aExp.Current());
      if (aM.Add(aE)) {
        if (BRep_Tool::IsClosed(aE, aF)) {
          ++aNbSE;
        }
      }
    }
    //
    if (aNbSE==2) {
      aInfo.SetKindOfDef(GEOMAlgo_KD_SPECIFIED);
    }
  }
}
//=======================================================================
//function : TreatStepSphere
//purpose  :
//=======================================================================
Standard_Boolean  GEOMAlgo_ShapeInfoFiller::TreatStepSphere
  (const TopoDS_Solid& aSd)
{
  Standard_Boolean bRet, bIsAllowedType, bOnlyClosed, bIsEqual;
  Standard_Integer j;
  Standard_Real aTolAng, aTol;
  Standard_Real aVolume, aVolumeS, dV, aArea, aAreaS, dA;
  gp_Sphere aSphere[2];
  GeomAbs_SurfaceType aST;
  Handle(Geom_Surface) aS;
  GeomAdaptor_Surface aGAS;
  TopExp_Explorer aExp;
  //
  bRet=Standard_False;
  aTol=Precision::Confusion();
  aTolAng=Precision::Angular();
  //
  aExp.Init(aSd, TopAbs_FACE);
  for (j=0; aExp.More(); aExp.Next(), ++j) {
    const TopoDS_Face& aF=*((TopoDS_Face*)&aExp.Current());
    aS=BRep_Tool::Surface(aF);
    aGAS.Load(aS);
    aST=aGAS.GetType();
    bIsAllowedType=GEOMAlgo_ShapeInfoFiller::IsAllowedType(aST);
    if (!bIsAllowedType) {
      return bRet;
    }
    //
    if (aST!=GeomAbs_Sphere) {
      return bRet;
    }
    //
    aSphere[j]=aGAS.Sphere();
  }
  //
  bIsEqual=IsEqual(aSphere[0], aSphere[1], aTol);
  if (!bIsEqual) {
    return bRet;
  }
  //
  //--------------------------------
  GProp_GProps aGProps;
  //
  bOnlyClosed=Standard_False;
  //
  aVolume=aSphere[0].Volume();
  //
  //modified by NIZNHY-PKV Tue Jun 09 08:39:47 2015f
  BRepGProp::VolumeProperties(aSd, aGProps, aTol,  bOnlyClosed);
  //BRepGProp::VolumeProperties(aSd, aGProps,  bOnlyClosed);
  //modified by NIZNHY-PKV Tue Jun 09 08:39:50 2015t
  aVolumeS=aGProps.Mass();
  if (aVolumeS<0.) {
    aVolumeS=-aVolumeS;
  }
  //
  dV=fabs(aVolumeS-aVolume);
  if (dV>aTol) {
    return bRet;
  }
  //--------------------------------
  aArea=aSphere[0].Area();
  //
  //modified by NIZNHY-PKV Tue Jun 09 08:23:54 2015f
  BRepGProp::SurfaceProperties(aSd, aGProps, aTol);
  //BRepGProp::SurfaceProperties(aSd, aGProps);
  //modified by NIZNHY-PKV Tue Jun 09 08:23:56 2015t
  aAreaS=aGProps.Mass();
  //
  dA=fabs(aAreaS-aArea);
  if (dA>aTol) {
    return bRet;
  }
  //
  //--------------------------------
  gp_Pnt aP0;
  gp_Ax3 aAx3;
  Standard_Real aR1;
  //
  aP0=aSphere[0].Location();
  aAx3=aSphere[0].Position();
  aR1=aSphere[0].Radius();
  //
  GEOMAlgo_ShapeInfo& aInfo=myMapInfo.ChangeFromKey(aSd);
  //
  aInfo.SetKindOfName(GEOMAlgo_KN_SPHERE);
  aInfo.SetLocation(aP0);
  aInfo.SetPosition(aAx3);
  aInfo.SetRadius1(aR1);
  //
  return !bRet;// true
}
//=======================================================================
//function : IsEqual
//purpose  :
//=======================================================================
Standard_Boolean IsEqual(const gp_Sphere& aSp1,
                         const gp_Sphere& aSp2,
                         const Standard_Real aTolLin)
{
  Standard_Boolean bRet;
  Standard_Real aR1, aR2, aD2;
  //
  bRet=Standard_False;
  aR1=aSp1.Radius();
  aR2=aSp2.Radius();
  if (fabs(aR1-aR2)>aTolLin) {
    return bRet;
  }
  //
  const gp_Pnt& aPC1=aSp1.Position().Location();
  const gp_Pnt& aPC2=aSp2.Position().Location();
  //
  aD2=aPC1.SquareDistance(aPC2);
  bRet=(aD2<aTolLin*aTolLin);
  //
  return bRet;
}
