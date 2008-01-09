//  GEOM GEOMGUI : GUI for Geometry component
//
//  Copyright (C) 2003  OPEN CASCADE 
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
//
//
//  File   : BasicGUI_EllipseDlg.cxx
//  Author : Nicolas REJNERI
//  Module : GEOM
//  $Header$

#include "BasicGUI_EllipseDlg.h"

#include "SUIT_Desktop.h"
#include "SUIT_Session.h"
#include "SalomeApp_Application.h"
#include "LightApp_SelectionMgr.h"

#include <TColStd_IndexedMapOfInteger.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>

#include <qlabel.h>

#include "GEOMImpl_Types.hxx"

#include "utilities.h"

using namespace std;

//=================================================================================
// class    : BasicGUI_EllipseDlg()
// purpose  : Constructs a BasicGUI_EllipseDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
BasicGUI_EllipseDlg::BasicGUI_EllipseDlg(GeometryGUI* theGeometryGUI, QWidget* parent,
                                         const char* name, bool modal, WFlags fl)
  :GEOMBase_Skeleton(theGeometryGUI, parent, name, modal, WStyle_Customize |
                     WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_DLG_ELLIPSE_PV")));
  QPixmap image1(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_ELLIPSE_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_ELLIPSE"));
  RadioButton1->setPixmap(image0);
  RadioButton2->close(TRUE);
  RadioButton3->close(TRUE);

  GroupPoints = new DlgRef_2Sel2Spin(this, "GroupPoints");
  GroupPoints->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  GroupPoints->TextLabel1->setText(tr("GEOM_CENTER"));
  GroupPoints->TextLabel2->setText(tr("GEOM_VECTOR"));
  GroupPoints->TextLabel3->setText(tr("GEOM_RADIUS_MAJOR"));
  GroupPoints->TextLabel4->setText(tr("GEOM_RADIUS_MINOR"));
  GroupPoints->PushButton1->setPixmap(image1);
  GroupPoints->PushButton2->setPixmap(image1);

  GroupPoints->LineEdit1->setReadOnly( true );
  GroupPoints->LineEdit2->setReadOnly( true );

  Layout1->addWidget(GroupPoints, 2, 0);
  /***************************************************************/

  setHelpFileName("create_ellipse_page.html");

  Init();
}


//=================================================================================
// function : ~BasicGUI_EllipseDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
BasicGUI_EllipseDlg::~BasicGUI_EllipseDlg()
{
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BasicGUI_EllipseDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupPoints->LineEdit1;
  globalSelection(); // close local contexts, if any
  localSelection(GEOM::GEOM_Object::_nil(), TopAbs_VERTEX);

  myPoint = myDir = GEOM::GEOM_Object::_nil();

  /* Get setting of step value from file configuration */
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  double step = resMgr->doubleValue( "Geometry", "SettingsGeomStep", 100);
  
  double aMajorR( 200. ), aMinorR( 100. );

  /* min, max, step and decimals for spin boxes & initial values */
  GroupPoints->SpinBox_DX->RangeStepAndValidator( 0.001, COORD_MAX, step, DBL_DIGITS_DISPLAY );
  GroupPoints->SpinBox_DY->RangeStepAndValidator( 0.001, COORD_MAX, step, DBL_DIGITS_DISPLAY );
  GroupPoints->SpinBox_DX->SetValue( aMajorR );
  GroupPoints->SpinBox_DY->SetValue( aMinorR );

  /* signals and slots connections */
  connect(buttonCancel, SIGNAL(clicked()), this, SLOT(ClickOnCancel()));
  connect(myGeomGUI, SIGNAL(SignalDeactivateActiveDialog()), this, SLOT(DeactivateActiveDialog()));
  connect(myGeomGUI, SIGNAL(SignalCloseAllDialogs()), this, SLOT(ClickOnCancel()));

  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(GroupPoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(GroupPoints->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupPoints->LineEdit2, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(GroupPoints->SpinBox_DX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupPoints->SpinBox_DY, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));

  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)),
          GroupPoints->SpinBox_DX, SLOT(SetStep(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)),
          GroupPoints->SpinBox_DY, SLOT(SetStep(double)));
  
  connect(myGeomGUI->getApp()->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument())) ;

  initName( tr( "GEOM_ELLIPSE" ) );
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void BasicGUI_EllipseDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool BasicGUI_EllipseDlg::ClickOnApply()
{
  if ( !onAccept() )
    return false;

  initName();

  // reset
  myPoint = myDir = GEOM::GEOM_Object::_nil();
  GroupPoints->LineEdit1->setText( "" );
  GroupPoints->LineEdit2->setText( "" );
  myEditCurrentArgument = GroupPoints->LineEdit1;
  //globalSelection(GEOM_POINT);
  globalSelection(); // close local contexts, if any
  localSelection(GEOM::GEOM_Object::_nil(), TopAbs_VERTEX);

  return true;
}

//=======================================================================
// function : ClickOnCancel()
// purpose  :
//=======================================================================
void BasicGUI_EllipseDlg::ClickOnCancel()
{
  GEOMBase_Skeleton::ClickOnCancel();
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void BasicGUI_EllipseDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("");

  if ( IObjectCount() != 1 )  
  {
    if      ( myEditCurrentArgument == GroupPoints->LineEdit1 ) myPoint = GEOM::GEOM_Object::_nil();
    else if ( myEditCurrentArgument == GroupPoints->LineEdit2 ) myDir   = GEOM::GEOM_Object::_nil();
    return;
  }

  Standard_Boolean aRes = Standard_False;
  Handle(SALOME_InteractiveObject) anIO = firstIObject();
  GEOM::GEOM_Object_var aSelectedObject = GEOMBase::ConvertIOinGEOMObject(firstIObject(), aRes);
  if (!CORBA::is_nil(aSelectedObject) && aRes)
  {  
    QString aName = GEOMBase::GetName(aSelectedObject);

    // Get Selected object if selected subshape
    TopoDS_Shape aShape;

    if (GEOMBase::GetShape(aSelectedObject, aShape, TopAbs_SHAPE) && !aShape.IsNull())
    {
      TopAbs_ShapeEnum aNeedType = TopAbs_VERTEX;
      if (myEditCurrentArgument == GroupPoints->LineEdit2)
        aNeedType = TopAbs_EDGE;

      LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
      TColStd_IndexedMapOfInteger aMap;
      aSelMgr->GetIndexes(anIO, aMap);
      if (aMap.Extent() == 1)
      {
        int anIndex = aMap(1);
        if (aNeedType == TopAbs_EDGE)
          aName += QString(":edge_%1").arg(anIndex);
        else
          aName += QString(":vertex_%1").arg(anIndex);

	//Find SubShape Object in Father
	GEOM::GEOM_Object_var aFindedObject = GEOMBase_Helper::findObjectInFather(aSelectedObject, aName);

	if ( aFindedObject == GEOM::GEOM_Object::_nil() ) { // Object not found in study
        GEOM::GEOM_IShapesOperations_var aShapesOp = getGeomEngine()->GetIShapesOperations(getStudyId());
        aSelectedObject = aShapesOp->GetSubShape(aSelectedObject, anIndex);
	} else {
	  aSelectedObject = aFindedObject; // get Object from study
	}
      }
      else // Global Selection
      {
        if (aShape.ShapeType() != aNeedType) {
          aSelectedObject = GEOM::GEOM_Object::_nil();
          aName = "";
        }
      }
    }

    myEditCurrentArgument->setText(aName);

    if      ( myEditCurrentArgument == GroupPoints->LineEdit1 ) myPoint = aSelectedObject;
    else if ( myEditCurrentArgument == GroupPoints->LineEdit2 ) myDir   = aSelectedObject;
  }

  displayPreview();
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BasicGUI_EllipseDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();
  globalSelection( GEOM_POINT );

  if      ( send == GroupPoints->PushButton1 ) myEditCurrentArgument = GroupPoints->LineEdit1;
  else if ( send == GroupPoints->PushButton2 ) myEditCurrentArgument = GroupPoints->LineEdit2;
  
  myEditCurrentArgument->setFocus();
  globalSelection(); // close local contexts, if any
  if ( myEditCurrentArgument == GroupPoints->LineEdit2 )
    localSelection(GEOM::GEOM_Object::_nil(), TopAbs_EDGE);
  else
    localSelection(GEOM::GEOM_Object::_nil(), TopAbs_VERTEX);
  SelectionIntoArgument();
}

//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void BasicGUI_EllipseDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if ( send == GroupPoints->LineEdit1 ||
       send == GroupPoints->LineEdit2 )
  {
    myEditCurrentArgument = send;
    GEOMBase_Skeleton::LineEditReturnPressed();
  }
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void BasicGUI_EllipseDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));

  GroupPoints->LineEdit1->setFocus();
  myEditCurrentArgument = GroupPoints->LineEdit1;

  GroupPoints->LineEdit1->setText( "" );
  GroupPoints->LineEdit2->setText( "" );

  myPoint = myDir = GEOM::GEOM_Object::_nil();
  //globalSelection( GEOM_POINT );
  globalSelection(); // close local contexts, if any
  localSelection(GEOM::GEOM_Object::_nil(), TopAbs_VERTEX);
}

//=================================================================================
// function : DeactivateActiveDialog()
// purpose  : public slot to deactivate if active
//=================================================================================
void BasicGUI_EllipseDlg::DeactivateActiveDialog()
{
  // myGeomGUI->SetState( -1 );
  GEOMBase_Skeleton::DeactivateActiveDialog();
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void BasicGUI_EllipseDlg::enterEvent(QEvent* e)
{
  if ( !GroupConstructors->isEnabled() )
    ActivateThisDialog();
}

//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void BasicGUI_EllipseDlg::ValueChangedInSpinBox(double newValue)
{
  displayPreview();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr BasicGUI_EllipseDlg::createOperation()
{
  return myGeomGUI->GetGeomGen()->GetICurvesOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool BasicGUI_EllipseDlg::isValid( QString& msg )
{
  double aMajorR = GroupPoints->SpinBox_DX->GetValue();
  double aMinorR = GroupPoints->SpinBox_DY->GetValue();
  if ( aMajorR < aMinorR )
  {
  	msg = tr( "GEOM_ELLIPSE_ERROR_1" );
   	return false;
  }
  return !myPoint->_is_nil() && !myDir->_is_nil();
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool BasicGUI_EllipseDlg::execute( ObjectList& objects )
{
  double aMajorR = GroupPoints->SpinBox_DX->GetValue();
  double aMinorR = GroupPoints->SpinBox_DY->GetValue();
  GEOM::GEOM_Object_var anObj = GEOM::GEOM_ICurvesOperations::_narrow( getOperation() )->MakeEllipse( myPoint, myDir, aMajorR, aMinorR );

  if ( !anObj->_is_nil() )
    objects.push_back( anObj._retn() );

  return true;
}

//=================================================================================
// function : closeEvent
// purpose  :
//=================================================================================
void BasicGUI_EllipseDlg::closeEvent( QCloseEvent* e )
{
  GEOMBase_Skeleton::closeEvent( e );
}

//=================================================================================
// function : addSubshapeToStudy
// purpose  : virtual method to add new SubObjects if local selection
//=================================================================================
void BasicGUI_EllipseDlg::addSubshapesToStudy()
{
  QMap<QString, GEOM::GEOM_Object_var> objMap;

  objMap[GroupPoints->LineEdit1->text()] = myPoint;
  objMap[GroupPoints->LineEdit2->text()] = myDir;

  addSubshapesToFather( objMap );
}
