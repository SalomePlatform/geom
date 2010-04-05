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
// File   : TransformationGUI_RotationDlg.cxx
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.
//
#include "TransformationGUI_RotationDlg.h"

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
// class    : TransformationGUI_RotationDlg()
// purpose  : Constructs a TransformationGUI_RotationDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
TransformationGUI_RotationDlg::TransformationGUI_RotationDlg
(GeometryGUI* theGeometryGUI, QWidget* parent, bool modal, Qt::WindowFlags fl)
  : GEOMBase_Skeleton(theGeometryGUI, parent, modal, fl),
    myInitial(true)
{
  SUIT_ResourceMgr* aResMgr = myGeomGUI->getApp()->resourceMgr();
  QPixmap image0 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_ROTATION")));
  QPixmap image1 (aResMgr->loadPixmap("GEOM", tr("ICON_SELECT")));
  QPixmap image2 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_ROTATION_THREE_POINTS")));

  setWindowTitle(tr("GEOM_ROTATION_TITLE"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_ROTATION"));
  mainFrame()->RadioButton1->setIcon(image0);
  mainFrame()->RadioButton2->setIcon(image2);
  mainFrame()->RadioButton3->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton3->close();

  GroupPoints = new DlgRef_4Sel1Spin2Check(centralWidget());
  GroupPoints->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  GroupPoints->TextLabel1->setText(tr("GEOM_OBJECTS"));
  GroupPoints->TextLabel2->setText(tr("GEOM_AXIS"));
  GroupPoints->TextLabel3->setText(tr("GEOM_ANGLE"));
  GroupPoints->TextLabel4->setText(tr("GEOM_POINT_I").arg(1));
  GroupPoints->TextLabel5->setText(tr("GEOM_POINT_I").arg(2));

  GroupPoints->LineEdit1->setReadOnly(true);
  GroupPoints->LineEdit2->setReadOnly(true);
  GroupPoints->LineEdit4->setReadOnly(true);
  GroupPoints->LineEdit5->setReadOnly(true);
  GroupPoints->PushButton1->setIcon(image1);
  GroupPoints->PushButton2->setIcon(image1);
  GroupPoints->PushButton4->setIcon(image1);
  GroupPoints->PushButton5->setIcon(image1);
  GroupPoints->CheckButton1->setText(tr("GEOM_CREATE_COPY"));
  GroupPoints->CheckButton2->setText(tr("GEOM_REVERSE"));

  QVBoxLayout* layout = new QVBoxLayout(centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
  layout->addWidget(GroupPoints);
  /***************************************************************/

  setHelpFileName("rotation_operation_page.html");

  // Activate Create a Copy mode
  GroupPoints->CheckButton1->setChecked(true);
  CreateCopyModeChanged(true);

  Init();
}

//=================================================================================
// function : ~TransformationGUI_RotationDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
TransformationGUI_RotationDlg::~TransformationGUI_RotationDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void TransformationGUI_RotationDlg::Init()
{
  double anAngle = 0;
  double SpecificStep = 5;

  // min, max, step and decimals for spin boxes & initial values
  initSpinBox(GroupPoints->SpinBox_DX, COORD_MIN, COORD_MAX, SpecificStep, "angle_precision" );
  GroupPoints->SpinBox_DX->setValue(anAngle);

  // init variables
  GroupPoints->LineEdit1->setText("");
  GroupPoints->LineEdit2->setText("");
  GroupPoints->LineEdit4->setText("");
  GroupPoints->LineEdit5->setText("");

  myAxis = myCentPoint = myPoint1 = myPoint2 = GEOM::GEOM_Object::_nil();

  mainFrame()->GroupBoxPublish->show();

  // signals and slots connections
  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(this, SIGNAL(constructorsClicked(int)), this, SLOT(ConstructorsClicked(int)));

  connect(GroupPoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->PushButton4, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->PushButton5, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(GroupPoints->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupPoints->LineEdit2, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(GroupPoints->SpinBox_DX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox()));

  connect(GroupPoints->SpinBox_DX, SIGNAL(textChanged( const QString& )),
          this, SLOT(TextValueChangedInSpinBox( const QString&)));

  connect(GroupPoints->CheckButton1, SIGNAL(toggled(bool)), this, SLOT(CreateCopyModeChanged(bool)));
  connect(GroupPoints->CheckButton2, SIGNAL(toggled(bool)), this, SLOT(onReverse()));

  initName(tr("GEOM_ROTATION"));

  ConstructorsClicked(0);
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void TransformationGUI_RotationDlg::ConstructorsClicked (int constructorId)
{
  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);

  switch (constructorId) {
  case 0: // rotation an object angle and axis
    {
      GroupPoints->ShowRows(2, 3, false);
      GroupPoints->TextLabel2->setText(tr("GEOM_AXIS"));
      GroupPoints->LineEdit2->clear();
      GroupPoints->ShowRows(4, 4, true);

      myAxis = GEOM::GEOM_Object::_nil();

      GroupPoints->PushButton1->click();
    }
    break;
  case 1: // rotation an object by 3 points
    {
      GroupPoints->ShowRows(4, 4, false);
      GroupPoints->ShowRows(2, 3, true);
      GroupPoints->TextLabel2->setText(tr("GEOM_CENTRAL_POINT"));
      GroupPoints->TextLabel4->setText(tr("GEOM_POINT_I").arg(1));
      GroupPoints->TextLabel5->setText(tr("GEOM_POINT_I").arg(2));

      GroupPoints->LineEdit2->clear();
      GroupPoints->LineEdit4->clear();
      GroupPoints->LineEdit5->clear();

      myCentPoint = myPoint1 = myPoint2 = GEOM::GEOM_Object::_nil();

      GroupPoints->PushButton1->click();
    }
    break;
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
void TransformationGUI_RotationDlg::ClickOnOk()
{
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool TransformationGUI_RotationDlg::ClickOnApply()
{
  if (!onAccept(GroupPoints->CheckButton1->isChecked()))
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
void TransformationGUI_RotationDlg::SelectionIntoArgument()
{
  erasePreview();
  myEditCurrentArgument->setText("");

  if (myEditCurrentArgument == GroupPoints->LineEdit1)
    myObjects.length(0);
  else if (myEditCurrentArgument == GroupPoints->LineEdit2 && getConstructorId() == 0)
    myAxis = GEOM::GEOM_Object::_nil();
  else if (myEditCurrentArgument == GroupPoints->LineEdit2 && getConstructorId() == 1)
    myCentPoint = GEOM::GEOM_Object::_nil();
  else if (myEditCurrentArgument == GroupPoints->LineEdit4)
    myPoint1 = GEOM::GEOM_Object::_nil();
  else if (myEditCurrentArgument == GroupPoints->LineEdit5)
    myPoint2 = GEOM::GEOM_Object::_nil();

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);
  GEOM::GEOM_Object_var aSelectedObject;

  QString aName;

  if (myEditCurrentArgument == GroupPoints->LineEdit1) {
    int aNbSel = GEOMBase::GetNameOfSelectedIObjects(aSelList, aName);
    if (aNbSel < 1)
      return;

    GEOMBase::ConvertListOfIOInListOfGO(aSelList, myObjects);
    if (!myObjects.length())
      return;
  }
  else {
    if (aSelList.Extent() != 1)
      return;

    // nbSel == 1
    Standard_Boolean testResult = Standard_False;
    aSelectedObject = GEOMBase::ConvertIOinGEOMObject(aSelList.First(), testResult);

    if (!testResult || CORBA::is_nil(aSelectedObject))
      return;

    aName = GEOMBase::GetName(aSelectedObject);

    // Get Selected object if selected subshape
    TopoDS_Shape aShape;
    if (GEOMBase::GetShape(aSelectedObject, aShape, TopAbs_SHAPE) && !aShape.IsNull())
    {
      TopAbs_ShapeEnum aNeedType = TopAbs_VERTEX;
      if (myEditCurrentArgument == GroupPoints->LineEdit2 && getConstructorId() == 0)
        aNeedType = TopAbs_EDGE;

      TColStd_IndexedMapOfInteger aMap;
      aSelMgr->GetIndexes(aSelList.First(), aMap);
      if (aMap.Extent() == 1)
      {
        int anIndex = aMap(1);
        if (aNeedType == TopAbs_EDGE)
          aName += QString(":edge_%1").arg(anIndex);
        else
          aName += QString(":vertex_%1").arg(anIndex);

        //Find SubShape Object in Father
        GEOM::GEOM_Object_var aFindedObject = findObjectInFather(aSelectedObject, aName);

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
        if (aShape.ShapeType() != aNeedType) {
          aSelectedObject = GEOM::GEOM_Object::_nil();
          aName = "";
        }
      }
    }
  }
  
  myEditCurrentArgument->setText(aName);

  if (myEditCurrentArgument == GroupPoints->LineEdit1) {
    /*    if (myObjects.length()) {
      if (getConstructorId() == 0 && myAxis->_is_nil() || getConstructorId() == 1 && myCentPoint->_is_nil() )
        GroupPoints->PushButton2->click();
        }*/   // here commented, because multiple objects can be selected IPAL 21437
  }
  else if (myEditCurrentArgument == GroupPoints->LineEdit2 && getConstructorId() == 0) {
    myAxis = aSelectedObject;
    if (!myAxis->_is_nil() && !myObjects.length())
      GroupPoints->PushButton1->click();
  }
  else if (myEditCurrentArgument == GroupPoints->LineEdit2 && getConstructorId() == 1) {
    myCentPoint = aSelectedObject;
    if (!myCentPoint->_is_nil() && myPoint1->_is_nil())
      GroupPoints->PushButton4->click();
  }
  else if (myEditCurrentArgument == GroupPoints->LineEdit4) {
    myPoint1 = aSelectedObject;
    if (!myPoint1->_is_nil() && myPoint2->_is_nil())
      GroupPoints->PushButton5->click();
  }
  else if (myEditCurrentArgument == GroupPoints->LineEdit5) {
    myPoint2 = aSelectedObject;
    if (!myPoint2->_is_nil() && !myObjects.length())
      GroupPoints->PushButton1->click();
  }

  // clear selection
  /*  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
  myGeomGUI->getApp()->selectionMgr()->clearSelected();
  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
  this, SLOT(SelectionIntoArgument()));*/
  // here commented, because multiple objects can be selected IPAL 21437

  displayPreview();
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void TransformationGUI_RotationDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
  globalSelection();

  if (send == GroupPoints->PushButton1) {
    myEditCurrentArgument = GroupPoints->LineEdit1;

    GroupPoints->PushButton2->setDown(false);
    GroupPoints->PushButton4->setDown(false);
    GroupPoints->PushButton5->setDown(false);
    GroupPoints->LineEdit2->setEnabled(false);
    GroupPoints->LineEdit4->setEnabled(false);
    GroupPoints->LineEdit5->setEnabled(false);
  }
  else if (send == GroupPoints->PushButton2) {
    myEditCurrentArgument = GroupPoints->LineEdit2;

    GroupPoints->PushButton1->setDown(false);
    GroupPoints->PushButton4->setDown(false);
    GroupPoints->PushButton5->setDown(false);
    GroupPoints->LineEdit1->setEnabled(false);
    GroupPoints->LineEdit4->setEnabled(false);
    GroupPoints->LineEdit5->setEnabled(false);

    if (getConstructorId() == 0)
      localSelection(GEOM::GEOM_Object::_nil(), TopAbs_EDGE);
    else
      localSelection(GEOM::GEOM_Object::_nil(), TopAbs_VERTEX);
  }
  else if (send == GroupPoints->PushButton4) {
    myEditCurrentArgument = GroupPoints->LineEdit4;

    GroupPoints->PushButton1->setDown(false);
    GroupPoints->PushButton2->setDown(false);
    GroupPoints->PushButton5->setDown(false);
    GroupPoints->LineEdit1->setEnabled(false);
    GroupPoints->LineEdit2->setEnabled(false);
    GroupPoints->LineEdit5->setEnabled(false);

    localSelection(GEOM::GEOM_Object::_nil(), TopAbs_VERTEX);
  }
  else if (send == GroupPoints->PushButton5) {
    myEditCurrentArgument = GroupPoints->LineEdit5;

    GroupPoints->PushButton1->setDown(false);
    GroupPoints->PushButton2->setDown(false);
    GroupPoints->PushButton4->setDown(false);
    GroupPoints->LineEdit1->setEnabled(false);
    GroupPoints->LineEdit2->setEnabled(false);
    GroupPoints->LineEdit4->setEnabled(false);

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
void TransformationGUI_RotationDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if (send == GroupPoints->LineEdit1 ||
      send == GroupPoints->LineEdit2) {
    myEditCurrentArgument = send;
    GEOMBase_Skeleton::LineEditReturnPressed();
  }
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void TransformationGUI_RotationDlg::ActivateThisDialog()
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
void TransformationGUI_RotationDlg::enterEvent (QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

void TransformationGUI_RotationDlg::TextValueChangedInSpinBox( const QString& s)
{
  bool isDigit = true;
  s.toDouble(&isDigit);
  if(!isDigit) {
    GroupPoints->CheckButton2->setChecked(false);
  }
  GroupPoints->CheckButton2->setEnabled(isDigit); 
}

//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void TransformationGUI_RotationDlg::ValueChangedInSpinBox()
{
  displayPreview();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr TransformationGUI_RotationDlg::createOperation()
{
  return getGeomEngine()->GetITransformOperations(getStudyId());
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool TransformationGUI_RotationDlg::isValid (QString& msg)
{
  switch (getConstructorId()) {
  case 0: {
    bool ok = GroupPoints->SpinBox_DX->isValid( msg, !IsPreview() );
    return myObjects.length() > 0 && !(myAxis->_is_nil()) && ok;
    break;
  }
  case 1:
    return myObjects.length() > 0 && !(myCentPoint->_is_nil() || myPoint1->_is_nil() || myPoint2->_is_nil());
    break;
  default:
    break;
  }
  return false;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool TransformationGUI_RotationDlg::execute (ObjectList& objects)
{
  bool res = false;
  bool toCreateCopy = IsPreview() || GroupPoints->CheckButton1->isChecked();

  GEOM::GEOM_Object_var anObj;

  GEOM::GEOM_ITransformOperations_var anOper = GEOM::GEOM_ITransformOperations::_narrow(getOperation());

  switch (getConstructorId()) {
  case 0:
    {
      QStringList aParameters;
      aParameters<<GroupPoints->SpinBox_DX->text();
      if (toCreateCopy) {
        for (int i = 0; i < myObjects.length(); i++) {
          myCurrObject = myObjects[i];
          anObj = anOper->RotateCopy(myObjects[i], myAxis, GetAngle() * PI180);
          if (!anObj->_is_nil()) {
            if(!IsPreview()) {
              anObj->SetParameters(aParameters.join(":").toLatin1().constData());
            }
            objects.push_back(anObj._retn());
          }
        }
      }
      else {
        for (int i = 0; i < myObjects.length(); i++) {
          myCurrObject = myObjects[i];
          anObj = anOper->Rotate(myObjects[i], myAxis, GetAngle() * PI180);
          if (!anObj->_is_nil()) {
            if(!IsPreview()) {
              anObj->SetParameters(aParameters.join(":").toLatin1().constData());
              updateAttributes(anObj, aParameters);
            }
            objects.push_back(anObj._retn());
          }
        }
      }
      res = true;
      break;
    }
  case 1:
    {
      if (toCreateCopy) {
        for (int i = 0; i < myObjects.length(); i++) {
          myCurrObject = myObjects[i];
          anObj = anOper->RotateThreePointsCopy(myObjects[i], myCentPoint, myPoint1, myPoint2);
          if (!anObj->_is_nil())
            objects.push_back(anObj._retn());
        }
      }
      else {
        for (int i = 0; i < myObjects.length(); i++) {
          myCurrObject = myObjects[i];
          anObj = anOper->RotateThreePoints(myObjects[i], myCentPoint, myPoint1, myPoint2);
          if (!anObj->_is_nil())
            objects.push_back(anObj._retn());
        }
      }
      res = true;
      break;
    }
  }

  return res;
}

//=================================================================================
// function : restoreSubShapes
// purpose  :
//=================================================================================
void TransformationGUI_RotationDlg::restoreSubShapes (SALOMEDS::Study_ptr   theStudy,
                                                      SALOMEDS::SObject_ptr theSObject)
{
  if (mainFrame()->CheckBoxRestoreSS->isChecked()) {
    // we pass here the first operation argument (object) through the list of arguments
    // because the rotation operation place its arguments in the data structure in another order,
    // and we need to point the first argument directly
    GEOM::ListOfGO_var anArgs = new GEOM::ListOfGO;
    anArgs->length(1);
    anArgs[0] = myCurrObject;
    getGeomEngine()->RestoreSubShapesSO(theStudy, theSObject, anArgs,
                                        /*theFindMethod=*/GEOM::FSM_Transformed,
                                        /*theInheritFirstArg=*/true,
                                        mainFrame()->CheckBoxAddPrefix->isChecked());
  }
}

//=================================================================================
// function : GetAngle()
// purpose  :
//=================================================================================
double TransformationGUI_RotationDlg::GetAngle() const
{
  return GroupPoints->SpinBox_DX->value();
}

//=================================================================================
// function :  CreateCopyModeChanged()
// purpose  :
//=================================================================================
void TransformationGUI_RotationDlg::CreateCopyModeChanged (bool isCreateCopy)
{
  mainFrame()->GroupBoxName->setEnabled(isCreateCopy);
}

//=================================================================================
// function :  onReverse()
// purpose  :
//=================================================================================
void TransformationGUI_RotationDlg::onReverse()
{
  double anOldValue = GroupPoints->SpinBox_DX->value();
  GroupPoints->SpinBox_DX->setValue(-anOldValue);
}

//=================================================================================
// function : addSubshapesToStudy
// purpose  : virtual method to add new SubObjects if local selection
//=================================================================================
void TransformationGUI_RotationDlg::addSubshapesToStudy()
{
  bool toCreateCopy = IsPreview() || GroupPoints->CheckButton1->isChecked();
  if (toCreateCopy) {
    QMap<QString, GEOM::GEOM_Object_var> objMap;
    switch (getConstructorId()) {
    case 0:
      objMap[GroupPoints->LineEdit2->text()] = myAxis;
      break;
    case 1:
      objMap[GroupPoints->LineEdit2->text()] = myCentPoint;
      objMap[GroupPoints->LineEdit4->text()] = myPoint1;
      objMap[GroupPoints->LineEdit5->text()] = myPoint2;
      break;
    }
    addSubshapesToFather(objMap);
  }
}
