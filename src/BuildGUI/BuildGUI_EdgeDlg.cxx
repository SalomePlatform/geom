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
//  File   : BuildGUI_EdgeDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

using namespace std;
#include "BuildGUI_EdgeDlg.h"

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <Precision.hxx>

//=================================================================================
// class    : BuildGUI_EdgeDlg()
// purpose  : Constructs a BuildGUI_EdgeDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
BuildGUI_EdgeDlg::BuildGUI_EdgeDlg(QWidget* parent, const char* name, BuildGUI* theBuildGUI, SALOME_Selection* Sel, bool modal, WFlags fl)
  :GEOMBase_Skeleton(parent, name, Sel, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_DLG_BUILD_EDGE")));
  QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_EDGE_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_EDGE"));
  RadioButton1->setPixmap(image0);
  RadioButton2->close(TRUE);
  RadioButton3->close(TRUE);

  GroupPoints = new DlgRef_2Sel_QTD(this, "GroupPoints");
  GroupPoints->GroupBox1->setTitle(tr("GEOM_POINTS"));
  GroupPoints->TextLabel1->setText(tr("GEOM_POINT_I").arg("1"));
  GroupPoints->TextLabel2->setText(tr("GEOM_POINT_I").arg("2"));
  GroupPoints->PushButton1->setPixmap(image1);
  GroupPoints->PushButton2->setPixmap(image1);

  Layout1->addWidget(GroupPoints, 1, 0);
  /***************************************************************/

  /* Initialisations */
  myBuildGUI = theBuildGUI;
  Init();
}


//=================================================================================
// function : ~BuildGUI_EdgeDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
BuildGUI_EdgeDlg::~BuildGUI_EdgeDlg()
{
    // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BuildGUI_EdgeDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupPoints->LineEdit1;

  myPoint1.SetCoord(0.0, 0.0, 0.0);
  myPoint2.SetCoord(0.0, 0.0, 0.0);
  myOkPoint1 = myOkPoint2 = false;

  /* first filter used */
  myVertexFilter = new GEOM_ShapeTypeFilter(TopAbs_VERTEX, myGeom);
  mySelection->AddFilter(myVertexFilter);

  /* signals and slots connections */
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(GroupPoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(GroupPoints->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupPoints->LineEdit2, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  
  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument())) ;

  /* displays Dialog */
  GroupPoints->show();
  this->show();

  return;
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void BuildGUI_EdgeDlg::ClickOnOk()
{
  this->ClickOnApply();
  ClickOnCancel();
  return;
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
void BuildGUI_EdgeDlg::ClickOnApply()
{
  QAD_Application::getDesktop()->putInfo(tr(""));
  if (mySimulationTopoDs.IsNull())
    return;
  myGeomBase->EraseSimulationShape();
  mySimulationTopoDs.Nullify();

  if(myOkPoint1 && myOkPoint2) 
    myBuildGUI->MakeLinearEdgeAndDisplay(myPoint1, myPoint2);
  return;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void BuildGUI_EdgeDlg::SelectionIntoArgument()
{
  myGeomBase->EraseSimulationShape();
  mySimulationTopoDs.Nullify();
  myEditCurrentArgument->setText("");
  QString aString = ""; /* name of selection */
  
  int nbSel = myGeomBase->GetNameOfSelectedIObjects(mySelection, aString);
  if(nbSel != 1) {
    if(myEditCurrentArgument == GroupPoints->LineEdit1)
      myOkPoint1 = false;
    else if(myEditCurrentArgument == GroupPoints->LineEdit2)
      myOkPoint2 = false;
    return;
  }
  
  // nbSel == 1
  TopoDS_Shape S; 
  if(!myGeomBase->GetTopoFromSelection(mySelection, S))
    return;
  
  if(myEditCurrentArgument == GroupPoints->LineEdit1 && myGeomBase->VertexToPoint(S, myPoint1)) {
    myEditCurrentArgument->setText(aString);
    myOkPoint1 = true;
  }
  else if(myEditCurrentArgument == GroupPoints->LineEdit2 && myGeomBase->VertexToPoint(S, myPoint2)) {
    myEditCurrentArgument->setText(aString);
    myOkPoint2 = true;
  }

  if(myOkPoint1 && myOkPoint2 && myPoint1.Distance(myPoint2) > Precision::Confusion())
    this->MakeEdgeSimulationAndDisplay();
  return;
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void BuildGUI_EdgeDlg::LineEditReturnPressed()
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
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BuildGUI_EdgeDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();
  mySelection->ClearFilters();

  if(send == GroupPoints->PushButton1) {
    GroupPoints->LineEdit1->setFocus();
    myEditCurrentArgument = GroupPoints->LineEdit1;
  }
  else if(send == GroupPoints->PushButton2) {
    GroupPoints->LineEdit2->setFocus();
    myEditCurrentArgument = GroupPoints->LineEdit2;
  }
  mySelection->AddFilter(myVertexFilter);
  this->SelectionIntoArgument();

  return;
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void BuildGUI_EdgeDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
  mySelection->AddFilter(myVertexFilter);
  if(!mySimulationTopoDs.IsNull())
    myGeomBase->DisplaySimulationShape(mySimulationTopoDs);
  return;
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void BuildGUI_EdgeDlg::enterEvent(QEvent* e)
{
  if (GroupConstructors->isEnabled())
    return;
  this->ActivateThisDialog();
  return;
}


//=================================================================================
// function : MakeMirrorSimulationAndDisplay()
// purpose  : S1 is a shape and S2 a mirror.
//=================================================================================
void BuildGUI_EdgeDlg::MakeEdgeSimulationAndDisplay()
{
  myGeomBase->EraseSimulationShape();
  mySimulationTopoDs.Nullify();
  
  try {
    mySimulationTopoDs = BRepBuilderAPI_MakeEdge(myPoint1, myPoint2).Shape();
    myGeomBase->DisplaySimulationShape(mySimulationTopoDs); 
  }
  catch(Standard_Failure) {
    MESSAGE("Exception catched in MakeEdgeSimulationAndDisplay");
    return;
  }
  return;
}
