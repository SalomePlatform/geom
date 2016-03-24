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
// File   : BasicGUI_CurveDlg.cxx
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.

#include "BasicGUI_CurveDlg.h"
#include "BasicGUI_ParamCurveWidget.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>
#include <LightApp_SelectionMgr.h>

#include <SALOME_ListIO.hxx>

#include <TopoDS_Shape.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#include <GEOMImpl_Types.hxx>

//=================================================================================
// class    : BasicGUI_CurveDlg()
// purpose  : Constructs a BasicGUI_CurveDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
BasicGUI_CurveDlg::BasicGUI_CurveDlg( GeometryGUI* theGeometryGUI, QWidget* parent,
                                      bool modal, Qt::WindowFlags fl )
  : GEOMBase_Skeleton( theGeometryGUI, parent, modal, fl )
{
  QPixmap image0 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_POLYLINE")));
  QPixmap image2 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_SPLINE")));
  QPixmap image3 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_BEZIER")));
  QPixmap image1 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_SELECT")));

  setWindowTitle(tr("GEOM_CURVE_TITLE"));

  /***************************************************************/
  mainFrame()->RadioButton1->setIcon(image0);
  mainFrame()->RadioButton2->setIcon(image3);
  mainFrame()->RadioButton3->setIcon(image2);

  // Creation mode
  QGroupBox* creationModeGroup = new QGroupBox (this);
  QButtonGroup* bg = new QButtonGroup (this);

  creationModeGroup->setTitle(tr("GEOM_CURVE_CRMODE"));
  QHBoxLayout * creationModeLayout = new QHBoxLayout (creationModeGroup);
  myBySelectionBtn = new QRadioButton (tr("GEOM_CURVE_SELECTION") ,creationModeGroup);
  myAnaliticalBtn = new QRadioButton (tr("GEOM_CURVE_ANALITICAL") ,creationModeGroup);

  bg->addButton(myBySelectionBtn);
  bg->addButton(myAnaliticalBtn);

  creationModeLayout->addWidget(myBySelectionBtn);
  creationModeLayout->addWidget(myAnaliticalBtn);

  // Points and flags
  myGroupPoints = new DlgRef_1Sel3Check (centralWidget());

  myGroupPoints->GroupBox1->setTitle(tr("GEOM_NODES"));
  myGroupPoints->TextLabel1->setText(tr("GEOM_POINTS"));
  myGroupPoints->PushButton1->setIcon(image1);
  myGroupPoints->PushButton1->setDown(true);

  myGroupPoints->LineEdit1->setReadOnly( true );

  myGroupPoints->CheckButton1->setText( tr( "GEOM_IS_CLOSED" ) );
  myGroupPoints->CheckButton1->setChecked(false);
  //myGroupPoints->CheckButton1->hide();

  myGroupPoints->CheckButton2->setText( tr( "GEOM_IS_REORDER" ) );
  myGroupPoints->CheckButton2->setChecked(false);
  myGroupPoints->CheckButton2->hide();

  myGroupPoints->CheckButton3->hide();

  // Parametrical mode
  myGroupParams = new BasicGUI_ParamCurveWidget( centralWidget() );

  // Tangents (only for Interpolation constructor and only not closed and no reordering)
  myGroupTangents = new QGroupBox (this);
  myGroupTangents->setCheckable(true);
  myGroupTangents->setChecked(false);
  myGroupTangents->setTitle(tr("GEOM_INTERPOL_TANGENTS"));

  QGridLayout* tangentsLayout = new QGridLayout (myGroupTangents);

  myPushBtnV1 = new QPushButton (myGroupTangents);
  myPushBtnV2 = new QPushButton (myGroupTangents);
  myPushBtnV1->setIcon(image1);
  myPushBtnV2->setIcon(image1);

  myLineEditV1 = new QLineEdit (myGroupTangents);
  myLineEditV2 = new QLineEdit (myGroupTangents);
  myLineEditV1->setReadOnly(true);
  myLineEditV2->setReadOnly(true);

  QLabel* aTextLabelV1 = new QLabel (myGroupTangents);
  QLabel* aTextLabelV2 = new QLabel (myGroupTangents);
  aTextLabelV1->setText(tr("GEOM_INTERPOL_FIRST_VEC"));
  aTextLabelV2->setText(tr("GEOM_INTERPOL_LAST_VEC"));

  tangentsLayout->addWidget(aTextLabelV1, 0, 0);
  tangentsLayout->addWidget(myPushBtnV1 , 0, 1);
  tangentsLayout->addWidget(myLineEditV1, 0, 2);
  tangentsLayout->addWidget(aTextLabelV2, 1, 0);
  tangentsLayout->addWidget(myPushBtnV2 , 1, 1);
  tangentsLayout->addWidget(myLineEditV2, 1, 2);

  // Layout
  QVBoxLayout* layout = new QVBoxLayout (centralWidget());
  layout->setMargin(0);
  layout->setSpacing(6);
  layout->addWidget(creationModeGroup);
  layout->addWidget(myGroupPoints);
  layout->addWidget(myGroupParams);
  layout->addWidget(myGroupTangents);
  /***************************************************************/

  setHelpFileName("create_curve_page.html");

  Init();
}

//=================================================================================
// function : ~BasicGUI_CurveDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
BasicGUI_CurveDlg::~BasicGUI_CurveDlg()
{
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BasicGUI_CurveDlg::Init()
{
  /* init variables */
  showOnlyPreviewControl();
  myBySelectionBtn->setChecked(true);

  /* Get setting of step value from file configuration */
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  double step = resMgr ? resMgr->doubleValue( "Geometry", "SettingsGeomStep", 10. ) : 10.;

  double aMax( 100. ), aMin( 0.0 );

  /* min, max, step and decimals for spin boxes & initial values */
  initSpinBox( myGroupParams->myPMin, COORD_MIN, COORD_MAX, step, "length_precision" );
  initSpinBox( myGroupParams->myPMax, COORD_MIN, COORD_MAX, step, "length_precision" );
  myGroupParams->myPStep->setValue( 10 );
  myGroupParams->myPStep->setMaximum( 999 );
  myGroupParams->myPStep->setSingleStep( 10 );
  myGroupParams->myPMin->setValue( aMin );
  myGroupParams->myPMax->setValue( aMax );
  myGroupParams->myPStep->setValue( step );
  myGroupParams->myXExpr->setText("t");
  myGroupParams->myYExpr->setText("t");
  myGroupParams->myZExpr->setText("t");

  myGroupParams->hide();

  /* signals and slots connections */
  connect(myGeomGUI,     SIGNAL(SignalDeactivateActiveDialog()), this, SLOT(DeactivateActiveDialog()));
  connect(myGeomGUI,     SIGNAL(SignalCloseAllDialogs()),        this, SLOT(ClickOnCancel()));

  connect(buttonOk(),    SIGNAL(clicked()),                      this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()),                      this, SLOT(ClickOnApply()));

  connect(this,          SIGNAL(constructorsClicked(int)),       this, SLOT(ConstructorsClicked(int)));

  connect(myGroupPoints->PushButton1,  SIGNAL(clicked()),        this, SLOT(SetEditCurrentArgument()));
  connect(myPushBtnV1,                 SIGNAL(clicked()),        this, SLOT(SetEditCurrentArgument()));
  connect(myPushBtnV2,                 SIGNAL(clicked()),        this, SLOT(SetEditCurrentArgument()));

  connect(myGroupPoints->CheckButton1, SIGNAL(toggled(bool)),    this, SLOT(CheckButtonToggled()));
  connect(myGroupPoints->CheckButton2, SIGNAL(toggled(bool)),    this, SLOT(CheckButtonToggled()));
  connect(myGroupTangents,             SIGNAL(toggled(bool)),    this, SLOT(CheckButtonToggled()));

  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));

  connect(myBySelectionBtn,       SIGNAL(clicked()),             this, SLOT(CreationModeChanged()));
  connect(myAnaliticalBtn,        SIGNAL(clicked()),             this, SLOT(CreationModeChanged()));

  connect(myGroupParams->myPMin,  SIGNAL(valueChanged(double)),  this, SLOT(ValueChangedInSpinBox(double)));
  connect(myGroupParams->myPMax,  SIGNAL(valueChanged(double)),  this, SLOT(ValueChangedInSpinBox(double)));
  connect(myGroupParams->myPStep, SIGNAL(valueChanged(int)),     this, SLOT(ValueChangedInSpinBox(int)));

  connect(myGroupParams->myXExpr, SIGNAL(editingFinished()),     this, SLOT(OnEditingFinished()));
  connect(myGroupParams->myYExpr, SIGNAL(editingFinished()),     this, SLOT(OnEditingFinished()));
  connect(myGroupParams->myZExpr, SIGNAL(editingFinished()),     this, SLOT(OnEditingFinished()));

  initName(tr("GEOM_CURVE"));
  //resize(100, 100);
  ConstructorsClicked(0);
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  :
//=================================================================================
void BasicGUI_CurveDlg::ConstructorsClicked (int id)
{
  QString aTitle = tr(id == 0 ? "GEOM_POLYLINE" : id == 1 ? "GEOM_BEZIER" : "GEOM_INTERPOL");
  mainFrame()->GroupConstructors->setTitle(aTitle);

  if (id == 0) { // polyline (wire)
    myGroupPoints->CheckButton1->setText( tr( "GEOM_BUILD_CLOSED_WIRE" ) );
    myGroupPoints->CheckButton2->hide();
    myGroupTangents->hide();
  }
  else if (id == 1) { // bezier
    myGroupPoints->CheckButton1->setText( tr( "GEOM_IS_CLOSED" ) );
    myGroupPoints->CheckButton2->hide();
    myGroupTangents->hide();
  }
  else { // b-spline
    myGroupPoints->CheckButton1->setText( tr( "GEOM_IS_CLOSED" ) );
    myGroupPoints->CheckButton2->show();
    myGroupTangents->setVisible(myBySelectionBtn->isChecked());
  }

  myPoints.clear();
  myVec1.nullify();
  myVec2.nullify();

  myGroupPoints->LineEdit1->setText("");
  myLineEditV1->setText("");
  myLineEditV2->setText("");

  qApp->processEvents();
  updateGeometry();
  resize(minimumSizeHint());

  myGroupPoints->PushButton1->click();
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BasicGUI_CurveDlg::SetEditCurrentArgument()
{
  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);

  globalSelection(); // close local contexts, if any

  if (sender() == myGroupPoints->PushButton1) {
    myEditCurrentArgument = myGroupPoints->LineEdit1;
    localSelection(TopAbs_VERTEX);
  }
  else if (sender() == myPushBtnV1) {
    myEditCurrentArgument = myLineEditV1;
    localSelection(TopAbs_EDGE);
  }
  else if (sender() == myPushBtnV2) {
    myEditCurrentArgument = myLineEditV2;
    localSelection(TopAbs_EDGE);
  }

  myEditCurrentArgument->setFocus();

  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));

  SelectionIntoArgument();
}

//=================================================================================
// function : CheckButtonToggled()
// purpose  :
//=================================================================================
void BasicGUI_CurveDlg::CheckButtonToggled()
{
  if (sender() == myGroupTangents) {
    if (myGroupTangents->isChecked())
      myPushBtnV1->click();
    else
      myGroupPoints->PushButton1->click();
  }
  else {
    if (getConstructorId() == 2) { // Interpolation
      bool disableTangents = (myGroupPoints->CheckButton1->isChecked() ||
                              myGroupPoints->CheckButton2->isChecked());
      myGroupTangents->setEnabled(!disableTangents);
    }
    processPreview();
  }
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void BasicGUI_CurveDlg::ClickOnOk()
{
  setIsApplyAndClose(true);
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool BasicGUI_CurveDlg::ClickOnApply()
{
  if (!onAccept())
    return false;

  initName();
  ConstructorsClicked(getConstructorId());
  return true;
}

void BasicGUI_CurveDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("");

  if (myEditCurrentArgument == myGroupPoints->LineEdit1) {
    QList<GEOM::GeomObjPtr> points = getSelected(TopAbs_VERTEX, -1);
    GEOMBase::Synchronize(myPoints, points);
    if (!myPoints.isEmpty())
      myGroupPoints->LineEdit1->setText(QString::number(myPoints.count()) + "_" +
                                        tr("GEOM_POINT") + tr("_S_"));
  }
  else {
    QList<GEOM::GeomObjPtr> vecs = getSelected(TopAbs_EDGE, -1);
    if (vecs.count() != 1) {
      if      (myEditCurrentArgument == myLineEditV1) myVec1.nullify();
      else if (myEditCurrentArgument == myLineEditV2) myVec2.nullify();
    }
    else {
      if (myEditCurrentArgument == myLineEditV1) {
        myVec1 = vecs.first();
      }
      else if (myEditCurrentArgument == myLineEditV2) {
        myVec2 = vecs.first();
      }
      QString aName = GEOMBase::GetName(vecs.first().get());
      myEditCurrentArgument->setText(aName);
    }
  }

  processPreview();
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void BasicGUI_CurveDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();

  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
           this, SLOT(SelectionIntoArgument()));

  ConstructorsClicked(getConstructorId());
}

//=================================================================================
// function : DeactivateActiveDialog()
// purpose  : public slot to deactivate if active
//=================================================================================
void BasicGUI_CurveDlg::DeactivateActiveDialog()
{
  // myGeomGUI->SetState( -1 );
  GEOMBase_Skeleton::DeactivateActiveDialog();
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void BasicGUI_CurveDlg::enterEvent (QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr BasicGUI_CurveDlg::createOperation()
{
  return myGeomGUI->GetGeomGen()->GetICurvesOperations(getStudyId());
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool BasicGUI_CurveDlg::isValid (QString& msg)
{
  if (myBySelectionBtn->isChecked()) {
    bool ok = true;
    if (getConstructorId() == 2) { // Interpolation
      bool disableTangents = (myGroupPoints->CheckButton1->isChecked() ||
                              myGroupPoints->CheckButton2->isChecked());
      if (!disableTangents && myGroupTangents->isChecked()) {
        ok = (myVec1 && myVec2);
        if (!ok) msg = tr("GEOM_BOTH_TANGENTS_REQUIRED");
      }
    }
    return ok && myPoints.count() > 1;
  }
  else {
    bool ok = myGroupParams->myPMin->isValid( msg, !IsPreview() ) &&
              myGroupParams->myPMax->isValid( msg, !IsPreview() ) &&
              myGroupParams->myPStep->isValid( msg, !IsPreview() );
    ok &= !myGroupParams->myXExpr->text().isEmpty();
    ok &= !myGroupParams->myYExpr->text().isEmpty();
    ok &= !myGroupParams->myZExpr->text().isEmpty();
    return ok;
  }
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool BasicGUI_CurveDlg::execute (ObjectList& objects)
{
  bool res = false;

  GEOM::GEOM_Object_var anObj;

  GEOM::GEOM_ICurvesOperations_var anOper = GEOM::GEOM_ICurvesOperations::_narrow(getOperation());

  GEOM::ListOfGO_var points = new GEOM::ListOfGO();
  points->length(myPoints.count());
  for (int i = 0; i < myPoints.count(); i++)
    points[i] = myPoints[i].copy();

  switch (getConstructorId()) {
  case 0 :
    if (myBySelectionBtn->isChecked())
      anObj = anOper->MakePolyline(points.in(), myGroupPoints->CheckButton1->isChecked());
    else
      anObj = anOper->MakeCurveParametricNew(qPrintable(myGroupParams->myXExpr->text()),
                                             qPrintable(myGroupParams->myYExpr->text()),
                                             qPrintable(myGroupParams->myZExpr->text()),
                                             myGroupParams->myPMin->value(),
                                             myGroupParams->myPMax->value(),
                                             myGroupParams->myPStep->value(),
                                             GEOM::Polyline);
    res = true;
    break;
  case 1 :
    if (myBySelectionBtn->isChecked())
      anObj = anOper->MakeSplineBezier(points.in(), myGroupPoints->CheckButton1->isChecked());
    else
      anObj = anOper->MakeCurveParametricNew(qPrintable(myGroupParams->myXExpr->text()),
                                             qPrintable(myGroupParams->myYExpr->text()),
                                             qPrintable(myGroupParams->myZExpr->text()),
                                             myGroupParams->myPMin->value(),
                                             myGroupParams->myPMax->value(),
                                             myGroupParams->myPStep->value(),
                                             GEOM::Bezier);
    res = true;
    break;
  case 2 :
    if (myBySelectionBtn->isChecked()) {
      bool disableTangents = (myGroupPoints->CheckButton1->isChecked() ||
                              myGroupPoints->CheckButton2->isChecked());
      if (!disableTangents && myGroupTangents->isChecked()) {
        anObj = anOper->MakeSplineInterpolWithTangents(points.in(), myVec1.get(), myVec2.get());
      }
      else
        anObj = anOper->MakeSplineInterpolation(points.in(), myGroupPoints->CheckButton1->isChecked(),
                                                myGroupPoints->CheckButton2->isChecked());
    }
    else
      anObj = anOper->MakeCurveParametricNew(qPrintable(myGroupParams->myXExpr->text()),
                                             qPrintable(myGroupParams->myYExpr->text()),
                                             qPrintable(myGroupParams->myZExpr->text()),
                                             myGroupParams->myPMin->value(),
                                             myGroupParams->myPMax->value(),
                                             myGroupParams->myPStep->value(),
                                             GEOM::Interpolation);
    res = true;
    break;
  }

  if (!anObj->_is_nil()) {
    if (myAnaliticalBtn->isChecked() && !IsPreview()) {
      QStringList aParameters;
      aParameters<<myGroupParams->myPMin->text();
      aParameters<<myGroupParams->myPMax->text();
      aParameters<<myGroupParams->myPStep->text();
      anObj->SetParameters(aParameters.join(":").toLatin1().constData());
    }
    objects.push_back(anObj._retn());
  }

  return res;
}

//=================================================================================
// function : addSubshapesToStudy
// purpose  : virtual method to add new SubObjects if local selection
//=================================================================================
void BasicGUI_CurveDlg::addSubshapesToStudy()
{
  for ( int i = 0; i < myPoints.count(); i++ )
    GEOMBase::PublishSubObject( myPoints[i].get() );
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> BasicGUI_CurveDlg::getSourceObjects()
{
  return myPoints;
}
//=================================================================================
// function : CreationModeChanged
// purpose  :
//=================================================================================
void BasicGUI_CurveDlg::CreationModeChanged()
{
  const QObject* s = sender();
  myGroupPoints->setVisible(myBySelectionBtn == s);
  myGroupParams->setVisible(myBySelectionBtn != s);

  ConstructorsClicked(getConstructorId());
}

//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void BasicGUI_CurveDlg::ValueChangedInSpinBox (double/*theValue*/)
{
  processPreview();
}

//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void BasicGUI_CurveDlg::ValueChangedInSpinBox (int/*theValue*/)
{
  processPreview();
}

//=================================================================================
// function : OnEditingFinished()
// purpose  :
//=================================================================================
void BasicGUI_CurveDlg::OnEditingFinished()
{
  processPreview();
}
