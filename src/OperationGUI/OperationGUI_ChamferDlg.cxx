//  GEOM GEOMGUI : GUI for Geometry component
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
// 
//  This library is free software; you can redistribute it and/or 
//  modify it under the terms of the GNU Lesser General Public 
//  License as published by the Free Software Foundation; either 
//  version 2.1 of the License. 
// 
//  This library is distributed in the hope that it will be useful, 
//  but WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
//  Lesser General Public License for more details. 
// 
//  You should have received a copy of the GNU Lesser General Public 
//  License along with this library; if not, write to the Free Software 
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
// 
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : OperationGUI_ChamferDlg.cxx
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header$

using namespace std;
#include "OperationGUI_ChamferDlg.h"

#include "DisplayGUI.h"
#include "QAD_MessageBox.h"

#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <AIS_InteractiveContext.hxx>
#include <BRepFilletAPI_MakeChamfer.hxx>
#include <BRepTools.hxx>
#include <BRep_Tool.hxx>
#include <TopExp.hxx>
#include <Precision.hxx>

#include <Standard_ErrorHandler.hxx> 
#include <Standard_Failure.hxx>

//=================================================================================
// class    : OperationGUI_ChamferDlg()
// purpose  : Constructs a OperationGUI_ChamferDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
OperationGUI_ChamferDlg::OperationGUI_ChamferDlg(QWidget* parent, const char* name, OperationGUI* theOperationGUI, SALOME_Selection* Sel, Handle(AIS_InteractiveContext) ic, bool modal, WFlags fl)
  :GEOMBase_Skeleton(parent, name, Sel, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_DLG_CHAMFER_ALL")));
  QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_DLG_CHAMFER_EDGE")));
  QPixmap image2(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_DLG_CHAMFER_FACE")));
  QPixmap image3(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_SELECT")));

    setCaption(tr("GEOM_CHAMFER_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_CHAMFER"));
  RadioButton1->setPixmap(image0);
  RadioButton2->setPixmap(image1);
  RadioButton3->setPixmap(image2);

  Group1 = new DlgRef_1Sel2Spin(this, "Group1");
  Group1->GroupBox1->setTitle(tr("GEOM_CHAMFER_ALL"));
  Group1->TextLabel1->setText(tr("GEOM_MAIN_OBJECT"));
  Group1->TextLabel2->setText(tr("GEOM_D1"));
  Group1->TextLabel3->setText(tr("GEOM_D2"));
  Group1->PushButton1->setPixmap(image3);

  Layout1->addWidget(Group1, 1, 0);
  /***************************************************************/

  /* Initialisations */
  myOperationGUI = theOperationGUI;
  Init(ic);
}


//=================================================================================
// function : ~OperationGUI_ChamferDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
OperationGUI_ChamferDlg::~OperationGUI_ChamferDlg()
{  
  /* no need to delete child widgets, Qt does it all for us */
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void OperationGUI_ChamferDlg::Init(Handle (AIS_InteractiveContext) ic)
{
  /* init variables */
  myConstructorId = 0;
  myEditCurrentArgument = Group1->LineEdit1;

  myD1 = 5.0;
  myD2 = 5.0;
  myOkShape = false;
  myIC = ic;
  myLocalContextId = -1;
  myUseLocalContext = false;

  double SpecificStep = 10.0;
  /* min, max, step and decimals for spin boxes */
  Group1->SpinBox_DX->RangeStepAndValidator(0.001, 999.999, SpecificStep, 3);
  Group1->SpinBox_DY->RangeStepAndValidator(0.001, 999.999, SpecificStep, 3);

  Group1->SpinBox_DX->SetValue(myD1);
  Group1->SpinBox_DY->SetValue(myD2);

  /* signals and slots connections */
  connect(buttonCancel, SIGNAL(clicked()), this, SLOT(ClickOnCancel()));
  connect(myGeomGUI, SIGNAL(SignalDeactivateActiveDialog()), this, SLOT(DeactivateActiveDialog()));
  connect(myGeomGUI, SIGNAL(SignalCloseAllDialogs()), this, SLOT(ClickOnCancel()));

  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));
  connect(GroupConstructors, SIGNAL(clicked(int)), this, SLOT(ConstructorsClicked(int)));

  connect(Group1->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(Group1->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(Group1->SpinBox_DX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(Group1->SpinBox_DY, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), Group1->SpinBox_DX, SLOT(SetStep(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), Group1->SpinBox_DY, SLOT(SetStep(double)));

  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  /* displays Dialog */
  Group1->show();
  this->show();

  return;
}


//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void OperationGUI_ChamferDlg::ConstructorsClicked(int constructorId)
{
  myConstructorId = constructorId;
  myGeomBase->EraseSimulationShape();
  mySimulationTopoDs.Nullify();
  disconnect(mySelection, 0, this, 0);
  myOkShape = false;
  myD1 = 5.0;
  myD2 = 5.0;

  if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC && myUseLocalContext) {
    myIC->CloseLocalContext(myLocalContextId);
    DisplayGUI* myDisplayGUI = new DisplayGUI();
    myDisplayGUI->OnDisplayAll(true);
    myUseLocalContext = false;
  }

  if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() != VIEW_OCC) {
    myConstructorId = constructorId = 0; //No subshape selection if viewer is not OCC
    RadioButton1->setChecked(TRUE);
  }

  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
  Group1->LineEdit1->setText("");
  Group1->SpinBox_DX->SetValue(myD1);
  Group1->SpinBox_DY->SetValue(myD2);
  switch (constructorId)
    {
    case 0: /* Chamfer All */
      {
	Group1->GroupBox1->setTitle(tr("GEOM_CHAMFER_ALL"));
	myShapeType = -1;
	break;
      }
    case 1: /* Chamfer edges */
      {
	Group1->GroupBox1->setTitle(tr("GEOM_CHAMFER_EDGES"));
	myShapeType = 6;
	break;
      }
    case 2: /* Chamfer Faces */
      {
	Group1->GroupBox1->setTitle(tr("GEOM_CHAMFER_FACES"));
	myShapeType = 4;
	break;
      }
    }
 return;
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void OperationGUI_ChamferDlg::ClickOnOk()
{
  this->ClickOnApply();
  this->ClickOnCancel();
  return;
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
void OperationGUI_ChamferDlg::ClickOnApply()
{
  buttonApply->setFocus();
  QApplication::setOverrideCursor(Qt::waitCursor);
  QAD_Application::getDesktop()->putInfo(tr(""));
  myGeomBase->EraseSimulationShape();
  mySimulationTopoDs.Nullify();

  bool testResult = false;
  switch(myConstructorId)
    { 
    case 0 : /* Chamfer All */
      {	
	if(myOkShape)
	  testResult = myOperationGUI->OnChamferGetAll(myShape, myD1, myD2, myShapeType, myShapeIOR);
	break;
      }
    case 1 : /* Chamfer Edge */
      {	
	if(myOkShape)
	  testResult = myOperationGUI->OnChamferGetSelected(myShape, myShapeIOR, myD1, myD2, myShapeType, 
							    myLocalContextId, myUseLocalContext);
	break;
      }
    case 2 :  /* Chamfer Face */
      {
	if(myOkShape)
	  testResult = myOperationGUI->OnChamferGetSelected(myShape, myShapeIOR, myD1, myD2, myShapeType, 
							    myLocalContextId, myUseLocalContext);
	break;
      }
    }

  if(!testResult) 
    QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_ABORT"));
  else
    QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_DONE"));
  /* Reset all arguments and local context to allow user a new selection ...*/
  this->ResetStateOfDialog();
  QApplication::restoreOverrideCursor();
  return;
}


//=================================================================================
// function : ClickOnCancel()
// purpose  :
//=================================================================================
void OperationGUI_ChamferDlg::ClickOnCancel()
{
  this->ResetStateOfDialog();
  GEOMBase_Skeleton::ClickOnCancel();
  return;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection has changed
//=================================================================================
void OperationGUI_ChamferDlg::SelectionIntoArgument()
{
  myGeomBase->EraseSimulationShape();
  mySimulationTopoDs.Nullify();
  myEditCurrentArgument->setText("");
  this->ResetStateOfDialog();
  QString aString = ""; /* name of selection */

  int nbSel = myGeomBase->GetNameOfSelectedIObjects(mySelection, aString);
  if (nbSel == 1) {
    TopoDS_Shape S;
    Handle(SALOME_InteractiveObject) IO = mySelection->firstIObject();
    if(!myGeomBase->GetTopoFromSelection(mySelection, S))
      return;
    if(!IO->hasEntry()) {
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_SHAPE_IN_STUDY"));
      return;
    }

    if(!S.IsNull() &&  S.ShapeType() <= 2) {
      if(IO->IsInstance(STANDARD_TYPE(GEOM_InteractiveObject))) {
	Handle(GEOM_InteractiveObject) GIObject = Handle(GEOM_InteractiveObject)::DownCast(IO);
	myShapeIOR = GIObject->getIOR(); /* the Geom IOR string of selection */
	myEditCurrentArgument->setText(aString);
	myShape = S;
	myOkShape = true;
      }
	
      if(IO->hasEntry()) {
	SALOMEDS::Study_var aStudy = QAD_Application::getDesktop()->getActiveStudy()->getStudyDocument();
	SALOMEDS::SObject_var obj = aStudy->FindObjectID(IO->getEntry());
        SALOMEDS::GenericAttribute_var anAttr;
        SALOMEDS::AttributeIOR_var anIOR;
	if(!obj->_is_nil()) {
	  if(obj->FindAttribute(anAttr, "AttributeIOR")) {
            anIOR = SALOMEDS::AttributeIOR::_narrow(anAttr);
	    myShapeIOR = anIOR->Value();
	    myOkShape = true;
	    myShape = S;
	    myEditCurrentArgument->setText(aString);
	  }
	}
      }
    }
    if(myOkShape && QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC && myConstructorId == 0)
      this->MakePreview();

  }
  else 
    return;
  
  if(myOkShape && myShapeType!=-1 && myConstructorId != 0) {
    /* local context is defined into the method */
    DisplayGUI* myDisplayGUI = new DisplayGUI();
    myDisplayGUI->PrepareSubShapeSelection(myShapeType, myLocalContextId);  
    myUseLocalContext = true;
  }
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void OperationGUI_ChamferDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();  

  if(send == Group1->PushButton1)
    Group1->LineEdit1->setFocus();

  this->SelectionIntoArgument();
  return;
}


//=================================================================================
// function : DeactivateActiveDialog()
// purpose  :
//=================================================================================
void OperationGUI_ChamferDlg::DeactivateActiveDialog()
{
  this->ResetStateOfDialog();
  GEOMBase_Skeleton::DeactivateActiveDialog();
  return;
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void OperationGUI_ChamferDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
  if(!mySimulationTopoDs.IsNull())
    myGeomBase->DisplaySimulationShape(mySimulationTopoDs);
  return;
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void OperationGUI_ChamferDlg::enterEvent(QEvent* e)
{
  if(GroupConstructors->isEnabled())
    return;
  this->ActivateThisDialog();
  return;
}


//=================================================================================
// function : closeEvent()
// purpose  :
//=================================================================================
void OperationGUI_ChamferDlg::closeEvent(QCloseEvent* e)
{ 
  /* same than click on cancel button */
  this->ClickOnCancel();
  return;
}


//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void OperationGUI_ChamferDlg::ValueChangedInSpinBox(double newValue)
{
  myGeomBase->EraseSimulationShape(); 
  mySimulationTopoDs.Nullify();

  QObject* send = (QObject*)sender();
  if(send == Group1->SpinBox_DX)  /* D1 */
    myD1 = newValue;
  else if(send == Group1->SpinBox_DY)  /* D2 */
    myD2 = newValue;

  if(myOkShape && QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC && myConstructorId == 0)
    this->MakePreview();

  return;
}


//=================================================================================
// function : ResetStateOfDialog()
// purpose  :
//=================================================================================
void OperationGUI_ChamferDlg::ResetStateOfDialog()
{
  myOkShape = false;
  myEditCurrentArgument->setText("");
  QApplication::restoreOverrideCursor();

  /* Close its local contact if opened */
  if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC && myUseLocalContext) {
    myIC->CloseLocalContext(myLocalContextId);
    myUseLocalContext = false;
    DisplayGUI* myDisplayGUI = new DisplayGUI();
    myDisplayGUI->OnDisplayAll(true);
  }
  return;
}


//=================================================================================
// function : MakePreview()
// purpose  :
//=================================================================================
void OperationGUI_ChamferDlg::MakePreview()
{
  QApplication::setOverrideCursor(Qt::waitCursor);
  TopoDS_Shape tds;

  try {
    BRepFilletAPI_MakeChamfer MC(myShape);
    switch(myConstructorId)
      {
      case 0: /* Chamfer All */
	{
	  TopTools_IndexedDataMapOfShapeListOfShape M;
	  TopExp::MapShapesAndAncestors(myShape, TopAbs_EDGE, TopAbs_FACE, M);
	  for(int i = 1; i <= M.Extent(); i++) {
	    TopoDS_Edge E = TopoDS::Edge(M.FindKey(i));
	    TopoDS_Face F = TopoDS::Face(M.FindFromIndex(i).First());
	    if(!BRepTools::IsReallyClosed(E, F) && !BRep_Tool::Degenerated(E))
	      MC.Add(myD1, myD2, E, F);
	  }
	  tds = MC.Shape();
	  break;
	}
      //    case 1: /* Chamfer edges */
      //    case 2: /* Chamfer Faces */
      }

    if(!tds.IsNull()) {
      mySimulationTopoDs = tds;
      myGeomBase->DisplaySimulationShape(mySimulationTopoDs); 
    }
  }
  catch(Standard_Failure) {
    QApplication::restoreOverrideCursor();
    QAD_MessageBox::warn1 (QAD_Application::getDesktop(), tr("GEOM_WRN_WARNING"), tr("GEOM_CHAMFER_ABORT").arg(myD1).arg(myD2), tr("GEOM_BUT_OK"));
    QApplication::setOverrideCursor(Qt::waitCursor);
    myGeomBase->EraseSimulationShape(); 
    mySimulationTopoDs.Nullify();
    Group1->SpinBox_DX->SetValue(5.0);
    Group1->SpinBox_DY->SetValue(5.0);
  }

  QApplication::restoreOverrideCursor();
  return;
}
