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
//  File   : BasicGUI_ArcDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

using namespace std;
#include "BasicGUI_ArcDlg.h"

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <GC_MakeArcOfCircle.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Precision.hxx>

//=================================================================================
// class    : BasicGUI_ArcDlg()
// purpose  : Constructs a BasicGUI_ArcDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
BasicGUI_ArcDlg::BasicGUI_ArcDlg(QWidget* parent, const char* name, BasicGUI* theBasicGUI, SALOME_Selection* Sel, bool modal, WFlags fl)
  :GEOMBase_Skeleton(parent, name, Sel, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_DLG_ARC")));
  QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_ARC_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_ARC"));
  RadioButton1->setPixmap(image0);
  RadioButton2->close(TRUE);
  RadioButton3->close(TRUE);

  GroupPoints = new DlgRef_3Sel_QTD(this, "GroupPoints");
  GroupPoints->GroupBox1->setTitle(tr("GEOM_POINTS"));
  GroupPoints->TextLabel1->setText(tr("GEOM_POINT_I").arg("1"));
  GroupPoints->TextLabel2->setText(tr("GEOM_POINT_I").arg("2"));
  GroupPoints->TextLabel3->setText(tr("GEOM_POINT_I").arg("3"));
  GroupPoints->PushButton1->setPixmap(image1);
  GroupPoints->PushButton2->setPixmap(image1);
  GroupPoints->PushButton3->setPixmap(image1);

  Layout1->addWidget(GroupPoints, 1, 0);
  /***************************************************************/

  /* Initialisations */
  myBasicGUI = theBasicGUI;
  Init();
}


//=================================================================================
// function : ~BasicGUI_ArcDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
BasicGUI_ArcDlg::~BasicGUI_ArcDlg()
{
    // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BasicGUI_ArcDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupPoints->LineEdit1;
  myOkPoint1 = myOkPoint2 = myOkPoint3 = false;

  /*  Vertices Filter for all arguments */
  myVertexFilter = new GEOM_ShapeTypeFilter(TopAbs_VERTEX, myGeom);
  mySelection->AddFilter(myVertexFilter);

  /* signals and slots connections */
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(GroupPoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->PushButton3, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(GroupPoints->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupPoints->LineEdit2, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupPoints->LineEdit3, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

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
void BasicGUI_ArcDlg::ClickOnOk()
{
  this->ClickOnApply();
  ClickOnCancel();
  return;
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
void BasicGUI_ArcDlg::ClickOnApply()
{
  QAD_Application::getDesktop()->putInfo(tr(""));
  if (mySimulationTopoDs.IsNull())
    return;
  myGeomBase->EraseSimulationShape();
  mySimulationTopoDs.Nullify();

  if(myOkPoint1 && myOkPoint2 && myOkPoint3) 
    myBasicGUI->MakeArcAndDisplay(myPoint1, myPoint2, myPoint3);
  return;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void BasicGUI_ArcDlg::SelectionIntoArgument()
{
  myGeomBase->EraseSimulationShape();
  mySimulationTopoDs.Nullify();
  myEditCurrentArgument->setText("");
  QString aString = ""; /* name of selection */

  int nbSel = myGeomBase->GetNameOfSelectedIObjects(mySelection, aString);
  if (nbSel != 1) {
    if (myEditCurrentArgument == GroupPoints->LineEdit1)
      myOkPoint1 = false;
    else if(myEditCurrentArgument == GroupPoints->LineEdit2)
      myOkPoint2 = false;
    else if(myEditCurrentArgument == GroupPoints->LineEdit3)
      myOkPoint3 = false;
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
  else if(myEditCurrentArgument == GroupPoints->LineEdit3 && myGeomBase->VertexToPoint(S, myPoint3)) {
    myEditCurrentArgument->setText(aString);
    myOkPoint3 = true;
  }

  if(myOkPoint1 && myOkPoint2 && myOkPoint3)
    this->MakeArcSimulationAndDisplay();
  return;
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BasicGUI_ArcDlg::SetEditCurrentArgument()
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
  else if(send == GroupPoints->PushButton3) {
    GroupPoints->LineEdit3->setFocus();
    myEditCurrentArgument = GroupPoints->LineEdit3;
  }
  mySelection->AddFilter(myVertexFilter);
  this->SelectionIntoArgument();

  return;
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void BasicGUI_ArcDlg::LineEditReturnPressed()
{ 
  QLineEdit* send = (QLineEdit*)sender();
  if(send == GroupPoints->LineEdit1)
    myEditCurrentArgument = GroupPoints->LineEdit1;
  else if(send == GroupPoints->LineEdit2)
    myEditCurrentArgument = GroupPoints->LineEdit2;
  else if(send == GroupPoints->LineEdit3)
    myEditCurrentArgument = GroupPoints->LineEdit3;
  else
    return;

  GEOMBase_Skeleton::LineEditReturnPressed();
  return;
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void BasicGUI_ArcDlg::ActivateThisDialog()
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
void BasicGUI_ArcDlg::enterEvent(QEvent* e)
{
  if(GroupConstructors->isEnabled())
    return;
  this->ActivateThisDialog();
  return;
}


//=================================================================================
// function : MakeArcSimulationAndDisplay()
// purpose  :
//=================================================================================
void BasicGUI_ArcDlg::MakeArcSimulationAndDisplay() 
{
  myGeomBase->EraseSimulationShape();
  mySimulationTopoDs.Nullify();

  try {
    if(myPoint2.IsEqual(myPoint1, Precision::Confusion()) || 
       myPoint2.IsEqual(myPoint3, Precision::Confusion())) {
      myEditCurrentArgument->setText("");
      return;
    }

    gp_Vec v1(myPoint2, myPoint1);
    gp_Vec v2(myPoint2, myPoint3);
    if(v1.IsParallel(v2, Precision::Angular())) {
      myEditCurrentArgument->setText("");
      return;
    }
    
    GC_MakeArcOfCircle Arc(myPoint1, myPoint2, myPoint3);
    if(Arc.IsDone()) {
      Handle(Geom_TrimmedCurve) curve = Arc.Value();
      mySimulationTopoDs = BRepBuilderAPI_MakeEdge(curve).Shape();
      myGeomBase->DisplaySimulationShape(mySimulationTopoDs);
    }
  }
  catch(Standard_Failure) {
    MESSAGE("Exception catched in MakeArcSimulationAndDisplay");
  }
  return;
}
