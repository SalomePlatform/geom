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
// File   : BuildGUI_FaceDlg.cxx
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.

#include "BuildGUI_FaceDlg.h"

#include <GEOMImpl_Types.hxx>
#include <TColStd_MapOfInteger.hxx>

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>

#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SUIT_MessageBox.h>
#include <SUIT_OverrideCursor.h>

//=================================================================================
// class    : BuildGUI_FaceDlg()
// purpose  : Constructs a BuildGUI_FaceDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
BuildGUI_FaceDlg::BuildGUI_FaceDlg( GeometryGUI* theGeometryGUI, QWidget* parent )
  : GEOMBase_Skeleton( theGeometryGUI, parent )
{
  QPixmap image0( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_BUILD_FACE" ) ) );
  QPixmap image1( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );

  setWindowTitle( tr( "GEOM_FACE_TITLE" ) );

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle( tr( "GEOM_FACE" ) );
  mainFrame()->RadioButton1->setIcon( image0 );
  mainFrame()->RadioButton2->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton3->close();

  GroupWire = new DlgRef_1Sel1Check( centralWidget() );

  GroupWire->GroupBox1->setTitle( tr( "GEOM_FACE_FFW" ) );
  GroupWire->TextLabel1->setText( tr( "GEOM_OBJECTS" ) );
  GroupWire->CheckButton1->setText( tr( "GEOM_FACE_OPT" ) );
  GroupWire->PushButton1->setIcon( image1 );

  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( GroupWire );
  /***************************************************************/

  setHelpFileName("create_face_page.html");

  /* Initialisations */
  Init();
}


//=================================================================================
// function : ~BuildGUI_FaceDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
BuildGUI_FaceDlg::~BuildGUI_FaceDlg()
{
  // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BuildGUI_FaceDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupWire->LineEdit1;
  GroupWire->LineEdit1->setReadOnly( true );

  GroupWire->CheckButton1->setChecked( true );
  myWires.clear();

  TColStd_MapOfInteger aMap;
  aMap.Add( GEOM_EDGE );
  aMap.Add( GEOM_WIRE );
  globalSelection( aMap );

  /* signals and slots connections */
  connect( buttonOk(),    SIGNAL( clicked() ), this, SLOT( ClickOnOk() ) );
  connect( buttonApply(), SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );
  connect( GroupWire->LineEdit1,   SIGNAL( returnPressed()), this, SLOT( LineEditReturnPressed() ) );
  connect( GroupWire->PushButton1, SIGNAL( clicked() ),      this, SLOT( SetEditCurrentArgument() ) );
  connect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr(),
           SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );

  initName( tr( "GEOM_FACE" ) );
  SelectionIntoArgument();
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void BuildGUI_FaceDlg::ClickOnOk()
{
  setIsApplyAndClose( true );
  if ( ClickOnApply() )
    ClickOnCancel();
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool BuildGUI_FaceDlg::ClickOnApply()
{
  if ( !onAccept() )
    return false;

  initName();
  return true;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void BuildGUI_FaceDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText( "" );

  QList<TopAbs_ShapeEnum> types;
  types << TopAbs_EDGE << TopAbs_WIRE;
  myWires = getSelected( types, -1 );

  if ( !myWires.isEmpty() ) {
    QString aName = myWires.count() > 1 ? QString( "%1_objects").arg( myWires.count() ) : GEOMBase::GetName( myWires[0].get() );
    myEditCurrentArgument->setText( aName );
  }
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BuildGUI_FaceDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();
  if ( send != GroupWire->PushButton1 )
    return;
  
  TColStd_MapOfInteger aMap;
  aMap.Add( GEOM_EDGE );
  aMap.Add( GEOM_WIRE );
  globalSelection( aMap );

  myEditCurrentArgument = GroupWire->LineEdit1;

  myEditCurrentArgument->setFocus();
  SelectionIntoArgument();
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void BuildGUI_FaceDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr(),
           SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );
  TColStd_MapOfInteger aMap;
  aMap.Add( GEOM_EDGE );
  aMap.Add( GEOM_WIRE );
  globalSelection( aMap );
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void BuildGUI_FaceDlg::enterEvent( QEvent* )
{
  if ( !mainFrame()->GroupConstructors->isEnabled() )
    ActivateThisDialog(); 
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr BuildGUI_FaceDlg::createOperation()
{
  return getGeomEngine()->GetIShapesOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool BuildGUI_FaceDlg::isValid( QString& )
{
  return !myWires.isEmpty();
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool BuildGUI_FaceDlg::execute( ObjectList& objects )
{
  GEOM::GEOM_IShapesOperations_var anOper = GEOM::GEOM_IShapesOperations::_narrow( getOperation() );

  GEOM::ListOfGO_var objlist = new GEOM::ListOfGO();
  objlist->length( myWires.count() );
  for ( int i = 0; i < myWires.count(); i++ )
    objlist[i] = myWires[i].copy();

  GEOM::GEOM_Object_var anObj = anOper->MakeFaceWires( objlist.in(), GroupWire->CheckButton1->isChecked() );

  if (!anObj->_is_nil()) {
    objects.push_back(anObj._retn());

    if (!anOper->IsDone() && QString(anOper->GetErrorCode()) == "MAKE_FACE_TOLERANCE_TOO_BIG") {
      SUIT_OverrideCursor wc;
      wc.suspend();
      QString msgw = QObject::tr(anOper->GetErrorCode());
      SUIT_MessageBox::warning(this, tr("WRN_WARNING"), msgw, tr("BUT_OK"));
      anOper->SetErrorCode("PAL_NO_ERROR");
    }
  }

  return true;
}
