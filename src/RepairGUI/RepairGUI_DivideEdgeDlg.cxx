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
// File   : RepairGUI_DivideEdgeDlg.cxx
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.
//
#include "RepairGUI_DivideEdgeDlg.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>
#include <QtxDoubleSpinBox.h>

#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>
#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>

#include <GEOMImpl_Types.hxx>

#include <TopAbs.hxx>
#include <Geom_Curve.hxx>
#include <gp_Pnt.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>

//=================================================================================
// class    : RepairGUI_DivideEdgeDlg()
// purpose  : Constructs a RepairGUI_DivideEdgeDlg  which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
RepairGUI_DivideEdgeDlg::RepairGUI_DivideEdgeDlg( GeometryGUI* theGeometryGUI, QWidget* parent,
                                                  bool modal )
  : GEOMBase_Skeleton( theGeometryGUI, parent, modal )
{
  QPixmap image0( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_DIVIDE_EDGE" ) ) );
  QPixmap image1( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );

  setWindowTitle( tr( "GEOM_DIVIDE_EDGE_TITLE" ) );

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_DIVIDE_EDGE_TITLE"));
  mainFrame()->RadioButton1->setIcon( image0 );
  mainFrame()->RadioButton2->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton3->close();

  GroupPoints = new DlgRef_1SelExt( centralWidget() );
  GroupPoints->GroupBox1->setTitle( tr( "GEOM_ADD_POINT" ) );
  GroupPoints->TextLabel1->setText( tr( "GEOM_EDGE" ) );
  GroupPoints->PushButton1->setIcon( image1 );
  GroupPoints->LineEdit1->setReadOnly( true );

  QRadioButton* rb1 = new QRadioButton( tr( "GEOM_BY_PARAMETER" ), GroupPoints->Box );
  QRadioButton* rb2 = new QRadioButton( tr( "GEOM_BY_LENGTH" ),    GroupPoints->Box );

  myIsParameterGr = new QButtonGroup( GroupPoints->Box );
  myIsParameterGr->addButton( rb1, 0 );
  myIsParameterGr->addButton( rb2, 1 );
  rb1->setChecked( true );

  myValEdt = new SalomeApp_DoubleSpinBox( 0., 1., 0.1, 3, 32, GroupPoints->Box );
  myValEdt->setValue( 0.5 );
  QLabel* aLbl1 = new QLabel( tr( "GEOM_VALUE" ), GroupPoints->Box );

  QGridLayout* l = new QGridLayout( GroupPoints->Box );
  l->setMargin( 0 ); l->setSpacing( 6 );
  l->addWidget( rb1, 0, 0, 1, 2 );
  l->addWidget( rb2, 1, 0, 1, 2 );
  l->addWidget( aLbl1, 2, 0 );
  l->addWidget( myValEdt, 2, 1 );

  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( GroupPoints );

  /***************************************************************/

  setHelpFileName( "add_point_on_edge_operation_page.html" );

  Init();
}

//=================================================================================
// function : ~RepairGUI_DivideEdgeDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
RepairGUI_DivideEdgeDlg::~RepairGUI_DivideEdgeDlg()
{
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void RepairGUI_DivideEdgeDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupPoints->LineEdit1;

  myObject = GEOM::GEOM_Object::_nil();

  //myGeomGUI->SetState( 0 );
  initSelection();

  /* signals and slots connections */
  connect( buttonOk(),    SIGNAL( clicked() ), this, SLOT( ClickOnOk() ) );
  connect( buttonApply(), SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );

  connect( myValEdt,   SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox() ) );

  connect( GroupPoints->PushButton1, SIGNAL( clicked() ),       this, SLOT( SetEditCurrentArgument() ) );
  connect( GroupPoints->LineEdit1,   SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );

  connect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr(),
	   SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );

  initName( tr( "DEVIDE_EDGE_NEW_OBJECT_NAME" ) );
}

//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  : On change value in spin box
//=================================================================================
void RepairGUI_DivideEdgeDlg::ValueChangedInSpinBox()
{
  displayPreview();
}

//=================================================================================
// function : displayPreview()
// purpose  : On display Preview
//=================================================================================
void RepairGUI_DivideEdgeDlg::displayPreview()
{
  if ( myObject->_is_nil() )
    return;

  TopoDS_Shape aShape;
  gp_Pnt aPnt;
  if ( GEOMBase::GetShape( myObject, aShape, TopAbs_SHAPE ) ) {
    if (aShape.ShapeType() == TopAbs_EDGE) {
      Standard_Real aFP, aLP, aP;
	Handle(Geom_Curve) aCurve = BRep_Tool::Curve(TopoDS::Edge(aShape), aFP, aLP);
	aP = aFP + (aLP - aFP) * myValEdt->value();
	aPnt = aCurve->Value(aP);
	BRepBuilderAPI_MakeVertex mkVertex (aPnt);
	aShape = mkVertex.Shape();
	// Build prs
	SALOME_Prs* aPrs = getDisplayer()->BuildPrs( aShape );
	if ( aPrs != 0 && !aPrs->IsNull() )
	  GEOMBase_Helper::displayPreview( aPrs, false, true );
    }
  }
}

//=================================================================================
// function : ClickOnOk()
// purpose  : Same than click on apply but close this dialog.
//=================================================================================
void RepairGUI_DivideEdgeDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool RepairGUI_DivideEdgeDlg::ClickOnApply()
{
  if ( !onAccept() )
    return false;

  initName();

  myEditCurrentArgument->setText( "" );
  myObject = GEOM::GEOM_Object::_nil();

  initSelection();

  return true;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//          : used only by SelectButtonC1A1 (LineEditC1A1)
//=================================================================================
void RepairGUI_DivideEdgeDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText( "" );

  myObject = GEOM::GEOM_Object::_nil();

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  if ( aSelList.Extent() == 1 ) {
    Handle(SALOME_InteractiveObject) anIO = aSelList.First();
    Standard_Boolean aRes;
    GEOM::GEOM_Object_var aSelectedObject = GEOMBase::ConvertIOinGEOMObject( anIO, aRes );
    if ( !CORBA::is_nil( aSelectedObject ) && aRes ) {
      QString aName = GEOMBase::GetName( aSelectedObject );
      TopoDS_Shape aShape;
      if ( GEOMBase::GetShape( aSelectedObject, aShape, TopAbs_SHAPE ) ) {
        const int aType = aShape.ShapeType();
        if ( aType <= TopAbs_EDGE ) {
	  // edge, wire, face, shell, solid, compound
          GEOM::short_array anIndexes;

          TColStd_IndexedMapOfInteger aMap;
          aSelMgr->GetIndexes( anIO, aMap );

          if ( aMap.Extent() == 1 ) { // local selection
            int anIndex = aMap( 1 );
            myEditCurrentArgument->setText( aName += QString( ":edge_%1" ).arg( anIndex ) );

	    //Find SubShape Object in Father
	    GEOM::GEOM_Object_var aFindedObject = GEOMBase_Helper::findObjectInFather( aSelectedObject, aName );
	    if ( aFindedObject == GEOM::GEOM_Object::_nil() ) { // Object not found in study
	      GEOM::GEOM_IShapesOperations_var aShapesOp = getGeomEngine()->GetIShapesOperations( getStudyId() );
	      myObject = aShapesOp->GetSubShape( aSelectedObject, anIndex );
	    }
	    else {
	      myObject = aFindedObject; // get Object from study
	    }
          }
          else if ( aType == TopAbs_EDGE ) { 
	    // single shape selection
            myObject = aSelectedObject;
            myEditCurrentArgument->setText( GEOMBase::GetName( myObject ) );
          }
          else {
	    // face, shell, solid or compound was selected, and NOT its subshape.
            myObject = GEOM::GEOM_Object::_nil();
          }
        }
      }
    }
  }
  displayPreview();
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void RepairGUI_DivideEdgeDlg::SetEditCurrentArgument()
{
  if ( sender() == GroupPoints->PushButton1 ) {
    GroupPoints->LineEdit1->setFocus();
    myEditCurrentArgument = GroupPoints->LineEdit1;
  }
  SelectionIntoArgument();
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void RepairGUI_DivideEdgeDlg::LineEditReturnPressed()
{
  if ( sender() == GroupPoints->LineEdit1 ) {
    myEditCurrentArgument = GroupPoints->LineEdit1;
    GEOMBase_Skeleton::LineEditReturnPressed();
  }
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void RepairGUI_DivideEdgeDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr(),
	   SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );

//  myObject = GEOM::GEOM_Object::_nil();

  //myGeomGUI->SetState( 0 );
  initSelection();
}

//=================================================================================
// function : enterEvent()
// purpose  : Mouse enter onto the dialog to activate it
//=================================================================================
void RepairGUI_DivideEdgeDlg::enterEvent( QEvent* )
{
  if ( !mainFrame()->GroupConstructors->isEnabled() )
    ActivateThisDialog();
}


//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr RepairGUI_DivideEdgeDlg::createOperation()
{
  return getGeomEngine()->GetIHealingOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool RepairGUI_DivideEdgeDlg::isValid( QString& msg )
{
  bool ok = myValEdt->isValid( msg, !IsPreview() );
  return !myObject->_is_nil() && ok;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool RepairGUI_DivideEdgeDlg::execute( ObjectList& objects )
{
  GEOM::GEOM_Object_var anObj = GEOM::GEOM_IHealingOperations::_narrow( getOperation() )->DivideEdge
    ( myObject, -1, myValEdt->value(), getIsByParameter() );
  bool aResult = !anObj->_is_nil();
  if ( aResult )
  {
    if ( !IsPreview() )
    {
      QStringList aParameters;
      aParameters << "";
      aParameters << myValEdt->text();
      aParameters << "";
      anObj->SetParameters(GeometryGUI::JoinObjectParameters(aParameters));
    }
    objects.push_back( anObj._retn() );
  }

  return aResult;
}

//=================================================================================
// function : getIsByParameter
// purpose  :
//=================================================================================
bool RepairGUI_DivideEdgeDlg::getIsByParameter() const
{
  return myIsParameterGr->button( 0 )->isChecked();
}

//=================================================================================
// function : initSelection
// purpose  :
//=================================================================================
void RepairGUI_DivideEdgeDlg::initSelection()
{
  GEOM::GEOM_Object_var aNullGeomObject;
  localSelection( aNullGeomObject, TopAbs_EDGE ); // load local selection on ALL objects
}

//=================================================================================
// function : addSubshapeToStudy
// purpose  : virtual method to add new SubObjects if local selection
//=================================================================================
void RepairGUI_DivideEdgeDlg::addSubshapesToStudy()
{
  QMap<QString, GEOM::GEOM_Object_var> objMap;
  objMap[GroupPoints->LineEdit1->text()] = myObject;

  addSubshapesToFather( objMap );
}
