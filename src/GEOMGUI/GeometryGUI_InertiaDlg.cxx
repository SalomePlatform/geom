using namespace std;
//  File      : GeometryGUI_InertiaDlg.cxx
//  Created   :
//  Author    : Lucien PIGNOLONI
//  Project   : SALOME
//  Module    : GeometryGUI
//  Copyright : OPEN CASCADE
//  $Headers:

#include "GeometryGUI_InertiaDlg.h"

#include "GeometryGUI.h"

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
#include <qpixmap.h>

//=================================================================================
// class    : GeometryGUI_InertiaDlg()
// purpose  : Constructs a GeometryGUI_InertiaDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
GeometryGUI_InertiaDlg::GeometryGUI_InertiaDlg( QWidget* parent, const char* name, SALOME_Selection* Sel, bool modal, WFlags fl )
    : QDialog( parent, name, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu )
{
    QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap( "GEOM",tr("ICON_DLG_INERTIA")));
    QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap( "GEOM",tr("ICON_SELECT")));
    
    if ( !name )
	setName( "GeometryGUI_InertiaDlg" );
    resize( 356, 303 ); 
    setCaption( tr( "GEOM_INERTIA_TITLE"  ) );
    setSizeGripEnabled( TRUE );
    GeometryGUI_InertiaDlgLayout = new QGridLayout( this ); 
    GeometryGUI_InertiaDlgLayout->setSpacing( 6 );
    GeometryGUI_InertiaDlgLayout->setMargin( 11 );

    /***************************************************************/
    GroupConstructors = new QButtonGroup( this, "GroupConstructors" );
    GroupConstructors->setTitle( tr( "GEOM_INERTIA_CONSTR"  ) );
    GroupConstructors->setExclusive( TRUE );
    GroupConstructors->setColumnLayout(0, Qt::Vertical );
    GroupConstructors->layout()->setSpacing( 0 );
    GroupConstructors->layout()->setMargin( 0 );
    GroupConstructorsLayout = new QGridLayout( GroupConstructors->layout() );
    GroupConstructorsLayout->setAlignment( Qt::AlignTop );
    GroupConstructorsLayout->setSpacing( 6 );
    GroupConstructorsLayout->setMargin( 11 );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    GroupConstructorsLayout->addItem( spacer, 0, 1 );
    Constructor1 = new QRadioButton( GroupConstructors, "Constructor1" );
    Constructor1->setText( tr( ""  ) );
    Constructor1->setPixmap( image0 );
    Constructor1->setChecked( TRUE );
    Constructor1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, Constructor1->sizePolicy().hasHeightForWidth() ) );
    GroupConstructorsLayout->addWidget( Constructor1, 0, 0 );
    GeometryGUI_InertiaDlgLayout->addWidget( GroupConstructors, 0, 0 );

    /***************************************************************/
    GroupC1 = new QGroupBox( this, "GroupC1" );
    GroupC1->setTitle( tr( "GEOM_OBJECT_RESULT"  ) );
    GroupC1->setColumnLayout(0, Qt::Vertical );
    GroupC1->layout()->setSpacing( 0 );
    GroupC1->layout()->setMargin( 0 );
    GroupC1Layout = new QGridLayout( GroupC1->layout() );
    GroupC1Layout->setAlignment( Qt::AlignTop );
    GroupC1Layout->setSpacing( 6 );
    GroupC1Layout->setMargin( 11 );
    TextLabelC1A1 = new QLabel( GroupC1, "TextLabelC1A1" );
    TextLabelC1A1->setText( tr( "GEOM_OBJECT"  ) );
    TextLabelC1A1->setMinimumSize( QSize( 50, 0 ) );
    TextLabelC1A1->setFrameShape( QLabel::NoFrame );
    TextLabelC1A1->setFrameShadow( QLabel::Plain );
    GroupC1Layout->addWidget( TextLabelC1A1, 0, 0 );
    SelectButtonC1A1 = new QPushButton( GroupC1, "SelectButtonC1A1" );
    SelectButtonC1A1->setText( tr( ""  ) );
    SelectButtonC1A1->setPixmap( image1 );
    GroupC1Layout->addWidget( SelectButtonC1A1, 0, 1 );
    LineEditC1A1 = new QLineEdit( GroupC1, "LineEditC1A1" );
    LineEditC1A1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, LineEditC1A1->sizePolicy().hasHeightForWidth() ) );
    LineEditC1A1->setMinimumSize( QSize( 220, 0 ) );
    GroupC1Layout->addMultiCellWidget( LineEditC1A1, 0, 0, 2, 4 );
    TextLabel_Matrix = new QLabel( GroupC1, "TextLabel_Matrix" );
    TextLabel_Matrix->setText( tr( "GEOM_MATRIX"  ) );
    TextLabel_Matrix->setMinimumSize( QSize( 50, 0 ) );
    TextLabel_Matrix->setFrameShape( QLabel::NoFrame );
    TextLabel_Matrix->setFrameShadow( QLabel::Plain );
    GroupC1Layout->addWidget( TextLabel_Matrix, 1, 0 );
    TextLabelMatrix_11 = new QLabel( GroupC1, "TextLabelMatrix_11" );
    TextLabelMatrix_11->setText( tr( "GEOM_INERTIA_I"  ).arg("1") );
    TextLabelMatrix_11->setMinimumSize( QSize( 0, 0 ) );
    TextLabelMatrix_11->setFrameShape( QLabel::NoFrame );
    TextLabelMatrix_11->setFrameShadow( QLabel::Plain );
    TextLabelMatrix_11->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );
    GroupC1Layout->addWidget( TextLabelMatrix_11, 1, 1 );
    TextLabelMatrix_21 = new QLabel( GroupC1, "TextLabelMatrix_21" );
    TextLabelMatrix_21->setText( tr( "GEOM_INERTIA_I"  ).arg("2") );
    TextLabelMatrix_21->setMinimumSize( QSize( 0, 0 ) );
    TextLabelMatrix_21->setFrameShape( QLabel::NoFrame );
    TextLabelMatrix_21->setFrameShadow( QLabel::Plain );
    TextLabelMatrix_21->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );
    GroupC1Layout->addWidget( TextLabelMatrix_21, 2, 1 );
    TextLabelMatrix_31 = new QLabel( GroupC1, "TextLabelMatrix_31" );
    TextLabelMatrix_31->setText( tr( "GEOM_INERTIA_I"  ).arg("3") );
    TextLabelMatrix_31->setMinimumSize( QSize( 0, 0 ) );
    TextLabelMatrix_31->setFrameShape( QLabel::NoFrame );
    TextLabelMatrix_31->setFrameShadow( QLabel::Plain );
    TextLabelMatrix_31->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );
    GroupC1Layout->addWidget( TextLabelMatrix_31, 3, 1 );
    LineEdit_L1C1 = new QLineEdit( GroupC1, "LineEdit_L1C1" );
    LineEdit_L1C1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, LineEdit_L1C1->sizePolicy().hasHeightForWidth() ) );
    LineEdit_L1C1->setMinimumSize( QSize( 70, 0 ) );
    //LineEdit_L1C1->setEnabled( FALSE );
    LineEdit_L1C1->setReadOnly( TRUE );
    LineEdit_L1C1->setText( tr( ""  ) );
    GroupC1Layout->addWidget( LineEdit_L1C1, 1, 2 );
    LineEdit_L1C2 = new QLineEdit( GroupC1, "LineEdit_L1C2" );
    LineEdit_L1C2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, LineEdit_L1C2->sizePolicy().hasHeightForWidth() ) );
    LineEdit_L1C2->setMinimumSize( QSize( 70, 0 ) );
    //LineEdit_L1C2->setEnabled( FALSE );
    LineEdit_L1C2->setReadOnly( TRUE );
    GroupC1Layout->addWidget( LineEdit_L1C2, 1, 3 );
    LineEdit_L1C3 = new QLineEdit( GroupC1, "LineEdit_L1C3" );
    LineEdit_L1C3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, LineEdit_L1C3->sizePolicy().hasHeightForWidth() ) );
    LineEdit_L1C3->setMinimumSize( QSize( 70, 0 ) );
    //LineEdit_L1C3->setEnabled( FALSE );
    LineEdit_L1C3->setReadOnly( TRUE );
    GroupC1Layout->addWidget( LineEdit_L1C3, 1, 4 );
    LineEdit_L2C1 = new QLineEdit( GroupC1, "LineEdit_L2C1" );
    LineEdit_L2C1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, LineEdit_L2C1->sizePolicy().hasHeightForWidth() ) );
    LineEdit_L2C1->setMinimumSize( QSize( 70, 0 ) );
    //LineEdit_L2C1->setEnabled( FALSE );
    LineEdit_L2C1->setReadOnly( TRUE );
    GroupC1Layout->addWidget( LineEdit_L2C1, 2, 2 );
    LineEdit_L2C2 = new QLineEdit( GroupC1, "LineEdit_L2C2" );
    LineEdit_L2C2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, LineEdit_L2C2->sizePolicy().hasHeightForWidth() ) );
    LineEdit_L2C2->setMinimumSize( QSize( 70, 0 ) );
    //LineEdit_L2C2->setEnabled( FALSE );
    LineEdit_L2C2->setReadOnly( TRUE );
    GroupC1Layout->addWidget( LineEdit_L2C2, 2, 3 );
    LineEdit_L2C3 = new QLineEdit( GroupC1, "LineEdit_L2C3" );
    LineEdit_L2C3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, LineEdit_L2C3->sizePolicy().hasHeightForWidth() ) );
    LineEdit_L2C3->setMinimumSize( QSize( 70, 0 ) );
    //LineEdit_L2C3->setEnabled( FALSE );
    LineEdit_L2C3->setReadOnly( TRUE );
    GroupC1Layout->addWidget( LineEdit_L2C3, 2, 4 );
    LineEdit_L3C1 = new QLineEdit( GroupC1, "LineEdit_L3C1" );
    LineEdit_L3C1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, LineEdit_L3C1->sizePolicy().hasHeightForWidth() ) );
    LineEdit_L3C1->setMinimumSize( QSize( 70, 0 ) );
    //LineEdit_L3C1->setEnabled( FALSE );
    LineEdit_L3C1->setReadOnly( TRUE );
    GroupC1Layout->addWidget( LineEdit_L3C1, 3, 2 );
    LineEdit_L3C2 = new QLineEdit( GroupC1, "LineEdit_L3C2" );
    LineEdit_L3C2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, LineEdit_L3C2->sizePolicy().hasHeightForWidth() ) );
    LineEdit_L3C2->setMinimumSize( QSize( 70, 0 ) );
    //LineEdit_L3C2->setEnabled( FALSE );
    LineEdit_L3C2->setReadOnly( TRUE );
    GroupC1Layout->addWidget( LineEdit_L3C2, 3, 3 );
    LineEdit_L3C3 = new QLineEdit( GroupC1, "LineEdit_L3C3" );
    LineEdit_L3C3->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, LineEdit_L3C3->sizePolicy().hasHeightForWidth() ) );
    LineEdit_L3C3->setMinimumSize( QSize( 70, 0 ) );
    //LineEdit_L3C3->setEnabled( FALSE );
    LineEdit_L3C3->setReadOnly( TRUE );
    GroupC1Layout->addWidget( LineEdit_L3C3, 3, 4 );
    LineEdit_IX = new QLineEdit( GroupC1, "LineEdit_IX" );
    LineEdit_IX->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, LineEdit_IX->sizePolicy().hasHeightForWidth() ) );
    LineEdit_IX->setMinimumSize( QSize( 70, 0 ) );
    //LineEdit_IX->setEnabled( FALSE );
    LineEdit_IX->setReadOnly( TRUE );
    GroupC1Layout->addWidget( LineEdit_IX, 4, 2 );
    LineEdit_IY = new QLineEdit( GroupC1, "LineEdit_IY" );
    LineEdit_IY->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, LineEdit_IY->sizePolicy().hasHeightForWidth() ) );
    LineEdit_IY->setMinimumSize( QSize( 70, 0 ) );
    //LineEdit_IY->setEnabled( FALSE );
    LineEdit_IY->setReadOnly( TRUE );
    GroupC1Layout->addWidget( LineEdit_IY, 4, 3 );
    LineEdit_IZ = new QLineEdit( GroupC1, "LineEdit_IZ" );
    LineEdit_IZ->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, LineEdit_IZ->sizePolicy().hasHeightForWidth() ) );
    LineEdit_IZ->setMinimumSize( QSize( 70, 0 ) );
    //LineEdit_IZ->setEnabled( FALSE );
    LineEdit_IZ->setReadOnly( TRUE );
    GroupC1Layout->addWidget( LineEdit_IZ, 4, 4 );
    TextLabel_IXIYIZ = new QLabel( GroupC1, "TextLabel_IXIYIZ" );
    TextLabel_IXIYIZ->setText( tr( "GEOM_INERTIA_IXYZ"  ) );
    TextLabel_IXIYIZ->setMinimumSize( QSize( 50, 0 ) );
    TextLabel_IXIYIZ->setFrameShape( QLabel::NoFrame );
    TextLabel_IXIYIZ->setFrameShadow( QLabel::Plain );
    TextLabel_IXIYIZ->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );
    GroupC1Layout->addMultiCellWidget( TextLabel_IXIYIZ, 4, 4, 0, 1 );
    GeometryGUI_InertiaDlgLayout->addWidget( GroupC1, 1, 0 );

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
    GroupButtonsLayout->addWidget( buttonCancel, 0, 1 );

    QSpacerItem* spacer_8 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    GroupButtonsLayout->addItem( spacer_8, 0, 0 );
    QSpacerItem* spacer_9 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    GroupButtonsLayout->addItem( spacer_9, 0, 2 );

    GeometryGUI_InertiaDlgLayout->addWidget( GroupButtons, 2, 0 );
    /***************************************************************/

    Init(Sel) ; /* Initialisations */
}


//=================================================================================
// function : ~GeometryGUI_InertiaDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
GeometryGUI_InertiaDlg::~GeometryGUI_InertiaDlg()
{
    // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void GeometryGUI_InertiaDlg::Init( SALOME_Selection* Sel )
{

  LineEdit_L1C1->setMaxLength( 10 );
  LineEdit_L1C2->setMaxLength( 10 );
  LineEdit_L1C3->setMaxLength( 10 );
  LineEdit_L2C1->setMaxLength( 10 );
  LineEdit_L2C2->setMaxLength( 10 );
  LineEdit_L2C3->setMaxLength( 10 );
  LineEdit_L3C1->setMaxLength( 10 );
  LineEdit_L3C2->setMaxLength( 10 );
  LineEdit_L3C3->setMaxLength( 10 );

  LineEdit_IX->setMaxLength( 10 );
  LineEdit_IY->setMaxLength( 10 );
  LineEdit_IZ->setMaxLength( 10 );

  myConstructorId = 0 ;
  Constructor1->setChecked( TRUE );
  myEditCurrentArgument = LineEditC1A1 ;	
  mySelection = Sel;
  myGeomGUI = GeometryGUI::GetGeometryGUI() ;
  myGeomGUI->SetActiveDialogBox( (QDialog*)this ) ;
 
  // TODO : previous selection into argument ?

  /* Filter definitions */
  Engines::Component_var comp = QAD_Application::getDesktop()->getEngine("FactoryServer", "GEOM");
  myGeom = GEOM::GEOM_Gen::_narrow(comp);

  /* signals and slots connections */
  connect( buttonCancel,      SIGNAL( clicked() ),   this, SLOT( ClickOnCancel() ) ) ;
  connect( GroupConstructors, SIGNAL(clicked(int) ), this, SLOT( ConstructorsClicked(int) ) );
  connect( SelectButtonC1A1,  SIGNAL (clicked() ),   this, SLOT( SetEditCurrentArgument() ) ) ;

  connect( LineEditC1A1, SIGNAL ( returnPressed() ), this, SLOT( LineEditReturnPressed() ) ) ;

  connect( mySelection,  SIGNAL( currentSelectionChanged() ),     this, SLOT( SelectionIntoArgument() ) );
  connect( myGeomGUI,    SIGNAL ( SignalDeactivateActiveDialog() ), this, SLOT( DeactivateActiveDialog() ) ) ;

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
void GeometryGUI_InertiaDlg::ConstructorsClicked(int constructorId)
{
  return ;
}



//=================================================================================
// function : ClickOnCancel()
// purpose  :
//=================================================================================
void GeometryGUI_InertiaDlg::ClickOnCancel()
{
  disconnect( mySelection, 0, this, 0 );
  myGeomGUI->ResetState() ;
  reject() ;
  return ;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void GeometryGUI_InertiaDlg::SelectionIntoArgument()
{
  LineEdit_L1C1->setText("") ;
  LineEdit_L1C2->setText("") ;
  LineEdit_L1C3->setText("") ;
  LineEdit_L2C1->setText("") ;
  LineEdit_L2C2->setText("") ;
  LineEdit_L2C3->setText("") ;
  LineEdit_L3C1->setText("") ;
  LineEdit_L3C2->setText("") ;
  LineEdit_L3C3->setText("") ;

  LineEdit_IX->setText("") ;
  LineEdit_IY->setText("") ;
  LineEdit_IZ->setText("") ;

  myEditCurrentArgument->setText("") ;
  QString aString = ""; /* future the name of selection */

  int nbSel = myGeomGUI->GetNameOfSelectedIObjects(mySelection, aString) ;
  if ( nbSel != 1 ) {
    return ;
  }
  
  /*  nbSel == 1  */ 
  TopoDS_Shape S;
  if( !myGeomGUI->GetTopoFromSelection(mySelection, S) || S.IsNull() )
    return ;
    
  LineEditC1A1->setText(aString) ;
  this->CalculateAndDisplayInertia(S) ;  
  return ;
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void GeometryGUI_InertiaDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();
  switch (myConstructorId)
    {
    case 0: /* default constructor */
      {	
	if(send == SelectButtonC1A1) {
	  LineEditC1A1->setFocus() ;
	  myEditCurrentArgument = LineEditC1A1;
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
void GeometryGUI_InertiaDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();  
  if( send == LineEditC1A1 )
    myEditCurrentArgument = LineEditC1A1 ;
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
  return ;
}


//=================================================================================
// function : DeactivateActiveDialog()
// purpose  :
//=================================================================================
void GeometryGUI_InertiaDlg::DeactivateActiveDialog()
{
  if ( GroupConstructors->isEnabled() ) {    
    disconnect( mySelection, 0, this, 0 );
    GroupConstructors->setEnabled(false) ;
    GroupC1->setEnabled(false) ;
    GroupButtons->setEnabled(false) ;
  }
  return ;
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void GeometryGUI_InertiaDlg::ActivateThisDialog()
{
  /* Emit a signal to deactivate the active dialog */
  myGeomGUI->EmitSignalDeactivateDialog() ;   
  GroupConstructors->setEnabled(true) ;
  GroupC1->setEnabled(true) ;
  GroupButtons->setEnabled(true) ;
  connect( mySelection, SIGNAL( currentSelectionChanged() ),     this, SLOT( SelectionIntoArgument() ) );
  return ;
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void GeometryGUI_InertiaDlg::enterEvent(QEvent* e)
{
  if ( GroupConstructors->isEnabled() )
    return ;  
  ActivateThisDialog() ;
  return ;
}


//=================================================================================
// function : closeEvent()
// purpose  :
//=================================================================================
void GeometryGUI_InertiaDlg::closeEvent( QCloseEvent* e )
{
  /* same than click on cancel button */
  this->ClickOnCancel() ;
  return ;
}


//=================================================================================
// function : CalculateAndDisplayInertia()
// purpose  :
//=================================================================================
void GeometryGUI_InertiaDlg::CalculateAndDisplayInertia(const TopoDS_Shape& S)
{
  LineEdit_L1C1->setText("") ;
  LineEdit_L1C2->setText("") ;
  LineEdit_L1C3->setText("") ;
  LineEdit_L2C1->setText("") ;
  LineEdit_L2C2->setText("") ;
  LineEdit_L2C3->setText("") ;
  LineEdit_L3C1->setText("") ;
  LineEdit_L3C2->setText("") ;
  LineEdit_L3C3->setText("") ;

  LineEdit_IX->setText("") ;
  LineEdit_IY->setText("") ;
  LineEdit_IZ->setText("") ;


  if( S.IsNull() ) 
    return ;

  try {

    QString resString;
    GProp_GProps System;

    if ( S.ShapeType() == TopAbs_VERTEX || S.ShapeType() == TopAbs_EDGE || S.ShapeType() == TopAbs_WIRE ) {
      BRepGProp::LinearProperties(S, System);
    }
    else if ( S.ShapeType() == TopAbs_FACE || S.ShapeType() == TopAbs_SHELL ) {
      BRepGProp::SurfaceProperties(S, System);
    }
    else {
      BRepGProp::VolumeProperties(S, System);
    }

    gp_Mat I = System.MatrixOfInertia() ;
    GProp_PrincipalProps Pr = System.PrincipalProperties();
    Standard_Real Ix,Iy,Iz;
    Pr.Moments(Ix,Iy,Iz);
    
    /* matrix 3x3 */
    resString = tr("%1").arg( I(1,1), 12, 'f', 6 ) ;
    LineEdit_L1C1->setText(resString) ;
    resString = tr("%1").arg( I(1,2), 12, 'f', 6 ) ;
    LineEdit_L1C2->setText(resString) ;
    resString = tr("%1").arg( I(1,3), 12, 'f', 6 ) ;
    LineEdit_L1C3->setText(resString) ;

    resString = tr("%1").arg( I(2,1), 12, 'f', 6 ) ;
    LineEdit_L2C1->setText(resString) ;
    resString = tr("%1").arg( I(2,2), 12, 'f', 6 ) ;
    LineEdit_L2C2->setText(resString) ;
    resString = tr("%1").arg( I(2,3), 12, 'f', 6 ) ;
    LineEdit_L2C3->setText(resString) ;

    resString = tr("%1").arg( I(3,1), 12, 'f', 6 ) ;
    LineEdit_L3C1->setText(resString) ;
    resString = tr("%1").arg( I(3,2), 12, 'f', 6 ) ;
    LineEdit_L3C2->setText(resString) ;
    resString = tr("%1").arg( I(3,3), 12, 'f', 6 ) ;
    LineEdit_L3C3->setText(resString) ;

    /* moments */
    resString = tr("%1").arg( Ix, 12, 'f', 6 ) ;
    LineEdit_IX->setText(resString) ;
    resString = tr("%1").arg( Ix, 12, 'f', 6 ) ;
    LineEdit_IY->setText(resString) ;
    resString = tr("%1").arg( Iz, 12, 'f', 6 ) ;
    LineEdit_IZ->setText(resString) ;

  }
  catch(Standard_Failure) {
    MESSAGE("Catch intercepted in CalculateAndDisplayInertia()" << endl ) ;
  }
  return ;
}


