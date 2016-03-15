// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "AdvancedGUI_PipeTShapeDlg.h"

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(AdvancedGEOM)

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_OverrideCursor.h>
#include <LightApp_SelectionMgr.h>
#include <SalomeApp_Application.h>
#include <SalomeApp_Tools.h>

// OCCT Includes
#include <BRep_Tool.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <gp_Pnt.hxx>

#include <QTabWidget>

#include <GEOMImpl_Types.hxx>

//=================================================================================
// Constructor
//=================================================================================
AdvancedGUI_PipeTShapeDlg::AdvancedGUI_PipeTShapeDlg (GeometryGUI* theGeometryGUI, QWidget* parent)
  : GEOMBase_Skeleton(theGeometryGUI, parent, false)
{
  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
  //QPixmap imageOp  = aResMgr->loadPixmap("GEOM", tr("ICON_DLG_PIPETSHAPE"));
  QPixmap imageSel = aResMgr->loadPixmap("GEOM", tr("ICON_SELECT"));
  QPixmap imageImp = aResMgr->loadPixmap("GEOM", tr("ICO_PIPETSHAPE_IMPORT"));
  imagePipeTShape  = aResMgr->loadPixmap("GEOM", tr("DLG_PIPETSHAPE"));
  imageReduction   = aResMgr->loadPixmap("GEOM", tr("IMG_PIPETSHAPE_SECT"));

  setWindowTitle(tr("GEOM_PIPE_TSHAPE_TITLE"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->GroupConstructors->close();

  QTabWidget* aTabWidget = new QTabWidget (centralWidget());

  QGridLayout* myMainLayout = new QGridLayout (centralWidget());
  myMainLayout->setMargin(0);
  myMainLayout->setSpacing(6);

  // Main parameters
  QGroupBox* GroupMain = new QGroupBox();
  QGridLayout* layoutGroupMain = new QGridLayout (GroupMain);

  MainTubeGroupParams = new DlgRef_3Spin();
  MainTubeGroupParams->GroupBox1->setTitle(tr("GEOM_PIPE_TSHAPE_MPIPE"));
  MainTubeGroupParams->TextLabel1->setText(tr("GEOM_PIPE_TSHAPE_R"));
  MainTubeGroupParams->TextLabel2->setText(tr("GEOM_PIPE_TSHAPE_W"));
  MainTubeGroupParams->TextLabel3->setText(tr("GEOM_PIPE_TSHAPE_L"));
  MainTubeGroupParams->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum); // ??

  IncidentTubeGroupParams = new DlgRef_3Spin();
  IncidentTubeGroupParams->GroupBox1->setTitle(tr("GEOM_PIPE_TSHAPE_IPIPE"));
  IncidentTubeGroupParams->TextLabel1->setText(tr("GEOM_PIPE_TSHAPE_R"));
  IncidentTubeGroupParams->TextLabel2->setText(tr("GEOM_PIPE_TSHAPE_W"));
  IncidentTubeGroupParams->TextLabel3->setText(tr("GEOM_PIPE_TSHAPE_L"));

  ChamferGroupParams = new DlgRef_2Spin();
  ChamferGroupParams->GroupBox1->setCheckable(true);
  ChamferGroupParams->GroupBox1->setChecked(false);
  ChamferGroupParams->GroupBox1->setTitle(tr("GEOM_PIPE_TSHAPE_CHAMFER"));
  ChamferGroupParams->TextLabel1->setText(tr("GEOM_PIPE_TSHAPE_CHAMFER_H"));
  ChamferGroupParams->TextLabel2->setText(tr("GEOM_PIPE_TSHAPE_CHAMFER_W"));

  FilletGroupParams = new DlgRef_1Spin();
  FilletGroupParams->GroupBox1->setCheckable(true);
  FilletGroupParams->GroupBox1->setChecked(false);
  FilletGroupParams->GroupBox1->setTitle(tr("GEOM_PIPE_TSHAPE_FILLET"));
  FilletGroupParams->TextLabel1->setText(tr("GEOM_PIPE_TSHAPE_R"));

  HexMeshCheckBox = new QCheckBox();
  HexMeshCheckBox->setText(tr("GEOM_PIPE_TSHAPE_HEX"));
  HexMeshCheckBox->setChecked(true);

  LabelImgPipeTShape = new QLabel();
  LabelImgPipeTShape->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  LabelImgPipeTShape->setAlignment(Qt::AlignCenter);
  LabelImgPipeTShape->setMinimumSize(300, 300);

  layoutGroupMain->addWidget(MainTubeGroupParams, 0, 0, 1, 1);
  layoutGroupMain->addWidget(IncidentTubeGroupParams, 1, 0, 1, 1);
  layoutGroupMain->addWidget(ChamferGroupParams, 2, 0, 1, 1);
  layoutGroupMain->addWidget(FilletGroupParams, 3, 0, 1, 1);
  layoutGroupMain->addWidget(HexMeshCheckBox, 4, 0, 1, 1);

  layoutGroupMain->addWidget(LabelImgPipeTShape, 0, 1, 5, 1);
  layoutGroupMain->setColumnStretch(1, 1); // stretch of column 0 is supposed 0 by default

  // Thickness reduction parameters
  QGroupBox* GroupReduct = new QGroupBox();
  QGridLayout* layoutGroupReduct = new QGridLayout (GroupReduct);

  LReductionGroupParams = new AdvancedGUI_4Spin();
  LReductionGroupParams->GroupBox1->setCheckable(true);
  LReductionGroupParams->GroupBox1->setChecked(false);
  LReductionGroupParams->GroupBox1->setTitle(tr("GEOM_PIPE_TSHAPE_LEFT_TR"));
  LReductionGroupParams->TextLabel1->setText(tr("GEOM_PIPE_TSHAPE_TR_R").arg("L"));
  LReductionGroupParams->TextLabel2->setText(tr("GEOM_PIPE_TSHAPE_TR_W").arg("L"));
  LReductionGroupParams->TextLabel3->setText(tr("GEOM_PIPE_TSHAPE_TR_L_TRANS").arg("L"));
  LReductionGroupParams->TextLabel4->setText(tr("GEOM_PIPE_TSHAPE_TR_L_THIN").arg("L"));

  RReductionGroupParams = new AdvancedGUI_4Spin();
  RReductionGroupParams->GroupBox1->setCheckable(true);
  RReductionGroupParams->GroupBox1->setChecked(false);
  RReductionGroupParams->GroupBox1->setTitle(tr("GEOM_PIPE_TSHAPE_RIGHT_TR"));
  RReductionGroupParams->TextLabel1->setText(tr("GEOM_PIPE_TSHAPE_TR_R").arg("R"));
  RReductionGroupParams->TextLabel2->setText(tr("GEOM_PIPE_TSHAPE_TR_W").arg("R"));
  RReductionGroupParams->TextLabel3->setText(tr("GEOM_PIPE_TSHAPE_TR_L_TRANS").arg("R"));
  RReductionGroupParams->TextLabel4->setText(tr("GEOM_PIPE_TSHAPE_TR_L_THIN").arg("R"));

  IReductionGroupParams = new AdvancedGUI_4Spin();
  IReductionGroupParams->GroupBox1->setCheckable(true);
  IReductionGroupParams->GroupBox1->setChecked(false);
  IReductionGroupParams->GroupBox1->setTitle(tr("GEOM_PIPE_TSHAPE_INCI_TR"));
  IReductionGroupParams->TextLabel1->setText(tr("GEOM_PIPE_TSHAPE_TR_R").arg("I"));
  IReductionGroupParams->TextLabel2->setText(tr("GEOM_PIPE_TSHAPE_TR_W").arg("I"));
  IReductionGroupParams->TextLabel3->setText(tr("GEOM_PIPE_TSHAPE_TR_L_TRANS").arg("I"));
  IReductionGroupParams->TextLabel4->setText(tr("GEOM_PIPE_TSHAPE_TR_L_THIN").arg("I"));

  LabelImgReduction = new QLabel();
  LabelImgReduction->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  LabelImgReduction->setAlignment(Qt::AlignCenter);
  LabelImgReduction->setMinimumSize(300, 300);

  layoutGroupReduct->addWidget(LReductionGroupParams->GroupBox1, 0, 0, 1, 1);
  layoutGroupReduct->addWidget(RReductionGroupParams->GroupBox1, 1, 0, 1, 1);
  layoutGroupReduct->addWidget(IReductionGroupParams->GroupBox1, 2, 0, 1, 1);

  layoutGroupReduct->addWidget(LabelImgReduction, 0, 1, 3, 1);
  layoutGroupReduct->setColumnStretch(1, 1); // stretch of column 0 is supposed 0 by default

  // Position parameters
  QGroupBox* GroupPos = new QGroupBox();
  QVBoxLayout* layoutGroupPos = new QVBoxLayout (GroupPos);

  JunctionPointsSel = new DlgRef_6Sel();
  JunctionPointsSel->GroupBox1->setTitle(tr("GEOM_PIPE_TSHAPE_POSITION"));
  JunctionPointsSel->GroupBox1->setCheckable(true);
  JunctionPointsSel->GroupBox1->setChecked(false);
  JunctionPointsSel->PushButton1->setIcon(imageSel);
  JunctionPointsSel->LineEdit1->setReadOnly(true);
  JunctionPointsSel->LineEdit1->setText("");
  JunctionPointsSel->TextLabel1->setText(tr("GEOM_PIPE_TSHAPE_POSITION_P1"));

  JunctionPointsSel->PushButton2->setIcon(imageSel);
  JunctionPointsSel->LineEdit2->setReadOnly(true);
  JunctionPointsSel->LineEdit2->setText("");
  JunctionPointsSel->TextLabel2->setText(tr("GEOM_PIPE_TSHAPE_POSITION_P2"));

  JunctionPointsSel->PushButton3->setIcon(imageSel);
  JunctionPointsSel->LineEdit3->setReadOnly(true);
  JunctionPointsSel->LineEdit3->setText("");
  JunctionPointsSel->TextLabel3->setText(tr("GEOM_PIPE_TSHAPE_POSITION_P3"));

  JunctionPointsSel->PushButton4->setIcon(imageImp);
  JunctionPointsSel->LineEdit4->setReadOnly(true);
  JunctionPointsSel->LineEdit4->setText("");
  JunctionPointsSel->TextLabel4->setText(tr("GEOM_PIPE_TSHAPE_POSITION_LBL_L1"));

  JunctionPointsSel->PushButton5->setIcon(imageImp);
  JunctionPointsSel->LineEdit5->setReadOnly(true);
  JunctionPointsSel->LineEdit5->setText("");
  JunctionPointsSel->TextLabel5->setText(tr("GEOM_PIPE_TSHAPE_POSITION_LBL_L2"));

  JunctionPointsSel->PushButton6->setAttribute(Qt::WA_DeleteOnClose);
  JunctionPointsSel->PushButton6->close();
  JunctionPointsSel->LineEdit6->setAttribute(Qt::WA_DeleteOnClose);
  JunctionPointsSel->LineEdit6->close();
  JunctionPointsSel->TextLabel6->setAttribute(Qt::WA_DeleteOnClose);
  JunctionPointsSel->TextLabel6->close();

  layoutGroupPos->addWidget(JunctionPointsSel);
  layoutGroupPos->addStretch();

  //aTabWidget->addTab(GroupMain, imageOp, tr("GEOM_PIPETSHAPE_GROUPMAIN"));
  aTabWidget->addTab(GroupMain, tr("GEOM_PIPETSHAPE_GROUPMAIN"));
  aTabWidget->addTab(GroupReduct, tr("GEOM_PIPETSHAPE_GROUPREDUCT"));
  aTabWidget->addTab(GroupPos, tr("GEOM_PIPETSHAPE_GROUPPOS"));

  myMainLayout->addWidget(aTabWidget, 0, 0, 1, 1);
  connect(aTabWidget, SIGNAL(currentChanged(int)), this, SLOT(currentChanged(int)));
  /***************************************************************/

  setHelpFileName("create_pipetshape_page.html");

  Init();
}

//=================================================================================
// Destructor
//=================================================================================
AdvancedGUI_PipeTShapeDlg::~AdvancedGUI_PipeTShapeDlg() {
  // no need to delete child widgets, Qt does it all for us
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void AdvancedGUI_PipeTShapeDlg::Init()
{
  // Get setting of step value from file configuration
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  double step = resMgr->doubleValue("Geometry", "SettingsGeomStep", 100);
  double minValue = Precision::Approximation();

  myPoint1 = myPoint2 = myPoint3 = GEOM::GEOM_Object::_nil();
  myOkPoint1 = myOkPoint2 = myOkPoint3 = false;

  pipeTShapeGroupObjects.clear();

  // min, max, step and decimals for spin boxes
  initSpinBox(MainTubeGroupParams->SpinBox_DX, minValue, COORD_MAX, step, "length_precision");
  initSpinBox(MainTubeGroupParams->SpinBox_DY, minValue, COORD_MAX, step, "length_precision");
  initSpinBox(MainTubeGroupParams->SpinBox_DZ, minValue, COORD_MAX, step, "length_precision");
  initSpinBox(IncidentTubeGroupParams->SpinBox_DX, minValue, COORD_MAX, step, "length_precision");
  initSpinBox(IncidentTubeGroupParams->SpinBox_DY, minValue, COORD_MAX, step, "length_precision");
  initSpinBox(IncidentTubeGroupParams->SpinBox_DZ, minValue, COORD_MAX, step, "length_precision");
  initSpinBox(ChamferGroupParams->SpinBox_DX, minValue, COORD_MAX, step, "length_precision");
  initSpinBox(ChamferGroupParams->SpinBox_DY, minValue, COORD_MAX, step, "length_precision");
  initSpinBox(FilletGroupParams->SpinBox_DX, minValue, COORD_MAX, step, "length_precision");

  initSpinBox(LReductionGroupParams->SpinBox1, minValue, COORD_MAX, step, "length_precision");
  initSpinBox(LReductionGroupParams->SpinBox2, minValue, COORD_MAX, step, "length_precision");
  initSpinBox(LReductionGroupParams->SpinBox3, minValue, COORD_MAX, step, "length_precision");
  initSpinBox(LReductionGroupParams->SpinBox4, 0.,       COORD_MAX, step, "length_precision");

  initSpinBox(RReductionGroupParams->SpinBox1, minValue, COORD_MAX, step, "length_precision");
  initSpinBox(RReductionGroupParams->SpinBox2, minValue, COORD_MAX, step, "length_precision");
  initSpinBox(RReductionGroupParams->SpinBox3, minValue, COORD_MAX, step, "length_precision");
  initSpinBox(RReductionGroupParams->SpinBox4, 0.,       COORD_MAX, step, "length_precision");

  initSpinBox(IReductionGroupParams->SpinBox1, minValue, COORD_MAX, step, "length_precision");
  initSpinBox(IReductionGroupParams->SpinBox2, minValue, COORD_MAX, step, "length_precision");
  initSpinBox(IReductionGroupParams->SpinBox3, minValue, COORD_MAX, step, "length_precision");
  initSpinBox(IReductionGroupParams->SpinBox4, 0.,       COORD_MAX, step, "length_precision");

  // init variables
  MainTubeGroupParams->SpinBox_DX->setValue(80);
  MainTubeGroupParams->SpinBox_DY->setValue(20);
  MainTubeGroupParams->SpinBox_DZ->setValue(200);
  IncidentTubeGroupParams->SpinBox_DX->setValue(50);
  IncidentTubeGroupParams->SpinBox_DY->setValue(20);
  IncidentTubeGroupParams->SpinBox_DZ->setValue(200);
  ChamferGroupParams->SpinBox_DX->setValue(20);
  ChamferGroupParams->SpinBox_DY->setValue(10);
  FilletGroupParams->SpinBox_DX->setValue(20);

  LReductionGroupParams->SpinBox1->setValue(60);
  LReductionGroupParams->SpinBox2->setValue(30);
  LReductionGroupParams->SpinBox3->setValue(40);
  LReductionGroupParams->SpinBox4->setValue(20);

  RReductionGroupParams->SpinBox1->setValue(60);
  RReductionGroupParams->SpinBox2->setValue(30);
  RReductionGroupParams->SpinBox3->setValue(40);
  RReductionGroupParams->SpinBox4->setValue(20);

  IReductionGroupParams->SpinBox1->setValue(40);
  IReductionGroupParams->SpinBox2->setValue(10);
  IReductionGroupParams->SpinBox3->setValue(30);
  IReductionGroupParams->SpinBox4->setValue(15);

  CssNormal = QString("QDoubleSpinBox {");
  CssNormal.append(MainTubeGroupParams->SpinBox_DZ->styleSheet());
  CssNormal.append("}");
  CssNormal.append("\nQPushButton {");
  CssNormal.append(JunctionPointsSel->PushButton4->styleSheet());
  CssNormal.append("}");
  CssAcceptable = "QDoubleSpinBox, QPushButton {background-color: rgb(85, 170, 127)}";
  CssRefused = "QDoubleSpinBox, QPushButton {background-color: rgb(255, 0, 0)}";

  showOnlyPreviewControl();

  // Signal/slot connections
  connect(buttonOk(), SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), this, SLOT(SetDoubleSpinBoxStep(double)));
  //connect(this, SIGNAL(constructorsClicked(int)), this, SLOT(ConstructorsClicked(int)));
  // ValueChangedInSpinBox
  connect(MainTubeGroupParams->SpinBox_DX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(MainTubeGroupParams->SpinBox_DY, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(MainTubeGroupParams->SpinBox_DZ, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(IncidentTubeGroupParams->SpinBox_DX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(IncidentTubeGroupParams->SpinBox_DY, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(IncidentTubeGroupParams->SpinBox_DZ, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(ChamferGroupParams->SpinBox_DX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(ChamferGroupParams->SpinBox_DY, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(FilletGroupParams->SpinBox_DX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  // ChamferOrFillet
  connect(ChamferGroupParams->GroupBox1, SIGNAL(toggled(bool)), this, SLOT(ChamferOrFillet(bool)));
  connect(FilletGroupParams->GroupBox1, SIGNAL(toggled(bool)), this, SLOT(ChamferOrFillet(bool)));
  // Thickness reduction:
  //   Left
  connect(LReductionGroupParams->GroupBox1, SIGNAL(toggled(bool)), this, SLOT(processPreview()));
  connect(LReductionGroupParams->SpinBox1, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(LReductionGroupParams->SpinBox2, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(LReductionGroupParams->SpinBox3, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(LReductionGroupParams->SpinBox4, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  //   Right
  connect(RReductionGroupParams->GroupBox1, SIGNAL(toggled(bool)), this, SLOT(processPreview()));
  connect(RReductionGroupParams->SpinBox1, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(RReductionGroupParams->SpinBox2, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(RReductionGroupParams->SpinBox3, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(RReductionGroupParams->SpinBox4, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  //   Incident pipe
  connect(IReductionGroupParams->GroupBox1, SIGNAL(toggled(bool)), this, SLOT(processPreview()));
  connect(IReductionGroupParams->SpinBox1, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(IReductionGroupParams->SpinBox2, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(IReductionGroupParams->SpinBox3, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(IReductionGroupParams->SpinBox4, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  // Position
  connect(JunctionPointsSel->GroupBox1, SIGNAL(toggled(bool)), this, SLOT(SetPosition(bool)));
  connect(JunctionPointsSel->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(JunctionPointsSel->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(JunctionPointsSel->PushButton3, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  // ApplyNewDimensions
  connect(JunctionPointsSel->PushButton4, SIGNAL(clicked()), this, SLOT(ApplyNewDimensions()));
  connect(JunctionPointsSel->PushButton5, SIGNAL(clicked()), this, SLOT(ApplyNewDimensions()));
  connect(QApplication::instance(), SIGNAL(focusChanged(QWidget*, QWidget*)), this, SLOT(UpdatePicture(QWidget*, QWidget*)));
  //@@ put additional signal/slot connections here @@//

  initName(tr("GEOM_PIPE_TSHAPE"));

  qApp->processEvents();
  updateGeometry();
  resize(minimumSizeHint());

  updateLabelImgPipeTShape();
  LabelImgReduction->setPixmap(imageReduction.scaled(LabelImgReduction->size(),
                                                     Qt::KeepAspectRatio,
                                                     Qt::SmoothTransformation));
  processPreview();
}

//=================================================================================
// function : currentChanged()
// purpose  : Tab widget management
//=================================================================================
void AdvancedGUI_PipeTShapeDlg::currentChanged (int tabId)
{
  if (tabId == 0 || tabId == 1)
    resizeEvent(NULL);
}

//=================================================================================
// function : ApplyNewDimensions()
// purpose  :
//=================================================================================
void AdvancedGUI_PipeTShapeDlg::ApplyNewDimensions()
{
  QPushButton* send = (QPushButton*) sender();

  bool ok = false;
  double newVal;
  if (send == JunctionPointsSel->PushButton4) {
    if (!JunctionPointsSel->LineEdit4->text().isEmpty()) {
      newVal = JunctionPointsSel->LineEdit4->text().toDouble(&ok);
      if (ok) {
        disconnect(MainTubeGroupParams->SpinBox_DZ, 0, this, 0);
        MainTubeGroupParams->SpinBox_DZ->setValue(newVal);
        connect(MainTubeGroupParams->SpinBox_DZ, SIGNAL(valueChanged( double )), this, SLOT(ValueChangedInSpinBox(double)));
        MainTubeGroupParams->SpinBox_DZ->setToolTip("");
        MainTubeGroupParams->SpinBox_DZ->setStyleSheet("background-color: rgb(255, 255, 255);");
        CheckCompatiblePosition(myPoint1, myPoint2, myPoint3, 0.01);
        processPreview();
      }
    }
  }
  else if (send == JunctionPointsSel->PushButton5) {
    if (!JunctionPointsSel->LineEdit5->text().isEmpty()) {
      newVal = JunctionPointsSel->LineEdit5->text().toDouble(&ok);
      if (ok) {
        disconnect(IncidentTubeGroupParams->SpinBox_DZ, 0, this, 0);
        IncidentTubeGroupParams->SpinBox_DZ->setValue(newVal);
        connect(IncidentTubeGroupParams->SpinBox_DZ, SIGNAL(valueChanged( double )), this, SLOT(ValueChangedInSpinBox(double)));
        IncidentTubeGroupParams->SpinBox_DZ->setToolTip("");
        IncidentTubeGroupParams->SpinBox_DZ->setStyleSheet("background-color: rgb(255, 255, 255);");
        CheckCompatiblePosition(myPoint1, myPoint2, myPoint3, 0.01);
        processPreview();
      }
    }
  }
}

//=================================================================================
// function : UpdatePicture()
// purpose  :
//=================================================================================
void AdvancedGUI_PipeTShapeDlg::UpdatePicture (QWidget* old, QWidget* now)
{
  if (ChamferGroupParams->GroupBox1->isChecked())
    if (now == MainTubeGroupParams->SpinBox_DX)
      imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE_CHAMFER_R1"));
    else if (now == MainTubeGroupParams->SpinBox_DY)
      imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE_CHAMFER_W1"));
    else if (now == MainTubeGroupParams->SpinBox_DZ)
      imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE_CHAMFER_L1"));
    else if (now == IncidentTubeGroupParams->SpinBox_DX)
      imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE_CHAMFER_R2"));
    else if (now == IncidentTubeGroupParams->SpinBox_DY)
      imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE_CHAMFER_W2"));
    else if (now == IncidentTubeGroupParams->SpinBox_DZ)
      imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE_CHAMFER_L2"));
    else if (now == ChamferGroupParams->SpinBox_DX)
      imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE_CHAMFER_H"));
    else if (now == ChamferGroupParams->SpinBox_DY)
      imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE_CHAMFER_W"));
    else
      imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE_CHAMFER"));
  else if (FilletGroupParams->GroupBox1->isChecked())
    if (now == MainTubeGroupParams->SpinBox_DX)
      imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE_FILLET_R1"));
    else if (now == MainTubeGroupParams->SpinBox_DY)
      imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE_FILLET_W1"));
    else if (now == MainTubeGroupParams->SpinBox_DZ)
      imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE_FILLET_L1"));
    else if (now == IncidentTubeGroupParams->SpinBox_DX)
      imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE_FILLET_R2"));
    else if (now == IncidentTubeGroupParams->SpinBox_DY)
      imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE_FILLET_W2"));
    else if (now == IncidentTubeGroupParams->SpinBox_DZ)
      imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE_FILLET_L2"));
    else if (now == FilletGroupParams->SpinBox_DX)
      imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE_FILLET_RF"));
    else
      imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE_FILLET"));
  else
    if (now == MainTubeGroupParams->SpinBox_DX)
      imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE_R1"));
    else if (now == MainTubeGroupParams->SpinBox_DY)
      imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE_W1"));
    else if (now == MainTubeGroupParams->SpinBox_DZ)
      imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE_L1"));
    else if (now == IncidentTubeGroupParams->SpinBox_DX)
      imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE_R2"));
    else if (now == IncidentTubeGroupParams->SpinBox_DY)
      imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE_W2"));
    else if (now == IncidentTubeGroupParams->SpinBox_DZ)
      imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE_L2"));
    else
      imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE"));

  updateLabelImgPipeTShape();
}

//=================================================================================
// function : SetPosition()
// purpose  :
//=================================================================================
void AdvancedGUI_PipeTShapeDlg::SetPosition (bool isChecked)
{
  if (isChecked) {
    erasePreview();
    JunctionPointsSel->LineEdit4->setText("");
    JunctionPointsSel->LineEdit5->setText("");
    //connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()), this, SLOT(
    //SelectionIntoArgument()));
    JunctionPointsSel->PushButton1->click();
    SelectionIntoArgument();
  }
  else {
    disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
    JunctionPointsSel->LineEdit4->setText("");
    JunctionPointsSel->LineEdit5->setText("");
    processPreview();
  }
}

//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void AdvancedGUI_PipeTShapeDlg::ValueChangedInSpinBox (double newValue)
{
  if (JunctionPointsSel->GroupBox1->isChecked() && myOkPoint1 && myOkPoint2 && myOkPoint3)
    CheckCompatiblePosition(myPoint1, myPoint2, myPoint3, 0.01);
  processPreview();
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void AdvancedGUI_PipeTShapeDlg::SelectionIntoArgument()
{
  erasePreview();
  //myEditCurrentArgument->setText("");

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  if (aSelList.Extent() != 1) {
    if (myEditCurrentArgument == JunctionPointsSel->LineEdit1)
      myOkPoint1 = false;
    else if (myEditCurrentArgument == JunctionPointsSel->LineEdit2)
      myOkPoint2 = false;
    else if (myEditCurrentArgument == JunctionPointsSel->LineEdit3)
      myOkPoint3 = false;
    return;
    myEditCurrentArgument->setText("");
  }

  // nbSel == 1
  if (aSelList.Extent() == 1) {
    Handle(SALOME_InteractiveObject) anIO = aSelList.First();
    GEOM::GEOM_Object_var aSelectedObject = GEOMBase::ConvertIOinGEOMObject(anIO);
    if (!CORBA::is_nil(aSelectedObject)) {
      QString aName = GEOMBase::GetName(aSelectedObject);
      TopoDS_Shape aShape;
      if (GEOMBase::GetShape(aSelectedObject, aShape, TopAbs_SHAPE) && !aShape.IsNull()) {

        TColStd_IndexedMapOfInteger aMap;
        aSelMgr->GetIndexes(anIO, aMap);
        if (aMap.Extent() == 1) { // Local Selection
          int anIndex = aMap(1);
          aName += QString(":vertex_%1").arg(anIndex);

          //Find SubShape Object in Father
          GEOM::GEOM_Object_var aFindedObject = GEOMBase_Helper::findObjectInFather(aSelectedObject, aName);

          if (aFindedObject->_is_nil()) { // Object not found in study
            GEOM::GEOM_IShapesOperations_var aShapesOp =
              getGeomEngine()->GetIShapesOperations(getStudyId());
            aSelectedObject = aShapesOp->GetSubShape(aSelectedObject, anIndex);
          }
          else {
            aSelectedObject = aFindedObject; // get Object from study
          }
          GEOMBase::GetShape(aSelectedObject, aShape, TopAbs_SHAPE);
        } else { // Global Selection
          if (aShape.ShapeType() != TopAbs_VERTEX) {
            aSelectedObject = GEOM::GEOM_Object::_nil();
            aName = "";
          }
        }
      }

      if (aShape.IsNull() || aShape.ShapeType() != TopAbs_VERTEX) {
        return;
      }
      myEditCurrentArgument->setText(aName);
      if (myEditCurrentArgument == JunctionPointsSel->LineEdit1) {
        myPoint1 = aSelectedObject;
        myOkPoint1 = true;
        if (!myOkPoint2)
          JunctionPointsSel->PushButton2->click();
      } else if (myEditCurrentArgument == JunctionPointsSel->LineEdit2) {
        myPoint2 = aSelectedObject;
        myOkPoint2 = true;
        if (!myOkPoint3)
          JunctionPointsSel->PushButton3->click();
      } else if (myEditCurrentArgument == JunctionPointsSel->LineEdit3) {
        myPoint3 = aSelectedObject;
        myOkPoint3 = true;
        if (!myOkPoint1)
          JunctionPointsSel->PushButton1->click();
      }
    }
  }

  if (myOkPoint1 && myOkPoint2 && myOkPoint3) {
    CheckCompatiblePosition(myPoint1, myPoint2, myPoint3, 0.01);
    processPreview();
  }
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void AdvancedGUI_PipeTShapeDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*) sender();

  if (send == JunctionPointsSel->PushButton1) {
    myEditCurrentArgument = JunctionPointsSel->LineEdit1;
    JunctionPointsSel->PushButton2->setDown(false);
    JunctionPointsSel->LineEdit2->setEnabled(false);
    JunctionPointsSel->PushButton3->setDown(false);
    JunctionPointsSel->LineEdit3->setEnabled(false);
  }
  else if (send == JunctionPointsSel->PushButton2) {
    myEditCurrentArgument = JunctionPointsSel->LineEdit2;
    JunctionPointsSel->PushButton1->setDown(false);
    JunctionPointsSel->LineEdit1->setEnabled(false);
    JunctionPointsSel->PushButton3->setDown(false);
    JunctionPointsSel->LineEdit3->setEnabled(false);
  }
  else if (send == JunctionPointsSel->PushButton3) {
    myEditCurrentArgument = JunctionPointsSel->LineEdit3;
    JunctionPointsSel->PushButton1->setDown(false);
    JunctionPointsSel->LineEdit1->setEnabled(false);
    JunctionPointsSel->PushButton2->setDown(false);
    JunctionPointsSel->LineEdit2->setEnabled(false);
  }

  // enable line edit
  myEditCurrentArgument->setEnabled(true);
  myEditCurrentArgument->setFocus();
  myEditCurrentArgument->setText("");
  // after setFocus(), because it will be setDown(false) when loses focus
  send->setDown(true);

  JunctionPointsSel->LineEdit4->setText("");
  JunctionPointsSel->LineEdit5->setText("");

  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
  //globalSelection(GEOM_POINT);
  globalSelection(); // close local contexts, if any
  localSelection( TopAbs_VERTEX );
  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));
}

//=================================================================================
// function : SetDoubleSpinBoxStep()
// purpose  : Double spin box management
//=================================================================================
void AdvancedGUI_PipeTShapeDlg::SetDoubleSpinBoxStep (double step)
{
  MainTubeGroupParams->SpinBox_DX->setSingleStep(step);
  MainTubeGroupParams->SpinBox_DY->setSingleStep(step);
  MainTubeGroupParams->SpinBox_DZ->setSingleStep(step);
  IncidentTubeGroupParams->SpinBox_DX->setSingleStep(step);
  IncidentTubeGroupParams->SpinBox_DY->setSingleStep(step);
  IncidentTubeGroupParams->SpinBox_DZ->setSingleStep(step);
  ChamferGroupParams->SpinBox_DX->setSingleStep(step);
  ChamferGroupParams->SpinBox_DY->setSingleStep(step);
  FilletGroupParams->SpinBox_DX->setSingleStep(step);

  LReductionGroupParams->SpinBox1->setSingleStep(step);
  LReductionGroupParams->SpinBox2->setSingleStep(step);
  LReductionGroupParams->SpinBox3->setSingleStep(step);
  LReductionGroupParams->SpinBox4->setSingleStep(step);

  RReductionGroupParams->SpinBox1->setSingleStep(step);
  RReductionGroupParams->SpinBox2->setSingleStep(step);
  RReductionGroupParams->SpinBox3->setSingleStep(step);
  RReductionGroupParams->SpinBox4->setSingleStep(step);

  IReductionGroupParams->SpinBox1->setSingleStep(step);
  IReductionGroupParams->SpinBox2->setSingleStep(step);
  IReductionGroupParams->SpinBox3->setSingleStep(step);
  IReductionGroupParams->SpinBox4->setSingleStep(step);
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void AdvancedGUI_PipeTShapeDlg::ClickOnOk()
{
  setIsApplyAndClose( true );
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool AdvancedGUI_PipeTShapeDlg::ClickOnApply()
{
  if (!onAccept())
    return false;

  initName();

  if (!isApplyAndClose())
    processPreview();
  return true;
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void AdvancedGUI_PipeTShapeDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  //globalSelection( GEOM_POINT);
  //connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(
  //        currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
  if (myOkPoint1 && myOkPoint2 && myOkPoint3)
    CheckCompatiblePosition(myPoint1, myPoint2, myPoint3, 0.01);
  processPreview();
}

//=================================================================================
// function : enterEvent [REDEFINED]
// purpose  :
//=================================================================================
void AdvancedGUI_PipeTShapeDlg::enterEvent (QEvent*)
{
  if (!mainFrame()->GroupButtons->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : resizeEvent [REDEFINED]
// purpose  :
//=================================================================================
void AdvancedGUI_PipeTShapeDlg::resizeEvent (QResizeEvent */*event*/)
{
  QSize scaledSize1 = imagePipeTShape.size();
  scaledSize1.scale(LabelImgPipeTShape->size(), Qt::KeepAspectRatio);
  if (!LabelImgPipeTShape->pixmap() || scaledSize1 != LabelImgPipeTShape->pixmap()->size())
    updateLabelImgPipeTShape();

  QSize scaledSize2 = imageReduction.size();
  scaledSize2.scale(LabelImgReduction->size(), Qt::KeepAspectRatio);
  if (!LabelImgReduction->pixmap() || scaledSize2 != LabelImgReduction->pixmap()->size())
    LabelImgReduction->setPixmap(imageReduction.scaled(LabelImgReduction->size(),
                                                       Qt::KeepAspectRatio,
                                                       Qt::SmoothTransformation));
}

//=================================================================================
// function : updateLabelImgPipeTShape
// purpose  :
//=================================================================================
void AdvancedGUI_PipeTShapeDlg::updateLabelImgPipeTShape()
{
  LabelImgPipeTShape->setPixmap(imagePipeTShape.scaled(LabelImgPipeTShape->size(),
                                                       Qt::KeepAspectRatio,
                                                       Qt::SmoothTransformation));
}

//=================================================================================
// function : ChamferOrFillet()
// purpose  :
//=================================================================================
void AdvancedGUI_PipeTShapeDlg::ChamferOrFillet (bool)
{
  QGroupBox* send = (QGroupBox*) sender();

  if (send == ChamferGroupParams->GroupBox1) {
    if (send->isChecked()) {
      disconnect(FilletGroupParams->GroupBox1, SIGNAL(toggled(bool)), this, 0);
      FilletGroupParams->GroupBox1->setChecked(false);
      connect(FilletGroupParams->GroupBox1, SIGNAL(toggled(bool)), this, SLOT(ChamferOrFillet(bool)));
      imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE_CHAMFER"));
    }
    else
      imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE"));

    updateLabelImgPipeTShape();
    if (myOkPoint1 && myOkPoint2 && myOkPoint3)
      CheckCompatiblePosition(myPoint1, myPoint2, myPoint3, 0.01);
    processPreview();
  }
  else if (send == FilletGroupParams->GroupBox1) {
    if (send->isChecked()) {
      disconnect(ChamferGroupParams->GroupBox1, SIGNAL(toggled(bool)), this, 0);
      ChamferGroupParams->GroupBox1->setChecked(!send->isChecked());
      connect(ChamferGroupParams->GroupBox1, SIGNAL(toggled(bool)), this, SLOT(ChamferOrFillet(bool)));
      imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE_FILLET"));
    }
    else
      imagePipeTShape = SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("DLG_PIPETSHAPE"));

    updateLabelImgPipeTShape();
    if (myOkPoint1 && myOkPoint2 && myOkPoint3)
      CheckCompatiblePosition(myPoint1, myPoint2, myPoint3, 0.01);
    processPreview();
  }
}

//=================================================================================
// function : DisplayPreview()
// purpose  :
//=================================================================================
void AdvancedGUI_PipeTShapeDlg::DisplayPreview (const bool activate, const bool update,
                                                const bool toRemoveFromEngine, const double lineWidth,
                                                const int displayMode, const int color)
{
  QString msg;
  if (!isValid(msg)) {
    erasePreview(update);
    return;
  }

  erasePreview(false);

  try {
    SUIT_OverrideCursor wc;
    ObjectList objects;
    bool hexMeshState = HexMeshCheckBox->isChecked();
    HexMeshCheckBox->setChecked(false);
    if (!executeNoCheck(objects) || !getOperation()->IsDone()) {
      wc.suspend();
    }
    else {
      ObjectList::iterator it = objects.begin();
      GEOM::GEOM_Object_var obj = *it;
      displayPreview(obj, true, activate, false, lineWidth, displayMode, color);
      if (toRemoveFromEngine)
        obj->UnRegister();
    }
    HexMeshCheckBox->setChecked(hexMeshState);
  } catch (const SALOME::SALOME_Exception& e) {
    SalomeApp_Tools::QtCatchCorbaException(e);
  }

  if (update)
    updateViewer();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr AdvancedGUI_PipeTShapeDlg::createOperation()
{
  return getGeomEngine()->GetPluginOperations(getStudyId(), "AdvancedEngine");
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool AdvancedGUI_PipeTShapeDlg::isValid (QString& msg)
{
  bool ok = true;

  ok = MainTubeGroupParams->SpinBox_DX->isValid(msg, !IsPreview()) && ok;
  ok = MainTubeGroupParams->SpinBox_DY->isValid(msg, !IsPreview()) && ok;
  ok = MainTubeGroupParams->SpinBox_DZ->isValid(msg, !IsPreview()) && ok;
  ok = IncidentTubeGroupParams->SpinBox_DX->isValid(msg, !IsPreview()) && ok;
  ok = IncidentTubeGroupParams->SpinBox_DY->isValid(msg, !IsPreview()) && ok;
  ok = IncidentTubeGroupParams->SpinBox_DZ->isValid(msg, !IsPreview()) && ok;
  ok = ChamferGroupParams->SpinBox_DX->isValid(msg, !IsPreview()) && ok;
  ok = ChamferGroupParams->SpinBox_DY->isValid(msg, !IsPreview()) && ok;
  ok = FilletGroupParams->SpinBox_DX->isValid(msg, !IsPreview()) && ok;

  if (LReductionGroupParams->GroupBox1->isChecked()) {
    ok = LReductionGroupParams->SpinBox1->isValid(msg, !IsPreview()) && ok;
    ok = LReductionGroupParams->SpinBox2->isValid(msg, !IsPreview()) && ok;
    ok = LReductionGroupParams->SpinBox3->isValid(msg, !IsPreview()) && ok;
    ok = LReductionGroupParams->SpinBox4->isValid(msg, !IsPreview()) && ok;
    if(MainTubeGroupParams->SpinBox_DX->value() == LReductionGroupParams->SpinBox1->value()) {
      msg += tr("GEOM_PIPETSHAPE_ERR_EQUAL_RADII_L") + "\n";
      ok = false;
    }
    if(MainTubeGroupParams->SpinBox_DX->value() + MainTubeGroupParams->SpinBox_DY->value() ==
       LReductionGroupParams->SpinBox1->value() + LReductionGroupParams->SpinBox2->value()) {
      msg += tr("GEOM_PIPETSHAPE_ERR_EQUAL_EXT_RADII_L") + "\n";
      ok = false;
    }
  }
  if (RReductionGroupParams->GroupBox1->isChecked()) {
    ok = RReductionGroupParams->SpinBox1->isValid(msg, !IsPreview()) && ok;
    ok = RReductionGroupParams->SpinBox2->isValid(msg, !IsPreview()) && ok;
    ok = RReductionGroupParams->SpinBox3->isValid(msg, !IsPreview()) && ok;
    ok = RReductionGroupParams->SpinBox4->isValid(msg, !IsPreview()) && ok;
    if(MainTubeGroupParams->SpinBox_DX->value() == RReductionGroupParams->SpinBox1->value()) {
      msg += tr("GEOM_PIPETSHAPE_ERR_EQUAL_RADII_R") + "\n";
      ok = false;
    }
    if(MainTubeGroupParams->SpinBox_DX->value() + MainTubeGroupParams->SpinBox_DY->value() ==
       RReductionGroupParams->SpinBox1->value() + RReductionGroupParams->SpinBox2->value()) {
      msg += tr("GEOM_PIPETSHAPE_ERR_EQUAL_EXT_RADII_R") + "\n";
      ok = false;
    }
  }
  if (IReductionGroupParams->GroupBox1->isChecked()) {
    ok = IReductionGroupParams->SpinBox1->isValid(msg, !IsPreview()) && ok;
    ok = IReductionGroupParams->SpinBox2->isValid(msg, !IsPreview()) && ok;
    ok = IReductionGroupParams->SpinBox3->isValid(msg, !IsPreview()) && ok;
    ok = IReductionGroupParams->SpinBox4->isValid(msg, !IsPreview()) && ok;
    if(IncidentTubeGroupParams->SpinBox_DX->value() == IReductionGroupParams->SpinBox1->value()) {
      msg += tr("GEOM_PIPETSHAPE_ERR_EQUAL_RADII_I") + "\n";
      ok = false;
    }
    if(IncidentTubeGroupParams->SpinBox_DX->value() + IncidentTubeGroupParams->SpinBox_DY->value() ==
       IReductionGroupParams->SpinBox1->value() + IReductionGroupParams->SpinBox2->value()) {
      msg += tr("GEOM_PIPETSHAPE_ERR_EQUAL_EXT_RADII_I") + "\n";
      ok = false;
    }
  }

  ok = fabs(MainTubeGroupParams->SpinBox_DX->value()) > Precision::Confusion() && ok;
  ok = fabs(MainTubeGroupParams->SpinBox_DY->value()) > Precision::Confusion() && ok;
  ok = fabs(MainTubeGroupParams->SpinBox_DZ->value()) > Precision::Confusion() && ok;
  ok = fabs(IncidentTubeGroupParams->SpinBox_DX->value()) > Precision::Confusion() && ok;
  ok = fabs(IncidentTubeGroupParams->SpinBox_DY->value()) > Precision::Confusion() && ok;
  ok = fabs(IncidentTubeGroupParams->SpinBox_DZ->value()) > Precision::Confusion() && ok;
  ok = fabs(ChamferGroupParams->SpinBox_DX->value()) > Precision::Confusion() && ok;
  ok = fabs(ChamferGroupParams->SpinBox_DY->value()) > Precision::Confusion() && ok;
  ok = fabs(FilletGroupParams->SpinBox_DX->value()) > Precision::Confusion() && ok;

  if (JunctionPointsSel->GroupBox1->isChecked())
    ok = myOkPoint1 && myOkPoint2 && myOkPoint3 && ok;

  return ok;
}

//=================================================================================
// function : CheckCompatiblePosition()
// purpose  :
//=================================================================================
bool AdvancedGUI_PipeTShapeDlg::CheckCompatiblePosition
     (GEOM::GEOM_Object_var theP1, GEOM::GEOM_Object_var theP2, GEOM::GEOM_Object_var theP3,
      double theTolerance)
{
  MainTubeGroupParams->SpinBox_DZ->setStyleSheet(CssNormal);
  IncidentTubeGroupParams->SpinBox_DZ->setStyleSheet(CssNormal);
  JunctionPointsSel->PushButton4->setStyleSheet(CssNormal);
  JunctionPointsSel->PushButton5->setStyleSheet(CssNormal);

  CORBA::Double theL1 = MainTubeGroupParams->SpinBox_DZ->value();
  CORBA::Double theL2 = IncidentTubeGroupParams->SpinBox_DZ->value();

  JunctionPointsSel->LineEdit4->setText("");
  JunctionPointsSel->LineEdit5->setText("");

  MainTubeGroupParams->SpinBox_DZ->setToolTip("");
  IncidentTubeGroupParams->SpinBox_DZ->setToolTip("");

  TopoDS_Shape aShape;
  gp_Pnt P1, P2, P3;
  if ( GEOMBase::GetShape( theP1, aShape ) && !aShape.IsNull() && aShape.ShapeType() == TopAbs_VERTEX )
    P1 = BRep_Tool::Pnt(TopoDS::Vertex(aShape));
  else
    return false;

  if ( GEOMBase::GetShape( theP2, aShape ) && !aShape.IsNull() && aShape.ShapeType() == TopAbs_VERTEX )
    P2 = BRep_Tool::Pnt(TopoDS::Vertex(aShape));
  else
    return false;

  if ( GEOMBase::GetShape( theP3, aShape ) && !aShape.IsNull() && aShape.ShapeType() == TopAbs_VERTEX )
    P3 = BRep_Tool::Pnt(TopoDS::Vertex(aShape));
  else
    return false;

  double d12 = P1.Distance(P2);
  double d13 = P1.Distance(P3);
  double d23 = P2.Distance(P3);

  if (Abs(d12) <= Precision::Confusion()) {
    //SetErrorCode("Junctions points P1 and P2 are identical");
    return false;
  }
  if (Abs(d13) <= Precision::Confusion()) {
    //SetErrorCode("Junctions points P1 and P3 are identical");
    return false;
  }
  if (Abs(d23) <= Precision::Confusion()) {
    //SetErrorCode("Junctions points P2 and P3 are identical");
    return false;
  }

  long double newL1 = 0.5 * d12;
  long double newL2 = sqrt(pow(d13,2)-pow(newL1,2));

  JunctionPointsSel->LineEdit4->setText(QString::number(newL1,'f',7));
  JunctionPointsSel->LineEdit5->setText(QString::number(newL2,'f',7));

  if (fabs(newL1 - theL1) > Precision::Approximation()) {
    if ((newL1 * (1 - theTolerance) - theL1 <= Precision::Approximation()) &&
        (newL1 * (1 + theTolerance) - theL1 >= Precision::Approximation())) {
      disconnect(MainTubeGroupParams->SpinBox_DZ, 0, this, 0);
      MainTubeGroupParams->SpinBox_DZ->setValue(newL1);
      connect(MainTubeGroupParams->SpinBox_DZ, SIGNAL(valueChanged(double)),
              this, SLOT(ValueChangedInSpinBox(double)));
      MainTubeGroupParams->SpinBox_DZ->setToolTip("Value was recomputed to fit with position");
      MainTubeGroupParams->SpinBox_DZ->setStyleSheet(CssAcceptable);
      JunctionPointsSel->PushButton4->setStyleSheet(CssAcceptable);
    }
    else {
      MainTubeGroupParams->SpinBox_DZ->setToolTip("Value is incompatible with position");
      MainTubeGroupParams->SpinBox_DZ->setStyleSheet(CssRefused);
      JunctionPointsSel->PushButton4->setStyleSheet(CssRefused);
    }
  }
  else {
    MainTubeGroupParams->SpinBox_DZ->setStyleSheet(CssNormal);
    JunctionPointsSel->PushButton4->setStyleSheet(CssNormal);
  }

  if (fabs(newL2 - theL2) > Precision::Approximation()) {
    if ((newL2 * (1 - theTolerance) - theL2 <= Precision::Approximation()) &&
        (newL2 * (1 + theTolerance) - theL2 >= Precision::Approximation())) {
      disconnect(IncidentTubeGroupParams->SpinBox_DZ, 0, this, 0);
      IncidentTubeGroupParams->SpinBox_DZ->setValue(newL2);
      connect(IncidentTubeGroupParams->SpinBox_DZ, SIGNAL(valueChanged( double )), this, SLOT(ValueChangedInSpinBox(double)));
      IncidentTubeGroupParams->SpinBox_DZ->setToolTip("Value was recomputed to fit with position");
      IncidentTubeGroupParams->SpinBox_DZ->setStyleSheet(CssAcceptable);
      JunctionPointsSel->PushButton5->setStyleSheet(CssAcceptable);
    }
    else {
      IncidentTubeGroupParams->SpinBox_DZ->setToolTip("Value is incompatible with position");
      IncidentTubeGroupParams->SpinBox_DZ->setStyleSheet(CssRefused);
      JunctionPointsSel->PushButton5->setStyleSheet(CssRefused);
    }
  }
  else {
    IncidentTubeGroupParams->SpinBox_DZ->setStyleSheet(CssNormal);
    JunctionPointsSel->PushButton5->setStyleSheet(CssNormal);
  }

  return true;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool AdvancedGUI_PipeTShapeDlg::execute (ObjectList& objects)
{
  if (JunctionPointsSel->GroupBox1->isChecked() && myOkPoint1 && myOkPoint2 && myOkPoint3)
    CheckCompatiblePosition(myPoint1, myPoint2, myPoint3, 0.01);

  return executeNoCheck(objects);
}

//=================================================================================
// function : executeNoCheck
// purpose  :
//=================================================================================
bool AdvancedGUI_PipeTShapeDlg::executeNoCheck (ObjectList& objects)
{
  bool res = false;

  //   GEOM::GEOM_Object_var anObj;
  GEOM::ListOfGO_var anObj;

  GEOM::IAdvancedOperations_var anOper = GEOM::IAdvancedOperations::_narrow(getOperation());

  //@@ retrieve input values from the widgets here @@//
  CORBA::Double theR1 = MainTubeGroupParams->SpinBox_DX->value();
  CORBA::Double theW1 = MainTubeGroupParams->SpinBox_DY->value();
  CORBA::Double theL1 = MainTubeGroupParams->SpinBox_DZ->value();
  CORBA::Double theR2 = IncidentTubeGroupParams->SpinBox_DX->value();
  CORBA::Double theW2 = IncidentTubeGroupParams->SpinBox_DY->value();
  CORBA::Double theL2 = IncidentTubeGroupParams->SpinBox_DZ->value();
  CORBA::Double theH = ChamferGroupParams->SpinBox_DX->value();
  CORBA::Double theW = ChamferGroupParams->SpinBox_DY->value();
  CORBA::Double theRF = FilletGroupParams->SpinBox_DX->value();
  CORBA::Boolean theHexMesh = HexMeshCheckBox->isChecked();

  // Thickness reductions
  CORBA::Double theRL = 0., theWL = 0., theLtransL = 0., theLthinL = 0.;
  CORBA::Double theRR = 0., theWR = 0., theLtransR = 0., theLthinR = 0.;
  CORBA::Double theRI = 0., theWI = 0., theLtransI = 0., theLthinI = 0.;

  if (LReductionGroupParams->GroupBox1->isChecked()) {
    theRL = LReductionGroupParams->SpinBox1->value();
    theWL = LReductionGroupParams->SpinBox2->value();
    theLtransL = LReductionGroupParams->SpinBox3->value();
    theLthinL = LReductionGroupParams->SpinBox4->value();
  }
  if (RReductionGroupParams->GroupBox1->isChecked()) {
    theRR = RReductionGroupParams->SpinBox1->value();
    theWR = RReductionGroupParams->SpinBox2->value();
    theLtransR = RReductionGroupParams->SpinBox3->value();
    theLthinR = RReductionGroupParams->SpinBox4->value();
  }
  if (IReductionGroupParams->GroupBox1->isChecked()) {
    theRI = IReductionGroupParams->SpinBox1->value();
    theWI = IReductionGroupParams->SpinBox2->value();
    theLtransI = IReductionGroupParams->SpinBox3->value();
    theLthinI = IReductionGroupParams->SpinBox4->value();
  }

  //if (JunctionPointsSel->GroupBox1->isChecked()) {
  //  CheckCompatiblePosition(theL1, theL2, myPoint1, myPoint2, myPoint3, 0.01);
  //  theL1 = MainTubeGroupParams->SpinBox_DZ->value();
  //  theL2 = IncidentTubeGroupParams->SpinBox_DZ->value();
  //}

  // call engine function
  if (ChamferGroupParams->GroupBox1->isChecked()) {
    if (JunctionPointsSel->GroupBox1->isChecked())
      anObj = anOper->MakePipeTShapeTRChamferWithPosition(theR1, theW1, theL1, theR2, theW2, theL2,
                                                          theRL, theWL, theLtransL, theLthinL,
                                                          theRR, theWR, theLtransR, theLthinR,
                                                          theRI, theWI, theLtransI, theLthinI,
                                                          theH, theW, theHexMesh, myPoint1, myPoint2, myPoint3);
    else
      anObj = anOper->MakePipeTShapeTRChamfer(theR1, theW1, theL1, theR2, theW2, theL2,
                                              theRL, theWL, theLtransL, theLthinL,
                                              theRR, theWR, theLtransR, theLthinR,
                                              theRI, theWI, theLtransI, theLthinI,
                                              theH, theW, theHexMesh);
  }
  else if (FilletGroupParams->GroupBox1->isChecked()) {
    if (JunctionPointsSel->GroupBox1->isChecked())
      anObj = anOper->MakePipeTShapeTRFilletWithPosition(theR1, theW1, theL1, theR2, theW2, theL2,
                                                         theRL, theWL, theLtransL, theLthinL,
                                                         theRR, theWR, theLtransR, theLthinR,
                                                         theRI, theWI, theLtransI, theLthinI,
                                                         theRF, theHexMesh, myPoint1, myPoint2, myPoint3);
    else
      anObj = anOper->MakePipeTShapeTRFillet(theR1, theW1, theL1, theR2, theW2, theL2,
                                             theRL, theWL, theLtransL, theLthinL,
                                             theRR, theWR, theLtransR, theLthinR,
                                             theRI, theWI, theLtransI, theLthinI,
                                             theRF, theHexMesh);
  }
  else {
    if (JunctionPointsSel->GroupBox1->isChecked())
      anObj = anOper->MakePipeTShapeTRWithPosition(theR1, theW1, theL1, theR2, theW2, theL2,
                                                   theRL, theWL, theLtransL, theLthinL,
                                                   theRR, theWR, theLtransR, theLthinR,
                                                   theRI, theWI, theLtransI, theLthinI,
                                                   theHexMesh, myPoint1, myPoint2, myPoint3);
    else
      anObj = anOper->MakePipeTShapeTR(theR1, theW1, theL1, theR2, theW2, theL2,
                                       theRL, theWL, theLtransL, theLthinL,
                                       theRR, theWR, theLtransR, theLthinR,
                                       theRI, theWI, theLtransI, theLthinI,
                                       theHexMesh);
  }

  res = anObj->length();
  if (!res)
    return false;

  //res = !anObj->_is_nil();
  if (res && !IsPreview()) {
    QStringList aParameters;
    //@@ put stringified input parameters to the string list here to store in the data model for notebook @@//
    aParameters << MainTubeGroupParams->SpinBox_DX->text(); // R1 parameter
    aParameters << MainTubeGroupParams->SpinBox_DY->text(); // W1 parameter
    aParameters << MainTubeGroupParams->SpinBox_DZ->text(); // L1 parameter
    aParameters << IncidentTubeGroupParams->SpinBox_DX->text(); // R2 parameter
    aParameters << IncidentTubeGroupParams->SpinBox_DY->text(); // W2 parameter
    aParameters << IncidentTubeGroupParams->SpinBox_DZ->text(); // L2 parameter

    if (ChamferGroupParams->GroupBox1->isChecked()) { // Chamfer parameter
      aParameters << ChamferGroupParams->SpinBox_DX->text(); // H parameter
      aParameters << ChamferGroupParams->SpinBox_DY->text(); // W parameter
    }
    if (FilletGroupParams->GroupBox1->isChecked()) // Fillet parameter
      aParameters << FilletGroupParams->SpinBox_DX->text(); // RF parameter

    // Thickness reductions
    if (LReductionGroupParams->GroupBox1->isChecked()) {
      aParameters << LReductionGroupParams->SpinBox1->text();
      aParameters << LReductionGroupParams->SpinBox2->text();
      aParameters << LReductionGroupParams->SpinBox3->text();
      aParameters << LReductionGroupParams->SpinBox4->text();
    }
    else {
      aParameters << "0" << "0" << "0" << "0";
    }
    if (RReductionGroupParams->GroupBox1->isChecked()) {
      aParameters << RReductionGroupParams->SpinBox1->text();
      aParameters << RReductionGroupParams->SpinBox2->text();
      aParameters << RReductionGroupParams->SpinBox3->text();
      aParameters << RReductionGroupParams->SpinBox4->text();
    }
    else {
      aParameters << "0" << "0" << "0" << "0";
    }
    if (IReductionGroupParams->GroupBox1->isChecked()) {
      aParameters << IReductionGroupParams->SpinBox1->text();
      aParameters << IReductionGroupParams->SpinBox2->text();
      aParameters << IReductionGroupParams->SpinBox3->text();
      aParameters << IReductionGroupParams->SpinBox4->text();
    }
    else {
      aParameters << "0" << "0" << "0" << "0";
    }

    if (aParameters.count() > 0)
      anObj[0]->SetParameters(aParameters.join(":").toLatin1().constData());
  }

  objects.push_back(anObj[0]._retn());
  pipeTShapeGroupObjects.clear();
  for (int i = 1, n = anObj->length(); i < n; i++) {
    pipeTShapeGroupObjects.push_back(anObj[i]._retn());
  }

  return objects.size() > 0;
}

//=================================================================================
// function : restoreSubShapes
// purpose  :
//=================================================================================
void AdvancedGUI_PipeTShapeDlg::restoreSubShapes (SALOMEDS::Study_ptr theStudy,
                                                  SALOMEDS::SObject_ptr theSObject)
{
  SALOMEDS::GenericAttribute_var anAttr;
  if (!theSObject->FindAttribute(anAttr, "AttributeIOR"))
    return;

  SALOMEDS::AttributeIOR_var anAttrIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
  CORBA::String_var anIORso = anAttrIOR->Value();

  // get Object from SObject
  GEOM::GEOM_Object_var theFather =
    GEOM::GEOM_Object::_narrow(myGeomGUI->getApp()->orb()->string_to_object(anIORso));
  if (CORBA::is_nil(theFather))
    return;

  ObjectList::iterator it = pipeTShapeGroupObjects.begin();

  for (int i = 0; it != pipeTShapeGroupObjects.end(); it++, i++) {
    getGeomEngine()->AddInStudy(theStudy, (*it), tr((*it)->GetName()).toStdString().c_str(), theFather);
  }
}

//=================================================================================
// function : processPreview()
// purpose  : Display preview if CheckBoxPreview is checked
//=================================================================================
void AdvancedGUI_PipeTShapeDlg::processPreview()
{
  if (mainFrame()->CheckBoxPreview->isChecked())
    DisplayPreview();
  else
    erasePreview(true);
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> AdvancedGUI_PipeTShapeDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res;
  GEOM::GeomObjPtr aGeomObjPtr1(myPoint1), aGeomObjPtr2(myPoint2), aGeomObjPtr3(myPoint3);
  res << aGeomObjPtr1 << aGeomObjPtr2 << aGeomObjPtr3;
  return res;
}

//=================================================================================
// function : AdvancedGUI_4Spin
// purpose  :
//=================================================================================
AdvancedGUI_4Spin::AdvancedGUI_4Spin()
{
  GroupBox1 = new QGroupBox ();

  gridLayout1 = new QGridLayout (GroupBox1);
  gridLayout1->setSpacing(6);
  gridLayout1->setContentsMargins(11, 11, 11, 11);
  gridLayout1->setHorizontalSpacing(6);
  gridLayout1->setVerticalSpacing(6);
  gridLayout1->setContentsMargins(9, 9, 9, 9);

  TextLabel1 = new QLabel (GroupBox1);
  TextLabel2 = new QLabel (GroupBox1);
  TextLabel3 = new QLabel (GroupBox1);
  TextLabel4 = new QLabel (GroupBox1);

  SpinBox1 = new SalomeApp_DoubleSpinBox (GroupBox1);
  SpinBox2 = new SalomeApp_DoubleSpinBox (GroupBox1);
  SpinBox3 = new SalomeApp_DoubleSpinBox (GroupBox1);
  SpinBox4 = new SalomeApp_DoubleSpinBox (GroupBox1);

  SpinBox1->setMinimumWidth(60);
  SpinBox2->setMinimumWidth(60);
  SpinBox3->setMinimumWidth(60);
  SpinBox4->setMinimumWidth(60);

  gridLayout1->addWidget(TextLabel1, 0, 0, 1, 1);
  gridLayout1->addWidget(TextLabel2, 1, 0, 1, 1);
  gridLayout1->addWidget(TextLabel3, 2, 0, 1, 1);
  gridLayout1->addWidget(TextLabel4, 3, 0, 1, 1);

  gridLayout1->addWidget(SpinBox1, 0, 1, 1, 1);
  gridLayout1->addWidget(SpinBox2, 1, 1, 1, 1);
  gridLayout1->addWidget(SpinBox3, 2, 1, 1, 1);
  gridLayout1->addWidget(SpinBox4, 3, 1, 1, 1);
}

//=================================================================================
// function : ~AdvancedGUI_4Spin()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
AdvancedGUI_4Spin::~AdvancedGUI_4Spin()
{
  // no need to delete child widgets, Qt does it all for us
}
