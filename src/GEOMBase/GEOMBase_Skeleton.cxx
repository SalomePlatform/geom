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
//  File   : GEOMBase_Skeleton.cxx
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header$

using namespace std;
#include "GEOMBase_Skeleton.h"
#include "GeometryGUI.h"
#include "SALOME_Selection.h"

#include <qpushbutton.h>

//=================================================================================
// class    : GEOMBase_Skeleton()
// purpose  : Constructs a GEOMBase_Skeleton which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
GEOMBase_Skeleton::GEOMBase_Skeleton(QWidget* parent, const char* name, SALOME_Selection* Sel, bool modal, WFlags fl)
:DlgRef_Skeleton_QTD(parent, name, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu | WDestructiveClose ), GEOMBase_Helper()
{
  if (!name)
    setName("GEOMBase_Skeleton");

  buttonCancel->setText(tr("GEOM_BUT_CLOSE"));
  buttonOk->setText(tr("GEOM_BUT_OK"));
  buttonApply->setText(tr("GEOM_BUT_APPLY"));

  GroupMedium->close(TRUE);
  resize(0, 0);

  Init(Sel);
}


//=================================================================================
// function : ~GEOMBase_Skeleton()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
GEOMBase_Skeleton::~GEOMBase_Skeleton()
{
  myGeomGUI->SetActiveDialogBox( 0 );
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void GEOMBase_Skeleton::Init(SALOME_Selection* Sel)
{
  myGeomGUI = GeometryGUI::GetGeomGUI();

  /* init variables */
  mySelection = Sel;

  myGeomBase = new GEOMBase();  // SAN -- TO BE REMOVED !!!
  myGeomGUI->SetActiveDialogBox(this);

  /* signals and slots connections */
  connect(buttonCancel, SIGNAL(clicked()), this, SLOT(ClickOnCancel()));
  connect(myGeomGUI, SIGNAL(SignalDeactivateActiveDialog()), this, SLOT(DeactivateActiveDialog()));
  connect(myGeomGUI, SIGNAL(SignalCloseAllDialogs()), this, SLOT(ClickOnCancel()));

  /* Move widget on the botton right corner of main widget */
//   int x, y;
//   myGeomBase->DefineDlgPosition( this, x, y );

  /* displays Dialog */
  RadioButton1->setChecked(TRUE);
  RadioButton4->hide();

  return;
}


//=================================================================================
// function : ClickOnCancel()
// purpose  :
//=================================================================================
void GEOMBase_Skeleton::ClickOnCancel()
{
  close();
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void GEOMBase_Skeleton::LineEditReturnPressed()
{
  /* User name of object input management                          */
  /* If successfull the selection is changed and signal emitted... */
  /* so SelectionIntoArgument() is automatically called.           */
  const QString objectUserName = myEditCurrentArgument->text();
  QWidget* thisWidget = (QWidget*)this;
  if(GEOMBase::SelectionByNameInDialogs(thisWidget, objectUserName, mySelection))
    myEditCurrentArgument->setText(objectUserName);

  return;
}


//=================================================================================
// function : DeactivateActiveDialog()
// purpose  :
//=================================================================================
void GEOMBase_Skeleton::DeactivateActiveDialog()
{
  this->setEnabled(false);
  globalSelection();
  //myGeomGUI->ResetState();
  disconnect(mySelection, 0, this, 0);
  GeometryGUI::GetGeomGUI()->SetActiveDialogBox(0);
  return;
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void GEOMBase_Skeleton::ActivateThisDialog()
{
  /* Emit a signal to deactivate the active dialog */
  GeometryGUI::GetGeomGUI()->EmitSignalDeactivateDialog();
  this->setEnabled(true);
  GeometryGUI::GetGeomGUI()->SetActiveDialogBox((QDialog*)this);
  return;
}


//=================================================================================
// function : closeEvent()
// purpose  : same than click on cancel button
//=================================================================================
void GEOMBase_Skeleton::closeEvent(QCloseEvent* e)
{
  disconnect(mySelection, 0, this, 0);
  QDialog::closeEvent( e );
}

//=================================================================================
// function : initName()
// purpose  : initialize the Name field with a string "thePrefix_X" (Vertex_3)
//=================================================================================
void GEOMBase_Skeleton::initName( const char* thePrefix )
{
  if ( thePrefix )
    setPrefix( thePrefix );
  ResultName->setText( GEOMBase::GetDefaultName( getPrefix() ) );
}

//=================================================================================
// function : getNewObjectName()
// purpose  : returns contents of Name field
//=================================================================================
const char* GEOMBase_Skeleton::getNewObjectName() const
{
  return ResultName->text();
}

//=================================================================================
// function : getConstructorId()
// purpose  :
//=================================================================================
int GEOMBase_Skeleton::getConstructorId() const
{
  if ( GroupConstructors != NULL && GroupConstructors->selected() != NULL )
    return GroupConstructors->id( GroupConstructors->selected() );
  return -1;
}
