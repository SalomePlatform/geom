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
// File   : TransformationGUI_ExtensionDlg.cxx
// Author : Sergey KHROMOV, Open CASCADE S.A.S.

#include "TransformationGUI_ExtensionDlg.h"
#include <GEOMBase.h>
#include <GeometryGUI.h>
#include <GEOMImpl_Types.hxx>

#include <SalomeApp_Application.h>
#include <SalomeApp_DoubleSpinBox.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <LightApp_SelectionMgr.h>

#include <TColStd_MapOfInteger.hxx>

#include <QApplication>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>


//=================================================================================
// class    : TransformationGUI_ExtensionDlg
// purpose  :
//=================================================================================
TransformationGUI_ExtensionDlg::TransformationGUI_ExtensionDlg
                      (GeometryGUI* theGeometryGUI, QWidget* parent,
                       bool modal, Qt::WindowFlags fl)
  : GEOMBase_Skeleton(theGeometryGUI, parent, modal, fl),
    myObjLbl         (0),
    myUMinLbl        (0),
    myUMaxLbl        (0),
    myVMinLbl        (0),
    myVMaxLbl        (0),
    mySelButton      (0),
    myEditObjName    (0),
    myUMinSpinBox    (0),
    myUMaxSpinBox    (0),
    myVMinSpinBox    (0),
    myVMaxSpinBox    (0)
{
  QPixmap image0 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_SELECT")));
  QPixmap image1 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_EXTEND_EDGE")));
  QPixmap image2 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_EXTEND_FACE")));

  setWindowTitle(tr("GEOM_EXTENSION_TITLE"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr ("GEOM_EXTENSION"));
  mainFrame()->RadioButton1->setIcon(image1);
  mainFrame()->RadioButton2->setIcon(image2);
  mainFrame()->RadioButton3->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton3->close();

  QGroupBox *aGrp = new QGroupBox(tr("GEOM_ARGUMENTS"));

  myObjLbl        = new QLabel;
  myUMinLbl       = new QLabel;
  myUMaxLbl       = new QLabel;
  myVMinLbl       = new QLabel;
  myVMaxLbl       = new QLabel;
  mySelButton     = new QPushButton;
  myEditObjName   = new QLineEdit;
  myUMinSpinBox   = new SalomeApp_DoubleSpinBox;
  myUMaxSpinBox   = new SalomeApp_DoubleSpinBox;
  myVMinSpinBox   = new SalomeApp_DoubleSpinBox;
  myVMaxSpinBox   = new SalomeApp_DoubleSpinBox;

  myVMinLbl->setText(tr("GEOM_EXTENSION_MIN_V"));
  myVMaxLbl->setText(tr("GEOM_EXTENSION_MAX_V"));
  mySelButton->setIcon(image0);

  QGridLayout *aGrpLayout = new QGridLayout(aGrp);

  aGrpLayout->setMargin(9);
  aGrpLayout->setSpacing(6);
  aGrpLayout->addWidget(myObjLbl,      0, 0);
  aGrpLayout->addWidget(myUMinLbl,     1, 0);
  aGrpLayout->addWidget(myUMaxLbl,     2, 0);
  aGrpLayout->addWidget(myVMinLbl,     3, 0);
  aGrpLayout->addWidget(myVMaxLbl,     4, 0);
  aGrpLayout->addWidget(mySelButton,   0, 1);
  aGrpLayout->addWidget(myEditObjName, 0, 2);
  aGrpLayout->addWidget(myUMinSpinBox, 1, 1, 1, 2);
  aGrpLayout->addWidget(myUMaxSpinBox, 2, 1, 1, 2);
  aGrpLayout->addWidget(myVMinSpinBox, 3, 1, 1, 2);
  aGrpLayout->addWidget(myVMaxSpinBox, 4, 1, 1, 2);

  myVMinLbl->hide();
  myVMaxLbl->hide();
  myVMinSpinBox->hide();
  myVMaxSpinBox->hide();

  QVBoxLayout* layout = new QVBoxLayout(centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
  layout->addWidget(aGrp);
  /***************************************************************/

  setHelpFileName("extension_operation_page.html");

  /* Initialisations */
  Init();
}

//=================================================================================
// function : ~GenerationGUI_FillingDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
TransformationGUI_ExtensionDlg::~TransformationGUI_ExtensionDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void TransformationGUI_ExtensionDlg::Init()
{
  /* init variables */
  showOnlyPreviewControl();
  myEditCurrentArgument = myEditObjName;

  double aStep = 0.1;
  /* min, max, step and decimals for spin boxes & initial values */
  initSpinBox(myUMinSpinBox, -MAX_NUMBER, MAX_NUMBER, aStep, "parametric_precision");
  initSpinBox(myUMaxSpinBox, -MAX_NUMBER, MAX_NUMBER, aStep, "parametric_precision");
  initSpinBox(myVMinSpinBox, -MAX_NUMBER, MAX_NUMBER, aStep, "parametric_precision");
  initSpinBox(myVMaxSpinBox, -MAX_NUMBER, MAX_NUMBER, aStep, "parametric_precision");

  myUMinSpinBox->setValue(0.);
  myUMaxSpinBox->setValue(1.);
  myVMinSpinBox->setValue(0.);
  myVMaxSpinBox->setValue(1.);

  /* signals and slots connections */
  connect(myGeomGUI, SIGNAL(SignalDeactivateActiveDialog()), this, SLOT(DeactivateActiveDialog()));
  connect(myGeomGUI, SIGNAL(SignalCloseAllDialogs()),        this, SLOT(ClickOnCancel()));

  connect(this, SIGNAL(constructorsClicked(int)), this, SLOT(ConstructorsClicked(int)));

  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));
  connect(mySelButton,   SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(myUMinSpinBox, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(myUMaxSpinBox, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(myVMinSpinBox, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(myVMaxSpinBox, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));

  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), this, SLOT(SetDoubleSpinBoxStep(double)));

  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(),
           SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  initName(tr("GEOM_EXTENSION_EDGE_NAME"));
  resize(100,100);

  ConstructorsClicked(0);
}

//=================================================================================
// function : SetDoubleSpinBoxStep()
// purpose  : Double spin box management
//=================================================================================
void TransformationGUI_ExtensionDlg::SetDoubleSpinBoxStep(double step)
{
  myUMinSpinBox->setSingleStep(step);
  myUMaxSpinBox->setSingleStep(step);
  myVMinSpinBox->setSingleStep(step);
  myVMaxSpinBox->setSingleStep(step);
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void TransformationGUI_ExtensionDlg::ConstructorsClicked (int constructorId)
{
  switch (constructorId) {
  case 0: // Extend edge
    {
      TColStd_MapOfInteger aMap;

      aMap.Add(GEOM_EDGE);
      globalSelection(aMap);

      myObjLbl->setText(tr("GEOM_EDGE"));
      myUMinLbl->setText(tr("GEOM_EXTENSION_MIN"));
      myUMaxLbl->setText(tr("GEOM_EXTENSION_MAX"));
      myVMinLbl->hide();
      myVMaxLbl->hide();
      myVMinSpinBox->hide();
      myVMaxSpinBox->hide();

      initName(tr("GEOM_EXTENSION_EDGE_NAME"));

      break;
    }
  case 1: // Extend face
    {
      TColStd_MapOfInteger aMap;

      aMap.Add(GEOM_FACE);
      globalSelection(aMap);

      myObjLbl->setText(tr("GEOM_FACE"));
      myUMinLbl->setText(tr("GEOM_EXTENSION_MIN_U"));
      myUMaxLbl->setText(tr("GEOM_EXTENSION_MAX_U"));
      myVMinLbl->show();
      myVMaxLbl->show();
      myVMinSpinBox->show();
      myVMaxSpinBox->show();

      initName(tr("GEOM_EXTENSION_FACE_NAME"));

      break;
    }
  default:
    break;
  }

  myBase.nullify();
  qApp->processEvents();
  updateGeometry();
  resize(minimumSizeHint());

  SelectionIntoArgument();
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void TransformationGUI_ExtensionDlg::ClickOnOk()
{
  setIsApplyAndClose(true);
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool TransformationGUI_ExtensionDlg::ClickOnApply()
{
  if (!onAccept())
    return false;

  initName();

  return true;
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void TransformationGUI_ExtensionDlg::SelectionIntoArgument()
{
  erasePreview();
  myEditCurrentArgument->setText("");

  if (myEditCurrentArgument == myEditObjName) {
    const TopAbs_ShapeEnum aType =
      getConstructorId() == 0 ? TopAbs_EDGE: TopAbs_FACE;
    GEOM::GeomObjPtr aSelectedObject = getSelected(aType);

    if (aSelectedObject) {
      myEditCurrentArgument->setText(GEOMBase::GetName(aSelectedObject.get()));
      myBase = aSelectedObject;
    } else {
      myBase.nullify();
    }
  }

  processPreview();
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void TransformationGUI_ExtensionDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  if (send == mySelButton) {
    myEditObjName->setFocus();
    myEditCurrentArgument = myEditObjName;
    SelectionIntoArgument();
  }
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void TransformationGUI_ExtensionDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(),
           SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  ConstructorsClicked(getConstructorId());
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void TransformationGUI_ExtensionDlg::enterEvent(QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void TransformationGUI_ExtensionDlg::ValueChangedInSpinBox(double newValue)
{
  processPreview();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr TransformationGUI_ExtensionDlg::createOperation()
{
  return getGeomEngine()->GetIShapesOperations(getStudyId());
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool TransformationGUI_ExtensionDlg::isValid(QString& msg)
{
  bool ok = (myUMinSpinBox->isValid(msg, !IsPreview()) &&
             myUMaxSpinBox->isValid(msg, !IsPreview()) &&
             myVMinSpinBox->isValid(msg, !IsPreview()) &&
             myVMaxSpinBox->isValid(msg, !IsPreview()) &&
             myBase);
  return ok;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool TransformationGUI_ExtensionDlg::execute(ObjectList& objects)
{
  bool res = false;
  GEOM::GEOM_IShapesOperations_var anOper =
    GEOM::GEOM_IShapesOperations::_narrow(getOperation());
  GEOM::GEOM_Object_var anObj;

  switch (getConstructorId()) {
  case 0:
    anObj = anOper->ExtendEdge(myBase.get(), myUMinSpinBox->value(),
                               myUMaxSpinBox->value());
    if (!anObj->_is_nil() && !IsPreview())
    {
	  QStringList aParameters;
	  aParameters << myUMinSpinBox->text();
	  aParameters << myUMaxSpinBox->text();
	  anObj->SetParameters(aParameters.join(":").toUtf8().constData());
    }
    res   = true;
    break;
  case 1:
    anObj = anOper->ExtendFace(myBase.get(), myUMinSpinBox->value(),
                               myUMaxSpinBox->value(),
                               myVMinSpinBox->value(),
                               myVMaxSpinBox->value());
    if (!anObj->_is_nil() && !IsPreview())
    {
	  QStringList aParameters;
	  aParameters << myUMinSpinBox->text();
	  aParameters << myUMaxSpinBox->text();
	  aParameters << myVMinSpinBox->text();
	  aParameters << myVMaxSpinBox->text();
	  anObj->SetParameters(aParameters.join(":").toUtf8().constData());
    }
    res   = true;
    break;
  default:
    break;
  }

  if (!anObj->_is_nil()) {
    objects.push_back(anObj._retn());
  }

  return res;
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> TransformationGUI_ExtensionDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res;
  res << myBase;
  return res;
}
