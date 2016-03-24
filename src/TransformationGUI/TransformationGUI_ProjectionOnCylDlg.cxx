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
// File   : TransformationGUI_ProjectionOnCylDlg.cxx
// Author : Sergey KHROMOV, Open CASCADE S.A.S.

#include "TransformationGUI_ProjectionOnCylDlg.h"

#include <GeometryGUI.h>
#include <GEOMBase.h>
#include <GEOMImpl_Types.hxx>

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SalomeApp_Application.h>
#include <SalomeApp_DoubleSpinBox.h>
#include <LightApp_SelectionMgr.h>

#include <QCheckBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>

#include <TColStd_MapOfInteger.hxx>

//=================================================================================
// class    : TransformationGUI_ProjectionOnCylDlg()
// purpose  : Constructs a TransformationGUI_ProjectionOnCylDlg which is
//            a child of 'parent', with the name 'name' and widget flags set to
//            'f'. The dialog will by default be modeless, unless you set
//            'modal' to TRUE to construct a modal dialog.
//=================================================================================
TransformationGUI_ProjectionOnCylDlg::TransformationGUI_ProjectionOnCylDlg
(GeometryGUI* theGeometryGUI, QWidget* parent, bool modal, Qt::WindowFlags fl)
  : GEOMBase_Skeleton (theGeometryGUI, parent, modal, fl),
    myObjectName      (0),
    mySelButton       (0),
    myRadiusSpin      (0),
    myStartAngleSpin  (0),
    myUseAngleLen     (0),
    myAngleLenSpin    (0),
    myAngleRotSpin    (0)
{
  SUIT_ResourceMgr* aResMgr = myGeomGUI->getApp()->resourceMgr();
  QPixmap image0 (aResMgr->loadPixmap("GEOM", tr("ICO_DLG_PROJ_ON_CYL")));
  QPixmap image1 (aResMgr->loadPixmap("GEOM", tr("ICON_SELECT")));

  setWindowTitle(tr("GEOM_PROJ_ON_CYL_TITLE"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_PROJECTION_TITLE"));
  mainFrame()->RadioButton1->setIcon(image0);
  mainFrame()->RadioButton2->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton3->close();

  QGroupBox   *aGrpParams    =
    new QGroupBox(tr("GEOM_ARGUMENTS"), centralWidget());
  QGridLayout *aParamsLayout  = new QGridLayout(aGrpParams);
  QLabel      *anObjLbl       = new QLabel(tr("GEOM_OBJECT"), aGrpParams);
  QLabel      *aRadiusLbl     = new QLabel(tr("GEOM_RADIUS"), aGrpParams);
  QLabel      *aStartAngleLbl =
    new QLabel(tr("GEOM_PROJ_ON_CYL_START_ANGLE"), aGrpParams);
  QLabel      *anAngleLenLbl  =
    new QLabel(tr("GEOM_PROJ_ON_CYL_LENGTH_ANGLE"), aGrpParams);
  QLabel      *anAngleRotLbl  =
    new QLabel(tr("GEOM_PROJ_ON_CYL_ROTATION_ANGLE"), aGrpParams);

  myObjectName     = new QLineEdit(aGrpParams);
  mySelButton      = new QPushButton(aGrpParams);
  myRadiusSpin     = new SalomeApp_DoubleSpinBox(aGrpParams);
  myStartAngleSpin = new SalomeApp_DoubleSpinBox(aGrpParams);
  myUseAngleLen    = new QCheckBox(aGrpParams);
  myAngleLenSpin   = new SalomeApp_DoubleSpinBox(aGrpParams);
  myAngleRotSpin   = new SalomeApp_DoubleSpinBox(aGrpParams);

  myObjectName->setReadOnly(true);
  mySelButton->setIcon(image1);
  mySelButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  myUseAngleLen->setChecked(false);

  aParamsLayout->setMargin(9);
  aParamsLayout->setSpacing(6);
  aParamsLayout->addWidget(anObjLbl,         0, 1);
  aParamsLayout->addWidget(mySelButton,      0, 2);
  aParamsLayout->addWidget(myObjectName,     0, 3);
  aParamsLayout->addWidget(aRadiusLbl,       1, 1);
  aParamsLayout->addWidget(myRadiusSpin,     1, 2, 1, 2);
  aParamsLayout->addWidget(aStartAngleLbl,   2, 1);
  aParamsLayout->addWidget(myStartAngleSpin, 2, 2, 1, 2);
  aParamsLayout->addWidget(myUseAngleLen,    3, 0);
  aParamsLayout->addWidget(anAngleLenLbl,    3, 1);
  aParamsLayout->addWidget(myAngleLenSpin,   3, 2, 1, 2);
  aParamsLayout->addWidget(anAngleRotLbl,    4, 1);
  aParamsLayout->addWidget(myAngleRotSpin,   4, 2, 1, 2);

  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( aGrpParams );

  /***************************************************************/

  setHelpFileName("projection_on_cylinder_operation_page.html");

  Init();
}

//=================================================================================
// function : ~TransformationGUI_ProjectionOnCylDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
TransformationGUI_ProjectionOnCylDlg::~TransformationGUI_ProjectionOnCylDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void TransformationGUI_ProjectionOnCylDlg::Init()
{
  showOnlyPreviewControl();

  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  double aStep = resMgr->doubleValue("Geometry", "SettingsGeomStep", 100);
  double aSpecificStep = 5;
  double aRadius       = 100.0;
  double aStartAngle   = 0.;
  double anAngleLen    = 360.;
  double aRotAngle     = 0.;

  initSpinBox(myRadiusSpin, 0.00001, COORD_MAX, aStep, "length_precision");
  initSpinBox(myStartAngleSpin, -180., 180., aSpecificStep, "angle_precision");
  initSpinBox(myAngleLenSpin, 0.00001, COORD_MAX, aSpecificStep, "angle_precision");
  initSpinBox(myAngleRotSpin, -180., 180., aSpecificStep, "angle_precision");

  myRadiusSpin->setValue(aRadius);
  myStartAngleSpin->setValue(aStartAngle);
  myAngleLenSpin->setValue(anAngleLen);
  myAngleRotSpin->setValue(aRotAngle);

  myObjectName->setText("");
  myUseAngleLen->setChecked(true);

  // signals and slots connections
  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(mySelButton,      SIGNAL(clicked()),            this, SLOT(SetEditCurrentArgument()));
  connect(myRadiusSpin,     SIGNAL(valueChanged(double)), this, SLOT(processPreview()));
  connect(myStartAngleSpin, SIGNAL(valueChanged(double)), this, SLOT(processPreview()));
  connect(myAngleLenSpin,   SIGNAL(valueChanged(double)), this, SLOT(processPreview()));
  connect(myAngleRotSpin,   SIGNAL(valueChanged(double)), this, SLOT(processPreview()));
  connect(myUseAngleLen,    SIGNAL(clicked()),            this, SLOT(SetUseLengthAngle()));

  connect(myGeomGUI->getApp()->selectionMgr(),SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));

  initName(tr("GEOM_PROJECTION_NAME"));

  buttonOk()->setEnabled(false);
  buttonApply()->setEnabled(false);

  activateSelection();
  SelectionIntoArgument();
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void TransformationGUI_ProjectionOnCylDlg::ClickOnOk()
{
  setIsApplyAndClose( true );
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool TransformationGUI_ProjectionOnCylDlg::ClickOnApply()
{
  if ( !onAccept() )
    return false;

  initName();
  return true;
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection is changed or on dialog initialization or activation
//=================================================================================
void TransformationGUI_ProjectionOnCylDlg::SelectionIntoArgument()
{
  erasePreview();
  myObj = GEOM::GEOM_Object::_nil();

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  if (aSelList.Extent() == 1) {
    // Single object is selected.
    GEOM::GEOM_Object_var aSelectedObject =
      GEOMBase::ConvertIOinGEOMObject( aSelList.First() );

    if (aSelectedObject->_is_nil()) {
      // Null object.
      myObjectName->setText("");
      buttonOk()->setEnabled(false);
      buttonApply()->setEnabled(false);
    } else {
      // Not Null object.
      myObj = aSelectedObject;
      myObjectName->setText(GEOMBase::GetName(myObj));
      buttonOk()->setEnabled(true);
      buttonApply()->setEnabled(true);
    }
  } else {
    // Not a single object is selected.
    myObjectName->setText("");
    buttonOk()->setEnabled(false);
    buttonApply()->setEnabled(false);
  }

  processPreview();
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void TransformationGUI_ProjectionOnCylDlg::SetEditCurrentArgument()
{
  myObjectName->setFocus();
  myEditCurrentArgument = myObjectName;
  SelectionIntoArgument();
}

//=================================================================================
// function : SetUseLengthAngle()
// purpose  :
//=================================================================================
void TransformationGUI_ProjectionOnCylDlg::SetUseLengthAngle()
{
  myAngleLenSpin->setEnabled(myUseAngleLen->isChecked());
  processPreview();
}

//=================================================================================
// function : activateSelection
// purpose  : activate selection of wires or faces
//=================================================================================
void TransformationGUI_ProjectionOnCylDlg::activateSelection()
{
  TColStd_MapOfInteger aMap;

  aMap.Add(GEOM_WIRE);
  aMap.Add(GEOM_FACE);
  globalSelection(aMap);
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void TransformationGUI_ProjectionOnCylDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
           this, SLOT( SelectionIntoArgument() ) );

  activateSelection();
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void TransformationGUI_ProjectionOnCylDlg::enterEvent (QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr TransformationGUI_ProjectionOnCylDlg::createOperation()
{
  return getGeomEngine()->GetITransformOperations(getStudyId());
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool TransformationGUI_ProjectionOnCylDlg::isValid (QString &msg)
{
  bool isOk = false;

  if (!myObj->_is_nil()                      &&
       myRadiusSpin->isValid(msg, !IsPreview()) &&
       myStartAngleSpin->isValid(msg, !IsPreview()) &&
       myAngleRotSpin->isValid(msg, !IsPreview())) {
    if (myUseAngleLen->isChecked()) {
      // Check length angle spin.
      isOk =  myAngleLenSpin->isValid(msg, !IsPreview());
    } else {
      // Don't check length angle spin.
      isOk = true;
    }
  }

  return isOk;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool TransformationGUI_ProjectionOnCylDlg::execute (ObjectList& objects)
{
  GEOM::GEOM_ITransformOperations_var anOper  =
    GEOM::GEOM_ITransformOperations::_narrow(getOperation());

  double aRadius      = myRadiusSpin->value();
  double aStartAngle  = myStartAngleSpin->value()*M_PI/180.;
  double aRotAngle    = myAngleRotSpin->value()*M_PI/180.;
  double aLengthAngle = -1.;

  if (myUseAngleLen->isChecked()) {
    aLengthAngle = myAngleLenSpin->value()*M_PI/180.;
  }

  GEOM::GEOM_Object_var anObj = anOper->MakeProjectionOnCylinder
    (myObj, aRadius, aStartAngle, aLengthAngle, aRotAngle);

  if (!anObj->_is_nil()) {
    if (!IsPreview()) {
      QStringList aParameters;
      aParameters << myRadiusSpin->text();
      aParameters << myStartAngleSpin->text();
      if (myUseAngleLen->isChecked())
        aParameters << myAngleLenSpin->text();
      aParameters << myAngleRotSpin->text();
      anObj->SetParameters(aParameters.join(":").toUtf8().constData());
    }
    objects.push_back(anObj._retn());
  }

  return true;
}
