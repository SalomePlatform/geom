// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
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

// GEOM GEOMGUI : GUI for Geometry component
// File   : BlocksGUI_ExplodeDlg.cxx
// Author : Julia DOROVSKIKH, Open CASCADE S.A.S. (julia.dorovskikh@opencascade.com)
//
#include "BlocksGUI_ExplodeDlg.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <SUIT_Session.h>
#include <SUIT_Desktop.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_ViewWindow.h>
#include <SUIT_ViewManager.h>
#include <SUIT_MessageBox.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>
#include <OCCViewer_ViewModel.h>
#include <SALOME_ListIO.hxx>

//=================================================================================
// class    : BlocksGUI_ExplodeDlg()
// purpose  : Constructs a BlocksGUI_ExplodeDlg which is a child of 'parent'.
//=================================================================================
BlocksGUI_ExplodeDlg::BlocksGUI_ExplodeDlg( GeometryGUI* theGeometryGUI, QWidget* parent )
  : GEOMBase_Skeleton( theGeometryGUI, parent ), myNbBlocks( 0 )
{
  QPixmap image1( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_BLOCK_EXPLODE" ) ) );
  QPixmap imageS( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );

  setWindowTitle( tr( "GEOM_BLOCK_EXPLODE_TITLE" ) );

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle( tr( "GEOM_BLOCK_EXPLODE" ) );

  mainFrame()->RadioButton1->setIcon( image1 );
  mainFrame()->RadioButton2->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton3->close();

  // Create first group
  myGrp1 = new DlgRef_1Sel2Spin1View1Check( centralWidget() );
  myGrp1->GroupBox1->setTitle( tr( "GEOM_ARGUMENTS" ) );
  myGrp1->TextLabel1->setText( tr( "GEOM_MAIN_OBJECT" ) );
  myGrp1->PushButton1->setIcon( imageS );
  myGrp1->LineEdit1->setReadOnly( true );
  myGrp1->TextLabel2->setText( tr( "NB_FACES_MIN" ) );
  myGrp1->TextLabel3->setText( tr( "NB_FACES_MAX" ) );
  myGrp1->CheckBox1->setText( tr( "GEOM_SUBSHAPE_SELECT" ) );

  // Add groups to layout
  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( myGrp1 );
  /***************************************************************/

  setHelpFileName( "explode_on_blocks_operation_page.html" );

  Init();
}

//=================================================================================
// function : ~BlocksGUI_ExplodeDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
BlocksGUI_ExplodeDlg::~BlocksGUI_ExplodeDlg()
{
  // no need to delete child widgets, Qt does it all for us
  clearTemporary();
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BlocksGUI_ExplodeDlg::Init()
{
  mainFrame()->GroupBoxName->hide();

  // Set range of spinboxes
  int SpecificStep = 1;
  initSpinBox( myGrp1->SpinBox1, 0, 999, SpecificStep );
  initSpinBox( myGrp1->SpinBox2, 0, 999, SpecificStep );

  if ( SUIT_Session::session()->activeApplication()->desktop()->activeWindow()->getViewManager()->getType() 
       != OCCViewer_Viewer::Type() )
    myGrp1->CheckBox1->setEnabled( false );

  // signals and slots connections
  connect( buttonOk(),    SIGNAL( clicked() ), this, SLOT( ClickOnOk() ) );
  connect( buttonApply(), SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );

  connect( myGrp1->PushButton1, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );

  connect( myGrp1->SpinBox1, SIGNAL( valueChanged( int ) ), this, SLOT( ValueChangedInSpinBox( int ) ) );
  connect( myGrp1->SpinBox2, SIGNAL( valueChanged( int ) ), this, SLOT( ValueChangedInSpinBox( int ) ) );

  connect( myGrp1->CheckBox1, SIGNAL( stateChanged( int ) ), this, SLOT( SubShapeToggled() ) );

  connect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr(),
           SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );

  myConstructorId = -1;
  ConstructorsClicked( 0 );
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void BlocksGUI_ExplodeDlg::ConstructorsClicked( int constructorId )
{
  if ( myConstructorId == constructorId )
    return;

  myConstructorId = constructorId;

  switch ( constructorId ) {
  case 0:
    myGrp1->show();
    myGrp1->SpinBox1->setValue( 6 );
    myGrp1->SpinBox2->setValue( 6 );
    myGrp1->CheckBox1->setChecked( false );
    break;
  default:
    break;
  }

  // init fields
  myEditCurrentArgument = myGrp1->LineEdit1;
  myObject = GEOM::GEOM_Object::_nil();

  activateSelection();
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void BlocksGUI_ExplodeDlg::ClickOnOk()
{
  setIsApplyAndClose( true );
  if ( ClickOnApply() )
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool BlocksGUI_ExplodeDlg::ClickOnApply()
{
  SUIT_Session::session()->activeApplication()->putInfo( tr( "" ) );

  // Explode all sub-shapes
  if ( isAllSubShapes() ) {
    // More than 30 sub-shapes : ask confirmation
    if ( myNbBlocks > 30 ) {
      if ( SUIT_MessageBox::warning( this, 
                                     tr( "GEOM_CONFIRM" ),
                                     tr( "GEOM_CONFIRM_INFO" ).arg( myNbBlocks ),
                                     tr( "GEOM_BUT_EXPLODE" ),
                                     tr( "GEOM_BUT_CANCEL" ) ) != 0 )
        return false;  /* aborted */
    }
  }

  if ( !onAccept( true, true, false ) )
    return false;

  activateSelection();

  return true;
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection has changed
//=================================================================================
void BlocksGUI_ExplodeDlg::SelectionIntoArgument()
{
  if (!isAllSubShapes())
    return;

  myObject = GEOM::GEOM_Object::_nil();
  myGrp1->LineEdit1->setText("");

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  if (aSelList.Extent() == 1) {
    GEOM::GEOM_Object_var anObj = GEOMBase::ConvertIOinGEOMObject( aSelList.First() );

    if ( GEOMBase::IsShape(anObj) ) {
      myObject = anObj;
      myGrp1->LineEdit1->setText(GEOMBase::GetName(anObj));
    }
  }

  updateButtonState();
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BlocksGUI_ExplodeDlg::SetEditCurrentArgument()
{
  QPushButton* aSender = (QPushButton*)sender();

  if ( myGrp1->PushButton1 == aSender ) {
    myGrp1->LineEdit1->setFocus();
    myEditCurrentArgument = myGrp1->LineEdit1;
    myGrp1->CheckBox1->setChecked( false );
  }

  activateSelection();
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void BlocksGUI_ExplodeDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr(), 
           SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );

  activateSelection();
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void BlocksGUI_ExplodeDlg::enterEvent( QEvent* )
{
  if ( !mainFrame()->GroupConstructors->isEnabled() )
    this->ActivateThisDialog();
}

//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void BlocksGUI_ExplodeDlg::ValueChangedInSpinBox()
{
  if ( !isAllSubShapes() )
    activateSelection();
  else
    updateButtonState();
}

//=================================================================================
// function : SubShapeToggled()
// purpose  : Allow user selection of all or only selected sub-shapes
//          : Called when 'myGrp1->CheckBox1' state change
//=================================================================================
void BlocksGUI_ExplodeDlg::SubShapeToggled()
{
  activateSelection();
}

//=================================================================================
// function : activateSelection
// purpose  : Redisplay preview and Activate selection
//=================================================================================
void BlocksGUI_ExplodeDlg::activateSelection()
{
  clearTemporary();
  erasePreview( true );

  if ( isAllSubShapes() ) { // Sub-shapes selection disabled
    disconnect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr(),
                SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );
    globalSelection( GEOM_ALLSHAPES );
    if ( myObject->_is_nil() ) {
      SelectionIntoArgument();
    }
    connect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr(),
             SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );
  } 
  else {
    displayPreview( true, true, true, false );
    globalSelection( GEOM_PREVIEW );
  }
}

//=================================================================================
// function : updateButtonState
// purpose  :
//=================================================================================
void BlocksGUI_ExplodeDlg::updateButtonState()
{
  if ( SUIT_Session::session()->activeApplication()->desktop()->activeWindow()->getViewManager()->getType() 
       != OCCViewer_Viewer::Type() || myObject->_is_nil() ) {
    myGrp1->CheckBox1->setChecked( false );
    myGrp1->CheckBox1->setEnabled( false );
  } 
  else {
    myGrp1->CheckBox1->setEnabled( true );
  }

  myNbBlocks = 0;

  if ( myObject->_is_nil() ) {
    myGrp1->TextBrowser1->setText( "" );
  } 
  else {
    GEOM::GEOM_IBlocksOperations_var anOper = GEOM::GEOM_IBlocksOperations::_narrow(getOperation());
    bool isOnlyBlocks = anOper->IsCompoundOfBlocks( myObject,
                                                    myGrp1->SpinBox1->value(),
                                                    myGrp1->SpinBox2->value(),
                                                    myNbBlocks );
    if ( isOnlyBlocks )
      myGrp1->TextBrowser1->setText( tr( "GEOM_NB_BLOCKS_NO_OTHERS" ).arg( myNbBlocks ) );
    else
      myGrp1->TextBrowser1->setText( tr( "GEOM_NB_BLOCKS_SOME_OTHERS" ).arg( myNbBlocks ) );
  }
}

//=================================================================================
// function : isAllSubShapes
// purpose  :
//=================================================================================
bool BlocksGUI_ExplodeDlg::isAllSubShapes() const
{
  return !(myGrp1->CheckBox1->isEnabled() && myGrp1->CheckBox1->isChecked());
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr BlocksGUI_ExplodeDlg::createOperation()
{
  return getGeomEngine()->GetIBlocksOperations( getStudyId() );
}

//=================================================================================
// function : isValid()
// purpose  : Verify validity of input data
//=================================================================================
bool BlocksGUI_ExplodeDlg::isValid (QString& msg)
{
  bool okSP = true;
  okSP = myGrp1->SpinBox1->isValid( msg, !IsPreview() ) && okSP;
  okSP = myGrp1->SpinBox2->isValid( msg, !IsPreview() ) && okSP;

  bool ok = false;
  switch ( getConstructorId() ) {
  case 0:
    if ( IsPreview() ) {
      ok = !myObject->_is_nil();
    }
    else {
      LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
      SALOME_ListIO aSelList;
      aSelMgr->selectedObjects(aSelList);

      ok = !myObject->_is_nil() && (isAllSubShapes() || aSelList.Extent());
    }
    break;
  default:
    break;
  }

  return ok && okSP;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool BlocksGUI_ExplodeDlg::execute( ObjectList& objects )
{
  GEOM::ListOfGO_var aList;

  GEOM::GEOM_IBlocksOperations_var anOper = GEOM::GEOM_IBlocksOperations::_narrow(getOperation());

  switch ( getConstructorId() ) {
  case 0:
    aList = anOper->ExplodeCompoundOfBlocks( myObject,
                                             myGrp1->SpinBox1->value(),
                                             myGrp1->SpinBox2->value() );
    break;
  }
  
  if ( !aList->length() )
    return false;

  if ( IsPreview() ) {
    clearTemporary();
    
    // Store objects. They will be put in study when "Apply" is pressed
    for ( int i = 0, n = aList->length(); i < n; i++ ) {
      objects.push_back( GEOM::GEOM_Object::_duplicate( aList[i] ) );
      myTmpObjs.push_back( GEOM::GEOM_Object::_duplicate( aList[i] ) );
    }

    return objects.size() > 0;
  }

  QStringList aParameters;
  aParameters << myGrp1->SpinBox1->text();
  aParameters << myGrp1->SpinBox2->text();

  // Throw away sub-shapes not selected by user if not in preview mode
  // and manual selection is active
  if ( !isAllSubShapes() ) {
    QMap<QString, char> selected;

    LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
    SALOME_ListIO aSelList;
    aSelMgr->selectedObjects(aSelList);

    // Get names of selected objects
    SALOME_ListIteratorOfListIO it (aSelList);
    for ( ; it.More(); it.Next() ) {
      selected.insert( it.Value()->getName(), 0 );
    }

    // Iterate through result and select objects with names from selection
    ObjectList toRemoveFromEngine;
    ObjectList::iterator anIter;
    for ( anIter = myTmpObjs.begin(); anIter != myTmpObjs.end(); ++anIter ) {
      CORBA::String_var objStr = myGeomGUI->getApp()->orb()->object_to_string( *anIter );
      if ( selected.contains( QString( objStr.in() ) ) )
      {
        if ( !IsPreview() )
          (*anIter)->SetParameters(aParameters.join(":").toLatin1().constData());
        objects.push_back( *anIter );
      }
      else
        toRemoveFromEngine.push_back( *anIter );
    }

    // Remove from engine useless objects
    ObjectList::iterator anIter2 = toRemoveFromEngine.begin();
    for ( ; anIter2 != toRemoveFromEngine.end(); ++anIter2 )
      getGeomEngine()->RemoveObject( *anIter2 );

    myTmpObjs.clear();

  }
  else {
    for ( int i = 0, n = aList->length(); i < n; i++ )
    {
      GEOM::GEOM_Object_var anObj = GEOM::GEOM_Object::_duplicate( aList[i] );
      if ( !IsPreview() )
        anObj->SetParameters(aParameters.join(":").toLatin1().constData());
      objects.push_back( anObj._retn() );
    }
  }

  return objects.size();
}

//=================================================================================
// function : clearTemporary
// purpose  : Remove temporary objects from engine
//=================================================================================
void BlocksGUI_ExplodeDlg::clearTemporary()
{
  ObjectList::iterator anIter;
  for ( anIter = myTmpObjs.begin(); anIter != myTmpObjs.end(); ++anIter )
    getGeomEngine()->RemoveObject( *anIter );

  myTmpObjs.clear();
}

//================================================================
// Function : getFather
// Purpose  : Get father object for object to be added in study
//            ( called with addInStudy method )
//================================================================
GEOM::GEOM_Object_ptr BlocksGUI_ExplodeDlg::getFather( GEOM::GEOM_Object_ptr )
{
  return myObject;
}

//================================================================
// Function : getNewObjectName
// Purpose  : Redefine this method to return proper name for a new object
//================================================================
QString BlocksGUI_ExplodeDlg::getNewObjectName (int) const
{
  return QString::null;
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> BlocksGUI_ExplodeDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res;
  GEOM::GeomObjPtr aGeomObjPtr(myObject);
  res << aGeomObjPtr;
  return res;
}
