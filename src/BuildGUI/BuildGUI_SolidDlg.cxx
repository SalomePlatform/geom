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
//  File   : BuildGUI_SolidDlg.cxx
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header: 

#include "BuildGUI_SolidDlg.h"
#include "GEOMImpl_Types.hxx"

#include "SUIT_Session.h"
#include "SalomeApp_Application.h"
#include "LightApp_SelectionMgr.h"

//Qt includes
#include <qcheckbox.h>
#include <qlabel.h>

//=================================================================================
// class    : BuildGUI_SolidDlg()
// purpose  : Constructs a BuildGUI_SolidDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
BuildGUI_SolidDlg::BuildGUI_SolidDlg(QWidget* parent, const char* name, bool modal, WFlags fl)
  :GEOMBase_Skeleton(parent, name, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_BUILD_SOLID")));
  QPixmap image1(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_SELECT")));

  setCaption(tr("GEOM_SOLID_TITLE"));
    
  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_SOLID"));
  RadioButton1->setPixmap(image0);
  RadioButton2->close(TRUE);
  RadioButton3->close(TRUE);

  GroupSolid = new DlgRef_1Sel1Check_QTD(this, "GroupSolid");
  GroupSolid->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  GroupSolid->TextLabel1->setText(tr("GEOM_OBJECTS"));
  GroupSolid->CheckButton1->setText(tr("GEOM_CREATE_SINGLE_SOLID"));
  GroupSolid->PushButton1->setPixmap(image1);
  GroupSolid->LineEdit1->setReadOnly( true );
  
  Layout1->addWidget(GroupSolid, 2, 0);
  /***************************************************************/

  /* Initialisations */
  Init();
}


//=================================================================================
// function : ~BuildGUI_SolidDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
BuildGUI_SolidDlg::~BuildGUI_SolidDlg()
{
    // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BuildGUI_SolidDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupSolid->LineEdit1;
  GroupSolid->LineEdit1->setReadOnly( true );
  GroupSolid->CheckButton1->setChecked( true );

  myOkShells = false;

  globalSelection( GEOM_SHELL );

  /* signals and slots connections */
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(GroupSolid->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupSolid->CheckButton1, SIGNAL(toggled(bool)), this, SLOT(EnableNameField(bool)));

  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument())) ;

  initName(tr("GEOM_SOLID"));
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void BuildGUI_SolidDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool BuildGUI_SolidDlg::ClickOnApply()
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
void BuildGUI_SolidDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("");
  QString aString = "";
  
  myOkShells = false;
  int nbSel = GEOMBase::GetNameOfSelectedIObjects(selectedIO(), aString);
  if (nbSel == 0) 
    return;
  if(nbSel != 1)
    aString = tr("%1_objects").arg(nbSel);
  
  GEOMBase::ConvertListOfIOInListOfGO(selectedIO(), myShells);
  if (!myShells.length()) 
    return;
  
  myEditCurrentArgument->setText(aString);
  myOkShells = true;
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BuildGUI_SolidDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();
  if(send != GroupSolid->PushButton1)
    return;
  
  globalSelection( GEOM_SHELL );
  myEditCurrentArgument = GroupSolid->LineEdit1;

  myEditCurrentArgument->setFocus();
  SelectionIntoArgument();
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void BuildGUI_SolidDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument())) ;
  globalSelection( GEOM_SHELL );
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void BuildGUI_SolidDlg::enterEvent(QEvent* e)
{
  if ( !GroupConstructors->isEnabled() )
    ActivateThisDialog();
}


//=================================================================================
// function :  EnableNameField()
// purpose  :
//=================================================================================
void  BuildGUI_SolidDlg::EnableNameField(bool toEnable)
{
  this->GroupBoxName->setEnabled(toEnable); 
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr BuildGUI_SolidDlg::createOperation()
{
  return getGeomEngine()->GetIShapesOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool BuildGUI_SolidDlg::isValid( QString& )
{
  return myOkShells;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool BuildGUI_SolidDlg::execute( ObjectList& objects )
{
  bool toCreateSingleSolid = GroupSolid->CheckButton1->isChecked();
  
  if ( toCreateSingleSolid )
  {
    GEOM::GEOM_Object_var anObj = GEOM::GEOM_IShapesOperations::_narrow(
      getOperation() )->MakeSolidShells( myShells );

    if ( !anObj->_is_nil() )
      objects.push_back( anObj._retn() );
  }
  else
  {
    for ( int i = 0, n = myShells.length(); i< n; i++ )
    {
      GEOM::GEOM_Object_var anObj = GEOM::GEOM_IShapesOperations::_narrow(
        getOperation() )->MakeSolidShell( myShells[ i ] );

     if ( !anObj->_is_nil() )
       objects.push_back( anObj._retn() );
    }
  }

  return true;
}

