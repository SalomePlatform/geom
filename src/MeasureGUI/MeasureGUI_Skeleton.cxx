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
//  File   : MeasureGUI_Skeleton.cxx
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header: 

using namespace std;
#include "MeasureGUI_Skeleton.h"

//=================================================================================
// class    : MeasureGUI_Skeleton()
// purpose  : Constructs a MeasureGUI_Skeleton which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
MeasureGUI_Skeleton::MeasureGUI_Skeleton(QWidget* parent, const char* name, SALOME_Selection* Sel, bool modal, WFlags fl)
  :MeasureGUI_Skeleton_QTD(parent, name, modal, fl)
{
  if (!name)
    setName("MeasureGUI_Skeleton");

  buttonClose->setText(tr("GEOM_BUT_CLOSE"));

  GroupMedium->close(TRUE);
  resize(350, 0);

  Init(Sel);
}


//=================================================================================
// function : ~MeasureGUI_Skeleton()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
MeasureGUI_Skeleton::~MeasureGUI_Skeleton()
{
  // no need to delete child widgets, Qt does it all for us
  this->destroy(TRUE, TRUE);
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void MeasureGUI_Skeleton::Init(SALOME_Selection* Sel)
{
  /* init variables */
  mySelection = Sel;
  mySimulationTopoDs.Nullify();

  myGeomBase = new GEOMBase();
  myGeomGUI = GEOMContext::GetGeomGUI();
  myGeomGUI->SetActiveDialogBox((QDialog*)this);
  myGeom = myGeomGUI->myComponentGeom;

  /* signals and slots connections */
  connect(buttonClose, SIGNAL(clicked()), this, SLOT(ClickOnCancel()));
  connect(myGeomGUI, SIGNAL(SignalDeactivateActiveDialog()), this, SLOT(DeactivateActiveDialog()));
  connect(myGeomGUI, SIGNAL(SignalCloseAllDialogs()), this, SLOT(ClickOnCancel()));

  /* Move widget on the botton right corner of main widget */
//   int x, y;
//   myGeomBase->DefineDlgPosition( this, x, y );

  /* displays Dialog */
  RadioButton1->setChecked(TRUE);

  return;
}


//=================================================================================
// function : ClickOnCancel()
// purpose  :
//=================================================================================
void MeasureGUI_Skeleton::ClickOnCancel()
{
  myGeomBase->EraseSimulationShape();
  mySimulationTopoDs.Nullify();

  disconnect(mySelection, 0, this, 0);
  //myGeomGUI->ResetState();
  reject();

  return;
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void MeasureGUI_Skeleton::LineEditReturnPressed()
{
  /* User name of object input management                          */
  /* If successfull the selection is changed and signal emitted... */
  /* so SelectionIntoArgument() is automatically called.           */
  const QString objectUserName = myEditCurrentArgument->text();
  QWidget* thisWidget = (QWidget*)this;
  if(myGeomBase->SelectionByNameInDialogs(thisWidget, objectUserName, mySelection))
    myEditCurrentArgument->setText(objectUserName);

  return;
}


//=================================================================================
// function : DeactivateActiveDialog()
// purpose  :
//=================================================================================
void MeasureGUI_Skeleton::DeactivateActiveDialog()
{
  this->setEnabled(false);
  //myGeomGUI->ResetState();
  disconnect(mySelection, 0, this, 0);
  myGeomBase->EraseSimulationShape();
  myGeomGUI->SetActiveDialogBox(0);
  return;
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void MeasureGUI_Skeleton::ActivateThisDialog()
{
  /* Emit a signal to deactivate the active dialog */
  myGeomGUI->EmitSignalDeactivateDialog();
  this->setEnabled(true);
  myGeomGUI->SetActiveDialogBox((QDialog*)this);
  return;
}


//=================================================================================
// function : closeEvent()
// purpose  :
//=================================================================================
void MeasureGUI_Skeleton::closeEvent(QCloseEvent* e)
{
  /* same than click on cancel button */
  this->ClickOnCancel();
  return;
}
