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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : BuildGUI_FaceDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#include "BuildGUI_FaceDlg.h"
#include "GEOMImpl_Types.hxx"
#include "TColStd_MapOfInteger.hxx"

#include "SUIT_Session.h"
#include "SalomeApp_Application.h"
#include "LightApp_SelectionMgr.h"

//Qt includes
#include <qcheckbox.h>
#include <qlabel.h>

using namespace std;

//=================================================================================
// class    : BuildGUI_FaceDlg()
// purpose  : Constructs a BuildGUI_FaceDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
BuildGUI_FaceDlg::BuildGUI_FaceDlg(GeometryGUI* theGeometryGUI, QWidget* parent,
                                   const char* name, bool modal, WFlags fl)
  :GEOMBase_Skeleton(theGeometryGUI, parent, name, modal, WStyle_Customize |
                     WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_DLG_BUILD_FACE")));
  QPixmap image1(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_FACE_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_FACE"));
  RadioButton1->setPixmap(image0);
  RadioButton2->close(TRUE);
  RadioButton3->close(TRUE);

  GroupWire = new DlgRef_1Sel1Check_QTD(this, "GroupWire");
  GroupWire->GroupBox1->setTitle(tr("GEOM_FACE_FFW"));
  GroupWire->TextLabel1->setText(tr("GEOM_OBJECTS"));
  GroupWire->CheckButton1->setText(tr("GEOM_FACE_OPT"));
  GroupWire->PushButton1->setPixmap(image1);

  Layout1->addWidget(GroupWire, 2, 0);
  /***************************************************************/

  setHelpFileName("face.htm");

  /* Initialisations */
  Init();
}


//=================================================================================
// function : ~BuildGUI_FaceDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
BuildGUI_FaceDlg::~BuildGUI_FaceDlg()
{
  // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BuildGUI_FaceDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupWire->LineEdit1;
  GroupWire->LineEdit1->setReadOnly( true );

  GroupWire->CheckButton1->setChecked(TRUE);

  TColStd_MapOfInteger aMap;
  aMap.Add( GEOM_EDGE );
  aMap.Add( GEOM_WIRE );
  globalSelection( aMap );

  /* signals and slots connections */
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));
  connect(GroupWire->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupWire->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));  
  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument())) ;

  initName(tr("GEOM_FACE"));
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void BuildGUI_FaceDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool BuildGUI_FaceDlg::ClickOnApply()
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
void BuildGUI_FaceDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("");
  QString aName;
  
  int aNbSel = GEOMBase::GetNameOfSelectedIObjects(selectedIO(), aName);
  
  if(aNbSel < 1)
    {
      myWires.length(0);
      return;
    }
  
  GEOMBase::ConvertListOfIOInListOfGO(selectedIO(), myWires);
  if (!myWires.length())
    return;
  if(aNbSel != 1)
    aName = tr("%1_objects").arg(aNbSel);
  
  myEditCurrentArgument->setText( aName );
  
  myEditCurrentArgument->setText( aName );
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BuildGUI_FaceDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();
  if (send != GroupWire->PushButton1)
    return;
  
  TColStd_MapOfInteger aMap;
  aMap.Add( GEOM_EDGE );
  aMap.Add( GEOM_WIRE );
  globalSelection( aMap );

  myEditCurrentArgument = GroupWire->LineEdit1;

  myEditCurrentArgument->setFocus();
  SelectionIntoArgument();
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void BuildGUI_FaceDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument())) ;
  TColStd_MapOfInteger aMap;
  aMap.Add( GEOM_EDGE );
  aMap.Add( GEOM_WIRE );
  globalSelection( aMap );
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void BuildGUI_FaceDlg::enterEvent(QEvent* e)
{
  if ( !GroupConstructors->isEnabled() )
    ActivateThisDialog(); 
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr BuildGUI_FaceDlg::createOperation()
{
  return getGeomEngine()->GetIShapesOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool BuildGUI_FaceDlg::isValid( QString& )
{
  return (myWires.length() != 0);
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool BuildGUI_FaceDlg::execute( ObjectList& objects )
{
  GEOM::GEOM_Object_var anObj;

  bool isPlanarWanted = GroupWire->CheckButton1->isChecked();
  anObj = GEOM::GEOM_IShapesOperations::_narrow(
    getOperation() )->MakeFaceWires( myWires, isPlanarWanted );

  if ( !anObj->_is_nil() )
    objects.push_back( anObj._retn() );

  return true;
}

