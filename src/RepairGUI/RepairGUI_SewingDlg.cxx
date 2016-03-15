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
// File   : RepairGUI_SewingDlg.cxx
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.
//
#include "RepairGUI_SewingDlg.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>
#include <SalomeApp_DoubleSpinBox.h>

#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>
#include <SUIT_Session.h>
#include <SUIT_MessageBox.h>
#include <SUIT_ResourceMgr.h>

#include <GEOMImpl_Types.hxx>

#include <TopAbs.hxx>
#include <TColStd_MapOfInteger.hxx>

#define DEFAULT_TOLERANCE_VALUE 1e-07

//=================================================================================
// class    : RepairGUI_SewingDlg()
// purpose  : Constructs a RepairGUI_SewingDlg  which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
RepairGUI_SewingDlg::RepairGUI_SewingDlg( GeometryGUI* theGeometryGUI, QWidget* parent,
                                          bool modal )
  : GEOMBase_Skeleton( theGeometryGUI, parent, modal )
{
  QPixmap image0( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_SEWING" ) ) );
  QPixmap image1( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );

  setWindowTitle( tr( "GEOM_SEWING_TITLE" ) );

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle( tr( "GEOM_SEWING_TITLE" ) );
  mainFrame()->RadioButton1->setIcon( image0 );
  mainFrame()->RadioButton2->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton3->close();

  GroupPoints = new DlgRef_1SelExt( centralWidget() );
  GroupPoints->GroupBox1->setTitle( tr( "GEOM_SEWING" ) );
  GroupPoints->TextLabel1->setText( tr( "GEOM_SELECTED_SHAPES" ) );
  GroupPoints->PushButton1->setIcon( image1 );
  GroupPoints->LineEdit1->setReadOnly( true );

  QGridLayout* aLay = new QGridLayout( GroupPoints->Box );
  aLay->setMargin( 0 ); aLay->setSpacing( 6 );
  myAllowNonManifoldChk = new QCheckBox (tr("GEOM_ALLOW_NON_MANIFOLD"), GroupPoints->Box);
  myTolEdt = new SalomeApp_DoubleSpinBox( GroupPoints->Box );
  initSpinBox( myTolEdt, 0.0, 100.0, DEFAULT_TOLERANCE_VALUE, "len_tol_precision" );
  myTolEdt->setValue( DEFAULT_TOLERANCE_VALUE );
  QLabel* aLbl1 = new QLabel( tr( "GEOM_TOLERANCE" ), GroupPoints->Box );
  myFreeBoundBtn = new QPushButton( tr( "GEOM_DETECT" ) + QString( " [%1]" ).arg( tr( "GEOM_FREE_BOUNDARIES" ) ), 
                                    GroupPoints->Box );
  aLay->addWidget( myAllowNonManifoldChk, 0, 0 );
  aLay->addWidget( aLbl1,          1, 0 );
  aLay->addWidget( myTolEdt,       1, 1 );
  aLay->addWidget( myFreeBoundBtn, 2, 0, 1, 2 );

  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( GroupPoints );
  /***************************************************************/

  setHelpFileName( "sewing_operation_page.html" );

  Init();
}


//=================================================================================
// function : ~RepairGUI_SewingDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
RepairGUI_SewingDlg::~RepairGUI_SewingDlg()
{
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void RepairGUI_SewingDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupPoints->LineEdit1;

  myObjects.clear();

  //myGeomGUI->SetState( 0 );
  initSelection();

  myClosed = -1;
  myOpen = -1;
        
  /* signals and slots connections */
  connect( buttonOk(),    SIGNAL( clicked() ), this, SLOT( ClickOnOk() ) );
  connect( buttonApply(), SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );

  connect( GroupPoints->PushButton1, SIGNAL( clicked() ),       this, SLOT( SetEditCurrentArgument() ) );
  connect( GroupPoints->LineEdit1,   SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );

  connect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr(),
           SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );

  connect( myFreeBoundBtn, SIGNAL( clicked() ), this, SLOT( onDetect() ) );

  initName( tr( "SEWING_NEW_OBJ_NAME" ) );
  resize(100,100);
  SelectionIntoArgument();
}


//=================================================================================
// function : ClickOnOk()
// purpose  : Same than click on apply but close this dialog.
//=================================================================================
void RepairGUI_SewingDlg::ClickOnOk()
{
  setIsApplyAndClose( true );
  if ( ClickOnApply() )
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool RepairGUI_SewingDlg::ClickOnApply()
{
  if ( !onAccept() )
        return false;

  initName();

  GroupPoints->LineEdit1->setText( "" );
  myObjects.clear();

  initSelection();

  return true;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection
//=================================================================================
void RepairGUI_SewingDlg::SelectionIntoArgument()
{
  erasePreview();
  myEditCurrentArgument->setText( "" );
  myObjects.clear();

  myObjects = getSelected( TopAbs_SHAPE, -1 );

  if ( !myObjects.isEmpty() ) {
    QString aName = myObjects.count() > 1 ? QString( "%1_objects").arg( myObjects.count() ) : GEOMBase::GetName( myObjects[0].get() );
    myEditCurrentArgument->setText( aName );
  }
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void RepairGUI_SewingDlg::SetEditCurrentArgument()
{
  const QObject* send = sender();
  if ( send == GroupPoints->PushButton1 ) {
    myEditCurrentArgument->setFocus();
    SelectionIntoArgument();
  }
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void RepairGUI_SewingDlg::LineEditReturnPressed()
{
  const QObject* send = sender();
  if( send == GroupPoints->LineEdit1 ) {
    myEditCurrentArgument = GroupPoints->LineEdit1;
    GEOMBase_Skeleton::LineEditReturnPressed();
  }
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void RepairGUI_SewingDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr(), 
           SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );

  GroupPoints->LineEdit1->setText( "" );
  myObjects.clear();

  myClosed = -1;
  myOpen = -1;

  //myGeomGUI->SetState( 0 );
  initSelection();
}


//=================================================================================
// function : enterEvent()
// purpose  : Mouse enter onto the dialog to activate it
//=================================================================================
void RepairGUI_SewingDlg::enterEvent( QEvent* )
{
  if ( !mainFrame()->GroupConstructors->isEnabled() )
    ActivateThisDialog();
}


//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr RepairGUI_SewingDlg::createOperation()
{
  return getGeomEngine()->GetIHealingOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool RepairGUI_SewingDlg::isValid( QString& msg )
{
  myClosed = -1;
  bool ok = myTolEdt->isValid( msg, !IsPreview() );
  return !myObjects.isEmpty() && ( IsPreview() || myTolEdt->value() > 0. ) && ok;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool RepairGUI_SewingDlg::execute( ObjectList& objects )
{
  bool aResult = false;
  GEOM::GEOM_IHealingOperations_var anOper = GEOM::GEOM_IHealingOperations::_narrow( getOperation() );

  GEOM::ListOfGO_var objList = new GEOM::ListOfGO;
  objList->length( myObjects.count() );
  for ( int i = 0; i < myObjects.count(); ++i )
    objList[i] = myObjects[i].copy();

  if ( IsPreview() ) { // called from onDetect(): detect free boundary edges, highlight them (add to objects), display message dialog
    GEOM::ListOfGO_var aClosed, anOpen;

    aResult = anOper->GetFreeBoundary( objList, aClosed, anOpen );

    if ( aResult ) {
      myClosed = aClosed->length();
      myOpen = anOpen->length();
      int i;
      for ( i = 0; i < myClosed; i++ )
        objects.push_back( aClosed[i]._retn() );
      for ( i = 0; i < myOpen; i++ )
        objects.push_back( anOpen[i]._retn() );
    }
    else
      myClosed = -1;
  }
  else {
    GEOM::GEOM_Object_var anObj;

    if (myAllowNonManifoldChk->isChecked()) {
      anObj = anOper->SewAllowNonManifold( objList, myTolEdt->value() );
    } else {
      anObj = anOper->Sew( objList, myTolEdt->value() );
    }

    aResult = !anObj->_is_nil();
    if ( aResult )
    {
      if ( !IsPreview() )
      {
        QStringList aParameters;
        aParameters << myTolEdt->text();
        anObj->SetParameters(aParameters.join(":").toLatin1().constData());
      }
      objects.push_back( anObj._retn() );
    }
  }

  return aResult;
}

//=================================================================================
// function : initSelection
// purpose  :
//=================================================================================
void RepairGUI_SewingDlg::initSelection()
{
  TColStd_MapOfInteger aTypes;
  aTypes.Add( GEOM_FACE );
  aTypes.Add( GEOM_SHELL );
  aTypes.Add( GEOM_SOLID );
  aTypes.Add( GEOM_COMPOUND );
  globalSelection( aTypes );
}

//=================================================================================
// function : onDetect
// purpose  :
//=================================================================================
void RepairGUI_SewingDlg::onDetect()
{
  displayPreview( true, false, true, true, 3 );

  // field myClosed,myOpen is initialized in execute() method, called by displayPreview().
  QString msg;
  if ( myClosed != -1 )
    msg = tr( "GEOM_FREE_BOUNDS_MSG" ).arg( myClosed + myOpen ).arg( myClosed ).arg( myOpen );
  else
    msg = tr( "GEOM_FREE_BOUNDS_ERROR" );
  SUIT_MessageBox::information( this, tr( "GEOM_FREE_BOUNDS_TLT" ), msg );
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> RepairGUI_SewingDlg::getSourceObjects()
{
  return myObjects;
}
