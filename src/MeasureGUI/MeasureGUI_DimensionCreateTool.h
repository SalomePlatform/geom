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
#include <V3d_View.hxx>

//=================================================================================
// class    : MeasureGUI_DimensionCreateTool
// purpose  :
//=================================================================================
class MeasureGUI_DimensionCreateTool
{
 public:
  struct Segment
  {
    gp_Pnt First;
    gp_Pnt Last;
  };
  typedef NCollection_Sequence<gp_Dir>  SeqOfDirs;
  typedef NCollection_Sequence<gp_Pln>  SeqOfPlanes;
  typedef NCollection_Sequence<Segment> SeqOfSegments;

  MeasureGUI_DimensionCreateTool();

  struct
  {
    Standard_Real DefaultFlyout;
    Handle(V3d_View) ActiveView;
  } Settings;

/* construction methods */
public:
  Handle(AIS_LengthDimension)   LengthOnEdge( const GEOM::GeomObjPtr& ) const;

  Handle(AIS_LengthDimension)   LengthByPoints( const GEOM::GeomObjPtr&,
                                                const GEOM::GeomObjPtr& ) const;

  Handle(AIS_LengthDimension)   LengthByParallelEdges( const GEOM::GeomObjPtr&,
                                                       const GEOM::GeomObjPtr& ) const;

  Handle(AIS_DiameterDimension) Diameter( const GEOM::GeomObjPtr& ) const;

  Handle(AIS_AngleDimension)    AngleByTwoEdges( const GEOM::GeomObjPtr&,
                                                 const GEOM::GeomObjPtr& ) const;

  Handle(AIS_AngleDimension)    AngleByThreePoints( const GEOM::GeomObjPtr&,
                                                    const GEOM::GeomObjPtr&,
                                                    const GEOM::GeomObjPtr& ) const;


/* selecting flyout direction for length dimensions */
protected:

  void   ChooseLengthFlyoutsFromBnd( SeqOfDirs&,
                                     const gp_Pnt&,
                                     const gp_Pnt&,
                                     const Bnd_Box& ) const;

  gp_Dir ChooseDirFromBnd( const SeqOfDirs&,
                           const gp_Pnt&,
                           const Bnd_Box& ) const;

/* selecting best flyout direction taking into account view projection */
protected:

  template <typename TPlane>
  TPlane SelectPlaneForProjection( const NCollection_Sequence<TPlane>&,
                                   const Handle(V3d_View)& ) const;

/* positioning */
protected:

  void PositionLength( const Bnd_Box&,
                       const gp_Vec&,
                       const gp_Vec&,
                       const gp_Vec&,
                       const gp_Vec&,
                       const gp_Pnt&,
                       const gp_Pnt&,
                       gp_Pln& ) const;

  void PositionDiameter( const Bnd_Box&,
                         const gp_Vec&,
                         const gp_Circ&,
                         gp_Pnt&,
                         gp_Pnt&,
                         gp_Pln& ) const;

  void PositionDiameter( const Bnd_Box&,
                         const gp_Vec&,
                         const gp_Circ&,
                         const Standard_Real&,
                         gp_Pln& ) const;

/* utility */
protected:
  GEOM::GeomObjPtr              GetMainShape( const GEOM::GeomObjPtr& ) const;
  bool                          GetFaceSide( const TopoDS_Face&,
                                             const TopoDS_Edge&,
                                             gp_Dir& ) const;
  SeqOfSegments                 GetInPlaneSegments( const gp_Circ&,
                                                    const SeqOfDirs& ) const;
};

#endif
