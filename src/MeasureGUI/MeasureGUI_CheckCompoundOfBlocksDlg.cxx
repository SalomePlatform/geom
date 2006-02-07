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
//  File   : MeasureGUI_CheckCompoundOfBlocksDlg.cxx
//  Author : VKN
//  Module : GEOM
//  $Header$

#include "MeasureGUI_CheckCompoundOfBlocksDlg.h"
#include "MeasureGUI_1Sel1TextView_QTD.h"

#include "utilities.h"
#include "SUIT_Session.h"
#include "SalomeApp_Tools.h"

#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>
#include <TColStd_MapOfInteger.hxx>
#include "GEOMBase.h"
#include "GEOMImpl_Types.hxx"

#include <qtextedit.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
// QT Includes
#include <qgroupbox.h>
#include <qlabel.h>
#include <qvaluelist.h>

//VRV: porting on Qt 3.0.5
#if QT_VERSION >= 0x030005
#include <qlistbox.h>
#endif
//VRV: porting on Qt 3.0.5

#define TEXTEDIT_FONT_FAMILY "Courier"
#define TEXTEDIT_FONT_SIZE 11

//=================================================================================
// class    : MeasureGUI_CheckCompoundOfBlocksDlg()
// purpose  : Constructs a MeasureGUI_CheckCompoundOfBlocksDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
MeasureGUI_CheckCompoundOfBlocksDlg::MeasureGUI_CheckCompoundOfBlocksDlg( GeometryGUI* GUI, QWidget* parent )
: MeasureGUI_Skeleton( GUI, parent, "MeasureGUI_CheckCompoundOfBlocksDlg" )
{
  QPixmap image0( SUIT_Session::session()->resourceMgr()->loadPixmap(
    "GEOM",tr( "ICON_DLG_CHECK_COMPOUND_OF_BLOCKS" ) ) );
  QPixmap image1( SUIT_Session::session()->resourceMgr()->loadPixmap(
    "GEOM",tr( "ICON_SELECT" ) ) );

  setCaption( tr( "GEOM_CHECK_BLOCKS_COMPOUND" ) );

  /***************************************************************/

  GroupConstructors->setTitle( tr( "GEOM_CHECK_BLOCKS_COMPOUND" ) );
  RadioButton1->setPixmap( image0 );

  myGrp = new MeasureGUI_1Sel1TextView_QTD( this, "myGrp" );
  myGrp->GroupBox1->setTitle( tr( "GEOM_CHECK_INFOS" ) );
  myGrp->TextLabel1->setText( tr( "GEOM_OBJECT" ) );
  myGrp->TextEdit1->setReadOnly( TRUE );
  
  QFont aFont( TEXTEDIT_FONT_FAMILY, TEXTEDIT_FONT_SIZE );
  aFont.setStyleHint( QFont::TypeWriter, QFont::PreferAntialias );
  myGrp->TextEdit1->setFont( aFont );
  myGrp->PushButton1->setPixmap( image1 );
  myGrp->LineEdit1->setReadOnly( true );


  /***************************************************************/
  QGridLayout* aGBLayout = new QGridLayout( myGrp->GroupBox1->layout() );
  aGBLayout->setAlignment( Qt::AlignTop );
  QGridLayout* Layout2 = new QGridLayout( 0, 1, 1, 0, 6, "Layout2"); 
  myErrorsLbl = new QLabel( tr( "GEOM_CHECK_BLOCKS_COMPOUND_ERRORS" ), myGrp, "Errors" );
  Layout2->addWidget( myErrorsLbl, 0, 0 );

  myErrorsLBox = new QListBox( myGrp, "ListBlockCompoundErrors" );
  myErrorsLBox->setMinimumSize( 100, 100 );
  Layout2->addWidget( myErrorsLBox, 1, 0 );


  mySubShapesLbl = new QLabel( tr( "GEOM_CHECK_BLOCKS_COMPOUND_SUBSHAPES" ), myGrp, "BlockCompoundSubShapes" );
  Layout2->addWidget( mySubShapesLbl, 0, 1 );

  mySubShapesLBox = new QListBox( myGrp, "ListSubShapes" );
  mySubShapesLBox->setMinimumSize( 100, 100 );
  mySubShapesLBox->setSelectionMode(QListBox::Extended);
  Layout2->addWidget( mySubShapesLBox, 1, 1 );
  aGBLayout->addLayout( Layout2, 1, 0 );

  Layout1->addWidget( myGrp, 1, 0 );
  connect( myErrorsLBox, SIGNAL( selectionChanged() ), SLOT( onErrorsListSelectionChanged() ) );
  connect( mySubShapesLBox, SIGNAL( selectionChanged() ), SLOT( onSubShapesListSelectionChanged() ) );
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
  activateSelection();
  mySelBtn = myGrp->PushButton1;
  mySelEdit = myGrp->LineEdit1;
  MeasureGUI_Skeleton::Init();
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
  else
  {
    try
    {
      GEOM::GEOM_IBlocksOperations::BCErrors_var aErrs;
      theIsCompoundOfBlocks =
	GEOM::GEOM_IBlocksOperations::_narrow( getOperation() )->CheckCompoundOfBlocks( myObj, aErrs );
      theErrors = aErrs;
    }
    catch( const SALOME::SALOME_Exception& e )
    {
      SalomeApp_Tools::QtCatchCorbaException( e );
      return false;
    }

    return getOperation()->IsDone();
  }
}

//=================================================================================
// function : processObject
// purpose  :
//=================================================================================
void MeasureGUI_CheckCompoundOfBlocksDlg::processObject()
{
  QString aMsg ("");
  bool isCompoundOfBlocks;
  GEOM::GEOM_IBlocksOperations::BCErrors aErrs;
  if ( !getBCErrors( isCompoundOfBlocks, aErrs ) )
  {
    myGrp->TextEdit1->setText( aMsg );
    myErrorsLBox->clear();
    mySubShapesLBox->clear();
    erasePreview();
    return;
  }

  if (isCompoundOfBlocks) {
    aMsg += tr( "GEOM_CHECK_BLOCKS_COMPOUND_HAS_NO_ERRORS" );
  } else {
    aMsg += tr( "GEOM_CHECK_BLOCKS_COMPOUND_HAS_ERRORS" );
  }
  myGrp->TextEdit1->setText(aMsg);
  QStringList aErrList;
  QString aErrStr( "" );
  QString aConSfx( " # " );
  QString aGluedSfx( " # " );
  int aConNum = 1;
  int aGluedNum = 1;
  for ( int i = 0, n = aErrs.length(); i < n; i++ )
  {
    aErrStr = "";
    switch ( aErrs[i].error )
    {
      case GEOM::GEOM_IBlocksOperations::NOT_BLOCK :
	aErrStr = "Not a Block";
        break;
      case GEOM::GEOM_IBlocksOperations::EXTRA_EDGE :
	aErrStr = "Extra Edge";
        break;
      case GEOM::GEOM_IBlocksOperations::INVALID_CONNECTION :
	aErrStr = "Invalid Connection";
	aErrStr += aConSfx;
	aErrStr += QString::number(aConNum);
	aConNum++;
        break;
      case GEOM::GEOM_IBlocksOperations::NOT_CONNECTED :
	aErrStr = "Not Connected";
        break;
      case GEOM::GEOM_IBlocksOperations::NOT_GLUED :
	aErrStr = "Not Glued";
	aErrStr += aGluedSfx;
	aErrStr += QString::number(aGluedNum);
	aGluedNum++;
        break;
      default :
	aErrStr = "";
	break;
    }
    if ( !aErrStr.isEmpty() )
      aErrList.append(aErrStr);
  }
  myErrorsLBox->clear();
  mySubShapesLBox->clear();
  myErrorsLBox->insertStringList(aErrList);
  return;
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr MeasureGUI_CheckCompoundOfBlocksDlg::createOperation()
{
  return getGeomEngine()->GetIBlocksOperations(getStudyId());
}

//=================================================================================
// function : onErrorsListSelectionChanged
// purpose  :
//=================================================================================
void MeasureGUI_CheckCompoundOfBlocksDlg::onErrorsListSelectionChanged()
{
  erasePreview();
  int aCurItem = myErrorsLBox->currentItem();
  if ( aCurItem < 0 )
    return;
  bool isCompoundOfBlocks;
  GEOM::GEOM_IBlocksOperations::BCErrors aErrs;
  if ( !getBCErrors( isCompoundOfBlocks, aErrs ) )
  {
    myGrp->TextEdit1->setText( "" );
    myErrorsLBox->clear();
    mySubShapesLBox->clear();
    return;
  }
  
  GEOM::GEOM_IBlocksOperations::BCError aErr = aErrs[aCurItem];
  GEOM::ListOfLong aObjLst = aErr.incriminated;
  TopoDS_Shape aSelShape;
  TopoDS_Shape aSubShape; 
  TopTools_IndexedMapOfShape anIndices;
  QStringList aSubShapeList;
  QString aSubShapeName("");
  Standard_CString aTypeString;
  if ( !myObj->_is_nil() && GEOMBase::GetShape( myObj, aSelShape ) )
  {
    TopExp::MapShapes( aSelShape, anIndices);
    for ( int i = 0, n = aObjLst.length(); i < n; i++ )
    {
      aSubShapeName = "";
      aSubShape = anIndices.FindKey(aObjLst[i]);
      if ( GEOMBase::GetShapeTypeString( aSubShape, aTypeString ) )	
	aSubShapeName = QString(aTypeString) + QString("_") + QString::number(aObjLst[i]);	
      if ( !aSubShapeName.isEmpty() )
      aSubShapeList.append(aSubShapeName);
    }
  }
  mySubShapesLBox->clear();
  mySubShapesLBox->insertStringList(aSubShapeList);
  return;
}

//=================================================================================
// function : onSubShapesListSelectionChanged
// purpose  :
//=================================================================================
void MeasureGUI_CheckCompoundOfBlocksDlg::onSubShapesListSelectionChanged()
{
  erasePreview();
  int aErrCurItem = myErrorsLBox->currentItem();
  if ( aErrCurItem < 0 )
    return;
  QValueList<int> aIds;
  for ( int i = 0, n = mySubShapesLBox->count(); i < n; i++ )
  {
    if ( mySubShapesLBox->isSelected( i ) ) 
      aIds.append( i );
  }
  if ( aIds.count() < 1 )
    return;
  bool isCompoundOfBlocks;
  GEOM::GEOM_IBlocksOperations::BCErrors aErrs;
  if ( !getBCErrors( isCompoundOfBlocks, aErrs ) )
  {
    myGrp->TextEdit1->setText( "" );
    myErrorsLBox->clear();
    mySubShapesLBox->clear();
    return;
  }
  
  GEOM::GEOM_IBlocksOperations::BCError aErr = aErrs[aErrCurItem];
  GEOM::ListOfLong aObjLst = aErr.incriminated;
  TopoDS_Shape aSelShape;
  TopoDS_Shape aSubShape; 
  TopTools_IndexedMapOfShape anIndices;
  if ( !myObj->_is_nil() && GEOMBase::GetShape( myObj, aSelShape ) )
  {
    QString aMess;
    if ( !isValid( aMess ) )
    {
      return;
    }
    SALOME_Prs* aPrs = 0;
    TopExp::MapShapes( aSelShape, anIndices);
    QValueList<int>::iterator it;
    for ( it = aIds.begin(); it != aIds.end(); ++it )
    {
      aSubShape = anIndices.FindKey(aObjLst[(*it)]);
      try
      {
        getDisplayer()->SetColor( Quantity_NOC_RED );
        getDisplayer()->SetWidth( 3 );
        getDisplayer()->SetToActivate( false );
        aPrs = !aSubShape.IsNull() ? getDisplayer()->BuildPrs( aSubShape ) : 0;
        if ( aPrs )
	  displayPreview( aPrs, true );
      }
      catch( const SALOME::SALOME_Exception& e )
      {
        SalomeApp_Tools::QtCatchCorbaException( e );
      }
    }
  }
  return;
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
