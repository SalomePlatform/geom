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
//  File   : RepairGUI_FreeFacesDlg.cxx
//  Author : VKN
//  Module : GEOM
//  $Header$

using namespace std;
#include "RepairGUI_FreeFacesDlg.h"

#include "QAD_Desktop.h"
#include "QAD_WaitCursor.h"

#include "SALOME_ListIteratorOfListIO.hxx"

#include "GEOMImpl_Types.hxx"
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>
#include "GEOMBase.h"
#include "GeometryGUI.h"
#include "GEOM_Displayer.h"
#include "SALOMEGUI_QtCatchCorbaException.hxx"
#include "SALOME_Selection.h"
#include "SALOME_Prs.h"


#include <qlineedit.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qgroupbox.h>
#include <qpushbutton.h>
#define SPACING 5
#define MARGIN 10
#define MIN_WIDTH 200


//=================================================================================
// class    : RepairGUI_FreeFacesDlg()
// purpose  : Constructs a RepairGUI_FreeFacesDlg  which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
RepairGUI_FreeFacesDlg::RepairGUI_FreeFacesDlg(QWidget* parent, const char* name, SALOME_Selection* theSelection, bool modal, WFlags fl)
:QDialog( parent, "RepairGUI_FreeBoundDlg", false,
    WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu | WDestructiveClose )
{
  myDisplayer = 0;

  setSizeGripEnabled( TRUE );
  QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_FREE_FACES_TITLE"));

  /***************************************************************/

  QGroupBox* aMainGrp = new QGroupBox( 1, Qt::Horizontal, tr( "GEOM_SELECTED_SHAPE" ), this );
  

  QGroupBox* aSelGrp = new QGroupBox( 1, Qt::Vertical, aMainGrp );

  aSelGrp->setInsideMargin( 0 );
  aSelGrp->setFrameStyle( QFrame::NoFrame );
  new QLabel( tr( "GEOM_OBJECT" ), aSelGrp );
  mySelBtn = new QPushButton( aSelGrp );
  mySelBtn->setPixmap( image1 );
  myEdit = new QLineEdit( aSelGrp );
  myEdit->setReadOnly( true );
  myEdit->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  myEdit->setMinimumWidth( MIN_WIDTH );

  QFrame* aFrame = new QFrame( this );
  aFrame->setFrameStyle( QFrame::Box | QFrame::Sunken );
  QPushButton* aCloseBtn = new QPushButton( tr( "GEOM_BUT_CLOSE" ), aFrame );
  QHBoxLayout* aBtnLay = new QHBoxLayout( aFrame, MARGIN, SPACING );
  aBtnLay->addItem( new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum ) );
  aBtnLay->addWidget( aCloseBtn );
  aBtnLay->addItem( new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum ) );

  QVBoxLayout* aLay = new QVBoxLayout( this );
  aLay->setSpacing( SPACING );
  aLay->setMargin( MARGIN );
  aLay->addWidget( aMainGrp );
  aLay->addItem( new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum ) );
  aLay->addWidget( aFrame );
  
  connect( aCloseBtn, SIGNAL( clicked() ), SLOT( onClose() ) );
  connect( mySelBtn,    SIGNAL( clicked() ),
           this,        SLOT  ( onSetEditCurrentArgument() ) );
  /***************************************************************/

  Init( theSelection );
}


//=================================================================================
// function : ~RepairGUI_FreeFacesDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
RepairGUI_FreeFacesDlg::~RepairGUI_FreeFacesDlg()
{
}


//=================================================================================
// function : onClose
// purpose  : SLOT. Called when "close" button pressed. Close dialog
//=================================================================================
void RepairGUI_FreeFacesDlg::onClose()
{
  globalSelection();
  disconnect( mySelection, 0, this, 0 );
  GeometryGUI::GetGeomGUI()->SetActiveDialogBox( 0 );
  reject();
  erasePreview();
}

//=================================================================================
// function : onDeactivate
// purpose  : Deactivate this dialog
//=================================================================================
void RepairGUI_FreeFacesDlg::onDeactivate()
{
  setEnabled(false);
  globalSelection();
  disconnect( mySelection, 0, this, 0 );
  GeometryGUI::GetGeomGUI()->SetActiveDialogBox( 0 );
}

//=================================================================================
// function : onActivate
// purpose  : Activate this dialog
//=================================================================================
void RepairGUI_FreeFacesDlg::onActivate()
{
  GeometryGUI::GetGeomGUI()->EmitSignalDeactivateDialog();
  setEnabled( true );
  GeometryGUI::GetGeomGUI()->SetActiveDialogBox( this );
  connect( mySelection, SIGNAL( currentSelectionChanged() ), SLOT  ( onSelectionDone() ) );
  activateSelection();
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void RepairGUI_FreeFacesDlg::Init(SALOME_Selection* theSel)
{
  myObj = GEOM::GEOM_Object::_nil();

  mySelection = theSel;

  /* signals and slots connections */
  GeometryGUI* aGeomGUI = GeometryGUI::GetGeomGUI();
  connect( aGeomGUI, SIGNAL( SignalDeactivateActiveDialog() ), SLOT  ( onDeactivate() ) );
  connect( mySelection, SIGNAL( currentSelectionChanged() ), SLOT  ( onSelectionDone() ) );

  activateSelection();
  onSelectionDone();
}

//=================================================================================
// function : onSelectionDone
// purpose  : SLOT. Called when selection changed.
//=================================================================================
void RepairGUI_FreeFacesDlg::onSelectionDone()
{
  erasePreview();
  if( mySelection->IObjectCount() != 1 )
  {
    myEdit->setText( "" );
    return;
  }

  Standard_Boolean isOk = Standard_False;
  GEOM::GEOM_Object_var anObj =
    GEOMBase::ConvertIOinGEOMObject( mySelection->firstIObject(), isOk );

  if ( !isOk || anObj->_is_nil() || !GEOMBase::IsShape( anObj ) )
  {
    myEdit->setText( "" );
    return;
  }
  else
  {
    myObj = anObj;
    displayPreview( false, true, true, 3 );
  }
}

//=================================================================================
// function : enterEvent()
// purpose  : Mouse enter onto the dialog to activate it
//=================================================================================
void RepairGUI_FreeFacesDlg::enterEvent(QEvent* e)
{
  onActivate();
}

//=================================================================================
// function : activateSelection
// purpose  : activate selection of faces, shells, and solids
//=================================================================================
void RepairGUI_FreeFacesDlg::activateSelection()
{
  TColStd_MapOfInteger aMap;
  aMap.Add( GEOM_SOLID );
  aMap.Add( GEOM_COMPOUND );
  globalSelection( aMap );
}

//=================================================================================
// function : closeEvent()
// purpose  :
//=================================================================================
void RepairGUI_FreeFacesDlg::closeEvent(QCloseEvent* e)
{
  onClose();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr RepairGUI_FreeFacesDlg::createOperation()
{
  return getGeomEngine()->GetIShapesOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool RepairGUI_FreeFacesDlg::isValid( QString& msg )
{
  return !myObj->_is_nil() ;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool RepairGUI_FreeFacesDlg::execute( ObjectList& objects )
{
  bool aResult = false;
  GEOM::ListOfLong_var aFaceLst = 
    GEOM::GEOM_IShapesOperations::_narrow( getOperation() )->GetFreeFacesIDs( myObj );
  TopoDS_Shape aSelShape;
  TopoDS_Shape aFace; 
  TopTools_IndexedMapOfShape anIndices;
  if ( !myObj->_is_nil() && GEOMBase::GetShape( myObj, aSelShape ) )
  {
    myEdit->setText( GEOMBase::GetName( myObj ) );
    QString aMess;
    if ( !isValid( aMess ) )
    {
      erasePreview( true );
      return false;
    }

    TopExp::MapShapes( aSelShape, anIndices);
    SALOME_Prs* aPrs = 0;
    QAD_WaitCursor wc;

    for ( int i = 0, n = aFaceLst->length(); i < n; i++ )
    {
      aFace = anIndices.FindKey( aFaceLst[i] );
      try
      {
        getDisplayer()->SetColor( Quantity_NOC_RED );
        getDisplayer()->SetToActivate( false );
	aPrs = !aFace.IsNull() ? getDisplayer()->BuildPrs( aFace ) : 0;
        if ( aPrs )
	  displayPreview( aPrs, true );
      }
      catch( const SALOME::SALOME_Exception& e )
      {
        QtCatchCorbaException( e );
      }
    }
  }
  return aResult;
}

//================================================================
// Function : getDisplayer
// Purpose  :
//================================================================
GEOM_Displayer* RepairGUI_FreeFacesDlg::getDisplayer()
{
  if ( !myDisplayer )
    myDisplayer = new GEOM_Displayer();
  return myDisplayer;
}

//=================================================================================
// function : SetEditCurrentArgument
// purpose  :
//=================================================================================
void RepairGUI_FreeFacesDlg::onSetEditCurrentArgument()
{
  myEdit->setFocus();
  onSelectionDone();
}

