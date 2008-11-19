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
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : BasicGUI_PointDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#include "BasicGUI_PointDlg.h"

#include "SUIT_Session.h"
#include "SalomeApp_Application.h"
#include "LightApp_SelectionMgr.h"

#include "GEOMImpl_Types.hxx"

#include <qlabel.h>

#include "utilities.h"

#include <gp_Pnt.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS.hxx>
#include <BRep_Tool.hxx>
#include <TopExp.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#include <qapplication.h>

using namespace std;

//=================================================================================
// class    : BasicGUI_PointDlg()
// purpose  : Constructs a BasicGUI_PointDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
BasicGUI_PointDlg::BasicGUI_PointDlg(GeometryGUI* theGeometryGUI, QWidget* parent,
                                     const char* name, bool modal, WFlags fl)
  :GEOMBase_Skeleton(theGeometryGUI, parent, name, modal, fl )
{
  QPixmap image0(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_DLG_POINT")));
  QPixmap image1(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_DLG_POINT_EDGE")));
  QPixmap image2(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_SELECT")));
  QPixmap image3(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_DLG_POINT_REF")));
  QPixmap image4(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_DLG_POINT_LINES")));
  QPixmap image5(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_DLG_POINT_FACE")));

  setCaption(tr("GEOM_POINT_TITLE"));

  QGroupBox* aFrame = new QGroupBox( 1, Qt::Horizontal, this );
  aFrame->setInsideMargin( 0 );
  aFrame->setFrameStyle( QFrame::NoFrame );

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_POINTS"));
  RadioButton1->setPixmap(image0);
  RadioButton2->setPixmap(image3);
  RadioButton3->setPixmap(image1);
  RadioButton4->show();
  RadioButton4->setPixmap(image4);
  RadioButton5->show();
  RadioButton5->setPixmap(image5);

  GroupXYZ = new DlgRef_3Spin( aFrame, "GroupXYZ" );
  GroupXYZ->GroupBox1->setTitle(tr("GEOM_COORDINATES"));
  GroupXYZ->TextLabel1->setText(tr("GEOM_X"));
  GroupXYZ->TextLabel2->setText(tr("GEOM_Y"));
  GroupXYZ->TextLabel3->setText(tr("GEOM_Z"));

  GroupOnCurve = new DlgRef_1Sel1Spin( aFrame, "GroupOnCurve" );
  GroupOnCurve->GroupBox1->setTitle(tr("GEOM_PARAM_POINT"));
  GroupOnCurve->TextLabel1->setText(tr("GEOM_EDGE"));
  GroupOnCurve->TextLabel2->setText(tr("GEOM_PARAMETER"));
  GroupOnCurve->PushButton1->setPixmap(image2);

  GroupOnSurface = new DlgRef_1Sel2Spin( aFrame, "GroupOnSurface" );
  GroupOnSurface->GroupBox1->setTitle(tr("GEOM_PARAM_POINT"));
  GroupOnSurface->TextLabel1->setText(tr("GEOM_FACE"));
  GroupOnSurface->TextLabel2->setText(tr("GEOM_UPARAMETER"));
  GroupOnSurface->TextLabel3->setText(tr("GEOM_VPARAMETER"));
  GroupOnSurface->PushButton1->setPixmap(image2);

  GroupRefPoint = new DlgRef_1Sel3Spin( aFrame, "GoupRefPoint" );
  GroupRefPoint->GroupBox1->setTitle(tr("GEOM_REF_POINT"));
  GroupRefPoint->TextLabel1->setText(tr("GEOM_POINT"));
  GroupRefPoint->PushButton1->setPixmap(image2);
  GroupRefPoint->TextLabel2->setText(tr("GEOM_DX"));
  GroupRefPoint->TextLabel3->setText(tr("GEOM_DY"));
  GroupRefPoint->TextLabel4->setText(tr("GEOM_DZ"));

  GroupLineIntersection = new DlgRef_2Sel_QTD (aFrame, "GroupLineIntersection");
  GroupLineIntersection->GroupBox1->setTitle(tr("GEOM_LINE_INTERSECTION"));
  GroupLineIntersection->TextLabel1->setText(tr("GEOM_LINE1"));
  GroupLineIntersection->PushButton1->setPixmap(image2);
  GroupLineIntersection->TextLabel2->setText(tr("GEOM_LINE2"));
  GroupLineIntersection->PushButton2->setPixmap(image2);

  Layout1->addWidget( aFrame, 2, 0 );
  /***************************************************************/

  myCoordGrp = new QGroupBox( 2, Qt::Horizontal, tr( "GEOM_COORDINATES" ), aFrame );
  new QLabel( tr( "GEOM_X" ), myCoordGrp );
  myX = new QLineEdit( myCoordGrp );
  new QLabel( tr( "GEOM_Y" ), myCoordGrp );
  myY = new QLineEdit( myCoordGrp );
  new QLabel( tr( "GEOM_Z" ), myCoordGrp );
  myZ = new QLineEdit( myCoordGrp );

  myX->setReadOnly( true );
  myY->setReadOnly( true );
  myZ->setReadOnly( true );

  myX->setEnabled( false );
  myY->setEnabled( false );
  myZ->setEnabled( false );

  QPalette aPal = myX->palette();
  aPal.setColor( QPalette::Disabled, QColorGroup::Text, QColor( 0, 0, 0 ) ) ;
  myX->setPalette( aPal );
  myY->setPalette( aPal );
  myZ->setPalette( aPal );

  setHelpFileName("create_point_page.html");

  Init();
}

//=================================================================================
// function : ~BasicGUI_PointDlg()
// purpose  : Destructor
//=================================================================================
BasicGUI_PointDlg::~BasicGUI_PointDlg()
{
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BasicGUI_PointDlg::Init()
{
  GroupOnCurve->LineEdit1->setReadOnly( true );
  GroupOnSurface->LineEdit1->setReadOnly( true );
  GroupRefPoint->LineEdit1->setReadOnly( true );
  GroupLineIntersection->LineEdit1->setReadOnly( true );
  GroupLineIntersection->LineEdit2->setReadOnly( true );

  myEdge = GEOM::GEOM_Object::_nil();
  myRefPoint = GEOM::GEOM_Object::_nil();

  myEditCurrentArgument = 0;

  /* Get setting of step value from file configuration */
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  double step = resMgr->doubleValue( "Geometry", "SettingsGeomStep", 100);

  /* min, max, step and decimals for spin boxes */
  GroupXYZ->SpinBox_DX->RangeStepAndValidator(COORD_MIN, COORD_MAX, step, DBL_DIGITS_DISPLAY);
  GroupXYZ->SpinBox_DY->RangeStepAndValidator(COORD_MIN, COORD_MAX, step, DBL_DIGITS_DISPLAY);
  GroupXYZ->SpinBox_DZ->RangeStepAndValidator(COORD_MIN, COORD_MAX, step, DBL_DIGITS_DISPLAY);
  GroupXYZ->SpinBox_DX->SetValue(0.0);
  GroupXYZ->SpinBox_DY->SetValue(0.0);
  GroupXYZ->SpinBox_DZ->SetValue(0.0);

  GroupRefPoint->SpinBox_DX->RangeStepAndValidator(COORD_MIN, COORD_MAX, step, DBL_DIGITS_DISPLAY);
  GroupRefPoint->SpinBox_DY->RangeStepAndValidator(COORD_MIN, COORD_MAX, step, DBL_DIGITS_DISPLAY);
  GroupRefPoint->SpinBox_DZ->RangeStepAndValidator(COORD_MIN, COORD_MAX, step, DBL_DIGITS_DISPLAY);
  GroupRefPoint->SpinBox_DX->SetValue(0.0);
  GroupRefPoint->SpinBox_DY->SetValue(0.0);
  GroupRefPoint->SpinBox_DZ->SetValue(0.0);

  step = 0.1;
  GroupOnCurve->SpinBox_DX->RangeStepAndValidator(0., 1., step, DBL_DIGITS_DISPLAY);
  GroupOnCurve->SpinBox_DX->SetValue( 0.5 );

  GroupOnSurface->SpinBox_DX->RangeStepAndValidator(0., 1., step, DBL_DIGITS_DISPLAY);
  GroupOnSurface->SpinBox_DX->SetValue( 0.5 );
  GroupOnSurface->SpinBox_DY->RangeStepAndValidator(0., 1., step, DBL_DIGITS_DISPLAY);
  GroupOnSurface->SpinBox_DY->SetValue( 0.5 );

  /* signals and slots connections */
  connect(buttonCancel, SIGNAL(clicked()), this, SLOT(ClickOnCancel()));
  connect(myGeomGUI, SIGNAL(SignalDeactivateActiveDialog()), this, SLOT(DeactivateActiveDialog()));
  connect(myGeomGUI, SIGNAL(SignalCloseAllDialogs()), this, SLOT(ClickOnCancel()));

  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));
  connect(GroupConstructors, SIGNAL(clicked(int)), this, SLOT(ConstructorsClicked(int)));

  connect(GroupOnCurve->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupOnCurve->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupOnSurface->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupOnSurface->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupLineIntersection->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupLineIntersection->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupLineIntersection->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupLineIntersection->LineEdit2, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(GroupOnCurve->SpinBox_DX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupOnSurface->SpinBox_DX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupOnSurface->SpinBox_DY, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupXYZ->SpinBox_DX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupXYZ->SpinBox_DY, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupXYZ->SpinBox_DZ, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupRefPoint->SpinBox_DX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupRefPoint->SpinBox_DY, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupRefPoint->SpinBox_DZ, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));

  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)),
          GroupOnCurve->SpinBox_DX, SLOT(SetStep(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)),
          GroupOnSurface->SpinBox_DX, SLOT(SetStep(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)),
          GroupOnSurface->SpinBox_DY, SLOT(SetStep(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)),
          GroupXYZ->SpinBox_DX, SLOT(SetStep(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)),
          GroupXYZ->SpinBox_DY, SLOT(SetStep(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)),
          GroupXYZ->SpinBox_DZ, SLOT(SetStep(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)),
          GroupRefPoint->SpinBox_DX, SLOT(SetStep(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)),
          GroupRefPoint->SpinBox_DY, SLOT(SetStep(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)),
          GroupRefPoint->SpinBox_DZ, SLOT(SetStep(double)));

  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));

  initName( tr("GEOM_VERTEX") );

  ConstructorsClicked( 0 );
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void BasicGUI_PointDlg::ConstructorsClicked (int constructorId)
{
  globalSelection(); // close local contexts, if any

  switch (constructorId)
  {
  case 0:
    {
      localSelection(GEOM::GEOM_Object::_nil(), TopAbs_VERTEX);

      GroupRefPoint->hide();
      GroupOnCurve->hide();
      GroupLineIntersection->hide();
      GroupXYZ->show();
      GroupOnSurface->hide();
      myCoordGrp->hide();
      break;
    }
  case 1:
    {
      myEditCurrentArgument = GroupRefPoint->LineEdit1;
      myEditCurrentArgument->setText("");
      myRefPoint = GEOM::GEOM_Object::_nil();

      localSelection(GEOM::GEOM_Object::_nil(), TopAbs_VERTEX);

      GroupXYZ->hide();
      GroupOnCurve->hide();
      GroupRefPoint->show();
      GroupLineIntersection->hide();
      GroupOnSurface->hide();
      myCoordGrp->show();
      break;
    }
  case 2:
    {
      myEditCurrentArgument = GroupOnCurve->LineEdit1;
      myEditCurrentArgument->setText("");
      myEdge = GEOM::GEOM_Object::_nil();

      localSelection(GEOM::GEOM_Object::_nil(), TopAbs_EDGE);

      GroupXYZ->hide();
      GroupRefPoint->hide();
      GroupOnCurve->show();
      GroupLineIntersection->hide();
      GroupOnSurface->hide();
      myCoordGrp->show();
      break;
    }
  case 3:
    {
      myEditCurrentArgument = GroupLineIntersection->LineEdit1;
      GroupLineIntersection->LineEdit1->setText("");
      GroupLineIntersection->LineEdit2->setText("");
      myLine1 = GEOM::GEOM_Object::_nil();
      myLine2 = GEOM::GEOM_Object::_nil();

      localSelection(GEOM::GEOM_Object::_nil(), TopAbs_EDGE);

      GroupXYZ->hide();
      GroupRefPoint->hide();
      GroupOnCurve->hide();
      GroupLineIntersection->show();
      GroupOnSurface->hide();
      myCoordGrp->hide();
      break;
    }
  case 4:
    {
      myEditCurrentArgument = GroupOnSurface->LineEdit1;
      myEditCurrentArgument->setText("");
      myFace = GEOM::GEOM_Object::_nil();

      localSelection(GEOM::GEOM_Object::_nil(), TopAbs_FACE);

      GroupXYZ->hide();
      GroupRefPoint->hide();
      GroupOnCurve->hide();
      GroupLineIntersection->hide();
      GroupOnSurface->show();
      myCoordGrp->show();
      break;
    }
  }

  myX->setText( "" );
  myY->setText( "" );
  myZ->setText( "" );

  qApp->processEvents();
  updateGeometry();
  resize( minimumSize() );

  SelectionIntoArgument();
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void BasicGUI_PointDlg::ClickOnOk()
{
  if (onAccept())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool BasicGUI_PointDlg::ClickOnApply()
{
  if (!onAccept())
    return false;

  initName();
  ConstructorsClicked(getConstructorId());
  return true;
}

//=================================================================================
// function : ClickOnCancel()
// purpose  :
//=================================================================================
void BasicGUI_PointDlg::ClickOnCancel()
{
  GEOMBase_Skeleton::ClickOnCancel();
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed (for constructors not using local context)
//=================================================================================
void BasicGUI_PointDlg::SelectionIntoArgument()
{
  const int id = getConstructorId();

  if ((id == 1 || id == 2 || id == 4 ) && myEditCurrentArgument != 0)
  {
    myEditCurrentArgument->setText("");
    myX->setText( "" );
    myY->setText( "" );
    myZ->setText( "" );
    myRefPoint = myEdge = myFace = GEOM::GEOM_Object::_nil();
  }

  if (IObjectCount() == 1)
  {
    Standard_Boolean aRes = Standard_False;
    Handle(SALOME_InteractiveObject) anIO = firstIObject();
    GEOM::GEOM_Object_var aSelectedObject = GEOMBase::ConvertIOinGEOMObject(anIO, aRes);
    if (!CORBA::is_nil(aSelectedObject) && aRes)
    {
      QString aName = GEOMBase::GetName(aSelectedObject);
      TopAbs_ShapeEnum aNeedType = TopAbs_VERTEX;
      TopoDS_Shape aShape;
      if (GEOMBase::GetShape(aSelectedObject, aShape, TopAbs_SHAPE) && !aShape.IsNull())
      {
        if (id == 2 || id == 3)
          aNeedType = TopAbs_EDGE;
        if(id == 4)
          aNeedType = TopAbs_FACE;

        LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
        TColStd_IndexedMapOfInteger aMap;
        aSelMgr->GetIndexes(firstIObject(), aMap);
        if (aMap.Extent() == 1) // Local Selection
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
	  }
	  else
	    aSelectedObject = aFindedObject; // get Object from study
	  GEOMBase::GetShape(aSelectedObject, aShape, TopAbs_SHAPE);
        }
        else // Global Selection
        {
          if (aShape.ShapeType() != aNeedType) {
            aSelectedObject = GEOM::GEOM_Object::_nil();
            aName = "";
            if (id == 0) return;
          }
        }
      }

      if (id == 0)
      {
        if (aShape.IsNull() || aShape.ShapeType() != aNeedType)
	  return;
        gp_Pnt aPnt = BRep_Tool::Pnt( TopoDS::Vertex( aShape ) );
        GroupXYZ->SpinBox_DX->SetValue( aPnt.X() );
        GroupXYZ->SpinBox_DY->SetValue( aPnt.Y() );
        GroupXYZ->SpinBox_DZ->SetValue( aPnt.Z() );
      }
      else if (id == 1)
      {
        myRefPoint = aSelectedObject;
        GroupRefPoint->LineEdit1->setText(aName);
      }
      else if (id == 2)
      {
        myEdge = aSelectedObject;
        GroupOnCurve->LineEdit1->setText(aName);
      }
      else if (id == 3)
      {
        if (myEditCurrentArgument == GroupLineIntersection->LineEdit1) {
          myLine1 = aSelectedObject;
          myEditCurrentArgument->setText(aName);
        }
        else if (myEditCurrentArgument == GroupLineIntersection->LineEdit2) {
          myLine2 = aSelectedObject;
          myEditCurrentArgument->setText(aName);
        }
      }
      else if (id == 4)
      {
        myFace = aSelectedObject;
        GroupOnSurface->LineEdit1->setText(aName);
      }
    }
  }

  displayPreview();
}

//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void BasicGUI_PointDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if ( send == GroupRefPoint->LineEdit1 || send == GroupOnCurve->LineEdit1 ||
       send == GroupLineIntersection->LineEdit1 || send == GroupLineIntersection->LineEdit2  )
  {
    myEditCurrentArgument = send;
    GEOMBase_Skeleton::LineEditReturnPressed();
  }
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BasicGUI_PointDlg::SetEditCurrentArgument()
{
  globalSelection(); // close local contexts, if any

  QPushButton* send = (QPushButton*)sender();
  globalSelection( GEOM_POINT); // to break previous local selection

  if ( send == GroupRefPoint->PushButton1 )
  {
    GroupRefPoint->LineEdit1->setFocus();
    myEditCurrentArgument = GroupRefPoint->LineEdit1;

    localSelection( GEOM::GEOM_Object::_nil(), TopAbs_VERTEX );
  }
  else if ( send == GroupOnCurve->PushButton1 )
  {
    GroupOnCurve->LineEdit1->setFocus();
    myEditCurrentArgument = GroupOnCurve->LineEdit1;

    localSelection( GEOM::GEOM_Object::_nil(), TopAbs_EDGE );
  }
  else if ( send == GroupOnSurface->PushButton1 )
  {
    GroupOnSurface->LineEdit1->setFocus();
    myEditCurrentArgument = GroupOnSurface->LineEdit1;

    localSelection( GEOM::GEOM_Object::_nil(), TopAbs_FACE );
  }
  else if ( send == GroupLineIntersection->PushButton1 )
  {
    GroupLineIntersection->LineEdit1->setFocus();
    myEditCurrentArgument = GroupLineIntersection->LineEdit1;

    localSelection( GEOM::GEOM_Object::_nil(), TopAbs_EDGE );
  }
  else if ( send == GroupLineIntersection->PushButton2 )
  {
    GroupLineIntersection->LineEdit2->setFocus();
    myEditCurrentArgument = GroupLineIntersection->LineEdit2;

    localSelection( GEOM::GEOM_Object::_nil(), TopAbs_EDGE );
  }
}

//=================================================================================
// function : enterEvent()
// purpose  : to reactivate this dialog box when mouse enter onto the window
//=================================================================================
void BasicGUI_PointDlg::enterEvent(QEvent* e)
{
  if ( !GroupConstructors->isEnabled() )
    ActivateThisDialog();
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void BasicGUI_PointDlg::ActivateThisDialog( )
{
  GEOMBase_Skeleton::ActivateThisDialog();
  // myGeomGUI->SetState( 0 );
  ConstructorsClicked( getConstructorId() );
}

//=================================================================================
// function : DeactivateActiveDialog()
// purpose  : public slot to deactivate if active
//=================================================================================
void BasicGUI_PointDlg::DeactivateActiveDialog()
{
  // myGeomGUI->SetState( -1 );
  GEOMBase_Skeleton::DeactivateActiveDialog();
}

//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void BasicGUI_PointDlg::ValueChangedInSpinBox(double newValue)
{
  displayPreview();
}

//=================================================================================
// funcion  : getParameter()
// purpose  :
//=================================================================================
double BasicGUI_PointDlg::getParameter() const
{
  return GroupOnCurve->SpinBox_DX->GetValue();
}


//=================================================================================
// funcion  : getUParameter()
// purpose  :
//=================================================================================
double BasicGUI_PointDlg::getUParameter() const
{
  return GroupOnSurface->SpinBox_DX->GetValue();
}


//=================================================================================
// funcion  : getVParameter()
// purpose  :
//=================================================================================
double BasicGUI_PointDlg::getVParameter() const
{
  return GroupOnSurface->SpinBox_DY->GetValue();
}


//=================================================================================
// function : OnPointSelected
// purpose  :
//=================================================================================
void BasicGUI_PointDlg::OnPointSelected( const gp_Pnt& thePnt )
{
  if ( getConstructorId() == 0 )
  {
    GroupXYZ->SpinBox_DX->SetValue( thePnt.X() );
    GroupXYZ->SpinBox_DY->SetValue( thePnt.Y() );
    GroupXYZ->SpinBox_DZ->SetValue( thePnt.Z() );

    displayPreview();
  }
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr BasicGUI_PointDlg::createOperation()
{
  return myGeomGUI->GetGeomGen()->GetIBasicOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool BasicGUI_PointDlg::isValid( QString& msg )
{
  const int id = getConstructorId();
  if ( id == 0 )
    return true;
  else if ( id == 1 )
    return !myRefPoint->_is_nil();
  else if ( id == 2 )
    return !myEdge->_is_nil();
  else if ( id == 3)
    return ( !myLine1->_is_nil() && !myLine2->_is_nil() );
  else if ( id == 4 )
    return !myFace->_is_nil();
  return false;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool BasicGUI_PointDlg::execute( ObjectList& objects )
{
  bool res = false;

  GEOM::GEOM_Object_var anObj;

  switch ( getConstructorId() )
  {
  case 0 :
  {
    double x = GroupXYZ->SpinBox_DX->GetValue();
    double y = GroupXYZ->SpinBox_DY->GetValue();
    double z = GroupXYZ->SpinBox_DZ->GetValue();

    anObj = GEOM::GEOM_IBasicOperations::_narrow( getOperation() )->MakePointXYZ( x,y,z );
    res = true;
    break;
  }
  case 1 :
  {
    double dx = GroupRefPoint->SpinBox_DX->GetValue();
    double dy = GroupRefPoint->SpinBox_DY->GetValue();
    double dz = GroupRefPoint->SpinBox_DZ->GetValue();

    anObj = GEOM::GEOM_IBasicOperations::_narrow( getOperation() )->
      MakePointWithReference( myRefPoint, dx, dy, dz );
    res = true;
    break;
  }
  case 2 :
    anObj = GEOM::GEOM_IBasicOperations::_narrow( getOperation() )->
      MakePointOnCurve( myEdge, getParameter() );
    res = true;
    break;
  case 3 :
    anObj = GEOM::GEOM_IBasicOperations::_narrow( getOperation() )->
      MakePointOnLinesIntersection( myLine1, myLine2 );
      res = true;
      break;
  case 4 :
    anObj = GEOM::GEOM_IBasicOperations::_narrow( getOperation() )->
      MakePointOnSurface( myFace, getUParameter(), getVParameter() );
    res = true;
    break;
  }

  if ( getConstructorId() == 1 || getConstructorId() == 2 ||
       getConstructorId() == 4 ) {
    TopoDS_Shape aShape;
    if ( GEOMBase::GetShape( anObj, aShape ) && !aShape.IsNull() &&
         aShape.ShapeType() == TopAbs_VERTEX ) {
      gp_Pnt aPnt = BRep_Tool::Pnt( TopoDS::Vertex( aShape ) );
      myX->setText( QString( "%1" ).arg( aPnt.X() ) );
      myY->setText( QString( "%1" ).arg( aPnt.Y() ) );
      myZ->setText( QString( "%1" ).arg( aPnt.Z() ) );
    }
    else {
      myX->setText( "" );
      myY->setText( "" );
      myZ->setText( "" );
    }
  }

  if ( !anObj->_is_nil() )
  {
    //printf( "--> a valid point is created\n" );
    objects.push_back( anObj._retn() );
  }

  return res;
}

//=================================================================================
// function : closeEvent
// purpose  :
//=================================================================================
void BasicGUI_PointDlg::closeEvent( QCloseEvent* e )
{
  GEOMBase_Skeleton::closeEvent( e );
}

//=================================================================================
// function : addSubshapeToStudy
// purpose  : virtual method to add new SubObjects if local selection
//=================================================================================
void BasicGUI_PointDlg::addSubshapesToStudy()
{
  QMap<QString, GEOM::GEOM_Object_var> objMap;

  switch (getConstructorId())
  {
  case 0:
    break;
  case 1:
    objMap[GroupRefPoint->LineEdit1->text()] = myRefPoint;
    break;
  case 2:
    objMap[GroupOnCurve->LineEdit1->text()] = myEdge;
    break;
  case 3:
    objMap[GroupLineIntersection->LineEdit1->text()] = myLine1;
    objMap[GroupLineIntersection->LineEdit2->text()] = myLine2;
    break;
  case 4:
    objMap[GroupOnSurface->LineEdit1->text()] = myFace;
    break;
  }
 addSubshapesToFather( objMap );
}
