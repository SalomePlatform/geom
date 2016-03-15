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
//  File   : TransformationGUI_MultiTranslationDlg.cxx
//  Author : Damien COQUERET, Open CASCADE S.A.S.

#include "TransformationGUI_MultiTranslationDlg.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Desktop.h>
#include <SUIT_ViewWindow.h>
#include <SUIT_ViewManager.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>
#include <GEOM_AISVector.hxx>
#include <SOCC_Prs.h>
#include <SOCC_ViewModel.h>

// OCCT Includes
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRep_Builder.hxx>

#include <GEOMImpl_Types.hxx>

//=================================================================================
// class    : TransformationGUI_MultiTranslationDlg()
// purpose  : Constructs a TransformationGUI_MultiTranslationDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
TransformationGUI_MultiTranslationDlg::TransformationGUI_MultiTranslationDlg
(GeometryGUI* theGeometryGUI, QWidget* parent, bool modal, Qt::WindowFlags fl)
  : GEOMBase_Skeleton(theGeometryGUI, parent, modal, fl),
    myInitial(true)
{
  SUIT_ResourceMgr* aResMgr = myGeomGUI->getApp()->resourceMgr();
  QPixmap image0 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_MULTITRANSLATION_SIMPLE")));
  QPixmap image1 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_MULTITRANSLATION_DOUBLE")));
  QPixmap image2 (aResMgr->loadPixmap("GEOM", tr("ICON_SELECT")));

  setWindowTitle(tr("GEOM_MULTITRANSLATION_TITLE"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_MULTITRANSLATION"));
  mainFrame()->RadioButton1->setIcon(image0);
  mainFrame()->RadioButton2->setIcon(image1);
  mainFrame()->RadioButton3->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton3->close();

  GroupPoints = new DlgRef_2Sel2Spin1Check(centralWidget());
  GroupPoints->GroupBox1->setTitle(tr("GEOM_MULTITRANSLATION_SIMPLE"));
  GroupPoints->TextLabel1->setText(tr("GEOM_MAIN_OBJECT"));
  GroupPoints->TextLabel2->setText(tr("GEOM_VECTOR"));
  GroupPoints->TextLabel3->setText(tr("GEOM_STEP"));
  GroupPoints->TextLabel4->setText(tr("GEOM_NB_TIMES"));
  GroupPoints->CheckButton1->setText(tr("GEOM_REVERSE_DIRECTION"));
  GroupPoints->PushButton1->setIcon(image2);
  GroupPoints->PushButton2->setIcon(image2);
  GroupPoints->LineEdit1->setReadOnly(true);
  GroupPoints->LineEdit2->setReadOnly(true);

  GroupDimensions = new DlgRef_3Sel4Spin2Check(centralWidget());
  GroupDimensions->GroupBox1->setTitle(tr("GEOM_MULTITRANSLATION_DOUBLE"));
  GroupDimensions->TextLabel1->setText(tr("GEOM_MAIN_OBJECT"));
  GroupDimensions->TextLabel2->setText(tr("GEOM_VECTOR_U"));
  GroupDimensions->TextLabel3->setText(tr("GEOM_VECTOR_V"));
  GroupDimensions->TextLabel4->setText(tr("GEOM_STEP_U"));
  GroupDimensions->TextLabel5->setText(tr("GEOM_NB_TIMES_U"));
  GroupDimensions->TextLabel6->setText(tr("GEOM_STEP_V"));
  GroupDimensions->TextLabel7->setText(tr("GEOM_NB_TIMES_V"));
  GroupDimensions->CheckButton1->setText(tr("GEOM_REVERSE_U"));
  GroupDimensions->CheckButton2->setText(tr("GEOM_REVERSE_V"));
  GroupDimensions->PushButton1->setIcon(image2);
  GroupDimensions->PushButton2->setIcon(image2);
  GroupDimensions->PushButton3->setIcon(image2);
  GroupDimensions->LineEdit1->setReadOnly(true);
  GroupDimensions->LineEdit2->setReadOnly(true);
  GroupDimensions->LineEdit3->setReadOnly(true);

  QVBoxLayout* layout = new QVBoxLayout(centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
  layout->addWidget(GroupPoints);
  layout->addWidget(GroupDimensions);
  /***************************************************************/

  setHelpFileName("multi_translation_operation_page.html");

  Init();
}

//=================================================================================
// function : ~TransformationGUI_MultiTranslationDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
TransformationGUI_MultiTranslationDlg::~TransformationGUI_MultiTranslationDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void TransformationGUI_MultiTranslationDlg::Init()
{
  // Get setting of step value from file configuration
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  double step = resMgr->doubleValue("Geometry", "SettingsGeomStep", 100);

  int SpecificStep = 1;
  // init variables
  myStepU = myStepV = 50.0;
  myNbTimesU = myNbTimesV = 3;

  // min, max, step and decimals for spin boxes & initial values
  initSpinBox(GroupPoints->SpinBox_DX, COORD_MIN, COORD_MAX, step, "length_precision" );
  initSpinBox(GroupPoints->SpinBox_DY, 1, 999, SpecificStep);
  GroupPoints->SpinBox_DX->setValue(myStepU);
  GroupPoints->SpinBox_DY->setValue(myNbTimesU);

  initSpinBox(GroupDimensions->SpinBox_DX1, COORD_MIN, COORD_MAX, step, "length_precision" );
  initSpinBox(GroupDimensions->SpinBox_DY1, 1, 999, SpecificStep);
  initSpinBox(GroupDimensions->SpinBox_DX2, COORD_MIN, COORD_MAX, step, "length_precision" );
  initSpinBox(GroupDimensions->SpinBox_DY2, 1, 999, SpecificStep);
  GroupDimensions->SpinBox_DX1->setValue(myStepU);
  GroupDimensions->SpinBox_DY1->setValue(myNbTimesU);
  GroupDimensions->SpinBox_DX2->setValue(myStepV);
  GroupDimensions->SpinBox_DY2->setValue(myNbTimesV);

  GroupPoints->LineEdit1->setText("");
  GroupPoints->LineEdit2->setText("");

  GroupDimensions->LineEdit1->setText("");
  GroupDimensions->LineEdit2->setText("");
  GroupDimensions->LineEdit3->setText("");

  myBase.nullify();
  myVectorU.nullify();
  myVectorV.nullify();

  mainFrame()->GroupBoxPublish->show();

  // signals and slots connections
  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(this, SIGNAL(constructorsClicked(int)), this, SLOT(ConstructorsClicked(int)));

  connect(GroupPoints->PushButton1,     SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->PushButton2,     SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupDimensions->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupDimensions->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupDimensions->PushButton3, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(GroupPoints->SpinBox_DX,      SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupPoints->SpinBox_DY,      SIGNAL(valueChanged(int)),    this, SLOT(ValueChangedInSpinBox(int)));
  connect(GroupDimensions->SpinBox_DX1, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupDimensions->SpinBox_DY1, SIGNAL(valueChanged(int)),    this, SLOT(ValueChangedInSpinBox(int)));
  connect(GroupDimensions->SpinBox_DX2, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupDimensions->SpinBox_DY2, SIGNAL(valueChanged(int)),    this, SLOT(ValueChangedInSpinBox(int)));

  connect(GroupPoints->SpinBox_DX,      SIGNAL(textChanged(const QString& )),
          this, SLOT(TextValueChangedInSpinBox(const QString& )));
  connect(GroupDimensions->SpinBox_DX1, SIGNAL(textChanged(const QString& )),
          this, SLOT(TextValueChangedInSpinBox(const QString& )));
  connect(GroupDimensions->SpinBox_DX2, SIGNAL(textChanged(const QString& )),
          this, SLOT(TextValueChangedInSpinBox(const QString& )));

  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), this, SLOT(SetDoubleSpinBoxStep(double)));

  connect(GroupPoints->CheckButton1,     SIGNAL(toggled(bool)), this, SLOT(ReverseStepU()));
  connect(GroupDimensions->CheckButton1, SIGNAL(toggled(bool)), this, SLOT(ReverseStepU()));
  connect(GroupDimensions->CheckButton2, SIGNAL(toggled(bool)), this, SLOT(ReverseStepV()));

  initName(tr("GEOM_MULTITRANSLATION"));

  ConstructorsClicked(0);
}

//=================================================================================
// function : SetDoubleSpinBoxStep()
// purpose  : Double spin box management
//=================================================================================
void TransformationGUI_MultiTranslationDlg::SetDoubleSpinBoxStep (double step)
{
  GroupPoints->SpinBox_DX->setSingleStep(step);
  GroupDimensions->SpinBox_DX1->setSingleStep(step);
  GroupDimensions->SpinBox_DX2->setSingleStep(step);

  // san : Commented so as not to override specific step settings
  //GroupPoints->SpinBox_DY->setSingleStep((int)step);
  //GroupDimensions->SpinBox_DY1->setSingleStep((int)step);
  //GroupDimensions->SpinBox_DY2->setSingleStep((int)step);
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void TransformationGUI_MultiTranslationDlg::ConstructorsClicked (int constructorId)
{
  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);

  switch (constructorId) {
  case 0: // Translate simple
    {
      GroupDimensions->hide();
      GroupPoints->show();

      //GroupPoints->LineEdit1->setText("");
      //GroupPoints->LineEdit2->setText("");
      //myBase.nullify();
      //myVectorU.nullify();

      if ( myBase ) GroupDimensions->LineEdit1->setText( GEOMBase::GetName( myBase.get() ) );
      if ( myVectorU ) GroupDimensions->LineEdit2->setText( GEOMBase::GetName( myVectorU.get() ) );

      GroupPoints->SpinBox_DX->setValue(myStepU);
      GroupPoints->SpinBox_DY->setValue(myNbTimesU);

      if ( !myBase )
	GroupPoints->PushButton1->click();
      else if ( !myVectorU )
	GroupPoints->PushButton2->click();
      break;
    }
  case 1: // Translate double
    {
      GroupPoints->hide();
      GroupDimensions->show();

      //GroupDimensions->LineEdit1->setText("");
      //GroupDimensions->LineEdit2->setText("");
      //GroupDimensions->LineEdit3->setText("");
      //myBase.nullify();
      //myVectorU.nullify();
      //myVectorV.nullify();

      if ( myBase ) GroupDimensions->LineEdit1->setText( GEOMBase::GetName( myBase.get() ) );
      if ( myVectorU ) GroupDimensions->LineEdit2->setText( GEOMBase::GetName( myVectorU.get() ) );
      if ( myVectorV ) GroupDimensions->LineEdit3->setText( GEOMBase::GetName( myVectorV.get() ) );

      GroupDimensions->SpinBox_DX1->setValue(myStepU);
      GroupDimensions->SpinBox_DY1->setValue(myNbTimesU);
      GroupDimensions->SpinBox_DX2->setValue(myStepV);
      GroupDimensions->SpinBox_DY2->setValue(myNbTimesV);

      if ( !myBase )
	GroupDimensions->PushButton1->click();
      else if ( !myVectorU )
	GroupDimensions->PushButton2->click();
      else if ( !myVectorV )
	GroupDimensions->PushButton3->click();
      break;
    }
  }

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
void TransformationGUI_MultiTranslationDlg::ClickOnOk()
{
  setIsApplyAndClose( true );
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool TransformationGUI_MultiTranslationDlg::ClickOnApply()
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
void TransformationGUI_MultiTranslationDlg::SelectionIntoArgument()
{
  erasePreview();

  TopAbs_ShapeEnum aNeedType = ( myEditCurrentArgument == GroupPoints->LineEdit2 ||
                                 myEditCurrentArgument == GroupDimensions->LineEdit2 ||
                                 myEditCurrentArgument == GroupDimensions->LineEdit3 ) ?
    TopAbs_EDGE : TopAbs_SHAPE;
  GEOM::GeomObjPtr aSelectedObject = getSelected( aNeedType );
  TopoDS_Shape aShape;
  if ( aSelectedObject && GEOMBase::GetShape( aSelectedObject.get(), aShape ) && !aShape.IsNull() ) {
    QString aName = GEOMBase::GetName( aSelectedObject.get() );
    myEditCurrentArgument->setText( aName );

    if ( myEditCurrentArgument == GroupPoints->LineEdit1 || myEditCurrentArgument == GroupDimensions->LineEdit1 ) {
      myBase = aSelectedObject;

      // recompute myStepU (Mantis issue 0021718)
      GEOM::GEOM_IMeasureOperations_var anOper = getGeomEngine()->GetIMeasureOperations(getStudyId());
      double Xmin, Xmax, Ymin, Ymax, Zmin, Zmax;
      anOper->GetBoundingBox(myBase.get(), true, Xmin, Xmax, Ymin, Ymax, Zmin, Zmax);
      if (anOper->IsDone()) {
        myStepU = floor(1.5 * (Xmax - Xmin));
        myStepV = floor(1.5 * (Ymax - Ymin));
        GroupPoints->SpinBox_DX->setValue(myStepU);
        GroupDimensions->SpinBox_DX1->setValue(myStepU);
        GroupDimensions->SpinBox_DX2->setValue(myStepV);
      }

      if ( myEditCurrentArgument == GroupPoints->LineEdit1 && !myVectorU )
        GroupPoints->PushButton2->click();
      if ( myEditCurrentArgument == GroupDimensions->LineEdit1 ) {
	if ( !myVectorU )
	  GroupDimensions->PushButton2->click();
	else if ( !myVectorV )
	  GroupDimensions->PushButton3->click();
      }
    }
    else if ( myEditCurrentArgument == GroupPoints->LineEdit2 ) {
      myVectorU = aSelectedObject;
      if ( !myBase )
        GroupPoints->PushButton1->click();
    }
    else if ( myEditCurrentArgument == GroupDimensions->LineEdit2 ) {
      myVectorU = aSelectedObject;
      if ( !myVectorV )
        GroupDimensions->PushButton3->click();
      else if ( !myBase )
        GroupDimensions->PushButton1->click();
    }
    else if ( myEditCurrentArgument == GroupDimensions->LineEdit3 ) {
      myVectorV = aSelectedObject;
      if ( !myBase )
        GroupDimensions->PushButton1->click();
      else if ( !myVectorU )
        GroupDimensions->PushButton2->click();
    }

    // clear selection
    // disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
    // myGeomGUI->getApp()->selectionMgr()->clearSelected();
    // connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
    //         this, SLOT(SelectionIntoArgument()));
  }
  else {
    if ( myEditCurrentArgument == GroupPoints->LineEdit1 ||
         myEditCurrentArgument == GroupDimensions->LineEdit1 )
      myBase.nullify();
    else if ( myEditCurrentArgument == GroupPoints->LineEdit2 ||
              myEditCurrentArgument == GroupDimensions->LineEdit2 )
      myVectorU.nullify();
    else if ( myEditCurrentArgument == GroupDimensions->LineEdit3 )
      myVectorV.nullify();
    myEditCurrentArgument->setText("");
  }

  processPreview();
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void TransformationGUI_MultiTranslationDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
  globalSelection(GEOM_ALLSHAPES);

  if (send == GroupPoints->PushButton1) {
    myEditCurrentArgument = GroupPoints->LineEdit1;

    GroupPoints->PushButton2->setDown(false);
    GroupPoints->LineEdit2->setEnabled(false);
  }
  else if (send == GroupPoints->PushButton2) {
    myEditCurrentArgument = GroupPoints->LineEdit2;

    localSelection(TopAbs_EDGE);

    GroupPoints->PushButton1->setDown(false);
    GroupPoints->LineEdit1->setEnabled(false);
  }
  else if (send == GroupDimensions->PushButton1) {
    myEditCurrentArgument = GroupDimensions->LineEdit1;

    GroupDimensions->PushButton2->setDown(false);
    GroupDimensions->PushButton3->setDown(false);
    GroupDimensions->LineEdit2->setEnabled(false);
    GroupDimensions->LineEdit3->setEnabled(false);
  }
  else if (send == GroupDimensions->PushButton2) {
    myEditCurrentArgument = GroupDimensions->LineEdit2;

    localSelection(TopAbs_EDGE);

    GroupDimensions->PushButton1->setDown(false);
    GroupDimensions->PushButton3->setDown(false);
    GroupDimensions->LineEdit1->setEnabled(false);
    GroupDimensions->LineEdit3->setEnabled(false);
  }
  else if (send == GroupDimensions->PushButton3) {
    myEditCurrentArgument = GroupDimensions->LineEdit3;

    localSelection(TopAbs_EDGE);

    GroupDimensions->PushButton1->setDown(false);
    GroupDimensions->PushButton2->setDown(false);
    GroupDimensions->LineEdit1->setEnabled(false);
    GroupDimensions->LineEdit2->setEnabled(false);
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
void TransformationGUI_MultiTranslationDlg::ActivateThisDialog()
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
void TransformationGUI_MultiTranslationDlg::enterEvent (QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : TextValueChangedInSpinBox
// purpose  :
//=================================================================================
void TransformationGUI_MultiTranslationDlg::TextValueChangedInSpinBox( const QString& s)
{
  QObject* send = (QObject*)sender();
  bool isDigit = true;

  switch (getConstructorId()) {
  case 0:
    GroupPoints->SpinBox_DX->text().toDouble(&isDigit);
    if(!isDigit){
      GroupPoints->CheckButton1->setChecked(false);
    }
    GroupPoints->CheckButton1->setEnabled(isDigit);
    break;
  case 1:
    if (send == GroupDimensions->SpinBox_DX1) {
      GroupDimensions->SpinBox_DX1->text().toDouble(&isDigit);
      if(!isDigit)
        GroupDimensions->CheckButton1->setChecked(false);
      GroupDimensions->CheckButton1->setEnabled(isDigit);
    }
    else if(send == GroupDimensions->SpinBox_DX2){
      GroupDimensions->SpinBox_DX2->text().toDouble(&isDigit);
      if(!isDigit)
        GroupDimensions->CheckButton2->setChecked(false);
      GroupDimensions->CheckButton2->setEnabled(isDigit);
    }
    break;
  }
}

//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void TransformationGUI_MultiTranslationDlg::ValueChangedInSpinBox (double newValue)
{
  QObject* send = (QObject*)sender();
  switch (getConstructorId()) {
  case 0:
    if (send == GroupPoints->SpinBox_DX)
      myStepU = newValue;
    break;
  case 1:
    if (send == GroupDimensions->SpinBox_DX1)
      myStepU = newValue;
    else if (send == GroupDimensions->SpinBox_DX2)
      myStepV = newValue;
    break;
  }

  processPreview();
}

//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void TransformationGUI_MultiTranslationDlg::ValueChangedInSpinBox (int newValue)
{
  QObject* send = (QObject*)sender();
  switch (getConstructorId()) {
  case 0:
    if (send == GroupPoints->SpinBox_DY)
      myNbTimesU = newValue;
    break;
  case 1:
    if (send == GroupDimensions->SpinBox_DY1)
      myNbTimesU = newValue;
    else if (send == GroupDimensions->SpinBox_DY2)
      myNbTimesV = newValue;
    break;
  }

  processPreview();
}

//=================================================================================
// function : ReverseStepU()
// purpose  : 'state' not used here
//=================================================================================
void TransformationGUI_MultiTranslationDlg::ReverseStepU()
{
  myStepU = -myStepU;

  int aConstructorId = getConstructorId();

  if (aConstructorId == 0)
    GroupPoints->SpinBox_DX->setValue(myStepU);
  else if (aConstructorId == 1)
    GroupDimensions->SpinBox_DX1->setValue(myStepU);

  processPreview();
}

//=================================================================================
// function : ReverseStepV
// purpose  : 'state' not used here
//=================================================================================
void TransformationGUI_MultiTranslationDlg::ReverseStepV()
{
  myStepV = -myStepV;

  GroupDimensions->SpinBox_DX2->setValue(myStepV);

  processPreview();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr TransformationGUI_MultiTranslationDlg::createOperation()
{
  return getGeomEngine()->GetITransformOperations(getStudyId());
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool TransformationGUI_MultiTranslationDlg::isValid (QString& msg)
{
  bool ok = false;
  switch (getConstructorId()) {
  case 0:
    ok = GroupPoints->SpinBox_DX->isValid( msg, !IsPreview() ) &&
         GroupPoints->SpinBox_DY->isValid( msg, !IsPreview() ) &&
         myBase;
         //myBase && myVectorU; // Mantis issue 0021718
    break;
  case 1:
    ok = GroupDimensions->SpinBox_DX1->isValid( msg, !IsPreview() ) &&
         GroupDimensions->SpinBox_DY1->isValid( msg, !IsPreview() ) &&
         GroupDimensions->SpinBox_DX2->isValid( msg, !IsPreview() ) &&
         GroupDimensions->SpinBox_DY2->isValid( msg, !IsPreview() ) &&
         myBase;
         //myBase && myVectorU && myVectorV; // Mantis issue 0021718
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
bool TransformationGUI_MultiTranslationDlg::execute (ObjectList& objects)
{
  bool res = false;

  GEOM::GEOM_Object_var anObj;
  QStringList aParameters;

  GEOM::GEOM_ITransformOperations_var anOper = GEOM::GEOM_ITransformOperations::_narrow(getOperation());

  switch (getConstructorId()) {
  case 0:
    createPathPreview(myVectorU.get());
    anObj = anOper->MultiTranslate1D(myBase.get(), myVectorU.get(), myStepU, myNbTimesU);
    if (!IsPreview()) {
      aParameters << GroupPoints->SpinBox_DX->text();
      aParameters << GroupPoints->SpinBox_DY->text();
    }
    res = true;
    break;
  case 1:
    createPathPreview(myVectorU.get());
    createPathPreview(myVectorV.get());
    anObj = anOper->MultiTranslate2D(myBase.get(),
				     myVectorU.get(), myStepU, myNbTimesU,
				     myVectorV.get(), myStepV, myNbTimesV);
    if (!IsPreview()) {
      aParameters << GroupDimensions->SpinBox_DX1->text();
      aParameters << GroupDimensions->SpinBox_DY1->text();
      aParameters << GroupDimensions->SpinBox_DX2->text();
      aParameters << GroupDimensions->SpinBox_DY2->text();
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
void TransformationGUI_MultiTranslationDlg::addSubshapesToStudy()
{
  switch (getConstructorId()) {
  case 0:
    GEOMBase::PublishSubObject( myVectorU.get() );
    break;
  case 1:
    GEOMBase::PublishSubObject( myVectorU.get() );
    GEOMBase::PublishSubObject( myVectorV.get() );
    break;
  default:
    break;
  }
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> TransformationGUI_MultiTranslationDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res;
  res << myBase << myVectorU << myVectorV;
  return res;
}

//=================================================================================
// function : restoreSubShapes
// purpose  :
//=================================================================================
void TransformationGUI_MultiTranslationDlg::restoreSubShapes (SALOMEDS::Study_ptr   theStudy,
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
// function : createPathPreview
// purpose  :
//=================================================================================
void TransformationGUI_MultiTranslationDlg::createPathPreview ( GEOM::GEOM_Object_ptr thePath )
{
  if ( IsPreview() ) {
    if (thePath->_is_nil()) return;
    TopoDS_Shape aShape;
    GEOMBase::GetShape( thePath, aShape, TopAbs_SHAPE );
    TopoDS_Edge anEdge = TopoDS::Edge( aShape );
    ShapeAnalysis_Edge aShapeAnal;
    TopoDS_Vertex aFirst = aShapeAnal.FirstVertex( anEdge );
    TopoDS_Vertex aLast = aShapeAnal.LastVertex( anEdge );
    if ( BRep_Tool::Pnt(aFirst).IsEqual( BRep_Tool::Pnt(aLast), Precision::Confusion() ) ) return;
    TopoDS_Shape aVector = BRepBuilderAPI_MakeEdge(BRep_Tool::Pnt(aFirst), BRep_Tool::Pnt(aLast)).Shape();
    const char* aName = "tmpVector";
    Handle(GEOM_AISVector) anIO = new GEOM_AISVector( aVector, aName );
    
    // add Prs to preview
    SUIT_ViewWindow* vw = SUIT_Session::session()->activeApplication()->desktop()->activeWindow();
    SOCC_Prs* aPrs = dynamic_cast<SOCC_Prs*>(((SOCC_Viewer*)(vw->getViewManager()->getViewModel()))->CreatePrs(0));
    if (aPrs)
      aPrs->AddObject(anIO);
    GEOMBase_Helper::displayPreview( aPrs, false, true );
  }
}
