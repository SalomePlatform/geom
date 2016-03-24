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
// File   : RepairGUI_ChangeOrientationDlg.cxx
// Author : Sergey KUUL, Open CASCADE S.A.S. (sergey.kuul@opencascade.com)
//
#include "RepairGUI_ChangeOrientationDlg.h"

#include "DlgRef.h"
#include "GeometryGUI.h"
#include "GEOMBase.h"
#include "RepairGUI.h"

#include <LightApp_SelectionMgr.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SalomeApp_Application.h>
#include <TopAbs.hxx>

#include "GEOMImpl_Types.hxx"


//=================================================================================
// class    : RepairGUI_ChangeOrientationDlg()
// purpose  : Constructs a RepairGUI_ChangeOrientationDlg  which is a child of 'parent',
//            with the name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
RepairGUI_ChangeOrientationDlg::RepairGUI_ChangeOrientationDlg( GeometryGUI* theGeometryGUI,
                                                                QWidget* parent,
                                                                bool modal )
  : GEOMBase_Skeleton( theGeometryGUI, parent, modal )
{
  //QPixmap image0(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_DLG_SUPRESS_FACE")));
  QPixmap image0( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_CHANGE_ORIENTATION" ) ));
  QPixmap image1( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );
    
  setWindowTitle( tr( "GEOM_CHANGE_ORIENTATION_TITLE" ) );

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle( tr( "GEOM_CHANGE_ORIENTATION_TITLE" ) );
  mainFrame()->RadioButton1->setIcon( image0 );
  mainFrame()->RadioButton2->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton3->close();

  GroupPoints = new DlgRef_1Sel1Check( centralWidget() );
  GroupPoints->GroupBox1->setTitle( tr( "GEOM_CHANGE_ORIENTATION" ) );
  GroupPoints->TextLabel1->setText( tr( "GEOM_SELECTED_SHAPE" ) );
  GroupPoints->CheckButton1->setText( tr( "GEOM_CREATE_COPY" ) );
  GroupPoints->PushButton1->setIcon( image1 );
//  GroupPoints->LineEdit1->setReadOnly( true );
  
  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( GroupPoints );

  setHelpFileName( "change_orientation_operation_page.html" );

  Init();
}


//=================================================================================
// function : ~RepairGUI_ChangeOrientationDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
RepairGUI_ChangeOrientationDlg::~RepairGUI_ChangeOrientationDlg()
{
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void RepairGUI_ChangeOrientationDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupPoints->LineEdit1;

  GroupPoints->CheckButton1->setChecked( true );

  myOkObject = false;

  /* signals and slots connections */
  connect( buttonOk(),    SIGNAL( clicked() ), this, SLOT( ClickOnOk() ) );
  connect( buttonApply(), SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );

  connect( GroupPoints->PushButton1,  SIGNAL( clicked() ),       this, SLOT( SetEditCurrentArgument() ) );
  connect( GroupPoints->LineEdit1,    SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );

  connect( GroupPoints->CheckButton1, SIGNAL( toggled( bool ) ), this, SLOT( CreateCopyModeChanged( bool ) ) );

  connect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr(), 
           SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );

  initName( tr( "CHANGE_ORIENTATION_NEW_OBJ_NAME" ) );
  resize(100,100);
  SelectionIntoArgument();
}


//=================================================================================
// function : ClickOnOk()
// purpose  : Same than click on apply but close this dialog.
//=================================================================================
void RepairGUI_ChangeOrientationDlg::ClickOnOk()
{
  setIsApplyAndClose( true );
  if ( ClickOnApply() )
    ClickOnCancel();
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool RepairGUI_ChangeOrientationDlg::ClickOnApply()
{
//  if ( !onAccept() )
  if ( !onAccept( GroupPoints->CheckButton1->isChecked() ) )
    return false;

  initName();

  myEditCurrentArgument->setText( "" );
  
  return true;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void RepairGUI_ChangeOrientationDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText( "" );
  QString aName;
  
  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  if ( myEditCurrentArgument == GroupPoints->LineEdit1 ) {
    if ( aSelList.Extent() != 1 ) {
      if ( myEditCurrentArgument == GroupPoints->LineEdit1 )
        myOkObject = false;
      return;
    }
  }

  // nbSel == 1
  GEOM::GEOM_Object_ptr aSelectedObject =
    GEOMBase::ConvertIOinGEOMObject( aSelList.First() );
  
  if ( CORBA::is_nil( aSelectedObject ) )
    return;

  if ( myEditCurrentArgument == GroupPoints->LineEdit1 ) {
    myObject = aSelectedObject;
    myOkObject = true;
  }

  myEditCurrentArgument->setText( GEOMBase::GetName( aSelectedObject ) );
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void RepairGUI_ChangeOrientationDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  if ( send == GroupPoints->PushButton1 ) {
    GroupPoints->LineEdit1->setFocus();
    myEditCurrentArgument = GroupPoints->LineEdit1;
  }
  SelectionIntoArgument();
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void RepairGUI_ChangeOrientationDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if ( send == GroupPoints->LineEdit1 ) {
    myEditCurrentArgument = GroupPoints->LineEdit1;
    GEOMBase_Skeleton::LineEditReturnPressed();
  }
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void RepairGUI_ChangeOrientationDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr(), 
           SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );
}


//=================================================================================
// function : enterEvent()
// purpose  : Mouse enter onto the dialog to activate it
//=================================================================================
void RepairGUI_ChangeOrientationDlg::enterEvent( QEvent* )
{
  if ( !mainFrame()->GroupConstructors->isEnabled() )
    ActivateThisDialog();
}


//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr RepairGUI_ChangeOrientationDlg::createOperation()
{
  return getGeomEngine()->GetIHealingOperations( getStudyId() );
}


//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool RepairGUI_ChangeOrientationDlg::isValid( QString& )
{
  return myOkObject;
}


//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool RepairGUI_ChangeOrientationDlg::execute( ObjectList& objects )
{
  bool toCreateCopy = GroupPoints->CheckButton1->isChecked();

  GEOM::GEOM_Object_var anObj;
  GEOM::GEOM_IHealingOperations_var anOper = GEOM::GEOM_IHealingOperations::_narrow( getOperation() );
  if ( toCreateCopy ) {
    anObj = anOper->ChangeOrientationCopy( myObject );
  }
  else {
    anObj = anOper->ChangeOrientation( myObject );
  }

  if ( !anObj->_is_nil() )
  {
    if ( !IsPreview() )
      RepairGUI::ShowStatistics( anOper, this );
    objects.push_back( anObj._retn() );
  }

  return true;
}


//=================================================================================
// function :  CreateCopyModeChanged()
// purpose  :
//=================================================================================
void RepairGUI_ChangeOrientationDlg::CreateCopyModeChanged( bool isCreateCopy )
{
  mainFrame()->GroupBoxName->setEnabled( isCreateCopy );
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> RepairGUI_ChangeOrientationDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res;
  if ( GroupPoints->CheckButton1->isChecked() ) {
    GEOM::GeomObjPtr aGeomObjPtr(myObject);
    res << aGeomObjPtr;
  }
  return res;
}
