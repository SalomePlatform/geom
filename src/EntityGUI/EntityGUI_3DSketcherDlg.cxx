// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

// GEOM GEOMGUI : GUI for Geometry component
// File   : EntityGUI_3DSketcherDlg.cxx
// Author : DMV, OCN

#include <cmath>
#include <string>
#include <boost/lexical_cast.hpp>

#include "EntityGUI_3DSketcherDlg.h"
#include "EntityGUI_Widgets.h"
#include <SalomeApp_DoubleSpinBox.h>

#include <GEOMBase.h>
#include <GeometryGUI.h>
#include <Precision.hxx>

#include <DlgRef.h>
#include <SUIT_Session.h>
#include <SUIT_Desktop.h>
#include <SUIT_MessageBox.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_ViewWindow.h>
#include <SUIT_ViewManager.h>
#include <SOCC_Prs.h>
#include <SOCC_ViewModel.h>
#include <SalomeApp_Application.h>
#include <LightApp_Application.h>
#include <LightApp_SelectionMgr.h>

#include <SalomeApp_Tools.h>

//OCCT includes
#include <TopoDS.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>

#include <AIS_Trihedron.hxx>
#include <AIS_AngleDimension.hxx>
#include <AIS_LengthDimension.hxx>
#include <AIS_Drawer.hxx>
#include <Geom_Axis2Placement.hxx>
#include <Geom_Plane.hxx>
#include <SelectMgr_Selection.hxx>
#include <gce_MakePln.hxx>
#include <Prs3d_AngleAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_LengthAspect.hxx>

// This include must be *AFTER* SOCC_ViewModel.h because
// of the constant ROTATE which is a #define in
// GEOMImpl_Types.hxx and an enum in SOCC_ViewModel.h
#include <GEOMImpl_Types.hxx>

enum
{
  NONE,
  TYPE_LENGTH,
  TYPE_ANGLE,
  TYPE_TWO_ANGLES
};

enum
{
  OXY,
  OYZ,
  OXZ
};

class Locker
{
public:
  Locker(bool& l) : myLock(l) { myLock = true;  }
  ~Locker()                   { myLock = false; }
private:
  bool& myLock;
};

//=================================================================================
// class    : EntityGUI_3DSketcherDlg()
// purpose  : Constructs a EntityGUI_3DSketcherDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
EntityGUI_3DSketcherDlg::EntityGUI_3DSketcherDlg (GeometryGUI* theGeometryGUI, QWidget* parent,
                                                  bool modal, Qt::WindowFlags fl,
                                                  const double lineWidth)
  : GEOMBase_Skeleton(theGeometryGUI, parent, modal, fl),
    myMode(-1),
    myOK(false),
    myLineWidth(lineWidth),
    myGeometryGUI(theGeometryGUI),
    myLengthIORedoList()
//     myLastAngleNormal()
{
  QPixmap image0(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_SELECT")));
  QPixmap image1(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_UNDO")));
  QPixmap image2(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_REDO")));
  QPixmap image3(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICO_3DSKETCH")));

  setWindowTitle(tr("GEOM_3DSKETCHER_TITLE"));

  /***************************************************************/

  mainFrame()->GroupConstructors->setTitle(tr("GEOM_3DSKETCHER"));
  mainFrame()->RadioButton1->setIcon(image3);;
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->close();

  GroupType = new DlgRef_3Radio(centralWidget());
  GroupType->GroupBox1->setTitle(tr("GEOM_COORDINATES_TYPE"));
  GroupType->RadioButton1->setText(tr("GEOM_SKETCHER_ABS"));
  GroupType->RadioButton2->setText(tr("GEOM_SKETCHER_REL"));
  GroupType->RadioButton3->setText(tr("Angles")); //TODO translation
//   GroupType->RadioButton3->close();
  myTypeGroup = new QButtonGroup(this);
  myTypeGroup->addButton(GroupType->RadioButton1, 0);
  myTypeGroup->addButton(GroupType->RadioButton2, 1);
  myTypeGroup->addButton(GroupType->RadioButton3, 2);

  Group3Spin = new EntityGUI_3Spin(centralWidget());
  Group3Spin->GroupBox1->setTitle(tr("GEOM_SKETCHER_VALUES"));
  Group3Spin->buttonApply->setText(tr("GEOM_SKETCHER_APPLY"));
  Group3Spin->buttonUndo->setIcon(image1);
  Group3Spin->buttonRedo->setIcon(image2);
  Group3Spin->TextLabel1->setText(tr("GEOM_SKETCHER_X2"));
  Group3Spin->TextLabel2->setText(tr("GEOM_SKETCHER_Y2"));
  Group3Spin->TextLabel3->setText(tr("GEOM_SKETCHER_Z2"));

  GroupAngles = new EntityGUI_Angles(centralWidget());
  GroupAngles->buttonApply->setText(tr("GEOM_SKETCHER_APPLY"));
  GroupAngles->buttonUndo->setIcon(image1);
  GroupAngles->buttonRedo->setIcon(image2);
  GroupAngles->checkBox->setText(tr("Angle 2")); //TODO translation

  GroupControls = new EntityGUI_Controls(centralWidget());
  GroupControls->GroupBox1->setTitle(tr("GEOM_CONTROLS"));
  GroupControls->CheckBox1->setText(tr("Show length dimensions")); //TODO translation
  GroupControls->CheckBox2->setText(tr("Show angle dimensions"));  //TODO translation
  GroupControls->CheckBox3->setText(tr("Show start/end point coordinates"));  //TODO translation
  GroupControls->lineEdit_1->setReadOnly(true);
  GroupControls->lineEdit_2->setReadOnly(true);
  GroupControls->lineEdit_3->setReadOnly(true);
  GroupControls->lineEdit_4->setReadOnly(true);
  GroupControls->lineEdit_5->setReadOnly(true);
  GroupControls->lineEdit_6->setReadOnly(true);
  GroupControls->label_1->setText(tr("X:"));
  GroupControls->label_2->setText(tr("Y:"));
  GroupControls->label_3->setText(tr("Z:"));
  GroupControls->label_4->setText(tr("X:"));
  GroupControls->label_5->setText(tr("Y:"));
  GroupControls->label_6->setText(tr("Z:"));

  buttonOk()->setText(tr("GEOM_BUT_END_SKETCH"));
  buttonApply()->setText(tr("GEOM_BUT_CLOSE_SKETCH"));

  QVBoxLayout* layout = new QVBoxLayout(centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
  layout->addWidget(GroupType);
  layout->addWidget(Group3Spin);
  layout->addWidget(GroupAngles);
  layout->addWidget(GroupControls);

  setHelpFileName("create_3dsketcher_page.html");

  resize(100,100);
  Init();
}

//=================================================================================
// function : ~EntityGUI_3DSketcherDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
EntityGUI_3DSketcherDlg::~EntityGUI_3DSketcherDlg()
{
  myGeomGUI->SetActiveDialogBox(0);
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void EntityGUI_3DSketcherDlg::Init()
{
  myOK = false;
  myOrientation = OXY;
  myPrsType = NONE;

  SUIT_ViewWindow* vw = SUIT_Session::session()->activeApplication()->desktop()->activeWindow();
  myAnglePrs = dynamic_cast<SOCC_Prs*>(((SOCC_Viewer*)(vw->getViewManager()->getViewModel()))->CreatePrs(0));
  myLengthPrs = dynamic_cast<SOCC_Prs*>(((SOCC_Viewer*)(vw->getViewManager()->getViewModel()))->CreatePrs(0));

  //TEST
  localSelection(GEOM::GEOM_Object::_nil(), TopAbs_VERTEX);
//   globalSelection(GEOM_PREVIEW);
//   setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed); 

  /* Get setting of step value from file configuration */
  double step = SUIT_Session::session()->resourceMgr()->doubleValue("Geometry", "SettingsGeomStep", 100.0);

  /* min, max, step and decimals for spin boxes */
  initSpinBox(Group3Spin->SpinBox_DX, COORD_MIN, COORD_MAX, step, "length_precision");
  initSpinBox(Group3Spin->SpinBox_DY, COORD_MIN, COORD_MAX, step, "length_precision");
  initSpinBox(Group3Spin->SpinBox_DZ, COORD_MIN, COORD_MAX, step, "length_precision");

  initSpinBox(GroupAngles->SpinBox_DA , -180.0, 180.0, step, "angular_precision");
  initSpinBox(GroupAngles->SpinBox_DA2,  -90.0,  90.0, step, "angular_precision");
  initSpinBox(GroupAngles->SpinBox_DL , COORD_MIN, COORD_MAX, step, "length_precision");

  Group3Spin->SpinBox_DX->setValue(0.0);
  Group3Spin->SpinBox_DY->setValue(0.0);
  Group3Spin->SpinBox_DZ->setValue(0.0);

  GroupAngles->SpinBox_DA->setValue(0.0);
  GroupAngles->SpinBox_DA2->setValue(0.0);
  GroupAngles->SpinBox_DL->setValue(0.0);

  GroupAngles->radioButton_1->setChecked(true);
  GroupAngles->checkBox->setChecked(false);
  GroupAngles->SpinBox_DA2->setEnabled(false);

  GroupControls->CheckBox1->setChecked(true);
  GroupControls->CheckBox2->setChecked(true);
  GroupControls->CheckBox3->setChecked(true);
  
  isLengthVisible = true;
  isAngleVisible = true;

  GroupAngles->hide();

  /* signals and slots connections */
  connect(buttonOk(),     SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(),  SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  connect(Group3Spin->buttonApply,  SIGNAL(clicked()), this, SLOT(ClickOnAddPoint()));
  connect(Group3Spin->buttonUndo,   SIGNAL(clicked()), this, SLOT(ClickOnUndo()));
  connect(Group3Spin->buttonRedo,   SIGNAL(clicked()), this, SLOT(ClickOnRedo())) ;

  connect(GroupAngles->buttonApply,  SIGNAL(clicked()), this, SLOT(ClickOnAddPoint()));
  connect(GroupAngles->buttonUndo,   SIGNAL(clicked()), this, SLOT(ClickOnUndo()));
  connect(GroupAngles->buttonRedo,   SIGNAL(clicked()), this, SLOT(ClickOnRedo())) ;

  connect(myTypeGroup, SIGNAL(buttonClicked(int)),  this, SLOT(TypeClicked(int)));

  connect(Group3Spin->SpinBox_DX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(Group3Spin->SpinBox_DY, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(Group3Spin->SpinBox_DZ, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));

  connect(GroupAngles->SpinBox_DA,  SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupAngles->SpinBox_DA2, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupAngles->SpinBox_DL,  SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));

  connect(GroupAngles->radioButton_1,   SIGNAL(clicked (bool)), this, SLOT(ButtonClicked(bool))) ;
  connect(GroupAngles->radioButton_2,   SIGNAL(clicked (bool)), this, SLOT(ButtonClicked(bool))) ;
  connect(GroupAngles->radioButton_3,   SIGNAL(clicked (bool)), this, SLOT(ButtonClicked(bool))) ;

  connect(GroupAngles->checkBox,        SIGNAL(clicked (bool)), this, SLOT(BoxChecked (bool))) ;
  connect(GroupControls->CheckBox1,     SIGNAL(clicked (bool)), this, SLOT(BoxChecked (bool))) ;
  connect(GroupControls->CheckBox2,     SIGNAL(clicked (bool)), this, SLOT(BoxChecked (bool))) ;
  connect(GroupControls->CheckBox3,     SIGNAL(clicked (bool)), this, SLOT(BoxChecked (bool))) ;

  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), this, SLOT(SetDoubleSpinBoxStep(double)));

  connect(myGeomGUI, SIGNAL(SignalDeactivateActiveDialog()), this, SLOT(DeactivateActiveDialog()));
  connect(myGeomGUI, SIGNAL(SignalCloseAllDialogs()),        this, SLOT(ClickOnCancel()));

  initName(tr("GEOM_3DSKETCHER"));
  
  GroupControls->CheckBox3->click();

  UpdateButtonsState();
  GEOMBase_Helper::displayPreview(true, false, true, true, myLineWidth);
}

//=================================================================================
// function : TypeClicked()
// purpose  : Radio button management
//=================================================================================
void EntityGUI_3DSketcherDlg::TypeClicked (int mode)
{
  if (mode == myMode) return;

  GroupAngles->hide();
  Group3Spin->show();

  bool blocked = Group3Spin->SpinBox_DX->signalsBlocked();
  Group3Spin->SpinBox_DX->blockSignals(true);
  Group3Spin->SpinBox_DY->blockSignals(true);
  Group3Spin->SpinBox_DZ->blockSignals(true);

  // Get setting of step value from file configuration
  XYZ xyz = getLastPoint();
  bool okx, oky, okz;
  Group3Spin->SpinBox_DX->text().toDouble(&okx);
  Group3Spin->SpinBox_DY->text().toDouble(&oky);
  Group3Spin->SpinBox_DZ->text().toDouble(&okz);

  if (mode == 0) {  // XYZ
    Group3Spin->TextLabel1->setText(tr("GEOM_SKETCHER_X2"));
    Group3Spin->TextLabel2->setText(tr("GEOM_SKETCHER_Y2"));
    Group3Spin->TextLabel3->setText(tr("GEOM_SKETCHER_Z2"));
    if (myMode == 1)
    {
      if (okx) Group3Spin->SpinBox_DX->setValue(xyz.x + Group3Spin->SpinBox_DX->value());
      if (oky) Group3Spin->SpinBox_DY->setValue(xyz.y + Group3Spin->SpinBox_DY->value());
      if (okz) Group3Spin->SpinBox_DZ->setValue(xyz.z + Group3Spin->SpinBox_DZ->value());
    }
    Group3Spin->buttonApply->setFocus();
  }
  else if (mode == 1) { // DXDYDZ
    Group3Spin->TextLabel1->setText(tr("GEOM_SKETCHER_DX2"));
    Group3Spin->TextLabel2->setText(tr("GEOM_SKETCHER_DY2"));
    Group3Spin->TextLabel3->setText(tr("GEOM_SKETCHER_DZ2"));
    if (myMode == 0)
    {
      if (okx) Group3Spin->SpinBox_DX->setValue(Group3Spin->SpinBox_DX->value() - xyz.x);
      if (oky) Group3Spin->SpinBox_DY->setValue(Group3Spin->SpinBox_DY->value() - xyz.y);
      if (okz) Group3Spin->SpinBox_DZ->setValue(Group3Spin->SpinBox_DZ->value() - xyz.z);
    }
    Group3Spin->buttonApply->setFocus();
  }
  else if (mode == 2) { // Angles and Length
    Group3Spin->hide();
    GroupAngles->show();
    GroupAngles->buttonApply->setFocus();
  }

  Group3Spin->SpinBox_DX->blockSignals(blocked);
  Group3Spin->SpinBox_DY->blockSignals(blocked);
  Group3Spin->SpinBox_DZ->blockSignals(blocked);

  myMode = mode;

  updateGeometry();
  resize(minimumSizeHint());

  GEOMBase_Helper::displayPreview(true, false, true, true, myLineWidth);
}

//=================================================================================
// function : ClickOnAddPoint()
// purpose  : called when the point coordinates is Applyed
//=================================================================================
void EntityGUI_3DSketcherDlg::ClickOnAddPoint()
{ 
  QString msg;
  if (!isValid(msg)) {
    showError(msg);
    return;
  }

  // Display and store angle dimensions interactive objects in Prs
  if (GroupType->RadioButton3->isChecked())    // ANGLES
  {
    double anAngle2 = 0.0;
    if (GroupAngles->checkBox->isChecked())
      anAngle2 = GroupAngles->SpinBox_DA2->value();

    // Store angle dimensions
    displayAngle(GroupAngles->SpinBox_DA->value(), anAngle2,
                 GroupAngles->SpinBox_DL->value(), myOrientation,  /*store =*/true);
    // Store length dimensions
    displayLength(GroupAngles->SpinBox_DL->value(), /*store =*/true, /*type=*/myPrsType);
  }

  // Display and store store length dimension interactive object in Prs
  if (GroupType->RadioButton1->isChecked() ||   // ABSOLUTE or RELATIVE coordinates
      GroupType->RadioButton2->isChecked())
  {
    displayLength(-1, /*store=*/true);
  }

  myPointsList.append(getCurrentPoint());
  myPrsTypeList.push_back(myPrsType);

  // Clean redo lists
  myRedoList.clear();
  myPrsTypeRedoList.clear();
  myLengthIORedoList.Clear();
  myAngleIORedoList.Clear();

  if (myMode == 1) 
  {
    Group3Spin->SpinBox_DX->setValue(0.0);
    Group3Spin->SpinBox_DY->setValue(0.0);
    Group3Spin->SpinBox_DZ->setValue(0.0);
  }
  else if (myMode == 2)
  {
    GroupAngles->SpinBox_DA->setValue(0.0);
    GroupAngles->SpinBox_DL->setValue(0.0);
    GroupAngles->SpinBox_DA2->setValue(0.0);
  }
  
  UpdatePointCoordinates();
    
  UpdateButtonsState();
  GEOMBase_Helper::displayPreview(true, false, true, true, myLineWidth);
}

//=================================================================================
// function : UpdateButtonsState()
// purpose  :
//=================================================================================
void EntityGUI_3DSketcherDlg::UpdateButtonsState()
{
  if (myPointsList.count() == 0) GroupType->RadioButton1->click();
  GroupType->RadioButton2->setEnabled(myPointsList.count() > 0);
  GroupType->RadioButton3->setEnabled(myPointsList.count() > 0);
  Group3Spin->buttonUndo->setEnabled(myPointsList.count() > 0);
  Group3Spin->buttonRedo->setEnabled(myRedoList.count() > 0);
  GroupAngles->buttonUndo->setEnabled(myPointsList.count() > 0);
  GroupAngles->buttonRedo->setEnabled(myRedoList.count() > 0);
}

//=================================================================================
// function : UpdatePointCoordinates()
// purpose  :Update point coordinates in the control groupbox
//=================================================================================
void EntityGUI_3DSketcherDlg::UpdatePointCoordinates()
{
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  int aPrecision = resMgr->integerValue("Geometry", "length_precision", 6);
  
  if (myPointsList.count() == 0)
  {
    GroupControls->lineEdit_1->setText("");
    GroupControls->lineEdit_2->setText("");
    GroupControls->lineEdit_3->setText("");
    
    GroupControls->lineEdit_4->setText("");
    GroupControls->lineEdit_5->setText("");
    GroupControls->lineEdit_6->setText("");
  }
  else if (myPointsList.count() == 1)
  {
    GroupControls->lineEdit_1->setText(DlgRef::PrintDoubleValue(getLastPoint().x, aPrecision));
    GroupControls->lineEdit_2->setText(DlgRef::PrintDoubleValue(getLastPoint().y, aPrecision));
    GroupControls->lineEdit_3->setText(DlgRef::PrintDoubleValue(getLastPoint().z, aPrecision));
    
    GroupControls->lineEdit_4->setText("");
    GroupControls->lineEdit_5->setText("");
    GroupControls->lineEdit_6->setText("");
  }
  else
  {
    GroupControls->lineEdit_4->setText(DlgRef::PrintDoubleValue(getLastPoint().x, aPrecision));
    GroupControls->lineEdit_5->setText(DlgRef::PrintDoubleValue(getLastPoint().y, aPrecision));
    GroupControls->lineEdit_6->setText(DlgRef::PrintDoubleValue(getLastPoint().z, aPrecision));
  }
  
}

//=================================================================================
// function : ClickOnUndo()
// purpose  :
//=================================================================================
void EntityGUI_3DSketcherDlg::ClickOnUndo()
{
  if (myPointsList.count() > 0) {
    myRedoList.append(myPointsList.takeLast());
    UpdateButtonsState();
    GEOMBase_Helper::displayPreview(true, false, true, true, myLineWidth);

    // Erase dimensions presentations
    SUIT_ViewWindow* vw = SUIT_Session::session()->activeApplication()->desktop()->activeWindow();
    ((SOCC_Viewer*)(vw->getViewManager()->getViewModel()))->Erase(myLengthPrs, true);
    ((SOCC_Viewer*)(vw->getViewManager()->getViewModel()))->Erase(myAnglePrs, true);

    if (myPrsTypeList.back() != NONE)
    {
      // Remove last prepended IO
      removeLastIOFromPrs(TYPE_LENGTH);
    }
    if (myPrsTypeList.back() == TYPE_ANGLE ||
         myPrsTypeList.back() == TYPE_TWO_ANGLES)
    {
      // Remove first Angle IO from presentation
      removeLastIOFromPrs(TYPE_ANGLE);
      if (myPrsTypeList.back() == TYPE_TWO_ANGLES)
      {
        // Remove second  Angle IO
        removeLastIOFromPrs(TYPE_ANGLE);
      }
    }

    // Erase last action type and store it in redo list
    myPrsTypeRedoList.push_back(myPrsTypeList.back());
    myPrsTypeList.pop_back();

    // Display modified presentation
    if (isLengthVisible)
      ((SOCC_Viewer*)(vw->getViewManager()->getViewModel()))->Display(myLengthPrs);
    if (isAngleVisible)
      ((SOCC_Viewer*)(vw->getViewManager()->getViewModel()))->Display(myAnglePrs);
    
    // Update of point coordinates in the control groupbox
    UpdatePointCoordinates();

    updateViewer();
  }
}

//=================================================================================
// function : ClickOnRedo()
// purpose  :
//=================================================================================
void EntityGUI_3DSketcherDlg::ClickOnRedo()
{
  if (myRedoList.count() > 0) {
    myPointsList.append(myRedoList.takeLast());
    UpdateButtonsState();
    GEOMBase_Helper::displayPreview(true, false, true, true, myLineWidth);

    // Erase dimensions presentations
    SUIT_ViewWindow* vw = SUIT_Session::session()->activeApplication()->desktop()->activeWindow();
    ((SOCC_Viewer*)(vw->getViewManager()->getViewModel()))->Erase(myLengthPrs, true);
    ((SOCC_Viewer*)(vw->getViewManager()->getViewModel()))->Erase(myAnglePrs, true);

    if (myPrsTypeRedoList.back() != NONE)
      restoreLastIOToPrs(TYPE_LENGTH);

    if (myPrsTypeRedoList.back() == TYPE_ANGLE ||
         myPrsTypeRedoList.back() == TYPE_TWO_ANGLES)
    {
      // Add a first IO from the Redo list
      restoreLastIOToPrs(TYPE_ANGLE);
      if (myPrsTypeRedoList.back() == TYPE_TWO_ANGLES)
      {
        // Add a second IO from the Redo list
        restoreLastIOToPrs(TYPE_ANGLE);
      }
    }

    // Record last prs type
    myPrsTypeList.push_back(myPrsTypeRedoList.back());
    myPrsTypeRedoList.pop_back();

    // Display modified presentation
    if (isLengthVisible)
      ((SOCC_Viewer*)(vw->getViewManager()->getViewModel()))->Display(myLengthPrs);
    if (isAngleVisible)
      ((SOCC_Viewer*)(vw->getViewManager()->getViewModel()))->Display(myAnglePrs);
    
    // Update of point coordinates in the control groupbox
    UpdatePointCoordinates();

    updateViewer();
  }
}

//=================================================================================
// function : removeLastIO()
// purpose  :
//=================================================================================
void EntityGUI_3DSketcherDlg::removeLastIOFromPrs (int type)
{
  AIS_ListOfInteractive anIOList;

  if (type == TYPE_LENGTH)
  {
    myLengthPrs->GetObjects(anIOList);
    myLengthIORedoList.Prepend(anIOList.First());  // Store last prepended Length IO in redo list
    myLengthPrs->RemoveFirst();                    // Remove it from myLengthPrs
  }
  if (type == TYPE_ANGLE)
  {
    myAnglePrs->GetObjects(anIOList);
    myAngleIORedoList.Prepend(anIOList.First());  // Store last prepended Angle IO in redo list
    myAnglePrs->RemoveFirst();                    // Remove it from myAnglePrs
  }
}

//=================================================================================
// function : restoreLastIO()
// purpose  :
//=================================================================================
void EntityGUI_3DSketcherDlg::restoreLastIOToPrs (int type)
{
  if (type == TYPE_LENGTH)
  {
    myLengthPrs->PrependObject(myLengthIORedoList.First()); // Restore last removed IO
    myLengthIORedoList.RemoveFirst();                       // Remove it from redo list
  }
  if (type == TYPE_ANGLE)
  {
    myAnglePrs->PrependObject(myAngleIORedoList.First());  // Restore last removed IO
    myAngleIORedoList.RemoveFirst();                       // Remove it from redo list
  }
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed
//=================================================================================
void EntityGUI_3DSketcherDlg::SelectionIntoArgument()
{
  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  int nbSel = aSelList.Extent();
  if (nbSel == 1) {
    GEOM::GEOM_Object_var aSelectedObject = GEOMBase::ConvertIOinGEOMObject(aSelList.First());
    if (!CORBA::is_nil(aSelectedObject)) {
      TopoDS_Shape aShape;
      if (GEOMBase::GetShape(aSelectedObject, aShape, TopAbs_SHAPE)) {
        // Explore the shape if its a local selection
        TColStd_IndexedMapOfInteger aMap;
        aSelMgr->GetIndexes(aSelList.First(), aMap);
        if (aMap.Extent() == 1) {
          int anIndex = aMap(1);
          GEOM::GEOM_IShapesOperations_var aShapesOp = getGeomEngine()->GetIShapesOperations(getStudyId());
          aSelectedObject = aShapesOp->GetSubShape(aSelectedObject, anIndex);
        }
      }
      bool isOk = true;
      if (aShape.ShapeType() != TopAbs_VERTEX)
        isOk = GEOMBase::GetShape(aSelectedObject, aShape, TopAbs_VERTEX);
      if (isOk) {
        gp_Pnt aPnt;
        if (GEOMBase::VertexToPoint(aShape, aPnt)) {
          // set coordinates to the Spin Boxes
          double aX, aY, aZ;
          aX = aPnt.X();
          aY = aPnt.Y();
          aZ = aPnt.Z();
          bool blocked = Group3Spin->SpinBox_DX->signalsBlocked();
          Group3Spin->SpinBox_DX->blockSignals(true);
          Group3Spin->SpinBox_DY->blockSignals(true);
          Group3Spin->SpinBox_DZ->blockSignals(true);
          if (GroupType->RadioButton1->isChecked()) {
            Group3Spin->SpinBox_DX->setValue(aX);
            Group3Spin->SpinBox_DY->setValue(aY);
            Group3Spin->SpinBox_DZ->setValue(aZ);
          }
          else if (GroupType->RadioButton2->isChecked()) {
            XYZ xyz = getLastPoint();
            Group3Spin->SpinBox_DX->setValue(aX - xyz.x);
            Group3Spin->SpinBox_DY->setValue(aY - xyz.y);
            Group3Spin->SpinBox_DZ->setValue(aZ - xyz.z);
          }
          Group3Spin->SpinBox_DX->blockSignals(blocked);
          Group3Spin->SpinBox_DY->blockSignals(blocked);
          Group3Spin->SpinBox_DZ->blockSignals(blocked);
        }
      }
    }
  }
  GEOMBase_Helper::displayPreview(true, false, true, true, myLineWidth);
}

//=================================================================================
// function : DeactivateActiveDialog()
// purpose  :
//=================================================================================
void EntityGUI_3DSketcherDlg::DeactivateActiveDialog()
{
  setEnabled(false);
  globalSelection();
  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
  myGeomGUI->SetActiveDialogBox(0);
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void EntityGUI_3DSketcherDlg::ActivateThisDialog()
{
  myGeomGUI->EmitSignalDeactivateDialog();
  setEnabled(true);
  myGeomGUI->SetActiveDialogBox(this);

  connect(myGeomGUI->getApp()->selectionMgr(),
          SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  localSelection(GEOM::GEOM_Object::_nil(), TopAbs_VERTEX);
  GEOMBase_Helper::displayPreview(true, false, true, true, myLineWidth);
}

//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void EntityGUI_3DSketcherDlg::ValueChangedInSpinBox (double newValue)
{
  GEOMBase_Helper::displayPreview(true, false, true, true, myLineWidth);
}

//=================================================================================
// function : BoxChecked()
// purpose  : ChecBoxes management
//=================================================================================
void EntityGUI_3DSketcherDlg::BoxChecked (bool checked)
{
  QCheckBox* send = (QCheckBox*) sender();
  SUIT_ViewWindow* vw = SUIT_Session::session()->activeApplication()->desktop()->activeWindow();

  if (send == GroupAngles->checkBox)
    GroupAngles->SpinBox_DA2->setEnabled(checked);

  else if (send == GroupControls->CheckBox1)
  {
    ((SOCC_Viewer*)(vw->getViewManager()->getViewModel()))->Erase(myLengthPrs, true);
    if (checked){
      ((SOCC_Viewer*)(vw->getViewManager()->getViewModel()))->Display(myLengthPrs);
      isLengthVisible=true;
    }
    else
      isLengthVisible=false;
  }
  else if (send == GroupControls->CheckBox2)
  {
    ((SOCC_Viewer*)(vw->getViewManager()->getViewModel()))->Erase(myAnglePrs, true);
    if (checked)
    {
      ((SOCC_Viewer*)(vw->getViewManager()->getViewModel()))->Display(myAnglePrs);
      isAngleVisible=true;
    }
    else
      isAngleVisible=false;
  }
  else if (send == GroupControls->CheckBox3)
  {
    GroupControls->lineEdit_1->setVisible(checked);
    GroupControls->lineEdit_2->setVisible(checked);
    GroupControls->lineEdit_3->setVisible(checked);
    GroupControls->lineEdit_4->setVisible(checked);
    GroupControls->lineEdit_5->setVisible(checked);
    GroupControls->lineEdit_6->setVisible(checked);
    
    GroupControls->label_1->setVisible(checked);
    GroupControls->label_2->setVisible(checked);
    GroupControls->label_3->setVisible(checked);
    GroupControls->label_4->setVisible(checked);
    GroupControls->label_5->setVisible(checked);
    GroupControls->label_6->setVisible(checked);
    GroupControls->label_7->setVisible(checked);
    GroupControls->label_8->setVisible(checked);
    
    GroupControls->updateGeometry();
    GroupControls->resize(minimumSizeHint());
  } 

  updateGeometry();
  resize(minimumSizeHint());
  
  GEOMBase_Helper::displayPreview(true, false, true, true, myLineWidth);
}

//=================================================================================
// function : ButtonClicked()
// purpose  :
//=================================================================================
void EntityGUI_3DSketcherDlg::ButtonClicked (bool checked)
{
  if (GroupAngles->radioButton_1->isChecked())
    myOrientation = OXY;
  else if (GroupAngles->radioButton_2->isChecked())
    myOrientation = OYZ;
  else
    myOrientation = OXZ;

  GEOMBase_Helper::displayPreview(true, false, true, true, myLineWidth);
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void EntityGUI_3DSketcherDlg::enterEvent (QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr EntityGUI_3DSketcherDlg::createOperation()
{
  return getGeomEngine()->GetICurvesOperations(getStudyId());
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool EntityGUI_3DSketcherDlg::isValid (QString& msg)
{
  bool ok = true;
  ok = Group3Spin->SpinBox_DX->isValid(msg, !IsPreview()) && ok;
  ok = Group3Spin->SpinBox_DY->isValid(msg, !IsPreview()) && ok;
  ok = Group3Spin->SpinBox_DZ->isValid(msg, !IsPreview()) && ok;
  return ok;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool EntityGUI_3DSketcherDlg::execute (ObjectList& objects)
{
  //GEOM::ListOfDouble_var aCoordsArray = new GEOM::ListOfDouble;
  //if (!myOK || myPointsList.size() == 0)
  //  aCoordsArray->length((myPointsList.size()+1)*3);
  //else
  //  aCoordsArray->length(myPointsList.size()*3);

  QStringList aCommands;
  aCommands << "3DSketcher";
  QStringList aParameters;

  //int i = 0;
  QList<XYZ>::const_iterator it;
  for (it = myPointsList.begin(); it != myPointsList.end(); ++it) {
    //aCoordsArray[i++] = (*it).x;
    //aCoordsArray[i++] = (*it).y;
    //aCoordsArray[i++] = (*it).z;
    aCommands << (*it).command;
    aParameters << (*it).params;
  }

  if (!myOK || myPointsList.size() == 0) {
    XYZ xyz = getCurrentPoint();
    //aCoordsArray[i++] = xyz.x;
    //aCoordsArray[i++] = xyz.y;
    //aCoordsArray[i++] = xyz.z;
    aCommands << xyz.command;
    aParameters << xyz.params;
  }

  GEOM::GEOM_ICurvesOperations_var anOper = GEOM::GEOM_ICurvesOperations::_narrow(getOperation());
  //GEOM::GEOM_Object_var anObj = anOper->Make3DSketcher(aCoordsArray);
  GEOM::GEOM_Object_var anObj = anOper->Make3DSketcherCommand(aCommands.join(":").toLatin1().constData());

  if (!anObj->_is_nil()) {
    if (!IsPreview()) anObj->SetParameters(aParameters.join(":").toLatin1().constData());
    objects.push_back(anObj._retn());
  }

  return true;
}

//=================================================================================
// function : SetDoubleSpinBoxStep()
// purpose  : Double spin box management
//=================================================================================
void EntityGUI_3DSketcherDlg::SetDoubleSpinBoxStep (double step)
{
  Group3Spin->SpinBox_DX->setSingleStep(step);
  Group3Spin->SpinBox_DY->setSingleStep(step);
  Group3Spin->SpinBox_DZ->setSingleStep(step);
  GroupAngles->SpinBox_DA->setSingleStep(step);
  GroupAngles->SpinBox_DL->setSingleStep(step);
  GroupAngles->SpinBox_DA2->setSingleStep(step);
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void EntityGUI_3DSketcherDlg::ClickOnOk()
{
  Locker lock(myOK);

  if (!onAccept())
    return;

  ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool EntityGUI_3DSketcherDlg::ClickOnApply()
{
  QString msg;
  if (!isValid(msg)) {
    showError(msg);
    return false;
  }

  if (myPointsList.count() > 0)
    myPointsList.append(myPointsList[0]);

  Locker lock(myOK);

  if (!onAccept())
    return false;

  ClickOnCancel();
  return true;
}

//=================================================================================
// function : getLastPoint()
// purpose  : return last points from list
//=================================================================================
EntityGUI_3DSketcherDlg::XYZ EntityGUI_3DSketcherDlg::getLastPoint() const
{
  return myPointsList.count() > 0 ? myPointsList.last() : XYZ();
}

//=================================================================================
// function : getPenultimatePoint()
// purpose  : return penultimate point from list
//=================================================================================
EntityGUI_3DSketcherDlg::XYZ EntityGUI_3DSketcherDlg::getPenultimatePoint() const
{
  double size = myPointsList.count();
  return size > 1 ? myPointsList.at(size - 2) : XYZ();
}

//=================================================================================
// function : getCurrentPoint()
// purpose  : returns current point
//=================================================================================
EntityGUI_3DSketcherDlg::XYZ EntityGUI_3DSketcherDlg::getCurrentPoint() const
{
  XYZ xyz;

  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  int aPrecision = resMgr->integerValue("Geometry", "length_precision", 7);

  if (myMode == 2) {
    if (GroupAngles->radioButton_1->isChecked())
      xyz.command = "OXY ";
    else if (GroupAngles->radioButton_2->isChecked())
      xyz.command = "OYZ ";
    else
      xyz.command = "OXZ ";

    double anAngle  = GroupAngles->SpinBox_DA->value();
    double aLength  = GroupAngles->SpinBox_DL->value();

    double anAngle2 = 0.0;
    QString da2 = "0";
    if (GroupAngles->checkBox->isChecked()) {
      anAngle2 = GroupAngles->SpinBox_DA2->value();
      da2 = GroupAngles->SpinBox_DA2->text();
    }

    xyz.command +=
      QString::number(anAngle, 'g', aPrecision) + " " +
      QString::number(anAngle2, 'g', aPrecision) + " " +
      QString::number(aLength, 'g', aPrecision);
    xyz.params =
      GroupAngles->SpinBox_DA->text() + ":" +
      da2 + ":" +
      GroupAngles->SpinBox_DL->text();

    // Calculate point coordinates for preview
    anAngle  = anAngle * M_PI/180.0;
    anAngle2 = anAngle2 * M_PI/180.0;
    double aProjectedLength = aLength * cos(anAngle2);

    XYZ xyzP = getLastPoint();
    if (GroupAngles->radioButton_1->isChecked()) // OXY
    {
      xyz.x = xyzP.x + aProjectedLength * cos(anAngle);
      xyz.y = xyzP.y + aProjectedLength * sin(anAngle);
      xyz.z = xyzP.z + aLength * sin(anAngle2);
    }
    else if (GroupAngles->radioButton_2->isChecked()) // OYZ
    {
      xyz.y = xyzP.y + aProjectedLength * cos(anAngle);
      xyz.z = xyzP.z + aProjectedLength * sin(anAngle);
      xyz.x = xyzP.x + aLength * sin(anAngle2);
    }
    else // OXZ
    {
      xyz.z = xyzP.z + aProjectedLength * sin(anAngle);
      xyz.x = xyzP.x + aProjectedLength * cos(anAngle);
      xyz.y = xyzP.y + aLength * sin(anAngle2);
    }
  }
  else {
    if (myMode == 0) { // XYZ
      xyz.x = Group3Spin->SpinBox_DX->value();
      xyz.y = Group3Spin->SpinBox_DY->value();
      xyz.z = Group3Spin->SpinBox_DZ->value();
      xyz.command = "TT ";
    }
    else { // DXDYDZ
      xyz = getLastPoint();
      xyz.x += Group3Spin->SpinBox_DX->value();
      xyz.y += Group3Spin->SpinBox_DY->value();
      xyz.z += Group3Spin->SpinBox_DZ->value();
      xyz.command = "T ";
    }

    double aX  = Group3Spin->SpinBox_DX->value();
    double aY  = Group3Spin->SpinBox_DY->value();
    double aZ  = Group3Spin->SpinBox_DZ->value();

    xyz.command +=
      QString::number(aX, 'g', aPrecision) + " " +
      QString::number(aY, 'g', aPrecision) + " " +
      QString::number(aZ, 'g', aPrecision);
    xyz.params =
      Group3Spin->SpinBox_DX->text() + ":" +
      Group3Spin->SpinBox_DY->text() + ":" +
      Group3Spin->SpinBox_DZ->text();
  }
  return xyz;
}

//================================================================
// Function : displayPreview
// Purpose  : Method for displaying preview of resulting shape
//            Redefined from GEOMBase_Helper.
//================================================================
void EntityGUI_3DSketcherDlg::displayPreview (GEOM::GEOM_Object_ptr object,
                                              const bool            append,
                                              const bool            activate,
                                              const bool            update,
                                              const double          lineWidth,
                                              const int             displayMode,
                                              const int             color)
{
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();

  QColor aColor = resMgr->colorValue("Geometry","line_color",QColor(255,0,0));
  Quantity_NameOfColor line_color = SalomeApp_Tools::color(aColor).Name();

  // set width of displayed shape
  int lw = lineWidth;
  if (lw == -1) {
    lw = resMgr->integerValue("Geometry", "preview_edge_width", -1);
  }
  getDisplayer()->SetWidth(lw);

  // Disable activation of selection
  getDisplayer()->SetToActivate(activate);

  // Make a reference to GEOM_Object
  CORBA::String_var objStr = myGeometryGUI->getApp()->orb()->object_to_string(object);
  getDisplayer()->SetName(objStr.in());

  // Create wire from applied object
  TopoDS_Shape anApplyedWire, aLastSegment;
  if (!createShapes(object, anApplyedWire, aLastSegment))
    return;

  // Set color for preview shape
  getDisplayer()->SetColor(line_color);

  // Build prs
  SALOME_Prs* aPrs = getDisplayer()->BuildPrs(anApplyedWire);
  if (aPrs != 0 && !aPrs->IsNull())
    GEOMBase_Helper::displayPreview(aPrs, append, update);

  getDisplayer()->SetColor(Quantity_NOC_VIOLET);
  aPrs = getDisplayer()->BuildPrs(aLastSegment);

  if (aPrs != 0 && !aPrs->IsNull())
    GEOMBase_Helper::displayPreview(aPrs, append, update);

  getDisplayer()->SetColor(line_color);

  // Display local trihedron if the mode is relatives coordinates or angles
  if (myMode == 1 || myMode == 2)
    displayTrihedron(2);

  // Display preview of suitable dimension presentations
  if (myMode == 2)                 // ANGLES
  {
    double anAngle2 = 0.0;
    if (GroupAngles->checkBox->isChecked())
      anAngle2 = GroupAngles->SpinBox_DA2->value();

    displayAngle(GroupAngles->SpinBox_DA->value(), anAngle2, GroupAngles->SpinBox_DL->value(), myOrientation);
    displayLength(GroupAngles->SpinBox_DL->value(), /*store=*/false, /*type=*/myPrsType);
  }
  if (myMode == 0 || myMode == 1)  // COORDINATES
    displayLength();

  getDisplayer()->UnsetName();

  // Enable activation of displayed objects
  getDisplayer()->SetToActivate(true);
}

//================================================================
// Function : displayTrihedron()
// Purpose  : Method for displaying trihedron
//================================================================
void EntityGUI_3DSketcherDlg::displayTrihedron (int selMode)
{
  // Add trihedron to preview
  SUIT_ViewWindow* vw = SUIT_Session::session()->activeApplication()->desktop()->activeWindow();

  gp_Pnt P(getLastPoint().x,getLastPoint().y,getLastPoint().z);
  Handle(Geom_Axis2Placement) anAxis = new Geom_Axis2Placement(P,gp::DZ(),gp::DX());
  Handle(AIS_Trihedron) anIO = new AIS_Trihedron(anAxis);
  anIO->SetSelectionMode(selMode);

  SOCC_Prs* aSPrs = dynamic_cast<SOCC_Prs*>
    (((SOCC_Viewer*)(vw->getViewManager()->getViewModel()))->CreatePrs(0));

  if (aSPrs) {
    aSPrs->PrependObject(anIO);
    GEOMBase_Helper::displayPreview(aSPrs, true, true);
  }
}

//================================================================
// Function : displayAngle()
// Purpose  : Method for displaying angle dimensions
//================================================================
void EntityGUI_3DSketcherDlg::displayAngle (double theAngle1, double theAngle2,
                                            double theLength, int theOrientation, bool store)
{
  if (Abs(theAngle2 - 90.0) < Precision::Angular() ||
      theLength < Precision::Confusion())
    return;

  SUIT_ViewWindow* vw = SUIT_Session::session()->activeApplication()->desktop()->activeWindow();

  XYZ Last    = getLastPoint();
  XYZ Current = getCurrentPoint();

  gp_Pnt Last_Pnt(Last.x,Last.y,Last.z);
  gp_Pnt Current_Pnt(Current.x,Current.y,Current.z);
  gp_Pnt P1, P2;

  bool twoAngles = GroupAngles->checkBox->isChecked();

  switch(theOrientation)
  {
    case OXY:
    {
      P1 = gp_Pnt(Last.x + theLength,Last.y,Last.z);    // X direction
      P2 = gp_Pnt(Last.x + theLength * cos(theAngle1 * M_PI / 180.),
                  Last.y + theLength * sin(theAngle1 * M_PI / 180.),
                  Last.z);
      break;
    }
    case OYZ:
    {
      P1 = gp_Pnt(Last.x, Last.y + theLength,Last.z);     // Y direction
      P2 = gp_Pnt(Last.x,
                  Last.y + theLength * cos(theAngle1 * M_PI / 180.),
                  Last.z + theLength * sin(theAngle1 * M_PI / 180.));
      break;
    }
    case OXZ:
    {
      P1 = gp_Pnt(Last.x + theLength,Last.y,Last.z);     // X direction
      P2 = gp_Pnt(Last.x + theLength * cos(theAngle1 * M_PI / 180.) ,
                  Last.y,
                  Last.z + theLength * sin(theAngle1 * M_PI / 180.));
      break;
    }
  }

  TopoDS_Vertex V1    = BRepBuilderAPI_MakeVertex(P1);
  TopoDS_Vertex V2    = BRepBuilderAPI_MakeVertex(P2);
  TopoDS_Vertex LastV = BRepBuilderAPI_MakeVertex(Last_Pnt);
  TopoDS_Vertex CurV  = BRepBuilderAPI_MakeVertex(Current_Pnt);
  TopoDS_Edge anEdge1 = BRepBuilderAPI_MakeEdge(LastV, V1);
  TopoDS_Edge anEdge2 = BRepBuilderAPI_MakeEdge(LastV, V2);
  TopoDS_Edge anEdge3 = BRepBuilderAPI_MakeEdge(LastV, CurV);

  gce_MakePln gce_MP (Last_Pnt, P1, P2);
  Handle(Geom_Plane) aPlane = new Geom_Plane(gce_MP.Value());

  // Convert angles to string
  std::string Angle1_str = doubleToString(theAngle1);
  std::string Angle2_str = doubleToString(theAngle2);

  // Create interactive object
  Handle(AIS_AngleDimension) anAngleIO = new AIS_AngleDimension
    (anEdge1, anEdge2, aPlane, theAngle1 * M_PI / 180.,
     TCollection_ExtendedString(Angle1_str.c_str()));
  anAngleIO->SetArrowSize((theAngle1 * M_PI / 180) * (theLength/20));

  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  int w = resMgr->integerValue("Geometry", "measures_line_width", 1);

  Handle(Prs3d_AngleAspect) asp = new Prs3d_AngleAspect();
  asp->LineAspect()->SetWidth(w);
  anAngleIO->Attributes()->SetAngleAspect(asp);

  SOCC_Prs* aSPrs = dynamic_cast<SOCC_Prs*>
    (((SOCC_Viewer*)(vw->getViewManager()->getViewModel()))->CreatePrs(0));

  if (store)
  {
    // Erase dimensions presentations
    ((SOCC_Viewer*)(vw->getViewManager()->getViewModel()))->Erase(myAnglePrs, true);
    myAnglePrs->PrependObject(anAngleIO);

    // Display modified presentation
    if (isAngleVisible)
      ((SOCC_Viewer*)(vw->getViewManager()->getViewModel()))->Display(myAnglePrs);
  }
  else if (aSPrs)
  {
    aSPrs->AddObject(anAngleIO);
  }
  myPrsType = TYPE_ANGLE;  // Overwrite type with ANGLE
  
  if (twoAngles)
  {
    gce_MakePln gce_MP2(Last_Pnt, P2, Current_Pnt);
    Handle(Geom_Plane) aPlane2 = new Geom_Plane(gce_MP2.Value());

    Handle(AIS_AngleDimension) anAngle2IO =
      new AIS_AngleDimension(anEdge2, anEdge3, aPlane2, theAngle2 * M_PI / 180.,
                             TCollection_ExtendedString(Angle2_str.c_str()));
    anAngle2IO->SetArrowSize((theAngle2 * M_PI / 180) * (theLength/20));

    anAngle2IO->Attributes()->SetAngleAspect(asp);

    if (store)
    {
      // Erase dimensions presentations
      ((SOCC_Viewer*)(vw->getViewManager()->getViewModel()))->Erase(myAnglePrs, true);
      myAnglePrs->PrependObject(anAngle2IO);

      // Display modified presentation
      if (isAngleVisible)
        ((SOCC_Viewer*)(vw->getViewManager()->getViewModel()))->Display(myAnglePrs);
    }
    else if (aSPrs)
    {
      aSPrs->AddObject(anAngle2IO);
    }
    
    myPrsType = TYPE_TWO_ANGLES;   // Overwrite type with TWO_ANGLES
  }

  if (!store && isAngleVisible)
  {
    GEOMBase_Helper::displayPreview(aSPrs, true, true);
  }
}

//================================================================
// Function : displayLength()
// Purpose  : Method for displaying length dimensions
//================================================================
void EntityGUI_3DSketcherDlg::displayLength (double theLength, bool store, int type)
{
  SUIT_ViewWindow* vw = SUIT_Session::session()->activeApplication()->desktop()->activeWindow();

  XYZ Last    = getLastPoint();
  XYZ Current = getCurrentPoint();
  XYZ Penultimate = getPenultimatePoint();
  
  bool withAngle = (type == TYPE_ANGLE || type == TYPE_TWO_ANGLES);
  bool twoAngles = (type == TYPE_TWO_ANGLES);
  
  double aLength = 0.0;

  if (theLength < 0)  // Calculate length if not given
  {
    aLength = sqrt((Last.x - Current.x)*(Last.x - Current.x) +
                   (Last.y - Current.y)*(Last.y - Current.y) +
                   (Last.z - Current.z)*(Last.z - Current.z));
  }
  else
    aLength = theLength;

  if (aLength<Precision::Confusion())
    return;

  gp_Pnt P1 = gp_Pnt(Last.x,Last.y,Last.z);
  gp_Pnt P2 = gp_Pnt(Current.x,Current.y,Current.z);
  gp_Pnt P3 = gp_Pnt(Penultimate.x,Penultimate.y,Penultimate.z);

  gp_Vec Vec1(P1,P2);
  gp_Vec Vec2(P1,P3);
  gp_Vec Vec3 = Vec1;

  TopoDS_Vertex aVert1 = BRepBuilderAPI_MakeVertex(P1);
  TopoDS_Vertex aVert2 = BRepBuilderAPI_MakeVertex(P2);

  // Convert length to string
  std::string aLength_str = doubleToString(aLength);
  
  // Define the suitable plane for right relative positioning of dimension presentations
  
  gp_Dir aNormal;                  // Normal defining the plane of the presentation 
  if (withAngle)                   // If one angle
  {
    switch(myOrientation)
    {
      case OXY:
      {
        aNormal = gp::DZ().Crossed(gp_Dir(Vec1));//--> the plane is orthogonal to the angle presentation   
                                                 //    plane and contains the current edge
        if (twoAngles)            // If two angles 
        { 
          gp_XYZ Vec1_XY(Vec1.X(),Vec1.Y(),0.0);// --> define Vec3 as the projection of the current 
          Vec3 = gp_Vec(Vec1_XY);               //     edge on the plane chosen for the first angle
        }
        break;
      }
      case OYZ:
      {
        aNormal = gp::DX().Crossed(gp_Dir(Vec1));
        if (twoAngles)
        {
          gp_XYZ Vec1_YZ(0.0,Vec1.Y(),Vec1.Z());
          Vec3 = gp_Vec(Vec1_YZ);
        }
        break;
      }
      case OXZ:
      {
        aNormal = gp::DY().Crossed(gp_Dir(Vec1));
        if (twoAngles)
        {
          gp_XYZ Vec1_XZ(Vec1.X(),0.0,Vec1.Z());
          Vec3 = gp_Vec(Vec1_XZ);
        }
        break;
      }
    }
    if(twoAngles                  // If two angles 
      && Abs(Vec1.CrossMagnitude(Vec3)) > Precision::Confusion())                                  
    {                                                                    
      aNormal = gp_Dir(Vec1.Crossed(Vec3));//       --> set the normal as the cross product of
    }                                      //           the current edge with its projection           
  }                                        //           it ensures that the dimension changes     
  else
  {
    // Check colinearity
    if (Abs(Vec1.CrossMagnitude(Vec2)) < Precision::Confusion())
    {
      Vec2 = gp_Vec(gp::DX());
      if (Abs(Vec1.CrossMagnitude(Vec2)) < Precision::Confusion())
      {
        Vec2 = gp_Vec(gp::DY());
      }
    }
    aNormal = gp_Dir(Vec1.Crossed(Vec2)); // If no angles --> the plane is the one formed by
  }                                       //                  the last edge and the current one
  
  // Plane construction
  gce_MakePln gce_MP(P1, aNormal);
  Handle(Geom_Plane) aPlane = new Geom_Plane(gce_MP.Value());

  Handle(AIS_LengthDimension) anIO =
    new AIS_LengthDimension(aVert1,
                            aVert2,
                            aPlane,
                            aLength,
                            TCollection_ExtendedString(aLength_str.c_str()));
  anIO->SetArrowSize(aLength/20);

  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  int w = resMgr->integerValue("Geometry", "measures_line_width", 1);
  Handle(Prs3d_LengthAspect) asp = new Prs3d_LengthAspect();
  asp->LineAspect()->SetWidth(w);
  anIO->Attributes()->SetLengthAspect(asp);

  if (store)
  {
    // Erase length dimensions presentation
    ((SOCC_Viewer*)(vw->getViewManager()->getViewModel()))->Erase(myLengthPrs, true);
    myLengthPrs->PrependObject(anIO);

    // Display modified presentation
    if (isLengthVisible)
      ((SOCC_Viewer*)(vw->getViewManager()->getViewModel()))->Display(myLengthPrs);
  }
  else if (isLengthVisible)
  {
    SOCC_Prs* aSPrs = dynamic_cast<SOCC_Prs*>
      (((SOCC_Viewer*)(vw->getViewManager()->getViewModel()))->CreatePrs(0));
    if (aSPrs)
    {
      aSPrs->PrependObject(anIO);
      GEOMBase_Helper::displayPreview(aSPrs, true, true);
    }
  }
  
  myPrsType = TYPE_LENGTH;
}

//================================================================
// Function : createShapes
// Purpose  : Create applyed wire, and last segment from entry object
//================================================================
bool EntityGUI_3DSketcherDlg::createShapes (GEOM::GEOM_Object_ptr /*theObject*/,
                                            TopoDS_Shape&         theApplyedWire,
                                            TopoDS_Shape&         theLastSegment)
{
  QList<gp_Pnt> points;
  foreach (XYZ xyz, myPointsList) {
    gp_Pnt p(xyz.x, xyz.y, xyz.z);
    if (points.isEmpty() || points.last().Distance(p) > gp::Resolution())
      points << p;
  }

  if (points.count() == 1) {
    // only one point is created
    BRepBuilderAPI_MakeVertex mkVertex (points.last());
    theApplyedWire = mkVertex.Shape();
  }
  else if (points.count() > 1) {
    // wire is created
    BRepBuilderAPI_MakePolygon mkWire;
    foreach(gp_Pnt p, points)
      mkWire.Add(p);
    theApplyedWire = mkWire.Shape();
  }

  XYZ curxyz = getCurrentPoint();
  gp_Pnt curpnt(curxyz.x, curxyz.y, curxyz.z);

  if (points.isEmpty() || points.last().Distance(curpnt) <= gp::Resolution()) {
    BRepBuilderAPI_MakeVertex mkVertex (curpnt);
    theLastSegment = mkVertex.Shape();
  }
  else {
    BRepBuilderAPI_MakeEdge mkEdge(points.last(), curpnt);
    theLastSegment = mkEdge.Shape();
  }

  /* VSR: old algorithm does not work properly, see bug 0020899
  TopoDS_Shape aShape;
  if (!GEOMBase::GetShape(theObject, aShape))
    return false;

  if (aShape.ShapeType() != TopAbs_WIRE && aShape.ShapeType() != TopAbs_VERTEX)
    return false;

  theApplyedWire = aShape;
  if (myOK)
     return true;

  BRepBuilderAPI_MakeWire aBuilder;
  TopExp_Explorer edgeExp(aShape, TopAbs_EDGE);
  while (1) {
    TopoDS_Shape anEdge = edgeExp.Current();
    edgeExp.Next();
    if (edgeExp.More()) // i.e. non-last edge
      aBuilder.Add(TopoDS::Edge(anEdge));
    else {
      theLastSegment = anEdge;
      break;
    }
  }

  if (aBuilder.IsDone()) {
    theApplyedWire = aBuilder.Shape();
  }
  else if (!theLastSegment.IsNull()) {
    TopExp_Explorer vertexExp(theLastSegment, TopAbs_VERTEX);
    theApplyedWire = vertexExp.Current();
    }
  */

  return true;
}

//================================================================
// Function : doubleToString
// Purpose  : converts double to string
//================================================================
std::string EntityGUI_3DSketcherDlg::doubleToString (double num)
{
  // truncate num
  int digNum = 5;
  char format = 'g'; // truncated to a number of significant digits

  return QString::number(num, format, digNum).toStdString();
}
