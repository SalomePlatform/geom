using namespace std;
//  File      : GeometryGUI_BoxDlg.cxx
//  Created   :
//  Author    : Lucien PIGNOLONI
//  Project   : SALOME
//  Module    : GeometryGUI
//  Copyright : OPEN CASCADE
//  $Header$
    

#include "GeometryGUI_BoxDlg.h"
#include "GeometryGUI_SpinBox.h"

#include "GeometryGUI.h"
#include "QAD_Application.h"
#include "QAD_Desktop.h"
#include "QAD_Config.h"
#include "utilities.h"

#include <qbuttongroup.h>
#include <qframe.h>
#include <qgroupbox.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qlabel.h>
#include <qvalidator.h>
#include <qevent.h>
#include <qmessagebox.h>



//=================================================================================
// class    : GeometryGUI_BoxDlg()
// purpose  : Constructs a GeometryGUI_BoxDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
GeometryGUI_BoxDlg::GeometryGUI_BoxDlg( QWidget* parent, const char* name, SALOME_Selection* Sel, bool modal, WFlags fl )
    : QDialog( parent, name, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu )
{
    QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap( "GeometryGUI",tr("ICON_DLG_BOX_2P")));
    QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap( "GeometryGUI",tr("ICON_SELECT")));
    QPixmap image2(QAD_Desktop::getResourceManager()->loadPixmap( "GeometryGUI",tr("ICON_DLG_BOX_DXYZ")));

    if ( !name )
	setName( "GeometryGUI_BoxDlg" );
    resize( 335, 220 ); 
    setCaption( tr( "GEOM_BOX_TITLE"  ) );
    setSizeGripEnabled( TRUE );
    GeometryGUI_BoxDlgLayout = new QGridLayout( this ); 
    GeometryGUI_BoxDlgLayout->setSpacing( 6 );
    GeometryGUI_BoxDlgLayout->setMargin( 11 );
    
    /***************************************************************/
    GroupConstructors = new QButtonGroup( this, "GroupConstructors" );
    GroupConstructors->setTitle( tr( "GEOM_BOX"  ) );
    GroupConstructors->setExclusive( TRUE );
    GroupConstructors->setColumnLayout(0, Qt::Vertical );
    GroupConstructors->layout()->setSpacing( 0 );
    GroupConstructors->layout()->setMargin( 0 );
    GroupConstructorsLayout = new QGridLayout( GroupConstructors->layout() );
    GroupConstructorsLayout->setAlignment( Qt::AlignTop );
    GroupConstructorsLayout->setSpacing( 6 );
    GroupConstructorsLayout->setMargin( 11 );
    QSpacerItem* spacer_1 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    GroupConstructorsLayout->addItem( spacer_1, 0, 3 );
    Constructor1 = new QRadioButton( GroupConstructors, "Constructor1" );
    Constructor1->setText( tr( ""  ) );
    Constructor1->setPixmap( image0 );
    Constructor1->setChecked( TRUE );
    Constructor1->setMinimumSize( QSize( 50, 0 ) );
    GroupConstructorsLayout->addWidget( Constructor1, 0, 0 );
    Constructor2 = new QRadioButton( GroupConstructors, "Constructor2" );
    Constructor2->setText( tr( ""  ) );
    Constructor2->setPixmap( image2 );
    Constructor2->setMinimumSize( QSize( 50, 0 ) );
    GroupConstructorsLayout->addWidget( Constructor2, 0, 2 );
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    GroupConstructorsLayout->addItem( spacer_2, 0, 1 );
    GeometryGUI_BoxDlgLayout->addWidget( GroupConstructors, 0, 0 );

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
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    GroupButtonsLayout->addItem( spacer, 0, 2 );
    buttonOk = new QPushButton( GroupButtons, "buttonOk" );
    buttonOk->setText( tr( "GEOM_BUT_OK"  ) );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
    GroupButtonsLayout->addWidget( buttonOk, 0, 0 );
    GeometryGUI_BoxDlgLayout->addWidget( GroupButtons, 2, 0 ); 

    /***************************************************************/
    GroupPoints = new QGroupBox( this, "GroupPoints" );
    GroupPoints->setGeometry( QRect( 10, 10, 280, 90 ) ); 
    GroupPoints->setTitle( tr( "GEOM_DIAGONAL_POINTS"  ) );
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
    GeometryGUI_BoxDlgLayout->addWidget( GroupPoints, 1, 0 );

    /***************************************************************/
    GroupDimensions = new QGroupBox( this, "GroupDimensions" );
    GroupDimensions->setGeometry( QRect( 11, 75, 310, 80 ) ); 
    GroupDimensions->setTitle( tr( "GEOM_BOX_OBJ"  ) );
    GroupDimensions->setColumnLayout(0, Qt::Vertical );
    GroupDimensions->setMinimumSize( QSize( 0, 90 ) );
    GroupDimensions->layout()->setSpacing( 0 );
    GroupDimensions->layout()->setMargin( 0 );
    GroupDimensionsLayout = new QGridLayout( GroupDimensions->layout() );
    GroupDimensionsLayout->setAlignment( Qt::AlignTop );
    GroupDimensionsLayout->setSpacing( 6 );
    GroupDimensionsLayout->setMargin( 11 );

    TextLabel_DZ = new QLabel( GroupDimensions, "TextLabel_DZ" );
    TextLabel_DZ->setText( tr( "GEOM_DZ"  ) );
    GroupDimensionsLayout->addWidget( TextLabel_DZ, 0, 4 );
    TextLabel_DY = new QLabel( GroupDimensions, "TextLabel_DY" );
    TextLabel_DY->setText( tr( "GEOM_DY"  ) );
    GroupDimensionsLayout->addWidget( TextLabel_DY, 0, 2 );
    TextLabel_DX = new QLabel( GroupDimensions, "TextLabel_DX" );
    TextLabel_DX->setText( tr( "GEOM_DX"  ) );
    GroupDimensionsLayout->addWidget( TextLabel_DX, 0, 0 );
 
    /* Spin boxes construction */
    SpinBox_DX = new GeometryGUI_SpinBox( GroupDimensions, "GeomSpinBox_DX" ) ;
    GroupDimensionsLayout->addWidget( SpinBox_DX, 0, 1 );
    SpinBox_DY = new GeometryGUI_SpinBox( GroupDimensions, "GeomSpinBox_DY" ) ;
    GroupDimensionsLayout->addWidget( SpinBox_DY, 0, 3 );
    SpinBox_DZ = new GeometryGUI_SpinBox( GroupDimensions, "GeomSpinBox_DZ" ) ;
    GroupDimensionsLayout->addWidget( SpinBox_DZ, 0, 5 );

    QSpacerItem* spacer1 = new QSpacerItem( 20, 24, QSizePolicy::Minimum, QSizePolicy::Fixed );
    GroupDimensionsLayout->addItem( spacer1, 1, 3 );

    GeometryGUI_BoxDlgLayout->addWidget( GroupDimensions, 1, 0 );

    /* Initialisations */
    Init(Sel) ;
}


//=================================================================================
// function : ~DialogBox_Box()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
GeometryGUI_BoxDlg::~GeometryGUI_BoxDlg()
{
  // no need to delete child widgets, Qt does it all for us
  this->destroy(TRUE, TRUE) ;
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void GeometryGUI_BoxDlg::Init(SALOME_Selection *Sel)
{


  /* Get setting of step value from file configuration */
  double step ;
  QString St = QAD_CONFIG->getSetting( "Geometry:SettingsGeomStep" ) ;
  step = St.toDouble() ;

  /* min, max, step and decimals for spin boxes */
  SpinBox_DX->RangeStepAndValidator( -999.999, 999.999, step, 3 ) ;
  SpinBox_DX->SetValue( 200.0 ) ;  
  SpinBox_DY->RangeStepAndValidator( -999.999, 999.999, step, 3 ) ;
  SpinBox_DY->SetValue( 200.0 ) ;
  SpinBox_DZ->RangeStepAndValidator( -999.999, 999.999, step, 3 ) ;
  SpinBox_DZ->SetValue( 200.0 ) ; 

  GroupPoints->show();
  GroupDimensions->hide() ;
  myConstructorId = 0 ;
  Constructor1->setChecked( TRUE );
  myEditCurrentArgument = LineEditPt1 ;
  mySelection = Sel;
  myGeomGUI = GeometryGUI::GetGeometryGUI() ;
  myPoint1.SetCoord( 0.0, 0.0, 0.0 );
  myPoint2.SetCoord( 0.0, 0.0, 0.0 );
  myOkPoint1 = myOkPoint2 = false ;
  mySimulationTopoDs.Nullify() ;
  
  myGeomGUI->SetActiveDialogBox( (QDialog*)this ) ;
  
  // TODO previous selection into argument ?
  
  /*  Vertices Filter for all arguments */
  Engines::Component_var comp = QAD_Application::getDesktop()->getEngine("FactoryServer", "Geometry");
  myGeom = GEOM::GEOM_Gen::_narrow(comp);
  myVertexFilter = new GEOM_ShapeTypeFilter( TopAbs_VERTEX, myGeom );
  mySelection->AddFilter( myVertexFilter ); /* filter for next selection */
  
  /* signals and slots connections */
  connect( buttonOk, SIGNAL( clicked() ), this, SLOT( ClickOnOk() ) ) ;
  connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( ClickOnCancel() ) ) ;
  connect( buttonApply, SIGNAL( clicked() ),  this, SLOT( ClickOnApply() ) ) ;
  connect( GroupConstructors, SIGNAL(clicked(int) ), this, SLOT( ConstructorsClicked(int) ) ) ;
  connect( SelectButtonPt1, SIGNAL (clicked() ), this, SLOT( SetEditCurrentArgument() ) ) ;
  connect( SelectButtonPt2, SIGNAL (clicked() ), this, SLOT( SetEditCurrentArgument() ) ) ;
 
  connect( SpinBox_DX, SIGNAL ( valueChanged( double) ), this, SLOT( ValueChangedInSpinBox( double) ) ) ;
  connect( SpinBox_DY, SIGNAL ( valueChanged( double) ), this, SLOT( ValueChangedInSpinBox( double) ) ) ;
  connect( SpinBox_DZ, SIGNAL ( valueChanged( double) ), this, SLOT( ValueChangedInSpinBox( double) ) ) ;

  connect( LineEditPt1, SIGNAL ( returnPressed() ), this, SLOT( LineEditReturnPressed() ) ) ;
  connect( LineEditPt2, SIGNAL ( returnPressed() ), this, SLOT( LineEditReturnPressed() ) ) ;
  
  connect( myGeomGUI,   SIGNAL ( SignalDeactivateActiveDialog() ), this, SLOT( DeactivateActiveDialog() ) ) ;      
  connect( mySelection, SIGNAL ( currentSelectionChanged() ),      this, SLOT( SelectionIntoArgument() ) ) ;
  /* To close dialog if study change */
  connect( myGeomGUI, SIGNAL ( SignalCloseAllDialogs() ), this, SLOT( ClickOnCancel() ) ) ;

  /* Move widget on the botton right corner of main widget */
  int x, y ;
  myGeomGUI->DefineDlgPosition( this, x, y ) ;
  this->move( x, y ) ;
  
  this->show() ; /* displays Dialog */
}



//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void GeometryGUI_BoxDlg::ConstructorsClicked(int constructorId)
{

  mySelection->ClearFilters() ;
  myGeomGUI->EraseSimulationShape() ;

  switch (constructorId)
    {
    case 0:
      {
	GroupPoints->show();
	GroupDimensions->hide() ;
	myConstructorId = constructorId ;
	myEditCurrentArgument = LineEditPt1 ;
	Constructor1->setChecked( TRUE );
	LineEditPt1->setText("") ;
	LineEditPt2->setText("") ;
	myOkPoint1 = myOkPoint2 = false ;
	
	/* filter for next selection */
	mySelection->AddFilter( myVertexFilter );
	connect ( mySelection, SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );
	break;
      }
    case 1:
      {
	GroupPoints->hide();
	GroupDimensions->show();
	myConstructorId = constructorId ;
	myOkPoint1 = myOkPoint2 = false ;

	disconnect( mySelection, 0, this, 0 );
	
	double initValue = 200.0 ;
	SpinBox_DX->SetValue( initValue ) ;
	SpinBox_DY->SetValue( initValue ) ;
	SpinBox_DZ->SetValue( initValue ) ;

	myPoint1.SetCoord( 0.0, 0.0, 0.0 ) ;
	myPoint2.SetCoord( initValue, initValue,initValue  ) ;

	mySimulationTopoDs = BRepPrimAPI_MakeBox( myPoint1, myPoint2 ).Shape();
	myGeomGUI->DisplaySimulationShape( mySimulationTopoDs ) ; 
	break;
      }
    }
 return ;
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void GeometryGUI_BoxDlg::ClickOnOk()
{
  this->ClickOnApply() ;
  this->ClickOnCancel() ;
  return ;
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
void GeometryGUI_BoxDlg::ClickOnApply()
{
//NRI+ : 02/12/2202 - BugID 1065
//    if (mySimulationTopoDs.IsNull())
//      return;
  myGeomGUI->EraseSimulationShape() ;
  mySimulationTopoDs.Nullify() ;
  myGeomGUI->GetDesktop()->putInfo( tr("") ) ; 

  switch(myConstructorId)
    { 
    case 0 :
      {
	//NRI+ : 02/12/2202 - BugID 1065	mySelection->ClearFilters() ;
	if(myOkPoint1 && myOkPoint2) 
	  myGeomGUI->MakeBoxAndDisplay( myPoint1, myPoint2 ) ;
	break ;
      }
    case 1 :
      {
	/* Recup args and call method */
	double vx = SpinBox_DX->GetValue() ;
	double vy = SpinBox_DY->GetValue() ;
	double vz = SpinBox_DZ->GetValue() ;
	myPoint1.SetCoord(0.0, 0.0, 0.0) ;
	myPoint2.SetCoord(vx, vy, vz) ;
	myGeomGUI->MakeBoxAndDisplay( myPoint1, myPoint2 ) ;
	break ;
      }
    }
  return ;
}


//=================================================================================
// function : ClickOnCancel()
// purpose  :
//=================================================================================
void GeometryGUI_BoxDlg::ClickOnCancel()
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
// purpose  : Called when selection as changed
//=================================================================================
void GeometryGUI_BoxDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("") ;
  myGeomGUI->EraseSimulationShape() ; 
  mySimulationTopoDs.Nullify() ;

  QString aString = "" ; /* name of selection */

  int nbSel = myGeomGUI->GetNameOfSelectedIObjects(mySelection, aString) ;
  if ( nbSel != 1 ) {
    if ( myEditCurrentArgument == LineEditPt1 ) {
      myOkPoint1 = false ;
    }
    else if ( myEditCurrentArgument == LineEditPt2 ) {
      myOkPoint2 = false ;
    }
    return ;
  }

  // nbSel == 1
  TopoDS_Shape S; 
  if( !myGeomGUI->GetTopoFromSelection(mySelection, S) )
    return ;

  if ( myEditCurrentArgument == LineEditPt1 && myGeomGUI->VertexToPoint(S, myPoint1) ) {
    myEditCurrentArgument->setText( aString ) ;
    myOkPoint1 = true ;
  }
  else if ( myEditCurrentArgument == LineEditPt2 && myGeomGUI->VertexToPoint(S, myPoint2) ) {
    myEditCurrentArgument->setText( aString ) ;
    myOkPoint2 = true ;
  }

  if( myOkPoint1 && myOkPoint2 && TestBoxDimensions( myPoint1, myPoint2 ) ) {    
    mySimulationTopoDs = BRepPrimAPI_MakeBox( myPoint1, myPoint2 ).Shape();
    myGeomGUI->DisplaySimulationShape( mySimulationTopoDs ) ; 
  }
  return ;
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void GeometryGUI_BoxDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();
  switch (myConstructorId)
    {
    case 0: // default constructor
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
    case 1:
      {	
	/* nothing to do here */
	break;
      }
    }
  return ;
}

//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void GeometryGUI_BoxDlg::ValueChangedInSpinBox( double newValue )
{
  if(myConstructorId != 1)
    return ;
  
  myGeomGUI->EraseSimulationShape() ; 
  mySimulationTopoDs.Nullify() ;

  QObject* send = (QObject*)sender() ; 
  double vx, vy, vz ;
  
  if( send == SpinBox_DX ) {
    vx = newValue ;
    vy = SpinBox_DY->GetValue() ;
    vz = SpinBox_DZ->GetValue() ;
  } else if ( send == SpinBox_DY ) {
    vx = SpinBox_DX->GetValue() ;
    vy = newValue ;
    vz = SpinBox_DZ->GetValue() ;
  } else if (send == SpinBox_DZ ) {
    vx = SpinBox_DX->GetValue() ;
    vy = SpinBox_DY->GetValue() ;
    vz = newValue ;
  }
  
  myPoint1.SetCoord(0.0, 0.0, 0.0) ;
  myPoint2.SetCoord(vx, vy, vz) ;
  
  if ( TestBoxDimensions( myPoint1, myPoint2 ) ) {
    mySimulationTopoDs = BRepPrimAPI_MakeBox( myPoint1, myPoint2 ).Shape();
    myGeomGUI->DisplaySimulationShape( mySimulationTopoDs ) ; 
  }
  return ;
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void GeometryGUI_BoxDlg::LineEditReturnPressed()
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
  const QString objectUserName = myEditCurrentArgument->text() ;
  QWidget* thisWidget = (QWidget*)this ;
  if( myGeomGUI->SelectionByNameInDialogs( thisWidget, objectUserName, mySelection ) ) {
    myEditCurrentArgument->setText( objectUserName ) ;
  }
  return ;
}


//=================================================================================
// function : DeactivateActiveDialog()
// purpose  :
//=================================================================================
void GeometryGUI_BoxDlg::DeactivateActiveDialog()
{
  if ( GroupConstructors->isEnabled() ) {

    GroupConstructors->setEnabled(false) ;
    GroupDimensions->setEnabled(false) ;
    GroupButtons->setEnabled(false) ;
    GroupPoints->setEnabled(false) ;

    mySelection->ClearFilters() ;
    disconnect( mySelection, 0, this, 0 );
    myGeomGUI->SetActiveDialogBox(0) ;
    myGeomGUI->EraseSimulationShape() ;
  }
  return ;
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void GeometryGUI_BoxDlg::ActivateThisDialog()
{

  /* Emit a signal to deactivate the active dialog */
  myGeomGUI->EmitSignalDeactivateDialog() ; 

  GroupConstructors->setEnabled(true) ;
  GroupDimensions->setEnabled(true) ;
  GroupButtons->setEnabled(true) ;
  GroupPoints->setEnabled(true) ;  

  connect ( mySelection, SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );
  myGeomGUI->SetActiveDialogBox( (QDialog*)this ) ;
  if( !mySimulationTopoDs.IsNull() )
    myGeomGUI->DisplaySimulationShape( mySimulationTopoDs ) ;
  return ;
}


//=================================================================================
// function : enterEvent [REDEFINED]
// purpose  :
//=================================================================================
void GeometryGUI_BoxDlg::enterEvent(QEvent* e)
{
  if ( GroupConstructors->isEnabled() )
    return ;
  ActivateThisDialog() ;
}


//=================================================================================
// function : closeEvent()
// purpose  :
//=================================================================================
void GeometryGUI_BoxDlg::closeEvent( QCloseEvent* e )
{
  this->ClickOnCancel() ; /* same than click on cancel button */
}


//=================================================================================
// function : TestBoxDimensions()
// purpose  :
//=================================================================================
bool GeometryGUI_BoxDlg::TestBoxDimensions(gp_Pnt P1,  gp_Pnt P2)
{
  if ( ( fabs( P1.X() - P2.X() ) > Precision::Confusion() ) &&
       ( fabs( P1.Y() - P2.Y() ) > Precision::Confusion() ) &&
       ( fabs( P1.Z() - P2.Z() ) > Precision::Confusion() ) )
    return true ;
  return false ;
}
