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

//  GEOM GEOMGUI : GUI for Geometry component
//  File   : BasicGUI_PointDlg.cxx
//  Author : Lucien PIGNOLONI, Open CASCADE S.A.S.

#include "BasicGUI_PointDlg.h"

#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>

#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <DlgRef.h>

#include <GEOMImpl_Types.hxx>

#include <QApplication>
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include <QMenu>
#include <QTimer>
#include <QList>

#include <gp_Pnt.hxx>
#include <TopoDS_Shape.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS.hxx>
#include <BRep_Tool.hxx>
#include <TopExp.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#define PARAM_VALUE 0
#define COORD_VALUE 1
#define LENGTH_VALUE 2

#define GEOM_POINT_XYZ    0
#define GEOM_POINT_REF    1
#define GEOM_POINT_EDGE   2
#define GEOM_POINT_INTINT 3
#define GEOM_POINT_SURF   4

#define SPACING 6
#define MARGIN  9

//=================================================================================
// class    : BasicGUI_PointDlg()
// purpose  : Constructs a BasicGUI_PointDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
BasicGUI_PointDlg::BasicGUI_PointDlg(GeometryGUI* theGeometryGUI, QWidget* parent,
                                      bool modal, Qt::WindowFlags fl)
  : GEOMBase_Skeleton(theGeometryGUI, parent, modal, fl),
    myBusy (false)
{
  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
  QPixmap image0 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_POINT")));
  QPixmap image1 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_POINT_EDGE")));
  QPixmap image2 (aResMgr->loadPixmap("GEOM", tr("ICON_SELECT")));
  QPixmap image3 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_POINT_REF")));
  QPixmap image4 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_POINT_LINES")));
  QPixmap image5 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_POINT_FACE")));
  QPixmap image6 (aResMgr->loadPixmap("GEOM", tr("ICO_LINE")));
  QPixmap image7 (aResMgr->loadPixmap("GEOM", tr("ICO_WIRE")));

  setWindowTitle(tr("GEOM_POINT_TITLE"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_POINTS"));
  mainFrame()->RadioButton1->setIcon(image0);
  mainFrame()->RadioButton2->setIcon(image3);
  mainFrame()->RadioButton3->setIcon(image1);
  mainFrame()->RadioButton4->show();
  mainFrame()->RadioButton4->setIcon(image4);
  mainFrame()->RadioButton5->show();
  mainFrame()->RadioButton5->setIcon(image5);

  myParamGroup = new QGroupBox(centralWidget());
  myParamCoord = new QButtonGroup(myParamGroup);
  QHBoxLayout* boxLayout = new QHBoxLayout(myParamGroup);
  boxLayout->setMargin(MARGIN); boxLayout->setSpacing(SPACING);
  QRadioButton* btn = new QRadioButton(tr("GEOM_PARAM_VALUE"), myParamGroup);
  myParamCoord->addButton(btn, PARAM_VALUE);
  boxLayout->addWidget(btn);
  btn = new QRadioButton(tr("GEOM_LENGTH_VALUE"), myParamGroup);
  myParamCoord->addButton(btn, LENGTH_VALUE);
  boxLayout->addWidget(btn);
  btn = new QRadioButton(tr("GEOM_COORD_VALUE"), myParamGroup);
  myParamCoord->addButton(btn, COORD_VALUE);
  boxLayout->addWidget(btn);
  myParamCoord->setExclusive(true);
  myParamCoord->button(PARAM_VALUE)->setChecked(true);

  GroupXYZ = new DlgRef_3Spin(centralWidget());
  GroupXYZ->GroupBox1->setTitle(tr("GEOM_COORDINATES"));
  GroupXYZ->TextLabel1->setText(tr("GEOM_X"));
  GroupXYZ->TextLabel2->setText(tr("GEOM_Y"));
  GroupXYZ->TextLabel3->setText(tr("GEOM_Z"));

  GroupOnCurve = new DlgRef_2Sel1Spin2Check(centralWidget());
  GroupOnCurve->GroupBox1->setTitle(tr("GEOM_POINT_ON_EDGE"));
  GroupOnCurve->TextLabel1->setText(tr("GEOM_EDGE"));
  GroupOnCurve->TextLabel2->setText(tr("GEOM_START_POINT"));
  GroupOnCurve->TextLabel3->setText(tr("GEOM_PARAMETER"));
  GroupOnCurve->PushButton1->setIcon(image2);
  GroupOnCurve->PushButton2->setIcon(image2);
  GroupOnCurve->TextLabel3->setText(tr("GEOM_PARAMETER"));
  GroupOnCurve->CheckButton1->setText(tr("GEOM_TAKE_ORIENTATION_INTO_ACCOUNT"));
  GroupOnCurve->CheckButton2->setAttribute( Qt::WA_DeleteOnClose );
  GroupOnCurve->CheckButton2->close();

  GroupOnSurface = new DlgRef_1Sel2Spin(centralWidget());
  GroupOnSurface->GroupBox1->setTitle(tr("GEOM_POINT_ON_FACE"));
  GroupOnSurface->TextLabel1->setText(tr("GEOM_FACE"));
  GroupOnSurface->TextLabel2->setText(tr("GEOM_UPARAMETER"));
  GroupOnSurface->TextLabel3->setText(tr("GEOM_VPARAMETER"));
  GroupOnSurface->PushButton1->setIcon(image2);

  GroupRefPoint = new DlgRef_1Sel3Spin(centralWidget());
  GroupRefPoint->GroupBox1->setTitle(tr("GEOM_REF_POINT"));
  GroupRefPoint->TextLabel1->setText(tr("GEOM_POINT"));
  GroupRefPoint->PushButton1->setIcon(image2);
  GroupRefPoint->TextLabel2->setText(tr("GEOM_DX"));
  GroupRefPoint->TextLabel3->setText(tr("GEOM_DY"));
  GroupRefPoint->TextLabel4->setText(tr("GEOM_DZ"));

  /* popup menu for line intersect buttons */
  QIcon ico_line = QIcon(image6);
  QIcon ico_wire = QIcon(image7);

  GroupLineIntersection = new DlgRef_2Sel(centralWidget());
  GroupLineIntersection->GroupBox1->setTitle(tr("GEOM_LINE_INTERSECTION"));
  GroupLineIntersection->TextLabel1->setText(tr("GEOM_LINE1"));
  GroupLineIntersection->TextLabel2->setText(tr("GEOM_LINE2"));
  GroupLineIntersection->PushButton1->setIcon(image2);
  GroupLineIntersection->PushButton2->setIcon(image2);
  GroupLineIntersection->LineEdit2->setEnabled(false);

  myCoordGrp = new QGroupBox(tr("GEOM_COORDINATES_RES"), centralWidget());
  QGridLayout* myCoordGrpLayout = new QGridLayout(myCoordGrp);
  myCoordGrpLayout->addWidget(new QLabel(tr("GEOM_X"), myCoordGrp), 0, 0);
  myX = new QLineEdit(myCoordGrp);
  myCoordGrpLayout->addWidget(myX, 0, 1);
  myCoordGrpLayout->addWidget(new QLabel(tr("GEOM_Y"), myCoordGrp), 1, 0);
  myY = new QLineEdit(myCoordGrp);
  myCoordGrpLayout->addWidget(myY, 1, 1);
  myCoordGrpLayout->addWidget(new QLabel(tr("GEOM_Z"), myCoordGrp), 2, 0);
  myZ = new QLineEdit(myCoordGrp);
  myCoordGrpLayout->addWidget(myZ, 2, 1);

  QVBoxLayout* layout = new QVBoxLayout(centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
  layout->addWidget(myParamGroup);
  layout->addWidget(GroupXYZ);
  layout->addWidget(GroupOnCurve);
  layout->addWidget(GroupOnSurface);
  layout->addWidget(GroupRefPoint);
  layout->addWidget(GroupLineIntersection);
  layout->addWidget(myCoordGrp);

  myX->setReadOnly(true);
  myY->setReadOnly(true);
  myZ->setReadOnly(true);

  myX->setEnabled(false);
  myY->setEnabled(false);
  myZ->setEnabled(false);

  QPalette aPal = myX->palette();
  aPal.setColor(QPalette::Disabled, QPalette::Text, QColor(0, 0, 0));
  myX->setPalette(aPal);
  myY->setPalette(aPal);
  myZ->setPalette(aPal);

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
  GroupOnCurve->LineEdit1->setReadOnly(true);
  GroupOnCurve->LineEdit2->setReadOnly(true);
  GroupOnSurface->LineEdit1->setReadOnly(true);
  GroupRefPoint->LineEdit1->setReadOnly(true);
  GroupLineIntersection->LineEdit1->setReadOnly(true);
  GroupLineIntersection->LineEdit2->setReadOnly(true);
  GroupLineIntersection->LineEdit1->setEnabled(true);
  GroupLineIntersection->LineEdit2->setEnabled(false);

  myEdge.nullify();
  myFace.nullify();
  myRefPoint.nullify();
  myLine1.nullify();
  myLine2.nullify();
  myNeedType = TopAbs_VERTEX;

  myEditCurrentArgument = 0;

  /* Get setting of step value from file configuration */
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  double step = resMgr->doubleValue("Geometry", "SettingsGeomStep", 100);

  /* min, max, step and decimals for spin boxes */
  initSpinBox(GroupXYZ->SpinBox_DX, COORD_MIN, COORD_MAX, step, "length_precision");
  initSpinBox(GroupXYZ->SpinBox_DY, COORD_MIN, COORD_MAX, step, "length_precision");
  initSpinBox(GroupXYZ->SpinBox_DZ, COORD_MIN, COORD_MAX, step, "length_precision");
  GroupXYZ->SpinBox_DX->setValue(0.0);
  GroupXYZ->SpinBox_DY->setValue(0.0);
  GroupXYZ->SpinBox_DZ->setValue(0.0);

  initSpinBox(GroupRefPoint->SpinBox_DX, COORD_MIN, COORD_MAX, step, "length_precision");
  initSpinBox(GroupRefPoint->SpinBox_DY, COORD_MIN, COORD_MAX, step, "length_precision");
  initSpinBox(GroupRefPoint->SpinBox_DZ, COORD_MIN, COORD_MAX, step, "length_precision");
  GroupRefPoint->SpinBox_DX->setValue(0.0);
  GroupRefPoint->SpinBox_DY->setValue(0.0);
  GroupRefPoint->SpinBox_DZ->setValue(0.0);

  step = 0.1;

  initSpinBox(GroupOnCurve->SpinBox_DX, 0., 1., step, "parametric_precision");
  GroupOnCurve->SpinBox_DX->setValue(0.5);
  GroupOnCurve->CheckButton1->setChecked(true);

  initSpinBox(GroupOnSurface->SpinBox_DX, 0., 1., step, "parametric_precision");
  GroupOnSurface->SpinBox_DX->setValue(0.5);
  initSpinBox(GroupOnSurface->SpinBox_DY, 0., 1., step, "parametric_precision");
  GroupOnSurface->SpinBox_DY->setValue(0.5);

  /* signals and slots connections */
  connect(myGeomGUI,      SIGNAL(SignalDeactivateActiveDialog()), this, SLOT(DeactivateActiveDialog()));
  connect(myGeomGUI,      SIGNAL(SignalCloseAllDialogs()),        this, SLOT(ClickOnCancel()));

  connect(buttonOk(),     SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(),  SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(this,           SIGNAL(constructorsClicked(int)), this, SLOT(ConstructorsClicked(int)));

  connect(myParamCoord,   SIGNAL(buttonClicked(int)), this, SLOT(ClickParamCoord(int)));

  connect(GroupOnCurve->CheckButton1,         SIGNAL(toggled(bool)), this, SLOT(CheckBoxClicked()));

  connect(GroupOnCurve->PushButton1,          SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupOnCurve->PushButton2,          SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupOnSurface->PushButton1,        SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupLineIntersection->PushButton1, SIGNAL(pressed()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupLineIntersection->PushButton2, SIGNAL(pressed()), this, SLOT(SetEditCurrentArgument()));
  
  connect(GroupOnCurve->SpinBox_DX,   SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupOnSurface->SpinBox_DX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupOnSurface->SpinBox_DY, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupXYZ->SpinBox_DX,       SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupXYZ->SpinBox_DY,       SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupXYZ->SpinBox_DZ,       SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupRefPoint->SpinBox_DX,  SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupRefPoint->SpinBox_DY,  SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupRefPoint->SpinBox_DZ,  SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));

  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), this,  SLOT(SetDoubleSpinBoxStep(double)));

  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
           this,  SLOT(SelectionIntoArgument()));

  initName(tr("GEOM_VERTEX"));

  ConstructorsClicked(0);
}

//=================================================================================
// function : SetDoubleSpinBoxStep()
// purpose  : Double spin box management
//=================================================================================
void BasicGUI_PointDlg::SetDoubleSpinBoxStep(double step)
{
  GroupOnCurve->SpinBox_DX->setSingleStep(step);
  GroupXYZ->SpinBox_DX->setSingleStep(step);
  GroupXYZ->SpinBox_DY->setSingleStep(step);
  GroupXYZ->SpinBox_DZ->setSingleStep(step);
  GroupRefPoint->SpinBox_DX->setSingleStep(step);
  GroupRefPoint->SpinBox_DY->setSingleStep(step);
  GroupRefPoint->SpinBox_DZ->setSingleStep(step);
}


//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void BasicGUI_PointDlg::ConstructorsClicked(int constructorId)
{
  switch (constructorId) {
  case GEOM_POINT_XYZ:
    {
      globalSelection(); // close local contexts, if any
      myNeedType = TopAbs_VERTEX;
      localSelection(myNeedType);

      GroupRefPoint->hide();
      GroupOnCurve->hide();
      GroupLineIntersection->hide();
      GroupOnSurface->hide();
      myCoordGrp->hide();
      myParamGroup->hide();
      GroupXYZ->show();
      break;
    }
  case GEOM_POINT_REF:
    {
      globalSelection(); // close local contexts, if any
      myNeedType = TopAbs_VERTEX;
      localSelection(myNeedType);

      myEditCurrentArgument = GroupRefPoint->LineEdit1;
      myEditCurrentArgument->setText("");
      myRefPoint.nullify();
      GroupRefPoint->PushButton1->setDown(true);
      myParamGroup->hide();
      GroupXYZ->hide();
      GroupOnCurve->hide();
      GroupLineIntersection->hide();
      GroupOnSurface->hide();
      GroupRefPoint->show();
      myCoordGrp->show();
      break;
    }
  case GEOM_POINT_EDGE:
    {
      globalSelection(); // close local contexts, if any
      myNeedType = TopAbs_EDGE;
      localSelection(myNeedType);

      myEditCurrentArgument = GroupOnCurve->LineEdit1;
      myEditCurrentArgument->setText("");
      myEdge.nullify();
      myRefPoint.nullify();
      GroupOnCurve->PushButton1->setDown(true);
      GroupRefPoint->hide();
      GroupLineIntersection->hide();
      GroupOnSurface->hide();
      myParamGroup->show();
      myParamCoord->button(LENGTH_VALUE)->show();
      myParamCoord->button(PARAM_VALUE)->setChecked(true);
      GroupOnCurve->show();
      myCoordGrp->show();
      updateParamCoord(false);
      break;
    }
  case GEOM_POINT_INTINT:
    {
      globalSelection(); // close local contexts, if any
      std::list<int> needTypes;
      needTypes.push_back( TopAbs_EDGE ), needTypes.push_back( TopAbs_WIRE );
      localSelection(needTypes );

      myEditCurrentArgument = GroupLineIntersection->LineEdit1;
      GroupLineIntersection->LineEdit1->setText("");
      GroupLineIntersection->LineEdit2->setText("");
      GroupLineIntersection->LineEdit1->setEnabled(true);
      GroupLineIntersection->LineEdit2->setEnabled(false);
      myLine1.nullify();
      myLine2.nullify();
      GroupLineIntersection->PushButton1->setDown(true);
      GroupLineIntersection->PushButton2->setDown(false);
      myParamGroup->hide();
      GroupXYZ->hide();
      GroupRefPoint->hide();
      GroupOnCurve->hide();
      GroupOnSurface->hide();
      myCoordGrp->hide();
      GroupLineIntersection->show();
      break;
    }
  case GEOM_POINT_SURF:
    {
      globalSelection(); // close local contexts, if any
      myNeedType = TopAbs_FACE;
      localSelection(myNeedType);

      myEditCurrentArgument = GroupOnSurface->LineEdit1;
      myEditCurrentArgument->setText("");
      myFace.nullify();
      GroupOnSurface->PushButton1->setDown(true);
      GroupRefPoint->hide();
      GroupOnCurve->hide();
      GroupLineIntersection->hide();
      myParamGroup->show();
      myParamCoord->button(LENGTH_VALUE)->hide();
      myParamCoord->button(PARAM_VALUE)->setChecked(true);
      GroupOnSurface->show();
      myCoordGrp->show();
      updateParamCoord(false);
      break;
    }
  }

  myX->setText("");
  myY->setText("");
  myZ->setText("");

  QTimer::singleShot(50, this, SLOT(updateSize()));

  SelectionIntoArgument();
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void BasicGUI_PointDlg::ClickOnOk()
{
  setIsApplyAndClose(true);
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
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed (for constructors not using local context)
//=================================================================================
void BasicGUI_PointDlg::SelectionIntoArgument()
{
  erasePreview();
  const int id = getConstructorId();

  if ((id == GEOM_POINT_REF || id == GEOM_POINT_EDGE || id == GEOM_POINT_SURF)
       && myEditCurrentArgument != 0)
  {
    myEditCurrentArgument->setText("");
    myX->setText("");
    myY->setText("");
    myZ->setText("");
    myFace.nullify();
    if (myEditCurrentArgument == GroupOnCurve->LineEdit1)
      myEdge.nullify();
    else if (myEditCurrentArgument == GroupOnCurve->LineEdit2)
      myRefPoint.nullify();
  }
  else if (id == GEOM_POINT_INTINT) {
    myEditCurrentArgument->setText("");
    if (myEditCurrentArgument == GroupLineIntersection->LineEdit1)
      myLine1.nullify();
    else if (myEditCurrentArgument == GroupLineIntersection->LineEdit2)
      myLine2.nullify();
  }

  GEOM::GeomObjPtr aSelectedObject;
  if (id == GEOM_POINT_INTINT) {
    QList<TopAbs_ShapeEnum> needTypes;
    needTypes << TopAbs_EDGE, needTypes << TopAbs_WIRE;
    aSelectedObject= getSelected(needTypes);
  }
  else 
    aSelectedObject= getSelected(myNeedType);
  TopoDS_Shape aShape;
  if (aSelectedObject && GEOMBase::GetShape(aSelectedObject.get(), aShape) && !aShape.IsNull()) {
    QString aName = GEOMBase::GetName(aSelectedObject.get());
    myBusy = true;
    if (id == GEOM_POINT_XYZ) {
      gp_Pnt aPnt = BRep_Tool::Pnt(TopoDS::Vertex(aShape));
      GroupXYZ->SpinBox_DX->setValue(aPnt.X());
      GroupXYZ->SpinBox_DY->setValue(aPnt.Y());
      GroupXYZ->SpinBox_DZ->setValue(aPnt.Z());
    }
    else if (id == GEOM_POINT_REF) {
      myRefPoint = aSelectedObject;
      GroupRefPoint->LineEdit1->setText(aName);
    }
    else if (id == GEOM_POINT_EDGE) {
      myEditCurrentArgument->setText(aName);
      if (myEditCurrentArgument == GroupOnCurve->LineEdit1) {
        myEdge = aSelectedObject;
        if (myEdge && !myRefPoint) {
          GroupOnCurve->PushButton2->click();
        }
      }
      else if (myEditCurrentArgument == GroupOnCurve->LineEdit2) {
        myRefPoint = aSelectedObject;
        if (myRefPoint && !myEdge) {
          GroupOnCurve->PushButton1->click();
        }
      }
    }
    else if (id == GEOM_POINT_INTINT) {
      myEditCurrentArgument->setText(aName);
      if (myEditCurrentArgument == GroupLineIntersection->LineEdit1) {
        myLine1 = aSelectedObject;
        if (myLine1 && !myLine2) {
          GroupLineIntersection->PushButton2->click();
        }
      }
      else if (myEditCurrentArgument == GroupLineIntersection->LineEdit2) {
        myLine2 = aSelectedObject;
        if (myLine2 && !myLine1) {
          GroupLineIntersection->PushButton1->click();
        }
      }
    }
    else if (id == GEOM_POINT_SURF) {
      myFace = aSelectedObject;
      GroupOnSurface->LineEdit1->setText(aName);
    }
    myBusy = false;
  }

  displayPreview(true);
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BasicGUI_PointDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  if (send == GroupRefPoint->PushButton1) {
    GroupRefPoint->LineEdit1->setFocus();
    myEditCurrentArgument = GroupRefPoint->LineEdit1;
    globalSelection(); // close local contexts, if any
    localSelection(TopAbs_VERTEX);
  }
  else if (send == GroupOnCurve->PushButton1) {
    GroupOnCurve->LineEdit1->setFocus();
    myEditCurrentArgument = GroupOnCurve->LineEdit1;
    globalSelection(); // close local contexts, if any
    myNeedType = TopAbs_EDGE;
    localSelection(myNeedType);
    GroupOnCurve->PushButton2->setDown(false);
    GroupOnCurve->LineEdit1->setEnabled(true);
    GroupOnCurve->LineEdit2->setEnabled(false);
  }
  else if (send == GroupOnCurve->PushButton2) {
    GroupOnCurve->LineEdit2->setFocus();
    myEditCurrentArgument = GroupOnCurve->LineEdit2;
    globalSelection(); // close local contexts, if any
    myNeedType = TopAbs_VERTEX;
    localSelection(myNeedType);
    GroupOnCurve->PushButton1->setDown(false);
    GroupOnCurve->LineEdit2->setEnabled(true);
    GroupOnCurve->LineEdit1->setEnabled(false);
  }
  else if (send == GroupOnSurface->PushButton1)
  {
    GroupOnSurface->LineEdit1->setFocus();
    myEditCurrentArgument = GroupOnSurface->LineEdit1;
    globalSelection(); // close local contexts, if any
    localSelection(TopAbs_FACE);
  }
  else if (send == GroupLineIntersection->PushButton1) {
    GroupLineIntersection->LineEdit1->setFocus();
    myEditCurrentArgument = GroupLineIntersection->LineEdit1;
    GroupLineIntersection->PushButton2->setDown(false);
    GroupLineIntersection->LineEdit1->setEnabled(true);
    GroupLineIntersection->LineEdit2->setEnabled(false);
  }
  else if (send == GroupLineIntersection->PushButton2) {
    GroupLineIntersection->LineEdit2->setFocus();
    myEditCurrentArgument = GroupLineIntersection->LineEdit2;
    GroupLineIntersection->PushButton1->setDown(false);
    GroupLineIntersection->LineEdit1->setEnabled(false);
    GroupLineIntersection->LineEdit2->setEnabled(true);
  }
  send->setDown(true);

  //  if ((send == GroupLineIntersection->PushButton1 ||
  //         send == GroupLineIntersection->PushButton2) && !myBusy)
  //    SelectionIntoArgument();
}


//=================================================================================
// function : enterEvent()
// purpose  : to reactivate this dialog box when mouse enter onto the window
//=================================================================================
void BasicGUI_PointDlg::enterEvent(QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void BasicGUI_PointDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();

  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
           this, SLOT(SelectionIntoArgument()));

  ConstructorsClicked(getConstructorId());
}


//=================================================================================
// function : DeactivateActiveDialog()
// purpose  : public slot to deactivate if active
//=================================================================================
void BasicGUI_PointDlg::DeactivateActiveDialog()
{
  // myGeomGUI->SetState(-1);
  GEOMBase_Skeleton::DeactivateActiveDialog();
}


//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void BasicGUI_PointDlg::ValueChangedInSpinBox(double newValue)
{
  displayPreview(true);
}

//=================================================================================
// function : CheckBoxClicked()
// purpose  : Check Boxes Management
//=================================================================================
void BasicGUI_PointDlg::CheckBoxClicked()
{
  displayPreview(true);
}


//=================================================================================
// funcion  : getParameter()
// purpose  :
//=================================================================================
double BasicGUI_PointDlg::getParameter() const
{
  return GroupOnCurve->SpinBox_DX->value();
}


//=================================================================================
// funcion  : getUParameter()
// purpose  :
//=================================================================================
double BasicGUI_PointDlg::getUParameter() const
{
  return GroupOnSurface->SpinBox_DX->value();
}


//=================================================================================
// funcion  : getVParameter()
// purpose  :
//=================================================================================
double BasicGUI_PointDlg::getVParameter() const
{
  return GroupOnSurface->SpinBox_DY->value();
}


//=================================================================================
// function : OnPointSelected
// purpose  :
//=================================================================================
void BasicGUI_PointDlg::OnPointSelected(const gp_Pnt& thePnt)
{
  if (getConstructorId() == GEOM_POINT_XYZ) {
    GroupXYZ->SpinBox_DX->setValue(thePnt.X());
    GroupXYZ->SpinBox_DY->setValue(thePnt.Y());
    GroupXYZ->SpinBox_DZ->setValue(thePnt.Z());

    displayPreview(true);
  }
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr BasicGUI_PointDlg::createOperation()
{
  return myGeomGUI->GetGeomGen()->GetIBasicOperations(getStudyId());
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool BasicGUI_PointDlg::isValid(QString& msg)
{
  bool ok = false;
  switch (getConstructorId()) {
  case GEOM_POINT_XYZ :
    ok = GroupXYZ->SpinBox_DX->isValid(msg, !IsPreview()) &&
         GroupXYZ->SpinBox_DY->isValid(msg, !IsPreview()) &&
         GroupXYZ->SpinBox_DZ->isValid(msg, !IsPreview());
    break;
  case GEOM_POINT_REF:
    ok = GroupRefPoint->SpinBox_DX->isValid(msg, !IsPreview()) &&
         GroupRefPoint->SpinBox_DY->isValid(msg, !IsPreview()) &&
         GroupRefPoint->SpinBox_DZ->isValid(msg, !IsPreview()) &&
         myRefPoint;
    break;
  case GEOM_POINT_EDGE:
    if (myParamCoord->checkedId() == PARAM_VALUE || myParamCoord->checkedId() == LENGTH_VALUE)
      ok = GroupOnCurve->SpinBox_DX->isValid(msg, !IsPreview());
    else
      ok = GroupXYZ->SpinBox_DX->isValid(msg, !IsPreview()) &&
           GroupXYZ->SpinBox_DY->isValid(msg, !IsPreview()) &&
           GroupXYZ->SpinBox_DZ->isValid(msg, !IsPreview());
    ok = myEdge && ok;
    break;
  case GEOM_POINT_INTINT:
    ok = myLine1 && myLine2;
    break;
  case GEOM_POINT_SURF:
    if (myParamCoord->checkedId() == PARAM_VALUE)
      ok = GroupOnSurface->SpinBox_DX->isValid(msg, !IsPreview()) &&
           GroupOnSurface->SpinBox_DY->isValid(msg, !IsPreview());
    else
      ok = GroupXYZ->SpinBox_DX->isValid(msg, !IsPreview()) &&
           GroupXYZ->SpinBox_DY->isValid(msg, !IsPreview()) &&
           GroupXYZ->SpinBox_DZ->isValid(msg, !IsPreview());
    ok = myFace && ok;
    break;
  default:
    break;
  }
  return ok;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool BasicGUI_PointDlg::execute(ObjectList& objects)
{
  bool res = false;

  GEOM::GEOM_Object_var anObj;
  QStringList aParameters;

  GEOM::GEOM_IBasicOperations_var anOper = GEOM::GEOM_IBasicOperations::_narrow(getOperation());

  switch (getConstructorId()) {
  case GEOM_POINT_XYZ :
    {
      double x = GroupXYZ->SpinBox_DX->value();
      double y = GroupXYZ->SpinBox_DY->value();
      double z = GroupXYZ->SpinBox_DZ->value();

      aParameters << GroupXYZ->SpinBox_DX->text();
      aParameters << GroupXYZ->SpinBox_DY->text();
      aParameters << GroupXYZ->SpinBox_DZ->text();

      anObj = anOper->MakePointXYZ(x, y, z);
      res = true;
      break;
    }
  case GEOM_POINT_REF :
    {
      double dx = GroupRefPoint->SpinBox_DX->value();
      double dy = GroupRefPoint->SpinBox_DY->value();
      double dz = GroupRefPoint->SpinBox_DZ->value();

      aParameters << GroupRefPoint->SpinBox_DX->text();
      aParameters << GroupRefPoint->SpinBox_DY->text();
      aParameters << GroupRefPoint->SpinBox_DZ->text();

      anObj = anOper->MakePointWithReference(myRefPoint.get(), dx, dy, dz);
      res = true;
      break;
    }
  case GEOM_POINT_EDGE :
    {
      if (myParamCoord->checkedId() == PARAM_VALUE) {
        bool isUseOrientation = GroupOnCurve->CheckButton1->isChecked();

        anObj = anOper->MakePointOnCurve(myEdge.get(), getParameter(), isUseOrientation);
        aParameters<<GroupOnCurve->SpinBox_DX->text();
      }
      else if (myParamCoord->checkedId() == LENGTH_VALUE) {
        anObj = anOper->MakePointOnCurveByLength(myEdge.get(), getParameter(), myRefPoint.get());
        aParameters<<GroupOnCurve->SpinBox_DX->text();
      }
      else if (myParamCoord->checkedId() == COORD_VALUE) {
        double x = GroupXYZ->SpinBox_DX->value();
        double y = GroupXYZ->SpinBox_DY->value();
        double z = GroupXYZ->SpinBox_DZ->value();

        aParameters << GroupXYZ->SpinBox_DX->text();
        aParameters << GroupXYZ->SpinBox_DY->text();
        aParameters << GroupXYZ->SpinBox_DZ->text();

        anObj = anOper->MakePointOnCurveByCoord(myEdge.get(), x, y, z);
      }
      res = true;
      break;
    }
  case GEOM_POINT_INTINT :
    anObj = anOper->MakePointOnLinesIntersection(myLine1.get(), myLine2.get());
    if (!anObj->_is_nil()) {
      QString aName = getNewObjectName();
      if (anObj->GetShapeType() == GEOM::COMPOUND && aName.startsWith("Vertex"))
        initName(tr("GEOM_COMPOUND"));
      else if (anObj->GetShapeType() == GEOM::VERTEX && aName.startsWith("Compound"))
        initName(tr("GEOM_VERTEX"));
    }
    res = true;
    break;
  case GEOM_POINT_SURF :
    {
      if (myParamCoord->checkedId() == PARAM_VALUE) {
        anObj = anOper->MakePointOnSurface(myFace.get(), getUParameter(), getVParameter());
        aParameters<<GroupOnSurface->SpinBox_DX->text();
        aParameters<<GroupOnSurface->SpinBox_DY->text();
      } else {
        double x = GroupXYZ->SpinBox_DX->value();
        double y = GroupXYZ->SpinBox_DY->value();
        double z = GroupXYZ->SpinBox_DZ->value();

        aParameters << GroupXYZ->SpinBox_DX->text();
        aParameters << GroupXYZ->SpinBox_DY->text();
        aParameters << GroupXYZ->SpinBox_DZ->text();

        anObj = anOper->MakePointOnSurfaceByCoord(myFace.get(), x, y, z);
      }
      res = true;
      break;
    }
  }

  const int id = getConstructorId();
  if(!anObj->_is_nil() && !IsPreview() && (id == GEOM_POINT_XYZ ||
                                           id == GEOM_POINT_REF ||
                                           id == GEOM_POINT_EDGE ||
                                           id == GEOM_POINT_SURF)) {
      anObj->SetParameters(aParameters.join(":").toLatin1().constData());
  }

  if (id == GEOM_POINT_REF || id == GEOM_POINT_EDGE || id == GEOM_POINT_SURF) {
    TopoDS_Shape aShape;
    if (GEOMBase::GetShape(anObj, aShape) && !aShape.IsNull() &&
         aShape.ShapeType() == TopAbs_VERTEX) {
      gp_Pnt aPnt = BRep_Tool::Pnt(TopoDS::Vertex(aShape));
      SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
      int aPrecision = resMgr->integerValue("Geometry", "length_precision", 6);
      myX->setText(DlgRef::PrintDoubleValue(aPnt.X(), aPrecision));
      myY->setText(DlgRef::PrintDoubleValue(aPnt.Y(), aPrecision));
      myZ->setText(DlgRef::PrintDoubleValue(aPnt.Z(), aPrecision));
    }
    else {
      myX->setText("");
      myY->setText("");
      myZ->setText("");
    }
  }

  if (!anObj->_is_nil()) {
    objects.push_back(anObj._retn());
  }

  return res;
}

//=================================================================================
// function : addSubshapeToStudy
// purpose  : virtual method to add new SubObjects if local selection
//=================================================================================
void BasicGUI_PointDlg::addSubshapesToStudy()
{
  switch (getConstructorId()) {
  case GEOM_POINT_REF:
    GEOMBase::PublishSubObject(myRefPoint.get());
    break;
  case GEOM_POINT_EDGE:
    GEOMBase::PublishSubObject(myEdge.get());
    break;
  case GEOM_POINT_INTINT:
    GEOMBase::PublishSubObject(myLine1.get());
    GEOMBase::PublishSubObject(myLine2.get());
    break;
  case GEOM_POINT_SURF:
    GEOMBase::PublishSubObject(myFace.get());
    break;
  default:
    break;
  }
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> BasicGUI_PointDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res;
  res << myRefPoint << myEdge << myLine1 << myLine2 << myFace;
  return res;
}

//=================================================================================
// function : ClickParamCoord()
// purpose  :
//=================================================================================
void BasicGUI_PointDlg::ClickParamCoord(int id)
{
  updateParamCoord(true);
  displayPreview(true);
}

//=================================================================================
// function : updateParamCoord
// purpose  :
//=================================================================================
void BasicGUI_PointDlg::updateParamCoord(bool theIsUpdate)
{
  bool isParam = myParamCoord->checkedId() == PARAM_VALUE;
  bool isLength = myParamCoord->checkedId() == LENGTH_VALUE;

  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  double step = resMgr->doubleValue("Geometry", "SettingsGeomStep", 100);

  const int id = getConstructorId();
  if (id == GEOM_POINT_EDGE) {
    GroupOnCurve->TextLabel2->setVisible(isLength);
    GroupOnCurve->PushButton2->setVisible(isLength);
    GroupOnCurve->LineEdit2->setVisible(isLength);
    GroupOnCurve->TextLabel3->setVisible(isParam || isLength);
    GroupOnCurve->SpinBox_DX->setVisible(isParam || isLength);
    GroupOnCurve->CheckButton1->setVisible(isParam);
    if (isParam){
      initSpinBox(GroupOnCurve->SpinBox_DX, 0., 1., 0.1, "parametric_precision");
      GroupOnCurve->SpinBox_DX->setValue(0.5);
      GroupOnCurve->TextLabel3->setText(tr("GEOM_PARAMETER"));
    }
    else if (isLength){
      initSpinBox(GroupOnCurve->SpinBox_DX, COORD_MIN, COORD_MAX, step, "length_precision");
      GroupOnCurve->SpinBox_DX->setValue(0.0);
      GroupOnCurve->TextLabel3->setText(tr("GEOM_LENGTH"));
    }
  }
  else if (id == GEOM_POINT_SURF) {
    GroupOnSurface->TextLabel2->setVisible(isParam);
    GroupOnSurface->TextLabel3->setVisible(isParam);
    GroupOnSurface->SpinBox_DX->setVisible(isParam);
    GroupOnSurface->SpinBox_DY->setVisible(isParam);
  }

  GroupXYZ->setVisible(!isParam && !isLength);

  if (theIsUpdate)
    QTimer::singleShot(50, this, SLOT(updateSize()));
}

//=================================================================================
// function : updateSize
// purpose  : adjust dialog size to minimum
//=================================================================================
void BasicGUI_PointDlg::updateSize()
{
  qApp->processEvents();
  updateGeometry();
  resize(minimumSizeHint());
}
