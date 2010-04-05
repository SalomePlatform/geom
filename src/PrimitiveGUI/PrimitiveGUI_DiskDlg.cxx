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
// File   : PrimitiveGUI_DiskDlg.cxx
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.
//
#include "PrimitiveGUI_DiskDlg.h"

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

//=================================================================================
// class    : PrimitiveGUI_DiskDlg()
// purpose  : Constructs a PrimitiveGUI_DiskDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
PrimitiveGUI_DiskDlg::PrimitiveGUI_DiskDlg (GeometryGUI* theGeometryGUI, QWidget* parent,
                                            bool modal, Qt::WindowFlags fl)
  : GEOMBase_Skeleton(theGeometryGUI, parent, modal, fl),
    myInitial(true)
{
  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
  QPixmap image0 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_DISK_PNT_VEC_R")));
  QPixmap image1 (aResMgr->loadPixmap("GEOM", tr("ICON_SELECT")));
  QPixmap image2 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_DISK_THREE_POINTS")));
  QPixmap image3 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_DISK_R")));

  setWindowTitle(tr("GEOM_DISK_TITLE"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_DISK"));
  mainFrame()->RadioButton1->setIcon(image3);
  mainFrame()->RadioButton2->setIcon(image0);
  mainFrame()->RadioButton3->setIcon(image2);

  GroupPntVecR = new DlgRef_2Sel1Spin(centralWidget());
  GroupPntVecR->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  GroupPntVecR->TextLabel1->setText(tr("GEOM_CENTER_POINT"));
  GroupPntVecR->TextLabel2->setText(tr("GEOM_VECTOR"));
  GroupPntVecR->TextLabel3->setText(tr("GEOM_RADIUS"));
  GroupPntVecR->PushButton1->setIcon(image1);
  GroupPntVecR->PushButton2->setIcon(image1);
  GroupPntVecR->LineEdit1->setReadOnly(true);
  GroupPntVecR->LineEdit2->setReadOnly(true);

  Group3Pnts = new DlgRef_3Sel(centralWidget());
  Group3Pnts->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  Group3Pnts->TextLabel1->setText(tr("GEOM_POINT1"));
  Group3Pnts->TextLabel2->setText(tr("GEOM_POINT2"));
  Group3Pnts->TextLabel3->setText(tr("GEOM_POINT3"));
  Group3Pnts->PushButton1->setIcon(image1);
  Group3Pnts->PushButton2->setIcon(image1);
  Group3Pnts->PushButton3->setIcon(image1);
  Group3Pnts->LineEdit1->setReadOnly(true);
  Group3Pnts->LineEdit2->setReadOnly(true);
  Group3Pnts->LineEdit3->setReadOnly(true);

  GroupDimensions = new DlgRef_1Spin(centralWidget());
  GroupDimensions->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  GroupDimensions->TextLabel1->setText(tr("GEOM_RADIUS"));

  GroupOrientation = new DlgRef_3Radio(centralWidget());
  GroupOrientation->GroupBox1->setTitle(tr("GEOM_ORIENTATION"));
  GroupOrientation->RadioButton1->setText(tr("GEOM_WPLANE_OXY"));
  GroupOrientation->RadioButton2->setText(tr("GEOM_WPLANE_OYZ"));
  GroupOrientation->RadioButton3->setText(tr("GEOM_WPLANE_OZX"));

  QVBoxLayout* layout = new QVBoxLayout(centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
  layout->addWidget(GroupPntVecR);
  layout->addWidget(Group3Pnts);
  layout->addWidget(GroupDimensions);
  layout->addWidget(GroupOrientation);
  /***************************************************************/

  setHelpFileName("create_disk_page.html");

  Init();
}

//=================================================================================
// function : ~PrimitiveGUI_DiskDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
PrimitiveGUI_DiskDlg::~PrimitiveGUI_DiskDlg()
{
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void PrimitiveGUI_DiskDlg::Init()
{
  // Get setting of step value from file configuration
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  double aStep = resMgr->doubleValue("Geometry", "SettingsGeomStep", 100);

  // min, max, step and decimals for spin boxes & initial values
  initSpinBox(GroupPntVecR->SpinBox_DX, 0.00001, COORD_MAX, aStep, "length_precision" );
  initSpinBox(GroupDimensions->SpinBox_DX, 0.00001, COORD_MAX, aStep, "length_precision" );

  // init variables
  myEditCurrentArgument = GroupPntVecR->LineEdit1;
  myOrientationType = 1;
  GroupOrientation->RadioButton1->setChecked(true);

  GroupPntVecR->LineEdit1->setText("");
  GroupPntVecR->LineEdit2->setText("");
  Group3Pnts->LineEdit1->setText("");
  Group3Pnts->LineEdit2->setText("");
  Group3Pnts->LineEdit3->setText("");
  myPoint = myDir = myPoint1 = myPoint2 = myPoint3 = GEOM::GEOM_Object::_nil();

  GroupPntVecR->SpinBox_DX->setValue(100);
  GroupDimensions->SpinBox_DX->setValue(100);

  // signals and slots connections
  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(this,          SIGNAL(constructorsClicked(int)), this, SLOT(ConstructorsClicked(int)));

  connect(GroupPntVecR->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPntVecR->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(GroupPntVecR->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupPntVecR->LineEdit2, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(Group3Pnts->PushButton1,   SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(Group3Pnts->PushButton2,   SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(Group3Pnts->PushButton3,   SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(Group3Pnts->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(Group3Pnts->LineEdit2, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(Group3Pnts->LineEdit3, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(GroupPntVecR->SpinBox_DX,    SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox()));
  connect(GroupDimensions->SpinBox_DX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox()));

  connect(GroupOrientation->RadioButton1, SIGNAL(clicked()), this, SLOT(RadioButtonClicked()));
  connect(GroupOrientation->RadioButton2, SIGNAL(clicked()), this, SLOT(RadioButtonClicked()));
  connect(GroupOrientation->RadioButton3, SIGNAL(clicked()), this, SLOT(RadioButtonClicked()));

  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), this, SLOT(SetDoubleSpinBoxStep(double)));
  connect(myGeomGUI, SIGNAL(SignalCloseAllDialogs()),               this, SLOT(ClickOnCancel()));

  initName(tr("GEOM_DISK"));

  ConstructorsClicked(0);
}

//=================================================================================
// function : SetDoubleSpinBoxStep()
// purpose  : Double spin box management
//=================================================================================
void PrimitiveGUI_DiskDlg::SetDoubleSpinBoxStep (double step)
{
  GroupDimensions->SpinBox_DX->setSingleStep(step);
  GroupPntVecR->SpinBox_DX->setSingleStep(step);
}

//=================================================================================
// function : RadioBittonClicked()
// purpose  : Radio button management
//=================================================================================
void PrimitiveGUI_DiskDlg::RadioButtonClicked()
{
  if (GroupOrientation->RadioButton1->isChecked())
    myOrientationType = 1;
  else if (GroupOrientation->RadioButton2->isChecked())
    myOrientationType = 2;
  else if (GroupOrientation->RadioButton3->isChecked())
    myOrientationType = 3;
  displayPreview();
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void PrimitiveGUI_DiskDlg::ConstructorsClicked (int constructorId)
{
  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);

  switch (constructorId) {
  case 0:
    {
      GroupPntVecR->hide();
      Group3Pnts->hide();
      GroupDimensions->show();
      GroupOrientation->show();

      disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
      globalSelection(); // close local contexts, if any
      break;
    }
  case 1:
    {
      GroupDimensions->hide();
      GroupOrientation->hide();
      GroupPntVecR->show();
      Group3Pnts->hide();

      GroupPntVecR->PushButton1->click();
      break;
    }
  case 2:
    {
      GroupDimensions->hide();
      GroupOrientation->hide();
      GroupPntVecR->hide();
      Group3Pnts->show();

      Group3Pnts->PushButton1->click();
      break;
    }
  }

  qApp->processEvents();
  updateGeometry();
  resize(minimumSizeHint());

  if (myInitial) {
    myInitial = false;
    if (constructorId == 1 || constructorId == 2) {
      // on dialog initialization we init the first field with a selected object (if any)
      SelectionIntoArgument();
    }
    else {
      displayPreview();
    }
  }
  else {
    displayPreview();
  }
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void PrimitiveGUI_DiskDlg::ClickOnOk()
{
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool PrimitiveGUI_DiskDlg::ClickOnApply()
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
void PrimitiveGUI_DiskDlg::SelectionIntoArgument()
{
  if (getConstructorId() == 0)
    return;

  erasePreview();
  myEditCurrentArgument->setText("");

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  if (aSelList.Extent() != 1) {
    if      (myEditCurrentArgument == GroupPntVecR->LineEdit1) myPoint  = GEOM::GEOM_Object::_nil();
    else if (myEditCurrentArgument == GroupPntVecR->LineEdit2) myDir    = GEOM::GEOM_Object::_nil();
    else if (myEditCurrentArgument == Group3Pnts->LineEdit1)   myPoint1 = GEOM::GEOM_Object::_nil();
    else if (myEditCurrentArgument == Group3Pnts->LineEdit2)   myPoint2 = GEOM::GEOM_Object::_nil();
    else if (myEditCurrentArgument == Group3Pnts->LineEdit3)   myPoint3 = GEOM::GEOM_Object::_nil();
    return;
  }

  // nbSel == 1
  Handle(SALOME_InteractiveObject) anIO = aSelList.First();
  Standard_Boolean testResult = Standard_False;
  GEOM::GEOM_Object_var aSelectedObject = GEOMBase::ConvertIOinGEOMObject(anIO, testResult);

  if (!testResult || CORBA::is_nil(aSelectedObject))
    return;

  QString aName = GEOMBase::GetName(aSelectedObject);

  // If selected Vertex or Edge on the some Shape Get selection Subshape
  TopoDS_Shape aShape;
  if (GEOMBase::GetShape(aSelectedObject, aShape, TopAbs_SHAPE) && !aShape.IsNull())
  {
    TopAbs_ShapeEnum aNeedType = TopAbs_VERTEX;
    if (myEditCurrentArgument == GroupPntVecR->LineEdit2)
      aNeedType = TopAbs_EDGE;

    TColStd_IndexedMapOfInteger aMap;
    aSelMgr->GetIndexes(anIO, aMap);
    if (aMap.Extent() == 1) { // Local Selection
      int anIndex = aMap(1);
      if (aNeedType == TopAbs_EDGE)
        aName += QString(":edge_%1").arg(anIndex);
      else
        aName += QString(":vertex_%1").arg(anIndex);

      //Find SubShape Object in Father
      GEOM::GEOM_Object_var aFindedObject = GEOMBase_Helper::findObjectInFather(aSelectedObject, aName);

      if (aFindedObject == GEOM::GEOM_Object::_nil()) { // Object not found in study
        GEOM::GEOM_IShapesOperations_var aShapesOp = getGeomEngine()->GetIShapesOperations(getStudyId());
        aSelectedObject = aShapesOp->GetSubShape(aSelectedObject, anIndex);
      }
      else {
        aSelectedObject = aFindedObject; // get Object from study
      }
    }
    else { // Global Selection
      if (aShape.ShapeType() != aNeedType) {
        aSelectedObject = GEOM::GEOM_Object::_nil();
        aName = "";
      }
    }
  }

  myEditCurrentArgument->setText(aName);

  // clear selection
  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
  myGeomGUI->getApp()->selectionMgr()->clearSelected();
  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));

  if (myEditCurrentArgument == GroupPntVecR->LineEdit1) {
    myPoint = aSelectedObject;
    if (!myPoint->_is_nil() && myDir->_is_nil())
      GroupPntVecR->PushButton2->click();
  }
  else if (myEditCurrentArgument == GroupPntVecR->LineEdit2) {
    myDir = aSelectedObject;
    if (!myDir->_is_nil() && myPoint->_is_nil())
      GroupPntVecR->PushButton1->click();
  }
  else if (myEditCurrentArgument == Group3Pnts->LineEdit1) {
    myPoint1 = aSelectedObject;
    if (!myPoint1->_is_nil() && myPoint2->_is_nil())
      Group3Pnts->PushButton2->click();
  }
  else if (myEditCurrentArgument == Group3Pnts->LineEdit2) {
    myPoint2 = aSelectedObject;
    if (!myPoint2->_is_nil() && myPoint3->_is_nil())
      Group3Pnts->PushButton3->click();
  }
  else if (myEditCurrentArgument == Group3Pnts->LineEdit3) {
    myPoint3 = aSelectedObject;
    if (!myPoint3->_is_nil() && myPoint1->_is_nil())
      Group3Pnts->PushButton1->click();
  }

  displayPreview();
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void PrimitiveGUI_DiskDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  if (send == GroupPntVecR->PushButton1) {
    myEditCurrentArgument = GroupPntVecR->LineEdit1;

    GroupPntVecR->PushButton2->setDown(false);
    GroupPntVecR->LineEdit2->setEnabled(false);
  }
  else if (send == GroupPntVecR->PushButton2) {
    myEditCurrentArgument = GroupPntVecR->LineEdit2;

    GroupPntVecR->PushButton1->setDown(false);
    GroupPntVecR->LineEdit1->setEnabled(false);
  }
  else if (send == Group3Pnts->PushButton1) {
    myEditCurrentArgument = Group3Pnts->LineEdit1;

    Group3Pnts->PushButton2->setDown(false);
    Group3Pnts->PushButton3->setDown(false);
    Group3Pnts->LineEdit2->setEnabled(false);
    Group3Pnts->LineEdit3->setEnabled(false);
  }
  else if (send == Group3Pnts->PushButton2) {
    myEditCurrentArgument = Group3Pnts->LineEdit2;

    Group3Pnts->PushButton1->setDown(false);
    Group3Pnts->PushButton3->setDown(false);
    Group3Pnts->LineEdit1->setEnabled(false);
    Group3Pnts->LineEdit3->setEnabled(false);
  }
  else if (send == Group3Pnts->PushButton3) {
    myEditCurrentArgument = Group3Pnts->LineEdit3;

    Group3Pnts->PushButton1->setDown(false);
    Group3Pnts->PushButton2->setDown(false);
    Group3Pnts->LineEdit1->setEnabled(false);
    Group3Pnts->LineEdit2->setEnabled(false);
  }

  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
  if (myEditCurrentArgument == GroupPntVecR->LineEdit2) {
    globalSelection(); // close local contexts, if any
    localSelection(GEOM::GEOM_Object::_nil(), TopAbs_EDGE);
  }
  else {
    globalSelection(); // close local contexts, if any
    localSelection(GEOM::GEOM_Object::_nil(), TopAbs_VERTEX);
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
void PrimitiveGUI_DiskDlg::LineEditReturnPressed()
{
QLineEdit* send = (QLineEdit*)sender();
  if (send == GroupPntVecR->LineEdit1 ||
      send == GroupPntVecR->LineEdit2 ||
      send == Group3Pnts->LineEdit1 ||
      send == Group3Pnts->LineEdit2 ||
      send == Group3Pnts->LineEdit3) {
    myEditCurrentArgument = send;
    GEOMBase_Skeleton::LineEditReturnPressed();
  }
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void PrimitiveGUI_DiskDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
           this, SLOT( SelectionIntoArgument() ) );

  ConstructorsClicked( getConstructorId() );
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void PrimitiveGUI_DiskDlg::enterEvent (QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void PrimitiveGUI_DiskDlg::ValueChangedInSpinBox()
{
  displayPreview();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr PrimitiveGUI_DiskDlg::createOperation()
{
  return getGeomEngine()->GetI3DPrimOperations(getStudyId());
}

//=================================================================================
// function : isEqual
// purpose  : it may also be needed to check for min distance between gp_Pnt-s...
//=================================================================================
static bool isEqual (const GEOM::GEOM_Object_var& thePnt1, const GEOM::GEOM_Object_var& thePnt2)
{
  return thePnt1->_is_equivalent(thePnt2);
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool PrimitiveGUI_DiskDlg::isValid (QString& msg)
{
  bool ok = true;
  if( getConstructorId() == 0 )
    ok = GroupDimensions->SpinBox_DX->isValid( msg, !IsPreview() ) && ok;
  else if( getConstructorId() == 1 )
    ok = GroupPntVecR->SpinBox_DX->isValid( msg, !IsPreview() ) && ok;

  const int id = getConstructorId();
  if (id == 0)
    return ok;
  else if (id == 1)
    return !myPoint->_is_nil() && !myDir->_is_nil() && getRadius() > 0 && ok;
  else if (id == 2)
    return !myPoint1->_is_nil() && !myPoint2->_is_nil() && !myPoint3->_is_nil() &&
      !isEqual(myPoint1, myPoint2) && !isEqual(myPoint1, myPoint3) && !isEqual(myPoint2, myPoint3);
  return false;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool PrimitiveGUI_DiskDlg::execute (ObjectList& objects)
{
  bool res = false;
  QStringList aParameters;

  GEOM::GEOM_Object_var anObj;

  GEOM::GEOM_I3DPrimOperations_var anOper = GEOM::GEOM_I3DPrimOperations::_narrow(getOperation());

  switch (getConstructorId()) {
  case 0:
    anObj = anOper->MakeDiskR(getRadius(), myOrientationType);
    if (!anObj->_is_nil() && !IsPreview())
    {
      aParameters << GroupDimensions->SpinBox_DX->text();
      anObj->SetParameters(aParameters.join(":").toLatin1().constData());
    }
    res = true;
    break;
  case 1:
    anObj = anOper->MakeDiskPntVecR(myPoint, myDir, getRadius());
    if (!anObj->_is_nil() && !IsPreview())
    {
      aParameters << GroupPntVecR->SpinBox_DX->text();
      anObj->SetParameters(aParameters.join(":").toLatin1().constData());
    }
    res = true;
    break;
  case 2:
    anObj = anOper->MakeDiskThreePnt(myPoint1, myPoint2, myPoint3);
    res = true;
    break;
  }

  if (!anObj->_is_nil())
    objects.push_back(anObj._retn());

  return res;
}

//=================================================================================
// function : getRadius()
// purpose  :
//=================================================================================
double PrimitiveGUI_DiskDlg::getRadius() const
{
  double r = 0.;
  switch (getConstructorId()) {
  case 0:
    r = GroupDimensions->SpinBox_DX->value(); break;
  case 1:
    r = GroupPntVecR->SpinBox_DX->value(); break;
  }
  return r;
}

//=================================================================================
// function : addSubshapeToStudy
// purpose  : virtual method to add new SubObjects if local selection
//=================================================================================
void PrimitiveGUI_DiskDlg::addSubshapesToStudy()
{
  QMap<QString, GEOM::GEOM_Object_var> objMap;

  switch (getConstructorId()) {
  case 1:
    objMap[GroupPntVecR->LineEdit1->text()] = myPoint;
    objMap[GroupPntVecR->LineEdit2->text()] = myDir;
    break;
  case 2:
    objMap[Group3Pnts->LineEdit1->text()] = myPoint1;
    objMap[Group3Pnts->LineEdit2->text()] = myPoint2;
    objMap[Group3Pnts->LineEdit3->text()] = myPoint3;
    break;
  }
  addSubshapesToFather(objMap);
}
