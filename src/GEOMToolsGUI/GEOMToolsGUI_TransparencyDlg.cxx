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
// File   : GEOMToolsGUI_TransparencyDlg.cxx
// Author : Lucien PIGNOLONI
//
#include "GEOMToolsGUI_TransparencyDlg.h"
#include <GEOMBase.h>
#include <GEOM_AISShape.hxx>
#include <GeometryGUI.h>
#include <GEOM_Constants.h>

#include <SALOME_ListIO.hxx>

#include <SVTK_ViewModel.h>
#include <SVTK_ViewWindow.h>
#include <SVTK_View.h>

#include <OCCViewer_ViewModel.h>

#include <SUIT_ViewManager.h>
#include <SUIT_Desktop.h>
#include <SUIT_MessageBox.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SUIT_OverrideCursor.h>

#include <SalomeApp_Application.h>
#include <LightApp_Application.h>
#include <LightApp_SelectionMgr.h>
#include <SalomeApp_Study.h>

#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QKeyEvent>

#include <AIS_InteractiveContext.hxx>


//=================================================================================
// class    : GEOMBase_TransparencyDlg()
// purpose  : Constructs a GEOMBase_SUBSHAPE which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//
//          : WARNING : this dialog is modal !
//
//=================================================================================
GEOMToolsGUI_TransparencyDlg::GEOMToolsGUI_TransparencyDlg( QWidget* parent )
  :QDialog( parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint )
{
  setModal( true );
  
  resize(152, 107); 
  setWindowTitle(tr("GEOM_TRANSPARENCY_TITLE"));
  setSizeGripEnabled(true);
  QVBoxLayout* lay = new QVBoxLayout(this); 
  lay->setSpacing(6);
  lay->setMargin(11);
  
  /*************************************************************************/
  QGroupBox* GroupButtons = new QGroupBox( this );
  QHBoxLayout* GroupButtonsLayout = new QHBoxLayout( GroupButtons );
  GroupButtonsLayout->setAlignment( Qt::AlignTop );
  GroupButtonsLayout->setSpacing( 6 );
  GroupButtonsLayout->setMargin( 11 );
  
  /*************************************************************************/
  QGroupBox* GroupC1 = new QGroupBox( this );
  QGridLayout* GroupC1Layout = new QGridLayout( GroupC1 );
  GroupC1Layout->setAlignment( Qt::AlignTop );
  GroupC1Layout->setSpacing( 6 );
  GroupC1Layout->setMargin( 11 );
  
  QLabel* TextLabelTransparent = new QLabel( tr( "GEOM_TRANSPARENCY_TRANSPARENT" ), GroupC1 );
  TextLabelTransparent->setAlignment( Qt::AlignLeft );
  GroupC1Layout->addWidget( TextLabelTransparent, 0, 2 );
  
  myValueLab = new QLabel( GroupC1 );
  myValueLab->setAlignment( Qt::AlignCenter );
  myValueLab->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  QFont fnt = myValueLab->font(); fnt.setBold( true ); myValueLab->setFont( fnt );
  GroupC1Layout->addWidget( myValueLab, 0, 1 );

  QLabel* TextLabelOpaque = new QLabel( tr( "GEOM_TRANSPARENCY_OPAQUE" ), GroupC1 );
  TextLabelOpaque->setAlignment( Qt::AlignRight );
  GroupC1Layout->addWidget( TextLabelOpaque, 0, 0 );
  //GroupC1Layout->addItem( new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ), 0, 1 );
  
  mySlider = new QSlider( Qt::Horizontal, GroupC1 );
  mySlider->setFocusPolicy( Qt::NoFocus );
  mySlider->setMinimumSize( 300, 0 );
  mySlider->setTickPosition( QSlider::TicksAbove );
  mySlider->setTickInterval( 10 );
  mySlider->setMinimum( 0 );
  mySlider->setMaximum( 100 );
  mySlider->setSingleStep( 1 );
  mySlider->setPageStep( 10 );
  //mySlider->setValue( 5 );
  mySlider->setTracking(false);

  mySlider->setTickPosition( QSlider::TicksAbove );
  GroupC1Layout->addWidget( mySlider, 1, 0, 1, 3 );

  /*************************************************************************/
  QPushButton* buttonOk = new QPushButton( tr( "GEOM_BUT_OK" ), GroupButtons );
  buttonOk->setAutoDefault( true );
  buttonOk->setDefault( true );
  
  QPushButton* buttonHelp = new QPushButton( tr( "GEOM_BUT_HELP" ), GroupButtons );
  buttonHelp->setAutoDefault( true );
  buttonHelp->setDefault( true );
  
  GroupButtonsLayout->addWidget( buttonOk );
  GroupButtonsLayout->addSpacing( 10 );
  GroupButtonsLayout->addStretch();
  GroupButtonsLayout->addWidget( buttonHelp );
  
  /*************************************************************************/
  lay->addWidget(GroupC1);
  lay->addWidget(GroupButtons);
    
  /* First call valueChanged() method for initialisation               */
  /* The default value of transparency will change with the selection  */
  myFirstInit = true;
  //  mySlider->setMaxValue( 10 );
  //  mySlider->setValue( 5 ) ;
  SetTransparency();
  
  myHelpFileName = "transparency_page.html";

  // signals and slots connections : after ValueHasChanged()
  connect(buttonOk,   SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonHelp, SIGNAL(clicked()), this, SLOT(ClickOnHelp()));
  connect(mySlider,   SIGNAL(valueChanged(int)), this, SLOT(SetTransparency()));
  connect(mySlider,   SIGNAL(sliderMoved(int)),  this, SLOT(SliderHasMoved(int)));
}


//=================================================================================
// function : ~GEOMBase_TransparencyDlg()
// purpose  :
//=================================================================================
GEOMToolsGUI_TransparencyDlg::~GEOMToolsGUI_TransparencyDlg()
{
    // no need to delete child widgets, Qt does it all for us
}


//=======================================================================
// function : ClickOnOk()
// purpose  :
//=======================================================================
void GEOMToolsGUI_TransparencyDlg::ClickOnOk()
{
  accept();
  return;
}


//=======================================================================
// function : ClickOnClose()
// purpose  :
//=======================================================================
void GEOMToolsGUI_TransparencyDlg::ClickOnClose()
{
  accept();
  return;
}

//=================================================================================
// function : ClickOnHelp()
// purpose  :
//=================================================================================
void GEOMToolsGUI_TransparencyDlg::ClickOnHelp()
{
  LightApp_Application* app = (LightApp_Application*)(SUIT_Session::session()->activeApplication());
  if (app) {
    GeometryGUI* aGeomGUI = dynamic_cast<GeometryGUI*>( app->module( "Geometry" ) );
    app->onHelpContextModule(aGeomGUI ? app->moduleName(aGeomGUI->moduleName()) : QString(""), myHelpFileName);
  }
  else {
                QString platform;
#ifdef WIN32
                platform = "winapplication";
#else
                platform = "application";
#endif
    SUIT_MessageBox::warning(0, QObject::tr("WRN_WARNING"),
                             QObject::tr("EXTERNAL_BROWSER_CANNOT_SHOW_PAGE").
                             arg(app->resourceMgr()->stringValue("ExternalBrowser", platform)).arg(myHelpFileName),
                             QObject::tr("BUT_OK"));
  }
}

//=================================================================================
// function : ValueHasChanged()
// purpose  : Called when value of slider change
//=================================================================================
void GEOMToolsGUI_TransparencyDlg::ValueHasChanged()
{
  myValueLab->setText( QString("%1%").arg( mySlider->value() ) );
}

//=================================================================================
// function : SliderHasMoved()
// purpose  : Called when value of slider change
//=================================================================================
void GEOMToolsGUI_TransparencyDlg::SliderHasMoved(int value)
{
  myValueLab->setText( QString("%1%").arg( value ) );
}

//=================================================================================
// function : SetTransparency()
// purpose  : Called when value of slider change
//          : or the first time as initilisation
//=================================================================================
void GEOMToolsGUI_TransparencyDlg::SetTransparency()
{
  float newValue = mySlider->value() / 100.;

  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  if ( !app )
    return;
  LightApp_SelectionMgr* aSelMgr = app->selectionMgr();
  if ( !aSelMgr )
    return;
  
  SalomeApp_Study* aStudy = dynamic_cast<SalomeApp_Study*>(app->activeStudy());
  
  if(!aStudy)
    return;
  
  SALOME_ListIO selected;
  aSelMgr->selectedObjects( selected );
  if ( selected.IsEmpty() )
    return;

  Handle(SALOME_InteractiveObject) FirstIOS =  selected.First();
  if ( FirstIOS.IsNull() )
    return;
        
  SUIT_ViewWindow* window = app->desktop()->activeWindow();
  bool isOCC = ( window && window->getViewManager()->getType() == OCCViewer_Viewer::Type() );
  bool isVTK = ( window && window->getViewManager()->getType() == SVTK_Viewer::Type() );
  int aMgrId = window->getViewManager()->getGlobalId();
  if ( isVTK ) {
    SVTK_ViewWindow* vtkVW = dynamic_cast<SVTK_ViewWindow*>( window );
    if ( !vtkVW )
      return;
    SVTK_View* aView = vtkVW->getView();
    if ( myFirstInit ) {        
      myFirstInit = false;
      int transp = int ((aView->GetTransparency(FirstIOS))*100.0 + 0.5);
      mySlider->setValue(transp);
      ValueHasChanged();
      return;
    }

    SUIT_OverrideCursor wc;
    for ( SALOME_ListIteratorOfListIO It( selected ); It.More(); It.Next() ) {
      aView->SetTransparency( It.Value(), newValue );
      aStudy->setObjectProperty( aMgrId , It.Value()->getEntry(), GEOM::propertyName( GEOM::Transparency ) , newValue );
    }
    GeometryGUI::Modified();
    aView->Repaint();
  } // if ( isVTK )
        
  else if ( isOCC ) {
    GEOMBase* gb = new GEOMBase();
    Handle(GEOM_AISShape) aisShape;
    if ( myFirstInit ) {
      mySlider->setValue(mySlider->maximum());
      myFirstInit = false;
      aisShape = gb->ConvertIOinGEOMAISShape( FirstIOS, true );
      if( aisShape.IsNull() )
        return;
      int transp = int( aisShape->Transparency() * 100.0 + 0.5);
      mySlider->setValue(transp);
      ValueHasChanged();
      return;
    }
    
    SUIT_OverrideCursor wc;
    OCCViewer_Viewer* vm = dynamic_cast<OCCViewer_Viewer*>( window->getViewManager()->getViewModel() );
    if ( !vm )
      return;
    Handle(AIS_InteractiveContext) ic = vm->getAISContext();
    for ( SALOME_ListIteratorOfListIO It( selected ); It.More(); It.Next() ) {
      aisShape = gb->ConvertIOinGEOMAISShape( It.Value(), true );
      if ( !aisShape.IsNull() ) {
        ic->SetTransparency( aisShape, newValue, false );
        ic->Redisplay( aisShape, Standard_False, Standard_True );
        aStudy->setObjectProperty( aMgrId , It.Value()->getEntry(), GEOM::propertyName( GEOM::Transparency ) , newValue );
      }
    } // for...
    ic->UpdateCurrentViewer();
    GeometryGUI::Modified();
  } // if ( isOCC )

  ValueHasChanged();
}

//=================================================================================
// function : keyPressEvent()
// purpose  :
//=================================================================================
void GEOMToolsGUI_TransparencyDlg::keyPressEvent( QKeyEvent* e )
{
  QDialog::keyPressEvent( e );
  if ( e->isAccepted() )
    return;

  if ( e->key() == Qt::Key_F1 )
    {
      e->accept();
      ClickOnHelp();
    }
}
