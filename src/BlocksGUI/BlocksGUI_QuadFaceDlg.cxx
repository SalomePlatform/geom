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
// File   : BlocksGUI_QuadFaceDlg.cxx
// Author : Julia DOROVSKIKH, Open CASCADE S.A.S. (julia.dorovskikh@opencascade.com)
//

#include "BlocksGUI_QuadFaceDlg.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>
#include <GEOMImpl_Types.hxx>

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>

#include <qlabel.h>

//=================================================================================
// class    : BlocksGUI_QuadFaceDlg()
// purpose  : Constructs a BlocksGUI_QuadFaceDlg which is a child of 'parent'.
//=================================================================================
BlocksGUI_QuadFaceDlg::BlocksGUI_QuadFaceDlg( GeometryGUI* theGeometryGUI, QWidget* parent )
  : GEOMBase_Skeleton( theGeometryGUI, parent )
{
  SUIT_ResourceMgr* aResMgr = myGeomGUI->getApp()->resourceMgr();
  QPixmap image1( aResMgr->loadPixmap( "GEOM", tr( "ICON_DLG_QUAD_FACE_4_VERT" ) ) );
  QPixmap image2( aResMgr->loadPixmap( "GEOM", tr( "ICON_DLG_QUAD_FACE_2_EDGE" ) ) );
  QPixmap image3( aResMgr->loadPixmap( "GEOM", tr( "ICON_DLG_QUAD_FACE_4_EDGE" ) ) );
  QPixmap imageS( aResMgr->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );

  setWindowTitle( tr( "GEOM_QUAD_FACE_TITLE" ) );

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle( tr( "GEOM_QUAD_FACE" ) );

  mainFrame()->RadioButton1->setIcon( image1 );
  mainFrame()->RadioButton2->setIcon( image2 );
  mainFrame()->RadioButton3->setIcon( image3 );

  // Create first group
  myGrp1 = new QGroupBox( tr( "GEOM_ARGUMENTS" ), centralWidget() );

  createSelWg( tr( "VERTEX_1" ), imageS, myGrp1, Vertex1 );
  createSelWg( tr( "VERTEX_2" ), imageS, myGrp1, Vertex2 );
  createSelWg( tr( "VERTEX_3" ), imageS, myGrp1, Vertex3 );
  createSelWg( tr( "VERTEX_4" ), imageS, myGrp1, Vertex4 );

  // Create second group
  myGrp2 = new QGroupBox( tr( "GEOM_ARGUMENTS" ), centralWidget() );

  createSelWg( tr( "EDGE_1" ), imageS, myGrp2, Edge12 );
  createSelWg( tr( "EDGE_2" ), imageS, myGrp2, Edge22 );

  // Create fird group
  myGrp3 = new QGroupBox( tr( "GEOM_ARGUMENTS" ), centralWidget() );

  createSelWg( tr( "EDGE_1" ), imageS, myGrp3, Edge14 );
  createSelWg( tr( "EDGE_2" ), imageS, myGrp3, Edge24 );
  createSelWg( tr( "EDGE_3" ), imageS, myGrp3, Edge34 );
  createSelWg( tr( "EDGE_4" ), imageS, myGrp3, Edge44 );

  // Add groups to layout
  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( myGrp1 );
  layout->addWidget( myGrp2 );
  layout->addWidget( myGrp3 );

  /***************************************************************/

  setHelpFileName( "newentity_blocks.htm#QuadrangleFace" );

  Init();
}

//=================================================================================
// function : ~BlocksGUI_QuadFaceDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
BlocksGUI_QuadFaceDlg::~BlocksGUI_QuadFaceDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BlocksGUI_QuadFaceDlg::Init()
{
  // signals and slots connections
  connect( buttonOk(),    SIGNAL( clicked() ), this, SLOT( ClickOnOk() ) );
  connect( buttonApply(), SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );

  connect( this, SIGNAL( constructorsClicked( int ) ), 
	   this, SLOT( ConstructorsClicked( int ) ) );

  QMap<int, QPushButton*>::iterator anIterBtn;
  for ( anIterBtn = mySelBtn.begin(); anIterBtn != mySelBtn.end(); ++anIterBtn )
    connect( anIterBtn.value(), SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );

  connect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr(),
	   SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );

  // init controls and fields
  initName( tr( "GEOM_QUAD_FACE" ) );

  myConstructorId = -1;
  ConstructorsClicked( 0 );
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void BlocksGUI_QuadFaceDlg::ConstructorsClicked( int constructorId )
{
  if ( myConstructorId == constructorId )
    return;

  myConstructorId = constructorId;

  switch ( constructorId ) {
  case 0:
    myGrp2->hide();
    myGrp3->hide();
    myGrp1->show();
    myEditCurrentArgument = mySelName[Vertex1];
    break;
  case 1:
    myGrp1->hide();
    myGrp3->hide();
    myGrp2->show();
    myEditCurrentArgument = mySelName[Edge12];
    break;
  case 2:
    myGrp1->hide();
    myGrp2->hide();
    myGrp3->show();
    myEditCurrentArgument = mySelName[Edge14];
    break;
  default:
    break;
  }

  // clear line edits
  QMap<int, QLineEdit*>::iterator anIterLE;
  for ( anIterLE = mySelName.begin(); anIterLE != mySelName.end(); ++anIterLE )
    anIterLE.value()->setText( "" );

  // init fields
  myShape1 = myShape2 = GEOM::GEOM_Object::_nil();
  myShape3 = myShape4 = myShape1;

  qApp->processEvents();
  updateGeometry();
  resize( minimumSize() );

  activateSelection();
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void BlocksGUI_QuadFaceDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool BlocksGUI_QuadFaceDlg::ClickOnApply()
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
void BlocksGUI_QuadFaceDlg::SelectionIntoArgument()
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

  GEOM::GEOM_Object_var anObj;
  Standard_Boolean aResult = Standard_False;
  if ( IObjectCount() == 1 ) {
    anObj = GEOMBase::ConvertIOinGEOMObject( firstIObject(), aResult );
    if ( aResult ) {
      if (anObj->_is_nil()) {
        aResult = Standard_False;
      } 
      else {
        mySelName[aCurrFocus]->setText( GEOMBase::GetName( anObj ) );
      }
    } 
    else {
      anObj = GEOM::GEOM_Object::_nil();
    }
  }

  switch ( aCurrFocus ) {
  case Vertex1:
  case Edge12:
  case Edge14:
    myShape1 = anObj; break;
  case Vertex2:
  case Edge22:
  case Edge24:
    myShape2 = anObj; break;
  case Vertex3:
  case Edge34:
    myShape3 = anObj; break;
  case Vertex4:
  case Edge44:
    myShape4 = anObj; break;
  default:
    return;
  }
  displayPreview();
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BlocksGUI_QuadFaceDlg::SetEditCurrentArgument()
{
  QPushButton* aSender = (QPushButton*)sender();

  QMap<int, QPushButton*>::iterator anIter;
  for ( anIter = mySelBtn.begin(); anIter != mySelBtn.end(); ++anIter ) {
    if ( anIter.value() == aSender ) {
      mySelName[anIter.key()]->setFocus();
      myEditCurrentArgument = mySelName[anIter.key()];
    }
  }

  activateSelection();
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void BlocksGUI_QuadFaceDlg::ActivateThisDialog()
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
void BlocksGUI_QuadFaceDlg::enterEvent( QEvent* )
{
  if ( !mainFrame()->GroupConstructors->isEnabled() )
    this->ActivateThisDialog();
}

//=================================================================================
// function : DeactivateActiveDialog()
// purpose  :
//=================================================================================
//void BlocksGUI_QuadFaceDlg::DeactivateActiveDialog()
//{
//  // disconnect selection
//  GEOMBase_Skeleton::DeactivateActiveDialog();
//}

//=================================================================================
// function : createSelWg()
// purpose  :
//=================================================================================
void BlocksGUI_QuadFaceDlg::createSelWg( const QString& theLbl,
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
// function : activateSelection
// purpose  : Activate selection in accordance with myEditCurrentArgument
//=================================================================================
void BlocksGUI_QuadFaceDlg::activateSelection()
{
  if ( myEditCurrentArgument == mySelName[Vertex1] ||
       myEditCurrentArgument == mySelName[Vertex2] ||
       myEditCurrentArgument == mySelName[Vertex3] ||
       myEditCurrentArgument == mySelName[Vertex4] ) {
    globalSelection( GEOM_POINT );
  } 
  else {
    globalSelection( GEOM_EDGE );
  }

  SelectionIntoArgument();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr BlocksGUI_QuadFaceDlg::createOperation()
{
  return getGeomEngine()->GetIBlocksOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  : Verify validity of input data
//=================================================================================
bool BlocksGUI_QuadFaceDlg::isValid( QString& )
{
  bool ok = false;
  switch ( getConstructorId() ) {
  case 0:
    ok = ( !myShape1->_is_nil() && !myShape2->_is_nil() &&
	   !myShape3->_is_nil() && !myShape4->_is_nil() ); 
    break;
  case 1:
    ok = ( !myShape1->_is_nil() && !myShape2->_is_nil() );
    break;
  case 2:
    ok = ( !myShape1->_is_nil() && !myShape2->_is_nil() &&
	   !myShape3->_is_nil() && !myShape4->_is_nil() );
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
bool BlocksGUI_QuadFaceDlg::execute( ObjectList& objects )
{
  bool res = false;

  GEOM::GEOM_Object_var anObj;

  switch ( getConstructorId() ) {
  case 0:
    anObj = GEOM::GEOM_IBlocksOperations::_narrow( getOperation() )->MakeQuad4Vertices
      ( myShape1, myShape2, myShape3, myShape4 );
    res = true;
    break;
  case 1:
    anObj = GEOM::GEOM_IBlocksOperations::_narrow( getOperation() )->MakeQuad2Edges
      ( myShape1, myShape2 );
    res = true;
    break;
  case 2:
    anObj = GEOM::GEOM_IBlocksOperations::_narrow( getOperation() )->MakeQuad
      ( myShape1, myShape2, myShape3, myShape4 );
    res = true;
    break;
  default:
    break;
  }

  if ( !anObj->_is_nil() )
    objects.push_back( anObj._retn() );
  
  return res;
}
