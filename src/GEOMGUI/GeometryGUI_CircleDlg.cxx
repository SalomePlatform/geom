using namespace std;
//  File      : GeometryGUI_CircleDlg.cxx
//  Created   :
//  Author    : Lucien PIGNOLONI
//  Project   : SALOME
//  Module    : GeometryGUI
//  Copyright : OPEN CASCADE
//  $Header$

#include "GeometryGUI_CircleDlg.h"

#include "GeometryGUI.h"
#include "QAD_Application.h"
#include "QAD_Desktop.h"
#include "QAD_Config.h"
#include "utilities.h"

#include <BRepAdaptor_Curve.hxx>

#include <qbuttongroup.h>
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
#include <qvalidator.h>
#include <qpixmap.h>

//=================================================================================
// class    : GeometryGUI_CircleDlg()
// purpose  : Constructs a GeometryGUI_CircleDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
GeometryGUI_CircleDlg::GeometryGUI_CircleDlg( QWidget* parent,  const char* name, SALOME_Selection* Sel, bool modal, WFlags fl )
    : QDialog( parent, name, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu )
{
    QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap( "GeometryGUI",tr("ICON_DLG_CIRCLE_PV")));
    QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap( "GeometryGUI",tr("ICON_SELECT")));

    if ( !name )
	setName( "GeometryGUI_CircleDlg" );
    resize( 303, 251 ); 
    setCaption( tr( "GEOM_CIRCLE_TITLE"  ) );
    setSizeGripEnabled( TRUE );
    GeometryGUI_CircleDlgLayout = new QGridLayout( this ); 
    GeometryGUI_CircleDlgLayout->setSpacing( 6 );
    GeometryGUI_CircleDlgLayout->setMargin( 11 );

    /***************************************************************/
    GroupConstructors = new QButtonGroup( this, "GroupConstructors" );
    GroupConstructors->setTitle( tr( "GEOM_CIRCLE"  ) );
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
    Constructor1->setChecked( TRUE );
    Constructor1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, Constructor1->sizePolicy().hasHeightForWidth() ) );
    Constructor1->setMinimumSize( QSize( 50, 0 ) );
    GroupConstructorsLayout->addWidget( Constructor1, 0, 0 );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    GroupConstructorsLayout->addItem( spacer, 0, 1 );
    GeometryGUI_CircleDlgLayout->addWidget( GroupConstructors, 0, 0 );

    /***************************************************************/
    GroupC1 = new QGroupBox( this, "GroupC1" );
    GroupC1->setTitle( tr( "GEOM_ARGUMENTS"  ) );
    GroupC1->setFrameShape( QGroupBox::Box );
    GroupC1->setFrameShadow( QGroupBox::Sunken );
    GroupC1->setColumnLayout(0, Qt::Vertical );
    GroupC1->layout()->setSpacing( 0 );
    GroupC1->layout()->setMargin( 0 );
    GroupC1Layout = new QGridLayout( GroupC1->layout() );
    GroupC1Layout->setAlignment( Qt::AlignTop );
    GroupC1Layout->setSpacing( 6 );
    GroupC1Layout->setMargin( 11 );

    TextLabelC1A1 = new QLabel( GroupC1, "TextLabelC1A1" );
    TextLabelC1A1->setText( tr( "GEOM_CENTER_POINT"  ) );
    TextLabelC1A1->setMinimumSize( QSize( 50, 0 ) );
    TextLabelC1A1->setFrameShape( QLabel::NoFrame );
    TextLabelC1A1->setFrameShadow( QLabel::Plain );
    GroupC1Layout->addWidget( TextLabelC1A1, 0, 0 );
    SelectButtonC1A1 = new QPushButton( GroupC1, "SelectButtonC1A1" );
    SelectButtonC1A1->setText( tr( ""  ) );
    SelectButtonC1A1->setPixmap( image1 );
    SelectButtonC1A1->setToggleButton( FALSE );
    GroupC1Layout->addWidget( SelectButtonC1A1, 0, 1 );
    LineEditC1A1 = new QLineEdit( GroupC1, "LineEditC1A1" );
    GroupC1Layout->addWidget( LineEditC1A1, 0, 2 );
    TextLabelC1A2 = new QLabel( GroupC1, "TextLabelC1A2" );
    TextLabelC1A2->setText( tr( "GEOM_VECTOR"  ) );
    TextLabelC1A2->setMinimumSize( QSize( 50, 0 ) );
    GroupC1Layout->addWidget( TextLabelC1A2, 1, 0 );
    SelectButtonC1A2 = new QPushButton( GroupC1, "SelectButtonC1A2" );
    SelectButtonC1A2->setText( tr( ""  ) );
    SelectButtonC1A2->setPixmap( image1 );
    GroupC1Layout->addWidget( SelectButtonC1A2, 1, 1 );
    LineEditC1A2 = new QLineEdit( GroupC1, "LineEditC1A2" );
    GroupC1Layout->addWidget( LineEditC1A2, 1, 2 );
    TextLabelC1A3 = new QLabel( GroupC1, "TextLabelC1A3" );
    TextLabelC1A3->setText( tr( "GEOM_RADIUS"  ) );
    TextLabelC1A3->setMinimumSize( QSize( 50, 0 ) );
    GroupC1Layout->addWidget( TextLabelC1A3, 2, 0 );
    SpinBox_C1A3 = new GeometryGUI_SpinBox( GroupC1, "GeomSpinBox_C1A3" ) ;
    GroupC1Layout->addWidget( SpinBox_C1A3, 2, 2 );
    GeometryGUI_CircleDlgLayout->addWidget( GroupC1, 1, 0 );
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
    buttonOk = new QPushButton( GroupButtons, "buttonOk" );
    buttonOk->setText( tr( "GEOM_BUT_OK"  ) );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
    GroupButtonsLayout->addWidget( buttonOk, 0, 0 );
    buttonApply = new QPushButton( GroupButtons, "buttonApply" );
    buttonApply->setText( tr( "GEOM_BUT_APPLY"  ) );
    buttonApply->setAutoDefault( TRUE );
    GroupButtonsLayout->addWidget( buttonApply, 0, 1 );
    QSpacerItem* spacer_9 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    GroupButtonsLayout->addItem( spacer_9, 0, 2 );
    buttonCancel = new QPushButton( GroupButtons, "buttonCancel" );
    buttonCancel->setText( tr( "GEOM_BUT_CLOSE"  ) );
    buttonCancel->setAutoDefault( TRUE );
    GroupButtonsLayout->addWidget( buttonCancel, 0, 3 );
    GeometryGUI_CircleDlgLayout->addWidget( GroupButtons, 2, 0 );
    /***************************************************************/

    Init(Sel) ; /* Initialisations */
}


//=================================================================================
// function : ~GeometryGUI_CircleDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
GeometryGUI_CircleDlg::~GeometryGUI_CircleDlg()
{
    // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void GeometryGUI_CircleDlg::Init( SALOME_Selection* Sel )
{
  double step ;
  QString St = QAD_CONFIG->getSetting( "Geometry:SettingsGeomStep" ) ;
  step = St.toDouble() ;

  /* min, max, step and decimals for spin boxes */
  SpinBox_C1A3->RangeStepAndValidator( 0.001, 999.999, step, 3 ) ;
  SpinBox_C1A3->SetValue( 100.0 ) ;   /* = myRadius */

  GroupC1->show();
  myConstructorId = 0 ;
  Constructor1->setChecked( TRUE );
  myEditCurrentArgument = LineEditC1A1 ;	
  mySelection = Sel;
  myGeomGUI = GeometryGUI::GetGeometryGUI() ;
  myRadius = 100.0 ;
  myOkPoint1 = myOkDir = false ;
  mySimulationTopoDs.Nullify() ;
  myGeomGUI->SetActiveDialogBox( (QDialog*)this ) ;

  // TODO : previous selection into argument ?

  /* Filter definitions */
  Engines::Component_var comp = QAD_Application::getDesktop()->getEngine("FactoryServer", "Geometry");
  myGeom = GEOM::GEOM_Gen::_narrow(comp);  
  myEdgeFilter   = new GEOM_EdgeFilter( StdSelect_Line, myGeom );
  myVertexFilter = new GEOM_ShapeTypeFilter( TopAbs_VERTEX, myGeom );

  mySelection->AddFilter(myVertexFilter) ; /* first filter used */

  /* signals and slots connections */
  connect( buttonOk, SIGNAL( clicked() ),     this, SLOT( ClickOnOk() ) );
  connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( ClickOnCancel() ) ) ;
  connect( buttonApply, SIGNAL( clicked() ),     this, SLOT( ClickOnApply() ) );
  connect( GroupConstructors, SIGNAL(clicked(int) ), SLOT( ConstructorsClicked(int) ) );  
  connect( SelectButtonC1A1, SIGNAL (clicked() ), this, SLOT( SetEditCurrentArgument() ) ) ;
  connect( SelectButtonC1A2, SIGNAL (clicked() ), this, SLOT( SetEditCurrentArgument() ) ) ;

  connect( LineEditC1A1, SIGNAL ( returnPressed() ), this, SLOT( LineEditReturnPressed() ) ) ;
  connect( LineEditC1A2, SIGNAL ( returnPressed() ), this, SLOT( LineEditReturnPressed() ) ) ;
  
  connect( SpinBox_C1A3, SIGNAL ( valueChanged( double) ), this, SLOT( ValueChangedInSpinBox( double) ) ) ;
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
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void GeometryGUI_CircleDlg::ConstructorsClicked(int constructorId)
{
  /* only a constructor now */
  return ;
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void GeometryGUI_CircleDlg::ClickOnOk()
{
  this->ClickOnApply() ;
  this->ClickOnCancel() ;

  return ;
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
void GeometryGUI_CircleDlg::ClickOnApply()
{
  myGeomGUI->EraseSimulationShape() ;
  mySimulationTopoDs.Nullify() ;
  switch(myConstructorId)
    { 
    case 0 :
      { 
	if(myOkPoint1 && myOkDir) {	  
	  myGeomGUI->MakeCircleAndDisplay( myPoint1, myDir, myRadius) ; 
	}
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
void GeometryGUI_CircleDlg::ClickOnCancel()
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
void GeometryGUI_CircleDlg::SelectionIntoArgument()
{

  myGeomGUI->EraseSimulationShape() ; 
  mySimulationTopoDs.Nullify() ;

  /* Future name of selection */
  QString aString = "";
  
  int nbSel = myGeomGUI->GetNameOfSelectedIObjects(mySelection, aString) ;
  if ( nbSel != 1 ) {
    if ( myEditCurrentArgument == LineEditC1A1 ) {
      LineEditC1A1->setText("") ;
      myOkPoint1 = false ;
    }
    else if ( myEditCurrentArgument == LineEditC1A2 ) { 
      LineEditC1A2->setText("") ;
      myOkDir = false ;
    }
    return ;
  }

  /* nbSel == 1 */
  TopoDS_Shape S; 
  if( !myGeomGUI->GetTopoFromSelection(mySelection, S) )
    return ;
 
  /*  gp_Pnt : not used */
  if ( myEditCurrentArgument == LineEditC1A1 && myGeomGUI->VertexToPoint(S, myPoint1) ) {
    LineEditC1A1->setText(aString) ;
    myOkPoint1 = true ;
  }    
  else if ( myEditCurrentArgument == LineEditC1A2 /*&& myGeomGUI->LinearLocationAndDirection(S, notUsed, myDir) */) {
    BRepAdaptor_Curve curv(TopoDS::Edge(S));
    myDir = curv.Line().Direction();    
    LineEditC1A2->setText(aString) ;
    myOkDir = true ;
  }

  if( myOkPoint1 && myOkDir ) {
    MakeCircleSimulationAndDisplay() ;
  }
  return ;
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void GeometryGUI_CircleDlg::SetEditCurrentArgument()
{
  mySelection->ClearFilters() ;
  QPushButton* send = (QPushButton*)sender();
  switch (myConstructorId)
    {
    case 0: /* default constructor */
      {	
	if(send == SelectButtonC1A1) {
	  LineEditC1A1->setFocus() ;
	  myEditCurrentArgument = LineEditC1A1;
	  mySelection->AddFilter(myVertexFilter) ;
	}
	else if(send == SelectButtonC1A2) {
	  LineEditC1A2->setFocus() ;	  
	  myEditCurrentArgument = LineEditC1A2;
	  mySelection->AddFilter(myEdgeFilter) ;
	}	
	SelectionIntoArgument() ;
	break;
      }
    }
  return ;
}

//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void GeometryGUI_CircleDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();  
  if( send == LineEditC1A1 )
    myEditCurrentArgument = LineEditC1A1 ;
  else if ( send == LineEditC1A2 )
    myEditCurrentArgument = LineEditC1A2 ; 
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
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void GeometryGUI_CircleDlg::ValueChangedInSpinBox( double newValue )
{  
  myRadius = newValue ;
  
  if (myOkPoint1 && myOkDir) {
    MakeCircleSimulationAndDisplay() ;
  }
  else {
    myGeomGUI->EraseSimulationShape() ; 
    mySimulationTopoDs.Nullify() ;
  }
  return ;
}

//=================================================================================
// function : DeactivateActiveDialog()
// purpose  :
//=================================================================================
void GeometryGUI_CircleDlg::DeactivateActiveDialog()
{
  if ( GroupConstructors->isEnabled() ) {
    GroupConstructors->setEnabled(false) ;
    GroupC1->setEnabled(false) ;
    GroupButtons->setEnabled(false) ;
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
void GeometryGUI_CircleDlg::ActivateThisDialog()
{
  /* Emit a signal to deactivate the active dialog */
  myGeomGUI->EmitSignalDeactivateDialog() ;   
  GroupConstructors->setEnabled(true) ;
  GroupC1->setEnabled(true) ;
  GroupButtons->setEnabled(true) ;
  connect ( mySelection, SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );
  if( !mySimulationTopoDs.IsNull() )
    myGeomGUI->DisplaySimulationShape( mySimulationTopoDs ) ;
  return ;
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void GeometryGUI_CircleDlg::enterEvent(QEvent* e)
{
  if ( GroupConstructors->isEnabled() )
    return ;  
  ActivateThisDialog() ;
}


//=================================================================================
// function : closeEvent()
// purpose  :
//=================================================================================
void GeometryGUI_CircleDlg::closeEvent( QCloseEvent* e )
{
  this->ClickOnCancel() ; /* same than click on cancel button */
}


//=================================================================================
// function : MakeCircleSimulationAndDisplay()
// purpose  :
//=================================================================================
void GeometryGUI_CircleDlg::MakeCircleSimulationAndDisplay() 
{
  myGeomGUI->EraseSimulationShape() ;
  
  try {
    gp_Ax2 axis( this->myPoint1, this->myDir ) ;
    gp_Circ circ( axis, this->myRadius);
    BRepBuilderAPI_MakeEdge MakeEdge( circ );
    mySimulationTopoDs = MakeEdge.Shape();
    myGeomGUI->DisplaySimulationShape( mySimulationTopoDs ) ;
  }
  catch(Standard_Failure) {
    MESSAGE( "Exception catched in MakeCircleSimulationAndDisplay" ) ;
  }
  return ;
}
