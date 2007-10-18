// GEOM GEOMGUI : GUI for Geometry component
//
// Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
// 
// This library is free software; you can redistribute it and/or 
// modify it under the terms of the GNU Lesser General Public 
// License as published by the Free Software Foundation; either 
// version 2.1 of the License. 
// 
// This library is distributed in the hope that it will be useful, 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
// Lesser General Public License for more details. 
// 
// You should have received a copy of the GNU Lesser General Public 
// License along with this library; if not, write to the Free Software 
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
// 
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// File   : OperationGUI_ChamferDlg.cxx
// Author : Damien COQUERET, Open CASCADE S.A.S.
//

#include "OperationGUI_ChamferDlg.h"

#include <GEOM_DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <SUIT_Desktop.h>
#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_ViewWindow.h>
#include <SUIT_ViewManager.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>
#include <OCCViewer_ViewModel.h>

#include <TColStd_MapOfInteger.hxx>

#include <GEOMImpl_Types.hxx>

//=================================================================================
// class    : OperationGUI_ChamferDlg()
// purpose  : Constructs a OperationGUI_ChamferDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
OperationGUI_ChamferDlg::OperationGUI_ChamferDlg( GeometryGUI* theGeometryGUI, QWidget* parent )
  : GEOMBase_Skeleton( theGeometryGUI, parent, false )
{
  QPixmap image1( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_CHAMFER_ALL" ) ) );
  QPixmap image2( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_CHAMFER_EDGE" ) ) );
  QPixmap image3( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_CHAMFER_FACE" ) ) );
  QPixmap iconSelect( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );

  setWindowTitle( tr( "GEOM_CHAMFER_TITLE" ) );

  mainFrame()->GroupConstructors->setTitle( tr( "GEOM_CHAMFER" ) );

  mainFrame()->RadioButton1->setIcon( image1 );
  mainFrame()->RadioButton2->setIcon( image2 );
  mainFrame()->RadioButton3->setIcon( image3 );

  // Create first group

  myGrp1 = new QGroupBox( tr( "GEOM_CHAMFER_ALL" ), centralWidget() );

  QGridLayout* aLayout = new QGridLayout( myGrp1 );
  aLayout->setMargin( 9 ); aLayout->setSpacing( 6 );

  createSelWg( tr( "GEOM_MAIN_OBJECT" ), iconSelect, myGrp1, aLayout, MainObj1 );

  int row = aLayout->rowCount();
  aLayout->addWidget( new QLabel( tr( "D" ), myGrp1 ), row, 0 );
  aLayout->addWidget( ( mySpinBox[ SpinBox1 ] = new QDoubleSpinBox( myGrp1 ) ), row++, 2 );
  aLayout->setRowStretch( row, 10 );

  // Create second group

  myGrp2 = new QGroupBox( tr( "GEOM_CHAMFER_EDGES" ), centralWidget() );

  aLayout = new QGridLayout( myGrp2 );
  aLayout->setMargin( 9 ); aLayout->setSpacing( 6 );

  createSelWg( tr( "GEOM_MAIN_OBJECT" ), iconSelect, myGrp2, aLayout, MainObj2 );
  createSelWg( tr( "FACE_1" ),           iconSelect, myGrp2, aLayout, Face1 );
  createSelWg( tr( "FACE_2" ),           iconSelect, myGrp2, aLayout, Face2 );

  row = aLayout->rowCount();

  aLayout->addWidget( new QLabel( tr( "GEOM_D1" ), myGrp2 ), row, 0 );
  aLayout->addWidget( ( mySpinBox[ SpinBox21 ] = new QDoubleSpinBox( myGrp2 ) ), row++, 2 );
  aLayout->addWidget( new QLabel( tr( "GEOM_D2" ), myGrp2 ), row, 0 );
  aLayout->addWidget( ( mySpinBox[ SpinBox22 ] = new QDoubleSpinBox( myGrp2 ) ), row++, 2 );
  aLayout->setRowStretch( row, 10 );

  // Create third group

  myGrp3 = new QGroupBox( tr( "GEOM_CHAMFER_FACES" ), centralWidget() );

  aLayout = new QGridLayout( myGrp3 );
  aLayout->setMargin( 9 ); aLayout->setSpacing( 6 );

  createSelWg( tr( "GEOM_MAIN_OBJECT" ), iconSelect, myGrp3, aLayout, MainObj3 );
  createSelWg( tr( "SELECTED_FACES" ),   iconSelect, myGrp3, aLayout, Faces );

  row = aLayout->rowCount();

  aLayout->addWidget( new QLabel( tr( "GEOM_D1" ), myGrp3 ), row, 0 );
  aLayout->addWidget( ( mySpinBox[ SpinBox31 ] = new QDoubleSpinBox( myGrp3 ) ), row++, 2 );
  aLayout->addWidget( new QLabel( tr( "GEOM_D2" ), myGrp3 ), row, 0 );
  aLayout->addWidget( ( mySpinBox[ SpinBox32 ] = new QDoubleSpinBox( myGrp3 ) ), row++, 2 );
  aLayout->setRowStretch( row, 10 );

  // Add groups to layout

  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( myGrp1 );
  layout->addWidget( myGrp2 );
  layout->addWidget( myGrp3 );

  // Set range of spinboxes

  double SpecificStep = 10.0;
  QMap< int, QDoubleSpinBox* >::iterator anIter;
  for ( anIter = mySpinBox.begin(); anIter != mySpinBox.end(); ++anIter )
    initSpinBox( anIter.value(), 0.001, COORD_MAX, SpecificStep, 3 );

  setHelpFileName( "chamfer.htm" );

  /* Initialisations */
  Init();
}


//=================================================================================
// function : ~OperationGUI_ChamferDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
OperationGUI_ChamferDlg::~OperationGUI_ChamferDlg()
{
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void OperationGUI_ChamferDlg::Init()
{
  myConstructorId = -1;
  reset();

  /* signals and slots connections */

  // main buttons
  connect( buttonOk(),    SIGNAL( clicked() ), this, SLOT( ClickOnOk()    ) );
  connect( buttonApply(), SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );

  // group box
  connect( this,          SIGNAL( constructorsClicked( int ) ), this, SLOT( ConstructorsClicked( int ) ) );

  // push buttons
  QMap< int, QPushButton* >::iterator anIterBtn;
  for ( anIterBtn = mySelBtn.begin(); anIterBtn != mySelBtn.end(); ++anIterBtn )
    connect( anIterBtn.value(), SIGNAL( clicked() ),
             this, SLOT( SetEditCurrentArgument() ) );

  // line edits
  QMap< int, QLineEdit* >::iterator anIterLE2;
  for ( anIterLE2 = mySelName.begin(); anIterLE2 != mySelName.end(); ++anIterLE2 )
    connect( anIterLE2.value(), SIGNAL( returnPressed() ),
             this, SLOT( LineEditReturnPressed() ) );

  // spin boxes
  QMap< int, QDoubleSpinBox* >::iterator anIterSpin;
  for ( anIterSpin = mySpinBox.begin(); anIterSpin != mySpinBox.end(); ++anIterSpin )
    connect( anIterSpin.value(), SIGNAL( valueChanged( double ) ),
             this, SLOT( ValueChangedInSpinBox( double ) ) );

  // selection
  connect( myGeomGUI->getApp()->selectionMgr(), 
	   SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );

  initName( tr( "GEOM_CHAMFER" ) );

  ConstructorsClicked( 0 );
}


//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void OperationGUI_ChamferDlg::ConstructorsClicked( int constructorId )
{
   // Activate next widget
  if ( myGeomGUI->getApp()->desktop()->activeWindow()->getViewManager()->getType() 
       != OCCViewer_Viewer::Type() ) {
    mainFrame()->RadioButton1->setChecked( true );
    return;
  }

  if ( myConstructorId == constructorId )
    return;

  // Get values from previous widget
  double D1 = 5, D2 = 5;
  if ( myConstructorId == 0 )
    D1 = D2 = mySpinBox[ SpinBox1 ]->value();
  else if ( myConstructorId == 1 ) {
    D1 = mySpinBox[ SpinBox21 ]->value();
    D2 = mySpinBox[ SpinBox22 ]->value();
  }
  else if ( myConstructorId == 2 ) {
    D1 = mySpinBox[ SpinBox31 ]->value();
    D2 = mySpinBox[ SpinBox32 ]->value();
  }

  myConstructorId = constructorId;

  switch ( constructorId ) {
  case 0:
    myGrp2->hide();
    myGrp3->hide();
    myGrp1->show();
    mySpinBox[ SpinBox1 ]->setValue( D1 );
    break;
  case 1:
    myGrp1->hide();
    myGrp3->hide();
    myGrp2->show();
    mySpinBox[ SpinBox21 ]->setValue( D1 );
    mySpinBox[ SpinBox22 ]->setValue( D2 );
    break;
  case 2:
    myGrp1->hide();
    myGrp2->hide();
    myGrp3->show();
    mySpinBox[ SpinBox31 ]->setValue( D1 );
    mySpinBox[ SpinBox32 ]->setValue( D2 );        
    break;
  default:
    break;
  }
  
  if      ( constructorId == 0 ) myEditCurrentArgument = mySelName[ MainObj1 ];
  else if ( constructorId == 1 ) myEditCurrentArgument = mySelName[ MainObj2 ];
  else                           myEditCurrentArgument = mySelName[ MainObj3 ];

  activateSelection(); 
  enableWidgets();
  
  if ( !myShape->_is_nil() )
  {
    myEditCurrentArgument->setText( GEOMBase::GetName( myShape ) );
    GEOMBase_Skeleton::LineEditReturnPressed();
  }
  else
    myEditCurrentArgument->setText( "" );

  qApp->processEvents();
  updateGeometry();
  resize( minimumSize() );

  displayPreview();
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void OperationGUI_ChamferDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool OperationGUI_ChamferDlg::ClickOnApply()
{
  if ( !onAccept() )
    return false;

  initName();
  return true;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection has changed
//=================================================================================
void OperationGUI_ChamferDlg::SelectionIntoArgument()
{
  erasePreview();
  myEditCurrentArgument->setText( "" );

  // Get index of current selection focus
  int aCurrFocus = -1;
  QMap< int, QLineEdit* >::iterator anIter;
  for ( anIter = mySelName.begin(); anIter != mySelName.end(); ++anIter ) {
    if ( myEditCurrentArgument == anIter.value() ) {
      aCurrFocus = anIter.key();
      break;
    }
  }

  // If selection of main object is activated
  if ( aCurrFocus == MainObj1 || aCurrFocus == MainObj2 || aCurrFocus == MainObj3 ) {
    if ( IObjectCount() == 1 ) {
      Standard_Boolean aResult = Standard_False;
      GEOM::GEOM_Object_var anObj =
        GEOMBase::ConvertIOinGEOMObject( firstIObject(), aResult );

      if ( aResult && !anObj->_is_nil() ) {
        myShape = anObj;
        mySelName[ aCurrFocus ]->setText( GEOMBase::GetName( anObj ) );
        displayPreview();
        enableWidgets();
        return;
      }
    }

    myShape = GEOM::GEOM_Object::_nil();
    enableWidgets();
  }
  // If face selection of second tab is activated
  else if ( aCurrFocus == Face1 || aCurrFocus == Face2 ) {
    if ( IObjectCount() == 1 ) {
      Standard_Boolean aResult = Standard_False;
      GEOM::GEOM_Object_var anObj =
        GEOMBase::ConvertIOinGEOMObject( firstIObject(), aResult );

      if ( aResult && !anObj->_is_nil() ) {
         TColStd_IndexedMapOfInteger anIndexes;
	 ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr()->GetIndexes( firstIObject(), anIndexes );

         if ( anIndexes.Extent() == 1 ) {
            int anIndex = anIndexes( 1 );
            QString aFaceName = QString( GEOMBase::GetName( anObj ) ) + ":%1";
            myEditCurrentArgument->setText( aFaceName.arg( anIndex ) );
            myFace[ aCurrFocus ] = anIndex;
            displayPreview();
            return;
         }
      }
    }

    myFace[ aCurrFocus ] = -1;
  }
  // If face selection of third tab is activated
  else if ( aCurrFocus == Faces ) {
    if ( IObjectCount() == 1 ) {
      Standard_Boolean aResult = Standard_False;
      GEOM::GEOM_Object_var anObj =
        GEOMBase::ConvertIOinGEOMObject( firstIObject(), aResult );
      
      if ( aResult && !anObj->_is_nil() ) {
	TColStd_IndexedMapOfInteger anIndexes;
	( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr()->GetIndexes( firstIObject(), anIndexes );
	
	if ( anIndexes.Extent() > 0 ) {
	  QString aFaceName;
	  if ( anIndexes.Extent() == 1 ) {
	    int anIndex = anIndexes( 1 );
	    
	    aFaceName = QString( GEOMBase::GetName( anObj ) ) + QString( ":%1" ).arg( anIndex );
	  }
	  else {
	    aFaceName = tr( "GEOM_MEN_POPUP_NAME" ).arg( anIndexes.Extent() );
	  }
	  
	  myEditCurrentArgument->setText( aFaceName );
	  myFaces = anIndexes;
	  displayPreview();
	  return;
	}
      }
    }
    myFaces.Clear();
  }
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void OperationGUI_ChamferDlg::LineEditReturnPressed()
{
  QLineEdit* aSender = ( QLineEdit* )sender();

  QMap< int, QLineEdit* >::iterator anIterLE;
  for ( anIterLE = mySelName.begin(); anIterLE != mySelName.end(); ++anIterLE )
    if ( anIterLE.value() == aSender )
      myEditCurrentArgument = anIterLE.value();

  GEOMBase_Skeleton::LineEditReturnPressed();
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void OperationGUI_ChamferDlg::SetEditCurrentArgument()
{
  QPushButton* aSender = ( QPushButton* )sender();

  QMap< int, QPushButton* >::iterator anIter;
  for ( anIter = mySelBtn.begin(); anIter != mySelBtn.end(); ++anIter ) {
    if ( anIter.value() == aSender ) {
      mySelName[ anIter.key() ]->setFocus();
      myEditCurrentArgument = mySelName[ anIter.key() ];
    }
  }

  activateSelection();
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void OperationGUI_ChamferDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();

  connect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr(), 
	   SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );

  activateSelection();
  displayPreview();
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void OperationGUI_ChamferDlg::enterEvent( QEvent* )
{
  if ( !mainFrame()->GroupConstructors->isEnabled() )
    this->ActivateThisDialog();
}


//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void OperationGUI_ChamferDlg::ValueChangedInSpinBox( double )
{
  displayPreview();
}


//=================================================================================
// function : createSelWg()
// purpose  :
//=================================================================================
void OperationGUI_ChamferDlg::createSelWg( const QString& theLbl,
                                           QPixmap&       thePix,
                                           QWidget*       theParent,
					   QGridLayout*   theLayout,
                                           const int      theId )
{
  QLabel* lab = new QLabel( theLbl, theParent );
  mySelBtn[ theId ] = new QPushButton( theParent );
  mySelBtn[ theId ]->setIcon( thePix );
  mySelName[ theId ] = new QLineEdit( theParent );
  mySelName[ theId ]->setReadOnly( true );
  int row = theLayout->rowCount();
  theLayout->addWidget( lab,                row, 0 );
  theLayout->addWidget( mySelBtn[ theId ],  row, 1 );
  theLayout->addWidget( mySelName[ theId ], row, 2 );
}

//=================================================================================
// function : reset()
// purpose  :
//=================================================================================
void OperationGUI_ChamferDlg::reset()
{
  // Set Initial values of spinboxes
  QMap< int, QDoubleSpinBox* >::iterator anIter;
  for ( anIter = mySpinBox.begin(); anIter != mySpinBox.end(); ++anIter )
    anIter.value()->setValue( 5 );

  // clear line edits
  QMap< int, QLineEdit* >::iterator anIterLE;
  for ( anIterLE = mySelName.begin(); anIterLE != mySelName.end(); ++anIterLE )
    anIterLE.value()->setText( "" );

  // constructor id
  int aConstructorId = getConstructorId();

  if      ( aConstructorId == 0 ) myEditCurrentArgument = mySelName[ MainObj1 ];
  else if ( aConstructorId == 1 ) myEditCurrentArgument = mySelName[ MainObj2 ];
  else                            myEditCurrentArgument = mySelName[ MainObj3 ];

  myShape = GEOM::GEOM_Object::_nil();

  myFaces.Clear();
  myFace[ Face1 ] = -1;
  myFace[ Face2 ] = -1;

  erasePreview( true );

  activateSelection();

  enableWidgets();
}

//=================================================================================
// function : activateSelection
// purpose  : Activate selection in accordance with myEditCurrentArgument
//=================================================================================
void OperationGUI_ChamferDlg::activateSelection()
{
  if (  !myShape->_is_nil() &&
       ( myEditCurrentArgument == mySelName[ Face1 ] ||
         myEditCurrentArgument == mySelName[ Face2 ] ||
         myEditCurrentArgument == mySelName[ Faces ] ) )
    localSelection( myShape, TopAbs_FACE );
  else
  {
    TColStd_MapOfInteger aMap;
    aMap.Add( GEOM_SHELL );
    aMap.Add( GEOM_SOLID );
    aMap.Add( GEOM_COMPOUND );
    globalSelection( aMap );
  }

  SelectionIntoArgument();
}

//=================================================================================
// function : enableWidgets
// purpose  : Enable widgets of faces in accordance with value of main object
//=================================================================================
void OperationGUI_ChamferDlg::enableWidgets()
{
  int anId = getConstructorId();

  bool toEnable = !myShape->_is_nil();
  
  if ( anId == 1 )
  {
    mySelName[ Face1 ]->setEnabled( toEnable );
    mySelName[ Face2 ]->setEnabled( toEnable );
    mySelBtn[ Face1 ]->setEnabled( toEnable );
    mySelBtn[ Face2 ]->setEnabled( toEnable );

    if ( !toEnable )
    {
      mySelName[ Face1 ]->setText( "" );
      mySelName[ Face2 ]->setText( "" );
      myFace[ Face1 ] = -1;
      myFace[ Face2 ] = -1;
    }
  }
  else if ( anId == 2 )
  {
    mySelName[ Faces ]->setEnabled( toEnable );

    if ( !toEnable )
      myFaces.Clear();
  }
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr OperationGUI_ChamferDlg::createOperation()
{
  return getGeomEngine()->GetILocalOperations( getStudyId() );
}

//=================================================================================
// function : ClickOnApply()
// purpose  : Verify validity of input data
//=================================================================================
bool OperationGUI_ChamferDlg::isValid( QString& )
{
  switch ( getConstructorId() )
  {
    case 0: return !myShape->_is_nil();
    case 1: return !myShape->_is_nil() && myFace[ Face1 ] > 0 && myFace[ Face2 ] > 0;
    case 2: return !myShape->_is_nil() && myFaces.Extent() > 0;
    default: return false;
  }
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool OperationGUI_ChamferDlg::execute( ObjectList& objects )
{
  GEOM::GEOM_Object_var anObj;

  int anId = getConstructorId();
  if ( anId == 0 )
    anObj = GEOM::GEOM_ILocalOperations::_narrow(
      getOperation() )->MakeChamferAll( myShape,
                                        mySpinBox[ SpinBox1 ]->value() );
  else if ( anId == 1 )
    anObj = GEOM::GEOM_ILocalOperations::_narrow(
      getOperation() )->MakeChamferEdge( myShape,
                                         mySpinBox[ SpinBox21 ]->value(),
                                         mySpinBox[ SpinBox22 ]->value(),
                                         myFace[ Face1 ],
                                         myFace[ Face2 ] );
  else if ( anId == 2 )
  {
    GEOM::ListOfLong_var anArray = new GEOM::ListOfLong;
    anArray->length( myFaces.Extent() );


    for ( int i = 1, n = myFaces.Extent(); i <= n; i++ )
      anArray[ i - 1 ] = myFaces( i );             
    
    anObj = GEOM::GEOM_ILocalOperations::_narrow(
      getOperation() )->MakeChamferFaces( myShape,
                                          mySpinBox[ SpinBox31 ]->value(),
                                          mySpinBox[ SpinBox32 ]->value(),
                                          anArray );
  }


  if ( !anObj->_is_nil() )
    objects.push_back( anObj._retn() );

  return true;
}









