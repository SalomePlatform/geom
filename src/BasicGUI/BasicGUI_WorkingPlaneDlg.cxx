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
//  File   : BasicGUI_WorkingPlaneDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM

using namespace std;
#include "BasicGUI_WorkingPlaneDlg.h"

#include <BRepAdaptor_Surface.hxx>
#include <gp_Pln.hxx>
#include <gp_Dir.hxx>
#include <gp_Ax1.hxx>

//=================================================================================
// class    : BasicGUI_WorkingPlaneDlg()
// purpose  : Constructs a BasicGUI_WorkingPlaneDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
BasicGUI_WorkingPlaneDlg::BasicGUI_WorkingPlaneDlg(QWidget* parent, const char* name, BasicGUI* theBasicGUI, SALOME_Selection* Sel, bool modal, WFlags fl)
  :GEOMBase_Skeleton(parent, name, Sel, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_DLG_WPLANE_FACE")));
  QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_WPLANE_TITLE"));
    
  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_WPLANE"));
  RadioButton1->setPixmap(image0);
  RadioButton2->close(TRUE);
  RadioButton3->close(TRUE);

  GroupWPlane = new DlgRef_1Sel_QTD(this, "GroupWPlane");
  GroupWPlane->GroupBox1->setTitle(tr("GEOM_WPLANE_FACE"));
  GroupWPlane->TextLabel1->setText(tr("GEOM_SELECTION"));
  GroupWPlane->PushButton1->setPixmap(image1);

  Layout1->addWidget(GroupWPlane, 1, 0);
  /***************************************************************/

  /* Initialisation */
  myBasicGUI = theBasicGUI;
  Init();
}


//=================================================================================
// function : ~BasicGUI_WorkingPlaneDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
BasicGUI_WorkingPlaneDlg::~BasicGUI_WorkingPlaneDlg()
{
    // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BasicGUI_WorkingPlaneDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupWPlane->LineEdit1;

  myOkPlane = false;

  /* Filter definition */
  myFaceFilter = new GEOM_FaceFilter(StdSelect_Plane, myGeom);
  mySelection->AddFilter(myFaceFilter); /* filter for next selection */

   /* signals and slots connections */
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(GroupWPlane->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupWPlane->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  /* displays Dialog */
  GroupWPlane->show();
  this->show();

  return;
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void BasicGUI_WorkingPlaneDlg::ClickOnOk()
{
  this->ClickOnApply();
  ClickOnCancel();
  return;
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
void BasicGUI_WorkingPlaneDlg::ClickOnApply()
{
  buttonApply->setFocus();
  QAD_Application::getDesktop()->putInfo(tr(""));
  if(myOkPlane)
    myBasicGUI->MakeWorkingPlane(myLoc, myDir);

  return;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void BasicGUI_WorkingPlaneDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("");
  QString aString = "";

  int nbSel = myGeomBase->GetNameOfSelectedIObjects(mySelection, aString);
  if(nbSel != 1) {
    myOkPlane = false;
    return;
  }
  
  /*  nbSel == 1  */ 
  TopoDS_Shape S;
  if(!myGeomBase->GetTopoFromSelection(mySelection, S))
    return;
   
  BRepAdaptor_Surface surf(TopoDS::Face(S));
  gp_Pln Plane = surf.Plane();
  myLoc = Plane.Location();
  myDir = Plane.Axis().Direction();

  GroupWPlane->LineEdit1->setText(aString);
  myOkPlane = true;

  /* no simulation */
  return;
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BasicGUI_WorkingPlaneDlg::SetEditCurrentArgument()
{
  GroupWPlane->LineEdit1->setFocus();
  this->SelectionIntoArgument();
  return;
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void BasicGUI_WorkingPlaneDlg::LineEditReturnPressed()
{
  GEOMBase_Skeleton::LineEditReturnPressed();
  return;
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void BasicGUI_WorkingPlaneDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
  mySelection->AddFilter(myFaceFilter);
  return;
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void BasicGUI_WorkingPlaneDlg::enterEvent(QEvent* e)
{
  if(GroupConstructors->isEnabled())
    return;
  this->ActivateThisDialog();
  return;
}
