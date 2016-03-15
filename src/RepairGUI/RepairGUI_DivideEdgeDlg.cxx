// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
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

// GEOM GEOMGUI : GUI for Geometry component
// File   : RepairGUI_DivideEdgeDlg.cxx
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.
//
#include "RepairGUI_DivideEdgeDlg.h"

#include "DlgRef.h"
#include "GeometryGUI.h"
#include "GEOMBase.h"
#include "RepairGUI.h"

#include <SalomeApp_DoubleSpinBox.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>
#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>

#include <GEOMImpl_Types.hxx>

#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <Geom_Curve.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <gp_Pnt.hxx>

enum { BY_PARAM, BY_POINT_PROJ };

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
  QPixmap image2( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_DIVIDE_EDGE_BY_PNT" ) ) );
  QPixmap image1( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );

  setWindowTitle( tr( "GEOM_DIVIDE_EDGE_TITLE" ) );

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_DIVIDE_EDGE_TITLE"));
  mainFrame()->RadioButton1->setIcon( image0 );
  mainFrame()->RadioButton2->setIcon( image2 );
  mainFrame()->RadioButton3->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton3->close();

  GroupPoints = new DlgRef_2SelExt( centralWidget() );
  GroupPoints->GroupBox1->setTitle( tr( "GEOM_ADD_POINT" ) );
  GroupPoints->TextLabel1->setText( tr( "GEOM_EDGE" ) );
  GroupPoints->PushButton1->setIcon( image1 );
  GroupPoints->LineEdit1->setReadOnly( true );
  GroupPoints->TextLabel2->setText( tr( "DEVIDE_EDGE_BY_PROJ_POINT" ) );
  GroupPoints->PushButton2->setIcon( image1 );
  GroupPoints->LineEdit2->setReadOnly( true );

  QRadioButton* rb1 = new QRadioButton( tr( "GEOM_BY_PARAMETER" ), GroupPoints->Box );
  QRadioButton* rb2 = new QRadioButton( tr( "GEOM_BY_LENGTH" ),    GroupPoints->Box );

  myIsParameterGr = new QButtonGroup( GroupPoints->Box );
  myIsParameterGr->addButton( rb1, 0 );
  myIsParameterGr->addButton( rb2, 1 );
  rb1->setChecked( true );

  myValEdt = new SalomeApp_DoubleSpinBox( GroupPoints->Box );
  initSpinBox( myValEdt, 0., 1., 0.1, "parametric_precision" );
  myValEdt->setValue( 0.5 );
  myValLbl = new QLabel( tr( "GEOM_VALUE" ), GroupPoints->Box );

  QGridLayout* l = new QGridLayout( GroupPoints->Box );
  l->setMargin( 0 ); l->setSpacing( 6 );
  l->addWidget( rb1, 0, 0, 1, 2 );
  l->addWidget( rb2, 1, 0, 1, 2 );
  l->addWidget( myValLbl, 2, 0 );
  l->addWidget( myValEdt, 2, 1 );

  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( GroupPoints );

  /***************************************************************/

  setHelpFileName( "add_point_on_edge_operation_page.html" );

  Init();
}

//=======================================================================
//function : ConstructorsClicked
//purpose  : hide/show widgets depending on a selected constructor
//=======================================================================
void RepairGUI_DivideEdgeDlg::ConstructorsClicked( int constructorId )
{
  myIsParameterGr->button( 0 )->setVisible( constructorId == BY_PARAM );
  myIsParameterGr->button( 1 )->setVisible( constructorId == BY_PARAM );
  myValEdt                    ->setVisible( constructorId == BY_PARAM );
  myValLbl                    ->setVisible( constructorId == BY_PARAM );
  GroupPoints->TextLabel2     ->setVisible( constructorId == BY_POINT_PROJ );
  GroupPoints->PushButton2    ->setVisible( constructorId == BY_POINT_PROJ );
  GroupPoints->LineEdit2      ->setVisible( constructorId == BY_POINT_PROJ );

  initSelection();

  if ( constructorId == BY_PARAM &&
       !GroupPoints->PushButton1->isDown() )
    GroupPoints->PushButton1->click();

  displayPreview();
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
  myPoints.clear();
  myIndex  = -1;
  myProjectionOK = false;

  //myGeomGUI->SetState( 0 );

  /* signals and slots connections */
  connect( buttonOk(),    SIGNAL( clicked() ), this, SLOT( ClickOnOk() ) );
  connect( buttonApply(), SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );

  connect(this,           SIGNAL(constructorsClicked(int)), this, SLOT(ConstructorsClicked(int)));

  connect( myValEdt,   SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox() ) );

  connect( GroupPoints->PushButton1, SIGNAL( clicked() ),       this, SLOT( SetEditCurrentArgument() ) );
  connect( GroupPoints->LineEdit1,   SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );
  connect( GroupPoints->PushButton2, SIGNAL( clicked() ),       this, SLOT( SetEditCurrentArgument() ) );
  connect( GroupPoints->LineEdit2,   SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );

  connect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr(),
           SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );

  initName( tr( "DEVIDE_EDGE_NEW_OBJECT_NAME" ) );
  resize(100,100);
  ConstructorsClicked( BY_PARAM );
  SelectionIntoArgument();
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
  myProjectionOK = false;
  erasePreview();
  if ( myObject->_is_nil() )
    return;

  TopoDS_Shape aShape;
  gp_Pnt aPnt;
  GEOMBase::GetShape( myObject, aShape, TopAbs_SHAPE );

  if ( myIndex != -1 ) {
    TopTools_IndexedMapOfShape aShapes;
    TopExp::MapShapes(aShape, aShapes);
    aShape = aShapes.FindKey(myIndex);
  }

  if (aShape.ShapeType() == TopAbs_EDGE) {
    Standard_Real aFP, aLP, aP;
    Handle(Geom_Curve) aCurve = BRep_Tool::Curve(TopoDS::Edge(aShape), aFP, aLP);
    if ( aCurve.IsNull() ) return;

    if ( getConstructorId() == BY_PARAM )
    {
      aP = aFP + (aLP - aFP) * myValEdt->value();
      aPnt = aCurve->Value(aP);
      BRepBuilderAPI_MakeVertex mkVertex (aPnt);
      aShape = mkVertex.Shape();
    }
    else if ( getConstructorId() == BY_POINT_PROJ && !myPoints.empty() )
    {
      BRep_Builder builder;
      TopoDS_Compound compoundOfVV;
      builder.MakeCompound(compoundOfVV);
      aShape = compoundOfVV;

      GeomAPI_ProjectPointOnCurve aProjector;
      aProjector.Init( aCurve, aFP, aLP );

      TopTools_MapOfShape vMap;
      for ( int i = 0; i < myPoints.count(); ++i )
      {
        TopoDS_Shape aPoint;
        GEOMBase::GetShape( myPoints[i].get(), aPoint, TopAbs_VERTEX );
        if ( !vMap.Add( aPoint )) continue;

        gp_Pnt p = BRep_Tool::Pnt( TopoDS::Vertex( aPoint ));
        aProjector.Perform( p );
        if ( aProjector.NbPoints() > 0 )
        {
          double u = aProjector.LowerDistanceParameter();
          if ( Min( u - aFP, aLP - u ) > Precision::Confusion() )
          {
            builder.Add( compoundOfVV, BRepBuilderAPI_MakeVertex( aProjector.NearestPoint()));
            myProjectionOK = true;
          }
        }
      }
    }
    else
    {
      return;
    }
    // Build prs
    SALOME_Prs* aPrs = getDisplayer()->BuildPrs( aShape );
    if ( aPrs != 0 && !aPrs->IsNull() )
      GEOMBase_Helper::displayPreview( aPrs, false, true );
  }
}

//=================================================================================
// function : ClickOnOk()
// purpose  : Same than click on apply but close this dialog.
//=================================================================================
void RepairGUI_DivideEdgeDlg::ClickOnOk()
{
  setIsApplyAndClose( true );
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

  GroupPoints->LineEdit1->setText( "" );
  GroupPoints->LineEdit2->setText( "" );
  myObject = GEOM::GEOM_Object::_nil();
  myPoints.clear();
  myIndex = -1;
  myProjectionOK = false;

  GroupPoints->PushButton1->click();

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

  const bool toSelectObject = ( myEditCurrentArgument == GroupPoints->LineEdit1 );
  if ( toSelectObject )
  {
    myObject = GEOM::GEOM_Object::_nil();
    myIndex = -1;
    myProjectionOK = false;
  }
  else //if ( myEditCurrentArgument == GroupPoints->LineEdit2 )
  {
    myPoints.clear();
    myProjectionOK = false;
  }

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  if ( toSelectObject && aSelList.Extent() == 1 ) {
    Handle(SALOME_InteractiveObject) anIO = aSelList.First();
    GEOM::GEOM_Object_var aSelectedObj = GEOMBase::ConvertIOinGEOMObject( anIO );
    if ( !CORBA::is_nil( aSelectedObj ) )
    {
      TopoDS_Shape aShape;
      QString aName = GEOMBase::GetName( aSelectedObj );
      if ( GEOMBase::GetShape( aSelectedObj, aShape, TopAbs_SHAPE ) )
      {
        const int aType = aShape.ShapeType();
        if ( aType <= TopAbs_EDGE ) // edge, wire, face, shell, solid, compound
        {
          GEOM::short_array anIndexes;

          TColStd_IndexedMapOfInteger aMap;
          SalomeApp_Application* anApp =
            (SalomeApp_Application*)(SUIT_Session::session()->activeApplication());
          anApp->selectionMgr()->GetIndexes( anIO, aMap );

          if ( !aMap.IsEmpty() ) // sub-shape selection
          {
            myIndex = aMap( 1 );
            myObject = aSelectedObj;
            myEditCurrentArgument->setText( aName += QString( ":edge_%1" ).arg( myIndex ) );
          }
          else if ( aType == TopAbs_EDGE ) // single shape selection
          {
            myIndex = -1;
            myObject = aSelectedObj;
            myEditCurrentArgument->setText( GEOMBase::GetName( myObject ) );
          }
          else // face, shell, solid or compound was selected, and NOT its sub-shape.
          {
            myIndex = -1;
            myObject = GEOM::GEOM_Object::_nil();
          }
        }
      }
    }
  }
  if ( !toSelectObject )
  {
    myPoints = getSelected( TopAbs_VERTEX, -1, /*strict=*/true );
    if ( myPoints.empty() )
      myEditCurrentArgument->setText( "" );
    else if ( myPoints.count() == 1 )
      myEditCurrentArgument->setText( GEOMBase::GetName( myPoints[0].get() ));
    else
      myEditCurrentArgument->setText( QObject::tr( "%1_vertices" ).arg( myPoints.count() ));
  }

  displayPreview();
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void RepairGUI_DivideEdgeDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  if ( send == GroupPoints->PushButton1 ) {
    GroupPoints->LineEdit1->setFocus();
    myEditCurrentArgument = GroupPoints->LineEdit1;

    GroupPoints->PushButton2->setDown(false);
    GroupPoints->LineEdit2->setEnabled(false);
  }
  if ( send == GroupPoints->PushButton2 ) {
    GroupPoints->LineEdit2->setFocus();
    myEditCurrentArgument = GroupPoints->LineEdit2;

    GroupPoints->PushButton1->setDown(false);
    GroupPoints->LineEdit1->setEnabled(false);
  }
  // enable line edit
  myEditCurrentArgument->setEnabled(true);
  myEditCurrentArgument->setFocus();
  // after setFocus(), because it will be setDown(false) when loses focus
  send->setDown(true);

  initSelection();
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
  if ( sender() == GroupPoints->LineEdit2 &&
       !GroupPoints->LineEdit2->isHidden() ) {
    myEditCurrentArgument = GroupPoints->LineEdit2;
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
  myIndex = -1;

  //myGeomGUI->SetState( 0 );
  ConstructorsClicked(getConstructorId());
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
  bool ok = false;
  if ( getConstructorId() == BY_PARAM )
  {
    ok = myValEdt->isValid( msg, !IsPreview() );
  }
  else if ( getConstructorId() == BY_POINT_PROJ )
  {
    if (( ok = myPoints.count() ) && !( ok = myProjectionOK ))
      msg = tr("DEVIDE_EDGE_BAD_PROJ_MSG");
  }
  return !myObject->_is_nil() && ok;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool RepairGUI_DivideEdgeDlg::execute( ObjectList& objects )
{
  GEOM::GEOM_IHealingOperations_var anOper = GEOM::GEOM_IHealingOperations::_narrow( getOperation() );
  GEOM::GEOM_Object_var anObj;
  if ( getConstructorId() == BY_PARAM )
  {
    anObj = anOper->DivideEdge( myObject, myIndex, myValEdt->value(), getIsByParameter() );
  }
  else
  {
    GEOM::ListOfGO_var objList = new GEOM::ListOfGO;
    objList->length( myPoints.count() );
    for ( int i = 0; i < myPoints.count(); ++i )
      objList[i] = myPoints[i].copy();
    anObj = anOper->DivideEdgeByPoint( myObject, myIndex, objList );
  }
  bool aResult = !anObj->_is_nil();
  if ( aResult )
  {
    if ( !IsPreview() && ( getConstructorId() == BY_PARAM ))
    {
      QStringList aParameters;
      aParameters << "";
      aParameters << myValEdt->text();
      aParameters << "";
      anObj->SetParameters(aParameters.join(":").toLatin1().constData());
    }
    if ( !IsPreview() )
      RepairGUI::ShowStatistics( anOper, this );
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
  // close local contexts
  globalSelection();
  // load local selection on ALL objects
  localSelection( myEditCurrentArgument == GroupPoints->LineEdit2 ? TopAbs_VERTEX : TopAbs_EDGE );
}

//=================================================================================
// function : addSubshapeToStudy
// purpose  : virtual method to add new SubObjects if local selection
//=================================================================================
void RepairGUI_DivideEdgeDlg::addSubshapesToStudy()
{
  for ( int i = 0; i < myPoints.count(); ++i )
    GEOMBase::PublishSubObject( myPoints[i].get() );
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> RepairGUI_DivideEdgeDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res(myPoints);
  GEOM::GeomObjPtr aGeomObjPtr(myObject);
  res << aGeomObjPtr;
  return res;
}
