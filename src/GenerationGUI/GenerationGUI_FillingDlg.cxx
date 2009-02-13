//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// GEOM GEOMGUI : GUI for Geometry component
// File   : GenerationGUI_FillingDlg.cxx
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.
//
#include "GenerationGUI_FillingDlg.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>

#include <TopoDS_Iterator.hxx>
#include <GEOMImpl_Types.hxx>

//=================================================================================
// class    : GenerationGUI_FillingDlg()
// purpose  : Constructs a GenerationGUI_FillingDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
GenerationGUI_FillingDlg::GenerationGUI_FillingDlg( GeometryGUI* theGeometryGUI, QWidget* parent,
						    bool modal, Qt::WindowFlags fl )
  : GEOMBase_Skeleton( theGeometryGUI, parent, modal, fl )
{
  QPixmap image0( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_FILLING" ) ) );
  QPixmap image1( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );

  setWindowTitle( tr( "GEOM_FILLING_TITLE" ) );

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle( tr ( "GEOM_FILLING" ) );
  mainFrame()->RadioButton1->setIcon( image0 );
  mainFrame()->RadioButton2->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton3->close();

  GroupPoints = new DlgRef_1Sel5Spin1Check( centralWidget() );
  GroupPoints->GroupBox1->setTitle( tr( "GEOM_ARGUMENTS" ) );
  GroupPoints->TextLabel1->setText( tr( "GEOM_FILLING_COMPOUND" ) );
  GroupPoints->TextLabel2->setText( tr( "GEOM_FILLING_MIN_DEG" ) );
  GroupPoints->TextLabel3->setText( tr( "GEOM_FILLING_TOL_2D" ) );
  GroupPoints->TextLabel4->setText( tr( "GEOM_FILLING_NB_ITER" ) );
  GroupPoints->TextLabel5->setText( tr( "GEOM_FILLING_MAX_DEG" ) );
  GroupPoints->TextLabel6->setText( tr( "GEOM_FILLING_TOL_3D" ) );
  GroupPoints->CheckBox1->setText( tr( "GEOM_FILLING_APPROX" ) );
  GroupPoints->PushButton1->setIcon( image1 );
  GroupPoints->LineEdit1->setReadOnly( true );

  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( GroupPoints  );
  /***************************************************************/

  setHelpFileName( "create_filling_page.html" );

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
  myNbIter = 0;
  myIsApprox = false;
  myOkCompound = false;

  globalSelection( GEOM_COMPOUND );

  double SpecificStep1 = 1;
  double SpecificStep2 = 0.0001;
  /* min, max, step and decimals for spin boxes & initial values */
  initSpinBox( GroupPoints->SpinBox1, 2.0, MAX_NUMBER,  SpecificStep1, 3 );
  initSpinBox( GroupPoints->SpinBox2, 0.00001, 10000.0, SpecificStep2, 5 );
  initSpinBox( GroupPoints->SpinBox3, 0.0, MAX_NUMBER,  SpecificStep1, 3 );
  initSpinBox( GroupPoints->SpinBox4, 1.0, MAX_NUMBER,  SpecificStep1, 3 );
  initSpinBox( GroupPoints->SpinBox5, 0.00001, 10000.0, SpecificStep2, 5 );

  GroupPoints->SpinBox1->setValue( myMinDeg );
  GroupPoints->SpinBox2->setValue( myTol2D );
  GroupPoints->SpinBox3->setValue( myNbIter );
  GroupPoints->SpinBox4->setValue( myMaxDeg );
  GroupPoints->SpinBox5->setValue( myTol3D );

  /* signals and slots connections */
  connect( buttonOk(),    SIGNAL( clicked() ), this, SLOT( ClickOnOk() ) );
  connect( buttonApply(), SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );

  connect( GroupPoints->PushButton1, SIGNAL( clicked() ),       this, SLOT( SetEditCurrentArgument() ) );
  connect( GroupPoints->LineEdit1,   SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );

  connect( GroupPoints->SpinBox1, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( GroupPoints->SpinBox2, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( GroupPoints->SpinBox3, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( GroupPoints->SpinBox4, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );
  connect( GroupPoints->SpinBox5, SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );

  connect( GroupPoints->CheckBox1, SIGNAL( stateChanged( int ) ), this, SLOT( ApproxChanged() ) );

  connect( myGeomGUI, SIGNAL( SignalDefaultStepValueChanged( double ) ), this, SLOT( SetDoubleSpinBoxStep( double ) ) );

  connect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr(),
	   SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );

  initName( tr( "GEOM_FILLING" ) );
}

//=================================================================================
// function : SetDoubleSpinBoxStep()
// purpose  : Double spin box management
//=================================================================================
void GenerationGUI_FillingDlg::SetDoubleSpinBoxStep( double step )
{
  GroupPoints->SpinBox1->setSingleStep(step);
  GroupPoints->SpinBox2->setSingleStep(step);
  GroupPoints->SpinBox3->setSingleStep(step);
  GroupPoints->SpinBox4->setSingleStep(step);
  GroupPoints->SpinBox5->setSingleStep(step);
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

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  if (aSelList.Extent() != 1) {
    if (myEditCurrentArgument == GroupPoints->LineEdit1)
      myOkCompound = false;
    return;
  }

  // nbSel == 1
  Standard_Boolean testResult = Standard_False;
  GEOM::GEOM_Object_ptr aSelectedObject = GEOMBase::ConvertIOinGEOMObject(aSelList.First(), testResult);

  if (!testResult)
    return;

  if (myEditCurrentArgument == GroupPoints->LineEdit1) {
    TopoDS_Shape S;
    myOkCompound = false;

    if (GEOMBase::GetShape(aSelectedObject, S) && S.ShapeType() == TopAbs_COMPOUND) {
      // myCompound should be a compound of edges
      for (TopoDS_Iterator it (S); it.More(); it.Next())
        if (it.Value().ShapeType() != TopAbs_EDGE)
          return;
      myCompound = aSelectedObject;
      myOkCompound = true;
    }
  }

  myEditCurrentArgument->setText(GEOMBase::GetName(aSelectedObject));
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

  if ( send == GroupPoints->PushButton1 ) {
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
  if ( send == GroupPoints->LineEdit1 ) {
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
  connect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr(),
	   SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );
  globalSelection( GEOM_COMPOUND );
  displayPreview();
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void GenerationGUI_FillingDlg::enterEvent( QEvent* )
{
  if ( !mainFrame()->GroupConstructors->isEnabled() )
    ActivateThisDialog();
}


//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void GenerationGUI_FillingDlg::ValueChangedInSpinBox( double newValue )
{
  QObject* send = (QObject*)sender();

  if ( send == GroupPoints->SpinBox1 )
    myMinDeg = int( newValue );
  else if ( send == GroupPoints->SpinBox2 )
    myTol2D = newValue;
  else if ( send == GroupPoints->SpinBox3 )
    myNbIter = int(newValue);
  else if ( send == GroupPoints->SpinBox4 )
    myMaxDeg = int(newValue);
  else if ( send == GroupPoints->SpinBox5 )
    myTol3D = newValue;

  displayPreview();
}

//=================================================================================
// function : ApproxChanged()
// purpose  :
//=================================================================================
void GenerationGUI_FillingDlg::ApproxChanged()
{
  myIsApprox = GroupPoints->CheckBox1->isChecked();
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
bool GenerationGUI_FillingDlg::isValid( QString& msg )
{
  bool ok = true;
  ok = GroupPoints->SpinBox1->isValid( msg, !IsPreview() ) && ok;
  ok = GroupPoints->SpinBox2->isValid( msg, !IsPreview() ) && ok;
  ok = GroupPoints->SpinBox3->isValid( msg, !IsPreview() ) && ok;
  ok = GroupPoints->SpinBox4->isValid( msg, !IsPreview() ) && ok;
  ok = GroupPoints->SpinBox5->isValid( msg, !IsPreview() ) && ok;

  return myOkCompound > 0 && ok;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool GenerationGUI_FillingDlg::execute( ObjectList& objects )
{
  GEOM::GEOM_Object_var anObj;

  anObj = GEOM::GEOM_I3DPrimOperations::_narrow(getOperation() )->MakeFilling(
    myCompound, myMinDeg, myMaxDeg, myTol2D, myTol3D, myNbIter, myIsApprox );
  if ( !anObj->_is_nil() )
  {
    if ( !IsPreview() )
    {
      QStringList aParameters;
      aParameters << GroupPoints->SpinBox1->text();
      aParameters << GroupPoints->SpinBox2->text();
      aParameters << GroupPoints->SpinBox3->text();
      aParameters << GroupPoints->SpinBox4->text();
      aParameters << GroupPoints->SpinBox5->text();
      anObj->SetParameters(GeometryGUI::JoinObjectParameters(aParameters));
    }
    objects.push_back( anObj._retn() );
  }

  return true;
}
