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

//  GEOM GEOMGUI : GUI for Geometry component
//  File   : OperationGUI_ExtrudedFeatureDlg.cxx
//  Author : Renaud NEDELEC, Open CASCADE S.A.S.

#include "OperationGUI.h"
#include "OperationGUI_ExtrudedFeatureDlg.h"

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>

#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <DlgRef.h>

// OCCT Includes
#include <TColStd_HSequenceOfTransient.hxx>

#include <GEOMImpl_Types.hxx>

//=================================================================================
// class    : OperationGUI_ExtrudedFeatureDlg()
// purpose  : Constructs a OperationGUI_ExtrudedCutDlg which is a child of 'parent',
//            with the name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================

OperationGUI_ExtrudedFeatureDlg::OperationGUI_ExtrudedFeatureDlg(const int theOperation,
                                                                 GeometryGUI* theGeometryGUI,
                                                                 QWidget* parent,bool modal, 
                                                                 Qt::WindowFlags fl)
  : GEOMBase_Skeleton(theGeometryGUI, parent, modal, fl),
    myOperation(theOperation)
{
  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
  QPixmap image0;
  QPixmap image1 (aResMgr->loadPixmap("GEOM", tr("ICON_SELECT")));
  QPixmap image2 (aResMgr->loadPixmap("GEOM", tr("ICO_DRAFT")));
  QPixmap image3 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_CHANGE_DIRECTION")));
  QString aTitle;
  switch (myOperation) 
  {
    case OperationGUI::BOSS:
      image0 = QPixmap(aResMgr->loadPixmap("GEOM", tr("ICO_EXTRUDED_BOSS")));
      aTitle = tr("GEOM_EXTRUDED_BOSS_TITLE");
      setHelpFileName("extruded_boss_operation_page.html");
      break;
    case OperationGUI::CUT:
      image0 = QPixmap( aResMgr->loadPixmap("GEOM", tr("ICO_EXTRUDED_CUT")));
      aTitle = tr("GEOM_EXTRUDED_CUT_TITLE");
      setHelpFileName("extruded_cut_operation_page.html");
      break;
  }
    
  setWindowTitle(aTitle);
  mainFrame()->GroupConstructors->setTitle(aTitle);
  mainFrame()->RadioButton1->setIcon(image0);
  mainFrame()->RadioButton2->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton3->close();
  
  mainFrame()->RadioButton1->setChecked(true);
  
  myGroup = new DlgRef_2Sel2Spin2Push(centralWidget());
  
  myGroup->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  
  myGroup->PushButton1->setIcon(image1);
  myGroup->PushButton2->setIcon(image1);
  myGroup->PushButton3->setIcon(image2);
  myGroup->PushButton4->setIcon(image3);
  myGroup->LineEdit1->setReadOnly(true);
  myGroup->LineEdit2->setReadOnly(true);
  myGroup->TextLabel1->setText(tr("GEOM_INIT_SHAPE"));
  myGroup->TextLabel2->setText(tr("GEOM_PROFILE"));
  myGroup->TextLabel3->setText(tr("GEOM_HEIGHT"));
  myGroup->TextLabel4->setText(tr("GEOM_DRAFT_ANGLE"));
  myGroup->TextLabel5->setText(tr("GEOM_CHANGE_DIRECTION"));
 
  
  QVBoxLayout* layout = new QVBoxLayout(centralWidget());
  layout->setMargin(0); 
  layout->setSpacing(6);
  layout->addWidget(myGroup);

  Init();
   
}

//=================================================================================
// function : ~OperationGUI_ExtrudedFeatureDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
OperationGUI_ExtrudedFeatureDlg::~OperationGUI_ExtrudedFeatureDlg()
{
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void OperationGUI_ExtrudedFeatureDlg::Init()
{
  // init variables
  myEditCurrentArgument = myGroup->LineEdit1;
  
  // Get setting of step value from file configuration
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  double step = resMgr->doubleValue("Geometry", "SettingsGeomStep", 100);
  
  initSpinBox(myGroup->SpinBox_DX, COORD_MIN, COORD_MAX, step, "length_precision" );
  initSpinBox(myGroup->SpinBox_DY, COORD_MIN, COORD_MAX, step, "length_precision" );

  myGroup->LineEdit1->setText("");
  myGroup->LineEdit2->setText("");
  myGroup->SpinBox_DX->setValue(10.0);
  myGroup->SpinBox_DY->setValue(10.0);
  myGroup->SpinBox_DY->setEnabled(false);
  myObject1.nullify();
  myObject2.nullify();
  
  // signals and slots connections
  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(myGroup->PushButton1, SIGNAL(clicked()),     this, SLOT(SetEditCurrentArgument()));
  connect(myGroup->PushButton2, SIGNAL(clicked()),     this, SLOT(SetEditCurrentArgument()));
  connect(myGroup->PushButton3, SIGNAL(clicked(bool)), this, SLOT(ButtonClicked(bool)));
  connect(myGroup->PushButton4, SIGNAL(clicked(bool)), this, SLOT(ButtonClicked(bool)));
  connect(myGroup->SpinBox_DX,  SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox()));
  connect(myGroup->SpinBox_DY,  SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox()));
  

  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(),
           SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
  
  switch (myOperation) 
  {
    case OperationGUI::BOSS:
      initName(tr("GEOM_EXTRUDED_BOSS"));     
      break;
    case OperationGUI::CUT:
      initName(tr("GEOM_EXTRUDED_CUT"));  
      break;
  }
  
  mainFrame()->RadioButton1->setFocus();
  
  globalSelection(GEOM_ALLSHAPES);
  
  myGroup->PushButton1->click();
  SelectionIntoArgument();
  
  resize(minimumSizeHint());
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void OperationGUI_ExtrudedFeatureDlg::ClickOnOk()
{
  setIsApplyAndClose( true );
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool OperationGUI_ExtrudedFeatureDlg::ClickOnApply()
{
  if (!onAccept())
    return false;

  initName();
  // activate selection and connect selection manager
  myGroup->PushButton1->click();
  return true;
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void OperationGUI_ExtrudedFeatureDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();
  
  if (send == myGroup->PushButton1) {
    myEditCurrentArgument = myGroup->LineEdit1;

    myGroup->PushButton2->setDown(false);
    myGroup->LineEdit2->setEnabled(false);
  }
  else if (send == myGroup->PushButton2) {
    myEditCurrentArgument = myGroup->LineEdit2;

    myGroup->PushButton1->setDown(false);
    myGroup->LineEdit1->setEnabled(false);
  }
  
  // enable line edit
  myEditCurrentArgument->setEnabled(true);
  myEditCurrentArgument->setFocus();
  // after setFocus(), because it will be setDown(false) when loses focus
  send->setDown(true);
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection is changed or on dialog initialization or activation
//=================================================================================
void OperationGUI_ExtrudedFeatureDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("");

  QList<TopAbs_ShapeEnum> types;
  types<<TopAbs_SHAPE;
  
  if (myEditCurrentArgument == myGroup->LineEdit1)
  {
    types.clear();
    types<<TopAbs_COMPOUND<<TopAbs_SOLID;
  }
  if (myEditCurrentArgument == myGroup->LineEdit2)
  {
    types.clear();
    types << TopAbs_EDGE << TopAbs_WIRE << TopAbs_FACE;
  }

  GEOM::GeomObjPtr aSelectedObject = getSelected( types );
  TopoDS_Shape aShape;
  if ( aSelectedObject && GEOMBase::GetShape( aSelectedObject.get(), aShape ) && !aShape.IsNull() ) {
    QString aName = GEOMBase::GetName( aSelectedObject.get() );
    myEditCurrentArgument->setText( aName );

    // clear selection
    disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
    myGeomGUI->getApp()->selectionMgr()->clearSelected();
    connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
            this, SLOT(SelectionIntoArgument()));

    if (myEditCurrentArgument == myGroup->LineEdit1) {
      myObject1 = aSelectedObject;
      if (!myObject2)
        myGroup->PushButton2->click();
    }
    else if (myEditCurrentArgument == myGroup->LineEdit2) {
      myObject2 = aSelectedObject;
      if (!myObject1)
        myGroup->PushButton1->click();
    }
    
    displayPreview(true);
  }
  else {
    if      (myEditCurrentArgument == myGroup->LineEdit1) myObject1.nullify();
    else if (myEditCurrentArgument == myGroup->LineEdit2) myObject2.nullify();
  }
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void OperationGUI_ExtrudedFeatureDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();

  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
           this, SLOT( SelectionIntoArgument() ) );
  processPreview();
}


//=================================================================================
// function : enterEvent()
// purpose  : when mouse enter onto the QWidget
//=================================================================================
void OperationGUI_ExtrudedFeatureDlg::enterEvent (QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void OperationGUI_ExtrudedFeatureDlg::ValueChangedInSpinBox()
{
  displayPreview(true);
}

//=================================================================================
// function : ButtonClicked(bool checked)
// purpose  :
//=================================================================================
void OperationGUI_ExtrudedFeatureDlg::ButtonClicked(bool checked)
{
  QPushButton* send = (QPushButton*)sender();
  if (send == myGroup->PushButton3)
    myGroup->SpinBox_DY->setEnabled(checked);
 
  displayPreview(true);
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr OperationGUI_ExtrudedFeatureDlg::createOperation()
{
  return getGeomEngine()->GetI3DPrimOperations(getStudyId());
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool OperationGUI_ExtrudedFeatureDlg::isValid (QString&)
{
  return myObject1 && myObject2; 
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool OperationGUI_ExtrudedFeatureDlg::execute (ObjectList& objects)
{
  GEOM::GEOM_I3DPrimOperations_var anOper = GEOM::GEOM_I3DPrimOperations::_narrow(getOperation());
  
  double angle=0.0;
  double aHeight = myGroup->SpinBox_DX->value();
  
  if (myGroup->PushButton3->isChecked())
    angle=myGroup->SpinBox_DY->value();
    
  bool isProtrusion = (myOperation == OperationGUI::BOSS);
  bool isInvert = myGroup->PushButton4->isChecked();
  
  // Hide the initial shape in order to see the modifications on the preview
  erase(myObject1.get(),false); 
  
  GEOM::GEOM_Object_var anObj = anOper->MakeDraftPrism(myObject1.get(), myObject2.get(), 
                                                       aHeight,
                                                       angle,
                                                       isProtrusion,
                                                       isInvert);
  if (!anObj->_is_nil()) {
    if (!IsPreview()) {
      QStringList aParameters;
      aParameters << myGroup->SpinBox_DX->text();
      if (myGroup->PushButton3->isChecked())
        aParameters << myGroup->SpinBox_DY->text();
      anObj->SetParameters(aParameters.join(":").toLatin1().constData());
    }
    objects.push_back(anObj._retn());
  }

  return true;
}

// //=================================================================================
// // function : addSubshapeToStudy
// // purpose  : virtual method to add new SubObjects if local selection
// //=================================================================================
// void OperationGUI_ExtrudedFeatureDlg::addSubshapesToStudy()
// {
//   GEOMBase::PublishSubObject( myObject2.get() );
// }

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> OperationGUI_ExtrudedFeatureDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res;
  res << myObject1 << myObject2;
  return res;
}
