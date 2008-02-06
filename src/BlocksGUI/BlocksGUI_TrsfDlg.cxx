// GEOM GEOMGUI : GUI for Geometry component
//
// Copyright (C) 2003  CEA
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
// File   : BlocksGUI_TrsfDlg.cxx
// Author : Julia DOROVSKIKH, Open CASCADE S.A.S. (julia.dorovskikh@opencascade.com)
//

#include "BlocksGUI_TrsfDlg.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <SUIT_Session.h>
#include <SUIT_Desktop.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_ViewWindow.h>
#include <SUIT_ViewManager.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>
#include <OCCViewer_ViewModel.h>

#include <TColStd_IndexedMapOfInteger.hxx>

//=================================================================================
// class    : BlocksGUI_TrsfDlg()
// purpose  : Constructs a BlocksGUI_TrsfDlg which is a child of 'parent'.
//=================================================================================
BlocksGUI_TrsfDlg::BlocksGUI_TrsfDlg( GeometryGUI* theGeometryGUI, QWidget* parent )
  : GEOMBase_Skeleton( theGeometryGUI, parent )
{
  SUIT_ResourceMgr* aResMgr = myGeomGUI->getApp()->resourceMgr();
  QPixmap image1( aResMgr->loadPixmap( "GEOM", tr( "ICON_DLG_BLOCK_MULTITRSF_SIMPLE" ) ) );
  QPixmap image2( aResMgr->loadPixmap( "GEOM", tr( "ICON_DLG_BLOCK_MULTITRSF_DOUBLE" ) ) );
  QPixmap imageS( aResMgr->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );

  setWindowTitle( tr( "GEOM_BLOCK_MULTITRSF_TITLE" ) );

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle( tr( "GEOM_BLOCK_MULTITRSF" ) );

  mainFrame()->RadioButton1->setIcon( image1 );
  mainFrame()->RadioButton2->setIcon( image2 );
  mainFrame()->RadioButton3->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton3->close();

  // Create first group
  myGrp1 = new QGroupBox( tr( "GEOM_BLOCK_MULTITRSF_SIMPLE" ), centralWidget() );

  createSelWg(  tr( "GEOM_MAIN_OBJECT" ), imageS, myGrp1, MainObj1 );
  createSelWg(  tr( "FACE_1" ),           imageS, myGrp1, Face1 );
  createSelWg(  tr( "FACE_2" ),           imageS, myGrp1, Face2 );
  createSpinWg( tr( "GEOM_NB_TIMES" ),            myGrp1, SpinBox1 );

  // Create second group
  myGrp2 = new QGroupBox( tr( "GEOM_BLOCK_MULTITRSF_DOUBLE" ), centralWidget() );

  createSelWg(  tr( "GEOM_MAIN_OBJECT"), imageS, myGrp2, MainObj2 );
  createSelWg(  tr( "FACE_1U"),          imageS, myGrp2, Face1U );
  createSelWg(  tr( "FACE_2U"),          imageS, myGrp2, Face2U );
  createSpinWg( tr( "GEOM_NB_TIMES_U" ),         myGrp2, SpinBox2U );
  createSelWg(  tr( "FACE_1V" ),         imageS, myGrp2, Face1V );
  createSelWg(  tr( "FACE_2V" ),         imageS, myGrp2, Face2V );
  createSpinWg( tr( "GEOM_NB_TIMES_V" ),         myGrp2, SpinBox2V );

  // Add groups to layout
  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( myGrp1 );
  layout->addWidget( myGrp2 );
  /***************************************************************/

  setHelpFileName( "multi_transformation_operation_page.html" );

  Init();
}

//=================================================================================
// function : ~BlocksGUI_TrsfDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
BlocksGUI_TrsfDlg::~BlocksGUI_TrsfDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BlocksGUI_TrsfDlg::Init()
{
  // Set range of spinboxes
  double SpecificStep = 1.0;
  QMap<int, QDoubleSpinBox*>::iterator anIter;
  for ( anIter = mySpinBox.begin(); anIter != mySpinBox.end(); ++anIter ) {
    //anIter.data()->RangeStepAndValidator(1.0, 999.999, SpecificStep, 3);
    initSpinBox( anIter.value(), 1.0, MAX_NUMBER, SpecificStep, 3 );
  }

  // signals and slots connections
  connect( buttonOk(),    SIGNAL( clicked() ), this, SLOT( ClickOnOk() ) );
  connect( buttonApply(), SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );

  connect( this, SIGNAL( constructorsClicked( int ) ), 
	   this, SLOT( ConstructorsClicked( int ) ) );

  QMap<int, QPushButton*>::iterator anIterBtn;
  for ( anIterBtn = mySelBtn.begin(); anIterBtn != mySelBtn.end(); ++anIterBtn )
    connect( anIterBtn.value(), SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );

  QMap<int, QDoubleSpinBox*>::iterator anIterSpin;
  for ( anIterSpin = mySpinBox.begin(); anIterSpin != mySpinBox.end(); ++anIterSpin )
    connect( anIterSpin.value(), SIGNAL( valueChanged( double ) ), this, SLOT( ValueChangedInSpinBox( double ) ) );

  connect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr(), 
	   SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) ) ;

  // init controls and fields
  initName( tr( "GEOM_BLOCK_MULTITRSF" ) );

  myConstructorId = -1;
  ConstructorsClicked( 0 );
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void BlocksGUI_TrsfDlg::ConstructorsClicked( int constructorId )
{
  if ( myConstructorId == constructorId )
    return;

  myConstructorId = constructorId;

  switch ( constructorId ) {
  case 0:
    myGrp2->hide();
    myGrp1->show();
    mySpinBox[SpinBox1]->setValue( 2.0 );
    myEditCurrentArgument = mySelName[MainObj1];
    myFaces[Face1] = -1;
    myFaces[Face2] = -1;
    break;
  case 1:
    myGrp1->hide();
    myGrp2->show();
    mySpinBox[SpinBox2U]->setValue( 2.0 );
    mySpinBox[SpinBox2V]->setValue( 2.0 );
    myEditCurrentArgument = mySelName[MainObj2];
    myFaces[Face1U] = -1;
    myFaces[Face2U] = -1;
    myFaces[Face1V] = -1;
    myFaces[Face2V] = -1;
    break;
  default:
    break;
  }

  // clear line edits
  QMap<int, QLineEdit*>::iterator anIterLE;
  for ( anIterLE = mySelName.begin(); anIterLE != mySelName.end(); ++anIterLE )
    anIterLE.value()->setText( "" );

  // init fields
  myShape = GEOM::GEOM_Object::_nil();

  qApp->processEvents();
  updateGeometry();
  resize( minimumSize() );

  activateSelection();
//  enableWidgets();
//  displayPreview();
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void BlocksGUI_TrsfDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool BlocksGUI_TrsfDlg::ClickOnApply()
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
void BlocksGUI_TrsfDlg::SelectionIntoArgument()
{
  erasePreview();
  myEditCurrentArgument->setText( "" );

  // Get index of current selection focus
  int aCurrFocus = -1;
  QMap<int, QLineEdit*>::iterator anIter;
  for ( anIter = mySelName.begin(); anIter != mySelName.end(); ++anIter ) {
    if ( myEditCurrentArgument == anIter.value() ) {
      aCurrFocus = anIter.key();
      break;
    }
  }

  // If selection of main object is activated
  if ( aCurrFocus == MainObj1 || aCurrFocus == MainObj2 ) {
    if ( IObjectCount() == 1 ) {
      Standard_Boolean aResult = Standard_False;
      GEOM::GEOM_Object_var anObj =
        GEOMBase::ConvertIOinGEOMObject( firstIObject(), aResult );

      if ( aResult && !anObj->_is_nil() && GEOMBase::IsShape( anObj ) ) {
        myShape = anObj;
        mySelName[aCurrFocus]->setText( GEOMBase::GetName( anObj ) );
        enableWidgets();
        return;
      }
    }

    myShape = GEOM::GEOM_Object::_nil();
    enableWidgets();
  }
  // If face selection is activated
  else if ( aCurrFocus == Face1  || aCurrFocus == Face2  ||
	    aCurrFocus == Face1U || aCurrFocus == Face2U ||
	    aCurrFocus == Face1V || aCurrFocus == Face2V ) {
    if ( IObjectCount() == 1 ) {
      Standard_Boolean aResult = Standard_False;
      GEOM::GEOM_Object_var anObj =
        GEOMBase::ConvertIOinGEOMObject( firstIObject(), aResult );

      if ( aResult && !anObj->_is_nil() && GEOMBase::IsShape( anObj ) ) {
        TColStd_IndexedMapOfInteger anIndexes;
	( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr()->GetIndexes( firstIObject(), anIndexes );

        if ( anIndexes.Extent() == 1 ) {
          int anIndex = anIndexes( 1 );
          QString aFaceName = QString( GEOMBase::GetName( anObj ) ) + ":%1";
          myEditCurrentArgument->setText( aFaceName.arg( anIndex ) );
          myFaces[aCurrFocus] = anIndex;
          displayPreview();
          return;
        }
      }
    }

    myFaces[aCurrFocus] = -1;
  }
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BlocksGUI_TrsfDlg::SetEditCurrentArgument()
{
  QPushButton* aSender = (QPushButton*)sender();

  QMap<int, QPushButton*>::iterator anIter;
  for ( anIter = mySelBtn.begin(); anIter != mySelBtn.end(); ++anIter ) {
    if ( anIter.value() == aSender ) {
      mySelName[anIter.key()]->setFocus();
      myEditCurrentArgument = mySelName[anIter.key()];
      break;
    }
  }

  activateSelection();
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void BlocksGUI_TrsfDlg::ActivateThisDialog()
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
void BlocksGUI_TrsfDlg::enterEvent( QEvent* )
{
  if ( !mainFrame()->GroupConstructors->isEnabled() )
    this->ActivateThisDialog();
}

//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void BlocksGUI_TrsfDlg::ValueChangedInSpinBox( double )
{
  displayPreview();
}

//=================================================================================
// function : createSelWg()
// purpose  :
//=================================================================================
void BlocksGUI_TrsfDlg::createSelWg( const QString& theLbl,
                                     QPixmap&       thePix,
                                     QWidget*       theParent,
                                     const int      theId )
{
  QLabel* lab = new QLabel( theLbl, theParent );
  mySelBtn[theId] = new QPushButton( theParent );
  mySelBtn[theId]->setIcon( thePix );
  mySelBtn[theId]->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ) );
  mySelName[theId] = new QLineEdit( theParent );
  mySelName[theId]->setReadOnly( true );
  QGridLayout* l = 0;
  if ( !theParent->layout() ) {
    l = new QGridLayout( theParent );
    l->setMargin( 9 ); l->setSpacing( 6 );
  }
  else {
    l = qobject_cast<QGridLayout*>( theParent->layout() );
  }
  int row = l->rowCount();
  l->addWidget( lab,              row, 0 );
  l->addWidget( mySelBtn[theId],  row, 1 );
  l->addWidget( mySelName[theId], row, 2 );
}

//=================================================================================
// function : createSpinWg()
// purpose  :
//=================================================================================
void BlocksGUI_TrsfDlg::createSpinWg( const QString& theLbl,
				      QWidget*       theParent,
				      const int      theId )
{
  QLabel* lab = new QLabel( theLbl, theParent );
  mySpinBox[theId] = new QDoubleSpinBox( theParent );
  QGridLayout* l = 0;
  if ( !theParent->layout() ) {
    l = new QGridLayout( theParent );
    l->setMargin( 9 ); l->setSpacing( 6 );
  }
  else {
    l = qobject_cast<QGridLayout*>( theParent->layout() );
  }
  int row = l->rowCount();
  l->addWidget( lab,              row, 0 );
  l->addWidget( mySpinBox[theId], row, 2 );
}

//=================================================================================
// function : activateSelection
// purpose  : Activate selection in accordance with myEditCurrentArgument
//=================================================================================
void BlocksGUI_TrsfDlg::activateSelection()
{
  if ( !myShape->_is_nil() &&
       ( myEditCurrentArgument == mySelName[ Face1  ] ||
	 myEditCurrentArgument == mySelName[ Face2  ] ||
	 myEditCurrentArgument == mySelName[ Face1U ] ||
	 myEditCurrentArgument == mySelName[ Face2U ] ||
	 myEditCurrentArgument == mySelName[ Face1V ] ||
	 myEditCurrentArgument == mySelName[ Face2V ] ) ) {

    // Local selection is available only in the OCC Viewer
    if ( SUIT_Session::session()->activeApplication()->desktop()->activeWindow()->getViewManager()->getType() 
	 == OCCViewer_Viewer::Type() ) {
      localSelection( myShape, TopAbs_FACE );
    } 
    else {
      return;
    }
  } 
  else {
    globalSelection( GEOM_ALLSHAPES );
  }

  SelectionIntoArgument();
}

//=================================================================================
// function : enableWidgets
// purpose  : Enable widgets of faces in accordance with value of main object
//=================================================================================
void BlocksGUI_TrsfDlg::enableWidgets()
{
  int anId = getConstructorId();

  bool toEnable = !myShape->_is_nil();

  if ( anId == 0 ) {
    mySelName[Face1]->setEnabled( toEnable );
    mySelName[Face2]->setEnabled( toEnable );
    mySelBtn[Face1]->setEnabled( toEnable );
    mySelBtn[Face2]->setEnabled( toEnable );

    if ( !toEnable)  {
      mySelName[Face1]->setText( "" );
      mySelName[Face2]->setText( "" );
      myFaces[Face1] = -1;
      myFaces[Face2] = -1;
    }
  }
  else if ( anId == 1 ) {
    mySelName[Face1U]->setEnabled( toEnable );
    mySelName[Face2U]->setEnabled( toEnable );
    mySelName[Face1V]->setEnabled( toEnable );
    mySelName[Face2V]->setEnabled( toEnable );
    mySelBtn[Face1U]->setEnabled( toEnable );
    mySelBtn[Face2U]->setEnabled( toEnable );
    mySelBtn[Face1V]->setEnabled( toEnable );
    mySelBtn[Face2V]->setEnabled( toEnable );

    if ( !toEnable ) {
      mySelName[Face1U]->setText( "" );
      mySelName[Face2U]->setText( "" );
      mySelName[Face1V]->setText( "" );
      mySelName[Face2V]->setText( "" );
      myFaces[Face1U] = -1;
      myFaces[Face2U] = -1;
      myFaces[Face1V] = -1;
      myFaces[Face2V] = -1;
    }
  }
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr BlocksGUI_TrsfDlg::createOperation()
{
  return getGeomEngine()->GetIBlocksOperations( getStudyId() );
}

//=================================================================================
// function : ClickOnApply()
// purpose  : Verify validity of input data
//=================================================================================
bool BlocksGUI_TrsfDlg::isValid( QString& )
{
  bool ok = false;
  switch ( getConstructorId() ) {
  case 0:
    ok = !myShape->_is_nil() && myFaces[Face1] > 0;
    break;
  case 1:
    ok = !myShape->_is_nil() && myFaces[Face1U] > 0 && myFaces[Face1V] > 0;
    break;
  default:
    break;
  }
  return ok;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool BlocksGUI_TrsfDlg::execute( ObjectList& objects )
{
  bool res = false;

  GEOM::GEOM_Object_var anObj;

  switch ( getConstructorId() ) {
  case 0:
    anObj = GEOM::GEOM_IBlocksOperations::_narrow( getOperation() )->MakeMultiTransformation1D
      ( myShape,
	myFaces[Face1],
	myFaces[Face2],
	(int)mySpinBox[SpinBox1]->value() );
    res = true;
    break;
  case 1:
    anObj = GEOM::GEOM_IBlocksOperations::_narrow( getOperation() )->MakeMultiTransformation2D
      ( myShape,
	myFaces[Face1U],
	myFaces[Face2U],
	(int)mySpinBox[SpinBox2U]->value(),
	myFaces[Face1V],
	myFaces[Face2V],
	(int)mySpinBox[SpinBox2V]->value() );
    res = true;
    break;
  }

  if ( !anObj->_is_nil() )
    objects.push_back( anObj._retn() );

  return res;
}
