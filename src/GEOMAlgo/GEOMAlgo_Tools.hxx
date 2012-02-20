// Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

#ifndef _GEOMAlgo_Tools_HeaderFile
#define _GEOMAlgo_Tools_HeaderFile

#include <Basics_OCCTVersion.hxx>

#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif

#ifndef _Standard_Boolean_HeaderFile
#include <Standard_Boolean.hxx>
#endif
#ifndef _Standard_Integer_HeaderFile
#include <Standard_Integer.hxx>
#endif
#ifndef _Standard_Real_HeaderFile
#include <Standard_Real.hxx>
#endif

#if OCC_VERSION_LARGE > 0x06050200
#ifndef _Handle_IntTools_Context_HeaderFile
#include <Handle_IntTools_Context.hxx>
#endif
#endif

#ifndef _Handle_Geom_Surface_HeaderFile
#include <Handle_Geom_Surface.hxx>
#endif

class TopoDS_Shape;
class GEOMAlgo_IndexedDataMapOfPassKeyShapeListOfShape;
class IntTools_Context;
class TopTools_ListOfShape;
class TopTools_IndexedDataMapOfShapeListOfShape;
class gp_Pnt;
class TopoDS_Edge;
class TopoDS_Face;
class Geom_Surface;


class GEOMAlgo_Tools {

public:

  void* operator new(size_t,void* anAddress)
  {
    return anAddress;
  }
  void* operator new(size_t size)
  {
    return Standard::Allocate(size);
  }
  void  operator delete(void *anAddress)
  {
    if (anAddress) Standard::Free((Standard_Address&)anAddress);
  }

  Standard_EXPORT static  Standard_Boolean IsCompositeShape(const TopoDS_Shape& aS);

  Standard_EXPORT static  Standard_Integer RefineSDShapes (GEOMAlgo_IndexedDataMapOfPassKeyShapeListOfShape& aMSD,
                                                           const Standard_Real aTol,
#if OCC_VERSION_LARGE > 0x06050200
                                                           const Handle(IntTools_Context)& aCtx);
#else
                                                           IntTools_Context& aCtx);
#endif

  Standard_EXPORT static  Standard_Integer FindSDShapes (const TopTools_ListOfShape& aLE,
                                                         const Standard_Real aTol,
                                                         TopTools_IndexedDataMapOfShapeListOfShape& aMEE,
#if OCC_VERSION_LARGE > 0x06050200
                                                         const Handle(IntTools_Context)& aCtx);
#else
                                                         IntTools_Context& aCtx);
#endif

  Standard_EXPORT static  Standard_Integer FindSDShapes (const TopoDS_Shape& aE1,
                                                         const TopTools_ListOfShape& aLE,
                                                         const Standard_Real aTol,
                                                         TopTools_ListOfShape& aLESD,
#if OCC_VERSION_LARGE > 0x06050200
                                                         const Handle(IntTools_Context)& aCtx);
#else
                                                         IntTools_Context& aCtx);
#endif

  Standard_EXPORT static  Standard_Boolean ProjectPointOnShape (const gp_Pnt& aP1,
                                                                const TopoDS_Shape& aS,
                                                                gp_Pnt& aP2,
#if OCC_VERSION_LARGE > 0x06050200
                                                                const Handle(IntTools_Context)& aCtx);
#else
                                                                IntTools_Context& aCtx);
#endif

  Standard_EXPORT static  void PointOnShape(const TopoDS_Shape& aS,gp_Pnt& aP3D);

  Standard_EXPORT static  void PointOnEdge(const TopoDS_Edge& aE,gp_Pnt& aP3D);

  Standard_EXPORT static  void PointOnEdge(const TopoDS_Edge& aE,const Standard_Real aT,gp_Pnt& aP3D);

  Standard_EXPORT static  void PointOnFace(const TopoDS_Face& aF,gp_Pnt& aP3D);

  Standard_EXPORT static  void PointOnFace(const TopoDS_Face& aF,const Standard_Real aU,const Standard_Real aV,gp_Pnt& aP3D);

  Standard_EXPORT static  void RefinePCurveForEdgeOnFace(const TopoDS_Edge& aE,const TopoDS_Face& aF,const Standard_Real aU1,const Standard_Real aU2);

  Standard_EXPORT static  Standard_Boolean IsUPeriodic(const Handle(Geom_Surface)& aS);

  Standard_EXPORT static  Standard_Boolean CorrectWires(const TopoDS_Shape& aS);

  Standard_EXPORT static  Standard_Integer BuildPCurveForEdgeOnFace (const TopoDS_Edge& aE,
                                                                     const TopoDS_Edge& aEold,
                                                                     const TopoDS_Face& aF,
#if OCC_VERSION_LARGE > 0x06050200
                                                                     const Handle(IntTools_Context)& aCtx);
#else
                                                                     IntTools_Context& aCtx);
#endif

protected:

private:

};

// other Inline functions and methods (like "C++: function call" methods)

#endif
