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
//  File   : GeometryGUI_VectorDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

using namespace std;
#include "GeometryGUI_VectorDlg.h"

#include "GeometryGUI.h"
#include "QAD_Application.h"
#include "QAD_Desktop.h"
#include "QAD_Config.h"
#include "utilities.h"

#include <qbuttongroup.h>
#include <qframe.h>
#include <qgroupbox.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qtoolbutton.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qvalidator.h>
#include <qevent.h>




//=================================================================================
// class    : GeometryGUI_VectorDlg()
// purpose  : Constructs a GeometryGUI_VectorDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
GeometryGUI_VectorDlg::GeometryGUI_VectorDlg( QWidget* parent,  const char* name, SALOME_Selection* Sel, bool modal, WFlags fl )
    : QDialog( parent, name, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu )
{
    QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap( "GEOM",tr("ICON_DLG_VECTOR_2P")));
    QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap( "GEOM",tr("ICON_SELECT")));
    QPixmap image2(QAD_Desktop::getResourceManager()->loadPixmap( "GEOM",tr("ICON_DLG_VECTOR_DXYZ")));

    if ( !name )
	setName( "GeometryGUI_VectorDlg" );
    resize( 303, 221 );
    setCaption( tr( "GEOM_VECTOR_TITLE"  ) );
    setSizeGripEnabled( TRUE );
    GeometryGUI_VectorDlgLayout = new QGridLayout( this ); 
    GeometryGUI_VectorDlgLayout->setSpacing( 6 );
    GeometryGUI_VectorDlgLayout->setMargin( 11 );

    /***************************************************************/
    GroupPoints = new QGroupBox( this, "GroupPoints" );
    GroupPoints->setGeometry( QRect( 10, 10, 280, 90 ) ); 
    GroupPoints->setTitle( tr( "GEOM_POINTS"  ) );
    GroupPoints->setFrameShape( QGroupBox::Box );
    GroupPoints->setFrameShadow( QGroupBox::Sunken );
    GroupPoints->setColumnLayout(0, Qt::Vertical );
    GroupPoints->layout()->setSpacing( 0 );
    GroupPoints->layout()->setMargin( 0 );
    GroupPointsLayout = new QGridLayout( GroupPoints->layout() );
    GroupPointsLayout->setAlignment( Qt::AlignTop );
    GroupPointsLayout->setSpacing( 6 );
    GroupPointsLayout->setMargin( 11 );
    SelectButtonPt2 = new QPushButton( GroupPoints, "SelectButtonPt2" );
    SelectButtonPt2->setText( tr( ""  ) );
    SelectButtonPt2->setPixmap( image1 );
    GroupPointsLayout->addWidget( SelectButtonPt2, 1, 1 );
    LineEditPt1 = new QLineEdit( GroupPoints, "LineEditPt1" );
    GroupPointsLayout->addWidget( LineEditPt1, 0, 2 );
    LineEditPt2 = new QLineEdit( GroupPoints, "LineEditPt2" );
    GroupPointsLayout->addWidget( LineEditPt2, 1, 2 );
    SelectButtonPt1 = new QPushButton( GroupPoints, "SelectButtonPt1" );
    SelectButtonPt1->setText( tr( ""  ) );
    SelectButtonPt1->setPixmap( image1 );
    SelectButtonPt1->setToggleButton( FALSE );
    GroupPointsLayout->addWidget( SelectButtonPt1, 0, 1 );
    TextLabelPt1 = new QLabel( GroupPoints, "TextLabelPt1" );
    TextLabelPt1->setText( tr( "GEOM_POINT_I"  ).arg("1") );
    TextLabelPt1->setMinimumSize( QSize( 50, 0 ) );
    TextLabelPt1->setFrameShape( QLabel::NoFrame );
    TextLabelPt1->setFrameShadow( QLabel::Plain );
    GroupPointsLayout->addWidget( TextLabelPt1, 0, 0 );
    TextLabelPt2 = new QLabel( GroupPoints, "TextLabelPt2" );
    TextLabelPt2->setText( tr( "GEOM_POINT_I"  ).arg("2") );
    TextLabelPt2->setMinimumSize( QSize( 50, 0 ) );
    GroupPointsLayout->addWidget( TextLabelPt2, 1, 0 );
    GeometryGUI_VectorDlgLayout->addWidget( GroupPoints, 1, 0 );

    /***************************************************************/
    GroupButtons = new QGroupBox( this, "GroupButtons" );
    GroupButtons->setGeometry( QRect( 10, 10, 281, 48 ) ); 
    GroupButtons->setTitle( tr( ""  ) );
    GroupButtons->setColumnLayout(0, Qt::Vertical );
    GroupButtons->layout()->setSpacing( 0 );
    GroupButtons->layout()->setMargin( 0 );
    GroupButtonsLayout = new QGridLayout( GroupButtons->layout() );
    GroupButtonsLayout->setAlignment( Qt::AlignTop );
    GroupButtonsLayout->setSpacing( 6 );
    GroupButtonsLayout->setMargin( 11 );
    buttonCancel = new QPushButton( GroupButtons, "buttonCancel" );
    buttonCancel->setText( tr( "GEOM_BUT_CLOSE"  ) );
    buttonCancel->setAutoDefault( TRUE );
    GroupButtonsLayout->addWidget( buttonCancel, 0, 3 );
    buttonApply = new QPushButton( GroupButtons, "buttonApply" );
    buttonApply->setText( tr( "GEOM_BUT_APPLY"  ) );
    buttonApply->setAutoDefault( TRUE );
    GroupButtonsLayout->addWidget( buttonApply, 0, 1 );
    QSpacerItem* spacer_9 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    GroupButtonsLayout->addItem( spacer_9, 0, 2 );
    buttonOk = new QPushButton( GroupButtons, "buttonOk" );
    buttonOk->setText( tr( "GEOM_BUT_OK"  ) );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
    GroupButtonsLayout->addWidget( buttonOk, 0, 0 );
    GeometryGUI_VectorDlgLayout->addWidget( GroupButtons, 2, 0 );

    /***************************************************************/
    GroupConstructors = new QButtonGroup( this, "GroupConstructors" );
    GroupConstructors->setTitle( tr( "GEOM_VECTOR"  ) );
    GroupConstructors->setColumnLayout(0, Qt::Vertical );
    GroupConstructors->layout()->setSpacing( 0 );
    GroupConstructors->layout()->setMargin( 0 );
    GroupConstructorsLayout = new QGridLayout( GroupConstructors->layout() );
    GroupConstructorsLayout->setAlignment( Qt::AlignTop );
    GroupConstructorsLayout->setSpacing( 6 );
    GroupConstructorsLayout->setMargin( 11 );
    Constructor1 = new QRadioButton( GroupConstructors, "Constructor1" );
    Constructor1->setText( tr( ""  ) );
    Constructor1->setChecked( TRUE );
    Constructor1->setPixmap( image0 );
    Constructor1->setMinimumSize( QSize( 50, 0 ) );
    GroupConstructorsLayout->addWidget( Constructor1, 0, 0 );
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    GroupConstructorsLayout->addItem( spacer_2, 0, 3 );
    Constructor2 = new QRadioButton( GroupConstructors, "Constructor2" );
    Constructor2->setText( tr( ""  ) );
    Constructor2->setPixmap( image2 );
    Constructor2->setMinimumSize( QSize( 50, 0 ) );
    GroupConstructorsLayout->addWidget( Constructor2, 0, 2 );
    QSpacerItem* spacer_3 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    GroupConstructorsLayout->addItem( spacer_3, 0, 1 );
    GeometryGUI_VectorDlgLayout->addWidget( GroupConstructors, 0, 0 );

    /***************************************************************/
    GroupCoordinates = new QGroupBox( this, "GroupCoordinates" );
    GroupCoordinates->setGeometry( QRect( 10, 70, 280, 80 ) ); 
    GroupCoordinates->setTitle( tr( "GEOM_COORDINATES"  ) );
    GroupCoordinates->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, GroupCoordinates->sizePolicy().hasHeightForWidth() ) );
    GroupCoordinates->setColumnLayout(0, Qt::Vertical );
    GroupCoordinates->layout()->setSpacing( 0 );
    GroupCoordinates->layout()->setMargin( 0 );
    GroupCoordinatesLayout = new QGridLayout( GroupCoordinates->layout() );
    GroupCoordinatesLayout->setAlignment( Qt::AlignTop );
    GroupCoordinatesLayout->setSpacing( 6 );
    GroupCoordinatesLayout->setMargin( 11 );

    TextLabel_DZ = new QLabel( GroupCoordinates, "TextLabel_DZ" );
    TextLabel_DZ->setText( tr( "GEOM_DZ"  ) );
    GroupCoordinatesLayout->addWidget( TextLabel_DZ, 0, 4 );
    TextLabel_DY = new QLabel( GroupCoordinates, "TextLabel_DY" );
    TextLabel_DY->setText( tr( "GEOM_DY"  ) );
    GroupCoordinatesLayout->addWidget( TextLabel_DY, 0, 2 );
    TextLabel_DX = new QLabel( GroupCoordinates, "TextLabel_DX" );
    TextLabel_DX->setText( tr( "GEOM_DX"  ) );
    GroupCoordinatesLayout->addWidget( TextLabel_DX, 0, 0 );

    /* Spin boxes construction */
    SpinBox_DX = new GeometryGUI_SpinBox( GroupCoordinates, "GeomSpinBox_DX" ) ;
    GroupCoordinatesLayout->addWidget( SpinBox_DX, 0, 1 );
    SpinBox_DY = new GeometryGUI_SpinBox( GroupCoordinates, "GeomSpinBox_DY" ) ;
    GroupCoordinatesLayout->addWidget( SpinBox_DY, 0, 3 );
    SpinBox_DZ = new GeometryGUI_SpinBox( GroupCoordinates, "GeomSpinBox_DZ" ) ;
    GroupCoordinatesLayout->addWidget( SpinBox_DZ, 0, 5 );

    QSpacerItem* spacer2 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Minimum );
    GroupCoordinatesLayout->addItem( spacer2, 1, 3 );

    /* QCheckBox to reverse vector */
    CheckBoxReverseVector = new QCheckBox( GroupCoordinates, "CheckBoxReverseVector" );
    CheckBoxReverseVector->setText( tr( "GEOM_REVERSE_VECTOR"  ) );
    GroupCoordinatesLayout->addMultiCellWidget( CheckBoxReverseVector, 1, 1, 1, 5 );
    /* same position than GroupPoints */
    GeometryGUI_VectorDlgLayout->addWidget( GroupCoordinates, 1, 0 );
    
    Init(Sel) ; /* Initialisations */
    
}


//=================================================================================
// function : ~GeometryGUI_VectorDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
GeometryGUI_VectorDlg::~GeometryGUI_VectorDlg()
{  
  /* no need to delete child widgets, Qt does it all for us */
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void GeometryGUI_VectorDlg::Init( SALOME_Selection* Sel )
{
  
  double step ;
  QString St = QAD_CONFIG->getSetting( "Geometry:SettingsGeomStep" ) ;
  step = St.toDouble() ;
 
  /* min, max, step and decimals for spin boxes */
  SpinBox_DX->RangeStepAndValidator( -999.999, 999.999, step, 3 ) ;
  SpinBox_DX->SetValue( 0.0 ) ;  
  SpinBox_DY->RangeStepAndValidator( -999.999, 999.999, step, 3 ) ;
  SpinBox_DY->SetValue( 0.0 ) ;
  SpinBox_DZ->RangeStepAndValidator( -999.999, 999.999, step, 3 ) ;
  SpinBox_DZ->SetValue( 200.0 ) ; 
      
  GroupPoints->show();
  GroupCoordinates->hide() ;
  myConstructorId = 0 ;
  Constructor1->setChecked( TRUE );
  CheckBoxReverseVector->setChecked( FALSE );

  myEditCurrentArgument = LineEditPt1 ;	
  mySelection = Sel;
  myGeomGUI = GeometryGUI::GetGeometryGUI() ;
  myPoint1.SetCoord( 0.0, 0.0, 0.0 );
  myPoint2.SetCoord( 0.0, 0.0, 0.0 );
  myOkPoint1 = myOkPoint2 = false ;
  mySimulationTopoDs.Nullify() ;
  myGeomGUI->SetActiveDialogBox( (QDialog*)this ) ;  

  // TODO : previous selection in argument ?


  /* Filter definitions */
  Engines::Component_var comp = QAD_Application::getDesktop()->getEngine("FactoryServer", "GEOM");
  myGeom = GEOM::GEOM_Gen::_narrow(comp);
  myVertexFilter = new GEOM_ShapeTypeFilter( TopAbs_VERTEX, myGeom );
  myEdgeFilter   = new GEOM_ShapeTypeFilter( TopAbs_EDGE, myGeom );
  mySelection->AddFilter(myVertexFilter) ; /* first filter used */

  /* signals and slots connections */
  connect( buttonOk, SIGNAL( clicked() ),     this, SLOT( ClickOnOk() ) );
  connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( ClickOnCancel() ) ) ;
  connect( buttonApply, SIGNAL( clicked() ), this, SLOT(ClickOnApply() ) );
  connect( GroupConstructors, SIGNAL(clicked(int) ), SLOT( ConstructorsClicked(int) ) );  
  connect( SelectButtonPt1, SIGNAL (clicked() ), this, SLOT( SetEditCurrentArgument() ) ) ;
  connect( SelectButtonPt2, SIGNAL (clicked() ), this, SLOT( SetEditCurrentArgument() ) ) ;

  connect( SpinBox_DX, SIGNAL ( valueChanged( double) ), this, SLOT( ValueChangedInSpinBox( double) ) ) ;
  connect( SpinBox_DY, SIGNAL ( valueChanged( double) ), this, SLOT( ValueChangedInSpinBox( double) ) ) ;
  connect( SpinBox_DZ, SIGNAL ( valueChanged( double) ), this, SLOT( ValueChangedInSpinBox( double) ) ) ;
  
  connect( LineEditPt1, SIGNAL ( returnPressed() ), this, SLOT( LineEditReturnPressed() ) ) ;
  connect( LineEditPt2, SIGNAL ( returnPressed() ), this, SLOT( LineEditReturnPressed() ) ) ;

  connect( CheckBoxReverseVector, SIGNAL (stateChanged(int) ), this, SLOT( ReverseVector(int) ) ) ;
  
  connect( myGeomGUI, SIGNAL ( SignalDeactivateActiveDialog() ), this, SLOT( DeactivateActiveDialog() ) ) ;  
  connect( mySelection, SIGNAL( currentSelectionChanged() ),     this, SLOT( SelectionIntoArgument() ) );
  /* to close dialog if study change */
  connect( myGeomGUI, SIGNAL ( SignalCloseAllDialogs() ), this, SLOT( ClickOnCancel() ) ) ;
 
  /* Move widget on the botton right corner of main widget */
  int x, y ;
  myGeomGUI->DefineDlgPosition( this, x, y ) ;
  this->move( x, y ) ;
  this->show() ; /* displays Dialog */

  return ;
}

//=================================================================================
// function : ReverseVector()
// purpose  : 'state' not used here
//=================================================================================
void GeometryGUI_VectorDlg::ReverseVector(int state)
{
  if( myConstructorId == 1 ) {
    
    myDx = -myDx ; 
    myDy = -myDy ; 
    myDz = -myDz ;
    
    SpinBox_DX->SetValue( myDx ) ;
    SpinBox_DY->SetValue( myDy ) ;
    SpinBox_DZ->SetValue( myDz ) ;
    
    myPoint1.SetCoord(0.0, 0.0, 0.0) ;
    myPoint2.SetCoord(myDx, myDy, myDz) ;
    
    myGeomGUI->EraseSimulationShape() ; 
    mySimulationTopoDs.Nullify() ;
    /* In the appropriate constructor */
    if( myPoint1.Distance(myPoint2) > Precision::Confusion() ) {
      mySimulationTopoDs = BRepBuilderAPI_MakeEdge( myPoint1, myPoint2 ).Shape();
      /* Add arrow in simulation */
      bool noNeedToTest = AddArrowToSimulation(mySimulationTopoDs) ;
      myGeomGUI->DisplaySimulationShape( mySimulationTopoDs ) ;
    }
  }
  return ;
} 



//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void GeometryGUI_VectorDlg::ConstructorsClicked(int constructorId)
{
  myGeomGUI->EraseSimulationShape() ;
  mySelection->ClearFilters() ;

  switch (constructorId)
    {
    case 0:
      {
	GroupPoints->show();
	GroupCoordinates->hide() ;
	myConstructorId = constructorId ;
	myEditCurrentArgument = LineEditPt1 ;	
	Constructor1->setChecked( TRUE );
	LineEditPt1->setText(tr("")) ;
	LineEditPt2->setText(tr("")) ;
	myOkPoint1 = myOkPoint2 = false ;

	/* filter for next selections */
	mySelection->AddFilter( myVertexFilter );
	connect ( mySelection, SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );
	break;
      }
    case 1:
      {
	GroupPoints->hide();
	GroupCoordinates->show();
	myConstructorId = constructorId ;
	CheckBoxReverseVector->setChecked( FALSE );
	myOkPoint1 = myOkPoint2 = false ;

	myEditCurrentArgument = SpinBox_DX ;	
	myPoint1.SetCoord( 0.0, 0.0, 0.0 ) ;
	myDx = 0.0 ;
	myDy = 0.0 ;
	myDz = 200.0 ;
	
	myPoint2.SetCoord( myDx, myDy, myDz ) ;
	SpinBox_DX->SetValue( myDx ) ;
	SpinBox_DY->SetValue( myDy ) ;
	SpinBox_DZ->SetValue( myDz ) ;
	
	/* filter for next selections */
	mySelection->AddFilter( myEdgeFilter );
	connect ( mySelection, SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );

	if( myPoint1.Distance(myPoint2) > Precision::Confusion() ) {
	  mySimulationTopoDs = BRepBuilderAPI_MakeEdge( myPoint1, myPoint2 ).Shape();
	  /* Add arrow in simulation */
	  bool noNeedToTest = AddArrowToSimulation(mySimulationTopoDs) ;
	  myGeomGUI->DisplaySimulationShape( mySimulationTopoDs ) ; 
	}
	break;
      }
    }
 return ;
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void GeometryGUI_VectorDlg::ClickOnOk()
{
  this->ClickOnApply() ;
  this->ClickOnCancel() ;

  return ;
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
void GeometryGUI_VectorDlg::ClickOnApply()
{
  myGeomGUI->EraseSimulationShape() ;
  myGeomGUI->GetDesktop()->putInfo( tr("") ) ; 
  mySimulationTopoDs.Nullify() ;
  switch(myConstructorId)
    { 
    case 0 :
      { 
	if(myOkPoint1 && myOkPoint2) 
	  myGeomGUI->MakeVectorAndDisplay( myPoint1, myPoint2 ) ;
	break ;
      }
    case 1 :
      {
	/* Recup args and call method */
	myDx = SpinBox_DX->GetValue() ;
	myDy = SpinBox_DY->GetValue() ;
	myDz = SpinBox_DZ->GetValue() ;
	myPoint1.SetCoord(0.0, 0.0, 0.0) ;
	myPoint2.SetCoord(myDx, myDy, myDz) ;	
	myGeomGUI->MakeVectorAndDisplay( myPoint1, myPoint2 ) ;
	break ;
      }
    }

  // accept();
  return ;
}


//=================================================================================
// function : ClickOnCancel()
// purpose  :
//=================================================================================
void GeometryGUI_VectorDlg::ClickOnCancel()
{
  mySelection->ClearFilters() ;
  myGeomGUI->EraseSimulationShape() ;
  mySimulationTopoDs.Nullify() ;
  disconnect( mySelection, 0, this, 0 );
  myGeomGUI->ResetState() ;
  reject() ;
  return ;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void GeometryGUI_VectorDlg::SelectionIntoArgument()
{
  myGeomGUI->EraseSimulationShape() ; 
  mySimulationTopoDs.Nullify() ;

  /* Future name of shape */
  QString aString = "";
  
  int nbSel = myGeomGUI->GetNameOfSelectedIObjects(mySelection, aString) ;
  if ( nbSel != 1 ) {
    if ( myEditCurrentArgument == LineEditPt1 ) {
      LineEditPt1->setText("") ;
      myOkPoint1 = false ;
    }
    else if ( myEditCurrentArgument == LineEditPt2 ) { 
      LineEditPt2->setText("") ;
      myOkPoint2 = false ;
    }
    return ;
  }

  TopoDS_Shape S; 
  if( !myGeomGUI->GetTopoFromSelection(mySelection, S) )
    return ;

  /* Constructors treatment */
  if ( myConstructorId == 0 && myEditCurrentArgument == LineEditPt1 && myGeomGUI->VertexToPoint(S, myPoint1) ) {
    LineEditPt1->setText(aString) ;
    myOkPoint1 = true ;
  }    
  else if ( myConstructorId == 0 && myEditCurrentArgument == LineEditPt2 && myGeomGUI->VertexToPoint(S, myPoint2)  ) {
    LineEditPt2->setText(aString) ;
    myOkPoint2 = true ;
  }
  else if( myConstructorId == 1) {
    gp_Pnt P1, P2 ;
     /* linear edge selection verified */
    if(  myGeomGUI->LinearEdgeExtremities(S, P1, P2) )  {

      myGeomGUI->GetBipointDxDyDz( P1, P2, myDx, myDy, myDz ) ;
      SpinBox_DX->SetValue( myDx ) ;
      SpinBox_DY->SetValue( myDy ) ;
      SpinBox_DZ->SetValue( myDz ) ;

      myPoint1.SetCoord(0.0, 0.0, 0.0) ;
      myPoint2.SetCoord(myDx, myDy, myDz) ;
    }
  }
  
  Standard_Real d = myPoint1.Distance(myPoint2) ;

  if( ( myConstructorId == 0 && myOkPoint1 && myOkPoint2 && d > Precision::Confusion() ) ||
      ( myConstructorId == 1 && d > Precision::Confusion() ) ) {
    mySimulationTopoDs = BRepBuilderAPI_MakeEdge( myPoint1, myPoint2 ).Shape();
    /* Add arrow in simulation */
    bool noNeedToTest  = AddArrowToSimulation( mySimulationTopoDs ) ;
    myGeomGUI->DisplaySimulationShape( mySimulationTopoDs ) ; 
  }

  return ;
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void GeometryGUI_VectorDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();
  switch (myConstructorId)
    {
    case 0: /* default constructor */
      {	
	if(send == SelectButtonPt1) {
	  LineEditPt1->setFocus() ;
	  myEditCurrentArgument = LineEditPt1;
	}
	else if(send == SelectButtonPt2) {
	  LineEditPt2->setFocus() ;	  
	  myEditCurrentArgument = LineEditPt2;
	}	
	mySelection->AddFilter(myVertexFilter) ;
	SelectionIntoArgument() ;
	break;
      }
    }
  return ;
}



//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void GeometryGUI_VectorDlg::ValueChangedInSpinBox( double newValue )
{
  if(myConstructorId != 1)
    return ;
  
  myGeomGUI->EraseSimulationShape() ; 
  mySimulationTopoDs.Nullify() ;
  
  QObject* send = (QObject*)sender() ;
  if (send == SpinBox_DX) {
    myDx = newValue ;
  } else if( send == SpinBox_DY ) {
    myDy = newValue ;
  } else if( send == SpinBox_DZ ) {
    myDz = newValue ;
  }
  
  myPoint1.SetCoord(0.0, 0.0, 0.0) ;
  myPoint2.SetCoord(myDx, myDy, myDz) ;
  
  if ( myConstructorId == 1 && myPoint1.Distance(myPoint2) > Precision::Confusion() ) { // mySimulationTopoDs
    mySimulationTopoDs = BRepBuilderAPI_MakeEdge( myPoint1, myPoint2 ).Shape();
    /* Create simulation vector with an arrow */
    this->AddArrowToSimulation(mySimulationTopoDs ) ;
    myGeomGUI->DisplaySimulationShape( mySimulationTopoDs ) ; 
  }
  return ;
}
  


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void GeometryGUI_VectorDlg::LineEditReturnPressed()
{ 
  QLineEdit* send = (QLineEdit*)sender();  
  if( send == LineEditPt1 )
    myEditCurrentArgument = LineEditPt1 ;
  else if ( send == LineEditPt2 )
    myEditCurrentArgument = LineEditPt2 ; 
  else
    return ;
  
  /* User name of object input management                          */
  /* If successfull the selection is changed and signal emitted... */
  /* so SelectionIntoArgument() is automatically called.           */
  QLineEdit* LE = (QLineEdit*)myEditCurrentArgument ;
  const QString objectUserName = LE->text() ;
  QWidget* thisWidget = (QWidget*)this ;
  if( myGeomGUI->SelectionByNameInDialogs( thisWidget, objectUserName, mySelection ) ) {
    LE->setText( objectUserName ) ;
  }
  return ;
}


//=================================================================================
// function : DeactivateActiveDialog()
// purpose  :
//=================================================================================
void GeometryGUI_VectorDlg::DeactivateActiveDialog()
{
  if ( GroupConstructors->isEnabled() ) {

    GroupConstructors->setEnabled(false) ;
    GroupCoordinates->setEnabled(false) ;
    GroupButtons->setEnabled(false) ;
    GroupPoints->setEnabled(false) ;
    disconnect( mySelection, 0, this, 0 );
    myGeomGUI->EraseSimulationShape() ;
    mySelection->ClearFilters() ;
  }
  return ;
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void GeometryGUI_VectorDlg::ActivateThisDialog()
{
  /* Emit a signal to deactivate the active dialog */
  myGeomGUI->EmitSignalDeactivateDialog() ; 
  
  GroupConstructors->setEnabled(true) ;
  GroupCoordinates->setEnabled(true) ;
  GroupButtons->setEnabled(true) ;
  GroupPoints->setEnabled(true) ;

  connect ( mySelection, SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );
  if( !mySimulationTopoDs.IsNull() )
    myGeomGUI->DisplaySimulationShape( mySimulationTopoDs ) ;

  return ;
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void GeometryGUI_VectorDlg::enterEvent(QEvent* e)
{
  if ( GroupConstructors->isEnabled() )
    return ;  
  ActivateThisDialog() ;
}



//=================================================================================
// function : closeEvent()
// purpose  :
//=================================================================================
void GeometryGUI_VectorDlg::closeEvent( QCloseEvent* e )
{
  this->ClickOnCancel() ; /* same than click on cancel button */
}


//=================================================================================
// function : AddArrowToSimulation()
// purpose  : An arrow (cone topology) is added to 'modifiedShape'
//          : to simulate a vector or an 'oriented line' display. The result is in 'modifiedShape'.
//          : If an arrow can't be added returns false and 'modifiedShape' isn't modified !
//=================================================================================
bool GeometryGUI_VectorDlg::AddArrowToSimulation( TopoDS_Shape& modifiedShape )
{
  TopoDS_Shape arrow ;
  /* Try to add a cone simulation shape to show direction of a linear edge */
  if( myGeomGUI->CreateArrowForLinearEdge( modifiedShape, arrow ) ) {
    TopoDS_Compound Comp ;
    BRep_Builder B;
    B.MakeCompound (Comp);
    B.Add( Comp, modifiedShape ) ;
    B.Add( Comp, arrow ) ;
    modifiedShape = Comp ;
    return true ;
  }
  return false ;
}
