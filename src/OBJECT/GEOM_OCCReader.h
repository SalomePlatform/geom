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

//  GEOM OBJECT : interactive object for Geometry entities visualization
//  File   : GEOM_OCCReader.h
//  Author : Christophe ATTANASIO
//  Module : GEOM
//
/*!
  \class GEOM_OCCReader GEOM_OCCReader.h
  \brief This class allow to display a OpenCASCADE CAD model in a VTK viewer.
*/

#ifndef GEOM_OCCREADER_H
#define GEOM_OCCREADER_H

#include "GEOM_OBJECT_defs.hxx"

#include <vtkAlgorithm.h>

class vtkPoints;
class vtkCellArray;
namespace GEOMUtils
{
  class Hatcher;
}

// OpenCASCADE
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <GeomAbs_IsoType.hxx>
#include <BRepAdaptor_Surface.hxx>

class GEOM_OBJECT_EXPORT GEOM_OCCReader : public vtkAlgorithm {

  // methods    

 public:        

  static GEOM_OCCReader* New();

  const TopoDS_Shape& getTopo();

  void setTopo(const TopoDS_Shape& ashape, bool isVector = false);

  int  getDisplayMode();
  void setDisplayMode(int);

  void setForceUpdate(Standard_Boolean bol);

 protected:

  GEOM_OCCReader();
  ~GEOM_OCCReader();

  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

  void ComputeShading(vtkPoints* Pts,vtkCellArray* Cells);
  void ComputeWireframe(vtkPoints* Pts,vtkCellArray* Cells);

  void TransferFaceSData(const TopoDS_Face& aFace,
                         vtkPoints* Pts,
                         vtkCellArray* Cells);

  void TransferFaceWData(const TopoDS_Face& aFace,
                         vtkPoints* Pts,
                         vtkCellArray* Cells);

  void TransferEdgeSData(const TopoDS_Edge& aEdge,
                         vtkPoints* Pts,
                         vtkCellArray* Cells);

  void TransferEdgeWData(const TopoDS_Edge& aEdge,
                         vtkPoints* Pts,
                         vtkCellArray* Cells);

  void TransferVertexWData(const TopoDS_Vertex& aVertex,
                           vtkPoints* Pts,
                           vtkCellArray* Cells);

  void createISO(const TopoDS_Face &, 
                 int,
                 vtkPoints* Pts,
                 vtkCellArray* Cells);

  void createIsos(const GEOMUtils::Hatcher &theHatcher,
                  const Standard_Boolean   IsUIso,
		  Standard_Integer  &pt_start_idx,
		  vtkPoints         *Pts,
		  vtkCellArray      *Cell);
  
  void DrawIso(GeomAbs_IsoType aType,
               Standard_Real PParm,
               Standard_Real p1,
               Standard_Real p2,
               vtkPoints* Pts,
               vtkCellArray* Cells,
               Standard_Integer& startidx);
  
  void MoveTo(gp_Pnt P,
              vtkPoints* Pts);

  void DrawTo(gp_Pnt P,
              vtkPoints* Pts,
              vtkCellArray* Cells);

  void PlotIso(BRepAdaptor_Surface& S, 
               GeomAbs_IsoType T,
               Standard_Real& U, 
               Standard_Real& V, 
               Standard_Real Step, 
               Standard_Boolean& halt,
               vtkPoints* Pts,
               vtkCellArray* Cells);

  // fields

 private:

  Standard_Boolean       forced;
  int                    discretiso;
  int                    amode;
  int                    nbisos;
  TopoDS_Shape           myShape;
  bool                   myIsVector;

};

#endif //GEOM_OCCREADER_H
