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
//  File   : RepairGUI_SuppressHoleDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

using namespace std;
#include "RepairGUI_SuppressHoleDlg.h"

#include "DisplayGUI.h"

#include "QAD_RightFrame.h"
#include "OCCViewer_Viewer3d.h"

#include <TopExp_Explorer.hxx>

//=================================================================================
// class    : RepairGUI_SuppressHoleDlg()
// purpose  : Constructs a RepairGUI_SuppressHoleDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
RepairGUI_SuppressHoleDlg::RepairGUI_SuppressHoleDlg(QWidget* parent, const char* name, RepairGUI* theRepairGUI, SALOME_Selection* Sel, Handle(AIS_InteractiveContext) ic, bool modal, WFlags fl)
  :GEOMBase_Skeleton(parent, name, Sel, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  
  QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_DLG_SUPRESS_HOLE")));
  QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_DLG_SUPRESS_HOLE_FACE_SHELL")));
  QPixmap image2(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_SUPPRESSHOLE_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr(""));
  RadioButton1->setPixmap(image0);
  RadioButton2->setPixmap(image1);
  RadioButton3->close(TRUE);

  Group1 = new DlgRef_1Sel3Check_QTD(this, "Group1");
  Group1->GroupBox1->setTitle(tr(""));
  Group1->TextLabel1->setText(tr("GEOM_MAIN_OBJECT"));
  Group1->CheckButton1->setText(tr("GEOM_SUPPRESSHOLE_SELECTFACE"));
  Group1->CheckButton2->setText(tr("GEOM_SUPPRESSHOLE_SELECTWIRE"));
  Group1->CheckButton3->setText(tr("GEOM_SUPPRESSHOLE_SELECTFACE_END"));
  Group1->PushButton1->setPixmap(image2);

  Group2 = new DlgRef_1Sel1Check_QTD(this, "Group2");
  Group2->GroupBox1->setTitle(tr(""));
  Group2->TextLabel1->setText(tr("GEOM_SUPPRESSHOLE_FACE_SHELL"));
  Group2->CheckButton1->setText(tr("GEOM_SUPPRESSHOLE_SELECT_HOLES_ON_FACE"));
  Group2->PushButton1->setPixmap(image2);

  Layout1->addWidget(Group1, 1, 0);
  Layout1->addWidget(Group2, 1, 0);
  /***************************************************************/

  /* Initialisations */
  myRepairGUI = theRepairGUI;
  Init(ic);
}


//=================================================================================
// function : ~RepairGUI_SuppressHoleDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
RepairGUI_SuppressHoleDlg::~RepairGUI_SuppressHoleDlg()
{
  // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void RepairGUI_SuppressHoleDlg::Init(Handle (AIS_InteractiveContext) ic)
{
  /* init variables */
  myConstructorId = 0;
  myEditCurrentArgument = Group1->LineEdit1;

  myIC = ic;
  myUseLocalContext = myOkShape = myOkSelectFace = false;
  myLocalContextId = -1;

  myListOfIdFace = new GEOM::GEOM_Shape::ListOfSubShapeID;
  myListOfIdWire = new GEOM::GEOM_Shape::ListOfSubShapeID;
  myListOfIdEndFace = new GEOM::GEOM_Shape::ListOfSubShapeID;
  
  myListOfIdFace->length(0);
  myListOfIdWire->length(0);
  myListOfIdEndFace->length(0); 

  /* signals and slots connections */
  connect(buttonCancel, SIGNAL(clicked()), this, SLOT(ClickOnCancel()));
  connect(myGeomGUI, SIGNAL(SignalDeactivateActiveDialog()), this, SLOT(DeactivateActiveDialog()));
  connect(myGeomGUI, SIGNAL(SignalCloseAllDialogs()), this, SLOT(ClickOnCancel()));

  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));
  connect(GroupConstructors, SIGNAL(clicked(int)), this, SLOT(ConstructorsClicked(int)));

  connect(Group1->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(Group2->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(Group1->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(Group2->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(Group1->CheckButton1, SIGNAL(stateChanged(int)), this, SLOT(ActivateUserFaceSelection()));
  connect(Group1->CheckButton2, SIGNAL(stateChanged(int)), this, SLOT(ActivateUserWireSelection()));
  connect(Group1->CheckButton3, SIGNAL(stateChanged(int)), this, SLOT(ActivateUserEndFaceSelection()));
  connect(Group2->CheckButton1, SIGNAL(stateChanged(int)), this, SLOT(ActivateUserWiresOnFaceShellSelection()));
  
  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  /* displays Dialog */
  Group2->hide();
  Group1->show();
  this->show();

  return;
}


//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void RepairGUI_SuppressHoleDlg::ConstructorsClicked(int constructorId)
{
  myConstructorId = constructorId;
  disconnect(mySelection, 0, this, 0);
  myOkShape = false;

  if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC && myUseLocalContext) {
    myIC->CloseLocalContext(myLocalContextId);
    DisplayGUI* myDisplayGUI = new DisplayGUI();
    myDisplayGUI->OnDisplayAll(true);
    myUseLocalContext = false;
  }

  switch (constructorId)
    {
    case 0:
      {
	Group2->hide();
	resize(0, 0);
	Group1->show();

	myEditCurrentArgument = Group1->LineEdit1;
	Group1->LineEdit1->setText("");
	connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

	myOkSelectFace = false;
	this->ResetPartial();
	break;
      }
    case 1:
      {
	Group1->hide();
	resize(0, 0);
	Group2->show();

	myEditCurrentArgument = Group2->LineEdit1;
	Group2->LineEdit1->setText("");

	this->ResetPartial();
	connect ( mySelection, SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );
	this->ResetPartial();
	break;
      }
    }
 return;
}


//=================================================================================
// function : ClickOnOk()
// purpose  : Same than click on apply but close this dialog.
//=================================================================================
void RepairGUI_SuppressHoleDlg::ClickOnOk()
{
  this->ClickOnApply();
  this->ClickOnCancel();
  return;
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
void RepairGUI_SuppressHoleDlg::ClickOnApply()
{
  QAD_Application::getDesktop()->putInfo(tr(""));
  bool testResult = false;
  
  if(!myOkShape)
    return;

  switch (myConstructorId)
    {
    case 0: /* default constructor */
      {
	if(!myOkSelectFace)
	  return;
  
	if(Group1->CheckButton2->isChecked()) {
	  if(!Group1->CheckButton3->isChecked()) {
	    /* Call method to get sub shape selection of GEOM::WIRE */
	    bool aTest = this->GetIndexSubShapeSelected(myFace, int(TopAbs_WIRE), myListOfIdWire, myLocalContextId, myUseLocalContext);
	    
	    /* Display all objects so that next method using ic can memorize them */
	    DisplayGUI* myDisplayGUI = new DisplayGUI();
	    myDisplayGUI->OnDisplayAll(true);

	    if(!aTest || myListOfIdWire->length() != 1) {
	      Group1->CheckButton2->setChecked(FALSE);
	      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_ABORT")); 
	    }
	    else {
	      myListOfIdEndFace->length(0); /* no end face */	
	      QApplication::setOverrideCursor(Qt::waitCursor);
	      testResult = myRepairGUI->OnSuppressHole(myShapeIOR, myListOfIdFace, myListOfIdWire, myListOfIdEndFace);
	      QApplication::restoreOverrideCursor();
	    }
	  }
	  else { /* Group1->CheckButton3->isChecked() */
	    
	    /* Call method to get sub shape selection of END GEOM::FACE */
	    bool aTest = this->GetIndexSubShapeSelected(myShape, int(TopAbs_FACE), myListOfIdEndFace, myLocalContextId, myUseLocalContext);

	    /* Display all objects so that next method using ic can memorize them */
	    DisplayGUI* myDisplayGUI = new DisplayGUI();
	    myDisplayGUI->OnDisplayAll(true);

	    if(!aTest || myListOfIdEndFace->length() != 1) {
	      Group1->CheckButton3->setChecked(FALSE);
	      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_ABORT")); 
	    }
	    else {	
	      QApplication::setOverrideCursor(Qt::waitCursor);
	      testResult = myRepairGUI->OnSuppressHole(myShapeIOR, myListOfIdFace, myListOfIdWire, myListOfIdEndFace); 
	      QApplication::restoreOverrideCursor();
	    }     
	  }
	}
	break;
      }
    case 1:  /* second constructor */
      {
	if(Group2->CheckButton1->isChecked()) {
	  /* Call method to get sub shape selection of one or more GEOM::WIRE(s) on a face or a shell */
	  bool aTest = this->GetIndexSubShapeSelected(myShape, int(TopAbs_WIRE), myListOfIdWire, myLocalContextId, myUseLocalContext);
	  
	  /* Display all objects so that next method using ic can memorize them */
	  DisplayGUI* myDisplayGUI = new DisplayGUI();
	  myDisplayGUI->OnDisplayAll(true);
	  
	  if(!aTest || myListOfIdWire->length() < 1) {
	    Group2->CheckButton1->setChecked(FALSE);
	    QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_ABORT"));
	  }
	  else {
	    QApplication::setOverrideCursor(Qt::waitCursor);
	    testResult = myRepairGUI->OnSuppressHolesInFaceOrShell(myShapeIOR, myListOfIdWire);
	    QApplication::restoreOverrideCursor();
	  }
	}
	break;
      }
    }
  

  if(!testResult)
    QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_ABORT"));
  else
    QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_DONE"));
  
  /* Reset arguments to allow a new selection */
  this->ResetStateOfDialog();
  return;
}


//=================================================================================
// function : ClickOnCancel()
// purpose  :
//=================================================================================
void RepairGUI_SuppressHoleDlg::ClickOnCancel()
{
  this->ResetStateOfDialog();
  GEOMBase_Skeleton::ClickOnCancel();
  return;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//          : used only by SelectButtonC1A1 and SelectButtonC2A1
//=================================================================================
void RepairGUI_SuppressHoleDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("");
  this->ResetStateOfDialog();
  QString aString = ""; /* name of selection */

  int nbSel = myGeomBase->GetNameOfSelectedIObjects(mySelection, aString);
  if(nbSel != 1)
    return;
  
  /* nbSel == 1 */
  TopoDS_Shape S;
  Handle(SALOME_InteractiveObject) IO = mySelection->firstIObject();

  if(!myGeomBase->GetTopoFromSelection(mySelection, S))
    return;
  
  if(S.IsNull() || S.ShapeType() == TopAbs_VERTEX || S.ShapeType() == TopAbs_EDGE || S.ShapeType() == TopAbs_WIRE)
    return;
  
  /* Test the exact type of topology to suppress faces into.         */
  /* For the second constructor a face or shell selection is needed  */
  if(myConstructorId == 0 || (myConstructorId == 1 && (S.ShapeType() == TopAbs_FACE || S.ShapeType() == TopAbs_SHELL))) {
    if(IO->IsInstance(STANDARD_TYPE(GEOM_InteractiveObject))) {
      Handle(GEOM_InteractiveObject) GIObject = Handle(GEOM_InteractiveObject)::DownCast(IO);

      /* The Geom IOR string of selection */
      myShapeIOR = GIObject->getIOR();
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
void RepairGUI_SuppressHoleDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();  

  if(send == Group1->PushButton1) {
    Group1->LineEdit1->setFocus();
    myEditCurrentArgument = Group1->LineEdit1;
  }
  else if(send == Group2->PushButton1) {
    Group2->LineEdit1->setFocus();
    myEditCurrentArgument = Group2->LineEdit1;
  }
  this->SelectionIntoArgument();
  return;
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void RepairGUI_SuppressHoleDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if(send == Group1->LineEdit1)
    myEditCurrentArgument = Group1->LineEdit1;
  else if (send == Group2->LineEdit1)
    myEditCurrentArgument = Group2->LineEdit1;
  else
    return;

  GEOMBase_Skeleton::LineEditReturnPressed();
  return;
}


//=================================================================================
// function : DeactivateActiveDialog()
// purpose  :
//=================================================================================
void RepairGUI_SuppressHoleDlg::DeactivateActiveDialog()
{
  this->ResetStateOfDialog();
  GEOMBase_Skeleton::DeactivateActiveDialog();
  return;
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void RepairGUI_SuppressHoleDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
  return;
}


//=================================================================================
// function : enterEvent()
// purpose  : Mouse enter onto the dialog to activate it
//=================================================================================
void RepairGUI_SuppressHoleDlg::enterEvent(QEvent* e)
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
void RepairGUI_SuppressHoleDlg::closeEvent(QCloseEvent* e)
{
  /* same than click on cancel button */
  this->ClickOnCancel();
  return;
}


//=================================================================================
// function : ActivateUserFaceSelection()
// purpose  : Called when CheckBox1 state has changed. (Face selection is ckecked)
//          : Be careful user must first select a face then a wire !
//=================================================================================
void RepairGUI_SuppressHoleDlg::ActivateUserFaceSelection()
{
  if(!this->myOkShape) {
    this->ResetStateOfDialog();
    QAD_Application::getDesktop()->putInfo(tr("GEOM_MAIN_OBJECT"));
    return;
  }
  
  if(Group1->CheckButton1->isChecked()) {
    /* local context is opened into the method : Prepare GEOM::FACE sub selection */
    DisplayGUI* myDisplayGUI = new DisplayGUI();
    myDisplayGUI->PrepareSubShapeSelection(int(TopAbs_FACE), myLocalContextId);    
    myUseLocalContext = true;
    QAD_Application::getDesktop()->putInfo(tr("GEOM_SUPPRESSHOLE_SELECTFACE"));
  } 
  else
    this->ResetPartial();

  return;
}


//=================================================================================
// function : ActivateUserWireSelection()
// purpose  : Called when CheckBox2 state has changed. (Wire selection is ckecked)
//          : Be careful user must first select a face then a wire !
//=================================================================================
void RepairGUI_SuppressHoleDlg::ActivateUserWireSelection()
{
  
  if(!myOkShape) {
    this->ResetStateOfDialog();
    QAD_Application::getDesktop()->putInfo(tr("GEOM_MAIN_OBJECT"));
    return;
  } 

  if(Group1->CheckButton1->isChecked()) {
    /* Get sub shape selection GEOM::FACE : local context is closed */    
    bool aTest = this->GetIndexSubShapeSelected(myShape, int(TopAbs_FACE), myListOfIdFace, myLocalContextId, myUseLocalContext);
    
    /* Display all objects so that next method using ic can memorize them */
    DisplayGUI* myDisplayGUI = new DisplayGUI();
    myDisplayGUI->OnDisplayAll(true);
    if(!aTest || myListOfIdFace->length() != 1) {
      Group1->CheckButton1->setChecked(FALSE);
      myOkSelectFace = false;
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_ABORT")); 
    }
    else
      myOkSelectFace = true;
  }
  else {
    this->ResetPartial();
    return;
  }
  

  if(Group1->CheckButton2->isChecked()) {
    /* Get the face selection */
    myFace = FaceFromList(myShape, myListOfIdFace);    
    /* Local context is opened into the method : Prepare GEOM::WIRE sub selection into a face */
    DisplayGUI* myDisplayGUI = new DisplayGUI();
    myDisplayGUI->PrepareSubShapeSelectionArgumentShape(myFace, int(TopAbs_WIRE), myLocalContextId);    
    myUseLocalContext = true;
    QAD_Application::getDesktop()->putInfo(tr("GEOM_SUPPRESSHOLE_SELECTWIRE"));
  }
  else {
    this->ResetPartial();
  }
  return;
}


//=================================================================================
// function : ActivateUserlEndFaceSelection()
// purpose  : Called when CheckBox3 state has changed. ( Optional End Face selection )
//          : Be careful user must first select a face then a wire then this optional end face !
//=================================================================================
void RepairGUI_SuppressHoleDlg::ActivateUserEndFaceSelection()
{
  
  if(!myOkShape) {
    this->ResetStateOfDialog();
    QAD_Application::getDesktop()->putInfo(tr("GEOM_MAIN_OBJECT"));
    return;
  }
  
  if(Group1->CheckButton2->isChecked()) {
    /* Call method to get sub shape selection for the GEOM::WIRE into myFace : local context is closed */
    bool aTest = this->GetIndexSubShapeSelected(myFace, int(TopAbs_WIRE), myListOfIdWire, myLocalContextId, myUseLocalContext);
    
    /* Display all objects so that next method using ic can memorize them */
    DisplayGUI* myDisplayGUI = new DisplayGUI();
    myDisplayGUI->OnDisplayAll(true);
    
    if(!aTest || myListOfIdWire->length() != 1) {
      Group1->CheckButton2->setChecked(FALSE);
      Group1->CheckButton3->setChecked(FALSE);
      QAD_Application::getDesktop()->putInfo(tr("GEOM_PRP_ABORT"));     
      return;
    }
  }
  else {
    this->ResetPartial();
    return;
  }
  
  
  if(Group1->CheckButton3->isChecked()) {    
    /* Local context is opened into the method : prepare GEOM::FACE(end) into myShape sub selection */
    DisplayGUI* myDisplayGUI = new DisplayGUI();
    myDisplayGUI->PrepareSubShapeSelectionArgumentShape(myShape, int(TopAbs_FACE), myLocalContextId);    
    myUseLocalContext = true;
    QAD_Application::getDesktop()->putInfo(tr("GEOM_SUPPRESSHOLE_SELECTFACE_END"));
  }
  else
    this->ResetPartial();
  return;
}


//=================================================================================
// function : ActivateUserWiresOnFaceShellSelection()
// purpose  : Called when CheckBoxC2_1 state has changed.
//          : Only for second constructor !
//          : Prepare selection for wire(s) on main object that is a face or a shell
//=================================================================================
void RepairGUI_SuppressHoleDlg::ActivateUserWiresOnFaceShellSelection()
{
  
  if(!myOkShape) {
    this->ResetStateOfDialog();
    QAD_Application::getDesktop()->putInfo(tr("GEOM_MAIN_OBJECT"));
    return;
  }
  
  if(Group2->CheckButton1->isChecked()) {    
    /* Local context is opened to prepare GEOM::WIRE(S) selection into 'myShape' that is a (main) face */
    DisplayGUI* myDisplayGUI = new DisplayGUI();
    myDisplayGUI->PrepareSubShapeSelectionArgumentShape(myShape, int(TopAbs_WIRE), myLocalContextId);    
    myUseLocalContext = true;
    QAD_Application::getDesktop()->putInfo(tr("GEOM_SUPPRESSHOLE_SELECT_HOLES_ON_FACE"));
  }
  else
    this->ResetPartial();
  return;
}


//=================================================================================
// function : FaceFromList()
// purpose  : Return the face (selected by user) that is a sub shape of 'aShape'
//          : and which unique index is in 'ListOfSub'.
//          : This allows opening a local context with this face loaded.
//          : See : myGeomBase->PrepareSubShapeSelectionArgumentShape(...)
//=================================================================================
TopoDS_Shape RepairGUI_SuppressHoleDlg::FaceFromList(const TopoDS_Shape& aShape,
						     const GEOM::GEOM_Shape::ListOfSubShapeID& ListOfSub)
{
  TopoDS_Shape tds;
  tds.Nullify();
  if(ListOfSub.length() != 1 || aShape.IsNull())
    return tds;
  
  int i = ListOfSub[0];
  TopExp_Explorer exp;
  int j = 1; 
  for(exp.Init(aShape, TopAbs_FACE); exp.More(); exp.Next()) {
    if(j == i)
      return exp.Current();    
    j++;
  }
  return tds;
}


//=================================================================================
// function : ResetStateOfDialog()
// purpose  : Completely reset the state of method including local context
//=================================================================================
void RepairGUI_SuppressHoleDlg::ResetStateOfDialog()
{
  myOkShape = false;
  myEditCurrentArgument->setText("");
  QApplication::restoreOverrideCursor();
  
  /* Partial reset and more ...*/
  this->ResetPartial();

  return;
}


//=================================================================================
// function : ResetPartial()
// purpose  : Partially reset to keep only main selection
//=================================================================================
void RepairGUI_SuppressHoleDlg::ResetPartial()
{
  /* Select sub shape modes not checked */
  myOkSelectFace = false ;
  Group1->CheckButton1->setChecked(FALSE);
  Group1->CheckButton2->setChecked(FALSE);
  Group1->CheckButton3->setChecked(FALSE);
  Group2->CheckButton1->setChecked(FALSE);
  
  myListOfIdFace->length(0);
  myListOfIdWire->length(0);
  myListOfIdEndFace->length(0);

  /* Close its local contact if opened */
  if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC && myUseLocalContext) {
    myIC->CloseLocalContext(myLocalContextId);
    myUseLocalContext = false;
    DisplayGUI* myDisplayGUI = new DisplayGUI();
    myDisplayGUI->OnDisplayAll(true);
  }
  return;
}


//=====================================================================================
// function : GetIndexSubShapeSelected()
// purpose  : Define a ListOfID of sub shapes selected in ShapeTopo with SubShapeType
//          : Method used by Dialogs
//=====================================================================================
bool RepairGUI_SuppressHoleDlg::GetIndexSubShapeSelected(const TopoDS_Shape& ShapeTopo, const int SubShapeType, GEOM::GEOM_Shape::ListOfSubShapeID& ListOfID, Standard_Integer& aLocalContextId, bool& myUseLocalContext)
{
  //* Test the type of viewer */
  if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() > VIEW_OCC)
    return false;
  
  OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
  Handle(AIS_InteractiveContext) ic = v3d->getAISContext();

  ic->InitSelected();
  int nbSelected = ic->NbSelected();
  ListOfID.length(nbSelected);
  
  //***************** DEFINE INDEX OF EACH SELECTION *********************//
  int i = 0;
  ic->InitSelected(); /* to restart */
  while(ic->MoreSelected()) {
    /* Find index of sub shape into main shape */
    TopExp_Explorer Exp (ShapeTopo, TopAbs_ShapeEnum(SubShapeType));
    int index = 1;
    bool found = false;
    while( Exp.More()) {
      if((Exp.Current()).IsSame(ic->SelectedShape())) {
	found = true;
	break;
      }
      index++;
      Exp.Next();
    }
    if(!found) {
      /* Manage local context from DialogBox */
      ic->CloseLocalContext(aLocalContextId);
      myUseLocalContext = false;
      return false;
    }
    ListOfID[i] = index;
    i++;
    ic->NextSelected();
  }
    //***************** END  *********************//

  /* Manage local context from DialogBox */
  ic->CloseLocalContext(aLocalContextId);
  myUseLocalContext = false;
  
  return true;
}
