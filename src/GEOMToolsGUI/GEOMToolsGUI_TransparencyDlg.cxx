//  GEOM GEOMGUI : GUI for Geometry component
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
// 
//  This library is free software; you can redistribute it and/or 
//  modify it under the terms of the GNU Lesser General Public 
//  License as published by the Free Software Foundation; either 
//  version 2.1 of the License. 
// 
//  This library is distributed in the hope that it will be useful, 
//  but WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
//  Lesser General Public License for more details. 
// 
//  You should have received a copy of the GNU Lesser General Public 
//  License along with this library; if not, write to the Free Software 
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
// 
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : GEOMBase_TransparencyDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM


#include "GEOMToolsGUI_TransparencyDlg.h"
#include "GEOMBase.h"
#include "GEOM_AISShape.hxx"
#include "GeometryGUI.h"

#include "SALOME_ListIO.hxx"
#include "SALOME_ListIteratorOfListIO.hxx"

#include <SVTK_ViewModel.h>
#include <SVTK_ViewWindow.h>
#include <SVTK_View.h>

#include <OCCViewer_ViewModel.h>
#include <OCCViewer_ViewWindow.h>

#include <SUIT_ViewManager.h>
#include <SUIT_Application.h>
#include <SUIT_Desktop.h>
#include <SUIT_MessageBox.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SUIT_OverrideCursor.h>

#include <SalomeApp_Application.h>
#include <LightApp_Application.h>
#include <LightApp_SelectionMgr.h>

#include <qframe.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qslider.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qapplication.h>
#include <qgroupbox.h>

#include <AIS_InteractiveContext.hxx>
using namespace std;

//=================================================================================
// class    : GEOMBase_TransparencyDlg()
// purpose  : Constructs a GEOMBase_SUBSHAPE which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//
//          : WARNING : this dialog is modal !
//
//=================================================================================
GEOMToolsGUI_TransparencyDlg::GEOMToolsGUI_TransparencyDlg( QWidget* parent )
  :QDialog( parent, "GEOMBase_TransparencyDlg", true, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu )
{
  resize(152, 107); 
  setCaption(tr("GEOM_TRANSPARENCY_TITLE"));
  setSizeGripEnabled(TRUE);
  QGridLayout* lay = new QGridLayout(this); 
  lay->setSpacing(6);
  lay->setMargin(11);

  /*************************************************************************/
  QGroupBox* GroupButtons = new QGroupBox( this, "GroupButtons" );
  GroupButtons->setColumnLayout(0, Qt::Vertical );
  GroupButtons->layout()->setSpacing( 0 );
  GroupButtons->layout()->setMargin( 0 );
  QGridLayout* GroupButtonsLayout = new QGridLayout( GroupButtons->layout() );
  GroupButtonsLayout->setAlignment( Qt::AlignTop );
  GroupButtonsLayout->setSpacing( 6 );
  GroupButtonsLayout->setMargin( 11 );
  
  QPushButton* buttonOk = new QPushButton( GroupButtons, "buttonOk" );
  buttonOk->setText( tr( "GEOM_BUT_OK" ) );
  buttonOk->setAutoDefault( TRUE );
  buttonOk->setDefault( TRUE );

  QPushButton* buttonHelp = new QPushButton( GroupButtons, "buttonHelp" );
  buttonHelp->setText( tr( "GEOM_BUT_HELP" ) );
  buttonHelp->setAutoDefault( TRUE );
  buttonHelp->setDefault( TRUE );
  
  GroupButtonsLayout->addWidget( buttonOk, 0, 0 );
  GroupButtonsLayout->addItem( new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum), 0, 1 );
  GroupButtonsLayout->addWidget( buttonHelp, 0, 2 );

  /*************************************************************************/
  QGroupBox* GroupC1 = new QGroupBox( this, "GroupC1" );
  GroupC1->setColumnLayout(0, Qt::Vertical );
  GroupC1->layout()->setSpacing( 0 );
  GroupC1->layout()->setMargin( 0 );
  QGridLayout* GroupC1Layout = new QGridLayout( GroupC1->layout() );
  GroupC1Layout->setAlignment( Qt::AlignTop );
  GroupC1Layout->setSpacing( 6 );
  GroupC1Layout->setMargin( 11 );
  
  QLabel* TextLabelOpaque = new QLabel( GroupC1, "TextLabelOpaque" );
  TextLabelOpaque->setText( tr( "GEOM_TRANSPARENCY_OPAQUE"  ) );
  TextLabelOpaque->setAlignment( int( QLabel::AlignLeft ) );
  GroupC1Layout->addWidget( TextLabelOpaque, 0, 0 );
  GroupC1Layout->addItem( new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ), 0, 1 );
  
  QLabel* TextLabelTransparent = new QLabel( GroupC1, "TextLabelTransparent" );
  TextLabelTransparent->setText( tr( "GEOM_TRANSPARENCY_TRANSPARENT"  ) );
  TextLabelTransparent->setAlignment( int( QLabel::AlignRight ) );
  GroupC1Layout->addWidget( TextLabelTransparent, 0, 2 );
  
  mySlider = new QSlider( 0, 10, 1, 5, Horizontal, GroupC1, "mySlider" );
  mySlider->setMinimumSize( 300, 0 );
  mySlider->setTickmarks( QSlider::Left );
  GroupC1Layout->addMultiCellWidget( mySlider, 1, 1, 0, 2 );
  /*************************************************************************/
  
  lay->addWidget(GroupC1, 0,  0);
  lay->addWidget(GroupButtons, 1, 0);
    
  /* First call valueChanged() method for initialisation               */
  /* The default value of transparency will change with the selection  */
  myFirstInit = true;
  //  mySlider->setMaxValue( 10 );
  //  mySlider->setValue( 5 ) ;
  ValueHasChanged(mySlider->value());
  
  myHelpFileName = "transparency.htm";

  // signals and slots connections : after ValueHasChanged()
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonHelp, SIGNAL(clicked()), this, SLOT(ClickOnHelp()));
  connect(mySlider, SIGNAL(valueChanged(int)), this, SLOT(ValueHasChanged(int)));
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
    SUIT_MessageBox::warn1(0, QObject::tr("WRN_WARNING"),
			   QObject::tr("EXTERNAL_BROWSER_CANNOT_SHOW_PAGE").
			   arg(app->resourceMgr()->stringValue("ExternalBrowser", "application")).arg(myHelpFileName),
			   QObject::tr("BUT_OK"));
  }
}

//=================================================================================
// function : ValueHasChanged()
// purpose  : Called when value of slider change
//          : or the first time as initilisation
//=================================================================================
void GEOMToolsGUI_TransparencyDlg::ValueHasChanged( int newValue )
{
  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  if ( !app )
    return;
  LightApp_SelectionMgr* aSelMgr = app->selectionMgr();
  if ( !aSelMgr )
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

  if ( isVTK ) {
    SVTK_ViewWindow* vtkVW = dynamic_cast<SVTK_ViewWindow*>( window );
    if ( !vtkVW )
      return;
    SVTK_View* aView = vtkVW->getView();
    if ( myFirstInit ) {	
      myFirstInit = false;
      float transp = (aView->GetTransparency(FirstIOS))*10.0;
      mySlider->setValue(int(transp));
      return;
    }

    SUIT_OverrideCursor();
    for ( SALOME_ListIteratorOfListIO It( selected ); It.More(); It.Next() ) {
      aView->SetTransparency( It.Value(), newValue/10.0 );
    }
    aView->Repaint();
  } // if ( isVTK )
	
  else if ( isOCC ) {
    GEOMBase* gb = new GEOMBase();
    Standard_Boolean found;
    Handle(GEOM_AISShape) aisShape;
    if ( myFirstInit ) {
      myFirstInit = false;
      aisShape = gb->ConvertIOinGEOMAISShape( FirstIOS, found );
      if( !found )
	return;
      float transp = (int(aisShape->Transparency() * 10.0 + 0.001));
      mySlider->setValue(int(transp));
      return;
    }
    
    SUIT_OverrideCursor();
    OCCViewer_Viewer* vm = dynamic_cast<OCCViewer_Viewer*>( window->getViewManager()->getViewModel() );
    if ( !vm )
      return;
    Handle(AIS_InteractiveContext) ic = vm->getAISContext();
    for ( SALOME_ListIteratorOfListIO It( selected ); It.More(); It.Next() ) {
      aisShape = gb->ConvertIOinGEOMAISShape( It.Value(), found );
      if ( found ) {
	ic->SetTransparency( aisShape, newValue / 10.0, false );
	ic->Redisplay( aisShape, Standard_False, Standard_True );
      }
    } // for...
    ic->UpdateCurrentViewer();
  } // if ( isOCC )
}
