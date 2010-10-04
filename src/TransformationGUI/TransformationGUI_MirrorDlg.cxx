//  Copyright (C) 2007-2010  CEA/DEN, EDF R&D, OPEN CASCADE
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
// File   : TransformationGUI_MirrorDlg.cxx
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.
//
#include "TransformationGUI_MirrorDlg.h"

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
// class    : TransformationGUI_MirrorDlg()
// purpose  : Constructs a TransformationGUI_MirrorDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
TransformationGUI_MirrorDlg::TransformationGUI_MirrorDlg (GeometryGUI* theGeometryGUI, QWidget* parent,
                                                          bool modal, Qt::WindowFlags fl)
  : GEOMBase_Skeleton(theGeometryGUI, parent, modal, fl),
    myInitial(true)
{
  SUIT_ResourceMgr* aResMgr = myGeomGUI->getApp()->resourceMgr();
  QPixmap image0 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_MIRROR_POINT")));
  QPixmap image1 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_MIRROR_AXE")));
  QPixmap image2 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_MIRROR_PLANE")));
  QPixmap image3 (aResMgr->loadPixmap("GEOM", tr("ICON_SELECT")));

  setWindowTitle(tr("GEOM_MIRROR_TITLE"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_MIRROR"));
  mainFrame()->RadioButton1->setIcon(image0);
  mainFrame()->RadioButton2->setIcon(image1);
  mainFrame()->RadioButton3->setIcon(image2);

  GroupPoints = new DlgRef_2Sel1Spin2Check(centralWidget());
  GroupPoints->SpinBox_DX->hide();
  GroupPoints->TextLabel3->hide();
  GroupPoints->CheckButton2->hide();
  GroupPoints->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  GroupPoints->TextLabel1->setText(tr("GEOM_OBJECTS"));
  GroupPoints->TextLabel2->setText(tr("GEOM_POINT_MIRROR"));
  GroupPoints->TextLabel2->setFixedWidth(74);
  GroupPoints->PushButton1->setIcon(image3);
  GroupPoints->PushButton2->setIcon(image3);
  GroupPoints->CheckButton1->setText(tr("GEOM_CREATE_COPY"));

  QVBoxLayout* layout = new QVBoxLayout(centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
  layout->addWidget(GroupPoints);
  /***************************************************************/

  setHelpFileName("mirror_operation_page.html");

  // Activate Create a Copy mode
  GroupPoints->CheckButton1->setChecked(true);
  CreateCopyModeChanged(true);

  Init();
}

//=================================================================================
// function : ~TransformationGUI_MirrorDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
TransformationGUI_MirrorDlg::~TransformationGUI_MirrorDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void TransformationGUI_MirrorDlg::Init()
{
  // init variables
  GroupPoints->LineEdit1->setReadOnly(true);
  GroupPoints->LineEdit2->setReadOnly(true);

  GroupPoints->LineEdit1->setText("");
  GroupPoints->LineEdit2->setText("");

  myArgument = GEOM::GEOM_Object::_nil();

  mainFrame()->GroupBoxPublish->show();

  // signals and slots connections
  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(this, SIGNAL(constructorsClicked(int)), this, SLOT(ConstructorsClicked(int)));

  connect(GroupPoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(GroupPoints->LineEdit1,   SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupPoints->LineEdit2,   SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(GroupPoints->CheckButton1, SIGNAL(toggled(bool)), this, SLOT(CreateCopyModeChanged(bool)));

  initName(tr("GEOM_MIRROR"));

  ConstructorsClicked(0);
  resize(minimumSizeHint());
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void TransformationGUI_MirrorDlg::ConstructorsClicked (int constructorId)
{
  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);

  GroupPoints->LineEdit2->clear();
  myArgument = GEOM::GEOM_Object::_nil();

  switch (constructorId) {
  case 0: // mirror an object by point
    GroupPoints->TextLabel2->setText(tr("GEOM_POINT_MIRROR"));
    break;
  case 1: // mirror an object by axe
    GroupPoints->TextLabel2->setText(tr("GEOM_AXE_MIRROR"));
    break;
  case 2: // mirror an object by plane
    GroupPoints->TextLabel2->setText(tr("GEOM_PLANE_MIRROR"));
    break;
  }

  GroupPoints->PushButton1->click();

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
void TransformationGUI_MirrorDlg::ClickOnOk()
{
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool TransformationGUI_MirrorDlg::ClickOnApply()
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
void TransformationGUI_MirrorDlg::SelectionIntoArgument()
{
  erasePreview();
  myEditCurrentArgument->setText("");

  if (myEditCurrentArgument == GroupPoints->LineEdit1)
    myObjects.length(0);
  else if (myEditCurrentArgument == GroupPoints->LineEdit2)
    myArgument = GEOM::GEOM_Object::_nil();

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  QString aName;

  if (myEditCurrentArgument == GroupPoints->LineEdit1) {
    int aNbSel = GEOMBase::GetNameOfSelectedIObjects(aSelList, aName);
    if (aNbSel < 1)
      return;

    GEOMBase::ConvertListOfIOInListOfGO(aSelList, myObjects);
    if (!myObjects.length())
      return;
    else
      myEditCurrentArgument->setText(aName);
  }
  else if (myEditCurrentArgument == GroupPoints->LineEdit2) {
    if (aSelList.Extent() != 1)
      return;

    // nbSel == 1
    Standard_Boolean testResult = Standard_False;
    myArgument = GEOMBase::ConvertIOinGEOMObject(aSelList.First(), testResult);
    if (!testResult || CORBA::is_nil(myArgument))
      return;

    aName = GEOMBase::GetName(myArgument);

    if (testResult && !myArgument->_is_nil()) {
      TopoDS_Shape aShape;
      if (GEOMBase::GetShape(myArgument, aShape, TopAbs_SHAPE) && !aShape.IsNull()) {
        TopAbs_ShapeEnum aNeedType = TopAbs_VERTEX;
        if (getConstructorId() == 1)
          aNeedType = TopAbs_EDGE;
        else if (getConstructorId() == 2)
          aNeedType = TopAbs_FACE;

        TColStd_IndexedMapOfInteger aMap;
        aSelMgr->GetIndexes(aSelList.First(), aMap);
        if (aMap.Extent() == 1) {
          int anIndex = aMap(1);
          if (aNeedType == TopAbs_VERTEX)
            aName += QString(":vertex_%1").arg(anIndex);
          else
            aName += QString(":edge_%1").arg(anIndex);

          //Find SubShape Object in Father
          GEOM::GEOM_Object_var aFindedObject = findObjectInFather(myArgument, aName);

          if (aFindedObject->_is_nil()) { // Object not found in study
            GEOM::GEOM_IShapesOperations_var aShapesOp =
              getGeomEngine()->GetIShapesOperations(getStudyId());
            myArgument = aShapesOp->GetSubShape(myArgument, anIndex);
          }
          else {
            myArgument = aFindedObject; // get Object from study
          }
        }
        else {
          if (aShape.ShapeType() != aNeedType) {
            myArgument = GEOM::GEOM_Object::_nil();
            aName = "";
          }
        }
      }
    }
    myEditCurrentArgument->setText(aName);

    if (!myArgument->_is_nil() && !myObjects.length())
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
void TransformationGUI_MirrorDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
  globalSelection();

  if (send == GroupPoints->PushButton1) {
    myEditCurrentArgument = GroupPoints->LineEdit1;

    GroupPoints->PushButton2->setDown(false);
    GroupPoints->LineEdit2->setEnabled(false);
  }
  else if (send == GroupPoints->PushButton2) {
    myEditCurrentArgument = GroupPoints->LineEdit2;

    switch (getConstructorId()) {
    case 0:
      localSelection(GEOM::GEOM_Object::_nil(), TopAbs_VERTEX);
      break;
    case 1:
      localSelection(GEOM::GEOM_Object::_nil(), TopAbs_EDGE);
      break;
    case 2:
      globalSelection(GEOM_PLANE);
      break;
    }

    GroupPoints->PushButton1->setDown(false);
    GroupPoints->LineEdit1->setEnabled(false);
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
void TransformationGUI_MirrorDlg::LineEditReturnPressed()
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
void TransformationGUI_MirrorDlg::ActivateThisDialog()
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
void TransformationGUI_MirrorDlg::enterEvent (QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr  TransformationGUI_MirrorDlg::createOperation()
{
  return getGeomEngine()->GetITransformOperations(getStudyId());
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool TransformationGUI_MirrorDlg::isValid (QString& /*msg*/)
{
  return !(myObjects.length() == 0 || myArgument->_is_nil());
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool  TransformationGUI_MirrorDlg::execute (ObjectList& objects)
{
  bool res = false;
  bool toCreateCopy = IsPreview() || GroupPoints->CheckButton1->isChecked();

  GEOM::GEOM_Object_var anObj;

  GEOM::GEOM_ITransformOperations_var anOper = GEOM::GEOM_ITransformOperations::_narrow(getOperation());

  switch (getConstructorId()) {
  case 0:
    {
      if (toCreateCopy) {
        for (int i = 0; i < myObjects.length(); i++) {
          anObj = anOper->MirrorPointCopy(myObjects[i], myArgument);
          if (!anObj->_is_nil())
            objects.push_back(anObj._retn());
        }
      }
      else {
        for (int i = 0; i < myObjects.length(); i++) {
          anObj = anOper->MirrorPoint(myObjects[i], myArgument);
          if (!anObj->_is_nil())
            objects.push_back(anObj._retn());
        }
      }
      res = true;
      break;
    }
  case 1:
    {
      if (toCreateCopy) {
        for (int i = 0; i < myObjects.length(); i++) {
          anObj = anOper->MirrorAxisCopy(myObjects[i], myArgument);
          if (!anObj->_is_nil())
            objects.push_back(anObj._retn());
        }
      }
      else {
        for (int i = 0; i < myObjects.length(); i++) {
          anObj = anOper->MirrorAxis(myObjects[i], myArgument);
          if (!anObj->_is_nil())
            objects.push_back(anObj._retn());
        }
      }
      res = true;
      break;
    }
  case 2:
    {
      if (toCreateCopy) {
        for (int i = 0; i < myObjects.length(); i++) {
            anObj = anOper->MirrorPlaneCopy(myObjects[i], myArgument);
            if (!anObj->_is_nil())
              objects.push_back(anObj._retn());
        }
      }
      else {
        for (int i = 0; i < myObjects.length(); i++) {
          anObj = anOper->MirrorPlane(myObjects[i], myArgument);
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
void TransformationGUI_MirrorDlg::restoreSubShapes (SALOMEDS::Study_ptr   theStudy,
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
void TransformationGUI_MirrorDlg::CreateCopyModeChanged (bool isCreateCopy)
{
  mainFrame()->GroupBoxName->setEnabled(isCreateCopy);
}

//=================================================================================
// function : addSubshapeToStudy
// purpose  : virtual method to add new SubObjects if local selection
//=================================================================================
void TransformationGUI_MirrorDlg::addSubshapesToStudy()
{
  bool toCreateCopy = IsPreview() || GroupPoints->CheckButton1->isChecked();
  if (toCreateCopy) {
    QMap<QString, GEOM::GEOM_Object_var> objMap;

    switch (getConstructorId()) {
    case 0:
      objMap[GroupPoints->LineEdit2->text()] = myArgument;
      break;
    case 1:
      objMap[GroupPoints->LineEdit2->text()] = myArgument;
      break;
    case 2:
      return;
    }
    addSubshapesToFather(objMap);
  }
}
