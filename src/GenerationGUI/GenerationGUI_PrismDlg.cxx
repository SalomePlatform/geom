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
//  File   : GenerationGUI_PrismDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#include "GenerationGUI_PrismDlg.h"

#include "SUIT_Desktop.h"
#include "SUIT_Session.h"
#include "SalomeApp_Application.h"
#include "LightApp_SelectionMgr.h"

#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <gp_Lin.hxx>
#include "GEOMImpl_Types.hxx"

#include <qlabel.h>
#include <qcheckbox.h>

#include "utilities.h"

//=================================================================================
// class    : GenerationGUI_PrismDlg()
// purpose  : Constructs a GenerationGUI_PrismDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
GenerationGUI_PrismDlg::GenerationGUI_PrismDlg(GeometryGUI* theGeometryGUI, QWidget* parent, const char* name, bool modal, WFlags fl)
  :GEOMBase_Skeleton(parent, name, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu), myGeometryGUI(theGeometryGUI)
{
  QPixmap image0(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_DLG_PRISM")));
  QPixmap image1(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_PRISM_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_PRISM"));
  RadioButton1->setPixmap(image0);
  RadioButton2->close(TRUE);
  RadioButton3->close(TRUE);

  GroupPoints = new DlgRef_2Sel1Spin2Check(this, "GroupPoints");
  GroupPoints->CheckButton1->hide();
  GroupPoints->GroupBox1->setTitle(tr("GEOM_PRISM_BSV"));
  GroupPoints->TextLabel1->setText(tr("GEOM_BASE"));
  GroupPoints->TextLabel2->setText(tr("GEOM_VECTOR"));
  GroupPoints->TextLabel3->setText(tr("GEOM_HEIGHT"));
  GroupPoints->PushButton1->setPixmap(image1);
  GroupPoints->PushButton2->setPixmap(image1);
  GroupPoints->LineEdit1->setReadOnly( true );
  GroupPoints->LineEdit2->setReadOnly( true );
  GroupPoints->CheckButton2->setText(tr("GEOM_REVERSE"));

  Layout1->addWidget(GroupPoints, 2, 0);
  /***************************************************************/

  /* Initialisations */
  Init();
}


//=================================================================================
// function : ~GenerationGUI_PrismDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
GenerationGUI_PrismDlg::~GenerationGUI_PrismDlg()
{  
  /* no need to delete child widgets, Qt does it all for us */
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void GenerationGUI_PrismDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupPoints->LineEdit1;
  GroupPoints->LineEdit1->setReadOnly( true );
  GroupPoints->LineEdit2->setReadOnly( true );

  myOkBase = myOkVec = false;
  
  /* Get setting of step value from file configuration */
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  double step = resMgr->doubleValue( "Geometry", "SettingsGeomStep", 100);

  /* min, max, step and decimals for spin boxes & initial values */
  GroupPoints->SpinBox_DX->RangeStepAndValidator(-999.999, +999.999, step, 3);
  GroupPoints->SpinBox_DX->SetValue(100.0);

  /* signals and slots connections */
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(GroupPoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(GroupPoints->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupPoints->LineEdit2, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(GroupPoints->SpinBox_DX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox()));
  connect(myGeometryGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupPoints->SpinBox_DX, SLOT(SetStep(double)));

  connect(GroupPoints->CheckButton2, SIGNAL(toggled(bool)),      this, SLOT(onReverse()));
   
  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument())) ;

  initName(tr("GEOM_PRISM"));

  globalSelection( GEOM_ALLSHAPES );
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void GenerationGUI_PrismDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool GenerationGUI_PrismDlg::ClickOnApply()
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
void GenerationGUI_PrismDlg::SelectionIntoArgument()
{
  erasePreview();
  myEditCurrentArgument->setText("");
  
  if(IObjectCount() != 1) 
    {
      if(myEditCurrentArgument == GroupPoints->LineEdit1)
	myOkBase = false;
      else if(myEditCurrentArgument == GroupPoints->LineEdit2)
	myOkVec = false;
      return;
    }
  
  // nbSel == 1
  Standard_Boolean testResult = Standard_False;
  GEOM::GEOM_Object_ptr aSelectedObject = GEOMBase::ConvertIOinGEOMObject( firstIObject(), testResult );
  
  if (!testResult)
    return;

  if(myEditCurrentArgument == GroupPoints->LineEdit1) {
    myOkBase = false;
    TopoDS_Shape S;
    
    if ( !GEOMBase::GetShape(aSelectedObject, S) ||
	 S.ShapeType() <= 2)
      return;
    
    myBase = aSelectedObject;
    myOkBase = true;
  }
  else if(myEditCurrentArgument == GroupPoints->LineEdit2) {
    myVec = aSelectedObject;
    myOkVec = true;
  }
  myEditCurrentArgument->setText( GEOMBase::GetName( aSelectedObject ) );
  
  displayPreview();
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void GenerationGUI_PrismDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();
  globalSelection( GEOM_ALLSHAPES );

  if(send == GroupPoints->PushButton1) {
    GroupPoints->LineEdit1->setFocus();
    myEditCurrentArgument = GroupPoints->LineEdit1;
  }
  else if(send == GroupPoints->PushButton2) {
    GroupPoints->LineEdit2->setFocus();
    myEditCurrentArgument = GroupPoints->LineEdit2;
    globalSelection( GEOM_LINE );
  }
  SelectionIntoArgument();
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void GenerationGUI_PrismDlg::LineEditReturnPressed()
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
void GenerationGUI_PrismDlg::enterEvent(QEvent * e)
{
  if ( !GroupConstructors->isEnabled() )
    ActivateThisDialog();
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void GenerationGUI_PrismDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  globalSelection( GEOM_ALLSHAPES );
  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
  GroupPoints->LineEdit1->setFocus();
  myEditCurrentArgument = GroupPoints->LineEdit1;
  displayPreview();
}


//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void GenerationGUI_PrismDlg::ValueChangedInSpinBox()
{
  displayPreview();
}



//=================================================================================
// function : getHeight()
// purpose  :
//=================================================================================
double GenerationGUI_PrismDlg::getHeight() const
{
  return GroupPoints->SpinBox_DX->GetValue();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr GenerationGUI_PrismDlg::createOperation()
{
  return getGeomEngine()->GetI3DPrimOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool GenerationGUI_PrismDlg::isValid( QString& )
{
  return myOkBase && myOkVec;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool GenerationGUI_PrismDlg::execute( ObjectList& objects )
{
  GEOM::GEOM_Object_var anObj;
  
  anObj = GEOM::GEOM_I3DPrimOperations::_narrow(getOperation() )->MakePrismVecH ( myBase, myVec, getHeight() );
  
  if ( !anObj->_is_nil() )
    objects.push_back( anObj._retn() );

  return true;
}


//=================================================================================
// function :  onReverse()
// purpose  :
//=================================================================================
void GenerationGUI_PrismDlg::onReverse()
{
  double anOldValue = GroupPoints->SpinBox_DX->GetValue();
  GroupPoints->SpinBox_DX->SetValue( -anOldValue );
}



