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
// File   : BlocksGUI_PropagateDlg.cxx
// Author : Vladimir KLYACHIN, Open CASCADE S.A.S. (vladimir.klyachin@opencascade.com)
//
#include "BlocksGUI_PropagateDlg.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>
#include <GEOMImpl_Types.hxx>

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>

#include <TColStd_MapOfInteger.hxx>

//=================================================================================
// class    : BlocksGUI_PropagateDlg()
// purpose  : Constructs a BlocksGUI_PropagateDlg  which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
BlocksGUI_PropagateDlg::BlocksGUI_PropagateDlg( GeometryGUI* theGeometryGUI, QWidget* parent )
  : GEOMBase_Skeleton( theGeometryGUI, parent )
{
  QPixmap image0( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_PROPAGATE" ) ) );
  QPixmap image1( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );

  setWindowTitle( tr( "GEOM_PROPAGATE_TITLE" ) );

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle( tr( "GEOM_PROPAGATE_TITLE" ) );
  mainFrame()->RadioButton1->setIcon( image0 );
  mainFrame()->RadioButton2->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton3->close();

  myGrp = new DlgRef_1Sel( centralWidget() );
  myGrp->GroupBox1->setTitle( tr( "GEOM_SELECTED_SHAPE" ) );
  myGrp->TextLabel1->setText( tr( "GEOM_OBJECT" ) );
  myGrp->PushButton1->setIcon( image1 );
  myGrp->LineEdit1->setReadOnly( true );

  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( myGrp );

  /***************************************************************/

  setHelpFileName( "propagate_operation_page.html" );

  Init();
}

//=================================================================================
// function : ~BlocksGUI_PropagateDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
BlocksGUI_PropagateDlg::~BlocksGUI_PropagateDlg()
{
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BlocksGUI_PropagateDlg::Init()
{
  /* init variables */

  myObject = GEOM::GEOM_Object::_nil();
  mainFrame()->ResultName->setText( "" );
  mainFrame()->GroupBoxName->hide();

  //myGeomGUI->SetState( 0 );

  /* signals and slots connections */
  connect( buttonOk(),    SIGNAL( clicked() ), this, SLOT( ClickOnOk() ) );
  connect( buttonApply(), SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );

  connect( myGrp->PushButton1, SIGNAL( clicked() ),       this, SLOT( SetEditCurrentArgument() ) );
  connect( myGrp->LineEdit1,   SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );

  activateSelection();
}


//=================================================================================
// function : ClickOnOk()
// purpose  : Same than click on apply but close this dialog.
//=================================================================================
void BlocksGUI_PropagateDlg::ClickOnOk()
{
  setIsApplyAndClose( true );
  if ( ClickOnApply() )
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool BlocksGUI_PropagateDlg::ClickOnApply()
{
  if ( !onAccept() )
    return false;

  initName();

  myGrp->LineEdit1->setText( "" );
  myObject = GEOM::GEOM_Object::_nil();

  activateSelection();

  return true;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection
//=================================================================================
void BlocksGUI_PropagateDlg::SelectionIntoArgument()
{
  myGrp->LineEdit1->setText( "" );
  myObject = GEOM::GEOM_Object::_nil();

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  if (aSelList.Extent() == 1) {
    Handle(SALOME_InteractiveObject) anIO = aSelList.First();
    myObject = GEOMBase::ConvertIOinGEOMObject( anIO );
    if ( !CORBA::is_nil( myObject ) )
      myGrp->LineEdit1->setText( GEOMBase::GetName( myObject ) );
  }
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BlocksGUI_PropagateDlg::SetEditCurrentArgument()
{
  const QObject* send = sender();
  if ( send == myGrp->PushButton1 )  {
    myGrp->LineEdit1->setFocus();
  }
  activateSelection();
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void BlocksGUI_PropagateDlg::LineEditReturnPressed()
{
  const QObject* send = sender();
  if ( send == myGrp->LineEdit1 ) {
    GEOMBase_Skeleton::LineEditReturnPressed();
  }
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void BlocksGUI_PropagateDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();

  myGrp->LineEdit1->setText( "" );
  myObject = GEOM::GEOM_Object::_nil();

  //myGeomGUI->SetState( 0 );
  activateSelection();
}


//=================================================================================
// function : enterEvent()
// purpose  : Mouse enter onto the dialog to activate it
//=================================================================================
void BlocksGUI_PropagateDlg::enterEvent( QEvent* )
{
  if ( !mainFrame()->GroupConstructors->isEnabled() )
    ActivateThisDialog();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr BlocksGUI_PropagateDlg::createOperation()
{
  return getGeomEngine()->GetIBlocksOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool BlocksGUI_PropagateDlg::isValid( QString& )
{
  return !myObject->_is_nil() ;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool BlocksGUI_PropagateDlg::execute( ObjectList& objects )
{
  GEOM::GEOM_IBlocksOperations_var anOper = GEOM::GEOM_IBlocksOperations::_narrow(getOperation());
  GEOM::ListOfGO_var aList = anOper->Propagate( myObject );
  mainFrame()->ResultName->setText( "" );

  if ( !aList->length() )
    return false;

  for ( int i = 0, n = aList->length(); i < n; i++ ) {
    objects.push_back( aList[i]._retn() );
  }

  return objects.size() > 0;
}

//=================================================================================
// function : activateSelection
// purpose  : Activate selection
//=================================================================================
void BlocksGUI_PropagateDlg::activateSelection()
{
  TColStd_MapOfInteger aMap;
  aMap.Add( GEOM_FACE );
  aMap.Add( GEOM_SHELL );
  aMap.Add( GEOM_SOLID );
  aMap.Add( GEOM_COMPOUND );
  globalSelection( aMap );
  if ( myObject->_is_nil() ) {
    SelectionIntoArgument();
  }
  connect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr(),
           SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );
}

//================================================================
// Function : getFather
// Purpose  : Get father object for object to be added in study
//            ( called with addInStudy method )
//================================================================
GEOM::GEOM_Object_ptr BlocksGUI_PropagateDlg::getFather( GEOM::GEOM_Object_ptr )
{
  return myObject;
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> BlocksGUI_PropagateDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res;
  GEOM::GeomObjPtr aGeomObjPtr(myObject);
  res << aGeomObjPtr;
  return res;
}
