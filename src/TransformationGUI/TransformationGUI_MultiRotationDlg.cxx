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
//  File   : TransformationGUI_MultiRotationDlg.cxx
//  Author : Damien COQUERET, Open CASCADE S.A.S.

#include "TransformationGUI_MultiRotationDlg.h"

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
// class    : TransformationGUI_MultiRotationDlg()
// purpose  : Constructs a TransformationGUI_MultiRotationDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
TransformationGUI_MultiRotationDlg::TransformationGUI_MultiRotationDlg
(GeometryGUI* theGeometryGUI, QWidget* parent, bool modal, Qt::WindowFlags fl)
  : GEOMBase_Skeleton(theGeometryGUI, parent, modal, fl),
    myInitial(true)
{
  SUIT_ResourceMgr* aResMgr = myGeomGUI->getApp()->resourceMgr();
  QPixmap image0 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_MULTIROTATION_SIMPLE")));
  QPixmap image1 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_MULTIROTATION_DOUBLE")));
  QPixmap image2 (aResMgr->loadPixmap("GEOM", tr("ICON_SELECT")));

  setWindowTitle(tr("GEOM_MULTIROTATION_TITLE"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_MULTIROTATION"));
  mainFrame()->RadioButton1->setIcon(image0);
  mainFrame()->RadioButton2->setIcon(image1);
  mainFrame()->RadioButton3->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton3->close();

  bool isAngleStep = false;

  GroupArgs = new TransformationGUI_2Sel4Spin1Check (centralWidget());
  GroupArgs->GroupBox1->setTitle(tr("GEOM_MULTIROTATION_DOUBLE"));
  GroupArgs->TextLabel1->setText(tr("GEOM_MAIN_OBJECT"));
  GroupArgs->TextLabel2->setText(tr("GEOM_VECTOR"));
  //GroupArgs->TextLabel3->setText(tr("GEOM_ANGLE_STEP"));
  GroupArgs->CheckAngleStep->setText(tr("GEOM_ANGLE_STEP"));
  GroupArgs->CheckAngleStep->setChecked(isAngleStep);
  GroupArgs->TextLabel4->setText(tr("GEOM_NB_TIMES"));
  GroupArgs->TextLabel5->setText(tr("GEOM_STEP_R"));
  GroupArgs->TextLabel6->setText(tr("GEOM_NB_TIMES"));
  GroupArgs->CheckButton1->setText(tr("GEOM_REVERSE"));
  GroupArgs->PushButton1->setIcon(image2);
  GroupArgs->PushButton2->setIcon(image2);
  GroupArgs->LineEdit1->setReadOnly(true);
  GroupArgs->LineEdit2->setReadOnly(true);
  GroupArgs->SpinBox_DX1->setEnabled(isAngleStep);
  GroupArgs->CheckButton1->setEnabled(isAngleStep);
  /***************************************************************/

  setHelpFileName("multi_rotation_operation_page.html");

  Init();
}

//=================================================================================
// function : ~TransformationGUI_MultiRotationDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
TransformationGUI_MultiRotationDlg::~TransformationGUI_MultiRotationDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void TransformationGUI_MultiRotationDlg::Init()
{
  // init variables
  myAng = 45.0;
  myStep = 50.0;
  myNbTimes1 = myNbTimes2 = 3;

  // Get setting of step value from file configuration
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  double step = resMgr->doubleValue("Geometry", "SettingsGeomStep", 100);

  int SpecificStep1 = 5;
  int SpecificStep2 = 1;

  // min, max, step and decimals for spin boxes & initial values
  initSpinBox(GroupArgs->SpinBox_DX1, COORD_MIN, COORD_MAX, SpecificStep1, "angle_precision");
  initSpinBox(GroupArgs->SpinBox_DY1, 1, 999, SpecificStep2);
  initSpinBox(GroupArgs->SpinBox_DX2, COORD_MIN, COORD_MAX, step, "length_precision");
  initSpinBox(GroupArgs->SpinBox_DY2, 1, 999, SpecificStep2);

  mainFrame()->GroupBoxPublish->show();

  // signals and slots connections
  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(this, SIGNAL(constructorsClicked(int)), this, SLOT(ConstructorsClicked(int)));

  connect(GroupArgs->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupArgs->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(GroupArgs->SpinBox_DX1, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupArgs->SpinBox_DY1, SIGNAL(valueChanged(int)),    this, SLOT(ValueChangedInSpinBox(int)));
  connect(GroupArgs->SpinBox_DX2, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupArgs->SpinBox_DY2, SIGNAL(valueChanged(int)),    this, SLOT(ValueChangedInSpinBox(int)));

  connect(GroupArgs->SpinBox_DX1, SIGNAL(textChanged(const QString&)),
          this, SLOT(TextValueChangedInSpinBox(const QString&)));

  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), this, SLOT(SetDoubleSpinBoxStep(double)));

  connect(GroupArgs->CheckAngleStep, SIGNAL(toggled(bool)), this, SLOT(CheckAngleStep(bool)));
  connect(GroupArgs->CheckButton1, SIGNAL(toggled(bool)), this, SLOT(ReverseAngle()));

  initName(tr("GEOM_MULTIROTATION"));

  ConstructorsClicked(0);
}

//=================================================================================
// function : SetDoubleSpinBoxStep()
// purpose  : Double spin box management
//=================================================================================
void TransformationGUI_MultiRotationDlg::SetDoubleSpinBoxStep (double step)
{
  GroupArgs->SpinBox_DX2->setSingleStep(step);
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void TransformationGUI_MultiRotationDlg::ConstructorsClicked (int constructorId)
{
  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);

  //myBase.nullify();
  //myVector.nullify();

  //GroupArgs->LineEdit1->setText("");
  //GroupArgs->LineEdit2->setText("");

  GroupArgs->SpinBox_DX1->setValue(myAng);
  GroupArgs->SpinBox_DY1->setValue(myNbTimes1);
  GroupArgs->SpinBox_DX2->setValue(myStep);
  GroupArgs->SpinBox_DY2->setValue(myNbTimes2);

  switch (constructorId) {
  case 0: // Rotate simple
    {
      GroupArgs->GroupBox1->setTitle(tr("GEOM_MULTIROTATION_SIMPLE"));
      GroupArgs->TextLabel5->hide();
      GroupArgs->TextLabel6->hide();
      GroupArgs->SpinBox_DX2->hide();
      GroupArgs->SpinBox_DY2->hide();
      GroupArgs->CheckButton1->hide();
    }
    break;
  case 1: // Rotate double
    {
      GroupArgs->GroupBox1->setTitle(tr("GEOM_MULTIROTATION_DOUBLE"));
      GroupArgs->TextLabel5->show();
      GroupArgs->TextLabel6->show();
      GroupArgs->SpinBox_DX2->show();
      GroupArgs->SpinBox_DY2->show();
      GroupArgs->CheckButton1->show();
    }
    break;
  }

  GroupArgs->PushButton1->click();

  qApp->processEvents();
  updateGeometry();
  resize(minimumSizeHint());

  if (myInitial) {
    myInitial = false;
    SelectionIntoArgument();
  }
  else {
    processPreview();
  }
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void TransformationGUI_MultiRotationDlg::ClickOnOk()
{
  setIsApplyAndClose( true );
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool TransformationGUI_MultiRotationDlg::ClickOnApply()
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
void TransformationGUI_MultiRotationDlg::SelectionIntoArgument()
{
  erasePreview();

  TopAbs_ShapeEnum aNeedType = ( myEditCurrentArgument == GroupArgs->LineEdit2 ) ?
    TopAbs_EDGE : TopAbs_SHAPE;
  GEOM::GeomObjPtr aSelectedObject = getSelected( aNeedType );
  TopoDS_Shape aShape;
  if ( aSelectedObject && GEOMBase::GetShape( aSelectedObject.get(), aShape ) && !aShape.IsNull() ) {
    QString aName = GEOMBase::GetName( aSelectedObject.get() );
    myEditCurrentArgument->setText( aName );
    if ( myEditCurrentArgument == GroupArgs->LineEdit1 ) {
      myBase = aSelectedObject;

      // recompute myAng and myStep (Mantis issue 0021718)
      GEOM::GEOM_IMeasureOperations_var anOper = getGeomEngine()->GetIMeasureOperations(getStudyId());
      double Xmin, Xmax, Ymin, Ymax, Zmin, Zmax;
      anOper->GetBoundingBox(myBase.get(), true, Xmin, Xmax, Ymin, Ymax, Zmin, Zmax);
      if (anOper->IsDone()) {
        // angular step
        double diag = sqrt((Xmax-Xmin)*(Xmax-Xmin) + (Ymax-Ymin)*(Ymax-Ymin));
        double d = sqrt((0.5*(Xmax+Xmin))*(0.5*(Xmax+Xmin)) + (0.5*(Ymax+Ymin))*(0.5*(Ymax+Ymin)));
        if ( fabs(d) > 1.e-16 )
	  myAng = floor(2.0 * atan(diag/d) * 180.0 / M_PI);
        GroupArgs->SpinBox_DX1->setValue(myAng);

        // radial step
        myStep = Max(floor(1.5*(Xmax-Xmin)), floor(1.5*(Ymax-Ymin)));
        GroupArgs->SpinBox_DX2->setValue(myStep);
      }

      if ( !myVector )
        GroupArgs->PushButton2->click();
    }
    else if ( myEditCurrentArgument == GroupArgs->LineEdit2 ) {
      myVector = aSelectedObject;
      if ( !myBase )
        GroupArgs->PushButton1->click();
    }
    
    // clear selection
    // disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
    // myGeomGUI->getApp()->selectionMgr()->clearSelected();
    // connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
    //         this, SLOT(SelectionIntoArgument()));
  }
  else {
    if ( myEditCurrentArgument == GroupArgs->LineEdit1 )
      myBase.nullify();
    else if ( myEditCurrentArgument == GroupArgs->LineEdit2 )
      myVector.nullify();
    myEditCurrentArgument->setText("");
  }

  processPreview();
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void TransformationGUI_MultiRotationDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
  globalSelection(GEOM_ALLSHAPES);

  if (send == GroupArgs->PushButton1) {
    myEditCurrentArgument = GroupArgs->LineEdit1;

    GroupArgs->PushButton2->setDown(false);
    GroupArgs->LineEdit2->setEnabled(false);
  }
  else if (send == GroupArgs->PushButton2) {
    myEditCurrentArgument = GroupArgs->LineEdit2;

    localSelection(TopAbs_EDGE);

    GroupArgs->PushButton1->setDown(false);
    GroupArgs->LineEdit1->setEnabled(false);
  }
  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));

  // enable line edit
  myEditCurrentArgument->setEnabled(true);
  myEditCurrentArgument->setFocus();
  // after setFocus(), because it will be setDown(false) when loses focus
  send->setDown(true);

  // seems we need it only to avoid preview disappearing, caused by selection mode change
  processPreview();
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void TransformationGUI_MultiRotationDlg::ActivateThisDialog()
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
void TransformationGUI_MultiRotationDlg::enterEvent (QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : TextValueChangedInSpinBox()
// purpose  :
//=================================================================================
void TransformationGUI_MultiRotationDlg::TextValueChangedInSpinBox (const QString& s)
{
  bool isDigit;
  s.toDouble(&isDigit);
  if (!isDigit)
    GroupArgs->CheckButton1->setChecked(false);
  GroupArgs->CheckButton1->setEnabled(isDigit);
}

//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void TransformationGUI_MultiRotationDlg::ValueChangedInSpinBox (double newValue)
{
  QObject* send = (QObject*)sender();

  if (send == GroupArgs->SpinBox_DX1)
    myAng = newValue;
  else if (send == GroupArgs->SpinBox_DX2)
    myStep = newValue;

  processPreview();
}

//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void TransformationGUI_MultiRotationDlg::ValueChangedInSpinBox (int newValue)
{
  QObject* send = (QObject*)sender();

  if (send == GroupArgs->SpinBox_DY1)
    myNbTimes1 = newValue;
  else if (send == GroupArgs->SpinBox_DY2)
    myNbTimes2 = newValue;

  processPreview();
}

//=================================================================================
// function : ReverseAngle()
// purpose  :
//=================================================================================
void TransformationGUI_MultiRotationDlg::ReverseAngle()
{
  myAng = -myAng;

  int aConstructorId = getConstructorId();

  if (aConstructorId == 1)
    GroupArgs->SpinBox_DX1->setValue(myAng);

  processPreview();
}

//=================================================================================
// function : CheckAngleStep()
// purpose  :
//=================================================================================
void TransformationGUI_MultiRotationDlg::CheckAngleStep (bool isOn)
{
  GroupArgs->SpinBox_DX1->setEnabled(isOn);
  GroupArgs->CheckButton1->setEnabled(isOn);
  processPreview();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr TransformationGUI_MultiRotationDlg::createOperation()
{
  return getGeomEngine()->GetITransformOperations(getStudyId());
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool TransformationGUI_MultiRotationDlg::isValid (QString& msg)
{
  bool ok = false;
  switch (getConstructorId()) {
  case 0:
    ok = (GroupArgs->SpinBox_DX1->isValid( msg, !IsPreview() ) &&
          GroupArgs->SpinBox_DY1->isValid( msg, !IsPreview() ) &&
          //myBase && myVector); // myVector = DZ by default
          myBase);
    break;
  case 1:
    ok = (GroupArgs->SpinBox_DX1->isValid( msg, !IsPreview() ) &&
          GroupArgs->SpinBox_DY1->isValid( msg, !IsPreview() ) &&
          GroupArgs->SpinBox_DX2->isValid( msg, !IsPreview() ) &&
          GroupArgs->SpinBox_DY2->isValid( msg, !IsPreview() ) &&
          //myBase && myVector); // myVector = DZ by default
          myBase);
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
bool TransformationGUI_MultiRotationDlg::execute (ObjectList& objects)
{
  bool res = false;

  GEOM::GEOM_Object_var anObj;
  QStringList aParameters;

  GEOM::GEOM_ITransformOperations_var anOper = GEOM::GEOM_ITransformOperations::_narrow(getOperation());

  switch (getConstructorId()) {
  case 0:
    if (GroupArgs->CheckAngleStep->isChecked())
      anObj = anOper->MultiRotate1DByStep(myBase.get(), myVector.get(), myAng * M_PI / 180., myNbTimes1);
    else
      anObj = anOper->MultiRotate1D(myBase.get(), myVector.get(), myNbTimes1);

    if (!IsPreview()) {
      if (GroupArgs->CheckAngleStep->isChecked())
        aParameters << GroupArgs->SpinBox_DX1->text();
      aParameters << GroupArgs->SpinBox_DY1->text();
    }
    res = true;
    break;
  case 1:
    if (GroupArgs->CheckAngleStep->isChecked())
      anObj = anOper->MultiRotate2DByStep(myBase.get(), myVector.get(), myAng * M_PI / 180., myNbTimes1,
                                          myStep, myNbTimes2);
    else
      anObj = anOper->MultiRotate2DNbTimes(myBase.get(), myVector.get(), myNbTimes1, myStep, myNbTimes2);

    if (!IsPreview()) {
      if (GroupArgs->CheckAngleStep->isChecked())
        aParameters << GroupArgs->SpinBox_DX1->text();
      aParameters << GroupArgs->SpinBox_DY1->text();
      aParameters << GroupArgs->SpinBox_DX2->text();
      aParameters << GroupArgs->SpinBox_DY2->text();
    }
    res = true;
    break;
  }

  if (!anObj->_is_nil()) {
    if (!IsPreview())
      anObj->SetParameters(aParameters.join(":").toLatin1().constData());
    objects.push_back(anObj._retn());
  }

  return res;
}

//=================================================================================
// function : addSubshapeToStudy
// purpose  : virtual method to add new SubObjects if local selection
//=================================================================================
void TransformationGUI_MultiRotationDlg::addSubshapesToStudy()
{
  switch (getConstructorId()) {
  case 0:
  case 1:
    GEOMBase::PublishSubObject( myVector.get() );
    break;
  default:
    break;
  }
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> TransformationGUI_MultiRotationDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res;
  res << myBase << myVector;
  return res;
}

//=================================================================================
// function : restoreSubShapes
// purpose  :
//=================================================================================
void TransformationGUI_MultiRotationDlg::restoreSubShapes (SALOMEDS::Study_ptr   theStudy,
                                                           SALOMEDS::SObject_ptr theSObject)
{
  if (mainFrame()->CheckBoxRestoreSS->isChecked()) {
    // we pass here the first operation argument (object) through the list of arguments
    // because the rotation operation place its arguments in the data structure in another order,
    // and we need to point the first argument directly
    GEOM::ListOfGO_var anArgs = new GEOM::ListOfGO;
    anArgs->length(1);
    anArgs[0] = myBase.copy();
    getGeomEngine()->RestoreSubShapesSO(theStudy, theSObject, anArgs,
                                        /*theFindMethod=*/GEOM::FSM_MultiTransformed,
                                        /*theInheritFirstArg=*/true,
                                        mainFrame()->CheckBoxAddPrefix->isChecked());
  }
}

//=================================================================================
// function : TransformationGUI_2Sel4Spin1Check
// purpose  :
//=================================================================================
TransformationGUI_2Sel4Spin1Check::TransformationGUI_2Sel4Spin1Check (QWidget *parent)
{
  gridLayout = new QGridLayout (parent);
  gridLayout->setSpacing(6);
  gridLayout->setContentsMargins(11, 11, 11, 11);
  gridLayout->setHorizontalSpacing(0);
  gridLayout->setVerticalSpacing(0);
  gridLayout->setContentsMargins(0, 0, 0, 0);

  GroupBox1 = new QGroupBox (parent);

  gridLayout1 = new QGridLayout (GroupBox1);
  gridLayout1->setSpacing(6);
  gridLayout1->setContentsMargins(11, 11, 11, 11);
  gridLayout1->setHorizontalSpacing(6);
  gridLayout1->setVerticalSpacing(6);
  gridLayout1->setContentsMargins(9, 9, 9, 9);

  // 2Sel
  TextLabel1 = new QLabel(GroupBox1);
  TextLabel2 = new QLabel(GroupBox1);

  PushButton1 = new QPushButton (GroupBox1);
  PushButton2 = new QPushButton (GroupBox1);

  LineEdit2 = new QLineEdit(GroupBox1);
  LineEdit1 = new QLineEdit(GroupBox1);

  gridLayout1->addWidget(TextLabel1, 0, 0, 1, 1);
  gridLayout1->addWidget(TextLabel2, 1, 0, 1, 1);
  gridLayout1->addWidget(PushButton1, 0, 1, 1, 1);
  gridLayout1->addWidget(PushButton2, 1, 1, 1, 1);
  gridLayout1->addWidget(LineEdit1, 0, 2, 1, 1);
  gridLayout1->addWidget(LineEdit2, 1, 2, 1, 1);

  // 4Spin (double-int-double-int)
  //TextLabel3 = new QLabel (GroupBox1);
  CheckAngleStep = new QCheckBox (GroupBox1);
  TextLabel4 = new QLabel (GroupBox1);
  TextLabel5 = new QLabel (GroupBox1);
  TextLabel6 = new QLabel (GroupBox1);

  SpinBox_DX1 = new SalomeApp_DoubleSpinBox (GroupBox1);
  SpinBox_DY1 = new SalomeApp_IntSpinBox (GroupBox1);
  SpinBox_DX2 = new SalomeApp_DoubleSpinBox (GroupBox1);
  SpinBox_DY2 = new SalomeApp_IntSpinBox (GroupBox1);

  //gridLayout1->addWidget(TextLabel3, 2, 0, 1, 1);
  gridLayout1->addWidget(CheckAngleStep, 2, 0, 1, 1);
  gridLayout1->addWidget(TextLabel4, 3, 0, 1, 1);
  gridLayout1->addWidget(TextLabel5, 5, 0, 1, 1);
  gridLayout1->addWidget(TextLabel6, 6, 0, 1, 1);

  gridLayout1->addWidget(SpinBox_DX1, 2, 1, 1, 2);
  gridLayout1->addWidget(SpinBox_DY1, 3, 1, 1, 2);
  gridLayout1->addWidget(SpinBox_DX2, 5, 1, 1, 2);
  gridLayout1->addWidget(SpinBox_DY2, 6, 1, 1, 2);

  // 1Check
  CheckButton1 = new QCheckBox (GroupBox1);

  gridLayout1->addWidget(CheckButton1, 4, 0, 1, 3);

  gridLayout->addWidget(GroupBox1, 0, 0, 1, 1);
}

//=================================================================================
// function : ~TransformationGUI_2Sel4Spin1Check()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
TransformationGUI_2Sel4Spin1Check::~TransformationGUI_2Sel4Spin1Check()
{
  // no need to delete child widgets, Qt does it all for us
}
