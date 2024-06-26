// Copyright (C) 2007-2024  CEA, EDF, OPEN CASCADE
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


#ifndef _GEOMAlgo_ShapeInfo_HeaderFile
#define _GEOMAlgo_ShapeInfo_HeaderFile

#include <Standard.hxx>
#include <Standard_Macro.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <Standard_Integer.hxx>


#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gp_Ax3.hxx>
#include <Standard_Real.hxx>
#include <gp_Ax2.hxx>

#include <GEOMAlgo_KindOfShape.hxx>
#include <GEOMAlgo_KindOfName.hxx>
#include <GEOMAlgo_KindOfBounds.hxx>
#include <GEOMAlgo_KindOfClosed.hxx>
#include <GEOMAlgo_KindOfDef.hxx>
#include <GEOMAlgo_KindOfPeriod.hxx>

#include <TColgp_HArray1OfPnt.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray1OfInteger.hxx>


//=======================================================================
//class : GEOMAlgo_ShapeInfo
//purpose  :
//=======================================================================
class GEOMAlgo_ShapeInfo
{
 public:
  Standard_EXPORT
    GEOMAlgo_ShapeInfo();

  Standard_EXPORT
    virtual ~GEOMAlgo_ShapeInfo();

  Standard_EXPORT
    void Reset() ;

  Standard_EXPORT
    void SetType(const TopAbs_ShapeEnum aType) ;

  Standard_EXPORT
    TopAbs_ShapeEnum Type() const;

  Standard_EXPORT
    void SetNbSubShapes(const TopAbs_ShapeEnum aType,
                        const Standard_Integer aNb) ;

  Standard_EXPORT
    Standard_Integer NbSubShapes(const TopAbs_ShapeEnum aType) const;

  Standard_EXPORT
    void SetKindOfShape(const GEOMAlgo_KindOfShape aT) ;

  Standard_EXPORT
    GEOMAlgo_KindOfShape KindOfShape() const;

  Standard_EXPORT
    void SetKindOfName(const GEOMAlgo_KindOfName aT) ;

  Standard_EXPORT
    GEOMAlgo_KindOfName KindOfName() const;

  Standard_EXPORT
    void SetKindOfBounds(const GEOMAlgo_KindOfBounds aT) ;

  Standard_EXPORT
    GEOMAlgo_KindOfBounds KindOfBounds() const;

  Standard_EXPORT
    void SetKindOfClosed(const GEOMAlgo_KindOfClosed aT) ;

  Standard_EXPORT
    GEOMAlgo_KindOfClosed KindOfClosed() const;

  Standard_EXPORT
    void SetKindOfDef(const GEOMAlgo_KindOfDef aT) ;

  Standard_EXPORT
    GEOMAlgo_KindOfDef KindOfDef() const;

  Standard_EXPORT
    void SetKindOfPeriod(const GEOMAlgo_KindOfPeriod aT) ;

  Standard_EXPORT
    GEOMAlgo_KindOfPeriod KindOfPeriod() const;

  Standard_EXPORT
    void SetLocation(const gp_Pnt& aP) ;

  Standard_EXPORT
    const gp_Pnt& Location() const;

  Standard_EXPORT
    void SetDirection(const gp_Dir& aD) ;

  Standard_EXPORT
    const gp_Dir& Direction() const;

  Standard_EXPORT
    void SetPosition(const gp_Ax2& aAx2) ;

  Standard_EXPORT
    void SetPosition(const gp_Ax3& aAx3) ;

  Standard_EXPORT
    const gp_Ax3& Position() const;

  Standard_EXPORT
    void SetPnt1(const gp_Pnt& aP) ;

  Standard_EXPORT
    const gp_Pnt& Pnt1() const;

  Standard_EXPORT
    void SetPnt2(const gp_Pnt& aP) ;

  Standard_EXPORT
    const gp_Pnt& Pnt2() const;

  Standard_EXPORT
    void SetRadius1(const Standard_Real aR) ;

  Standard_EXPORT
    Standard_Real Radius1() const;

  Standard_EXPORT
    void SetRadius2(const Standard_Real aR) ;

  Standard_EXPORT
    Standard_Real Radius2() const;

  Standard_EXPORT
    void SetLength(const Standard_Real aL) ;

  Standard_EXPORT
    Standard_Real Length() const;

  Standard_EXPORT
    void SetWidth(const Standard_Real aW) ;

  Standard_EXPORT
    Standard_Real Width() const;

  Standard_EXPORT
    void SetHeight(const Standard_Real aH) ;

  Standard_EXPORT
    Standard_Real Height() const;

  Standard_EXPORT
    void SetDegree(const Standard_Integer aDeg) ;

  Standard_EXPORT
    Standard_Integer Degree() const;

  Standard_EXPORT
    void SetNbPoles(const Standard_Integer aNb) ;

  Standard_EXPORT
    Standard_Integer NbPoles() const;

  Standard_EXPORT
    void SetNbKnots(const Standard_Integer aNb) ;

  Standard_EXPORT
    Standard_Integer NbKnots() const;

  Standard_EXPORT
    void SetNbWeights(const Standard_Integer aNb) ;

  Standard_EXPORT
    Standard_Integer NbWeights() const;

  Standard_EXPORT
    void SetNbMultiplicities(const Standard_Integer aNb) ;

  Standard_EXPORT
    Standard_Integer NbMultiplicities() const;

  Standard_EXPORT
    void SetPoles(Handle(TColgp_HArray1OfPnt) P) ;

  Standard_EXPORT
    Handle(TColgp_HArray1OfPnt) Poles() const;

  Standard_EXPORT
    void SetKnots(Handle(TColStd_HArray1OfReal) K) ;

  Standard_EXPORT
    Handle(TColStd_HArray1OfReal) Knots() const;

  Standard_EXPORT
    void SetWeights(Handle(TColStd_HArray1OfReal) W) ;

  Standard_EXPORT
    Handle(TColStd_HArray1OfReal) Weights() const;

  Standard_EXPORT
    void SetMultiplicities(Handle(TColStd_HArray1OfInteger) M) ;

  Standard_EXPORT
    Handle(TColStd_HArray1OfInteger) Multiplicities() const;

  Standard_EXPORT
    void Dump() const;

 protected:
  Standard_EXPORT
    void DumpVertex() const;

  Standard_EXPORT
    void DumpEdge() const;

  Standard_EXPORT
    void DumpWire() const;

  Standard_EXPORT
    void DumpFace() const;

  Standard_EXPORT
    void DumpShell() const;

  Standard_EXPORT
    void DumpSolid() const;

  Standard_EXPORT
    void DumpCompSolid() const;

  Standard_EXPORT
    void DumpCompound() const;


  TopAbs_ShapeEnum myType;
  Standard_Integer myNbSubShapes[9];
  GEOMAlgo_KindOfShape myKindOfShape;
  GEOMAlgo_KindOfName myKindOfName;
  GEOMAlgo_KindOfBounds myKindOfBounds;
  GEOMAlgo_KindOfClosed myKindOfClosed;
  GEOMAlgo_KindOfDef myKindOfDef;
  gp_Pnt myLocation;
  gp_Dir myDirection;
  gp_Ax3 myPosition;
  Standard_Real myRadius1;
  Standard_Real myRadius2;
  Standard_Real myLength;
  Standard_Real myWidth;
  Standard_Real myHeight;
  gp_Pnt myPnt1;
  gp_Pnt myPnt2;
  GEOMAlgo_KindOfPeriod myKindOfPeriod;
  Standard_Integer myNbTypes;
  Standard_Integer myNbKnots;
  Standard_Integer myNbPoles;
  Standard_Integer myNbWeights;
  Standard_Integer myNbMultiplicities;
  Standard_Integer myDegree;
  Handle(TColgp_HArray1OfPnt)      myPoles;
  Handle(TColStd_HArray1OfReal)    myKnots;
  Handle(TColStd_HArray1OfReal)    myWeights;
  Handle(TColStd_HArray1OfInteger) myMultiplicities;
};
#endif
