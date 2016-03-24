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
// File   : MeasureGUI_DimensionInteractor.cxx
// Author : Anton POLETAEV, Open CASCADE S.A.S.

#include "MeasureGUI_DimensionInteractor.h"

#include <GEOM_Displayer.h>
#include <SalomeApp_Application.h>
#include <OCCViewer_ViewManager.h>
#include <OCCViewer_ViewWindow.h>
#include <OCCViewer_ViewPort3d.h>
#include <SUIT_ViewManager.h>
#include <SUIT_ViewWindow.h>
#include <SUIT_Desktop.h>

#include <QMouseEvent>

#include <AIS_InteractiveContext.hxx>
#include <AIS_LengthDimension.hxx>
#include <AIS_DiameterDimension.hxx>
#include <AIS_AngleDimension.hxx>
#include <Prs3d_DimensionAspect.hxx>
#include <Prs3d_TextAspect.hxx>
#include <gp_Dir.hxx>
#include <gp_Vec.hxx>
#include <gce_MakeDir.hxx>
#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <Extrema_ExtCC.hxx>
#include <Extrema_POnCurv.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAPI_IntCS.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <NCollection_Sequence.hxx>

//=================================================================================
// function : Constructor
// purpose  : 
//=================================================================================
MeasureGUI_DimensionInteractor::MeasureGUI_DimensionInteractor( GeometryGUI* theGUI,
                                                                QObject* theParent )
: QObject( theParent ),
  myGeomGUI( theGUI ),
  myIsEnabled( false ),
  myVM( NULL ),
  myViewer( NULL ),
  myOperation( Operation_None )
{
}

//=================================================================================
// function : Deactivate
// purpose  : 
//=================================================================================
MeasureGUI_DimensionInteractor::~MeasureGUI_DimensionInteractor()
{
  Disable();
}

//=================================================================================
// function : Enable
// purpose  : 
//=================================================================================
void MeasureGUI_DimensionInteractor::Enable()
{
  if ( myIsEnabled )
  {
    return;
  }

  myIsEnabled = true;

  // install event filtering on viewer windows
  SalomeApp_Application* anApp = myGeomGUI->getApp();
  if ( !anApp )
  {
    return;
  }

  myVM     = (OCCViewer_ViewManager*) anApp->getViewManager( OCCViewer_Viewer::Type(), false );
  myViewer = (OCCViewer_Viewer*) myVM->getViewModel();
  if ( !myVM || !myViewer )
  {
    return;
  }

  connect( myVM, SIGNAL( viewCreated( SUIT_ViewWindow* ) ), SLOT( OnViewCreated( SUIT_ViewWindow* ) ) );
  connect( myVM, SIGNAL( deleteView ( SUIT_ViewWindow* ) ), SLOT( OnViewRemoved( SUIT_ViewWindow* ) ) );

  QVector<SUIT_ViewWindow*>           aViews  = myVM->getViews();
  QVector<SUIT_ViewWindow*>::iterator aViewIt = aViews.begin();
  for ( ; aViewIt != aViews.end(); ++aViewIt )
  {
    ConnectView( *aViewIt );
  }
}

//=================================================================================
// function : Disable
// purpose  : 
//=================================================================================
void MeasureGUI_DimensionInteractor::Disable()
{
  if ( !myIsEnabled )
  {
    return;
  }

  myIsEnabled = false;

  // remove event filtering from viewer windows
  QVector<SUIT_ViewWindow*>           aViews  = myVM->getViews();
  QVector<SUIT_ViewWindow*>::iterator aViewIt = aViews.begin();
  for ( ; aViewIt != aViews.end(); ++aViewIt )
  {
    DisconnectView( *aViewIt );
  }
}

//=================================================================================
// function : GetOperation
// purpose  : 
//=================================================================================
MeasureGUI_DimensionInteractor::Operation
  MeasureGUI_DimensionInteractor::GetOperation( const Handle(SelectMgr_EntityOwner)& theEntity,
                                                const Qt::MouseButtons theButtons,
                                                const Qt::KeyboardModifiers theKeys )
{
  if ( ( theButtons & Qt::LeftButton ) == 0 )
  {
    return Operation_None;
  }

  Handle(AIS_DimensionOwner) anOwner = Handle(AIS_DimensionOwner)::DownCast( theEntity );
  if ( anOwner.IsNull() )
  {
    return Operation_None;
  }

  Standard_Real anAngTolerance = M_PI / 30.0; // 6 degree tolerance

  switch ( anOwner->SelectionMode() )
  {
    case AIS_DSM_Line :
    {
      if ( ( theKeys & Qt::ControlModifier ) == 0 )
      {
        return Operation_MoveFlyoutInPlane;
      }

      // "free moving" is only available for length and diameter
      if ( !myInteractedIO->IsKind( STANDARD_TYPE( AIS_LengthDimension ) )
        && !myInteractedIO->IsKind( STANDARD_TYPE( AIS_DiameterDimension ) ) )
      {
        return Operation_None;
      }

      return Operation_MoveFlyoutFree;
    }

    case AIS_DSM_Text : return Operation_MoveText;
    default :           return Operation_None;
  }
}

//=================================================================================
// function : StartOperation
// purpose  : 
//=================================================================================
bool MeasureGUI_DimensionInteractor::StartOperation( const Operation theOp,
                                                     const Handle(V3d_View)& theView,
                                                     const int theX,
                                                     const int theY )
{
  Standard_Real anAngTolerance = M_PI / 30.0; // 6 degree tolerance

  switch ( theOp )
  {
    case Operation_MoveFlyoutInPlane :
    case Operation_MoveText :
    {
      // operation can be started if the plane in which the flyout 
      // or text moved is not completely orthogonal to the screen
      gp_Pln aPlane = myInteractedIO->GetPlane();
      gp_Lin aProj  = Projection( theView, theX, theY );
      Standard_Real aCrossAng = aProj.Direction().Angle( aPlane.Axis().Direction() );
      return Abs( M_PI * 0.5 - aCrossAng ) > anAngTolerance;
    }

    case Operation_MoveFlyoutFree :
    {
      // operation can be started only for linear dimensions
      if ( !myInteractedIO->IsKind( STANDARD_TYPE( AIS_LengthDimension ) )
        && !myInteractedIO->IsKind( STANDARD_TYPE( AIS_DiameterDimension ) ) )
      {
        return false;
      }

      // determine whether the rotation operation is frontal or side
      gp_Pnt aFirstPoint;
      gp_Pnt aSecondPoint;

      // get first and second point from which the flyout line is normally extended
      Handle(AIS_LengthDimension) aLength = Handle(AIS_LengthDimension)::DownCast( myInteractedIO );
      if ( !aLength.IsNull() )
      {
        aFirstPoint  = aLength->FirstPoint();
        aSecondPoint = aLength->SecondPoint();
      }

      Handle(AIS_DiameterDimension) aDiameter = Handle(AIS_DiameterDimension)::DownCast( myInteractedIO );
      if ( !aDiameter.IsNull() )
      {
        const gp_Pnt& aCenter = aDiameter->Circle().Location();
        aFirstPoint    = aDiameter->AnchorPoint();
        aSecondPoint   = gp_Pnt( aFirstPoint.XYZ() + gp_Vec( aFirstPoint, aCenter ).XYZ() * 2.0 );
      }

      gp_Dir aBaseDir = gce_MakeDir( aFirstPoint, aSecondPoint );
      gp_Pln aPlane   = myInteractedIO->GetPlane();
      gp_Dir aPlaneN  = aPlane.Axis().Direction();
      gp_Lin aProj    = Projection( theView, theX, theY );

      const gp_Dir& aProjDir = aProj.Direction();

      // can turn only if looking from side
      if ( aProjDir.IsParallel( aPlaneN, anAngTolerance ) ||
           aProjDir.IsNormal( aBaseDir, anAngTolerance ) )
      {
        return false;
      }

      bool isProjected     = false;
      gp_Pnt aPointOnPlane = ProjectPlane( theView, theX, theY, aPlane, isProjected );
      if ( !isProjected )
      {
        return false;
      }

      gp_Vec aPointVector( aFirstPoint, aPointOnPlane );
      gp_Pnt aPointOnBase = gp_Pnt( aFirstPoint.XYZ() + aBaseDir.XYZ() * aPointVector.Dot( aBaseDir ) );
      myFreeMovePlane = gp_Pln( aPointOnBase, aBaseDir );

      return true;
    }

    default:
      return false;
  }
}

//=================================================================================
// function : MoveFlyoutFree
// purpose  : 
//=================================================================================
void MeasureGUI_DimensionInteractor::MoveFlyoutFree( const Handle(V3d_View)& theView,
                                                     const int theX,
                                                     const int theY )
{
  // project point onto dimension plane
  bool isProjected     = false;
  gp_Pln aPlane        = myFreeMovePlane;
  gp_Pnt aPointOnPlane = ProjectPlane( theView, theX, theY, aPlane, isProjected );
  if ( !isProjected )
  {
    return;
  }

  gp_Pnt aFirstPoint;
  gp_Pnt aSecondPoint;

  // get first and second point from which the flyout line is normally extended
  Handle(AIS_LengthDimension) aLength = Handle(AIS_LengthDimension)::DownCast( myInteractedIO );
  if ( !aLength.IsNull() )
  {
    aFirstPoint  = aLength->FirstPoint();
    aSecondPoint = aLength->SecondPoint();
  }

  Handle(AIS_DiameterDimension) aDiameter = Handle(AIS_DiameterDimension)::DownCast( myInteractedIO );
  if ( !aDiameter.IsNull() )
  {
    const gp_Pnt& aCenter = aDiameter->Circle().Location();
    aFirstPoint    = aDiameter->AnchorPoint();
    aSecondPoint   = gp_Pnt( aFirstPoint.XYZ() + gp_Vec( aFirstPoint, aCenter ).XYZ() * 2.0 );
  }

  gp_Vec aPointVector( aFirstPoint, aPointOnPlane );
  gp_Dir aBaseDir     = gce_MakeDir( aFirstPoint, aSecondPoint );
  gp_Pnt aPointOnBase = gp_Pnt( aFirstPoint.XYZ() + aBaseDir.XYZ() * aPointVector.Dot( aBaseDir ) );

  // snapping tolerance
  Quantity_Length aSize[2];
  theView->Size( aSize[0], aSize[1] );
  Standard_Real aSnapTolerance = 1e-2 * Max( aSize[0], aSize[1] );

  gp_Dir aFlyoutDir = gce_MakeDir( aPointOnBase, aPointOnPlane );

  // snapping to planes
  NCollection_Sequence<gp_Pln> aSnapPlanes;

  if ( aPointOnPlane.Distance( aPointOnBase ) > aSnapTolerance )
  {
    if ( !aBaseDir.IsParallel( gp::DZ(), Precision::Angular() ) )
    {
      aSnapPlanes.Append( gp_Pln( aFirstPoint, gp::DZ().Crossed( aBaseDir ) ) );
    }
    if ( !aBaseDir.IsParallel( gp::DX(), Precision::Angular() ) )
    {
      aSnapPlanes.Append( gp_Pln( aFirstPoint, gp::DX().Crossed( aBaseDir ) ) );
    }
    if ( !aBaseDir.IsParallel( gp::DY(), Precision::Angular() ) )
    {
      aSnapPlanes.Append( gp_Pln( aFirstPoint, gp::DY().Crossed( aBaseDir ) ) );
    }
  }

  // snap "drag proj" to Flyout plane, relative XOY, YOZ, ZOX planes
  NCollection_Sequence<gp_Pln>::Iterator aSnapIt( aSnapPlanes );

  for ( ; aSnapIt.More(); aSnapIt.Next() )
  {
    const gp_Pln& aSnapPln = aSnapIt.Value();

    // project direction into plane
    gp_Vec aPlaneDir = gp_Vec( aSnapPln.Axis().Direction() );
    gp_Vec aFlyoutProj = gp_Vec( aFlyoutDir ) - aPlaneDir * gp_Vec( aFlyoutDir ).Dot( aPlaneDir );

    // snapping is not applicable
    if ( aSnapPln.Contains( gp_Lin( aFirstPoint, aFlyoutDir ), Precision::Confusion(), Precision::Angular() ) )
    {
      continue;
    }
    if ( Abs( gp_Vec( aPointOnBase, aPointOnPlane ).Dot( aPlaneDir ) ) > aSnapTolerance )
    {
      continue;
    }
    if ( aFlyoutProj.Magnitude() <= Precision::Confusion() )
    {
      continue;
    }

    aFlyoutDir = gp_Dir( aFlyoutProj );

    break;
  }
  
  Standard_Real aNewFlyout = aPointOnPlane.Distance( aPointOnBase );

  if ( aNewFlyout <= Precision::Confusion() )
  {
    myInteractedIO->SetFlyout( 0.0 );
    myViewer->getAISContext()->Redisplay( myInteractedIO );
    return;
  }

  gp_Pln aNewPlane = gp_Pln( aFirstPoint, aBaseDir.Crossed( aFlyoutDir ) );

  myInteractedIO->SetFlyout( aNewFlyout );
  myInteractedIO->SetCustomPlane( aNewPlane );

  myViewer->getAISContext()->Redisplay( myInteractedIO );
}

//=================================================================================
// function : MoveFlyoutInPlane
// purpose  : 
//=================================================================================
void MeasureGUI_DimensionInteractor::MoveFlyoutInPlane( const Handle(V3d_View)& theView,
                                                        const int theX,
                                                        const int theY )
{
  // project point onto dimension plane
  bool isProjected     = false;
  gp_Pln aPlane        = myInteractedIO->GetPlane();
  gp_Pnt aPointOnPlane = ProjectPlane( theView, theX, theY, aPlane, isProjected );
  if ( !isProjected )
  {
    return;
  }

  const gp_Dir& aPlaneN = aPlane.Axis().Direction();

  // new flyout value
  Standard_Real aFlyout = 0.0;

  // extend flyout of linear dimension
  if ( myInteractedIO->IsKind( STANDARD_TYPE( AIS_LengthDimension ) )
    || myInteractedIO->IsKind( STANDARD_TYPE( AIS_DiameterDimension ) ) )
  {
    gp_Pnt aFirstPoint;
    gp_Pnt aSecondPoint;

    // get first and second point from which the flyout line is normally extended
    Handle(AIS_LengthDimension) aLength = Handle(AIS_LengthDimension)::DownCast( myInteractedIO );
    if ( !aLength.IsNull() )
    {
      aFirstPoint  = aLength->FirstPoint();
      aSecondPoint = aLength->SecondPoint();
    }

    Handle(AIS_DiameterDimension) aDiameter = Handle(AIS_DiameterDimension)::DownCast( myInteractedIO );
    if ( !aDiameter.IsNull() )
    {
      const gp_Pnt& aCenter = aDiameter->Circle().Location();
      aFirstPoint    = aDiameter->AnchorPoint();
      aSecondPoint   = gp_Pnt( aFirstPoint.XYZ() + gp_Vec( aFirstPoint, aCenter ).XYZ() * 2.0 );
    }

    gp_Lin aBaseLine( aFirstPoint, gce_MakeDir( aFirstPoint, aSecondPoint ) );

    // check if the negative value is expected
    gp_Dir aPositiveDir = aPlaneN.Crossed( aBaseLine.Direction() );
    gp_Dir aPointDir    = gce_MakeDir( aBaseLine.Location(), aPointOnPlane );
    Standard_Boolean isPositive = aPointDir.Dot( aPositiveDir ) > 0;

    aFlyout = isPositive 
      ?  aBaseLine.Distance( aPointOnPlane )
      : -aBaseLine.Distance( aPointOnPlane );
  }

  // exten flyout of angular dimension
  if ( myInteractedIO->IsKind( STANDARD_TYPE( AIS_AngleDimension ) ) )
  {
    Handle(AIS_AngleDimension) anAngle = Handle(AIS_AngleDimension)::DownCast( myInteractedIO );

    const gp_Pnt& aCenterPoint = anAngle->CenterPoint();
    const gp_Pnt& aFirstPoint  = anAngle->FirstPoint();
    const gp_Pnt& aSecondPoint = anAngle->SecondPoint();

    gp_Dir aFirstDir  = gce_MakeDir( aCenterPoint, aFirstPoint );
    gp_Dir aSecondDir = gce_MakeDir( aCenterPoint, aSecondPoint );

    Standard_Real aDirAngle = aFirstDir.Angle( aSecondDir );

    //                 p1 center p2
    //                   \ dir /
    // the direction are: \ | /
    //            first    \|/   second
    //             dir      c      dir
    // 

    gp_Dir aCenterDir = aFirstDir.Rotated( gp_Ax1( aCenterPoint, aPlaneN ), -aDirAngle * 0.5 );

    gp_Dir aPointDir  = gce_MakeDir( aCenterPoint, aPointOnPlane );

    Standard_Boolean isPositive = aPointDir.Dot( aCenterDir ) > 0;

    Standard_Real aPointAngle = aPointDir.AngleWithRef(  aCenterDir,  aPlaneN );
    if ( !isPositive )
    {
      aPointAngle = ( M_PI - Abs( aPointAngle ) ) * ( -Sign( 1.0, aPointAngle ) );
    }

    gp_Vec aPointVec = gp_Vec( aCenterPoint, aPointOnPlane );

    // calculate flyout for each separate case of point location
    if ( aPointAngle < -aDirAngle * 0.5 ) // outside of second dir
    {
      aFlyout = aPointVec.Dot( aFirstDir );
    }
    else if ( aPointAngle > aDirAngle * 0.5 ) // outside of first dir
    {
      aFlyout = aPointVec.Dot( aSecondDir );
    }
    else // between first and second direction
    {
      aFlyout = isPositive
        ?  aPointVec.Magnitude()
        : -aPointVec.Magnitude();
    }
  }

  myInteractedIO->SetFlyout( aFlyout );

  myViewer->getAISContext()->Redisplay( myInteractedIO );
}

//=================================================================================
// function : MoveText
// purpose  : 
//=================================================================================
void MeasureGUI_DimensionInteractor::MoveText( const Handle(V3d_View)& theView,
                                               const int theX,
                                               const int theY )
{
  // project point onto dimension plane
  bool isProjected     = false;
  gp_Pln aPlane        = myInteractedIO->GetPlane();
  gp_Pnt aPointOnPlane = ProjectPlane( theView, theX, theY, aPlane, isProjected );
  if ( !isProjected )
  {
    return;
  }

  const gp_Dir& aPlaneN = aPlane.Axis().Direction();

  Prs3d_DimensionTextHorizontalPosition aHPos   = myInteractedIO->DimensionAspect()->TextHorizontalPosition();
  Prs3d_DimensionTextVerticalPosition   aVPos   = myInteractedIO->DimensionAspect()->TextVerticalPosition();
  Prs3d_DimensionArrowOrientation       aArrPos = myInteractedIO->DimensionAspect()->ArrowOrientation();

  Standard_Real aHeight = myInteractedIO->DimensionAspect()->TextAspect()->Height() * 0.5;

  // move text of linear dimension
  if ( myInteractedIO->IsKind( STANDARD_TYPE( AIS_LengthDimension ) )
    || myInteractedIO->IsKind( STANDARD_TYPE( AIS_DiameterDimension ) ) )
  {
    gp_Pnt aFirstPoint;
    gp_Pnt aSecondPoint;

    // get first and second point from which the flyout line is normally extended
    Handle(AIS_LengthDimension) aLength = Handle(AIS_LengthDimension)::DownCast( myInteractedIO );
    if ( !aLength.IsNull() )
    {
      aFirstPoint  = aLength->FirstPoint();
      aSecondPoint = aLength->SecondPoint();
    }

    Handle(AIS_DiameterDimension) aDiameter = Handle(AIS_DiameterDimension)::DownCast( myInteractedIO );
    if ( !aDiameter.IsNull() )
    {
      const gp_Pnt& aCenter = aDiameter->Circle().Location();
      aFirstPoint    = aDiameter->AnchorPoint();
      aSecondPoint   = gp_Pnt( aFirstPoint.XYZ() + gp_Vec( aFirstPoint, aCenter ).XYZ() * 2.0 );
    }

    // get flyout line
    Standard_Real aFlyout   = myInteractedIO->GetFlyout();
    gp_Dir aBaseDir         = gce_MakeDir( aFirstPoint, aSecondPoint );
    gp_Dir aFlyoutDir       = aFlyout >= 0.0 
      ?  aPlaneN.Crossed( gce_MakeDir( aFirstPoint, aSecondPoint ) )
      : -aPlaneN.Crossed( gce_MakeDir( aFirstPoint, aSecondPoint ) );
    gp_Vec aFlyoutTranslate = gp_Vec( aFlyoutDir ) * Abs( myInteractedIO->GetFlyout() );
    gp_Lin aFlyoutLine      = gp_Lin( aFirstPoint, aBaseDir ).Translated( aFlyoutTranslate );

    // check if positive or negative offset along flyout line
    gp_Vec aFlyout2Point = gp_Vec( aFlyoutLine.Location(), aPointOnPlane );

    Standard_Real aPosY = aFlyout2Point.Dot( aFlyoutDir );
    Standard_Real aPosX = aFlyout2Point.Dot( aFlyoutLine.Direction() );

    if ( aPosX < 0.0 )
    {
      aHPos   = Prs3d_DTHP_Left;
      aArrPos = Prs3d_DAO_External;
    }
    else if ( aPosX > aFirstPoint.Distance( aSecondPoint ) )
    {
      aHPos   = Prs3d_DTHP_Right;
      aArrPos = Prs3d_DAO_External;
    }
    else
    {
      aHPos   = Prs3d_DTHP_Center;
      aArrPos = Prs3d_DAO_Fit;
    }

    if ( aPosY > aHeight )
    {
      aVPos = Prs3d_DTVP_Above;
    }
    else if ( aPosY < -aHeight )
    {
      aVPos = Prs3d_DTVP_Below;
    }
    else
    {
      aVPos = Prs3d_DTVP_Center;
    }
  }

  // move text of angular dimension
  if ( myInteractedIO->IsKind( STANDARD_TYPE( AIS_AngleDimension ) ) )
  {
    Handle(AIS_AngleDimension) anAngle = Handle(AIS_AngleDimension)::DownCast( myInteractedIO );

    const gp_Pnt& aCenterPoint = anAngle->CenterPoint();
    const gp_Pnt& aFirstPoint  = anAngle->FirstPoint();
    const gp_Pnt& aSecondPoint = anAngle->SecondPoint();

    Standard_Boolean isPositive = anAngle->GetFlyout() > 0.0;

    gp_Dir aFirstDir  = gce_MakeDir( aCenterPoint, aFirstPoint );
    gp_Dir aSecondDir = gce_MakeDir( aCenterPoint, aSecondPoint );
    if ( anAngle->GetFlyout() < 0.0 )
    {
      aFirstDir.Reverse();
      aSecondDir.Reverse();
    }

    Standard_Real aDirAngle = aFirstDir.Angle( aSecondDir );

    //                 p1 center p2
    //                   \ dir /
    // the direction are: \ | /
    //            first    \|/   second
    //             dir      c      dir

    gp_Dir aCenterDir = aFirstDir.Rotated( gp_Ax1( aCenterPoint, aPlaneN ), -aDirAngle * 0.5 );

    gp_Dir aPointDir = gce_MakeDir( aCenterPoint, aPointOnPlane );

    Standard_Real aPointAngle = aPointDir.AngleWithRef( aCenterDir, aPlaneN );

    gp_Vec aPointVec = gp_Vec( aCenterPoint, aPointOnPlane );

    Standard_Real aPosY = 0.0;

    // calculate flyout for each separate case of point location
    if ( aPointAngle < -aDirAngle * 0.5 ) // outside of second dir
    {
      aHPos   = Prs3d_DTHP_Left;
      aArrPos = Prs3d_DAO_External;
      aPosY   = aPointVec.Dot( aFirstDir ) - Abs( anAngle->GetFlyout() );
    }
    else if ( aPointAngle > aDirAngle * 0.5 ) // outside of first dir
    {
      aHPos   = Prs3d_DTHP_Right;
      aArrPos = Prs3d_DAO_External;
      aPosY   = aPointVec.Dot( aSecondDir ) - Abs( anAngle->GetFlyout() );
    }
    else // between first and second direction
    {
      aHPos   = Prs3d_DTHP_Center;
      aArrPos = Prs3d_DAO_Fit;
      aPosY   = aPointVec.Magnitude() - Abs( anAngle->GetFlyout() );
    }

    if ( aPosY > aHeight )
    {
      aVPos = Prs3d_DTVP_Above;
    }
    else if ( aPosY < -aHeight )
    {
      aVPos = Prs3d_DTVP_Below;
    }
    else
    {
      aVPos = Prs3d_DTVP_Center;
    }
  }

  myInteractedIO->DimensionAspect()->SetTextVerticalPosition( aVPos );
  myInteractedIO->DimensionAspect()->SetTextHorizontalPosition( aHPos );
  myInteractedIO->DimensionAspect()->SetArrowOrientation( aArrPos );
  myInteractedIO->SetToUpdate();

  myViewer->getAISContext()->Redisplay( myInteractedIO );
}

//=================================================================================
// function : eventFilter
// purpose  : 
//=================================================================================
bool MeasureGUI_DimensionInteractor::eventFilter( QObject* theObject, QEvent* theEvent )
{
  OCCViewer_ViewPort3d* aViewPort = (OCCViewer_ViewPort3d*) theObject;

  Handle(V3d_View) aView3d = aViewPort->getView();

  switch ( theEvent->type() )
  {
    // check whether it is the "dimension modify" operation or not.
    case QEvent::MouseButtonPress :
    {
      Handle(AIS_InteractiveContext) anAISContext = myViewer->getAISContext();

      QMouseEvent* aMouseEv = dynamic_cast<QMouseEvent*>( theEvent );

      // init detection
      anAISContext->MoveTo( aMouseEv->x(), aMouseEv->y(), aView3d );
      if ( !anAISContext->HasDetected() )
      {
        return false;
      }

      // commented by mpa 18.03.2015: since OCCT version 6.8.0 it's impossible
      // to change position of the dimensions presentations (flyout, text),
      // because anAISContext has 2 detected objects.

      // check that there is only one detected entity
      //anAISContext->InitDetected();
      //if ( anAISContext->MoreDetected() )
      //{
      //  return false;
      //}

      Handle(SelectMgr_EntityOwner) aDetectedOwner = anAISContext->DetectedOwner();
      if( aDetectedOwner.IsNull() )
      {
        return false;
      }

      myInteractedIO = Handle(AIS_Dimension)::DownCast( aDetectedOwner->Selectable() );

      // try to start operation for the detected entity
      Operation aStartOp = GetOperation( aDetectedOwner, aMouseEv->buttons(), aMouseEv->modifiers() );
      if ( aStartOp == Operation_None )
      {
        return false;
      }

      if ( !StartOperation( aStartOp, aView3d, aMouseEv->x(), aMouseEv->y() ) )
      {
        return false;
      }

      myOperation = aStartOp;

      mySelection.Clear();

      for ( anAISContext->InitSelected(); anAISContext->MoreSelected(); anAISContext->NextSelected() )
      {
        mySelection.Append( anAISContext->SelectedOwner() );
      }

      anAISContext->ClearSelected( Standard_False );
      anAISContext->AddOrRemoveSelected( aDetectedOwner );

      emit InteractionStarted( myInteractedIO );

      return true;
    }

    // stop event processing on mouse release
    case QEvent::MouseButtonRelease :
    {
      if ( myOperation != Operation_None )
      {
        Handle(AIS_InteractiveContext) anAISContext = myViewer->getAISContext();

        anAISContext->ClearSelected( Standard_False );
        SeqOfOwners::Iterator anIt( mySelection );
        for( ; anIt.More(); anIt.Next() )
        {
          anAISContext->AddOrRemoveSelected( anIt.Value(), Standard_False );
        }

        anAISContext->UpdateCurrentViewer();

        mySelection.Clear();

        myOperation = Operation_None;

        emit InteractionFinished( myInteractedIO );

        return true;
      }

      return false;
    }

    // perform operation
    case QEvent::MouseMove :
    {
      QMouseEvent* aMouseEv = (QMouseEvent*) theEvent;
      switch( myOperation )
      {
        case Operation_MoveFlyoutFree    : MoveFlyoutFree   ( aView3d, aMouseEv->x(), aMouseEv->y() ); return true;
        case Operation_MoveFlyoutInPlane : MoveFlyoutInPlane( aView3d, aMouseEv->x(), aMouseEv->y() ); return true;
        case Operation_MoveText          : MoveText         ( aView3d, aMouseEv->x(), aMouseEv->y() ); return true;
        default : return false;
      }
    }

    default: return false;
  }
}

//=================================================================================
// function : ConnectView
// purpose  : 
//=================================================================================
void MeasureGUI_DimensionInteractor::ConnectView( SUIT_ViewWindow* theView )
{
  ( (OCCViewer_ViewWindow*) theView )->getViewPort()->installEventFilter( this );
}

//=================================================================================
// function : DisconnectView
// purpose  : 
//=================================================================================
void MeasureGUI_DimensionInteractor::DisconnectView( SUIT_ViewWindow* theView )
{
  ( (OCCViewer_ViewWindow*) theView )->getViewPort()->removeEventFilter( this );
}

//=================================================================================
// function : Projection
// purpose  : 
//=================================================================================
gp_Lin MeasureGUI_DimensionInteractor::Projection( const Handle(V3d_View)& theView,
                                                   const int theMouseX,
                                                   const int theMouseY )
{
  Standard_Real P[3], D[3];
  theView->ConvertWithProj( theMouseX, theMouseY, P[0], P[1], P[2], D[0], D[1], D[2] );
  return gp_Lin( gp_Pnt( P[0], P[1], P[2] ), gp_Dir( D[0], D[1], D[2] ) );
}

//=================================================================================
// function : ProjectPlane
// purpose  : 
//=================================================================================
gp_Pnt MeasureGUI_DimensionInteractor::ProjectPlane( const Handle(V3d_View)& theView,
                                                     const int theMouseX,
                                                     const int theMouseY,
                                                     const gp_Pln& thePlane,
                                                     bool& theIsDone )
{
  gp_Lin aProjection = Projection( theView, theMouseX, theMouseY );

  Handle(Geom_Line)  aCrossLine  = new Geom_Line( aProjection );
  Handle(Geom_Plane) aCrossPlane = new Geom_Plane( thePlane );

  GeomAPI_IntCS aFindCross( aCrossLine, aCrossPlane );
  if ( !aFindCross.IsDone() || aFindCross.NbPoints() == 0 )
  {
    theIsDone = false;
    return gp::Origin();
  }

  theIsDone = true;
  return aFindCross.Point( 1 );
}

//=================================================================================
// function : SensitivityTolerance
// purpose  : 
//=================================================================================
Standard_Real MeasureGUI_DimensionInteractor::SensitivityTolerance( const Handle(V3d_View)& theView )
{
  // snapping tolerance
  Quantity_Length aSize[2];
  theView->Size( aSize[0], aSize[1] );
  return 1e-2 * Max( aSize[0], aSize[1] );
}

//=================================================================================
// function : OnViewCreated
// purpose  : 
//=================================================================================
void MeasureGUI_DimensionInteractor::OnViewCreated( SUIT_ViewWindow* theView )
{
  ConnectView( theView );
}

//=================================================================================
// function : OnViewRemoved
// purpose  : 
//=================================================================================
void MeasureGUI_DimensionInteractor::OnViewRemoved( SUIT_ViewWindow* theView )
{
  DisconnectView( theView );
}
