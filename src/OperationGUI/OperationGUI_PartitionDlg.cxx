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
//  File   : OperationGUI_PartitionDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#include "OperationGUI_PartitionDlg.h"
#include "OperationGUI_MaterialDlg.h"

#include "GEOMImpl_Types.hxx"

#include "SUIT_Desktop.h"
#include "SUIT_Session.h"
#include "SalomeApp_Application.h"
#include "LightApp_SelectionMgr.h"

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlabel.h>

#include "utilities.h"

//=================================================================================
// class    : OperationGUI_PartitionDlg()
// purpose  : Constructs a OperationGUI_PartitionDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
OperationGUI_PartitionDlg::OperationGUI_PartitionDlg(QWidget* parent, const char* name, bool modal, WFlags fl)
  :GEOMBase_Skeleton(parent, name, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_DLG_PARTITION")));
  QPixmap image1(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_DLG_PARTITION_PLANE")));
  QPixmap image2(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_PARTITION_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_PARTITION"));
  RadioButton1->setPixmap(image0);
  RadioButton2->setPixmap(image1);
  RadioButton3->close(TRUE);

  // Full partition (contains half-space partition)
  GroupPoints = new DlgRef_4Sel1List1Check_QTD(this, "GroupPoints");
  GroupPoints->GroupBox1->setTitle(tr("GEOM_PARTITION"));
  GroupPoints->GroupBox2->setTitle(tr("SUPPRESS_RESULT"));
  GroupPoints->TextLabel1->setText(tr("GEOM_OBJECTS"));
  GroupPoints->TextLabel2->setText(tr("GEOM_TOOL_OBJECT"));
  GroupPoints->TextLabel3->setText(tr("GEOM_RECONSTRUCTION_LIMIT"));
  GroupPoints->TextLabel4->setText(tr("GEOM_SUPPRESS_RESULT_INSIDE"));
  GroupPoints->TextLabel5->setText(tr("GEOM_SUPPRESS_RESULT_OUTSIDE"));
  GroupPoints->CheckBox1->setText(tr("GEOM_REMOVE_WEBS"));
  GroupPoints->PushButton1->setPixmap(image2);
  GroupPoints->PushButton2->setPixmap(image2);
  GroupPoints->PushButton3->setPixmap(image2);
  GroupPoints->PushButton4->setPixmap(image2);
  GroupPoints->PushButton5->setText(tr("GEOM_SET_MATERIALS"));
  GroupPoints->LineEdit1->setReadOnly( true );
  GroupPoints->LineEdit2->setReadOnly( true );
  GroupPoints->LineEdit3->setReadOnly( true );
  GroupPoints->LineEdit4->setReadOnly( true );

  Layout1->addWidget(GroupPoints, 2, 0);
  /***************************************************************/
 
  Init();
}


//=================================================================================
// function : ~OperationGUI_PartitionDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
OperationGUI_PartitionDlg::~OperationGUI_PartitionDlg()
{
  // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void OperationGUI_PartitionDlg::Init()
{
  /* type for sub shape selection */
  GroupPoints->ComboBox1->insertItem(tr("GEOM_RECONSTRUCTION_LIMIT_SOLID"));
  GroupPoints->ComboBox1->insertItem(tr("GEOM_RECONSTRUCTION_LIMIT_SHELL"));
  GroupPoints->ComboBox1->insertItem(tr("GEOM_RECONSTRUCTION_LIMIT_FACE"));
  GroupPoints->ComboBox1->insertItem(tr("GEOM_RECONSTRUCTION_LIMIT_WIRE"));
  GroupPoints->ComboBox1->insertItem(tr("GEOM_RECONSTRUCTION_LIMIT_EDGE"));
  GroupPoints->ComboBox1->insertItem(tr("GEOM_RECONSTRUCTION_LIMIT_VERTEX"));
  
  /* signals and slots connections */
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));
  connect(GroupConstructors, SIGNAL(clicked(int)), this, SLOT(ConstructorsClicked(int)));
  
  connect(GroupPoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->PushButton3, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->PushButton4, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->PushButton5, SIGNAL(clicked()), this, SLOT(SetMaterials()));
  
  connect(GroupPoints->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupPoints->LineEdit2, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupPoints->LineEdit3, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupPoints->LineEdit4, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  
  connect(GroupPoints->CheckBox1, SIGNAL(toggled(bool)), this, SLOT(onRemoveWebs(bool)));
  
  connect(GroupPoints->ComboBox1, SIGNAL(activated(int)), this, SLOT(ComboTextChanged()));
  
  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
  
  initName( tr( "GEOM_PARTITION" ) );
  ConstructorsClicked( 0 );
}


//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void OperationGUI_PartitionDlg::ConstructorsClicked(int constructorId)
{
  disconnect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 0, this, 0);
  globalSelection();
  
  myListShapes.length(0);
  myListTools.length(0);  
  myListKeepInside.length(0);
  myListRemoveInside.length(0);
  myListMaterials.length(0);
  
  switch (constructorId)
    {
    case 0: /*Full partition */
      {
	GroupPoints->GroupBox1->setTitle(tr("GEOM_PARTITION"));
	GroupPoints->TextLabel2->setText(tr("GEOM_TOOL_OBJECT"));
	resize(0, 0);
	GroupPoints->TextLabel3->show();
	GroupPoints->ComboBox1->show();
	GroupPoints->GroupBox2->show();
	
	GroupPoints->ComboBox1->setCurrentItem(0);
	GroupPoints->LineEdit3->clear();
	GroupPoints->LineEdit4->clear();
	break;
      }
    case 1: /*Half-space partition */
      {

	GroupPoints->GroupBox1->setTitle(tr("GEOM_PARTITION_HALFSPACE"));
	GroupPoints->GroupBox2->hide();
	GroupPoints->TextLabel3->hide();
	GroupPoints->ComboBox1->hide();
	GroupPoints->TextLabel2->setText(tr("GEOM_PLANE"));
	resize(0, 0);
	break;
      } 
    }
  
  myEditCurrentArgument = GroupPoints->LineEdit1;
  GroupPoints->LineEdit1->clear();
  GroupPoints->LineEdit2->clear();
  onRemoveWebs(false);

  myEditCurrentArgument->setFocus();
  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
  MESSAGE(width()<<" "<<height());
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void OperationGUI_PartitionDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool OperationGUI_PartitionDlg::ClickOnApply()
{
  if ( !onAccept() )
    return false;
  
  initName();
  ConstructorsClicked( getConstructorId() );
  return true;
}


//=======================================================================
// function : ClickOnCancel()
// purpose  :
//=======================================================================
void OperationGUI_PartitionDlg::ClickOnCancel()
{
  GEOMBase_Skeleton::ClickOnCancel();
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void OperationGUI_PartitionDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText( "" );
  QString aString = "";
  
  int nbSel = GEOMBase::GetNameOfSelectedIObjects( selectedIO(), aString, true );
    
  if ( nbSel < 1 )
  {
    if ( myEditCurrentArgument == GroupPoints->LineEdit1 )
    {
      myListShapes.length( 0 );
      myListMaterials.length( 0 );
    }
    else if ( myEditCurrentArgument == GroupPoints->LineEdit2 )
      myListTools.length( 0 );
    else if ( myEditCurrentArgument == GroupPoints->LineEdit3 )
      myListRemoveInside.length( 0 );
    else if ( myEditCurrentArgument == GroupPoints->LineEdit4 )
      myListKeepInside.length( 0 );
  }
  
  // One and only one plane can be selected
  
  if ( getConstructorId() == 1 && myEditCurrentArgument == GroupPoints->LineEdit2 && nbSel != 1 )
  {
    myListTools.length( 0 );
    return;
  }
  
  if ( myEditCurrentArgument == GroupPoints->LineEdit1 )
  {
    GEOMBase::ConvertListOfIOInListOfGO( selectedIO(), myListShapes, true );
    myListMaterials.length( 0 );
    if ( !myListShapes.length() )
      return;
  }
  else if ( myEditCurrentArgument == GroupPoints->LineEdit2 )
  {
    GEOMBase::ConvertListOfIOInListOfGO( selectedIO(), myListTools, true );
    if ( !myListTools.length() )
      return;
  }
  else if(myEditCurrentArgument == GroupPoints->LineEdit3)
  {
    GEOMBase::ConvertListOfIOInListOfGO( selectedIO(), myListRemoveInside, true );
    if (!myListRemoveInside.length())
      return;
  }
  else if(myEditCurrentArgument == GroupPoints->LineEdit4)
  {
    GEOMBase::ConvertListOfIOInListOfGO( selectedIO(),  myListKeepInside, true );
    if ( !myListKeepInside.length() )
      return;
  }
  
  myEditCurrentArgument->setText( aString );
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void OperationGUI_PartitionDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();
  
  if(send == GroupPoints->PushButton1) 
    myEditCurrentArgument = GroupPoints->LineEdit1;
  else if(send == GroupPoints->PushButton2)
  {
    myEditCurrentArgument = GroupPoints->LineEdit2;
    if( getConstructorId()==1 )
      globalSelection( GEOM_PLANE  );
  }
  else if(send == GroupPoints->PushButton3)
    myEditCurrentArgument = GroupPoints->LineEdit3;
  else if(send == GroupPoints->PushButton4)
    myEditCurrentArgument = GroupPoints->LineEdit4;
 
  globalSelection( GEOM_ALLSHAPES );
      
  myEditCurrentArgument->setFocus();
  SelectionIntoArgument();
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void OperationGUI_PartitionDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  
  if(send == GroupPoints->LineEdit1 || 
     send == GroupPoints->LineEdit2 ||
     send == GroupPoints->LineEdit3 ||
     send == GroupPoints->LineEdit4 )
    {
      myEditCurrentArgument = send;
      GEOMBase_Skeleton::LineEditReturnPressed();
    }
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void OperationGUI_PartitionDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  ConstructorsClicked( getConstructorId() ); 
}


//=================================================================================
// function : DeactivateActiveDialog()
// purpose  : public slot to deactivate if active
//=================================================================================
void OperationGUI_PartitionDlg::DeactivateActiveDialog()
{
  GEOMBase_Skeleton::DeactivateActiveDialog();
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void OperationGUI_PartitionDlg::enterEvent(QEvent* e)
{
  if(!GroupConstructors->isEnabled())
    this->ActivateThisDialog();
}


//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr OperationGUI_PartitionDlg::createOperation()
{
  return getGeomEngine()->GetIBooleanOperations( getStudyId() );
}


//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool OperationGUI_PartitionDlg::isValid( QString& msg )
{
  return (myListShapes.length()      || myListTools.length() ||
	  myListKeepInside.length()  || myListRemoveInside.length());
}


//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool OperationGUI_PartitionDlg::execute( ObjectList& objects )
{
  bool res = false;
  
  GEOM::GEOM_Object_var anObj;
  QString msg;
  
  int aLimit = GetLimit();
  int aConstructorId = getConstructorId();
  
  if (aConstructorId==1)
    aLimit = GEOM::SHAPE;
  
  if (isValid( msg ))    
    {
      
      if ( aConstructorId==0 && !toRemoveWebs() )
	myListMaterials.length(0);
      
      anObj = GEOM::GEOM_IBooleanOperations::_narrow( getOperation() )->MakePartition(myListShapes,
										      myListTools,
										      myListKeepInside,
										      myListRemoveInside,
										      aLimit,
										      toRemoveWebs(),
										      myListMaterials);
      res = true;
    }
  
  if ( !anObj->_is_nil() )
    objects.push_back( anObj._retn() );
  
  return res;
}


//=================================================================================
// function : closeEvent
// purpose  :
//=================================================================================
void OperationGUI_PartitionDlg::closeEvent( QCloseEvent* e )
{
  GEOMBase_Skeleton::closeEvent( e );
}


//=======================================================================
//function : ComboTextChanged
//purpose  : 
//=======================================================================

void OperationGUI_PartitionDlg::ComboTextChanged()
{
  bool IsEnabled = GroupPoints->ComboBox1->currentItem() < 3;
  GroupPoints->LineEdit3->setEnabled(IsEnabled);
  GroupPoints->LineEdit4->setEnabled(IsEnabled);
  GroupPoints->TextLabel4->setEnabled(IsEnabled);
  GroupPoints->TextLabel5->setEnabled(IsEnabled);
  GroupPoints->PushButton3->setEnabled(IsEnabled);
  GroupPoints->PushButton4->setEnabled(IsEnabled);
}


//=================================================================================
// function : SetMaterials()
// purpose  : 
//=================================================================================
void OperationGUI_PartitionDlg::SetMaterials()
{
  MESSAGE("OperationGUI_MaterialDlg ...");
  OperationGUI_MaterialDlg *aDlg =
    new OperationGUI_MaterialDlg(this, "", myListShapes, true);	
  MESSAGE("OperationGUI_MaterialDlg");
  return;
}


//=================================================================================
// function : onRemoveWebs()
// purpose  : 
//=================================================================================
void OperationGUI_PartitionDlg::onRemoveWebs(bool isChecked)
{
  if (GroupPoints->CheckBox1->isChecked() != isChecked)
    GroupPoints->CheckBox1->setChecked(isChecked);
  
  GroupPoints->PushButton5->setEnabled(isChecked);
}


//=================================================================================
// function : toRemoveWebs()
// purpose  : 
//=================================================================================
bool OperationGUI_PartitionDlg::toRemoveWebs() const
{
  return GroupPoints->CheckBox1->isChecked();
}


//=================================================================================
// function : GetLimit()
// purpose  : 
//=================================================================================
int OperationGUI_PartitionDlg::GetLimit() const
{
  int aLimit = GroupPoints->ComboBox1->currentItem();
  
  switch(aLimit)
    {
    case 0 : 
      {
	aLimit = GEOM::SOLID;
	break;
      }
    case 1 :
      {
	aLimit = GEOM::SHELL;
	break;
      }
    case 2 :
      {
	aLimit = GEOM::FACE;
	break;
      }
    case 3 :
      {
	aLimit = GEOM::WIRE;
	break;
      }
    case 4 :
      {
	aLimit = GEOM::EDGE;
	break;
      }
    case 5 :
      {
	aLimit = GEOM::VERTEX;
	break;
      }
    default :
      aLimit = GEOM::SHAPE;
    }
  
  return aLimit;
}
