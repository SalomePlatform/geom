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
//  File   : GenerationGUI_PipeDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#include "GenerationGUI_PipeDlg.h"

#include "SUIT_Desktop.h"
#include "SUIT_Session.h"
#include "SalomeApp_Application.h"
#include "LightApp_SelectionMgr.h"

#include <qlabel.h>

#include <TopoDS_Edge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepOffsetAPI_MakePipe.hxx>
#if OCC_VERSION_MAJOR >= 5
#include <BRepAlgo.hxx>
#else
#include <BRepAlgoAPI.hxx>
#endif

#include "GEOMImpl_Types.hxx"

#include "utilities.h"

//=================================================================================
// class    : GenerationGUI_PipeDlg()
// purpose  : Constructs a GenerationGUI_PipeDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
GenerationGUI_PipeDlg::GenerationGUI_PipeDlg(GeometryGUI* theGeometryGUI, QWidget* parent,
                                             const char* name, bool modal, WFlags fl)
  :GEOMBase_Skeleton(theGeometryGUI, parent, name, modal, WStyle_Customize |
                     WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_DLG_PIPE")));
  QPixmap image1(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_PIPE_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_PIPE"));
  RadioButton1->setPixmap(image0);
  RadioButton2->close(TRUE);
  RadioButton3->close(TRUE);

  GroupPoints = new DlgRef_2Sel_QTD(this, "GroupPoints");
  GroupPoints->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  GroupPoints->TextLabel1->setText(tr("GEOM_BASE_OBJECT"));
  GroupPoints->TextLabel2->setText(tr("GEOM_PATH_OBJECT"));
  GroupPoints->PushButton1->setPixmap(image1);
  GroupPoints->PushButton2->setPixmap(image1);
  GroupPoints->LineEdit1->setReadOnly( true );
  GroupPoints->LineEdit2->setReadOnly( true );

  Layout1->addWidget(GroupPoints, 2, 0);
  /***************************************************************/

  setHelpFileName("pipe_creation.htm");

  /* Initialisations */
  Init();
}


//=================================================================================
// function : ~GenerationGUI_PipeDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
GenerationGUI_PipeDlg::~GenerationGUI_PipeDlg()
{
  /* no need to delete child widgets, Qt does it all for us */
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void GenerationGUI_PipeDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupPoints->LineEdit1;
  GroupPoints->LineEdit1->setReadOnly( true );
  GroupPoints->LineEdit2->setReadOnly( true );

  myOkBase = myOkPath = false;

  /* signals and slots connections */
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(GroupPoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(GroupPoints->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupPoints->LineEdit2, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  
  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  initName(tr("GEOM_PIPE"));

  globalSelection( GEOM_ALLSHAPES );
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void GenerationGUI_PipeDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}


//=================================================================================
// function : ClickOnAply()
// purpose  :
//=================================================================================
bool GenerationGUI_PipeDlg::ClickOnApply()
{
  if ( !onAccept() )
    return false;

  initName();
  return true;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection has changed
//=================================================================================
void GenerationGUI_PipeDlg::SelectionIntoArgument()
{
  erasePreview();
  myEditCurrentArgument->setText("");
  
  if(IObjectCount() != 1) {
    if(myEditCurrentArgument == GroupPoints->LineEdit1)
      myOkBase = false;
    else if(myEditCurrentArgument == GroupPoints->LineEdit2)
      myOkPath = false;
    return;
  }
  
  // nbSel == 1
  Standard_Boolean testResult = Standard_False;
  GEOM::GEOM_Object_ptr aSelectedObject = GEOMBase::ConvertIOinGEOMObject(firstIObject(), testResult );
    
  if (!testResult)
    return;
  
  TopoDS_Shape S;
  
  if(myEditCurrentArgument == GroupPoints->LineEdit1) {
    myOkBase = false;
    
    if(!GEOMBase::GetShape(aSelectedObject, S) ||
       S.ShapeType() == TopAbs_COMPSOLID || 
       S.ShapeType() == TopAbs_COMPOUND || 
       S.ShapeType() == TopAbs_SOLID || 
       S.ShapeType() == TopAbs_SHAPE) 
      return;
    
    myBase = aSelectedObject;
    myOkBase = true;
  }
  else if(myEditCurrentArgument == GroupPoints->LineEdit2) {
    myOkPath = false;
    
    if(!GEOMBase::GetShape(aSelectedObject, S) ||
       !(S.ShapeType() == TopAbs_WIRE || S.ShapeType() == TopAbs_EDGE) ) 
      return;
    
    myPath = aSelectedObject;
    myOkPath = true;
  }
  myEditCurrentArgument->setText( GEOMBase::GetName( aSelectedObject ) );
  
  displayPreview();
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void GenerationGUI_PipeDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  if(send == GroupPoints->PushButton1) {
    GroupPoints->LineEdit1->setFocus();
    myEditCurrentArgument = GroupPoints->LineEdit1;
  }
  else if(send == GroupPoints->PushButton2) {
    GroupPoints->LineEdit2->setFocus();
    myEditCurrentArgument = GroupPoints->LineEdit2;
  }
  SelectionIntoArgument();
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void GenerationGUI_PipeDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if(send == GroupPoints->LineEdit1 ||
     send == GroupPoints->LineEdit2)
    {
      myEditCurrentArgument = send;
      GEOMBase_Skeleton::LineEditReturnPressed();
    }
}


//=================================================================================
// function : enterEvent()
// purpose  : when mouse enter onto the QWidget
//=================================================================================
void GenerationGUI_PipeDlg::enterEvent(QEvent* e)
{
  if ( !GroupConstructors->isEnabled() )
    ActivateThisDialog();
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void GenerationGUI_PipeDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  globalSelection( GEOM_ALLSHAPES );
  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
  displayPreview();
}


//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr GenerationGUI_PipeDlg::createOperation()
{
  return getGeomEngine()->GetI3DPrimOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool GenerationGUI_PipeDlg::isValid( QString& )
{
  return myOkBase && myOkPath;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool GenerationGUI_PipeDlg::execute( ObjectList& objects )
{
  GEOM::GEOM_Object_var anObj;

  anObj = GEOM::GEOM_I3DPrimOperations::_narrow(
    getOperation() )->MakePipe( myBase, myPath );

  if ( !anObj->_is_nil() )
    objects.push_back( anObj._retn() );

  return true;
}



