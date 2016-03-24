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
// File   : BuildGUI_EdgeDlg.cxx
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.

#include "BuildGUI_EdgeDlg.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>

#include <GEOMImpl_Types.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>

//=================================================================================
// class    : BuildGUI_EdgeDlg()
// purpose  : Constructs a BuildGUI_EdgeDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
BuildGUI_EdgeDlg::BuildGUI_EdgeDlg (GeometryGUI* theGeometryGUI, QWidget* parent)
  : GEOMBase_Skeleton(theGeometryGUI, parent)
{
  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
  QPixmap image0 (aResMgr->loadPixmap("GEOM", tr("ICON_SELECT")));
  QPixmap image1 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_BUILD_EDGE")));
  QPixmap image2 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_BUILD_EDGE_WIRE")));
  QPixmap image3 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_BUILD_EDGE_CURVE")));

  setWindowTitle(tr("GEOM_EDGE_TITLE"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_EDGE"));
  mainFrame()->RadioButton1->setIcon(image1);
  mainFrame()->RadioButton2->setIcon(image2);
  mainFrame()->RadioButton3->setIcon(image3);

  // two points

  GroupPoints = new DlgRef_2Sel(centralWidget());
  GroupPoints->GroupBox1->setTitle(tr("GEOM_POINTS"));
  GroupPoints->TextLabel1->setText(tr("GEOM_POINT_I").arg(1));
  GroupPoints->TextLabel2->setText(tr("GEOM_POINT_I").arg(2));
  GroupPoints->PushButton1->setIcon(image0);
  GroupPoints->PushButton2->setIcon(image0);
  GroupPoints->LineEdit1->setReadOnly(true);
  GroupPoints->LineEdit2->setReadOnly(true);

  // wire

  GroupWire = new DlgRef_1Sel2Spin(centralWidget());
  GroupWire->GroupBox1->setTitle(tr("GEOM_WIRE"));
  GroupWire->TextLabel1->setText(tr("GEOM_WIRE"));
  GroupWire->PushButton1->setIcon(image0);
  GroupWire->LineEdit1->setReadOnly(true);
  GroupWire->TextLabel2->setText(tr("GEOM_LINEAR_TOLERANCE"));
  GroupWire->TextLabel3->setText(tr("GEOM_ANGULAR_TOLERANCE"));
  double SpecificStep = 0.0001;
  double prec1 = Precision::Confusion();
  double prec2 = Precision::Angular();
  initSpinBox(GroupWire->SpinBox_DX, prec1, MAX_NUMBER, SpecificStep, "len_tol_precision");
  initSpinBox(GroupWire->SpinBox_DY, prec2, MAX_NUMBER, SpecificStep, "ang_tol_precision");
  GroupWire->SpinBox_DX->setValue(prec1);
  GroupWire->SpinBox_DY->setValue(prec2);

  // on curve

  GroupOnCurve = new DlgRef_2Sel1Spin(centralWidget());
  GroupOnCurve->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  GroupOnCurve->TextLabel1->setText(tr("GEOM_EDGE"));
  GroupOnCurve->PushButton1->setIcon(image0);
  GroupOnCurve->LineEdit1->setReadOnly(true);
  GroupOnCurve->TextLabel2->setText(tr("GEOM_START_POINT"));
  GroupOnCurve->PushButton2->setIcon(image0);
  GroupOnCurve->LineEdit2->setReadOnly(true);
  GroupOnCurve->TextLabel3->setText(tr("GEOM_LENGTH"));
  double step = aResMgr->doubleValue("Geometry", "SettingsGeomStep", 100.0);
  initSpinBox(GroupOnCurve->SpinBox_DX, COORD_MIN, COORD_MAX, step, "length_precision");
  GroupOnCurve->SpinBox_DX->setValue(step);

  // layout

  QVBoxLayout* layout = new QVBoxLayout(centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
  layout->addWidget(GroupPoints);
  layout->addWidget(GroupWire);
  layout->addWidget(GroupOnCurve);
  /***************************************************************/

  setHelpFileName("create_edge_page.html");

  // initialisation
  Init();
}

//=================================================================================
// function : ~BuildGUI_EdgeDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
BuildGUI_EdgeDlg::~BuildGUI_EdgeDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BuildGUI_EdgeDlg::Init()
{
  // init variables
  myPoint1.nullify();
  myPoint2.nullify();
  myWire.nullify();
  myCurve.nullify();
  myStartPoint.nullify();

  myEditCurrentArgument = GroupPoints->LineEdit1;
  GroupPoints->PushButton1->setDown(true);
  globalSelection(); // close local contexts, if any
  localSelection(TopAbs_VERTEX);

  // signals and slots connections
  connect(myGeomGUI, SIGNAL(SignalDeactivateActiveDialog()), this, SLOT(DeactivateActiveDialog()));
  connect(myGeomGUI, SIGNAL(SignalCloseAllDialogs()),        this, SLOT(ClickOnCancel()));

  connect(this,      SIGNAL(constructorsClicked(int)), this, SLOT(ConstructorsClicked(int)));

  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(GroupPoints->PushButton1,  SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->PushButton2,  SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupWire->PushButton1,    SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupOnCurve->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupOnCurve->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(GroupOnCurve->SpinBox_DX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));

  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
           this, SLOT(SelectionIntoArgument()));

  initName(tr("GEOM_EDGE"));

  ConstructorsClicked(0);
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void BuildGUI_EdgeDlg::ClickOnOk()
{
  setIsApplyAndClose( true );
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool BuildGUI_EdgeDlg::ClickOnApply()
{
  if (!onAccept())
    return false;

  initName();

  myEditCurrentArgument->setText("");
  ConstructorsClicked(getConstructorId());

  return true;
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void BuildGUI_EdgeDlg::ConstructorsClicked(int constructorId)
{
  switch (constructorId) {
  case 0:
    {
      globalSelection(); // close local contexts, if any
      localSelection(TopAbs_VERTEX);

      myEditCurrentArgument = GroupPoints->LineEdit1;
      GroupPoints->LineEdit1->setText("");
      GroupPoints->LineEdit2->setText("");
      myPoint1.nullify();
      myPoint2.nullify();
      GroupPoints->PushButton1->setDown(true);
      GroupPoints->PushButton2->setDown(false);
      GroupPoints->LineEdit1->setEnabled(true);
      GroupPoints->LineEdit2->setEnabled(false);
      GroupPoints->show();
      GroupWire->hide();
      GroupOnCurve->hide();
      break;
    }
  case 1:
    {
      globalSelection(); // close local contexts, if any
      localSelection(TopAbs_WIRE);

      myEditCurrentArgument = GroupWire->LineEdit1;
      GroupWire->LineEdit1->setText("");
      myWire.nullify();
      GroupWire->PushButton1->setDown(true);
      GroupWire->LineEdit1->setEnabled(true);
      GroupPoints->hide();
      GroupOnCurve->hide();
      GroupWire->show();
      break;
    }
  case 2:
    {
      globalSelection(); // close local contexts, if any
      localSelection(TopAbs_EDGE);

      myEditCurrentArgument = GroupOnCurve->LineEdit1;
      GroupOnCurve->LineEdit1->setText("");
      GroupOnCurve->LineEdit2->setText("");
      myCurve.nullify();
      myStartPoint.nullify();
      GroupOnCurve->PushButton1->setDown(true);
      GroupOnCurve->PushButton2->setDown(false);
      GroupOnCurve->LineEdit1->setEnabled(true);
      GroupOnCurve->LineEdit2->setEnabled(false);
      GroupPoints->hide();
      GroupWire->hide();
      GroupOnCurve->show();
      break;
    }
  }

  qApp->processEvents();
  updateGeometry();
  resize(minimumSizeHint());
  SelectionIntoArgument();
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection is changed or on dialog initialization or activation
//=================================================================================
void BuildGUI_EdgeDlg::SelectionIntoArgument()
{
  erasePreview();
  myEditCurrentArgument->setText("");

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  if (aSelList.Extent() != 1) {
    if      (myEditCurrentArgument == GroupPoints->LineEdit1)  myPoint1.nullify();
    else if (myEditCurrentArgument == GroupPoints->LineEdit2)  myPoint2.nullify();
    else if (myEditCurrentArgument == GroupWire->LineEdit1)    myWire.nullify();
    else if (myEditCurrentArgument == GroupOnCurve->LineEdit1) myCurve.nullify();
    else if (myEditCurrentArgument == GroupOnCurve->LineEdit2) myStartPoint.nullify();
    displayPreview(true);
    return;
  }

  TopAbs_ShapeEnum aNeedType = TopAbs_VERTEX;
  if (myEditCurrentArgument == GroupWire->LineEdit1) aNeedType = TopAbs_WIRE;
  else if (myEditCurrentArgument == GroupOnCurve->LineEdit1) aNeedType = TopAbs_EDGE;
  GEOM::GeomObjPtr aSelectedObject = getSelected(aNeedType);

  TopoDS_Shape aShape;
  if (aSelectedObject && GEOMBase::GetShape(aSelectedObject.get(), aShape) && !aShape.IsNull()) {
    QString aName = GEOMBase::GetName(aSelectedObject.get());
    myEditCurrentArgument->setText(aName);

    globalSelection();
    localSelection(aNeedType);

    if (myEditCurrentArgument == GroupPoints->LineEdit1) {
      myPoint1 = aSelectedObject;
      if (myPoint1 && !myPoint2)
        GroupPoints->PushButton2->click();
    }
    else if (myEditCurrentArgument == GroupPoints->LineEdit2) {
      myPoint2 = aSelectedObject;
      if (myPoint2 && !myPoint1)
        GroupPoints->PushButton1->click();
    }
    else if (myEditCurrentArgument == GroupWire->LineEdit1) {
      myWire = aSelectedObject;
    }
    else if (myEditCurrentArgument == GroupOnCurve->LineEdit1) {
      myCurve = aSelectedObject;
      if (myCurve && !myStartPoint)
        GroupOnCurve->PushButton2->click();
    }
    else if (myEditCurrentArgument == GroupOnCurve->LineEdit2) {
      myStartPoint = aSelectedObject;
      if (myStartPoint && !myCurve)
        GroupOnCurve->PushButton1->click();
    }
  }

  displayPreview(true);
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BuildGUI_EdgeDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  if (send == GroupPoints->PushButton1) {
    myEditCurrentArgument = GroupPoints->LineEdit1;
    GroupPoints->PushButton2->setDown(false);
    GroupPoints->LineEdit2->setEnabled(false);
  }
  else if (send == GroupPoints->PushButton2) {
    myEditCurrentArgument = GroupPoints->LineEdit2;
    GroupPoints->PushButton1->setDown(false);
    GroupPoints->LineEdit1->setEnabled(false);
  }
  else if (send == GroupWire->PushButton1) {
    myEditCurrentArgument = GroupWire->LineEdit1;
  }
  else if (send == GroupOnCurve->PushButton1) {
    myEditCurrentArgument = GroupOnCurve->LineEdit1;
    GroupOnCurve->PushButton2->setDown(false);
    GroupOnCurve->LineEdit2->setEnabled(false);

    globalSelection(); // close local contexts, if any
    localSelection(TopAbs_EDGE);
  }
  else if (send == GroupOnCurve->PushButton2) {
    myEditCurrentArgument = GroupOnCurve->LineEdit2;
    GroupOnCurve->PushButton1->setDown(false);
    GroupOnCurve->LineEdit1->setEnabled(false);

    globalSelection(); // close local contexts, if any
    localSelection(TopAbs_VERTEX);
  }

  // enable line edit
  myEditCurrentArgument->setEnabled(true);
  myEditCurrentArgument->setFocus();
  send->setDown(true);
  displayPreview(true);
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void BuildGUI_EdgeDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();

  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));

  ConstructorsClicked(getConstructorId());
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void BuildGUI_EdgeDlg::enterEvent (QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void BuildGUI_EdgeDlg::ValueChangedInSpinBox(double newValue)
{
  displayPreview(true);
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr BuildGUI_EdgeDlg::createOperation()
{
  return getGeomEngine()->GetIShapesOperations(getStudyId());
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool BuildGUI_EdgeDlg::isValid (QString& msg)
{
  bool ok = false;
  switch (getConstructorId()) {
  case 0:
    ok = myPoint1 && myPoint2;
    break;
  case 1:
    ok = GroupWire->SpinBox_DX->isValid(msg, !IsPreview()) &&
         GroupWire->SpinBox_DY->isValid(msg, !IsPreview()) &&
         myWire;
    break;
  case 2:
    ok = GroupOnCurve->SpinBox_DX->isValid(msg, !IsPreview()) &&
         myCurve;
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
bool BuildGUI_EdgeDlg::execute (ObjectList& objects)
{
  bool res = false;
  GEOM::GEOM_Object_var anObj;

  GEOM::GEOM_IShapesOperations_var anOper = GEOM::GEOM_IShapesOperations::_narrow(getOperation());

  switch (getConstructorId()) {
  case 0:
    {
      anObj = anOper->MakeEdge(myPoint1.get(), myPoint2.get());
      res = true;
      break;
    }
  case 1:
    {
      double aLinearTolerance   = GroupWire->SpinBox_DX->value();
      double anAngularTolerance = GroupWire->SpinBox_DY->value();

      QStringList aParameters;
      aParameters << GroupWire->SpinBox_DX->text();
      aParameters << GroupWire->SpinBox_DY->text();

      anObj = anOper->MakeEdgeWire(myWire.get(), aLinearTolerance, anAngularTolerance);

      if (!anObj->_is_nil() && !IsPreview())
        anObj->SetParameters(aParameters.join(":").toLatin1().constData());

      res = true;
      break;
    }
  case 2:
    {
      double aLength = GroupOnCurve->SpinBox_DX->value();

      anObj = anOper->MakeEdgeOnCurveByLength(myCurve.get(), aLength, myStartPoint.get());

      if (!anObj->_is_nil() && !IsPreview())
        anObj->SetParameters(GroupOnCurve->SpinBox_DX->text().toLatin1().constData());

      res = true;
      break;
    }
  }

  if (!anObj->_is_nil()) objects.push_back(anObj._retn());

  return res;
}

//=================================================================================
// function : addSubshapeToStudy
// purpose  : virtual method to add new SubObjects if local selection
//=================================================================================
void BuildGUI_EdgeDlg::addSubshapesToStudy()
{
  switch (getConstructorId()) {
  case 0:
    GEOMBase::PublishSubObject(myPoint1.get());
    GEOMBase::PublishSubObject(myPoint2.get());
    break;
  case 1:
    GEOMBase::PublishSubObject(myWire.get());
    break;
  case 2:
    GEOMBase::PublishSubObject(myCurve.get());
    GEOMBase::PublishSubObject(myStartPoint.get());
    break;
  default:
    break;
  }
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> BuildGUI_EdgeDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res;
  res << myPoint1 << myPoint2 << myWire << myCurve << myStartPoint;
  return res;
}
