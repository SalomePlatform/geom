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
// File   : MeasureGUI_CheckCompoundOfBlocksDlg.cxx
// Author : Vladimir KLYACHIN, Open CASCADE S.A.S. (vladimir.klyachin@opencascade.com)
//
#include "MeasureGUI.h"
#include "MeasureGUI_CheckCompoundOfBlocksDlg.h"

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

#include <QListWidget>

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
  : GEOMBase_Skeleton(GUI, parent, false),
    myObjectName     (0),
    mySelButton      (0),
    myUseC1Check     (0),
    myTolLbl         (0),
    mySpinTol        (0),
    myTextView       (0),
    myListBox1       (0),
    myListBox2       (0)
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

  QGroupBox   *aGrpParams    =
    new QGroupBox(tr("GEOM_CHECK_INFOS"), centralWidget());
  QGridLayout *aParamsLayout = new QGridLayout(aGrpParams);
  QLabel      *anObjLbl      = new QLabel(tr("GEOM_OBJECT"), aGrpParams);
  QLabel      *anErrorsLbl   =
    new QLabel(tr("GEOM_CHECK_BLOCKS_COMPOUND_ERRORS"), aGrpParams);
  QLabel      *aNonBlocksLbl =
    new QLabel(tr("GEOM_CHECK_BLOCKS_COMPOUND_SUBSHAPES"), aGrpParams);

  myObjectName = new QLineEdit(aGrpParams);
  mySelButton  = new QPushButton(aGrpParams);
  myUseC1Check = new QCheckBox(tr("GEOM_USE_C1_CRITERION"), aGrpParams);
  myTolLbl     = new QLabel(tr("GEOM_ANGULAR_TOLERANCE"), aGrpParams);
  mySpinTol    = new SalomeApp_DoubleSpinBox(aGrpParams);
  myTextView   = new QTextBrowser(aGrpParams);
  myListBox1   = new QListWidget(aGrpParams);
  myListBox2   = new QListWidget(aGrpParams);

  myObjectName->setReadOnly(true);
  mySelButton->setIcon(image1);
  mySelButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  myUseC1Check->setText(tr("GEOM_USE_C1_CRITERION"));
  myUseC1Check->setChecked(true);
  myTextView->setReadOnly(true);
  myListBox2->setSelectionMode(QAbstractItemView::ExtendedSelection);

  // Set text view font.
  QFont aFont(TEXTEDIT_FONT_FAMILY, TEXTEDIT_FONT_SIZE);

  aFont.setStyleHint(QFont::TypeWriter, QFont::PreferAntialias);
  myTextView->setFont(aFont);

  aParamsLayout->setMargin(9);
  aParamsLayout->setSpacing(6);
  aParamsLayout->addWidget(anObjLbl,      0, 0);
  aParamsLayout->addWidget(mySelButton,   0, 1);
  aParamsLayout->addWidget(myObjectName,  0, 2);
  aParamsLayout->addWidget(myUseC1Check,  1, 0, 1, 3);
  aParamsLayout->addWidget(myTolLbl,      2, 0);
  aParamsLayout->addWidget(mySpinTol,     2, 1, 1, 2);
  aParamsLayout->addWidget(myTextView,    3, 0, 1, 3);
  aParamsLayout->addWidget(anErrorsLbl,   4, 0);
  aParamsLayout->addWidget(myListBox1,    5, 0, 1, 2);
  aParamsLayout->addWidget(aNonBlocksLbl, 4, 2);
  aParamsLayout->addWidget(myListBox2,    5, 2);

  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( aGrpParams );

  /***************************************************************/

  myHelpFileName = "check_compound_of_blocks_page.html";

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
  /* init variables */
  double SpecificStep = 0.0001;
  double aDefaultTol  = Precision::Angular();

  initSpinBox(mySpinTol, aDefaultTol, MAX_NUMBER, SpecificStep, "ang_tol_precision");
  mySpinTol->setValue(aDefaultTol);
  myEditCurrentArgument = myObjectName;

  // signals and slots connections
  connect( buttonOk(),    SIGNAL( clicked() ), this, SLOT( ClickOnOk() ) );
  connect( buttonApply(), SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );

  connect(myObjectName, SIGNAL(returnPressed()),        this, SLOT(LineEditReturnPressed()));
  connect(mySelButton,  SIGNAL(clicked()),              this, SLOT(SetEditCurrentArgument()));
  connect(myListBox1,   SIGNAL(itemSelectionChanged()), this, SLOT(onErrorsListSelectionChanged()));
  connect(myListBox2,   SIGNAL(itemSelectionChanged()), this, SLOT(onSubShapesListSelectionChanged()));
  connect(myUseC1Check, SIGNAL(clicked()),              this, SLOT(SetUseC1Tolerance()));
  connect(mySpinTol,    SIGNAL(valueChanged(double)),   this, SLOT(processObject()));

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
    myObjectName->setText( "" );
    processObject();
    return;
  }

  GEOM::GEOM_Object_var aSelectedObject =
    GEOMBase::ConvertIOinGEOMObject( aSelList.First() );

  if ( aSelectedObject->_is_nil() ) {
    myObjectName->setText( "" );
    processObject();
    return;
  }

  myObj = aSelectedObject;
  myObjectName->setText( GEOMBase::GetName( myObj ) );
  processObject();
  DISPLAY_PREVIEW_MACRO;
}

//=================================================================================
// function : SetEditCurrentArgument
// purpose  :
//=================================================================================
void MeasureGUI_CheckCompoundOfBlocksDlg::SetEditCurrentArgument()
{
  myObjectName->setFocus();
  myEditCurrentArgument = myObjectName;
  SelectionIntoArgument();
}

//=================================================================================
// function : SetUseC1Tolerance()
// purpose  :
//=================================================================================
void MeasureGUI_CheckCompoundOfBlocksDlg::SetUseC1Tolerance()
{
  myTolLbl->setEnabled(myUseC1Check->isChecked());
  mySpinTol->setEnabled(myUseC1Check->isChecked());
  processObject();
}

//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void MeasureGUI_CheckCompoundOfBlocksDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if ( send == myObjectName ) {
    myEditCurrentArgument = myObjectName;
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
      double aC1Tol = -1.;

      if (myUseC1Check->isChecked()) {
        aC1Tol = mySpinTol->value();
      }

      theIsCompoundOfBlocks = anOper->CheckCompoundOfBlocks( myObj, aC1Tol, aErrs );
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
    myTextView->setText( aMsg );
    myListBox1->clear();
    myListBox2->clear();
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
  myTextView->setText( aMsg );

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
        aErrStr = tr("GEOM_CHECK_BLOCKS_NOT_BLOCK");
        break;
      case GEOM::GEOM_IBlocksOperations::EXTRA_EDGE :
        aErrStr = tr("GEOM_CHECK_BLOCKS_EXTRA_EDGE");
        break;
      case GEOM::GEOM_IBlocksOperations::INVALID_CONNECTION :
        aErrStr = tr("GEOM_CHECK_BLOCKS_INVALID_CONNECTION").arg(aConNum++);
        break;
      case GEOM::GEOM_IBlocksOperations::NOT_CONNECTED :
        aErrStr = tr("GEOM_CHECK_BLOCKS_NOT_CONNECTED");
        break;
      case GEOM::GEOM_IBlocksOperations::NOT_GLUED :
        aErrStr = tr("GEOM_CHECK_BLOCKS_NOT_GLUED").arg(aGluedNum++);
        break;
      default :
        aErrStr = "";
        break;
    }
    if ( !aErrStr.isEmpty() )
      aErrList.append( aErrStr );
  }

  myListBox1->clear();
  myListBox2->clear();
  myListBox1->addItems( aErrList );
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
  int aCurItem = myListBox1->currentRow();
  if ( aCurItem < 0 )
    return;
  bool isCompoundOfBlocks;
  GEOM::GEOM_IBlocksOperations::BCErrors aErrs;
  if ( !getBCErrors( isCompoundOfBlocks, aErrs ) ) {
    myTextView->setText( "" );
    myListBox1->clear();
    myListBox2->clear();
    return;
  }

  myListBox2->clear();

  if (aCurItem < aErrs.length()) {
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
    myListBox2->addItems( aSubShapeList );
  }
}

//=================================================================================
// function : onSubShapesListSelectionChanged
// purpose  :
//=================================================================================
void MeasureGUI_CheckCompoundOfBlocksDlg::onSubShapesListSelectionChanged()
{
  erasePreview();
  int aErrCurItem = myListBox1->currentRow();
  if ( aErrCurItem < 0 )
    return;
  QList<int> aIds;
  for ( int i = 0, n = myListBox2->count(); i < n; i++ ) {
    if ( myListBox2->item( i )->isSelected() )
      aIds.append( i );
  }
  if ( aIds.count() < 1 )
    return;
  bool isCompoundOfBlocks;
  GEOM::GEOM_IBlocksOperations::BCErrors aErrs;
  if ( !getBCErrors( isCompoundOfBlocks, aErrs ) ) {
    myTextView->setText( "" );
    myListBox1->clear();
    myListBox2->clear();
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
      int w = resMgr->integerValue("Geometry", "preview_edge_width", 1);
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
// function : onDisplayPreview
// purpose  : 
//=================================================================================
void MeasureGUI_CheckCompoundOfBlocksDlg::onDisplayPreview()
{
  DISPLAY_PREVIEW_MACRO;
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
bool MeasureGUI_CheckCompoundOfBlocksDlg::isValid( QString &msg)
{
  return !myObj->_is_nil() && mySpinTol->isValid(msg, !IsPreview());
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
