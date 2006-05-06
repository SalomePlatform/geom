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
//  File   : TransformationGUI_MirrorDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#include "TransformationGUI_MirrorDlg.h"

#include "SUIT_Desktop.h"
#include "SUIT_Session.h"
#include "SalomeApp_Application.h"
#include "LightApp_SelectionMgr.h"

#include <qcheckbox.h>
#include <qlabel.h>

#include "GEOMImpl_Types.hxx"

#include "utilities.h"

using namespace std;

//=================================================================================
// class    : TransformationGUI_MirrorDlg()
// purpose  : Constructs a TransformationGUI_MirrorDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
TransformationGUI_MirrorDlg::TransformationGUI_MirrorDlg(GeometryGUI* theGeometryGUI, QWidget* parent,
                                                         const char* name, bool modal, WFlags fl)
  :GEOMBase_Skeleton(theGeometryGUI, parent, name, modal, WStyle_Customize |
                     WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_DLG_MIRROR_POINT")));
  QPixmap image1(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_DLG_MIRROR_AXE")));
  QPixmap image2(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_DLG_MIRROR_PLANE")));
  QPixmap image3(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_MIRROR_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_MIRROR"));
  RadioButton1->setPixmap(image0);
  RadioButton2->setPixmap(image1);
  RadioButton3->setPixmap(image2);
 
  GroupPoints = new DlgRef_2Sel1Spin2Check(this, "GroupPoints");
  GroupPoints->SpinBox_DX->hide();
  GroupPoints->TextLabel3->hide();
  GroupPoints->CheckButton2->hide();
  GroupPoints->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  GroupPoints->TextLabel1->setText(tr("GEOM_OBJECTS"));
  GroupPoints->TextLabel2->setText(tr("GEOM_POINT_MIRROR"));
  GroupPoints->TextLabel2->setFixedWidth(74);
  GroupPoints->PushButton1->setPixmap(image3);
  GroupPoints->PushButton2->setPixmap(image3);
  GroupPoints->CheckButton1->setText(tr("GEOM_CREATE_COPY"));
  Layout1->addWidget(GroupPoints, 2, 0);
  /***************************************************************/

  setHelpFileName("mirror_image.htm");  
  
  Init();
}


//=================================================================================
// function : ~TransformationGUI_MirrorDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
TransformationGUI_MirrorDlg::~TransformationGUI_MirrorDlg()
{  
  /* no need to delete child widgets, Qt does it all for us */
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void TransformationGUI_MirrorDlg::Init()
{  
  /* init variables */
  myEditCurrentArgument = GroupPoints->LineEdit1;
  GroupPoints->LineEdit1->setReadOnly(true);
  GroupPoints->LineEdit2->setReadOnly(true);
  
  myArgument = GEOM::GEOM_Object::_nil();
    
  // Activate Create a Copy mode
  GroupPoints->CheckButton1->setChecked(true);
  CreateCopyModeChanged(true);

  /* signals and slots connections */
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));
  connect(GroupConstructors, SIGNAL(clicked(int)), SLOT(ConstructorsClicked(int)));

  connect(GroupPoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(GroupPoints->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupPoints->LineEdit2, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  
  connect(GroupPoints->CheckButton1, SIGNAL(toggled(bool)), this, SLOT(CreateCopyModeChanged(bool)));
  
  connect(myGeomGUI->getApp()->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  initName( tr( "GEOM_MIRROR" ) );
  ConstructorsClicked( 0 );
}


//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void TransformationGUI_MirrorDlg::ConstructorsClicked(int constructorId)
{
  disconnect( myGeomGUI->getApp()->selectionMgr(), 0, this, 0 );
  
  globalSelection();
  myEditCurrentArgument = GroupPoints->LineEdit1;
  GroupPoints->LineEdit2->clear();
  myArgument = GEOM::GEOM_Object::_nil();
  
  switch (constructorId)
    {
    case 0: /* mirror an object by point */
      {	
	GroupPoints->TextLabel2->setText(tr("GEOM_POINT_MIRROR"));
	break;
      }
    case 1: /* mirror an object by axe */
      {
	GroupPoints->TextLabel2->setText(tr("GEOM_AXE_MIRROR"));
	break;
      } 
    case 2: /* mirror an object by plane */
      {
	GroupPoints->TextLabel2->setText(tr("GEOM_PLANE_MIRROR"));
	break;
      }
    }
  connect(myGeomGUI->getApp()->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
}



//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void TransformationGUI_MirrorDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool TransformationGUI_MirrorDlg::ClickOnApply()
{
  if ( !onAccept(GroupPoints->CheckButton1->isChecked()) )
    return false;
  
  initName();
  ConstructorsClicked( getConstructorId() );
  return true;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection has changed
//=================================================================================
void TransformationGUI_MirrorDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("");
  QString aName;

  if(myEditCurrentArgument == GroupPoints->LineEdit1)
    {
      int aNbSel = GEOMBase::GetNameOfSelectedIObjects(selectedIO(), aName);
      if(aNbSel < 1)
	{
	  myObjects.length(0);
	  return;
	}
      GEOMBase::ConvertListOfIOInListOfGO(selectedIO(), myObjects);
      if (!myObjects.length())
	return;
      if(aNbSel != 1)
	aName = tr("%1_objects").arg(aNbSel);
    }
  else if(myEditCurrentArgument == GroupPoints->LineEdit2)
    {
      if(IObjectCount() != 1)
	{
	  myArgument = GEOM::GEOM_Object::_nil();
	  return;
	}
      Standard_Boolean testResult = Standard_False;
      myArgument = GEOMBase::ConvertIOinGEOMObject(firstIObject(), testResult );
      if(!testResult || CORBA::is_nil( myArgument ))
	return;
      aName = GEOMBase::GetName( myArgument );
    }
  myEditCurrentArgument->setText( aName );
   
  displayPreview(); 
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void TransformationGUI_MirrorDlg::LineEditReturnPressed()
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
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void TransformationGUI_MirrorDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();
  
  if(send == GroupPoints->PushButton1){
    myEditCurrentArgument = GroupPoints->LineEdit1;
    globalSelection();
  }
  else if(send == GroupPoints->PushButton2) {
    myEditCurrentArgument = GroupPoints->LineEdit2;
    switch(getConstructorId())
      {
      case 0:
	{
	  globalSelection( GEOM_POINT );
	  break;
	}
      case 1:
	{
	  globalSelection( GEOM_LINE );
	  break;
	}
      case 2:
	{
	  globalSelection( GEOM_PLANE );
	  break;
	}
      }
  }

  myEditCurrentArgument->setFocus();
  SelectionIntoArgument();
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void TransformationGUI_MirrorDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(myGeomGUI->getApp()->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
  ConstructorsClicked( getConstructorId() );
}


//=================================================================================
// function : enterEvent()
// purpose  : when mouse enter onto the QWidget
//=================================================================================
void TransformationGUI_MirrorDlg::enterEvent(QEvent * e)
{
  if(!GroupConstructors->isEnabled())
    ActivateThisDialog();
}


//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr  TransformationGUI_MirrorDlg::createOperation()
{
  return getGeomEngine()->GetITransformOperations( getStudyId() );
}


//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool  TransformationGUI_MirrorDlg::isValid( QString& msg )
{
  return !(myObjects.length() == 0 || myArgument->_is_nil());
}


//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool  TransformationGUI_MirrorDlg::execute( ObjectList& objects )
{
  bool res = false;
  bool toCreateCopy = IsPreview() || GroupPoints->CheckButton1->isChecked();
  
  GEOM::GEOM_Object_var anObj;
  
  switch ( getConstructorId() ) 
    {
    case 0 :
      {
	if (toCreateCopy)
	  for (int i = 0; i < myObjects.length(); i++)
	    {
	      anObj = GEOM::GEOM_ITransformOperations::_narrow( getOperation() )->MirrorPointCopy( myObjects[i], myArgument );
	      if ( !anObj->_is_nil() )
		objects.push_back( anObj._retn() );
	    }
	else
	  for (int i = 0; i < myObjects.length(); i++)
	    {
	      anObj = GEOM::GEOM_ITransformOperations::_narrow( getOperation() )->MirrorPoint( myObjects[i], myArgument );
	      if ( !anObj->_is_nil() )
		objects.push_back( anObj._retn() );
	    }
	res = true;
	break;
      }
    case 1 :
      {
	if (toCreateCopy)
	  for (int i = 0; i < myObjects.length(); i++)
	    {
	      anObj = GEOM::GEOM_ITransformOperations::_narrow( getOperation() )->MirrorAxisCopy( myObjects[i], myArgument );
	      if ( !anObj->_is_nil() )
		objects.push_back( anObj._retn() );
	    }
	else
	  for (int i = 0; i < myObjects.length(); i++)
	    {
	      anObj = GEOM::GEOM_ITransformOperations::_narrow( getOperation() )->MirrorAxis( myObjects[i], myArgument );
	      if ( !anObj->_is_nil() )
		objects.push_back( anObj._retn() );
	    }
	res = true;
	break;
      }
    case 2:
      {
	if (toCreateCopy)
	   for (int i = 0; i < myObjects.length(); i++)
	    {
	      anObj = GEOM::GEOM_ITransformOperations::_narrow( getOperation() )->MirrorPlaneCopy( myObjects[i], myArgument );
	      if ( !anObj->_is_nil() )
		objects.push_back( anObj._retn() );
	    }
	else
	   for (int i = 0; i < myObjects.length(); i++)
	    {
	      anObj = GEOM::GEOM_ITransformOperations::_narrow( getOperation() )->MirrorPlane( myObjects[i], myArgument );
	      if ( !anObj->_is_nil() )
		objects.push_back( anObj._retn() );
	    }
	res = true;
	break;
      }
    }
  
  return res;
}

//=================================================================================
// function : closeEvent
// purpose  :
//=================================================================================
void  TransformationGUI_MirrorDlg::closeEvent( QCloseEvent* e )
{
  GEOMBase_Skeleton::closeEvent( e );
}


//=================================================================================
// function :  CreateCopyModeChanged()
// purpose  :
//=================================================================================
void TransformationGUI_MirrorDlg::CreateCopyModeChanged(bool isCreateCopy)
{
  this->GroupBoxName->setEnabled(isCreateCopy);
}
