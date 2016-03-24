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
// File   : GenerationGUI_PrismDlg.cxx
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.

#include "GenerationGUI_PrismDlg.h"

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
#include <Precision.hxx>

#include <GEOMImpl_Types.hxx>

//=================================================================================
// class    : GenerationGUI_PrismDlg()
// purpose  : Constructs a GenerationGUI_PrismDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
GenerationGUI_PrismDlg::GenerationGUI_PrismDlg (GeometryGUI* theGeometryGUI, QWidget* parent,
                                                bool modal, Qt::WindowFlags fl)
  : GEOMBase_Skeleton(theGeometryGUI, parent, modal, fl)
{
  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
  QPixmap image0 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_PRISM")));
  QPixmap image1 (aResMgr->loadPixmap("GEOM", tr("ICON_SELECT")));
  QPixmap image2 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_PRISM_2P")));
  QPixmap image3 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_PRISM_DXDYDZ")));

  setWindowTitle(tr("GEOM_EXTRUSION_TITLE"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_EXTRUSION"));
  mainFrame()->RadioButton1->setIcon(image0);
  mainFrame()->RadioButton2->setIcon(image2);
  mainFrame()->RadioButton3->setIcon(image3);

  mainFrame()->RadioButton1->setChecked(true);

  GroupVecH = new DlgRef_2Sel1Spin3Check1Spin(centralWidget());
  GroupVecH->GroupBox1->setTitle(tr("GEOM_EXTRUSION_BSV"));
  GroupVecH->TextLabel1->setText(tr("GEOM_BASE"));
  GroupVecH->TextLabel2->setText(tr("GEOM_VECTOR"));
  GroupVecH->TextLabel3->setText(tr("GEOM_HEIGHT"));
  GroupVecH->TextLabel4->setText(tr("GEOM_SCALE_FACTOR"));
  GroupVecH->PushButton1->setIcon(image1);
  GroupVecH->PushButton2->setIcon(image1);
  GroupVecH->LineEdit1->setReadOnly(true);
  GroupVecH->LineEdit2->setReadOnly(true);
  GroupVecH->CheckBox1->setText(tr("GEOM_BOTHWAY"));
  GroupVecH->CheckBox2->setText(tr("GEOM_REVERSE"));
  GroupVecH->CheckBox3->setText(tr("GEOM_SCALE_PRISM"));

  Group2Points = new DlgRef_3Sel2Check3Spin(centralWidget());
  Group2Points->GroupBox1->setTitle(tr("GEOM_EXTRUSION_BSV_2P"));
  Group2Points->TextLabel1->setText(tr("GEOM_BASE"));
  Group2Points->TextLabel2->setText(tr("GEOM_POINT_I").arg(1));
  Group2Points->TextLabel3->setText(tr("GEOM_POINT_I").arg(2));
  Group2Points->TextLabel4->setText(tr("GEOM_SCALE_FACTOR"));
  Group2Points->PushButton1->setIcon(image1);
  Group2Points->PushButton2->setIcon(image1);
  Group2Points->PushButton3->setIcon(image1);
  Group2Points->CheckBox1->setText(tr("GEOM_BOTHWAY"));
  Group2Points->CheckBox2->setText(tr("GEOM_SCALE_PRISM"));

  GroupDXDYDZ = new DlgRef_1Sel3Spin2Check1Spin(centralWidget());
  GroupDXDYDZ->GroupBox1->setTitle(tr("GEOM_EXTRUSION_DXDYDZ"));
  GroupDXDYDZ->TextLabel1->setText(tr("GEOM_BASE"));
  GroupDXDYDZ->PushButton1->setIcon(image1);
  GroupDXDYDZ->TextLabel2->setText(tr("GEOM_DX"));
  GroupDXDYDZ->TextLabel3->setText(tr("GEOM_DY"));
  GroupDXDYDZ->TextLabel4->setText(tr("GEOM_DZ"));
  GroupDXDYDZ->TextLabel5->setText(tr("GEOM_SCALE_FACTOR"));
  GroupDXDYDZ->CheckBox1->setText(tr("GEOM_BOTHWAY"));
  GroupDXDYDZ->CheckBox2->setText(tr("GEOM_SCALE_PRISM"));
  
  GroupThickening = new DlgRef_1Check1Spin1Check(centralWidget());
  GroupThickening->GroupBox1->setTitle(tr("GEOM_THICKENING"));
  GroupThickening->checkButton1->setText(tr("GEOM_ADD_THICKNESS")); // "Add thickness (edges or wires only)"
  GroupThickening->checkButton2->setText(tr("GEOM_TOWARDS_INSIDE")); // "Thicken towards the inside"
  GroupThickening->TextLabel1->setText(tr("GEOM_THICKNESS"));

  QVBoxLayout* layout = new QVBoxLayout(centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
  layout->addWidget(GroupVecH);
  layout->addWidget(Group2Points);
  layout->addWidget(GroupDXDYDZ);
  layout->addWidget(GroupThickening);
  /***************************************************************/

  setHelpFileName("create_extrusion_page.html");

  Init();
}

//=================================================================================
// function : ~GenerationGUI_PrismDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
GenerationGUI_PrismDlg::~GenerationGUI_PrismDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void GenerationGUI_PrismDlg::Init()
{
  // Get setting of step value from file configuration
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  double step = resMgr->doubleValue("Geometry", "SettingsGeomStep", 100);

  // min, max, step and decimals for spin boxes & initial values
  double aScaleFactor = 2.0;
  double aScaleStep = 0.5;
  double aScaleMin = Precision::Confusion() * 10.0;
  
  double aThickness = 10;
  double aThicknessMin = Precision::Confusion() * 10.0;

  initSpinBox(GroupVecH->SpinBox_DX, COORD_MIN, COORD_MAX, step, "length_precision" );
  GroupVecH->SpinBox_DX->setValue(100.0);

  initSpinBox(GroupDXDYDZ->SpinBox_DX, COORD_MIN, COORD_MAX, step, "length_precision" );
  initSpinBox(GroupDXDYDZ->SpinBox_DY, COORD_MIN, COORD_MAX, step, "length_precision" );
  initSpinBox(GroupDXDYDZ->SpinBox_DZ, COORD_MIN, COORD_MAX, step, "length_precision" );
  GroupDXDYDZ->SpinBox_DX->setValue(0.0);
  GroupDXDYDZ->SpinBox_DY->setValue(0.0);
  GroupDXDYDZ->SpinBox_DZ->setValue(0.0);

  initSpinBox(GroupVecH->SpinBox_DY, aScaleMin, COORD_MAX, aScaleStep, "parametric_precision" );
  initSpinBox(Group2Points->SpinBox1, aScaleMin, COORD_MAX, aScaleStep, "parametric_precision" );
  initSpinBox(GroupDXDYDZ->SpinBox_SC, aScaleMin, COORD_MAX, aScaleStep, "parametric_precision" );

  GroupVecH->SpinBox_DY->setValue(aScaleFactor);
  Group2Points->SpinBox1->setValue(aScaleFactor);
  GroupDXDYDZ->SpinBox_SC->setValue(aScaleFactor);
  
  initSpinBox(GroupThickening->SpinBox_DX, aThicknessMin, COORD_MAX, step, "length_precision" );
  GroupThickening->SpinBox_DX->setValue(aThickness);

  // hide not used controls
  Group2Points->TextLabel5->hide();
  Group2Points->TextLabel6->hide();

  Group2Points->SpinBox2->hide();
  Group2Points->SpinBox3->hide();

  // init variables
  myEditCurrentArgument = GroupVecH->LineEdit1;
  GroupVecH->LineEdit1->setReadOnly(true);
  GroupVecH->LineEdit2->setReadOnly(true);

  Group2Points->LineEdit1->setReadOnly(true);
  Group2Points->LineEdit2->setReadOnly(true);
  Group2Points->LineEdit3->setReadOnly(true);

  GroupDXDYDZ->LineEdit1->setReadOnly(true);

  GroupVecH->LineEdit1->setText("");
  GroupVecH->LineEdit2->setText("");

  Group2Points->LineEdit1->setText("");
  Group2Points->LineEdit2->setText("");
  Group2Points->LineEdit3->setText("");

  GroupDXDYDZ->LineEdit1->setText("");
  
  GroupThickening->SpinBox_DX->hide();
  GroupThickening->checkButton2->hide();
  GroupThickening->TextLabel1->hide();
  
  GroupVecH->TextLabel4->hide();
  GroupVecH->SpinBox_DY->hide();

  Group2Points->TextLabel4->hide();
  Group2Points->SpinBox1->hide();

  GroupDXDYDZ->TextLabel5->hide();
  GroupDXDYDZ->SpinBox_SC->hide();

  myBaseObjects.clear();
  myPoint1.nullify();
  myPoint2.nullify();
  myVec.nullify();

  showOnlyPreviewControl();
    
  // signals and slots connections
  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(this,          SIGNAL(constructorsClicked(int)), this, SLOT(ConstructorsClicked(int)));

  connect(GroupVecH->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupVecH->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(GroupVecH->SpinBox_DX, SIGNAL(valueChanged(double)),    this, SLOT(ValueChangedInSpinBox()));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), this, SLOT(SetDoubleSpinBoxStep(double)));

  connect(GroupVecH->CheckBox1,  SIGNAL(toggled(bool)), this, SLOT(onBothway()));
  connect(GroupVecH->CheckBox2,  SIGNAL(toggled(bool)), this, SLOT(onReverse()));
  connect(GroupVecH->CheckBox3,  SIGNAL(toggled(bool)), this, SLOT(onScalePrism(bool)));
  connect(GroupVecH->SpinBox_DY, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox()));

  connect(Group2Points->PushButton1, SIGNAL(clicked()),       this, SLOT(SetEditCurrentArgument()));
  connect(Group2Points->PushButton2, SIGNAL(clicked()),       this, SLOT(SetEditCurrentArgument()));
  connect(Group2Points->PushButton3, SIGNAL(clicked()),       this, SLOT(SetEditCurrentArgument()));
  connect(Group2Points->CheckBox1,   SIGNAL(toggled(bool)),   this, SLOT(onBothway()));
  connect(Group2Points->CheckBox2,   SIGNAL(toggled(bool)),   this, SLOT(onScalePrism(bool)));
  connect(Group2Points->SpinBox1,    SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox()));

  connect(GroupDXDYDZ->PushButton1, SIGNAL(clicked()),            this, SLOT(SetEditCurrentArgument()));
  connect(GroupDXDYDZ->SpinBox_DX,  SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox()));
  connect(GroupDXDYDZ->SpinBox_DY,  SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox()));
  connect(GroupDXDYDZ->SpinBox_DZ,  SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox()));
  connect(GroupDXDYDZ->SpinBox_SC,  SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox()));
  connect(GroupDXDYDZ->CheckBox1,   SIGNAL(toggled(bool)),        this, SLOT(onBothway()));
  connect(GroupDXDYDZ->CheckBox2,   SIGNAL(toggled(bool)),        this, SLOT(onScalePrism(bool)));
  
  connect(GroupThickening->SpinBox_DX,   SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox()));
  connect(GroupThickening->checkButton1, SIGNAL(toggled(bool)),        this, SLOT(onAddThickening(bool)));
  connect(GroupThickening->checkButton2, SIGNAL(toggled(bool)),        this, SLOT(onChangeDirection(bool)));

  initName(tr("GEOM_EXTRUSION"));

  ConstructorsClicked(0);
  onBothway();
}

//=================================================================================
// function : SetDoubleSpinBoxStep()
// purpose  : Double spin box management
//=================================================================================
void GenerationGUI_PrismDlg::SetDoubleSpinBoxStep (double step)
{
  GroupVecH->SpinBox_DX->setSingleStep(step);
  GroupDXDYDZ->SpinBox_DX->setSingleStep(step);
  GroupDXDYDZ->SpinBox_DY->setSingleStep(step);
  GroupDXDYDZ->SpinBox_DZ->setSingleStep(step);
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void GenerationGUI_PrismDlg::ConstructorsClicked (int constructorId)
{
  switch (constructorId) {
  case 0:
    Group2Points->hide();
    GroupDXDYDZ->hide();
    GroupVecH->show();

    GroupVecH->LineEdit2->setText("");
    myVec.nullify();

    GroupVecH->PushButton1->click();
    break;
  case 1:
    GroupVecH->hide();
    Group2Points->show();
    GroupDXDYDZ->hide();

    Group2Points->LineEdit2->setText("");
    Group2Points->LineEdit3->setText("");
    myPoint1.nullify();
    myPoint2.nullify();

    Group2Points->PushButton1->click();
    break;
  case 2:
    GroupVecH->hide();
    Group2Points->hide();
    GroupDXDYDZ->show();

    GroupDXDYDZ->PushButton1->click();
    break;
  default:
    break;
  }

  qApp->processEvents();
  updateGeometry();
  resize(minimumSizeHint());

  SelectionIntoArgument();
  processPreview();
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void GenerationGUI_PrismDlg::ClickOnOk()
{
  setIsApplyAndClose( true );
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool GenerationGUI_PrismDlg::ClickOnApply()
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
void GenerationGUI_PrismDlg::SelectionIntoArgument()
{
  erasePreview();
  myEditCurrentArgument->setText("");

  if ( myEditCurrentArgument == GroupVecH->LineEdit1  ||
       myEditCurrentArgument == Group2Points->LineEdit1 ||
       myEditCurrentArgument == GroupDXDYDZ->LineEdit1 ) {
    myBaseObjects.clear();
    GroupThickening->checkButton1->setEnabled(true);
  
    QList<GEOM::GeomObjPtr> objects = getSelected( TopAbs_SHAPE, -1 );
    for ( int i = 0; i < objects.count(); i++ ) {
      GEOM::shape_type stype = objects[i]->GetMaxShapeType();
      if ( stype < GEOM::SHELL || stype > GEOM::VERTEX )
        continue;
      if ( stype > GEOM::EDGE || stype < GEOM::WIRE )
      {
        GroupThickening->checkButton1->setChecked(false);
        GroupThickening->checkButton1->setEnabled(false);
      }
        
      myBaseObjects << objects[i];
    }
    if ( !myBaseObjects.isEmpty() ) {
      QString aName = myBaseObjects.count() > 1 ? QString( "%1_objects").arg( myBaseObjects.count() ) : GEOMBase::GetName( myBaseObjects[0].get() );
      GroupVecH->LineEdit1->setText( aName );
      Group2Points->LineEdit1->setText( aName );
      GroupDXDYDZ->LineEdit1->setText( aName );
    }
    else {
      GroupVecH->LineEdit1->setText( "" );
      Group2Points->LineEdit1->setText( "" );
      GroupDXDYDZ->LineEdit1->setText( "" );
    }
  }
  else if ( myEditCurrentArgument == GroupVecH->LineEdit2 ) {
    myVec = getSelected( TopAbs_EDGE );
    if ( myVec ) {
      QString aName = GEOMBase::GetName( myVec.get() );
      myEditCurrentArgument->setText( aName );
      if ( myBaseObjects.isEmpty() )
        GroupVecH->PushButton1->click();
    }
  }
  if ( myEditCurrentArgument == Group2Points->LineEdit2 ) {
    myPoint1 = getSelected( TopAbs_VERTEX );
    if ( myPoint1 ) {
      QString aName = GEOMBase::GetName( myPoint1.get() );
      myEditCurrentArgument->setText( aName );
      if ( !myPoint2 )
        Group2Points->PushButton3->click();
      else if ( myBaseObjects.isEmpty() )
        Group2Points->PushButton1->click();
    }
  }
  if ( myEditCurrentArgument == Group2Points->LineEdit3 ) {
    myPoint2 = getSelected( TopAbs_VERTEX );
    if ( myPoint2 ) {
      QString aName = GEOMBase::GetName( myPoint2.get() );
      myEditCurrentArgument->setText( aName );
      if ( myBaseObjects.isEmpty() )
        Group2Points->PushButton1->click();
      else if ( !myPoint1 )
        Group2Points->PushButton2->click();
    }
  }

  processPreview();
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void GenerationGUI_PrismDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
  globalSelection(GEOM_ALLSHAPES);
  if (send == GroupVecH->PushButton1) {
    myEditCurrentArgument = GroupVecH->LineEdit1;
    GroupVecH->PushButton2->setDown(false);
    GroupVecH->LineEdit2->setEnabled(false);
  }
  else if (send == GroupVecH->PushButton2) {
    myEditCurrentArgument = GroupVecH->LineEdit2;
    GroupVecH->PushButton1->setDown(false);
    GroupVecH->LineEdit1->setEnabled(false);
    localSelection(TopAbs_EDGE);
  }
  else if (send == Group2Points->PushButton1) {
    myEditCurrentArgument = Group2Points->LineEdit1;
    Group2Points->PushButton2->setDown(false);
    Group2Points->PushButton3->setDown(false);
    Group2Points->LineEdit2->setEnabled(false);
    Group2Points->LineEdit3->setEnabled(false);
  }
  else if (send == Group2Points->PushButton2) {
    myEditCurrentArgument = Group2Points->LineEdit2;
    Group2Points->PushButton1->setDown(false);
    Group2Points->PushButton3->setDown(false);
    Group2Points->LineEdit1->setEnabled(false);
    Group2Points->LineEdit3->setEnabled(false);
    localSelection(TopAbs_VERTEX);
  }
  else if (send == Group2Points->PushButton3) {
    myEditCurrentArgument = Group2Points->LineEdit3;
    Group2Points->PushButton1->setDown(false);
    Group2Points->PushButton2->setDown(false);
    Group2Points->LineEdit1->setEnabled(false);
    Group2Points->LineEdit2->setEnabled(false);
    localSelection(TopAbs_VERTEX);
  }
  else   if (send == GroupDXDYDZ->PushButton1) {
    myEditCurrentArgument = GroupDXDYDZ->LineEdit1;
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
void GenerationGUI_PrismDlg::ActivateThisDialog()
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
void GenerationGUI_PrismDlg::enterEvent (QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void GenerationGUI_PrismDlg::ValueChangedInSpinBox()
{
  processPreview();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr GenerationGUI_PrismDlg::createOperation()
{
  return getGeomEngine()->GetI3DPrimOperations(getStudyId());
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool GenerationGUI_PrismDlg::isValid (QString& msg)
{
  bool ok = false;
  switch ( getConstructorId() ) {
  case 0:
    ok = GroupVecH->SpinBox_DX->isValid( msg, !IsPreview() ) &&
         !myBaseObjects.isEmpty() && myVec;
    break;
  case 1:
    ok = !myBaseObjects.isEmpty() && myPoint1 && myPoint2;
    break;
  case 2:
    ok = GroupDXDYDZ->SpinBox_DX->isValid( msg, !IsPreview() ) &&
         GroupDXDYDZ->SpinBox_DY->isValid( msg, !IsPreview() ) &&
         GroupDXDYDZ->SpinBox_DZ->isValid( msg, !IsPreview() ) &&
         !myBaseObjects.isEmpty();
    break;
  default:
    break;
  }

  return ok;
}

//=================================================================================
// function : onReverse()
// purpose  :
//=================================================================================
void GenerationGUI_PrismDlg::onReverse()
{
  double anOldValue = GroupVecH->SpinBox_DX->value();
  GroupVecH->SpinBox_DX->setValue(-anOldValue);
}

//=================================================================================
// function : onBothway()
// purpose  :
//=================================================================================
void GenerationGUI_PrismDlg::onBothway()
{
  GroupVecH->CheckBox2->setEnabled(!GroupVecH->CheckBox1->isChecked()); // is reversed
  GroupVecH->CheckBox3->setEnabled(!GroupVecH->CheckBox1->isChecked()); // is scaled
  GroupVecH->TextLabel4->setEnabled(!GroupVecH->CheckBox1->isChecked() &&
                                    GroupVecH->CheckBox3->isChecked()); // scale factor
  GroupVecH->SpinBox_DY->setEnabled(!GroupVecH->CheckBox1->isChecked() &&
                                    GroupVecH->CheckBox3->isChecked()); // scale factor

  Group2Points->CheckBox2->setEnabled(!Group2Points->CheckBox1->isChecked()); // is scaled
  Group2Points->TextLabel4->setEnabled(!Group2Points->CheckBox1->isChecked() &&
                                       Group2Points->CheckBox2->isChecked()); // scale factor
  Group2Points->SpinBox1->setEnabled(!Group2Points->CheckBox1->isChecked() &&
                                     Group2Points->CheckBox2->isChecked()); // scale factor

  GroupDXDYDZ->CheckBox2->setEnabled(!GroupDXDYDZ->CheckBox1->isChecked()); // is scaled
  GroupDXDYDZ->TextLabel5->setEnabled(!GroupDXDYDZ->CheckBox1->isChecked() &&
                                      GroupDXDYDZ->CheckBox2->isChecked()); // scale factor
  GroupDXDYDZ->SpinBox_SC->setEnabled(!GroupDXDYDZ->CheckBox1->isChecked() &&
                                      GroupDXDYDZ->CheckBox2->isChecked()); // scale factor

  processPreview();
}

//=================================================================================
// function : onScalePrism()
// purpose  :
//=================================================================================
void GenerationGUI_PrismDlg::onScalePrism(bool isChecked)
{
  GroupVecH->TextLabel4->setVisible(isChecked);
  GroupVecH->SpinBox_DY->setVisible(isChecked);

  Group2Points->TextLabel4->setVisible(isChecked);
  Group2Points->SpinBox1->setVisible(isChecked);

  GroupDXDYDZ->TextLabel5->setVisible(isChecked);
  GroupDXDYDZ->SpinBox_SC->setVisible(isChecked);
  
  GroupVecH->TextLabel4->setEnabled(isChecked);
  GroupVecH->SpinBox_DY->setEnabled(isChecked);

  Group2Points->TextLabel4->setEnabled(isChecked);
  Group2Points->SpinBox1->setEnabled(isChecked);

  GroupDXDYDZ->TextLabel5->setEnabled(isChecked);
  GroupDXDYDZ->SpinBox_SC->setEnabled(isChecked);

  processPreview();
}

//=================================================================================
// function : onAddThickening(bool)
// purpose  :
//=================================================================================
void GenerationGUI_PrismDlg::onAddThickening(bool isChecked)
{
  GroupThickening->SpinBox_DX->setVisible(isChecked);
  GroupThickening->checkButton2->setVisible(isChecked);
  GroupThickening->TextLabel1->setVisible(isChecked);
 
  updateGeometry();
  resize(minimumSizeHint());
  processPreview();
}

//=================================================================================
// function : onChangeDirection(bool)
// purpose  :
//=================================================================================
void GenerationGUI_PrismDlg::onChangeDirection(bool isChecked)
{
  processPreview();
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool GenerationGUI_PrismDlg::execute (ObjectList& objects)
{
  QStringList aParameters;
  GEOM::GEOM_Object_var anObj;

  GEOM::GEOM_I3DPrimOperations_var anOper = GEOM::GEOM_I3DPrimOperations::_narrow(getOperation());
  GEOM::GEOM_I3DPrimOperations_var anotherOper = GEOM::GEOM_I3DPrimOperations::_narrow(getOperation());

  for (int i = 0; i < myBaseObjects.count(); i++) {
    switch (getConstructorId()) {
    case 0:
      if (GroupVecH->CheckBox1->isChecked())
        anObj = anOper->MakePrismVecH2Ways(myBaseObjects[i].get(), myVec.get(),
                                           GroupVecH->SpinBox_DX->value());
      else {
        if (GroupVecH->CheckBox3->isChecked())
          anObj = anOper->MakePrismVecHWithScaling(myBaseObjects[i].get(), myVec.get(),
                                                   GroupVecH->SpinBox_DX->value(),
                                                   GroupVecH->SpinBox_DY->value());
        else
          anObj = anOper->MakePrismVecH(myBaseObjects[i].get(), myVec.get(),
                                        GroupVecH->SpinBox_DX->value());
      }

      if (!anObj->_is_nil() && !IsPreview()) {
        aParameters << GroupVecH->SpinBox_DX->text();
        anObj->SetParameters(aParameters.join(":").toLatin1().constData());
      }
      break;
    case 1:
      if (Group2Points->CheckBox1->isChecked())
        anObj = anOper->MakePrismTwoPnt2Ways(myBaseObjects[i].get(), myPoint1.get(), myPoint2.get());
      else {
        if (Group2Points->CheckBox2->isChecked())
          anObj = anOper->MakePrismTwoPntWithScaling(myBaseObjects[i].get(), myPoint1.get(),
                                                     myPoint2.get(), Group2Points->SpinBox1->value());
        else
          anObj = anOper->MakePrismTwoPnt(myBaseObjects[i].get(), myPoint1.get(), myPoint2.get());
      }
      break;
    case 2:
      double dx = GroupDXDYDZ->SpinBox_DX->value();
      double dy = GroupDXDYDZ->SpinBox_DY->value();
      double dz = GroupDXDYDZ->SpinBox_DZ->value();

      if (GroupDXDYDZ->CheckBox1->isChecked())
        anObj = anOper->MakePrismDXDYDZ2Ways(myBaseObjects[i].get(), dx, dy, dz);
      else {
        if (GroupDXDYDZ->CheckBox2->isChecked())
          anObj = anOper->MakePrismDXDYDZWithScaling(myBaseObjects[i].get(), dx, dy, dz,
                                                     GroupDXDYDZ->SpinBox_SC->value());
        else
          anObj = anOper->MakePrismDXDYDZ(myBaseObjects[i].get(), dx, dy, dz);
      }

      if (!anObj->_is_nil() && !IsPreview()) {
        aParameters << GroupDXDYDZ->SpinBox_DX->text();
        aParameters << GroupDXDYDZ->SpinBox_DY->text();
        aParameters << GroupDXDYDZ->SpinBox_DZ->text();
        anObj->SetParameters(aParameters.join(":").toLatin1().constData());
      }
      break;
    }
    
    if(GroupThickening->checkButton1->isChecked())
    { 
      double aThickness = GroupThickening->SpinBox_DX->value();
      bool isInside = GroupThickening->checkButton2->isChecked();

      GEOM::ListOfLong_var anArray = new GEOM::ListOfLong;

      anObj = anotherOper->MakeThickening
        (anObj, anArray, aThickness, /*copy=*/false, isInside);

      if (!anObj->_is_nil() && !IsPreview())
        anObj->SetParameters(GroupThickening->SpinBox_DX->text().toUtf8().constData());
    }
    
    if (!anObj->_is_nil())
      objects.push_back(anObj._retn());
  }

  return true;
}

//=================================================================================
// function : addSubshapeToStudy
// purpose  : virtual method to add new SubObjects if local selection
//=================================================================================
void GenerationGUI_PrismDlg::addSubshapesToStudy()
{
  switch (getConstructorId()) {
  case 0:
    GEOMBase::PublishSubObject( myVec.get() );
    break;
  case 1:
    GEOMBase::PublishSubObject( myPoint1.get() );
    GEOMBase::PublishSubObject( myPoint2.get() );
    break;
  default:
    break;
  }
}

//=================================================================================
// function : extractPrefix
// purpose  : redefined from GEOMBase_Helper class
//=================================================================================
bool GenerationGUI_PrismDlg::extractPrefix() const
{
  return myBaseObjects.count() > 1;
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> GenerationGUI_PrismDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res(myBaseObjects);
  res << myVec << myPoint1 << myPoint2 ;
  return res;
}
