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
#include <BRepAdaptor_Curve.hxx>
#include <BRepBndLib.hxx>
#include <gce_MakeDir.hxx>
#include <gce_MakePln.hxx>
#include <GC_MakePlane.hxx>
#include <Geom_Plane.hxx>
#include <Geom_ElementarySurface.hxx>
#include <Geom_Surface.hxx>
#include <GeomLib.hxx>
#include <GeomLib_Tool.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TColgp_SequenceOfDir.hxx>
#include <gp_Pnt.hxx>
#include <V3d_View.hxx>

//=================================================================================
// function : Constructor
// purpose  :
//=================================================================================
MeasureGUI_DimensionCreateTool::MeasureGUI_DimensionCreateTool( GeometryGUI* theGeomGUI )
: myGeomGUI( theGeomGUI )
{
}

//=================================================================================
// function : LengthOnEdge
// purpose  :
//=================================================================================
Handle(AIS_LengthDimension) MeasureGUI_DimensionCreateTool::LengthOnEdge( const GEOM::GeomObjPtr& theEdge )
{
  /* ---------------------------------------------------------------- *
   *                  get the edge and parent shape                   *
   * ---------------------------------------------------------------- */

  TopoDS_Shape aShapeEdge;
  TopoDS_Shape aShapeMain;
  if ( !GEOMBase::GetShape( theEdge.get(), aShapeEdge ) )
  {
    return NULL;
  }
  if ( !GEOMBase::GetShape( GetMainShape( theEdge ).get(), aShapeMain ) )
  {
    return NULL;
  }

  /* ------------------------------------------------- */
  /*            check the input geometry               */
  /* ------------------------------------------------- */

  TopoDS_Edge anEdge = TopoDS::Edge( aShapeEdge );

  TopoDS_Vertex aVertex1;
  TopoDS_Vertex aVertex2;
  TopExp::Vertices( anEdge, aVertex1, aVertex2 );

  gp_Pnt aPnt1 = BRep_Tool::Pnt( aVertex1 );
  gp_Pnt aPnt2 = BRep_Tool::Pnt( aVertex2 );
  if ( aPnt1.Distance( aPnt2 ) <= Precision::Confusion() )
  {
    return NULL;
  }

  /* ------------------------------------------------- *
   *   compose list of possible flyout directions      *
   * ------------------------------------------------- */

  Bnd_Box aBnd;
  BRepBndLib::AddClose( aShapeMain, aBnd );

  TColgp_SequenceOfDir aSeqOfFlyout;
  ChooseLengthFlyoutsFromShape( aSeqOfFlyout, anEdge, aShapeMain );
  ChooseLengthFlyoutsFromBnd( aSeqOfFlyout, aPnt1, aPnt2, aBnd );
  if ( aSeqOfFlyout.IsEmpty() )
  {
    return NULL;
  }

  gp_Dir aPointDir = gce_MakeDir( aPnt1, aPnt2 );

  // make planes for dimension presentation according to flyout directions
  NCollection_Sequence<gp_Pln> aSeqOfPlanes;
  for ( Standard_Integer aFlyoutIt = 1; aFlyoutIt <= aSeqOfFlyout.Length(); ++aFlyoutIt )
  {
    gp_Pln aPlane( aPnt1, aPointDir ^ aSeqOfFlyout.Value( aFlyoutIt ) );
    aSeqOfPlanes.Append( aPlane );
  }

  /* --------------------------------------------------------------------- *
   *     select best matching dimension plane for view projection          *
   * --------------------------------------------------------------------- */

  OCCViewer_ViewWindow* anActiveView = NULL;

  if ( myGeomGUI != NULL )
  {
    SalomeApp_Application* anApp = myGeomGUI->getApp();
    if ( !anApp )
    {
      OCCViewer_ViewManager* aViewMgr = (OCCViewer_ViewManager*) anApp->getViewManager( OCCViewer_Viewer::Type(), false );
      if ( aViewMgr )
      {
        anActiveView = (OCCViewer_ViewWindow*)  aViewMgr->getActiveView();
      }
    }
  }

  gp_Pln aChoosenPlane = anActiveView
    ? SelectPlaneForProjection( aSeqOfPlanes, anActiveView->getViewPort()->getView() )
    : aSeqOfPlanes.First();

  /* ------------------------------------------------------------------------------------ *
   *                        construct interactive presentation                            *
   * ------------------------------------------------------------------------------------ */

  Handle(AIS_LengthDimension) aDimension = new AIS_LengthDimension( anEdge, aChoosenPlane );
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
Handle(AIS_LengthDimension) MeasureGUI_DimensionCreateTool::LengthByPoints( const GEOM::GeomObjPtr& thePoint1,
                                                                            const GEOM::GeomObjPtr& thePoint2 )
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

  TopoDS_Vertex aFirstVertex  = TopoDS::Vertex( aFirstSh );
  TopoDS_Vertex aSecondVertex = TopoDS::Vertex( aSecondSh );

  gp_Pnt aPnt1 = BRep_Tool::Pnt( aFirstVertex );
  gp_Pnt aPnt2 = BRep_Tool::Pnt( aSecondVertex );

  gp_Vec aDir( aPnt1, aPnt2 );
  gp_Dir aUnitVecs[] = { gp::DZ(), gp::DY(), gp::DX() };
  int aUnitVecIt = 0;
  for ( ; aUnitVecIt < 3; ++aUnitVecIt )
  {
    if ( aDir.Dot( aUnitVecs[aUnitVecIt] ) <= 0.5 )
    {
      break;
    }
  }

  gp_Pnt aPnt3 = aPnt2.Translated( aUnitVecs[aUnitVecIt] );

  GC_MakePlane aMkPlane( aPnt1, aPnt2, aPnt3 );
  Handle(Geom_Plane) aPlane = aMkPlane.Value();

  // check whether it is possible to compute valid dimension
  Handle(AIS_LengthDimension) aDimension = new AIS_LengthDimension ( aFirstVertex, aSecondVertex, aPlane->Pln() );

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
                                                                                   const GEOM::GeomObjPtr& theEdge2 )
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
Handle(AIS_DiameterDimension) MeasureGUI_DimensionCreateTool::Diameter( const GEOM::GeomObjPtr& theShape )
{
  TopoDS_Shape aShape;
  if ( !GEOMBase::GetShape( theShape.operator ->(), aShape ) )
  {
    return NULL;
  }

  if ( aShape.ShapeType() != TopAbs_EDGE &&
       aShape.ShapeType() != TopAbs_FACE &&
       aShape.ShapeType() != TopAbs_WIRE )
  {
    return NULL;
  }

  // check whether it is possible to compute dimension on the passed geometry
  Handle(AIS_DiameterDimension) aDimension = new AIS_DiameterDimension( aShape );

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
                                                                            const GEOM::GeomObjPtr& theEdge2 )
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

  return aDimension;
}

//=================================================================================
// function : AngleByThreePoints
// purpose  :
//=================================================================================
Handle(AIS_AngleDimension) MeasureGUI_DimensionCreateTool::AngleByThreePoints( const GEOM::GeomObjPtr& thePoint1,
                                                                               const GEOM::GeomObjPtr& thePoint2,
                                                                               const GEOM::GeomObjPtr& thePoint3 )
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

  return aDimension;
}

//=================================================================================
// function : ChooseLengthFlyoutsFromShape
// purpose  :
//=================================================================================
void MeasureGUI_DimensionCreateTool::ChooseLengthFlyoutsFromShape( TColgp_SequenceOfDir& theDirs,
                                                                   const TopoDS_Vertex& theVertex1,
                                                                   const TopoDS_Vertex& theVertex2,
                                                                   const TopoDS_Shape& theShape )
{
}

//=================================================================================
// function : ChooseLengthFlyoutsFromShape
// purpose  :
//=================================================================================
void MeasureGUI_DimensionCreateTool::ChooseLengthFlyoutsFromShape( TColgp_SequenceOfDir& theDirs,
                                                                   const TopoDS_Edge& theEdge,
                                                                   const TopoDS_Shape& theShape )
{
  TopTools_IndexedDataMapOfShapeListOfShape aRelationMap;
  TopExp::MapShapesAndAncestors( theShape, TopAbs_EDGE, TopAbs_FACE, aRelationMap );
  const TopTools_ListOfShape& aRelatedFaces = aRelationMap.FindFromKey( theEdge );

  // get face side directions
  gp_Dir aSideDir;
  if ( aRelatedFaces.Extent() > 0 && GetFaceSide( TopoDS::Face( aRelatedFaces.First() ), theEdge, aSideDir ) )
  {
    theDirs.Append( aSideDir );
  }
  if ( aRelatedFaces.Extent() > 1 && GetFaceSide( TopoDS::Face( aRelatedFaces.Last() ), theEdge, aSideDir ) )
  {
    theDirs.Append( aSideDir );
  }

  // get average direction in case of two non-sharp angled faces
  if ( theDirs.Length() == 2 )
  {
    const gp_Dir& aDir1 = theDirs.First();
    const gp_Dir& aDir2 = theDirs.Last();
    Standard_Boolean isSame = aDir1.IsParallel( aDir2, Precision::Angular() );
    if ( !isSame )
    {
      gp_Dir aReferenceDir = aDir1 ^ aDir2;
      // compute angle between face sides [0 - 2PI]
      Standard_Real aDirAngle = aDir1.AngleWithRef( aDir2, aReferenceDir );
      if ( aDirAngle < 0 )
      {
        aDirAngle = ( M_PI * 2.0 ) - aDirAngle;
      }

      // non-sharp angle, use averaged direction
      if ( aDirAngle > M_PI )
      {
        theDirs.Clear();
        theDirs.Append( aDir1.Rotated( gp_Ax1( gp::Origin(), aReferenceDir ), aDirAngle * 0.5 ) );
      }
    }
  }
}

//=================================================================================
// function : ChooseLengthFlyoutsFromBnd
// purpose  :
//=================================================================================
void MeasureGUI_DimensionCreateTool::ChooseLengthFlyoutsFromBnd( TColgp_SequenceOfDir& theDirs,
                                                                 const gp_Pnt& thePnt1,
                                                                 const gp_Pnt& thePnt2,
                                                                 const Bnd_Box& theBnd )
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
// function : SelectPlaneForProjection
// purpose  : Select best matching plane in current view projection
//=================================================================================
gp_Pln MeasureGUI_DimensionCreateTool::SelectPlaneForProjection( const NCollection_Sequence<gp_Pln>& thePlanes,
                                                                 const Handle(V3d_View)& theView )
{
  Quantity_Parameter U[3];
  Quantity_Parameter N[3];
  theView->Up( U[0], U[1], U[2] );
  theView->Proj( N[0], N[1], N[2] );

  gp_Dir aViewN( (Standard_Real)N[0], (Standard_Real)N[1], (Standard_Real)N[2] );
  gp_Dir aViewU( (Standard_Real)U[0], (Standard_Real)U[1], (Standard_Real)U[2] );

  gp_Pln aBestPlane = thePlanes.First();

  Standard_Real aBestDotProduct = RealFirst();

  for ( Standard_Integer aPlnIt = 1; aPlnIt <= thePlanes.Length(); ++aPlnIt )
  {
    const gp_Pln& aPlane = thePlanes.Value( aPlnIt );

    Standard_Real aDotProduct = Abs( aPlane.Axis().Direction() * aViewN );

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
GEOM::GeomObjPtr MeasureGUI_DimensionCreateTool::GetMainShape( const GEOM::GeomObjPtr& theShape )
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
bool MeasureGUI_DimensionCreateTool::GetFaceSide( const TopoDS_Face& theFace, const TopoDS_Edge& theEdge, gp_Dir& theDir )
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
