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
//  File   : BuildGUI_EdgeDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#include "BuildGUI_EdgeDlg.h"

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <Precision.hxx>

#include "utilities.h"
#include "GEOMImpl_Types.hxx"

#include "SUIT_Session.h"
#include "SalomeApp_Application.h"
#include "LightApp_SelectionMgr.h"

#include <qlabel.h>

//=================================================================================
// class    : BuildGUI_EdgeDlg()
// purpose  : Constructs a BuildGUI_EdgeDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
BuildGUI_EdgeDlg::BuildGUI_EdgeDlg(GeometryGUI* theGeometryGUI, QWidget* parent,
                                   const char* name, bool modal, WFlags fl)
  :GEOMBase_Skeleton(theGeometryGUI, parent, name, modal, WStyle_Customize |
                     WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_DLG_BUILD_EDGE")));
  QPixmap image1(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_EDGE_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_EDGE"));
  RadioButton1->setPixmap(image0);
  RadioButton2->close(TRUE);
  RadioButton3->close(TRUE);

  GroupPoints = new DlgRef_2Sel_QTD(this, "GroupPoints");
  GroupPoints->GroupBox1->setTitle(tr("GEOM_POINTS"));
  GroupPoints->TextLabel1->setText(tr("GEOM_POINT_I").arg("1"));
  GroupPoints->TextLabel2->setText(tr("GEOM_POINT_I").arg("2"));
  GroupPoints->PushButton1->setPixmap(image1);
  GroupPoints->PushButton2->setPixmap(image1);
  
  GroupPoints->LineEdit1->setReadOnly( true );
  GroupPoints->LineEdit2->setReadOnly( true );

  Layout1->addWidget(GroupPoints, 2, 0);
  /***************************************************************/

  setHelpFileName("edge.htm");

  /* Initialisations */
  Init();
}


//=================================================================================
// function : ~BuildGUI_EdgeDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
BuildGUI_EdgeDlg::~BuildGUI_EdgeDlg()
{
    // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BuildGUI_EdgeDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupPoints->LineEdit1;
  GroupPoints->LineEdit1->setReadOnly( true );
  GroupPoints->LineEdit2->setReadOnly( true );

  myOkPoint1 = myOkPoint2 = false;

  globalSelection( GEOM_POINT );

  /* signals and slots connections */
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(GroupPoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(GroupPoints->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupPoints->LineEdit2, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  
  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument())) ;

  initName( tr( "GEOM_EDGE") );
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void BuildGUI_EdgeDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool BuildGUI_EdgeDlg::ClickOnApply()
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
void BuildGUI_EdgeDlg::SelectionIntoArgument()
{
  erasePreview();
  myEditCurrentArgument->setText("");
  
  if(IObjectCount() != 1) {
    if(myEditCurrentArgument == GroupPoints->LineEdit1)
      myOkPoint1 = false;
    else if(myEditCurrentArgument == GroupPoints->LineEdit2)
      myOkPoint2 = false;
    return;
  }
  
  // nbSel == 1
  Standard_Boolean testResult = Standard_False;
  GEOM::GEOM_Object_var aSelectedObject = GEOMBase::ConvertIOinGEOMObject(firstIObject(), testResult );
  
  if(!testResult)
    return;

  if(myEditCurrentArgument == GroupPoints->LineEdit1) {
    myPoint1 = aSelectedObject;
    myOkPoint1 = true;
  }
  else if(myEditCurrentArgument == GroupPoints->LineEdit2) {
    myPoint2 = aSelectedObject;
    myOkPoint2 = true;
  }
  
  myEditCurrentArgument->setText( GEOMBase::GetName( aSelectedObject ) );
  
  displayPreview();
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void BuildGUI_EdgeDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if(send == GroupPoints->LineEdit1 || send == GroupPoints->LineEdit2)
    {
      myEditCurrentArgument = send;
      GEOMBase_Skeleton::LineEditReturnPressed();
    }
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BuildGUI_EdgeDlg::SetEditCurrentArgument()
{
  
  QPushButton* send = (QPushButton*)sender();
  globalSelection();

  if(send == GroupPoints->PushButton1) {
    myEditCurrentArgument = GroupPoints->LineEdit1;
    globalSelection( GEOM_POINT );
  }
  else if(send == GroupPoints->PushButton2) {
    myEditCurrentArgument = GroupPoints->LineEdit2;
    globalSelection( GEOM_POINT );
  }
  
  myEditCurrentArgument->setFocus();
  SelectionIntoArgument();
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void BuildGUI_EdgeDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument())) ;
  globalSelection( GEOM_POINT );
  displayPreview();
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void BuildGUI_EdgeDlg::enterEvent(QEvent* e)
{
  if ( !GroupConstructors->isEnabled() )
    ActivateThisDialog();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr BuildGUI_EdgeDlg::createOperation()
{
  return getGeomEngine()->GetIShapesOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool BuildGUI_EdgeDlg::isValid( QString& )
{
  return myOkPoint1 && myOkPoint2;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool BuildGUI_EdgeDlg::execute( ObjectList& objects )
{
  GEOM::GEOM_Object_var anObj;

  anObj = GEOM::GEOM_IShapesOperations::_narrow( getOperation() )->MakeEdge( myPoint1, myPoint2 );

  if ( !anObj->_is_nil() )
    objects.push_back( anObj._retn() );

  return true;
}
