//  File      : GEOM_OCCReader.h
//  Created   : Wed Feb 20 17:24:59 2002
//  Author    : Christophe ATTANASIO
//  Project   : SALOME
//  Module    : GEOM
//  Copyright : Open CASCADE 2002
//  $Header$

/*!
  \class GEOM_OCCReader GEOM_OCCReader.h
  \brief This class allow to display a OpenCASCADE CAD model in a VTK viewer.
*/

#ifndef GEOM_OCCREADER_H
#define GEOM_OCCREADER_H

// VTK
#include <vtkPolyDataSource.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>

// OpenCASCADE
#include <TopoDS_Shape.hxx>
#include <TopoDS.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <GeomAbs_IsoType.hxx>
#include <BRepAdaptor_Surface.hxx>

#ifdef _WIN_32
#define VTKOCC_EXPORT __declspec (dllexport)
#else
#define VTKOCC_EXPORT
#endif
class VTKOCC_EXPORT GEOM_OCCReader : public vtkPolyDataSource {

  // methods	

 public:	

  static GEOM_OCCReader* New();

  const TopoDS_Shape& getTopo();

  void setTopo(const TopoDS_Shape& ashape);

  int  getDisplayMode();
  void setDisplayMode(int);

  void setForceUpdate(Standard_Boolean bol);

 protected:

  GEOM_OCCReader();
  ~GEOM_OCCReader();
  void Execute();	

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
		 double, int,
		 vtkPoints* Pts,
		 vtkCellArray* Cells);
  
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
  int			 amode;
  int                    nbisos;
  TopoDS_Shape           myShape;

};

#endif //GEOM_OCCREADER_H
