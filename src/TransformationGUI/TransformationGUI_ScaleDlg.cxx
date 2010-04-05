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
// File   : TransformationGUI_ScaleDlg.cxx
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.
//
#include "TransformationGUI_ScaleDlg.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <SalomeApp_DoubleSpinBox.h>
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

//=================================================================================
// class    : TransformationGUI_ScaleDlg()
// purpose  : Constructs a TransformationGUI_ScaleDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
TransformationGUI_ScaleDlg::TransformationGUI_ScaleDlg (GeometryGUI* theGeometryGUI, QWidget* parent,
                                                        bool modal, Qt::WindowFlags fl)
  : GEOMBase_Skeleton(theGeometryGUI, parent, modal, fl),
    myInitial(true)
{
  SUIT_ResourceMgr* aResMgr = myGeomGUI->getApp()->resourceMgr();
  QPixmap image1 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_SCALE")));
  QPixmap image2 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_SCALE_ALONG_AXES")));
  QPixmap image3 (aResMgr->loadPixmap("GEOM", tr("ICON_SELECT")));

  setWindowTitle(tr("GEOM_SCALE_TITLE"));

  // Constructors
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_SCALE"));
  mainFrame()->RadioButton1->setIcon(image1);
  mainFrame()->RadioButton2->setIcon(image2);
  mainFrame()->RadioButton3->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton3->close();

  // Own widgets
  GroupBox1 = new QGroupBox(tr("GEOM_ARGUMENTS"), this);
  QGridLayout* OwnLayout = new QGridLayout(GroupBox1);
  OwnLayout->setSpacing(6);
  OwnLayout->setMargin(11);

  TextLabel1 = new QLabel(tr("GEOM_OBJECTS"), GroupBox1);
  PushButton1 = new QPushButton(GroupBox1);
  PushButton1->setIcon(image3);
  PushButton1->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
  LineEdit1 = new QLineEdit(GroupBox1);
  LineEdit1->setReadOnly(true);
  LineEdit1->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));

  TextLabel2 = new QLabel(tr("GEOM_CENTRAL_POINT"), GroupBox1);
  PushButton2 = new QPushButton(GroupBox1);
  PushButton2->setIcon(image3);
  PushButton2->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
  LineEdit2 = new QLineEdit(GroupBox1);
  LineEdit2->setReadOnly(true);
  LineEdit2->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));

  TextLabel3 = new QLabel(tr("GEOM_SCALE_FACTOR"), GroupBox1);
  SpinBox_FX = new SalomeApp_DoubleSpinBox(GroupBox1);

  TextLabel4 = new QLabel(tr("GEOM_SCALE_FACTOR_Y"), GroupBox1);
  SpinBox_FY = new SalomeApp_DoubleSpinBox(GroupBox1);

  TextLabel5 = new QLabel(tr("GEOM_SCALE_FACTOR_Z"), GroupBox1);
  SpinBox_FZ = new SalomeApp_DoubleSpinBox(GroupBox1);

  CheckBoxCopy = new QCheckBox(tr("GEOM_CREATE_COPY"), GroupBox1);
  CheckBoxCopy->setChecked(true);

  // Layouting
  OwnLayout->addWidget(TextLabel1,   0, 0);
  OwnLayout->addWidget(PushButton1,  0, 1);
  OwnLayout->addWidget(LineEdit1,    0, 2, 1, 2);
  OwnLayout->addWidget(TextLabel2,   1, 0);
  OwnLayout->addWidget(PushButton2,  1, 1);
  OwnLayout->addWidget(LineEdit2,    1, 2, 1, 2);
  OwnLayout->addWidget(TextLabel3,   2, 0);
  OwnLayout->addWidget(SpinBox_FX,   2, 2);
  OwnLayout->addWidget(TextLabel4,   3, 0);
  OwnLayout->addWidget(SpinBox_FY,   3, 2);
  OwnLayout->addWidget(TextLabel5,   4, 0);
  OwnLayout->addWidget(SpinBox_FZ,   4, 2);
  OwnLayout->addWidget(CheckBoxCopy, 5, 0, 1, 4);

  QVBoxLayout* layout = new QVBoxLayout(centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
  layout->addWidget(GroupBox1);

  setHelpFileName("scale_operation_page.html");

  // Modification of an existing object by offset is not allowed
  CheckBoxCopy->hide();

  // Activate Create a Copy mode
  CreateCopyModeChanged(true);

  // Allowed inheritance of children and visual properties by the scaling result
  mainFrame()->GroupBoxPublish->show();

  Init();
}

//=================================================================================
// function : ~TransformationGUI_ScaleDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
TransformationGUI_ScaleDlg::~TransformationGUI_ScaleDlg()
{
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void TransformationGUI_ScaleDlg::Init()
{
  // Min, max, step and decimals for spin boxes & initial values
  double aFactor = 2.0;
  double SpecificStep = 0.5;

  initSpinBox(SpinBox_FX, COORD_MIN, COORD_MAX, SpecificStep, "parametric_precision" );
  initSpinBox(SpinBox_FY, COORD_MIN, COORD_MAX, SpecificStep, "parametric_precision" );
  initSpinBox(SpinBox_FZ, COORD_MIN, COORD_MAX, SpecificStep, "parametric_precision" );
  SpinBox_FX->setValue(aFactor);
  SpinBox_FY->setValue(aFactor);
  SpinBox_FZ->setValue(aFactor);

  // init variables
  LineEdit1->setText("");
  LineEdit2->setText("");

  myPoint = GEOM::GEOM_Object::_nil();

  // Signals and slots connections
  connect(buttonOk(),        SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(),     SIGNAL(clicked()), this, SLOT(ClickOnApply()));
  connect(this, SIGNAL(constructorsClicked(int)), this, SLOT(ConstructorsClicked(int)));

  connect(PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(LineEdit2, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(SpinBox_FX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox()));
  connect(SpinBox_FY, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox()));
  connect(SpinBox_FZ, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox()));
  
  // san : Commented so as not to override specific step settings
  //connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), this, SLOT(SetDoubleSpinBoxStep(double)));

  connect(CheckBoxCopy, SIGNAL(toggled(bool)), this, SLOT(CreateCopyModeChanged(bool)));

  initName(tr("GEOM_SCALE"));

  ConstructorsClicked(0);
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void TransformationGUI_ScaleDlg::ConstructorsClicked (int constructorId)
{
  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);

  switch (constructorId) {
  case 0: // translation an object by dx, dy, dz
    TextLabel3->setText(tr("GEOM_SCALE_FACTOR"));
    TextLabel4->hide();
    TextLabel5->hide();
    SpinBox_FY->hide();
    SpinBox_FZ->hide();

    PushButton1->click();
    break;
  case 1: // translation an object by 2 points
    TextLabel3->setText(tr("GEOM_SCALE_FACTOR_X"));
    TextLabel4->show();
    TextLabel5->show();
    SpinBox_FY->show();
    SpinBox_FZ->show();

    PushButton1->click();
    break;
  default:
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

  initName(tr("GEOM_SCALE"));
  // activate selection and connect selection manager
  ConstructorsClicked(getConstructorId());
  return true;
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection is changed or on dialog initialization or activation
//=================================================================================
void TransformationGUI_ScaleDlg::SelectionIntoArgument()
{
  erasePreview();
  myEditCurrentArgument->setText("");

  if (myEditCurrentArgument == LineEdit1)
    myObjects.length(0);
  else if (myEditCurrentArgument == LineEdit2)
    myPoint = GEOM::GEOM_Object::_nil();

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  QString aName;

  if (myEditCurrentArgument == LineEdit1)
  {
    int aNbSel = GEOMBase::GetNameOfSelectedIObjects(aSelList, aName);
    if (aNbSel < 1)
      return;

    GEOMBase::ConvertListOfIOInListOfGO(aSelList, myObjects);
    if (!myObjects.length())
      return;
    else
      myEditCurrentArgument->setText(aName);
  }
  else if (myEditCurrentArgument == LineEdit2)
  {
    GEOM::GEOM_Object_var aSelectedObject = GEOM::GEOM_Object::_nil();
    if (aSelList.Extent() == 1)
    {
      Standard_Boolean testResult = Standard_False;
      aSelectedObject = GEOMBase::ConvertIOinGEOMObject(aSelList.First(), testResult);
      if (testResult)
      {
        aName = GEOMBase::GetName(aSelectedObject);

        TopoDS_Shape aShape;
        if (GEOMBase::GetShape(aSelectedObject, aShape, TopAbs_SHAPE) && !aShape.IsNull())
        {
          TColStd_IndexedMapOfInteger aMap;
          aSelMgr->GetIndexes(aSelList.First(), aMap);
          if (aMap.Extent() == 1)
          {
            int anIndex = aMap(1);
            aName += QString(":vertex_%1").arg(anIndex);

            //Find SubShape Object in Father
            GEOM::GEOM_Object_var aFindedObject = findObjectInFather(aSelectedObject, aName);

            if (aFindedObject == GEOM::GEOM_Object::_nil()) { // Object not found in study
              GEOM::GEOM_IShapesOperations_var aShapesOp =
                getGeomEngine()->GetIShapesOperations(getStudyId());
              aSelectedObject = aShapesOp->GetSubShape(aSelectedObject, anIndex);
            }
            else
              aSelectedObject = aFindedObject; // get Object from study
          }
          else // Global Selection
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
    myEditCurrentArgument->setText(aName);

    if (!myPoint->_is_nil() && !myObjects.length())
      PushButton1->click();
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
void TransformationGUI_ScaleDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
  globalSelection();

  if (send == PushButton1) {
    myEditCurrentArgument = LineEdit1;

    PushButton2->setDown(false);
    LineEdit2->setEnabled(false);
  }
  else if (send == PushButton2) {
    myEditCurrentArgument = LineEdit2;

    localSelection(GEOM::GEOM_Object::_nil(), TopAbs_VERTEX);

    PushButton1->setDown(false);
    LineEdit1->setEnabled(false);
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
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void TransformationGUI_ScaleDlg::ActivateThisDialog()
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
void TransformationGUI_ScaleDlg::enterEvent (QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
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
// function : SetDoubleSpinBoxStep()
// purpose  : Double spin box management
//=================================================================================
void TransformationGUI_ScaleDlg::SetDoubleSpinBoxStep(double step)
{
  SpinBox_FX->setSingleStep(step);
  SpinBox_FY->setSingleStep(step);
  SpinBox_FZ->setSingleStep(step);
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr TransformationGUI_ScaleDlg::createOperation()
{
  return getGeomEngine()->GetITransformOperations(getStudyId());
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool TransformationGUI_ScaleDlg::isValid (QString& msg)
{
  // && !myPoint->_is_nil()
  if (getConstructorId() == 0) {
    bool ok = SpinBox_FX->isValid( msg, !IsPreview() );
    return myObjects.length() > 0 && fabs(SpinBox_FX->value()) > 0.00001 && ok;
  }

  bool ok = true;
  ok = SpinBox_FX->isValid( msg, !IsPreview() ) && ok;
  ok = SpinBox_FY->isValid( msg, !IsPreview() ) && ok;
  ok = SpinBox_FZ->isValid( msg, !IsPreview() ) && ok;
  return myObjects.length() > 0 &&
    fabs(SpinBox_FX->value()) > 0.00001 &&
    fabs(SpinBox_FY->value()) > 0.00001 &&
    fabs(SpinBox_FZ->value()) > 0.00001 && ok;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool TransformationGUI_ScaleDlg::execute (ObjectList& objects)
{
  bool toCreateCopy = IsPreview() || CheckBoxCopy->isChecked();

  GEOM::GEOM_Object_var anObj;

  GEOM::GEOM_ITransformOperations_var anOper = GEOM::GEOM_ITransformOperations::_narrow(getOperation());

  switch (getConstructorId())
  {
  case 0:
    {
      if (toCreateCopy)
      {
        for (int i = 0; i < myObjects.length(); i++)
        {
          anObj = anOper->ScaleShapeCopy(myObjects[i], myPoint, SpinBox_FX->value());
          if (!anObj->_is_nil()) {
            if(!IsPreview()) 
              anObj->SetParameters(SpinBox_FX->text().toLatin1().constData());
            objects.push_back(anObj._retn());
          }
        }
      }
      else
      {
        for (int i = 0; i < myObjects.length(); i++)
        {
          anObj = anOper->ScaleShape(myObjects[i], myPoint, SpinBox_FX->value());
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
          anObj = anOper->ScaleShapeAlongAxesCopy(myObjects[i], myPoint, SpinBox_FX->value(),
                                                  SpinBox_FY->value(), SpinBox_FZ->value());
          if (!anObj->_is_nil())
            if(!IsPreview()) {
              QStringList aParameters;
              aParameters<<SpinBox_FX->text();
              aParameters<<SpinBox_FY->text();
              aParameters<<SpinBox_FZ->text();
              anObj->SetParameters(aParameters.join(":").toLatin1().constData());
            }
            objects.push_back(anObj._retn());
        }
      }
      else
      {
        for (int i = 0; i < myObjects.length(); i++)
        {
          anObj = anOper->ScaleShapeAlongAxes(myObjects[i], myPoint, SpinBox_FX->value(),
                                              SpinBox_FY->value(), SpinBox_FZ->value());
          if (!anObj->_is_nil())
            objects.push_back(anObj._retn());
        }
      }
    }
    break;
  default:
    break;
  }

  return true;
}

//=================================================================================
// function : restoreSubShapes
// purpose  :
//=================================================================================
void TransformationGUI_ScaleDlg::restoreSubShapes (SALOMEDS::Study_ptr   theStudy,
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
void TransformationGUI_ScaleDlg::CreateCopyModeChanged (bool isCreateCopy)
{
  mainFrame()->GroupBoxName->setEnabled(isCreateCopy);
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
