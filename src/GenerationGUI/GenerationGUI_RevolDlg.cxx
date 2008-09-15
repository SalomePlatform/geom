// GEOM GEOMGUI : GUI for Geometry component
//
// Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
// 
// This library is free software; you can redistribute it and/or 
// modify it under the terms of the GNU Lesser General Public 
// License as published by the Free Software Foundation; either 
// version 2.1 of the License. 
// 
// This library is distributed in the hope that it will be useful, 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
// Lesser General Public License for more details. 
// 
// You should have received a copy of the GNU Lesser General Public 
// License along with this library; if not, write to the Free Software 
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
// 
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// File   : GenerationGUI_RevolDlg.cxx
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.
//

#include "GenerationGUI_RevolDlg.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>

#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp_Explorer.hxx>

#include <GEOMImpl_Types.hxx>

//=================================================================================
// class    : GenerationGUI_RevolDlg()
// purpose  : Constructs a GenerationGUI_RevolDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
GenerationGUI_RevolDlg::GenerationGUI_RevolDlg( GeometryGUI* theGeometryGUI, QWidget* parent,
						bool modal, Qt::WindowFlags fl )
  : GEOMBase_Skeleton( theGeometryGUI, parent, modal, fl )
{
  QPixmap image0( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_REVOL" ) ) );
  QPixmap image1( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );

  setWindowTitle( tr( "GEOM_REVOLUTION_TITLE" ) );

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle( tr( "GEOM_REVOLUTION" ) );
  mainFrame()->RadioButton1->setIcon( image0 );
  mainFrame()->RadioButton2->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton3->close();
  myBothway = false;

  GroupPoints = new DlgRef_2Sel1Spin2Check( centralWidget() );
  GroupPoints->GroupBox1->setTitle( tr( "GEOM_ARGUMENTS" ) );
  GroupPoints->TextLabel1->setText( tr( "GEOM_OBJECT" ) );
  GroupPoints->TextLabel2->setText( tr( "GEOM_AXIS" ) );
  GroupPoints->TextLabel3->setText( tr( "GEOM_ANGLE" ) );
  GroupPoints->PushButton1->setIcon( image1 );
  GroupPoints->PushButton2->setIcon( image1 );
  GroupPoints->LineEdit1->setReadOnly( true );
  GroupPoints->LineEdit2->setReadOnly( true );
  GroupPoints->CheckButton1->setText( tr( "GEOM_BOTHWAY" ) );
  GroupPoints->CheckButton2->setText( tr( "GEOM_REVERSE" ) );

  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( GroupPoints );
  /***************************************************************/

  setHelpFileName( "create_revolution_page.html" );

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
  //initSpinBox( GroupPoints->SpinBox_DX, COORD_MIN, COORD_MAX, SpecificStep, 3 ); // VSR: TODO: DBL_DIGITS_DISPLAY
  // 05.06.2008 skl for IPAL12958
  initSpinBox( GroupPoints->SpinBox_DX, 0.0, 360.0, SpecificStep, 3 ); // VSR: TODO: DBL_DIGITS_DISPLAY
  GroupPoints->SpinBox_DX->setValue( 45.0 );

  /* signals and slots connections */
  connect( buttonOk(),    SIGNAL( clicked() ), this, SLOT( ClickOnOk() ) );
  connect( buttonApply(), SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );

  connect( GroupPoints->PushButton1,  SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );
  connect( GroupPoints->PushButton2,  SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );

  connect( GroupPoints->LineEdit1,    SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );
  connect( GroupPoints->LineEdit2,    SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );

  connect( GroupPoints->SpinBox_DX,   SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox() ) );
  connect( GroupPoints->CheckButton1, SIGNAL( toggled( bool ) ),        this, SLOT( onBothway() ) );
  connect( GroupPoints->CheckButton2, SIGNAL( toggled( bool ) ),        this, SLOT( onReverse() ) );

  connect( myGeomGUI, SIGNAL( SignalDefaultStepValueChanged( double ) ), this, SLOT( SetDoubleSpinBoxStep( double ) ) );

  connect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr(),
	   SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );

  initName( tr( "GEOM_REVOLUTION" ) );

  globalSelection( GEOM_ALLSHAPES );
}

//=================================================================================
// function : SetDoubleSpinBoxStep()
// purpose  : Double spin box management
//=================================================================================
void GenerationGUI_RevolDlg::SetDoubleSpinBoxStep( double step )
{
  GroupPoints->SpinBox_DX->setSingleStep(step);
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

static bool isAcceptableBase( const TopoDS_Shape& theBase )
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
    TopExp_Explorer exp( theBase, TopAbs_SOLID );
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
  myEditCurrentArgument->setText( "" );
  
  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  if (aSelList.Extent() != 1) {
    if ( myEditCurrentArgument == GroupPoints->LineEdit1 )
      myOkBase = false;        
    else if ( myEditCurrentArgument == GroupPoints->LineEdit2 )
      myOkAxis = false;
    return;
  }
  
  // nbSel == 1
  Standard_Boolean testResult = Standard_False;
  GEOM::GEOM_Object_ptr aSelectedObject = GEOMBase::ConvertIOinGEOMObject(aSelList.First(), testResult);
  QString aName = GEOMBase::GetName( aSelectedObject );
  
  if ( !testResult )
    return;

  if ( myEditCurrentArgument == GroupPoints->LineEdit1 ) {
    TopoDS_Shape S;
    myOkBase = false;
    
    if ( !GEOMBase::GetShape(aSelectedObject, S) || !isAcceptableBase( S ) )
      return;
    
    myBase = aSelectedObject;
    myOkBase = true;
  }
  else if ( myEditCurrentArgument == GroupPoints->LineEdit2 ) {
    if ( testResult && !aSelectedObject->_is_nil() ) {
      TopoDS_Shape aShape;
      
      if ( GEOMBase::GetShape( aSelectedObject, aShape, TopAbs_SHAPE ) && !aShape.IsNull() ) {
	TColStd_IndexedMapOfInteger aMap;
	aSelMgr->GetIndexes(aSelList.First(), aMap);
	if ( aMap.Extent() == 1 ) {
	  
	  int anIndex = aMap( 1 );
	  aName.append( ":edge_" + QString::number( anIndex ) );
	  
	  //Find SubShape Object in Father
	  GEOM::GEOM_Object_var aFindedObject = GEOMBase_Helper::findObjectInFather( aSelectedObject, aName );
	  
	  if ( aFindedObject == GEOM::GEOM_Object::_nil() ) { // Object not found in study
	    GEOM::GEOM_IShapesOperations_var aShapesOp =
	      getGeomEngine()->GetIShapesOperations( getStudyId() );
	    myAxis = aShapesOp->GetSubShape( aSelectedObject, anIndex );
	    myOkAxis = true;
	  }
	  else {
	    myAxis = aFindedObject;
	    myOkAxis = true;
	  }
	}
	else {
	  myOkAxis = true;
	  if ( aShape.ShapeType() != TopAbs_EDGE ) {
	    aSelectedObject = GEOM::GEOM_Object::_nil();
	    aName = "";
	    myOkAxis = false;
	  }
	  myAxis = aSelectedObject;
	}
      }
    }
  }

  myEditCurrentArgument->setText( aName );

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

  if ( send == GroupPoints->PushButton1 ) {
    GroupPoints->LineEdit1->setFocus();
    myEditCurrentArgument = GroupPoints->LineEdit1;
  }
  else if ( send == GroupPoints->PushButton2 ) {
    GroupPoints->LineEdit2->setFocus();
    myEditCurrentArgument = GroupPoints->LineEdit2;
    localSelection( GEOM::GEOM_Object::_nil(), TopAbs_EDGE );
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
  if ( send == GroupPoints->LineEdit1 ||
       send == GroupPoints->LineEdit2 ) {
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
  connect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr(), SIGNAL(currentSelectionChanged()), this, SLOT( SelectionIntoArgument() ) );
  GroupPoints->LineEdit1->setFocus();
  myEditCurrentArgument = GroupPoints->LineEdit1;
  displayPreview();
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void GenerationGUI_RevolDlg::enterEvent( QEvent* )
{
  if ( !mainFrame()->GroupConstructors->isEnabled() )
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
  return GroupPoints->SpinBox_DX->value();
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

  if ( !myBothway ) {
    anObj = GEOM::GEOM_I3DPrimOperations::_narrow(
      getOperation() )->MakeRevolutionAxisAngle( myBase, myAxis, getAngle() * PI180 );
  }
  else {
    anObj = GEOM::GEOM_I3DPrimOperations::_narrow(
      getOperation() )->MakeRevolutionAxisAngle2Ways( myBase, myAxis, getAngle() * PI180 );
  }

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
  double anOldValue = GroupPoints->SpinBox_DX->value();
  GroupPoints->SpinBox_DX->setValue( -anOldValue );
}


//=================================================================================
// function :  onBothway()
// purpose  :
//=================================================================================
void GenerationGUI_RevolDlg::onBothway()
{
  bool anOldValue = myBothway;
  myBothway = !anOldValue;
  GroupPoints->CheckButton2->setEnabled( !myBothway );  
  displayPreview();
}

//=================================================================================
// function : addSubshapeToStudy
// purpose  : virtual method to add new SubObjects if local selection
//=================================================================================
void GenerationGUI_RevolDlg::addSubshapesToStudy()
{
  QMap<QString, GEOM::GEOM_Object_var> objMap;

  objMap[GroupPoints->LineEdit2->text()] = myAxis;

  addSubshapesToFather( objMap );
}
