// GEOM GEOMGUI : GUI for Geometry component
//
// Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
// File   : GEOMBase_Skeleton.cxx
// Author : Damien COQUERET, Open CASCADE S.A.S.
//

#include "GEOMBase_Skeleton.h"
#include "GEOMBase.h"

#include <DlgRef.h>
#include <GeometryGUI.h>

#include <SalomeApp_Application.h>
#include <LightApp_Application.h>
#include <LightApp_SelectionMgr.h>
#include <SUIT_Desktop.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SUIT_MessageBox.h>

#include <QKeyEvent>
#include <QSpinBox>
#include <QDoubleSpinBox>

//=================================================================================
// class    : GEOMBase_Skeleton()
// purpose  : Constructs a GEOMBase_Skeleton which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            true to construct a modal dialog.
//=================================================================================
GEOMBase_Skeleton::GEOMBase_Skeleton( GeometryGUI* theGeometryGUI, QWidget* parent,
				      bool modal, Qt::WindowFlags fl )
  : QDialog( parent, fl ), 
    GEOMBase_Helper( dynamic_cast<SUIT_Desktop*>( parent ) ),
    myGeomGUI( theGeometryGUI ),
    myRBGroup( 0 )
{
  setAttribute( Qt::WA_DeleteOnClose );

  setModal( modal );

  myMainFrame = new DlgRef_Skeleton( this );
  QVBoxLayout* topLayout = new QVBoxLayout( this );
  topLayout->setMargin( 0 ); topLayout->setSpacing( 0 );
  topLayout->addWidget( myMainFrame );

  myMainFrame->GroupBoxName->setTitle( tr( "GEOM_RESULT_NAME_GRP" ) );
  myMainFrame->NameLabel->setText( tr( "GEOM_RESULT_NAME_LBL" ) );

  myMainFrame->GroupBoxPublish->setTitle( tr( "GEOM_PUBLISH_RESULT_GRP" ) );
  myMainFrame->CheckBoxRestoreSS->setText( tr( "GEOM_RESTORE_SUB_SHAPES" ) );

  buttonCancel()->setText( tr( "GEOM_BUT_CLOSE" ) );
  buttonOk()->setText( tr( "GEOM_BUT_APPLY_AND_CLOSE" ) );
  buttonApply()->setText( tr( "GEOM_BUT_APPLY" ) );
  buttonHelp()->setText( tr( "GEOM_BUT_HELP" ) );

  myRBGroup = new QButtonGroup( this );
  myRBGroup->addButton( myMainFrame->RadioButton1, 0 );
  myRBGroup->addButton( myMainFrame->RadioButton2, 1 );
  myRBGroup->addButton( myMainFrame->RadioButton3, 2 );
  myRBGroup->addButton( myMainFrame->RadioButton4, 3 );
  myRBGroup->addButton( myMainFrame->RadioButton5, 4 );

  connect( myRBGroup, SIGNAL( buttonClicked( int ) ), this, SIGNAL( constructorsClicked( int ) ) );

  Init();
}

//=================================================================================
// function : ~GEOMBase_Skeleton()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
GEOMBase_Skeleton::~GEOMBase_Skeleton()
{
  if ( myGeomGUI )
    myGeomGUI->SetActiveDialogBox( 0 );
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void GEOMBase_Skeleton::Init()
{
  SalomeApp_Application* app = (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() );
  if ( !myGeomGUI && app )
    myGeomGUI = dynamic_cast<GeometryGUI*>( app->module( "Geometry" ) );

  /* init variables */
  if ( myGeomGUI )
    myGeomGUI->SetActiveDialogBox( this );

  /* signals and slots connections */
  connect( buttonCancel(), SIGNAL( clicked() ), this, SLOT( ClickOnCancel() ) );
  if ( myGeomGUI ) {
    connect( myGeomGUI, SIGNAL( SignalDeactivateActiveDialog() ), this, SLOT( DeactivateActiveDialog() ) );
    connect( myGeomGUI, SIGNAL( SignalCloseAllDialogs() ),        this, SLOT( ClickOnCancel() ) );
  }

  // connect help button on a private slot that displays help information
  connect( buttonHelp(), SIGNAL( clicked() ), this, SLOT( ClickOnHelp() ) );

  /* displays Dialog */
  myMainFrame->RadioButton1->setChecked( true );
  myMainFrame->RadioButton4->hide();
  myMainFrame->RadioButton5->hide();

  myMainFrame->CheckBoxRestoreSS->setChecked( false );
  myMainFrame->GroupBoxPublish->hide();
}

void GEOMBase_Skeleton::initSpinBox( QSpinBox* spinBox, 
				     int min,  int max, int step )
{
  spinBox->setRange( min, max );
  spinBox->setSingleStep( step );
}

void GEOMBase_Skeleton::initSpinBox( QDoubleSpinBox* spinBox, 
				     double min,  double max, 
				     double step, int decimals )
{
  spinBox->setDecimals( decimals ); // it's necessary to set decimals before the range setting,
                                    // by default Qt rounds boundaries to 2 decimals at setRange
  spinBox->setRange( min, max );
  spinBox->setSingleStep( step );
}

//=================================================================================
// function : ClickOnCancel()
// purpose  :
//=================================================================================
void GEOMBase_Skeleton::ClickOnCancel()
{
  close();
}

//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void GEOMBase_Skeleton::LineEditReturnPressed()
{
  if ( !myEditCurrentArgument )
    return;

  /* User name of object input management                          */
  /* If successfull the selection is changed and signal emitted... */
  /* so SelectionIntoArgument() is automatically called.           */
  const QString objectUserName = myEditCurrentArgument->text();
  QWidget* thisWidget = (QWidget*)this;
  
  if ( GEOMBase::SelectionByNameInDialogs( thisWidget, objectUserName, selectedIO() ) )
     myEditCurrentArgument->setText( objectUserName );
}

//=================================================================================
// function : DeactivateActiveDialog()
// purpose  :
//=================================================================================
void GEOMBase_Skeleton::DeactivateActiveDialog()
{
  this->setEnabled( false );
  globalSelection();
  if ( myGeomGUI ) {
    myGeomGUI->SetActiveDialogBox( 0 );
    disconnect( myGeomGUI->getApp()->selectionMgr(), 0, this, 0 );
  }
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void GEOMBase_Skeleton::ActivateThisDialog()
{
  /* Emit a signal to deactivate the active dialog */
  if ( myGeomGUI ) myGeomGUI->EmitSignalDeactivateDialog();
  this->setEnabled( true );
  if ( myGeomGUI ) myGeomGUI->SetActiveDialogBox( (QDialog*)this );
  return;
}

//=================================================================================
// function : closeEvent()
// purpose  : same than click on cancel button
//=================================================================================
void GEOMBase_Skeleton::closeEvent( QCloseEvent* e )
{
  if ( myGeomGUI ) {
    disconnect( myGeomGUI->getApp()->selectionMgr(), 0, this, 0 );
    myGeomGUI->getApp()->updateActions();
  }
  QDialog::closeEvent( e );
}

//=================================================================================
// function : initName()
// purpose  : initialize the Name field with a string "thePrefix_X" (Vertex_3)
//=================================================================================
void GEOMBase_Skeleton::initName( const QString& thePrefix )
{
  if ( !thePrefix.isNull() )
    setPrefix( thePrefix );
  myMainFrame->ResultName->setText( GEOMBase::GetDefaultName( getPrefix() ) );
}

//=================================================================================
// function : getNewObjectName()
// purpose  : returns contents of Name field
//=================================================================================
QString GEOMBase_Skeleton::getNewObjectName() const
{
  return myMainFrame->ResultName->text();
}

//=================================================================================
// function : getConstructorId()
// purpose  :
//=================================================================================
int GEOMBase_Skeleton::getConstructorId() const
{
  if ( myRBGroup )
    return myRBGroup->checkedId();
  return -1;
}

void GEOMBase_Skeleton::setConstructorId( const int id )
{
  if ( myRBGroup && myRBGroup->button( id ) )
    myRBGroup->button( id )->setChecked( true );
}

//=================================================================================
// function : ClickOnHelp()
// purpose  :
//=================================================================================
void GEOMBase_Skeleton::ClickOnHelp()
{
  LightApp_Application* app = (LightApp_Application*)( SUIT_Session::session()->activeApplication() );
  if ( app ) 
    app->onHelpContextModule( myGeomGUI ? app->moduleName( myGeomGUI->moduleName() ) : QString(""), myHelpFileName );
  else {
    QString platform;
#ifdef WIN32
    platform = "winapplication";
#else
    platform = "application";
#endif
    SUIT_MessageBox::warning( 0, QObject::tr( "WRN_WARNING" ),
			      QObject::tr( "EXTERNAL_BROWSER_CANNOT_SHOW_PAGE" ).
			      arg( app->resourceMgr()->stringValue( "ExternalBrowser", platform ) ).arg( myHelpFileName ),
			      QObject::tr( "BUT_OK" ) );
  }
}

//=================================================================================
//  function : setHelpFileName()
//  purpose  : set name for help file html
//=================================================================================
void GEOMBase_Skeleton::setHelpFileName( const QString& theName )
{
  myHelpFileName = theName;
}

DlgRef_Skeleton* GEOMBase_Skeleton::mainFrame()
{
  return myMainFrame;
}

QWidget* GEOMBase_Skeleton::centralWidget()
{
  return myMainFrame->GroupMedium;
}

QPushButton* GEOMBase_Skeleton::buttonCancel() const
{
  return myMainFrame->buttonCancel;
}

QPushButton* GEOMBase_Skeleton::buttonOk() const
{
  return myMainFrame->buttonOk;
}

QPushButton* GEOMBase_Skeleton::buttonApply() const
{
  return myMainFrame->buttonApply;
}

QPushButton* GEOMBase_Skeleton::buttonHelp() const
{
  return myMainFrame->buttonHelp;
}

//=================================================================================
// function : keyPressEvent()
// purpose  :
//=================================================================================
void GEOMBase_Skeleton::keyPressEvent( QKeyEvent* e )
{
  QDialog::keyPressEvent( e );
  if ( e->isAccepted() )
    return;

  if ( e->key() == Qt::Key_F1 ) {
    e->accept();
    ClickOnHelp();
  }
}
