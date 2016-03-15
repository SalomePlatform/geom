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

//  SALOME GEOM : 
//  File   : GEOM_VTKTrihedron.cxx
//  Author : Sergey LITONIN
//  Module : SALOME
//
#include "GEOM_VTKTrihedron.hxx"

#include <Geom_Axis2Placement.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pnt.hxx>

#include <vtkMapper.h>
#include <vtkRenderer.h>
#include <vtkObjectFactory.h>
#include <vtkActor.h>
#include <vtkActorCollection.h>
#include <vtkProperty.h>
#include <vtkLineSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkAppendPolyData.h>
#include <vtkMatrix4x4.h>
#include <vtkTransform.h>
#include <vtkVectorText.h>
#include <vtkCoordinate.h>
#include <vtkTextActor.h>
#include <vtkTextMapper.h>

/*
  Class       : GEOM_VTKTrihedron
  Description : Class for displaying axis of trihedron in VTK viewer.
                Placement of trihedron is chahged with SetPlacement() method
*/
class GEOM_VTKTrihedronAxis : public VTKViewer_Axis
{
protected:

                                GEOM_VTKTrihedronAxis();
                                GEOM_VTKTrihedronAxis( const GEOM_VTKTrihedronAxis& );
                                ~GEOM_VTKTrihedronAxis();

public:

                                vtkTypeMacro( GEOM_VTKTrihedronAxis, VTKViewer_Axis );
  static GEOM_VTKTrihedronAxis* New();

  void                          SetAxis( const gp_Ax1& theAxis, const int theRot, double theColor[ 3 ] );
  virtual void                  Render( vtkRenderer* theRenderer );
  virtual void                  SetSize( double theSize );
  gp_Pnt                        GetOri() const;
  void                          SetColor( const double theColor[ 3 ] );

private:

  double          myOri[ 3 ];
  vtkMatrix4x4*                 myMatrix;
  vtkTransform*                 myTrsf;
};

vtkStandardNewMacro( GEOM_VTKTrihedronAxis );

GEOM_VTKTrihedronAxis::GEOM_VTKTrihedronAxis()
{
  VTKViewer_Axis::SetColor( 0.0, 0.0, 1.0 );
  myMatrix = vtkMatrix4x4::New();
  myTrsf = vtkTransform::New();
}

GEOM_VTKTrihedronAxis::~GEOM_VTKTrihedronAxis()
{
  myMatrix->Delete();
  myTrsf->Delete();
}

void GEOM_VTKTrihedronAxis::SetSize( double theSize )
{
  double aPosition[ 3 ] = { myOri[ 0 ] + myDir[ 0 ] * theSize,
                                          myOri[ 1 ] + myDir[ 1 ] * theSize,
                                          myOri[ 2 ] + myDir[ 2 ] * theSize };
                           
  myLineSource->SetPoint1( myOri[ 0 ], myOri[ 1 ], myOri[ 2 ] );
  myLineSource->SetPoint2( aPosition );

  myArrowActor->SetPosition( 0, 0, 0 );
  myArrowActor->SetOrientation( 0, 0, 0 );
  myArrowActor->SetUserMatrix( 0 );
  

  gp_Trsf aTrsf;
  aTrsf.SetDisplacement( gp_Ax3( gp_Pnt( 0, 0, 0 ), gp_Dir( 1, 0, 0 ) ),
                         gp_Ax3( GetOri(), gp_Dir( myDir[ 0 ], myDir[ 1 ], myDir[ 2 ] ) ) );

//   const gp_XYZ& aTrans = aTrsf.TranslationPart();
  gp_Mat aRot = aTrsf.VectorialPart();

  for ( int i = 1; i <= 3; i++ )
  {
    for ( int j = 1; j <= 3; j++ )
      myMatrix->SetElement( i - 1, j - 1, aRot( i, j ) );
 
  // Set Arrow Position
  myMatrix->SetElement( i - 1 , 3, aPosition[ i - 1 ] );
  }

  myArrowActor->SetUserMatrix( myMatrix );
    
#ifdef IPAL21440
  if( vtkCoordinate* aCoord = myLabelActor->GetPositionCoordinate()->GetReferenceCoordinate() )
    aCoord->SetValue( aPosition );
#else
  myLabelActor->SetPosition( 0, 0, 0 );
  myLabelActor->AddPosition( aPosition );
#endif
}

void GEOM_VTKTrihedronAxis::Render(vtkRenderer* theRenderer)
{
  myLineActor->Render( theRenderer );
  myArrowActor->Render( theRenderer );
#ifndef IPAL21440
  myLabelActor->Render( theRenderer );
#endif

  vtkCamera* aCamera = theRenderer->GetActiveCamera();
  SetCamera( aCamera );
}

gp_Pnt GEOM_VTKTrihedronAxis::GetOri() const
{
  return gp_Pnt( myOri[ 0 ], myOri[ 1 ], myOri[ 2 ] );
}

void GEOM_VTKTrihedronAxis::SetAxis( const gp_Ax1& theAxis,
                                     const int     theRot,
                                     double theColor[ 3 ] )
{
  gp_Pnt aLoc = theAxis.Location();
  gp_Dir aDir = theAxis.Direction();

  myOri[ 0 ] = aLoc.X();
  myOri[ 1 ] = aLoc.Y();
  myOri[ 2 ] = aLoc.Z();

  myDir[ 0 ] = aDir.X();
  myDir[ 1 ] = aDir.Y();
  myDir[ 2 ] = aDir.Z();

  double aColor[ 3 ] = { 0, 0, 0 };
  aColor[ theRot ] = 1;
  if ( theColor[ 0 ] == -1 )
    VTKViewer_Axis::SetColor( aColor[ 0 ], aColor[ 1 ], aColor[ 2 ] );
  else
    VTKViewer_Axis::SetColor( theColor[ 0 ], theColor[ 1 ], theColor[ 2 ] );

#ifdef IPAL21440
  if      ( theRot == 0 ) myTextMapper->SetInput( "X" );
  else if ( theRot == 1 ) myTextMapper->SetInput( "Y" );
  else if ( theRot == 2 ) myTextMapper->SetInput( "Z" );
#else
  if      ( theRot == 0 ) myVectorText->SetText( "X" );
  else if ( theRot == 1 ) myVectorText->SetText( "Y" );
  else if ( theRot == 2 ) myVectorText->SetText( "Z" );
#endif
}

void GEOM_VTKTrihedronAxis::SetColor( const double theColor[ 3 ] )
{
  VTKViewer_Axis::SetColor( theColor[ 0 ], theColor[ 1 ], theColor[ 2 ] );
}


/*
  Class       : GEOM_VTKTrihedron
  Description : Class for displaying trihedron of local CS in VTK viewer.
                Placement of trihedron is chahged with SetPlacement() method
*/

vtkStandardNewMacro( GEOM_VTKTrihedron );

GEOM_VTKTrihedron::GEOM_VTKTrihedron()
{
  myMapper = vtkPolyDataMapper::New();
  myAxis[ 0 ] = myAxis[ 1 ] = myAxis[ 2 ] = 0;
  mySize = 100;

  myColor[ 0 ] = myColor[ 1 ] = myColor[ 2 ] = -1;

  myDefaultColor[ 0 ] = myDefaultColor[ 1 ] = myDefaultColor[ 2 ] = 1;

  myPreHighlightColor[ 0 ] = 0;
  myPreHighlightColor[ 1 ] = myPreHighlightColor[ 2 ] = 1;

  myHighlightColor[ 0 ] = myHighlightColor[ 1 ] = myHighlightColor[ 2 ] = 1;

  //SetInfinitive( true );
  SetPickable( true );
}

GEOM_VTKTrihedron::~GEOM_VTKTrihedron()
{
  for ( int i = 0; i < 3; i++ )
    myAxis[i]->Delete();

  if ( myMapper )
    myMapper->Delete();
}

void GEOM_VTKTrihedron::SetSize( double theSize )
{
  mySize = theSize;
  for ( int i = 0; i < 3; i++ )
    myAxis[i]->SetSize( theSize );


  // Calculate new sensitive area
  gp_Pnt anOri = ( (GEOM_VTKTrihedronAxis*)myAxis[ 0 ] )->GetOri();
  gp_Pnt aEndX( myLocation.XYZ() + myDirX.XYZ() * mySize );
  gp_Pnt aEndY( myLocation.XYZ() + myDirY.XYZ() * mySize );
  gp_Pnt aEndZ( myLocation.XYZ() + myDirZ.XYZ() * mySize );

  vtkLineSource* aSrcX = vtkLineSource::New();
  vtkLineSource* aSrcY = vtkLineSource::New();
  vtkLineSource* aSrcZ = vtkLineSource::New();

  aSrcX->SetPoint1( anOri.X(), anOri.Y(), anOri.Z() );
  aSrcX->SetPoint2( aEndX.X(), aEndX.Y(), aEndX.Z() );

  aSrcY->SetPoint1( anOri.X(), anOri.Y(), anOri.Z() );
  aSrcY->SetPoint2( aEndY.X(), aEndY.Y(), aEndY.Z() );

  aSrcZ->SetPoint1( anOri.X(), anOri.Y(), anOri.Z() );
  aSrcZ->SetPoint2( aEndZ.X(), aEndZ.Y(), aEndZ.Z() );

  vtkAppendPolyData* aRes = vtkAppendPolyData::New();
  aRes->AddInputConnection( aSrcX->GetOutputPort() );
  aRes->AddInputConnection( aSrcY->GetOutputPort() );
  aRes->AddInputConnection( aSrcZ->GetOutputPort() );
  
  myMapper->SetInputConnection( aRes->GetOutputPort() );
  aRes->Update();
  SALOME_Actor::SetMapper( myMapper );

  aSrcX->Delete();
  aSrcY->Delete();
  aSrcZ->Delete();
  aRes->Delete();
}

void GEOM_VTKTrihedron::SetVisibility( int theVisibility )
{
  Superclass::SetVisibility( theVisibility );
  SetVisibility( theVisibility == 1 ? VTKViewer_Trihedron::eOn : VTKViewer_Trihedron::eOff );
}

void GEOM_VTKTrihedron::SetVisibility( VTKViewer_Trihedron::TVisibility theVis )
{
  for ( int i = 0; i < 3; i++ )
    myAxis[i]->SetVisibility( theVis );
}

void GEOM_VTKTrihedron::AddToRender( vtkRenderer* theRenderer )
{
  vtkCamera* aCamera = theRenderer->GetActiveCamera();
  for(int i = 0; i < 3; i++)
  {
    myAxis[i]->AddToRender(theRenderer);
    myAxis[i]->SetCamera(aCamera);
  }

  theRenderer->AddActor( this );
}

void GEOM_VTKTrihedron::RemoveFromRender( vtkRenderer* theRenderer )
{
  for(int i = 0; i < 3; i++)
  {
    myAxis[i]->RemoveFromRender(theRenderer);
  }

  theRenderer->RemoveActor( this );
}

int GEOM_VTKTrihedron::GetVisibleActorCount(vtkRenderer* theRenderer)
{
  vtkActorCollection* aCollection = theRenderer->GetActors();
  aCollection->InitTraversal();
  int aCount = 0;
  while( vtkActor* prop = aCollection->GetNextActor() )
    if ( prop->GetVisibility() )
      if ( SALOME_Actor* anActor = SALOME_Actor::SafeDownCast( prop ) )
    if ( !anActor->IsInfinitive() )
      aCount++;

  vtkCamera* aCamera = theRenderer->GetActiveCamera();
  for(int i = 0; i < 3; i++)
    myAxis[i]->SetCamera( aCamera );

  return aCount;
}

void GEOM_VTKTrihedron::SetCamera( vtkCamera* theCamera )
{
  for(int i = 0; i < 3; i++)
    myAxis[i]->SetCamera( theCamera );
}

void GEOM_VTKTrihedron::SetPlacement( const Handle(Geom_Axis2Placement)& thePlc )
{
  gp_Ax2 anAx2 = thePlc->Ax2();

  myDirX = anAx2.XDirection();
  myDirY = anAx2.YDirection();
  myDirZ = anAx2.Direction();

  myLocation = anAx2.Axis().Location();

  if ( myAxis[ 0 ] != 0 ) myAxis[ 0 ]->Delete();
  if ( myAxis[ 1 ] != 0 ) myAxis[ 1 ]->Delete();
  if ( myAxis[ 2 ] != 0 ) myAxis[ 2 ]->Delete();

  myAxis[ 0 ] = GEOM_VTKTrihedronAxis::New();
  myAxis[ 1 ] = GEOM_VTKTrihedronAxis::New();
  myAxis[ 2 ] = GEOM_VTKTrihedronAxis::New();

  ( (GEOM_VTKTrihedronAxis*)myAxis[ 0 ] )->SetAxis( gp_Ax1( myLocation, myDirX ), 0, myColor );
  ( (GEOM_VTKTrihedronAxis*)myAxis[ 1 ] )->SetAxis( gp_Ax1( myLocation, myDirY ), 1, myColor );
  ( (GEOM_VTKTrihedronAxis*)myAxis[ 2 ] )->SetAxis( gp_Ax1( myLocation, myDirZ ), 2, myColor );

  SetSize( mySize );
}

vtkMapper* GEOM_VTKTrihedron::GetMapper()
{
  return myMapper;
}

void GEOM_VTKTrihedron::Render(vtkRenderer* r, vtkMapper *)
{
  ( (GEOM_VTKTrihedronAxis*)myAxis[ 0 ] )->Render( r );
  ( (GEOM_VTKTrihedronAxis*)myAxis[ 1 ] )->Render( r );
  ( (GEOM_VTKTrihedronAxis*)myAxis[ 2 ] )->Render( r );
}

void GEOM_VTKTrihedron::SetColor( double r, double g, double b )
{
  myColor[ 0 ] = r;
  myColor[ 1 ] = g;
  myColor[ 2 ] = b;

  if ( myAxis[ 0 ] ) ( (GEOM_VTKTrihedronAxis*)myAxis[ 0 ] )->SetColor( myColor );
  if ( myAxis[ 1 ] ) ( (GEOM_VTKTrihedronAxis*)myAxis[ 1 ] )->SetColor( myColor );
  if ( myAxis[ 2 ] ) ( (GEOM_VTKTrihedronAxis*)myAxis[ 2 ] )->SetColor( myColor );
}

void GEOM_VTKTrihedron::GetColor( double& r, double& g, double& b )
{
  r = myColor[ 0 ];
  g = myColor[ 1 ];
  b = myColor[ 2 ];
}

bool GEOM_VTKTrihedron::IsSetCamera() const
{
  return true;
}

bool GEOM_VTKTrihedron::IsResizable() const
{
  return false;
}

void GEOM_VTKTrihedron::Highlight( bool theIsHighlight )
{
  if( theIsHighlight )
    SetAxesColors( myHighlightColor );
  else
    ResetAxesColors();

  Superclass::Highlight( theIsHighlight );
}

bool GEOM_VTKTrihedron::PreHighlight( vtkInteractorStyle *theInteractorStyle, 
                                      SVTK_SelectionEvent* theSelectionEvent,
                                      bool theIsHighlight )
{
  if ( !GetPickable() )
    return false;  

  if ( !isHighlighted() )
  {
    if( theIsHighlight )
      SetAxesColors( myPreHighlightColor );
    else
      ResetAxesColors();
  }

  return Superclass::PreHighlight( theInteractorStyle, theSelectionEvent, theIsHighlight );
}

void GEOM_VTKTrihedron::ResetAxesColors()
{
  if( myColor[0] != -1 )
    SetAxesColors( myColor );
  else
    SetAxesColors( myDefaultColor, true );
}

void GEOM_VTKTrihedron::SetAxesColors( double theColor[3], bool theIsDiffuse )
{
  myAxis[ 0 ]->SetColor( theColor[0], theIsDiffuse ? 0.0 : theColor[1], theIsDiffuse ? 0.0 : theColor[2] );
  myAxis[ 1 ]->SetColor( theIsDiffuse ? 0.0 : theColor[0], theColor[1], theIsDiffuse ? 0.0 : theColor[2] );
  myAxis[ 2 ]->SetColor( theIsDiffuse ? 0.0 : theColor[0], theIsDiffuse ? 0.0 : theColor[1], theColor[2] );
}

