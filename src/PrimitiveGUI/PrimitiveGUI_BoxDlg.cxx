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
// File   : PrimitiveGUI_BoxDlg.cxx
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.
//
#include "PrimitiveGUI_BoxDlg.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>

// OCCT Includes
#include <TopoDS_Shape.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#include <GEOMImpl_Types.hxx>
//#include <ostream>

//=================================================================================
// class    : PrimitiveGUI_BoxDlg()
// purpose  : Constructs a PrimitiveGUI_BoxDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
PrimitiveGUI_BoxDlg::PrimitiveGUI_BoxDlg (GeometryGUI* theGeometryGUI, QWidget* parent,
                                          bool modal, Qt::WindowFlags fl)
  : GEOMBase_Skeleton(theGeometryGUI, parent, modal, fl),
    myInitial(true)
{
  QPixmap image0 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_BOX_2P")));
  QPixmap image1 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_BOX_DXYZ")));
  QPixmap image2 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_SELECT")));

  setWindowTitle(tr("GEOM_BOX_TITLE"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_BOX"));
  mainFrame()->RadioButton1->setIcon(image0);
  mainFrame()->RadioButton2->setIcon(image1);
  mainFrame()->RadioButton3->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton3->close();

  GroupPoints = new DlgRef_2Sel(centralWidget());
  GroupPoints->GroupBox1->setTitle(tr("GEOM_DIAGONAL_POINTS"));
  GroupPoints->TextLabel1->setText(tr("GEOM_POINT_I").arg(1));
  GroupPoints->TextLabel2->setText(tr("GEOM_POINT_I").arg(2));
  GroupPoints->PushButton1->setIcon(image2);
  GroupPoints->PushButton2->setIcon(image2);

  GroupDimensions = new DlgRef_3Spin(centralWidget());
  GroupDimensions->GroupBox1->setTitle(tr("GEOM_BOX_OBJ"));
  GroupDimensions->TextLabel1->setText(tr("GEOM_DX"));
  GroupDimensions->TextLabel2->setText(tr("GEOM_DY"));
  GroupDimensions->TextLabel3->setText(tr("GEOM_DZ"));

  QVBoxLayout* layout = new QVBoxLayout(centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
  layout->addWidget(GroupPoints);
  layout->addWidget(GroupDimensions);
  /***************************************************************/

  setHelpFileName("create_box_page.html");

  Init();
}

//=================================================================================
// function : ~DialogBox_Box()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
PrimitiveGUI_BoxDlg::~PrimitiveGUI_BoxDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void PrimitiveGUI_BoxDlg::Init()
{
  // Get setting of step value from file configuration
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  double step = resMgr->doubleValue("Geometry", "SettingsGeomStep", 100);

  // min, max, step and decimals for spin boxes
  initSpinBox(GroupDimensions->SpinBox_DX, 0.000001, COORD_MAX, step, "length_precision" );
  initSpinBox(GroupDimensions->SpinBox_DY, 0.000001, COORD_MAX, step, "length_precision" );
  initSpinBox(GroupDimensions->SpinBox_DZ, 0.000001, COORD_MAX, step, "length_precision" );

  // init variables
  GroupPoints->LineEdit1->setReadOnly(true);
  GroupPoints->LineEdit2->setReadOnly(true);

  GroupPoints->LineEdit1->setText("");
  GroupPoints->LineEdit2->setText("");
  myPoint1 = myPoint2 = GEOM::GEOM_Object::_nil();

  double initValue = 200.0;
  GroupDimensions->SpinBox_DX->setValue(initValue);
  GroupDimensions->SpinBox_DY->setValue(initValue);
  GroupDimensions->SpinBox_DZ->setValue(initValue);

  // signals and slots connections
  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(this,          SIGNAL(constructorsClicked(int)), this, SLOT(ConstructorsClicked(int)));

  connect(GroupPoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(GroupPoints->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupPoints->LineEdit2, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(GroupDimensions->SpinBox_DX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox()));
  connect(GroupDimensions->SpinBox_DY, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox()));
  connect(GroupDimensions->SpinBox_DZ, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox()));

  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), this, SLOT(SetDoubleSpinBoxStep(double)));

  initName(tr("GEOM_BOX"));

  setConstructorId(1); // simplest constructor
  ConstructorsClicked(1);
}

//=================================================================================
// function : SetDoubleSpinBoxStep()
// purpose  : Double spin box management
//=================================================================================
void PrimitiveGUI_BoxDlg::SetDoubleSpinBoxStep (double step)
{
  GroupDimensions->SpinBox_DX->setSingleStep(step);
  GroupDimensions->SpinBox_DY->setSingleStep(step);
  GroupDimensions->SpinBox_DZ->setSingleStep(step);
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void PrimitiveGUI_BoxDlg::ConstructorsClicked (int constructorId)
{
  switch (constructorId) {
  case 0:
    {
      GroupDimensions->hide();
      GroupPoints->show();

      GroupPoints->PushButton1->click();
      break;
    }
  case 1:
    {
      GroupPoints->hide();
      GroupDimensions->show();
      disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
      globalSelection(); // close local contexts, if any

      displayPreview();
      break;
    }
  }

  qApp->processEvents();
  updateGeometry();
  resize(minimumSizeHint());

  if (myInitial) {
    // on dialog initialization we init the first field with a selected object (if any)
    SelectionIntoArgument();
    myInitial = false;
  }
  else {
    displayPreview();
  }
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void PrimitiveGUI_BoxDlg::ClickOnOk()
{
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool PrimitiveGUI_BoxDlg::ClickOnApply()
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
void PrimitiveGUI_BoxDlg::SelectionIntoArgument()
{
  if (getConstructorId() != 0)
    return;

  erasePreview();
  myEditCurrentArgument->setText("");

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  if (aSelList.Extent() != 1) {
    if      (myEditCurrentArgument == GroupPoints->LineEdit1) myPoint1 = GEOM::GEOM_Object::_nil();
    else if (myEditCurrentArgument == GroupPoints->LineEdit2) myPoint2 = GEOM::GEOM_Object::_nil();
    return;
  }

  // nbSel == 1
  Standard_Boolean testResult = Standard_False;
  GEOM::GEOM_Object_var aSelectedObject = GEOMBase::ConvertIOinGEOMObject(aSelList.First(), testResult);

  if (!testResult || CORBA::is_nil(aSelectedObject))
    return;

  QString aName = GEOMBase::GetName(aSelectedObject);

  // Get Selected object if selected subshape
  TopoDS_Shape aShape;
  if (GEOMBase::GetShape(aSelectedObject, aShape, TopAbs_SHAPE) && !aShape.IsNull())
  {
    TColStd_IndexedMapOfInteger aMap;
    aSelMgr->GetIndexes(aSelList.First(), aMap);
    if (aMap.Extent() == 1) // Local Selection
    {
      int anIndex = aMap(1);
      aName.append(":vertex_" + QString::number(anIndex));        

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
    else // Global Selection
    {
      if (aShape.ShapeType() != TopAbs_VERTEX) {
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

  if (myEditCurrentArgument == GroupPoints->LineEdit1) {
    myPoint1 = aSelectedObject;
    if (!myPoint1->_is_nil() && myPoint2->_is_nil())
      GroupPoints->PushButton2->click();
  }
  else if (myEditCurrentArgument == GroupPoints->LineEdit2) {
    myPoint2 = aSelectedObject;
    if (!myPoint2->_is_nil() && myPoint1->_is_nil())
      GroupPoints->PushButton1->click();
  }

  displayPreview();
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void PrimitiveGUI_BoxDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  // ?? Commented, because we need this flag in ConstructorsClicked, because
  // SelectionIntoArgument must be called only on dialog creation, and must not be called on
  // simple switch between constructors (as we need to keep old values in fields in this case)

  // clear selection
  //disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
  //if (myInitial)
  //  myInitial = false;
  //else
  //  myGeomGUI->getApp()->selectionMgr()->clearSelected();

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

  // enable line edit
  myEditCurrentArgument->setEnabled(true);
  myEditCurrentArgument->setFocus();
  // after setFocus(), because it will be setDown(false) when loses focus
  send->setDown(true);

  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
  globalSelection(); // close local contexts, if any
  localSelection(GEOM::GEOM_Object::_nil(), TopAbs_VERTEX);
  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));

  // seems we need it only to avoid preview disappearing, caused by selection mode change
  displayPreview();
}

//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void PrimitiveGUI_BoxDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if (send == GroupPoints->LineEdit1 || send == GroupPoints->LineEdit2) {
    myEditCurrentArgument = send;
    GEOMBase_Skeleton::LineEditReturnPressed();
  }
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void PrimitiveGUI_BoxDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  if (getConstructorId() == 0) {
    localSelection(GEOM::GEOM_Object::_nil(), TopAbs_VERTEX);
    connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
            this, SLOT(SelectionIntoArgument()));
  }
  displayPreview();
}

//=================================================================================
// function : enterEvent [REDEFINED]
// purpose  :
//=================================================================================
void PrimitiveGUI_BoxDlg::enterEvent (QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void PrimitiveGUI_BoxDlg::ValueChangedInSpinBox()
{
  displayPreview();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr PrimitiveGUI_BoxDlg::createOperation()
{
  return getGeomEngine()->GetI3DPrimOperations(getStudyId());
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool PrimitiveGUI_BoxDlg::isValid (QString& msg)
{
  bool ok = true;
  if( getConstructorId() == 1 )
  {
    ok = GroupDimensions->SpinBox_DX->isValid( msg, !IsPreview() ) && ok;
    ok = GroupDimensions->SpinBox_DY->isValid( msg, !IsPreview() ) && ok;
    ok = GroupDimensions->SpinBox_DZ->isValid( msg, !IsPreview() ) && ok;

    ok = fabs( GroupDimensions->SpinBox_DX->value() ) > Precision::Confusion() && ok;
    ok = fabs( GroupDimensions->SpinBox_DY->value() ) > Precision::Confusion() && ok;
    ok = fabs( GroupDimensions->SpinBox_DZ->value() ) > Precision::Confusion() && ok;
  }
  return getConstructorId() == 0 ? !(myPoint1->_is_nil() || myPoint2->_is_nil()) : ok;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool PrimitiveGUI_BoxDlg::execute (ObjectList& objects)
{
  bool res = false;

  GEOM::GEOM_Object_var anObj;

  GEOM::GEOM_I3DPrimOperations_var anOper = GEOM::GEOM_I3DPrimOperations::_narrow(getOperation());
  
  switch (getConstructorId()) {
  case 0:
    {
      if (!CORBA::is_nil(myPoint1) && !CORBA::is_nil(myPoint2)) {
        anObj = anOper->MakeBoxTwoPnt(myPoint1, myPoint2);
        res = true;
      }
    }
    break;
  case 1:
    {
      double x = GroupDimensions->SpinBox_DX->value();
      double y = GroupDimensions->SpinBox_DY->value();
      double z = GroupDimensions->SpinBox_DZ->value();

      anObj = anOper->MakeBoxDXDYDZ(x, y, z);
      if (!anObj->_is_nil() && !IsPreview())
      {
        QStringList aParameters;
        aParameters << GroupDimensions->SpinBox_DX->text();
        aParameters << GroupDimensions->SpinBox_DY->text();
        aParameters << GroupDimensions->SpinBox_DZ->text();
        anObj->SetParameters(aParameters.join(":").toLatin1().constData());
      }
      res = true;
    }
    break;
  }

  if (!anObj->_is_nil())
    objects.push_back(anObj._retn());

  return res;
}

//=================================================================================
// function : addSubshapesToStudy
// purpose  : virtual method to add new SubObjects if local selection
//=================================================================================
void PrimitiveGUI_BoxDlg::addSubshapesToStudy()
{
  if (getConstructorId() == 0) {
    QMap<QString, GEOM::GEOM_Object_var> objMap;
    objMap[GroupPoints->LineEdit1->text()] = myPoint1;
    objMap[GroupPoints->LineEdit2->text()] = myPoint2;
    addSubshapesToFather(objMap);
  }
}
