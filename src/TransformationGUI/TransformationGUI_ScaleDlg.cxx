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
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
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
//  File   : TransformationGUI_ScaleDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#include "TransformationGUI_ScaleDlg.h"

#include "SUIT_Desktop.h"
#include "SUIT_Session.h"
#include "SalomeApp_Application.h"
#include "LightApp_SelectionMgr.h"

#include <TopoDS_Shape.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#include "DlgRef_SpinBox.h"

#include <qlabel.h>
#include <qcheckbox.h>

#include "GEOMImpl_Types.hxx"

#include "utilities.h"

using namespace std;

//=================================================================================
// class    : TransformationGUI_ScaleDlg()
// purpose  : Constructs a TransformationGUI_ScaleDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
TransformationGUI_ScaleDlg::TransformationGUI_ScaleDlg (GeometryGUI* theGeometryGUI, QWidget* parent,
                                                        const char* name, bool modal, WFlags fl)
  : GEOMBase_Skeleton(theGeometryGUI, parent, name, modal, WStyle_Customize |
                      WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  SUIT_ResourceMgr* aResMgr = myGeomGUI->getApp()->resourceMgr();
  QPixmap image1 (aResMgr->loadPixmap("GEOM",tr("ICON_DLG_SCALE")));
  QPixmap image2 (aResMgr->loadPixmap("GEOM",tr("ICON_DLG_SCALE_ALONG_AXES")));
  QPixmap image3 (aResMgr->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_SCALE_TITLE"));

  // Constructors
  GroupConstructors->setTitle(tr("GEOM_SCALE"));
  RadioButton1->setPixmap(image1);
  RadioButton2->setPixmap(image2);
  RadioButton3->close(TRUE);

  // Own widgets
  GroupBox1 = new QGroupBox (this, "GroupBox1");
  GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  GroupBox1->setColumnLayout(0, Qt::Vertical);
  GroupBox1->layout()->setSpacing(6);
  GroupBox1->layout()->setMargin(11);

  TextLabel1 = new QLabel (GroupBox1, "TextLabel1");
  TextLabel1->setSizePolicy(QSizePolicy((QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0,
                                        0, 0, TextLabel1->sizePolicy().hasHeightForWidth()));

  TextLabel2 = new QLabel (GroupBox1, "TextLabel2");
  TextLabel2->setSizePolicy(QSizePolicy((QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0,
                                        0, 0, TextLabel2->sizePolicy().hasHeightForWidth()));

  PushButton1 = new QPushButton(GroupBox1, "PushButton1");
  PushButton1->setSizePolicy(QSizePolicy((QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0,
                                         0, 0, PushButton1->sizePolicy().hasHeightForWidth()));

  PushButton2 = new QPushButton( GroupBox1, "PushButton2" );
  PushButton2->setSizePolicy(QSizePolicy((QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0,
                                         0, 0, PushButton2->sizePolicy().hasHeightForWidth()));

  LineEdit1 = new QLineEdit (GroupBox1, "LineEdit1");
  LineEdit2 = new QLineEdit (GroupBox1, "LineEdit2");

  TextLabel3 = new QLabel (GroupBox1, "TextLabel3");
  TextLabel3->setSizePolicy(QSizePolicy((QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0,
                                        0, 0, TextLabel3->sizePolicy().hasHeightForWidth()));

  TextLabel4 = new QLabel (GroupBox1, "TextLabel4");
  TextLabel4->setSizePolicy(QSizePolicy((QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0,
                                        0, 0, TextLabel3->sizePolicy().hasHeightForWidth()));

  TextLabel5 = new QLabel (GroupBox1, "TextLabel5");
  TextLabel5->setSizePolicy(QSizePolicy((QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0,
                                        0, 0, TextLabel3->sizePolicy().hasHeightForWidth()));

  SpinBox_FX = new DlgRef_SpinBox(GroupBox1, "SpinBox_FX");
  SpinBox_FY = new DlgRef_SpinBox(GroupBox1, "SpinBox_FY");
  SpinBox_FZ = new DlgRef_SpinBox(GroupBox1, "SpinBox_FZ");

  CheckBoxCopy = new QCheckBox (GroupBox1, "CheckBoxCopy");

  QSpacerItem* spacer = new QSpacerItem(0, 316, QSizePolicy::Minimum, QSizePolicy::Expanding);

  // Layouting
  OwnLayout = new QGridLayout (0, 1, 1, 0, 6, "Layout1");

  OwnLayout->addWidget(TextLabel1, 0, 0);
  OwnLayout->addWidget(TextLabel2, 1, 0);

  OwnLayout->addWidget(PushButton1, 0, 1);
  OwnLayout->addWidget(PushButton2, 1, 1);

  OwnLayout->addMultiCellWidget(LineEdit1, 0, 0, 2, 3);
  OwnLayout->addMultiCellWidget(LineEdit2, 1, 1, 2, 3);

  OwnLayout->addWidget(TextLabel3, 2, 0);
  OwnLayout->addWidget(TextLabel4, 3, 0);
  OwnLayout->addWidget(TextLabel5, 4, 0);

  OwnLayout->addWidget(SpinBox_FX, 2, 2);
  OwnLayout->addWidget(SpinBox_FY, 3, 2);
  OwnLayout->addWidget(SpinBox_FZ, 4, 2);

  OwnLayout->addMultiCellWidget(CheckBoxCopy, 5, 5, 0, 3);

  OwnLayout->addItem(spacer, 6, 2);

  GroupBox1Layout = new QGridLayout (GroupBox1->layout());
  GroupBox1Layout->setAlignment(Qt::AlignTop);
  GroupBox1Layout->addLayout(OwnLayout, 0, 0);

  Layout1->addWidget(GroupBox1, 2, 0);

  // Text and pictures for own widgets
  TextLabel1->setText(tr("GEOM_OBJECTS"));
  TextLabel2->setText(tr("GEOM_CENTRAL_POINT"));

  PushButton1->setPixmap(image3);
  PushButton2->setPixmap(image3);

  LineEdit1->setReadOnly( true );
  LineEdit2->setReadOnly( true );

  TextLabel3->setText(tr("GEOM_SCALE_FACTOR"));
  TextLabel4->setText(tr("GEOM_SCALE_FACTOR_Y"));
  TextLabel5->setText(tr("GEOM_SCALE_FACTOR_Z"));

  // Min, max, step and decimals for spin boxes & initial values
  double aFactor = 2.0;
  double SpecificStep = 0.5;
  SpinBox_FX->RangeStepAndValidator(COORD_MIN, COORD_MAX, SpecificStep, DBL_DIGITS_DISPLAY);
  SpinBox_FY->RangeStepAndValidator(COORD_MIN, COORD_MAX, SpecificStep, DBL_DIGITS_DISPLAY);
  SpinBox_FZ->RangeStepAndValidator(COORD_MIN, COORD_MAX, SpecificStep, DBL_DIGITS_DISPLAY);
  SpinBox_FX->SetValue(aFactor);
  SpinBox_FY->SetValue(aFactor);
  SpinBox_FZ->SetValue(aFactor);

  // Tab order
  setTabOrder(PushButton1, LineEdit1);
  setTabOrder(LineEdit1, PushButton2);
  setTabOrder(PushButton2, LineEdit2);
  setTabOrder(LineEdit2 , SpinBox_FX);
  setTabOrder(SpinBox_FX, SpinBox_FY);
  setTabOrder(SpinBox_FY, SpinBox_FZ);
  setTabOrder(SpinBox_FZ, CheckBoxCopy);
  setTabOrder(CheckBoxCopy, CheckBoxRestoreSS);

  // Modification of an existing object by offset is not allowed
  CheckBoxCopy->hide();

  // Activate Create a Copy mode
  CheckBoxCopy->setText(tr("GEOM_CREATE_COPY"));
  CheckBoxCopy->setChecked(true);
  CreateCopyModeChanged(true);

  // Allowed inheritance of children and visual properties by the scaling result
  GroupBoxPublish->show();

  // Signals and slots connections
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));
  connect(GroupConstructors, SIGNAL(clicked(int)), SLOT(ConstructorsClicked(int)));

  connect(PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(LineEdit2, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(SpinBox_FX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox()));
  connect(SpinBox_FY, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox()));
  connect(SpinBox_FZ, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox()));

  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), SpinBox_FX, SLOT(SetStep(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), SpinBox_FY, SLOT(SetStep(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), SpinBox_FZ, SLOT(SetStep(double)));

  connect(CheckBoxCopy, SIGNAL(toggled(bool)), this, SLOT(CreateCopyModeChanged(bool)));

  connect(myGeomGUI->getApp()->selectionMgr(),
          SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument())) ;

  setHelpFileName("scale_operation_page.html");

  Init();
}

//=================================================================================
// function : ~TransformationGUI_ScaleDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
TransformationGUI_ScaleDlg::~TransformationGUI_ScaleDlg()
{
  /* no need to delete child widgets, Qt does it all for us */
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void TransformationGUI_ScaleDlg::Init()
{
  myEditCurrentArgument = LineEdit1;
  LineEdit2->clear();

  myPoint = GEOM::GEOM_Object::_nil();

  initName(tr("GEOM_SCALE"));

  ConstructorsClicked(0);
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void TransformationGUI_ScaleDlg::ConstructorsClicked(int constructorId)
{
  erasePreview();

  switch (constructorId)
  {
  case 0: /* translation an object by dx, dy, dz */
    TextLabel3->setText(tr("GEOM_SCALE_FACTOR"));
    TextLabel4->hide();
    TextLabel5->hide();
    SpinBox_FY->hide();
    SpinBox_FZ->hide();
    resize(0,0);
    break;
  case 1: /* translation an object by 2 points */
    TextLabel3->setText(tr("GEOM_SCALE_FACTOR_X"));
    TextLabel4->show();
    TextLabel5->show();
    SpinBox_FY->show();
    SpinBox_FZ->show();
    resize(0,0);
    break;
  default:
    break;
  }

  //qApp->processEvents();
  //updateGeometry();
  //resize(minimumSize());
  displayPreview();
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void TransformationGUI_ScaleDlg::ClickOnOk()
{
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool TransformationGUI_ScaleDlg::ClickOnApply()
{
  if (!onAccept(CheckBoxCopy->isChecked()))
    return false;

  //Init();
  initName(tr("GEOM_SCALE"));
  return true;
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void TransformationGUI_ScaleDlg::SelectionIntoArgument()
{
  erasePreview();
  myEditCurrentArgument->setText("");
  QString aName;

  if (myEditCurrentArgument == LineEdit1)
  {
    int aNbSel = GEOMBase::GetNameOfSelectedIObjects(selectedIO(), aName);
    if (aNbSel < 1)
    {
      myObjects.length(0);
      return;
    }
    GEOMBase::ConvertListOfIOInListOfGO(selectedIO(), myObjects);
    if (!myObjects.length())
      return;
  }
  else if (myEditCurrentArgument == LineEdit2)
  {
    GEOM::GEOM_Object_var aSelectedObject = GEOM::GEOM_Object::_nil();
    if (IObjectCount() == 1)
    {
      Standard_Boolean testResult = Standard_False;
      aSelectedObject = GEOMBase::ConvertIOinGEOMObject(firstIObject(), testResult);
      if (testResult)
      {
        aName = GEOMBase::GetName(aSelectedObject);

        TopoDS_Shape aShape;
        if (GEOMBase::GetShape(aSelectedObject, aShape, TopAbs_SHAPE) && !aShape.IsNull())
        {
          LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
          TColStd_IndexedMapOfInteger aMap;
          aSelMgr->GetIndexes(firstIObject(), aMap);
          if (aMap.Extent() == 1)
          {
            int anIndex = aMap( 1 );
            aName += QString(":vertex_%1").arg(anIndex);

            //Find SubShape Object in Father
            GEOM::GEOM_Object_var aFindedObject = findObjectInFather(aSelectedObject, aName);

            if ( aFindedObject == GEOM::GEOM_Object::_nil() ) { // Object not found in study
              GEOM::GEOM_IShapesOperations_var aShapesOp =
                getGeomEngine()->GetIShapesOperations( getStudyId() );
              aSelectedObject = aShapesOp->GetSubShape(aSelectedObject, anIndex);
              aSelMgr->clearSelected();
            }
            else
              aSelectedObject = aFindedObject; // get Object from study
          }
          else
          {
            if (aShape.ShapeType() != TopAbs_VERTEX) {
              aSelectedObject = GEOM::GEOM_Object::_nil();
              aName = "";
            }
          }
        }
      }
    }

    myPoint = aSelectedObject;
  }

  myEditCurrentArgument->setText(aName);
  displayPreview();
}

//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void TransformationGUI_ScaleDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if (send == LineEdit1 || send == LineEdit2)
  {
    myEditCurrentArgument = send;
    GEOMBase_Skeleton::LineEditReturnPressed();
  }
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void TransformationGUI_ScaleDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();
  globalSelection();

  if (send == PushButton1) {
    myEditCurrentArgument = LineEdit1;
  }
  else if (send == PushButton2) {
    myEditCurrentArgument = LineEdit2;
    localSelection(GEOM::GEOM_Object::_nil(), TopAbs_VERTEX);
  }

  myEditCurrentArgument->setFocus();
  SelectionIntoArgument();
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void TransformationGUI_ScaleDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));

  globalSelection();
  myEditCurrentArgument->setFocus();
  if (myEditCurrentArgument == LineEdit2) {
    localSelection(GEOM::GEOM_Object::_nil(), TopAbs_VERTEX);
  }
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void TransformationGUI_ScaleDlg::enterEvent (QEvent* e)
{
  if (!GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void TransformationGUI_ScaleDlg::ValueChangedInSpinBox()
{
  displayPreview();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr TransformationGUI_ScaleDlg::createOperation()
{
  return myGeomGUI->GetGeomGen()->GetITransformOperations(getStudyId());
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool TransformationGUI_ScaleDlg::isValid (QString& msg)
{
  if (myObjects.length() > 0 && fabs(SpinBox_FX->GetValue()) > 0.00001)
  {
    // && !myPoint->_is_nil()
    if (getConstructorId() == 0)
      return true;
    if (fabs(SpinBox_FY->GetValue()) > 0.00001 &&
        fabs(SpinBox_FZ->GetValue()) > 0.00001)
      return true;
  }
  return false;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool TransformationGUI_ScaleDlg::execute (ObjectList& objects)
{
  bool res = false;
  bool toCreateCopy = IsPreview() || CheckBoxCopy->isChecked();

  GEOM::GEOM_Object_var anObj;

  switch (getConstructorId())
  {
  case 0:
    {
      if (toCreateCopy)
      {
        for (int i = 0; i < myObjects.length(); i++)
        {
          anObj = GEOM::GEOM_ITransformOperations::_narrow(getOperation())->
            ScaleShapeCopy(myObjects[i], myPoint, SpinBox_FX->GetValue());
          if (!anObj->_is_nil())
            objects.push_back(anObj._retn());
        }
      }
      else
      {
        for (int i = 0; i < myObjects.length(); i++)
        {
          anObj = GEOM::GEOM_ITransformOperations::_narrow(getOperation())->
            ScaleShape(myObjects[i], myPoint, SpinBox_FX->GetValue());
          if (!anObj->_is_nil())
            objects.push_back(anObj._retn());
        }
      }
    }
    break;
  case 1:
    {
      if (toCreateCopy)
      {
        for (int i = 0; i < myObjects.length(); i++)
        {
          anObj = GEOM::GEOM_ITransformOperations::_narrow(getOperation())->
            ScaleShapeAlongAxesCopy(myObjects[i], myPoint, SpinBox_FX->GetValue(),
                                    SpinBox_FY->GetValue(), SpinBox_FZ->GetValue());
          if (!anObj->_is_nil())
            objects.push_back(anObj._retn());
        }
      }
      else
      {
        for (int i = 0; i < myObjects.length(); i++)
        {
          anObj = GEOM::GEOM_ITransformOperations::_narrow(getOperation())->
            ScaleShapeAlongAxes(myObjects[i], myPoint, SpinBox_FX->GetValue(),
                                SpinBox_FY->GetValue(), SpinBox_FZ->GetValue());
          if (!anObj->_is_nil())
            objects.push_back(anObj._retn());
        }
      }
    }
    break;
  default:
    break;
  }

  res = true;
  return res;
}

//=================================================================================
// function : restoreSubShapes
// purpose  :
//=================================================================================
void TransformationGUI_ScaleDlg::restoreSubShapes (SALOMEDS::Study_ptr   theStudy,
                                                   SALOMEDS::SObject_ptr theSObject)
{
  if (CheckBoxRestoreSS->isChecked()) {
    // empty list of arguments means that all arguments should be restored
    getGeomEngine()->RestoreSubShapesSO(theStudy, theSObject, GEOM::ListOfGO(),
                                        /*theFindMethod=*/GEOM::FSM_Transformed,
                                        /*theInheritFirstArg=*/true);
  }
}

//=================================================================================
// function :  CreateCopyModeChanged()
// purpose  :
//=================================================================================
void TransformationGUI_ScaleDlg::CreateCopyModeChanged (bool isCreateCopy)
{
  GroupBoxName->setEnabled(isCreateCopy);
}

//=================================================================================
// function : addSubshapesToStudy
// purpose  : virtual method to add new SubObjects if local selection
//=================================================================================
void TransformationGUI_ScaleDlg::addSubshapesToStudy()
{
  bool toCreateCopy = IsPreview() || CheckBoxCopy->isChecked();
  if (toCreateCopy)
  {
    if (!myPoint->_is_nil())
    {
      QMap<QString, GEOM::GEOM_Object_var> objMap;
      objMap[LineEdit2->text()] = myPoint;
      addSubshapesToFather(objMap);
    }
  }
}
