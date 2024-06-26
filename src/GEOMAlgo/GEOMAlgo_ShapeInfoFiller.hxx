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

#ifndef _GEOMAlgo_ShapeInfoFiller_HeaderFile
#define _GEOMAlgo_ShapeInfoFiller_HeaderFile

#include <Standard.hxx>
#include <Standard_Macro.hxx>
#include <Standard_Real.hxx>

#include <gp_Pln.hxx>
#include <gp_Sphere.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Cone.hxx>
#include <gp_Torus.hxx>

#include <GeomAbs_CurveType.hxx>
#include <GeomAbs_SurfaceType.hxx>

#include <TopoDS_Solid.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>

#include <GEOMAlgo_ShapeInfo.hxx>
#include <GEOMAlgo_IndexedDataMapOfShapeShapeInfo.hxx>
#include <GEOMAlgo_Algo.hxx>

//=======================================================================
//class    : GEOMAlgo_ShapeInfoFiller
//purpose  :
//=======================================================================
class GEOMAlgo_ShapeInfoFiller  : public GEOMAlgo_Algo
{
 public:
  Standard_EXPORT
    GEOMAlgo_ShapeInfoFiller();

  Standard_EXPORT
    virtual ~GEOMAlgo_ShapeInfoFiller();

  Standard_EXPORT
    void SetShape(const TopoDS_Shape& aS) ;

  Standard_EXPORT
    const TopoDS_Shape& Shape() const;

  Standard_EXPORT
    void SetTolerance(const Standard_Real aT) ;

  Standard_EXPORT
    Standard_Real Tolerance() const;

  Standard_EXPORT
    const GEOMAlgo_ShapeInfo& Info() const;

  Standard_EXPORT
    const GEOMAlgo_ShapeInfo& Info(const TopoDS_Shape& aS) const;

  Standard_EXPORT
    virtual  void Perform() ;

 protected:
  Standard_EXPORT
    virtual  void CheckData() ;

  Standard_EXPORT
    void FillNbSubShapes(const TopoDS_Shape& aS,GEOMAlgo_ShapeInfo& aInfo) ;

  Standard_EXPORT
    void FillSubShapes(const TopoDS_Shape& aS) ;

  Standard_EXPORT
    void FillShape(const TopoDS_Shape& aS) ;

  Standard_EXPORT
    void FillVertex(const TopoDS_Shape& aS) ;

  Standard_EXPORT
    void FillEdge(const TopoDS_Shape& aS) ;

  Standard_EXPORT
    void FillFace(const TopoDS_Shape& aS) ;

  Standard_EXPORT
    void FillSolid(const TopoDS_Shape& aS) ;

  Standard_EXPORT
    void FillContainer(const TopoDS_Shape& aS) ;

  Standard_EXPORT
    void FillDetails(const TopoDS_Face& aF,const gp_Pln& aPln) ;

  Standard_EXPORT
    void FillDetails(const TopoDS_Face& aF,const gp_Sphere& aSph) ;

  Standard_EXPORT
    void FillDetails(const TopoDS_Face& aF,const gp_Cylinder& aCyl) ;

  Standard_EXPORT
    void FillDetails(const TopoDS_Face& aF,const gp_Torus& aTorus) ;

  Standard_EXPORT
    void FillDetails(const TopoDS_Solid& aS) ;

  //modified by NIZNHY-PKV Tue Jul 03 13:31:43 2012f
  Standard_EXPORT
    Standard_Boolean TreatStepSphere(const TopoDS_Solid& aS);

  Standard_EXPORT  static
    Standard_Boolean IsAllowedType(const GeomAbs_CurveType aCT);

  Standard_EXPORT  static
    Standard_Boolean IsAllowedType(const GeomAbs_SurfaceType aST);

  Standard_EXPORT  static
    Standard_Integer NbWires(const TopoDS_Face& aF);

  Standard_EXPORT  static
    Standard_Integer NbShells(const TopoDS_Solid& aS);
  //modified by NIZNHY-PKV Tue Jul 03 13:31:49 2012t

  TopoDS_Shape myShape;
  GEOMAlgo_ShapeInfo myEmptyInfo;
  GEOMAlgo_IndexedDataMapOfShapeShapeInfo myMapInfo;
  Standard_Real myTolerance;
};

#endif
