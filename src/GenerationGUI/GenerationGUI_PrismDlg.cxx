//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
// GEOM GEOMGUI : GUI for Geometry component
// File   : GenerationGUI_PrismDlg.cxx
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.
//
#include "GenerationGUI_PrismDlg.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>

// OCCT Includes
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#include <GEOMImpl_Types.hxx>

//=================================================================================
// class    : GenerationGUI_PrismDlg()
// purpose  : Constructs a GenerationGUI_PrismDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
GenerationGUI_PrismDlg::GenerationGUI_PrismDlg (GeometryGUI* theGeometryGUI, QWidget* parent,
                                                bool modal, Qt::WindowFlags fl)
  : GEOMBase_Skeleton(theGeometryGUI, parent, modal, fl),
    myInitial(true)
{
  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
  QPixmap image0 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_PRISM")));
  QPixmap image1 (aResMgr->loadPixmap("GEOM", tr("ICON_SELECT")));
  QPixmap image2 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_PRISM_2P")));
  QPixmap image3 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_PRISM_DXDYDZ")));

  setWindowTitle(tr("GEOM_EXTRUSION_TITLE"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_EXTRUSION"));
  mainFrame()->RadioButton1->setIcon(image0);
  mainFrame()->RadioButton2->setIcon(image2);
  mainFrame()->RadioButton3->setIcon(image3);

  mainFrame()->RadioButton1->setChecked(true);
  myBothway = myBothway2 = myBothway3 = false;

  GroupPoints = new DlgRef_2Sel1Spin2Check(centralWidget());
  GroupPoints->GroupBox1->setTitle(tr("GEOM_EXTRUSION_BSV"));
  GroupPoints->TextLabel1->setText(tr("GEOM_BASE"));
  GroupPoints->TextLabel2->setText(tr("GEOM_VECTOR"));
  GroupPoints->TextLabel3->setText(tr("GEOM_HEIGHT"));
  GroupPoints->PushButton1->setIcon(image1);
  GroupPoints->PushButton2->setIcon(image1);
  GroupPoints->LineEdit1->setReadOnly(true);
  GroupPoints->LineEdit2->setReadOnly(true);
  GroupPoints->CheckButton1->setText(tr("GEOM_BOTHWAY"));
  GroupPoints->CheckButton1->setChecked(myBothway);
  GroupPoints->CheckButton2->setText(tr("GEOM_REVERSE"));

  GroupPoints2 = new DlgRef_3Sel1Check(centralWidget());
  GroupPoints2->GroupBox1->setTitle(tr("GEOM_EXTRUSION_BSV_2P"));
  GroupPoints2->TextLabel1->setText(tr("GEOM_BASE"));
  GroupPoints2->TextLabel2->setText(tr("GEOM_POINT_I").arg(1));
  GroupPoints2->TextLabel3->setText(tr("GEOM_POINT_I").arg(2));
  GroupPoints2->PushButton1->setIcon(image1);
  GroupPoints2->PushButton2->setIcon(image1);
  GroupPoints2->PushButton3->setIcon(image1);
  GroupPoints2->CheckButton1->setText(tr("GEOM_BOTHWAY"));
  GroupPoints2->CheckButton1->setChecked(myBothway2);

  GroupPoints3 = new DlgRef_1Sel3Spin1Check(centralWidget());
  GroupPoints3->GroupBox1->setTitle(tr("GEOM_EXTRUSION_DXDYDZ"));
  GroupPoints3->TextLabel1->setText(tr("GEOM_BASE"));
  GroupPoints3->PushButton1->setIcon(image1);
  GroupPoints3->TextLabel2->setText(tr("GEOM_DX"));
  GroupPoints3->TextLabel3->setText(tr("GEOM_DY"));
  GroupPoints3->TextLabel4->setText(tr("GEOM_DZ"));
  GroupPoints3->CheckButton1->setText(tr("GEOM_BOTHWAY"));
  GroupPoints3->CheckButton1->setChecked(myBothway3);

  QVBoxLayout* layout = new QVBoxLayout(centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
  layout->addWidget(GroupPoints);
  layout->addWidget(GroupPoints2);
  layout->addWidget(GroupPoints3);
  /***************************************************************/

  setHelpFileName("create_extrusion_page.html");

  Init();
}

//=================================================================================
// function : ~GenerationGUI_PrismDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
GenerationGUI_PrismDlg::~GenerationGUI_PrismDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void GenerationGUI_PrismDlg::Init()
{
  // Get setting of step value from file configuration
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  double step = resMgr->doubleValue("Geometry", "SettingsGeomStep", 100);

  // min, max, step and decimals for spin boxes & initial values
  initSpinBox(GroupPoints3->SpinBox_DX, COORD_MIN, COORD_MAX, step, 6); // VSR:TODO : DBL_DIGITS_DISPLAY
  initSpinBox(GroupPoints3->SpinBox_DY, COORD_MIN, COORD_MAX, step, 6); // VSR:TODO : DBL_DIGITS_DISPLAY
  initSpinBox(GroupPoints3->SpinBox_DZ, COORD_MIN, COORD_MAX, step, 6); // VSR:TODO : DBL_DIGITS_DISPLAY
  GroupPoints3->SpinBox_DX->setValue(0.0);
  GroupPoints3->SpinBox_DY->setValue(0.0);
  GroupPoints3->SpinBox_DZ->setValue(0.0);

  initSpinBox(GroupPoints->SpinBox_DX, 0.000001, COORD_MAX, step, 6); // VSR: TODO: DBL_DIGITS_DISPLAY
  GroupPoints->SpinBox_DX->setValue(100.0);

  // init variables
  myEditCurrentArgument = GroupPoints->LineEdit1;
  GroupPoints->LineEdit1->setReadOnly(true);
  GroupPoints->LineEdit2->setReadOnly(true);

  GroupPoints2->LineEdit1->setReadOnly(true);
  GroupPoints2->LineEdit2->setReadOnly(true);
  GroupPoints2->LineEdit3->setReadOnly(true);

  GroupPoints3->LineEdit1->setReadOnly(true);

  GroupPoints->LineEdit1->setText("");
  GroupPoints->LineEdit2->setText("");

  GroupPoints2->LineEdit1->setText("");
  GroupPoints2->LineEdit2->setText("");
  GroupPoints2->LineEdit3->setText("");

  GroupPoints3->LineEdit1->setText("");

  myPoint1 = myPoint2 = myBase = myVec = GEOM::GEOM_Object::_nil();
  myOkBase = myOkVec = myOkPnt1 = myOkPnt2 = false;

  // signals and slots connections
  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(this,          SIGNAL(constructorsClicked(int)), this, SLOT(ConstructorsClicked(int)));

  connect(GroupPoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(GroupPoints->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupPoints->LineEdit2, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(GroupPoints->SpinBox_DX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox()));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), this, SLOT(SetDoubleSpinBoxStep(double)));

  connect(GroupPoints->CheckButton1,  SIGNAL(toggled(bool)), this, SLOT(onBothway()));
  connect(GroupPoints->CheckButton2,  SIGNAL(toggled(bool)), this, SLOT(onReverse()));

  connect(GroupPoints2->PushButton1,  SIGNAL(clicked()),       this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints2->PushButton2,  SIGNAL(clicked()),       this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints2->PushButton3,  SIGNAL(clicked()),       this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints2->CheckButton1, SIGNAL(toggled(bool)), this, SLOT(onBothway()));

  connect(GroupPoints2->LineEdit1,    SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupPoints2->LineEdit2,    SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupPoints2->LineEdit3,    SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(GroupPoints3->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints3->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupPoints3->SpinBox_DX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox()));
  connect(GroupPoints3->SpinBox_DY, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox()));
  connect(GroupPoints3->SpinBox_DZ, SIGNAL(valueChanged (double)), this, SLOT(ValueChangedInSpinBox()));
  connect(GroupPoints3->CheckButton1, SIGNAL(toggled(bool)), this, SLOT(onBothway()));

  initName(tr("GEOM_EXTRUSION"));

  ConstructorsClicked(0);
}

//=================================================================================
// function : SetDoubleSpinBoxStep()
// purpose  : Double spin box management
//=================================================================================
void GenerationGUI_PrismDlg::SetDoubleSpinBoxStep (double step)
{
  GroupPoints->SpinBox_DX->setSingleStep(step);
  GroupPoints3->SpinBox_DX->setSingleStep(step);
  GroupPoints3->SpinBox_DY->setSingleStep(step);
  GroupPoints3->SpinBox_DZ->setSingleStep(step);
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void GenerationGUI_PrismDlg::ConstructorsClicked (int constructorId)
{
  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);

  switch (constructorId) {
  case 0:
    {
      GroupPoints2->hide();
      GroupPoints3->hide();
      GroupPoints->show();

      GroupPoints->LineEdit1->setText(GroupPoints2->LineEdit1->text()); // keep base
      GroupPoints->LineEdit2->setText("");
      myVec = GEOM::GEOM_Object::_nil();
      myOkVec = false;

      GroupPoints->PushButton1->click();
      break;
    }
  case 1:
    {
      GroupPoints->hide();
      GroupPoints2->show();
      GroupPoints3->hide();

      GroupPoints2->LineEdit1->setText(GroupPoints->LineEdit1->text()); // keep base
      GroupPoints2->LineEdit2->setText("");
      GroupPoints2->LineEdit3->setText("");
      myPoint1 = myPoint2 = GEOM::GEOM_Object::_nil();
      myOkPnt1 = myOkPnt2 = false;

      GroupPoints2->PushButton1->click();
      break;
    }
  case 2:
    {
      GroupPoints->hide();
      GroupPoints2->hide();
      GroupPoints3->show();

      GroupPoints3->LineEdit1->setText(GroupPoints->LineEdit1->text()); // keep base

      GroupPoints3->PushButton1->click();
      break;
    }
  }

  qApp->processEvents();
  updateGeometry();
  resize(minimumSizeHint());

  if (myInitial) {
    myInitial = false;
    SelectionIntoArgument();
  }
  else {
    displayPreview();
  }
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void GenerationGUI_PrismDlg::ClickOnOk()
{
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool GenerationGUI_PrismDlg::ClickOnApply()
{
  if (!onAccept())
    return false;

  initName();
  // activate selection and connect selection manager
  ConstructorsClicked(getConstructorId());
  return true;
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection is changed or on dialog initialization or activation
//=================================================================================
void GenerationGUI_PrismDlg::SelectionIntoArgument()
{
  erasePreview();
  myEditCurrentArgument->setText("");

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  if (getConstructorId() == 0)
  {
    if (aSelList.Extent() != 1) {
      if (myEditCurrentArgument == GroupPoints->LineEdit1)
        myOkBase = false;
      else if (myEditCurrentArgument == GroupPoints->LineEdit2)
        myOkVec = false;
      return;
    }

    // nbSel == 1
    Standard_Boolean testResult = Standard_False;
    GEOM::GEOM_Object_var aSelectedObject =
      GEOMBase::ConvertIOinGEOMObject(aSelList.First(), testResult);
    if (!testResult || CORBA::is_nil(aSelectedObject))
      return;

    QString aName = GEOMBase::GetName(aSelectedObject);

    // Get Selected object if selected subshape
    bool myOk = true;
    TopoDS_Shape aShape;
    if (GEOMBase::GetShape(aSelectedObject, aShape, TopAbs_SHAPE) && !aShape.IsNull())
    {
      TColStd_IndexedMapOfInteger aMap;
      aSelMgr->GetIndexes(aSelList.First(), aMap);
      if (aMap.Extent() == 1) {
        int anIndex = aMap(1);
        aName.append(":edge_" + QString::number(anIndex));

        //Find SubShape Object in Father
        GEOM::GEOM_Object_var aFindedObject = GEOMBase_Helper::findObjectInFather(aSelectedObject, aName);

        if (aFindedObject == GEOM::GEOM_Object::_nil()) { // Object not found in study
          GEOM::GEOM_IShapesOperations_var aShapesOp =
            getGeomEngine()->GetIShapesOperations(getStudyId());
          aSelectedObject = aShapesOp->GetSubShape(aSelectedObject, anIndex);
        }
        else {
          aSelectedObject = aFindedObject; // get Object from study
        }
      }
      else { // Global Selection
        if (aShape.ShapeType() != TopAbs_EDGE && myEditCurrentArgument == GroupPoints->LineEdit2) {
          aSelectedObject = GEOM::GEOM_Object::_nil();
          aName = "";
          myOk = false;
        }
      }
    }

    myEditCurrentArgument->setText(aName);

    if (myEditCurrentArgument == GroupPoints->LineEdit1) {
      myBase = aSelectedObject;
      myOkBase = true;
      if (!myOkVec)
        GroupPoints->PushButton2->click();
    }
    else if (myEditCurrentArgument == GroupPoints->LineEdit2 && myOk) {
      myOkVec = true;
      myVec = aSelectedObject;
      if (!myOkBase)
        GroupPoints->PushButton1->click();
    }
  }
  else if (getConstructorId() == 1) { // getConstructorId()==1 - extrusion using 2 points
    if (aSelList.Extent() != 1) {
      if (myEditCurrentArgument == GroupPoints2->LineEdit1)
        myOkBase = false;
      else if (myEditCurrentArgument == GroupPoints2->LineEdit2) {
        myPoint1 = GEOM::GEOM_Object::_nil();
        myOkPnt1 = false;
      }
      else if (myEditCurrentArgument == GroupPoints2->LineEdit3) {
        myPoint2 = GEOM::GEOM_Object::_nil();
        myOkPnt2 = false;
      }
      return;
    }

    // nbSel == 1
    Standard_Boolean testResult = Standard_False;
    GEOM::GEOM_Object_var aSelectedObject =
      GEOMBase::ConvertIOinGEOMObject(aSelList.First(), testResult);

    if (!testResult || CORBA::is_nil(aSelectedObject))
      return;

    QString aName = GEOMBase::GetName(aSelectedObject);
    TopoDS_Shape aShape;
    bool myOk = true;
    if (GEOMBase::GetShape(aSelectedObject, aShape, TopAbs_SHAPE) && !aShape.IsNull()) {
      TColStd_IndexedMapOfInteger aMap;
      aSelMgr->GetIndexes(aSelList.First(), aMap);
      if (aMap.Extent() == 1) {
        int anIndex = aMap(1);
        aName.append(":vertex_" + QString::number(anIndex));

        //Find SubShape Object in Father
        GEOM::GEOM_Object_var aFindedObject = findObjectInFather(aSelectedObject, aName);

        if (aFindedObject == GEOM::GEOM_Object::_nil()) { // Object not found in study
          GEOM::GEOM_IShapesOperations_var aShapesOp = getGeomEngine()->GetIShapesOperations(getStudyId());
          aSelectedObject = aShapesOp->GetSubShape(aSelectedObject, anIndex);
        }
        else {
          aSelectedObject = aFindedObject;
        }
      }
      else {
        if ((aShape.ShapeType() != TopAbs_VERTEX && myEditCurrentArgument == GroupPoints2->LineEdit2) ||
             (aShape.ShapeType() != TopAbs_VERTEX && myEditCurrentArgument == GroupPoints2->LineEdit3)) {
          aSelectedObject = GEOM::GEOM_Object::_nil();
          aName = "";
          myOk = false;
        }
      }
    }

    myEditCurrentArgument->setText(aName);

    if (myEditCurrentArgument == GroupPoints2->LineEdit1) {
      myOkBase = true;
      myBase = aSelectedObject;
      if (!myOkPnt1)
        GroupPoints2->PushButton2->click();
    }
    else if (myEditCurrentArgument == GroupPoints2->LineEdit2 && myOk) {
      myOkPnt1 = true;
      myPoint1 = aSelectedObject;
      if (!myOkPnt2)
        GroupPoints2->PushButton3->click();
    }
    else if (myEditCurrentArgument == GroupPoints2->LineEdit3 && myOk) {
      myOkPnt2 = true;
      myPoint2 = aSelectedObject;
      if (!myOkBase)
        GroupPoints2->PushButton1->click();
    }
  }
  else if (getConstructorId() == 2) { // extrusion using dx dy dz
    Standard_Boolean testResult = Standard_False;
    GEOM::GEOM_Object_var aSelectedObject =
      GEOMBase::ConvertIOinGEOMObject(aSelList.First(), testResult);

    if (!testResult || CORBA::is_nil(aSelectedObject))
      return;

    QString aName = GEOMBase::GetName(aSelectedObject);
    myBase = aSelectedObject;
    myOkBase = true;
    myEditCurrentArgument->setText(aName);
  }

  // clear selection
  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
  myGeomGUI->getApp()->selectionMgr()->clearSelected();
  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));

  displayPreview();
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void GenerationGUI_PrismDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
  globalSelection(GEOM_ALLSHAPES);
  if (send == GroupPoints->PushButton1) {
    myEditCurrentArgument = GroupPoints->LineEdit1;
    GroupPoints->PushButton2->setDown(false);
    GroupPoints->LineEdit2->setEnabled(false);
  }
  else if (send == GroupPoints->PushButton2) {
    myEditCurrentArgument = GroupPoints->LineEdit2;
    GroupPoints->PushButton1->setDown(false);
    GroupPoints->LineEdit1->setEnabled(false);
    localSelection(GEOM::GEOM_Object::_nil(), TopAbs_EDGE);
  }
  else if (send == GroupPoints2->PushButton1) {
    myEditCurrentArgument = GroupPoints2->LineEdit1;
    GroupPoints2->PushButton2->setDown(false);
    GroupPoints2->PushButton3->setDown(false);
    GroupPoints2->LineEdit2->setEnabled(false);
    GroupPoints2->LineEdit3->setEnabled(false);
  }
  else if (send == GroupPoints2->PushButton2) {
    myEditCurrentArgument = GroupPoints2->LineEdit2;
    GroupPoints2->PushButton1->setDown(false);
    GroupPoints2->PushButton3->setDown(false);
    GroupPoints2->LineEdit1->setEnabled(false);
    GroupPoints2->LineEdit3->setEnabled(false);
    localSelection(GEOM::GEOM_Object::_nil(), TopAbs_VERTEX);
  }
  else if (send == GroupPoints2->PushButton3) {
    myEditCurrentArgument = GroupPoints2->LineEdit3;
    GroupPoints2->PushButton1->setDown(false);
    GroupPoints2->PushButton2->setDown(false);
    GroupPoints2->LineEdit1->setEnabled(false);
    GroupPoints2->LineEdit2->setEnabled(false);
    localSelection(GEOM::GEOM_Object::_nil(), TopAbs_VERTEX);
  }
  else   if (send == GroupPoints3->PushButton1) {
    myEditCurrentArgument = GroupPoints3->LineEdit1;
  }
  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));

  // enable line edit
  myEditCurrentArgument->setEnabled(true);
  myEditCurrentArgument->setFocus();
  // after setFocus(), because it will be setDown(false) when loses focus
  send->setDown(true);

  // seems we need it only to avoid preview disappearing, caused by selection mode change
  displayPreview();
}

//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void GenerationGUI_PrismDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if (send == GroupPoints->LineEdit1 ||
      send == GroupPoints->LineEdit2 ||
      send == GroupPoints2->LineEdit1 ||
      send == GroupPoints2->LineEdit2 ||
      send == GroupPoints2->LineEdit3 ||
      send == GroupPoints3->LineEdit1) {
    myEditCurrentArgument = send;
    GEOMBase_Skeleton::LineEditReturnPressed();
  }
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void GenerationGUI_PrismDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();

  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
	   this, SLOT( SelectionIntoArgument() ) );

  ConstructorsClicked( getConstructorId() );
}

//=================================================================================
// function : enterEvent()
// purpose  : when mouse enter onto the QWidget
//=================================================================================
void GenerationGUI_PrismDlg::enterEvent (QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void GenerationGUI_PrismDlg::ValueChangedInSpinBox()
{
  displayPreview();
}

//=================================================================================
// function : getHeight()
// purpose  :
//=================================================================================
double GenerationGUI_PrismDlg::getHeight() const
{
  return GroupPoints->SpinBox_DX->value();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr GenerationGUI_PrismDlg::createOperation()
{
  return getGeomEngine()->GetI3DPrimOperations(getStudyId());
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool GenerationGUI_PrismDlg::isValid (QString& msg)
{
  bool ok = true;
  if( getConstructorId() == 0 )
    ok = GroupPoints->SpinBox_DX->isValid( msg, !IsPreview() ) && ok;
  else if( getConstructorId() == 2 )
  {
    ok = GroupPoints3->SpinBox_DX->isValid( msg, !IsPreview() ) && ok;
    ok = GroupPoints3->SpinBox_DY->isValid( msg, !IsPreview() ) && ok;
    ok = GroupPoints3->SpinBox_DZ->isValid( msg, !IsPreview() ) && ok;
  }

  if (getConstructorId() == 0)
    return (myOkBase && myOkVec) && ok;     // by vector and height
  else if (getConstructorId() == 1)
    return (myOkBase && myOkPnt1 && myOkPnt2);   // by two points
  else if (getConstructorId() == 2)
    return myOkBase && ok;

  return false;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool GenerationGUI_PrismDlg::execute (ObjectList& objects)
{
  QStringList aParameters;
  GEOM::GEOM_Object_var anObj;

  switch (getConstructorId()) {
  case 0:
    if (!myBothway) {
      anObj = GEOM::GEOM_I3DPrimOperations::_narrow(getOperation())->
        MakePrismVecH(myBase, myVec, getHeight());
    }
    else {
      anObj = GEOM::GEOM_I3DPrimOperations::_narrow(getOperation())->
        MakePrismVecH2Ways(myBase, myVec, getHeight());
    }

    if (!anObj->_is_nil() && !IsPreview())
    {
      aParameters << GroupPoints->SpinBox_DX->text();
      anObj->SetParameters(GeometryGUI::JoinObjectParameters(aParameters));
    }

    break;
  case 1:
    if (!myBothway2) {
      anObj = GEOM::GEOM_I3DPrimOperations::_narrow(getOperation())->
        MakePrismTwoPnt(myBase, myPoint1, myPoint2);
    }
    else {
      anObj = GEOM::GEOM_I3DPrimOperations::_narrow(getOperation())->
        MakePrismTwoPnt2Ways(myBase, myPoint1, myPoint2);
    }
    break;
  case 2:
    double dx = GroupPoints3->SpinBox_DX->value();
    double dy = GroupPoints3->SpinBox_DY->value();
    double dz = GroupPoints3->SpinBox_DZ->value();

    if (!myBothway3) {
      anObj = GEOM::GEOM_I3DPrimOperations::_narrow(getOperation())->
        MakePrismDXDYDZ(myBase, dx, dy, dz);
    }
    else {
      anObj = GEOM::GEOM_I3DPrimOperations::_narrow(getOperation())->
        MakePrismDXDYDZ2Ways(myBase, dx, dy, dz);
    }

    if (!anObj->_is_nil() && !IsPreview())
    {
      aParameters << GroupPoints3->SpinBox_DX->text();
      aParameters << GroupPoints3->SpinBox_DY->text();
      aParameters << GroupPoints3->SpinBox_DZ->text();
      anObj->SetParameters(GeometryGUI::JoinObjectParameters(aParameters));
    }

    break;
  }

  if (!anObj->_is_nil())
    objects.push_back(anObj._retn());

  return true;
}

//=================================================================================
// function : onReverse()
// purpose  :
//=================================================================================
void GenerationGUI_PrismDlg::onReverse()
{
  double anOldValue = GroupPoints->SpinBox_DX->value();
  GroupPoints->SpinBox_DX->setValue(-anOldValue);
}

//=================================================================================
// function : onBothway()
// purpose  :
//=================================================================================
void GenerationGUI_PrismDlg::onBothway()
{
  bool anOldValue;
  switch (getConstructorId()) {
  case 0:
    anOldValue = myBothway;
    myBothway = !anOldValue;
    GroupPoints->CheckButton2->setEnabled(!myBothway);
    displayPreview();
    break;
  case 1:
    anOldValue = myBothway2;
    myBothway2 = !anOldValue;
    displayPreview();
    break;
  case 2:
    anOldValue = myBothway3;
    myBothway3 = !anOldValue;
    displayPreview();
    break;
  }
}

//=================================================================================
// function : addSubshapeToStudy
// purpose  : virtual method to add new SubObjects if local selection
//=================================================================================
void GenerationGUI_PrismDlg::addSubshapesToStudy()
{
  QMap<QString, GEOM::GEOM_Object_var> objMap;

  switch (getConstructorId()) {
  case 0:
    objMap[GroupPoints->LineEdit2->text()] = myVec;
    break;
  case 1:
    objMap[GroupPoints2->LineEdit2->text()] = myPoint1;
    objMap[GroupPoints2->LineEdit3->text()] = myPoint2;
    break;
  }
  addSubshapesToFather(objMap);
}
