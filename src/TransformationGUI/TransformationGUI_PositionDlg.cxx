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
// File   : TransformationGUI_PositionDlg.cxx
// Author : Damien COQUERET, Open CASCADE S.A.S.
//
#include "TransformationGUI_PositionDlg.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>
#include <TColStd_IndexedMapOfInteger.hxx>

// OCCT Includes
#include <TColStd_MapOfInteger.hxx>

#include <GEOMImpl_Types.hxx>

//=================================================================================
// class    : TransformationGUI_PositionDlg()
// purpose  : Constructs a TransformationGUI_PositionDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
TransformationGUI_PositionDlg::TransformationGUI_PositionDlg
(GeometryGUI* theGeometryGUI, QWidget* parent, bool modal, Qt::WindowFlags fl)
  : GEOMBase_Skeleton(theGeometryGUI, parent, modal, fl),
    myInitial(true)
{
  SUIT_ResourceMgr* aResMgr = myGeomGUI->getApp()->resourceMgr();
  QPixmap image0 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_POSITION")));
  QPixmap image1 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_POSITION2")));
  QPixmap image2 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_POSITION3")));
  QPixmap imageselect (aResMgr->loadPixmap("GEOM", tr("ICON_SELECT")));

  setWindowTitle(tr("GEOM_POSITION_TITLE"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_POSITION"));
  mainFrame()->RadioButton1->setIcon(image0);
  mainFrame()->RadioButton2->setIcon(image1);
  mainFrame()->RadioButton3->setIcon(image2);

  Group1 = new DlgRef_4Sel1Spin3Check(centralWidget());
  Group1->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  Group1->TextLabel1->setText(tr("GEOM_OBJECTS"));
  Group1->TextLabel2->setText(tr("GEOM_START_LCS"));
  Group1->TextLabel3->setText(tr("GEOM_DISTANCE"));
  Group1->TextLabel4->setText(tr("GEOM_END_LCS"));
  Group1->TextLabel5->setText(tr("GEOM_PATH_OBJECT"));
  Group1->PushButton1->setIcon(imageselect);
  Group1->PushButton2->setIcon(imageselect);
  Group1->PushButton4->setIcon(imageselect);
  Group1->PushButton5->setIcon(imageselect);
  Group1->CheckButton1->setText(tr("GEOM_CREATE_COPY"));
  Group1->CheckButton2->setText(tr("SELECT_UNPUBLISHED_EDGES"));
  Group1->CheckButton3->setText(tr("GEOM_REVERSE_DIRECTION"));

  QVBoxLayout* layout = new QVBoxLayout(centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
  layout->addWidget(Group1);
  /***************************************************************/

  setHelpFileName("modify_location_operation_page.html");

  // Activate Create a Copy mode
  Group1->CheckButton1->setChecked(true);
  Group1->CheckButton1->setChecked(false);
  CreateCopyModeChanged(true);

  Init();
}

//=================================================================================
// function : ~TransformationGUI_PositionDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
TransformationGUI_PositionDlg::~TransformationGUI_PositionDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void TransformationGUI_PositionDlg::Init()
{
  // init variables
  Group1->LineEdit1->setReadOnly(true);
  Group1->LineEdit2->setReadOnly(true);
  Group1->LineEdit4->setReadOnly(true);
  Group1->LineEdit5->setReadOnly(true);

  Group1->LineEdit1->setText("");
  Group1->LineEdit2->setText("");
  Group1->LineEdit4->setText("");
  Group1->LineEdit5->setText("");

  initSpinBox(Group1->SpinBox_DX, 0, 1, 0.05, "parametric_precision" );
  Group1->SpinBox_DX->setValue(0);

  Group1->CheckButton2->setEnabled(false);

  myStartLCS = myEndLCS = GEOM::GEOM_Object::_nil();

  mainFrame()->GroupBoxPublish->show();

  // signals and slots connections
  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(this, SIGNAL(constructorsClicked(int)), this, SLOT(ConstructorsClicked(int)));

  connect(Group1->PushButton1, SIGNAL(clicked()),       this, SLOT(SetEditCurrentArgument()));
  connect(Group1->PushButton2, SIGNAL(clicked()),       this, SLOT(SetEditCurrentArgument()));
  connect(Group1->PushButton4, SIGNAL(clicked()),       this, SLOT(SetEditCurrentArgument()));
  connect(Group1->PushButton5, SIGNAL(clicked()),       this, SLOT(SetEditCurrentArgument()));

  connect(Group1->LineEdit1,   SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(Group1->LineEdit2,   SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(Group1->LineEdit4,   SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(Group1->LineEdit5,   SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(Group1->CheckButton1,   SIGNAL(toggled(bool)), this, SLOT(CreateCopyModeChanged(bool)));
  connect(Group1->SpinBox_DX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox()));

  connect(Group1->CheckButton2,   SIGNAL(toggled(bool)), this, SLOT(SelectionTypeButtonClicked()));
  connect(Group1->CheckButton3,   SIGNAL(toggled(bool)), this, SLOT(ValueChangedInSpinBox()));

  initName(tr("GEOM_POSITION"));

  ConstructorsClicked(0);
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void TransformationGUI_PositionDlg::ConstructorsClicked (int constructorId)
{
  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);

  Group1->LineEdit2->clear();
  Group1->LineEdit4->clear();
  Group1->LineEdit5->clear();
  myStartLCS = GEOM::GEOM_Object::_nil();
  myEndLCS = GEOM::GEOM_Object::_nil();
  myPath = GEOM::GEOM_Object::_nil();

  switch (constructorId) {
  case 0:
    {
      Group1->LineEdit2->hide();
      Group1->TextLabel2->hide();
      Group1->PushButton2->hide();

      Group1->LineEdit5->hide();
      Group1->TextLabel5->hide();
      Group1->PushButton5->hide();

      Group1->SpinBox_DX->hide();
      Group1->TextLabel3->hide();

      Group1->LineEdit4->show();
      Group1->TextLabel4->show();
      Group1->PushButton4->show();

      Group1->PushButton1->click();

      Group1->CheckButton2->hide();
      Group1->CheckButton3->hide();
    }
    break;
  case 1:
    {
      Group1->LineEdit2->show();
      Group1->TextLabel2->show();
      Group1->PushButton2->show();

      Group1->LineEdit5->hide();
      Group1->TextLabel5->hide();
      Group1->PushButton5->hide();

      Group1->SpinBox_DX->hide();
      Group1->TextLabel3->hide();

      Group1->PushButton1->click();

      Group1->CheckButton2->hide();
      Group1->CheckButton3->hide();
    }
    break;
  case 2:
    {
      Group1->LineEdit4->hide();
      Group1->TextLabel4->hide();
      Group1->PushButton4->hide();

      Group1->LineEdit2->hide();
      Group1->TextLabel2->hide();
      Group1->PushButton2->hide();

      Group1->LineEdit5->show();
      Group1->TextLabel5->show();
      Group1->PushButton5->show();

      Group1->SpinBox_DX->show();
      Group1->TextLabel3->show();

      Group1->PushButton1->click();

      Group1->CheckButton2->show();
      Group1->CheckButton3->show();
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
// function : SelectionBittonClicked()
// purpose  : Selection type Radio button management
//=================================================================================
void TransformationGUI_PositionDlg::SelectionTypeButtonClicked()
{
  if ( Group1->CheckButton2->isChecked() ) {
    localSelection( GEOM::GEOM_Object::_nil(), TopAbs_EDGE );
  } else {
    TColStd_MapOfInteger aMap;
    aMap.Add(GEOM_WIRE);
    aMap.Add(GEOM_LINE);
    globalSelection(aMap);
  }
}

//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void TransformationGUI_PositionDlg::ValueChangedInSpinBox()
{
  displayPreview();
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void TransformationGUI_PositionDlg::ClickOnOk()
{
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool TransformationGUI_PositionDlg::ClickOnApply()
{
  if (!onAccept(Group1->CheckButton1->isChecked()))
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
void TransformationGUI_PositionDlg::SelectionIntoArgument()
{
  erasePreview();
  myEditCurrentArgument->setText("");

  if (myEditCurrentArgument == Group1->LineEdit1)
    myObjects.length(0);
  else if (myEditCurrentArgument == Group1->LineEdit2)
    myStartLCS = GEOM::GEOM_Object::_nil();
  else if (myEditCurrentArgument == Group1->LineEdit4)
    myEndLCS = GEOM::GEOM_Object::_nil();
  else if (myEditCurrentArgument == Group1->LineEdit5)
    myPath = GEOM::GEOM_Object::_nil();

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  QString aName;

  if (myEditCurrentArgument == Group1->LineEdit1) {
    int aNbSel = GEOMBase::GetNameOfSelectedIObjects(aSelList, aName);
    if (aNbSel < 1)
      return;

    GEOMBase::ConvertListOfIOInListOfGO(aSelList, myObjects);
    if (!myObjects.length())
      return;
    else
      myEditCurrentArgument->setText(aName);

    if (getConstructorId() == 2)
      Group1->PushButton5->click();
  }
  else if (myEditCurrentArgument == Group1->LineEdit2) {
    if (aSelList.Extent() != 1)
      return;

    // nbSel == 1
    Standard_Boolean testResult = Standard_False;
    myStartLCS = GEOMBase::ConvertIOinGEOMObject(aSelList.First(), testResult);
    if (!testResult || CORBA::is_nil(myStartLCS))
      return;

    aName = GEOMBase::GetName(myStartLCS);
    myEditCurrentArgument->setText(aName);

    if (!myStartLCS->_is_nil() && myEndLCS->_is_nil())
      Group1->PushButton4->click();
  }
  else if (myEditCurrentArgument == Group1->LineEdit4) {
    myEndLCS = GEOM::GEOM_Object::_nil();
    if (aSelList.Extent() != 1)
      return;

    Standard_Boolean testResult = Standard_False;
    myEndLCS = GEOMBase::ConvertIOinGEOMObject(aSelList.First(), testResult);
    if (!testResult || CORBA::is_nil(myEndLCS))
      return;

    aName = GEOMBase::GetName(myEndLCS);
    myEditCurrentArgument->setText(aName);

    if (!myEndLCS->_is_nil() && !myObjects.length())
      Group1->PushButton1->click();
  }
  else if (myEditCurrentArgument == Group1->LineEdit5) {
    myPath = GEOM::GEOM_Object::_nil();
    if (aSelList.Extent() != 1)
      return;

    Standard_Boolean testResult = Standard_False;
    GEOM::GEOM_Object_ptr aSelectedObject = GEOMBase::ConvertIOinGEOMObject(aSelList.First(), testResult);
    if (!testResult || aSelectedObject->_is_nil())
      return;

    aName = GEOMBase::GetName(myPath);
    // Local Selection
    TopoDS_Shape S;
    if (!GEOMBase::GetShape(aSelectedObject, S))
      return;
    
    TColStd_IndexedMapOfInteger aMap;
    aSelMgr->GetIndexes(aSelList.First(), aMap);
    aName = GEOMBase::GetName(aSelectedObject);
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
      else { // get Object from study
        aSelectedObject = aFindedObject;
      }
    }
    else {
      if (S.ShapeType() != TopAbs_EDGE && S.ShapeType() != TopAbs_WIRE) {
        aSelectedObject = GEOM::GEOM_Object::_nil();
        aName = "";
        return;
      }
    }
    
    myEditCurrentArgument->setText(aName);
    myPath = aSelectedObject;

    if (!myPath->_is_nil() && !myObjects.length())
      Group1->PushButton1->click();
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
void TransformationGUI_PositionDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
  if (send == Group1->PushButton1) {
    myEditCurrentArgument = Group1->LineEdit1;

    globalSelection();

    Group1->PushButton2->setDown(false);
    Group1->PushButton4->setDown(false);
    Group1->PushButton5->setDown(false);
    Group1->LineEdit2->setEnabled(false);
    Group1->LineEdit4->setEnabled(false);
    Group1->LineEdit5->setEnabled(false);

    Group1->CheckButton2->setEnabled(false);
  }
  else if (send == Group1->PushButton2) {
    myEditCurrentArgument = Group1->LineEdit2;

    TColStd_MapOfInteger aMap;
    aMap.Add(GEOM_PLANE);
    aMap.Add(GEOM_MARKER);
    globalSelection(aMap);

    Group1->PushButton1->setDown(false);
    Group1->PushButton4->setDown(false);
    Group1->PushButton5->setDown(false);
    Group1->LineEdit1->setEnabled(false);
    Group1->LineEdit4->setEnabled(false);
    Group1->LineEdit5->setEnabled(false);

    Group1->CheckButton2->setEnabled(false);
  }
  else if (send == Group1->PushButton4) {
    myEditCurrentArgument = Group1->LineEdit4;

    TColStd_MapOfInteger aMap;
    aMap.Add(GEOM_PLANE);
    aMap.Add(GEOM_MARKER);
    globalSelection(aMap);

    Group1->PushButton1->setDown(false);
    Group1->PushButton2->setDown(false);
    Group1->PushButton5->setDown(false);
    Group1->LineEdit1->setEnabled(false);
    Group1->LineEdit2->setEnabled(false);
    Group1->LineEdit5->setEnabled(false);

    Group1->CheckButton2->setEnabled(false);
  }
  else if (send == Group1->PushButton5) {
    myEditCurrentArgument = Group1->LineEdit5;

    Group1->CheckButton2->setEnabled(true);

    if ( Group1->CheckButton2->isChecked() ) {
      localSelection( GEOM::GEOM_Object::_nil(), TopAbs_EDGE );
    } else {
      TColStd_MapOfInteger aMap;
      aMap.Add(GEOM_WIRE);
      aMap.Add(GEOM_LINE);
      globalSelection(aMap);
    }

    Group1->PushButton1->setDown(false);
    Group1->PushButton2->setDown(false);
    Group1->PushButton4->setDown(false);
    Group1->LineEdit1->setEnabled(false);
    Group1->LineEdit2->setEnabled(false);
    Group1->LineEdit4->setEnabled(false);
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
void TransformationGUI_PositionDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if (send == Group1->LineEdit1 ||
      send == Group1->LineEdit2 ||
      send == Group1->LineEdit4 ||
      send == Group1->LineEdit5 ) {
    myEditCurrentArgument = send;
    GEOMBase_Skeleton::LineEditReturnPressed();
  }

  if (send == Group1->LineEdit5)
    Group1->CheckButton2->setEnabled(true);
  else
    Group1->CheckButton2->setEnabled(false);
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void TransformationGUI_PositionDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
           this, SLOT( SelectionIntoArgument() ) );

  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
           this, SLOT( SelectionIntoArgument() ) );

  ConstructorsClicked( getConstructorId() );
}

//=================================================================================
// function : enterEvent()
// purpose  : when mouse enter onto the QWidget
//=================================================================================
void TransformationGUI_PositionDlg::enterEvent (QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr TransformationGUI_PositionDlg::createOperation()
{
  return getGeomEngine()->GetITransformOperations(getStudyId());
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool TransformationGUI_PositionDlg::isValid (QString& /*msg*/)
{
  bool res;
  if (getConstructorId() == 0)
    res = !(myObjects.length() == 0 || myEndLCS->_is_nil());
  else if ( getConstructorId() == 1 )
    res = !(myObjects.length() == 0 || myStartLCS->_is_nil() || myEndLCS->_is_nil());
  else if ( getConstructorId() == 2 )
    res = !(myObjects.length() == 0 || myPath->_is_nil());

  return res;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool TransformationGUI_PositionDlg::execute (ObjectList& objects)
{
  bool res = false;
  bool toCreateCopy = IsPreview() || Group1->CheckButton1->isChecked();
  GEOM::GEOM_Object_var anObj;

  GEOM::GEOM_ITransformOperations_var anOper = GEOM::GEOM_ITransformOperations::_narrow(getOperation());

  switch (getConstructorId()) {
  case 0:
    {
      for (int i = 0; i < myObjects.length(); i++) {
        anObj = toCreateCopy ? 
          anOper->PositionShapeCopy(myObjects[i], myObjects[i], myEndLCS) :
          anOper->PositionShape(myObjects[i], myObjects[i], myEndLCS);

        if (!anObj->_is_nil())
          objects.push_back(anObj._retn());
      }
      res = true;
      break;
    }
  case 1:
    {
      for (int i = 0; i < myObjects.length(); i++) {
        if (toCreateCopy)
          anObj = toCreateCopy ? 
            anOper->PositionShapeCopy(myObjects[i], myStartLCS, myEndLCS) :
            anOper->PositionShape(myObjects[i], myStartLCS, myEndLCS);
        if (!anObj->_is_nil())
          objects.push_back(anObj._retn());
      }
      res = true;
      break;
    }
  case 2:
    {
      double aDistance = Group1->SpinBox_DX->value();
      bool toReverse = Group1->CheckButton3->isChecked();
      for (int i = 0; i < myObjects.length(); i++) {
        anObj = anOper->PositionAlongPath(myObjects[i], myPath, aDistance, toCreateCopy, toReverse);
        if (!anObj->_is_nil())
          objects.push_back(anObj._retn());
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
void TransformationGUI_PositionDlg::restoreSubShapes (SALOMEDS::Study_ptr   theStudy,
                                                      SALOMEDS::SObject_ptr theSObject)
{
  if (mainFrame()->CheckBoxRestoreSS->isChecked()) {
    // empty list of arguments means that all arguments should be restored
    getGeomEngine()->RestoreSubShapesSO(theStudy, theSObject, GEOM::ListOfGO(),
                                        /*theFindMethod=*/GEOM::FSM_Transformed,
                                        /*theInheritFirstArg=*/true,
                                        mainFrame()->CheckBoxAddPrefix->isChecked());
  }
}

//=================================================================================
// function :  CreateCopyModeChanged()
// purpose  :
//=================================================================================
void TransformationGUI_PositionDlg::CreateCopyModeChanged (bool isCreateCopy)
{
  mainFrame()->GroupBoxName->setEnabled(isCreateCopy);
}

//=================================================================================
// function : addSubshapeToStudy
// purpose  : virtual method to add new SubObjects if local selection
//=================================================================================
void TransformationGUI_PositionDlg::addSubshapesToStudy()
{
  QMap<QString, GEOM::GEOM_Object_var> objMap;

  objMap[Group1->LineEdit5->text()] = myPath;

  addSubshapesToFather(objMap);
}
