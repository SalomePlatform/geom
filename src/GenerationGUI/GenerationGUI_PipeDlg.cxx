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
//  File   : GenerationGUI_PipeDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

using namespace std;
#include "GenerationGUI_PipeDlg.h"

#include <TopoDS_Edge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepOffsetAPI_MakePipe.hxx>
#if OCC_VERSION_MAJOR >= 5
#include <BRepAlgo.hxx>
#else
#include <BRepAlgoAPI.hxx>
#endif

//=================================================================================
// class    : GenerationGUI_PipeDlg()
// purpose  : Constructs a GenerationGUI_PipeDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
GenerationGUI_PipeDlg::GenerationGUI_PipeDlg(QWidget* parent, const char* name, GenerationGUI* theGenerationGUI, SALOME_Selection* Sel, bool modal, WFlags fl)
  :GEOMBase_Skeleton(parent, name, Sel, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_DLG_PIPE")));
  QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_PIPE_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_PIPE"));
  RadioButton1->setPixmap(image0);
  RadioButton2->close(TRUE);
  RadioButton3->close(TRUE);

  GroupPoints = new DlgRef_2Sel_QTD(this, "GroupPoints");
  GroupPoints->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  GroupPoints->TextLabel1->setText(tr("GEOM_BASE_OBJECT"));
  GroupPoints->TextLabel2->setText(tr("GEOM_PATH_OBJECT"));
  GroupPoints->PushButton1->setPixmap(image1);
  GroupPoints->PushButton2->setPixmap(image1);

  Layout1->addWidget(GroupPoints, 1, 0);
  /***************************************************************/

  /* Initialisations */
  myGenerationGUI = theGenerationGUI;
  Init();
}


//=================================================================================
// function : ~GenerationGUI_PipeDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
GenerationGUI_PipeDlg::~GenerationGUI_PipeDlg()
{
  /* no need to delete child widgets, Qt does it all for us */
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void GenerationGUI_PipeDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupPoints->LineEdit1;

  myOkShape1 = myOkShape2 = false;

  /* signals and slots connections */
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(GroupPoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(GroupPoints->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupPoints->LineEdit2, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  
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
void GenerationGUI_PipeDlg::ClickOnOk()
{
  this->ClickOnApply();
  ClickOnCancel();
  return;
}


//=================================================================================
// function : ClickOnAply()
// purpose  :
//=================================================================================
void GenerationGUI_PipeDlg::ClickOnApply()
{
  QAD_Application::getDesktop()->putInfo(tr(""));
  if (mySimulationTopoDs.IsNull())
    return;
  myGeomBase->EraseSimulationShape();
  mySimulationTopoDs.Nullify();

  if(myOkShape1 && myOkShape2)	  
    myGenerationGUI->MakePipeAndDisplay(myGeomShape2 ,myGeomShape1);

  return;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection has changed
//=================================================================================
void GenerationGUI_PipeDlg::SelectionIntoArgument()
{
  myGeomBase->EraseSimulationShape();
  mySimulationTopoDs.Nullify();
  myEditCurrentArgument->setText("");
  QString aString = ""; /* name of selection */
  
  int nbSel = myGeomBase->GetNameOfSelectedIObjects(mySelection, aString);
  if(nbSel != 1) {
    if(myEditCurrentArgument == GroupPoints->LineEdit1)
      myOkShape1 = false;
    else if(myEditCurrentArgument == GroupPoints->LineEdit2)
      myOkShape2 = false;
    return;
  }
  
  // nbSel == 1
  TopoDS_Shape S; 
  Standard_Boolean testResult;
  Handle(SALOME_InteractiveObject) IO = mySelection->firstIObject();
  if(!myGeomBase->GetTopoFromSelection(mySelection, S))
    return;
  
  if(myEditCurrentArgument == GroupPoints->LineEdit1) {
    myOkShape1 = false;
    if(S.ShapeType() != TopAbs_COMPSOLID && S.ShapeType() != TopAbs_COMPOUND && S.ShapeType() != TopAbs_SOLID && S.ShapeType() != TopAbs_SHAPE) {
      myGeomShape1 = myGeomBase->ConvertIOinGEOMShape(IO, testResult);
      if(!testResult)
	return;
      myEditCurrentArgument->setText(aString);
      myOkShape1 = true;
      myShape1 = S;
    }
  }
  else if(myEditCurrentArgument == GroupPoints->LineEdit2) {
    myOkShape2 = false;
    if(S.ShapeType() != TopAbs_COMPSOLID && S.ShapeType() != TopAbs_COMPOUND && S.ShapeType() != TopAbs_SOLID && S.ShapeType() != TopAbs_SHAPE && S.ShapeType() != TopAbs_VERTEX) {
      myGeomShape2 = myGeomBase->ConvertIOinGEOMShape(IO, testResult);
      if(!testResult)
	return;
      myEditCurrentArgument->setText(aString);
      myOkShape2 = true;
      myShape2 = S;
    }
  }

  if(myOkShape1 && myOkShape2)
    this->MakePipeSimulationAndDisplay();
  return;
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void GenerationGUI_PipeDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  if(send == GroupPoints->PushButton1) {
    GroupPoints->LineEdit1->setFocus();
    myEditCurrentArgument = GroupPoints->LineEdit1;
  }
  else if(send == GroupPoints->PushButton2) {
    GroupPoints->LineEdit2->setFocus();
    myEditCurrentArgument = GroupPoints->LineEdit2;
  }
  this->SelectionIntoArgument();

  return;
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void GenerationGUI_PipeDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if(send == GroupPoints->LineEdit1)
    myEditCurrentArgument = GroupPoints->LineEdit1;
  else if (send == GroupPoints->LineEdit2)
    myEditCurrentArgument = GroupPoints->LineEdit2;
  else
    return;

  GEOMBase_Skeleton::LineEditReturnPressed();
  return;
}


//=================================================================================
// function : enterEvent()
// purpose  : when mouse enter onto the QWidget
//=================================================================================
void GenerationGUI_PipeDlg::enterEvent(QEvent* e)
{
  if (GroupConstructors->isEnabled())
    return;
  this->ActivateThisDialog();
  return;
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void GenerationGUI_PipeDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
  if(!mySimulationTopoDs.IsNull())
    myGeomBase->DisplaySimulationShape(mySimulationTopoDs);
  return;
}


//=================================================================================
// function : MakeMirrorSimulationAndDisplay()
// purpose  : S1 is a shape and S2 a mirror.
//=================================================================================
void GenerationGUI_PipeDlg::MakePipeSimulationAndDisplay()
{
  myGeomBase->EraseSimulationShape();
  mySimulationTopoDs.Nullify();
  
  try {
    TopoDS_Wire aWire;

    if(myShape2.ShapeType() == TopAbs_WIRE) 
      aWire = TopoDS::Wire(myShape2);
    else if(myShape2.ShapeType() == TopAbs_EDGE) {
      TopoDS_Edge aEdge = TopoDS::Edge(myShape2);
      aWire = BRepBuilderAPI_MakeWire(aEdge);
    }

    TopoDS_Shape tds = BRepOffsetAPI_MakePipe(aWire,myShape1);
#if OCC_VERSION_MAJOR >= 5
    if(BRepAlgo::IsValid(tds)) 
#else
    if(BRepAlgoAPI::IsValid(tds)) 
#endif
    {
      //Draw Pipe
      mySimulationTopoDs = tds;
      myGeomBase->DisplaySimulationShape(mySimulationTopoDs); 
    }
  }
  catch(Standard_Failure) {
    MESSAGE("Exception catched in MakePipeSimulationAndDisplay");
    return;
  }
  return;
}
