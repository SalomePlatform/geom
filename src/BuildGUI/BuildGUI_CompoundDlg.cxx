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
//  File   : BuildGUI_CompoundDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#include "BuildGUI_CompoundDlg.h"
#include "GEOMImpl_Types.hxx"

#include "SUIT_Session.h"
#include "SalomeApp_Application.h"
#include "LightApp_SelectionMgr.h"

#include <qlabel.h>

//=================================================================================
// class    : BuildGUI_CompoundDlg()
// purpose  : Constructs a BuildGUI_CompoundDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
BuildGUI_CompoundDlg::BuildGUI_CompoundDlg(GeometryGUI* theGeometryGUI, QWidget* parent,
                                           const char* name, bool modal, WFlags fl)
  :GEOMBase_Skeleton(theGeometryGUI, parent, name, modal, WStyle_Customize |
                     WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_DLG_BUILD_COMPOUND")));
  QPixmap image1(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_COMPOUND_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_COMPOUND"));
  RadioButton1->setPixmap(image0);
  RadioButton2->close(TRUE);
  RadioButton3->close(TRUE);

  GroupShapes = new DlgRef_1Sel_QTD(this, "GroupShapes");
  GroupShapes->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  GroupShapes->TextLabel1->setText(tr("GEOM_OBJECTS"));
  GroupShapes->PushButton1->setPixmap(image1);
  GroupShapes->LineEdit1->setReadOnly( true );

  Layout1->addWidget(GroupShapes, 2, 0);
  /***************************************************************/

  setHelpFileName("compound.htm");

  /* Initialisations */
  Init();
}


//=================================================================================
// function : ~BuildGUI_CompoundDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
BuildGUI_CompoundDlg::~BuildGUI_CompoundDlg()
{
  // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BuildGUI_CompoundDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupShapes->LineEdit1;
  GroupShapes->LineEdit1->setReadOnly( true );
  
  myOkShapes = false;

  /* signals and slots connections */
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));
  connect(GroupShapes->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));  
  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument())) ;

  globalSelection( GEOM_ALLSHAPES );

  initName( tr( "GEOM_COMPOUND" ) );
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void BuildGUI_CompoundDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool BuildGUI_CompoundDlg::ClickOnApply()
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
void BuildGUI_CompoundDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText( "" );
  QString aString = ""; /* name of selection */

  myOkShapes = false;
  int nbSel = GEOMBase::GetNameOfSelectedIObjects( selectedIO(), aString, true );
  if ( nbSel == 0 ) 
    return;
  if ( nbSel != 1 )
    aString = QString( "%1_objects").arg( nbSel );
  
  GEOMBase::ConvertListOfIOInListOfGO( selectedIO(), myShapes, true );
  myEditCurrentArgument->setText( aString );
  myOkShapes = true;
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BuildGUI_CompoundDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();
  if (send != GroupShapes->PushButton1)
    return;
  
  myEditCurrentArgument = GroupShapes->LineEdit1;
  
  myEditCurrentArgument->setFocus();
  SelectionIntoArgument();
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void BuildGUI_CompoundDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  globalSelection( GEOM_ALLSHAPES );
  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument())) ;
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void BuildGUI_CompoundDlg::enterEvent(QEvent* e)
{
  if ( !GroupConstructors->isEnabled() )
    ActivateThisDialog(); 
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr BuildGUI_CompoundDlg::createOperation()
{
  return getGeomEngine()->GetIShapesOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool BuildGUI_CompoundDlg::isValid( QString& )
{
  return myOkShapes;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool BuildGUI_CompoundDlg::execute( ObjectList& objects )
{
  GEOM::GEOM_Object_var anObj;

  anObj = GEOM::GEOM_IShapesOperations::_narrow( getOperation() )->MakeCompound( myShapes );

  if ( !anObj->_is_nil() )
    objects.push_back( anObj._retn() );

  return true;
}
