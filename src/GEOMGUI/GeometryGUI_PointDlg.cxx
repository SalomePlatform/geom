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
//  File   : GeometryGUI_PointDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

using namespace std;
#include "GeometryGUI_PointDlg.h"

#include "GeometryGUI.h"
#include "QAD_Application.h"
#include "QAD_RightFrame.h"
#include "QAD_Config.h"
#include "QAD_Desktop.h"
#include "QAD_Tools.h"
#include "OCCViewer_Viewer3d.h"
#include "utilities.h"

#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRep_Tool.hxx>
#if OCC_VERSION_MAJOR >= 5
#include <BRepAlgo.hxx>
#else
#include <BRepAlgoAPI.hxx>
#endif
#include <Geom_Curve.hxx>

#include <qbuttongroup.h>
#include <qframe.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qvalidator.h>
#include <qevent.h>



//=================================================================================
// class    : GeometryGUI_PointDlg()
// purpose  : Constructs a GeometryGUI_PointDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
GeometryGUI_PointDlg::GeometryGUI_PointDlg( QWidget* parent,
					    const char* name,
					    SALOME_Selection* Sel,
					    const Handle (AIS_InteractiveContext)& ic,
					    bool modal,
					    WFlags fl )
    : QDialog( parent, name, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu )
{

    QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap( "GEOM",tr("ICON_DLG_POINT")));
    QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap( "GEOM",tr("ICON_SELECT")));
    QPixmap image2(QAD_Desktop::getResourceManager()->loadPixmap( "GEOM",tr("ICON_DLG_POINT_EDGE")));

    if ( !name )
	setName( "GeometryGUI_PointDlg" );
    resize( 303, 185 ); 
    setCaption( tr( "GEOM_POINT_TITLE"  ) );
    setSizeGripEnabled( TRUE );
    GeometryGUI_PointDlgLayout = new QGridLayout( this ); 
    GeometryGUI_PointDlgLayout->setSpacing( 6 );
    GeometryGUI_PointDlgLayout->setMargin( 11 );

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
    GeometryGUI_PointDlgLayout->addWidget( GroupButtons, 2, 0 );

    /***************************************************************/
    GroupConstructors = new QButtonGroup( this, "GroupConstructors" );
    GroupConstructors->setTitle( tr( "Constructors"  ) );
    GroupConstructors->setExclusive( TRUE );
    GroupConstructors->setColumnLayout(0, Qt::Vertical );
    GroupConstructors->layout()->setSpacing( 0 );
    GroupConstructors->layout()->setMargin( 0 );
    GroupConstructorsLayout = new QGridLayout( GroupConstructors->layout() );
    GroupConstructorsLayout->setAlignment( Qt::AlignTop );
    GroupConstructorsLayout->setSpacing( 6 );
    GroupConstructorsLayout->setMargin( 11 );

    Constructor1 = new QRadioButton( GroupConstructors, "Constructor1" );
    Constructor1->setText( tr( ""  ) );
    Constructor1->setPixmap( image0 );
    Constructor1->setChecked( FALSE );
    Constructor1->setMinimumSize( QSize( 50, 0 ) );

    GroupConstructorsLayout->addWidget( Constructor1, 0, 0 );
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    GroupConstructorsLayout->addItem( spacer_2, 0, 1 );

    Constructor2 = new QRadioButton( GroupConstructors, "Constructor2" );
    Constructor2->setText( tr( ""  ) );
    Constructor2->setPixmap( image2 );
    Constructor2->setChecked( TRUE );
    Constructor2->setMinimumSize( QSize( 50, 0 ) );

    GroupConstructorsLayout->addWidget( Constructor2, 0, 2 );
    QSpacerItem* spacer_3 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    GroupConstructorsLayout->addItem( spacer_3, 0, 3 );

    GeometryGUI_PointDlgLayout->addWidget( GroupConstructors, 0, 0 );


    /***************************************************************/
    GroupCoordinates = new QGroupBox( this, "GroupCoordinates" );
    GroupCoordinates->setTitle( tr( "GEOM_COORDINATES"  ) );
    GroupCoordinates->setColumnLayout(0, Qt::Vertical );
    GroupCoordinates->layout()->setSpacing( 0 );
    GroupCoordinates->layout()->setMargin( 0 );
    GroupCoordinatesLayout = new QGridLayout( GroupCoordinates->layout() );
    GroupCoordinatesLayout->setAlignment( Qt::AlignTop );
    GroupCoordinatesLayout->setSpacing( 6 );
    GroupCoordinatesLayout->setMargin( 11 );

    TextLabel_X = new QLabel( GroupCoordinates, "TextLabel_X" );
    TextLabel_X->setText( tr( "GEOM_X"  ) );
    GroupCoordinatesLayout->addWidget( TextLabel_X, 0, 0 );
    TextLabel_Y = new QLabel( GroupCoordinates, "TextLabel_Y" );
    TextLabel_Y->setText( tr( "GEOM_Y"  ) );
    GroupCoordinatesLayout->addWidget( TextLabel_Y, 0, 2 );
    TextLabel_Z = new QLabel( GroupCoordinates, "TextLabel_Z" );
    TextLabel_Z->setText( tr( "GEOM_Z"  ) );
    GroupCoordinatesLayout->addWidget( TextLabel_Z, 0, 4 );
 
    /* Spin boxes construction */
    SpinBox_X = new GeometryGUI_SpinBox( GroupCoordinates, "GeomSpinBox_X" ) ;
    GroupCoordinatesLayout->addWidget( SpinBox_X, 0, 1 );
    SpinBox_Y = new GeometryGUI_SpinBox( GroupCoordinates, "GeomSpinBox_Y" ) ;
    GroupCoordinatesLayout->addWidget( SpinBox_Y, 0, 3 );
    SpinBox_Z = new GeometryGUI_SpinBox( GroupCoordinates, "GeomSpinBox_Z" ) ;
    GroupCoordinatesLayout->addWidget( SpinBox_Z, 0, 5 );
    GeometryGUI_PointDlgLayout->addWidget( GroupCoordinates, 1, 0 );

    /***************************************************************/
    GroupWithEdge = new QGroupBox( this, "GroupWithEdge" );
    GroupWithEdge->setTitle( tr( "GEOM_PARAM_POINT" ) );
    GroupWithEdge->setFrameShape( QGroupBox::Box );
    GroupWithEdge->setFrameShadow( QGroupBox::Sunken );
    GroupWithEdge->setColumnLayout(0, Qt::Vertical );
    GroupWithEdge->layout()->setSpacing( 0 );
    GroupWithEdge->layout()->setMargin( 0 );
    GroupWithEdgeLayout = new QGridLayout( GroupWithEdge->layout() );
    GroupWithEdgeLayout->setAlignment( Qt::AlignTop );
    GroupWithEdgeLayout->setSpacing( 6 );
    GroupWithEdgeLayout->setMargin( 11 );

    LineEdit_Edge = new QLineEdit( GroupWithEdge, "LineEdit_Edge" );
    GroupWithEdgeLayout->addWidget( LineEdit_Edge, 0, 2 );

    SelectButton_Edge = new QPushButton( GroupWithEdge, "SelectButton_Edge" );
    SelectButton_Edge->setText( tr( ""  ) );
    SelectButton_Edge->setPixmap( image1 );
    SelectButton_Edge->setToggleButton( FALSE );
    GroupWithEdgeLayout->addWidget( SelectButton_Edge, 0, 1 );
 
    /* Spin box */
    SpinBox_Parameter = new GeometryGUI_SpinBox( GroupWithEdge, "SpinBox_Parameter" );
    GroupWithEdgeLayout->addWidget( SpinBox_Parameter, 1, 2 );

    TextLabel_Edge = new QLabel( GroupWithEdge, "TextLabel_Edge" );
    TextLabel_Edge->setText( tr( "GEOM_EDGE"  ) );
    TextLabel_Edge->setMinimumSize( QSize( 50, 0 ) );
    TextLabel_Edge->setFrameShape( QLabel::NoFrame );
    TextLabel_Edge->setFrameShadow( QLabel::Plain );
    GroupWithEdgeLayout->addWidget( TextLabel_Edge, 0, 0 );

    TextLabel_Parameter = new QLabel( GroupWithEdge, "TextLabel_Parameter" );
    TextLabel_Parameter->setText( tr( "GEOM_PARAMETER"  ) );
    TextLabel_Parameter->setMinimumSize( QSize( 50, 0 ) );
    TextLabel_Parameter->setFrameShape( QLabel::NoFrame );
    TextLabel_Parameter->setFrameShadow( QLabel::Plain );
    GroupWithEdgeLayout->addWidget( TextLabel_Parameter, 1, 0 );

    GeometryGUI_PointDlgLayout->addWidget( GroupWithEdge, 1, 0 );
    /***************************************************************/

    /* Initialisation and display */
    Init(Sel, ic) ;
 
    /* Move widget on the botton right corner of main widget */
    QAD_Tools::alignWidget(this, parent, AlignBottom | AlignRight);
    /* Display  Dialog */
    this->show() ; 
}


//=======================================================================
// function : ~GeometryGUI_PointDlg()
// purpose  : Destructor
//=======================================================================
GeometryGUI_PointDlg::~GeometryGUI_PointDlg()
{
  /* no need to delete child widgets, Qt does it all for us */
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void GeometryGUI_PointDlg::Init(SALOME_Selection* Sel, const Handle(AIS_InteractiveContext)& ic)
{
  
  /* Get setting of step value from file configuration */
  double step ;
  QString St = QAD_CONFIG->getSetting( "Geometry:SettingsGeomStep" ) ;
  step = St.toDouble() ;

  /* min, max, step and decimals for spin boxes */
  SpinBox_X->RangeStepAndValidator( -999.999, 999.999, step, 3 ) ;
  SpinBox_X->SetValue( 0.0 ) ;
  SpinBox_Y->RangeStepAndValidator( -999.999, 999.999, step, 3 ) ;
  SpinBox_Y->SetValue( 0.0 ) ;
  SpinBox_Z->RangeStepAndValidator( -999.999, 999.999, step, 3 ) ;
  SpinBox_Z->SetValue( 0.0 ) ; 

  /* spin box for parameter on edge */
  double specificStep = 0.1 ;
  SpinBox_Parameter->RangeStepAndValidator( -999999.99999, 999999.99999, specificStep, 5 ) ;
  SpinBox_Parameter->SetValue( 0.50 ) ;
  myParameter = 0.50 ;

  GroupCoordinates->show();
  GroupWithEdge->hide() ;
  myConstructorId = 0 ;
  Constructor1->setChecked( TRUE );

  /* filter for the second constructor */
  Engines::Component_var comp = QAD_Application::getDesktop()->getEngine("FactoryServer", "GEOM");
  myGeom = GEOM::GEOM_Gen::_narrow(comp);
  myEdgeFilter = new GEOM_ShapeTypeFilter( TopAbs_EDGE, myGeom );
  
  bool displayPoint = false ;
  mySelection = Sel ;
  myGeomGUI = GeometryGUI::GetGeometryGUI() ;   
  myGeomGUI->SetActiveDialogBox( (QDialog*)this ) ;
  myGeomGUI->SetState(POINT_METHOD) ;
  
  mySelection = Sel;
  myOkEdge = false ;
  
  /* manages local context selection */
  myIC = ic ;

  if ( myGeomGUI->GetActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC ) {
    myLocalContextId = myIC->OpenLocalContext();
    myGeomGUI->SetDisplayedObjectList();
    /* sub shapes selection */
    myLocalContextMode = TopAbs_VERTEX ;
    myIC->ActivateStandardMode(myLocalContextMode) ;
    myUseLocalContext = true ;
  } else {
    myUseLocalContext = false ;
  }

  
  myPoint.SetCoord( 0.0, 0.0, 0.0 ) ;
  TopoDS_Shape S; 
  
  if( myGeomGUI->GetTopoFromSelection(mySelection, S) ) {
    
    /* Filter a possibly previous selection and try to put it into coordinates */
    if( myGeomGUI->VertexToPoint( S,  myPoint) )
      displayPoint = false ;
    else
      displayPoint = true ;
  }
  else {
    displayPoint = true ;
  }
  
  mySimulationTopoDs.Nullify() ;
  PointIntoCoordinates(myPoint, displayPoint) ;
  
  /* signals and slots connections */
  connect( buttonOk,          SIGNAL( clicked() ),   this, SLOT( ClickOnOk() ) );
  connect( buttonCancel,      SIGNAL( clicked() ),   this, SLOT( ClickOnCancel() ) );
  connect( buttonApply,       SIGNAL( clicked() ),   this, SLOT(ClickOnApply() ) );
  connect( SelectButton_Edge, SIGNAL (clicked() ), this, SLOT( SetEditCurrentArgument() ) ) ;

  connect( GroupConstructors, SIGNAL(clicked(int) ), this, SLOT( ConstructorsClicked(int) ) ) ;

  connect( SpinBox_X, SIGNAL ( valueChanged( double) ), this, SLOT( ValueChangedInSpinBox( double ) ) ) ;
  connect( SpinBox_Y, SIGNAL ( valueChanged( double) ), this, SLOT( ValueChangedInSpinBox( double ) ) ) ;
  connect( SpinBox_Z, SIGNAL ( valueChanged( double) ), this, SLOT( ValueChangedInSpinBox( double ) ) ) ;

  connect( SpinBox_Parameter,  SIGNAL ( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) ) ;
  connect( LineEdit_Edge,      SIGNAL ( returnPressed() ),                   this, SLOT( LineEditReturnPressed() ) ) ;
  
  connect( myGeomGUI,   SIGNAL ( SignalDeactivateActiveDialog() ),this, SLOT( DeactivateActiveDialog() ) ) ;
  connect( myGeomGUI,   SIGNAL ( SignalCloseAllDialogs() ),       this, SLOT( ClickOnCancel() ) ) ;
  // connect( mySelection, SIGNAL ( currentSelectionChanged() ),     this, SLOT( SelectionIntoArgument() ) ) ; done in constructor clicked !

  /* Move widget on the botton right corner of main widget */
  int x, y ;
  myGeomGUI->DefineDlgPosition( this, x, y ) ;
  this->move( x, y ) ;
  this->show() ; /* Displays this Dialog */
}



//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void GeometryGUI_PointDlg::ValueChangedInSpinBox( double newValue )
{
  GeometryGUI_SpinBox* send = (GeometryGUI_SpinBox*)sender() ;
  double vx, vy, vz ;
  if( send == SpinBox_X ) {
    vx = newValue ;
    vy = SpinBox_Y->GetValue() ;
    vz = SpinBox_Z->GetValue() ;
  } else if ( send == SpinBox_Y ) {
    vx = SpinBox_X->GetValue() ;
    vy = newValue ;
    vz = SpinBox_Z->GetValue() ;
  } else if (send == SpinBox_Z ) {
    vx = SpinBox_X->GetValue() ;
    vy = SpinBox_Y->GetValue() ;
    vz = newValue ;
  }  else if (send == SpinBox_Parameter ) {
    myParameter = newValue ;
  } else
    return ;
  
  
  switch (myConstructorId) {
  case 0: // default constructor 
    {
      myPoint.SetCoord(vx, vy, vz) ;
      myGeomGUI->EraseSimulationShape() ;
      mySimulationTopoDs = BRepBuilderAPI_MakeVertex (myPoint).Shape() ;
      myGeomGUI->DisplaySimulationShape( mySimulationTopoDs ) ;
      break ;
    }
  case 1: 
    {
      this->SelectionIntoArgument() ;	
      break ;
    }
  }

  return ;
}



//=======================================================================
// funcion  : PointIntoCoordinates()
// purpose  : Sets user point coordinates into this dialog Spin boxes
//          : and displays it or not according to 'bool displayPoint'
//=======================================================================
void GeometryGUI_PointDlg::PointIntoCoordinates(gp_Pnt P, bool displayPoint)
{
  switch (myConstructorId) {    
  case 0 : {
    SpinBox_X->SetValue( P.X() ) ;
    SpinBox_Y->SetValue( P.Y() ) ;
    SpinBox_Z->SetValue( P.Z() ) ;
    this->myPoint.SetCoord( P.X(), P.Y(), P.Z() ) ;
    if( displayPoint ) {
      mySimulationTopoDs = BRepBuilderAPI_MakeVertex(P).Shape() ;
      myGeomGUI->DisplaySimulationShape(mySimulationTopoDs) ;
    }
    break ;
  }
  case 1 :
    {
      // TODO
      break ;
    }
  }
  
  return ;
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void GeometryGUI_PointDlg::ConstructorsClicked(int constructorId)
{
  myGeomGUI->EraseSimulationShape() ;
  myConstructorId = constructorId ;
    switch (constructorId)
    {
    case 0:
      {
	if ( myUseLocalContext == false && myGeomGUI->GetActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC ) {
	  myLocalContextId = myIC->OpenLocalContext();
	  myGeomGUI->SetDisplayedObjectList();
	  /* sub shapes selection */
	  myLocalContextMode = TopAbs_VERTEX ;
	  myIC->ActivateStandardMode(myLocalContextMode) ;
	  myUseLocalContext = true ;
	}
	mySelection->ClearFilters() ;
	GroupCoordinates->show();
	GroupWithEdge->hide() ;
	/* Display point simulation */
	PointIntoCoordinates( this->myPoint, true ) ;
	disconnect( mySelection, 0, this, 0 );	
	break;
      }
    case 1:
      {
	if ( myUseLocalContext == true && myGeomGUI->GetActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC ) {
	  myIC->CloseLocalContext(myLocalContextId) ;
	  myUseLocalContext = false ;
	}
	LineEdit_Edge->setText("") ;
	mySelection->AddFilter( myEdgeFilter );
	GroupCoordinates->hide();
	GroupWithEdge->show() ;
	myOkEdge = false ;
	connect ( mySelection, SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );
	break;
      }
    }
 return ;
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void GeometryGUI_PointDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if( send == LineEdit_Edge ) {
    /* User name of object input management                          */
    /* If successfull the selection is changed and signal emitted... */
    /* so SelectionIntoArgument() is automatically called.           */
    const QString objectUserName = send->text() ;
    QWidget* thisWidget = (QWidget*)this ;
    if( myGeomGUI->SelectionByNameInDialogs( thisWidget, objectUserName, mySelection ) ) {
      LineEdit_Edge->setText( objectUserName ) ;
    }
  }
  return ;
}



//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void GeometryGUI_PointDlg::ClickOnOk()
{
  this->ClickOnApply() ;
  this->ClickOnCancel() ;

  return ;
}

//=======================================================================
// function : ClickOnApply()
// purpose  :
//=======================================================================
void GeometryGUI_PointDlg::ClickOnApply()
{
  
  myGeomGUI->EraseSimulationShape() ;
  
  /* Close local context */
  if ( myGeomGUI->GetActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC ) {
    myIC->CloseLocalContext(myLocalContextId) ;
    myUseLocalContext = false ;
  }

  switch (myConstructorId) {
    
    case 0 :
    {
      /* Recup args and call method */
      double x = SpinBox_X->GetValue() ;
      double y = SpinBox_Y->GetValue() ;
      double z = SpinBox_Z->GetValue() ;
      
      myGeomGUI->MakePointAndDisplay(x,y,z) ;  /* WARNING : no display if a local context is opened */
      if ( myGeomGUI->GetActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC ) {
	/* no display if a local context is opened */
	myLocalContextId = myIC->OpenLocalContext();
	myGeomGUI->SetDisplayedObjectList();
	/* sub shapes selection */
	myLocalContextMode = TopAbs_VERTEX ;
	myIC->ActivateStandardMode(myLocalContextMode);
	myUseLocalContext = true ;
      }
      break ;
    }
  case 1 :
    {
      if( myOkEdge == true ) {
	/* this constructor method has no idl interface : we use same than constructor 0 */
	myGeomGUI->MakePointAndDisplay( myPoint.X(), myPoint.Y(), myPoint.Z() ) ;
      }
      break ;
    }
  }
  
  return ;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed (for constructors not using local context)
//=================================================================================
void GeometryGUI_PointDlg::SelectionIntoArgument()
{
  myGeomGUI->EraseSimulationShape() ; 
  LineEdit_Edge->setText("") ;
  QString aString = "" ; /* future name of selection */
  int nbSel = myGeomGUI->GetNameOfSelectedIObjects(mySelection, aString) ;
  if ( nbSel != 1 ) {
    this->myOkEdge = false ;
    return ;
  }
  
  // nbSel == 1
  TopoDS_Shape S; 
  if( !myGeomGUI->GetTopoFromSelection(mySelection, S) )
    return ;
  
  switch (myConstructorId)
    {
    case 0:
      break ;
    case 1:
      {
	LineEdit_Edge->setText(aString) ;
	if( S.ShapeType() == TopAbs_EDGE ) {
	  if( CalculateVertexOnCurve( TopoDS::Edge(S), myParameter, mySimulationTopoDs) ) {
	    if ( myGeomGUI->VertexToPoint( mySimulationTopoDs, myPoint ) ) {
	      this->myOkEdge = true ;
	      myGeomGUI->DisplaySimulationShape(mySimulationTopoDs) ;
	    }
	  }
	}
	break ; 
      }
    }
  return ;
}



//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void GeometryGUI_PointDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();
  switch (myConstructorId)
    {
    case 0: // default constructor
       break;
    case 1:
      {	
	if(send == SelectButton_Edge) {
	  LineEdit_Edge->setFocus() ;
	  SelectionIntoArgument() ;
	}
	break;
      }
    }
  return ;
}


//=======================================================================
// function : UseLocalContext()
// purpose  : Return true when this method has opened a local context
//          : Used from GeometryGUI
//=======================================================================
bool GeometryGUI_PointDlg::UseLocalContext()
{
  return this->myUseLocalContext ;
}


//=======================================================================
// function : closeEvent()
// purpose  :
//=======================================================================
void GeometryGUI_PointDlg::closeEvent(QCloseEvent* e)
{
  this->ClickOnCancel() ; /* same than click on cancel button */
}


//=======================================================================
// function : ClickOnCancel()
// purpose  :
//=======================================================================
void GeometryGUI_PointDlg::ClickOnCancel()
{
  if ( myConstructorId == 0 && myGeomGUI->GetActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC ) {
    OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)myGeomGUI->GetActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
    myIC = v3d->getAISContext();
    if ( myIC->HasOpenedContext() ) {
      myIC->CloseLocalContext(myLocalContextId) ;
      myUseLocalContext = false ;
    }
  }
  mySelection->ClearFilters() ;
  myGeomGUI->SetActiveDialogBox(0) ;
  myGeomGUI->EraseSimulationShape() ;
  myGeomGUI->ResetState() ;
  reject() ;
  return ;
}


//=================================================================================
// function : enterEvent()
// purpose  : to reactivate this dialog box when mouse enter onto the window
//=================================================================================
void GeometryGUI_PointDlg::enterEvent( QEvent* e)
{
  if ( GroupConstructors->isEnabled() )
    return ;
  ActivateThisDialog() ;
}


//=================================================================================
// function : DeactivateActiveDialog()
// purpose  : public slot to deactivate if active
//=================================================================================
void GeometryGUI_PointDlg::DeactivateActiveDialog()
{
  if ( GroupConstructors->isEnabled() ) {
    
    mySelection->ClearFilters() ;
    GroupConstructors->setEnabled(false) ;
    GroupCoordinates->setEnabled(false) ;
    GroupWithEdge->setEnabled(false) ;
    GroupButtons->setEnabled(false) ;
    myGeomGUI->EraseSimulationShape() ;
    myGeomGUI->ResetState() ;    
    myGeomGUI->SetActiveDialogBox(0) ;
    if ( myConstructorId == 0 && myGeomGUI->GetActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC ) {
      myIC->CloseLocalContext(myLocalContextId) ;
      myUseLocalContext = false ;
    }
  }

  return ;
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void GeometryGUI_PointDlg::ActivateThisDialog( )
{
  
  if ( !GroupConstructors->isEnabled() ) { /* if not active */
    
    myGeomGUI->EmitSignalDeactivateDialog() ;
    GroupConstructors->setEnabled(true) ;
    GroupCoordinates->setEnabled(true) ;
    GroupWithEdge->setEnabled(true) ;
    GroupButtons->setEnabled(true) ;
    
    myGeomGUI->SetState(POINT_METHOD) ;
    myGeomGUI->SetActiveDialogBox( (QDialog*)this ) ;
    
    if ( myConstructorId == 0 ) {
      
      mySelection->ClearFilters() ;
      if( myGeomGUI->GetActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC ) {
	OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)myGeomGUI->GetActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
	myIC = v3d->getAISContext();
	myLocalContextId = myIC->OpenLocalContext();
	myGeomGUI->SetDisplayedObjectList();
	/* sub shapes selection */
	myLocalContextMode = TopAbs_VERTEX ;
	myIC->ActivateStandardMode(myLocalContextMode);
	myUseLocalContext = true ;
      }
    }
    
    if ( myConstructorId == 1 )
      mySelection->AddFilter( myEdgeFilter );
    
    if( !mySimulationTopoDs.IsNull() )
      myGeomGUI->DisplaySimulationShape( mySimulationTopoDs ) ;
    
  }
  return ;
}


//=================================================================================
// function : CalculateVertexOnCurve()
// purpose  : Calculate a Vertex on the curve given by 'anEdge'.
//          : The position of resultVertex is given by aParameter.
//          : For a linear edge, aParameter=0.0 gives the first vertex of edge
//          :                    aParameter=1.0 gives the last vertex of edge
//          :                    aParameter=0.5 gives the vertex on the middle of edge
//          : It is possible to get vertices out of edge using values > 1.0 or < 0.0
//=================================================================================
bool GeometryGUI_PointDlg::CalculateVertexOnCurve(const TopoDS_Edge& anEdge, const Standard_Real aParameter, TopoDS_Shape& resultVertex) 
{
#if OCC_VERSION_MAJOR >= 5
  if( anEdge.IsNull() || !BRepAlgo::IsValid(anEdge) )
#else
  if( anEdge.IsNull() || !BRepAlgoAPI::IsValid(anEdge) )
#endif
    return false ;

  Standard_Real first, last ;
  Handle(Geom_Curve) curv = BRep_Tool::Curve(anEdge, first, last);
  if( !curv->IsCN(0) )
    return false ;
  
  Standard_Real param;
  if( anEdge.Orientation() == TopAbs_FORWARD ) {
    param = first + (last-first) * aParameter ;
  }
  else {
    param = last + (first-last) * aParameter ;
  }
  gp_Pnt paramPoint ;
  curv->D0( param, paramPoint ) ;
  resultVertex = BRepBuilderAPI_MakeVertex(paramPoint);
  return true ;
}

