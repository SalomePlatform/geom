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
//  File   : BuildGUI_WireDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#include "BuildGUI_WireDlg.h"
#include "GEOMImpl_Types.hxx"

#include "SUIT_Session.h"
#include "SalomeApp_Application.h"
#include "LightApp_SelectionMgr.h"

#include "TColStd_MapOfInteger.hxx"

#include <qlabel.h>

//=================================================================================
// class    : BuildGUI_WireDlg()
// purpose  : Constructs a BuildGUI_WireDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
BuildGUI_WireDlg::BuildGUI_WireDlg(QWidget* parent, const char* name, bool modal, WFlags fl)
  :GEOMBase_Skeleton(parent, name, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_DLG_BUILD_WIRE")));
  QPixmap image1(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_WIRE_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_WIRE"));
  RadioButton1->setPixmap(image0);
  RadioButton2->close(TRUE);
  RadioButton3->close(TRUE);

  GroupPoints = new DlgRef_1Sel_QTD(this, "GroupPoints");
  GroupPoints->GroupBox1->setTitle(tr("GEOM_WIRE_CONNECT"));
  GroupPoints->TextLabel1->setText(tr("GEOM_OBJECTS"));
  GroupPoints->PushButton1->setPixmap(image1);
  GroupPoints->LineEdit1->setReadOnly( true );

  Layout1->addWidget(GroupPoints, 2, 0);
  /***************************************************************/

  /* Initialisations */
  Init();
}


//=================================================================================
// function : ~BuildGUI_WireDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
BuildGUI_WireDlg::~BuildGUI_WireDlg()
{
  // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BuildGUI_WireDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupPoints->LineEdit1;
  GroupPoints->LineEdit1->setReadOnly( true );
  
  myOkEdgesAndWires = false;
  
  TColStd_MapOfInteger aMap;
  aMap.Add(GEOM_WIRE);
  aMap.Add(GEOM_EDGE);
  globalSelection( aMap );

  /* signals and slots connections */
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));
  connect(GroupPoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument())) ;
  
  initName(tr("GEOM_WIRE"));
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void BuildGUI_WireDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool BuildGUI_WireDlg::ClickOnApply()
{
  if ( !onAccept() )
    return false;

  initName();
  return true;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void BuildGUI_WireDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("");
  QString aString = ""; /* name of selection */

  myOkEdgesAndWires = false;
  int nbSel = GEOMBase::GetNameOfSelectedIObjects(selectedIO(), aString);

  if(nbSel == 0)
    return;
  if(nbSel != 1)
    aString = tr("%1_objects").arg(nbSel);

  GEOMBase::ConvertListOfIOInListOfGO(selectedIO(),  myEdgesAndWires);
  if (!myEdgesAndWires.length())
    return;

  myEditCurrentArgument->setText(aString);
  myOkEdgesAndWires = true;
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BuildGUI_WireDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();
  if (send != GroupPoints->PushButton1)
    return;

  TColStd_MapOfInteger aMap;
  aMap.Add(GEOM_WIRE);
  aMap.Add(GEOM_EDGE);
  globalSelection( aMap );
  myEditCurrentArgument = GroupPoints->LineEdit1;

  myEditCurrentArgument->setFocus();
  SelectionIntoArgument();
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void BuildGUI_WireDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument())) ;
  TColStd_MapOfInteger aMap;
  aMap.Add(GEOM_WIRE);
  aMap.Add(GEOM_EDGE);
  globalSelection( aMap );
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void BuildGUI_WireDlg::enterEvent(QEvent* e)
{
  if ( !GroupConstructors->isEnabled() )
    ActivateThisDialog();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr BuildGUI_WireDlg::createOperation()
{
  return getGeomEngine()->GetIShapesOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool BuildGUI_WireDlg::isValid( QString& )
{
  return myOkEdgesAndWires;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool BuildGUI_WireDlg::execute( ObjectList& objects )
{
  GEOM::GEOM_Object_var anObj;

  anObj = GEOM::GEOM_IShapesOperations::_narrow(
    getOperation() )->MakeWire( myEdgesAndWires );

  if ( !anObj->_is_nil() )
    objects.push_back( anObj._retn() );

  return true;
}

