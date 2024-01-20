// Copyright (C) 2007-2024  CEA, EDF, OPEN CASCADE
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

#include <GEOMImpl_IMeasureOperations.hxx>
#include <GEOMImpl_IMeasure.hxx>
#include <GEOMImpl_MeasureDriver.hxx>

#include <GEOMImpl_IPatchFace.hxx>
#include <GEOMImpl_IProximity.hxx>
#include <GEOMImpl_PatchFaceDriver.hxx>
#include <GEOMImpl_ShapeProximityDriver.hxx>

#include <GEOMImpl_Types.hxx>

#include <GEOMImpl_IConformity.hxx>
#include <GEOMImpl_ConformityDriver.hxx>

#include <GEOMUtils.hxx>

#include <GEOMAlgo_AlgoTools.hxx>
#include <GEOMAlgo_KindOfName.hxx>
#include <GEOMAlgo_ShapeInfoFiller.hxx>

#include <GEOM_PythonDump.hxx>

#include <utilities.h>

// OCCT Includes
#include <Bnd_Box.hxx>
#include <BOPAlgo_CheckerSI.hxx>
#include <BOPDS_DS.hxx>
#include <BOPDS_MapOfPair.hxx>
#include <BOPDS_Pair.hxx>
#include <BOPTools_AlgoTools.hxx>
#include <BRepBndLib.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepCheck_ListIteratorOfListOfStatus.hxx>
#include <BRepCheck_Shell.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <BRepClass_FaceClassifier.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <BRepExtrema_ShapeProximity.hxx>
#include <BRepExtrema_SelfIntersection.hxx>
#include <BRepExtrema_MapOfIntegerPackedMapOfInteger.hxx>
#include <BRepGProp.hxx>
#include <BRepTools.hxx>
#include <BRep_Tool.hxx>

#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <GeomLProp_CLProps.hxx>
#include <GeomLProp_SLProps.hxx>
#include <GProp_GProps.hxx>
#include <GProp_PrincipalProps.hxx>

#include <ShapeAnalysis_ShapeTolerance.hxx>
#include <ShapeAnalysis_Surface.hxx>

#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfIntegerListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>

#include <Standard_ErrorHandler.hxx> // CAREFUL ! position of this file is critic : see Lucien PIGNOLONI / OCC

#include <set>

//=============================================================================
/*!
 *  Constructor
 */
//=============================================================================
GEOMImpl_IMeasureOperations::GEOMImpl_IMeasureOperations (GEOM_Engine* theEngine)
: GEOM_IOperations(theEngine)
{
  MESSAGE("GEOMImpl_IMeasureOperations::GEOMImpl_IMeasureOperations");
}

//=============================================================================
/*!
 *  Destructor
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

  int geom_type = theShape->GetType();

  // check if it's advanced shape
  if ( geom_type > USER_TYPE ) {
    SetErrorCode(OK);
    return SK_ADVANCED;
  }

  // Call algorithm
  GEOMAlgo_ShapeInfoFiller aSF;
  aSF.SetShape(aShape);
  aSF.Perform();

  Standard_Integer iErr = aSF.ErrorStatus();

  if (iErr) {
    SetErrorCode("Error in GEOMAlgo_ShapeInfoFiller");
    return SK_NO_SHAPE;
  }
  const GEOMAlgo_ShapeInfo& anInfo = aSF.Info();

  // specific processing for some "advanced" objects
  switch ( geom_type ) {
  case GEOM_MARKER:
    // local coordinate system
    // (+) geompy.kind.LCS  xc yc zc xx xy xz yx yy yz zx zy zz

    TopoDS_Face aFace = TopoDS::Face( aShape );
    Handle(Geom_Plane) aPlane = Handle(Geom_Plane)::DownCast( BRep_Tool::Surface( aFace ) );
    gp_Pnt aC = aPlane->Pln().Location();
    gp_Ax3 anAx3 = aPlane->Pln().Position();

    theDoubles->Append(aC.X());
    theDoubles->Append(aC.Y());
    theDoubles->Append(aC.Z());
    
    gp_Dir aD = anAx3.XDirection();
    theDoubles->Append(aD.X());
    theDoubles->Append(aD.Y());
    theDoubles->Append(aD.Z());
    aD = anAx3.YDirection();
    theDoubles->Append(aD.X());
    theDoubles->Append(aD.Y());
    theDoubles->Append(aD.Z());
    aD = anAx3.Direction();
    theDoubles->Append(aD.X());
    theDoubles->Append(aD.Y());
    theDoubles->Append(aD.Z());

    SetErrorCode(OK);
    return SK_LCS;
  }

  // Interpret results
  TopAbs_ShapeEnum aType = anInfo.Type();
  switch (aType)
  {
  case TopAbs_COMPOUND:
  case TopAbs_COMPSOLID:
    {
      // (+) geompy.kind.COMPOUND     nb_solids nb_faces nb_edges nb_vertices
      // (+) geompy.kind.COMPSOLID    nb_solids nb_faces nb_edges nb_vertices
      // ??? "nb_faces" - all faces or only 'standalone' faces?
      if (aType == TopAbs_COMPOUND)
        aKind = SK_COMPOUND;
      else
        aKind = SK_COMPSOLID;

      //theIntegers->Append(anInfo.NbSubShapes(TopAbs_COMPOUND));
      //theIntegers->Append(anInfo.NbSubShapes(TopAbs_COMPSOLID));
      theIntegers->Append(anInfo.NbSubShapes(TopAbs_SOLID));
      theIntegers->Append(anInfo.NbSubShapes(TopAbs_FACE));
      theIntegers->Append(anInfo.NbSubShapes(TopAbs_EDGE));
      theIntegers->Append(anInfo.NbSubShapes(TopAbs_VERTEX));
    }
    break;

  case TopAbs_SHELL:
    {
      // (+) geompy.kind.SHELL  geompy.info.closed   nb_faces nb_edges nb_vertices
      // (+) geompy.kind.SHELL  geompy.info.unclosed nb_faces nb_edges nb_vertices
      aKind = SK_SHELL;

      theIntegers->Append((int)anInfo.KindOfClosed());

      theIntegers->Append(anInfo.NbSubShapes(TopAbs_FACE));
      theIntegers->Append(anInfo.NbSubShapes(TopAbs_EDGE));
      theIntegers->Append(anInfo.NbSubShapes(TopAbs_VERTEX));
    }
    break;

  case TopAbs_WIRE:
    {
      // (+) geompy.kind.WIRE  geompy.info.closed   nb_edges nb_vertices
      // (+) geompy.kind.WIRE  geompy.info.unclosed nb_edges nb_vertices
      aKind = SK_WIRE;

      theIntegers->Append((int)anInfo.KindOfClosed());

      theIntegers->Append(anInfo.NbSubShapes(TopAbs_EDGE));
      theIntegers->Append(anInfo.NbSubShapes(TopAbs_VERTEX));
    }
    break;

  case TopAbs_SOLID:
    {
      aKind = SK_SOLID;

      GEOMAlgo_KindOfName aKN = anInfo.KindOfName();
      switch (aKN)
      {
      case GEOMAlgo_KN_SPHERE:
        // (+) geompy.kind.SPHERE  xc yc zc  R
        {
          aKind = SK_SPHERE;

          gp_Pnt aC = anInfo.Location();
          theDoubles->Append(aC.X());
          theDoubles->Append(aC.Y());
          theDoubles->Append(aC.Z());

          theDoubles->Append(anInfo.Radius1());
        }
        break;
      case GEOMAlgo_KN_CYLINDER:
        // (+) geompy.kind.CYLINDER  xb yb zb  dx dy dz  R  H
        {
          aKind = SK_CYLINDER;

          gp_Pnt aC = anInfo.Location();
          theDoubles->Append(aC.X());
          theDoubles->Append(aC.Y());
          theDoubles->Append(aC.Z());

          gp_Ax3 anAx3 = anInfo.Position();
          gp_Dir aD = anAx3.Direction();
          theDoubles->Append(aD.X());
          theDoubles->Append(aD.Y());
          theDoubles->Append(aD.Z());

          theDoubles->Append(anInfo.Radius1());
          theDoubles->Append(anInfo.Height());
        }
        break;
      case GEOMAlgo_KN_BOX:
        // (+) geompy.kind.BOX  xc yc zc  ax ay az
        {
          aKind = SK_BOX;

          gp_Pnt aC = anInfo.Location();
          theDoubles->Append(aC.X());
          theDoubles->Append(aC.Y());
          theDoubles->Append(aC.Z());

          gp_Ax3 anAx3 = anInfo.Position();
          gp_Dir aD = anAx3.Direction();
          gp_Dir aX = anAx3.XDirection();

          // ax ay az
          if (aD.IsParallel(gp::DZ(), Precision::Angular()) &&
              aX.IsParallel(gp::DX(), Precision::Angular())) {
            theDoubles->Append(anInfo.Length()); // ax'
            theDoubles->Append(anInfo.Width());  // ay'
            theDoubles->Append(anInfo.Height()); // az'
          }
          else if (aD.IsParallel(gp::DZ(), Precision::Angular()) &&
                   aX.IsParallel(gp::DY(), Precision::Angular())) {
            theDoubles->Append(anInfo.Width());  // ay'
            theDoubles->Append(anInfo.Length()); // ax'
            theDoubles->Append(anInfo.Height()); // az'
          }
          else if (aD.IsParallel(gp::DX(), Precision::Angular()) &&
                   aX.IsParallel(gp::DZ(), Precision::Angular())) {
            theDoubles->Append(anInfo.Height()); // az'
            theDoubles->Append(anInfo.Width());  // ay'
            theDoubles->Append(anInfo.Length()); // ax'
          }
          else if (aD.IsParallel(gp::DX(), Precision::Angular()) &&
                   aX.IsParallel(gp::DY(), Precision::Angular())) {
            theDoubles->Append(anInfo.Height()); // az'
            theDoubles->Append(anInfo.Length()); // ax'
            theDoubles->Append(anInfo.Width());  // ay'
          }
          else if (aD.IsParallel(gp::DY(), Precision::Angular()) &&
                   aX.IsParallel(gp::DZ(), Precision::Angular())) {
            theDoubles->Append(anInfo.Width());  // ay'
            theDoubles->Append(anInfo.Height()); // az'
            theDoubles->Append(anInfo.Length()); // ax'
          }
          else if (aD.IsParallel(gp::DY(), Precision::Angular()) &&
                   aX.IsParallel(gp::DX(), Precision::Angular())) {
            theDoubles->Append(anInfo.Length()); // ax'
            theDoubles->Append(anInfo.Height()); // az'
            theDoubles->Append(anInfo.Width());  // ay'
          }
          else {
            // (+) geompy.kind.ROTATED_BOX  xo yo zo  zx zy zz  xx xy xz  ax ay az
            aKind = SK_ROTATED_BOX;

            // Direction and XDirection
            theDoubles->Append(aD.X());
            theDoubles->Append(aD.Y());
            theDoubles->Append(aD.Z());

            theDoubles->Append(aX.X());
            theDoubles->Append(aX.Y());
            theDoubles->Append(aX.Z());

            // ax ay az
            theDoubles->Append(anInfo.Length());
            theDoubles->Append(anInfo.Width());
            theDoubles->Append(anInfo.Height());
          }
        }
        break;
      case GEOMAlgo_KN_TORUS:
        // (+) geompy.kind.TORUS  xc yc zc  dx dy dz  R_1 R_2
        {
          aKind = SK_TORUS;

          gp_Pnt aO = anInfo.Location();
          theDoubles->Append(aO.X());
          theDoubles->Append(aO.Y());
          theDoubles->Append(aO.Z());

          gp_Ax3 anAx3 = anInfo.Position();
          gp_Dir aD = anAx3.Direction();
          theDoubles->Append(aD.X());
          theDoubles->Append(aD.Y());
          theDoubles->Append(aD.Z());

          theDoubles->Append(anInfo.Radius1());
          theDoubles->Append(anInfo.Radius2());
        }
        break;
      case GEOMAlgo_KN_CONE:
        // (+) geompy.kind.CONE  xb yb zb  dx dy dz  R_1 R_2  H
        {
          aKind = SK_CONE;

          gp_Pnt aO = anInfo.Location();
          theDoubles->Append(aO.X());
          theDoubles->Append(aO.Y());
          theDoubles->Append(aO.Z());

          gp_Ax3 anAx3 = anInfo.Position();
          gp_Dir aD = anAx3.Direction();
          theDoubles->Append(aD.X());
          theDoubles->Append(aD.Y());
          theDoubles->Append(aD.Z());

          theDoubles->Append(anInfo.Radius1());
          theDoubles->Append(anInfo.Radius2());
          theDoubles->Append(anInfo.Height());
        }
        break;
      case GEOMAlgo_KN_POLYHEDRON:
        // (+) geompy.kind.POLYHEDRON  nb_faces nb_edges nb_vertices
        {
          aKind = SK_POLYHEDRON;

          theIntegers->Append(anInfo.NbSubShapes(TopAbs_FACE));
          theIntegers->Append(anInfo.NbSubShapes(TopAbs_EDGE));
          theIntegers->Append(anInfo.NbSubShapes(TopAbs_VERTEX));
        }
        break;
      default:
        // (+) geompy.kind.SOLID  nb_faces nb_edges nb_vertices
        {
          theIntegers->Append(anInfo.NbSubShapes(TopAbs_FACE));
          theIntegers->Append(anInfo.NbSubShapes(TopAbs_EDGE));
          theIntegers->Append(anInfo.NbSubShapes(TopAbs_VERTEX));
        }
      }
    }
    break;

  case TopAbs_FACE:
    {
      aKind = SK_FACE;

      GEOMAlgo_KindOfName aKN = anInfo.KindOfName();
      switch (aKN) {
      case GEOMAlgo_KN_SPHERE:
        // (+) geompy.kind.SPHERE2D  xc yc zc  R
        {
          aKind = SK_SPHERE2D;

          gp_Pnt aC = anInfo.Location();
          theDoubles->Append(aC.X());
          theDoubles->Append(aC.Y());
          theDoubles->Append(aC.Z());

          theDoubles->Append(anInfo.Radius1());
        }
        break;
      case GEOMAlgo_KN_CYLINDER:
        // (+) geompy.kind.CYLINDER2D  xb yb zb  dx dy dz  R  H
        {
          aKind = SK_CYLINDER2D;

          gp_Pnt aO = anInfo.Location();
          theDoubles->Append(aO.X());
          theDoubles->Append(aO.Y());
          theDoubles->Append(aO.Z());

          gp_Ax3 anAx3 = anInfo.Position();
          gp_Dir aD = anAx3.Direction();
          theDoubles->Append(aD.X());
          theDoubles->Append(aD.Y());
          theDoubles->Append(aD.Z());

          theDoubles->Append(anInfo.Radius1());
          theDoubles->Append(anInfo.Height());
        }
        break;
      case GEOMAlgo_KN_TORUS:
        // (+) geompy.kind.TORUS2D  xc yc zc  dx dy dz  R_1 R_2
        {
          aKind = SK_TORUS2D;

          gp_Pnt aO = anInfo.Location();
          theDoubles->Append(aO.X());
          theDoubles->Append(aO.Y());
          theDoubles->Append(aO.Z());

          gp_Ax3 anAx3 = anInfo.Position();
          gp_Dir aD = anAx3.Direction();
          theDoubles->Append(aD.X());
          theDoubles->Append(aD.Y());
          theDoubles->Append(aD.Z());

          theDoubles->Append(anInfo.Radius1());
          theDoubles->Append(anInfo.Radius2());
        }
        break;
      case GEOMAlgo_KN_CONE:
        // (+) geompy.kind.CONE2D  xc yc zc  dx dy dz  R_1 R_2  H
        {
          aKind = SK_CONE2D;

          gp_Pnt aO = anInfo.Location();
          theDoubles->Append(aO.X());
          theDoubles->Append(aO.Y());
          theDoubles->Append(aO.Z());

          gp_Ax3 anAx3 = anInfo.Position();
          gp_Dir aD = anAx3.Direction();
          theDoubles->Append(aD.X());
          theDoubles->Append(aD.Y());
          theDoubles->Append(aD.Z());

          theDoubles->Append(anInfo.Radius1());
          theDoubles->Append(anInfo.Radius2());
          theDoubles->Append(anInfo.Height());
        }
        break;
      case GEOMAlgo_KN_DISKCIRCLE:
        // (+) geompy.kind.DISK_CIRCLE  xc yc zc  dx dy dz  R
        {
          aKind = SK_DISK_CIRCLE;

          gp_Pnt aC = anInfo.Location();
          theDoubles->Append(aC.X());
          theDoubles->Append(aC.Y());
          theDoubles->Append(aC.Z());

          gp_Ax3 anAx3 = anInfo.Position();
          gp_Dir aD = anAx3.Direction();
          theDoubles->Append(aD.X());
          theDoubles->Append(aD.Y());
          theDoubles->Append(aD.Z());

          theDoubles->Append(anInfo.Radius1());
        }
        break;
      case GEOMAlgo_KN_DISKELLIPSE:
        // (+) geompy.kind.DISK_ELLIPSE  xc yc zc  dx dy dz  R_1 R_2
        {
          aKind = SK_DISK_ELLIPSE;

          gp_Pnt aC = anInfo.Location();
          theDoubles->Append(aC.X());
          theDoubles->Append(aC.Y());
          theDoubles->Append(aC.Z());

          gp_Ax3 anAx3 = anInfo.Position();
          gp_Dir aD = anAx3.Direction();
          theDoubles->Append(aD.X());
          theDoubles->Append(aD.Y());
          theDoubles->Append(aD.Z());

          theDoubles->Append(anInfo.Radius1());
          theDoubles->Append(anInfo.Radius2());
        }
        break;
      case GEOMAlgo_KN_RECTANGLE:
      case GEOMAlgo_KN_TRIANGLE:
      case GEOMAlgo_KN_QUADRANGLE:
      case GEOMAlgo_KN_POLYGON:
        // (+) geompy.kind.POLYGON  xo yo zo  dx dy dz  nb_edges nb_vertices
        {
          aKind = SK_POLYGON;

          gp_Pnt aO = anInfo.Location();
          theDoubles->Append(aO.X());
          theDoubles->Append(aO.Y());
          theDoubles->Append(aO.Z());

          gp_Ax3 anAx3 = anInfo.Position();
          gp_Dir aD = anAx3.Direction();
          theDoubles->Append(aD.X());
          theDoubles->Append(aD.Y());
          theDoubles->Append(aD.Z());

          theIntegers->Append(anInfo.NbSubShapes(TopAbs_EDGE));
          theIntegers->Append(anInfo.NbSubShapes(TopAbs_VERTEX));
        }
        break;
      case GEOMAlgo_KN_PLANE: // infinite
        // (+) geompy.kind.PLANE  xo yo zo  dx dy dz
        {
          aKind = SK_PLANE;

          gp_Pnt aC = anInfo.Location();
          theDoubles->Append(aC.X());
          theDoubles->Append(aC.Y());
          theDoubles->Append(aC.Z());

          gp_Ax3 anAx3 = anInfo.Position();
          gp_Dir aD = anAx3.Direction();
          theDoubles->Append(aD.X());
          theDoubles->Append(aD.Y());
          theDoubles->Append(aD.Z());

          if (anInfo.KindOfBounds() != GEOMAlgo_KB_INFINITE)
          {
            // (+) geompy.kind.PLANAR  xo yo zo  dx dy dz  nb_edges nb_vertices
            aKind = SK_PLANAR;
            
            theIntegers->Append(anInfo.NbSubShapes(TopAbs_EDGE));
            theIntegers->Append(anInfo.NbSubShapes(TopAbs_VERTEX));
          }
        }
        break;
      default:
        // ??? geompy.kind.FACE  nb_edges nb_vertices _surface_type_id_
        // (+) geompy.kind.FACE  nb_edges nb_vertices
        theIntegers->Append(anInfo.NbSubShapes(TopAbs_EDGE));
        theIntegers->Append(anInfo.NbSubShapes(TopAbs_VERTEX));
      }
    }
    break;

  case TopAbs_EDGE:
    {
      aKind = SK_EDGE;

      GEOMAlgo_KindOfName aKN = anInfo.KindOfName();
      switch (aKN) {
      case GEOMAlgo_KN_CIRCLE:
        {
          // (+) geompy.kind.CIRCLE  xc yc zc  dx dy dz  R
          aKind = SK_CIRCLE;

          gp_Pnt aC = anInfo.Location();
          theDoubles->Append(aC.X());
          theDoubles->Append(aC.Y());
          theDoubles->Append(aC.Z());

          gp_Ax3 anAx3 = anInfo.Position();
          gp_Dir aD = anAx3.Direction();
          theDoubles->Append(aD.X());
          theDoubles->Append(aD.Y());
          theDoubles->Append(aD.Z());

          theDoubles->Append(anInfo.Radius1());
        }
        break;
      case GEOMAlgo_KN_ARCCIRCLE:
        {
          // (+) geompy.kind.ARC_CIRCLE  xc yc zc  dx dy dz  R  x1 y1 z1  x2 y2 z2
          aKind = SK_ARC_CIRCLE;

          gp_Pnt aC = anInfo.Location();
          theDoubles->Append(aC.X());
          theDoubles->Append(aC.Y());
          theDoubles->Append(aC.Z());

          gp_Ax3 anAx3 = anInfo.Position();
          gp_Dir aD = anAx3.Direction();
          theDoubles->Append(aD.X());
          theDoubles->Append(aD.Y());
          theDoubles->Append(aD.Z());

          theDoubles->Append(anInfo.Radius1());

          gp_Pnt aP1 = anInfo.Pnt1();
          theDoubles->Append(aP1.X());
          theDoubles->Append(aP1.Y());
          theDoubles->Append(aP1.Z());

          gp_Pnt aP2 = anInfo.Pnt2();
          theDoubles->Append(aP2.X());
          theDoubles->Append(aP2.Y());
          theDoubles->Append(aP2.Z());
        }
        break;
      case GEOMAlgo_KN_ELLIPSE:
        {
          // (+) geompy.kind.ELLIPSE  xc yc zc  dx dy dz  R_1 R_2  xVx yVx zVx  xVy yVy zVy
          aKind = SK_ELLIPSE;

          gp_Pnt aC = anInfo.Location();
          theDoubles->Append(aC.X());
          theDoubles->Append(aC.Y());
          theDoubles->Append(aC.Z());

          gp_Ax3 anAx3 = anInfo.Position();
          gp_Dir aD = anAx3.Direction();
          theDoubles->Append(aD.X());
          theDoubles->Append(aD.Y());
          theDoubles->Append(aD.Z());

          theDoubles->Append(anInfo.Radius1());
          theDoubles->Append(anInfo.Radius2());

          gp_Dir aXD = anAx3.XDirection();
          theDoubles->Append(aXD.X());
          theDoubles->Append(aXD.Y());
          theDoubles->Append(aXD.Z());

          gp_Dir aYD = anAx3.YDirection();
          theDoubles->Append(aYD.X());
          theDoubles->Append(aYD.Y());
          theDoubles->Append(aYD.Z());
        }
        break;
      case GEOMAlgo_KN_ARCELLIPSE:
        {
          // (+) geompy.kind.ARC_ELLIPSE  xc yc zc  dx dy dz  R_1 R_2  x1 y1 z1  x2 y2 z2  xVx yVx zVx  xVy yVy zVy
          aKind = SK_ARC_ELLIPSE;

          gp_Pnt aC = anInfo.Location();
          theDoubles->Append(aC.X());
          theDoubles->Append(aC.Y());
          theDoubles->Append(aC.Z());

          gp_Ax3 anAx3 = anInfo.Position();
          gp_Dir aD = anAx3.Direction();
          theDoubles->Append(aD.X());
          theDoubles->Append(aD.Y());
          theDoubles->Append(aD.Z());

          theDoubles->Append(anInfo.Radius1());
          theDoubles->Append(anInfo.Radius2());

          gp_Pnt aP1 = anInfo.Pnt1();
          theDoubles->Append(aP1.X());
          theDoubles->Append(aP1.Y());
          theDoubles->Append(aP1.Z());

          gp_Pnt aP2 = anInfo.Pnt2();
          theDoubles->Append(aP2.X());
          theDoubles->Append(aP2.Y());
          theDoubles->Append(aP2.Z());

          gp_Dir aXD = anAx3.XDirection();
          theDoubles->Append(aXD.X());
          theDoubles->Append(aXD.Y());
          theDoubles->Append(aXD.Z());

          gp_Dir aYD = anAx3.YDirection();
          theDoubles->Append(aYD.X());
          theDoubles->Append(aYD.Y());
          theDoubles->Append(aYD.Z());
        }
        break;
      case GEOMAlgo_KN_LINE:
        {
          // ??? geompy.kind.LINE  x1 y1 z1  x2 y2 z2
          // (+) geompy.kind.LINE  x1 y1 z1  dx dy dz
          aKind = SK_LINE;

          gp_Pnt aO = anInfo.Location();
          theDoubles->Append(aO.X());
          theDoubles->Append(aO.Y());
          theDoubles->Append(aO.Z());

          gp_Dir aD = anInfo.Direction();
          theDoubles->Append(aD.X());
          theDoubles->Append(aD.Y());
          theDoubles->Append(aD.Z());
        }
        break;
      case GEOMAlgo_KN_SEGMENT:
        {
          // (+) geompy.kind.SEGMENT  x1 y1 z1  x2 y2 z2
          aKind = SK_SEGMENT;

          gp_Pnt aP1 = anInfo.Pnt1();
          theDoubles->Append(aP1.X());
          theDoubles->Append(aP1.Y());
          theDoubles->Append(aP1.Z());

          gp_Pnt aP2 = anInfo.Pnt2();
          theDoubles->Append(aP2.X());
          theDoubles->Append(aP2.Y());
          theDoubles->Append(aP2.Z());
        }
        break;
      case GEOMAlgo_KN_CURVEBSPLINE:
        {
          // (+) geompy.kind.CRV_BSPLINE  p d np nk nw nm  x1 y1 z1 ... xnp ynp znp  k1 ... knk  w1 ... wnw  m1 ... mnm
          aKind = SK_CRV_BSPLINE;
          Standard_Integer aNbPoles = anInfo.NbPoles();
          Standard_Integer aNbKnots = anInfo.NbKnots();
          Standard_Integer aNbWeights = anInfo.NbWeights();
          Standard_Integer aNbMultiplicities = anInfo.NbMultiplicities();

          theIntegers->Append(anInfo.KindOfPeriod() == GEOMAlgo_KP_PERIODIC ? 1 : 0);
          theIntegers->Append(anInfo.Degree());
          theIntegers->Append(aNbPoles);
          theIntegers->Append(aNbKnots);
          theIntegers->Append(aNbWeights);
          theIntegers->Append(aNbMultiplicities);
          //
          Standard_Integer i;
          if (aNbPoles > 0) {
            Handle(TColgp_HArray1OfPnt) aPoles = anInfo.Poles();
            if (aPoles.IsNull() || aPoles->Length() != aNbPoles) {
              SetErrorCode("B-Spline Curve: no or wrong number of poles given");
              return aKind;
            }
            for (i=1; i<=aNbPoles; i++) {
              const gp_Pnt &aP = aPoles->Value(i);
              theDoubles->Append(aP.X());
              theDoubles->Append(aP.Y());
              theDoubles->Append(aP.Z());
            }
          }
          //
          if (aNbKnots > 0) {
            Handle(TColStd_HArray1OfReal) aKnots = anInfo.Knots();
            if (aKnots.IsNull() || aKnots->Length() != aNbKnots) {
              SetErrorCode("B-Spline Curve: no or wrong number of knots given");
              return aKind;
            }
            for (i=1; i<=aNbKnots; i++)
              theDoubles->Append(aKnots->Value(i));
          }
          //
          if (aNbWeights > 0) {
            Handle(TColStd_HArray1OfReal) aWeights = anInfo.Weights();
            if (aNbWeights > 0 && (aWeights.IsNull() || aWeights->Length() != aNbWeights)) {
              SetErrorCode("B-Spline Curve: no or wrong number of weights given");
              return aKind;
            }
            for (i=1; i<=aNbWeights; i++)
              theDoubles->Append(aWeights->Value(i));
          }
          //
          if (aNbMultiplicities > 0) {
            Handle(TColStd_HArray1OfInteger) aMults = anInfo.Multiplicities();
            if (aMults.IsNull() || aMults->Length() != aNbMultiplicities) {
              SetErrorCode("B-Spline Curve: no or wrong number of multiplicities given");
              return aKind;
            }
            for (i=1; i<=aNbMultiplicities; i++)
              theIntegers->Append(aMults->Value(i));
          }
        }
        break;
      case GEOMAlgo_KN_CURVEBEZIER:
        {
          // (+) geompy.kind.CRV_BEZIER  np nw  x1 y1 z1 ... xnp ynp znp  w1 ... wnw
          aKind = SK_CRV_BEZIER;
          Standard_Integer aNbPoles = anInfo.NbPoles();
          Standard_Integer aNbWeights = anInfo.NbWeights();

          theIntegers->Append(aNbPoles);
          theIntegers->Append(aNbWeights);
          //
          Standard_Integer i;
          if (aNbPoles > 0) {
            Handle(TColgp_HArray1OfPnt) aPoles = anInfo.Poles();
            if (aPoles.IsNull() || aPoles->Length() != aNbPoles) {
              SetErrorCode("Bezier Curve: no or wrong number of poles given");
              return aKind;
            }
            for (i=1; i<=aNbPoles; i++) {
              const gp_Pnt &aP = aPoles->Value(i);
              theDoubles->Append(aP.X());
              theDoubles->Append(aP.Y());
              theDoubles->Append(aP.Z());
            }
          }
          //
          if (aNbWeights > 0) {
            Handle(TColStd_HArray1OfReal) aWeights = anInfo.Weights();
            if (aNbWeights > 0 && (aWeights.IsNull() || aWeights->Length() != aNbWeights)) {
              SetErrorCode("B-Spline Curve: no or wrong number of weights given");
              return aKind;
            }
            for (i=1; i<=aNbWeights; i++)
              theDoubles->Append(aWeights->Value(i));
          }
        }
        break;
      case GEOMAlgo_KN_HYPERBOLA:
        {
          // (+) geompy.kind.HYPERBOLA  xc yc zc  dx dy dz  R_1 R_2  xVx yVx zVx  xVy yVy zVy
          aKind = SK_HYPERBOLA;

          gp_Pnt aC = anInfo.Location();
          theDoubles->Append(aC.X());
          theDoubles->Append(aC.Y());
          theDoubles->Append(aC.Z());

          gp_Ax3 anAx3 = anInfo.Position();
          gp_Dir aD = anAx3.Direction();
          theDoubles->Append(aD.X());
          theDoubles->Append(aD.Y());
          theDoubles->Append(aD.Z());

          theDoubles->Append(anInfo.Radius1());
          theDoubles->Append(anInfo.Radius2());

          gp_Dir aXD = anAx3.XDirection();
          theDoubles->Append(aXD.X());
          theDoubles->Append(aXD.Y());
          theDoubles->Append(aXD.Z());

          gp_Dir aYD = anAx3.YDirection();
          theDoubles->Append(aYD.X());
          theDoubles->Append(aYD.Y());
          theDoubles->Append(aYD.Z());
        }
        break;
      case GEOMAlgo_KN_PARABOLA:
        {
          // (+) geompy.kind.PARABOLA  xc yc zc  dx dy dz  F  xVx yVx zVx  xVy yVy zVy
          aKind = SK_PARABOLA;

          gp_Pnt aC = anInfo.Location();
          theDoubles->Append(aC.X());
          theDoubles->Append(aC.Y());
          theDoubles->Append(aC.Z());

          gp_Ax3 anAx3 = anInfo.Position();
          gp_Dir aD = anAx3.Direction();
          theDoubles->Append(aD.X());
          theDoubles->Append(aD.Y());
          theDoubles->Append(aD.Z());

          theDoubles->Append(anInfo.Radius1());

          gp_Dir aXD = anAx3.XDirection();
          theDoubles->Append(aXD.X());
          theDoubles->Append(aXD.Y());
          theDoubles->Append(aXD.Z());

          gp_Dir aYD = anAx3.YDirection();
          theDoubles->Append(aYD.X());
          theDoubles->Append(aYD.Y());
          theDoubles->Append(aYD.Z());
        }
        break;
      default:
        // ??? geompy.kind.EDGE  nb_vertices _curve_type_id_
        // (+) geompy.kind.EDGE  nb_vertices
        theIntegers->Append(anInfo.NbSubShapes(TopAbs_VERTEX));
      }
    }
    break;

  case TopAbs_VERTEX:
    {
      // (+) geompy.kind.VERTEX  x y z
      aKind = SK_VERTEX;

      gp_Pnt aP = anInfo.Location();
      theDoubles->Append(aP.X());
      theDoubles->Append(aP.Y());
      theDoubles->Append(aP.Z());
    }
    break;
  default:;
  }

  SetErrorCode(OK);
  return aKind;
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
    OCC_CATCH_SIGNALS;

    gp_Ax3 anAx3 = GEOMUtils::GetPosition(aShape);

    gp_Pnt anOri = anAx3.Location();
    gp_Dir aDirZ = anAx3.Direction();
    gp_Dir aDirX = anAx3.XDirection();

    // Output values
    anOri.Coord(Ox, Oy, Oz);
    aDirZ.Coord(Zx, Zy, Zz);
    aDirX.Coord(Xx, Xy, Xz);
  }
  catch (Standard_Failure& aFail) {
    SetErrorCode(aFail.GetMessageString());
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
  Handle(GEOM_Object) aCDG = GetEngine()->AddObject(GEOM_CDG);

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
    OCC_CATCH_SIGNALS;
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Measure driver failed to compute centre of mass");
      return NULL;
    }
  }
  catch (Standard_Failure& aFail) {
    SetErrorCode(aFail.GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction) << aCDG << " = geompy.MakeCDG(" << theShape << ")";

  SetErrorCode(OK);
  return aCDG;
}

//=============================================================================
/*!
 *  GetVertexByIndex
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IMeasureOperations::GetVertexByIndex
                                                (Handle(GEOM_Object) theShape,
                                                 Standard_Integer theIndex,
                                                 Standard_Boolean theUseOri)
{
  SetErrorCode(KO);

  if (theShape.IsNull()) return NULL;

  Handle(GEOM_Function) aRefShape = theShape->GetLastFunction();
  if (aRefShape.IsNull()) return NULL;

  //Add a new Vertex object
  Handle(GEOM_Object) aVertex = GetEngine()->AddObject(GEOM_POINT);

  //Add a function
  Handle(GEOM_Function) aFunction =
    aVertex->AddFunction(GEOMImpl_MeasureDriver::GetID(), VERTEX_BY_INDEX);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_MeasureDriver::GetID()) return NULL;

  GEOMImpl_IMeasure aCI (aFunction);
  aCI.SetBase(aRefShape);
  aCI.SetIndex(theIndex);
  aCI.SetUseOri(theUseOri);

  //Compute
  try {
    OCC_CATCH_SIGNALS;
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Vertex by index driver failed.");
      return NULL;
    }
  }
  catch (Standard_Failure& aFail) {
    SetErrorCode(aFail.GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction) << aVertex << " = geompy.GetVertexByIndex("
                               << theShape << ", "
                               << theIndex << ", "
                               << theUseOri << ")";

  SetErrorCode(OK);
  return aVertex;
}

//=============================================================================
/*!
 *  GetNormal
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IMeasureOperations::GetNormal
                                         (Handle(GEOM_Object) theFace,
                                          Handle(GEOM_Object) theOptionalPoint)
{
  SetErrorCode(KO);

  if (theFace.IsNull()) return NULL;

  //Add a new Normale object
  Handle(GEOM_Object) aNorm = GetEngine()->AddObject(GEOM_VECTOR);

  //Add a new Normale function
  Handle(GEOM_Function) aFunction =
    aNorm->AddFunction(GEOMImpl_MeasureDriver::GetID(), VECTOR_FACE_NORMALE);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_MeasureDriver::GetID()) return NULL;

  GEOMImpl_IMeasure aCI (aFunction);

  Handle(GEOM_Function) aFace = theFace->GetLastFunction();
  if (aFace.IsNull()) return NULL;

  aCI.SetBase(aFace);

  if (!theOptionalPoint.IsNull()) {
    Handle(GEOM_Function) anOptPnt = theOptionalPoint->GetLastFunction();
    aCI.SetPoint(anOptPnt);
  }

  //Compute the Normale value
  try {
    OCC_CATCH_SIGNALS;
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Measure driver failed to compute normake of face");
      return NULL;
    }
  }
  catch (Standard_Failure& aFail) {
    SetErrorCode(aFail.GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump pd (aFunction);
  pd << aNorm << " = geompy.GetNormal(" << theFace;
  if (!theOptionalPoint.IsNull()) {
    pd << ", " << theOptionalPoint;
  }
  pd << ")";

  SetErrorCode(OK);
  return aNorm;
}

//=============================================================================
/*!
 *  GetBasicProperties
 */
//=============================================================================
void GEOMImpl_IMeasureOperations::GetBasicProperties (Handle(GEOM_Object) theShape,
                                                      const Standard_Real theTolerance,
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
  Standard_Real anEps = theTolerance >= 0 ? theTolerance : 1.e-6;
  try {
    OCC_CATCH_SIGNALS;
    BRepGProp::LinearProperties(aShape, LProps, Standard_True);
    theLength = LProps.Mass();

    BRepGProp::SurfaceProperties(aShape, SProps, anEps, Standard_True);
    theSurfArea = SProps.Mass();

    theVolume = 0.0;
    if (aShape.ShapeType() < TopAbs_SHELL) {
      for (TopExp_Explorer Exp (aShape, TopAbs_SOLID); Exp.More(); Exp.Next()) {
        GProp_GProps VProps;
        BRepGProp::VolumeProperties(Exp.Current(), VProps, anEps, Standard_True);
        theVolume += VProps.Mass();
      }
    }
  }
  catch (Standard_Failure& aFail) {
    SetErrorCode(aFail.GetMessageString());
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
    OCC_CATCH_SIGNALS;
    if (aShape.ShapeType() == TopAbs_VERTEX ||
        aShape.ShapeType() == TopAbs_EDGE ||
        aShape.ShapeType() == TopAbs_WIRE) {
      BRepGProp::LinearProperties(aShape, System, Standard_True);
    } else if (aShape.ShapeType() == TopAbs_FACE ||
               aShape.ShapeType() == TopAbs_SHELL) {
      BRepGProp::SurfaceProperties(aShape, System, Standard_True);
    } else {
      BRepGProp::VolumeProperties(aShape, System, Standard_True);
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
  catch (Standard_Failure& aFail) {
    SetErrorCode(aFail.GetMessageString());
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
                                      const Standard_Boolean precise,
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
    OCC_CATCH_SIGNALS;
    BRepBuilderAPI_Copy aCopyTool (aShape);
    if (!aCopyTool.IsDone()) {
      SetErrorCode("GetBoundingBox Error: Bad shape detected");
      return;
    }

    aShape = aCopyTool.Shape();

    // remove triangulation to obtain more exact boundaries
    BRepTools::Clean(aShape);

    BRepBndLib::Add(aShape, B);

    if (precise) {
      if (!GEOMUtils::PreciseBoundingBox(aShape, B)) {
        SetErrorCode("GetBoundingBox Error: Bounding box cannot be precised");
        return;
      }
    }

    B.Get(Xmin, Ymin, Zmin, Xmax, Ymax, Zmax);
  }
  catch (Standard_Failure& aFail) {
    SetErrorCode(aFail.GetMessageString());
    return;
  }

  SetErrorCode(OK);
}

//=============================================================================
/*!
 *  GetBoundingBox
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IMeasureOperations::GetBoundingBox
                                                (Handle(GEOM_Object) theShape,
                                                 const Standard_Boolean precise)
{
  SetErrorCode(KO);

  if (theShape.IsNull()) return NULL;

  //Add a new BoundingBox object
  Handle(GEOM_Object) aBnd = GetEngine()->AddObject(GEOM_BOX);

  //Add a new BoundingBox function
  const int aType = (precise ? BND_BOX_MEASURE_PRECISE : BND_BOX_MEASURE);
  Handle(GEOM_Function) aFunction =
    aBnd->AddFunction(GEOMImpl_MeasureDriver::GetID(), aType);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_MeasureDriver::GetID()) return NULL;

  GEOMImpl_IMeasure aCI (aFunction);

  Handle(GEOM_Function) aRefShape = theShape->GetLastFunction();
  if (aRefShape.IsNull()) return NULL;

  aCI.SetBase(aRefShape);

  //Compute the BoundingBox value
  try {
    OCC_CATCH_SIGNALS;
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Measure driver failed to compute a bounding box");
      return NULL;
    }
  }
  catch (Standard_Failure& aFail) {
    SetErrorCode(aFail.GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump aPd(aFunction);
  
  aPd << aBnd << " = geompy.MakeBoundingBox(" << theShape;

  if (precise) {
    aPd << ", True";
  }

  aPd << ")";

  SetErrorCode(OK);
  return aBnd;
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
    OCC_CATCH_SIGNALS;
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
  catch (Standard_Failure& aFail) {
    SetErrorCode(aFail.GetMessageString());
    return;
  }

  SetErrorCode(OK);
}

//=============================================================================
/*!
 *  CheckShape
 */
//=============================================================================
bool GEOMImpl_IMeasureOperations::CheckShape (Handle(GEOM_Object)     theShape,
                                              const Standard_Boolean  theIsCheckGeom,
                                              std::list<ShapeError>  &theErrors)
{
  SetErrorCode(KO);
  theErrors.clear();

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
    OCC_CATCH_SIGNALS;
    BRepCheck_Analyzer ana (aShape, theIsCheckGeom);
    if (ana.IsValid()) {
      isValid = true;
    } else {
      FillErrors(ana, aShape, theErrors);
    }
  }
  catch (Standard_Failure& aFail) {
    SetErrorCode(aFail.GetMessageString());
    return false;
  }

  SetErrorCode(OK);
  return isValid;
}

//=============================================================================
/*!
 *  PrintShapeErrors
 */
//=============================================================================
TCollection_AsciiString GEOMImpl_IMeasureOperations::PrintShapeErrors
                                 (Handle(GEOM_Object)          theShape,
                                  const std::list<ShapeError> &theErrors)
{
  TCollection_AsciiString aDump;

  if (theShape.IsNull()) {
    return aDump;
  }

  Handle(GEOM_Function) aRefShape = theShape->GetLastFunction();

  if (aRefShape.IsNull()) {
    return aDump;
  }

  TopoDS_Shape aShape = aRefShape->GetValue();

  if (aShape.IsNull()) {
    SetErrorCode("The Objects has NULL Shape");
    return aDump;
  }

  if (!theErrors.empty()) {
    // The shape is not valid. Prepare errors for dump.
    TopTools_IndexedMapOfShape anIndices;
    std::list<ShapeError>::const_iterator anIter = theErrors.begin();
    Standard_Integer nbv, nbe, nbw, nbf, nbs, nbo;
    nbv = nbe = nbw = nbf = nbs = nbo = 0;

    // Map sub-shapes and their indices
    TopExp::MapShapes(aShape, anIndices);

    const Standard_Integer aNbSubShapes = anIndices.Extent();
    TColStd_MapOfInteger   aMapPbInd;

    aDump += " -- The Shape has problems :\n";
    aDump += "  Check                                    Count\n";
    aDump += " ------------------------------------------------\n";

    for (; anIter != theErrors.end(); anIter++) {
      Standard_Integer aNbShapes = anIter->incriminated.size();

      switch(anIter->error) {
      case BRepCheck_InvalidPointOnCurve:
        aDump += "  Invalid Point on Curve ................... ";
        break;
      case BRepCheck_InvalidPointOnCurveOnSurface:
        aDump += "  Invalid Point on CurveOnSurface .......... ";
        break;
      case BRepCheck_InvalidPointOnSurface:
        aDump += "  Invalid Point on Surface ................. ";
        break;
      case BRepCheck_No3DCurve:
        aDump += "  No 3D Curve .............................. ";
        break;
      case BRepCheck_Multiple3DCurve:
        aDump += "  Multiple 3D Curve ........................ ";
        break;
      case BRepCheck_Invalid3DCurve:
        aDump += "  Invalid 3D Curve ......................... ";
        break;
      case BRepCheck_NoCurveOnSurface:
        aDump += "  No Curve on Surface ...................... ";
        break;
      case BRepCheck_InvalidCurveOnSurface:
        aDump += "  Invalid Curve on Surface ................. ";
        break;
      case BRepCheck_InvalidCurveOnClosedSurface:
        aDump += "  Invalid Curve on closed Surface .......... ";
        break;
      case BRepCheck_InvalidSameRangeFlag:
        aDump += "  Invalid SameRange Flag ................... ";
        break;
      case BRepCheck_InvalidSameParameterFlag:
        aDump += "  Invalid SameParameter Flag ............... ";
        break;
      case BRepCheck_InvalidDegeneratedFlag:
        aDump += "  Invalid Degenerated Flag ................. ";
        break;
      case BRepCheck_FreeEdge:
        aDump += "  Free Edge ................................ ";
        break;
      case BRepCheck_InvalidMultiConnexity:
        aDump += "  Invalid MultiConnexity ................... ";
        break;
      case BRepCheck_InvalidRange:
        aDump += "  Invalid Range ............................ ";
        break;
      case BRepCheck_EmptyWire:
        aDump += "  Empty Wire ............................... ";
        break;
      case BRepCheck_RedundantEdge:
        aDump += "  Redundant Edge ........................... ";
        break;
      case BRepCheck_SelfIntersectingWire:
        aDump += "  Self Intersecting Wire ................... ";
        break;
      case BRepCheck_NoSurface:
        aDump += "  No Surface ............................... ";
        break;
      case BRepCheck_InvalidWire:
        aDump += "  Invalid Wire ............................. ";
        break;
      case BRepCheck_RedundantWire:
        aDump += "  Redundant Wire ........................... ";
        break;
      case BRepCheck_IntersectingWires:
        aDump += "  Intersecting Wires ....................... ";
        break;
      case BRepCheck_InvalidImbricationOfWires:
        aDump += "  Invalid Imbrication of Wires ............. ";
        break;
      case BRepCheck_EmptyShell:
        aDump += "  Empty Shell .............................. ";
        break;
      case BRepCheck_RedundantFace:
        aDump += "  Redundant Face ........................... ";
        break;
      case BRepCheck_UnorientableShape:
        aDump += "  Unorientable Shape ....................... ";
        break;
      case BRepCheck_NotClosed:
        aDump += "  Not Closed ............................... ";
        break;
      case BRepCheck_NotConnected:
        aDump += "  Not Connected ............................ ";
        break;
      case BRepCheck_SubshapeNotInShape:
        aDump += "  Sub-shape not in Shape ................... ";
        break;
      case BRepCheck_BadOrientation:
        aDump += "  Bad Orientation .......................... ";
        break;
      case BRepCheck_BadOrientationOfSubshape:
        aDump += "  Bad Orientation of Sub-shape ............. ";
        break;
      case BRepCheck_InvalidToleranceValue:
        aDump += "  Invalid Tolerance Value .................. ";
        break;
      case BRepCheck_CheckFail:
        aDump += "  Check Shape Failure ...................... ";
        break;
      default:
        break;
      }

      aDump += TCollection_AsciiString(aNbShapes) + "\n";

      // Count types of shape.
      std::list<int>::const_iterator aShIter = anIter->incriminated.begin();

      for (; aShIter != anIter->incriminated.end(); aShIter++) {
        const Standard_Integer anIndex = *aShIter;

        if (anIndex > 0 && anIndex <= aNbSubShapes && aMapPbInd.Add(anIndex)) {
          const TopoDS_Shape     &aSubShape = anIndices.FindKey(anIndex);
          const TopAbs_ShapeEnum  aType     = aSubShape.ShapeType();

          switch (aType) {
            case TopAbs_VERTEX : nbv++; break;
            case TopAbs_EDGE   : nbe++; break;
            case TopAbs_WIRE   : nbw++; break;
            case TopAbs_FACE   : nbf++; break;
            case TopAbs_SHELL  : nbs++; break;
            case TopAbs_SOLID  : nbo++; break;
            default            : break;
          }
        }
      }
    }

    const Standard_Integer aNbFaultyShapes = nbv + nbe + nbw + nbf + nbs + nbo;
    aDump += " ------------------------------------------------\n";
    aDump += "*** Shapes with problems : ";
    aDump += TCollection_AsciiString(aNbFaultyShapes) + "\n";

    if (nbv > 0) {
      aDump += "VERTEX : ";
      if (nbv < 10) aDump += " ";
      aDump += TCollection_AsciiString(nbv) + "\n";
    }
    if (nbe > 0) {
      aDump += "EDGE   : ";
      if (nbe < 10) aDump += " ";
      aDump += TCollection_AsciiString(nbe) + "\n";
    }
    if (nbw > 0) {
      aDump += "WIRE   : ";
      if (nbw < 10) aDump += " ";
      aDump += TCollection_AsciiString(nbw) + "\n";
    }
    if (nbf > 0) {
      aDump += "FACE   : ";
      if (nbf < 10) aDump += " ";
      aDump += TCollection_AsciiString(nbf) + "\n";
    }
    if (nbs > 0) {
      aDump += "SHELL  : ";
      if (nbs < 10) aDump += " ";
      aDump += TCollection_AsciiString(nbs) + "\n";
    }
    if (nbo > 0) {
      aDump += "SOLID  : ";
      if (nbo < 10) aDump += " ";
      aDump += TCollection_AsciiString(nbo) + "\n";
    }
  }

  return aDump;
}

//=============================================================================
/*!
 *  CheckSelfIntersections
 */
//=============================================================================
bool GEOMImpl_IMeasureOperations::CheckSelfIntersections
                         (Handle(GEOM_Object)                 theShape,
                          const SICheckLevel                  theCheckLevel,
                          Handle(TColStd_HSequenceOfInteger)& theIntersections)
{
  SetErrorCode(KO);

  if (theIntersections.IsNull())
    theIntersections = new TColStd_HSequenceOfInteger;
  else
    theIntersections->Clear();

  if (theShape.IsNull())
    return false;

  Handle(GEOM_Function) aRefShape = theShape->GetLastFunction();
  if (aRefShape.IsNull()) return false;

  TopoDS_Shape aShape = aRefShape->GetValue();
  if (aShape.IsNull()) return false;

  // 0. Prepare data
  TopoDS_Shape aScopy;
  //
  GEOMAlgo_AlgoTools::CopyShape(aShape, aScopy);
  //
  // Map sub-shapes and their indices
  TopTools_IndexedMapOfShape anIndices;
  TopExp::MapShapes(aScopy, anIndices);

  TopTools_ListOfShape aLCS;
  aLCS.Append(aScopy);
  //
  BOPAlgo_CheckerSI aCSI; // checker of self-interferences
  aCSI.SetArguments(aLCS);
  aCSI.SetLevelOfCheck(theCheckLevel);

  // 1. Launch the checker
  aCSI.Perform();
  Standard_Boolean iErr = aCSI.HasErrors();

  //
  Standard_Integer aNbS, n1, n2;
  BOPDS_MapIteratorOfMapOfPair aItMPK;
  //
  // 2. Take the shapes from DS
  const BOPDS_DS& aDS = aCSI.DS();
  aNbS=aDS.NbShapes();
  //
  // 3. Get the pairs of interfered shapes
  const BOPDS_MapOfPair& aMPK=aDS.Interferences();
  aItMPK.Initialize(aMPK);
  for (; aItMPK.More(); aItMPK.Next()) {
    const BOPDS_Pair& aPK=aItMPK.Value();
    aPK.Indices(n1, n2);
    //
    if (n1 > aNbS || n2 > aNbS){
      return false; // Error
    }
    const TopoDS_Shape& aS1 = aDS.Shape(n1);
    const TopoDS_Shape& aS2 = aDS.Shape(n2);

    theIntersections->Append(anIndices.FindIndex(aS1));
    theIntersections->Append(anIndices.FindIndex(aS2));
  }

  if (!iErr) {
    SetErrorCode(OK);
  }

  return theIntersections->IsEmpty();
}

//=============================================================================
/*!
 *  CheckSelfIntersectionsFast
 */
//=============================================================================
bool GEOMImpl_IMeasureOperations::CheckSelfIntersectionsFast
                         (Handle(GEOM_Object) theShape,
                          float theDeflection, double theTolerance,
                          Handle(TColStd_HSequenceOfInteger)& theIntersections)
{
  SetErrorCode(KO);

  if (theIntersections.IsNull())
    theIntersections = new TColStd_HSequenceOfInteger;
  else
    theIntersections->Clear();

  if (theShape.IsNull())
    return false;

  Handle(GEOM_Function) aRefShape = theShape->GetLastFunction();
  if (aRefShape.IsNull()) return false;

  TopoDS_Shape aShape = aRefShape->GetValue();
  if (aShape.IsNull()) return false;

  // Prepare data
  TopoDS_Shape aScopy;

  GEOMAlgo_AlgoTools::CopyShape(aShape, aScopy);
  GEOMUtils::MeshShape(aScopy, theDeflection);

  // Map sub-shapes and their indices
  TopTools_IndexedMapOfShape anIndices;
  TopExp::MapShapes(aScopy, anIndices);

  // Checker of fast interferences
  BRepExtrema_SelfIntersection aTool(aScopy, (theTolerance <= 0.) ? 0.0 : theTolerance);

  // Launch the checker
  aTool.Perform();
  
  const BRepExtrema_MapOfIntegerPackedMapOfInteger& intersections = aTool.OverlapElements();
  
  std::set<Standard_Integer> processed;
  
  for (BRepExtrema_MapOfIntegerPackedMapOfInteger::Iterator it(intersections); it.More(); it.Next()) {
    Standard_Integer idxLeft = it.Key();
    if (processed.count(idxLeft) > 0) continue; // already added
    processed.insert(idxLeft);
    const TColStd_PackedMapOfInteger& overlaps = it.Value();
    for (TColStd_MapIteratorOfPackedMapOfInteger subit(overlaps); subit.More(); subit.Next()) {
      Standard_Integer idxRight = subit.Key();
      if (processed.count(idxRight) > 0) continue; // already added
      const TopoDS_Shape& aS1 = aTool.GetSubShape(idxLeft);
      const TopoDS_Shape& aS2 = aTool.GetSubShape(idxRight);
      theIntersections->Append(anIndices.FindIndex(aS1));
      theIntersections->Append(anIndices.FindIndex(aS2));
    }
  }

  if (aTool.IsDone())
    SetErrorCode(OK);

  return theIntersections->IsEmpty();
}

//=============================================================================
/*!
 *  CheckBOPArguments
 */
//=============================================================================
bool GEOMImpl_IMeasureOperations::CheckBOPArguments
                                      (const Handle(GEOM_Object) &theShape)
{
  SetErrorCode(KO);

  if (theShape.IsNull()) {
    return false;
  }

  Handle(GEOM_Function) aRefShape = theShape->GetLastFunction();

  if (aRefShape.IsNull()) {
    return false;
  }

  TopoDS_Shape aShape = aRefShape->GetValue();

  if (aShape.IsNull()) {
    return false;
  }

  //Compute the parameters
  bool isValid = GEOMUtils::CheckBOPArguments(aShape);

  SetErrorCode(OK);

  return isValid;
}

//=============================================================================
/*!
 *  FastIntersect
 */
//=============================================================================
bool GEOMImpl_IMeasureOperations::FastIntersect (Handle(GEOM_Object) theShape1, Handle(GEOM_Object) theShape2,
                                                 double theTolerance, float theDeflection,
                                                 Handle(TColStd_HSequenceOfInteger)& theIntersections1,
                                                 Handle(TColStd_HSequenceOfInteger)& theIntersections2)
{
  SetErrorCode(KO);
  bool isGood = false;

  if (theIntersections1.IsNull())
    theIntersections1 = new TColStd_HSequenceOfInteger;
  else
    theIntersections1->Clear();

  if (theIntersections2.IsNull())
    theIntersections2 = new TColStd_HSequenceOfInteger;
  else
    theIntersections2->Clear();

  if (theShape1.IsNull() || theShape2.IsNull()) {
    SetErrorCode("Objects have NULL Shape");
    return isGood;
  }

  if (theShape1 == theShape2) {
    SetErrorCode("Objects are equal");
    return isGood;
  }
  Handle(GEOM_Function) aRefShape1 = theShape1->GetLastFunction();
  Handle(GEOM_Function) aRefShape2 = theShape2->GetLastFunction();
  if (aRefShape1.IsNull() || aRefShape2.IsNull()) return isGood;

  TopoDS_Shape aShape1 = aRefShape1->GetValue();
  TopoDS_Shape aShape2 = aRefShape2->GetValue();
  if (aShape1.IsNull() || aShape2.IsNull()) return isGood;

  // 0. Prepare data
  TopoDS_Shape aScopy1, aScopy2;
  GEOMAlgo_AlgoTools::CopyShape(aShape1, aScopy1);
  GEOMAlgo_AlgoTools::CopyShape(aShape2, aScopy2);

  GEOMUtils::MeshShape(aScopy1, theDeflection);
  GEOMUtils::MeshShape(aScopy2, theDeflection);
  //
  // Map sub-shapes and their indices
  TopTools_IndexedMapOfShape anIndices1, anIndices2;
  TopExp::MapShapes(aScopy1, anIndices1);
  TopExp::MapShapes(aScopy2, anIndices2);

  TopTools_ListOfShape aLCS1, aLCS2;
  aLCS1.Append(aScopy1); aLCS2.Append(aScopy2);
  //
  BRepExtrema_ShapeProximity aBSP; // checker of fast interferences
  aBSP.LoadShape1(aScopy1); aBSP.LoadShape2(aScopy2);
  aBSP.SetTolerance((theTolerance <= 0.) ? 0.0 : theTolerance);

  // 1. Launch the checker
  aBSP.Perform();
 
  // 2. Get sets of IDs of overlapped faces
  for (BRepExtrema_MapOfIntegerPackedMapOfInteger::Iterator anIt1 (aBSP.OverlapSubShapes1()); anIt1.More(); anIt1.Next())
  {
    const TopoDS_Shape& aS1 = aBSP.GetSubShape1(anIt1.Key());
    theIntersections1->Append(anIndices1.FindIndex(aS1));
  }
  
  for (BRepExtrema_MapOfIntegerPackedMapOfInteger::Iterator anIt2 (aBSP.OverlapSubShapes2()); anIt2.More(); anIt2.Next())
  {
    const TopoDS_Shape& aS2 = aBSP.GetSubShape2(anIt2.Key());
    theIntersections2->Append(anIndices2.FindIndex(aS2));
  }

  isGood = !theIntersections1->IsEmpty() && !theIntersections1->IsEmpty();

  if (aBSP.IsDone())
    SetErrorCode(OK);

  return isGood;
}

//=============================================================================
/*!
 *  IsGoodForSolid
 */
//=============================================================================
TCollection_AsciiString GEOMImpl_IMeasureOperations::IsGoodForSolid (Handle(GEOM_Object) theShape)
{
  SetErrorCode(KO);

  TCollection_AsciiString aRes = "";

  if (theShape.IsNull()) {
    aRes = "WRN_NULL_OBJECT_OR_SHAPE";
  }
  else {
    Handle(GEOM_Function) aRefShape = theShape->GetLastFunction();
    if (aRefShape.IsNull()) {
      aRes = "WRN_NULL_OBJECT_OR_SHAPE";
    }
    else {
      TopoDS_Shape aShape = aRefShape->GetValue();
      if (aShape.IsNull()) {
        aRes = "WRN_NULL_OBJECT_OR_SHAPE";
      }
      else {
        if (aShape.ShapeType() == TopAbs_COMPOUND) {
          TopoDS_Iterator It (aShape, Standard_True, Standard_True);
          if (It.More()) aShape = It.Value();
        }
        if (aShape.ShapeType() == TopAbs_SHELL) {
          BRepCheck_Shell chkShell (TopoDS::Shell(aShape));
          if (chkShell.Closed() == BRepCheck_NotClosed) {
            aRes = "WRN_SHAPE_UNCLOSED";
          }
        }
        else {
          aRes = "WRN_SHAPE_NOT_SHELL";
        }
      }
    }
  }

  if (aRes.IsEmpty())
    SetErrorCode(OK);

  return aRes;
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
    OCC_CATCH_SIGNALS;
    int iType, nbTypes [TopAbs_SHAPE], nbFlatType [TopAbs_SHAPE];
    for (iType = 0; iType < TopAbs_SHAPE; ++iType) {
      nbTypes[iType] = 0;
      nbFlatType[iType] = 0;
    }
    nbTypes[aShape.ShapeType()]++;

    TopTools_MapOfShape aMapOfShape;
    aMapOfShape.Add(aShape);
    TopTools_ListOfShape aListOfShape;
    aListOfShape.Append(aShape);

    TopTools_ListIteratorOfListOfShape itL (aListOfShape);
    for (; itL.More(); itL.Next()) {
      TopoDS_Shape sp = itL.Value();
      TopoDS_Iterator it (sp);
      for (; it.More(); it.Next()) {
        TopoDS_Shape s = it.Value();
        if (aMapOfShape.Add(s)) {
          aListOfShape.Append(s);
          nbTypes[s.ShapeType()]++;
          if ((sp.ShapeType() == TopAbs_COMPOUND) || (sp.ShapeType() == TopAbs_COMPSOLID)) {
            nbFlatType[s.ShapeType()]++;
          }
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
    Astr = Astr + " SHAPE : " + TCollection_AsciiString(aMapOfShape.Extent()) + "\n";

    if ((aShape.ShapeType() == TopAbs_COMPOUND) || (aShape.ShapeType() == TopAbs_COMPSOLID)){
      Astr = Astr + " --------------------- \n Flat content : \n";
      if (nbFlatType[TopAbs_VERTEX] > 0)
        Astr = Astr + " VERTEX : " + TCollection_AsciiString(nbFlatType[TopAbs_VERTEX]) + "\n";
      if (nbFlatType[TopAbs_EDGE] > 0)
        Astr = Astr + " EDGE : " + TCollection_AsciiString(nbFlatType[TopAbs_EDGE]) + "\n";
      if (nbFlatType[TopAbs_WIRE] > 0)
        Astr = Astr + " WIRE : " + TCollection_AsciiString(nbFlatType[TopAbs_WIRE]) + "\n";
      if (nbFlatType[TopAbs_FACE] > 0)
        Astr = Astr + " FACE : " + TCollection_AsciiString(nbFlatType[TopAbs_FACE]) + "\n";
      if (nbFlatType[TopAbs_SHELL] > 0)
        Astr = Astr + " SHELL : " + TCollection_AsciiString(nbFlatType[TopAbs_SHELL]) + "\n";
      if (nbFlatType[TopAbs_SOLID] > 0)
        Astr = Astr + " SOLID : " + TCollection_AsciiString(nbFlatType[TopAbs_SOLID]) + "\n";
    }
  }
  catch (Standard_Failure& aFail) {
    SetErrorCode(aFail.GetMessageString());
    return Astr;
  }

  SetErrorCode(OK);
  return Astr;
}

//=============================================================================
/*!
 *  AreCoordsInside
 */
//=============================================================================
std::vector<bool>
GEOMImpl_IMeasureOperations::AreCoordsInside(Handle(GEOM_Object)        theShape,
                                             const std::vector<double>& coords,
                                             double                     tolerance)
{
  std::vector<bool> isInsideRes;
  if (!theShape.IsNull()) {
    Handle(GEOM_Function) aRefShape = theShape->GetLastFunction();
    if (!aRefShape.IsNull()) {
      TopoDS_Shape aShape = aRefShape->GetValue();
      if (!aShape.IsNull())
      {
        TopTools_IndexedMapOfShape mapShape;
        {
          TopExp_Explorer anExp;
          for ( anExp.Init( aShape, TopAbs_SOLID ); anExp.More(); anExp.Next() )
            mapShape.Add( anExp.Current() );
          for ( anExp.Init( aShape, TopAbs_FACE, TopAbs_SOLID ); anExp.More(); anExp.Next() )
            mapShape.Add( anExp.Current() );
          for ( anExp.Init( aShape, TopAbs_EDGE, TopAbs_FACE ); anExp.More(); anExp.Next() )
            mapShape.Add( anExp.Current() );
          for ( anExp.Init( aShape, TopAbs_VERTEX, TopAbs_EDGE ); anExp.More(); anExp.Next() )
            mapShape.Add( anExp.Current() ); //// ?????????
        }
        size_t nb_points = coords.size()/3, nb_points_inside = 0;
        isInsideRes.resize( nb_points, false );

        for ( int iS = 1; iS <= mapShape.Extent(); ++iS )
        {
          if ( nb_points_inside == nb_points )
            break;
          aShape = mapShape( iS );
          switch ( aShape.ShapeType() ) {
          case TopAbs_SOLID:
          {
            BRepClass3d_SolidClassifier SC( TopoDS::Solid( aShape ));
            for ( size_t i = 0; i < nb_points; i++)
            {
              if ( isInsideRes[ i ]) continue;
              gp_Pnt aPnt( coords[3*i], coords[3*i+1], coords[3*i+2] );
              SC.Perform( aPnt, tolerance );
              isInsideRes[ i ] = (( SC.State() == TopAbs_IN ) || ( SC.State() == TopAbs_ON ));
              nb_points_inside += isInsideRes[ i ];
            }
            break;
          }
          case TopAbs_FACE:
          {
            Standard_Real u1,u2,v1,v2;
            const TopoDS_Face&   face = TopoDS::Face( aShape );
            Handle(Geom_Surface) surf = BRep_Tool::Surface( face );
            surf->Bounds( u1,u2,v1,v2 );
            GeomAPI_ProjectPointOnSurf project;
            project.Init(surf, u1,u2, v1,v2, tolerance );
            for ( size_t i = 0; i < nb_points; i++)
            {
              if ( isInsideRes[ i ]) continue;
              gp_Pnt aPnt( coords[3*i], coords[3*i+1], coords[3*i+2] );
              project.Perform( aPnt );
              if ( project.IsDone() &&
                   project.NbPoints() > 0 &&
                   project.LowerDistance() <= tolerance )
              {
                Standard_Real u, v;
                project.LowerDistanceParameters(u, v);
                gp_Pnt2d uv( u, v );
                BRepClass_FaceClassifier FC ( face, uv, tolerance );
                isInsideRes[ i ] = (( FC.State() == TopAbs_IN ) || ( FC.State() == TopAbs_ON ));
                nb_points_inside += isInsideRes[ i ];
              }
            }
            break;
          }
          case TopAbs_EDGE:
          {
            Standard_Real f,l;
            const TopoDS_Edge&  edge = TopoDS::Edge( aShape );
            Handle(Geom_Curve) curve = BRep_Tool::Curve( edge, f, l );
            GeomAPI_ProjectPointOnCurve project;
            project.Init( curve, f, l );
            for ( size_t i = 0; i < nb_points; i++)
            {
              if ( isInsideRes[ i ]) continue;
              gp_Pnt aPnt( coords[3*i], coords[3*i+1], coords[3*i+2] );
              project.Perform( aPnt );
              isInsideRes[ i ] = ( project.NbPoints() > 0 &&
                                   project.LowerDistance() <= tolerance );
              nb_points_inside += isInsideRes[ i ];
            }
            break;
          }
          case TopAbs_VERTEX:
          {
            gp_Pnt aVPnt = BRep_Tool::Pnt( TopoDS::Vertex( aShape ));
            for ( size_t i = 0; i < nb_points; i++)
            {
              if ( isInsideRes[ i ]) continue;
              gp_Pnt aPnt( coords[3*i], coords[3*i+1], coords[3*i+2] );
              isInsideRes[ i ] = ( aPnt.SquareDistance( aVPnt ) <= tolerance * tolerance );
              nb_points_inside += isInsideRes[ i ];
            }
            break;
          }
          default:;
          } // switch ( aShape.ShapeType() )
        }
      }
    }
  }
  return isInsideRes;
}

//=============================================================================
/*!
 *  GetMinDistance
 */
//=============================================================================
Standard_Real
GEOMImpl_IMeasureOperations::GetMinDistance (Handle(GEOM_Object) theShape1,
                                             Handle(GEOM_Object) theShape2,
                                             Standard_Real& X1,
                                             Standard_Real& Y1,
                                             Standard_Real& Z1,
                                             Standard_Real& X2,
                                             Standard_Real& Y2,
                                             Standard_Real& Z2)
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
    OCC_CATCH_SIGNALS;

    gp_Pnt aPnt1, aPnt2;

    MinDist = GEOMUtils::GetMinDistance(aShape1, aShape2, aPnt1, aPnt2);

    if (MinDist >= 0.0) {
      aPnt1.Coord(X1, Y1, Z1);
      aPnt2.Coord(X2, Y2, Z2);
    } else {
      return MinDist;
    }
  }
  catch (Standard_Failure& aFail) {
    SetErrorCode(aFail.GetMessageString());
    return MinDist;
  }

  SetErrorCode(OK);
  return MinDist;
}

//=======================================================================
/*!
 *  Get coordinates of closest points of two shapes
 */
//=======================================================================
Standard_Integer GEOMImpl_IMeasureOperations::ClosestPoints (Handle(GEOM_Object) theShape1,
                                                             Handle(GEOM_Object) theShape2,
                                                             Handle(TColStd_HSequenceOfReal)& theDoubles)
{
  SetErrorCode(KO);
  Standard_Integer nbSolutions = 0;

  if (theShape1.IsNull() || theShape2.IsNull()) return nbSolutions;

  Handle(GEOM_Function) aRefShape1 = theShape1->GetLastFunction();
  Handle(GEOM_Function) aRefShape2 = theShape2->GetLastFunction();
  if (aRefShape1.IsNull() || aRefShape2.IsNull()) return nbSolutions;

  TopoDS_Shape aShape1 = aRefShape1->GetValue();
  TopoDS_Shape aShape2 = aRefShape2->GetValue();
  if (aShape1.IsNull() || aShape2.IsNull()) {
    SetErrorCode("One of Objects has NULL Shape");
    return nbSolutions;
  }

  // Compute the extremities
  try {
    OCC_CATCH_SIGNALS;

    // skl 30.06.2008
    // additional workaround for bugs 19899, 19908 and 19910 from Mantis
    gp_Pnt P1s, P2s;
    double dist = GEOMUtils::GetMinDistanceSingular(aShape1, aShape2, P1s, P2s);
    bool singularBetter = dist >= 0;

    BRepExtrema_DistShapeShape dst (aShape1, aShape2);
    if (dst.IsDone()) {
      nbSolutions = dst.NbSolution();
      if (theDoubles.IsNull()) theDoubles = new TColStd_HSequenceOfReal;

      gp_Pnt P1, P2;
      for (int i = 1; i <= nbSolutions; i++) {
        P1 = dst.PointOnShape1(i);
        P2 = dst.PointOnShape2(i);
        
        theDoubles->Append(P1.X());
        theDoubles->Append(P1.Y());
        theDoubles->Append(P1.Z());
        theDoubles->Append(P2.X());
        theDoubles->Append(P2.Y());
        theDoubles->Append(P2.Z());
        
        Standard_Real Dist = P1.Distance(P2);
        singularBetter = singularBetter && dist < Dist;
      }
    }

    if (singularBetter) {
      if (theDoubles.IsNull()) theDoubles = new TColStd_HSequenceOfReal;
      else theDoubles->Clear();

      nbSolutions = 1;
    
      theDoubles->Append(P1s.X());
      theDoubles->Append(P1s.Y());
      theDoubles->Append(P1s.Z());
      theDoubles->Append(P2s.X());
      theDoubles->Append(P2s.Y());
      theDoubles->Append(P2s.Z());
    }
  }
  catch (Standard_Failure& aFail) {
    SetErrorCode(aFail.GetMessageString());
    return nbSolutions;
  }

  SetErrorCode(OK);
  return nbSolutions;
}

//=======================================================================
/*!
 *  Get coordinates of point
 */
//=======================================================================
void GEOMImpl_IMeasureOperations::PointCoordinates (Handle(GEOM_Object) theShape,
                        Standard_Real& theX, Standard_Real& theY, Standard_Real& theZ)
{
  SetErrorCode(KO);

  if (theShape.IsNull())
    return;

  Handle(GEOM_Function) aRefShape = theShape->GetLastFunction();
  if (aRefShape.IsNull())
    return;

  TopoDS_Shape aShape = aRefShape->GetValue();
  if (aShape.IsNull() || aShape.ShapeType() != TopAbs_VERTEX)
  {
    SetErrorCode( "Shape must be a vertex" );
    return;
  }

  try {
    OCC_CATCH_SIGNALS;
    gp_Pnt aPnt = BRep_Tool::Pnt( TopoDS::Vertex( aShape ) );
    theX = aPnt.X();
    theY = aPnt.Y();
    theZ = aPnt.Z();

    SetErrorCode(OK);
  }
  catch (Standard_Failure& aFail)
  {
    SetErrorCode( aFail.GetMessageString() );
  }
}

//=======================================================================
/*!
 *  Compute angle (in degrees) between two lines
 */
//=======================================================================
Standard_Real GEOMImpl_IMeasureOperations::GetAngle (Handle(GEOM_Object) theLine1,
                                                     Handle(GEOM_Object) theLine2)
{
  if (theLine1->GetType() == GEOM_VECTOR &&
      theLine2->GetType() == GEOM_VECTOR)
    return GetAngleBtwVectors(theLine1, theLine2);

  SetErrorCode(KO);

  Standard_Real anAngle = -1.0;

  if (theLine1.IsNull() || theLine2.IsNull())
    return anAngle;

  Handle(GEOM_Function) aRefLine1 = theLine1->GetLastFunction();
  Handle(GEOM_Function) aRefLine2 = theLine2->GetLastFunction();
  if (aRefLine1.IsNull() || aRefLine2.IsNull())
    return anAngle;

  TopoDS_Shape aLine1 = aRefLine1->GetValue();
  TopoDS_Shape aLine2 = aRefLine2->GetValue();
  if (aLine1.IsNull() || aLine2.IsNull() ||
      aLine1.ShapeType() != TopAbs_EDGE ||
      aLine2.ShapeType() != TopAbs_EDGE)
  {
    SetErrorCode("Two edges must be given");
    return anAngle;
  }

  try {
    OCC_CATCH_SIGNALS;
    TopoDS_Edge E1 = TopoDS::Edge(aLine1);
    TopoDS_Edge E2 = TopoDS::Edge(aLine2);

    double fp,lp;
    Handle(Geom_Curve) C1 = BRep_Tool::Curve(E1,fp,lp);
    Handle(Geom_Curve) C2 = BRep_Tool::Curve(E2,fp,lp);

    if ( C1.IsNull() || C2.IsNull() ||
        !C1->IsKind(STANDARD_TYPE(Geom_Line)) ||
        !C2->IsKind(STANDARD_TYPE(Geom_Line)))
    {
      SetErrorCode("The edges must be linear");
      return anAngle;
    }

    Handle(Geom_Line) L1 = Handle(Geom_Line)::DownCast(C1);
    Handle(Geom_Line) L2 = Handle(Geom_Line)::DownCast(C2);

    gp_Lin aLin1 = L1->Lin();
    gp_Lin aLin2 = L2->Lin();

    anAngle = aLin1.Angle(aLin2);
    anAngle *= 180. / M_PI; // convert radians into degrees

    if (anAngle > 90.0) {
      anAngle = 180.0 - anAngle;
    }

    SetErrorCode(OK);
  }
  catch (Standard_Failure& aFail)
  {
    SetErrorCode(aFail.GetMessageString());
  }

  return anAngle;
}

//=======================================================================
/*!
 *  Compute angle (in degrees) between two vectors
 */
//=======================================================================
Standard_Real GEOMImpl_IMeasureOperations::GetAngleBtwVectors (Handle(GEOM_Object) theVec1,
                                                               Handle(GEOM_Object) theVec2)
{
  SetErrorCode(KO);

  Standard_Real anAngle = -1.0;

  if (theVec1.IsNull() || theVec2.IsNull())
    return anAngle;

  if (theVec1->GetType() != GEOM_VECTOR || theVec2->GetType() != GEOM_VECTOR) {
    SetErrorCode("Two vectors must be given");
    return anAngle;
  }

  Handle(GEOM_Function) aRefVec1 = theVec1->GetLastFunction();
  Handle(GEOM_Function) aRefVec2 = theVec2->GetLastFunction();
  if (aRefVec1.IsNull() || aRefVec2.IsNull())
    return anAngle;

  TopoDS_Shape aVec1 = aRefVec1->GetValue();
  TopoDS_Shape aVec2 = aRefVec2->GetValue();
  if (aVec1.IsNull() || aVec2.IsNull() ||
      aVec1.ShapeType() != TopAbs_EDGE ||
      aVec2.ShapeType() != TopAbs_EDGE)
  {
    SetErrorCode("Two edges must be given");
    return anAngle;
  }

  try {
    OCC_CATCH_SIGNALS;
    TopoDS_Edge aE1 = TopoDS::Edge(aVec1);
    TopoDS_Edge aE2 = TopoDS::Edge(aVec2);

    TopoDS_Vertex aP11, aP12, aP21, aP22;
    TopExp::Vertices(aE1, aP11, aP12, Standard_True);
    TopExp::Vertices(aE2, aP21, aP22, Standard_True);
    if (aP11.IsNull() || aP12.IsNull() || aP21.IsNull() || aP22.IsNull()) {
      SetErrorCode("Bad edge given");
      return anAngle;
    }

    gp_Vec aV1 (BRep_Tool::Pnt(aP11), BRep_Tool::Pnt(aP12));
    gp_Vec aV2 (BRep_Tool::Pnt(aP21), BRep_Tool::Pnt(aP22)) ;

    anAngle = aV1.Angle(aV2);
    anAngle *= 180. / M_PI; // convert radians into degrees

    SetErrorCode(OK);
  }
  catch (Standard_Failure& aFail)
  {
    SetErrorCode(aFail.GetMessageString());
  }

  return anAngle;
}


//=============================================================================
/*!
 *  PatchFace
 */
 //=============================================================================
Handle(TColStd_HSequenceOfTransient) GEOMImpl_IMeasureOperations::PatchFace(Handle(GEOM_Object) theShape)
{
  SetErrorCode(KO);

  if (theShape.IsNull()) return NULL;

  Handle(GEOM_Object) aPatchFace = GetEngine()->AddObject(GEOM_PATCH_FACE);
  Handle(GEOM_Function) aFunction = aPatchFace->AddFunction(GEOMImpl_PatchFaceDriver::GetID(), 1);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_PatchFaceDriver::GetID()) return NULL;

  GEOMImpl_IPatchFace aPI(aFunction);
  Handle(GEOM_Function) aRefShape = theShape->GetLastFunction();
  if (aRefShape.IsNull()) return NULL;

  aPI.SetShape(aRefShape);
  Handle(TColStd_HSequenceOfTransient) aSeq = new TColStd_HSequenceOfTransient;

  // Perform
  try
  {
    OCC_CATCH_SIGNALS;
    if (!GetSolver()->ComputeFunction(aFunction))
    {
      SetErrorCode("patch face driver failed");
      return NULL;
    }

    // Get result compound and collect all faces into result sequence
    TopoDS_Shape aResCompound = aFunction->GetValue();
    TopTools_IndexedMapOfShape anIndices;
    TopExp::MapShapes(aResCompound, anIndices);

    Handle(TColStd_HArray1OfInteger) anArray;
    for (TopExp_Explorer anExpW(aResCompound, TopAbs_FACE); anExpW.More(); anExpW.Next())
    {
      TopoDS_Shape aValue = anExpW.Value();
      anArray = new TColStd_HArray1OfInteger(1, 1);
      anArray->SetValue(1, anIndices.FindIndex(aValue));

      Handle(GEOM_Object) anObj = GetEngine()->AddSubShape(aPatchFace, anArray);
      if (!anObj.IsNull())
      {
        aSeq->Append(anObj);
      }
    }
  }
  catch (Standard_Failure& aFail)
  {
    SetErrorCode(aFail.GetMessageString());
    return aSeq;
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction, true)
    << "[" << aSeq << "] = geompy.PatchFace(" << theShape << ")";

  SetErrorCode(OK);
  return aSeq;
}

//=============================================================================
/*!
 *  CurveCurvatureByParam
 */
//=============================================================================
Standard_Real GEOMImpl_IMeasureOperations::CurveCurvatureByParam
                        (Handle(GEOM_Object) theCurve, Standard_Real& theParam)
{
  SetErrorCode(KO);
  Standard_Real aRes = -1.0;

  if(theCurve.IsNull()) return aRes;

  Handle(GEOM_Function) aRefShape = theCurve->GetLastFunction();
  if(aRefShape.IsNull()) return aRes;

  TopoDS_Shape aShape = aRefShape->GetValue();
  if(aShape.IsNull()) {
    SetErrorCode("One of Objects has NULL Shape");
    return aRes;
  }

  Standard_Real aFP, aLP, aP;
  Handle(Geom_Curve) aCurve = BRep_Tool::Curve(TopoDS::Edge(aShape), aFP, aLP);
  aP = aFP + (aLP - aFP) * theParam;

  if(aCurve.IsNull()) return aRes;

  //Compute curvature
  try {
    OCC_CATCH_SIGNALS;
    GeomLProp_CLProps Prop = GeomLProp_CLProps
      (aCurve, aP, 2, Precision::Confusion());
    aRes = fabs(Prop.Curvature());
    SetErrorCode(OK);
  }
  catch (Standard_Failure& aFail) {
    SetErrorCode(aFail.GetMessageString());
    return aRes;
  }

  if( aRes > Precision::Confusion() )
    aRes = 1/aRes;
  else
    aRes = RealLast();

  return aRes;
}


//=============================================================================
/*!
 *  CurveCurvatureByPoint
 */
//=============================================================================
Standard_Real GEOMImpl_IMeasureOperations::CurveCurvatureByPoint
                   (Handle(GEOM_Object) theCurve, Handle(GEOM_Object) thePoint)
{
  SetErrorCode(KO);
  Standard_Real aRes = -1.0;

  if( theCurve.IsNull() || thePoint.IsNull() ) return aRes;

  Handle(GEOM_Function) aRefCurve = theCurve->GetLastFunction();
  Handle(GEOM_Function) aRefPoint = thePoint->GetLastFunction();
  if( aRefCurve.IsNull() || aRefPoint.IsNull() ) return aRes;

  TopoDS_Edge anEdge = TopoDS::Edge(aRefCurve->GetValue());
  TopoDS_Vertex aPnt = TopoDS::Vertex(aRefPoint->GetValue());
  if( anEdge.IsNull() || aPnt.IsNull() ) {
    SetErrorCode("One of Objects has NULL Shape");
    return aRes;
  }

  Standard_Real aFP, aLP;
  Handle(Geom_Curve) aCurve = BRep_Tool::Curve(anEdge, aFP, aLP);
  if(aCurve.IsNull()) return aRes;
  gp_Pnt aPoint = BRep_Tool::Pnt(aPnt);

  //Compute curvature
  try {
    OCC_CATCH_SIGNALS;
    GeomAPI_ProjectPointOnCurve PPCurve(aPoint, aCurve, aFP, aLP);
    if(PPCurve.NbPoints()>0) {
      GeomLProp_CLProps Prop = GeomLProp_CLProps
        (aCurve, PPCurve.LowerDistanceParameter(), 2, Precision::Confusion());
      aRes = fabs(Prop.Curvature());
      SetErrorCode(OK);
    }
  }
  catch (Standard_Failure& aFail) {
    SetErrorCode(aFail.GetMessageString());
    return aRes;
  }

  if( aRes > Precision::Confusion() )
    aRes = 1/aRes;
  else
    aRes = RealLast();

  return aRes;
}


//=============================================================================
/*!
 *  getSurfaceCurvatures
 */
//=============================================================================
Standard_Real GEOMImpl_IMeasureOperations::getSurfaceCurvatures
                                          (const Handle(Geom_Surface)& aSurf,
                                           Standard_Real theUParam,
                                           Standard_Real theVParam,
                                           Standard_Boolean theNeedMaxCurv)
{
  SetErrorCode(KO);
  Standard_Real aRes = 1.0;

  if (aSurf.IsNull()) return aRes;

  try {
    OCC_CATCH_SIGNALS;
    GeomLProp_SLProps Prop = GeomLProp_SLProps
      (aSurf, theUParam, theVParam, 2, Precision::Confusion());
    if(Prop.IsCurvatureDefined()) {
      if(Prop.IsUmbilic()) {
        //cout<<"is umbilic"<<endl;
        aRes = fabs(Prop.MeanCurvature());
      }
      else {
        //cout<<"is not umbilic"<<endl;
        double c1 = fabs(Prop.MaxCurvature());
        double c2 = fabs(Prop.MinCurvature());
        if(theNeedMaxCurv)
          aRes = Max(c1,c2);
        else
          aRes = Min(c1,c2);
      }
      SetErrorCode(OK);
    }
  }
  catch (Standard_Failure& aFail) {
    SetErrorCode(aFail.GetMessageString());
    return aRes;
  }

  if( fabs(aRes) > Precision::Confusion() )
    aRes = 1/aRes;
  else
    aRes = RealLast();

  return aRes;
}


//=============================================================================
/*!
 *  MaxSurfaceCurvatureByParam
 */
//=============================================================================
Standard_Real GEOMImpl_IMeasureOperations::MaxSurfaceCurvatureByParam
                                                  (Handle(GEOM_Object) theSurf,
                                                   Standard_Real& theUParam,
                                                   Standard_Real& theVParam)
{
  SetErrorCode(KO);
  Standard_Real aRes = -1.0;

  if (theSurf.IsNull()) return aRes;

  Handle(GEOM_Function) aRefShape = theSurf->GetLastFunction();
  if(aRefShape.IsNull()) return aRes;

  TopoDS_Shape aShape = aRefShape->GetValue();
  if(aShape.IsNull()) {
    SetErrorCode("One of Objects has NULL Shape");
    return aRes;
  }

  TopoDS_Face F = TopoDS::Face(aShape);
  Handle(Geom_Surface) aSurf = BRep_Tool::Surface(F);

  //Compute the parameters
  Standard_Real U1,U2,V1,V2;
  BRepTools::UVBounds(F,U1,U2,V1,V2);
  Standard_Real U = U1 + (U2-U1)*theUParam;
  Standard_Real V = V1 + (V2-V1)*theVParam;

  return getSurfaceCurvatures(aSurf, U, V, true);
}


//=============================================================================
/*!
 *  MaxSurfaceCurvatureByPoint
 */
//=============================================================================
Standard_Real GEOMImpl_IMeasureOperations::MaxSurfaceCurvatureByPoint
                    (Handle(GEOM_Object) theSurf, Handle(GEOM_Object) thePoint)
{
  SetErrorCode(KO);
  Standard_Real aRes = -1.0;

  if( theSurf.IsNull() || thePoint.IsNull() ) return aRes;

  Handle(GEOM_Function) aRefShape = theSurf->GetLastFunction();
  Handle(GEOM_Function) aRefPoint = thePoint->GetLastFunction();
  if( aRefShape.IsNull() || aRefPoint.IsNull() ) return aRes;

  TopoDS_Face aFace = TopoDS::Face(aRefShape->GetValue());
  TopoDS_Vertex aPnt = TopoDS::Vertex(aRefPoint->GetValue());
  if( aFace.IsNull() || aPnt.IsNull() ) {
    SetErrorCode("One of Objects has NULL Shape");
    return 0;
  }

  Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFace);
  if(aSurf.IsNull()) return aRes;
  gp_Pnt aPoint = BRep_Tool::Pnt(aPnt);

  //Compute the parameters
  ShapeAnalysis_Surface sas(aSurf);
  gp_Pnt2d UV = sas.ValueOfUV(aPoint,Precision::Confusion());

  return getSurfaceCurvatures(aSurf, UV.X(), UV.Y(), true);
}


//=============================================================================
/*!
 *  MinSurfaceCurvatureByParam
 */
//=============================================================================
Standard_Real GEOMImpl_IMeasureOperations::MinSurfaceCurvatureByParam
                                                  (Handle(GEOM_Object) theSurf,
                                                   Standard_Real& theUParam,
                                                   Standard_Real& theVParam)
{
  SetErrorCode(KO);
  Standard_Real aRes = -1.0;

  if (theSurf.IsNull()) return aRes;

  Handle(GEOM_Function) aRefShape = theSurf->GetLastFunction();
  if(aRefShape.IsNull()) return aRes;

  TopoDS_Shape aShape = aRefShape->GetValue();
  if(aShape.IsNull()) {
    SetErrorCode("One of Objects has NULL Shape");
    return aRes;
  }

  TopoDS_Face F = TopoDS::Face(aShape);
  Handle(Geom_Surface) aSurf = BRep_Tool::Surface(F);

  //Compute the parameters
  Standard_Real U1,U2,V1,V2;
  BRepTools::UVBounds(F,U1,U2,V1,V2);
  Standard_Real U = U1 + (U2-U1)*theUParam;
  Standard_Real V = V1 + (V2-V1)*theVParam;

  return getSurfaceCurvatures(aSurf, U, V, false);
}


//=============================================================================
/*!
 *  MinSurfaceCurvatureByPoint
 */
//=============================================================================
Standard_Real GEOMImpl_IMeasureOperations::MinSurfaceCurvatureByPoint
                    (Handle(GEOM_Object) theSurf, Handle(GEOM_Object) thePoint)
{
  SetErrorCode(KO);
  Standard_Real aRes = -1.0;

  if( theSurf.IsNull() || thePoint.IsNull() ) return aRes;

  Handle(GEOM_Function) aRefShape = theSurf->GetLastFunction();
  Handle(GEOM_Function) aRefPoint = thePoint->GetLastFunction();
  if( aRefShape.IsNull() || aRefPoint.IsNull() ) return aRes;

  TopoDS_Face aFace = TopoDS::Face(aRefShape->GetValue());
  TopoDS_Vertex aPnt = TopoDS::Vertex(aRefPoint->GetValue());
  if( aFace.IsNull() || aPnt.IsNull() ) {
    SetErrorCode("One of Objects has NULL Shape");
    return 0;
  }

  Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFace);
  if(aSurf.IsNull()) return aRes;
  gp_Pnt aPoint = BRep_Tool::Pnt(aPnt);

  //Compute the parameters
  ShapeAnalysis_Surface sas(aSurf);
  gp_Pnt2d UV = sas.ValueOfUV(aPoint,Precision::Confusion());

  return getSurfaceCurvatures(aSurf, UV.X(), UV.Y(), false);
}

//=============================================================================
/*!
 *  SurfaceCurvatureByPointAndDirection
 */
//=============================================================================
Handle(GEOM_Object) GEOMImpl_IMeasureOperations::SurfaceCurvatureByPointAndDirection
                                                 (Handle(GEOM_Object) theSurf,
                                                  Handle(GEOM_Object) thePoint,
                                                  Handle(GEOM_Object) theDirection)
{
  SetErrorCode(KO);

  if (theSurf.IsNull() || thePoint.IsNull() || theDirection.IsNull()) return NULL;

  Handle(GEOM_Function) aSurf = theSurf->GetLastFunction();
  Handle(GEOM_Function) aPoint = thePoint->GetLastFunction();
  Handle(GEOM_Function) aDirection = theDirection->GetLastFunction();
  if (aSurf.IsNull() || aPoint.IsNull() || aDirection.IsNull()) return NULL;

  //Add a new CurvatureVector object
  //Handle(GEOM_Object) aCV = GetEngine()->AddObject(GEOM_CURVATURE_VEC);
  Handle(GEOM_Object) aCV = GetEngine()->AddObject(GEOM_VECTOR);

  //Add a new CurvatureVector function
  Handle(GEOM_Function) aFunction =
    aCV->AddFunction(GEOMImpl_MeasureDriver::GetID(), CURVATURE_VEC_MEASURE);
  if (aFunction.IsNull()) return NULL;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_MeasureDriver::GetID()) return NULL;

  GEOMImpl_IMeasure aCI (aFunction);
  aCI.SetBase(aSurf);
  aCI.SetPoint(aPoint);
  aCI.SetDirection(aDirection);

  //Compute the CurvatureVector
  try {
    OCC_CATCH_SIGNALS;
    if (!GetSolver()->ComputeFunction(aFunction)) {
      SetErrorCode("Measure driver failed to compute a surface curvature");
      return NULL;
    }
  }
  catch (Standard_Failure& aFail) {
    SetErrorCode(aFail.GetMessageString());
    return NULL;
  }

  //Make a Python command
  GEOM::TPythonDump(aFunction) << aCV << " = geompy.CurvatureOnFace(" << theSurf
                               << ", " << thePoint << ", " << theDirection << ")";

  SetErrorCode(OK);
  return aCV;
}

//=============================================================================
/*!
 *  XYZtoUV
 */
 //=============================================================================
Handle(TColStd_HArray1OfReal) GEOMImpl_IMeasureOperations::XYZtoUV
                                                  (Handle(GEOM_Object) theSurf,
                                                   const Handle(TColStd_HArray1OfReal)& theXYZlist,
                                                   bool isNormalized)
{
  SetErrorCode(KO);

  Handle(TColStd_HArray1OfReal) aRet;

  // Check list of coordinates
  int nbC = theXYZlist->Length();
  int nbP = nbC / 3;
  if (nbP * 3 != nbC) {
    SetErrorCode("Coordinates list length is not divisible by 3");
    return aRet;
  }

  // Check face
  if (theSurf.IsNull()) {
    SetErrorCode("The shape is NULL");
    return aRet;
  }

  Handle(GEOM_Function) aRefShape = theSurf->GetLastFunction();
  if (aRefShape.IsNull()) {
    SetErrorCode("The shape is NULL");
    return aRet;
  }

  TopoDS_Shape aShape = aRefShape->GetValue();
  if (aShape.IsNull()) {
    SetErrorCode("The shape is NULL");
    return aRet;
  }

  // The shape can be a face, a shell of one face or a compound with one face
  TopoDS_Face F;
  if (aShape.ShapeType() == TopAbs_FACE) {
    F = TopoDS::Face(aShape);
  }
  else if (aShape.ShapeType() < TopAbs_FACE) {
    TopExp_Explorer Exp (aShape, TopAbs_FACE);
    if (Exp.More()) {
      F = TopoDS::Face(Exp.Current());
      Exp.Next();
      if (Exp.More()) {
        SetErrorCode("There should be only one face");
        return aRet;
      }
    }
  }
  if (F.IsNull()) {
    SetErrorCode("There are no faces");
    return aRet;
  }

  // Face tolerance
  ShapeAnalysis_ShapeTolerance sat;
  sat.InitTolerance();
  sat.AddTolerance(F);
  Standard_Real aTol = sat.GlobalTolerance(1);
  Standard_Real squareTolerance = aTol * aTol;

  // Compute parameters
  Handle(Geom_Surface) aSurf = BRep_Tool::Surface(F);
  aRet = new TColStd_HArray1OfReal (0, nbP * 2 - 1);

  Standard_Real U1,U2, V1,V2;
  BRepTools::UVBounds(F, U1, U2, V1, V2);
  Standard_Real dU = U2 - U1;
  Standard_Real dV = V2 - V1;

  int iCLower = theXYZlist->Lower();
  for (int iP = 0; iP < nbP; iP++) {
    gp_Pnt aP (theXYZlist->Value(iCLower + iP * 3),
               theXYZlist->Value(iCLower + iP * 3 + 1),
               theXYZlist->Value(iCLower + iP * 3 + 2));
    try { // as GEOMUtils::ProjectPointOnFace can throw exceptions
      Standard_Real U, V;
      gp_Pnt aPonF = GEOMUtils::ProjectPointOnFace(aP, F, U, V, aTol);
      if (aP.SquareDistance(aPonF) < squareTolerance) {
        if (isNormalized) {
          // Normalize parameters to be in [0, 1]
          U = (U - U1) / dU;
          V = (V - V1) / dV;
        }
        aRet->SetValue(iP * 2    , U);
        aRet->SetValue(iP * 2 + 1, V);
      }
      else {
        SetErrorCode("Point too far from face");
        return aRet;
      }
    }
    catch (Standard_Failure& aFail) {
      SetErrorCode(aFail.GetMessageString());
      return aRet;
    }
  }

  SetErrorCode(OK);
  return aRet;
}

//=============================================================================
/*!
 *  UVtoXYZ
 */
 //=============================================================================
Handle(TColStd_HArray1OfReal) GEOMImpl_IMeasureOperations::UVtoXYZ
                                                  (Handle(GEOM_Object) theSurf,
                                                   const Handle(TColStd_HArray1OfReal)& theUVlist,
                                                   bool isNormalized)
{
  SetErrorCode(KO);

  Handle(TColStd_HArray1OfReal) aRet;

  // Check list of parameters
  int nbC = theUVlist->Length();
  int nbP = nbC / 2;
  if (nbP * 2 != nbC) {
    SetErrorCode("Parameters list length is not divisible by 2");
    return aRet;
  }

  // Check face
  if (theSurf.IsNull()) {
    SetErrorCode("The shape is NULL");
    return aRet;
  }

  Handle(GEOM_Function) aRefShape = theSurf->GetLastFunction();
  if (aRefShape.IsNull()) {
    SetErrorCode("The shape is NULL");
    return aRet;
  }

  TopoDS_Shape aShape = aRefShape->GetValue();
  if (aShape.IsNull()) {
    SetErrorCode("The shape is NULL");
    return aRet;
  }

  // The shape can be a face, a shell of one face or a compound with one face
  TopoDS_Face F;
  if (aShape.ShapeType() == TopAbs_FACE) {
    F = TopoDS::Face(aShape);
  }
  else if (aShape.ShapeType() < TopAbs_FACE) {
    TopExp_Explorer Exp (aShape, TopAbs_FACE);
    if (Exp.More()) {
      F = TopoDS::Face(Exp.Current());
      Exp.Next();
      if (Exp.More()) {
        SetErrorCode("There should be only one face");
        return aRet;
      }
    }
  }
  if (F.IsNull()) {
    SetErrorCode("There are no faces");
    return aRet;
  }

  // Face tolerance
  Standard_Real squareTolerance = BRep_Tool::Tolerance(F);
  squareTolerance = squareTolerance * squareTolerance;

  // Compute coordinates
  Handle(Geom_Surface) aSurf = BRep_Tool::Surface(F);
  aRet = new TColStd_HArray1OfReal (0, nbP * 3 - 1);

  Standard_Real U1,U2, V1,V2;
  BRepTools::UVBounds(F, U1, U2, V1, V2);
  Standard_Real dU = U2 - U1;
  Standard_Real dV = V2 - V1;

  Standard_Real tol = 1.e-4;
  Standard_Real pc = Precision::Confusion();

  int iCLower = theUVlist->Lower();
  for (int iP = 0; iP < nbP; iP++) {
    Standard_Real U = theUVlist->Value(iCLower + iP * 2);
    Standard_Real V = theUVlist->Value(iCLower + iP * 2 + 1);

    if (isNormalized) {
      // Get real parameters from given normalized ones in [0, 1]
      if (!(-pc < U && U < 1 + pc) || !(-pc < V && V < 1 + pc)) {
        SetErrorCode("Normalized parameter is out of range [0,1]");
        return aRet;
      }
      U = U1 + dU * U;
      V = V1 + dV * V;
    }

    gp_Pnt2d aP2d (U, V);

    BRepClass_FaceClassifier aClsf (F, aP2d, tol);
    if (aClsf.State() != TopAbs_IN && aClsf.State() != TopAbs_ON) {
      SetErrorCode("Given parameters are out of face");
      return aRet;
    }
    gp_Pnt surfPnt = aSurf->Value(U, V);

    aRet->SetValue(iP * 3    , surfPnt.X());
    aRet->SetValue(iP * 3 + 1, surfPnt.Y());
    aRet->SetValue(iP * 3 + 2, surfPnt.Z());
  }

  SetErrorCode(OK);
  return aRet;
}

//=============================================================================
/*!
 *  SelfIntersected2D
 *  Find all self-intersected 2D curves.
 *  \param theChecks list of failed checks, contains type of check and failed shapes
 */
 //=============================================================================
std::list<GEOMImpl_IMeasureOperations::CoupleOfObjects>
  GEOMImpl_IMeasureOperations::SelfIntersected2D(const std::list<FailedChecks>& theChecks)
{
  SetErrorCode(KO);
  MESSAGE("GEOMImpl_IMeasureOperations::selfIntersected2D");

  std::list<GEOMImpl_IMeasureOperations::CoupleOfObjects> aSelfInters2D;
  try
  {
    OCC_CATCH_SIGNALS;
    for (std::list<FailedChecks>::const_iterator anIter(theChecks.begin());
         anIter != theChecks.end(); ++anIter)
    {
      if (anIter->TypeOfCheck == BOPAlgo_CheckStatus::BOPAlgo_InvalidCurveOnSurface)
        aSelfInters2D.push_back(anIter->FailedShapes);
    }
  }
  catch (Standard_Failure& aFail)
  {
    SetErrorCode(aFail.GetMessageString());
    return aSelfInters2D;
  }

  SetErrorCode(OK);
  return aSelfInters2D;
}

namespace
{
  static bool checkTypes(const GEOMImpl_IMeasureOperations::CoupleOfObjects& theShapes,
                         const int theShapeType1,
                         const int theShapeType2)
  {
    if (theShapeType1 == -1 && theShapeType2 == -1)
      return true;

    TopAbs_ShapeEnum aShapeType1 = theShapes.first.IsNull()
      ? TopAbs_SHAPE
      : theShapes.first->GetValue().ShapeType();
    TopAbs_ShapeEnum aShapeType2 = theShapes.second.IsNull()
      ? TopAbs_SHAPE
      : theShapes.second->GetValue().ShapeType();

    if (theShapeType1 == -1)
      return aShapeType1 == theShapeType2 || aShapeType2 == theShapeType2;
    else if (theShapeType2 == -1)
      return aShapeType1 == theShapeType1 || aShapeType2 == theShapeType1;
    return (aShapeType1 == theShapeType1 && aShapeType2 == theShapeType2) ||
      (aShapeType1 == theShapeType2 && aShapeType2 == theShapeType1);
  }
} // namespace

//=============================================================================
/*!
 *  InterferingSubshapes
 *  Find pairs of interfering sub-shapes, by default all pairs of interfering shapes are returned.
 *  \param theChecks list of failed checks, contains type of check and failed shapes
 *  \param theShapeType1 Type of shape.
 *  \param theShapeType2 Type of shape.
 */
 //=============================================================================
std::list<GEOMImpl_IMeasureOperations::CoupleOfObjects>
  GEOMImpl_IMeasureOperations::InterferingSubshapes
    (const std::list<FailedChecks>& theChecks,
     const int                      theShapeType1,
     const int                      theShapeType2)
{
  SetErrorCode(KO);
  MESSAGE("GEOMImpl_IMeasureOperations::interferingSubshapes");

  std::list<GEOMImpl_IMeasureOperations::CoupleOfObjects> anInterfer;
  try
  {
    OCC_CATCH_SIGNALS;
    for (std::list<FailedChecks>::const_iterator anIter(theChecks.begin());
         anIter != theChecks.end(); ++anIter)
    {
      if (anIter->TypeOfCheck == BOPAlgo_CheckStatus::BOPAlgo_SelfIntersect &&
          checkTypes(anIter->FailedShapes, theShapeType1, theShapeType2))
        anInterfer.push_back(anIter->FailedShapes);
    }
  }
  catch (Standard_Failure& aFail)
  {
    SetErrorCode(aFail.GetMessageString());
    return anInterfer;
  }

  SetErrorCode(OK);
  return anInterfer;
}

//=============================================================================
/*!
 *  SmallEdges
 *  Find edges, which are fully covered by tolerances of vertices.
 *  \param theChecks list of failed checks, contains type of check and failed shapes
 */
 //=============================================================================
Handle(TColStd_HSequenceOfTransient) GEOMImpl_IMeasureOperations::SmallEdges(
    const std::list<FailedChecks>& theChecks)
{
  SetErrorCode(KO);
  MESSAGE("GEOMImpl_IMeasureOperations::smallEdges");

  Handle(TColStd_HSequenceOfTransient) aSmallEdges = new TColStd_HSequenceOfTransient;
  try
  {
    OCC_CATCH_SIGNALS;
    for (std::list<FailedChecks>::const_iterator anIter(theChecks.begin());
         anIter != theChecks.end(); ++anIter)
    {
      if (anIter->TypeOfCheck == BOPAlgo_CheckStatus::BOPAlgo_TooSmallEdge)
        aSmallEdges->Append(anIter->FailedShapes.first);
    }
  }
  catch (Standard_Failure& aFail)
  {
    SetErrorCode(aFail.GetMessageString());
    return NULL;
  }

  SetErrorCode(OK);
  return aSmallEdges;
}

//=============================================================================
/*!
 *  DistantShapes
 *  find remote objects (sub-shape on a shape).
 *  \param theShape Shape for check.
 *  \param theShapeType Type of shape.
 *  \param theSubShapeType Type of sub-shape.
 *  \param theTolerance tolerance.
 */
 //=============================================================================
std::list<GEOMImpl_IMeasureOperations::CoupleOfObjects>
  GEOMImpl_IMeasureOperations::DistantShapes
    (const std::list<FailedChecks>& theChecks,
     const int                      theShapeType,
     const int                      theSubShapeType,
     double                         theTolerance)
{
  SetErrorCode(KO);
  MESSAGE("GEOMImpl_IMeasureOperations::distantShapes");

  std::list<GEOMImpl_IMeasureOperations::CoupleOfObjects> aDistShapes;
  try
  {
    OCC_CATCH_SIGNALS;
    for (std::list<FailedChecks>::const_iterator anIter(theChecks.begin());
         anIter != theChecks.end(); ++anIter)
    {
      Handle(GEOM_Object) aSubShape = anIter->FailedShapes.first;
      Handle(GEOM_Object) aShape = anIter->FailedShapes.second;
      if ((anIter->TypeOfCheck == BOPAlgo_CheckStatus::BOPAlgo_InvalidCurveOnSurface ||
           anIter->TypeOfCheck == BOPAlgo_CheckStatus::BOPAlgo_IncompatibilityOfVertex ||
           anIter->TypeOfCheck == BOPAlgo_CheckStatus::BOPAlgo_IncompatibilityOfEdge ||
           anIter->TypeOfCheck == BOPAlgo_CheckStatus::BOPAlgo_IncompatibilityOfFace) &&
          aShape && (theShapeType == -1 || aShape->GetValue().ShapeType() == theShapeType) &&
          aSubShape && (theSubShapeType == -1 || aSubShape->GetValue().ShapeType() == theSubShapeType))
      {
        gp_XYZ aP1, aP2;
        Standard_Real aDist = Precision::Infinite();
        if (anIter->TypeOfCheck == BOPAlgo_CheckStatus::BOPAlgo_InvalidCurveOnSurface)
          aDist = ComputeTolerance(aSubShape, aShape);
        if (aDist > theTolerance)
          aDistShapes.push_back(anIter->FailedShapes);
      }
    }
  }
  catch (Standard_Failure& aFail)
  {
    SetErrorCode(aFail.GetMessageString());
    return aDistShapes;
  }

  SetErrorCode(OK);
  return aDistShapes;
}

//=============================================================================
/*!
 *  CheckConformityShape
 *  Perform analyse of shape and find imperfections in the shape.
 *  \param theShape Shape for analyse.
 */
 //=============================================================================
void GEOMImpl_IMeasureOperations::CheckConformityShape(Handle(GEOM_Object) theShape, std::list<FailedChecks>& theChecks)
{
  SetErrorCode(KO);
  MESSAGE("GEOMImpl_IMeasureOperations::checkShape");

  Handle(GEOM_Object) aConformity = GetEngine()->AddObject(GEOM_CHECKCONFORMITY);
  Handle(GEOM_Function) aFunction = aConformity->AddFunction(GEOMImpl_ConformityDriver::GetID(), CONFORMITY_CHECK_SHAPE);
  if (aFunction.IsNull()) return;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_ConformityDriver::GetID()) return;

  GEOMImpl_IConformity aCI(aFunction);

  Handle(GEOM_Function) aRefShape = theShape->GetLastFunction();
  if (aRefShape.IsNull()) return;

  aCI.SetShape(aRefShape);

  try
  {
    OCC_CATCH_SIGNALS;
    if (!GetSolver()->ComputeFunction(aFunction))
    {
      SetErrorCode("Failed: checkShape");
      return;
    }
    Handle(TColStd_HArray1OfInteger) aTypesChecks = aFunction->GetIntegerArray(CHECKCONFORMITY_RET_TYPES_CHECKS);
    Handle(TColStd_HArray2OfInteger) aRes = aCI.GetListOfShapesIndices();
    if (aRes.IsNull())
      return;

    for (Standard_Integer anIndex = 1; anIndex <= aRes->NbRows(); ++anIndex)
    {
      std::pair<Handle(GEOM_Object), Handle(GEOM_Object)> aPair;
      Handle(TColStd_HArray1OfInteger) anArray;
      anArray = new TColStd_HArray1OfInteger(1, 1);
      anArray->SetValue(1, aRes->Value(anIndex, 1));

      Handle(GEOM_Object) anObj = GetEngine()->AddSubShape(theShape, anArray);
      if (!anObj.IsNull())
        aPair.first = anObj;

      anArray = new TColStd_HArray1OfInteger(1, 1);
      anArray->SetValue(1, aRes->Value(anIndex, 2));

      anObj = GetEngine()->AddSubShape(theShape, anArray);
      if (!anObj.IsNull())
        aPair.second = anObj;
      theChecks.push_back({ aTypesChecks->Value(anIndex), aPair });
    }
  }
  catch (Standard_Failure& aFail)
  {
    SetErrorCode(aFail.GetMessageString());
    return;
  }

  SetErrorCode(OK);
  return;
}

//=============================================================================
/*!
 *  UpdateTolerance
 *  Compute possible tolerance for the shape, minimize tolerance of shape as well
 *  as tolerance of sub-shapes as much as possible
 *  \param theShape Shape for compute tolerance.
 */
 //=============================================================================
double GEOMImpl_IMeasureOperations::UpdateTolerance(Handle(GEOM_Object) theShape)
{
  SetErrorCode(KO);
  MESSAGE("GEOMImpl_IMeasureOperations::updateTolerance");

  double aResTol = -1;
  Handle(GEOM_Object) aConformity = GetEngine()->AddObject(GEOM_CHECKCONFORMITY);
  Handle(GEOM_Function) aFunction = aConformity->AddFunction(GEOMImpl_ConformityDriver::GetID(), CONFORMITY_UPDATE_TOL);
  if (aFunction.IsNull()) return aResTol;

  //Check if the function is set correctly
  if (aFunction->GetDriverGUID() != GEOMImpl_ConformityDriver::GetID()) return aResTol;

  GEOMImpl_IConformity aCI(aFunction);

  Handle(GEOM_Function) aRefShape = theShape->GetLastFunction();
  if (aRefShape.IsNull()) return aResTol;

  aCI.SetShape(aRefShape);

  try
  {
    OCC_CATCH_SIGNALS;
    if (!GetSolver()->ComputeFunction(aFunction))
    {
      SetErrorCode("Failed: updateTolerance");
      return aResTol;
    }
    aResTol = aFunction->GetReal(CHECKCONFORMITY_RET_TOLERANCE);
  }
  catch (Standard_Failure& aFail)
  {
    SetErrorCode(aFail.GetMessageString());
    return aResTol;
  }

  SetErrorCode(OK);
  return aResTol;
}

//=============================================================================
/*!
 *  ComputeTolerance
 *  Compute distance from the edge to the face.
 */
 //=============================================================================
double GEOMImpl_IMeasureOperations::ComputeTolerance(Handle(GEOM_Object) theEdge,
                                                     Handle(GEOM_Object) theFace)
{
  double aMaxDist = Precision::Infinite();
  if (theEdge.IsNull() || theFace.IsNull())
    return aMaxDist;

  Handle(GEOM_Function) aRefEdge = theEdge->GetLastFunction();
  Handle(GEOM_Function) aRefFace = theFace->GetLastFunction();
  if (aRefEdge.IsNull() || aRefFace.IsNull())
    return aMaxDist;

  TopoDS_Edge aEdge = TopoDS::Edge(aRefEdge->GetValue());
  TopoDS_Face aFace = TopoDS::Face(aRefFace->GetValue());
  if (aEdge.IsNull() || aFace.IsNull())
    return aMaxDist;

  double aParam = 0.0;
  BOPTools_AlgoTools::ComputeTolerance(aFace, aEdge, aMaxDist, aParam);
  return aMaxDist;
}

//=======================================================================
//function : FillErrorsSub
//purpose  : Fill the errors list of subshapes on shape.
//=======================================================================
void GEOMImpl_IMeasureOperations::FillErrorsSub
           (const BRepCheck_Analyzer                   &theAna,
            const TopoDS_Shape                         &theShape,
            const TopAbs_ShapeEnum                     theSubType,
                  TopTools_DataMapOfIntegerListOfShape &theMapErrors) const
{
  TopExp_Explorer anExp(theShape, theSubType);
  TopTools_MapOfShape aMapSubShapes;

  for (; anExp.More(); anExp.Next()) {
    const TopoDS_Shape &aSubShape = anExp.Current();

    if (aMapSubShapes.Add(aSubShape)) {
      const Handle(BRepCheck_Result) &aRes = theAna.Result(aSubShape);

      for (aRes->InitContextIterator();
           aRes->MoreShapeInContext(); 
           aRes->NextShapeInContext()) {
        if (aRes->ContextualShape().IsSame(theShape)) {
          BRepCheck_ListIteratorOfListOfStatus itl(aRes->StatusOnShape());

          if (itl.Value() != BRepCheck_NoError) {
            // Add all errors for theShape and its sub-shape.
            for (;itl.More(); itl.Next()) {
              const Standard_Integer aStat = (Standard_Integer)itl.Value();

              if (!theMapErrors.IsBound(aStat)) {
                TopTools_ListOfShape anEmpty;

                theMapErrors.Bind(aStat, anEmpty);
              }

              TopTools_ListOfShape &theShapes = theMapErrors.ChangeFind(aStat);

              theShapes.Append(aSubShape);
              theShapes.Append(theShape);
            }
          }
        }

        break;
      }
    }
  }
}

//=======================================================================
//function : FillErrors
//purpose  : Fill the errors list.
//=======================================================================
void GEOMImpl_IMeasureOperations::FillErrors
             (const BRepCheck_Analyzer                   &theAna,
              const TopoDS_Shape                         &theShape,
                    TopTools_DataMapOfIntegerListOfShape &theMapErrors,
                    TopTools_MapOfShape                  &theMapShapes) const
{
  if (theMapShapes.Add(theShape)) {
    // Fill errors of child shapes.
    for (TopoDS_Iterator iter(theShape); iter.More(); iter.Next()) {
      FillErrors(theAna, iter.Value(), theMapErrors, theMapShapes);
    }

    // Fill errors of theShape.
    const Handle(BRepCheck_Result) &aRes = theAna.Result(theShape);

    if (!aRes.IsNull()) {
      BRepCheck_ListIteratorOfListOfStatus itl(aRes->Status());

      if (itl.Value() != BRepCheck_NoError) {
        // Add all errors for theShape.
        for (;itl.More(); itl.Next()) {
          const Standard_Integer aStat = (Standard_Integer)itl.Value();

          if (!theMapErrors.IsBound(aStat)) {
            TopTools_ListOfShape anEmpty;

            theMapErrors.Bind(aStat, anEmpty);
          }

          theMapErrors.ChangeFind(aStat).Append(theShape);
        }
      }
    }

    // Add errors of subshapes on theShape.
    const TopAbs_ShapeEnum aType = theShape.ShapeType();

    switch (aType) {
    case TopAbs_EDGE:
      FillErrorsSub(theAna, theShape, TopAbs_VERTEX, theMapErrors);
      break;
    case TopAbs_FACE:
      FillErrorsSub(theAna, theShape, TopAbs_WIRE, theMapErrors);
      FillErrorsSub(theAna, theShape, TopAbs_EDGE, theMapErrors);
      FillErrorsSub(theAna, theShape, TopAbs_VERTEX, theMapErrors);
      break;
    case TopAbs_SOLID:
      FillErrorsSub(theAna, theShape, TopAbs_SHELL, theMapErrors);
      break;
    default:
      break;
    }
  }
}

//=======================================================================
//function : FillErrors
//purpose  : Fill the errors list.
//=======================================================================
void GEOMImpl_IMeasureOperations::FillErrors
                  (const BRepCheck_Analyzer    &theAna,
                   const TopoDS_Shape          &theShape,
                         std::list<ShapeError> &theErrors) const
{
  // Fill the errors map.
  TopTools_DataMapOfIntegerListOfShape aMapErrors;
  TopTools_MapOfShape                  aMapShapes;

  FillErrors(theAna, theShape, aMapErrors, aMapShapes);

  // Map sub-shapes and their indices
  TopTools_IndexedMapOfShape anIndices;

  TopExp::MapShapes(theShape, anIndices);

  TopTools_DataMapIteratorOfDataMapOfIntegerListOfShape aMapIter(aMapErrors);

  for (; aMapIter.More(); aMapIter.Next()) {
    ShapeError anError;

    anError.error = (BRepCheck_Status)aMapIter.Key();

    TopTools_ListIteratorOfListOfShape aListIter(aMapIter.Value());
    TopTools_MapOfShape                aMapUnique;

    for (; aListIter.More(); aListIter.Next()) {
      const TopoDS_Shape &aShape = aListIter.Value();

      if (aMapUnique.Add(aShape)) {
        const Standard_Integer anIndex = anIndices.FindIndex(aShape);

        anError.incriminated.push_back(anIndex);
      }
    }

    if (!anError.incriminated.empty()) {
      theErrors.push_back(anError);
    }
  }
}

//=======================================================================
//function : ShapeProximityCalculator
//purpose  : returns an object to compute the proximity value
//=======================================================================
Handle(GEOM_Object) GEOMImpl_IMeasureOperations::ShapeProximityCalculator
                                          (Handle(GEOM_Object) theShape1,
                                           Handle(GEOM_Object) theShape2)
{
  SetErrorCode(KO);

  if (theShape1.IsNull() || theShape2.IsNull())
    return NULL;

  Handle(GEOM_Function) aShapeFunc1 = theShape1->GetLastFunction();
  Handle(GEOM_Function) aShapeFunc2 = theShape2->GetLastFunction();
  if (aShapeFunc1.IsNull() || aShapeFunc2.IsNull())
    return NULL;

  Handle(GEOM_Object) aProximityCalc = GetEngine()->AddObject(GEOM_SHAPE_PROXIMITY);
  if (aProximityCalc.IsNull())
    return NULL;

  Handle(GEOM_Function) aProximityFuncCoarse =
      aProximityCalc->AddFunction(GEOMImpl_ShapeProximityDriver::GetID(), PROXIMITY_COARSE);
  //Check if the function is set correctly
  if (aProximityFuncCoarse.IsNull() ||
      aProximityFuncCoarse->GetDriverGUID() != GEOMImpl_ShapeProximityDriver::GetID())
    return NULL;

  GEOMImpl_IProximity aProximity (aProximityFuncCoarse);
  aProximity.SetShapes(aShapeFunc1, aShapeFunc2);

  //Make a Python command
  GEOM::TPythonDump pd (aProximityFuncCoarse);
  pd << "p = geompy.ShapeProximity()\n";
  pd << "p.setShapes(" << theShape1 << ", " << theShape2 << ")";

  SetErrorCode(OK);
  return aProximityCalc;
}

//=======================================================================
//function : SetShapeSampling
//purpose  : set number sample points to compute the coarse proximity
//=======================================================================
void GEOMImpl_IMeasureOperations::SetShapeSampling(Handle(GEOM_Object) theCalculator,
                                                   Handle(GEOM_Object) theShape,
                                                   const Standard_Integer theNbSamples)
{
  SetErrorCode(KO);
  if (theShape.IsNull() ||
      theCalculator.IsNull() ||
      theCalculator->GetNbFunctions() <= 0 ||
      theNbSamples <= 0)
    return ;

  Handle(GEOM_Function) aProximityFuncCoarse = theCalculator->GetFunction(1);
  if (aProximityFuncCoarse.IsNull() ||
      aProximityFuncCoarse->GetDriverGUID() != GEOMImpl_ShapeProximityDriver::GetID())
    return ;

  Handle(GEOM_Function) aShapeFunc = theShape->GetLastFunction();
  if (aShapeFunc.IsNull())
    return ;

  GEOMImpl_IProximity aProximity(aProximityFuncCoarse);
  Handle(GEOM_Function) aShape1, aShape2;
  aProximity.GetShapes(aShape1, aShape2);
  if (aShape1->GetValue() == aShapeFunc->GetValue())
    aProximity.SetNbSamples(PROXIMITY_ARG_SAMPLES1, theNbSamples);
  else if (aShape2->GetValue() == aShapeFunc->GetValue())
    aProximity.SetNbSamples(PROXIMITY_ARG_SAMPLES2, theNbSamples);

  //Make a Python command
  GEOM::TPythonDump(aProximityFuncCoarse, /*append=*/true) <<
    "p.setSampling(" << theShape << ", " << theNbSamples << ")";

  SetErrorCode(OK);
}

//=======================================================================
//function : GetCoarseProximity
//purpose  : compute coarse proximity
//=======================================================================
Standard_Real GEOMImpl_IMeasureOperations::GetCoarseProximity(Handle(GEOM_Object) theCalculator,
                                                              bool doPythonDump)
{
  SetErrorCode(KO);
  if (theCalculator.IsNull())
    return -1;

  Handle(GEOM_Function) aProximityFuncCoarse = theCalculator->GetFunction(1);
  if (aProximityFuncCoarse.IsNull() ||
      aProximityFuncCoarse->GetDriverGUID() != GEOMImpl_ShapeProximityDriver::GetID() ||
      aProximityFuncCoarse->GetType() != PROXIMITY_COARSE)
    return -1;

  // Perform
  // We have to recompute the function each time,
  // because the number of samples can be changed
  try {
    OCC_CATCH_SIGNALS;
    if (!GetSolver()->ComputeFunction(aProximityFuncCoarse)) {
      SetErrorCode("shape proximity driver failed");
      return -1;
    }
  }
  catch (Standard_Failure& aFail) {
    SetErrorCode(aFail.GetMessageString());
    return -1;
  }

  //Make a Python command
  if (doPythonDump)
    GEOM::TPythonDump(aProximityFuncCoarse, /*append=*/true) << "value = p.coarseProximity()";

  SetErrorCode(OK);
  GEOMImpl_IProximity aProximity (aProximityFuncCoarse);
  return aProximity.GetValue();
}

//=======================================================================
//function : GetPreciseProximity
//purpose  : compute precise proximity
//=======================================================================
Standard_Real GEOMImpl_IMeasureOperations::GetPreciseProximity(Handle(GEOM_Object) theCalculator)
{
  SetErrorCode(KO);
  if (theCalculator.IsNull())
    return -1;

  Handle(GEOM_Function) aProximityFuncCoarse = theCalculator->GetFunction(1);
  Handle(GEOM_Function) aProximityFuncFine = theCalculator->GetFunction(2);
  if (aProximityFuncFine.IsNull())
    aProximityFuncFine = theCalculator->AddFunction
      (GEOMImpl_ShapeProximityDriver::GetID(), PROXIMITY_PRECISE);

  //Check if the functions are set correctly
  if (aProximityFuncCoarse.IsNull() ||
      aProximityFuncCoarse->GetDriverGUID() != GEOMImpl_ShapeProximityDriver::GetID() ||
      aProximityFuncFine.IsNull() ||
      aProximityFuncFine->GetDriverGUID() != GEOMImpl_ShapeProximityDriver::GetID())
    return -1;

  // perform coarse computation beforehand
  GetCoarseProximity(theCalculator, /*doPythonDump=*/false);

  // transfer parameters from the coarse to precise calculator
  GEOMImpl_IProximity aCoarseProximity (aProximityFuncCoarse);
  Handle(GEOM_Function) aShape1, aShape2;
  aCoarseProximity.GetShapes(aShape1, aShape2);
  if (aShape1.IsNull() || aShape2.IsNull())
    return -1;
  gp_Pnt aProxPnt1, aProxPnt2;
  Standard_Integer intStatus1, intStatus2;
  aCoarseProximity.GetProximityPoints(aProxPnt1, aProxPnt2);
  aCoarseProximity.GetStatusOfPoints(intStatus1, intStatus2);
  Standard_Real aResultValue = aCoarseProximity.GetValue();

  GEOMImpl_IProximity aFineProximity (aProximityFuncFine);
  aFineProximity.SetShapes(aShape1, aShape2);
  aFineProximity.SetProximityPoints(aProxPnt1, aProxPnt2);
  aFineProximity.SetStatusOfPoints(intStatus1, intStatus2);
  aFineProximity.SetValue(aResultValue); // in some cases this value cannot be precised

  // Perform
  try {
    OCC_CATCH_SIGNALS;
    if (!GetSolver()->ComputeFunction(aProximityFuncFine)) {
      SetErrorCode("shape proximity driver failed");
      return -1;
    }
  }
  catch (Standard_Failure& aFail) {
    SetErrorCode(aFail.GetMessageString());
    return -1;
  }

  aResultValue = aFineProximity.GetValue();
  aFineProximity.GetProximityPoints(aProxPnt1, aProxPnt2);

  //Make a Python command
  GEOM::TPythonDump(aProximityFuncCoarse, /*append=*/true) << "value = p.preciseProximity()";

  SetErrorCode(OK);
  return aResultValue;
}
