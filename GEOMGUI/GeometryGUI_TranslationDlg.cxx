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
//  File   : GeometryGUI_TranslationDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

using namespace std;
#include "GeometryGUI_TranslationDlg.h"

#include "GeometryGUI.h"
#include "QAD_Application.h"
#include "QAD_Config.h"
#include "QAD_Desktop.h"
#include "utilities.h"

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
#include <qevent.h>



//=================================================================================
// class    : GeometryGUI_TranslationDlg()
// purpose  : Constructs a GeometryGUI_TranslationDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
GeometryGUI_TranslationDlg::GeometryGUI_TranslationDlg( QWidget* parent, const char* name, SALOME_Selection* Sel, bool modal, WFlags fl )
    : QDialog( parent, name, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu )
{
    QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap( "GeometryGUI",tr("ICON_DLG_TRANSLATION")));
    QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap( "GeometryGUI",tr("ICON_SELECT")));

    if ( !name )
	setName( "GeometryGUI_TranslationDlg" );
    resize( 303, 219 ); 
    setCaption( tr( "GEOM_TRANSLATION_TITLE"  ) );
    setSizeGripEnabled( TRUE );
    GeometryGUI_TranslationDlgLayout = new QGridLayout( this ); 
    GeometryGUI_TranslationDlgLayout->setSpacing( 6 );
    GeometryGUI_TranslationDlgLayout->setMargin( 11 );

    /***************************************************************/
    GroupConstructors = new QButtonGroup( this, "GroupConstructors" );
    GroupConstructors->setTitle( tr( "GEOM_TRANSLATION"  ) );
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
    GeometryGUI_TranslationDlgLayout->addWidget( GroupConstructors, 0, 0 );

    GroupC1 = new QGroupBox( this, "GroupC1" );
    GroupC1->setTitle( tr( "GEOM_TRANSLATION"  ) ) ;
    GroupC1->setMinimumSize( QSize( 0, 0 ) );
    GroupC1->setFrameShape( QGroupBox::Box );
    GroupC1->setFrameShadow( QGroupBox::Sunken );
    GroupC1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)5, GroupC1->sizePolicy().hasHeightForWidth() ) );
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
    SelectButtonC1A1->setToggleButton( FALSE );
    SelectButtonC1A1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, SelectButtonC1A1->sizePolicy().hasHeightForWidth() ) );
    GroupC1Layout->addWidget( SelectButtonC1A1, 0, 1 );

    LineEditC1A1 = new QLineEdit( GroupC1, "LineEditC1A1" );
    GroupC1Layout->addWidget( LineEditC1A1, 0, 2 );

    Layout1 = new QHBoxLayout; 
    Layout1->setSpacing( 6 );
    Layout1->setMargin( 0 );

    TextLabel_DX = new QLabel( GroupC1, "TextLabel_DX" );
    TextLabel_DX->setText( tr( "GEOM_DX" ) );
    Layout1->addWidget( TextLabel_DX );

    SpinBox_DX = new GeometryGUI_SpinBox( GroupC1, "SpinBox_DX" );
    SpinBox_DX->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, SpinBox_DX->sizePolicy().hasHeightForWidth() ) );
    Layout1->addWidget( SpinBox_DX );

    TextLabel_DY = new QLabel( GroupC1, "TextLabel_DY" );
    TextLabel_DY->setText( tr( "GEOM_DY" ) );
    Layout1->addWidget( TextLabel_DY );

    SpinBox_DY = new GeometryGUI_SpinBox( GroupC1, "SpinBox_DY" );
    SpinBox_DY->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, SpinBox_DY->sizePolicy().hasHeightForWidth() ) );
    Layout1->addWidget( SpinBox_DY );

    TextLabel_DZ = new QLabel( GroupC1, "TextLabel_DZ" );
    TextLabel_DZ->setText( tr( "GEOM_DZ" ) );
    Layout1->addWidget( TextLabel_DZ );

    SpinBox_DZ = new GeometryGUI_SpinBox( GroupC1, "SpinBox_DZ" );
    SpinBox_DZ->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, SpinBox_DZ->sizePolicy().hasHeightForWidth() ) );
    Layout1->addWidget( SpinBox_DZ );

    GroupC1Layout->addMultiCellLayout( Layout1, 1, 1, 0, 2 );

    GeometryGUI_TranslationDlgLayout->addWidget( GroupC1, 1, 0 );


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
    GeometryGUI_TranslationDlgLayout->addWidget( GroupButtons, 2, 0 );
    /***************************************************************/

    Init(Sel) ; /* Initialisations */
}



//=================================================================================
// function : ~GeometryGUI_TranslationDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
GeometryGUI_TranslationDlg::~GeometryGUI_TranslationDlg()
{
    // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void GeometryGUI_TranslationDlg::Init( SALOME_Selection* Sel )
{
  double step ;
  QString St = QAD_CONFIG->getSetting( "Geometry:SettingsGeomStep" ) ;
  step = St.toDouble() ;

  /* min, max, step and decimals for spin boxes */
  SpinBox_DX->RangeStepAndValidator( -999.999, 999.999, step, 3 ) ;
  SpinBox_DX->SetValue( 100.0 ) ;
  SpinBox_DY->RangeStepAndValidator( -999.999, 999.999, step, 3 ) ;
  SpinBox_DY->SetValue( 100.0 ) ;
  SpinBox_DZ->RangeStepAndValidator( -999.999, 999.999, step, 3 ) ;
  SpinBox_DZ->SetValue( 100.0 ) ; 

  GroupC1->show();
  myConstructorId = 0 ;
  Constructor1->setChecked( TRUE );
  myEditCurrentArgument = LineEditC1A1 ;	
  mySelection = Sel;
  myGeomGUI = GeometryGUI::GetGeometryGUI() ;
  myOkBase = false ;

  this->myVec.SetCoord( 100.0, 100.0, 100.0 ) ;
  mySimulationTopoDs.Nullify() ;
  myBase.Nullify() ;
  myGeomGUI->SetActiveDialogBox( (QDialog*)this ) ;  

  // TODO : previous selection into argument ?

  /* Filter definitions */
  Engines::Component_var comp = QAD_Application::getDesktop()->getEngine("FactoryServer", "Geometry");
  myGeom = GEOM::GEOM_Gen::_narrow(comp);
  // myEdgeFilter   = new GEOM_ShapeTypeFilter( TopAbs_EDGE, myGeom );

  /* signals and slots connections */
  connect( buttonOk, SIGNAL( clicked() ),     this, SLOT( ClickOnOk() ) );
  connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( ClickOnCancel() ) ) ;
  connect( buttonApply, SIGNAL( clicked() ), this, SLOT(ClickOnApply() ) );
  connect( GroupConstructors, SIGNAL(clicked(int) ), SLOT( ConstructorsClicked(int) ) );  
  connect( SelectButtonC1A1, SIGNAL (clicked() ), this, SLOT( SetEditCurrentArgument() ) ) ;
  
  connect( SpinBox_DX, SIGNAL ( valueChanged( double) ), this, SLOT( ValueChangedInSpinBox( double) ) ) ;
  connect( SpinBox_DY, SIGNAL ( valueChanged( double) ), this, SLOT( ValueChangedInSpinBox( double) ) ) ;
  connect( SpinBox_DZ, SIGNAL ( valueChanged( double) ), this, SLOT( ValueChangedInSpinBox( double) ) ) ;

  connect( LineEditC1A1, SIGNAL ( returnPressed() ), this, SLOT( LineEditReturnPressed() ) ) ;

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
void GeometryGUI_TranslationDlg::ConstructorsClicked(int constructorId)
{
  // myGeomGUI->EraseSimulationShape() ;
  switch (constructorId)
    {
    case 0:
      {
	break;
      }
    case 1:
      {
	break;
      }
    }
  return ;
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void GeometryGUI_TranslationDlg::ClickOnOk()
{
  this->ClickOnApply() ;
  this->ClickOnCancel() ;

  return ;
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
void GeometryGUI_TranslationDlg::ClickOnApply()
{
  myGeomGUI->EraseSimulationShape() ;
  myGeomGUI->GetDesktop()->putInfo( tr("") ) ; 
  mySimulationTopoDs.Nullify() ;
  switch(myConstructorId)
    { 
    case 0 :
      { 
	if(myOkBase) {
	  myGeomGUI->MakeTranslationAndDisplay( myGeomShape, myVec ) ;
	}
	break ;
      }
    case 1 :
      {
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
void GeometryGUI_TranslationDlg::ClickOnCancel()
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
void GeometryGUI_TranslationDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("") ;
  myGeomGUI->EraseSimulationShape() ; 
  mySimulationTopoDs.Nullify() ;

  /* Future name of selection */
  QString aString = "";

  int nbSel = myGeomGUI->GetNameOfSelectedIObjects(mySelection, aString) ;
  if ( nbSel != 1 ) {
    if ( myEditCurrentArgument == LineEditC1A1 ) {
      LineEditC1A1->setText("") ;
      myOkBase = false ;
    }
    return ;
  }

  /* nbSel == 1 ! */
  TopoDS_Shape S; 
  Standard_Boolean testResult ;
  Handle(SALOME_InteractiveObject) IO = mySelection->firstIObject() ;

  if( !myGeomGUI->GetTopoFromSelection(mySelection, S) )
    return ;

  if ( myConstructorId == 0 && myEditCurrentArgument == LineEditC1A1 ) { 
    myGeomShape = myGeomGUI->ConvertIOinGEOMShape(IO, testResult) ;
    if( !testResult )
      return ;
    LineEditC1A1->setText(aString) ;
    myBase = S ;
    myOkBase = true ;
  }
  
  if( myOkBase ) {
    MakeTranslationSimulationAndDisplay() ;
  }
  return ;
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void GeometryGUI_TranslationDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();
  switch (myConstructorId)
    {
    case 0: /* default constructor */
      {	
	if(send == SelectButtonC1A1) {
	  LineEditC1A1->setFocus() ;
	  myEditCurrentArgument = LineEditC1A1 ;
	  SelectionIntoArgument() ;
	}
	break;
      }
    case 1:      
      {		
	break;
      }
    }
  return ;
}

//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void GeometryGUI_TranslationDlg::LineEditReturnPressed()
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
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void GeometryGUI_TranslationDlg::ValueChangedInSpinBox( double newValue )
{  

  QObject* send = (QObject*)sender() ; 
  Standard_Real Dx, Dy, Dz ;

  if( send == SpinBox_DX ) {
    Dx = newValue ;
    Dy = SpinBox_DY->GetValue() ;
    Dz = SpinBox_DZ->GetValue() ;
  } else if( send == SpinBox_DY ) {
    Dx = SpinBox_DX->GetValue() ;
    Dy = newValue ;
    Dz = SpinBox_DZ->GetValue() ;
  } else if( send == SpinBox_DZ ) {
    Dx = SpinBox_DX->GetValue() ;
    Dy = SpinBox_DY->GetValue() ;
    Dz = newValue ;
  }

  this->myVec.SetCoord(Dx, Dy, Dz) ;
  if ( myOkBase ) {
    MakeTranslationSimulationAndDisplay() ;
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
void GeometryGUI_TranslationDlg::DeactivateActiveDialog()
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
void GeometryGUI_TranslationDlg::ActivateThisDialog()
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
void GeometryGUI_TranslationDlg::enterEvent(QEvent* e)
{
  if ( GroupConstructors->isEnabled() )
    return ;  
  ActivateThisDialog() ;
}



//=================================================================================
// function : closeEvent()
// purpose  :
//=================================================================================
void GeometryGUI_TranslationDlg::closeEvent( QCloseEvent* e )
{
  /* same than click on cancel button */
  this->ClickOnCancel() ;
  return ;
}


//=================================================================================
// function : MakeTranslationSimulationAndDisplay()
// purpose  :
//=================================================================================
void GeometryGUI_TranslationDlg::MakeTranslationSimulationAndDisplay()
{
  myGeomGUI->EraseSimulationShape() ; 
  mySimulationTopoDs.Nullify() ;

  gp_Trsf theTransformation ;
  theTransformation.SetTranslation(this->myVec) ;

  BRepBuilderAPI_Transform myBRepTransformation(this->myBase, theTransformation, Standard_False) ;
  mySimulationTopoDs = myBRepTransformation.Shape() ;
  myGeomGUI->DisplaySimulationShape( mySimulationTopoDs ) ;
  return ;
}
