using namespace std;
//  File      : GeometryGUI_BndBoxDlg.cxx
//  Created   : Mon Mar 04 13:16:16 2002
//  Author    : Nicolas REJNERI

//  Project   : SALOME
//  Module    : GEOMGUI
//  Copyright : Open CASCADE 2002
//  $Header$


#include "GeometryGUI_BndBoxDlg.h"

#include "GeometryGUI.h"
#include "QAD_Application.h"
#include "QAD_Desktop.h"
#include "utilities.h"

#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepBndLib.hxx>

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
// class    : GeometryGUI_BndBoxDlg()
// purpose  : Constructs a GeometryGUI_BndBoxDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
GeometryGUI_BndBoxDlg::GeometryGUI_BndBoxDlg( QWidget* parent, const char* name, SALOME_Selection* Sel, bool modal, WFlags fl )
    : QDialog( parent, name, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu )
{
    QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap( "GEOM",tr("ICON_DLG_BOUNDING_BOX")));
    QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap( "GEOM",tr("ICON_SELECT")));

    if ( !name )
	setName( "GeometryGUI_BndBoxDlg" );
    resize( 303, 275 ); 
    setCaption( tr( "GEOM_BNDBOX_TITLE"  ) );
    setSizeGripEnabled( TRUE );
    GeometryGUI_BndBoxDlgLayout = new QGridLayout( this ); 
    GeometryGUI_BndBoxDlgLayout->setSpacing( 6 );
    GeometryGUI_BndBoxDlgLayout->setMargin( 11 );
    
    /***************************************************************/
    GroupConstructors = new QButtonGroup( this, "GroupConstructors" );
    GroupConstructors->setTitle( tr( "GEOM_BNDBOX"  ) );
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
    Constructor1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, 
					      Constructor1->sizePolicy().hasHeightForWidth() ) );
    Constructor1->setMinimumSize( QSize( 60, 0 ) );
    GroupConstructorsLayout->addWidget( Constructor1, 0, 0 );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    GroupConstructorsLayout->addItem( spacer, 0, 1 );
    GeometryGUI_BndBoxDlgLayout->addWidget( GroupConstructors, 0, 0 );

    /***************************************************************/
    GroupConstructor1 = new QGroupBox( this, "GroupConstructor1" );
    GroupConstructor1->setTitle( tr( "GEOM_BNDBOX_OBJDIM"  ) );
    GroupConstructor1->setColumnLayout(0, Qt::Vertical );
    GroupConstructor1->layout()->setSpacing( 0 );
    GroupConstructor1->layout()->setMargin( 0 );
    GroupConstructor1Layout = new QGridLayout( GroupConstructor1->layout() );
    GroupConstructor1Layout->setAlignment( Qt::AlignTop );
    GroupConstructor1Layout->setSpacing( 6 );
    GroupConstructor1Layout->setMargin( 11 );
    LineEditC1A1 = new QLineEdit( GroupConstructor1, "LineEditC1A1" );
    LineEditC1A1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, LineEditC1A1->sizePolicy().hasHeightForWidth() ) );
//    GroupConstructor1Layout->addWidget( LineEditC1A1, 0, 2 );
    SelectButtonC1A1 = new QPushButton( GroupConstructor1, "SelectButtonC1A1" );
    SelectButtonC1A1->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    SelectButtonC1A1->setText( tr( ""  ) );
    SelectButtonC1A1->setPixmap( image1 );
//    GroupConstructor1Layout->addWidget( SelectButtonC1A1, 0, 1 );

    TextLabelC1A1 = new QLabel( GroupConstructor1, "TextLabelC1A1" );
    TextLabelC1A1->setText( tr( "GEOM_OBJECT"  ) );
    TextLabelC1A1->setMinimumSize( QSize( 50, 0 ) );
    TextLabelC1A1->setFrameShape( QLabel::NoFrame );
    TextLabelC1A1->setFrameShadow( QLabel::Plain );
//    GroupConstructor1Layout->addWidget( TextLabelC1A1, 0, 0 );

    QHBoxLayout* bl = new QHBoxLayout;
    bl->setMargin(0); bl->setSpacing(6);
    bl->addWidget(TextLabelC1A1); bl->addWidget(SelectButtonC1A1); bl->addWidget(LineEditC1A1);
    GroupConstructor1Layout->addMultiCellLayout(bl, 0, 0, 0, 2);

    TextLabel_Min = new QLabel( GroupConstructor1, "TextLabel_Min" );
    TextLabel_Min->setText( tr( "GEOM_MIN"  ) );
    TextLabel_Min->setMinimumSize( QSize( 50, 0 ) );
    TextLabel_Min->setFrameShape( QLabel::NoFrame );
    TextLabel_Min->setFrameShadow( QLabel::Plain );
    GroupConstructor1Layout->addWidget( TextLabel_Min, 1, 1 );

    TextLabel_Max = new QLabel( GroupConstructor1, "TextLabel_Max" );
    TextLabel_Max->setText( tr( "GEOM_MAX"  ) );
    TextLabel_Max->setMinimumSize( QSize( 50, 0 ) );
    TextLabel_Max->setFrameShape( QLabel::NoFrame );
    TextLabel_Max->setFrameShadow( QLabel::Plain );
    GroupConstructor1Layout->addWidget( TextLabel_Max, 1, 2 );

    TextLabel_X = new QLabel( GroupConstructor1, "TextLabel_X" );
    TextLabel_X->setText( tr( "GEOM_X"  ) );
    TextLabel_X->setMinimumSize( QSize( 50, 0 ) );
    TextLabel_X->setFrameShape( QLabel::NoFrame );
    TextLabel_X->setFrameShadow( QLabel::Plain );
    GroupConstructor1Layout->addWidget( TextLabel_X, 2, 0 );
    LineEdit_MinX = new QLineEdit( GroupConstructor1, "LineEdit_MinX" );
    LineEdit_MinX->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0,
						 LineEdit_MinX->sizePolicy().hasHeightForWidth() ) );
    //LineEdit_MinX->setEnabled( FALSE );
    LineEdit_MinX->setReadOnly( TRUE );
    GroupConstructor1Layout->addWidget( LineEdit_MinX, 2, 1 );
    LineEdit_MaxX = new QLineEdit( GroupConstructor1, "LineEdit_MaxX" );
    LineEdit_MaxX->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0,
						 LineEdit_MaxX->sizePolicy().hasHeightForWidth() ) );
    //LineEdit_MaxX->setEnabled( FALSE );
    LineEdit_MaxX->setReadOnly( TRUE );
    GroupConstructor1Layout->addWidget( LineEdit_MaxX, 2, 2 );

    TextLabel_Y = new QLabel( GroupConstructor1, "TextLabel_Y" );
    TextLabel_Y->setText( tr( "GEOM_Y"  ) );
    TextLabel_Y->setMinimumSize( QSize( 50, 0 ) );
    TextLabel_Y->setFrameShape( QLabel::NoFrame );
    TextLabel_Y->setFrameShadow( QLabel::Plain );
    GroupConstructor1Layout->addWidget( TextLabel_Y, 3, 0 );
    LineEdit_MinY = new QLineEdit( GroupConstructor1, "LineEdit_MinY" );
    LineEdit_MinY->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0,
						 LineEdit_MinY->sizePolicy().hasHeightForWidth() ) );
    //LineEdit_MinY->setEnabled( FALSE );
    LineEdit_MinY->setReadOnly( TRUE );
    GroupConstructor1Layout->addWidget( LineEdit_MinY, 3, 1 );
    LineEdit_MaxY = new QLineEdit( GroupConstructor1, "LineEdit_MaxY" );
    LineEdit_MaxY->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0,
						 LineEdit_MaxY->sizePolicy().hasHeightForWidth() ) );
    //LineEdit_MaxY->setEnabled( FALSE );
    LineEdit_MaxY->setReadOnly( TRUE );
    GroupConstructor1Layout->addWidget( LineEdit_MaxY, 3, 2 );

    TextLabel_Z = new QLabel( GroupConstructor1, "TextLabel_Z" );
    TextLabel_Z->setText( tr( "GEOM_Z"  ) );
    TextLabel_Z->setMinimumSize( QSize( 50, 0 ) );
    TextLabel_Z->setFrameShape( QLabel::NoFrame );
    TextLabel_Z->setFrameShadow( QLabel::Plain );
    GroupConstructor1Layout->addWidget( TextLabel_Z, 4, 0 );
    LineEdit_MinZ = new QLineEdit( GroupConstructor1, "LineEdit_MinZ" );
    LineEdit_MinZ->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0,
						 LineEdit_MinZ->sizePolicy().hasHeightForWidth() ) );
    //LineEdit_MinZ->setEnabled( FALSE );
    LineEdit_MinZ->setReadOnly( TRUE );
    GroupConstructor1Layout->addWidget( LineEdit_MinZ, 4, 1 );
    LineEdit_MaxZ = new QLineEdit( GroupConstructor1, "LineEdit_MaxZ" );
    LineEdit_MaxZ->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0,
						 LineEdit_MaxZ->sizePolicy().hasHeightForWidth() ) );
    //LineEdit_MaxZ->setEnabled( FALSE );
    LineEdit_MaxZ->setReadOnly( TRUE );
    GroupConstructor1Layout->addWidget( LineEdit_MaxZ, 4, 2 );

    GeometryGUI_BndBoxDlgLayout->addWidget( GroupConstructor1, 1, 0 );

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

    GeometryGUI_BndBoxDlgLayout->addWidget( GroupButtons, 2, 0 );
    /***************************************************************/

    Init(Sel) ; /* Initialisations */
}


//=================================================================================
// function : ~GeometryGUI_BndBoxDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
GeometryGUI_BndBoxDlg::~GeometryGUI_BndBoxDlg()
{
    // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void GeometryGUI_BndBoxDlg::Init( SALOME_Selection* Sel )
{
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
  connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( ClickOnCancel() ) ) ;
  connect( GroupConstructors, SIGNAL(clicked(int) ), SLOT( ConstructorsClicked(int) ) );
  connect( SelectButtonC1A1, SIGNAL (clicked() ),   this, SLOT( SetEditCurrentArgument() ) ) ;
  connect( mySelection, SIGNAL( currentSelectionChanged() ),     this, SLOT( SelectionIntoArgument() ) );

  connect( myGeomGUI, SIGNAL ( SignalDeactivateActiveDialog() ), this, SLOT( DeactivateActiveDialog() ) ) ;
  /* to close dialog if study change */
  connect( myGeomGUI, SIGNAL ( SignalCloseAllDialogs() ), this, SLOT( ClickOnCancel() ) ) ;

  mySimulationTopoDs.Nullify() ;

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
void GeometryGUI_BndBoxDlg::ConstructorsClicked(int constructorId)
{
  return ;
}



//=================================================================================
// function : ClickOnCancel()
// purpose  :
//=================================================================================
void GeometryGUI_BndBoxDlg::ClickOnCancel()
{
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
void GeometryGUI_BndBoxDlg::SelectionIntoArgument()
{
  LineEdit_MinX->setText("") ;
  LineEdit_MinY->setText("") ;
  LineEdit_MinZ->setText("") ;
  LineEdit_MaxX->setText("") ;
  LineEdit_MaxY->setText("") ;
  LineEdit_MaxZ->setText("") ;  
  myEditCurrentArgument->setText("") ;
  mySimulationTopoDs.Nullify() ;

  QString aString = ""; /* future the name of selection */

  int nbSel = myGeomGUI->GetNameOfSelectedIObjects(mySelection, aString) ;
  if ( nbSel != 1 ) {
    return ;
  }

  /*  nbSel == 1  */ 
  TopoDS_Shape S;
  if( !myGeomGUI->GetTopoFromSelection(mySelection, S) )
    return ;
  
  if( S.IsNull() ) {
    myEditCurrentArgument->setText( "" );
    return ;
  }
 
  LineEditC1A1->setText(aString) ;
  this->CalculateAndDisplayBndBox(S) ;

  return ;
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void GeometryGUI_BndBoxDlg::SetEditCurrentArgument()
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
void GeometryGUI_BndBoxDlg::LineEditReturnPressed()
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
}


//=================================================================================
// function : DeactivateActiveDialog()
// purpose  :
//=================================================================================
void GeometryGUI_BndBoxDlg::DeactivateActiveDialog()
{
  if ( GroupConstructors->isEnabled() ) {    
    disconnect( mySelection, 0, this, 0 );
    GroupConstructors->setEnabled(false) ;
    GroupConstructor1->setEnabled(false) ;
    GroupButtons->setEnabled(false) ;
  }
  return ;
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void GeometryGUI_BndBoxDlg::ActivateThisDialog()
{
  /* Emit a signal to deactivate the active dialog */
  myGeomGUI->EmitSignalDeactivateDialog() ;   
  GroupConstructors->setEnabled(true) ;
  GroupConstructor1->setEnabled(true) ;
  GroupButtons->setEnabled(true) ;
  connect( mySelection, SIGNAL( currentSelectionChanged() ),     this, SLOT( SelectionIntoArgument() ) );
  return ;
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void GeometryGUI_BndBoxDlg::enterEvent(QEvent* e)
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
void GeometryGUI_BndBoxDlg::closeEvent( QCloseEvent* e )
{
  /* same than click on cancel button */
  this->ClickOnCancel() ;
  return ;
}


//=================================================================================
// function : CalculateAndDisplayBndBox()
// purpose  :
//=================================================================================
void GeometryGUI_BndBoxDlg::CalculateAndDisplayBndBox(const TopoDS_Shape& S)
{
  LineEdit_MinX->setText("") ;
  LineEdit_MinY->setText("") ;
  LineEdit_MinZ->setText("") ;
  LineEdit_MaxX->setText("") ;
  LineEdit_MaxY->setText("") ;
  LineEdit_MaxZ->setText("") ;
  if( S.IsNull() ) 
    return ;

  Standard_Real axmin,aymin,azmin,axmax,aymax,azmax;
  Bnd_Box B;
  try {
    BRepBndLib::Add(S,B);
    B.Get(axmin,aymin,azmin,axmax,aymax,azmax);
    LineEdit_MinX->setText( tr("%1").arg( axmin, 12, 'f', 6 ) ) ;
    LineEdit_MinY->setText( tr("%1").arg( aymin, 12, 'f', 6 ) ) ;
    LineEdit_MinZ->setText( tr("%1").arg( azmin, 12, 'f', 6 ) ) ;
    LineEdit_MaxX->setText( tr("%1").arg( axmax, 12, 'f', 6 ) ) ;
    LineEdit_MaxY->setText( tr("%1").arg( aymax, 12, 'f', 6 ) ) ;
    LineEdit_MaxZ->setText( tr("%1").arg( azmax, 12, 'f', 6 ) ) ;
    
    mySimulationTopoDs = BRepPrimAPI_MakeBox( gp_Pnt(axmin,aymin,azmin),
					      gp_Pnt(axmax,aymax,azmax) ).Shape();
    myGeomGUI->DisplaySimulationShape( mySimulationTopoDs ) ; 
  }
  catch(Standard_Failure) {
    MESSAGE("Catch intercepted in CalculateAndDisplayBndBox()" << endl ) ;
  }
  return ;
}
