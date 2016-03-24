// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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


#include <GEOMUtils_Hatcher.hxx>

#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <HatchGen_Domain.hxx>


static Standard_Real IntersectorConfusion = 1.e-10; // -8;
static Standard_Real IntersectorTangency  = 1.e-10; // -8;
static Standard_Real HatcherConfusion2d   = 1.e-8;
static Standard_Real HatcherConfusion3d   = 1.e-8;
// VTK uses float numbers - Precision::Infinite() is double and
// can not be accepted.
static float         InfiniteValue        = 1e38;

//=======================================================================
//function : GEOMUtils_Hatcher
//purpose  :
//=======================================================================
GEOMUtils::Hatcher::Hatcher(const TopoDS_Face &theFace)
: myHatcher(Geom2dHatch_Intersector (IntersectorConfusion, IntersectorTangency),
            HatcherConfusion2d, HatcherConfusion3d,
            Standard_True, Standard_False),
  myFace   (theFace),
  myIsDone (Standard_False),
  myUMin   (0.),
  myUMax   (0.),
  myVMin   (0.),
  myVMax   (0.)
{
  // Get bounds.
  BRepTools::UVBounds (theFace, myUMin, myUMax, myVMin, myVMax);

  Standard_Boolean InfiniteUMin = Precision::IsNegativeInfinite (myUMin);
  Standard_Boolean InfiniteUMax = Precision::IsPositiveInfinite (myUMax);
  Standard_Boolean InfiniteVMin = Precision::IsNegativeInfinite (myVMin);
  Standard_Boolean InfiniteVMax = Precision::IsPositiveInfinite (myVMax);

  if (InfiniteUMin && InfiniteUMax) {
    myUMin = - InfiniteValue;
    myUMax =   InfiniteValue;
  } else if (InfiniteUMin) {
    myUMin = myUMax - InfiniteValue;
  } else if (InfiniteUMax) {
    myUMax = myUMin + InfiniteValue;
  }

  if (InfiniteVMin && InfiniteVMax) {
    myVMin = - InfiniteValue;
    myVMax =   InfiniteValue;
  } else if (InfiniteVMin) {
    myVMin = myVMax - InfiniteValue;
  } else if (InfiniteVMax) {
    myVMax = myVMin + InfiniteValue;
  }

  // Add edges
  TopExp_Explorer     anExpEdges(theFace, TopAbs_EDGE);
  const Standard_Real aParamTol = Precision::PConfusion();

  for (; anExpEdges.More(); anExpEdges.Next()) {
    const TopoDS_Edge& anEdge = TopoDS::Edge (anExpEdges.Current());
    Standard_Real U1, U2;
    const Handle(Geom2d_Curve) PCurve =
      BRep_Tool::CurveOnSurface (anEdge, theFace, U1, U2);

    if (PCurve.IsNull()) {
      continue;
    }

    if (U1 == U2) {
      continue;
    }

    //-- Test if a TrimmedCurve is necessary
    if(Abs(PCurve->FirstParameter() - U1) <= aParamTol &&
       Abs(PCurve->LastParameter() - U2)  <= aParamTol) { 
      myHatcher.AddElement(PCurve, anEdge.Orientation());
    } else {
      if (!PCurve->IsPeriodic()) {
        Handle(Geom2d_TrimmedCurve) TrimPCurve =
          Handle(Geom2d_TrimmedCurve)::DownCast(PCurve);

        if (!TrimPCurve.IsNull()) {
          Handle(Geom2d_Curve) aBasisCurve = TrimPCurve->BasisCurve();

          if (aBasisCurve->FirstParameter() - U1 > aParamTol ||
              U2 - aBasisCurve->LastParameter()  > aParamTol) {
            myHatcher.AddElement (PCurve, anEdge.Orientation());
            return;
          }
        } else {
          if (PCurve->FirstParameter() - U1 > aParamTol) {
            U1 = PCurve->FirstParameter();
          }
          if (U2 - PCurve->LastParameter() > aParamTol) {
            U2=PCurve->LastParameter();
          }
        }
      }

      Handle (Geom2d_TrimmedCurve) TrimPCurve =
        new Geom2d_TrimmedCurve (PCurve, U1, U2);

      myHatcher.AddElement (TrimPCurve, anEdge.Orientation());
    }
  }
}

//=======================================================================
//function : Init
//purpose  :
//=======================================================================
void GEOMUtils::Hatcher::Init(const Standard_Integer theNbIsos)
{
  Init(theNbIsos, theNbIsos);
}

//=======================================================================
//function : Init
//purpose  :
//=======================================================================
void GEOMUtils::Hatcher::Init(const Standard_Integer theNbIsoU,
			      const Standard_Integer theNbIsoV)
{
  // Initialize data.
  Clear();

  if (theNbIsoU > 0 || theNbIsoV > 0) {
    Standard_Integer IIso;
    Standard_Real DeltaU = Abs (myUMax - myUMin);
    Standard_Real DeltaV = Abs (myVMax - myVMin);
    Standard_Real confusion = Min (DeltaU, DeltaV) * myHatcher.Confusion3d();

    myHatcher.Confusion3d (confusion);

    if (theNbIsoU > 0) {
      myUPrm = new TColStd_HArray1OfReal   (1, theNbIsoU);
      myUInd = new TColStd_HArray1OfInteger(1, theNbIsoU, 0);

      Standard_Real StepU = DeltaU / (Standard_Real) theNbIsoU;

      if (StepU > confusion) {
        Standard_Real UPrm = myUMin + StepU / 2.;
        gp_Dir2d Dir (0., 1.);

        for (IIso = 1; IIso <= theNbIsoU; IIso++) {
          myUPrm->SetValue(IIso, UPrm);
          gp_Pnt2d Ori (UPrm, 0.);
          Geom2dAdaptor_Curve HCur (new Geom2d_Line (Ori, Dir));
          myUInd->SetValue(IIso, myHatcher.AddHatching(HCur));
          UPrm += StepU;
        }
      }
    }

    if (theNbIsoV > 0) {
      myVPrm = new TColStd_HArray1OfReal   (1, theNbIsoV);
      myVInd = new TColStd_HArray1OfInteger(1, theNbIsoV, 0);

      Standard_Real StepV = DeltaV / (Standard_Real) theNbIsoV;

      if (StepV > confusion) {
        Standard_Real VPrm = myVMin + StepV / 2.;
        gp_Dir2d Dir (1., 0.);

        for (IIso = 1; IIso <= theNbIsoV; IIso++) {
          myVPrm->SetValue(IIso, VPrm);
          gp_Pnt2d Ori (0., VPrm);
          Geom2dAdaptor_Curve HCur (new Geom2d_Line (Ori, Dir));
          myVInd->SetValue(IIso, myHatcher.AddHatching(HCur));
          VPrm += StepV;
        }
      }
    }
  }
}

//=======================================================================
//function : Init
//purpose  :
//=======================================================================
void GEOMUtils::Hatcher::Init(const GeomAbs_IsoType theIsoType,
			      const Standard_Real   theParameter)
{
  // Initialize data.
  Clear();

  if (theIsoType == GeomAbs_IsoU || theIsoType == GeomAbs_IsoV) {
    const Standard_Boolean            isIsoU = (theIsoType == GeomAbs_IsoU);
    Handle(TColStd_HArray1OfReal)    &aPrm   = (isIsoU ? myUPrm : myVPrm);
    Handle(TColStd_HArray1OfInteger) &anInd  = (isIsoU ? myUInd : myVInd);
    Handle(Geom2d_Line) aLine;

    aPrm  = new TColStd_HArray1OfReal   (1, 1);
    anInd = new TColStd_HArray1OfInteger(1, 1);
    aPrm->SetValue(1, theParameter);

    if (isIsoU) {
      // U-isoline
      gp_Dir2d aDir (0., 1.);
      gp_Pnt2d anOri(theParameter, 0.);

      aLine = new Geom2d_Line(anOri, aDir);
    } else {
      // V-isoline
      gp_Dir2d aDir (1., 0.);
      gp_Pnt2d anOri(0., theParameter);

      aLine = new Geom2d_Line(anOri, aDir);
    }

    Geom2dAdaptor_Curve aGACurve (aLine);

    anInd->SetValue(1, myHatcher.AddHatching(aGACurve));
  }
}

//=======================================================================
//function : Perform
//purpose  :
//=======================================================================
void GEOMUtils::Hatcher::Perform()
{
  myHatcher.Trim();

  // Compute domains.
  Standard_Integer i;
  Standard_Integer anIndex;

  if (myUInd.IsNull() == Standard_False) {
    for (i = myUInd->Lower() ; i <= myUInd->Upper() ; i++) {
      anIndex = myUInd->Value(i);

      if (anIndex != 0) {
        if (myHatcher.TrimDone(anIndex) && !myHatcher.TrimFailed(anIndex)) {
          myHatcher.ComputeDomains(anIndex);

          if (!myIsDone) {
            myIsDone = (myHatcher.NbDomains(anIndex) > 0);
          }
        }
      }
    }
  }

  if (myVInd.IsNull() == Standard_False) {
    for (i = myVInd->Lower() ; i <= myVInd->Upper() ; i++) {
      anIndex = myVInd->Value(i);

      if (anIndex != 0) {
        if (myHatcher.TrimDone(anIndex) && !myHatcher.TrimFailed(anIndex)) {
          myHatcher.ComputeDomains(anIndex);

          if (!myIsDone) {
            myIsDone = (myHatcher.NbDomains(anIndex) > 0);
          }
        }
      }
    }
  }
}

//=======================================================================
//function : GetNbDomains
//purpose  :
//=======================================================================
Standard_Integer GEOMUtils::Hatcher::GetNbDomains
              (const Standard_Integer theHatchingIndex) const
{
  Standard_Integer aResult = -1;

  if (myIsDone && myHatcher.IsDone(theHatchingIndex)) {
    aResult = myHatcher.NbDomains(theHatchingIndex);
  }

  return aResult;
}

//=======================================================================
//function : GetDomain
//purpose  :
//=======================================================================
Standard_Boolean GEOMUtils::Hatcher::GetDomain
                           (const Standard_Integer  theHatchingIndex,
                            const Standard_Integer  theDomainIndex,
                                  Standard_Real    &theParam1,
                                  Standard_Real    &theParam2) const
{
  Standard_Boolean isOK = Standard_False;

  if (theDomainIndex > 0) {
    const Standard_Integer aNbDomains = GetNbDomains(theHatchingIndex);

    if (theDomainIndex <= aNbDomains) {
      const HatchGen_Domain& aDomain =
        myHatcher.Domain (theHatchingIndex, theDomainIndex);

      if (aDomain.HasFirstPoint()) {
        theParam1 = aDomain.FirstPoint().Parameter();
      } else {
        theParam1 = myVMin - InfiniteValue;
      }

      if (aDomain.HasSecondPoint()) {
        theParam2 = aDomain.SecondPoint().Parameter();
      } else {
        theParam2 = myVMax + InfiniteValue;
      }

      isOK = Standard_True;
    }
  }

  return isOK;
}

//=======================================================================
//function : IsDomainInfinite
//purpose  :
//=======================================================================
Standard_Boolean GEOMUtils::Hatcher::IsDomainInfinite
                           (const Standard_Integer  theHatchingIndex,
                            const Standard_Integer  theDomainIndex) const
{
  Standard_Boolean isInfinite = Standard_False;

  if (theDomainIndex > 0) {
    const Standard_Integer aNbDomains = GetNbDomains(theHatchingIndex);

    if (theDomainIndex <= aNbDomains) {
      const HatchGen_Domain& aDomain =
        myHatcher.Domain (theHatchingIndex, theDomainIndex);

      if (!aDomain.HasFirstPoint() || !aDomain.HasSecondPoint()) {
        isInfinite = Standard_True;
      }
    }
  }

  return isInfinite;
}

//=======================================================================
//function : GetHatching
//purpose  :
//=======================================================================
const Handle(Geom2d_Curve) &GEOMUtils::Hatcher::GetHatching
                      (const Standard_Integer theHatchingIndex) const
{
  const Geom2dAdaptor_Curve &aGACurve =
    myHatcher.HatchingCurve(theHatchingIndex);

  return aGACurve.Curve();
}

//=======================================================================
//function : Clear
//purpose  :
//=======================================================================
void GEOMUtils::Hatcher::Clear()
{
  myIsDone = Standard_False;
  myUPrm.Nullify();
  myVPrm.Nullify();
  myUInd.Nullify();
  myVInd.Nullify();
  myHatcher.ClrHatchings();
}
