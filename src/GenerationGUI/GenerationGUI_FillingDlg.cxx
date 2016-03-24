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
// File   : GenerationGUI_FillingDlg.cxx
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.

#include "GenerationGUI_FillingDlg.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>

#include <TColStd_MapOfInteger.hxx>

#include <GEOMImpl_Types.hxx>

//=================================================================================
// class    : GenerationGUI_FillingDlg()
// purpose  : Constructs a GenerationGUI_FillingDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
GenerationGUI_FillingDlg::GenerationGUI_FillingDlg (GeometryGUI* theGeometryGUI, QWidget* parent,
                                                    bool modal, Qt::WindowFlags fl)
  : GEOMBase_Skeleton(theGeometryGUI, parent, modal, fl)
{
  QPixmap image0 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_FILLING")));
  QPixmap image1 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_SELECT")));

  setWindowTitle(tr("GEOM_FILLING_TITLE"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr ("GEOM_FILLING"));
  mainFrame()->RadioButton1->setIcon(image0);
  mainFrame()->RadioButton2->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton3->close();

  GroupPoints = new DlgRef_1Sel5Spin1Check(centralWidget());
  GroupPoints->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  GroupPoints->TextLabel1->setText(tr("GEOM_FILLING_COMPOUND"));
  GroupPoints->TextLabel2->setText(tr("GEOM_FILLING_MIN_DEG"));
  GroupPoints->TextLabel3->setText(tr("GEOM_FILLING_TOL_2D"));
  GroupPoints->TextLabel4->setText(tr("GEOM_FILLING_NB_ITER"));
  GroupPoints->TextLabel5->setText(tr("GEOM_FILLING_MAX_DEG"));
  GroupPoints->TextLabel6->setText(tr("GEOM_FILLING_TOL_3D"));
  GroupPoints->CheckBox1->setText(tr("GEOM_FILLING_APPROX"));
  GroupPoints->TextLabel7->setText(tr("GEOM_FILLING_METHOD"));
  GroupPoints->PushButton1->setIcon(image1);
  GroupPoints->LineEdit1->setReadOnly(true);

  QVBoxLayout* layout = new QVBoxLayout(centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
  layout->addWidget(GroupPoints);
  /***************************************************************/

  setHelpFileName("create_filling_page.html");

  /* Initialisations */
  Init();
}

//=================================================================================
// function : ~GenerationGUI_FillingDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
GenerationGUI_FillingDlg::~GenerationGUI_FillingDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void GenerationGUI_FillingDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupPoints->LineEdit1;
  GroupPoints->LineEdit1->setReadOnly(true);

  initSelection();

  int SpecificStep1 = 1;
  double SpecificStep2 = 0.0001;
  /* min, max, step and decimals for spin boxes & initial values */
  initSpinBox(GroupPoints->SpinBox1, 2, MAX_NUMBER,  SpecificStep1);
  initSpinBox(GroupPoints->SpinBox2, 0.00001, 10000.0, SpecificStep2, "len_tol_precision");
  initSpinBox(GroupPoints->SpinBox3, 0, MAX_NUMBER,  SpecificStep1);
  initSpinBox(GroupPoints->SpinBox4, 1, MAX_NUMBER,  SpecificStep1);
  initSpinBox(GroupPoints->SpinBox5, 0.00001, 10000.0, SpecificStep2, "len_tol_precision");

  GroupPoints->SpinBox1->setValue(2);
  GroupPoints->SpinBox2->setValue(0.0001);
  GroupPoints->SpinBox3->setValue(0);
  GroupPoints->SpinBox4->setValue(5);
  GroupPoints->SpinBox5->setValue(0.0001);

  GroupPoints->ComboBox1->addItem(tr("GEOM_FILLING_DEFAULT"));
  GroupPoints->ComboBox1->addItem(tr("GEOM_FILLING_USEORI"));
  GroupPoints->ComboBox1->addItem(tr("GEOM_FILLING_AUTO"));

  showOnlyPreviewControl();

  /* signals and slots connections */
  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(GroupPoints->PushButton1, SIGNAL(clicked()),       this, SLOT(SetEditCurrentArgument()));

  connect(GroupPoints->ComboBox1, SIGNAL(activated(int)), this, SLOT(MethodChanged()));

  connect(GroupPoints->SpinBox1, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupPoints->SpinBox2, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupPoints->SpinBox3, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupPoints->SpinBox4, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupPoints->SpinBox5, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));

  connect(GroupPoints->CheckBox1, SIGNAL(stateChanged(int)),
           this, SLOT(ApproxChanged()));

  // san: Commented, as all spin boxes here either are integer or use specific non-default step value
  //connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), this, SLOT(SetDoubleSpinBoxStep(double)));

  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(),
           SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  initName(tr("GEOM_FILLING"));
  resize(100,100);
}

//=================================================================================
// function : SetDoubleSpinBoxStep()
// purpose  : Double spin box management
//=================================================================================
void GenerationGUI_FillingDlg::SetDoubleSpinBoxStep(double step)
{
  GroupPoints->SpinBox2->setSingleStep(step);
  GroupPoints->SpinBox5->setSingleStep(step);
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void GenerationGUI_FillingDlg::ClickOnOk()
{
  setIsApplyAndClose(true);
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool GenerationGUI_FillingDlg::ClickOnApply()
{
  if (!onAccept())
    return false;

  initName();
  initSelection();

  return true;
}

//=================================================================================
// function : initSelection
// purpose  :
//=================================================================================
void GenerationGUI_FillingDlg::initSelection()
{
  TColStd_MapOfInteger aTypes;
  aTypes.Add( GEOM_EDGE );
  aTypes.Add( GEOM_WIRE );
  aTypes.Add( GEOM_COMPOUND );

  std::list<int> needTypes;
  needTypes.push_back( TopAbs_EDGE );
  needTypes.push_back( TopAbs_WIRE );
  needTypes.push_back( TopAbs_COMPOUND );

  globalSelection( aTypes );
  localSelection( needTypes );
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void GenerationGUI_FillingDlg::SelectionIntoArgument()
{
  erasePreview();
  myEditCurrentArgument->setText("");

  if (myEditCurrentArgument == GroupPoints->LineEdit1) {
    QList<GEOM::GeomObjPtr> objs = getSelected(TopAbs_SHAPE, -1);
    GEOMBase::Synchronize(myObjects, objs);
    if ( !myObjects.isEmpty() ) {
      QString aName = myObjects.count() > 1 ? QString( "%1_objects").arg( myObjects.count() ) : GEOMBase::GetName( myObjects[0].get() );
      myEditCurrentArgument->setText( aName );
    }
  }
  processPreview();
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void GenerationGUI_FillingDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  if (send == GroupPoints->PushButton1) {
    GroupPoints->LineEdit1->setFocus();
    myEditCurrentArgument = GroupPoints->LineEdit1;
    initSelection();
    this->SelectionIntoArgument();
  }
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void GenerationGUI_FillingDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(),
           SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
  initSelection();
  processPreview();
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void GenerationGUI_FillingDlg::enterEvent(QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void GenerationGUI_FillingDlg::ValueChangedInSpinBox(double newValue)
{
  processPreview();
}

//=================================================================================
// function : MethodChanged
// purpose  :
//=================================================================================
void GenerationGUI_FillingDlg::MethodChanged()
{
  processPreview();
}

//=================================================================================
// function : ApproxChanged()
// purpose  :
//=================================================================================
void GenerationGUI_FillingDlg::ApproxChanged()
{
  if (GroupPoints->CheckBox1->isChecked()) {
    // tol2d
    GroupPoints->TextLabel3->setEnabled(false);
    GroupPoints->SpinBox2->setEnabled(false);
    // nbIter
    GroupPoints->TextLabel4->setEnabled(false);
    GroupPoints->SpinBox3->setEnabled(false);
    // method
    GroupPoints->TextLabel7->setEnabled(false);
    GroupPoints->ComboBox1->setEnabled(false);
  }
  else {
    // tol2d
    GroupPoints->TextLabel3->setEnabled(true);
    GroupPoints->SpinBox2->setEnabled(true);
    // nbIter
    GroupPoints->TextLabel4->setEnabled(true);
    GroupPoints->SpinBox3->setEnabled(true);
    // method
    GroupPoints->TextLabel7->setEnabled(true);
    GroupPoints->ComboBox1->setEnabled(true);
  }

  processPreview();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr GenerationGUI_FillingDlg::createOperation()
{
  return getGeomEngine()->GetI3DPrimOperations(getStudyId());
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool GenerationGUI_FillingDlg::isValid(QString& msg)
{
  bool ok = ( GroupPoints->SpinBox1->isValid(msg, !IsPreview()) &&
              GroupPoints->SpinBox2->isValid(msg, !IsPreview()) &&
              GroupPoints->SpinBox3->isValid(msg, !IsPreview()) &&
              GroupPoints->SpinBox4->isValid(msg, !IsPreview()) &&
              GroupPoints->SpinBox5->isValid(msg, !IsPreview()) &&
              myObjects.count() );
  return ok;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool GenerationGUI_FillingDlg::execute(ObjectList& objects)
{
  GEOM::GEOM_I3DPrimOperations_var anOper =
    GEOM::GEOM_I3DPrimOperations::_narrow(getOperation());

  GEOM::filling_oper_method aMethod;
  switch (GroupPoints->ComboBox1->currentIndex())
  {
  case 0:  aMethod = GEOM::FOM_Default; break;
  case 1:  aMethod = GEOM::FOM_UseOri; break;
  case 2:  aMethod = GEOM::FOM_AutoCorrect; break;
  default: break;
  }

  GEOM::ListOfGO_var objList = new GEOM::ListOfGO;
  objList->length( myObjects.count() );
  for ( int i = 0; i < myObjects.count(); ++i )
    objList[i] = myObjects[i].copy();

  GEOM::GEOM_Object_var anObj =
    anOper->MakeFilling(objList,
                        GroupPoints->SpinBox1->value(),
                        GroupPoints->SpinBox4->value(),
                        GroupPoints->SpinBox2->value(),
                        GroupPoints->SpinBox5->value(),
                        GroupPoints->SpinBox3->value(),
                        aMethod,
                        GroupPoints->CheckBox1->isChecked());
  if (!anObj->_is_nil())
  {
    if (!IsPreview())
    {
      QStringList aParameters;
      aParameters << GroupPoints->SpinBox1->text();
      aParameters << GroupPoints->SpinBox2->text();
      aParameters << GroupPoints->SpinBox3->text();
      aParameters << GroupPoints->SpinBox4->text();
      aParameters << GroupPoints->SpinBox5->text();
      anObj->SetParameters(aParameters.join(":").toLatin1().constData());
    }
    objects.push_back(anObj._retn());
  }

  return true;
}

//=================================================================================
// function : addSubshapesToStudy
// purpose  : virtual method to add new SubObjects if local selection
//=================================================================================
void GenerationGUI_FillingDlg::addSubshapesToStudy()
{
  foreach( GEOM::GeomObjPtr o, myObjects )
    GEOMBase::PublishSubObject( o.get() );
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> GenerationGUI_FillingDlg::getSourceObjects()
{
  return myObjects;
}
