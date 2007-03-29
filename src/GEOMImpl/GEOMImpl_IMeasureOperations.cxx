// Copyright (C) 2005  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful
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
#include <Standard_Stream.hxx>

#include <GEOMImpl_IMeasureOperations.hxx>

#include <GEOMImpl_Types.hxx>
#include <GEOMImpl_MeasureDriver.hxx>
#include <GEOMImpl_IMeasure.hxx>

#include <GEOM_Function.hxx>
#include <GEOM_PythonDump.hxx>

#include "utilities.h"
#include <OpUtil.hxx>
#include <Utils_ExceptHandlers.hxx>

// OCCT Includes
#include <TFunction_DriverTable.hxx>
#include <TFunction_Driver.hxx>
#include <TFunction_Logbook.hxx>
#include <TDF_Tool.hxx>

#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepBndLib.hxx>
#include <BRepCheck.hxx>
#include <BRepCheck_Result.hxx>
#include <BRepCheck_ListIteratorOfListOfStatus.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <BRepGProp.hxx>
#include <BRepTools.hxx>

#include <Bnd_Box.hxx>

#include <GProp_GProps.hxx>
#include <GProp_PrincipalProps.hxx>

#include <TopAbs.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

#include <GeomAbs_SurfaceType.hxx>
#include <Geom_Surface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_OffsetSurface.hxx>

#include <gp_Pln.hxx>

#include <Standard_Failure.hxx>
#include <Standard_ErrorHandler.hxx> // CAREFUL ! position of this file is critic : see Lucien PIGNOLONI / OCC

//=============================================================================
/*!
 *   constructor:
 */
//=============================================================================
GEOMImpl_IMeasureOperations::GEOMImpl_IMeasureOperations (GEOM_Engine* theEngine, int theDocID)
: GEOM_IOperations(theEngine, theDocID)
{
  MESSAGE("GEOMImpl_IMeasureOperations::GEOMImpl_IMeasureOperations");
}

//=============================================================================
/*!
 *  destructor
 */
//=============================================================================
GEOMImpl_IMeasureOperations::~GEOMImpl_IMeasureOperations()
{
  MESSAGE("GEOMImpl_IMeasureOperations::~GEOMImpl_IMeasureOperations");
}

//=============================================================================
/*! Get kind and parameters of the given shape.
 */
//=============================================================================
GEOMImpl_IMeasureOperations::ShapeKind GEOMImpl_IMeasureOperations::KindOfShape
                             (Handle(GEOM_Object) theShape,
                              Handle(TColStd_HSequenceOfInteger)& theIntegers,
                              Handle(TColStd_HSequenceOfReal)&    theDoubles)
{
  SetErrorCode(KO);
  ShapeKind aKind = SK_NO_SHAPE;

  if (theIntegers.IsNull()) theIntegers = new TColStd_HSequenceOfInteger;
  else                      theIntegers->Clear();

  if (theDoubles.IsNull()) theDoubles = new TColStd_HSequenceOfReal;
  else                     theDoubles->Clear();

  if (theShape.IsNull())
    return aKind;

  Handle(GEOM_Function) aRefShape = theShape->GetLastFunction();
  if (aRefShape.IsNull()) return aKind;

  TopoDS_Shape aShape = aRefShape->GetValue();
  if (aShape.IsNull()) return aKind;

  TopAbs_ShapeEnum aType = aShape.ShapeType();
  switch (aType)
  {
    //??? geompy.kind.compound     nb_solids nb_faces nb_edges nb_vertices
    //??? geompy.kind.compsolid    nb_solids nb_faces nb_edges nb_vertices
    //? "nb_faces" - all faces or only 'standalone' faces?
    case TopAbs_COMPOUND:
      aKind = SK_COMPOUND;
      //
      break;
    case TopAbs_COMPSOLID:
      aKind = SK_COMPSOLID;
      //
      break;
    case TopAbs_SHELL:
      //geompy.kind.shell        geompy.info.closed   nb_faces nb_edges nb_vertices
      //geompy.kind.shell        geompy.info.unclosed nb_faces nb_edges nb_vertices
      aKind = SK_SHELL;
      //
      break;
    case TopAbs_WIRE:
      //geompy.kind.wire         geompy.info.closed   nb_edges nb_vertices
      //geompy.kind.wire         geompy.info.unclosed nb_edges nb_vertices
      aKind = SK_WIRE;
      //
      break;
    case TopAbs_SOLID:
      //geompy.kind.sphere       xc yc zc  R
      //geompy.kind.cylinder     xb yb zb  dx dy dz  R  H
      //geompy.kind.box          xc yc zc  dx dy dz
      //geompy.kind.rotated_box  xo yo zo  zx zy zz  xx xy xz  dx dy dz
      //geompy.kind.torus        xc yc zc  dx dy dz  R_1 R_2
      //geompy.kind.cone         xb yb zb  dx dy dz  H  R_1  R_2
      //geompy.kind.polyhedron   nb_faces nb_edges nb_vertices
      //geompy.kind.solid        nb_faces nb_edges nb_vertices
      aKind = SK_SOLID;
      //if () {
      //  aKind = SK_SPHERE;
      //  aKind = SK_CYLINDER;
      //  aKind = SK_BOX;
      //  aKind = SK_ROTATED_BOX;
      //  aKind = SK_TORUS;
      //  aKind = SK_CONE;
      //  aKind = SK_POLYHEDRON;
      //}
      break;
    case TopAbs_FACE:
      // geompy.kind.sphere2d     xc yc zc  R
      // + geompy.kind.cylinder2d   xb yb zb  dx dy dz  R  H
      // geompy.kind.torus2d      xc yc zc  dx dy dz  R_1 R_2
      // geompy.kind.cone2d       xc yc zc  dx dy dz  R_1 R_2
      // geompy.kind.disk         xc yc zc  dx dy dz  R
      // geompy.kind.ellipse2d    xc yc zc  dx dy dz  R_1 R_2
      // geompy.kind.polygon      xo yo zo  dx dy dz  nb_edges nb_vertices
      // + geompy.kind.planar       xo yo zo  dx dy dz  nb_edges nb_vertices
      // + geompy.kind.face         nb_edges nb_vertices _surface_type_id_
      aKind = SK_FACE;
      {
        TopoDS_Face aF = TopoDS::Face(aShape);

        int nbWires = 0, nbEdges = 0, nbVertices = 0;

        TopTools_MapOfShape mapShape;
        TopExp_Explorer expw (aF, TopAbs_WIRE);
        for (; expw.More(); expw.Next()) {
          if (mapShape.Add(expw.Current())) {
            //listShape.Append(expw.Current());
            nbWires++;
          }
        }

        mapShape.Clear();
        TopExp_Explorer expe (aF, TopAbs_EDGE);
        for (; expe.More(); expe.Next()) {
          if (mapShape.Add(expe.Current())) {
            //listShape.Append(expe.Current());
            nbEdges++;
          }
        }

        mapShape.Clear();
        TopExp_Explorer expf (aF, TopAbs_VERTEX);
        for (; expf.More(); expf.Next()) {
          if (mapShape.Add(expf.Current())) {
            //listShape.Append(expf.Current());
            nbVertices++;
          }
        }

        // Geometry
        Handle(Geom_Surface) aGS = BRep_Tool::Surface(aF);
        if (!aGS.IsNull()) {
          BRepAdaptor_Surface aBAS (aF);

          if (aGS->IsKind(STANDARD_TYPE(Geom_Plane))) {
            // planar
            aKind = SK_PLANAR;

            Handle(Geom_Plane) aGPlane = Handle(Geom_Plane)::DownCast(aGS);
            gp_Pln aPln = aGPlane->Pln();
            gp_Ax3 aPos = aPln.Position();
            gp_Pnt anOri = aPos.Location();
            gp_Dir aDirZ = aPos.Direction();
            //gp_Dir aDirX = aPos.XDirection();

            // xo yo zo
            theDoubles->Append(anOri.X());
            theDoubles->Append(anOri.Y());
            theDoubles->Append(anOri.Z());

            // dx dy dz
            theDoubles->Append(aDirZ.X());
            theDoubles->Append(aDirZ.Y());
            theDoubles->Append(aDirZ.Z());

            // nb_edges nb_vertices (for planar only)
            theIntegers->Append(nbEdges);
            theIntegers->Append(nbVertices);

            //if () {
            //  aKind = SK_DISK;
            //  aKind = SK_ELLIPSE2D;
            //  aKind = SK_POLYGON;
            //}
          }
          else if (aGS->IsKind(STANDARD_TYPE(Geom_SphericalSurface))) {
            //if (/*isSphere*/false) {
            if (aBAS.IsUClosed() && aBAS.IsVClosed()) { // does not work
              Handle(Geom_SphericalSurface) aGSph = Handle(Geom_SphericalSurface)::DownCast(aGS);

              // parameters
              gp_Pnt aLoc = aGSph->Location();
              Standard_Real rr = aGSph->Radius();

              // xc yc zc
              theDoubles->Append(aLoc.X());
              theDoubles->Append(aLoc.Y());
              theDoubles->Append(aLoc.Z());

              // R
              theDoubles->Append(rr);

              aKind = SK_SPHERE2D;
            }
            else {
              // nb_edges nb_vertices (for spherical only)
              theIntegers->Append(nbEdges);
              theIntegers->Append(nbVertices);

              theIntegers->Append((Standard_Integer)GeomAbs_Sphere);
            }
          }
          else if (aGS->IsKind(STANDARD_TYPE(Geom_CylindricalSurface))) {
            // Pure cylinder or just a piece of cylindric surface
            TopLoc_Location aL;
            Handle(Geom_Surface) aGSLoc = BRep_Tool::Surface(aF, aL);

            //aF.Orientation(TopAbs_FORWARD);
            TopExp_Explorer ex (aF, TopAbs_EDGE);
            Standard_Real uMin, uMax, vMin, vMax;
            bool isCylinder = true;
            for (; ex.More(); ex.Next()) {
              // check all edges: pure cylinder has only one seam edge
              //                  and two edges with const v parameter
              TopoDS_Edge E = TopoDS::Edge(ex.Current());

              if (BRep_Tool::IsClosed(E, aGSLoc, aL)) {
                // seam edge
                //TopoDS_Edge ERevr = E;
                //ERevr.Reverse();
                //Handle(Geom2d_Curve) pcRepl1 = BRep_Tool::CurveOnSurface(E    , aF, f,l);
                //Handle(Geom2d_Curve) pcRepl2 = BRep_Tool::CurveOnSurface(ERevr, aF, f,l);
              }
              else {
                BRepTools::UVBounds(aF, E, uMin, uMax, vMin, vMax);
                if (Abs(vMin - vMax) > Precision::Confusion())
                  // neither seam, nor v-constant
                  isCylinder = false;
              }
            }

            if (isCylinder) {
              aKind = SK_CYLINDER2D;

              Handle(Geom_CylindricalSurface) aGCyl = Handle(Geom_CylindricalSurface)::DownCast(aGS);

              // parameters
              gp_Pnt aLoc = aGCyl->Location();
              gp_Ax1 anAx = aGCyl->Axis();
              gp_Dir aDir = anAx.Direction();
              Standard_Real rr = aGCyl->Radius();

              // xb yb zb
              theDoubles->Append(aLoc.X());
              theDoubles->Append(aLoc.Y());
              theDoubles->Append(aLoc.Z());

              // dx dy dz
              theDoubles->Append(aDir.X());
              theDoubles->Append(aDir.Y());
              theDoubles->Append(aDir.Z());

              // R
              theDoubles->Append(rr);

              // H
              Standard_Real hh = Abs(aBAS.FirstVParameter() - aBAS.LastVParameter());
              theDoubles->Append(hh);
            }
            else {
              // nb_edges nb_vertices (for cylinrical only)
              theIntegers->Append(nbEdges);
              theIntegers->Append(nbVertices);

              theIntegers->Append((Standard_Integer)GeomAbs_Cylinder);
            }
          }
          else if (aGS->IsKind(STANDARD_TYPE(Geom_ToroidalSurface))) {
            //  aKind = SK_TORUS2D;
            theIntegers->Append(nbEdges);
            theIntegers->Append(nbVertices);

            theIntegers->Append((Standard_Integer)GeomAbs_Torus);
          }
          else if (aGS->IsKind(STANDARD_TYPE(Geom_ConicalSurface))) {
            //  aKind = SK_CONE2D;
            theIntegers->Append(nbEdges);
            theIntegers->Append(nbVertices);

            theIntegers->Append((Standard_Integer)GeomAbs_Cone);
          }
          else if (aGS->IsKind(STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion))) {
            //
            theIntegers->Append(nbEdges);
            theIntegers->Append(nbVertices);

            theIntegers->Append((Standard_Integer)GeomAbs_SurfaceOfExtrusion);
          }
          else if (aGS->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution))) {
            //
            theIntegers->Append(nbEdges);
            theIntegers->Append(nbVertices);

            theIntegers->Append((Standard_Integer)GeomAbs_SurfaceOfRevolution);
          }
          else if (aGS->IsKind(STANDARD_TYPE(Geom_BezierSurface))) {
            //
            theIntegers->Append(nbEdges);
            theIntegers->Append(nbVertices);

            theIntegers->Append((Standard_Integer)GeomAbs_BezierSurface);
          }
          else if (aGS->IsKind(STANDARD_TYPE(Geom_BSplineSurface))) {
            //
            theIntegers->Append(nbEdges);
            theIntegers->Append(nbVertices);

            theIntegers->Append((Standard_Integer)GeomAbs_BSplineSurface);
          }
          else if (aGS->IsKind(STANDARD_TYPE(Geom_OffsetSurface))) {
            //
            theIntegers->Append(nbEdges);
            theIntegers->Append(nbVertices);

            theIntegers->Append((Standard_Integer)GeomAbs_OffsetSurface);
          }
          else if (aGS->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface))) {
            //
            theIntegers->Append(nbEdges);
            theIntegers->Append(nbVertices);

            theIntegers->Append((Standard_Integer)GeomAbs_OtherSurface);
          }
          else {
            // ???
            theIntegers->Append(nbEdges);
            theIntegers->Append(nbVertices);

            theIntegers->Append((Standard_Integer)GeomAbs_OtherSurface);
          }
        }
      }
      break;
    case TopAbs_EDGE:
      //geompy.kind.circle       xc yc zc  dx dy dz  R
      //geompy.kind.arc          xc yc zc  dx dy dz  R  x1 y1 z1  x2 y2 z2
      //geompy.kind.ellipse      xc yc zc  dx dy dz  R_1 R_2
      //geompy.kind.arcEllipse   xc yc zc  dx dy dz  R_1 R_2  x1 y1 z1  x2 y2 z2
      //geompy.kind.line         x1 y1 z1  x2 y2 z2
      //geompy.kind.segment      x1 y1 z1  x2 y2 z2
      //geompy.kind.edge         nb_vertices _curve_type_id_
      aKind = SK_EDGE;
      //if () {
      //  aKind = SK_CIRCLE;
      //  aKind = SK_ARC;
      //  aKind = SK_ELLIPSE;
      //  aKind = SK_ARC_ELLIPSE;
      //  aKind = SK_LINE;
      //  aKind = SK_SEGMENT;
      //}
      break;
    case TopAbs_VERTEX:
      //geompy.kind.VERTEX  x y z
      aKind = SK_VERTEX;
      {
        TopoDS_Vertex aV = TopoDS::Vertex(aShape);
        gp_Pnt aP = BRep_Tool::Pnt(aV);
        theDoubles->Append(aP.X());
        theDoubles->Append(aP.Y());
        theDoubles->Append(aP.Z());
      }
      break;
  }

  SetErrorCode(OK);
  return aKind;
}

//=============================================================================
/*! Get LCS, corresponding to the given shape.
 *  Origin of the LCS is situated at the shape's center of mass.
 *  Axes of the LCS are obtained from shape's location or,
 *  if the shape is a planar face, from position of its plane.
 */
//=============================================================================
gp_Ax3 GEOMImpl_IMeasureOperations::GetPosition (const TopoDS_Shape& theShape)
{
  gp_Ax3 aResult;

  if (theShape.IsNull())
    return aResult;

  // Axes
  aResult.Transform(theShape.Location().Transformation());
  if (theShape.ShapeType() == TopAbs_FACE) {
    Handle(Geom_Surface) aGS = BRep_Tool::Surface(TopoDS::Face(theShape));
    if (!aGS.IsNull() && aGS->IsKind(STANDARD_TYPE(Geom_Plane))) {
      Handle(Geom_Plane) aGPlane = Handle(Geom_Plane)::DownCast(aGS);
      gp_Pln aPln = aGPlane->Pln();
      aResult = aPln.Position();
    }
  }

  // Origin
  gp_Pnt aPnt;
  if (theShape.ShapeType() == TopAbs_VERTEX) {
    aPnt = BRep_Tool::Pnt(TopoDS::Vertex(theShape));
  }
  else {
    GProp_GProps aSystem;
    if (theShape.ShapeType() == TopAbs_EDGE || theShape.ShapeType() == TopAbs_WIRE)
      BRepGProp::LinearProperties(theShape, aSystem);
    else if (theShape.ShapeType() == TopAbs_FACE || theShape.ShapeType() == TopAbs_SHELL)
      BRepGProp::SurfaceProperties(theShape, aSystem);
    else
      BRepGProp::VolumeProperties(theShape, aSystem);

    aPnt = aSystem.CentreOfMass();
  }

  aResult.SetLocation(aPnt);

  return aResult;
}

//=============================================================================
/*!
 *  GetPosition
 */
//=============================================================================
void GEOMImpl_IMeasureOperations::GetPosition
                   (Handle(GEOM_Object) theShape,
                    Standard_Real& Ox, Standard_Real& Oy, Standard_Real& Oz,
                    Standard_Real& Zx, Standard_Real& Zy, Standard_Real& Zz,
                    Standard_Real& Xx, Standard_Real& Xy, Standard_Real& Xz)
{
  SetErrorCode(KO);

  //Set default values: global CS
  Ox = Oy = Oz = Zx = Zy = Xy = Xz = 0.;
  Zz = Xx = 1.;

  if (theShape.IsNull()) return;

  Handle(GEOM_Function) aRefShape = theShape->GetLastFunction();
  if (aRefShape.IsNull()) return;

  TopoDS_Shape aShape = aRefShape->GetValue();
  if (aShape.IsNull()) {
    SetErrorCode("The Objects has NULL Shape");
    return;
  }

  try {
#if (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE) > 0x060100
    OCC_CATCH_SIGNALS;
#endif

    gp_Ax3 anAx3 = GetPosition(aShape);

    gp_Pnt anOri = anAx3.Location();
    gp_Dir aDirZ = anAx3.Direction();
    gp_Dir aDirX = anAx3.XDirection();

    // Output values
    anOri.Coord(Ox, Oy, Oz);
    aDirZ.Coord(Zx, Zy, Zz);
    aDirX.Coord(Xx, Xy, Xz);
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return;
  }

  SetErrorCode(OK);
}

//=============================================================================
/*!
 *  GetCentreOfMass
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IMeasureOperations::GetCentreOfMass
                                                (Handle(GEOM_Object) theShape)
{
  SetErrorCode(KO);

  if (theShape.IsNull()) return NULL;

  //Add a new CentreOfMass object
  Handle(GEOM_Object) aCDG = GetEngine()->AddObject(GetDocID(), GEOM_CDG);

  //Add a new CentreOfMass function
  Handle(GEOM_Function) aFunction =
    aCDG->AddFunction(GEOMImpl_MeasureDriver::GetID(), CDG_MEASURE);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_MeasureDriver::GetID()) return NULL;

  GEOMImpl_IMeasure aCI (aFunction);

  Handle(GEOM_Function) aRefShape = theShape->GetLastFunction();
  if (aRefShape.IsNull()) return NULL;

  aCI.SetBase(aRefShape);

  //Compute the CentreOfMass value
  try {
#if (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE) > 0x060100
    OCC_CATCH_SIGNALS;
#endif
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Measure driver failed to compute centre of mass");
      return NULL;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction) << aCDG << " = geompy.MakeCDG(" << theShape << ")";

  SetErrorCode(OK);
  return aCDG;
}

//=============================================================================
/*!
 *  GetBasicProperties
 */
//=============================================================================
void GEOMImpl_IMeasureOperations::GetBasicProperties (Handle(GEOM_Object) theShape,
                                                      Standard_Real& theLength,
                                                      Standard_Real& theSurfArea,
                                                      Standard_Real& theVolume)
{
  SetErrorCode(KO);

  if (theShape.IsNull()) return;

  Handle(GEOM_Function) aRefShape = theShape->GetLastFunction();
  if (aRefShape.IsNull()) return;

  TopoDS_Shape aShape = aRefShape->GetValue();
  if (aShape.IsNull()) {
    SetErrorCode("The Objects has NULL Shape");
    return;
  }

  //Compute the parameters
  GProp_GProps LProps, SProps;
  try {
#if (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE) > 0x060100
    OCC_CATCH_SIGNALS;
#endif
    BRepGProp::LinearProperties(aShape, LProps);
    theLength = LProps.Mass();

    BRepGProp::SurfaceProperties(aShape, SProps);
    theSurfArea = SProps.Mass();

    theVolume = 0.0;
    if (aShape.ShapeType() < TopAbs_SHELL) {
      for (TopExp_Explorer Exp (aShape, TopAbs_SOLID); Exp.More(); Exp.Next()) {
	GProp_GProps VProps;
	BRepGProp::VolumeProperties(Exp.Current(), VProps);
	theVolume += VProps.Mass();
      }
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return;
  }

  SetErrorCode(OK);
}

//=============================================================================
/*!
 *  GetInertia
 */
//=============================================================================
void GEOMImpl_IMeasureOperations::GetInertia
                   (Handle(GEOM_Object) theShape,
                    Standard_Real& I11, Standard_Real& I12, Standard_Real& I13,
                    Standard_Real& I21, Standard_Real& I22, Standard_Real& I23,
                    Standard_Real& I31, Standard_Real& I32, Standard_Real& I33,
                    Standard_Real& Ix , Standard_Real& Iy , Standard_Real& Iz)
{
  SetErrorCode(KO);

  if (theShape.IsNull()) return;

  Handle(GEOM_Function) aRefShape = theShape->GetLastFunction();
  if (aRefShape.IsNull()) return;

  TopoDS_Shape aShape = aRefShape->GetValue();
  if (aShape.IsNull()) {
    SetErrorCode("The Objects has NULL Shape");
    return;
  }

  //Compute the parameters
  GProp_GProps System;

  try {
#if (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE) > 0x060100
    OCC_CATCH_SIGNALS;
#endif
    if (aShape.ShapeType() == TopAbs_VERTEX ||
        aShape.ShapeType() == TopAbs_EDGE ||
        aShape.ShapeType() == TopAbs_WIRE) {
      BRepGProp::LinearProperties(aShape, System);
    } else if (aShape.ShapeType() == TopAbs_FACE ||
               aShape.ShapeType() == TopAbs_SHELL) {
      BRepGProp::SurfaceProperties(aShape, System);
    } else {
      BRepGProp::VolumeProperties(aShape, System);
    }
    gp_Mat I = System.MatrixOfInertia();

    I11 = I(1,1);
    I12 = I(1,2);
    I13 = I(1,3);

    I21 = I(2,1);
    I22 = I(2,2);
    I23 = I(2,3);

    I31 = I(3,1);
    I32 = I(3,2);
    I33 = I(3,3);

    GProp_PrincipalProps Pr = System.PrincipalProperties();
    Pr.Moments(Ix,Iy,Iz);
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return;
  }

  SetErrorCode(OK);
}

//=============================================================================
/*!
 *  GetBoundingBox
 */
//=============================================================================
void GEOMImpl_IMeasureOperations::GetBoundingBox
                                     (Handle(GEOM_Object) theShape,
                                      Standard_Real& Xmin, Standard_Real& Xmax,
                                      Standard_Real& Ymin, Standard_Real& Ymax,
                                      Standard_Real& Zmin, Standard_Real& Zmax)
{
  SetErrorCode(KO);

  if (theShape.IsNull()) return;

  Handle(GEOM_Function) aRefShape = theShape->GetLastFunction();
  if (aRefShape.IsNull()) return;

  TopoDS_Shape aShape = aRefShape->GetValue();
  if (aShape.IsNull()) {
    SetErrorCode("The Objects has NULL Shape");
    return;
  }

  //Compute the parameters
  Bnd_Box B;

  try {
#if (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE) > 0x060100
    OCC_CATCH_SIGNALS;
#endif
    BRepBndLib::Add(aShape, B);
    B.Get(Xmin, Ymin, Zmin, Xmax, Ymax, Zmax);
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return;
  }

  SetErrorCode(OK);
}

//=============================================================================
/*!
 *  GetTolerance
 */
//=============================================================================
void GEOMImpl_IMeasureOperations::GetTolerance
                               (Handle(GEOM_Object) theShape,
                                Standard_Real& FaceMin, Standard_Real& FaceMax,
                                Standard_Real& EdgeMin, Standard_Real& EdgeMax,
                                Standard_Real& VertMin, Standard_Real& VertMax)
{
  SetErrorCode(KO);

  if (theShape.IsNull()) return;

  Handle(GEOM_Function) aRefShape = theShape->GetLastFunction();
  if (aRefShape.IsNull()) return;

  TopoDS_Shape aShape = aRefShape->GetValue();
  if (aShape.IsNull()) {
    SetErrorCode("The Objects has NULL Shape");
    return;
  }

  //Compute the parameters
  Standard_Real T;
  FaceMin = EdgeMin = VertMin = RealLast();
  FaceMax = EdgeMax = VertMax = -RealLast();

  try {
#if (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE) > 0x060100
    OCC_CATCH_SIGNALS;
#endif
    for (TopExp_Explorer ExF (aShape, TopAbs_FACE); ExF.More(); ExF.Next()) {
      TopoDS_Face Face = TopoDS::Face(ExF.Current());
      T = BRep_Tool::Tolerance(Face);
      if (T > FaceMax)
 	FaceMax = T;
      if (T < FaceMin)
	FaceMin = T;
    }
    for (TopExp_Explorer ExE (aShape, TopAbs_EDGE); ExE.More(); ExE.Next()) {
      TopoDS_Edge Edge = TopoDS::Edge(ExE.Current());
      T = BRep_Tool::Tolerance(Edge);
      if (T > EdgeMax)
	EdgeMax = T;
      if (T < EdgeMin)
	EdgeMin = T;
    }
    for (TopExp_Explorer ExV (aShape, TopAbs_VERTEX); ExV.More(); ExV.Next()) {
      TopoDS_Vertex Vertex = TopoDS::Vertex(ExV.Current());
      T = BRep_Tool::Tolerance(Vertex);
      if (T > VertMax)
	VertMax = T;
      if (T < VertMin)
	VertMin = T;
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return;
  }

  SetErrorCode(OK);
}

//=============================================================================
/*!
 *  CheckShape
 */
//=============================================================================
bool GEOMImpl_IMeasureOperations::CheckShape (Handle(GEOM_Object)      theShape,
                                              const Standard_Boolean   theIsCheckGeom,
                                              TCollection_AsciiString& theDump)
{
  SetErrorCode(KO);

  if (theShape.IsNull()) return false;

  Handle(GEOM_Function) aRefShape = theShape->GetLastFunction();
  if (aRefShape.IsNull()) return false;

  TopoDS_Shape aShape = aRefShape->GetValue();
  if (aShape.IsNull()) {
    SetErrorCode("The Objects has NULL Shape");
    return false;
  }

  //Compute the parameters
  bool isValid = false;
  try {
#if (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE) > 0x060100
    OCC_CATCH_SIGNALS;
#endif
    BRepCheck_Analyzer ana (aShape, theIsCheckGeom);
    if (ana.IsValid()) {
      theDump.Clear();
      isValid = true;
    } else {
      StructuralDump(ana, aShape, theDump);
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return false;
  }

  SetErrorCode(OK);
  return isValid;
}

//=============================================================================
/*!
 *  WhatIs
 */
//=============================================================================
TCollection_AsciiString GEOMImpl_IMeasureOperations::WhatIs (Handle(GEOM_Object) theShape)
{
  SetErrorCode(KO);

  TCollection_AsciiString Astr;

  if (theShape.IsNull()) return Astr;

  Handle(GEOM_Function) aRefShape = theShape->GetLastFunction();
  if (aRefShape.IsNull()) return Astr;

  TopoDS_Shape aShape = aRefShape->GetValue();
  if (aShape.IsNull()) {
    SetErrorCode("The Objects has NULL Shape");
    return Astr;
  }

  //Compute the parameters
  if (aShape.ShapeType() == TopAbs_EDGE) {
    if (BRep_Tool::Degenerated(TopoDS::Edge(aShape))) {
      Astr = Astr + " It is a degenerated edge \n";
    }
  }

  Astr = Astr + " Number of sub-shapes : \n";

  try {
#if (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE) > 0x060100
    OCC_CATCH_SIGNALS;
#endif
    int iType, nbTypes [TopAbs_SHAPE];
    for (iType = 0; iType < TopAbs_SHAPE; ++iType)
      nbTypes[iType] = 0;
    nbTypes[aShape.ShapeType()]++;

    TopTools_MapOfShape aMapOfShape;
    aMapOfShape.Add(aShape);
    TopTools_ListOfShape aListOfShape;
    aListOfShape.Append(aShape);

    TopTools_ListIteratorOfListOfShape itL (aListOfShape);
    for (; itL.More(); itL.Next()) {
      TopoDS_Iterator it (itL.Value());
      for (; it.More(); it.Next()) {
        TopoDS_Shape s = it.Value();
        if (aMapOfShape.Add(s)) {
          aListOfShape.Append(s);
          nbTypes[s.ShapeType()]++;
        }
      }
    }

    Astr = Astr + " VERTEX : " + TCollection_AsciiString(nbTypes[TopAbs_VERTEX]) + "\n";
    Astr = Astr + " EDGE : " + TCollection_AsciiString(nbTypes[TopAbs_EDGE]) + "\n";
    Astr = Astr + " WIRE : " + TCollection_AsciiString(nbTypes[TopAbs_WIRE]) + "\n";
    Astr = Astr + " FACE : " + TCollection_AsciiString(nbTypes[TopAbs_FACE]) + "\n";
    Astr = Astr + " SHELL : " + TCollection_AsciiString(nbTypes[TopAbs_SHELL]) + "\n";
    Astr = Astr + " SOLID : " + TCollection_AsciiString(nbTypes[TopAbs_SOLID]) + "\n";
    Astr = Astr + " COMPSOLID : " + TCollection_AsciiString(nbTypes[TopAbs_COMPSOLID]) + "\n";
    Astr = Astr + " COMPOUND : " + TCollection_AsciiString(nbTypes[TopAbs_COMPOUND]) + "\n";
    Astr = Astr + " SHAPE : " + TCollection_AsciiString(aMapOfShape.Extent());
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return Astr;
  }

  SetErrorCode(OK);
  return Astr;
}

//=============================================================================
/*!
 *  GetMinDistance
 */
//=============================================================================
Standard_Real GEOMImpl_IMeasureOperations::GetMinDistance
  (Handle(GEOM_Object) theShape1, Handle(GEOM_Object) theShape2,
   Standard_Real& X1, Standard_Real& Y1, Standard_Real& Z1,
   Standard_Real& X2, Standard_Real& Y2, Standard_Real& Z2)
{
  SetErrorCode(KO);
  Standard_Real MinDist = 1.e9;

  if (theShape1.IsNull() || theShape2.IsNull()) return MinDist;

  Handle(GEOM_Function) aRefShape1 = theShape1->GetLastFunction();
  Handle(GEOM_Function) aRefShape2 = theShape2->GetLastFunction();
  if (aRefShape1.IsNull() || aRefShape2.IsNull()) return MinDist;

  TopoDS_Shape aShape1 = aRefShape1->GetValue();
  TopoDS_Shape aShape2 = aRefShape2->GetValue();
  if (aShape1.IsNull() || aShape2.IsNull()) {
    SetErrorCode("One of Objects has NULL Shape");
    return MinDist;
  }

  //Compute the parameters
  try {
#if (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE) > 0x060100
    OCC_CATCH_SIGNALS;
#endif
    BRepExtrema_DistShapeShape dst (aShape1, aShape2);
    if (dst.IsDone()) {
      gp_Pnt PMin1, PMin2, P1, P2;

      for (int i = 1; i <= dst.NbSolution(); i++) {
	P1 = dst.PointOnShape1(i);
	P2 = dst.PointOnShape2(i);

	Standard_Real Dist = P1.Distance(P2);
	if (MinDist > Dist) {
	  MinDist = Dist;
          PMin1 = P1;
          PMin2 = P2;
        }
      }

      PMin1.Coord(X1, Y1, Z1);
      PMin2.Coord(X2, Y2, Z2);
    }
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode(aFail->GetMessageString());
    return MinDist;
  }

  SetErrorCode(OK);
  return MinDist;
}

//=======================================================================
//function : PointCoordinates
//purpose  : Get coordinates of point
//=======================================================================
void GEOMImpl_IMeasureOperations::PointCoordinates( Handle(GEOM_Object) theShape,
                        Standard_Real& theX, Standard_Real& theY, Standard_Real& theZ )
{
  SetErrorCode( KO );

  if ( theShape.IsNull() )
    return;

  Handle(GEOM_Function) aRefShape = theShape->GetLastFunction();
  if ( aRefShape.IsNull() )
    return;

  TopoDS_Shape aShape = aRefShape->GetValue();
  if ( aShape.IsNull() || aShape.ShapeType() != TopAbs_VERTEX )
  {
    SetErrorCode( "Shape must be a vertex" );
    return;
  }

  try {
#if (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE) > 0x060100
    OCC_CATCH_SIGNALS;
#endif
    gp_Pnt aPnt = BRep_Tool::Pnt( TopoDS::Vertex( aShape ) );
    theX = aPnt.X();
    theY = aPnt.Y();
    theZ = aPnt.Z();
    SetErrorCode( OK );
  }
  catch ( Standard_Failure )
  {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    SetErrorCode( aFail->GetMessageString() );
  }
}

//=======================================================================
//function : StructuralDump
//purpose  : Structural (data exchange) style of output.
//=======================================================================
void GEOMImpl_IMeasureOperations::StructuralDump (const BRepCheck_Analyzer& theAna,
                                                  const TopoDS_Shape&       theShape,
                                                  TCollection_AsciiString&  theDump)
{
  Standard_Integer i;
  theDump.Clear();
  theDump += " -- The Shape has problems :\n";
  theDump += "  Check                                    Count\n";
  theDump += " ------------------------------------------------\n";

  Standard_Integer last_stat = (Standard_Integer)BRepCheck_CheckFail;
  Handle(TColStd_HArray1OfInteger) NbProblems =
    new TColStd_HArray1OfInteger(1, last_stat);
  for (i = 1; i <= last_stat; i++)
    NbProblems->SetValue(i,0);

  Handle(TopTools_HSequenceOfShape) sl;
  sl = new TopTools_HSequenceOfShape();
  TopTools_DataMapOfShapeListOfShape theMap;
  theMap.Clear();
  GetProblemShapes(theAna, theShape, sl, NbProblems, theMap);
  theMap.Clear();

  Standard_Integer count = 0;
  count = NbProblems->Value((Standard_Integer)BRepCheck_InvalidPointOnCurve);
  if (count > 0) {
    theDump += "  Invalid Point on Curve ................... ";
    theDump += TCollection_AsciiString(count) + "\n";
  }
  count = NbProblems->Value((Standard_Integer)BRepCheck_InvalidPointOnCurveOnSurface);
  if (count > 0) {
    theDump += "  Invalid Point on CurveOnSurface .......... ";
    theDump += TCollection_AsciiString(count) + "\n";
  }
  count = NbProblems->Value((Standard_Integer)BRepCheck_InvalidPointOnSurface);
  if (count > 0) {
    theDump += "  Invalid Point on Surface ................. ";
    theDump += TCollection_AsciiString(count) + "\n";
  }
  count = NbProblems->Value((Standard_Integer)BRepCheck_No3DCurve);
  if (count > 0) {
    theDump += "  No 3D Curve .............................. ";
    theDump += TCollection_AsciiString(count) + "\n";
  }
  count = NbProblems->Value((Standard_Integer)BRepCheck_Multiple3DCurve);
  if (count > 0) {
    theDump += "  Multiple 3D Curve ........................ ";
    theDump += TCollection_AsciiString(count) + "\n";
  }
  count = NbProblems->Value((Standard_Integer)BRepCheck_Invalid3DCurve);
  if (count > 0) {
    theDump += "  Invalid 3D Curve ......................... ";
    theDump += TCollection_AsciiString(count) + "\n";
  }
  count = NbProblems->Value((Standard_Integer)BRepCheck_NoCurveOnSurface);
  if (count > 0) {
    theDump += "  No Curve on Surface ...................... ";
    theDump += TCollection_AsciiString(count) + "\n";
  }
  count = NbProblems->Value((Standard_Integer)BRepCheck_InvalidCurveOnSurface);
  if (count > 0) {
    theDump += "  Invalid Curve on Surface ................. ";
    theDump += TCollection_AsciiString(count) + "\n";
  }
  count = NbProblems->Value((Standard_Integer)BRepCheck_InvalidCurveOnClosedSurface);
  if (count > 0) {
    theDump += "  Invalid Curve on closed Surface .......... ";
    theDump += TCollection_AsciiString(count) + "\n";
  }
  count = NbProblems->Value((Standard_Integer)BRepCheck_InvalidSameRangeFlag);
  if (count > 0) {
    theDump += "  Invalid SameRange Flag ................... ";
    theDump += TCollection_AsciiString(count) + "\n";
  }
  count = NbProblems->Value((Standard_Integer)BRepCheck_InvalidSameParameterFlag);
  if (count > 0) {
    theDump += "  Invalid SameParameter Flag ............... ";
    theDump += TCollection_AsciiString(count) + "\n";
  }
  count = NbProblems->Value((Standard_Integer)BRepCheck_InvalidDegeneratedFlag);
  if (count > 0) {
    theDump += "  Invalid Degenerated Flag ................. ";
    theDump += TCollection_AsciiString(count) + "\n";
  }
  count = NbProblems->Value((Standard_Integer)BRepCheck_FreeEdge);
  if (count > 0) {
    theDump += "  Free Edge ................................ ";
    theDump += TCollection_AsciiString(count) + "\n";
  }
  count = NbProblems->Value((Standard_Integer)BRepCheck_InvalidMultiConnexity);
  if (count > 0) {
    theDump += "  Invalid MultiConnexity ................... ";
    theDump += TCollection_AsciiString(count) + "\n";
  }
  count = NbProblems->Value((Standard_Integer)BRepCheck_InvalidRange);
  if (count > 0) {
    theDump += "  Invalid Range ............................ ";
    theDump += TCollection_AsciiString(count) + "\n";
  }
  count = NbProblems->Value((Standard_Integer)BRepCheck_EmptyWire);
  if (count > 0) {
    theDump += "  Empty Wire ............................... ";
    theDump += TCollection_AsciiString(count) + "\n";
  }
  count = NbProblems->Value((Standard_Integer)BRepCheck_RedundantEdge);
  if (count > 0) {
    theDump += "  Redundant Edge ........................... ";
    theDump += TCollection_AsciiString(count) + "\n";
  }
  count = NbProblems->Value((Standard_Integer)BRepCheck_SelfIntersectingWire);
  if (count > 0) {
    theDump += "  Self Intersecting Wire ................... ";
    theDump += TCollection_AsciiString(count) + "\n";
  }
  count = NbProblems->Value((Standard_Integer)BRepCheck_NoSurface);
  if (count > 0) {
    theDump += "  No Surface ............................... ";
    theDump += TCollection_AsciiString(count) + "\n";
  }
  count = NbProblems->Value((Standard_Integer)BRepCheck_InvalidWire);
  if (count > 0) {
    theDump += "  Invalid Wire ............................. ";
    theDump += TCollection_AsciiString(count) + "\n";
  }
  count = NbProblems->Value((Standard_Integer)BRepCheck_RedundantWire);
  if (count > 0) {
    theDump += "  Redundant Wire ........................... ";
    theDump += TCollection_AsciiString(count) + "\n";
  }
  count = NbProblems->Value((Standard_Integer)BRepCheck_IntersectingWires);
  if (count > 0) {
    theDump += "  Intersecting Wires ....................... ";
    theDump += TCollection_AsciiString(count) + "\n";
  }
  count = NbProblems->Value((Standard_Integer)BRepCheck_InvalidImbricationOfWires);
  if (count > 0) {
    theDump += "  Invalid Imbrication of Wires ............. ";
    theDump += TCollection_AsciiString(count) + "\n";
  }
  count = NbProblems->Value((Standard_Integer)BRepCheck_EmptyShell);
  if (count > 0) {
    theDump += "  Empty Shell .............................. ";
    theDump += TCollection_AsciiString(count) + "\n";
  }
  count = NbProblems->Value((Standard_Integer)BRepCheck_RedundantFace);
  if (count > 0) {
    theDump += "  Redundant Face ........................... ";
    theDump += TCollection_AsciiString(count) + "\n";
  }
  count = NbProblems->Value((Standard_Integer)BRepCheck_UnorientableShape);
  if (count > 0) {
    theDump += "  Unorientable Shape ....................... ";
    theDump += TCollection_AsciiString(count) + "\n";
  }
  count = NbProblems->Value((Standard_Integer)BRepCheck_NotClosed);
  if (count > 0) {
    theDump += "  Not Closed ............................... ";
    theDump += TCollection_AsciiString(count) + "\n";
  }
  count = NbProblems->Value((Standard_Integer)BRepCheck_NotConnected);
  if (count > 0) {
    theDump += "  Not Connected ............................ ";
    theDump += TCollection_AsciiString(count) + "\n";
  }
  count = NbProblems->Value((Standard_Integer)BRepCheck_SubshapeNotInShape);
  if (count > 0) {
    theDump += "  Subshape not in Shape .................... ";
    theDump += TCollection_AsciiString(count) + "\n";
  }
  count = NbProblems->Value((Standard_Integer)BRepCheck_BadOrientation);
  if (count > 0) {
    theDump += "  Bad Orientation .......................... ";
    theDump += TCollection_AsciiString(count) + "\n";
  }
  count = NbProblems->Value((Standard_Integer)BRepCheck_BadOrientationOfSubshape);
  if (count > 0) {
    theDump += "  Bad Orientation of Subshape .............. ";
    theDump += TCollection_AsciiString(count) + "\n";
  }
  count = NbProblems->Value((Standard_Integer)BRepCheck_CheckFail);
  if (count > 0) {
    theDump += "  checkshape failure ....................... ";
    theDump += TCollection_AsciiString(count) + "\n";
  }

  theDump += " ------------------------------------------------\n";
  theDump += "*** Shapes with problems : ";
  theDump += TCollection_AsciiString(sl->Length()) + "\n";

  Standard_Integer nbv, nbe, nbw, nbf, nbs, nbo;
  nbv = nbe = nbw = nbf = nbs = nbo = 0;

  for (i = 1; i <= sl->Length(); i++) {
    TopoDS_Shape shi = sl->Value(i);
    TopAbs_ShapeEnum sti = shi.ShapeType();
    switch (sti) {
      case TopAbs_VERTEX : nbv++; break;
      case TopAbs_EDGE   : nbe++; break;
      case TopAbs_WIRE   : nbw++; break;
      case TopAbs_FACE   : nbf++; break;
      case TopAbs_SHELL  : nbs++; break;
      case TopAbs_SOLID  : nbo++; break;
      default            : break;
    }
  }

  if (nbv > 0) {
    theDump += "VERTEX : ";
    if (nbv < 10) theDump += " ";
    theDump += TCollection_AsciiString(nbv) + "\n";
  }
  if (nbe > 0) {
    theDump += "EDGE   : ";
    if (nbe < 10) theDump += " ";
    theDump += TCollection_AsciiString(nbe) + "\n";
  }
  if (nbw > 0) {
    theDump += "WIRE   : ";
    if (nbw < 10) theDump += " ";
    theDump += TCollection_AsciiString(nbw) + "\n";
  }
  if (nbf > 0) {
    theDump += "FACE   : ";
    if (nbf < 10) theDump += " ";
    theDump += TCollection_AsciiString(nbf) + "\n";
  }
  if (nbs > 0) {
    theDump += "SHELL  : ";
    if (nbs < 10) theDump += " ";
    theDump += TCollection_AsciiString(nbs) + "\n";
  }
  if (nbo > 0) {
    theDump += "SOLID  : ";
    if (nbo < 10) theDump += " ";
    theDump += TCollection_AsciiString(nbo) + "\n";
  }
}

//=======================================================================
//function : GetProblemShapes
// purpose : for StructuralDump
//=======================================================================
void GEOMImpl_IMeasureOperations::GetProblemShapes (const BRepCheck_Analyzer&           theAna,
                                                    const TopoDS_Shape&                 theShape,
                                                    Handle(TopTools_HSequenceOfShape)&  sl,
                                                    Handle(TColStd_HArray1OfInteger)&   NbProblems,
                                                    TopTools_DataMapOfShapeListOfShape& theMap)
{
  for (TopoDS_Iterator iter(theShape); iter.More(); iter.Next()) {
    GetProblemShapes(theAna, iter.Value(), sl, NbProblems, theMap);
  }
  TopAbs_ShapeEnum styp = theShape.ShapeType();
  BRepCheck_ListIteratorOfListOfStatus itl;
  TopTools_ListOfShape empty;
  if (!theMap.IsBound(theShape)) {
    theMap.Bind(theShape,empty);

    if (!theAna.Result(theShape).IsNull()) {
      itl.Initialize(theAna.Result(theShape)->Status());
      // !!! May be, we have to print all the problems, not only the first one ?
      if (itl.Value() != BRepCheck_NoError) {
        sl->Append(theShape);
        BRepCheck_Status stat = itl.Value();
        NbProblems->SetValue((Standard_Integer)stat,
                             NbProblems->Value((Standard_Integer)stat) + 1);
      }
    }
  }

  switch (styp) {
  case TopAbs_EDGE:
    GetProblemSub(theAna, theShape, sl, NbProblems, TopAbs_VERTEX, theMap);
    break;
  case TopAbs_FACE:
    GetProblemSub(theAna, theShape, sl, NbProblems, TopAbs_WIRE, theMap);
    GetProblemSub(theAna, theShape, sl, NbProblems, TopAbs_EDGE, theMap);
    GetProblemSub(theAna, theShape, sl, NbProblems, TopAbs_VERTEX, theMap);
    break;
  case TopAbs_SHELL:
    break;
  case TopAbs_SOLID:
    GetProblemSub(theAna, theShape, sl, NbProblems, TopAbs_SHELL, theMap);
    break;
  default:
    break;
  }
}

//=======================================================================
//function : Contains
//=======================================================================
static Standard_Boolean Contains (const TopTools_ListOfShape& L,
                                  const TopoDS_Shape& S)
{
  TopTools_ListIteratorOfListOfShape it;
  for (it.Initialize(L); it.More(); it.Next()) {
    if (it.Value().IsSame(S)) {
      return Standard_True;
    }
  }
  return Standard_False;
}

//=======================================================================
//function : GetProblemSub
// purpose : for StructuralDump
//=======================================================================
void GEOMImpl_IMeasureOperations::GetProblemSub (const BRepCheck_Analyzer&           theAna,
                                                 const TopoDS_Shape&                 theShape,
                                                 Handle(TopTools_HSequenceOfShape)&  sl,
                                                 Handle(TColStd_HArray1OfInteger)&   NbProblems,
                                                 const TopAbs_ShapeEnum              Subtype,
                                                 TopTools_DataMapOfShapeListOfShape& theMap)
{
  BRepCheck_ListIteratorOfListOfStatus itl;
  TopExp_Explorer exp;
  for (exp.Init(theShape, Subtype); exp.More(); exp.Next()) {
    const TopoDS_Shape& sub = exp.Current();

    const Handle(BRepCheck_Result)& res = theAna.Result(sub);
    for (res->InitContextIterator();
	 res->MoreShapeInContext();
	 res->NextShapeInContext()) {
      if (res->ContextualShape().IsSame(theShape) && !Contains(theMap(sub), theShape)) {
	theMap(sub).Append(theShape);
	itl.Initialize(res->StatusOnShape());

	if (itl.Value() != BRepCheck_NoError) {
	  Standard_Integer ii = 0;

          for (ii = 1; ii <= sl->Length(); ii++)
            if (sl->Value(ii).IsSame(sub)) break;

          if (ii > sl->Length()) {
            sl->Append(sub);
            BRepCheck_Status stat = itl.Value();
            NbProblems->SetValue((Standard_Integer)stat,
                                 NbProblems->Value((Standard_Integer)stat) + 1);
          }
          for (ii = 1; ii <= sl->Length(); ii++)
            if (sl->Value(ii).IsSame(theShape)) break;
          if (ii > sl->Length()) {
            sl->Append(theShape);
            BRepCheck_Status stat = itl.Value();
            NbProblems->SetValue((Standard_Integer)stat,
                                 NbProblems->Value((Standard_Integer)stat) + 1);
          }
	}
	break;
      }
    }
  }
}
