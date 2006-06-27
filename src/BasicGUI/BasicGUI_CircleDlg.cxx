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
//  File   : BasicGUI_CircleDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#include "BasicGUI_CircleDlg.h"

#include "SUIT_Desktop.h"
#include "SUIT_Session.h"
#include "SalomeApp_Application.h"
#include "LightApp_SelectionMgr.h"

#include <qlabel.h>

#include "GEOMImpl_Types.hxx"

#include "utilities.h"

//=================================================================================
// class    : BasicGUI_CircleDlg()
// purpose  : Constructs a BasicGUI_CircleDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
BasicGUI_CircleDlg::BasicGUI_CircleDlg(GeometryGUI* theGeometryGUI, QWidget* parent,
                                       const char* name, bool modal, WFlags fl)
  :GEOMBase_Skeleton(theGeometryGUI, parent, name, modal, WStyle_Customize |
                     WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_DLG_CIRCLE_PV")));
  QPixmap image2(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_DLG_CIRCLE_PNTS")));
  QPixmap image1(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_CIRCLE_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_CIRCLE"));
  RadioButton1->setPixmap(image0);
  RadioButton2->setPixmap(image2);
  RadioButton3->close(TRUE);

  GroupPntVecR = new DlgRef_2Sel1Spin(this, "GroupPntVecR");
  GroupPntVecR->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  
  GroupPntVecR->TextLabel1->setText(tr("GEOM_CENTER_POINT"));
  GroupPntVecR->TextLabel2->setText(tr("GEOM_VECTOR"));
  GroupPntVecR->TextLabel3->setText(tr("GEOM_RADIUS"));
  GroupPntVecR->PushButton1->setPixmap(image1);
  GroupPntVecR->PushButton2->setPixmap(image1);

  GroupPntVecR->LineEdit1->setReadOnly( true );
  GroupPntVecR->LineEdit2->setReadOnly( true );

  Group3Pnts = new DlgRef_3Sel_QTD(this, "Group3Pnts");
  Group3Pnts->GroupBox1->setTitle(tr("GEOM_3_POINTS"));
  Group3Pnts->TextLabel1->setText(tr("GEOM_POINT1"));
  Group3Pnts->TextLabel2->setText(tr("GEOM_POINT2"));
  Group3Pnts->TextLabel3->setText(tr("GEOM_POINT3"));
  Group3Pnts->PushButton1->setPixmap(image1);
  Group3Pnts->PushButton2->setPixmap(image1);
  Group3Pnts->PushButton3->setPixmap(image1);

  Group3Pnts->LineEdit1->setReadOnly( true );
  Group3Pnts->LineEdit2->setReadOnly( true );
  Group3Pnts->LineEdit3->setReadOnly( true );

  Layout1->addWidget( GroupPntVecR, 2, 0 );
  Layout1->addWidget( Group3Pnts, 2, 0 );
  /***************************************************************/

  setHelpFileName("circle.htm");

  Init();
}


//=================================================================================
// function : ~BasicGUI_CircleDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
BasicGUI_CircleDlg::~BasicGUI_CircleDlg()
{
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BasicGUI_CircleDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupPntVecR->LineEdit1;

  myPoint = myDir = myPoint1 = myPoint2 = myPoint3 = GEOM::GEOM_Object::_nil();

  // myGeomGUI->SetState( 0 );

  /* Get setting of step value from file configuration */
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  double aStep = resMgr->doubleValue( "Geometry", "SettingsGeomStep", 100);

  /* min, max, step and decimals for spin boxes & initial values */
  GroupPntVecR->SpinBox_DX->RangeStepAndValidator(0.001, COORD_MAX, aStep, 3);
  GroupPntVecR->SpinBox_DX->SetValue( 100 );

  /* signals and slots connections */
  connect(buttonCancel, SIGNAL(clicked()), this, SLOT(ClickOnCancel()));
  connect(myGeomGUI, SIGNAL(SignalDeactivateActiveDialog()), this, SLOT(DeactivateActiveDialog()));
  connect(myGeomGUI, SIGNAL(SignalCloseAllDialogs()), this, SLOT(ClickOnCancel()));
  
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));
  connect(GroupConstructors, SIGNAL(clicked(int)), this, SLOT(ConstructorsClicked(int)));

  connect(GroupPntVecR->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPntVecR->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(Group3Pnts->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(Group3Pnts->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(Group3Pnts->PushButton3, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(GroupPntVecR->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupPntVecR->LineEdit2, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(GroupPntVecR->SpinBox_DX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox()));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupPntVecR->SpinBox_DX, SLOT(SetStep(double)));
  
  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(),
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument())) ;

  initName( tr( "GEOM_CIRCLE" ) );

  Group3Pnts->hide();
  ConstructorsClicked( 0 );
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void BasicGUI_CircleDlg::ConstructorsClicked( int constructorId )
{
  disconnect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 0, this, 0 );
  myPoint = myDir = myPoint1 = myPoint2 = myPoint3 = GEOM::GEOM_Object::_nil();

  switch ( constructorId )
  {
    case 0:
      {
	Group3Pnts->hide();
	resize(0, 0);
	GroupPntVecR->show();
	
	myEditCurrentArgument = GroupPntVecR->LineEdit1;
	GroupPntVecR->LineEdit1->setText("");
	GroupPntVecR->LineEdit2->setText("");
	break;
      }
  case 1:
    {
      GroupPntVecR->hide();
      resize( 0, 0 );
      Group3Pnts->show();
      
      myEditCurrentArgument = Group3Pnts->LineEdit1;
      Group3Pnts->LineEdit1->setText("");
      Group3Pnts->LineEdit2->setText("");
      Group3Pnts->LineEdit3->setText("");
      break;
      }
  }
  
  myEditCurrentArgument->setFocus();
  globalSelection( GEOM_POINT );
  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void BasicGUI_CircleDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool BasicGUI_CircleDlg::ClickOnApply()
{
  if ( !onAccept() )
    return false;

  initName();
  ConstructorsClicked( getConstructorId() );
  return true;
}

//=======================================================================
// function : ClickOnCancel()
// purpose  :
//=======================================================================
void BasicGUI_CircleDlg::ClickOnCancel()
{
  GEOMBase_Skeleton::ClickOnCancel();
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void BasicGUI_CircleDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("");
  
  if ( IObjectCount() != 1 )  
  {
    if      ( myEditCurrentArgument == GroupPntVecR->LineEdit1 ) myPoint  = GEOM::GEOM_Object::_nil();
    else if ( myEditCurrentArgument == GroupPntVecR->LineEdit2 ) myDir    = GEOM::GEOM_Object::_nil();
    else if ( myEditCurrentArgument == Group3Pnts->LineEdit1 )   myPoint1 = GEOM::GEOM_Object::_nil();
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
    if      ( myEditCurrentArgument == GroupPntVecR->LineEdit1 ) myPoint  = aSelectedObject;
    else if ( myEditCurrentArgument == GroupPntVecR->LineEdit2 ) myDir    = aSelectedObject;
    else if ( myEditCurrentArgument == Group3Pnts->LineEdit1 )   myPoint1 = aSelectedObject;
    else if ( myEditCurrentArgument == Group3Pnts->LineEdit2 )   myPoint2 = aSelectedObject;
    else if ( myEditCurrentArgument == Group3Pnts->LineEdit3 )   myPoint3 = aSelectedObject;
  }

  displayPreview();
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BasicGUI_CircleDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  if ( send == GroupPntVecR->PushButton1 )      myEditCurrentArgument = GroupPntVecR->LineEdit1;
  else if ( send == GroupPntVecR->PushButton2 ) myEditCurrentArgument = GroupPntVecR->LineEdit2;
  else if ( send == Group3Pnts->PushButton1 )   myEditCurrentArgument = Group3Pnts->LineEdit1;
  else if ( send == Group3Pnts->PushButton2 )   myEditCurrentArgument = Group3Pnts->LineEdit2;
  else if ( send == Group3Pnts->PushButton3 )   myEditCurrentArgument = Group3Pnts->LineEdit3;
  
  myEditCurrentArgument->setFocus();
  
  if ( myEditCurrentArgument == GroupPntVecR->LineEdit2 ) 
    globalSelection( GEOM_LINE );
  else
    globalSelection( GEOM_POINT );
  SelectionIntoArgument();
}

//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void BasicGUI_CircleDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if ( send == GroupPntVecR->LineEdit1 ||
       send == GroupPntVecR->LineEdit2 ||
       send == Group3Pnts->LineEdit1 ||
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
void BasicGUI_CircleDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  globalSelection( GEOM_POINT );
  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  // myGeomGUI->SetState( 0 );

  ConstructorsClicked( getConstructorId() );
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void BasicGUI_CircleDlg::enterEvent(QEvent* e)
{
  if ( !GroupConstructors->isEnabled() )
    ActivateThisDialog();
}

//=================================================================================
// function : DeactivateActiveDialog()
// purpose  : public slot to deactivate if active
//=================================================================================
void BasicGUI_CircleDlg::DeactivateActiveDialog()
{
  // myGeomGUI->SetState( -1 );
  GEOMBase_Skeleton::DeactivateActiveDialog();
}

//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void BasicGUI_CircleDlg::ValueChangedInSpinBox()
{
  displayPreview();
}

//=================================================================================
// function : getRadius()
// purpose  :
//=================================================================================
double BasicGUI_CircleDlg::getRadius() const
{
  return GroupPntVecR->SpinBox_DX->GetValue();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr BasicGUI_CircleDlg::createOperation()
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
bool BasicGUI_CircleDlg::isValid( QString& msg )
{
  const int id = getConstructorId();
  if ( id == 0 )
    return !myPoint->_is_nil() && !myDir->_is_nil() && getRadius() > 0;
  else if ( id == 1 )
    return !myPoint1->_is_nil() && !myPoint2->_is_nil() && !myPoint3->_is_nil() &&
      !isEqual( myPoint1, myPoint2 ) && !isEqual( myPoint1, myPoint3 ) && !isEqual( myPoint2, myPoint3 );
  return false;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool BasicGUI_CircleDlg::execute( ObjectList& objects )
{
  bool res = false;
  
  GEOM::GEOM_Object_var anObj;
  
  switch ( getConstructorId() )
  {
  case 0 :
    anObj = GEOM::GEOM_ICurvesOperations::_narrow( getOperation() )->MakeCirclePntVecR( myPoint, myDir, getRadius() );
    res = true;
    break;
  case 1 :
    anObj = GEOM::GEOM_ICurvesOperations::_narrow( getOperation() )->MakeCircleThreePnt( myPoint1, myPoint2, myPoint3 );
    res = true;
    break;
  }
  
  if ( !anObj->_is_nil() )
    objects.push_back( anObj._retn() );
  
  return res;
}

//=================================================================================
// function : closeEvent
// purpose  :
//=================================================================================
void BasicGUI_CircleDlg::closeEvent( QCloseEvent* e )
{
  GEOMBase_Skeleton::closeEvent( e );
}

