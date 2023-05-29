// Copyright (C) 2007-2022  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include <GEOMAlgo_ShapeInfo.hxx>

static
  Standard_Integer TypeToInteger(const TopAbs_ShapeEnum aType);
static
  void DumpKindOfShape(const GEOMAlgo_KindOfShape aKS);
static
  void DumpKindOfClosed(const GEOMAlgo_KindOfClosed aKC);
static
  void DumpKindOfBounds(const GEOMAlgo_KindOfBounds aKB);
static
  void DumpKindOfName(const GEOMAlgo_KindOfName aKS);
static
  void DumpKindOfDef(const GEOMAlgo_KindOfDef aKD);
static
  void DumpKindOfPeriod(const GEOMAlgo_KindOfPeriod aKP);
static
  void DumpPosition(const gp_Ax3& aAx3);
static
  void DumpLocation(const gp_Pnt& aP);
static
  void DumpDirection(const gp_Dir& aD);
//=======================================================================
//function :
//purpose  :
//=======================================================================
GEOMAlgo_ShapeInfo::GEOMAlgo_ShapeInfo()
{
  Reset();
}
//=======================================================================
//function : ~
//purpose  :
//=======================================================================
GEOMAlgo_ShapeInfo::~GEOMAlgo_ShapeInfo()
{
}
//=======================================================================
//function : Reset
//purpose  :
//=======================================================================
void GEOMAlgo_ShapeInfo::Reset()
{
  Standard_Integer i;
  //
  myType=TopAbs_SHAPE;
  //
  myNbTypes=9;
  for (i=0; i<myNbTypes; ++i) {
    myNbSubShapes[i]=0;
  }
  //
  myKindOfShape=GEOMAlgo_KS_UNKNOWN;
  myKindOfBounds=GEOMAlgo_KB_UNKNOWN;
  myKindOfClosed=GEOMAlgo_KC_UNKNOWN;
  myKindOfName=GEOMAlgo_KN_UNKNOWN;
  myKindOfDef=GEOMAlgo_KD_UNKNOWN;
  myKindOfPeriod=GEOMAlgo_KP_UNKNOWN;
  //
  myLocation.SetCoord(99., 99., 99.);
  myDirection.SetCoord(1.,0.,0.);
  //
  myRadius1=-1.;
  myRadius2=-2.;
  myLength=-3.;
  myWidth=-3.;
  myHeight=-3.;
  myDegree=0;
  myNbPoles=0;
  myNbKnots=0;
  myNbWeights=0;
  myNbMultiplicities=0;
  myPoles = Handle(TColgp_HArray1OfPnt)();
  myKnots = Handle(TColStd_HArray1OfReal)();
  myWeights = Handle(TColStd_HArray1OfReal)();
  myMultiplicities = Handle(TColStd_HArray1OfInteger)();
}
//=======================================================================
//function : SetType
//purpose  :
//=======================================================================
void GEOMAlgo_ShapeInfo::SetType(const TopAbs_ShapeEnum aType)
{
  myType=aType;
}
//=======================================================================
//function : Type
//purpose  :
//=======================================================================
TopAbs_ShapeEnum GEOMAlgo_ShapeInfo::Type() const
{
  return myType;
}
//=======================================================================
//function : SetNbSubShapes
//purpose  :
//=======================================================================
void GEOMAlgo_ShapeInfo::SetNbSubShapes(const TopAbs_ShapeEnum aType,
                                        const Standard_Integer aNb)
{
  Standard_Integer iN;

  iN=TypeToInteger(aType);
  if (iN>=0 && iN<myNbTypes) {
    myNbSubShapes[iN]=aNb;
  }
}
//=======================================================================
//function : NbSubShapes
//purpose  :
//=======================================================================
Standard_Integer GEOMAlgo_ShapeInfo::NbSubShapes
  (const TopAbs_ShapeEnum aType) const
{
  Standard_Integer iN;

  iN=TypeToInteger(aType);
  if (iN>=0 && iN<myNbTypes) {
    return myNbSubShapes[iN];
  }
  return 0;
}
//=======================================================================
//function : SetKindOfShape
//purpose  :
//=======================================================================
void GEOMAlgo_ShapeInfo::SetKindOfShape(const GEOMAlgo_KindOfShape aT)
{
  myKindOfShape=aT;
}
//=======================================================================
//function : KindOfShape
//purpose  :
//=======================================================================
GEOMAlgo_KindOfShape GEOMAlgo_ShapeInfo::KindOfShape() const
{
  return myKindOfShape;
}
//=======================================================================
//function : SetKindOfName
//purpose  :
//=======================================================================
void GEOMAlgo_ShapeInfo::SetKindOfName(const GEOMAlgo_KindOfName aT)
{
  myKindOfName=aT;
}
//=======================================================================
//function : KindOfName
//purpose  :
//=======================================================================
GEOMAlgo_KindOfName GEOMAlgo_ShapeInfo::KindOfName() const
{
  return myKindOfName;
}
//=======================================================================
//function : SetKindOfBounds
//purpose  :
//=======================================================================
void GEOMAlgo_ShapeInfo::SetKindOfBounds(const GEOMAlgo_KindOfBounds aT)
{
  myKindOfBounds=aT;
}
//=======================================================================
//function : KindOfBounds
//purpose  :
//=======================================================================
GEOMAlgo_KindOfBounds GEOMAlgo_ShapeInfo::KindOfBounds() const
{
  return myKindOfBounds;
}
//=======================================================================
//function : SetKindOfClosed
//purpose  :
//=======================================================================
void GEOMAlgo_ShapeInfo::SetKindOfClosed(const GEOMAlgo_KindOfClosed aT)
{
  myKindOfClosed=aT;
}
//=======================================================================
//function : KindOfClosed
//purpose  :
//=======================================================================
GEOMAlgo_KindOfClosed GEOMAlgo_ShapeInfo::KindOfClosed() const
{
  return myKindOfClosed;
}
//=======================================================================
//function : SetKindOfDef
//purpose  :
//=======================================================================
void GEOMAlgo_ShapeInfo::SetKindOfDef(const GEOMAlgo_KindOfDef aT)
{
  myKindOfDef=aT;
}
//=======================================================================
//function : KindOfDef
//purpose  :
//=======================================================================
GEOMAlgo_KindOfDef GEOMAlgo_ShapeInfo::KindOfDef() const
{
  return myKindOfDef;
}
//=======================================================================
//function : SetKindOfPeriod
//purpose  :
//=======================================================================
void GEOMAlgo_ShapeInfo::SetKindOfPeriod(const GEOMAlgo_KindOfPeriod aT)
{
  myKindOfPeriod=aT;
}
//=======================================================================
//function : KindOfPeriod
//purpose  :
//=======================================================================
GEOMAlgo_KindOfPeriod GEOMAlgo_ShapeInfo::KindOfPeriod() const
{
  return myKindOfPeriod;
}
//=======================================================================
//function : SetLocation
//purpose  :
//=======================================================================
void GEOMAlgo_ShapeInfo::SetLocation(const gp_Pnt& aP)
{
  myLocation=aP;
}
//=======================================================================
//function : Location
//purpose  :
//=======================================================================
const gp_Pnt& GEOMAlgo_ShapeInfo::Location() const
{
  return myLocation;
}
//=======================================================================
//function : SetDirection
//purpose  :
//=======================================================================
void GEOMAlgo_ShapeInfo::SetDirection(const gp_Dir& aD)
{
  myDirection=aD;
}
//=======================================================================
//function : Direction
//purpose  :
//=======================================================================
const gp_Dir& GEOMAlgo_ShapeInfo::Direction() const
{
  return myDirection;
}
//=======================================================================
//function : SetPosition
//purpose  :
//=======================================================================
void GEOMAlgo_ShapeInfo::SetPosition(const gp_Ax2& aAx2)
{
  gp_Ax3 aAx3(aAx2);
  SetPosition(aAx3);
}
//=======================================================================
//function : SetPosition
//purpose  :
//=======================================================================
void GEOMAlgo_ShapeInfo::SetPosition(const gp_Ax3& aAx3)
{
  myPosition=aAx3;
}
//=======================================================================
//function : Position
//purpose  :
//=======================================================================
const gp_Ax3& GEOMAlgo_ShapeInfo::Position() const
{
  return myPosition;
}

//=======================================================================
//function : SetPnt1
//purpose  :
//=======================================================================
void GEOMAlgo_ShapeInfo::SetPnt1(const gp_Pnt& aP)
{
  myPnt1=aP;
}
//=======================================================================
//function : Pnt1
//purpose  :
//=======================================================================
const gp_Pnt& GEOMAlgo_ShapeInfo::Pnt1() const
{
  return myPnt1;
}
//=======================================================================
//function : SetPnt2
//purpose  :
//=======================================================================
void GEOMAlgo_ShapeInfo::SetPnt2(const gp_Pnt& aP)
{
  myPnt2=aP;
}
//=======================================================================
//function : Pnt2
//purpose  :
//=======================================================================
const gp_Pnt& GEOMAlgo_ShapeInfo::Pnt2() const
{
  return myPnt2;
}
//=======================================================================
//function : SetRadius1
//purpose  :
//=======================================================================
void GEOMAlgo_ShapeInfo::SetRadius1(const Standard_Real aR)
{
  myRadius1=aR;
}
//=======================================================================
//function : Radius1
//purpose  :
//=======================================================================
Standard_Real GEOMAlgo_ShapeInfo::Radius1() const
{
  return myRadius1;
}
//=======================================================================
//function : SetRadius2
//purpose  :
//=======================================================================
void GEOMAlgo_ShapeInfo::SetRadius2(const Standard_Real aR)
{
  myRadius2=aR;
}
//=======================================================================
//function : Radius2
//purpose  :
//=======================================================================
Standard_Real GEOMAlgo_ShapeInfo::Radius2() const
{
  return myRadius2;
}
//=======================================================================
//function : SetLength
//purpose  :
//=======================================================================
void GEOMAlgo_ShapeInfo::SetLength(const Standard_Real aL)
{
  myLength=aL;
}
//=======================================================================
//function : Length
//purpose  :
//=======================================================================
Standard_Real GEOMAlgo_ShapeInfo::Length() const
{
  return myLength;
}
//=======================================================================
//function : SetWidth
//purpose  :
//=======================================================================
void GEOMAlgo_ShapeInfo::SetWidth(const Standard_Real aW)
{
  myWidth=aW;
}
//=======================================================================
//function : Width
//purpose  :
//=======================================================================
Standard_Real GEOMAlgo_ShapeInfo::Width() const
{
  return myWidth;
}
//=======================================================================
//function : SetHeight
//purpose  :
//=======================================================================
void GEOMAlgo_ShapeInfo::SetHeight(const Standard_Real aH)
{
  myHeight=aH;
}
//=======================================================================
//function : Height
//purpose  :
//=======================================================================
Standard_Real GEOMAlgo_ShapeInfo::Height() const
{
  return myHeight;
}
//=======================================================================
//function : SetDegree
//purpose  :
//=======================================================================
void GEOMAlgo_ShapeInfo::SetDegree(const Standard_Integer aD)
{
  myDegree=aD;
}
//=======================================================================
//function : Degree
//purpose  :
//=======================================================================
Standard_Integer GEOMAlgo_ShapeInfo::Degree() const
{
  return myDegree;
}
//=======================================================================
//function : SetNbPoles
//purpose  :
//=======================================================================
void GEOMAlgo_ShapeInfo::SetNbPoles(const Standard_Integer aNb)
{
  myNbPoles=aNb;
}
//=======================================================================
//function : NbPoles
//purpose  :
//=======================================================================
Standard_Integer GEOMAlgo_ShapeInfo::NbPoles() const
{
  return myNbPoles;
}
//=======================================================================
//function : SetNbKnots
//purpose  :
//=======================================================================
void GEOMAlgo_ShapeInfo::SetNbKnots(const Standard_Integer aNb)
{
  myNbKnots=aNb;
}
//=======================================================================
//function : NbKnots
//purpose  :
//=======================================================================
Standard_Integer GEOMAlgo_ShapeInfo::NbKnots() const
{
  return myNbKnots;
}
//=======================================================================
//function : SetNbWeights
//purpose  :
//=======================================================================
void GEOMAlgo_ShapeInfo::SetNbWeights(const Standard_Integer aNb)
{
  myNbWeights=aNb;
}
//=======================================================================
//function : NbWeights
//purpose  :
//=======================================================================
Standard_Integer GEOMAlgo_ShapeInfo::NbWeights() const
{
  return myNbWeights;
}
//=======================================================================
//function : SetNbMultiplicities
//purpose  :
//=======================================================================
void GEOMAlgo_ShapeInfo::SetNbMultiplicities(const Standard_Integer aNb)
{
  myNbMultiplicities=aNb;
}
//=======================================================================
//function : NbWeights
//purpose  :
//=======================================================================
Standard_Integer GEOMAlgo_ShapeInfo::NbMultiplicities() const
{
  return myNbMultiplicities;
}
//=======================================================================
//function : SetPoles
//purpose  :
//=======================================================================
void GEOMAlgo_ShapeInfo::SetPoles(Handle(TColgp_HArray1OfPnt) P)
{
  myPoles = P;
}
//=======================================================================
//function : Poles
//purpose  :
//=======================================================================
Handle(TColgp_HArray1OfPnt) GEOMAlgo_ShapeInfo::Poles() const
{
  return myPoles;
}
//=======================================================================
//function : SetKnots
//purpose  :
//=======================================================================
void GEOMAlgo_ShapeInfo::SetKnots(Handle(TColStd_HArray1OfReal) K)
{
  myKnots = K;
}
//=======================================================================
//function : Knots
//purpose  :
//=======================================================================
Handle(TColStd_HArray1OfReal) GEOMAlgo_ShapeInfo::Knots() const
{
  return myKnots;
}
//=======================================================================
//function : SetWeights
//purpose  :
//=======================================================================
void GEOMAlgo_ShapeInfo::SetWeights(Handle(TColStd_HArray1OfReal) W)
{
  myWeights = W;
}
//=======================================================================
//function : Weights
//purpose  :
//=======================================================================
Handle(TColStd_HArray1OfReal) GEOMAlgo_ShapeInfo::Weights() const
{
  return myWeights;
}
//=======================================================================
//function : SetMultiplicities
//purpose  :
//=======================================================================
void GEOMAlgo_ShapeInfo::SetMultiplicities(Handle(TColStd_HArray1OfInteger) M)
{
  myMultiplicities = M;
}
//=======================================================================
//function : Multiplicities
//purpose  :
//=======================================================================
Handle(TColStd_HArray1OfInteger) GEOMAlgo_ShapeInfo::Multiplicities() const
{
  return myMultiplicities;
}
//=======================================================================
//function : TypeToInteger
//purpose  :
//=======================================================================
Standard_Integer TypeToInteger(const TopAbs_ShapeEnum aType)
{
  Standard_Integer iN;
  //
  iN=(Standard_Integer)aType;
  return iN;
}
//=======================================================================
//function : Dump
//purpose  :
//=======================================================================
void GEOMAlgo_ShapeInfo::Dump()const
{
  switch (myType) {
    //
  case TopAbs_VERTEX:
    DumpVertex();
    break;
    //
  case TopAbs_EDGE:
    DumpEdge();
    break;
    //
  case TopAbs_WIRE:
    DumpWire();
    break;
    //
  case TopAbs_FACE:
    DumpFace();
    break;
    //
  case TopAbs_SHELL:
    DumpShell();
    break;
    //
  case TopAbs_SOLID:
    DumpSolid();
    break;
    //
  case TopAbs_COMPSOLID:
    DumpCompSolid();
    break;
    //
  case TopAbs_COMPOUND:
    DumpCompound();
    break;
    //
  default:
    printf(" * not implemented yet\n");
    break;
  }
}
//=======================================================================
//function : DumpCompound
//purpose  :
//=======================================================================
void GEOMAlgo_ShapeInfo::DumpCompound()const
{
  Standard_Integer aNbV, aNbE, aNbF, aNbS, aNbC, aNbP;
  GEOMAlgo_KindOfShape aKS;
  GEOMAlgo_KindOfName aKN;
  GEOMAlgo_KindOfBounds aKB;
  GEOMAlgo_KindOfClosed aKC;
  //
  aNbV=NbSubShapes(TopAbs_VERTEX);
  aNbE=NbSubShapes(TopAbs_EDGE);
  aNbF=NbSubShapes(TopAbs_FACE);
  aNbS=NbSubShapes(TopAbs_SOLID);
  aNbC=NbSubShapes(TopAbs_COMPSOLID);
  aNbP=NbSubShapes(TopAbs_COMPOUND);
  aKS=KindOfShape();
  aKN=KindOfName();
  aKB=KindOfBounds();
  aKC=KindOfClosed();
  //
  printf(" *COMPOUND\n");
  printf("  number of vertices    : %d\n", aNbV);
  printf("  number of edges       : %d\n", aNbE);
  printf("  number of faces       : %d\n", aNbF);
  printf("  number of solids      : %d\n", aNbS);
  printf("  number of compsolids  : %d\n", aNbC);
  printf("  number of compounds   : %d\n", aNbP);
  DumpKindOfShape (aKS);
  DumpKindOfName (aKN);
  DumpKindOfBounds(aKB);
  DumpKindOfClosed(aKC);
}
//=======================================================================
//function : DumpCompSolid
//purpose  :
//=======================================================================
void GEOMAlgo_ShapeInfo::DumpCompSolid()const
{
  Standard_Integer aNbV, aNbE, aNbF, aNbS;
  GEOMAlgo_KindOfShape aKS;
  GEOMAlgo_KindOfName aKN;
  GEOMAlgo_KindOfBounds aKB;
  GEOMAlgo_KindOfClosed aKC;
  //
  aNbV=NbSubShapes(TopAbs_VERTEX);
  aNbE=NbSubShapes(TopAbs_EDGE);
  aNbF=NbSubShapes(TopAbs_FACE);
  aNbS=NbSubShapes(TopAbs_SOLID);
  aKS=KindOfShape();
  aKN=KindOfName();
  aKB=KindOfBounds();
  aKC=KindOfClosed();
  //
  printf(" *COMPSOLID\n");
  printf("  number of vertices: %d\n", aNbV);
  printf("  number of edges   : %d\n", aNbE);
  printf("  number of faces   : %d\n", aNbF);
  printf("  number of solids  : %d\n", aNbS);
  DumpKindOfShape (aKS);
  DumpKindOfName (aKN);
  DumpKindOfBounds(aKB);
  DumpKindOfClosed(aKC);
}
//=======================================================================
//function : DumpSolid
//purpose  :
//=======================================================================
void GEOMAlgo_ShapeInfo::DumpSolid()const
{
  Standard_Integer aNbV, aNbE, aNbF;
  GEOMAlgo_KindOfShape aKS;
  GEOMAlgo_KindOfName aKN;
  GEOMAlgo_KindOfBounds aKB;
  GEOMAlgo_KindOfClosed aKC;
  //
  aNbV=NbSubShapes(TopAbs_VERTEX);
  aNbE=NbSubShapes(TopAbs_EDGE);
  aNbF=NbSubShapes(TopAbs_FACE);
  aKS=KindOfShape();
  aKN=KindOfName();
  aKB=KindOfBounds();
  aKC=KindOfClosed();
  //
  printf(" *SOLID\n");
  printf("  number of vertices: %d\n", aNbV);
  printf("  number of edges   : %d\n", aNbE);
  printf("  number of faces   : %d\n", aNbF);
  DumpKindOfShape (aKS);
  DumpKindOfName (aKN);
  DumpKindOfBounds(aKB);
  DumpKindOfClosed(aKC);
  //
  if (aKN==GEOMAlgo_KN_SPHERE) {
    DumpLocation (myLocation);
    DumpPosition (myPosition);
    printf("  Radius1   : %.3lf\n", myRadius1);
  }
  if (aKN==GEOMAlgo_KN_CYLINDER) {
    DumpLocation (myLocation);
    DumpPosition (myPosition);
    printf("  Radius1   : %.3lf\n", myRadius1);
    printf("  Height    : %.3lf\n", myHeight);
  }
  else if (aKN==GEOMAlgo_KN_CONE) {
    DumpLocation (myLocation);
    DumpPosition (myPosition);
    printf("  Radius1   : %.3lf\n", myRadius1);
    printf("  Radius2   : %.3lf\n", myRadius2);
    printf("  Height    : %.3lf\n", myHeight);
  }
  else if (aKN==GEOMAlgo_KN_TORUS) {
    DumpLocation (myLocation);
    DumpPosition (myPosition);
    printf("  Radius1   : %.3lf\n", myRadius1);
    printf("  Radius2   : %.3lf\n", myRadius2);
  }
  else if (aKN==GEOMAlgo_KN_POLYHEDRON) {
    DumpLocation (myLocation);
    DumpPosition (myPosition);
  }
  else if (aKN==GEOMAlgo_KN_BOX) {
    DumpLocation (myLocation);
    DumpPosition (myPosition);
    printf("  Length    : %.3lf\n", myLength);
    printf("  Width     : %.3lf\n", myWidth);
    printf("  Height    : %.3lf\n", myHeight);
  }
}
//=======================================================================
//function : DumpFace
//purpose  :
//=======================================================================
void GEOMAlgo_ShapeInfo::DumpFace()const
{
  Standard_Integer aNbV, aNbE;
  GEOMAlgo_KindOfShape aKS;
  GEOMAlgo_KindOfName aKN;
  GEOMAlgo_KindOfBounds aKB;
  GEOMAlgo_KindOfClosed aKC;
  GEOMAlgo_KindOfDef aKD;
  //
  aNbV=NbSubShapes(TopAbs_VERTEX);
  aNbE=NbSubShapes(TopAbs_EDGE);
  aKS=KindOfShape();
  aKN=KindOfName();
  aKB=KindOfBounds();
  aKC=KindOfClosed();
  aKD=KindOfDef();
  //
  printf(" *FACE\n");
  printf("  number of vertices: %d\n", aNbV);
  printf("  number of edges   : %d\n", aNbE);
  DumpKindOfShape (aKS);
  DumpKindOfName (aKN);
  DumpKindOfBounds(aKB);
  DumpKindOfClosed(aKC);
  DumpKindOfDef(aKD);
  //
  // PLANE
  if (aKN==GEOMAlgo_KN_PLANE) {
    DumpLocation (myLocation);
    DumpPosition (myPosition);
  }
  else if (aKN==GEOMAlgo_KN_DISKCIRCLE) {
    DumpLocation (myLocation);
    DumpPosition (myPosition);
    printf("  Radius1   : %.3lf\n", myRadius1);
  }
  else if (aKN==GEOMAlgo_KN_DISKELLIPSE) {
    DumpLocation (myLocation);
    DumpPosition (myPosition);
    printf("  Radius1   : %.3lf\n", myRadius1);
    printf("  Radius2   : %.3lf\n", myRadius2);
  }
  else if (aKN==GEOMAlgo_KN_RECTANGLE) {
    DumpLocation (myLocation);
    DumpPosition (myPosition);
    printf("  Length    : %.3lf\n", myLength);
    printf("  Width     : %.3lf\n", myWidth);
  }
  else if (aKN==GEOMAlgo_KN_TRIANGLE ||
           aKN==GEOMAlgo_KN_QUADRANGLE||
           aKN==GEOMAlgo_KN_POLYGON) {
    DumpLocation (myLocation);
    DumpPosition (myPosition);
  }
  //
  // SPHERE
  else if (aKN==GEOMAlgo_KN_SPHERE) {
    DumpLocation (myLocation);
    DumpPosition (myPosition);
    printf("  Radius1   : %.3lf", myRadius1);
  }
  //
  // CYLINDER
  else if (aKN==GEOMAlgo_KN_CYLINDER) {
    DumpLocation (myLocation);
    DumpPosition (myPosition);
    printf("  Radius1   : %.3lf\n", myRadius1);
    printf("  Height    : %.3lf\n", myHeight);
  }
  //
  // CONE
  else if (aKN==GEOMAlgo_KN_CONE) {
    DumpLocation (myLocation);
    DumpPosition (myPosition);
    printf("  Radius1   : %.3lf\n", myRadius1);
    printf("  Radius2   : %.3lf\n", myRadius2);
    printf("  Height    : %.3lf\n", myHeight);
  }
  //
  // TORUS
  else if (aKN==GEOMAlgo_KN_TORUS) {
    DumpLocation (myLocation);
    DumpPosition (myPosition);
    printf("  Radius1   : %.3lf\n", myRadius1);
    printf("  Radius2   : %.3lf\n", myRadius2);
  }


  printf("\n");
}
//=======================================================================
//function : DumpShell
//purpose  :
//=======================================================================
void GEOMAlgo_ShapeInfo::DumpShell()const
{
  Standard_Integer aNbV, aNbE, aNbF;
  GEOMAlgo_KindOfClosed aKC;
  //
  printf(" *SHELL\n");
  //
  aNbV=NbSubShapes(TopAbs_VERTEX);
  aNbE=NbSubShapes(TopAbs_EDGE);
  aNbF=NbSubShapes(TopAbs_FACE);
  printf("  number of vertices: %d\n", aNbV);
  printf("  number of edges   : %d\n", aNbE);
  printf("  number of faces   : %d\n", aNbF);
  aKC=KindOfClosed();
  DumpKindOfClosed(aKC);
  printf("\n");
}
//=======================================================================
//function : DumpWire
//purpose  :
//=======================================================================
void GEOMAlgo_ShapeInfo::DumpWire()const
{
  Standard_Integer aNbV, aNbE;
  GEOMAlgo_KindOfClosed aKC;
  //
  printf(" *WIRE\n");
  //
  aNbV=NbSubShapes(TopAbs_VERTEX);
  aNbE=NbSubShapes(TopAbs_EDGE);
  printf("  number of vertices: %d\n", aNbV);
  printf("  number of edges   : %d\n", aNbE);

  aKC=KindOfClosed();
  DumpKindOfClosed(aKC);
  printf("\n");
}
//=======================================================================
//function : DumpEdge
//purpose  :
//=======================================================================
void GEOMAlgo_ShapeInfo::DumpEdge()const
{
  Standard_Integer aNbV;
  Standard_Real aX, aY, aZ;
  GEOMAlgo_KindOfShape aKS;
  GEOMAlgo_KindOfName aKN;
  GEOMAlgo_KindOfBounds aKB;
  GEOMAlgo_KindOfClosed aKC;
  GEOMAlgo_KindOfPeriod aKP;
  //
  aNbV=NbSubShapes(TopAbs_VERTEX);
  aKS=KindOfShape();
  aKN=KindOfName();
  aKB=KindOfBounds();
  aKC=KindOfClosed();
  //
  printf(" *EDGE\n");
  printf("  number of vertices: %d\n", aNbV);
  DumpKindOfShape (aKS);
  DumpKindOfName (aKN);
  DumpKindOfBounds(aKB);
  DumpKindOfClosed(aKC);
  //
  // LINE
  if (aKN==GEOMAlgo_KN_LINE) {
    DumpLocation (myLocation);
    DumpDirection(myDirection);
  }
  else if (aKN==GEOMAlgo_KN_SEGMENT) {
    DumpLocation (myLocation);
    DumpDirection(myDirection);
    printf("  Length    : %.3lf\n", myLength);
    myPnt1.Coord(aX, aY, aZ);
    printf("  Pnt1 : %.3lf %.3lf %.3lf\n", aX, aY, aZ);
    myPnt2.Coord(aX, aY, aZ);
    printf("  Pnt2 : %.3lf %.3lf %.3lf\n", aX, aY, aZ);

  }
  else if (aKN==GEOMAlgo_KN_CIRCLE) {
    DumpLocation (myLocation);
    DumpPosition (myPosition);
    printf("  Radius1   : %.3lf\n", myRadius1);
  }
  else if (aKN==GEOMAlgo_KN_ARCCIRCLE) {
    DumpLocation (myLocation);
    DumpPosition (myPosition);
    printf("  Radius1   : %.3lf\n", myRadius1);
    myPnt1.Coord(aX, aY, aZ);
    printf("  Pnt1 : %.3lf %.3lf %.3lf\n", aX, aY, aZ);
    myPnt2.Coord(aX, aY, aZ);
    printf("  Pnt2 : %.3lf %.3lf %.3lf\n", aX, aY, aZ);
  }
  else if (aKN==GEOMAlgo_KN_ELLIPSE) {
    DumpLocation (myLocation);
    DumpPosition (myPosition);
    printf("  Radius1   : %.3lf\n", myRadius1);
    printf("  Radius2   : %.3lf\n", myRadius2);
  }
  else if (aKN==GEOMAlgo_KN_ARCELLIPSE) {
    DumpLocation (myLocation);
    DumpPosition (myPosition);
    printf("  Radius1   : %.3lf\n", myRadius1);
    printf("  Radius2   : %.3lf\n", myRadius2);
    myPnt1.Coord(aX, aY, aZ);
    printf("  Pnt1 : %.3lf %.3lf %.3lf\n", aX, aY, aZ);
    myPnt2.Coord(aX, aY, aZ);
    printf("  Pnt2 : %.3lf %.3lf %.3lf\n", aX, aY, aZ);
  }
  else if (aKN==GEOMAlgo_KN_CURVEBSPLINE) {
    aKP=KindOfPeriod();
    DumpKindOfPeriod(aKP);
    printf("  Degree    : %d\n", myDegree);
    printf("  NbPoles   : %d\n", myNbPoles);
    printf("  NbKnots   : %d\n", myNbKnots);
    printf("  NbWeights : %d\n", myNbWeights);
    printf("  NbMultis  : %d\n", myNbMultiplicities);

    if (aKB == GEOMAlgo_KB_TRIMMED) {
      myPnt1.Coord(aX, aY, aZ);
      printf("  Pnt1 : %.3lf %.3lf %.3lf\n", aX, aY, aZ);
      myPnt2.Coord(aX, aY, aZ);
      printf("  Pnt2 : %.3lf %.3lf %.3lf\n", aX, aY, aZ);
    }

    Standard_Integer i;
    if (myNbPoles > 0 && !myPoles.IsNull() && myPoles->Length() == myNbPoles) {
      bool aHasWeight = (myNbWeights == myNbPoles);
      printf("  Poles\n");
      for (i=1; i<=myNbPoles; i++) {
        const gp_Pnt &aP = myPoles->Value(i);
        if (aHasWeight)
          printf("  %3d : %.3lf %.3lf %.3lf  %.3lf\n", i, aP.X(), aP.Y(), aP.Z(), myWeights->Value(i));
        else
          printf("  %3d : %.3lf %.3lf %.3lf\n", i, aP.X(), aP.Y(), aP.Z());
      }
    }

    if (myNbKnots > 0 && !myKnots.IsNull() && myKnots->Length() == myNbKnots) {
      bool aHasMulti = (myNbMultiplicities == myNbKnots);
      printf("  Knots\n");
      for (i=1; i<=myNbKnots; i++) {
        if (aHasMulti)
          printf("  %3d : %.3lf  %.3lf\n", i, myKnots->Value(i), myMultiplicities->Value(i));
        else
          printf("  %3d : %.3lf\n", i, myKnots->Value(i));
      }
    }
    
  }
  else if (aKN==GEOMAlgo_KN_CURVEBEZIER) {
    aKP=KindOfPeriod();
    DumpKindOfPeriod(aKP);
    printf("  Degree    : %d\n", myDegree);
    printf("  NbPoles   : %d\n", myNbPoles);
    printf("  NbWeights : %d\n", myNbWeights);
    if (aKB == GEOMAlgo_KB_TRIMMED) {
      myPnt1.Coord(aX, aY, aZ);
      printf("  Pnt1 : %.3lf %.3lf %.3lf\n", aX, aY, aZ);
      myPnt2.Coord(aX, aY, aZ);
      printf("  Pnt2 : %.3lf %.3lf %.3lf\n", aX, aY, aZ);
    }

    Standard_Integer i;
    if (myNbPoles > 0 && !myPoles.IsNull() && myPoles->Length() == myNbPoles) {
      bool aHasWeight = (myNbWeights == myNbPoles);
      printf("  Poles\n");
      for (i=1; i<=myNbPoles; i++) {
        const gp_Pnt &aP = myPoles->Value(i);
        if (aHasWeight)
          printf("  %3d : %.3lf %.3lf %.3lf  %.3lf\n", i, aP.X(), aP.Y(), aP.Z(), myWeights->Value(i));
        else
          printf("  %3d : %.3lf %.3lf %.3lf\n", i, aP.X(), aP.Y(), aP.Z());
      }
    }
  }
  else if (aKN==GEOMAlgo_KN_HYPERBOLA) {
    DumpLocation (myLocation);
    DumpPosition (myPosition);
    printf("  Radius1   : %.3lf\n", myRadius1);
    printf("  Radius2   : %.3lf\n", myRadius2);
  }
  else if (aKN==GEOMAlgo_KN_PARABOLA) {
    DumpLocation (myLocation);
    DumpPosition (myPosition);
    printf("  Focal     : %.3lf\n", myRadius1);
  }
  printf("\n");
}
//=======================================================================
//function : DumpVertex
//purpose  :
//=======================================================================
void GEOMAlgo_ShapeInfo::DumpVertex()const
{
  printf(" *VERTEX\n");
  DumpLocation(myLocation);
}
//=======================================================================
//function : DumpLocation
//purpose  :
//=======================================================================
void DumpLocation(const gp_Pnt& aP)
{
  Standard_Real aX, aY, aZ;
  //
  aP.Coord(aX, aY, aZ);
  printf("  Location : %.3lf %.3lf %.3lf \n", aX, aY, aZ);
}
//=======================================================================
//function : DumpDirection
//purpose  :
//=======================================================================
void DumpDirection(const gp_Dir& aD)
{
  Standard_Real aX, aY, aZ;
  //
  aD.Coord(aX, aY, aZ);
  printf("  Direction: %.3lf %.3lf %.3lf \n", aX, aY, aZ);
}
//=======================================================================
//function : DumpPosition
//purpose  :
//=======================================================================
void DumpPosition(const gp_Ax3& aAx3)
{
  const gp_Dir& aDZ=aAx3.Axis().Direction();
  const gp_Dir& aDX=aAx3.XDirection();
  const gp_Dir& aDY=aAx3.YDirection();
  printf("  Position:\n");
  printf("  Axis: %.3lf %.3lf %.3lf \n", aDZ.X(), aDZ.Y(), aDZ.Z());
  printf("  X   : %.3lf %.3lf %.3lf \n", aDX.X(), aDX.Y(), aDX.Z());
  printf("  Y   : %.3lf %.3lf %.3lf \n", aDY.X(), aDY.Y(), aDY.Z());
}
//=======================================================================
//function : DumpKindOfBounds
//purpose  :
//=======================================================================
void DumpKindOfBounds(const GEOMAlgo_KindOfBounds aKB)
{
  const char *pStr[]={
    "KB_UNKNOWN",
    "KB_TRIMMED",
    "KB_INFINITE"
  };
  int i;
  //
  i=(Standard_Integer)aKB;
  printf(" KindOfBounds: %s\n", pStr[i]);

}
//=======================================================================
//function : DumpKindOfClosed
//purpose  :
//=======================================================================
void DumpKindOfClosed(const GEOMAlgo_KindOfClosed aKC)
{
  const char *pStr[]={
    "KC_UNKNOWN",
    "KC_CLOSED",
    "KC_NOTCLOSED"
  };
  int i;
  //
  i=(Standard_Integer)aKC;
  printf(" KindOfClosed: %s\n", pStr[i]);

}
//=======================================================================
//function : DumpKindOfShape
//purpose  :
//=======================================================================
void DumpKindOfShape(const GEOMAlgo_KindOfShape aKS)
{
  const char *pStr[]={
    "KS_UNKNOWN",
    "KS_SPHERE",
    "KS_CYLINDER",
    "KS_BOX",
    "KS_TORUS",
    "KS_CONE",
    "KS_ELLIPSE",
    "KS_PLANE",
    "KS_CIRCLE",
    "KS_LINE",
    "KS_DEGENERATED",
    "KS_BSPLINE",
    "KS_BEZIER",
    "KS_HYPERBOLA",
    "KS_PARABOLA"
    };
  int i;
  //
  i=(Standard_Integer)aKS;
  printf(" KindOfShape : %s\n", pStr[i]);
}
//=======================================================================
//function : DumpKindOfName
//purpose  :
//=======================================================================
void DumpKindOfName(const GEOMAlgo_KindOfName aKS)
{
  const char *pStr[]={
    "KN_UNKNOWN",
    "KN_SPHERE",
    "KN_CYLINDER",
    "KN_TORUS",
    "KN_CONE",
    "KN_ELLIPSE",
    "KN_CIRCLE",
    "KN_PLANE",
    "KN_LINE",
    "KN_BOX",
    "KN_SEGMENT",
    "KN_ARCCIRCLE",
    "KN_POLYGON",
    "KN_POLYHEDRON",
    "KN_DISKCIRCLE",
    "KN_DISKELLIPSE",
    "KN_RECTANGLE",
    "KN_TRIANGLE",
    "KN_QUADRANGLE",
    "KN_ARCELLIPSE",
    "KN_SOLID",
    "KN_CURVEBSPLINE",
    "KN_CURVEBEZIER",
    "KN_HYPERBOLA",
    "KN_PARABOLA"
    };
  int i;
  //
  i=(Standard_Integer)aKS;
  printf(" KindOfName  : %s\n", pStr[i]);
}
//=======================================================================
//function : DumpKindOfDef
//purpose  :
//=======================================================================
void DumpKindOfDef(const GEOMAlgo_KindOfDef aKD)
{
  const char *pStr[]={
    "KD_UNKNOWN",
    "KD_SPECIFIED",
    "KD_ARBITRARY"
  };
  int i;
  //
  i=(Standard_Integer)aKD;
  printf(" KindOfDef: %s\n", pStr[i]);
}
//=======================================================================
//function : DumpKindOfPeriod
//purpose  :
//=======================================================================
void DumpKindOfPeriod(const GEOMAlgo_KindOfPeriod aKP)
{
  const char *pStr[]={
    "KP_UNKNOWN",
    "KP_PERIODIC",
    "KP_NONPERIODIC"
  };
  int i;
  //
  i=(Standard_Integer)aKP;
  printf(" KindOfPeriod: %s\n", pStr[i]);
}
