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
//  File   : RepairGUI_SuppressFacesDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

using namespace std;
#include "RepairGUI_SuppressFacesDlg.h"

#include "DisplayGUI.h"
#include "QAD_RightFrame.h"
#include "OCCViewer_Viewer3d.h"

//=================================================================================
// class    : RepairGUI_SuppressFacesDlg()
// purpose  : Constructs a RepairGUI_SuppressFacesDlg  which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
RepairGUI_SuppressFacesDlg::RepairGUI_SuppressFacesDlg(QWidget* parent, const char* name, RepairGUI* theRepairGUI, SALOME_Selection* Sel, Handle(AIS_InteractiveContext) ic, bool modal, WFlags fl)
  :GEOMBase_Skeleton(parent, name, Sel, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_DLG_SUPRESS_FACE")));
  QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_SELECT")));
    
  setCaption(tr("GEOM_SUPRESSFACE_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_SUPRESSFACE"));
  RadioButton1->setPixmap(image0);
  RadioButton2->close(TRUE);
  RadioButton3->close(TRUE);

  GroupPoints = new DlgRef_1Sel1Check_QTD(this, "GroupPoints");
  GroupPoints->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  GroupPoints->TextLabel1->setText(tr("GEOM_MAIN_OBJECT"));
  GroupPoints->CheckButton1->setText(tr("GEOM_SUPRESSFACE_SELECT"));
  GroupPoints->PushButton1->setPixmap(image1);

  Layout1->addWidget(GroupPoints, 1, 0);
  /***************************************************************/

  /* Initialisations */
  myRepairGUI = theRepairGUI;
  Init(ic);
}


//=================================================================================
// function : ~RepairGUI_SuppressFacesDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
RepairGUI_SuppressFacesDlg::~RepairGUI_SuppressFacesDlg()
{
  // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void RepairGUI_SuppressFacesDlg::Init(Handle(AIS_InteractiveContext) ic)
{
  /* init variables */
  myEditCurrentArgument = GroupPoints->LineEdit1;

  myOkSelectSubMode = false;
  myOkShape = myUseLocalContext = false;
  myIC = ic;
  myLocalContextId = -1;

  /* signals and slots connections */
  connect(buttonCancel, SIGNAL(clicked()), this, SLOT(ClickOnCancel()));
  connect(myGeomGUI, SIGNAL(SignalDeactivateActiveDialog()), this, SLOT(DeactivateActiveDialog()));
  connect(myGeomGUI, SIGNAL(SignalCloseAllDialogs()), this, SLOT(ClickOnCancel()));

  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(GroupPoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(GroupPoints->CheckButton1, SIGNAL(stateChanged(int)), this, SLOT(ActivateUserSelection()));
  
  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  /* displays Dialog */
  GroupPoints->show();
  this->show();

  return;
}


//=================================================================================
// function : ClickOnOk()
// purpose  : Same than click on apply but close this dialog.
//=================================================================================
void RepairGUI_SuppressFacesDlg::ClickOnOk()
{
  this->ClickOnApply();
  this->ClickOnCancel();
  return;
}



//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
void RepairGUI_SuppressFacesDlg::ClickOnApply()
{
  QAD_Application::getDesktop()->putInfo(tr(""));
  bool testResult = false;

  if(myOkShape && myOkSelectSubMode)
    testResult = myRepairGUI->OnSuppressFaces(myShape, myShapeIOR, myLocalContextId, myUseLocalContext); 

  if(!testResult)
    QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_ABORT"));
  else
    QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_DONE"));

  /* Reset arguments to allow a new selection */
  this->ResetStateOfDialog();
  return ;
}


//=================================================================================
// function : ClickOnCancel()
// purpose  :
//=================================================================================
void RepairGUI_SuppressFacesDlg::ClickOnCancel()
{
  if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC) {
    OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
    myIC = v3d->getAISContext();
    if(myUseLocalContext) {
      myIC->CloseLocalContext(myLocalContextId);
      myUseLocalContext = false;
    }
    DisplayGUI* myDisplayGUI = new DisplayGUI();
    myDisplayGUI->OnDisplayAll(true);
  }
  GEOMBase_Skeleton::ClickOnCancel();
  return;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//          : used only by SelectButtonC1A1 (LineEditC1A1)
//=================================================================================
void RepairGUI_SuppressFacesDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("");
  this->ResetStateOfDialog();
  QString aString = ""; /* name of selection */

  int nbSel = myGeomBase->GetNameOfSelectedIObjects(mySelection, aString);
  if(nbSel != 1) {
    myOkShape = false;
    return;
  }
  
  /* nbSel == 1 */
  TopoDS_Shape S;
  Handle(SALOME_InteractiveObject) IO = mySelection->firstIObject();
  if(!myGeomBase->GetTopoFromSelection(mySelection, S))
    return;
  
  if(!IO->hasEntry()) {
    QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_SHAPE_IN_STUDY"));
    return;
  }

  /* Test the exact type of topology to suppress faces into */
  if(!S.IsNull() && (S.ShapeType() == TopAbs_SOLID || S.ShapeType() == TopAbs_SHELL || S.ShapeType() == TopAbs_COMPOUND)) {
    if(IO->IsInstance(STANDARD_TYPE(GEOM_InteractiveObject))) {
      Handle(GEOM_InteractiveObject) GIObject = Handle(GEOM_InteractiveObject)::DownCast(IO);
      myShapeIOR = GIObject->getIOR(); /* the Geom IOR string of selection */
      myEditCurrentArgument->setText(aString);
      myShape = S;
      myOkShape = true;
      return;
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
	  return;
	}
      }
    }

  }
  return;
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void RepairGUI_SuppressFacesDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  if(send == GroupPoints->PushButton1) {
    GroupPoints->LineEdit1->setFocus();
    myEditCurrentArgument = GroupPoints->LineEdit1;
  }
  this->SelectionIntoArgument();

  return;
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void RepairGUI_SuppressFacesDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if(send == GroupPoints->LineEdit1)
    myEditCurrentArgument = GroupPoints->LineEdit1;
  else
    return;

  GEOMBase_Skeleton::LineEditReturnPressed();
  return;
}


//=================================================================================
// function : DeactivateActiveDialog()
// purpose  :
//=================================================================================
void RepairGUI_SuppressFacesDlg::DeactivateActiveDialog()
{
  if(GroupConstructors->isEnabled()) {
    this->ResetStateOfDialog();
    GEOMBase_Skeleton::DeactivateActiveDialog();
  }
  return;
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void RepairGUI_SuppressFacesDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
  return;
}


//=================================================================================
// function : enterEvent()
// purpose  : Mouse enter onto the dialog to activate it
//=================================================================================
void RepairGUI_SuppressFacesDlg::enterEvent(QEvent* e)
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
void RepairGUI_SuppressFacesDlg::closeEvent(QCloseEvent* e)
{
  /* same than click on cancel button */
  this->ClickOnCancel();
  return;
}


//=================================================================================
// function : ActivateUserSelection()
// purpose  : Activate selection of faces when CheckBox1->isChecked()...
//=================================================================================
void RepairGUI_SuppressFacesDlg::ActivateUserSelection()
{
  if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC) {
    if(!myOkShape) {
      this->ResetStateOfDialog();
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_SELECT_FIRST"));
      return;
    }
  
    myOkSelectSubMode = GroupPoints->CheckButton1->isChecked();
    
    if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC) {
      OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
      myIC = v3d->getAISContext();
      if(myUseLocalContext) {
	myIC->CloseLocalContext(myLocalContextId);
	myUseLocalContext = false;
      }
      DisplayGUI* myDisplayGUI = new DisplayGUI();
      myDisplayGUI->OnDisplayAll(true);
    }
    
    if(myOkShape && myOkSelectSubMode) {
      /* local context is defined into the method : 4 = FACES sub selection */
      DisplayGUI* myDisplayGUI = new DisplayGUI();
      myDisplayGUI->PrepareSubShapeSelection(int(TopAbs_FACE), myLocalContextId);    
      myUseLocalContext = true;
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_SELECT_FACE"));
    }
  } 
  else
    GroupPoints->CheckButton1->setChecked(false);
  return;
}



//=================================================================================
// function : ResetStateOfDialog()
// purpose  : Completely reset the state of method including local context
//=================================================================================
void RepairGUI_SuppressFacesDlg::ResetStateOfDialog()
{
  myOkShape = false ;
  myEditCurrentArgument->setText("");

  /* Select sub shapes mode not checked */
  myOkSelectSubMode = false;    
  GroupPoints->CheckButton1->setChecked(FALSE);

  /* Close its local contact if opened */
  if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC) {
    OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
    myIC = v3d->getAISContext();
    if(myUseLocalContext) {
      myIC->CloseLocalContext(myLocalContextId);
      myUseLocalContext = false;
    }
    DisplayGUI* myDisplayGUI = new DisplayGUI();
    myDisplayGUI->OnDisplayAll(true);
  }
  return;
}
