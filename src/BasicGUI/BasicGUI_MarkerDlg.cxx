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
//  File   : BasicGUI_MarkerDlg.cxx
//  Author : Sergey LITONIN
//  Module : GEOM
//  $Header$

#include "BasicGUI_MarkerDlg.h"
#include "DlgRef_SpinBox.h"
#include "QAD_Desktop.h"
#include "QAD_RightFrame.h"
#include "VTKViewer_ViewFrame.h"
#include "OCCViewer_ViewFrame.h"
#include "QAD_Config.h"
#include "GEOMImpl_Types.hxx"
#include "utilities.h"

#include <TopExp.hxx>
#include <TopAbs.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Face.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <BRep_Tool.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pln.hxx>
#include <Geom_Plane.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>

//=================================================================================
// class    : BasicGUI_MarkerDlg()
// purpose  : Constructor
//=================================================================================
BasicGUI_MarkerDlg::BasicGUI_MarkerDlg( QWidget* theParent, SALOME_Selection* theSel )
: GEOMBase_Skeleton( theParent, "BasicGUI_MarkerDlg", theSel, false,
    WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap iconCS1   ( QAD_Desktop::getResourceManager()->loadPixmap( "GEOM", tr( "ICON_MARKER" ) ) );
  QPixmap iconCS2   ( QAD_Desktop::getResourceManager()->loadPixmap( "GEOM", tr( "ICON_MARKER2" ) ) );
  QPixmap iconCS3   ( QAD_Desktop::getResourceManager()->loadPixmap( "GEOM", tr( "ICON_MARKER3" ) ) );
  QPixmap iconSelect( QAD_Desktop::getResourceManager()->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );

  setCaption( tr( "CAPTION" ) );

  GroupConstructors->setTitle( tr( "LOCALCS" ) );
  RadioButton1->setPixmap( iconCS1 );
  RadioButton2->setPixmap( iconCS2 );
  RadioButton3->setPixmap( iconCS3 );

  Group1 = new DlgRef_1Sel_QTD(this, "Group1");
  Group1->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  Group1->TextLabel1->setText(tr("GEOM_OBJECT"));
  Group1->PushButton1->setPixmap(iconSelect);

  Group2 = new DlgRef_3Sel_QTD(this, "Group2");
  Group2->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  Group2->TextLabel1->setText(tr("GEOM_POINT"));
  Group2->TextLabel2->setText(tr("XDIR"));
  Group2->TextLabel3->setText(tr("YDIR"));
  Group2->PushButton1->setPixmap(iconSelect);
  Group2->PushButton2->setPixmap(iconSelect);
  Group2->PushButton3->setPixmap(iconSelect);

  aMainGrp = new QGroupBox( 1, Qt::Horizontal, this );
  aMainGrp->setFrameStyle( QFrame::NoFrame );
  aMainGrp->setInsideMargin( 0 );

  QGroupBox* anOriGrp = new QGroupBox( 1, Qt::Vertical, tr( "ORIGIN" ), aMainGrp );
  new QLabel( tr( "GEOM_X" ), anOriGrp );
  myData[ X ] = new DlgRef_SpinBox( anOriGrp );
  new QLabel( tr( "GEOM_Y" ), anOriGrp );
  myData[ Y ] = new DlgRef_SpinBox( anOriGrp );
  new QLabel( tr( "GEOM_Z" ), anOriGrp );
  myData[ Z ] = new DlgRef_SpinBox( anOriGrp );

  QGroupBox* aXAxisGrp = new QGroupBox( 1, Qt::Vertical, tr( "XDIR" ), aMainGrp );
  new QLabel( tr( "DX" ), aXAxisGrp );
  myData[ DX1 ] = new DlgRef_SpinBox( aXAxisGrp );
  new QLabel( tr( "DY" ), aXAxisGrp );
  myData[ DY1 ] = new DlgRef_SpinBox( aXAxisGrp );
  new QLabel( tr( "DZ" ), aXAxisGrp );
  myData[ DZ1 ] = new DlgRef_SpinBox( aXAxisGrp );

  QGroupBox* anYAxisGrp = new QGroupBox( 1, Qt::Vertical, tr( "YDIR" ), aMainGrp );
  new QLabel( tr( "DX" ), anYAxisGrp );
  myData[ DX2 ] = new DlgRef_SpinBox( anYAxisGrp );
  new QLabel( tr( "DY" ), anYAxisGrp );
  myData[ DY2 ] = new DlgRef_SpinBox( anYAxisGrp );
  new QLabel( tr( "DZ" ), anYAxisGrp );
  myData[ DZ2 ] = new DlgRef_SpinBox( anYAxisGrp );
  
  Layout1->addWidget( aMainGrp, 2, 0 );
  Layout1->addWidget( Group1, 2, 0 );
  Layout1->addWidget( Group2, 2, 0 );

  Init( theSel );
}


//=================================================================================
// function : ~BasicGUI_MarkerDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
BasicGUI_MarkerDlg::~BasicGUI_MarkerDlg()
{
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BasicGUI_MarkerDlg::Init( SALOME_Selection* theSel )
{
  myBlockPreview = false;
  myConstructorId = -1;
  myEditCurrentArgument = Group1->LineEdit1;
  Group1->LineEdit1->setReadOnly( true );
  Group2->LineEdit1->setReadOnly( true );
  Group2->LineEdit2->setReadOnly( true );
  Group2->LineEdit3->setReadOnly( true );

  GeometryGUI* aGeomGUI = GeometryGUI::GetGeomGUI();

  connect(GroupConstructors, SIGNAL(clicked(int)), this, SLOT(ConstructorsClicked(int)));
  connect(Group1->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(Group2->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(Group2->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(Group2->PushButton3, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(Group1->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(Group2->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(Group2->LineEdit2, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(Group2->LineEdit3, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( onClose() ) );
  connect( aGeomGUI, SIGNAL( SignalDeactivateActiveDialog() ), this, SLOT( onDeactivate() ) );
  connect( aGeomGUI, SIGNAL( SignalCloseAllDialogs() ), this, SLOT( onClose() ) );

  connect( buttonOk, SIGNAL( clicked() ), this, SLOT( onOk() ) );
  connect( buttonApply, SIGNAL( clicked() ), this, SLOT( onApply() ) );

  connect( mySelection, SIGNAL( currentSelectionChanged() ), this, SLOT( onSelectionDone() ) );

  initName( tr( "LCS_NAME" ) );

  QString aStr = QAD_CONFIG->getSetting( "Geometry:SettingsGeomStep" );
  for ( DataMap::iterator anIter = myData.begin(); anIter != myData.end(); ++anIter )
  {
    anIter.data()->RangeStepAndValidator( -999.999, 999.999, aStr.toDouble(), 3 );
    connect( anIter.data(), SIGNAL( valueChanged( double ) ),
             this, SLOT( onValueChanged( double ) ) );
  }

  myBlockPreview = true;
  for ( DataMap::iterator anIter = myData.begin(); anIter != myData.end(); ++anIter )
    anIter.data()->SetValue( 0 );
  myData[ DX1 ]->SetValue( 1 );
  myData[ DY2 ]->SetValue( 1 );
  myBlockPreview = false;

  ConstructorsClicked( 0 );

  QAD_ViewFrame* aFrame = QAD_Application::getDesktop()->getActiveApp()->
    getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame();

  VTKViewer_ViewFrame* aVTKFrame = dynamic_cast<VTKViewer_ViewFrame*>( aFrame );
  if ( aVTKFrame )
  {
    aVTKFrame->AdjustTrihedrons( true );
    return;
  }

  OCCViewer_ViewFrame* aOCCFrame = dynamic_cast<OCCViewer_ViewFrame*>( aFrame );
  if ( aOCCFrame )
    aOCCFrame->AdjustTrihedrons( true );
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void BasicGUI_MarkerDlg::ConstructorsClicked( int constructorId )
{
  if ( myConstructorId == constructorId && myConstructorId == 0 )
  {
    localSelection( GEOM::GEOM_Object::_nil(), TopAbs_VERTEX );
    activate( GEOM_MARKER );
    displayPreview();
    return;
  }

  myConstructorId = constructorId;

  disconnect(mySelection, 0, this, 0);

  switch (constructorId)
    {
    case 0:
    {
      Group1->hide();
      Group2->hide();
      resize(0, 0);
      aMainGrp->show();
      localSelection( GEOM::GEOM_Object::_nil(), TopAbs_VERTEX );
      activate( GEOM_MARKER );
      break;
    }
    case 1:
      {
	Group2->hide();
	aMainGrp->hide();
	resize(0, 0);
	Group1->show();

	globalSelection( GEOM_ALLGEOM );
	myEditCurrentArgument = Group1->LineEdit1;
	Group1->LineEdit1->setText("");
	break;
      }
    case 2:
      {
	aMainGrp->hide();
	Group1->show();
	resize(0, 0);
	Group2->show();

	globalSelection( GEOM_POINT );
	myEditCurrentArgument = Group2->LineEdit1;
	Group2->LineEdit1->setText("");
	Group2->LineEdit2->setText("");
	Group2->LineEdit3->setText("");
	break;
      }
    }

  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(onSelectionDone()));
  onSelectionDone();
}

//=================================================================================
// function : onOk()
// purpose  :
//=================================================================================
void BasicGUI_MarkerDlg::onOk()
{
  if ( onApply() )
    onClose();
}

//=======================================================================
// function : onClose()
// purpose  :
//=======================================================================
void BasicGUI_MarkerDlg::onClose()
{
  GEOMBase_Skeleton::ClickOnCancel();
}

//=================================================================================
// function : onApply()
// purpose  :
//=================================================================================
bool BasicGUI_MarkerDlg::onApply()
{
  if ( !onAccept() )
    return false;

  initName();
  ConstructorsClicked( getConstructorId() );
  
  return true;
}

//=================================================================================
// function : onSelectionDone0()
// purpose  : Reaction on selection when first method of creation is current one
//=================================================================================
void BasicGUI_MarkerDlg::onSelectionDone0()
{
  if ( mySelection->IObjectCount() == 1 )
  {

    Standard_Boolean aRes = Standard_False;
    Handle(SALOME_InteractiveObject) anIO = mySelection->firstIObject();
    GEOM::GEOM_Object_var aSelectedObj = GEOMBase::ConvertIOinGEOMObject( anIO, aRes );

    if ( aRes && !aSelectedObj->_is_nil() )
    {
      TopoDS_Shape aShape;
      if ( myGeomBase->GetShape( aSelectedObj, aShape, TopAbs_SHAPE ) && !aShape.IsNull() )
      {
        if ( aSelectedObj->GetType() == GEOM_MARKER && aShape.ShapeType() == TopAbs_FACE )
        {
          TopoDS_Face aFace = TopoDS::Face( aShape );
          Handle(Geom_Plane) aPlane = Handle(Geom_Plane)::DownCast( BRep_Tool::Surface( aFace ) );

          if ( !aPlane.IsNull() )
          {
            gp_Ax3 anAx3 = aPlane->Pln().Position();
            gp_Pnt aLoc = anAx3.Location();
            gp_Dir aXDir = anAx3.XDirection();
            gp_Dir aYDir = anAx3.YDirection();

            myData[ X ]->SetValue( aLoc.X() );
            myData[ Y ]->SetValue( aLoc.Y() );
            myData[ Z ]->SetValue( aLoc.Z() );

            myData[ DX1 ]->SetValue( aXDir.X() );
            myData[ DY1 ]->SetValue( aXDir.Y() );
            myData[ DZ1 ]->SetValue( aXDir.Z() );

            myData[ DX2 ]->SetValue( aYDir.X() );
            myData[ DY2 ]->SetValue( aYDir.Y() );
            myData[ DZ2 ]->SetValue( aYDir.Z() );
            //mySelection->ClearIObjects();
          }
        }
        else
        {
          if ( mySelection->HasIndex( anIO ) )
          {
            TColStd_IndexedMapOfInteger aMap;
            mySelection->GetIndex( anIO, aMap );
            if ( aMap.Extent() == 1 )
            {
              int anIndex = aMap( 1 );
              TopTools_IndexedMapOfShape aShapes;
              TopExp::MapShapes( aShape, aShapes );
              aShape = aShapes.FindKey( anIndex );
              //mySelection->ClearIObjects();
            }
          }

          if ( !aShape.IsNull() && aShape.ShapeType() == TopAbs_VERTEX )
          {
            gp_Pnt aPnt = BRep_Tool::Pnt( TopoDS::Vertex( aShape ) );
            myData[ X ]->SetValue( aPnt.X() );
            myData[ Y ]->SetValue( aPnt.Y() );
            myData[ Z ]->SetValue( aPnt.Z() );
          }
        }
      }
    }
  }

  displayPreview();
}


//=================================================================================
// function : onSelectionDone()
// purpose  : Called when selection as changed or other case
//=================================================================================
void BasicGUI_MarkerDlg::onSelectionDone()
{
  if ( getConstructorId() == 0 )
  {
    onSelectionDone0();
    return;
  }
    

  myEditCurrentArgument->setText("");
  QString aName;

  if ( mySelection->IObjectCount() == 1 ) {
    Standard_Boolean aRes = Standard_False;
    Handle(SALOME_InteractiveObject) anIO = mySelection->firstIObject();
    GEOM::GEOM_Object_var aSelectedObj = GEOMBase::ConvertIOinGEOMObject( anIO, aRes );

    if ( !CORBA::is_nil( aSelectedObj ) && aRes ) {
      aName = GEOMBase::GetName( aSelectedObj );
      TopoDS_Shape aShape;
      if ( myGeomBase->GetShape( aSelectedObj, aShape, TopAbs_SHAPE ) ) {
	GEOM::short_array anIndexes;
	if ( mySelection->HasIndex( anIO ) ) {
	  TColStd_IndexedMapOfInteger aMap;
	  mySelection->GetIndex( anIO, aMap );
	  int anIndex = aMap( 1 );
	  TopTools_IndexedMapOfShape aShapes;
	  TopExp::MapShapes( aShape, aShapes );
	  aShape = aShapes.FindKey( anIndex );
	}

  if ( getConstructorId() == 1 ) {
	  if ( !aShape.IsNull() ) {
	    gp_Pnt aPnt;
	    if (aShape.ShapeType() == TopAbs_VERTEX) {
	      aPnt = BRep_Tool::Pnt(TopoDS::Vertex(aShape));
	    } 
	    else {
	      GProp_GProps aSystem;
	      if (aShape.ShapeType() == TopAbs_EDGE || aShape.ShapeType() == TopAbs_WIRE)
		BRepGProp::LinearProperties(aShape, aSystem);
	      else if (aShape.ShapeType() == TopAbs_FACE || aShape.ShapeType() == TopAbs_SHELL)
		BRepGProp::SurfaceProperties(aShape, aSystem);
	      else
		BRepGProp::VolumeProperties(aShape, aSystem);
	      
	      aPnt = aSystem.CentreOfMass();
	    }

	    gp_Ax3 anAx3;
	    anAx3.Transform(aShape.Location().Transformation());
	    if(aShape.ShapeType() == TopAbs_FACE) {
	      Handle(Geom_Surface) aGS = BRep_Tool::Surface( TopoDS::Face( aShape ) );
	      if (!aGS.IsNull() && aGS->IsKind( STANDARD_TYPE( Geom_Plane ) ) ) {
		Handle(Geom_Plane) aGPlane = Handle(Geom_Plane)::DownCast( aGS );
		gp_Pln aPln = aGPlane->Pln();
		anAx3 = aPln.Position();
	      }
	    }

	    gp_Dir aDirX = anAx3.XDirection();
	    gp_Dir aDirY = anAx3.YDirection();

	    myData[ X ]->SetValue( aPnt.X() );
	    myData[ Y ]->SetValue( aPnt.Y() );
	    myData[ Z ]->SetValue( aPnt.Z() );

	    myData[ DX1 ]->SetValue( aDirX.X() );
	    myData[ DY1 ]->SetValue( aDirX.Y() );
	    myData[ DZ1 ]->SetValue( aDirX.Z() );

	    myData[ DX2 ]->SetValue( aDirY.X() );
	    myData[ DY2 ]->SetValue( aDirY.Y() );
	    myData[ DZ2 ]->SetValue( aDirY.Z() );

	    myEditCurrentArgument->setText( aName );
	  }
	}
	else if ( getConstructorId() == 2 ) {
	  if (myEditCurrentArgument == Group2->LineEdit1) {
	    if ( !aShape.IsNull() && aShape.ShapeType() == TopAbs_VERTEX ) {
	      gp_Pnt aPnt = BRep_Tool::Pnt( TopoDS::Vertex( aShape ) );
	      myData[ X ]->SetValue( aPnt.X() );
	      myData[ Y ]->SetValue( aPnt.Y() );
	      myData[ Z ]->SetValue( aPnt.Z() );
	      myEditCurrentArgument->setText( aName );
	    }
	  }
	  else if (myEditCurrentArgument == Group2->LineEdit2) {
	    if ( !aShape.IsNull() && aShape.ShapeType() == TopAbs_EDGE ) {
	      gp_Pnt aP1 = BRep_Tool::Pnt(TopExp::FirstVertex(TopoDS::Edge(aShape)));
	      gp_Pnt aP2 = BRep_Tool::Pnt(TopExp::LastVertex(TopoDS::Edge(aShape)));
	      gp_Dir aDir(gp_Vec(aP1, aP2));

	      myData[ DX1 ]->SetValue( aDir.X() );
	      myData[ DY1 ]->SetValue( aDir.Y() );
	      myData[ DZ1 ]->SetValue( aDir.Z() );
	      myEditCurrentArgument->setText( aName );
	    }
	  }
	  else if (myEditCurrentArgument == Group2->LineEdit3) {
	    if ( !aShape.IsNull() && aShape.ShapeType() == TopAbs_EDGE ) {
	      gp_Pnt aP1 = BRep_Tool::Pnt(TopExp::FirstVertex(TopoDS::Edge(aShape)));
	      gp_Pnt aP2 = BRep_Tool::Pnt(TopExp::LastVertex(TopoDS::Edge(aShape)));
	      gp_Dir aDir(gp_Vec(aP1, aP2));

	      myData[ DX2 ]->SetValue( aDir.X() );
	      myData[ DY2 ]->SetValue( aDir.Y() );
	      myData[ DZ2 ]->SetValue( aDir.Z() );
	      myEditCurrentArgument->setText( aName );
	    }
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
void BasicGUI_MarkerDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();
  
  if(send == Group1->PushButton1) {
    myEditCurrentArgument = Group1->LineEdit1;
    globalSelection( GEOM_ALLGEOM );
  }
  else if(send == Group2->PushButton1) {
    myEditCurrentArgument = Group2->LineEdit1;
    globalSelection( GEOM_POINT );
  }
  else if(send == Group2->PushButton2) {
    myEditCurrentArgument = Group2->LineEdit2;
      globalSelection( GEOM_LINE );
  }
  else if(send == Group2->PushButton3) {
    myEditCurrentArgument = Group2->LineEdit3;
    globalSelection( GEOM_LINE );
  }
  
  myEditCurrentArgument->setFocus();
  onSelectionDone();
}

//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void BasicGUI_MarkerDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();

  myEditCurrentArgument = send;
  GEOMBase_Skeleton::LineEditReturnPressed();
}

//=================================================================================
// function : onActivate()
// purpose  :
//=================================================================================
void BasicGUI_MarkerDlg::onActivate()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect( mySelection, SIGNAL( currentSelectionChanged() ), this, SLOT( onSelectionDone() ) );

  ConstructorsClicked( getConstructorId() );
}

//=================================================================================
// function : onDeactivate()
// purpose  : public slot to deactivate if active
//=================================================================================
void BasicGUI_MarkerDlg::onDeactivate()
{
  GEOMBase_Skeleton::DeactivateActiveDialog();
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void BasicGUI_MarkerDlg::enterEvent(QEvent* e)
{                       
  if ( !GroupConstructors->isEnabled() )
    onActivate();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr BasicGUI_MarkerDlg::createOperation()
{
  return getGeomEngine()->GetIBasicOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool BasicGUI_MarkerDlg::isValid( QString& msg )
{
  const int id = getConstructorId();
  gp_Vec v1( myData[ DX1 ]->GetValue(), myData[ DY1 ]->GetValue(), myData[ DZ1 ]->GetValue() ),
         v2( myData[ DX2 ]->GetValue(), myData[ DY2 ]->GetValue(), myData[ DZ2 ]->GetValue() );
  bool isOrthogonal = v1.IsNormal( v2, Precision::Confusion() );
  switch ( id )
  {
    case 0:
      return isOrthogonal;
    case 1:
      return !Group1->LineEdit1->text().isEmpty() && isOrthogonal;
    case 2:
      return !Group2->LineEdit1->text().isEmpty() && 
	     !Group2->LineEdit2->text().isEmpty() && 
	     !Group2->LineEdit3->text().isEmpty() && isOrthogonal;
  }
  return false;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool BasicGUI_MarkerDlg::execute( ObjectList& objects )
{
  GEOM::GEOM_Object_var anObj = GEOM::GEOM_IBasicOperations::_narrow(
    getOperation() )->MakeMarker( myData[ X   ]->GetValue(), myData[ Y   ]->GetValue(), myData[ Z   ]->GetValue(),
                                  myData[ DX1 ]->GetValue(), myData[ DY1 ]->GetValue(), myData[ DZ1 ]->GetValue(),
                                  myData[ DX2 ]->GetValue(), myData[ DY2 ]->GetValue(), myData[ DZ2 ]->GetValue() );

  if ( !anObj->_is_nil() )
    objects.push_back( anObj._retn() );

  return true;
}

//=================================================================================
// function : closeEvent
// purpose  :
//=================================================================================
void BasicGUI_MarkerDlg::closeEvent( QCloseEvent* e )
{
  GEOMBase_Skeleton::closeEvent( e );
}

//=================================================================================
// function : onValueChanged
// purpose  :
//=================================================================================
void BasicGUI_MarkerDlg::onValueChanged( double )
{
  displayPreview();
}

//=================================================================================
// function : displayPreview
// purpose  :
//=================================================================================
void BasicGUI_MarkerDlg::displayPreview ( const bool   activate,
                                          const bool   update,
                                          const bool   toRemoveFromEngine,
                                          const double lineWidth )
{
  if ( !myBlockPreview )
  {
    GEOMBase_Skeleton::displayPreview( activate, update, toRemoveFromEngine, lineWidth );
    if ( myConstructorId == 0 )
    {
      GEOMBase_Skeleton::activate( GEOM_MARKER );
    }
  }
}














