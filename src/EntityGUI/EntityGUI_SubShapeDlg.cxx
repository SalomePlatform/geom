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
//  File   : EntityGUI_SubShapeDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

using namespace std;
#include "EntityGUI_SubShapeDlg.h"

#include "DisplayGUI.h"
#include "QAD_RightFrame.h"
#include "OCCViewer_Viewer3d.h"

#include <TopExp_Explorer.hxx>
#include <TopTools_MapOfShape.hxx>

#include <qmessagebox.h>

//=================================================================================
// class    : EntityGUI_SubShapeDlg()
// purpose  : Constructs a EntityGUI_SubShapeDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
EntityGUI_SubShapeDlg::EntityGUI_SubShapeDlg(QWidget* parent, const char* name, EntityGUI* theEntityGUI, SALOME_Selection* Sel, Handle(AIS_InteractiveContext) ic, bool modal, WFlags fl)
  :GEOMBase_Skeleton(parent, name, Sel, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_DLG_SUBSHAPE")));
  QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_SUBSHAPE_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_SUB_SHAPE"));
  RadioButton1->setPixmap(image0);
  RadioButton2->close(TRUE);
  RadioButton3->close(TRUE);

  GroupPoints = new DlgRef_1Sel1Check1List_QTD(this, "GroupPoints");
  GroupPoints->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  GroupPoints->TextLabel1->setText(tr("GEOM_MAIN_OBJECT"));
  GroupPoints->TextLabel2->setText(tr("GEOM_SUBSHAPE_TYPE"));
  GroupPoints->CheckButton1->setText(tr("GEOM_SUBSHAPE_SELECT"));
  GroupPoints->PushButton1->setPixmap(image1);

  Layout1->addWidget(GroupPoints, 1, 0);
  /***************************************************************/

  /* Initialisations */
  myEntityGUI = theEntityGUI;
  Init(ic);
}


//=================================================================================
// function : ~EntityGUI_SubShapeDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
EntityGUI_SubShapeDlg::~EntityGUI_SubShapeDlg()
{
  // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void EntityGUI_SubShapeDlg::Init(Handle(AIS_InteractiveContext) ic)
{
  /* init variables */
  myEditCurrentArgument = GroupPoints->LineEdit1;

  myWithShape = true;
  myAbort = myOkShape = myUseLocalContext = false;
  myIC = ic;
  myLocalContextId = -1;

  /* type for sub shape selection */
  GroupPoints->ComboBox1->insertItem("Compound");
  GroupPoints->ComboBox1->insertItem("Compsolid");
  GroupPoints->ComboBox1->insertItem("Solid");
  GroupPoints->ComboBox1->insertItem("Shell");
  GroupPoints->ComboBox1->insertItem("Face");
  GroupPoints->ComboBox1->insertItem("Wire");
  GroupPoints->ComboBox1->insertItem("Edge");
  GroupPoints->ComboBox1->insertItem("Vertex");
  GroupPoints->ComboBox1->insertItem("Shape");

  myShapeType = GroupPoints->ComboBox1->currentItem();
  myOkSelectSubMode = GroupPoints->CheckButton1->isChecked();

  if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() != VIEW_OCC)
    GroupPoints->CheckButton1->setEnabled(false);

  /* signals and slots connections */
  connect(buttonCancel, SIGNAL(clicked()), this, SLOT(ClickOnCancel()));
  connect(myGeomGUI, SIGNAL(SignalDeactivateActiveDialog()), this, SLOT(DeactivateActiveDialog()));
  connect(myGeomGUI, SIGNAL(SignalCloseAllDialogs()), this, SLOT(ClickOnCancel()));

  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(GroupPoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(GroupPoints->ComboBox1, SIGNAL(activated(int)), this, SLOT(ComboTextChanged()));
  connect(GroupPoints->CheckButton1, SIGNAL(stateChanged(int)), this, SLOT(AllOrNotAll()));
  
  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  /* displays Dialog */
  GroupPoints->show();
  this->show();

  return;
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void EntityGUI_SubShapeDlg::ClickOnOk()
{
  this->ClickOnApply();

  /* User has aborted or not operation of explode all with many sub shapes */
  if(myAbort == false)
    this->ClickOnCancel();
  else
    myAbort = false;

  return;
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
void EntityGUI_SubShapeDlg::ClickOnApply()
{
  QAD_Application::getDesktop()->putInfo(tr(""));
  bool testResult = false;
  myAbort = false; /* Not aborted by default */
    
  /* Explode all sub shapes */
  if(myOkShape && !myOkSelectSubMode) {
    /* More than 30 subshapes : ask confirmation */
    unsigned int nb = NumberOfSubShapes(myShape, myShapeType);
    if(nb > 30) {
      const QString caption = tr("GEOM_CONFIRM");
      const QString text = tr("GEOM_CONFIRM_INFO").arg(nb);
      const QString button0 = tr("GEOM_BUT_EXPLODE");
      const QString button1 = tr("GEOM_BUT_CANCEL");
	    
      if(QMessageBox::warning(this, caption, text, button0, button1) == 0)
	testResult = myEntityGUI->OnSubShapeGetAll(myShape, myShapeIOR, myShapeType);
      else
	myAbort = true;  /* aborted */
    }
    else
      testResult = myEntityGUI->OnSubShapeGetAll(myShape, myShapeIOR, myShapeType);
  }
  /* explode only selected sub shapes */
  else if(myOkShape && myOkSelectSubMode)
    testResult = myEntityGUI->OnSubShapeGetSelected(myShape, myShapeIOR, myShapeType, myLocalContextId, myUseLocalContext); 

  if(!testResult) {
    QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_ABORT"));
    myAbort = true;
  }
  else
    QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_DONE"));

  /* Reset all arguments and local context to allow user a new selection ...*/
  //this->ResetStateOfDialog();
  return;
}


//=================================================================================
// function : ClickOnCancel()
// purpose  :
//=================================================================================
void EntityGUI_SubShapeDlg::ClickOnCancel()
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
void EntityGUI_SubShapeDlg::SelectionIntoArgument()
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

  if(!S.IsNull() && S.ShapeType() != TopAbs_VERTEX) {
    if(IO->IsInstance(STANDARD_TYPE(GEOM_InteractiveObject))) {
      Handle(GEOM_InteractiveObject) GIObject = Handle(GEOM_InteractiveObject)::DownCast( IO );
      myShapeIOR = GIObject->getIOR(); /* the Geom IOR string of selection */
      myEditCurrentArgument->setText(aString);
      myShape = S;
      myOkShape = true;
    }
    else {
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
	  GroupPoints->LineEdit1->setText(aString);
	}
      }
    }
    
    int SelectedShapeType = GroupPoints->ComboBox1->currentItem();
    int count = GroupPoints->ComboBox1->count();

    if(myWithShape) 
      count = count - 1;

    int i = 0;
    while(i <= myShape.ShapeType()) {
      GroupPoints->ComboBox1->removeItem(0);
      i++;
    }

    if(myShape.ShapeType()==TopAbs_COMPOUND) {
      if(myWithShape == false) {
	GroupPoints->ComboBox1->insertItem("Shape");
	myWithShape = true;
      }
    }
    else {
      if(myWithShape == true) {
	GroupPoints->ComboBox1->removeItem(GroupPoints->ComboBox1->count() - 1);
	myWithShape = false;
      }
    }
    
    int count1 = GroupPoints->ComboBox1->count();
    if(myWithShape)
      count1 = count1 - 1;
    
    if(SelectedShapeType > myShape.ShapeType()) {
      if(SelectedShapeType == 8) {
	if(myShape.ShapeType() != TopAbs_COMPOUND) {
	  GroupPoints->ComboBox1->setCurrentItem(0);
	  myShapeType = 8 - count1;
	}
      }
      else {
	GroupPoints->ComboBox1->setCurrentItem(count1 - count + SelectedShapeType);
	myShapeType = 8 - count1 + GroupPoints->ComboBox1->currentItem();
      }
    } 
    else {
      GroupPoints->ComboBox1->setCurrentItem(0);
      myShapeType = 8 - count1;
    }    
  }
  return;
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void EntityGUI_SubShapeDlg::SetEditCurrentArgument()
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
void EntityGUI_SubShapeDlg::LineEditReturnPressed()
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
void EntityGUI_SubShapeDlg::DeactivateActiveDialog()
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
void EntityGUI_SubShapeDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
  if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC)
    GroupPoints->CheckButton1->setEnabled(true);
  else
    GroupPoints->CheckButton1->setEnabled(false);
  return;
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void EntityGUI_SubShapeDlg::enterEvent(QEvent* e)
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
void EntityGUI_SubShapeDlg::closeEvent(QCloseEvent* e)
{
  /* same than click on cancel button */
  this->ClickOnCancel();
  return;
}


//=================================================================================
// function : ResetStateOfDialog()
// purpose  : Completely reset the state of method including local context
//=================================================================================
void EntityGUI_SubShapeDlg::ResetStateOfDialog()
{
  if(myAbort == true) {
    myOkShape = false;
    myEditCurrentArgument->setText("");
  }

  int SelectedShapeType = GroupPoints->ComboBox1->currentItem();
  int count = GroupPoints->ComboBox1->count();
  if(myWithShape)
    count = count - 1;

  /* type for sub shape selection */
  GroupPoints->ComboBox1->clear();
  GroupPoints->ComboBox1->insertItem("Compound");
  GroupPoints->ComboBox1->insertItem("Compsolid");
  GroupPoints->ComboBox1->insertItem("Solid");
  GroupPoints->ComboBox1->insertItem("Shell");
  GroupPoints->ComboBox1->insertItem("Face");
  GroupPoints->ComboBox1->insertItem("Wire");
  GroupPoints->ComboBox1->insertItem("Edge");
  GroupPoints->ComboBox1->insertItem("Vertex");
  GroupPoints->ComboBox1->insertItem("Shape");
  myWithShape=true;
  GroupPoints->ComboBox1->setCurrentItem(8 - count + SelectedShapeType);
  myOkSelectSubMode = false;

  /* unpress buttons : due to abort box*/
  buttonApply->setDown(FALSE);
  buttonOk->setDown(FALSE);
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


//=================================================================================
// function : AllOrNotAll()
// purpose  : Allow user selection of all or only selected sub shapes
//          : Called when 'CheckButton1' state change
//=================================================================================
void EntityGUI_SubShapeDlg::AllOrNotAll()
{
  /* No sub shape selection if main shape not selected */
  if(!myOkShape) {
    this->ResetStateOfDialog();
    QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_SELECT_FIRST"));
    return;
  }
  
  if(myShapeType ==TopAbs_SHAPE && myShape.ShapeType()==TopAbs_COMPOUND) {
    /* Select sub shapes mode not checked */
    myOkSelectSubMode = false;    
    GroupPoints->CheckButton1->setChecked( FALSE );
    //no meaning to allow user selection for type = shape
    //TODO - add another message
    //QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_SELECT_FIRST")) ;
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
  else {
    QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_NOT_FOR_VTK_VIEWER"));
    return;
  }

  if(myOkShape && myOkSelectSubMode) {
    /* local context is defined into the method */
    DisplayGUI* myDisplayGUI = new DisplayGUI();
    myDisplayGUI->PrepareSubShapeSelection(myShapeType, myLocalContextId);    
    myUseLocalContext = true;
    QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_SELECT_FACE"));
  }
  return;
}


//=================================================================================
// function : ComboTextChanged()
// purpose  : 
//=================================================================================
void EntityGUI_SubShapeDlg::ComboTextChanged()
{
  if(myOkShape)
    myShapeType = GroupPoints->ComboBox1->currentItem() + myShape.ShapeType() + 1;
  else
    myShapeType = GroupPoints->ComboBox1->currentItem();

  /* Select sub shapes mode not checked */
  GroupPoints->CheckButton1->setChecked(FALSE);
  myOkSelectSubMode = FALSE;

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


//=================================================================================
// function : NumberOfSubShapes()
// purpose  :
//=================================================================================
unsigned int EntityGUI_SubShapeDlg::NumberOfSubShapes(const TopoDS_Shape& S, const int shapeType)
{
  if(S.IsNull())
    return 0;
  
  unsigned int index = 0;
  TopExp_Explorer Exp(S, TopAbs_ShapeEnum(shapeType));
  TopTools_MapOfShape M;
  while(Exp.More()) {
    if(M.Add(Exp.Current()))
      index++;
    Exp.Next();
  }
  M.Clear();
  return index;
}
