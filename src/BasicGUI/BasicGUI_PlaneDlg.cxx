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
//  File   : BasicGUI_PlaneDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#include "BasicGUI_PlaneDlg.h"

#include "SUIT_Desktop.h"
#include "SUIT_Session.h"
#include "SalomeApp_Application.h"
#include "LightApp_SelectionMgr.h"

#include <TColStd_MapOfInteger.hxx>

#include <qlabel.h>

#include "GEOMImpl_Types.hxx"

using namespace std;

//=================================================================================
// class    : BasicGUI_PlaneDlg()
// purpose  : Constructs a BasicGUI_PlaneDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
BasicGUI_PlaneDlg::BasicGUI_PlaneDlg(GeometryGUI* theGeometryGUI, QWidget* parent,
                                     const char* name, bool modal, WFlags fl)
  :GEOMBase_Skeleton(theGeometryGUI, parent, name, modal, WStyle_Customize |
                     WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_DLG_PLANE_PV")));
  QPixmap image1(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_DLG_PLANE_3PNTS")));
  QPixmap image2(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_DLG_PLANE_FACE")));
  QPixmap image3(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_PLANE_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_PLANE"));
  RadioButton1->setPixmap(image0);
  RadioButton2->setPixmap(image1);
  RadioButton3->setPixmap(image2);

  GroupPntDir = new DlgRef_2Sel1Spin(this, "GroupPointDirection");
  GroupPntDir->GroupBox1->setTitle(tr("GEOM_PLANE_PV"));
  GroupPntDir->TextLabel1->setText(tr("GEOM_POINT"));
  GroupPntDir->TextLabel2->setText(tr("GEOM_VECTOR"));
  GroupPntDir->TextLabel3->setText(tr("GEOM_PLANE_SIZE"));
  GroupPntDir->PushButton1->setPixmap(image3);
  GroupPntDir->PushButton2->setPixmap(image3);
  GroupPntDir->LineEdit1->setReadOnly( true );
  GroupPntDir->LineEdit2->setReadOnly( true );

  Group3Pnts = new DlgRef_3Sel1Spin(this, "Group3Pnts");
  Group3Pnts->GroupBox1->setTitle(tr("GEOM_3_POINTS"));
  Group3Pnts->TextLabel1->setText(tr("GEOM_POINT1"));
  Group3Pnts->TextLabel2->setText(tr("GEOM_POINT2"));
  Group3Pnts->TextLabel3->setText(tr("GEOM_POINT3"));
  Group3Pnts->TextLabel4->setText(tr("GEOM_PLANE_SIZE"));
  Group3Pnts->PushButton1->setPixmap(image3);
  Group3Pnts->PushButton2->setPixmap(image3);
  Group3Pnts->PushButton3->setPixmap(image3);

  Group3Pnts->LineEdit1->setReadOnly( true );
  Group3Pnts->LineEdit2->setReadOnly( true );
  Group3Pnts->LineEdit3->setReadOnly( true );

  GroupFace = new DlgRef_1Sel1Spin(this, "GroupFace");
  GroupFace->GroupBox1->setTitle(tr("GEOM_FACE_OR_LCS"));
  GroupFace->TextLabel1->setText(tr("GEOM_SELECTION"));
  GroupFace->TextLabel2->setText(tr("GEOM_PLANE_SIZE"));
  GroupFace->PushButton1->setPixmap(image3);

  GroupFace->LineEdit1->setReadOnly( true );

  Layout1->addWidget(GroupPntDir, 2, 0);
  Layout1->addWidget(Group3Pnts, 2, 0);
  Layout1->addWidget(GroupFace, 2, 0);
  /***************************************************************/

  setHelpFileName("plane.htm");

  Init();
}


//=================================================================================
// function : ~BasicGUI_PlaneDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
BasicGUI_PlaneDlg::~BasicGUI_PlaneDlg()
{
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BasicGUI_PlaneDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupPntDir->LineEdit1;

  myPoint = myDir = myPoint1 = myPoint2 = myPoint3 = myFace = GEOM::GEOM_Object::_nil();

  // myGeomGUI->SetState( 0 );

  /* Get setting of step value from file configuration */
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  double aStep = resMgr->doubleValue( "Geometry", "SettingsGeomStep", 100);

  double aTrimSize = 2000.0;

  /* min, max, step and decimals for spin boxes */
  GroupPntDir->SpinBox_DX->RangeStepAndValidator( 0.001, COORD_MAX, aStep, 3 );
  GroupPntDir->SpinBox_DX->SetValue( aTrimSize );
  Group3Pnts->SpinBox_DX->RangeStepAndValidator(0.001, COORD_MAX, aStep, 3);
  Group3Pnts->SpinBox_DX->SetValue( aTrimSize );
  GroupFace->SpinBox_DX->RangeStepAndValidator(0.001, COORD_MAX, aStep, 3);
  GroupFace->SpinBox_DX->SetValue( aTrimSize );

  /* signals and slots connections */
  connect(buttonCancel, SIGNAL(clicked()), this, SLOT(ClickOnCancel()));
  connect(myGeomGUI, SIGNAL(SignalDeactivateActiveDialog()), this, SLOT(DeactivateActiveDialog()));
  connect(myGeomGUI, SIGNAL(SignalCloseAllDialogs()), this, SLOT(ClickOnCancel()));

  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));
  connect(GroupConstructors, SIGNAL(clicked(int)), this, SLOT(ConstructorsClicked(int)));

  connect(GroupPntDir->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPntDir->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(Group3Pnts->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(Group3Pnts->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(Group3Pnts->PushButton3, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupFace->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(GroupPntDir->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupPntDir->LineEdit2, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(Group3Pnts->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(Group3Pnts->LineEdit2, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(Group3Pnts->LineEdit3, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupFace->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(GroupPntDir->SpinBox_DX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(Group3Pnts->SpinBox_DX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupFace->SpinBox_DX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));

  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupPntDir->SpinBox_DX, SLOT(SetStep(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), Group3Pnts->SpinBox_DX, SLOT(SetStep(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupFace->SpinBox_DX, SLOT(SetStep(double)));

  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  initName( tr( "GEOM_PLANE" ) );

  Group3Pnts->hide();
  GroupFace->hide();
  ConstructorsClicked( 0 );
}


//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void BasicGUI_PlaneDlg::ConstructorsClicked(int constructorId)
{
  disconnect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 0, this, 0);
  myPoint = myDir = myPoint1 = myPoint2 = myPoint3 = myFace = GEOM::GEOM_Object::_nil();

  switch ( constructorId )
  {
    case 0: /* plane from a point and a direction (vector, edge...) */
      {
				Group3Pnts->hide();
				GroupFace->hide();
				resize(0, 0);
				GroupPntDir->show();

				myEditCurrentArgument = GroupPntDir->LineEdit1;
				GroupPntDir->LineEdit1->setText(tr(""));
				GroupPntDir->LineEdit2->setText(tr(""));

				/* for the first argument */
				globalSelection( GEOM_POINT );
				break;
      }
    case 1: /* plane from 3 points */
      {
				GroupPntDir->hide();
				GroupFace->hide();
				resize(0, 0);
				Group3Pnts->show();

				myEditCurrentArgument = Group3Pnts->LineEdit1;
				Group3Pnts->LineEdit1->setText("");
				Group3Pnts->LineEdit2->setText("");
				Group3Pnts->LineEdit3->setText("");

				/* for the first argument */
				globalSelection( GEOM_POINT );
				break;
      }
    case 2: /* plane from a planar face selection */
      {
				GroupPntDir->hide();
				Group3Pnts->hide();
				resize(0, 0);
				GroupFace->show();

				myEditCurrentArgument = GroupFace->LineEdit1;
				GroupFace->LineEdit1->setText(tr(""));

				/* for the first argument */
				//globalSelection( GEOM_PLANE );
                                TColStd_MapOfInteger aMap;
                                aMap.Add( GEOM_PLANE );
                                aMap.Add( GEOM_MARKER );
                                globalSelection( aMap );
				break;
      }
    }

  myEditCurrentArgument->setFocus();
  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(),
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void BasicGUI_PlaneDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool BasicGUI_PlaneDlg::ClickOnApply()
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
void BasicGUI_PlaneDlg::ClickOnCancel()
{
  GEOMBase_Skeleton::ClickOnCancel();
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection has changed
//=================================================================================
void BasicGUI_PlaneDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("");

  if ( IObjectCount() != 1 )
  {
    if      ( myEditCurrentArgument == GroupPntDir->LineEdit1 ) myPoint  = GEOM::GEOM_Object::_nil();
    else if ( myEditCurrentArgument == GroupPntDir->LineEdit2 ) myDir    = GEOM::GEOM_Object::_nil();
    else if ( myEditCurrentArgument == Group3Pnts->LineEdit1 )  myPoint1 = GEOM::GEOM_Object::_nil();
    else if ( myEditCurrentArgument == Group3Pnts->LineEdit2 )  myPoint2 = GEOM::GEOM_Object::_nil();
    else if ( myEditCurrentArgument == Group3Pnts->LineEdit3 )  myPoint3 = GEOM::GEOM_Object::_nil();
    else if ( myEditCurrentArgument == GroupFace->LineEdit1 )   myFace   = GEOM::GEOM_Object::_nil();
    return;
  }

  // nbSel == 1
  Standard_Boolean aRes = Standard_False;
  GEOM::GEOM_Object_var aSelectedObject = GEOMBase::ConvertIOinGEOMObject( firstIObject(), aRes );
  if ( !CORBA::is_nil( aSelectedObject ) && aRes )
  {
    myEditCurrentArgument->setText( GEOMBase::GetName( aSelectedObject ) );
    if      ( myEditCurrentArgument == GroupPntDir->LineEdit1 ) myPoint  = aSelectedObject;
    else if ( myEditCurrentArgument == GroupPntDir->LineEdit2 ) myDir    = aSelectedObject;
    else if ( myEditCurrentArgument == Group3Pnts->LineEdit1 )  myPoint1 = aSelectedObject;
    else if ( myEditCurrentArgument == Group3Pnts->LineEdit2 )  myPoint2 = aSelectedObject;
    else if ( myEditCurrentArgument == Group3Pnts->LineEdit3 )  myPoint3 = aSelectedObject;
    else if ( myEditCurrentArgument == GroupFace->LineEdit1 )   myFace   = aSelectedObject;
  }

  displayPreview();
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BasicGUI_PlaneDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  if      ( send == GroupPntDir->PushButton1 ) myEditCurrentArgument = GroupPntDir->LineEdit1;
  else if ( send == GroupPntDir->PushButton2 ) myEditCurrentArgument = GroupPntDir->LineEdit2;
  else if ( send == Group3Pnts->PushButton1 )  myEditCurrentArgument = Group3Pnts->LineEdit1;
  else if ( send == Group3Pnts->PushButton2 )  myEditCurrentArgument = Group3Pnts->LineEdit2;
  else if ( send == Group3Pnts->PushButton3 )  myEditCurrentArgument = Group3Pnts->LineEdit3;
  else if ( send == GroupFace->PushButton1 )   myEditCurrentArgument = GroupFace->LineEdit1;

  myEditCurrentArgument->setFocus();

  if ( myEditCurrentArgument == GroupPntDir->LineEdit2 )
    globalSelection( GEOM_LINE );
  else if ( myEditCurrentArgument == GroupFace->LineEdit1 ) {
    //globalSelection( GEOM_PLANE );
    TColStd_MapOfInteger aMap;
    aMap.Add( GEOM_PLANE );
    aMap.Add( GEOM_MARKER );
    globalSelection( aMap );
  }
  else
    globalSelection( GEOM_POINT );

  SelectionIntoArgument();
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void BasicGUI_PlaneDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if ( send == GroupPntDir->LineEdit1 ||
       send == GroupPntDir->LineEdit2 ||
       send == Group3Pnts->LineEdit1 ||
       send == Group3Pnts->LineEdit2 ||
       send == Group3Pnts->LineEdit3 ||
       send == GroupFace->LineEdit1 )
  {
    myEditCurrentArgument = send;
    GEOMBase_Skeleton::LineEditReturnPressed();
  }
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void BasicGUI_PlaneDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(),
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  // myGeomGUI->SetState( 0 );

  ConstructorsClicked( getConstructorId() );
}

//=================================================================================
// function : DeactivateActiveDialog()
// purpose  : public slot to deactivate if active
//=================================================================================
void BasicGUI_PlaneDlg::DeactivateActiveDialog()
{
  // myGeomGUI->SetState( -1 );
  GEOMBase_Skeleton::DeactivateActiveDialog();
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void BasicGUI_PlaneDlg::enterEvent(QEvent* e)
{
  if ( !GroupConstructors->isEnabled() )
    ActivateThisDialog();
}


//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void BasicGUI_PlaneDlg::ValueChangedInSpinBox(double newValue)
{
  displayPreview();
}

//=================================================================================
// function : getSize()
// purpose  :
//=================================================================================
double BasicGUI_PlaneDlg::getSize() const
{
  switch ( getConstructorId() )
  {
  case 0 : return GroupPntDir->SpinBox_DX->GetValue();
  case 1 : return Group3Pnts->SpinBox_DX->GetValue();
  case 2 : return GroupFace->SpinBox_DX->GetValue();
  }
  return 0.;
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr BasicGUI_PlaneDlg::createOperation()
{
  return myGeomGUI->GetGeomGen()->GetIBasicOperations( getStudyId() );
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
bool BasicGUI_PlaneDlg::isValid( QString& msg )
{
	const int id = getConstructorId();
	if ( getSize() <= 0 )
	{
		msg = QString( "Please, enter size greater than 0." );
		return false;
	}
	if ( id == 0 )
		return !CORBA::is_nil( myPoint ) && !CORBA::is_nil( myDir );
	else if ( id == 1 )
		return !CORBA::is_nil( myPoint1  ) && !CORBA::is_nil( myPoint2 ) && !CORBA::is_nil( myPoint3 ) &&
		       !isEqual( myPoint1, myPoint2 ) && !isEqual( myPoint1, myPoint3 ) && !isEqual( myPoint2, myPoint3 );
	else if ( id == 2 )
		return !CORBA::is_nil( myFace );
	return false;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool BasicGUI_PlaneDlg::execute( ObjectList& objects )
{
  bool res = false;

  GEOM::GEOM_Object_var anObj;

  switch ( getConstructorId() )
  {
  case 0 :
    anObj = GEOM::GEOM_IBasicOperations::_narrow( getOperation() )->MakePlanePntVec( myPoint, myDir, getSize() );
    res = true;
    break;
  case 1 :
    anObj = GEOM::GEOM_IBasicOperations::_narrow( getOperation() )->MakePlaneThreePnt( myPoint1, myPoint2, myPoint3, getSize() );
    res = true;
    break;
  case 2 :
    anObj = GEOM::GEOM_IBasicOperations::_narrow( getOperation() )->MakePlaneFace( myFace, getSize() );
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
void BasicGUI_PlaneDlg::closeEvent( QCloseEvent* e )
{
  GEOMBase_Skeleton::closeEvent( e );
}

