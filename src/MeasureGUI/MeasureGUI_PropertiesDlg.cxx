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
//  File   : MeasureGUI_PropertiesDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#include "MeasureGUI_PropertiesDlg.h"

#include <TopExp_Explorer.hxx>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <GProp_PrincipalProps.hxx>

#include "utilities.h"

using namespace std;

//=================================================================================
// class    : MeasureGUI_PropertiesDlg()
// purpose  : Constructs a MeasureGUI_PropertiesDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
MeasureGUI_PropertiesDlg::MeasureGUI_PropertiesDlg(QWidget* parent, const char* name, SALOME_Selection* Sel, bool modal, WFlags fl)
  :MeasureGUI_Skeleton(parent, name, Sel, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_DLG_BASICPROPERTIES")));
  QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_PROPERTIES_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_PROPERTIES"));
  RadioButton1->setPixmap(image0);

  GroupC1 = new MeasureGUI_1Sel3LineEdit_QTD(this, "GroupC1");
  GroupC1->GroupBox1->setTitle(tr("GEOM_PROPERTIES_CONSTR"));
  GroupC1->TextLabel1->setText(tr("GEOM_OBJECT"));
  GroupC1->TextLabel2->setText(tr("GEOM_LENGTH"));
  GroupC1->TextLabel3->setText(tr("GEOM_PROPERTIES_SURFACE"));
  GroupC1->TextLabel4->setText(tr("GEOM_PROPERTIES_VOLUME"));
  GroupC1->LineEdit2->setReadOnly(TRUE);
  GroupC1->LineEdit3->setReadOnly(TRUE);
  GroupC1->LineEdit4->setReadOnly(TRUE);
  GroupC1->PushButton1->setPixmap(image1);

  Layout1->addWidget(GroupC1, 1, 0);
  /***************************************************************/

  /* Initialisation */
  Init();
}


//=================================================================================
// function : ~MeasureGUI_PropertiesDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
MeasureGUI_PropertiesDlg::~MeasureGUI_PropertiesDlg()
{
  // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void MeasureGUI_PropertiesDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupC1->LineEdit1;

   /* signals and slots connections */
  connect(GroupC1->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupC1->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  /* displays Dialog */
  GroupC1->show();
  this->show();

  return;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void MeasureGUI_PropertiesDlg::SelectionIntoArgument()
{
  myGeomBase->EraseSimulationShape();
  mySimulationTopoDs.Nullify();
  myEditCurrentArgument->setText("");
  QString aString = "";

  GroupC1->LineEdit2->setText("");
  GroupC1->LineEdit3->setText("");
  GroupC1->LineEdit4->setText("");

  int nbSel = myGeomBase->GetNameOfSelectedIObjects(mySelection, aString);
  if(nbSel != 1)
    return;

  /*  nbSel == 1  */
  TopoDS_Shape S;
  if(!myGeomBase->GetTopoFromSelection(mySelection, S))
    return;

  
  if(S.IsNull() || S.ShapeType() == TopAbs_VERTEX)
    return;
 
  GroupC1->LineEdit1->setText(aString);

  /* Try to display of a cone simulation shape to show direction of a linear edge only in OCC viewer */
  if(myGeomBase->CreateArrowForLinearEdge(S, mySimulationTopoDs))
    myGeomBase->DisplaySimulationShape(mySimulationTopoDs);

  this->CalculateAndDisplayProperties(S);
  return;
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void MeasureGUI_PropertiesDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  if(send == GroupC1->PushButton1) {
    GroupC1->LineEdit1->setFocus();
    myEditCurrentArgument = GroupC1->LineEdit1;
  }

  this->SelectionIntoArgument();
  return;
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void MeasureGUI_PropertiesDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if(send == GroupC1->LineEdit1)
    myEditCurrentArgument = GroupC1->LineEdit1;
  else
    return;

  MeasureGUI_Skeleton::LineEditReturnPressed();
  return;
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void MeasureGUI_PropertiesDlg::ActivateThisDialog()
{
  MeasureGUI_Skeleton::ActivateThisDialog();
  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
  if(!mySimulationTopoDs.IsNull())
    myGeomBase->DisplaySimulationShape(mySimulationTopoDs);
  return;
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void MeasureGUI_PropertiesDlg::enterEvent(QEvent* e)
{
  if(GroupConstructors->isEnabled())
    return;
  this->ActivateThisDialog();
  return;
}


//=================================================================================
// function : CalculateAndDisplayProperties()
// purpose  :
//=================================================================================
void MeasureGUI_PropertiesDlg::CalculateAndDisplayProperties(const TopoDS_Shape& S)
{
  GroupC1->LineEdit2->setText("");
  GroupC1->LineEdit3->setText("");
  GroupC1->LineEdit4->setText("");

  if(S.IsNull()) 
    return;

  Standard_Real result;
  GProp_GProps LProps, SProps;
  QString resString;

  try {
    BRepGProp::LinearProperties(S,LProps);
    result = LProps.Mass();
    if(!IsEqual(result, 0.0)) {	
      resString = tr("%1").arg(result, 12, 'f', 6);
      GroupC1->LineEdit2->setText(resString);
    }

    BRepGProp::SurfaceProperties(S, SProps);
    result = SProps.Mass();
    if(!IsEqual(result, 0.0)) {	
      resString = tr("%1").arg(result, 12, 'f', 6);
      GroupC1->LineEdit3->setText(resString);
    }
	
    result = 0.0;
    if(S.ShapeType() < TopAbs_SHELL) {
      for(TopExp_Explorer Exp(S,TopAbs_SOLID); Exp.More(); Exp.Next()) { 
	GProp_GProps VProps;
	BRepGProp::VolumeProperties(Exp.Current(), VProps);
	result += VProps.Mass();
      }
    }
    if (!IsEqual(result, 0.0)) {	
      resString = tr("%1").arg(result, 12, 'f', 6);
      GroupC1->LineEdit4->setText(resString);
    }
  }
  catch(Standard_Failure) {
    MESSAGE("Catch intercepted in CalculateAndDisplayProperties()");
  }
  return;
}
