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
//  File   : TransformationGUI_OffsetDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#include "TransformationGUI_OffsetDlg.h"

#include "SUIT_Desktop.h"
#include "SUIT_Session.h"
#include "SalomeApp_Application.h"
#include "LightApp_SelectionMgr.h"

#include <qlabel.h>
#include <qcheckbox.h>

#include "utilities.h"

using namespace std;

//=================================================================================
// class    : TransformationGUI_OffsetDlg()
// purpose  : Constructs a TransformationGUI_OffsetDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
TransformationGUI_OffsetDlg::TransformationGUI_OffsetDlg(QWidget* parent,  const char* name, bool modal, WFlags fl)
    :GEOMBase_Skeleton(parent, name, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_DLG_OFFSET")));
  QPixmap image1(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_OFFSET_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_OFFSET"));
  RadioButton1->setPixmap(image0);
  RadioButton2->close(TRUE);
  RadioButton3->close(TRUE);
 
  GroupPoints = new DlgRef_1Sel1Spin1Check(this, "GroupPoints");
  GroupPoints->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  GroupPoints->TextLabel1->setText(tr("GEOM_OBJECTS"));
  GroupPoints->TextLabel2->setText(tr("GEOM_OFFSET"));
  GroupPoints->CheckButton1->setText(tr("GEOM_CREATE_COPY"));

  // san -- modification of an exisitng object by offset is not allowed
  GroupPoints->CheckButton1->hide();

  GroupPoints->PushButton1->setPixmap(image1);
  
  Layout1->addWidget(GroupPoints, 2, 0);
  
  /***************************************************************/
  
  Init();
}


//=================================================================================
// function : ~TransformationGUI_OffsetDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
TransformationGUI_OffsetDlg::~TransformationGUI_OffsetDlg()
{  
  /* no need to delete child widgets, Qt does it all for us */
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void TransformationGUI_OffsetDlg::Init()
{  
  /* init variables */
  myEditCurrentArgument = GroupPoints->LineEdit1;
  GroupPoints->LineEdit1->setReadOnly( true );
  
  /* Get setting of step value from file configuration */
  double step = 1;
   
  /* min, max, step and decimals for spin boxes & initial values */
  GroupPoints->SpinBox_DX->RangeStepAndValidator(-999.999, 999.999, step, 3);
  GroupPoints->SpinBox_DX->setPrecision(5);
  //@ GroupPoints->SpinBox_DX->setDblPrecision(1e-05);    
  GroupPoints->SpinBox_DX->SetValue(1e-05);
  
  // Activate Create a Copy mode
  GroupPoints->CheckButton1->setChecked(true);
  CreateCopyModeChanged(true);

  /* signals and slots connections */
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));
  
  connect(GroupPoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  connect(GroupPoints->SpinBox_DX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox()));
  connect(GroupPoints->CheckButton1, SIGNAL(toggled(bool)), this, SLOT(CreateCopyModeChanged(bool)));
  
  initName( tr( "GEOM_OFFSET" ) );

  globalSelection( GEOM_ALLSHAPES );
  
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void TransformationGUI_OffsetDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool TransformationGUI_OffsetDlg::ClickOnApply()
{
  if ( !onAccept( GroupPoints->CheckButton1->isChecked() ) )
    return false;
  
  initName();
  return true;
}


//=======================================================================
// function : ClickOnCancel()
// purpose  :
//=======================================================================
void TransformationGUI_OffsetDlg::ClickOnCancel()
{
  GEOMBase_Skeleton::ClickOnCancel();
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection has changed
//=================================================================================
void TransformationGUI_OffsetDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("");
  QString aName;

  int aNbSel = GEOMBase::GetNameOfSelectedIObjects(selectedIO(), aName);
  if(aNbSel < 1)
    {
      myObjects.length(0);
      return;
    }

  // nbSel > 0
  GEOMBase::ConvertListOfIOInListOfGO(selectedIO(), myObjects);
  if (!myObjects.length())
    return;
  
  myEditCurrentArgument->setText( aName );
  
  displayPreview();
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void TransformationGUI_OffsetDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if ( send == GroupPoints->LineEdit1 )
  {
    myEditCurrentArgument = GroupPoints->LineEdit1;
    GEOMBase_Skeleton::LineEditReturnPressed();
  }
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void TransformationGUI_OffsetDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();
  
  if(send == GroupPoints->PushButton1)
    {
      myEditCurrentArgument = GroupPoints->LineEdit1;
      myEditCurrentArgument->setFocus();
      SelectionIntoArgument();
    }
}


//=================================================================================
// function : enterEvent()
// purpose  : when mouse enter onto the QWidget
//=================================================================================
void TransformationGUI_OffsetDlg::enterEvent(QEvent * e)
{
  if ( !GroupConstructors->isEnabled() )
    ActivateThisDialog();
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void TransformationGUI_OffsetDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
  globalSelection( GEOM_ALLSHAPES );
  myEditCurrentArgument = GroupPoints->LineEdit1;
  myEditCurrentArgument->setFocus();
}


//=================================================================================
// function : DeactivateActiveDialog()
// purpose  : public slot to deactivate if active
//=================================================================================
void TransformationGUI_OffsetDlg::DeactivateActiveDialog()
{
 GEOMBase_Skeleton::DeactivateActiveDialog();
}


//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void TransformationGUI_OffsetDlg::ValueChangedInSpinBox()
{
  displayPreview();
}


//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr TransformationGUI_OffsetDlg::createOperation()
{
  return getGeomEngine()->GetITransformOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool TransformationGUI_OffsetDlg::isValid( QString& msg )
{
  return !(myObjects.length() == 0);
}


//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool TransformationGUI_OffsetDlg::execute( ObjectList& objects )
{
  bool res = false;
  
  GEOM::GEOM_Object_var anObj;
  
  
  if (GroupPoints->CheckButton1->isChecked() || IsPreview())
    for (int i = 0; i < myObjects.length(); i++)
      {
	anObj = GEOM::GEOM_ITransformOperations::_narrow( getOperation() )->OffsetShapeCopy( myObjects[i], GetOffset() );
	if ( !anObj->_is_nil() )
	  objects.push_back( anObj._retn() );
      }
  else
    for (int i = 0; i < myObjects.length(); i++)
      {
	anObj = GEOM::GEOM_ITransformOperations::_narrow( getOperation() )->OffsetShape( myObjects[i], GetOffset() );
	if ( !anObj->_is_nil() )
	  objects.push_back( anObj._retn() );
      }
     
  res = true;
    
  return res;
}

//=================================================================================
// function : closeEvent
// purpose  :
//=================================================================================
void TransformationGUI_OffsetDlg::closeEvent( QCloseEvent* e )
{
  // myGeomGUI->SetState( -1 );
  GEOMBase_Skeleton::closeEvent( e );
}


//=================================================================================
// function : GetOffset()
// purpose  :
//=================================================================================
double TransformationGUI_OffsetDlg::GetOffset() const
{
  return GroupPoints->SpinBox_DX->GetValue();
}


//=================================================================================
// function :  CreateCopyModeChanged()
// purpose  :
//=================================================================================
void TransformationGUI_OffsetDlg::CreateCopyModeChanged(bool isCreateCopy)
{
  this->GroupBoxName->setEnabled(isCreateCopy);
}
