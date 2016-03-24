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
// File   : GenerationGUI_ThicknessDlg.cxx
// Author : Sergey KHROMOV, Open CASCADE S.A.S.

#include "GenerationGUI_ThicknessDlg.h"

#include <GEOMBase.h>
#include <GeometryGUI.h>

#include <LightApp_SelectionMgr.h>
#include <SalomeApp_Application.h>
#include <SalomeApp_DoubleSpinBox.h>
#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>

#include <QApplication>
#include <QCheckBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>

#include <TopExp.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <Precision.hxx>

#define GEOM_SOLID_THICK     0
#define GEOM_SOLID_HOLLOWED  1

//==============================================================================
// class    : GenerationGUI_ThicknessDlg()
// purpose  : 
//==============================================================================
GenerationGUI_ThicknessDlg::GenerationGUI_ThicknessDlg
                            (GeometryGUI* theGeometryGUI, QWidget* parent,
                             bool modal, Qt::WindowFlags fl)
  : GEOMBase_Skeleton(theGeometryGUI, parent, modal, fl),
    myFacesLbl       (0),
    myObjSelBtn      (0),
    myFacesSelBtn    (0),
    myObjEdit        (0),
    myFacesEdit      (0),
    myThicknessSpin  (0),
    myInsideCheck    (0)
{
  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
  QPixmap image0(aResMgr->loadPixmap("GEOM", tr("ICON_SELECT")));
  QPixmap image1(aResMgr->loadPixmap("GEOM", tr("ICON_DLG_THICKNESS")));
  QPixmap image2
    (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_THICKNESS_HOLLOWED")));

  setWindowTitle(tr("GEOM_THICKNESS_TITLE"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_THICKNESS"));
  mainFrame()->RadioButton1->setIcon(image1);
  mainFrame()->RadioButton2->setIcon(image2);

  mainFrame()->RadioButton3->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton3->close();

  mainFrame()->RadioButton1->setChecked(true);

  QGroupBox   *aMainGrp      = new QGroupBox (tr("GEOM_ARGUMENTS"), this);
  QGridLayout *aParamsLayout = new QGridLayout(aMainGrp);
  QLabel      *anObjLbl      = new QLabel (tr("GEOM_OBJECT"), aMainGrp);
  QLabel      *aThicknessLbl = new QLabel (tr("GEOM_THICKNESS"), aMainGrp);
  
  myFacesLbl      = new QLabel (tr("GEOM_FACES"), aMainGrp);
  myObjSelBtn     = new QPushButton(aMainGrp);
  myFacesSelBtn   = new QPushButton(aMainGrp);
  myObjEdit       = new QLineEdit(aMainGrp);
  myFacesEdit     = new QLineEdit(aMainGrp);
  myThicknessSpin = new SalomeApp_DoubleSpinBox(aMainGrp);
  myInsideCheck   = new QCheckBox(tr("GEOM_TOWARDS_INSIDE"), aMainGrp);

  myObjSelBtn->setIcon(image0);
  myFacesSelBtn->setIcon(image0);
  myObjEdit->setReadOnly(true);
  myObjEdit->setSizePolicy
    (QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
  myFacesEdit->setReadOnly(true);
  myFacesEdit->setSizePolicy
    (QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));

  aParamsLayout->setMargin(9);
  aParamsLayout->setSpacing(6);
  aParamsLayout->addWidget(anObjLbl,        0, 0);
  aParamsLayout->addWidget(myObjSelBtn,     0, 1);
  aParamsLayout->addWidget(myObjEdit,       0, 2);
  aParamsLayout->addWidget(myFacesLbl,      1, 0);
  aParamsLayout->addWidget(myFacesSelBtn,   1, 1);
  aParamsLayout->addWidget(myFacesEdit,     1, 2);
  aParamsLayout->addWidget(aThicknessLbl,   2, 0);
  aParamsLayout->addWidget(myThicknessSpin, 2, 1, 1, 2);
  aParamsLayout->addWidget(myInsideCheck,   3, 0, 1, 3);

  QVBoxLayout* layout = new QVBoxLayout(centralWidget());

  layout->setMargin(0);
  layout->setSpacing(6);
  layout->addWidget(aMainGrp);

  myHelpFileName = "create_thickness_page.html";

  /* Initialisation */
  Init();
}

//==============================================================================
// function : ~GenerationGUI_ThicknessDlg()
// purpose  : 
//==============================================================================
GenerationGUI_ThicknessDlg::~GenerationGUI_ThicknessDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

//==============================================================================
// function : Init()
// purpose  :
//==============================================================================
void GenerationGUI_ThicknessDlg::Init()
{
  // Get setting of step value from file configuration
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  double aStep = resMgr->doubleValue("Geometry", "SettingsGeomStep", 100);
  double aThickness    = 10.;
  double aThicknessMin = Precision::Confusion() * 10.0;

  initSpinBox(myThicknessSpin, aThicknessMin,
              COORD_MAX, aStep, "length_precision");
  myThicknessSpin->setValue(aThickness);
  myObject.nullify();
  myFaces.clear();

  showOnlyPreviewControl();
    
  // signals and slots connections
  connect(buttonOk(),      SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(),   SIGNAL(clicked()), this, SLOT(ClickOnApply()));
  connect(this,            SIGNAL(constructorsClicked(int)),
          this,            SLOT(ConstructorsClicked(int)));
  connect(myObjSelBtn,     SIGNAL(clicked()),
          this,            SLOT(SetEditCurrentArgument()));
  connect(myFacesSelBtn,   SIGNAL(clicked()),
          this,            SLOT(SetEditCurrentArgument()));
  connect(myThicknessSpin, SIGNAL(valueChanged(double)),
          this,            SLOT(ValueChangedInSpinBox()));
  connect(myGeomGUI,       SIGNAL(SignalDefaultStepValueChanged(double)),
          this,            SLOT(SetDoubleSpinBoxStep(double)));
  connect(myInsideCheck,   SIGNAL(toggled(bool)),
          this,            SLOT(onChangeDirection(bool)));

  connect(myGeomGUI->getApp()->selectionMgr(),
          SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));

  initName(tr("GEOM_THICKNESS"));

  ConstructorsClicked(0);
}

//==============================================================================
// function : SetDoubleSpinBoxStep()
// purpose  : Double spin box management
//==============================================================================
void GenerationGUI_ThicknessDlg::SetDoubleSpinBoxStep (double step)
{
  myThicknessSpin->setSingleStep(step);
}

//==============================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//==============================================================================
void GenerationGUI_ThicknessDlg::ConstructorsClicked (int constructorId)
{
  switch (constructorId) {
  case GEOM_SOLID_THICK:
    myObjSelBtn->setDown(false);
    myFacesLbl->hide();
    myFacesSelBtn->hide();
    myFacesEdit->hide();
    myFaces.clear();
    break;
  case GEOM_SOLID_HOLLOWED:
    myObjSelBtn->setDown(true);
    myFacesLbl->show();
    myFacesSelBtn->show();
    myFacesEdit->show();
    break;
  default:
    break;
  }

  myEditCurrentArgument = myObjEdit;
  qApp->processEvents();
  updateGeometry();
  resize(minimumSizeHint());

  SelectionIntoArgument();
}

//==============================================================================
// function : ClickOnOk()
// purpose  :
//==============================================================================
void GenerationGUI_ThicknessDlg::ClickOnOk()
{
  setIsApplyAndClose(true);

  if (ClickOnApply()) {
    ClickOnCancel();
  }
}

//==============================================================================
// function : ClickOnApply()
// purpose  :
//==============================================================================
bool GenerationGUI_ThicknessDlg::ClickOnApply()
{
  if (!onAccept()) {
    return false;
  }

  initName();

  // activate selection and connect selection manager
  ConstructorsClicked(getConstructorId());

  return true;
}

//==============================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection is changed or on dialog initialization or
//            activation
//==============================================================================
void GenerationGUI_ThicknessDlg::SelectionIntoArgument()
{
  erasePreview();
  myEditCurrentArgument->setText("");

  if (myEditCurrentArgument == myObjEdit) {
    QList<TopAbs_ShapeEnum> aTypes;
    const int               anID = getConstructorId();

    if (anID == GEOM_SOLID_THICK) {
      aTypes << TopAbs_SHELL << TopAbs_FACE;
    } else if (anID == GEOM_SOLID_HOLLOWED) {
      aTypes << TopAbs_SOLID;
    }

    myObject = getSelected(aTypes);

    if (myObject) {
      QString aName = GEOMBase::GetName(myObject.get());

      myEditCurrentArgument->setText(aName);

      if (anID == GEOM_SOLID_HOLLOWED && myFaces.empty()) {
        myFacesSelBtn->click();
      }
    }
  } else if (myEditCurrentArgument == myFacesEdit) {
    myFaces.clear();
    myFaces = getSelected(TopAbs_FACE, -1);

    int i = myFaces.count();

    if (i == 1) {
      myEditCurrentArgument->setText(GEOMBase::GetName(myFaces.first().get()));
    } else if (i > 0) {
      myEditCurrentArgument->setText
        (QString::number( i ) + "_" + tr("GEOM_OBJECTS"));
    }
  }

  processPreview();
}

//==============================================================================
// function : SetEditCurrentArgument()
// purpose  :
//==============================================================================
void GenerationGUI_ThicknessDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  if (send == myObjSelBtn) {
    myFacesSelBtn->setDown(false);
    myEditCurrentArgument = myObjEdit;
    globalSelection(GEOM_ALLSHAPES);
  } else if (send == myFacesSelBtn) {
    myObjSelBtn->setDown(false);
    myEditCurrentArgument = myFacesEdit;
    globalSelection(); // close local contexts, if any
    localSelection(myObject.get(), TopAbs_FACE);
  }

  myEditCurrentArgument->setFocus();

  const int anID = getConstructorId();

  // after setFocus(), because it will be setDown(false) when loses focus
  send->setDown(anID == GEOM_SOLID_HOLLOWED);

  // seems we need it only to avoid preview disappearing, caused by selection
  // mode change
  processPreview();
}

//==============================================================================
// function : ActivateThisDialog()
// purpose  :
//==============================================================================
void GenerationGUI_ThicknessDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();

  connect(myGeomGUI->getApp()->selectionMgr(),
          SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));

  ConstructorsClicked(getConstructorId());
}

//==============================================================================
// function : enterEvent()
// purpose  : when mouse enter onto the QWidget
//==============================================================================
void GenerationGUI_ThicknessDlg::enterEvent (QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//==============================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//==============================================================================
void GenerationGUI_ThicknessDlg::ValueChangedInSpinBox()
{
  processPreview();
}

//==============================================================================
// function : createOperation
// purpose  :
//==============================================================================
GEOM::GEOM_IOperations_ptr GenerationGUI_ThicknessDlg::createOperation()
{
  return getGeomEngine()->GetI3DPrimOperations(getStudyId());
}

//==============================================================================
// function : isValid
// purpose  :
//==============================================================================
bool GenerationGUI_ThicknessDlg::isValid (QString& msg)
{
  bool isOk = false;

  switch (getConstructorId()) {
  case GEOM_SOLID_THICK:
    isOk = myThicknessSpin->isValid(msg, !IsPreview()) && myObject;
    break;
  case GEOM_SOLID_HOLLOWED:
    isOk = myThicknessSpin->isValid(msg, !IsPreview()) &&
           myObject && !myFaces.empty();
    break;
  default:
    break;
  }

  return isOk;
}

//==============================================================================
// function : onChangeDirection(bool)
// purpose  :
//==============================================================================
void GenerationGUI_ThicknessDlg::onChangeDirection(bool)
{
  processPreview();
}

//==============================================================================
// function : execute
// purpose  :
//==============================================================================
bool GenerationGUI_ThicknessDlg::execute (ObjectList& objects)
{
  GEOM::GEOM_Object_var            anObj;
  GEOM::GEOM_I3DPrimOperations_var anOper       =
    GEOM::GEOM_I3DPrimOperations::_narrow(getOperation());
  double                           aThickness   = myThicknessSpin->value();
  bool                             anInside = myInsideCheck->isChecked();
  GEOM::ListOfLong_var             anObjIDsList = new GEOM::ListOfLong();
  TopoDS_Shape                     aShape;

  if (GEOMBase::GetShape(myObject.get(), aShape)) {
    TopTools_IndexedMapOfShape aMainMap;
    QList<int>                 aListIDs;

    TopExp::MapShapes(aShape, aMainMap);

    for (int i = 0; i < myFaces.count(); i++) {
      TopoDS_Shape aFace;

      if (GEOMBase::GetShape(myFaces[i].get(), aFace)) {
        int anIndex = aMainMap.FindIndex(aFace);

        if (anIndex >= 0) {
          aListIDs << anIndex;
        }
      }
    }


    if (!aListIDs.empty()) {
      anObjIDsList->length(aListIDs.length());

      for (int i = 0; i < aListIDs.length(); i++) {
        anObjIDsList[i] = aListIDs[i];
      }
    }
  }

  anObj = anOper->MakeThickening
    (myObject.get(), anObjIDsList.in(), aThickness, true, anInside);
    
  if (!anObj->_is_nil()) {
    if (!IsPreview()) {
      QStringList aParameters;
      aParameters << myThicknessSpin->text();
      anObj->SetParameters(aParameters.join(":").toUtf8().constData());
    }
    objects.push_back(anObj._retn());
  }

  return true;
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> GenerationGUI_ThicknessDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res(myFaces);
  res << myObject;
  return res;
}
