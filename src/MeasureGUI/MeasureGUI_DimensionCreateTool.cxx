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
// File   : MeasureGUI_DimensionCreateTool.cxx
// Author : Anton POLETAEV, Open CASCADE S.A.S.

#include "MeasureGUI_DimensionCreateTool.h"

// GEOM includes
#include <GEOMBase.h>

// GUI includes
#include <SalomeApp_Application.h>

// SUIT includes
#include <OCCViewer_ViewModel.h>
#include <OCCViewer_ViewManager.h>
#include <OCCViewer_ViewWindow.h>
#include <OCCViewer_ViewPort3d.h>

// OCCT includes
#include <Adaptor3d_CurveOnSurface.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepBndLib.hxx>
#include <ElCLib.hxx>
#include <gp_Pnt.hxx>
#include <gp_Circ.hxx>
#include <gp_Sphere.hxx>
#include <gp_Cone.hxx>
#include <gp_Torus.hxx>
#include <gce_MakeDir.hxx>
#include <gce_MakePln.hxx>
#include <gce_MakeCirc.hxx>
#include <GC_MakePlane.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Plane.hxx>
#include <Geom_ElementarySurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomLib.hxx>
#include <GeomLib_Tool.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TColgp_SequenceOfDir.hxx>
#include <V3d_View.hxx>


// plane associated with custom data
struct PlaneAndSegment
{
  PlaneAndSegment() {}
  PlaneAndSegment(const gp_Pln& thePlane, const MeasureGUI_DimensionCreateTool::Segment& theSegment) : pln(thePlane), seg(theSegment) {}
  operator gp_Pln () const { return pln; }
  operator MeasureGUI_DimensionCreateTool::Segment () const { return seg; }
  gp_Pln pln;
  MeasureGUI_DimensionCreateTool::Segment seg;
};

typedef NCollection_Sequence<PlaneAndSegment> SeqOfPlnsAndSegments;

//=================================================================================
// function : Constructor
// purpose  :
//=================================================================================
MeasureGUI_DimensionCreateTool::MeasureGUI_DimensionCreateTool()
{
  Settings.DefaultFlyout = 0.0;
  Settings.ActiveView = NULL;
}

//=================================================================================
// function : LengthOnEdge
// purpose  :
//=================================================================================
Handle(AIS_LengthDimension) MeasureGUI_DimensionCreateTool::LengthOnEdge( const GEOM::GeomObjPtr& theMeasuredObj ) const
{
  /* ---------------------------------------------------------------- *
   *                  get the edge and parent shape                   *
   * ---------------------------------------------------------------- */

  TopoDS_Shape aMeasuredShape;
  TopoDS_Shape aMainShape;
  if ( !GEOMBase::GetShape( theMeasuredObj.operator ->(), aMeasuredShape ) )
  {
    return NULL;
  }

  if ( !GEOMBase::GetShape( GetMainShape( theMeasuredObj ).get(), aMainShape ) )
  {
    return NULL;
  }

  /* ------------------------------------------------- */
  /*            check the input geometry               */
  /* ------------------------------------------------- */

  TopoDS_Edge anEdge = TopoDS::Edge( aMeasuredShape );

  TopoDS_Vertex aVertex1;
  TopoDS_Vertex aVertex2;
  TopExp::Vertices( anEdge, aVertex1, aVertex2 );

  gp_Pnt aPnt1 = BRep_Tool::Pnt( aVertex1 );
  gp_Pnt aPnt2 = BRep_Tool::Pnt( aVertex2 );
  if ( aPnt1.Distance( aPnt2 ) <= Precision::Confusion() )
  {
    return NULL;
  }

  /* ------------------------- *
   *   position the dimension 
   * ------------------------- */

  Bnd_Box aBnd;
  BRepBndLib::AddClose( aMainShape, aBnd );

  // get face sides
  TopTools_IndexedDataMapOfShapeListOfShape aRelationMap;
  TopExp::MapShapesAndAncestors( aMainShape, TopAbs_EDGE, TopAbs_FACE, aRelationMap );
  const TopTools_ListOfShape& aRelatedFaces = aRelationMap.FindFromKey( anEdge );

  gp_Vec aFaceN1( gp::Origin(), gp::Origin() );
  gp_Vec aFaceN2( gp::Origin(), gp::Origin() );
  gp_Vec aFaceS1( gp::Origin(), gp::Origin() );
  gp_Vec aFaceS2( gp::Origin(), gp::Origin() );

  gp_Pnt aMiddlePnt = gp_Pnt( ( aPnt1.XYZ() + aPnt2.XYZ() ) * 0.5 );

  TopTools_ListIteratorOfListOfShape aFaceIt( aRelatedFaces );

  // get face side directions
  if ( aFaceIt.More() )
  {
    TopoDS_Face aFace = TopoDS::Face( aFaceIt.Value() );

    gp_Dir aSideDir;
    if ( GetFaceSide( aFace, anEdge, aSideDir ) )
    {
      aFaceS1 = aSideDir;
    }

    Handle(Geom_Surface) aSurface = BRep_Tool::Surface( aFace );

    Standard_Real aU = 0.0, aV = 0.0;
    GeomLib_Tool::Parameters( aSurface, aMiddlePnt, Precision::Confusion(), aU, aV );

    gp_Dir aNorm;
    if ( GeomLib::NormEstim( aSurface, gp_Pnt2d( aU, aV ), Precision::Confusion(), aNorm ) <= 1 )
    {
      aFaceN1 = aFace.Orientation() == TopAbs_REVERSED ? -aNorm : aNorm;
    }

    aFaceIt.Next();
  }

  if ( aFaceIt.More() )
  {
    TopoDS_Face aFace = TopoDS::Face( aFaceIt.Value() );

    gp_Dir aSideDir;
    if ( GetFaceSide( aFace, anEdge, aSideDir ) )
    {
      aFaceS2 = aSideDir;
    }

    Handle(Geom_Surface) aSurface = BRep_Tool::Surface( aFace );

    Standard_Real aU = 0.0, aV = 0.0;
    GeomLib_Tool::Parameters( aSurface, aMiddlePnt, Precision::Confusion(), aU, aV );

    gp_Dir aNorm;
    if ( GeomLib::NormEstim( aSurface, gp_Pnt2d( aU, aV ), Precision::Confusion(), aNorm ) <= 1 )
    {
      aFaceN2 = aFace.Orientation() == TopAbs_REVERSED ? -aNorm : aNorm;
    }
  }

  gp_Pln aPln;
  PositionLength( aBnd, aFaceN1, aFaceN2, aFaceS1, aFaceS2, aPnt1, aPnt2, aPln );

  /* --------------------------------------------------------- *
   *   construct the dimension for the best selected position
   * --------------------------------------------------------- */

  Handle(AIS_LengthDimension) aDimension = new AIS_LengthDimension( anEdge, aPln );

  aDimension->SetFlyout( Settings.DefaultFlyout );

  if ( !aDimension->IsValid() )
  {
    return NULL;
  }

  return aDimension;
}

//=================================================================================
// function : LengthByPoints
// purpose  :
//=================================================================================
Handle(AIS_LengthDimension) MeasureGUI_DimensionCreateTool::LengthByPoints( const GEOM::GeomObjPtr& theMeasuredObj1,
                                                                            const GEOM::GeomObjPtr& theMeasuredObj2 ) const
{
  /* ---------------------------------------------------------------- *
   *                  get the edge and parent shape                   *
   * ---------------------------------------------------------------- */

  TopoDS_Shape aMeasuredShape1;
  TopoDS_Shape aMeasuredShape2;
  TopoDS_Shape aMainShape;

  if ( !GEOMBase::GetShape( theMeasuredObj1.operator ->(), aMeasuredShape1 ) )
  {
    return NULL;
  }

  if ( !GEOMBase::GetShape( theMeasuredObj2.operator ->(), aMeasuredShape2 ) )
  {
    return NULL;
  }

  if ( !GEOMBase::GetShape( GetMainShape( theMeasuredObj1 ).get(), aMainShape ) )
  {
    return NULL;
  }

  /* ------------------------------------------------- */
  /*            check the input geometry               */
  /* ------------------------------------------------- */

  TopoDS_Vertex aVertex1 = TopoDS::Vertex( aMeasuredShape1 );
  TopoDS_Vertex aVertex2 = TopoDS::Vertex( aMeasuredShape2 );

  gp_Pnt aPnt1 = BRep_Tool::Pnt( aVertex1 );
  gp_Pnt aPnt2 = BRep_Tool::Pnt( aVertex2 );
  if ( aPnt1.Distance( aPnt2 ) <= Precision::Confusion() )
  {
    return NULL;
  }

  /* ------------------------- *
   *   position the dimension 
   * ------------------------- */

  Bnd_Box aBnd;
  BRepBndLib::AddClose( aMainShape, aBnd );

  // check whether the points share same edge
  TopExp_Explorer anEdgeExp( aMainShape, TopAbs_EDGE, TopAbs_EDGE );
  for ( ; anEdgeExp.More(); anEdgeExp.Next() )
  {
    TopoDS_Vertex anEdgeV1;
    TopoDS_Vertex anEdgeV2;
    TopExp::Vertices( TopoDS::Edge( anEdgeExp.Current() ), anEdgeV1, anEdgeV2 );
    gp_Pnt anEdgePnt1 = BRep_Tool::Pnt( anEdgeV1 );
    gp_Pnt anEdgePnt2 = BRep_Tool::Pnt( anEdgeV2 );

    if ( aPnt1.Distance( anEdgePnt1 ) <= Precision::Confusion() )
    {
      if ( aPnt2.Distance( anEdgePnt2 ) <= Precision::Confusion() )
      {
        break;
      }
    }

    if ( aPnt2.Distance( anEdgePnt1 ) <= Precision::Confusion() )
    {
      if ( aPnt1.Distance( anEdgePnt2 ) <= Precision::Confusion() )
      {
        break;
      }
    }
  }

  gp_Vec aFaceN1( gp::Origin(), gp::Origin() );
  gp_Vec aFaceN2( gp::Origin(), gp::Origin() );
  gp_Vec aFaceS1( gp::Origin(), gp::Origin() );
  gp_Vec aFaceS2( gp::Origin(), gp::Origin() );

  // have shared edge
  if ( anEdgeExp.More() )
  {
    TopoDS_Edge anEdge = TopoDS::Edge( anEdgeExp.Current() );
    TopTools_IndexedDataMapOfShapeListOfShape aRelationMap;
    TopExp::MapShapesAndAncestors( aMainShape, TopAbs_EDGE, TopAbs_FACE, aRelationMap );
    const TopTools_ListOfShape& aRelatedFaces = aRelationMap.FindFromKey( anEdge );

    gp_Pnt aMiddlePnt = gp_Pnt( ( aPnt1.XYZ() + aPnt2.XYZ() ) * 0.5 );

    TopTools_ListIteratorOfListOfShape aFaceIt( aRelatedFaces );

    // get face side directions
    if ( aFaceIt.More() )
    {
      TopoDS_Face aFace = TopoDS::Face( aFaceIt.Value() );

      gp_Dir aSideDir;
      if ( GetFaceSide( aFace, anEdge, aSideDir ) )
      {
        aFaceS1 = aSideDir;
      }

      Handle(Geom_Surface) aSurface = BRep_Tool::Surface( aFace );

      Standard_Real aU = 0.0, aV = 0.0;
      GeomLib_Tool::Parameters( aSurface, aMiddlePnt, Precision::Confusion(), aU, aV );

      gp_Dir aNorm;
      if ( GeomLib::NormEstim( aSurface, gp_Pnt2d( aU, aV ), Precision::Confusion(), aNorm ) <= 1 )
      {
        aFaceN1 = aFace.Orientation() == TopAbs_REVERSED ? -aNorm : aNorm;
      }

      aFaceIt.Next();
    }

    if ( aFaceIt.More() )
    {
      TopoDS_Face aFace = TopoDS::Face( aFaceIt.Value() );

      gp_Dir aSideDir;
      if ( GetFaceSide( aFace, anEdge, aSideDir ) )
      {
        aFaceS2 = aSideDir;
      }

      Handle(Geom_Surface) aSurface = BRep_Tool::Surface( aFace );

      Standard_Real aU = 0.0, aV = 0.0;
      GeomLib_Tool::Parameters( aSurface, aMiddlePnt, Precision::Confusion(), aU, aV );

      gp_Dir aNorm;
      if ( GeomLib::NormEstim( aSurface, gp_Pnt2d( aU, aV ), Precision::Confusion(), aNorm ) <= 1 )
      {
        aFaceN2 = aFace.Orientation() == TopAbs_REVERSED ? -aNorm : aNorm;
      }
    }
  }

  gp_Pln aPln;
  PositionLength( aBnd, aFaceN1, aFaceN2, aFaceS1, aFaceS2, aPnt1, aPnt2, aPln );

  /* --------------------------------------------------------- *
   *   construct the dimension for the best selected position
   * --------------------------------------------------------- */

  Handle(AIS_LengthDimension) aDimension = new AIS_LengthDimension( aPnt1, aPnt2, aPln );

  aDimension->SetFlyout( Settings.DefaultFlyout );

  if ( !aDimension->IsValid() )
  {
    return NULL;
  }

  return aDimension;
}

//=================================================================================
// function : LengthByParallelEdges
// purpose  :
//=================================================================================
Handle(AIS_LengthDimension) MeasureGUI_DimensionCreateTool::LengthByParallelEdges( const GEOM::GeomObjPtr& theEdge1,
                                                                                   const GEOM::GeomObjPtr& theEdge2 ) const
{
  TopoDS_Shape aFirstSh;
  if ( !GEOMBase::GetShape( theEdge1.operator ->(), aFirstSh ) )
  {
    return NULL;
  }

  TopoDS_Shape aSecondSh;
  if ( !GEOMBase::GetShape( theEdge2.operator ->(), aSecondSh ) )
  {
    return NULL;
  }

  if( aFirstSh == aSecondSh )
    return NULL;

  TopoDS_Edge aFirstEdge  = TopoDS::Edge( aFirstSh );
  TopoDS_Edge aSecondEdge = TopoDS::Edge( aSecondSh );

  // Build plane through three points
  BRepAdaptor_Curve aCurve1( aFirstEdge );
  BRepAdaptor_Curve aCurve2( aSecondEdge );

  gp_Pnt aPnt1 = aCurve1.Value( 0.1 );
  gp_Pnt aPnt2 = aCurve1.Value( 0.9 );
  gp_Pnt aPnt3 = aCurve2.Value( 0.5 );

  GC_MakePlane aMkPlane( aPnt1, aPnt2, aPnt3 );
  Handle(Geom_Plane) aPlane = aMkPlane.Value();

  // check whether it is possible to compute valid dimension
  Handle(AIS_LengthDimension) aDimension = new AIS_LengthDimension ( aFirstEdge, aSecondEdge, aPlane->Pln() );

  aDimension->SetFlyout( Settings.DefaultFlyout );

  if ( !aDimension->IsValid() )
  {
    return NULL;
  }

  return aDimension;
}

//=================================================================================
// function : Diameter
// purpose  :
//=================================================================================
Handle(AIS_DiameterDimension) MeasureGUI_DimensionCreateTool::Diameter( const GEOM::GeomObjPtr& theMeasuredObj ) const
{
  /* ------------------------------------------------ *
   *     get the shape and its parent (if exist)      *
   * ------------------------------------------------ */

  TopoDS_Shape aMeasuredShape;
  TopoDS_Shape aMainShape;
  if ( !GEOMBase::GetShape( theMeasuredObj.operator ->(), aMeasuredShape ) )
  {
    return NULL;
  }

  if ( !GEOMBase::GetShape( GetMainShape( theMeasuredObj ).get(), aMainShape ) )
  {
    return NULL;
  }

  Bnd_Box aBnd;
  BRepBndLib::AddClose( aMainShape, aBnd );

  /* ------------------------------------------------ *
   *    get the dimension construction arguments      *
   * ------------------------------------------------ */

  Handle(Geom_Circle) aCircle;

  Standard_Real aPmin = 0, aPmax = 2 * M_PI;

  gp_Vec aFaceN( gp_Pnt(0.0, 0.0, 0.0), gp_Pnt(0.0, 0.0, 0.0) );

  switch ( aMeasuredShape.ShapeType() )
  {
    case TopAbs_FACE:
    {
      TopoDS_Face aMeasuredFace = TopoDS::Face(aMeasuredShape);

      BRepAdaptor_Surface aSurf( aMeasuredFace );

      Standard_Real aUmin = aSurf.FirstUParameter();
      Standard_Real aUmax = aSurf.LastUParameter();
      Standard_Real aVmin = aSurf.FirstVParameter();
      Standard_Real aVmax = aSurf.LastVParameter();

      // get arguments of sphere (the sphere should not be cutted at v-center)
      if ( aSurf.GetType() == GeomAbs_Sphere )
      {
        if ( aVmax <= Precision::PConfusion() || aVmin >= Precision::PConfusion() )
        {
          return NULL;
        }

        Handle(Geom_Surface) aBasisSurface = Handle(Geom_Surface)::DownCast(
          aSurf.Surface().Surface()->Transformed( aSurf.Trsf() ) );

        Handle(Geom_Curve) aCurve = aBasisSurface->VIso( 0.0 );

        if ( aCurve->IsKind( STANDARD_TYPE( Geom_Circle ) ) )
        {
          aPmin = aUmin;
          aPmax = aUmax;
          aCircle = Handle(Geom_Circle)::DownCast( aCurve );
        }
        else if (  aCurve->IsKind( STANDARD_TYPE( Geom_TrimmedCurve ) ) )
        {
          Handle(Geom_TrimmedCurve) aTrimmedCurve = Handle(Geom_TrimmedCurve)::DownCast( aCurve );
          aPmin = aTrimmedCurve->FirstParameter();
          aPmax = aTrimmedCurve->LastParameter();

          aCircle = Handle(Geom_Circle)::DownCast( aTrimmedCurve );
        }
        break;
      }

      // get arguments of cone
      if ( aSurf.GetType() == GeomAbs_Cone )
      {
        aPmin = aUmin;
        aPmax = aUmax;

        gp_Cone aCone = aSurf.Cone();
        gp_Ax2 anAx2 = aCone.Position().Ax2();
        aCircle = new Geom_Circle( anAx2, aCone.RefRadius() );

        aFaceN = aCone.SemiAngle() > 0.0 
          ?  anAx2.Axis().Direction()
          : -anAx2.Axis().Direction();
        break;
      }

      // get arguments of closed torus or cylinder
      if ( aSurf.GetType() == GeomAbs_Torus || aSurf.GetType() == GeomAbs_Cylinder )
      {
        Handle(Geom_Surface) aBasisSurface = Handle(Geom_Surface)::DownCast(
          aSurf.Surface().Surface()->Transformed( aSurf.Trsf() ) );

        Handle(Geom_Curve) aCurve = aBasisSurface->VIso( (aVmax + aVmin) * 0.5 );

        if ( aCurve->IsKind( STANDARD_TYPE( Geom_Circle ) ) )
        {
          aPmin = aUmin;
          aPmax = aUmax;
          aCircle = Handle(Geom_Circle)::DownCast( aCurve );
        }
        else if (  aCurve->IsKind( STANDARD_TYPE( Geom_TrimmedCurve ) ) )
        {
          Handle(Geom_TrimmedCurve) aTrimmedCurve = Handle(Geom_TrimmedCurve)::DownCast( aCurve );
          aPmin = aTrimmedCurve->FirstParameter();
          aPmax = aTrimmedCurve->LastParameter();

          aCircle = Handle(Geom_Circle)::DownCast( aTrimmedCurve );
        }

        break;
      }

      // face containing edge?
      TopExp_Explorer anExp( aMeasuredShape, TopAbs_EDGE );
      if ( !anExp.More() )
      {
        return NULL;
      }

      TopoDS_Shape anExpEdge = anExp.Current();
      if ( anExpEdge.IsNull() )
      {
        return NULL;
      }

      // only a single edge is expected
      anExp.Next();
      if ( anExp.More() )
      {
        return NULL;
      }

      // do not break, go to edge checking
      aMeasuredShape = anExpEdge;
    }

    case TopAbs_EDGE:
    {
      TopoDS_Edge aMeasureEdge = TopoDS::Edge( aMeasuredShape );

      BRepAdaptor_Curve aCurve(aMeasureEdge);

      if ( aCurve.GetType() != GeomAbs_Circle )
      {
        return NULL;
      }

      aPmin = aCurve.FirstParameter();
      aPmax = aCurve.LastParameter();

      aCircle = new Geom_Circle( aCurve.Circle() );

      // check if there is an parent face containing the edge
      TopTools_IndexedDataMapOfShapeListOfShape aShapeMap;
      TopExp::MapShapesAndAncestors( aMainShape, TopAbs_EDGE, TopAbs_FACE, aShapeMap );
      const TopTools_ListOfShape& aFaces = aShapeMap.FindFromKey( aMeasureEdge );

      TopTools_ListIteratorOfListOfShape aFaceIt( aFaces );
      for ( ; aFaceIt.More(); aFaceIt.Next() )
      {
        TopoDS_Face aFace = TopoDS::Face( aFaceIt.Value() );

        Handle(Geom_Surface) aSurface = BRep_Tool::Surface( TopoDS::Face( aFace ) );

        gp_Pnt aCircCenter = aCircle->Circ().Location();
        Standard_Real aCircU = 0.0, aCircV = 0.0;
        GeomLib_Tool::Parameters( aSurface, aCircCenter, Precision::Confusion(), aCircU, aCircV );

        gp_Dir aNorm;
        if ( GeomLib::NormEstim( aSurface, gp_Pnt2d( aCircU, aCircV ), Precision::Confusion(), aNorm ) > 1 )
        {
          break;
        }

        if ( aNorm.Angle( aCircle->Circ().Axis().Direction() ) > M_PI * 0.25 )
        {
          continue;
        }

        aFaceN = gp_Vec( aFace.Orientation() == TopAbs_REVERSED ? -aNorm : aNorm );
      }
    }
    break;
  }

  if ( aCircle.IsNull() )
  {
    return NULL;
  }

  ElCLib::AdjustPeriodic( 0.0, M_PI * 2, Precision::PConfusion(), aPmin, aPmax );

  /* ------------------------- *
   *   position the dimension 
   * ------------------------- */

  gp_Pnt aPnt1;
  gp_Pnt aPnt2;
  gp_Pln aPln;

  // diameter for closed circle
  if ( Abs( ( aPmax - aPmin ) - M_PI * 2 ) <= Precision::PConfusion() )
  {
    PositionDiameter( aBnd, aFaceN, aCircle->Circ(), aPnt1, aPnt2, aPln );
  }
  // diameter for half-closed circle
  else if ( Abs( aPmax - aPmin ) > M_PI )
  {
    Standard_Real anAnchor = aPmin + ( ( aPmax - aPmin ) - M_PI ) * 0.5;

    PositionDiameter( aBnd, aFaceN, aCircle->Circ(), anAnchor, aPln );

    aPnt1 = ElCLib::Value( anAnchor, aCircle->Circ() );
    aPnt2 = ElCLib::Value( anAnchor + M_PI, aCircle->Circ() );
  }
  // diameter for less than half-closed circle
  else
  {
    Standard_Real anAnchor = aPmin + ( aPmax - aPmin ) * 0.5;

    PositionDiameter( aBnd, aFaceN, aCircle->Circ(), anAnchor, aPln );

    aPnt1 = ElCLib::Value( anAnchor, aCircle->Circ() );
    aPnt2 = ElCLib::Value( anAnchor + M_PI, aCircle->Circ() );
  }

  /* --------------------------------------------------------- *
   *   construct the dimension for the best selected position
   * --------------------------------------------------------- */

  gp_Pnt aCircP = aCircle->Circ().Location();
  gp_Dir aCircN = aCircle->Circ().Axis().Direction();
  gp_Dir aCircX = gce_MakeDir( aPnt1, aPnt2 );
  Standard_Real aCircR = aCircle->Circ().Radius();

  // construct closed circle as base for the diameter dimension
  Standard_Boolean isReversed = ( ( aPln.Axis().Direction() ^ aCircX ) * aCircN ) < 0.0;

  gp_Circ aRuledCirc = gce_MakeCirc( gp_Ax2( aCircP, isReversed ? -aCircN : aCircN, aCircX ), aCircR );

  Handle(AIS_DiameterDimension) aDimension = new AIS_DiameterDimension( aRuledCirc, aPln );

  // if flyout is extended in tangent direction to circle, the default flyout value is used
  // if flyout is extended in plane of circle, the zero flyout value is choosen initially
  Standard_Real aFlyout = aCircN.IsParallel( aPln.Axis().Direction(), Precision::Angular() ) ? 0.0 : Settings.DefaultFlyout;

  aDimension->SetFlyout(aFlyout);

  if ( !aDimension->IsValid() )
  {
    return NULL;
  }

  return aDimension;
}

//=================================================================================
// function : AngleByTwoEdges
// purpose  :
//=================================================================================
Handle(AIS_AngleDimension) MeasureGUI_DimensionCreateTool::AngleByTwoEdges( const GEOM::GeomObjPtr& theEdge1,
                                                                            const GEOM::GeomObjPtr& theEdge2 ) const
{
  /* --------------------------------------------------- */
  /*         get construction and parent shapes          */
  /* --------------------------------------------------- */

  TopoDS_Shape aShapeEdge1;
  TopoDS_Shape aShapeMain1;
  if ( !GEOMBase::GetShape( theEdge1.get(), aShapeEdge1 ) )
  {
    return NULL;
  }
  if ( !GEOMBase::GetShape( GetMainShape( theEdge1 ).get(), aShapeMain1 ) )
  {
    return NULL;
  }

  TopoDS_Shape aShapeEdge2;
  TopoDS_Shape aShapeMain2;
  if ( !GEOMBase::GetShape( theEdge2.get(), aShapeEdge2 ) )
  {
    return NULL;
  }
  if ( !GEOMBase::GetShape( GetMainShape( theEdge2 ).get(), aShapeMain2 ) )
  {
    return NULL;
  }

  /* ---------------------------------------------------- */
  /*             check construction edges                 */
  /* ---------------------------------------------------- */

  TopoDS_Edge aFirstEdge  = TopoDS::Edge( aShapeEdge1 );
  TopoDS_Edge aSecondEdge = TopoDS::Edge( aShapeEdge2 );

  // check whether it is possible to compute dimension on the passed edges
  Handle(AIS_AngleDimension) aDimension = new AIS_AngleDimension( aFirstEdge, aSecondEdge );

  if ( !aDimension->IsValid() )
  {
    return NULL;
  }

  const gp_Pnt& aFirstPoint  = aDimension->FirstPoint();
  const gp_Pnt& aSecondPoint = aDimension->SecondPoint();
  const gp_Pnt& aCenterPoint = aDimension->CenterPoint();

  gp_Vec aVec1( aCenterPoint, aFirstPoint );
  gp_Vec aVec2( aCenterPoint, aSecondPoint );

  Standard_Real anAngle = aVec2.AngleWithRef( aVec1, aDimension->GetPlane().Axis().Direction() );

  if ( anAngle < 0.0 )
  {
    aDimension = new AIS_AngleDimension( aSecondPoint, aCenterPoint, aFirstPoint );
  }

  aDimension->SetFlyout( Settings.DefaultFlyout );

  return aDimension;
}

//=================================================================================
// function : AngleByThreePoints
// purpose  :
//=================================================================================
Handle(AIS_AngleDimension) MeasureGUI_DimensionCreateTool::AngleByThreePoints( const GEOM::GeomObjPtr& thePoint1,
                                                                               const GEOM::GeomObjPtr& thePoint2,
                                                                               const GEOM::GeomObjPtr& thePoint3 ) const
{
  TopoDS_Shape aFirstSh;
  if ( !GEOMBase::GetShape( thePoint1.operator ->(), aFirstSh ) )
  {
    return NULL;
  }

  TopoDS_Shape aSecondSh;
  if ( !GEOMBase::GetShape( thePoint2.operator ->(), aSecondSh ) )
  {
    return NULL;
  }

  TopoDS_Shape aThirdSh;
  if ( !GEOMBase::GetShape( thePoint3.operator ->(), aThirdSh ) )
  {
    return NULL;
  }

  TopoDS_Vertex aFirstVertex  = TopoDS::Vertex( aFirstSh );
  TopoDS_Vertex aSecondVertex = TopoDS::Vertex( aSecondSh );
  TopoDS_Vertex aThirdVertex  = TopoDS::Vertex( aThirdSh );

  gp_Pnt aPnt1 = BRep_Tool::Pnt( aFirstVertex );
  gp_Pnt aPnt2 = BRep_Tool::Pnt( aSecondVertex );
  gp_Pnt aPnt3 = BRep_Tool::Pnt( aThirdVertex );

  // check whether it is possible to compute dimension on the passed points
  Handle(AIS_AngleDimension) aDimension = new AIS_AngleDimension( aPnt1, aPnt2, aPnt3 );

  if ( !aDimension->IsValid() )
  {
    return NULL;
  }

  aDimension->SetFlyout( Settings.DefaultFlyout );

  return aDimension;
}

//=================================================================================
// function : PositionLength
// purpose  : The method provides preliminary positioning algorithm for
//            for length dimensions measuring the length between two points.
//            Parameters:
//              theBnd [in] - the bounding box of the main shape
//              theFaceN1 [in] - the normal to a first face of edge length (if any)
//              theFaceN2 [in] - the normal to a second face of edge length (if any)
//              theFaceS1 [in] - the side vector from a first face of edge length (if any)
//              theFaceS2 [in] - the side vector from a second face of edge length (if any)
//              thePnt1 [in] - the first measured point
//              thePnt2 [in] - the last measured point
//            The method selects flyout plane to best match the current
//            view projection. If edge length is constructed, then the flyout
//            can go in line with sides of faces, normal to the faces, or
//            aligned to XOY, YOZ, ZOX planes.
//=================================================================================
void MeasureGUI_DimensionCreateTool::PositionLength( const Bnd_Box& theBnd,
                                                     const gp_Vec& theFaceN1,
                                                     const gp_Vec& theFaceN2,
                                                     const gp_Vec& theFaceS1,
                                                     const gp_Vec& theFaceS2,
                                                     const gp_Pnt& thePnt1,
                                                     const gp_Pnt& thePnt2,
                                                     gp_Pln& thePln ) const
{
  Standard_Boolean isFace1 = theFaceN1.Magnitude() > Precision::Confusion();
  Standard_Boolean isFace2 = theFaceN2.Magnitude() > Precision::Confusion();
  gp_Vec anAverageN( gp_Pnt(0.0, 0.0, 0.0), gp_Pnt(0.0, 0.0, 0.0) );

  // get average direction in case of two non-sharp angled faces
  if ( isFace1 && isFace2 )
  {
    Standard_Boolean isSame = theFaceN1.IsParallel( theFaceN2, Precision::Angular() );
    if ( !isSame )
    {
      gp_Dir aReferenceDir = theFaceN1 ^ theFaceN2;
      // compute angle between face sides [0 - 2PI]
      Standard_Real aDirAngle = theFaceN1.AngleWithRef( theFaceN2, aReferenceDir );
      if ( aDirAngle < 0 )
      {
        aDirAngle = ( M_PI * 2.0 ) - aDirAngle;
      }

      // non-sharp angle, use averaged directio
      if ( aDirAngle > M_PI * 0.5 )
      {
        anAverageN = theFaceN1 + theFaceN2;
      }

      if ( aDirAngle > M_PI )
      {
        isFace1 = Standard_False;
        isFace2 = Standard_False;
      }
    }
  }

  Standard_Boolean isAverage = anAverageN.Magnitude() > Precision::Confusion();

  SeqOfDirs aFlyoutDirs;
  if ( isFace1 )
  {
    aFlyoutDirs.Append( theFaceN1 );
    aFlyoutDirs.Append( theFaceS1 );
  }
  if ( isFace2 )
  {
    aFlyoutDirs.Append( theFaceN2 );
    aFlyoutDirs.Append( theFaceS2 );
  }
  if ( isAverage )
  {
    aFlyoutDirs.Append( anAverageN );
  }

  ChooseLengthFlyoutsFromBnd( aFlyoutDirs, thePnt1, thePnt2, theBnd );

  if ( aFlyoutDirs.IsEmpty() )
  {
    return;
  }

  gp_Dir aPointDir = gce_MakeDir( thePnt1, thePnt2 );

  // make planes for dimension presentation according to flyout directions
  SeqOfPlanes aSeqOfPlanes;
  for ( Standard_Integer aFlyoutIt = 1; aFlyoutIt <= aFlyoutDirs.Length(); ++aFlyoutIt )
  {
    gp_Pln aPlane( thePnt1, aPointDir ^ aFlyoutDirs.Value( aFlyoutIt ) );
    aSeqOfPlanes.Append( aPlane );
  }

  Handle(V3d_View) aView = Settings.ActiveView;

  thePln = !aView.IsNull()
    ? SelectPlaneForProjection( aSeqOfPlanes, aView )
    : aSeqOfPlanes.First();
}

//=================================================================================
// function : PositionDiameter
// purpose  : The method provides preliminary positioning algorithm for
//            for diameter dimensions measuring the circle.
//            Parameters:
//              theBnd [in] - the bounding box of the shape
//              theFaceN [in] - the circle face normal (can be void)
//              theCirc [in] - the measured circle
//              thePnt1 [out] - first dimension point
//              thePnt2 [out] - second dimension point
//              thePln [out] - dimension flyout plane
//            The method selects points on the circle for diameter dimension and
//            flyout plane to best match the current view projection (if any)
//            The points are aligned to XOY, YOZ, ZOX planes.
//            The flyout takes into account bounding box of main shape of face normal
//            vector. The flyouts tangetial to the circle plane are directed in 
//            accordance with the face normal (if not-null), otherwise the flyouts
//            are turned to direct to the closest border of bounding box.
//=================================================================================
void MeasureGUI_DimensionCreateTool::PositionDiameter( const Bnd_Box& theBnd,
                                                       const gp_Vec& theFaceN,
                                                       const gp_Circ& theCirc,
                                                       gp_Pnt& thePnt1,
                                                       gp_Pnt& thePnt2,
                                                       gp_Pln& thePln ) const
{
  // select list of measured segments aligned to projection planes
  SeqOfDirs aProjectionDirs;
  aProjectionDirs.Append( gp::DX() );
  aProjectionDirs.Append( gp::DY() );
  aProjectionDirs.Append( gp::DZ() );

  SeqOfSegments aMeasureSegments = GetInPlaneSegments( theCirc, aProjectionDirs );

  SeqOfPlnsAndSegments aSelectedPlanes;

  // select in-circle-plane direction for flyout closest to border of bounding box
  for ( Standard_Integer aSegmentIt = 1; aSegmentIt <= aMeasureSegments.Length(); ++aSegmentIt )
  {
    const Segment& aSegment = aMeasureSegments.Value(aSegmentIt);

    Standard_Real anAnchor = ElCLib::Parameter( theCirc, aSegment.First );

    gp_Pln aSelectedPlane;

    PositionDiameter( theBnd, theFaceN, theCirc, anAnchor, aSelectedPlane );

    aSelectedPlanes.Append( PlaneAndSegment( aSelectedPlane, aSegment ) );
  }

  Handle(V3d_View) aView = Settings.ActiveView;

  PlaneAndSegment aChoosenParams = !aView.IsNull()
    ? SelectPlaneForProjection( aSelectedPlanes, aView )
    : aSelectedPlanes.First();

  thePnt1 = ((Segment)aChoosenParams).First;
  thePnt2 = ((Segment)aChoosenParams).Last;
  thePln  = ((gp_Pln)aChoosenParams);
}

//=================================================================================
// function : PositionDiameter
// purpose  : The method provides preliminary positioning algorithm for
//            for diameter dimensions measuring the circle. The diameter
//            dimension is bound at anchor point on the circle.
//            Parameters:
//              theBnd [in] the bounding box of the shape
//              theFaceN [in] - the circle face normal (can be void)
//              theCirc [in] - the measured circle
//              theAnchorAt [in] - the anchoring parameter
//              thePln [out] - dimension flyout plane
//            The method selects flyout plane to best match the current
//            view projection. The flyout plane can be parallel to circle,
//            or tangent to it.
//=================================================================================
void MeasureGUI_DimensionCreateTool::PositionDiameter( const Bnd_Box& theBnd,
                                                       const gp_Vec& theFaceN,
                                                       const gp_Circ& theCirc,
                                                       const Standard_Real& theAnchorAt,
                                                       gp_Pln& thePln ) const
{
  gp_Dir aCircN = theCirc.Axis().Direction();
  gp_Pnt aCircP = theCirc.Location();

  // select tangent direction for flyout closest to border of bounding box
  gp_Dir aSelectedTanDir;
  if ( theFaceN.Magnitude() < Precision::Confusion() )
  {
    SeqOfDirs aTangentDirs;
    aTangentDirs.Append(  aCircN );
    aTangentDirs.Append( -aCircN );
    aSelectedTanDir = ChooseDirFromBnd( aTangentDirs, aCircP, theBnd );
  }
  else
  {
    aSelectedTanDir = gp_Dir( theFaceN );
  }

  gp_Pnt aPnt1 = ElCLib::Value( theAnchorAt, theCirc );
  gp_Pnt aPnt2 = ElCLib::Value( theAnchorAt + M_PI, theCirc );

  gp_Dir aSegmentDir = gce_MakeDir( aPnt1, aPnt2 );

  SeqOfDirs aSegmentDirs;
  aSegmentDirs.Append(  aCircN ^ aSegmentDir );
  aSegmentDirs.Append( -aCircN ^ aSegmentDir );
  gp_Dir aSelectedSegDir = ChooseDirFromBnd( aSegmentDirs, aCircP, theBnd );

  gp_Pln aTangentFlyout( aCircP, aSegmentDir ^ aSelectedTanDir );
  gp_Pln aCoplanarFlyout( aCircP, aSegmentDir ^ aSelectedSegDir );

  SeqOfPlanes aSelectedPlanes;
  aSelectedPlanes.Append( aTangentFlyout );
  aSelectedPlanes.Append( aCoplanarFlyout );

  Handle(V3d_View) aView = Settings.ActiveView;

  thePln = !aView.IsNull()
    ? SelectPlaneForProjection( aSelectedPlanes, aView )
    : aSelectedPlanes.First();
}

//=================================================================================
// function : ChooseLengthFlyoutsFromBnd
// purpose  :
//=================================================================================
void MeasureGUI_DimensionCreateTool::ChooseLengthFlyoutsFromBnd( SeqOfDirs& theDirs,
                                                                 const gp_Pnt& thePnt1,
                                                                 const gp_Pnt& thePnt2,
                                                                 const Bnd_Box& theBnd ) const
{
  // compose a list of axis-aligned planes for lying-in flyouts
  NCollection_Sequence<gp_Pln> anAAPlanes;

  // the axis-aligned planes for flyouts are built from
  // three points (P1, P2, and P1 translated in orthogonal
  // direction dx, dy, dz)
  gp_Dir anAxes[3] = { gp::DX(), gp::DY(), gp::DZ() };

  for ( int anIt = 0; anIt < 3; ++anIt )
  {
    const gp_Dir& anAxisDir = anAxes[anIt];
    gp_Pnt aPnt3 = thePnt1.Translated( gp_Vec( anAxisDir ) );
    gce_MakePln aMakePlane( thePnt1, thePnt2, aPnt3 );
    if ( !aMakePlane.IsDone() )
    {
      continue;
    }

    anAAPlanes.Append( aMakePlane.Value() );
  }

  // find out what is the closest direction outside of the bounding box
  NCollection_Sequence<gp_Pln>::Iterator aPlaneIt( anAAPlanes );

  gp_Dir aPointDir = gce_MakeDir( thePnt1, thePnt2 );

  for ( ; aPlaneIt.More(); aPlaneIt.Next() )
  {
    const gp_Pln& aPlane = aPlaneIt.Value();

    // transform bounding box to orthogonal coordiantes relative to
    // dimension points P1, P2 (x-axis) and plane direction (z-axis),
    // where y coordinates will correspond to flyout direction against
    // the dimension point line
    gp_Ax3 aFlyoutSpace( thePnt1, aPlane.Axis().Direction(), aPointDir );

    gp_Trsf aRelativeTransform;
    aRelativeTransform.SetTransformation( gp_Ax3(), aFlyoutSpace );
    Bnd_Box aRelativeBounds = theBnd.Transformed( aRelativeTransform );

    Standard_Real aXmin, aXmax, aYmin, aYmax, aZmin, aZmax;
    aRelativeBounds.Get( aXmin, aYmin, aZmin, aXmax, aYmax, aZmax );

    gp_Dir aPosFlyout = aPlane.Axis().Direction() ^ aPointDir;
    gp_Dir aNegFlyout = aPosFlyout.Reversed();

    // select positive or negative flyout
    theDirs.Append( Abs( aYmax ) < Abs( aYmin ) ? aPosFlyout : aNegFlyout );
  }
}

//=================================================================================
// function : ChooseDirFromBnd
// purpose  : The method chooses the best direction from the passed list of
//            directions, which is closest to the bounding box border.
//            Parameters:
//              theCandidates [in] the list of candidate directions
//              thePos [in] the position from where the directions are traced
//              theBnd [in] the bounding box of main shape
//=================================================================================
gp_Dir MeasureGUI_DimensionCreateTool::ChooseDirFromBnd( const SeqOfDirs& theCandidates,
                                                         const gp_Pnt& thePos,
                                                         const Bnd_Box& theBnd ) const
{
  gp_Dir aBestDir;

  Standard_Real aBestDistance = RealLast();

  SeqOfDirs::Iterator anIt( theCandidates );
  for ( ; anIt.More(); anIt.Next() )
  {
    const gp_Dir& aDir = anIt.Value();

    gp_Ax3 aFlyoutSpace( thePos, aDir );

    gp_Trsf aRelativeTransform;
    aRelativeTransform.SetTransformation( gp_Ax3(), aFlyoutSpace );
    Bnd_Box aRelativeBounds = theBnd.Transformed( aRelativeTransform );

    Standard_Real aXmin, aXmax, aYmin, aYmax, aZmin, aZmax;
    aRelativeBounds.Get( aXmin, aYmin, aZmin, aXmax, aYmax, aZmax );

    if ( aYmax < aBestDistance )
    {
      aBestDir = aDir;
      aBestDistance = aYmax;
    }
  }

  return aBestDir;
}

//=================================================================================
// function : SelectPlaneForProjection
// purpose  : Select best matching plane in current view projection
//=================================================================================
template <typename TPlane>
TPlane MeasureGUI_DimensionCreateTool::SelectPlaneForProjection( const NCollection_Sequence<TPlane>& thePlanes,
                                                                 const Handle(V3d_View)& theView ) const
{
  Quantity_Parameter U[3];
  Quantity_Parameter N[3];
  theView->Up( U[0], U[1], U[2] );
  theView->Proj( N[0], N[1], N[2] );

  gp_Dir aViewN( (Standard_Real)N[0], (Standard_Real)N[1], (Standard_Real)N[2] );
  gp_Dir aViewU( (Standard_Real)U[0], (Standard_Real)U[1], (Standard_Real)U[2] );

  TPlane aBestPlane = thePlanes.First();

  Standard_Real aBestDotProduct = RealFirst();

  for ( Standard_Integer aPlnIt = 1; aPlnIt <= thePlanes.Length(); ++aPlnIt )
  {
    const TPlane& aPlane = thePlanes.Value( aPlnIt );

    Standard_Real aDotProduct = Abs( ((gp_Pln)aPlane).Axis().Direction() * aViewN );

    // preferred plane is "view parallel"
    if ( aDotProduct <= aBestDotProduct )
    {
      continue;
    }

    aBestPlane = aPlane;

    aBestDotProduct = aDotProduct;
  }

  return aBestPlane;
}

//=================================================================================
// function : GetMainShape
// purpose  :
//=================================================================================
GEOM::GeomObjPtr MeasureGUI_DimensionCreateTool::GetMainShape( const GEOM::GeomObjPtr& theShape ) const
{
  // iterate over top-level objects to search for main shape
  GEOM::GeomObjPtr aMainShapeIt = theShape;
  while ( !aMainShapeIt->IsMainShape() )
  {
    aMainShapeIt = aMainShapeIt->GetMainShape();
  }
  return aMainShapeIt;
}

//=================================================================================
// function : GetFaceSide
// purpose  :
//=================================================================================
bool MeasureGUI_DimensionCreateTool::GetFaceSide( const TopoDS_Face& theFace, const TopoDS_Edge& theEdge, gp_Dir& theDir ) const
{
  // get correctly oriented edge from main shape
  TopoDS_Edge anEdgeFromFace;
  TopExp_Explorer anExplorer( theFace.Oriented( TopAbs_FORWARD ), TopAbs_EDGE );
  for ( ; anExplorer.More(); anExplorer.Next() )
  {
    TopoDS_Edge aCurrentEdge = TopoDS::Edge( anExplorer.Current() );
    if ( theEdge.IsSame( aCurrentEdge ) )
    {
      anEdgeFromFace = aCurrentEdge;
      break;
    }
  }

  if ( anEdgeFromFace.IsNull() )
  {
    return false;
  }

  // check out the direction of face extensions from its boundaries at the edge location
  // made assumption here that for any linear bounding edge the
  // normals are same on the whole length of that edge
  Handle(Geom_Surface) aSurface = BRep_Tool::Surface( theFace );
  if ( aSurface.IsNull() || !aSurface->IsKind( STANDARD_TYPE(Geom_ElementarySurface) ) )
  {
    return false;
  }

  BRepAdaptor_Curve aSurfCurve( anEdgeFromFace, theFace );
  if ( !aSurfCurve.IsCurveOnSurface() )
  {
    return false;
  }

  Standard_Real aHalfRange = ( aSurfCurve.FirstParameter() + aSurfCurve.LastParameter() ) / 2.0;

  gp_Pnt aPoint = aSurfCurve.Value( aHalfRange );

  Standard_Real aPointU = 0.0;
  Standard_Real aPointV = 0.0;
  GeomLib_Tool::Parameters( aSurface, aPoint, Precision::Confusion(), aPointU, aPointV );

  gp_Dir aNorm;
  if ( GeomLib::NormEstim( aSurface, gp_Pnt2d( aPointU, aPointV ), Precision::Confusion(), aNorm ) > 1 )
  {
    return false;
  }

  gp_Vec aTangent = aSurfCurve.DN( aHalfRange, 1 );
  if ( aTangent.Magnitude() < Precision::Confusion() )
  {
    return false;
  }

  TopAbs_Orientation anEdgeOrientation = anEdgeFromFace.Orientation();
  if ( anEdgeOrientation == TopAbs_REVERSED )
  {
    aTangent.Reverse();
  }

  theDir = gp_Dir( aTangent ) ^ aNorm;
  return true;
}

//=================================================================================
// function : GetInPlaneSegments
// purpose  : The method finds segments crossing the passed circle,
//            which lie in the passed planes.
//            Parameters:
//              theCirc [in] the circle to be crossed.
//              thePlanes [in] the projection planes crossing the circle.
//=================================================================================
MeasureGUI_DimensionCreateTool::SeqOfSegments
  MeasureGUI_DimensionCreateTool::GetInPlaneSegments( const gp_Circ& theCirc,
                                                      const SeqOfDirs& thePlanes ) const
{
  SeqOfSegments aResult;

  gp_Pnt aCircP = theCirc.Location();
  gp_Dir aCircN = theCirc.Axis().Direction();
  Standard_Real aCircR = theCirc.Radius();

  SeqOfDirs::Iterator anIt( thePlanes );
  for ( ; anIt.More(); anIt.Next() )
  {
    const gp_Dir& aDir = anIt.Value();

    if ( aDir.IsParallel( aCircN, Precision::Angular() ) )
    {
      continue;
    }

    gp_Dir aIntDir = aDir ^ aCircN;

    gp_Pnt aPnt1 = gp_Pnt( aCircP.XYZ() - aIntDir.XYZ() * aCircR );
    gp_Pnt aPnt2 = gp_Pnt( aCircP.XYZ() + aIntDir.XYZ() * aCircR );
    Segment aSegment;
    aSegment.First = aPnt1;
    aSegment.Last  = aPnt2;
    aResult.Append( aSegment );
  }

  return aResult;
}
