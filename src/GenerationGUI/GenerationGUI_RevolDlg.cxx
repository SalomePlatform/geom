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
// File   : GenerationGUI_RevolDlg.cxx
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.

#include "GenerationGUI_RevolDlg.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>

#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp_Explorer.hxx>

#include <GEOMImpl_Types.hxx>

//=================================================================================
// class    : GenerationGUI_RevolDlg()
// purpose  : Constructs a GenerationGUI_RevolDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
GenerationGUI_RevolDlg::GenerationGUI_RevolDlg (GeometryGUI* theGeometryGUI, QWidget* parent,
                                                bool modal, Qt::WindowFlags fl)
  : GEOMBase_Skeleton(theGeometryGUI, parent, modal, fl)
{
  QPixmap image0 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_REVOL")));
  QPixmap image1 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_SELECT")));

  setWindowTitle(tr("GEOM_REVOLUTION_TITLE"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_REVOLUTION"));
  mainFrame()->RadioButton1->setIcon(image0);
  mainFrame()->RadioButton2->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton3->close();

  GroupPoints = new DlgRef_2Sel1Spin2Check(centralWidget());
  GroupPoints->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  GroupPoints->TextLabel1->setText(tr("GEOM_OBJECTS"));
  GroupPoints->TextLabel2->setText(tr("GEOM_AXIS"));
  GroupPoints->TextLabel3->setText(tr("GEOM_ANGLE"));
  GroupPoints->PushButton1->setIcon(image1);
  GroupPoints->PushButton2->setIcon(image1);
  GroupPoints->LineEdit1->setReadOnly(true);
  GroupPoints->LineEdit2->setReadOnly(true);
  GroupPoints->CheckButton1->setText(tr("GEOM_BOTHWAY"));
  GroupPoints->CheckButton2->setText(tr("GEOM_REVERSE"));

  QVBoxLayout* layout = new QVBoxLayout(centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
  layout->addWidget(GroupPoints);
  /***************************************************************/

  setHelpFileName("create_revolution_page.html");

  // Initialisation
  Init();
}

//=================================================================================
// function : ~GenerationGUI_RevolDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
GenerationGUI_RevolDlg::~GenerationGUI_RevolDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void GenerationGUI_RevolDlg::Init()
{
  // min, max, step and decimals for spin boxes & initial values
  double SpecificStep = 5;
  initSpinBox(GroupPoints->SpinBox_DX, -360.0, 360.0, SpecificStep, "angle_precision" );
  GroupPoints->SpinBox_DX->setValue(45.0);

  // init variables
  GroupPoints->LineEdit1->setReadOnly(true);
  GroupPoints->LineEdit2->setReadOnly(true);

  GroupPoints->LineEdit1->setText("");
  GroupPoints->LineEdit2->setText("");

  myBaseObjects.clear();
  myAxis.nullify();

  showOnlyPreviewControl();

  // signals and slots connections
  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(GroupPoints->PushButton1,  SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->PushButton2,  SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(GroupPoints->SpinBox_DX,   SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox()));
  connect(GroupPoints->CheckButton1, SIGNAL(toggled(bool)),        this, SLOT(onBothway()));
  connect(GroupPoints->CheckButton2, SIGNAL(toggled(bool)),        this, SLOT(onReverse()));

  // san: We don't need this, as the default step value is not used in this dialog box
  //connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), this, SLOT(SetDoubleSpinBoxStep(double)));

  initName(tr("GEOM_REVOLUTION"));
  resize(100,100);

  GroupPoints->PushButton1->click();
  SelectionIntoArgument();
}

//=================================================================================
// function : SetDoubleSpinBoxStep()
// purpose  : Double spin box management
//=================================================================================
void GenerationGUI_RevolDlg::SetDoubleSpinBoxStep (double step)
{
  GroupPoints->SpinBox_DX->setSingleStep(step);
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void GenerationGUI_RevolDlg::ClickOnOk()
{
  setIsApplyAndClose( true );
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool GenerationGUI_RevolDlg::ClickOnApply()
{
  if (!onAccept())
    return false;

  initName();
  // activate selection and connect selection manager
  GroupPoints->PushButton1->click();
  return true;
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection is changed or on dialog initialization or activation
//=================================================================================
void GenerationGUI_RevolDlg::SelectionIntoArgument()
{
  erasePreview();
  myEditCurrentArgument->setText("");

  if ( myEditCurrentArgument == GroupPoints->LineEdit1 ) {
    myBaseObjects.clear();
    QList<GEOM::GeomObjPtr> objects = getSelected( TopAbs_SHAPE, -1 );
    for ( int i = 0; i < objects.count(); i++ ) {
      GEOM::shape_type stype = objects[i]->GetMaxShapeType();
      if ( stype < GEOM::SHELL || stype > GEOM::VERTEX )
        continue;
      myBaseObjects << objects[i];
    }
    if ( !myBaseObjects.isEmpty() ) {
      QString aName = myBaseObjects.count() > 1 ? QString( "%1_objects").arg( myBaseObjects.count() ) : GEOMBase::GetName( myBaseObjects[0].get() );
      myEditCurrentArgument->setText( aName );
    }
  }
  else if (myEditCurrentArgument == GroupPoints->LineEdit2) {
    myAxis = getSelected( TopAbs_EDGE );
    if ( myAxis ) {
      QString aName = GEOMBase::GetName( myAxis.get() );
      myEditCurrentArgument->setText( aName );
      if ( myBaseObjects.isEmpty() )
        GroupPoints->PushButton1->click();
    }
  }
  processPreview();
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void GenerationGUI_RevolDlg::SetEditCurrentArgument()
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
    GroupPoints->PushButton1->setDown(false);
    GroupPoints->LineEdit1->setEnabled(false);
    localSelection(TopAbs_EDGE);
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
void GenerationGUI_RevolDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
           this, SLOT( SelectionIntoArgument() ) );

  processPreview();
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void GenerationGUI_RevolDlg::enterEvent (QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void GenerationGUI_RevolDlg::ValueChangedInSpinBox()
{
  processPreview();
}

//=================================================================================
// function : getAngle()
// purpose  :
//=================================================================================
double GenerationGUI_RevolDlg::getAngle() const
{
  return GroupPoints->SpinBox_DX->value();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr GenerationGUI_RevolDlg::createOperation()
{
  return getGeomEngine()->GetI3DPrimOperations(getStudyId());
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool GenerationGUI_RevolDlg::isValid (QString& msg)
{
  return GroupPoints->SpinBox_DX->isValid( msg, !IsPreview() ) && !myBaseObjects.isEmpty() && myAxis;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool GenerationGUI_RevolDlg::execute (ObjectList& objects)
{
  GEOM::GEOM_Object_var anObj;
  GEOM::GEOM_I3DPrimOperations_var anOper = GEOM::GEOM_I3DPrimOperations::_narrow(getOperation());

  for (int i = 0; i < myBaseObjects.count(); i++) {
    if (!GroupPoints->CheckButton1->isChecked())
      anObj = anOper->MakeRevolutionAxisAngle(myBaseObjects[i].get(), myAxis.get(),
                                              getAngle() * M_PI / 180.);
    else
      anObj = anOper->MakeRevolutionAxisAngle2Ways(myBaseObjects[i].get(), myAxis.get(),
                                                   getAngle() * M_PI / 180.);
    
    if (!anObj->_is_nil()) {
      if (!IsPreview()) {
        QStringList aParameters;
        aParameters << GroupPoints->SpinBox_DX->text();
        anObj->SetParameters(aParameters.join(":").toLatin1().constData());
      }
      objects.push_back(anObj._retn());
    }
  }

  return true;
}

//=================================================================================
// function :  onReverse()
// purpose  :
//=================================================================================
void GenerationGUI_RevolDlg::onReverse()
{
  double anOldValue = GroupPoints->SpinBox_DX->value();
  GroupPoints->SpinBox_DX->setValue(-anOldValue);
}

//=================================================================================
// function :  onBothway()
// purpose  :
//=================================================================================
void GenerationGUI_RevolDlg::onBothway()
{
  GroupPoints->CheckButton2->setEnabled(!GroupPoints->CheckButton1->isChecked());
  processPreview();
}

//=================================================================================
// function : addSubshapeToStudy
// purpose  : virtual method to add new SubObjects if local selection
//=================================================================================
void GenerationGUI_RevolDlg::addSubshapesToStudy()
{
  GEOMBase::PublishSubObject( myAxis.get() );
}

//=================================================================================
// function : extractPrefix
// purpose  : redefined from GEOMBase_Helper class
//=================================================================================
bool GenerationGUI_RevolDlg::extractPrefix() const
{
  return myBaseObjects.count() > 1;
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> GenerationGUI_RevolDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res(myBaseObjects);
  res << myAxis;
  return res;
}
