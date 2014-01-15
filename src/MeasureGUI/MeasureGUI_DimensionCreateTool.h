// Copyright (C) 2007-2013  CEA/DEN, EDF R&D, OPEN CASCADE
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
//

// GEOM GEOMGUI : GUI for Geometry component
// File   : MeasureGUI_DimensionCreateTool.h
// Author : Anton POLETAEV, Open CASCADE S.A.S.
//
#ifndef MEASUREGUI_DIMENSIONCREATETOOL_H
#define MEASUREGUI_DIMENSIONCREATETOOL_H

// GEOM includes
#include <GeometryGUI.h>
#include <GEOM_GenericObjPtr.h>

// OCCT includes
#include <AIS_LengthDimension.hxx>
#include <AIS_DiameterDimension.hxx>
#include <AIS_AngleDimension.hxx>
#include <Bnd_Box.hxx>
#include <gp_Dir.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TColgp_SequenceOfDir.hxx>

//=================================================================================
// class    : MeasureGUI_DimensionCreateTool
// purpose  :
//=================================================================================
class MeasureGUI_DimensionCreateTool
{
public:
  MeasureGUI_DimensionCreateTool( GeometryGUI* );

/* construction methods */
public:
  Handle(AIS_LengthDimension)   LengthOnEdge( const GEOM::GeomObjPtr& );

  Handle(AIS_LengthDimension)   LengthByPoints( const GEOM::GeomObjPtr&,
                                                const GEOM::GeomObjPtr& );

  Handle(AIS_LengthDimension)   LengthByParallelEdges( const GEOM::GeomObjPtr&,
                                                       const GEOM::GeomObjPtr& );

  Handle(AIS_DiameterDimension) Diameter( const GEOM::GeomObjPtr& );

  Handle(AIS_AngleDimension)    AngleByTwoEdges( const GEOM::GeomObjPtr&,
                                                 const GEOM::GeomObjPtr& );

  Handle(AIS_AngleDimension)    AngleByThreePoints( const GEOM::GeomObjPtr&,
                                                    const GEOM::GeomObjPtr&,
                                                    const GEOM::GeomObjPtr& );

/* selecting flyout direction for length dimensions */
protected:

  void ChooseLengthFlyoutsFromShape( TColgp_SequenceOfDir&,
                                     const TopoDS_Vertex&,
                                     const TopoDS_Vertex&,
                                     const TopoDS_Shape& );

  void ChooseLengthFlyoutsFromShape( TColgp_SequenceOfDir&,
                                     const TopoDS_Edge&,
                                     const TopoDS_Shape& );

  void ChooseLengthFlyoutsFromBnd( TColgp_SequenceOfDir&,
                                   const gp_Pnt&,
                                   const gp_Pnt&,
                                   const Bnd_Box& );

/* selecting best flyout direction taking into account view projection */
protected:

  gp_Pln SelectPlaneForProjection( const NCollection_Sequence<gp_Pln>&,
                                   const Handle(V3d_View)& );

/* utility */
protected:
  GEOM::GeomObjPtr              GetMainShape( const GEOM::GeomObjPtr& );
  bool                          GetFaceSide( const TopoDS_Face&,
                                             const TopoDS_Edge&,
                                             gp_Dir& );

private:
  GeometryGUI*                  myGeomGUI;
};

#endif
