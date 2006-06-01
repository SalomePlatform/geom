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
//  File   : BasicGUI_ArcDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#include "BasicGUI_ArcDlg.h"

#include "SUIT_Desktop.h"
#include "SUIT_Session.h"
#include "SalomeApp_Application.h"
#include "LightApp_SelectionMgr.h"

#include <qlabel.h>

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <GC_MakeArcOfCircle.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Precision.hxx>

#include "GEOMImpl_Types.hxx"

#include "utilities.h"

using namespace std;

//=================================================================================
// class    : BasicGUI_ArcDlg()
// purpose  : Constructs a BasicGUI_ArcDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
BasicGUI_ArcDlg::BasicGUI_ArcDlg(GeometryGUI* theGeometryGUI, QWidget* parent,
                                 const char* name, bool modal, WFlags fl)
  : GEOMBase_Skeleton(theGeometryGUI, parent, name, modal, WStyle_Customize |
                      WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_DLG_ARC")));
  QPixmap image1(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_ARC_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_ARC"));
  RadioButton1->setPixmap(image0);
  RadioButton2->close(TRUE);
  RadioButton3->close(TRUE);

  Group3Pnts = new DlgRef_3Sel_QTD(this, "Group3Pnts");
  Group3Pnts->GroupBox1->setTitle(tr("GEOM_POINTS"));
  Group3Pnts->TextLabel1->setText(tr("GEOM_POINT_I").arg("1"));
  Group3Pnts->TextLabel2->setText(tr("GEOM_POINT_I").arg("2"));
  Group3Pnts->TextLabel3->setText(tr("GEOM_POINT_I").arg("3"));

  Group3Pnts->LineEdit1->setReadOnly( true );
  Group3Pnts->LineEdit2->setReadOnly( true );
  Group3Pnts->LineEdit3->setReadOnly( true );

  Group3Pnts->PushButton1->setPixmap(image1);
  Group3Pnts->PushButton2->setPixmap(image1);
  Group3Pnts->PushButton3->setPixmap(image1);

  Layout1->addWidget( Group3Pnts, 2, 0 );

  setHelpFileName("arc.htm");

  Init();
}


//=================================================================================
// function : ~BasicGUI_ArcDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
BasicGUI_ArcDlg::~BasicGUI_ArcDlg()
{
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BasicGUI_ArcDlg::Init()
{
  /* init variables */
  // myGeomGUI->SetState( 0 );
  globalSelection( GEOM_POINT );

  myEditCurrentArgument = Group3Pnts->LineEdit1;
  myEditCurrentArgument->setFocus();

  myPoint1 = myPoint2 = myPoint3 = GEOM::GEOM_Object::_nil();

  /* signals and slots connections */
  connect(buttonCancel, SIGNAL(clicked()), this, SLOT(ClickOnCancel()));
  connect(myGeomGUI, SIGNAL(SignalDeactivateActiveDialog()), this, SLOT(DeactivateActiveDialog()));
  connect(myGeomGUI, SIGNAL(SignalCloseAllDialogs()), this, SLOT(ClickOnCancel()));

  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(Group3Pnts->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(Group3Pnts->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(Group3Pnts->PushButton3, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(Group3Pnts->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(Group3Pnts->LineEdit2, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(Group3Pnts->LineEdit3, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(),
          SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  initName( tr( "GEOM_ARC" ) );
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void BasicGUI_ArcDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnCancel()
// purpose  :
//=================================================================================
void BasicGUI_ArcDlg::ClickOnCancel()
{
  GEOMBase_Skeleton::ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool BasicGUI_ArcDlg::ClickOnApply()
{
  if ( !onAccept() )
    return false;

  initName();

  // reset
  myPoint1 = myPoint2 = myPoint3 = GEOM::GEOM_Object::_nil();
  Group3Pnts->LineEdit1->setText( "" );
  Group3Pnts->LineEdit2->setText( "" );
  Group3Pnts->LineEdit3->setText( "" );
  myEditCurrentArgument = Group3Pnts->LineEdit1;

  return true;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void BasicGUI_ArcDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("");

  if ( IObjectCount() != 1 )
  {
    if      ( myEditCurrentArgument == Group3Pnts->LineEdit1 )   myPoint1 = GEOM::GEOM_Object::_nil();
    else if ( myEditCurrentArgument == Group3Pnts->LineEdit2 )   myPoint2 = GEOM::GEOM_Object::_nil();
    else if ( myEditCurrentArgument == Group3Pnts->LineEdit3 )   myPoint3 = GEOM::GEOM_Object::_nil();
    return;
  }

  // nbSel == 1
  Standard_Boolean aRes = Standard_False;
  GEOM::GEOM_Object_var aSelectedObject = GEOMBase::ConvertIOinGEOMObject( firstIObject(), aRes );
  if ( !CORBA::is_nil( aSelectedObject ) && aRes )
  {
    myEditCurrentArgument->setText( GEOMBase::GetName( aSelectedObject ) );
    if      ( myEditCurrentArgument == Group3Pnts->LineEdit1 )   myPoint1 = aSelectedObject;
    else if ( myEditCurrentArgument == Group3Pnts->LineEdit2 )   myPoint2 = aSelectedObject;
    else if ( myEditCurrentArgument == Group3Pnts->LineEdit3 )   myPoint3 = aSelectedObject;
  }

  displayPreview();
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BasicGUI_ArcDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  if      ( send == Group3Pnts->PushButton1 )   myEditCurrentArgument = Group3Pnts->LineEdit1;
  else if ( send == Group3Pnts->PushButton2 )   myEditCurrentArgument = Group3Pnts->LineEdit2;
  else if ( send == Group3Pnts->PushButton3 )   myEditCurrentArgument = Group3Pnts->LineEdit3;

  myEditCurrentArgument->setFocus();
  SelectionIntoArgument();
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void BasicGUI_ArcDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if ( send == Group3Pnts->LineEdit1 ||
       send == Group3Pnts->LineEdit2 ||
       send == Group3Pnts->LineEdit3 )
  {
    myEditCurrentArgument = send;
    GEOMBase_Skeleton::LineEditReturnPressed();
  }
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void BasicGUI_ArcDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(),
          SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  // myGeomGUI->SetState( 0 );
  globalSelection( GEOM_POINT );

  myEditCurrentArgument = Group3Pnts->LineEdit1;
  myEditCurrentArgument->setFocus();

  Group3Pnts->LineEdit1->setText( "" );
  Group3Pnts->LineEdit2->setText( "" );
  Group3Pnts->LineEdit3->setText( "" );
  myPoint1 = myPoint2 = myPoint3 = GEOM::GEOM_Object::_nil();
}

//=================================================================================
// function : DeactivateActiveDialog()
// purpose  : public slot to deactivate if active
//=================================================================================
void BasicGUI_ArcDlg::DeactivateActiveDialog()
{
  // myGeomGUI->SetState( -1 );
  GEOMBase_Skeleton::DeactivateActiveDialog();
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void BasicGUI_ArcDlg::enterEvent(QEvent* e)
{
  if ( !GroupConstructors->isEnabled() )
    ActivateThisDialog();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr BasicGUI_ArcDlg::createOperation()
{
  return myGeomGUI->GetGeomGen()->GetICurvesOperations( getStudyId() );
}

//=================================================================================
// function : isEqual
// purpose  : it may also be needed to check for min distance between gp_Pnt-s...
//=================================================================================
static bool isEqual( const GEOM::GEOM_Object_var& thePnt1, const GEOM::GEOM_Object_var& thePnt2 )
{
  return thePnt1->_is_equivalent( thePnt2 );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool BasicGUI_ArcDlg::isValid( QString& msg )
{
  return !myPoint1->_is_nil() && !myPoint2->_is_nil() && !myPoint3->_is_nil() &&
    !isEqual( myPoint1, myPoint2 ) && !isEqual( myPoint1, myPoint3 ) && !isEqual( myPoint2, myPoint3 );
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool BasicGUI_ArcDlg::execute( ObjectList& objects )
{
  GEOM::GEOM_Object_var anObj =
    GEOM::GEOM_ICurvesOperations::_narrow(getOperation())->MakeArc(myPoint1, myPoint2, myPoint3);
  if ( !anObj->_is_nil() )
    objects.push_back( anObj._retn() );
  return true;
}

//=================================================================================
// function : closeEvent
// purpose  :
//=================================================================================
void BasicGUI_ArcDlg::closeEvent( QCloseEvent* e )
{
  GEOMBase_Skeleton::closeEvent( e );
}
