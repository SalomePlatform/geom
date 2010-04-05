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
// GEOM GEOMGUI : GUI for Geometry component
// File   : BuildGUI_WireDlg.cxx
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.

#include "BuildGUI_WireDlg.h"

#include <GEOMImpl_Types.hxx>

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>

#include <TColStd_MapOfInteger.hxx>
#include <Precision.hxx>

//=================================================================================
// class    : BuildGUI_WireDlg()
// purpose  : Constructs a BuildGUI_WireDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
BuildGUI_WireDlg::BuildGUI_WireDlg( GeometryGUI* theGeometryGUI, QWidget* parent )
  : GEOMBase_Skeleton( theGeometryGUI, parent )
{
  QPixmap image0( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_BUILD_WIRE" ) ) );
  QPixmap image1( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );

  setWindowTitle( tr( "GEOM_WIRE_TITLE" ) );

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle( tr( "GEOM_WIRE" ) );
  mainFrame()->RadioButton1->setIcon( image0 );
  mainFrame()->RadioButton2->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton3->close();

  GroupArgs = new DlgRef_1Sel1Spin( centralWidget() );

  GroupArgs->GroupBox1->setTitle( tr( "GEOM_WIRE_CONNECT" ) );
  GroupArgs->TextLabel1->setText( tr( "GEOM_OBJECTS" ) );
  GroupArgs->PushButton1->setIcon( image1 );
  GroupArgs->LineEdit1->setReadOnly( true );

  GroupArgs->TextLabel2->setText( tr( "GEOM_TOLERANCE" ) );
  double SpecificStep = 0.0001;
  double prec = Precision::Confusion();
  initSpinBox(GroupArgs->SpinBox_DX, prec, MAX_NUMBER, SpecificStep, "len_tol_precision" );
  GroupArgs->SpinBox_DX->setValue(prec);

  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( GroupArgs );
  /***************************************************************/

  setHelpFileName( "create_wire_page.html" );

  /* Initialisations */
  Init();
}


//=================================================================================
// function : ~BuildGUI_WireDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
BuildGUI_WireDlg::~BuildGUI_WireDlg()
{
  // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BuildGUI_WireDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupArgs->LineEdit1;
  GroupArgs->LineEdit1->setReadOnly( true );
  
  myOkEdgesAndWires = false;
  
  TColStd_MapOfInteger aMap;
  aMap.Add( GEOM_WIRE );
  aMap.Add( GEOM_EDGE );
  globalSelection( aMap );

  /* signals and slots connections */
  connect( buttonOk(),    SIGNAL( clicked() ), this, SLOT( ClickOnOk() ) );
  connect( buttonApply(), SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );
  connect( GroupArgs->PushButton1, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );
  connect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr(),
           SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );
  
  initName( tr( "GEOM_WIRE" ) );
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void BuildGUI_WireDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool BuildGUI_WireDlg::ClickOnApply()
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
void BuildGUI_WireDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText( "" );
  QString aString = ""; /* name of selection */

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  myOkEdgesAndWires = false;
  int nbSel = GEOMBase::GetNameOfSelectedIObjects(aSelList, aString);

  if ( nbSel == 0 )
    return;
  if ( nbSel != 1 )
    aString = tr( "%1_objects" ).arg( nbSel );

  GEOMBase::ConvertListOfIOInListOfGO(aSelList,  myEdgesAndWires);
  if ( !myEdgesAndWires.length() )
    return;

  myEditCurrentArgument->setText( aString );
  myOkEdgesAndWires = true;
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BuildGUI_WireDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();
  if ( send != GroupArgs->PushButton1 )
    return;

  TColStd_MapOfInteger aMap;
  aMap.Add( GEOM_WIRE );
  aMap.Add( GEOM_EDGE );
  globalSelection( aMap );
  myEditCurrentArgument = GroupArgs->LineEdit1;

  myEditCurrentArgument->setFocus();
  SelectionIntoArgument();
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void BuildGUI_WireDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr(),
           SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );
  TColStd_MapOfInteger aMap;
  aMap.Add( GEOM_WIRE );
  aMap.Add( GEOM_EDGE );
  globalSelection( aMap );
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void BuildGUI_WireDlg::enterEvent( QEvent* )
{
  if ( !mainFrame()->GroupConstructors->isEnabled() )
    ActivateThisDialog();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr BuildGUI_WireDlg::createOperation()
{
  return getGeomEngine()->GetIShapesOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool BuildGUI_WireDlg::isValid (QString& msg)
{
  bool ok = GroupArgs->SpinBox_DX->isValid(msg, !IsPreview());
  return myOkEdgesAndWires && ok;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool BuildGUI_WireDlg::execute (ObjectList& objects)
{
  GEOM::GEOM_IShapesOperations_var anOper = GEOM::GEOM_IShapesOperations::_narrow( getOperation() );
  GEOM::GEOM_Object_var anObj = anOper->MakeWire(myEdgesAndWires, GroupArgs->SpinBox_DX->value());

  if (!anObj->_is_nil())
    objects.push_back(anObj._retn());

  return true;
}
