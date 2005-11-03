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
//  File   : GenerationGUI_RevolDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#include "GenerationGUI_RevolDlg.h"

#include "SUIT_Desktop.h"
#include "SUIT_Session.h"
#include "SalomeApp_Application.h"
#include "LightApp_SelectionMgr.h"

#include <gp_Lin.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepPrimAPI_MakeRevol.hxx>
#include <TopExp_Explorer.hxx>
#include <Standard_ErrorHandler.hxx>
#include "GEOMImpl_Types.hxx"

#include <qlabel.h>
#include <qcheckbox.h>

#include "utilities.h"

//=================================================================================
// class    : GenerationGUI_RevolDlg()
// purpose  : Constructs a GenerationGUI_RevolDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
GenerationGUI_RevolDlg::GenerationGUI_RevolDlg(GeometryGUI* theGeometryGUI, QWidget* parent, const char* name, bool modal, WFlags fl)
  :GEOMBase_Skeleton(parent, name, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu), myGeometryGUI(theGeometryGUI)
{
  QPixmap image0(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_DLG_REVOL")));
  QPixmap image1(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_REVOLUTION_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_REVOLUTION"));
  RadioButton1->setPixmap(image0);
  RadioButton2->close(TRUE);
  RadioButton3->close(TRUE);

  GroupPoints = new DlgRef_2Sel1Spin2Check(this, "GroupPoints");
  GroupPoints->CheckButton1->hide();
  GroupPoints->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  GroupPoints->TextLabel1->setText(tr("GEOM_OBJECT"));
  GroupPoints->TextLabel2->setText(tr("GEOM_AXIS"));
  GroupPoints->TextLabel3->setText(tr("GEOM_ANGLE"));
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
// function : ~GenerationGUI_RevolDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
GenerationGUI_RevolDlg::~GenerationGUI_RevolDlg()
{
  // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void GenerationGUI_RevolDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupPoints->LineEdit1;
  GroupPoints->LineEdit1->setReadOnly( true );
  GroupPoints->LineEdit2->setReadOnly( true );

  myOkBase = myOkAxis = false;

  double SpecificStep = 5;
  /* min, max, step and decimals for spin boxes & initial values */
  GroupPoints->SpinBox_DX->RangeStepAndValidator(-999.999, 999.999, SpecificStep, 3);
  GroupPoints->SpinBox_DX->SetValue(45.0);

  /* signals and slots connections */
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(GroupPoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(GroupPoints->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupPoints->LineEdit2, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(GroupPoints->SpinBox_DX,   SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox()));
  connect(GroupPoints->CheckButton2, SIGNAL(toggled(bool)),        this, SLOT(onReverse()));

  connect(myGeometryGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupPoints->SpinBox_DX, SLOT(SetStep(double)));
   
  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument())) ;

  initName(tr("GEOM_REVOLUTION"));

  globalSelection( GEOM_ALLSHAPES );
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void GenerationGUI_RevolDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool GenerationGUI_RevolDlg::ClickOnApply()
{
  if ( !onAccept() )
    return false;

  initName();
  return true;
}

//=======================================================================
//function : isAcceptableBase
//purpose  : return true if theBase can be used as algo argument
//=======================================================================

static bool isAcceptableBase(const TopoDS_Shape& theBase)
{
  switch ( theBase.ShapeType() ) {
  case TopAbs_VERTEX:
  case TopAbs_EDGE:
  case TopAbs_WIRE:
  case TopAbs_FACE:
  case TopAbs_SHELL:
    return true;
  case TopAbs_SOLID:
  case TopAbs_COMPSOLID:
    return false;
  case TopAbs_COMPOUND: {
    TopExp_Explorer exp( theBase, TopAbs_SOLID);
    return !exp.More();
  }
  default:
    return false;
  }
  return false;
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void GenerationGUI_RevolDlg::SelectionIntoArgument()
{
  erasePreview();
  myEditCurrentArgument->setText("");
  
  if(IObjectCount() != 1) {
    if(myEditCurrentArgument == GroupPoints->LineEdit1)
      myOkBase = false;        
    else if(myEditCurrentArgument == GroupPoints->LineEdit2)
      myOkAxis = false;
    return;
  }
  
  // nbSel == 1
  Standard_Boolean testResult = Standard_False;
  GEOM::GEOM_Object_ptr aSelectedObject = GEOMBase::ConvertIOinGEOMObject( firstIObject(), testResult );
  
  if (!testResult)
    return;

  if(myEditCurrentArgument == GroupPoints->LineEdit1) {
    TopoDS_Shape S;
    myOkBase = false;
    
    if ( !GEOMBase::GetShape(aSelectedObject, S) || !isAcceptableBase( S ) )
      return;
    
    myBase = aSelectedObject;
    myOkBase = true;
  }
  else if(myEditCurrentArgument == GroupPoints->LineEdit2) {
    myAxis = aSelectedObject;
    myOkAxis = true;
  }
  myEditCurrentArgument->setText( GEOMBase::GetName( aSelectedObject ) );

  displayPreview();
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void GenerationGUI_RevolDlg::SetEditCurrentArgument()
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
void GenerationGUI_RevolDlg::LineEditReturnPressed()
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
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void GenerationGUI_RevolDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  globalSelection( GEOM_ALLSHAPES );
  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
  GroupPoints->LineEdit1->setFocus();
  myEditCurrentArgument = GroupPoints->LineEdit1;
  displayPreview();
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void GenerationGUI_RevolDlg::enterEvent(QEvent* e)
{
  if ( !GroupConstructors->isEnabled() )
    ActivateThisDialog();
}


//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void GenerationGUI_RevolDlg::ValueChangedInSpinBox()
{
  displayPreview();
}


//=================================================================================
// function : getAngle()
// purpose  :
//=================================================================================
double GenerationGUI_RevolDlg::getAngle() const
{
  return GroupPoints->SpinBox_DX->GetValue();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr GenerationGUI_RevolDlg::createOperation()
{
  return getGeomEngine()->GetI3DPrimOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool GenerationGUI_RevolDlg::isValid( QString& )
{
  return myOkBase && myOkAxis;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool GenerationGUI_RevolDlg::execute( ObjectList& objects )
{
  GEOM::GEOM_Object_var anObj;

  anObj = GEOM::GEOM_I3DPrimOperations::_narrow(
    getOperation() )->MakeRevolutionAxisAngle( myBase, myAxis, getAngle() * PI180 );

  if ( !anObj->_is_nil() )
    objects.push_back( anObj._retn() );

  return true;
}


//=================================================================================
// function :  onReverse()
// purpose  :
//=================================================================================
void GenerationGUI_RevolDlg::onReverse()
{
  double anOldValue = GroupPoints->SpinBox_DX->GetValue();
  GroupPoints->SpinBox_DX->SetValue( -anOldValue );
}
