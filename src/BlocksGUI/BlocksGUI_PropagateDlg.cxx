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
//  File   : BlocksGUI_PropagateDlg.cxx
//  Author : VKN
//  Module : GEOM
//  $Header$

#include "BlocksGUI_PropagateDlg.h"
#include "GEOMImpl_Types.hxx"

#include "SUIT_Session.h"
#include "SalomeApp_Application.h"
#include "LightApp_SelectionMgr.h"

#include <TColStd_MapOfInteger.hxx>

#include <qlabel.h>

using namespace std;

//=================================================================================
// class    : BlocksGUI_PropagateDlg()
// purpose  : Constructs a BlocksGUI_PropagateDlg  which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
BlocksGUI_PropagateDlg::BlocksGUI_PropagateDlg(GeometryGUI* theGeometryGUI, QWidget* parent,
                                               const char* name, bool modal, WFlags fl)
  :GEOMBase_Skeleton(theGeometryGUI, parent, name, modal, WStyle_Customize |
                     WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_DLG_PROPAGATE")));
  QPixmap image1(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_PROPAGATE_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_PROPAGATE_TITLE"));
  RadioButton1->setPixmap(image0);
  RadioButton2->close(TRUE);
  RadioButton3->close(TRUE);

  QGroupBox* aMainGrp = new QGroupBox( 1, Qt::Horizontal, tr( "GEOM_SELECTED_SHAPE" ), this );
  QGroupBox* aSelGrp = new QGroupBox(3, Qt::Horizontal, aMainGrp);
  aSelGrp->setFrameStyle(QFrame::NoFrame);
  aSelGrp->setInsideMargin(0);

  new QLabel(tr("GEOM_OBJECT"), aSelGrp);
  mySelBtn = new QPushButton(aSelGrp);
  mySelBtn->setPixmap(image1);
  mySelName = new QLineEdit(aSelGrp);
  mySelName->setReadOnly(true);

  Layout1->addWidget(aMainGrp, 1, 0);

  /***************************************************************/

  setHelpFileName("propagate.htm");

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
  ResultName->setText( "" );

  //myGeomGUI->SetState( 0 );

  /* signals and slots connections */
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(mySelBtn, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(mySelName, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  activateSelection();
}


//=================================================================================
// function : ClickOnOk()
// purpose  : Same than click on apply but close this dialog.
//=================================================================================
void BlocksGUI_PropagateDlg::ClickOnOk()
{
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

  mySelName->setText("");
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
  mySelName->setText("");
  myObject = GEOM::GEOM_Object::_nil();

  if ( IObjectCount() == 1 ) {
    Handle(SALOME_InteractiveObject) anIO = firstIObject();
    Standard_Boolean aRes;
    myObject = GEOMBase::ConvertIOinGEOMObject( anIO, aRes );
    if ( aRes )
      mySelName->setText( GEOMBase::GetName( myObject ) );
  }
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BlocksGUI_PropagateDlg::SetEditCurrentArgument()
{
  const QObject* send = sender();
  if ( send == mySelBtn )  {
    mySelName->setFocus();
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
  if( send == mySelName ) {
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

  mySelName->setText("");
  myObject = GEOM::GEOM_Object::_nil();

  //myGeomGUI->SetState( 0 );
  activateSelection();
}


//=================================================================================
// function : enterEvent()
// purpose  : Mouse enter onto the dialog to activate it
//=================================================================================
void BlocksGUI_PropagateDlg::enterEvent(QEvent* e)
{
  if ( !GroupConstructors->isEnabled() )
    ActivateThisDialog();
}


//=================================================================================
// function : closeEvent()
// purpose  :
//=================================================================================
void BlocksGUI_PropagateDlg::closeEvent(QCloseEvent* e)
{
  //myGeomGUI->SetState( -1 );
  GEOMBase_Skeleton::closeEvent( e );
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
bool BlocksGUI_PropagateDlg::isValid( QString& msg )
{
  return !myObject->_is_nil() ;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool BlocksGUI_PropagateDlg::execute( ObjectList& objects )
{

  GEOM::ListOfGO_var aList = GEOM::GEOM_IBlocksOperations::_narrow( getOperation() )->Propagate( myObject );
  ResultName->setText( "" );

  if ( !aList->length() )
    return false;

  for ( int i = 0, n = aList->length(); i < n; i++ )
  {
    objects.push_back(aList[i]._retn());
  }

  return objects.size() ? true : false;
}

//=================================================================================
// function : activateSelection
// purpose  : Activate selection
//=================================================================================
void BlocksGUI_PropagateDlg::activateSelection()
{
  TColStd_MapOfInteger aMap;
  aMap.Add( GEOM_SOLID );
  aMap.Add( GEOM_COMPOUND );
  globalSelection( aMap );
  if (myObject->_is_nil()) {
    SelectionIntoArgument();
  }
  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument())) ;
}

//================================================================
// Function : getFather
// Purpose  : Get father object for object to be added in study
//            ( called with addInStudy method )
//================================================================
GEOM::GEOM_Object_ptr BlocksGUI_PropagateDlg::getFather (GEOM::GEOM_Object_ptr)
{
  return myObject;
}
