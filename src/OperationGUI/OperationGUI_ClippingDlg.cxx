// Copyright (C) 2007-2015  CEA/DEN, EDF R&D, OPEN CASCADE
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
// File   : OperationGUI_ClippingDlg.cxx
// Author : Michael Zorin, Open CASCADE S.A.S. (mikhail.zorin@opencascade.com)
//
#include "OperationGUI_ClippingDlg.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>
#include <SalomeApp_DoubleSpinBox.h>

#include <SUIT_Session.h>
#include <SUIT_Desktop.h>
#include <SUIT_ViewManager.h>
#include <SVTK_ViewWindow.h>
#include <SVTK_ViewModel.h>

#include <OCCViewer_ViewModel.h>
#include <OCCViewer_ViewWindow.h>
#include <OCCViewer_ViewPort3d.h>

#include <VTKViewer.h>

// OCC Includes
#include <V3d_View.hxx>
#include <V3d.hxx>

// VTK Includes
#include <vtkCamera.h>
#include <vtkRenderer.h>

//=================================================================================
// class    : OperationGUI_ClippingDlg()
// purpose  :
//=================================================================================
OperationGUI_ClippingDlg::OperationGUI_ClippingDlg( GeometryGUI* theGeometryGUI, QWidget* parent )
  : GEOMBase_Skeleton( theGeometryGUI, parent, false )
{
  setWindowTitle( tr( "Change clipping range" ) );

  mainFrame()->GroupConstructors->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->GroupConstructors->close();
  mainFrame()->GroupBoxName->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->GroupBoxName->close();

  /***************************************************************/
  GroupArguments = new QGroupBox( centralWidget() );
  QGridLayout* GroupArgumentsLayout = new QGridLayout( GroupArguments );
  GroupArgumentsLayout->setSpacing( 6 );
  GroupArgumentsLayout->setMargin( 9 );

  // Controls
  TextLabelNear = new QLabel( tr( "Near" ), GroupArguments );
  GroupArgumentsLayout->addWidget( TextLabelNear, 0, 0 );

  SpinBox_Near = new SalomeApp_DoubleSpinBox( GroupArguments );
  initSpinBox( SpinBox_Near, COORD_MIN, COORD_MAX, 10.0, "length_precision" );
  GroupArgumentsLayout->addWidget( SpinBox_Near, 0, 1 );

  TextLabelFar = new QLabel( tr( "Far" ), GroupArguments );
  GroupArgumentsLayout->addWidget( TextLabelFar, 0, 2 );

  SpinBox_Far = new SalomeApp_DoubleSpinBox( GroupArguments );
  initSpinBox( SpinBox_Far, COORD_MIN, COORD_MAX, 10.0, "length_precision" );
  GroupArgumentsLayout->addWidget( SpinBox_Far, 0, 3 );

  resetButton  = new QPushButton( tr( "Reset" ), GroupArguments );
  GroupArgumentsLayout->addWidget( resetButton, 0, 4 );

  TypeCB = new QComboBox( GroupArguments );
  TypeCB->addItem( tr( "OFF" ) );
  TypeCB->addItem( tr( "BACK" ) );
  TypeCB->addItem( tr( "FRONT" ) );
  TypeCB->addItem( tr( "SLICE" ) );
  GroupArgumentsLayout->addWidget( TypeCB, 1, 0, 1, 5 );

  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( GroupArguments );

  /* signals and slots connections */
  connect( buttonOk(),    SIGNAL( clicked() ), this, SLOT( ClickOnOk() ) );
  connect( buttonApply(), SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );

  connect( resetButton,   SIGNAL( clicked() ), this, SLOT( onReset() ) ) ;

  setHelpFileName( "none.htm" );

  Init();
}

//=================================================================================
// function : ~ OperationGUI_ClippingDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
OperationGUI_ClippingDlg::~ OperationGUI_ClippingDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void OperationGUI_ClippingDlg::Init()
{
  SUIT_ViewWindow* anActiveWindow = SUIT_Session::session()->activeApplication()->desktop()->activeWindow();
  if ( !anActiveWindow )
    return;

  if ( anActiveWindow->getViewManager()->getType() == SVTK_Viewer::Type() ) {
    SVTK_ViewWindow* aVTKFrame = dynamic_cast<SVTK_ViewWindow*>( anActiveWindow );
    if ( !aVTKFrame )
      return;

    TextLabelNear->setText( tr( "Near" ) );
    TextLabelFar->setText( tr( "Far" ) );

    TypeCB->hide();

    vtkRenderer* aRenderer = aVTKFrame->getRenderer();
    if ( !aRenderer ) return;

    vtkCamera* anActiveCamera = aRenderer->GetActiveCamera();
    if ( anActiveCamera == NULL ) {
      return;
    }

      // Find the plane equation for the camera view plane
    double vn[3];
    anActiveCamera->GetViewPlaneNormal( vn );
    double  position[3];
    anActiveCamera->GetPosition( position );

    double bounds[6];
    aRenderer->ComputeVisiblePropBounds( bounds );

    double center[3];
    center[0] = ( bounds[0] + bounds[1] ) / 2.0;
    center[1] = ( bounds[2] + bounds[3] ) / 2.0;
    center[2] = ( bounds[4] + bounds[5] ) / 2.0;

    double width = sqrt( ( bounds[1]-bounds[0] ) * ( bounds[1]-bounds[0] ) +
                         ( bounds[3]-bounds[2] ) * ( bounds[3]-bounds[2] ) +
                         ( bounds[5]-bounds[4] ) * ( bounds[5]-bounds[4] ) );

    double distance = sqrt( ( position[0]-center[0] ) * ( position[0]-center[0] ) +
                            ( position[1]-center[1] ) * ( position[1]-center[1] ) +
                            ( position[2]-center[2] ) * ( position[2]-center[2] ) );

    double range[2] = { distance - width/2.0, distance + width/2.0 };

    SpinBox_Near->setValue( range[0] );
    SpinBox_Far->setValue( range[1] );

    return;
  }
  else if ( anActiveWindow->getViewManager()->getType() == OCCViewer_Viewer::Type() ) {
    OCCViewer_ViewWindow* aOCCFrame = dynamic_cast<OCCViewer_ViewWindow*>( anActiveWindow );

    TextLabelNear->setText( tr( "Depth" ) );
    TextLabelFar->setText( tr( "Thickness" ) );

    Handle(V3d_View) view3d = ( (OCCViewer_ViewPort3d*)aOCCFrame->getViewPort() )->getView();

    double depth, thickness;
    int ztype = view3d->ZClipping( depth, thickness );
    SpinBox_Near->setValue( depth );
    SpinBox_Far->setValue( thickness );
    TypeCB->setCurrentIndex( ztype );

    //Handle(V3d_Plane) aPlane = new V3d_Plane( aOCCFrame->getViewer()->getViewer3d() ,0, 0, 1, -100);
    //aPlane->Display(view3d);
  }
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool OperationGUI_ClippingDlg::ClickOnApply()
{
  SUIT_ViewWindow* anActiveWindow = SUIT_Session::session()->activeApplication()->desktop()->activeWindow();
  if ( !anActiveWindow )
    return false;

  if ( anActiveWindow->getViewManager()->getType() == SVTK_Viewer::Type() ) {
    SVTK_ViewWindow* aVTKFrame = dynamic_cast<SVTK_ViewWindow*>( anActiveWindow );
    if ( !aVTKFrame )
      return false;

    vtkRenderer* aRenderer = aVTKFrame->getRenderer();
    if ( !aRenderer ) return false;

    vtkCamera* anActiveCamera = aRenderer->GetActiveCamera();
    if ( anActiveCamera == NULL ) {
      return false;
    }

    double range[2] = { SpinBox_Near->value(), SpinBox_Far->value() };
    if ( range[0] < 0.0 ) range[0] = 0.0;
    anActiveCamera->SetClippingRange( range );

    return true;
  }
  else if ( anActiveWindow->getViewManager()->getType() == OCCViewer_Viewer::Type() ) {
    OCCViewer_ViewWindow* aOCCFrame = dynamic_cast<OCCViewer_ViewWindow*>( anActiveWindow );

    Handle(V3d_View) view3d = ( (OCCViewer_ViewPort3d*)aOCCFrame->getViewPort() )->getView();

    double depth = SpinBox_Near->value();
    double thickness = SpinBox_Far->value();
    int aType = TypeCB->currentIndex();

    view3d->SetZClippingType( V3d_TypeOfZclipping( aType ) );
    view3d->SetZClippingDepth( depth );
    view3d->SetZClippingWidth( thickness );
  }

  return true;
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void OperationGUI_ClippingDlg::ClickOnOk()
{
  setIsApplyAndClose( true );
  if ( ClickOnApply() )
    ClickOnCancel();
}

//=================================================================================
// function : onActivate()
// purpose  :
//=================================================================================
void OperationGUI_ClippingDlg::onActivate()
{
  GEOMBase_Skeleton::ActivateThisDialog();
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void OperationGUI_ClippingDlg::enterEvent( QEvent* )
{
  this->setEnabled( true );
}

//=================================================================================
// function : onReset()
// purpose  :
//=================================================================================
void OperationGUI_ClippingDlg::onReset()
{
  SUIT_ViewWindow* anActiveWindow =
    SUIT_Session::session()->activeApplication()->desktop()->activeWindow();
  if ( !anActiveWindow )
    return;

  if ( anActiveWindow->getViewManager()->getType() == SVTK_Viewer::Type() ) {
    SVTK_ViewWindow* aVTKFrame = dynamic_cast<SVTK_ViewWindow*>( anActiveWindow );
    if ( !aVTKFrame )
      return;

    vtkRenderer* aRenderer = aVTKFrame->getRenderer();
    if ( !aRenderer ) return;

    vtkCamera* anActiveCamera = aRenderer->GetActiveCamera();
    if ( anActiveCamera == NULL ) {
      return;
    }

      // Find the plane equation for the camera view plane
    double vn[3];
    anActiveCamera->GetViewPlaneNormal( vn );
    double  position[3];
    anActiveCamera->GetPosition( position );

    double bounds[6];
    aRenderer->ComputeVisiblePropBounds( bounds );

    double center[3];
    center[0] = ( bounds[0] + bounds[1] ) / 2.0;
    center[1] = ( bounds[2] + bounds[3] ) / 2.0;
    center[2] = ( bounds[4] + bounds[5] ) / 2.0;

    double width = sqrt( ( bounds[1]-bounds[0] ) * ( bounds[1]-bounds[0] ) +
                         ( bounds[3]-bounds[2] ) * ( bounds[3]-bounds[2] ) +
                         ( bounds[5]-bounds[4] ) * ( bounds[5]-bounds[4] ) );

    double distance = sqrt( ( position[0]-center[0] ) * ( position[0]-center[0] ) +
                            ( position[1]-center[1] ) * ( position[1]-center[1] ) +
                            ( position[2]-center[2] ) * ( position[2]-center[2] ) );

    double range[2] = { distance - width/2.0, distance + width/2.0 };

    SpinBox_Near->setValue( range[0] );
    SpinBox_Far->setValue( range[1] );

    return;
  }
  else if ( anActiveWindow->getViewManager()->getType() == OCCViewer_Viewer::Type() ) {
    OCCViewer_ViewWindow* aOCCFrame = dynamic_cast<OCCViewer_ViewWindow*>( anActiveWindow );

    TextLabelNear->setText( tr( "Depth"  ) );
    TextLabelFar->setText( tr( "Thickness"  ) );

    Handle(V3d_View) view3d = ( (OCCViewer_ViewPort3d*)aOCCFrame->getViewPort() )->getView();

    view3d->SetZClippingType( V3d_TypeOfZclipping( 0 ) );
    view3d->ZFitAll();
    double depth, thickness;
    int ztype= view3d->ZClipping( depth, thickness );
    SpinBox_Near->setValue( 0 );
    SpinBox_Far->setValue( 1000 );
    TypeCB->setCurrentIndex( ztype );
  }
}
