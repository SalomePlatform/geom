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
// File   : MeasureGUI_Skeleton.cxx
// Author : Damine COQUERET, Open CASCADE S.A.S.
//
#include "MeasureGUI_Skeleton.h"
#include "MeasureGUI_Widgets.h"

#include <GEOMBase.h>
#include <GEOM_Displayer.h>
#include <GeometryGUI.h>

#include <LightApp_SelectionMgr.h>
#include <SalomeApp_Application.h>
#include <SalomeApp_Tools.h>
#include <SUIT_Desktop.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_MessageBox.h>
#include <SUIT_Session.h>
#include <SUIT_OverrideCursor.h>

#include <QKeyEvent>

//=================================================================================
// class    : MeasureGUI_Skeleton()
// purpose  : Constructs a MeasureGUI_Skeleton which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
MeasureGUI_Skeleton::MeasureGUI_Skeleton( GeometryGUI* GUI, QWidget* parent,
                                          bool modal, Qt::WindowFlags f )
  : QDialog( parent, f ),
    GEOMBase_Helper( dynamic_cast<SUIT_Desktop*>( parent ) ),
    myGeomGUI( GUI )
{
  myMainFrame = new MeasureGUI_SkeletonBox( this );
  QVBoxLayout* topLayout = new QVBoxLayout( this );
  topLayout->setMargin( 9 ); topLayout->setSpacing( 0 );
  topLayout->addWidget( myMainFrame );

  setAttribute( Qt::WA_DeleteOnClose );
  setModal( modal );
  mySelBtn = 0;
  mySelEdit = 0;
  myDisplayer = 0;
  myHelpFileName = "";
  
  buttonClose()->setText( tr( "GEOM_BUT_CLOSE" ) );
  buttonHelp()->setText( tr( "GEOM_BUT_HELP" ) );
}


//=================================================================================
// function : ~MeasureGUI_Skeleton()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
MeasureGUI_Skeleton::~MeasureGUI_Skeleton()
{
  if ( myGeomGUI )
    myGeomGUI->SetActiveDialogBox( 0 );
  delete myDisplayer;
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void MeasureGUI_Skeleton::Init()
{
  /* init variables */
  if ( myGeomGUI )
    myGeomGUI->SetActiveDialogBox( (QDialog*)this );

  /* signals and slots connections */
  
  connect( buttonClose(), SIGNAL( clicked() ),
           this,          SLOT( ClickOnCancel() ) );

  connect( buttonHelp(),  SIGNAL( clicked() ), 
           this,          SLOT( ClickOnHelp() ) );
  
  if ( myGeomGUI ) {
    connect( myGeomGUI,   SIGNAL( SignalDeactivateActiveDialog() ),
             this,        SLOT  ( DeactivateActiveDialog() ) );
    
    connect( myGeomGUI,   SIGNAL( SignalCloseAllDialogs() ),
             this,        SLOT  ( ClickOnCancel() ) );
  }
  
  if ( mySelEdit ) {
    connect( mySelEdit,   SIGNAL( returnPressed() ),
             this,        SLOT( LineEditReturnPressed() ) );
  }
   
  if ( mySelBtn ) {
    connect( mySelBtn,    SIGNAL( clicked() ),
             this,        SLOT  ( SetEditCurrentArgument() ) );
  }

  LightApp_SelectionMgr* aSel = ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr();
  if ( aSel ) {
    connect( aSel, SIGNAL( currentSelectionChanged() ), 
             this, SLOT  ( SelectionIntoArgument() ) ) ;
  }

  /* displays Dialog */
  mainFrame()->RadioButton1->setChecked( true );

  activateSelection();
  SelectionIntoArgument();
}


//=================================================================================
// function : ClickOnCancel()
// purpose  :
//=================================================================================
void MeasureGUI_Skeleton::ClickOnCancel()
{
  close();
}

//=================================================================================
// function : ClickOnHelp()
// purpose  :
//=================================================================================
void MeasureGUI_Skeleton::ClickOnHelp()
{
  LightApp_Application* app = (LightApp_Application*)( SUIT_Session::session()->activeApplication() );
  if ( app ) {
    app->onHelpContextModule( myGeomGUI ? app->moduleName( myGeomGUI->moduleName() ) : QString( "" ), myHelpFileName );
  }
  else {
    QString platform;
#ifdef WIN32
    platform = "winapplication";
#else
    platform = "application";
#endif
    SUIT_MessageBox::warning( this, tr( "WRN_WARNING" ),
                              tr( "EXTERNAL_BROWSER_CANNOT_SHOW_PAGE" ).
                              arg( app->resourceMgr()->stringValue( "ExternalBrowser", platform ) ).arg( myHelpFileName ) );
  }
}

//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void MeasureGUI_Skeleton::LineEditReturnPressed()
{
  /* User name of object input management                          */
  /* If successfull the selection is changed and signal emitted... */
  /* so SelectionIntoArgument() is automatically called.           */
  
  if ( mySelEdit ) {
    const QString objectUserName = mySelEdit->text();
    QWidget* thisWidget = ( QWidget* )this;
    
    LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
    SALOME_ListIO aSelList;
    aSelMgr->selectedObjects(aSelList);

    if (GEOMBase::SelectionByNameInDialogs(thisWidget, objectUserName, aSelList))
      mySelEdit->setText(objectUserName);
  }
}


//=================================================================================
// function : DeactivateActiveDialog()
// purpose  :
//=================================================================================
void MeasureGUI_Skeleton::DeactivateActiveDialog()
{
  setEnabled( false );
  
  LightApp_SelectionMgr* aSel = ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr();
  if ( aSel )
    disconnect( aSel, 0, this, 0 );
  
  myGeomGUI->SetActiveDialogBox( 0 );

  globalSelection();
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void MeasureGUI_Skeleton::ActivateThisDialog()
{
  myGeomGUI->EmitSignalDeactivateDialog();
  
  setEnabled( true );
  
  myGeomGUI->SetActiveDialogBox( ( QDialog* )this );

  LightApp_SelectionMgr* aSel = ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr();
  if ( aSel )
    connect( aSel, SIGNAL( currentSelectionChanged() ), 
             this, SLOT  ( SelectionIntoArgument() ) ) ;
  
  redisplayPreview();
  activateSelection();
}

//=================================================================================
// function : SetEditCurrentArgument
// purpose  :
//=================================================================================
void MeasureGUI_Skeleton::SetEditCurrentArgument()
{
  mySelEdit->setFocus();
  SelectionIntoArgument();
}

//=================================================================================
// function : SelectionIntoArgument
// purpose  :
//=================================================================================
void MeasureGUI_Skeleton::SelectionIntoArgument()
{
  myObj.nullify();

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  if (aSelList.Extent() > 0) {
    myObj = GEOMBase::ConvertIOinGEOMObject( aSelList.First() );
  }

  if ( !myObj ) {
    mySelEdit->setText("");
    processObject();
    erasePreview();
    return;
  }

  mySelEdit->setText(GEOMBase::GetName(myObj.get()));
  processObject();
  redisplayPreview();
}

//=================================================================================
// function : processObject
// purpose  :
//=================================================================================
void MeasureGUI_Skeleton::processObject()
{  
}


//=================================================================================
// function : closeEvent
// purpose  :
//=================================================================================
void MeasureGUI_Skeleton::closeEvent( QCloseEvent* e )
{
  LightApp_SelectionMgr* aSel = ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr();
  if ( aSel )
    disconnect( aSel, 0, this, 0 );
  QDialog::closeEvent( e );
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void MeasureGUI_Skeleton::enterEvent( QEvent* )
{
  if ( mainFrame()->GroupConstructors->isEnabled() )
    return;
    
  ActivateThisDialog();
}

//=================================================================================
// function : buildPrs
// purpose  : Build presentation for "preview"
//=================================================================================
SALOME_Prs* MeasureGUI_Skeleton::buildPrs()
{
  return 0;
}

//=================================================================================
// function : closeEvent()
// purpose  : Build presentation for "preview"
//=================================================================================
void MeasureGUI_Skeleton::redisplayPreview()
{
  QString aMess;
  if ( !isValid( aMess ) ) {
    erasePreview( true );
    return;
  }

  erasePreview( false );

  try {
    SUIT_OverrideCursor wc;

    getDisplayer()->SetColor( Quantity_NOC_VIOLET );
    getDisplayer()->SetToActivate( false );
    
    if ( SALOME_Prs* aPrs = buildPrs() )
      displayPreview( aPrs );
  }
  catch ( const SALOME::SALOME_Exception& e ) {
    SalomeApp_Tools::QtCatchCorbaException( e );
  }
  
}

//=================================================================================
// function : activateSelection
// purpose  : 
//=================================================================================
void MeasureGUI_Skeleton::activateSelection()
{
  globalSelection( GEOM_ALLSHAPES );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool MeasureGUI_Skeleton::isValid( QString& )
{
  return myObj;
}

//================================================================
// Function : getDisplayer
// Purpose  :
//================================================================
GEOM_Displayer* MeasureGUI_Skeleton::getDisplayer()
{
  if ( !myDisplayer )
    myDisplayer = new GEOM_Displayer( getStudy() );
  return myDisplayer;
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr MeasureGUI_Skeleton::createOperation()
{
  return getGeomEngine()->GetIMeasureOperations( getStudyId() );
}

//=================================================================================
// function : keyPressEvent()
// purpose  :
//=================================================================================
void MeasureGUI_Skeleton::keyPressEvent( QKeyEvent* e )
{
  QDialog::keyPressEvent( e );
  if ( e->isAccepted() )
    return;

  if ( e->key() == Qt::Key_F1 ) {
    e->accept();
    ClickOnHelp();
  }
}

MeasureGUI_SkeletonBox* MeasureGUI_Skeleton::mainFrame() const
{
  return myMainFrame;
}

QWidget* MeasureGUI_Skeleton::centralWidget() const
{
  return mainFrame()->GroupMedium;
}

QPushButton* MeasureGUI_Skeleton::buttonClose() const
{
  return mainFrame()->buttonClose;
}

QPushButton* MeasureGUI_Skeleton::buttonHelp() const
{
  return mainFrame()->buttonHelp;
}
