//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// GEOM RepairGUI : GUI for Geometry component
// File   : RepairGUI_GlueDlg.cxx
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S. 
//
#include "RepairGUI_GlueDlg.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>
#include <QtxDoubleSpinBox.h>

#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>
#include <SalomeApp_Study.h>
#include <SalomeApp_Tools.h>
#include <SUIT_Session.h>
#include <SUIT_Desktop.h>
#include <SUIT_MessageBox.h>
#include <SUIT_OverrideCursor.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_ViewWindow.h>
#include <SUIT_ViewManager.h>
#include <OCCViewer_ViewModel.h>
#include <SALOME_ListIteratorOfListIO.hxx>

#include <GEOMImpl_Types.hxx>

#include <TCollection_AsciiString.hxx>

#define DEFAULT_TOLERANCE_VALUE 1e-07

//=================================================================================
// class    : RepairGUI_GlueDlg()
// purpose  : Constructs a RepairGUI_GlueDlg  which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
RepairGUI_GlueDlg::RepairGUI_GlueDlg( GeometryGUI* theGeometryGUI, QWidget* parent,
                                      bool modal )
  : GEOMBase_Skeleton( theGeometryGUI, parent, modal ),
    myCurrConstrId( -1 )
{
  QPixmap image0( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_GLUE_FACES" ) ) );
  QPixmap image2( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_GLUE_FACES2" )  ));
  QPixmap image1( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );

  setWindowTitle( tr( "GEOM_GLUE_TITLE" ) );

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle( tr( "GEOM_GLUE_TITLE" ) );
  mainFrame()->RadioButton1->setIcon( image0 );
  mainFrame()->RadioButton2->setIcon( image2 );
  mainFrame()->RadioButton3->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton3->close();

  GroupPoints = new DlgRef_1SelExt( centralWidget() );
  GroupPoints->GroupBox1->setTitle( tr( "GEOM_GLUE" ) );
  GroupPoints->TextLabel1->setText( tr( "GEOM_SELECTED_SHAPE" ) );
  GroupPoints->PushButton1->setIcon( image1 );
  GroupPoints->LineEdit1->setReadOnly( true );
  
  QLabel* aTolLab = new QLabel( tr( "GEOM_TOLERANCE" ), GroupPoints->Box );
  myTolEdt = new SalomeApp_DoubleSpinBox( GroupPoints->Box );
  initSpinBox( myTolEdt, 0., 100., DEFAULT_TOLERANCE_VALUE, "len_tol_precision" );
  myTolEdt->setValue( DEFAULT_TOLERANCE_VALUE );

  QGridLayout* boxLayout = new QGridLayout( GroupPoints->Box );
  boxLayout->setMargin( 0 ); boxLayout->setSpacing( 6 );
  boxLayout->addWidget( aTolLab,  0, 0 );
  boxLayout->addWidget( myTolEdt, 0, 2 );
  /***************************************************************/

  GroupPoints2 = new DlgRef_1SelExt( centralWidget() );
  GroupPoints2->GroupBox1->setTitle( tr( "GEOM_GLUE" ) );
  GroupPoints2->TextLabel1->setText( tr( "GEOM_SELECTED_SHAPE" ) );
  GroupPoints2->PushButton1->setIcon( image1 );
  GroupPoints2->LineEdit1->setReadOnly( true );

  QLabel* aTolLab2 = new QLabel( tr( "GEOM_TOLERANCE" ), GroupPoints2->Box );
  myTolEdt2 = new SalomeApp_DoubleSpinBox( GroupPoints2->Box );
  initSpinBox( myTolEdt2, 0., 100., DEFAULT_TOLERANCE_VALUE, "len_tol_precision" );
  myTolEdt2->setValue( DEFAULT_TOLERANCE_VALUE );

  myDetectBtn = new QPushButton( tr( "GEOM_DETECT" ) + QString( " [%1]" ).arg( tr( "GLUE_FACES" ) ), 
                                 GroupPoints2->Box );
  mySubShapesChk = new QCheckBox( tr( "SELECT_FACES" ), GroupPoints2->Box );

  boxLayout = new QGridLayout( GroupPoints2->Box );
  boxLayout->setMargin( 0 ); boxLayout->setSpacing( 6 );
  boxLayout->addWidget( aTolLab2,    0, 0 );
  boxLayout->addWidget( myTolEdt2,   0, 2 );
  boxLayout->addWidget( myDetectBtn, 1, 0, 1, 3 );
  boxLayout->addWidget( mySubShapesChk, 2, 0, 1, 3 );

  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( GroupPoints );
  layout->addWidget( GroupPoints2 );

  /***************************************************************/

  setHelpFileName( "glue_faces_operation_page.html" );
  
  // Disable second way of gluing if OCC viewer is not active one
  if ( myGeomGUI->getApp()->desktop()->activeWindow()->getViewManager()->getType()
       != OCCViewer_Viewer::Type() )
    mainFrame()->RadioButton2->setEnabled( false );

  Init();
}


//=================================================================================
// function : ~RepairGUI_GlueDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
RepairGUI_GlueDlg::~RepairGUI_GlueDlg()
{
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void RepairGUI_GlueDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupPoints->LineEdit1;

  myObject = GEOM::GEOM_Object::_nil();

  //myGeomGUI->SetState( 0 );
  //globalSelection( GEOM_COMPOUND );

  mainFrame()->GroupBoxPublish->show();

  /* signals and slots connections */
  connect( buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect( buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect( this,          SIGNAL( constructorsClicked( int ) ), this, SLOT( ConstructorsClicked( int ) ) );

  connect( GroupPoints->PushButton1,  SIGNAL( clicked() ),       this, SLOT( SetEditCurrentArgument() ) );
  connect( GroupPoints->LineEdit1,    SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );

  connect( GroupPoints2->PushButton1, SIGNAL( clicked() ),       this, SLOT( SetEditCurrentArgument() ) );
  connect( GroupPoints2->LineEdit1,   SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );
  connect( myTolEdt2, SIGNAL( valueChanged( double ) ), this, SLOT( onTolerChanged( double ) ) );
  connect( mySubShapesChk, SIGNAL( stateChanged( int ) ), this, SLOT( onSubShapesChk() ) );

  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
           this, SLOT( SelectionIntoArgument() ) );

  connect( myDetectBtn, SIGNAL( clicked() ), this, SLOT( onDetect() ) );

  initName( tr( "GLUE_NEW_OBJ_NAME" ) );

  ConstructorsClicked( 0 );
  
  activateSelection();
  updateButtonState();
}


//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void RepairGUI_GlueDlg::ConstructorsClicked( int constructorId )
{
  if ( myCurrConstrId == constructorId )
    return;
  
  disconnect( myGeomGUI->getApp()->selectionMgr(), 0, this, 0 );
  
  switch ( constructorId ) {
  case 0:
    GroupPoints2->hide();
    GroupPoints->show();
    GroupPoints->LineEdit1->setText( "" );
    myEditCurrentArgument = GroupPoints->LineEdit1;
    
    if ( myCurrConstrId >= 0 ) {
      // i.e. it is not initialisation
      // copy tolerance from previous tolerance field
      myTolEdt->setValue( myTolEdt2->value() );
    }
    break;
  case 1:
    GroupPoints->hide();
    GroupPoints2->show();
    GroupPoints->LineEdit1->setText( "" );
    myEditCurrentArgument = GroupPoints2->LineEdit1;
        
    if ( myCurrConstrId >= 0 ) {
      // i.e. it is not initialisation
      // copy tolerance from previous tolerance field
      myTolEdt2->setValue( myTolEdt->value() );
      mySubShapesChk->setChecked( false );
      clearTemporary();
    }
    break;
  }
    
  myCurrConstrId = constructorId;

  myEditCurrentArgument->setFocus();

  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
           this, SLOT( SelectionIntoArgument() ) );

  qApp->processEvents();
  updateGeometry();
  resize( minimumSizeHint() );

  displayPreview();
  updateButtonState();
  activateSelection();
  SelectionIntoArgument();
}


//=================================================================================
// function : ClickOnOk()
// purpose  : Same than click on apply but close this dialog.
//=================================================================================
void RepairGUI_GlueDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool RepairGUI_GlueDlg::ClickOnApply()
{
  if ( !onAcceptLocal() )
    return false;

  initName();

  //GroupPoints->LineEdit1->setText("");
  //myObject = GEOM::GEOM_Object::_nil();

  //globalSelection( GEOM_COMPOUND );

  ConstructorsClicked( getConstructorId() );

  return true;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection
//=================================================================================
void RepairGUI_GlueDlg::SelectionIntoArgument()
{
  if ( mySubShapesChk->isChecked() &&  getConstructorId() == 1 ) {
    updateButtonState();
    return;
  }
  
  erasePreview();
  myEditCurrentArgument->setText( "" );
  myObject = GEOM::GEOM_Object::_nil();

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  if ( aSelList.Extent() == 1 ) {
    Handle(SALOME_InteractiveObject) anIO = aSelList.First();
    Standard_Boolean aRes;
    myObject = GEOMBase::ConvertIOinGEOMObject( anIO, aRes );
    if ( aRes )
      myEditCurrentArgument->setText( GEOMBase::GetName( myObject ) );
  }
  updateButtonState();
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void RepairGUI_GlueDlg::SetEditCurrentArgument()
{
  const QObject* send = sender();
  if ( send == GroupPoints->PushButton1 || send == GroupPoints2->PushButton1 )  {
    myEditCurrentArgument->setFocus();
    SelectionIntoArgument();
  }
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void RepairGUI_GlueDlg::LineEditReturnPressed()
{
  const QObject* send = sender();
  if ( send == GroupPoints->LineEdit1 ) {
    myEditCurrentArgument = GroupPoints->LineEdit1;
    GEOMBase_Skeleton::LineEditReturnPressed();
  }
  else if ( send == GroupPoints2->LineEdit1 ) {
    myEditCurrentArgument = GroupPoints2->LineEdit1;
    GEOMBase_Skeleton::LineEditReturnPressed();
  }
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void RepairGUI_GlueDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ), 
           this, SLOT( SelectionIntoArgument() ) );

  //GroupPoints->LineEdit1->setText("");
  //GroupPoints2->LineEdit1->setText("");
  //myObject = GEOM::GEOM_Object::_nil();

  //myGeomGUI->SetState( 0 );
  //globalSelection( GEOM_COMPOUND );
  activateSelection();
}


//=================================================================================
// function : enterEvent()
// purpose  : Mouse enter onto the dialog to activate it
//=================================================================================
void RepairGUI_GlueDlg::enterEvent( QEvent* )
{
  if ( !mainFrame()->GroupConstructors->isEnabled() )
    ActivateThisDialog();
}


//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr RepairGUI_GlueDlg::createOperation()
{
  return getGeomEngine()->GetIShapesOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool RepairGUI_GlueDlg::isValid( QString& msg )
{
  bool ok = true;
  double v = 0;
  switch ( getConstructorId() )
  {
  case 0:
    v = myTolEdt->value();
    ok = myTolEdt->isValid( msg, !IsPreview() );
    break;
  case 1:
    v = myTolEdt2->value(); 
    ok = myTolEdt2->isValid( msg, !IsPreview() );
    break;
  }
  return !myObject->_is_nil() && ( IsPreview() || v > 0. ) && ok;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool RepairGUI_GlueDlg::execute( ObjectList& objects )
{
  bool aResult = false;
  objects.clear();

  GEOM::GEOM_IShapesOperations_var anOper = GEOM::GEOM_IShapesOperations::_narrow( getOperation() );
  switch ( getConstructorId() ) {
  case 0:
    {
      GEOM::GEOM_Object_var anObj = anOper->MakeGlueFaces( myObject, myTolEdt->value(), true );
      aResult = !anObj->_is_nil();
      if ( aResult && !IsPreview() )
      {
        QStringList aParameters;
        aParameters << myTolEdt->text();
        anObj->SetParameters(aParameters.join(":").toLatin1().constData());

        objects.push_back( anObj._retn() );
      }
      break;
    }
  case 1:
    {
      if ( IsPreview() ) {
        // if this method is used for displaying preview then we must detect glue faces only
        ObjectList::iterator anIter;
        for ( anIter = myTmpObjs.begin(); anIter != myTmpObjs.end(); ++anIter )
          objects.push_back( GEOM::GEOM_Object::_duplicate( *anIter ) );
        return myTmpObjs.size() ? true : false;
      } // IsPreview

      // Make glue face by list.
      // Iterate through myTmpObjs and verifies where each object is currently selected or not.
      QMap<QString, char> selected;

      // Get names of selected objects
      LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
      SALOME_ListIO aSelList;
      aSelMgr->selectedObjects(aSelList);

      SALOME_ListIteratorOfListIO it (aSelList);
      for (; it.More(); it.Next()) 
        selected.insert(it.Value()->getName(), 0);

      // Iterate through result and select objects with names from selection
      // ObjectList toRemoveFromEnggine;
      ObjectList toGlue;
      ObjectList::iterator anIter;
      for ( anIter = myTmpObjs.begin(); anIter != myTmpObjs.end(); ++anIter ) {
        if ( selected.contains( myGeomGUI->getApp()->orb()->object_to_string(*anIter) ) )
          toGlue.push_back( *anIter );
      }
      
      // make glue faces
      GEOM::ListOfGO_var aListForGlue = new GEOM::ListOfGO();
      aListForGlue->length( toGlue.size() );
      ObjectList::iterator anIter3 = toGlue.begin();
      for ( int i = 0; anIter3 != toGlue.end(); ++anIter3, ++i )
        aListForGlue[ i ] = *anIter3;
      GEOM::GEOM_Object_var anObj = anOper->MakeGlueFacesByList( myObject, myTolEdt2->value(), aListForGlue, true );

      aResult = !anObj->_is_nil();

      if ( aResult )
      {
        if ( !IsPreview() )
        {
          QStringList aParameters;
          aParameters << myTolEdt2->text();
          anObj->SetParameters(aParameters.join(":").toLatin1().constData());
        }
        objects.push_back( anObj._retn() );
      }

      // Remove from engine useless objects
      clearTemporary();

      updateButtonState();

      break;
    } // case 1
  } // switch

  return aResult;
}

//=================================================================================
// function : restoreSubShapes
// purpose  :
//=================================================================================
void RepairGUI_GlueDlg::restoreSubShapes( SALOMEDS::Study_ptr   theStudy,
                                          SALOMEDS::SObject_ptr theSObject )
{
  if ( mainFrame()->CheckBoxRestoreSS->isChecked() ) {
    GEOM::find_shape_method aFindMethod = GEOM::FSM_GetInPlace;
    if ( getConstructorId() == 0 ) // MakeGlueFaces
      aFindMethod = GEOM::FSM_GetInPlaceByHistory;

    // empty list of arguments means that all arguments should be restored
    getGeomEngine()->RestoreSubShapesSO( theStudy, theSObject, GEOM::ListOfGO(),
                                         aFindMethod, /*theInheritFirstArg=*/true,
                                         mainFrame()->CheckBoxAddPrefix->isChecked() );
  }
}

//================================================================
// Function : clearShapeBufferLocal
// Purpose  : 
//================================================================
void RepairGUI_GlueDlg::clearShapeBufferLocal( GEOM::GEOM_Object_ptr theObj )
{
  if ( CORBA::is_nil( theObj ) )
    return;

  CORBA::String_var IOR = myGeomGUI->getApp()->orb()->object_to_string( theObj );
  TCollection_AsciiString asciiIOR( (char *)( IOR.in() ) );
  myGeomGUI->GetShapeReader().RemoveShapeFromBuffer( asciiIOR );

  if ( !getStudy() || !( getStudy()->studyDS() ) )
    return;

  _PTR(Study) aStudy = getStudy()->studyDS();
  _PTR(SObject) aSObj ( aStudy->FindObjectIOR( std::string( IOR.in() ) ) );
  if ( !aSObj )
    return;

  _PTR(ChildIterator) anIt ( aStudy->NewChildIterator( aSObj ) );
  for ( anIt->InitEx( true ); anIt->More(); anIt->Next() ) {
    _PTR(GenericAttribute) anAttr;
    if ( anIt->Value()->FindAttribute(anAttr, "AttributeIOR") ) {
      _PTR(AttributeIOR) anIOR ( anAttr );
      TCollection_AsciiString asciiIOR( (char*)anIOR->Value().c_str() );
      myGeomGUI->GetShapeReader().RemoveShapeFromBuffer( asciiIOR );      
    }
  }
}

//================================================================
// Function : onAccept
// Purpose  : This method should be called from dialog's slots onOk() and onApply()
//            It perfroms user input validation, then it 
//            performs a proper operation and manages transactions, etc.
//================================================================
bool RepairGUI_GlueDlg::onAcceptLocal()
{
  if ( !getStudy() || !( getStudy()->studyDS() ) )
    return false;

  _PTR(Study) aStudy = getStudy()->studyDS();

  bool aLocked = aStudy->GetProperties()->IsLocked();
  if ( aLocked ) {
    MESSAGE("GEOMBase_Helper::onAccept - ActiveStudy is locked");
    SUIT_MessageBox::warning( this, tr( "WRN_WARNING" ), tr( "WRN_STUDY_LOCKED" ), tr( "BUT_OK" ) );
    return false;
  }

  QString msg;
  if ( !isValid( msg ) ) {
    showError( msg );
    return false;
  }

  erasePreview( false );

  try {
    if ( openCommand() ) {
      SUIT_OverrideCursor wc;
      
      myGeomGUI->getApp()->putInfo( "" );
      ObjectList objects;
      
      if ( !execute( objects ) ) { 
        wc.suspend();
        abortCommand();
        showError();
      }
      else {
        const int nbObjs = objects.size();
        for ( ObjectList::iterator it = objects.begin(); it != objects.end(); ++it ) {
          QString aName = getNewObjectName();
          if ( nbObjs > 1 ) {
            if ( aName.isEmpty() )
              aName = getPrefix( *it );
            aName = GEOMBase::GetDefaultName( aName );
          }
          else {
            // PAL6521: use a prefix, if some dialog box doesn't reimplement getNewObjectName()
            if ( aName.isEmpty() )
              aName = GEOMBase::GetDefaultName( getPrefix( *it ) );
          }
          addInStudy( *it, aName.toLatin1().data() );
          display( *it, false );
        }
        
        if ( nbObjs ) {
          commitCommand();
          updateObjBrowser();
          myGeomGUI->getApp()->putInfo( QObject::tr("GEOM_PRP_DONE") );
        }
        else {
          abortCommand();
        }

        // JFA 28.12.2004 BEGIN // To enable warnings
        GEOM::GEOM_IShapesOperations_var anOper = GEOM::GEOM_IShapesOperations::_narrow( getOperation() );
        if ( !CORBA::is_nil(anOper) && !anOper->IsDone() ) {
          wc.suspend();
          QString msgw = QObject::tr( anOper->GetErrorCode() );
          SUIT_MessageBox::warning( this, tr( "WRN_WARNING" ), msgw, tr( "BUT_OK" ) );
        }
        // JFA 28.12.2004 END
      }
    }
  }
  catch( const SALOME::SALOME_Exception& e ) {
    SalomeApp_Tools::QtCatchCorbaException( e );
    abortCommand();
  }

  updateViewer();
  activateSelection();
  updateButtonState();

  return true;
}


//=================================================================================
// function : onDetect
// purpose  :
//=================================================================================
void RepairGUI_GlueDlg::onDetect()
{
  clearTemporary();
  QString msg;
  if ( !isValid( msg ) ) {
    showError( msg );
    return;
  }

  buttonOk()->setEnabled( false );
  buttonApply()->setEnabled( false );
  globalSelection( GEOM_ALLSHAPES );

  GEOM::GEOM_IShapesOperations_var anOper = GEOM::GEOM_IShapesOperations::_narrow( getOperation() );
  GEOM::ListOfGO_var aList = anOper->GetGlueFaces( myObject, myTolEdt2->value() );
  
  for ( int i = 0, n = aList->length(); i < n; i++ ) 
    myTmpObjs.push_back(GEOM::GEOM_Object::_duplicate(aList[i]));
  
  if ( myTmpObjs.size() > 0  ) {
    msg = tr( "FACES_FOR_GLUING_ARE_DETECTED" ).arg( myTmpObjs.size() );
    mySubShapesChk->setChecked( true );
  }
  else {
    msg = tr( "THERE_ARE_NO_FACES_FOR_GLUING" );
  }
  
  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
           this, SLOT( SelectionIntoArgument() ) ) ;
  SUIT_MessageBox::information( this, tr( "GEOM_FREE_BOUNDS_TLT" ), msg, tr( "Close" ) );
  updateButtonState();
  activateSelection();
}

//=================================================================================
// function : activateSelection
// purpose  : Redisplay preview and Activate selection
//=================================================================================
void RepairGUI_GlueDlg::activateSelection()
{
  erasePreview( false );
  
  int anId = getConstructorId();
  if ( anId == 0 )  { 
    // Case of whole gluing
    disconnect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
                this, SLOT( SelectionIntoArgument() ) );
    
    globalSelection( GEOM_ALLSHAPES );
    if ( myObject->_is_nil()) 
      SelectionIntoArgument();

    connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
             this, SLOT( SelectionIntoArgument() ) );
  } 
  else {
    // Second case of gluing
    if ( !mySubShapesChk->isChecked() ) 
      globalSelection( GEOM_ALLSHAPES );
    else {
      displayPreview( true, false, false, 2/*line width*/, 1/*display mode*/, Quantity_NOC_RED );
      disconnect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
                  this, SLOT( SelectionIntoArgument() ) ) ;
      globalSelection( GEOM_PREVIEW );
      connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
               this, SLOT( SelectionIntoArgument() ) ) ;
    } 
  }
  updateViewer();
}

//=================================================================================
// function : updateButtonState
// purpose  : Update button state 
//=================================================================================
void RepairGUI_GlueDlg::updateButtonState()
{
  int anId = getConstructorId();
  bool hasMainObj = !myObject->_is_nil();
  if ( anId == 0 ) {
    buttonOk()->setEnabled( hasMainObj );
    buttonApply()->setEnabled( hasMainObj );
  }
  else
  {
    LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
    SALOME_ListIO aSelList;
    aSelMgr->selectedObjects(aSelList);

    bool wasSelected = false;
    SALOME_ListIteratorOfListIO it (aSelList);
    if (it.More() > 0)
      wasSelected = true;
    bool wasDetected = myTmpObjs.size() ? true : false;
    buttonOk()->setEnabled( hasMainObj && wasDetected && wasSelected );
    buttonApply()->setEnabled( hasMainObj && wasDetected && wasSelected );
    mySubShapesChk->setEnabled( hasMainObj && wasDetected );
    myDetectBtn->setEnabled( hasMainObj );
    if ( !hasMainObj || !wasDetected )
      mySubShapesChk->setChecked( false );
  }
}

//=================================================================================
// function : clearTemporary
// purpose  : Remove temporary objects from engine
//=================================================================================
void RepairGUI_GlueDlg::clearTemporary()
{
  ObjectList::iterator anIter;
  for ( anIter = myTmpObjs.begin(); anIter != myTmpObjs.end(); ++anIter )
    getGeomEngine()->RemoveObject(*anIter);

  myTmpObjs.clear();
}

//=================================================================================
// function : onTolerChanged
// purpose  : Remove temporary objects from engine
//=================================================================================
void RepairGUI_GlueDlg::onTolerChanged( double /*theVal*/ )
{
  clearTemporary();
  activateSelection();
  updateButtonState();
}

//=================================================================================
// function : onSubShapesChk
// purpose  : Update selection mode
//=================================================================================
void RepairGUI_GlueDlg::onSubShapesChk()
{
  if ( !mySubShapesChk->isChecked() )
    clearTemporary();
  activateSelection();
  updateButtonState();
}

//=================================================================================
// function : ClickOnCancel
// purpose  : Remove temporary objects from engine and call method of base class
//=================================================================================
void RepairGUI_GlueDlg::ClickOnCancel()
{
  clearTemporary();
  GEOMBase_Skeleton::ClickOnCancel();
}
