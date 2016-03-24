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
// File   : MeasureGUI_CheckShapeDlg.cxx
// Author : Nicolas REJNERI, Open CASCADE S.A.S.
//
#include "MeasureGUI.h"
#include "MeasureGUI_CheckShapeDlg.h"
#include "MeasureGUI_Widgets.h"
#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>
#include <GEOMImpl_Types.hxx>

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SalomeApp_Tools.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>

#include <TColStd_IndexedMapOfInteger.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>

#define TEXTEDIT_FONT_FAMILY "Courier"
#define TEXTEDIT_FONT_SIZE 11

//=================================================================================
// class    : MeasureGUI_CheckShapeDlg()
// purpose  : Constructs a MeasureGUI_CheckShapeDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            true to construct a modal dialog.
//=================================================================================
MeasureGUI_CheckShapeDlg::MeasureGUI_CheckShapeDlg( GeometryGUI* GUI, QWidget* parent )
  : GEOMBase_Skeleton( GUI, parent, false )
{
  QPixmap image0( SUIT_Session::session()->resourceMgr()->loadPixmap(
    "GEOM", tr( "ICON_DLG_CHECKSHAPE" ) ) );
  QPixmap image1( SUIT_Session::session()->resourceMgr()->loadPixmap(
    "GEOM", tr( "ICON_SELECT" ) ) );

  setWindowTitle( tr( "GEOM_CHECK_TITLE" ) );

  /***************************************************************/

  mainFrame()->GroupConstructors->setTitle( tr( "GEOM_CHECK_SHAPE" ) );
  mainFrame()->RadioButton1->setIcon( image0 );
  mainFrame()->RadioButton2->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton3->close();

  myGrp = new MeasureGUI_1Sel1Check1TextView2ListBox( centralWidget() );
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

  myHelpFileName = "check_shape_page.html";

  /* Initialisation */
  Init();
}

//=================================================================================
// function : ~MeasureGUI_CheckShapeDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
MeasureGUI_CheckShapeDlg::~MeasureGUI_CheckShapeDlg()
{
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void MeasureGUI_CheckShapeDlg::Init()
{
  // signals and slots connections
  connect( buttonOk(),    SIGNAL( clicked() ), this, SLOT( ClickOnOk() ) );
  connect( buttonApply(), SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );

  connect( myGrp->PushButton1, SIGNAL( clicked() ),       this, SLOT( SetEditCurrentArgument() ) );

  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
           this, SLOT( SelectionIntoArgument() ) );

  connect( myGrp->CheckBox1, SIGNAL( toggled( bool) ), 
           this, SLOT( SelectionIntoArgument() ) );

  initName( tr( "GEOM_CHECK_SHAPE_NAME") );
  buttonOk()->setEnabled( false );
  buttonApply()->setEnabled( false );
  activateSelection();
  SelectionIntoArgument();
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void MeasureGUI_CheckShapeDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool MeasureGUI_CheckShapeDlg::ClickOnApply()
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
bool MeasureGUI_CheckShapeDlg::extractPrefix() const
{
  return true;
}

//=================================================================================
// function : SelectionIntoArgument
// purpose  :
//=================================================================================
void MeasureGUI_CheckShapeDlg::SelectionIntoArgument()
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
void MeasureGUI_CheckShapeDlg::SetEditCurrentArgument()
{
  myGrp->LineEdit1->setFocus();
  SelectionIntoArgument();
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void MeasureGUI_CheckShapeDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();

  LightApp_SelectionMgr* aSel = myGeomGUI->getApp()->selectionMgr();
  if ( aSel )
    connect( aSel, SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );

  activateSelection();
  DISPLAY_PREVIEW_MACRO
}

//=================================================================================
// function : activateSelection
// purpose  : activate selection of faces, shells, and solids
//=================================================================================
void MeasureGUI_CheckShapeDlg::activateSelection()
{
  globalSelection( GEOM_ALLSHAPES );
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void MeasureGUI_CheckShapeDlg::enterEvent( QEvent* )
{
  if ( !mainFrame()->GroupConstructors->isEnabled() )
    ActivateThisDialog();
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool MeasureGUI_CheckShapeDlg::isValid( QString& )
{
  return !myObj->_is_nil();
}

//=================================================================================
// function : getErrors
// purpose  :
//=================================================================================
bool MeasureGUI_CheckShapeDlg::getErrors
                    ( bool& theIsValid,
                      GEOM::GEOM_IMeasureOperations::ShapeErrors& theErrors )
{
  if ( myObj->_is_nil() ) {
    return false;
  } else {
    GEOM::GEOM_IMeasureOperations_var anOper = GEOM::GEOM_IMeasureOperations::_narrow( getOperation() );
    try {
      GEOM::GEOM_IMeasureOperations::ShapeErrors_var aErrs;
      bool isCheckGeometry = myGrp->CheckBox1->isChecked();
      if ( isCheckGeometry )
        theIsValid = anOper->CheckShapeWithGeometry( myObj, aErrs );
      else
        theIsValid = anOper->CheckShape( myObj, aErrs );

      if (anOper->IsDone() && aErrs->length() > 0)
        theErrors = aErrs;
    }
    catch( const SALOME::SALOME_Exception& e ) {
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
void MeasureGUI_CheckShapeDlg::processObject()
{
  bool isShapeValid;
  GEOM::GEOM_IMeasureOperations::ShapeErrors aErrs;

  if ( !getErrors( isShapeValid, aErrs ) ) {
    myGrp->TextView1->setText( "" );
    myGrp->ListBox1->clear();
    myGrp->ListBox2->clear();
    erasePreview();

    return;
  }

  if ( isShapeValid ) {
    myGrp->TextView1->setText(tr("GEOM_CHECK_SHAPE_VALID"));
    buttonOk()->setEnabled( false );
    buttonApply()->setEnabled( false );
  } else {
    myGrp->TextView1->setText(tr("GEOM_CHECK_SHAPE_NOT_VALID"));
    buttonOk()->setEnabled( true );
    buttonApply()->setEnabled( true );
  }

  // Add Error groups
  QStringList aErrList;

  for ( int i = 0, n = aErrs.length(); i < n; i++ ) {
    QString aErrStr("CHECK_ERROR_");

    switch ( aErrs[i].error ) {
      case GEOM::GEOM_IMeasureOperations::InvalidPointOnCurve:
        aErrStr += "INVALID_POINT_ON_CURVE";
        break;
      case GEOM::GEOM_IMeasureOperations::InvalidPointOnCurveOnSurface:
        aErrStr += "INVALID_POINT_ON_CURVE_ON_SURFACE";
        break;
      case GEOM::GEOM_IMeasureOperations::InvalidPointOnSurface:
        aErrStr += "INVALID_POINT_ON_SURFACE";
        break;
      case GEOM::GEOM_IMeasureOperations::No3DCurve:
        aErrStr += "NO_3D_CURVE";
        break;
      case GEOM::GEOM_IMeasureOperations::Multiple3DCurve:
        aErrStr += "MULTIPLE_3D_CURVE";
        break;
      case GEOM::GEOM_IMeasureOperations::Invalid3DCurve:
        aErrStr += "INVALID_3D_CURVE";
        break;
      case GEOM::GEOM_IMeasureOperations::NoCurveOnSurface:
        aErrStr += "NO_CURVE_ON_SURFACE";
        break;
      case GEOM::GEOM_IMeasureOperations::InvalidCurveOnSurface:
        aErrStr += "INVALID_CURVE_ON_SURFACE";
        break;
      case GEOM::GEOM_IMeasureOperations::InvalidCurveOnClosedSurface:
        aErrStr += "INVALID_CURVE_ON_CLOSED_SURFACE";
        break;
      case GEOM::GEOM_IMeasureOperations::InvalidSameRangeFlag:
        aErrStr += "INVALID_SAME_RANGE_FLAG";
        break;
      case GEOM::GEOM_IMeasureOperations::InvalidSameParameterFlag:
        aErrStr += "INVALID_SAME_PARAMETER_FLAG";
        break;
      case GEOM::GEOM_IMeasureOperations::InvalidDegeneratedFlag:
        aErrStr += "INVALID_DEGENERATED_FLAG";
        break;
      case GEOM::GEOM_IMeasureOperations::FreeEdge:
        aErrStr += "FREE_EDGE";
        break;
      case GEOM::GEOM_IMeasureOperations::InvalidMultiConnexity:
        aErrStr += "INVALID_MULTI_CONNEXITY";
        break;
      case GEOM::GEOM_IMeasureOperations::InvalidRange:
        aErrStr += "INVALID_RANGE";
        break;
      case GEOM::GEOM_IMeasureOperations::EmptyWire:
        aErrStr += "EMPTY_WIRE";
        break;
      case GEOM::GEOM_IMeasureOperations::RedundantEdge:
        aErrStr += "REDUNDANT_EDGE";
        break;
      case GEOM::GEOM_IMeasureOperations::SelfIntersectingWire:
        aErrStr += "SELF_INTERSECTING_WIRE";
        break;
      case GEOM::GEOM_IMeasureOperations::NoSurface:
        aErrStr += "NO_SURFACE";
        break;
      case GEOM::GEOM_IMeasureOperations::InvalidWire:
        aErrStr += "INVALID_WIRE";
        break;
      case GEOM::GEOM_IMeasureOperations::RedundantWire:
        aErrStr += "REDUNDANT_WIRE";
        break;
      case GEOM::GEOM_IMeasureOperations::IntersectingWires:
        aErrStr += "INTERSECTING_WIRES";
        break;
      case GEOM::GEOM_IMeasureOperations::InvalidImbricationOfWires:
        aErrStr += "INVALID_IMBRICATION_OF_WIRES";
        break;
      case GEOM::GEOM_IMeasureOperations::EmptyShell:
        aErrStr += "EMPTY_SHELL";
        break;
      case GEOM::GEOM_IMeasureOperations::RedundantFace:
        aErrStr += "REDUNDANT_FACE";
        break;
      case GEOM::GEOM_IMeasureOperations::UnorientableShape:
        aErrStr += "UNORIENTABLE_SHAPE";
        break;
      case GEOM::GEOM_IMeasureOperations::NotClosed:
        aErrStr += "NOT_CLOSED";
        break;
      case GEOM::GEOM_IMeasureOperations::NotConnected:
        aErrStr += "NOT_CONNECTED";
        break;
      case GEOM::GEOM_IMeasureOperations::SubshapeNotInShape:
        aErrStr += "SUBSHAPE_NOT_IN_SHAPE";
        break;
      case GEOM::GEOM_IMeasureOperations::BadOrientation:
        aErrStr += "BAD_ORIENTATION";
        break;
      case GEOM::GEOM_IMeasureOperations::BadOrientationOfSubshape:
        aErrStr += "BAD_ORIENTATION_OF_SUBSHAPE";
        break;
      case GEOM::GEOM_IMeasureOperations::InvalidToleranceValue:
        aErrStr += "INVALID_TOLERANCE_VALUE";
        break;
      case GEOM::GEOM_IMeasureOperations::CheckFail:
        aErrStr += "CHECK_FAIL";
        break;
      default:
        aErrStr.clear();
        break;
    }

    if (!aErrStr.isEmpty()) {
      aErrList.append(tr(aErrStr.toLatin1().constData()));
    }
  }

  myGrp->ListBox1->clear();
  myGrp->ListBox2->clear();
  myGrp->ListBox1->addItems( aErrList );
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr MeasureGUI_CheckShapeDlg::createOperation()
{
  return getGeomEngine()->GetIMeasureOperations( getStudyId() );
}

//=================================================================================
// function : onErrorsListSelectionChanged
// purpose  :
//=================================================================================
void MeasureGUI_CheckShapeDlg::onErrorsListSelectionChanged()
{
  erasePreview();

  int aCurItem = myGrp->ListBox1->currentRow();

  if ( aCurItem < 0 ) {
    return;
  }

  bool isShapeValid;
  GEOM::GEOM_IMeasureOperations::ShapeErrors aErrs;

  if ( !getErrors( isShapeValid, aErrs ) ) {
    myGrp->TextView1->setText( "" );
    myGrp->ListBox1->clear();
    myGrp->ListBox2->clear();

    return;
  }

  myGrp->ListBox2->clear();

  if (aCurItem < aErrs.length()) {
    GEOM::GEOM_IMeasureOperations::ShapeError aErr = aErrs[aCurItem];
    GEOM::ListOfLong aObjLst = aErr.incriminated;
    QStringList aSubShapeList;
    TopoDS_Shape aSelShape;

    if ( !myObj->_is_nil() && GEOMBase::GetShape( myObj, aSelShape ) ) {
      TopTools_IndexedMapOfShape anIndices;

      TopExp::MapShapes( aSelShape, anIndices );

      for ( int i = 0, n = aObjLst.length(); i < n; i++ ) {
        TopoDS_Shape aSubShape = anIndices.FindKey( aObjLst[i] );
        QString aType = GEOMBase::GetShapeTypeString( aSubShape );

        if ( !aType.isEmpty() ) {
          aSubShapeList.append( QString( "%1_%2" ).arg( aType ).arg( aObjLst[i] ) );
        }
      }
    }

    myGrp->ListBox2->addItems( aSubShapeList );
  }
}

//=================================================================================
// function : onSubShapesListSelectionChanged
// purpose  :
//=================================================================================
void MeasureGUI_CheckShapeDlg::onSubShapesListSelectionChanged()
{
  erasePreview();

  int aErrCurItem = myGrp->ListBox1->currentRow();

  if (aErrCurItem < 0) {
    return;
  }

  QList<int> aIds;

  for (int i = 0, n = myGrp->ListBox2->count(); i < n; i++) {
    if (myGrp->ListBox2->item( i )->isSelected()) {
      aIds.append(i);
    }
  }

  if (aIds.count() < 1) {
    return;
  }

  bool isShapeValid;
  GEOM::GEOM_IMeasureOperations::ShapeErrors aErrs;

  if (!getErrors(isShapeValid, aErrs)) {
    myGrp->TextView1->setText("");
    myGrp->ListBox1->clear();
    myGrp->ListBox2->clear();

    return;
  }

  GEOM::GEOM_IMeasureOperations::ShapeError aErr = aErrs[aErrCurItem];
  GEOM::ListOfLong aObjLst = aErr.incriminated;
  TopoDS_Shape aSelShape;
  TopoDS_Shape aSubShape;
  TopTools_IndexedMapOfShape anIndices;

  if (!myObj->_is_nil() && GEOMBase::GetShape(myObj, aSelShape)) {
    QString aMess;

    if (!isValid(aMess)) {
      return;
    }

    SALOME_Prs* aPrs = 0;
    QList<int>::iterator it;

    TopExp::MapShapes(aSelShape, anIndices);

    for (it = aIds.begin(); it != aIds.end(); ++it) {
      SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
      int w = resMgr->integerValue("Geometry", "measures_line_width", 1);

      aSubShape = anIndices.FindKey(aObjLst[(*it)]);

      try {
        getDisplayer()->SetColor( Quantity_NOC_RED );
        getDisplayer()->SetWidth( w );
        getDisplayer()->SetToActivate( false );
        aPrs = !aSubShape.IsNull() ? getDisplayer()->BuildPrs(aSubShape) : 0;

        if (aPrs) {
          displayPreview(aPrs, true);
        }
      }
      catch (const SALOME::SALOME_Exception& e) {
        SalomeApp_Tools::QtCatchCorbaException(e);
      }
    }
  }
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool MeasureGUI_CheckShapeDlg::execute( ObjectList& objects )
{
  bool isShapeValid;
  GEOM::GEOM_IMeasureOperations::ShapeErrors aErrs;

  if (!getErrors(isShapeValid, aErrs)) {
    return false;
  }

  const int  aNbErrsSelected    = myGrp->ListBox1->selectedItems().size();
  const bool isPublishAllErrors = (aNbErrsSelected < 1);
  const bool isPublishAllShapes =
    (aNbErrsSelected != 1 || myGrp->ListBox2->selectedItems().empty());
  TColStd_IndexedMapOfInteger aMapIndex;
  const int aNbErrs = aErrs.length();
  int i;

  // Collect indices of shapes to be published.
  for (i = 0; i < aNbErrs; i++) {
    if (isPublishAllErrors || myGrp->ListBox1->item(i)->isSelected()) {
      GEOM::ListOfLong aObjLst = aErrs[i].incriminated;
      const int aNbShapes = aObjLst.length();
      int j;

      for (j = 0; j < aNbShapes; j++) {
        if (isPublishAllShapes || myGrp->ListBox2->item(j)->isSelected()) {
          aMapIndex.Add(aObjLst[j]);
        }
      }
    }
  }

  // Create objects.
  GEOM::ListOfLong_var anArray   = new GEOM::ListOfLong;
  const int            aNbShapes = aMapIndex.Extent();

  anArray->length(aNbShapes);

  for (i = 1; i <= aNbShapes; i++) {
    anArray[i - 1] = aMapIndex.FindKey(i);
  }

  if (myShapesOper->_is_nil()) {
    myShapesOper = getGeomEngine()->GetIShapesOperations(getStudyId());
  }

  GEOM::ListOfGO_var aList = myShapesOper->MakeSubShapes(myObj, anArray);
  const int aNbObj = aList->length();

  for (i = 0; i < aNbObj; i++) {
    objects.push_back(GEOM::GEOM_Object::_duplicate(aList[i]));
  }

  return true;
}
