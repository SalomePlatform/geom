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
//  File   : GeometryGUI_CenterMassDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

using namespace std;
#include "GeometryGUI_CenterMassDlg.h"
#include "GeometryGUI.h"

#include <BRepBuilderAPI_MakeVertex.hxx>

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
// class    : GeometryGUI_CenterMassDlg()
// purpose  : Constructs a GeometryGUI_CenterMassDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
GeometryGUI_CenterMassDlg::GeometryGUI_CenterMassDlg( QWidget* parent, const char* name, SALOME_Selection* Sel, bool modal, WFlags fl )
    : QDialog( parent, name, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu )
{
    QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap( "GEOM",tr("ICON_DLG_CENTERMASS")));
    QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap( "GEOM",tr("ICON_SELECT")));

    if ( !name )
	setName( "GeometryGUI_CenterMassDlg" );
    resize( 398, 219 ); 
    setCaption( tr( "GEOM_CMASS_TITLE"  ) );
    setSizeGripEnabled( TRUE );
    GeometryGUI_CenterMassDlgLayout = new QGridLayout( this ); 
    GeometryGUI_CenterMassDlgLayout->setSpacing( 6 );
    GeometryGUI_CenterMassDlgLayout->setMargin( 11 );
    
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
    GeometryGUI_CenterMassDlgLayout->addWidget( GroupButtons, 2, 0 );

    /***************************************************************/
    GroupConstructors = new QButtonGroup( this, "GroupConstructors" );
    GroupConstructors->setTitle( tr( "GEOM_CMASS"  ) );
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
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    GroupConstructorsLayout->addItem( spacer_2, 0, 1 );
    GeometryGUI_CenterMassDlgLayout->addWidget( GroupConstructors, 0, 0 );

    /***************************************************************/
    GroupC1 = new QGroupBox( this, "GroupC1" );
    GroupC1->setTitle( tr( "GEOM_OBJECT_RESULT"  ) );
    GroupC1->setMinimumSize( QSize( 0, 0 ) );
    GroupC1->setFrameShape( QGroupBox::Box );
    GroupC1->setFrameShadow( QGroupBox::Sunken );
    GroupC1->setColumnLayout(0, Qt::Vertical );
    GroupC1->layout()->setSpacing( 0 );
    GroupC1->layout()->setMargin( 0 );
    GroupC1Layout = new QGridLayout( GroupC1->layout() );
    GroupC1Layout->setAlignment( Qt::AlignTop );
    GroupC1Layout->setSpacing( 6 );
    GroupC1Layout->setMargin( 11 );
    SelectButtonC1A1 = new QPushButton( GroupC1, "SelectButtonC1A1" );
    SelectButtonC1A1->setText( tr( ""  ) );
    SelectButtonC1A1->setPixmap( image1 );
    SelectButtonC1A1->setToggleButton( FALSE );
    GroupC1Layout->addWidget( SelectButtonC1A1, 0, 1 );
    LineEdit_X = new QLineEdit( GroupC1, "LineEdit_X" );
    LineEdit_X->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, LineEdit_X->sizePolicy().hasHeightForWidth() ) );
    LineEdit_X->setMinimumSize( QSize( 70, 0 ) );
    //LineEdit_X->setEnabled( FALSE );
    LineEdit_X->setReadOnly( TRUE );
    GroupC1Layout->addWidget( LineEdit_X, 1, 2 );
    TextLabel_Z = new QLabel( GroupC1, "TextLabel_Z" );
    TextLabel_Z->setText( tr( "GEOM_Z"  ) );
    TextLabel_Z->setMinimumSize( QSize( 15, 0 ) );
    TextLabel_Z->setFrameShape( QLabel::NoFrame );
    TextLabel_Z->setFrameShadow( QLabel::Plain );
    TextLabel_Z->setMaximumSize( QSize( 15, 32767 ) );
    GroupC1Layout->addWidget( TextLabel_Z, 1, 5 );
    LineEdit_Z = new QLineEdit( GroupC1, "LineEdit_Z" );
    LineEdit_Z->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, LineEdit_Z->sizePolicy().hasHeightForWidth() ) );
    LineEdit_Z->setMinimumSize( QSize( 70, 0 ) );
    //LineEdit_Z->setEnabled( FALSE );
    LineEdit_Z->setReadOnly( TRUE );
    GroupC1Layout->addWidget( LineEdit_Z, 1, 6 );
    TextLabelC1A1 = new QLabel( GroupC1, "TextLabelC1A1" );
    TextLabelC1A1->setText( tr( "GEOM_OBJECT"  ) );
    TextLabelC1A1->setMinimumSize( QSize( 50, 0 ) );
    TextLabelC1A1->setFrameShape( QLabel::NoFrame );
    TextLabelC1A1->setFrameShadow( QLabel::Plain );
    GroupC1Layout->addWidget( TextLabelC1A1, 0, 0 );
    LineEdit_Y = new QLineEdit( GroupC1, "LineEdit_Y" );
    LineEdit_Y->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, LineEdit_Y->sizePolicy().hasHeightForWidth() ) );
    LineEdit_Y->setMinimumSize( QSize( 70, 0 ) );
    //LineEdit_Y->setEnabled( FALSE );
    LineEdit_Y->setReadOnly( TRUE );
    GroupC1Layout->addWidget( LineEdit_Y, 1, 4 );
    TextLabel_Y = new QLabel( GroupC1, "TextLabel_Y" );
    TextLabel_Y->setText( tr( "GEOM_Y"  ) );
    TextLabel_Y->setMinimumSize( QSize( 15, 0 ) );
    TextLabel_Y->setFrameShape( QLabel::NoFrame );
    TextLabel_Y->setFrameShadow( QLabel::Plain );
    TextLabel_Y->setMaximumSize( QSize( 15, 32767 ) );
    GroupC1Layout->addWidget( TextLabel_Y, 1, 3 );
    TextLabel_X = new QLabel( GroupC1, "TextLabel_X" );
    TextLabel_X->setText( tr( "GEOM_X"  ) );
    TextLabel_X->setMinimumSize( QSize( 15, 0 ) );
    TextLabel_X->setFrameShape( QLabel::NoFrame );
    TextLabel_X->setFrameShadow( QLabel::Plain );
    TextLabel_X->setMaximumSize( QSize( 15, 32767 ) );
    GroupC1Layout->addWidget( TextLabel_X, 1, 1 );
    TextLabel_Center = new QLabel( GroupC1, "TextLabel_Center" );
    TextLabel_Center->setText( tr( "GEOM_CENTER"  ) );
    TextLabel_Center->setMinimumSize( QSize( 50, 0 ) );
    TextLabel_Center->setFrameShape( QLabel::NoFrame );
    TextLabel_Center->setFrameShadow( QLabel::Plain );
    GroupC1Layout->addWidget( TextLabel_Center, 1, 0 );
    LineEditC1A1 = new QLineEdit( GroupC1, "LineEditC1A1" );
    LineEditC1A1->setMinimumSize( QSize( 260, 0 ) );
    GroupC1Layout->addMultiCellWidget( LineEditC1A1, 0, 0, 2, 6 );
    GeometryGUI_CenterMassDlgLayout->addWidget( GroupC1, 1, 0 );
    /***************************************************************/

    Init(Sel) ; /* Initialisations */

}


//=================================================================================
// function : ~GeometryGUI_CenterMassDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
GeometryGUI_CenterMassDlg::~GeometryGUI_CenterMassDlg()
{
    // no need to delete child widgets, Qt does it all for us
}



//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void GeometryGUI_CenterMassDlg::Init( SALOME_Selection* Sel )
{
  LineEdit_X->setMaxLength( 9 );
  LineEdit_Y->setMaxLength( 9 );
  LineEdit_Z->setMaxLength( 9 );
  QDoubleValidator *Va = new QDoubleValidator( -999999, +999999, 3, LineEdit_X ) ;
  QDoubleValidator *Vb = new QDoubleValidator( -999999, +999999, 3, LineEdit_Y ) ;
  QDoubleValidator *Vc = new QDoubleValidator( -999999, +999999, 3, LineEdit_Z ) ; 
  LineEdit_X->setValidator( Va ) ;
  LineEdit_Y->setValidator( Vb ) ;
  LineEdit_Z->setValidator( Vc ) ;

  myConstructorId = 0 ;

  LineEdit_X->setText("") ;
  LineEdit_Y->setText("") ;
  LineEdit_Z->setText("") ;

  Constructor1->setChecked( TRUE );
  myEditCurrentArgument = LineEditC1A1 ;	
  mySelection = Sel;
  myGeomGUI = GeometryGUI::GetGeometryGUI() ;
  myGeomGUI->SetActiveDialogBox( (QDialog*)this ) ;
  mySimulationTopoDs.Nullify() ;
  myShape.Nullify() ;
  myOkCenterMass = false ;

  // TODO : previous selection into argument ?

  /* Filter definitions */
  Engines::Component_var comp = QAD_Application::getDesktop()->getEngine("FactoryServer", "GEOM");
  myGeom = GEOM::GEOM_Gen::_narrow(comp);

  /* signals and slots connections */
  connect( buttonOk, SIGNAL( clicked() ), this, SLOT( ClickOnOk() ) ) ;
  connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( ClickOnCancel() ) ) ;
  connect( buttonApply, SIGNAL( clicked() ),     this, SLOT( ClickOnApply() ) );
  connect( GroupConstructors, SIGNAL(clicked(int) ), SLOT( ConstructorsClicked(int) ) );
  connect( SelectButtonC1A1, SIGNAL (clicked() ),   this, SLOT( SetEditCurrentArgument() ) ) ;
  connect( mySelection, SIGNAL( currentSelectionChanged() ),     this, SLOT( SelectionIntoArgument() ) );

  connect( myGeomGUI, SIGNAL ( SignalDeactivateActiveDialog() ), this, SLOT( DeactivateActiveDialog() ) ) ;
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
void GeometryGUI_CenterMassDlg::ConstructorsClicked(int constructorId)
{
  return ;
}



//=================================================================================
// function : ClickOnCancel()
// purpose  :
//=================================================================================
void GeometryGUI_CenterMassDlg::ClickOnCancel()
{
  myGeomGUI->EraseSimulationShape() ;
  mySimulationTopoDs.Nullify() ;
  disconnect( mySelection, 0, this, 0 );
  myGeomGUI->ResetState() ;
  reject() ;
  return ;
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void GeometryGUI_CenterMassDlg::ClickOnOk()
{
  this->ClickOnApply() ;
  this->ClickOnCancel() ;

  return ;
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
void GeometryGUI_CenterMassDlg::ClickOnApply()
{
  myGeomGUI->EraseSimulationShape() ;
  mySimulationTopoDs.Nullify() ;
  myGeomGUI->GetDesktop()->putInfo( tr("") ) ; 
  if( myOkCenterMass) {    
    myGeomGUI->MakeCDGAndDisplay( myGeomShape ) ;
  }
  return ;
}




//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void GeometryGUI_CenterMassDlg::SelectionIntoArgument()
{
  myGeomGUI->EraseSimulationShape() ;
  myEditCurrentArgument->setText("") ;
  myOkCenterMass = false ;
  Standard_Boolean testResult ;

  LineEdit_X->setText("") ;
  LineEdit_Y->setText("") ;
  LineEdit_Z->setText("") ;

  QString aString = ""; /* future the name of selection */

  int nbSel = myGeomGUI->GetNameOfSelectedIObjects(mySelection, aString) ;
  if ( nbSel != 1 ) {
    return ;
  }

  /*  nbSel == 1  */
  Handle(SALOME_InteractiveObject) IO = mySelection->firstIObject() ;
  if( !myGeomGUI->GetTopoFromSelection(mySelection, this->myShape) )
    return ;  

  myGeomShape = myGeomGUI->ConvertIOinGEOMShape(IO, testResult) ;
  if( !testResult )
	    return ;
  myEditCurrentArgument->setText(aString) ;
  if( this->CalculateAndDisplayCenterMass() ) {
    myOkCenterMass = true ;
  }

  return ;
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void GeometryGUI_CenterMassDlg::SetEditCurrentArgument()
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
void GeometryGUI_CenterMassDlg::LineEditReturnPressed()
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
void GeometryGUI_CenterMassDlg::DeactivateActiveDialog()
{
  if ( GroupConstructors->isEnabled() ) {    
    myGeomGUI->EraseSimulationShape() ;  
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
void GeometryGUI_CenterMassDlg::ActivateThisDialog()
{
  /* Emit a signal to deactivate the active dialog */
  myGeomGUI->EmitSignalDeactivateDialog() ;   
  GroupConstructors->setEnabled(true) ;
  GroupC1->setEnabled(true) ;
  GroupButtons->setEnabled(true) ;
  connect( mySelection, SIGNAL( currentSelectionChanged() ),     this, SLOT( SelectionIntoArgument() ) );
  if( !mySimulationTopoDs.IsNull() )
    myGeomGUI->DisplaySimulationShape( mySimulationTopoDs ) ;
  return ;
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void GeometryGUI_CenterMassDlg::enterEvent(QEvent* e)
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
void GeometryGUI_CenterMassDlg::closeEvent( QCloseEvent* e )
{
  /* same than click on cancel button */
  this->ClickOnCancel() ;
  return ;
}


//=================================================================================
// function : CalculateAndDisplayCenterMass()
// purpose  :
//=================================================================================
bool GeometryGUI_CenterMassDlg::CalculateAndDisplayCenterMass()
{
  myGeomGUI->EraseSimulationShape() ;
  mySimulationTopoDs.Nullify() ;

  try {

    QString resString;     
    GProp_GProps System;

    if ( this->myShape.ShapeType() == TopAbs_VERTEX) {
      myGeomGUI->VertexToPoint( this->myShape, this->myCenterMass  );
    } 
    else if ( this->myShape.ShapeType() == TopAbs_EDGE || this->myShape.ShapeType() == TopAbs_WIRE ) {
      BRepGProp::LinearProperties(this->myShape, System);
      this->myCenterMass = System.CentreOfMass() ;
    }
    else if ( this->myShape.ShapeType() == TopAbs_FACE || this->myShape.ShapeType() == TopAbs_SHELL ) {
      BRepGProp::SurfaceProperties(this->myShape, System);
      this->myCenterMass = System.CentreOfMass() ;
    }
    else {
      BRepGProp::VolumeProperties(this->myShape, System);
      this->myCenterMass = System.CentreOfMass() ;
    }
    
    BRepBuilderAPI_MakeVertex V(this->myCenterMass) ;
    mySimulationTopoDs = V.Shape() ;
    
    resString = tr("%1").arg( myCenterMass.X(), 12, 'f', 6 ) ;    
    LineEdit_X->setText(resString) ;
    
    resString = tr("%1").arg( myCenterMass.Y(), 12, 'f', 6 ) ;    
    LineEdit_Y->setText(resString) ;
    
    resString = tr("%1").arg( myCenterMass.Z(), 12, 'f', 6 ) ;    
    LineEdit_Z->setText(resString) ;
    
    
    if( !mySimulationTopoDs.IsNull() ) {
      myGeomGUI->DisplaySimulationShape( mySimulationTopoDs ) ;
      return true ;
    }
  }
  catch(Standard_Failure) {
    MESSAGE("Catch intercepted in CalculateAndDisplayCenterMass()" << endl ) ;
  }
  return false ;
}


