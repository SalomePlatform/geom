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
//  File   : GenerationGUI_FillingDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#include "GenerationGUI_FillingDlg.h"

#include "SUIT_Desktop.h"
#include "SUIT_Session.h"
#include "SalomeApp_Application.h"
#include "LightApp_SelectionMgr.h"

#include <GeomFill_SectionGenerator.hxx>
#include <GeomFill_Line.hxx>
#include <GeomFill_AppSurf.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Iterator.hxx>
#include <BRep_Tool.hxx>
#include <Precision.hxx>
#include <Standard_ErrorHandler.hxx>
#include "GEOMImpl_Types.hxx"

#include <qlabel.h>

#include "utilities.h"

//=================================================================================
// class    : GenerationGUI_FillingDlg()
// purpose  : Constructs a GenerationGUI_FillingDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
GenerationGUI_FillingDlg::GenerationGUI_FillingDlg(GeometryGUI* theGeometryGUI, QWidget* parent,
                                                   const char* name, bool modal, WFlags fl)
  :GEOMBase_Skeleton(parent, name, modal,
                     WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu),
   myGeometryGUI(theGeometryGUI)
{
  QPixmap image0 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_FILLING")));
  QPixmap image1 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_SELECT")));

  setCaption(tr("GEOM_FILLING_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_FILLING"));
  RadioButton1->setPixmap(image0);
  RadioButton2->close(TRUE);
  RadioButton3->close(TRUE);

  GroupPoints = new DlgRef_1Sel5Spin(this, "GroupPoints");
  GroupPoints->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  GroupPoints->TextLabel1->setText(tr("GEOM_FILLING_COMPOUND"));
  GroupPoints->TextLabel2->setText(tr("GEOM_FILLING_MIN_DEG"));
  GroupPoints->TextLabel3->setText(tr("GEOM_FILLING_TOL_2D"));
  GroupPoints->TextLabel4->setText(tr("GEOM_FILLING_NB_ITER"));
  GroupPoints->TextLabel5->setText(tr("GEOM_FILLING_MAX_DEG"));
  GroupPoints->TextLabel6->setText(tr("GEOM_FILLING_TOL_3D"));
  GroupPoints->PushButton1->setPixmap(image1);
  GroupPoints->LineEdit1->setReadOnly( true );

  Layout1->addWidget(GroupPoints, 2, 0);
  /***************************************************************/

  /* Initialisations */
  Init();
}

//=================================================================================
// function : ~GenerationGUI_FillingDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
GenerationGUI_FillingDlg::~GenerationGUI_FillingDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void GenerationGUI_FillingDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupPoints->LineEdit1;
  GroupPoints->LineEdit1->setReadOnly( true );

  myMinDeg = 2;
  myMaxDeg = 5;
  myTol3D = 0.0001;
  myTol2D = 0.0001;
  myNbIter = 5;
  myOkCompound = false;

  globalSelection( GEOM_COMPOUND );

  double SpecificStep1 = 1;
  double SpecificStep2 = 0.0001;
  /* min, max, step and decimals for spin boxes & initial values */
  GroupPoints->SpinBox_1->RangeStepAndValidator(2.0, 999.999, SpecificStep1, 3);
  GroupPoints->SpinBox_2->RangeStepAndValidator(0.00001, 10000.0, SpecificStep2, 5);
  GroupPoints->SpinBox_3->RangeStepAndValidator(1.0, 999.999, SpecificStep1, 3);
  GroupPoints->SpinBox_4->RangeStepAndValidator(1.0, 999.999, SpecificStep1, 3);
  GroupPoints->SpinBox_5->RangeStepAndValidator(0.00001, 10000.0, SpecificStep2, 5);

  GroupPoints->SpinBox_1->SetValue(myMinDeg);
  GroupPoints->SpinBox_2->SetValue(myTol2D);
  GroupPoints->SpinBox_3->SetValue(myNbIter);
  GroupPoints->SpinBox_4->SetValue(myMaxDeg);
  GroupPoints->SpinBox_5->SetValue(myTol3D);

  /* signals and slots connections */
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(GroupPoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(GroupPoints->SpinBox_1, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupPoints->SpinBox_2, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupPoints->SpinBox_3, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupPoints->SpinBox_4, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupPoints->SpinBox_5, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));

  connect(myGeometryGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupPoints->SpinBox_1, SLOT(SetStep(double)));
  connect(myGeometryGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupPoints->SpinBox_2, SLOT(SetStep(double)));
  connect(myGeometryGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupPoints->SpinBox_3, SLOT(SetStep(double)));
  connect(myGeometryGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupPoints->SpinBox_4, SLOT(SetStep(double)));
  connect(myGeometryGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupPoints->SpinBox_5, SLOT(SetStep(double)));

  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(),
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument())) ;

  initName(tr("GEOM_FILLING"));
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void GenerationGUI_FillingDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool GenerationGUI_FillingDlg::ClickOnApply()
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
void GenerationGUI_FillingDlg::SelectionIntoArgument()
{
  erasePreview();
  myEditCurrentArgument->setText("");

  if(IObjectCount() != 1) {
    if(myEditCurrentArgument == GroupPoints->LineEdit1)
      myOkCompound = false;
    return;
  }

  // nbSel == 1
  Standard_Boolean testResult = Standard_False;
  GEOM::GEOM_Object_ptr aSelectedObject = GEOMBase::ConvertIOinGEOMObject(firstIObject(), testResult );

  if (!testResult)
    return;

  if(myEditCurrentArgument == GroupPoints->LineEdit1) {
    TopoDS_Shape S;
    myOkCompound = false;

    if ( GEOMBase::GetShape(aSelectedObject, S) &&
	 S.ShapeType() == TopAbs_COMPOUND)
      {
	// myCompound should be a compound of edges
	for ( TopoDS_Iterator it( S ); it.More(); it.Next() )
	  if ( it.Value().ShapeType() != TopAbs_EDGE )
	    return;
	myCompound = aSelectedObject;
	myOkCompound = true;
      }
  }

  myEditCurrentArgument->setText( GEOMBase::GetName( aSelectedObject ) );
  displayPreview();
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void GenerationGUI_FillingDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();
  globalSelection( GEOM_ALLSHAPES );

  if(send == GroupPoints->PushButton1) {
    GroupPoints->LineEdit1->setFocus();
    myEditCurrentArgument = GroupPoints->LineEdit1;
    globalSelection( GEOM_COMPOUND );
    this->SelectionIntoArgument();
  }
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void GenerationGUI_FillingDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if(send == GroupPoints->LineEdit1)
    {
      myEditCurrentArgument = send;
      GEOMBase_Skeleton::LineEditReturnPressed();
    }
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void GenerationGUI_FillingDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(),
          SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
  globalSelection( GEOM_COMPOUND );
  displayPreview();
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void GenerationGUI_FillingDlg::enterEvent(QEvent* e)
{
  if ( !GroupConstructors->isEnabled() )
    ActivateThisDialog();
}


//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void GenerationGUI_FillingDlg::ValueChangedInSpinBox(double newValue)
{
  QObject* send = (QObject*)sender();

  if(send == GroupPoints->SpinBox_1)
    myMinDeg = int(newValue);
  else if(send == GroupPoints->SpinBox_2)
    myTol2D = newValue;
  else if(send == GroupPoints->SpinBox_3)
    myNbIter = int(newValue);
  else if(send == GroupPoints->SpinBox_4)
    myMaxDeg = int(newValue);
  else if(send == GroupPoints->SpinBox_5)
    myTol3D = newValue;

  displayPreview();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr GenerationGUI_FillingDlg::createOperation()
{
  return getGeomEngine()->GetI3DPrimOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool GenerationGUI_FillingDlg::isValid( QString& )
{
  return myOkCompound > 0;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool GenerationGUI_FillingDlg::execute( ObjectList& objects )
{
  GEOM::GEOM_Object_var anObj;

  anObj = GEOM::GEOM_I3DPrimOperations::_narrow(getOperation() )->MakeFilling(
    myCompound, myMinDeg, myMaxDeg, myTol2D, myTol3D, myNbIter );

  if ( !anObj->_is_nil() )
    objects.push_back( anObj._retn() );

  return true;
}
