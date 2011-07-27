//  Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

// GEOM GEOMGUI : GUI for Geometry component
// File   : RepairGUI_FreeBoundDlg.cxx
// Author : Sergey LITONIN, Open CASCADE S.A.S. (sergey.litonin@opencascade.com)
//
#include "RepairGUI_FreeBoundDlg.h"

#include <GEOMBase.h>
#include <GeometryGUI.h>
#include <GEOM_Displayer.h>
#include <GEOMImpl_Types.hxx>

#include <SalomeApp_Application.h>
#include <LightApp_Application.h>
#include <LightApp_SelectionMgr.h>
#include <SUIT_MessageBox.h>
#include <SUIT_Desktop.h>
#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>

#include <TColStd_MapOfInteger.hxx>

#include <QLineEdit>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QKeyEvent>

#define SPACING 6
#define MARGIN  9

/*!
  Class       : RepairGUI_FreeBoundDlg
  Description : Dialog for displaying free boundaries of selected face, shell or solid
*/

//=================================================================================
// function : RepairGUI_FreeBoundDlg
// purpose  : Constructor
//=================================================================================
RepairGUI_FreeBoundDlg::RepairGUI_FreeBoundDlg( GeometryGUI* theGUI, QWidget* theParent )
  : QDialog( theParent, false ),
    GEOMBase_Helper( dynamic_cast<SUIT_Desktop*>( theParent ) ),
    myGeomGUI( theGUI )
{
  setAttribute( Qt::WA_DeleteOnClose );

  setWindowTitle( tr( "CAPTION" ) );

  QPixmap iconSelect( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );
  
  QGroupBox* aMainGrp = new QGroupBox( tr( "FREE_BOUND" ), this );
  
  QLabel* lab = new QLabel( tr( "GEOM_OBJECT" ), aMainGrp );
  QPushButton* btn = new QPushButton( aMainGrp );
  btn->setIcon( iconSelect );
  myEdit = new QLineEdit( aMainGrp );
  myEdit->setReadOnly( true );
  myEdit->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );

  myClosedLbl = new QLabel( tr( "NUMBER_CLOSED" ), aMainGrp );
  myOpenLbl   = new QLabel( tr( "NUMBER_OPEN" ),   aMainGrp );
  myClosedLbl->setMinimumWidth( 150 );
  myOpenLbl->setMinimumWidth( 150 );
  
  QGridLayout* aMainGrpLayout = new QGridLayout( aMainGrp );
  aMainGrpLayout->setMargin( MARGIN ); aMainGrpLayout->setSpacing( SPACING );
  aMainGrpLayout->addWidget( lab,    0, 0 );
  aMainGrpLayout->addWidget( btn,    0, 1 );
  aMainGrpLayout->addWidget( myEdit, 0, 2 );
  aMainGrpLayout->addWidget( myClosedLbl, 1, 0, 1, 3 );
  aMainGrpLayout->addWidget( myOpenLbl,   2, 0, 1, 3 );

  QFrame* aFrame = new QFrame( this );
  aFrame->setFrameStyle( QFrame::Box | QFrame::Sunken );
  QPushButton* aCloseBtn = new QPushButton( tr( "GEOM_BUT_CLOSE" ), aFrame );
  QPushButton* aHelpBtn = new QPushButton( tr( "GEOM_BUT_HELP" ), aFrame );
  QHBoxLayout* aBtnLay = new QHBoxLayout( aFrame );
  aBtnLay->setMargin( MARGIN ); aBtnLay->setSpacing( SPACING );
  aBtnLay->addWidget( aCloseBtn );
  aBtnLay->addSpacing( SPACING ); aBtnLay->addStretch();
  aBtnLay->addWidget( aHelpBtn );

  QVBoxLayout* aLay = new QVBoxLayout( this );
  aLay->setSpacing( SPACING );
  aLay->setMargin( MARGIN );
  aLay->addWidget( aMainGrp );
  aLay->addWidget( aFrame );

  myHelpFileName = "using_measurement_tools_page.html#boundaries_anchor";

  connect( aCloseBtn, SIGNAL( clicked() ), SLOT( onClose() ) );
  connect( aHelpBtn,  SIGNAL( clicked() ), SLOT( onHelp() ) );

  Init();
}

//=================================================================================
// function : ~RepairGUI_FreeBoundDlg
// purpose  : Destructor
//=================================================================================
RepairGUI_FreeBoundDlg::~RepairGUI_FreeBoundDlg()
{
}

//=================================================================================
// function : onClose
// purpose  : SLOT. Called when "close" button pressed. Close dialog
//=================================================================================
void RepairGUI_FreeBoundDlg::onClose()
{
  globalSelection();
  disconnect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr(), 0, this, 0 );
  myGeomGUI->SetActiveDialogBox( 0 );
  reject();
  erasePreview();
}

//=================================================================================
// function : onHelp()
// purpose  :
//=================================================================================
void RepairGUI_FreeBoundDlg::onHelp()
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
    SUIT_MessageBox::warning( this, 
                              tr( "WRN_WARNING" ), 
                              tr( "EXTERNAL_BROWSER_CANNOT_SHOW_PAGE").
                              arg( app->resourceMgr()->stringValue( "ExternalBrowser", 
                                                                    platform ) ).arg( myHelpFileName ) );
  }
}

//=================================================================================
// function : onDeactivate
// purpose  : Deactivate this dialog
//=================================================================================
void RepairGUI_FreeBoundDlg::onDeactivate()
{
  setEnabled( false );
  globalSelection();
  disconnect( myGeomGUI->getApp()->selectionMgr(), 0, this, 0 );
  myGeomGUI->SetActiveDialogBox( 0 );
}

//=================================================================================
// function : onActivate
// purpose  : Activate this dialog
//=================================================================================
void RepairGUI_FreeBoundDlg::onActivate()
{
  myGeomGUI->EmitSignalDeactivateDialog();
  setEnabled( true );
  myGeomGUI->SetActiveDialogBox( this );
  connect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr(), 
           SIGNAL( currentSelectionChanged() ), SLOT( onSelectionDone() ) );
  activateSelection();
  onSelectionDone();
}

//=================================================================================
// function : onSelectionDone
// purpose  : SLOT. Called when selection changed.
//=================================================================================
void RepairGUI_FreeBoundDlg::onSelectionDone()
{
  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  if ( aSelList.Extent() != 1 )
    return;

  Standard_Boolean isOk = Standard_False;
  GEOM::GEOM_Object_var anObj =
    GEOMBase::ConvertIOinGEOMObject( aSelList.First(), isOk );

  if ( !isOk || anObj->_is_nil() || !GEOMBase::IsShape( anObj ) )
    return;
  else {
    myObj = anObj;
    displayPreview( false, true, true, 3 );
  }
}

//=================================================================================
// function : Init
// purpose  : Initialize dialog fields
//=================================================================================
void RepairGUI_FreeBoundDlg::Init()
{
  myNbClosed = myNbOpen = 0;
  myObj = GEOM::GEOM_Object::_nil();

  connect( myGeomGUI, SIGNAL( SignalDeactivateActiveDialog() ), SLOT  ( onDeactivate() ) );
  connect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr(), 
           SIGNAL( currentSelectionChanged() ), SLOT( onSelectionDone() ) );

  activateSelection();
  onSelectionDone();
}

//=================================================================================
// function : enterEvent
// purpose  : Activate dialog
//=================================================================================
void RepairGUI_FreeBoundDlg::enterEvent( QEvent* )
{
  onActivate();
}

//=================================================================================
// function : closeEvent
// purpose  : Close dialog
//=================================================================================
void RepairGUI_FreeBoundDlg::closeEvent( QCloseEvent* )
{
  onClose();
}

//=================================================================================
// function : activateSelection
// purpose  : activate selection of faces, shells, and solids
//=================================================================================
void RepairGUI_FreeBoundDlg::activateSelection()
{
  TColStd_MapOfInteger aMap;
  aMap.Add( GEOM_FACE );
  aMap.Add( GEOM_SHELL );
  aMap.Add( GEOM_SOLID );
  aMap.Add( GEOM_COMPOUND );
  globalSelection( aMap );
}

//=================================================================================
// function : createOperation
// purpose  : Create operation
//=================================================================================
GEOM::GEOM_IOperations_ptr RepairGUI_FreeBoundDlg::createOperation()
{
  return getGeomEngine()->GetIHealingOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool RepairGUI_FreeBoundDlg::isValid( QString& )
{
  return !myObj->_is_nil();
}

//=================================================================================
// function : execute
// purpose  : Get free boundaries
//=================================================================================
bool RepairGUI_FreeBoundDlg::execute( ObjectList& objects )
{
  if ( !IsPreview() || myObj->_is_nil() )
    return false;

  GEOM::ListOfGO_var aClosed, anOpen;

  GEOM::GEOM_IHealingOperations_var anOper = GEOM::GEOM_IHealingOperations::_narrow( getOperation() );
  bool result = anOper->GetFreeBoundary( myObj, aClosed, anOpen );

  if ( result ) {
    myNbClosed = aClosed->length();
    myNbOpen = anOpen->length();
    int i;
    for ( i = 0; i < myNbClosed; i++ )
      objects.push_back( aClosed[i]._retn() );
    for ( i = 0; i < myNbOpen; i++ )
      objects.push_back( anOpen[i]._retn() );

    myEdit->setText( GEOMBase::GetName( myObj ) );
    QString aLabelText = tr( "NUMBER_CLOSED" ) + QString( "%1" ).arg( myNbClosed );
    myClosedLbl->setText( aLabelText );
    aLabelText = tr( "NUMBER_OPEN" ) + QString( "%1" ).arg( myNbOpen );
    myOpenLbl->setText( aLabelText );
  }
  else {
    myEdit->setText( GEOMBase::GetName( myObj ) );
    myClosedLbl->setText( tr( "NUMBER_CLOSED" ) );
    myOpenLbl->setText( tr( "NUMBER_OPEN" ) );
  }

  return result;
}

//=================================================================================
// function : keyPressEvent()
// purpose  :
//=================================================================================
void RepairGUI_FreeBoundDlg::keyPressEvent( QKeyEvent* e )
{
  QDialog::keyPressEvent( e );
  if ( e->isAccepted() )
    return;

  if ( e->key() == Qt::Key_F1 ) {
    e->accept();
    onHelp();
  }
}
