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
//  File   : GeometryGUI_PartitionDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

using namespace std;
#include "GeometryGUI_PartitionDlg.h"

#include "GeometryGUI.h"
#include "QAD_Application.h"
#include "QAD_Desktop.h"
#include "utilities.h"

#include <qbuttongroup.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <qimage.h>
#include <qpixmap.h>

#define   MIN_EDIT_SIZE       150

//=================================================================================
// class    : GeometryGUI_PartitionDlg()
// purpose  : Constructs a GeometryGUI_PartitionDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
GeometryGUI_PartitionDlg::GeometryGUI_PartitionDlg( QWidget* parent, const char* name, SALOME_Selection* Sel, bool modal, WFlags fl )
    : QDialog( parent, name, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu )
{
    QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap( "GeometryGUI",tr("ICON_DLG_PARTITION")));
    QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap( "GeometryGUI",tr("ICON_SELECT")));
//    QPixmap image2(QAD_Desktop::getResourceManager()->loadPixmap( "GeometryGUI",tr("ICON_DLG_PARTITION_KEEP_FACES")));

    if ( !name )
	setName( "GeometryGUI_PartitionDlg" );
    setCaption( tr( "GEOM_PARTITION_TITLE"  ) );
    setSizeGripEnabled( TRUE );
    QGridLayout* GeometryGUI_PartitionDlgLayout = new QGridLayout( this ); 
    GeometryGUI_PartitionDlgLayout->setSpacing( 6 );
    GeometryGUI_PartitionDlgLayout->setMargin( 11 );
    
    /***************************************************************/
    /* Constructor group */
    /***************************************************************/
    GroupConstructors = new QButtonGroup( this, "GroupConstructors" );
    GroupConstructors->setTitle( tr( "GEOM_PARTITION"  ) );
    GroupConstructors->setColumnLayout(0, Qt::Vertical );
    GroupConstructors->layout()->setSpacing( 0 );
    GroupConstructors->layout()->setMargin( 0 );
    QGridLayout* GroupConstructorsLayout = new QGridLayout( GroupConstructors->layout() );
    GroupConstructorsLayout->setAlignment( Qt::AlignTop );
    GroupConstructorsLayout->setSpacing( 6 );
    GroupConstructorsLayout->setMargin( 11 );

    Constructor1 = new QRadioButton( GroupConstructors, "Constructor1" );
    Constructor1->setPixmap( image0 );
    Constructor1->setChecked( TRUE );
    Constructor1->setMinimumSize( QSize( 50, 0 ) );
    GroupConstructorsLayout->addWidget( Constructor1, 0, 0 );
    GroupConstructorsLayout->addItem( new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ), 1, 0 );

    GeometryGUI_PartitionDlgLayout->addWidget( GroupConstructors, 0, 0 );

    /***************************************************************/
    /* Arguments group */
    /***************************************************************/
    /* Shapes and Tools */
    GroupC1 = new QGroupBox( this, "GroupC1" );
    GroupC1->setTitle( tr( "GEOM_PARTITION"  ) );
    GroupC1->setColumnLayout(0, Qt::Vertical );
    GroupC1->layout()->setSpacing( 0 );
    GroupC1->layout()->setMargin( 0 );
    QGridLayout* GroupC1Layout = new QGridLayout( GroupC1->layout() );
    GroupC1Layout->setAlignment( Qt::AlignTop );
    GroupC1Layout->setSpacing( 6 );
    GroupC1Layout->setMargin( 11 );

    TextLabelC1A1 = new QLabel( GroupC1, "TextLabelC1A1" );
    TextLabelC1A1->setText( tr( "GEOM_OBJECTS"  ) );
    GroupC1Layout->addWidget( TextLabelC1A1, 0, 0 );
    SelectButtonC1A1 = new QPushButton( GroupC1, "SelectButtonC1A1" );
    SelectButtonC1A1->setPixmap( image1 );
    SelectButtonC1A1->setToggleButton( FALSE );
    GroupC1Layout->addWidget( SelectButtonC1A1, 0, 1 );
    LineEditC1A1 = new QLineEdit( GroupC1, "LineEditC1A1" );
    LineEditC1A1->setMinimumSize(MIN_EDIT_SIZE, 0);
    GroupC1Layout->addWidget( LineEditC1A1, 0, 2 );
    
    TextLabelC1A2 = new QLabel( GroupC1, "TextLabelC1A2" );
    TextLabelC1A2->setText( tr( "GEOM_TOOL_OBJECT"  ) );
    GroupC1Layout->addWidget( TextLabelC1A2, 1, 0 );
    SelectButtonC1A2 = new QPushButton( GroupC1, "SelectButtonC1A2" );
    SelectButtonC1A2->setPixmap( image1 );
    SelectButtonC1A2->setToggleButton( FALSE );
    GroupC1Layout->addWidget( SelectButtonC1A2, 1, 1 );
    LineEditC1A2 = new QLineEdit( GroupC1, "LineEditC1A2" );
    LineEditC1A2->setMinimumSize(MIN_EDIT_SIZE, 0);
    GroupC1Layout->addWidget( LineEditC1A2, 1, 2 );

    /* Limit */
    TextLabelComboBox1 = new QLabel( GroupC1, "TextLabelComboBox1" );
    TextLabelComboBox1->setText( tr( "RECONSTRUCTION_LIMIT") );
    GroupC1Layout->addWidget( TextLabelComboBox1, 2, 0 );
    ComboBox1 = new QComboBox( FALSE, GroupC1, "ComboBox1" );
    ComboBox1->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum ) );
    ComboBox1->setMinimumSize(MIN_EDIT_SIZE, 0);
    GroupC1Layout->addWidget( ComboBox1, 2, 2 );

    GeometryGUI_PartitionDlgLayout->addWidget( GroupC1, 1, 0 );

    /***************************************************************/
    /* Result suppresion */
    /***************************************************************/
    GroupC2 = new QGroupBox( this, "GroupC2" );
    GroupC2->setTitle( tr( "SUPPRESS_RESULT") );
    GroupC2->setColumnLayout(0, Qt::Vertical );
    GroupC2->layout()->setSpacing( 0 );
    GroupC2->layout()->setMargin( 0 );
    QGridLayout* GroupC2Layout = new QGridLayout( GroupC2->layout() );
    GroupC2Layout->setAlignment( Qt::AlignTop );
    GroupC2Layout->setSpacing( 6 );
    GroupC2Layout->setMargin( 11 );

    TextLabelC2A1 = new QLabel( GroupC2, "TextLabelC2A1" );
    TextLabelC2A1->setText( tr( "SUPPRESS_RESULT_INSIDE") );
    GroupC2Layout->addWidget( TextLabelC2A1, 0, 0 );
    SelectButtonC2A1 = new QPushButton( GroupC2, "SelectButtonC2A1" );
    SelectButtonC2A1->setPixmap( image1 );
    GroupC2Layout->addWidget( SelectButtonC2A1, 0, 1 );
    LineEditC2A1 = new QLineEdit( GroupC2, "LineEditC2A1" );
    GroupC2Layout->addWidget( LineEditC2A1, 0, 2 );

    TextLabelC2A2 = new QLabel( GroupC2, "TextLabelC2A2" );
    TextLabelC2A2->setText( tr( "SUPPRESS_RESULT_OUTSIDE") );
    GroupC2Layout->addWidget( TextLabelC2A2, 1, 0 );
    SelectButtonC2A2 = new QPushButton( GroupC2, "SelectButtonC2A2" );
    SelectButtonC2A2->setPixmap( image1 );
    GroupC2Layout->addWidget( SelectButtonC2A2, 1, 1 );
    LineEditC2A2 = new QLineEdit( GroupC2, "LineEditC2A2" );
    GroupC2Layout->addWidget( LineEditC2A2, 1, 2 );

    GeometryGUI_PartitionDlgLayout->addWidget( GroupC2, 2, 0 );
    
    /***************************************************************/
    /* <OK>, <Apply>, <Cancel> buttons */
    /***************************************************************/
    GroupButtons = new QGroupBox( this, "GroupButtons" );
    GroupButtons->setColumnLayout(0, Qt::Vertical );
    GroupButtons->layout()->setSpacing( 0 );
    GroupButtons->layout()->setMargin( 0 );
    QGridLayout* GroupButtonsLayout = new QGridLayout( GroupButtons->layout() );
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

    GroupButtonsLayout->addItem( new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ), 0, 2 );

    buttonCancel = new QPushButton( GroupButtons, "buttonCancel" );
    buttonCancel->setText( tr( "GEOM_BUT_CLOSE"  ) );
    buttonCancel->setAutoDefault( TRUE );
    GroupButtonsLayout->addWidget( buttonCancel, 0, 3 );

    GeometryGUI_PartitionDlgLayout->addWidget( GroupButtons, 3, 0 );

    /***************************************************************/
    Init(Sel) ; /* Initialisations */
}


//=================================================================================
// function : ~GeometryGUI_PartitionDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
GeometryGUI_PartitionDlg::~GeometryGUI_PartitionDlg()
{
    // no need to delete child widgets, Qt does it all for us
}



//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void GeometryGUI_PartitionDlg::Init( SALOME_Selection* Sel )
{
  myEditCurrentArgument = LineEditC1A1 ;	
  mySelection = Sel;
  myGeomGUI = GeometryGUI::GetGeometryGUI() ;
  myOkListShapes = myOkListTools = myOkKeepShape = myOkRemoveShape = false ;

  myGeomGUI->SetActiveDialogBox( (QDialog*)this ) ;

  /* type for sub shape selection */
//  ComboBox1->insertItem( tr( "RECONSTRUCTION_LIMIT_SHAPE"  ) );
  ComboBox1->insertItem( tr( "RECONSTRUCTION_LIMIT_SOLID"  ) );
  ComboBox1->insertItem( tr( "RECONSTRUCTION_LIMIT_SHELL"  ) );
  ComboBox1->insertItem( tr( "RECONSTRUCTION_LIMIT_FACE"   ) );
  ComboBox1->insertItem( tr( "RECONSTRUCTION_LIMIT_WIRE"   ) );
  ComboBox1->insertItem( tr( "RECONSTRUCTION_LIMIT_EDGE"   ) );
  ComboBox1->insertItem( tr( "RECONSTRUCTION_LIMIT_VERTEX" ) );

  /* Current item is 'Shape' */
  ComboBox1->setCurrentItem(0);
  myLimit = ComboBox1->currentItem();

  /* Filter definitions */
  Engines::Component_var comp = QAD_Application::getDesktop()->getEngine("FactoryServer", "Geometry");
  myGeom = GEOM::GEOM_Gen::_narrow(comp);

  /* signals and slots connections */
  connect( buttonOk,     SIGNAL( clicked() ), this, SLOT( ClickOnOk() ) );
  connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( ClickOnCancel() ) ) ;
  connect( buttonApply,  SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );
//  connect( GroupConstructors, SIGNAL(clicked(int) ), SLOT( ConstructorsClicked(int) ) );  
  connect( ComboBox1, SIGNAL( activated(int) ), this, SLOT( ComboTextChanged() ) );

  connect( SelectButtonC1A1, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) ) ;
  connect( SelectButtonC1A2, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) ) ;

  connect( SelectButtonC2A1, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) ) ;
  connect( SelectButtonC2A2, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) ) ;

  connect( LineEditC1A1, SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) ) ;
  connect( LineEditC1A2, SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) ) ;
  connect( LineEditC2A1, SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) ) ;
  connect( LineEditC2A2, SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) ) ;

  connect( myGeomGUI,   SIGNAL( SignalDeactivateActiveDialog() ), this, SLOT( DeactivateActiveDialog() ) ) ;  
  connect( mySelection, SIGNAL( currentSelectionChanged() ),      this, SLOT( SelectionIntoArgument() ) );
  /* to close dialog if study change */
  connect( myGeomGUI, SIGNAL ( SignalCloseAllDialogs() ), this, SLOT( ClickOnCancel() ) ) ;
 
  /* Move widget on the botton right corner of main widget */
  int x, y ;
  myGeomGUI->DefineDlgPosition( this, x, y ) ;
  this->move( x, y ) ;
  this->show() ; /* displays Dialog */

  ComboTextChanged();
  return ;
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void GeometryGUI_PartitionDlg::ClickOnOk()
{
  this->ClickOnApply() ;
  this->ClickOnCancel() ;

  return ;
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
void GeometryGUI_PartitionDlg::ClickOnApply()
{
  myGeomGUI->GetDesktop()->putInfo( tr("") ) ;
  if (myOkListShapes || myOkListTools || myOkKeepShape || myOkRemoveShape)
  {
    GEOM::shape_type limit;
    switch (myLimit) {
//    case 0: limit = GEOM::SOLID ; break;
    case 0: limit = GEOM::SHAPE ; break;  
    case 1: limit = GEOM::SHELL ; break;
    case 2: limit = GEOM::FACE  ; break;
    case 3: limit = GEOM::WIRE  ; break; 
    case 4: limit = GEOM::EDGE  ; break; 
    case 5: limit = GEOM::VERTEX; break; 
    default:limit = GEOM::SHAPE;
    }
    myGeomGUI->MakePartitionAndDisplay (myListShapes,
                                        myListTools,
                                        myListKeepInside,
                                        myListRemoveInside,
                                        limit);

  }
  // accept();
  return ;
}


//=================================================================================
// function : ClickOnCancel()
// purpose  :
//=================================================================================
void GeometryGUI_PartitionDlg::ClickOnCancel()
{
  myGeomGUI->EraseSimulationShape() ;
  disconnect( mySelection, 0, this, 0 );
  myGeomGUI->ResetState() ;
  reject() ;
  return ;
}


//=================================================================================
// function : ConstructorsClicked()
// purpose  :
//=================================================================================
// void GeometryGUI_PartitionDlg::ConstructorsClicked(int constructorId)
// {
// }


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void GeometryGUI_PartitionDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("") ;
  QString aString = ""; /* name of selection */
  int nbSel = myGeomGUI->GetNameOfSelectedIObjects(mySelection, aString) ;

  if ( nbSel < 1 ) {
    if      ( myEditCurrentArgument == LineEditC1A1 ) {
      myOkListShapes = false ;
    }
    else if ( myEditCurrentArgument == LineEditC1A2 ) {
      myOkListTools = false ;
    }
    else if ( myEditCurrentArgument == LineEditC2A1 ) {
      myOkListTools = false ;
    }
    else if ( myEditCurrentArgument == LineEditC2A2 ) {
      myOkListTools = false ;
    }
    return ;
  }
	
  if ( myEditCurrentArgument == LineEditC1A1  ) {
    myGeomGUI->ConvertListOfIOInListOfIOR(mySelection->StoredIObjects(), myListShapes) ;
    myEditCurrentArgument->setText(aString) ;
    myOkListShapes = true ;
  }
  else if ( myEditCurrentArgument == LineEditC1A2  ) {
    myGeomGUI->ConvertListOfIOInListOfIOR(mySelection->StoredIObjects(), myListTools)  ;
    myEditCurrentArgument->setText(aString) ;
    myOkListTools = true ;
  }
  else if ( myEditCurrentArgument == LineEditC2A1 ) {
    myGeomGUI->ConvertListOfIOInListOfIOR(mySelection->StoredIObjects(), myListRemoveInside)  ;
    myEditCurrentArgument->setText(aString) ;
    myOkKeepShape = true ;
  }
  else if ( myEditCurrentArgument == LineEditC2A2 ) {
    myGeomGUI->ConvertListOfIOInListOfIOR(mySelection->StoredIObjects(), myListKeepInside)  ;
    myEditCurrentArgument->setText(aString) ;
    myOkRemoveShape = true ;
  }
  /* no simulation */
  return ;
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void GeometryGUI_PartitionDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();
  
  if (send == SelectButtonC1A1) {
    LineEditC1A1->setFocus() ;
    myEditCurrentArgument = LineEditC1A1 ;
  }
  else if(send == SelectButtonC1A2) {
    LineEditC1A2->setFocus() ;
    myEditCurrentArgument = LineEditC1A2 ;
  }
  else if(send == SelectButtonC2A1) {
    LineEditC2A1->setFocus() ;
    myEditCurrentArgument = LineEditC2A1 ;
  }
  else if(send == SelectButtonC2A2) {
    LineEditC2A2->setFocus() ;
    myEditCurrentArgument = LineEditC2A2 ;
  }
  SelectionIntoArgument() ;
  
  return ;
}

//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void GeometryGUI_PartitionDlg::LineEditReturnPressed()
{  
  QLineEdit* send = (QLineEdit*)sender();  
  if( send == LineEditC1A1 )
    myEditCurrentArgument = LineEditC1A1 ;
  else if ( send == LineEditC1A2 )
    myEditCurrentArgument = LineEditC1A2 ; 
  else if ( send == LineEditC2A1 )
    myEditCurrentArgument = LineEditC2A1 ; 
  else if ( send == LineEditC2A2 )
    myEditCurrentArgument = LineEditC2A2 ; 
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

void GeometryGUI_PartitionDlg::DeactivateActiveDialog()
{
  if ( GroupConstructors->isEnabled() ) {
    GroupConstructors->setEnabled(false) ;
    GroupC1->setEnabled(false) ;
    GroupC2->setEnabled(false) ;
    GroupButtons->setEnabled(false) ;
  }
  return ;
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void GeometryGUI_PartitionDlg::ActivateThisDialog()
{
  /* Emit a signal to deactivate the active dialog */
  myGeomGUI->EmitSignalDeactivateDialog() ;   
  GroupConstructors->setEnabled(true) ;
  GroupC1->setEnabled(true) ;
  GroupC2->setEnabled(true) ;
  GroupButtons->setEnabled(true) ;
  return ;
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void GeometryGUI_PartitionDlg::enterEvent(QEvent* e)
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
void GeometryGUI_PartitionDlg::closeEvent( QCloseEvent* e )
{
  /* same than click on cancel button */
  this->ClickOnCancel() ;
  return ;
}

//=======================================================================
//function : ComboTextChanged
//purpose  : 
//=======================================================================

void GeometryGUI_PartitionDlg::ComboTextChanged()
{

 myLimit = ComboBox1->currentItem();
 GroupC2->setEnabled( ComboBox1->currentItem() < 3 );

 return ;
}
