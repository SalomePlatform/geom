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

// GEOM GEOMGUI : GUI for Geometry component
// File   : EntityGUI_3DSketcherDlg.cxx
// Author : DMV, OCN

#include <cmath>
#include <string>
#include <boost/lexical_cast.hpp>

#include "EntityGUI_3DSketcherDlg.h"
#include "EntityGUI_Widgets.h"

#include <Basics_OCCTVersion.hxx>

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
#include <SalomeApp_DoubleSpinBox.h>
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
#include <Geom_Axis2Placement.hxx>
#include <Geom_Plane.hxx>
#include <SelectMgr_Selection.hxx>
#include <gce_MakePln.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_DimensionAspect.hxx>
#include <Prs3d_TextAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Text.hxx>
#include <Graphic3d_VerticalTextAlignment.hxx>
#include <Graphic3d_HorizontalTextAlignment.hxx>
#include <Graphic3d_AspectText3d.hxx>
#include <Font_FontAspect.hxx>

// This include must be *AFTER* SOCC_ViewModel.h because
// of the constant ROTATE which is a #define in
// GEOMImpl_Types.hxx and an enum in SOCC_ViewModel.h
#include <GEOMImpl_Types.hxx>

// TODO
//
// Avoid duplication of angle coordinates in cylindrical mode
// Check spherical mode in absolute

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

DEFINE_STANDARD_HANDLE(AIS_Text, AIS_InteractiveObject)

class AIS_Text:public AIS_InteractiveObject
{
public:
  OCCT_DEFINE_STANDARD_RTTIEXT(AIS_Text,AIS_InteractiveObject)

  AIS_Text(){};

  AIS_Text
    (
      const TCollection_ExtendedString& , const gp_Pnt& ,
      Quantity_Color color,
      Standard_Integer aHJust,
      Standard_Integer aVJust ,
      Standard_Real Angle ,
      Standard_Boolean Zoom ,
      Standard_Real  Height,
      Font_FontAspect FontAspect,
      Standard_CString Font
    );

private:

  void Compute (  const Handle(PrsMgr_PresentationManager3d)& aPresentationManager,
                  const Handle(Prs3d_Presentation)& aPresentation,
                  const Standard_Integer aMode);

  void ComputeSelection (  const Handle(SelectMgr_Selection)& aSelection,
                           const Standard_Integer aMode){} ;

protected:
  TCollection_ExtendedString          aText;
  gp_Pnt                              aPosition;
  Standard_Real                       Red;
  Standard_Real                       Green;
  Standard_Real                       Blue;
  Standard_Real                       aAngle;
  Standard_Real                       aHeight;
  Standard_Boolean                    aZoomable;
  Quantity_Color                      aColor;
  Standard_CString                    aFont;
  Font_FontAspect                      aFontAspect;
  Graphic3d_HorizontalTextAlignment   aHJustification;
  Graphic3d_VerticalTextAlignment     aVJustification;
};

OCCT_IMPLEMENT_STANDARD_RTTIEXT(AIS_Text, AIS_InteractiveObject)

AIS_Text::AIS_Text( const TCollection_ExtendedString& text, const gp_Pnt& position,
                          Quantity_Color    color       = Quantity_NOC_YELLOW,
                          Standard_Integer  aHJust      = Graphic3d_HTA_LEFT,
                          Standard_Integer  aVJust      = Graphic3d_VTA_BOTTOM,
                          Standard_Real     angle       = 0.0 ,
                          Standard_Boolean  zoomable    = Standard_False,
                          Standard_Real     height      = 16.,
                          Font_FontAspect    fontAspect  = Font_FA_Regular,
                          Standard_CString  font        = "Courier")
{
  aText           = text;
  aPosition       = position;
  aHJustification = Graphic3d_HorizontalTextAlignment(aHJust);
  aVJustification = Graphic3d_VerticalTextAlignment(aVJust);
  aAngle          = angle;
  aZoomable       = zoomable;
  aHeight         = height;
  aColor          = color;
  aFontAspect     = fontAspect;
  aFont           = font;
};

void AIS_Text::Compute(const Handle(PrsMgr_PresentationManager3d)& aPresentationManager,
                          const Handle(Prs3d_Presentation)& aPresentation,
                          const Standard_Integer aMode)
{

  aPresentation->Clear();

  Handle(Prs3d_TextAspect) asp = myDrawer->TextAspect();

  asp->SetFont(aFont);
  asp->SetColor(aColor);
  asp->SetHeight(aHeight); // I am changing the myHeight value

  asp->SetHorizontalJustification(aHJustification);
  asp->SetVerticalJustification(aVJustification);
  asp->Aspect()->SetTextZoomable(aZoomable);
  asp->Aspect()->SetTextAngle(aAngle);
  asp->Aspect()->SetTextFontAspect(aFontAspect);
  Prs3d_Text::Draw(aPresentation, asp, aText, aPosition);
};

bool isSame (double d1, double d2)
{ 
  return Abs(d1 - d2) <= Precision::Confusion();
}

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
    myLengthIORedoList(),
    myIsUndoRedo(false)
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

  GroupType = new EntityGUI_Type(centralWidget());
  GroupType->GroupType2->setTitle(tr("GEOM_COORDINATES_TYPE"));
  GroupType->GroupType1->setTitle(tr("GEOM_MODE"));
  GroupType->RadioButton1->setText(tr("GEOM_SKETCHER_ABS"));
  GroupType->RadioButton2->setText(tr("GEOM_SKETCHER_REL"));
  GroupType->RadioButton3->setText(tr("(X,Y,Z)"));
  GroupType->RadioButton4->setText(tr("GEOM_ANGLES"));
 
  myTypeGroup1 = new QButtonGroup(this);
  myTypeGroup1->setExclusive(true);
  myTypeGroup1->addButton(GroupType->RadioButton1, 0);
  myTypeGroup1->addButton(GroupType->RadioButton2, 1);
  
  myTypeGroup2 = new QButtonGroup(this);
  myTypeGroup2->setExclusive(true);
  myTypeGroup2->addButton(GroupType->RadioButton3, 0);
  myTypeGroup2->addButton(GroupType->RadioButton4, 1);

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
  GroupAngles->TextLabel1->setText(tr("GEOM_LENGTH"));
  GroupAngles->TextLabel2->setText(tr("GEOM_ANGLE")); 
  GroupAngles->checkBox  ->setText(tr("GEOM_ANGLE_2"));
  GroupAngles->checkBox_2->setText(tr("GEOM_HEIGHT"));

  GroupControls = new EntityGUI_Controls(centralWidget());
  GroupControls->GroupBox1->setTitle(tr("GEOM_CONTROLS"));
  GroupControls->CheckBox1->setText(tr("GEOM_SHOW_LENGTH")); 
  GroupControls->CheckBox2->setText(tr("GEOM_SHOW_ANGLE"));
  GroupControls->CheckBox3->setText(tr("GEOM_SHOW_POINTS_COORD"));
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
  GroupControls->label_7->setText(tr("GEOM_START"));
  GroupControls->label_8->setText(tr("GEOM_END"));

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
  myPrsType = prsType();

  SUIT_ViewWindow* vw = SUIT_Session::session()->activeApplication()->desktop()->activeWindow();
  myAnglePrs = dynamic_cast<SOCC_Prs*>(((SOCC_Viewer*)(vw->getViewManager()->getViewModel()))->CreatePrs(0));
  myLengthPrs = dynamic_cast<SOCC_Prs*>(((SOCC_Viewer*)(vw->getViewManager()->getViewModel()))->CreatePrs(0));
  myTextPrs = dynamic_cast<SOCC_Prs*>(((SOCC_Viewer*)(vw->getViewManager()->getViewModel()))->CreatePrs(0));

  localSelection(TopAbs_VERTEX);

  /* Get setting of step value from file configuration */
  double step = SUIT_Session::session()->resourceMgr()->doubleValue("Geometry", "SettingsGeomStep", 100.0);

  /* min, max, step and decimals for spin boxes */
  initSpinBox(Group3Spin->SpinBox_DX, COORD_MIN, COORD_MAX, step, "length_precision");
  initSpinBox(Group3Spin->SpinBox_DY, COORD_MIN, COORD_MAX, step, "length_precision");
  initSpinBox(Group3Spin->SpinBox_DZ, COORD_MIN, COORD_MAX, step, "length_precision");

  initSpinBox(GroupAngles->SpinBox_DA , -360.0, 360.0, step, "angular_precision");
  initSpinBox(GroupAngles->SpinBox_DA2,  -90.0,  90.0, step, "angular_precision");
  initSpinBox(GroupAngles->SpinBox_DL , COORD_MIN, COORD_MAX, step, "length_precision");
  initSpinBox(GroupAngles->SpinBox_DH , COORD_MIN, COORD_MAX, step, "length_precision");

  Group3Spin->SpinBox_DX->setValue(0.0);
  Group3Spin->SpinBox_DY->setValue(0.0);
  Group3Spin->SpinBox_DZ->setValue(0.0);

  GroupAngles->SpinBox_DA->setValue(0.0);
  GroupAngles->SpinBox_DA2->setValue(0.0);
  GroupAngles->SpinBox_DL->setValue(0.0);
  GroupAngles->SpinBox_DH->setValue(0.0);

  GroupAngles->radioButton_1->setChecked(true);
  GroupAngles->checkBox->setChecked(false);
  GroupAngles->checkBox_2->setChecked(false);
  GroupAngles->SpinBox_DA2->setEnabled(false);
  GroupAngles->SpinBox_DH->setEnabled(false);

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

  connect(Group3Spin->buttonApply,   SIGNAL(clicked()), this, SLOT(ClickOnAddPoint()));
  connect(Group3Spin->buttonUndo,    SIGNAL(clicked()), this, SLOT(ClickOnUndo()));
  connect(Group3Spin->buttonRedo,    SIGNAL(clicked()), this, SLOT(ClickOnRedo())) ;

  connect(GroupAngles->buttonApply,  SIGNAL(clicked()), this, SLOT(ClickOnAddPoint()));
  connect(GroupAngles->buttonUndo,   SIGNAL(clicked()), this, SLOT(ClickOnUndo()));
  connect(GroupAngles->buttonRedo,   SIGNAL(clicked()), this, SLOT(ClickOnRedo())) ;

  connect(myTypeGroup1, SIGNAL(buttonClicked(int)),  this, SLOT(TypeClicked(int)));
  connect(myTypeGroup2, SIGNAL(buttonClicked(int)),  this, SLOT(TypeClicked(int)));

  connect(Group3Spin->SpinBox_DX,       SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(Group3Spin->SpinBox_DY,       SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(Group3Spin->SpinBox_DZ,       SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));

  connect(GroupAngles->SpinBox_DA,      SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupAngles->SpinBox_DA2,     SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupAngles->SpinBox_DL,      SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupAngles->SpinBox_DH,      SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));

  connect(GroupAngles->radioButton_1,   SIGNAL(clicked (bool)), this, SLOT(ButtonClicked(bool))) ;
  connect(GroupAngles->radioButton_2,   SIGNAL(clicked (bool)), this, SLOT(ButtonClicked(bool))) ;
  connect(GroupAngles->radioButton_3,   SIGNAL(clicked (bool)), this, SLOT(ButtonClicked(bool))) ;

  connect(GroupAngles->checkBox,        SIGNAL(clicked (bool)), this, SLOT(BoxChecked (bool))) ;
  connect(GroupAngles->checkBox_2,      SIGNAL(clicked (bool)), this, SLOT(BoxChecked (bool))) ;
  connect(GroupControls->CheckBox1,     SIGNAL(clicked (bool)), this, SLOT(BoxChecked (bool))) ;
  connect(GroupControls->CheckBox2,     SIGNAL(clicked (bool)), this, SLOT(BoxChecked (bool))) ;
  connect(GroupControls->CheckBox3,     SIGNAL(clicked (bool)), this, SLOT(BoxChecked (bool))) ;

  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), this, SLOT(SetDoubleSpinBoxStep(double)));

  connect(myGeomGUI, SIGNAL(SignalDeactivateActiveDialog()), this, SLOT(DeactivateActiveDialog()));
  connect(myGeomGUI, SIGNAL(SignalCloseAllDialogs()),        this, SLOT(ClickOnCancel()));

  initName(tr("GEOM_3DSKETCHER"));
  
  GroupControls->CheckBox3->click();

  UpdateButtonsState();
  
  myMode      = 0;
  myCoordType = 0;
  
  GEOMBase_Helper::displayPreview(true, false, true, true, myLineWidth);
}

//=================================================================================
// function : TypeClicked()
// purpose  : Radio button management
//=================================================================================
void EntityGUI_3DSketcherDlg::TypeClicked (int id)
{
  QButtonGroup* send = (QButtonGroup*) sender();
  
  int coordType = myCoordType;
  int mode      = myMode;
  
  if (send == myTypeGroup1)
  {
    if(id == myMode) return;
    mode = id;   
  }
  else if (send == myTypeGroup2)
  {
    if (id == myCoordType) return;
    coordType = id;  
  }
  
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

  if (coordType == 0)  // Cartesian coordinates
  {
    if (mode == 0) {   // XYZ
      Group3Spin->TextLabel1->setText(tr("GEOM_SKETCHER_X2"));
      Group3Spin->TextLabel2->setText(tr("GEOM_SKETCHER_Y2"));
      Group3Spin->TextLabel3->setText(tr("GEOM_SKETCHER_Z2"));
      if (myCoordType == 0 && myMode == 1)
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
      if (myCoordType == 0 && myMode == 0)
      {
        if (okx) Group3Spin->SpinBox_DX->setValue(Group3Spin->SpinBox_DX->value() - xyz.x);
        if (oky) Group3Spin->SpinBox_DY->setValue(Group3Spin->SpinBox_DY->value() - xyz.y);
        if (okz) Group3Spin->SpinBox_DZ->setValue(Group3Spin->SpinBox_DZ->value() - xyz.z);
      }
      Group3Spin->buttonApply->setFocus();
    }
  }
  else if (coordType == 1) // Angles and Length
  {
    Group3Spin->hide();
    GroupAngles->show();
    GroupAngles->buttonApply->setFocus();
  }

  Group3Spin->SpinBox_DX->blockSignals(blocked);
  Group3Spin->SpinBox_DY->blockSignals(blocked);
  Group3Spin->SpinBox_DZ->blockSignals(blocked);

  myMode = mode;
  myCoordType = coordType;

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
  
  myPrsType   = prsType();
  
//   if(myMode == 1)
    displayDimensions( /*store = */true);

  myPointsList.append(getCurrentPoint());
  myRedoList.clear();
  myLengthIORedoList.Clear();
  myAngleIORedoList.Clear();
  myTextIORedoList.Clear();

  if (myCoordType == 0 && myMode == 1)     // RELATIVE CARTESIAN COORDINATES
  {
    Group3Spin->SpinBox_DX->setValue(0.0);
    Group3Spin->SpinBox_DY->setValue(0.0);
    Group3Spin->SpinBox_DZ->setValue(0.0);
  }
  else if (myCoordType == 1 && myMode == 1) // RELATIVE ANGULAR COORDINATES
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
  if (myPointsList.count() == 0) 
  {
    GroupType->RadioButton1->click();
    GroupType->RadioButton3->click();
  }
  GroupType->RadioButton2->setEnabled(myPointsList.count() > 0);
//   GroupType->RadioButton3->setEnabled(myPointsList.count() > 0);
//   GroupType->RadioButton4->setEnabled(myPointsList.count() > 0);
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
  myIsUndoRedo = true;
  if (myPointsList.count() > 0) {
    
    myRedoList.append(myPointsList.last());

    // Erase dimensions presentations
    SUIT_ViewWindow* vw = SUIT_Session::session()->activeApplication()->desktop()->activeWindow();
    ((SOCC_Viewer*)(vw->getViewManager()->getViewModel()))->Erase(myLengthPrs, true);
    ((SOCC_Viewer*)(vw->getViewManager()->getViewModel()))->Erase(myAnglePrs, true);
    ((SOCC_Viewer*)(vw->getViewManager()->getViewModel()))->Erase(myTextPrs, true);
    
    removeLastIOFromPrs();

    // Display modified presentation
    if (isLengthVisible)
      ((SOCC_Viewer*)(vw->getViewManager()->getViewModel()))->Display(myLengthPrs);
    if (isAngleVisible)
      ((SOCC_Viewer*)(vw->getViewManager()->getViewModel()))->Display(myAnglePrs);
    ((SOCC_Viewer*)(vw->getViewManager()->getViewModel()))->Display(myTextPrs);
    
    // Remove last point from list
    myWorkPoint = myPointsList.last();
    myPointsList.removeLast();
    GEOMBase_Helper::displayPreview(true, false, true, true, myLineWidth);
    UpdateButtonsState();
    
    // Update of point coordinates in the control groupbox
    UpdatePointCoordinates();

    updateViewer();
  }
  myIsUndoRedo = false;
}

//=================================================================================
// function : ClickOnRedo()
// purpose  :
//=================================================================================
void EntityGUI_3DSketcherDlg::ClickOnRedo()
{
  myIsUndoRedo = true;
  if (myRedoList.count() > 0) {
    
    myPointsList.append(myRedoList.last());

    // Erase dimensions presentations
    SUIT_ViewWindow* vw = SUIT_Session::session()->activeApplication()->desktop()->activeWindow();
    ((SOCC_Viewer*)(vw->getViewManager()->getViewModel()))->Erase(myLengthPrs, true);
    ((SOCC_Viewer*)(vw->getViewManager()->getViewModel()))->Erase(myAnglePrs, true);
    ((SOCC_Viewer*)(vw->getViewManager()->getViewModel()))->Erase(myTextPrs, true);
    
    restoreLastIOToPrs();

    // Display modified presentation
    if (isLengthVisible)
      ((SOCC_Viewer*)(vw->getViewManager()->getViewModel()))->Display(myLengthPrs);
    if (isAngleVisible)
      ((SOCC_Viewer*)(vw->getViewManager()->getViewModel()))->Display(myAnglePrs);
    ((SOCC_Viewer*)(vw->getViewManager()->getViewModel()))->Display(myTextPrs);
    
    // Remove last point from redo list
    myWorkPoint = myRedoList.last();
    myRedoList.removeLast();
    GEOMBase_Helper::displayPreview(true, false, true, true, myLineWidth);
    UpdateButtonsState();
    
    // Update of point coordinates in the control groupbox
    UpdatePointCoordinates();

    updateViewer();
  }
  myIsUndoRedo = false;
}

//=================================================================================
// function : removeLastIO()
// purpose  :
//=================================================================================
void EntityGUI_3DSketcherDlg::removeLastIOFromPrs ()
{
  AIS_ListOfInteractive anIOList;
  XYZ Last = getLastPoint();
  
  for (int l = 0; l<Last.L; l++)
  {
    myLengthPrs->GetObjects(anIOList);
    myLengthIORedoList.Prepend(anIOList.First());  // Store last prepended Length IO in redo list
    myLengthPrs->RemoveFirst();                    // Remove it from myLengthPrs
  }
  for (int a = 0; a<Last.A; a++)
  {
    myAnglePrs->GetObjects(anIOList);
    myAngleIORedoList.Prepend(anIOList.First());  // Store last prepended Angle IO in redo list
    myAnglePrs->RemoveFirst();                    // Remove it from myAnglePrs
  }
  for (int t = 0; t<Last.T; t++)
  {
    myTextPrs->GetObjects(anIOList);
    myTextIORedoList.Prepend(anIOList.First());  // Store last prepended Text IO in redo list
    myTextPrs->RemoveFirst();                    // Remove it from myTextPrs
  }
}

//=================================================================================
// function : restoreLastIO()
// purpose  :
//=================================================================================
void EntityGUI_3DSketcherDlg::restoreLastIOToPrs ()
{
  XYZ LastDeleted = myRedoList.last();
  
  for (int l = 0; l<LastDeleted.L; l++)
  {
    myLengthPrs->PrependObject(myLengthIORedoList.First()); // Restore last removed IO
    myLengthIORedoList.RemoveFirst();                       // Remove it from redo list
  } 
  for (int a = 0; a<LastDeleted.A; a++)
  {
    myAnglePrs->PrependObject(myAngleIORedoList.First());  // Restore last removed IO
    myAngleIORedoList.RemoveFirst();                       // Remove it from redo list
  }
  for (int t = 0; t<LastDeleted.T; t++)
  {
    myTextPrs->PrependObject(myTextIORedoList.First());  // Restore last removed IO
    myTextIORedoList.RemoveFirst();                       // Remove it from redo list
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

  localSelection(TopAbs_VERTEX);
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
  {
    GroupAngles->SpinBox_DA2->setEnabled(checked);
    if(checked)
    {
      GroupAngles->SpinBox_DH->setEnabled(false);
      GroupAngles->checkBox_2->setChecked(false);
    }
  }
  else if (send == GroupAngles->checkBox_2)
  {
    GroupAngles->SpinBox_DH->setEnabled(checked);
    if(checked)
    {
      GroupAngles->SpinBox_DA2->setEnabled(false);
      GroupAngles->checkBox->setChecked(false);
    }
  }

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

//   MESSAGE("aCommands.last() = "<< aCommands.last().toStdString());
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
  GroupAngles->SpinBox_DA ->setSingleStep(step);
  GroupAngles->SpinBox_DA2->setSingleStep(step);
  GroupAngles->SpinBox_DL->setSingleStep(step);
  GroupAngles->SpinBox_DH->setSingleStep(step);
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
  
  bool spherical   = GroupAngles->checkBox->isChecked();
  bool cylindrical = GroupAngles->checkBox_2->isChecked();

  if (myCoordType == 1) 
  {
    if (GroupAngles->radioButton_1->isChecked())
      xyz.command = "OXY";
    else if (GroupAngles->radioButton_2->isChecked())
      xyz.command = "OYZ";
    else
      xyz.command = "OXZ";
    
    if (cylindrical)       // Cylindrical coordinates (radius, angle, height)
      xyz.command += "C";
    else
      xyz.command += "S";  // Spherical coordinates (radius, angle1, angle2) --> polar if angle2 = 0
    
    if (myMode == 0)       // Absolute coordinates
      xyz.command += "A ";
    else if (myMode == 1)  // Relative coordinates
      xyz.command += "R ";
      

    double anAngle  = GroupAngles->SpinBox_DA->value();
    double aLength  = GroupAngles->SpinBox_DL->value();

    double anAngle2 = 0.0;
    QString da2 = "0";
    if (spherical) {       // Spherical coordinates (radius, angle1, angle2)
      anAngle2 = GroupAngles->SpinBox_DA2->value();
      da2 = GroupAngles->SpinBox_DA2->text();
    }
    
    double aHeight = 0.0;
    QString dh = "0";
    if (cylindrical) {
      aHeight = GroupAngles->SpinBox_DH->value();
      dh = GroupAngles->SpinBox_DH->text();
    }

    xyz.command +=
      QString::number(anAngle, 'g', aPrecision) + " ";
    xyz.params =
      GroupAngles->SpinBox_DA->text() + ":"; 
    
    if(cylindrical)
    {
      xyz.command +=
        QString::number(aHeight, 'g', aPrecision) + " ";
      xyz.params +=
        dh + ":";
    }
    else                  // Spherical or polar coordinates
    {
      xyz.command +=
        QString::number(anAngle2, 'g', aPrecision) + " ";
      xyz.params +=
        dh + ":";
    }
    
    xyz.command +=
      QString::number(aLength, 'g', aPrecision);
    xyz.params +=
      GroupAngles->SpinBox_DL->text();

      
    // Calculate point coordinates for preview
    anAngle  = anAngle * M_PI/180.0;
    anAngle2 = anAngle2 * M_PI/180.0;
    double aProjectedLength = aLength * cos(anAngle2);
    
    XYZ xyzP = getLastPoint();
    if (myMode == 0)
      xyzP.x=xyzP.y=xyzP.z=0.0; 
    if (GroupAngles->radioButton_1->isChecked()) // OXY
    {
      xyz.x = xyzP.x + aProjectedLength * cos(anAngle);
      xyz.y = xyzP.y + aProjectedLength * sin(anAngle);
      if (cylindrical)
        xyz.z = xyzP.z + aHeight;
      else
        xyz.z = xyzP.z + aLength * sin(anAngle2);
    }
    else if (GroupAngles->radioButton_2->isChecked()) // OYZ
    {
      xyz.y = xyzP.y + aProjectedLength * cos(anAngle);
      xyz.z = xyzP.z + aProjectedLength * sin(anAngle);
      if (cylindrical)
        xyz.x = xyzP.x + aHeight;
      else
        xyz.x = xyzP.x + aLength * sin(anAngle2);
    }
    else // OXZ
    {
      xyz.z = xyzP.z + aProjectedLength * sin(anAngle);
      xyz.x = xyzP.x + aProjectedLength * cos(anAngle);
      if (cylindrical)
        xyz.y = xyzP.y + aHeight;
      else
        xyz.y = xyzP.y + aLength * sin(anAngle2);
    }
  }
  else if(myCoordType == 0) {
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
  // Update point presentation type
  xyz.A = myPrsType.A;  // Number of angle diomensions
  xyz.L = myPrsType.L;  // Number of length dimensions
  xyz.T = myPrsType.T;  // Number of text objects
  
  return xyz;
}


//=================================================================================
// function : getPresentationPlane()
// purpose  : returns the suitable plane for right
//            relative positioning of dimension presentations
//=================================================================================
gp_Dir EntityGUI_3DSketcherDlg::getPresentationPlane() const
{ 
  bool withAngle = (myCoordType == 1);
  bool twoAngles = GroupAngles->checkBox->isChecked();
  
  XYZ Last    = getLastPoint();
  XYZ Current;
  if( myIsUndoRedo ) {
      Current = myWorkPoint;
  } else {
    Current = getCurrentPoint();
  }
  
  XYZ Penultimate = getPenultimatePoint();
  
  gp_Pnt P1 = gp_Pnt(Last.x,Last.y,Last.z);
  if (myMode == 0) // Absolute coordinates
    P1 = gp::Origin();
  
  gp_Pnt P2 = gp_Pnt(Current.x,Current.y,Current.z);
  gp_Pnt P3 = gp_Pnt(Penultimate.x,Penultimate.y,Penultimate.z);
  
  gp_Vec Vec1(P1,P2);
  gp_Vec Vec2(P1,P3);
  
  gp_Dir aNormal;                  // Normal defining the plane of the presentation 
  
  if (withAngle)                   // If one angle
  {
    // Transformation from the current coordinate system 
    // to the reference coordinate system
    gp_Trsf aTransform = toReferenceSystem (P1);
    gp_Dir N1 = gp::DZ();
    gp_Dir N2 = gp::DY();
    N1.Transform(aTransform);
    N2.Transform(aTransform);
    
    if (Vec1.CrossMagnitude(N1) > Precision::Confusion())
    {
      // The plane is orthogonal to the angle presentation plane 
      // and contains the current edge
      aNormal = N1.Crossed(gp_Dir(Vec1));
    }
    else
      aNormal = N2;

    if (twoAngles)
    { 
      gp_Vec V = Vec1.Transformed(aTransform.Inverted());
      gp_Vec Vec3(V.X(),V.Y(),0.0);
      
      // Express the coordinates in the refernce coordinate system (OXY)
      Vec3.Transform(aTransform);   
      if(Abs(Vec1.CrossMagnitude(Vec3)) > Precision::Confusion())                                  
      { 
        //  set the normal as the cross product of the current edge with its projection
        //  it ensures that the dimension changes side when the angle becomes negative
        aNormal = gp_Dir(Vec1.Crossed(Vec3));  
      } 
    }         
  }
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
    // If no angles, the plane is the one formed by the last edge and the current one
    if(Abs(Vec1.CrossMagnitude(Vec2)) > Precision::Confusion()) {
      aNormal = gp_Dir(Vec1.Crossed(Vec2)); 
    }
  }
  return aNormal;
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

  // Display local trihedron if the mode is relative
  if (myMode == 1)
    displayTrihedron(2);

  // Display preview of suitable dimension presentations
  displayDimensions(false);

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
  
  SOCC_Viewer* anOCCViewer = dynamic_cast<SOCC_Viewer*>( vw->getViewManager()->getViewModel() );
  if( !anOCCViewer )
    return;
  
  Handle(AIS_InteractiveContext) anAISContext = anOCCViewer->getAISContext();
  if( !anAISContext )
    return;
  
  gp_Pnt P(getLastPoint().x,getLastPoint().y,getLastPoint().z);
  Handle(Geom_Axis2Placement) anAxis = new Geom_Axis2Placement(P,gp::DZ(),gp::DX());
  Handle(AIS_Trihedron) anIO = new AIS_Trihedron(anAxis);

  SOCC_Prs* aSPrs = dynamic_cast<SOCC_Prs*>( anOCCViewer->CreatePrs(0) );

  if (aSPrs) {
    aSPrs->PrependObject(anIO);
    GEOMBase_Helper::displayPreview(aSPrs, true, true);
  }
  anAISContext->Activate(anIO, selMode);
}

//================================================================
// Function : displayDimensions( bool store )
// Purpose  : Method for displaying dimensions. If store = true
//            the presentation is stored in a list
//================================================================
void EntityGUI_3DSketcherDlg::displayDimensions (bool store)
{
  myPrsType   = prsType();
  XYZ Last    = getLastPoint();
  XYZ Current = getCurrentPoint();

  gp_Pnt Last_Pnt(Last.x,Last.y,Last.z);
  
  gp_Pnt P0 = Last_Pnt;
  gp_Pnt Origin = gp::Origin();
  if (myMode == 0)                 // Absolute coordinates
    P0 = Origin;
  
  gp_Pnt Current_Pnt(Current.x,Current.y,Current.z);
  gp_Pnt P1, P2;
  
  // Check if last end current point are coincident
  if (Last_Pnt.IsEqual(Current_Pnt, 1e-7))
    return;
  
  gp_Dir aNormal = getPresentationPlane();
  
  if (myCoordType == 0)
  {
    bool oneDimensionalMove =  (isSame(Last_Pnt.X(), Current_Pnt.X()) && 
                                isSame(Last_Pnt.Y(), Current_Pnt.Y()) ) ||
                               (isSame(Last_Pnt.Y(), Current_Pnt.Y()) && 
                                isSame(Last_Pnt.Z(), Current_Pnt.Z()) ) ||
                               (isSame(Last_Pnt.X(), Current_Pnt.X()) && 
                                isSame(Last_Pnt.Z(), Current_Pnt.Z()) );
     
    if (myMode == 0)
    {
      std::string aCoordText = "( " + doubleToString(Current_Pnt.X()) + 
                               ", " + doubleToString(Current_Pnt.Y()) +
                               ", " + doubleToString(Current_Pnt.Z()) + " )";
      displayText(aCoordText, Current_Pnt, store);
    }
    else 
    { 
      if (oneDimensionalMove)
      {
        // For better colocation of dimensions if only one coordinate changes (aNormal is a better choice)
        displayLength(P0, Current_Pnt, aNormal, store);
      }
      else
      {
        displayLength(gp_Pnt(P0.X(),Current.y,P0.Z()), gp_Pnt(Current.x,Current.y,P0.Z()), gp::DZ().Reversed(), store);
        displayLength(gp_Pnt(Current.x,P0.Y(),P0.Z()), gp_Pnt(Current.x,Current.y,P0.Z()), gp::DZ(), store);
        displayLength(gp_Pnt(Current.x,Current.y,P0.Z()), Current_Pnt, gp::DX(), store);
      }
    }
  }
  else if (myCoordType == 1)             // ANGLES
  {
    bool spherical   = GroupAngles->checkBox->isChecked(); 
    bool cylindrical = GroupAngles->checkBox_2->isChecked();
    
    double anAngle1 = GroupAngles->SpinBox_DA->value();
    double aLength  = GroupAngles->SpinBox_DL->value();
    
    // Set the coordinates in the current coordinate system
    P1.SetCoord( aLength, 0.0, 0.0);    // X direction
    P2.SetCoord( aLength * cos(anAngle1 * M_PI / 180. ),
                 aLength * sin(anAngle1 * M_PI / 180. ),
                 0.0); 
    
    // Express the coordinates in the refernce coordinate system (OXY)
    gp_Trsf aTranform = toReferenceSystem(P0);
    P1.Transform(aTranform);    
    P2.Transform(aTranform);
    P1.Translate(Origin, P0);
    P2.Translate(Origin, P0);
    
    if(myMode !=0  || !store)
      displayAngle(anAngle1, P0, P1, P2, store);
    else
    {
      std::string anAngleText = doubleToString(anAngle1) + " deg.";
      displayText(anAngleText, Current_Pnt, store);
    }
    
    if(spherical)
    {
      double anAngle2 = GroupAngles->SpinBox_DA2->value();
      displayAngle(anAngle2, P0, P2, Current_Pnt, store);
      displayLength(P0, Current_Pnt, aNormal, store);
    }
    else
    {
      bool sameRadius = isSame ( radius(Last_Pnt), radius(Current_Pnt) );
      bool sameHeight = isSame ( height(Last_Pnt), height(Current_Pnt) );
                            
      gp_Vec aVec(P2, Current_Pnt);
      
      if (myMode == 0 && !sameRadius)
      {
        displayLength(P0.Translated(aVec), P2.Translated(aVec), aNormal, store);  // Radius  
      }
      else if (myMode == 1)
        displayLength(P0, P2, aNormal, store);
      
      if ( cylindrical && 
          (myMode == 1 || !sameHeight) )
        displayLength(P2, Current_Pnt, aNormal.Reversed(), store); // Height
    }
  }
}

//================================================================
// Function : displayAngle()
// Purpose  : Method for displaying angle dimensions
//================================================================
void EntityGUI_3DSketcherDlg::displayAngle (double theAngle,
                                            gp_Pnt P0, 
                                            gp_Pnt P1, 
                                            gp_Pnt P2,
                                            bool store)
{
  SUIT_ViewWindow* vw = SUIT_Session::session()->activeApplication()->desktop()->activeWindow();
  
  // Creation of the AIS object
  Handle(AIS_AngleDimension) anAngleIO = createAISAngleDimension(theAngle, 
                                                                 P0, 
                                                                 P1, 
                                                                 P2);
  if (anAngleIO.IsNull())
    return;
  
  if (store)
  {
    // Erase dimensions presentations
    ((SOCC_Viewer*)(vw->getViewManager()->getViewModel()))->Erase(myAnglePrs, true);
    myAnglePrs->PrependObject(anAngleIO);
    
    // Display modified presentation
    if (isAngleVisible)
      ((SOCC_Viewer*)(vw->getViewManager()->getViewModel()))->Display(myAnglePrs);
    
    // Update dimension presentation angle count for later undo / redo
    myPrsType.A += 1;
  }
  else if ( isAngleVisible)
  {
    SOCC_Prs* aSPrs = dynamic_cast<SOCC_Prs*>
        (((SOCC_Viewer*)(vw->getViewManager()->getViewModel()))->CreatePrs(0));
        
    if (aSPrs)
    {
      aSPrs->AddObject(anAngleIO);
      GEOMBase_Helper::displayPreview(aSPrs, true, true); 
    }
  }
}


//================================================================
// Function : displayLength()
// Purpose  : Method for displaying length dimensions for a segment
//            creation step
//================================================================
void EntityGUI_3DSketcherDlg::displayLength (gp_Pnt P1,
                                             gp_Pnt P2,
                                             gp_Dir theNormal,
                                             bool store)
{
  SUIT_ViewWindow* vw = SUIT_Session::session()->activeApplication()->desktop()->activeWindow();
  
  double aLength = P1.Distance(P2);
  
  if (aLength < Precision::Confusion())
    return;
    
  Handle(AIS_LengthDimension) anIO = createAISLengthDimension(aLength, P1, P2, theNormal);

  if (store)
  {
    // Erase length dimensions presentation
    ((SOCC_Viewer*)(vw->getViewManager()->getViewModel()))->Erase(myLengthPrs, true);
    myLengthPrs->PrependObject(anIO);

    // Display modified presentation
    if (isLengthVisible)
      ((SOCC_Viewer*)(vw->getViewManager()->getViewModel()))->Display(myLengthPrs);
    
    // Update dimension presentation length count for later undo / redo
    myPrsType.L += 1;
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
}

//================================================================
// Function : displayText()
// Purpose  : Method for displaying length dimensions for a segment
//            creation step
//================================================================
void EntityGUI_3DSketcherDlg::displayText ( std::string theText,
                                            gp_Pnt P,
                                            bool store )
{
  SUIT_ViewWindow* vw = SUIT_Session::session()->activeApplication()->desktop()->activeWindow();
    
  Handle(AIS_Text) anIO = new AIS_Text(TCollection_ExtendedString(theText.c_str()), P);

  if (store)
  {
    // Erase length dimensions presentation
    ((SOCC_Viewer*)(vw->getViewManager()->getViewModel()))->Erase(myTextPrs, true);
    myTextPrs->PrependObject(anIO);

    // Display modified presentation
    ((SOCC_Viewer*)(vw->getViewManager()->getViewModel()))->Display(myTextPrs);
    
    // Update dimension presentation text count for later undo / redo
    myPrsType.T += 1;
  }
  else
  {
    SOCC_Prs* aSPrs = dynamic_cast<SOCC_Prs*>
      (((SOCC_Viewer*)(vw->getViewManager()->getViewModel()))->CreatePrs(0));
    if (aSPrs)
    {
      aSPrs->PrependObject(anIO);
      GEOMBase_Helper::displayPreview(aSPrs, true, true);
    }
  }

}

//================================================================
// Function : createAISLengthDimension()
// Purpose  : Method for creation of a length dimension object
//            Returns an Handle on the AIS_LengthDimension obect
//================================================================
Handle(AIS_LengthDimension) EntityGUI_3DSketcherDlg::createAISLengthDimension(double theLength, 
                                                                              gp_Pnt P1, 
                                                                              gp_Pnt P2, 
                                                                              gp_Dir theNormal)
{
  // Plane construction
  gce_MakePln gce_MP(P1, theNormal);
  Handle(Geom_Plane) aPlane = new Geom_Plane(gce_MP.Value());
  
  TopoDS_Vertex aVert1 = BRepBuilderAPI_MakeVertex(P1);
  TopoDS_Vertex aVert2 = BRepBuilderAPI_MakeVertex(P2);

  Handle(AIS_LengthDimension) anIO = new AIS_LengthDimension( aVert1, aVert2, aPlane->Pln() );

  anIO->SetCustomValue( theLength );

  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  int w = resMgr->integerValue( "Geometry", "measures_line_width", 1 );

  Handle(Prs3d_DimensionAspect) aDimensionStyle = new Prs3d_DimensionAspect;

  aDimensionStyle->ArrowAspect()->SetLength( theLength / 20.0 );
  aDimensionStyle->LineAspect()->SetWidth( w );
  aDimensionStyle->MakeText3d( Standard_False );
  aDimensionStyle->SetTextHorizontalPosition( Prs3d_DTHP_Center );
  aDimensionStyle->SetTextVerticalPosition( Prs3d_DTVP_Center );
  aDimensionStyle->MakeArrows3d( Standard_True );
  anIO->SetFlyout( 0.0 );
  anIO->SetDimensionAspect( aDimensionStyle );

  return anIO;
}

//================================================================
// Function : createAISAngleDimension()
// Purpose  : Method for creation of an angle dimension object
//            Returns an Handle on the AIS_AngleDimension obect
//================================================================
Handle(AIS_AngleDimension) EntityGUI_3DSketcherDlg::createAISAngleDimension(double theAngle, 
                                                                            gp_Pnt P0, 
                                                                            gp_Pnt P1, 
                                                                            gp_Pnt P2)
{
  // Length of the built segment
  double aLength = P0.Distance(P1);
  
  // Check input data
  if (Abs(theAngle) < Precision::Angular() ||
      aLength < Precision::Confusion())
    return NULL;

  Handle(AIS_AngleDimension) anIO = new AIS_AngleDimension( P1, P0, P2 );

  //anIO->SetCustomValue( theAngle );

  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  int w = resMgr->integerValue( "Geometry", "measures_line_width", 1 );

  Handle(Prs3d_DimensionAspect) aDimensionStyle = new Prs3d_DimensionAspect;

  aDimensionStyle->ArrowAspect()->SetLength( (theAngle * M_PI / 180.0) * (aLength / 20.0) );
  aDimensionStyle->LineAspect()->SetWidth( w );
  aDimensionStyle->SetTextHorizontalPosition( Prs3d_DTHP_Center );
  aDimensionStyle->SetTextVerticalPosition( Prs3d_DTVP_Center );
  aDimensionStyle->MakeText3d( Standard_False );
  aDimensionStyle->MakeArrows3d( Standard_True );

  anIO->SetDimensionAspect( aDimensionStyle );

  return anIO;
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

//================================================================
// Function : toReferenceSystem ()
// Purpose  :
//================================================================
gp_Trsf EntityGUI_3DSketcherDlg::toReferenceSystem(gp_Pnt origin) const
{ 
  gp_Trsf T;                        // Identity transformation
  gp_Ax3 reference_system;          // OXY
  reference_system.SetLocation(origin);
  
  gp_Ax3 current_system = reference_system;
  switch (myOrientation)
  {
    case OYZ:
    {
      current_system = gp_Ax3(origin, gp::DX(), gp::DY());
      break;
    }
    case OXZ:
    {
      current_system = gp_Ax3(origin, gp::DY().Reversed(), gp::DX());
      break;
    }
  }
  
  T.SetTransformation( current_system, reference_system );
  
  return T;
}

//================================================================
// Function : toCurrentSystem ()
// Purpose  :
//================================================================
gp_Trsf EntityGUI_3DSketcherDlg::toCurrentSystem(gp_Pnt origin) const
{
  return toReferenceSystem(origin).Inverted();
}

//================================================================
// Function : radius (gp_Pnt) const
// Purpose  :
//================================================================
double EntityGUI_3DSketcherDlg::radius (gp_Pnt thePnt) const
{
  // Get the point coordinates in the current coordinates system
  gp_Trsf aTrsf = toCurrentSystem(gp::Origin());
  gp_Pnt aPnt = thePnt.Transformed(aTrsf);
  
  double radius = sqrt(aPnt.X()*aPnt.X() + aPnt.Y()*aPnt.Y());
  return radius;
}

//================================================================
// Function : height (gp_Pnt) const
// Purpose  :
//================================================================
double EntityGUI_3DSketcherDlg::height (gp_Pnt thePnt) const
{
  // Get the point coordinates in the current coordinates system
  gp_Trsf aTrsf = toCurrentSystem(gp::Origin());
  gp_Pnt aPnt = thePnt.Transformed(aTrsf);
  
  return aPnt.Z();
}
