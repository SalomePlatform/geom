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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : MeasureGUI_Skeleton.cxx
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header$

#include "MeasureGUI_Skeleton.h"
#include "GEOMBase.h"
#include "GEOM_Displayer.h"
#include "GeometryGUI.h"

#include "LightApp_Application.h"
#include "LightApp_SelectionMgr.h"
#include "SalomeApp_Application.h"
#include "SalomeApp_Tools.h"
#include "SUIT_MessageBox.h"
#include "SUIT_Session.h"
#include "SUIT_OverrideCursor.h"

#include <qlineedit.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qapplication.h>

//=================================================================================
// class    : MeasureGUI_Skeleton()
// purpose  : Constructs a MeasureGUI_Skeleton which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
MeasureGUI_Skeleton::MeasureGUI_Skeleton( GeometryGUI*      GUI,
					  QWidget*          parent,
                                          const char*       name )
: MeasureGUI_Skeleton_QTD( parent, name, false,
			   WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu | WDestructiveClose ),
  GEOMBase_Helper( dynamic_cast<SUIT_Desktop*>( parent ) ),
  myGeomGUI( GUI )
{

  mySelBtn = 0;
  mySelEdit = 0;
  myDisplayer = 0;
  myHelpFileName = "";
  
  if ( !name )
    setName( "MeasureGUI_Skeleton" );

  buttonClose->setText( tr( "GEOM_BUT_CLOSE" ) );
  buttonHelp->setText(tr("GEOM_BUT_HELP"));

  buttonClose->setAutoDefault( false );

  GroupMedium->close( TRUE );
  resize( 350, 0 );
}


//=================================================================================
// function : ~MeasureGUI_Skeleton()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
MeasureGUI_Skeleton::~MeasureGUI_Skeleton()
{
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
  myGeomGUI->SetActiveDialogBox((QDialog*)this);

  /* signals and slots connections */
  
  connect( buttonClose, SIGNAL( clicked() ),
          this,         SLOT( ClickOnCancel() ) );

  connect( buttonHelp,  SIGNAL( clicked() ), 
	   this,        SLOT( ClickOnHelp() ) );
  
  connect( myGeomGUI,   SIGNAL( SignalDeactivateActiveDialog() ),
           this,        SLOT  ( DeactivateActiveDialog() ) );
           
  connect( myGeomGUI,   SIGNAL( SignalCloseAllDialogs() ),
           this,        SLOT  ( ClickOnCancel() ) );

  connect( mySelEdit,   SIGNAL( returnPressed() ),
           this,        SLOT( LineEditReturnPressed() ) );
           
  connect( mySelBtn,    SIGNAL( clicked() ),
           this,        SLOT  ( SetEditCurrentArgument() ) );

  LightApp_SelectionMgr* aSel = ((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr();
  if ( aSel )
    connect( aSel, SIGNAL( currentSelectionChanged() ), 
	     this, SLOT  ( SelectionIntoArgument() ) ) ;

  /* displays Dialog */
  RadioButton1->setChecked( TRUE );

  activateSelection();
  SelectionIntoArgument();
  show();
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
  LightApp_Application* app = (LightApp_Application*)(SUIT_Session::session()->activeApplication());
  if (app) 
    app->onHelpContextModule(myGeomGUI ? app->moduleName(myGeomGUI->moduleName()) : QString(""), myHelpFileName);
  else {
		QString platform;
#ifdef WIN32
		platform = "winapplication";
#else
		platform = "application";
#endif
    SUIT_MessageBox::warn1(0, QObject::tr("WRN_WARNING"),
			   QObject::tr("EXTERNAL_BROWSER_CANNOT_SHOW_PAGE").
			   arg(app->resourceMgr()->stringValue("ExternalBrowser", platform)).arg(myHelpFileName),
			   QObject::tr("BUT_OK"));
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
  
  const QString objectUserName = mySelEdit->text();
  QWidget* thisWidget = ( QWidget* )this;
  
  if ( GEOMBase::SelectionByNameInDialogs( thisWidget, objectUserName, selectedIO() ) )
    mySelEdit->setText( objectUserName );
}


//=================================================================================
// function : DeactivateActiveDialog()
// purpose  :
//=================================================================================
void MeasureGUI_Skeleton::DeactivateActiveDialog()
{
  setEnabled( false );
  
  LightApp_SelectionMgr* aSel = ((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr();
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

  LightApp_SelectionMgr* aSel = ((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr();
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
  myObj = GEOM::GEOM_Object::_nil();
  
  Standard_Boolean testResult = Standard_False;
  GEOM::GEOM_Object_var aSelectedObject =
    GEOMBase::ConvertIOinGEOMObject( firstIObject(), testResult );
  
  if( !testResult || aSelectedObject->_is_nil() )
  {
    mySelEdit->setText( "" );
    processObject();
    erasePreview();
    return;
  }

  myObj = aSelectedObject;
  mySelEdit->setText( GEOMBase::GetName( myObj ) );
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
  LightApp_SelectionMgr* aSel = ((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr();
  if ( aSel )
    disconnect( aSel, 0, this, 0 );
  QDialog::closeEvent( e );
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void MeasureGUI_Skeleton::enterEvent(QEvent* e)
{
  if ( GroupConstructors->isEnabled() )
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
  if ( !isValid( aMess ) )
  {
    erasePreview( true );
    return;
  }

  erasePreview( false );

  try
  {
    SUIT_OverrideCursor();

    getDisplayer()->SetColor( Quantity_NOC_VIOLET );
    getDisplayer()->SetToActivate( false );
    
    if ( SALOME_Prs* aPrs = buildPrs() )
      displayPreview( aPrs );
  }
  catch( const SALOME::SALOME_Exception& e )
  {
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
  return !myObj->_is_nil();
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

  if ( e->key() == Key_F1 )
    {
      e->accept();
      ClickOnHelp();
    }
}
