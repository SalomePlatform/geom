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
//  File   : BuildGUI_ShellDlg.cxx
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header: 

#include "BuildGUI_ShellDlg.h"

#include "QAD_Desktop.h"
#include "GEOMImpl_Types.hxx"

//=================================================================================
// class    : BuildGUI_ShellDlg()
// purpose  : Constructs a BuildGUI_ShellDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
BuildGUI_ShellDlg::BuildGUI_ShellDlg(QWidget* parent, const char* name, SALOME_Selection* Sel, bool modal, WFlags fl)
  :GEOMBase_Skeleton(parent, name, Sel, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap("GEOM", tr("ICON_DLG_BUILD_SHELL")));
  QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap("GEOM", tr("ICON_SELECT")));

  setCaption(tr("GEOM_SHELL_TITLE"));
    
  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_SHELL"));
  RadioButton1->setPixmap(image0);
  RadioButton2->close(TRUE);
  RadioButton3->close(TRUE);

  GroupShell = new DlgRef_1Sel_QTD(this, "GroupShell");
  GroupShell->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  GroupShell->TextLabel1->setText(tr("GEOM_OBJECTS"));
  GroupShell->PushButton1->setPixmap(image1);
  GroupShell->LineEdit1->setReadOnly( true );
  
  Layout1->addWidget(GroupShell, 2, 0);
  /***************************************************************/

  /* Initialisations */
  Init();
}


//=================================================================================
// function : ~BuildGUI_ShellDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
BuildGUI_ShellDlg::~BuildGUI_ShellDlg()
{
    // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BuildGUI_ShellDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupShell->LineEdit1;
  GroupShell->LineEdit1->setReadOnly( true );
  
  myOkFacesAndShells = false;
  
  TColStd_MapOfInteger aMap;
  aMap.Add(GEOM_SHELL);
  aMap.Add(GEOM_FACE);
  globalSelection( aMap );

  /* signals and slots connections */
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(GroupShell->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  initName(tr("GEOM_SHELL"));
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void BuildGUI_ShellDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool BuildGUI_ShellDlg::ClickOnApply()
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
void BuildGUI_ShellDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText( "" );
  QString aString;

  myOkFacesAndShells = false;
  int nbSel = GEOMBase::GetNameOfSelectedIObjects( mySelection, aString, true );
  if ( nbSel == 0 )
    return;
  if ( nbSel != 1 )
    aString = QString( "%1_objects ").arg( nbSel );

  GEOMBase::ConvertListOfIOInListOfGO( mySelection->StoredIObjects(),  myFacesAndShells, true );
  if ( !myFacesAndShells.length() )
    return;

  myEditCurrentArgument->setText( aString );
  myOkFacesAndShells = true;
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BuildGUI_ShellDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();
  if (send != GroupShell->PushButton1)
    return;

  TColStd_MapOfInteger aMap;
  aMap.Add(GEOM_SHELL);
  aMap.Add(GEOM_FACE);
  globalSelection( aMap );
  myEditCurrentArgument = GroupShell->LineEdit1;

  myEditCurrentArgument->setFocus();
  SelectionIntoArgument();
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void BuildGUI_ShellDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
  TColStd_MapOfInteger aMap;
  aMap.Add(GEOM_SHELL);
  aMap.Add(GEOM_FACE);
  globalSelection( aMap );
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void BuildGUI_ShellDlg::enterEvent(QEvent* e)
{
  if ( !GroupConstructors->isEnabled() )
    ActivateThisDialog();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr BuildGUI_ShellDlg::createOperation()
{
  return getGeomEngine()->GetIShapesOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool BuildGUI_ShellDlg::isValid( QString& )
{
  return myOkFacesAndShells;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool BuildGUI_ShellDlg::execute( ObjectList& objects )
{
  GEOM::GEOM_Object_var anObj;

  anObj = GEOM::GEOM_IShapesOperations::_narrow(
    getOperation() )->MakeShell( myFacesAndShells );

  if ( !anObj->_is_nil() )
    objects.push_back( anObj._retn() );

  return true;
}

