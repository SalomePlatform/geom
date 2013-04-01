// Copyright (C) 2007-2013  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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

// GEOM GEOMGUI : GUI for Geometry component
// File   : MeasureGUI_CheckCompoundOfBlocksDlg.cxx
// Author : Vladimir KLYACHIN, Open CASCADE S.A.S. (vladimir.klyachin@opencascade.com)
//
#include "MeasureGUI.h"
#include "MeasureGUI_CheckCompoundOfBlocksDlg.h"
#include "MeasureGUI_Widgets.h"

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <LightApp_SelectionMgr.h>
#include <SalomeApp_Tools.h>
#include <SalomeApp_Application.h>

#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>
#include <TColStd_MapOfInteger.hxx>

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>
#include <GEOMImpl_Types.hxx>

#define TEXTEDIT_FONT_FAMILY "Courier"
#define TEXTEDIT_FONT_SIZE 11

//=================================================================================
// class    : MeasureGUI_CheckCompoundOfBlocksDlg()
// purpose  : Constructs a MeasureGUI_CheckCompoundOfBlocksDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            true to construct a modal dialog.
//=================================================================================
MeasureGUI_CheckCompoundOfBlocksDlg::MeasureGUI_CheckCompoundOfBlocksDlg( GeometryGUI* GUI, QWidget* parent )
  : GEOMBase_Skeleton( GUI, parent, false )
{
  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
  QPixmap image0( aResMgr->loadPixmap( "GEOM", tr( "ICON_DLG_CHECK_COMPOUND_OF_BLOCKS" ) ) );
  QPixmap image1( aResMgr->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );

  setWindowTitle( tr( "GEOM_CHECK_BLOCKS_COMPOUND" ) );

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle( tr( "GEOM_CHECK_BLOCKS_COMPOUND" ) );
  mainFrame()->RadioButton1->setIcon( image0 );
  mainFrame()->RadioButton2->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton3->close();

  myGrp = new MeasureGUI_1Sel1TextView2ListBox( centralWidget() );
  myGrp->GroupBox1->setTitle( tr( "GEOM_CHECK_INFOS" ) );
  myGrp->TextLabel1->setText( tr( "GEOM_OBJECT" ) );

  myGrp->TextView1->setReadOnly( true );
  QFont aFont( TEXTEDIT_FONT_FAMILY, TEXTEDIT_FONT_SIZE );
  aFont.setStyleHint( QFont::TypeWriter, QFont::PreferAntialias );
  myGrp->TextView1->setFont( aFont );

  myGrp->PushButton1->setIcon( image1 );
  myGrp->LineEdit1->setReadOnly( true );

  myGrp->TextLabel2->setText( tr( "GEOM_CHECK_BLOCKS_COMPOUND_ERRORS" ) );
  myGrp->TextLabel3->setText( tr( "GEOM_CHECK_BLOCKS_COMPOUND_SUBSHAPES" ) );

  myGrp->ListBox2->setSelectionMode( QAbstractItemView::ExtendedSelection );

  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( myGrp );

  connect( myGrp->ListBox1, SIGNAL( itemSelectionChanged() ), SLOT( onErrorsListSelectionChanged() ) );
  connect( myGrp->ListBox2, SIGNAL( itemSelectionChanged() ), SLOT( onSubShapesListSelectionChanged() ) );

  /***************************************************************/

  myHelpFileName = "using_measurement_tools_page.html#check_compound_anchor";

  /* Initialisation */
  Init();
}

//=================================================================================
// function : ~MeasureGUI_CheckCompoundOfBlocksDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
MeasureGUI_CheckCompoundOfBlocksDlg::~MeasureGUI_CheckCompoundOfBlocksDlg()
{
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void MeasureGUI_CheckCompoundOfBlocksDlg::Init()
{
  myEditCurrentArgument = myGrp->LineEdit1;

  // signals and slots connections
  connect( buttonOk(),    SIGNAL( clicked() ), this, SLOT( ClickOnOk() ) );
  connect( buttonApply(), SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );

  connect( myGrp->LineEdit1,   SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );
  connect( myGrp->PushButton1, SIGNAL( clicked() ),       this, SLOT( SetEditCurrentArgument() ) );

  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
           this, SLOT( SelectionIntoArgument() ) );

  initName( tr( "GEOM_BLOCKS_COMPOUND") );
  buttonOk()->setEnabled( false );
  buttonApply()->setEnabled( false );
  activateSelection();
  SelectionIntoArgument();
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void MeasureGUI_CheckCompoundOfBlocksDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool MeasureGUI_CheckCompoundOfBlocksDlg::ClickOnApply()
{
  if ( !onAccept() )
    return false;

  initName();
  return true;
}

//=================================================================================
// function : SelectionIntoArgument
// purpose  :
//=================================================================================
void MeasureGUI_CheckCompoundOfBlocksDlg::SelectionIntoArgument()
{
  erasePreview();
  myObj = GEOM::GEOM_Object::_nil();

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  if (aSelList.Extent() != 1) {
    myGrp->LineEdit1->setText( "" );
    processObject();
    return;
  }

  GEOM::GEOM_Object_var aSelectedObject =
    GEOMBase::ConvertIOinGEOMObject( aSelList.First() );

  if ( aSelectedObject->_is_nil() ) {
    myGrp->LineEdit1->setText( "" );
    processObject();
    return;
  }

  myObj = aSelectedObject;
  myGrp->LineEdit1->setText( GEOMBase::GetName( myObj ) );
  processObject();
  DISPLAY_PREVIEW_MACRO;
}

//=================================================================================
// function : SetEditCurrentArgument
// purpose  :
//=================================================================================
void MeasureGUI_CheckCompoundOfBlocksDlg::SetEditCurrentArgument()
{
  myGrp->LineEdit1->setFocus();
  myEditCurrentArgument = myGrp->LineEdit1;
  SelectionIntoArgument();
}

//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void MeasureGUI_CheckCompoundOfBlocksDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if ( send == myGrp->LineEdit1 ) {
    myEditCurrentArgument = myGrp->LineEdit1;
    GEOMBase_Skeleton::LineEditReturnPressed();
  }
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void MeasureGUI_CheckCompoundOfBlocksDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();

  LightApp_SelectionMgr* aSel = myGeomGUI->getApp()->selectionMgr();
  if ( aSel )
    connect( aSel, SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );

  activateSelection();
  DISPLAY_PREVIEW_MACRO
}

//=================================================================================
// function : getBCErrors
// purpose  :
//=================================================================================
bool MeasureGUI_CheckCompoundOfBlocksDlg::getBCErrors( bool& theIsCompoundOfBlocks,
                                                       GEOM::GEOM_IBlocksOperations::BCErrors& theErrors)
{
  if ( myObj->_is_nil() )
    return false;
  else {
    GEOM::GEOM_IBlocksOperations_var anOper = GEOM::GEOM_IBlocksOperations::_narrow( getOperation() );
    try {
      GEOM::GEOM_IBlocksOperations::BCErrors_var aErrs;
      theIsCompoundOfBlocks = anOper->CheckCompoundOfBlocks( myObj, aErrs );
      if (anOper->IsDone() && aErrs->length() > 0)
      //if (anOper->IsDone() && !aErrs._is_nil())
        theErrors = aErrs;
    }
    catch ( const SALOME::SALOME_Exception& e ) {
      SalomeApp_Tools::QtCatchCorbaException( e );
      return false;
    }

    return anOper->IsDone();
  }
}

//=================================================================================
// function : processObject
// purpose  :
//=================================================================================
void MeasureGUI_CheckCompoundOfBlocksDlg::processObject()
{
  QString aMsg ( "" );
  bool isCompoundOfBlocks;
  GEOM::GEOM_IBlocksOperations::BCErrors aErrs;
  if ( !getBCErrors( isCompoundOfBlocks, aErrs ) ) {
    aMsg += tr( "GEOM_CHECK_BLOCKS_COMPOUND_FAILED" );
    myGrp->TextView1->setText( aMsg );
    myGrp->ListBox1->clear();
    myGrp->ListBox2->clear();
    erasePreview();
    return;
  }

  if ( isCompoundOfBlocks ) {
    aMsg += tr( "GEOM_CHECK_BLOCKS_COMPOUND_HAS_NO_ERRORS" );
    buttonOk()->setEnabled( false );
    buttonApply()->setEnabled( false );
  }
  else {
    aMsg += tr( "GEOM_CHECK_BLOCKS_COMPOUND_HAS_ERRORS" );
    buttonOk()->setEnabled( true );
    buttonApply()->setEnabled( true );
  }
  myGrp->TextView1->setText( aMsg );

  QStringList aErrList;
  QString aErrStr( "" );
  QString aConSfx( " # " );
  QString aGluedSfx( " # " );
  int aConNum = 1;
  int aGluedNum = 1;
  for ( int i = 0, n = aErrs.length(); i < n; i++ ) {
    aErrStr = "";
    switch ( aErrs[i].error ) {
      case GEOM::GEOM_IBlocksOperations::NOT_BLOCK :
        aErrStr = "Not a Block";
        break;
      case GEOM::GEOM_IBlocksOperations::EXTRA_EDGE :
        aErrStr = "Extra Edge";
        break;
      case GEOM::GEOM_IBlocksOperations::INVALID_CONNECTION :
        aErrStr = "Invalid Connection";
        aErrStr += aConSfx;
        aErrStr += QString::number( aConNum );
        aConNum++;
        break;
      case GEOM::GEOM_IBlocksOperations::NOT_CONNECTED :
        aErrStr = "Not Connected";
        break;
      case GEOM::GEOM_IBlocksOperations::NOT_GLUED :
        aErrStr = "Not Glued";
        aErrStr += aGluedSfx;
        aErrStr += QString::number( aGluedNum );
        aGluedNum++;
        break;
      default :
        aErrStr = "";
        break;
    }
    if ( !aErrStr.isEmpty() )
      aErrList.append( aErrStr );
  }

  myGrp->ListBox1->clear();
  myGrp->ListBox2->clear();
  myGrp->ListBox1->addItems( aErrList );
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr MeasureGUI_CheckCompoundOfBlocksDlg::createOperation()
{
  return getGeomEngine()->GetIBlocksOperations( getStudyId() );
}

//=================================================================================
// function : onErrorsListSelectionChanged
// purpose  :
//=================================================================================
void MeasureGUI_CheckCompoundOfBlocksDlg::onErrorsListSelectionChanged()
{
  erasePreview();
  int aCurItem = myGrp->ListBox1->currentRow();
  if ( aCurItem < 0 )
    return;
  bool isCompoundOfBlocks;
  GEOM::GEOM_IBlocksOperations::BCErrors aErrs;
  if ( !getBCErrors( isCompoundOfBlocks, aErrs ) ) {
    myGrp->TextView1->setText( "" );
    myGrp->ListBox1->clear();
    myGrp->ListBox2->clear();
    return;
  }

  GEOM::GEOM_IBlocksOperations::BCError aErr = aErrs[aCurItem];
  GEOM::ListOfLong aObjLst = aErr.incriminated;
  QStringList aSubShapeList;
  TopoDS_Shape aSelShape;
  if ( !myObj->_is_nil() && GEOMBase::GetShape( myObj, aSelShape ) ) {
    TopTools_IndexedMapOfShape anIndices;
    TopExp::MapShapes( aSelShape, anIndices );
    for ( int i = 0, n = aObjLst.length(); i < n; i++ ) {
      TopoDS_Shape aSubShape = anIndices.FindKey( aObjLst[i] );
      QString aType = GEOMBase::GetShapeTypeString( aSubShape );
      if ( !aType.isEmpty() )
	aSubShapeList.append( QString( "%1_%2" ).arg( aType ).arg( aObjLst[i] ) );
    }
  }
  myGrp->ListBox2->clear();
  myGrp->ListBox2->addItems( aSubShapeList );
}

//=================================================================================
// function : onSubShapesListSelectionChanged
// purpose  :
//=================================================================================
void MeasureGUI_CheckCompoundOfBlocksDlg::onSubShapesListSelectionChanged()
{
  erasePreview();
  int aErrCurItem = myGrp->ListBox1->currentRow();
  if ( aErrCurItem < 0 )
    return;
  QList<int> aIds;
  for ( int i = 0, n = myGrp->ListBox2->count(); i < n; i++ ) {
    if ( myGrp->ListBox2->item( i )->isSelected() )
      aIds.append( i );
  }
  if ( aIds.count() < 1 )
    return;
  bool isCompoundOfBlocks;
  GEOM::GEOM_IBlocksOperations::BCErrors aErrs;
  if ( !getBCErrors( isCompoundOfBlocks, aErrs ) ) {
    myGrp->TextView1->setText( "" );
    myGrp->ListBox1->clear();
    myGrp->ListBox2->clear();
    return;
  }

  GEOM::GEOM_IBlocksOperations::BCError aErr = aErrs[aErrCurItem];
  GEOM::ListOfLong aObjLst = aErr.incriminated;
  TopoDS_Shape aSelShape;
  TopoDS_Shape aSubShape;
  TopTools_IndexedMapOfShape anIndices;
  if ( !myObj->_is_nil() && GEOMBase::GetShape( myObj, aSelShape ) ) {
    QString aMess;
    if ( !isValid( aMess ) ) {
      return;
    }
    SALOME_Prs* aPrs = 0;
    TopExp::MapShapes( aSelShape, anIndices);
    QList<int>::iterator it;
    for ( it = aIds.begin(); it != aIds.end(); ++it ) {
      aSubShape = anIndices.FindKey(aObjLst[(*it)]);
      SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
      int w = resMgr->integerValue("Geometry", "measures_line_width", 1);
      try {
        getDisplayer()->SetColor( Quantity_NOC_RED );
        getDisplayer()->SetWidth( w );
        getDisplayer()->SetToActivate( false );
        aPrs = !aSubShape.IsNull() ? getDisplayer()->BuildPrs( aSubShape ) : 0;
        if ( aPrs )
          displayPreview( aPrs, true );
      }
      catch ( const SALOME::SALOME_Exception& e ) {
        SalomeApp_Tools::QtCatchCorbaException( e );
      }
    }
  }
}

//=================================================================================
// function : activateSelection
// purpose  : activate selection of faces, shells, and solids
//=================================================================================
void MeasureGUI_CheckCompoundOfBlocksDlg::activateSelection()
{
  TColStd_MapOfInteger aMap;
  aMap.Add( GEOM_SOLID );
  aMap.Add( GEOM_COMPOUND );
  globalSelection( aMap );
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void MeasureGUI_CheckCompoundOfBlocksDlg::enterEvent( QEvent* )
{
  if ( !mainFrame()->GroupConstructors->isEnabled() )
    ActivateThisDialog();
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool MeasureGUI_CheckCompoundOfBlocksDlg::isValid( QString& )
{
  return !myObj->_is_nil();
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool MeasureGUI_CheckCompoundOfBlocksDlg::execute( ObjectList& objects )
{
  GEOM::GEOM_IBlocksOperations_var anOper = GEOM::GEOM_IBlocksOperations::_narrow( getOperation() );
  GEOM::GEOM_Object_var anObj = anOper->CheckAndImprove( myObj );

  if ( !anObj->_is_nil() )
    objects.push_back( anObj._retn() );

  return true;
}
