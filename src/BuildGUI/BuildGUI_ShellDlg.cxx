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
// File   : BuildGUI_ShellDlg.cxx
// Author : Damien COQUERET, Open CASCADE S.A.S.
//
#include "BuildGUI_ShellDlg.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <GEOMImpl_Types.hxx>

#include <SUIT_MessageBox.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>

#include <TColStd_MapOfInteger.hxx>

//=================================================================================
// class    : BuildGUI_ShellDlg()
// purpose  : Constructs a BuildGUI_ShellDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
BuildGUI_ShellDlg::BuildGUI_ShellDlg( GeometryGUI* theGeometryGUI, QWidget* parent )
  : GEOMBase_Skeleton( theGeometryGUI, parent )
{
  QPixmap image0( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_BUILD_SHELL" ) ) );
  QPixmap image1( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );

  setWindowTitle( tr( "GEOM_SHELL_TITLE" ) );
    
  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle( tr( "GEOM_SHELL" ) );
  mainFrame()->RadioButton1->setIcon( image0 );
  mainFrame()->RadioButton2->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton3->close();

  GroupShell = new DlgRef_1Sel( centralWidget() );

  GroupShell->GroupBox1->setTitle( tr( "GEOM_ARGUMENTS" ) );
  GroupShell->TextLabel1->setText( tr( "GEOM_OBJECTS" ) );
  GroupShell->PushButton1->setIcon( image1 );
  GroupShell->LineEdit1->setReadOnly( true );
  
  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( GroupShell );
  /***************************************************************/

  setHelpFileName("create_shell_page.html");

  /* Initialisations */
  Init();
}


//=================================================================================
// function : ~BuildGUI_ShellDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
BuildGUI_ShellDlg::~BuildGUI_ShellDlg()
{
    // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BuildGUI_ShellDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupShell->LineEdit1;
  GroupShell->LineEdit1->setReadOnly( true );
  
  myFacesAndShells.clear();
  
  TColStd_MapOfInteger aMap;
  aMap.Add( GEOM_SHELL );
  aMap.Add( GEOM_FACE );
  aMap.Add( GEOM_COMPOUNDFILTER );
  
  QList<int> aSubShapes;
  aSubShapes.append( GEOM_FACE );
  globalSelection( aMap, aSubShapes );

  /* signals and slots connections */
  connect( buttonOk(),    SIGNAL( clicked() ), this, SLOT( ClickOnOk() ) );
  connect( buttonApply(), SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );

  connect( GroupShell->PushButton1, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );

  connect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr(),
           SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );

  initName( tr( "GEOM_SHELL" ) );
  SelectionIntoArgument();
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void BuildGUI_ShellDlg::ClickOnOk()
{
  setIsApplyAndClose( true );
  if ( ClickOnApply() )
    ClickOnCancel();
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool BuildGUI_ShellDlg::ClickOnApply()
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
void BuildGUI_ShellDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText( "" );

  QList<TopAbs_ShapeEnum> types;
  types << TopAbs_FACE << TopAbs_SHELL << TopAbs_COMPOUND;
  myFacesAndShells = getSelected( types, -1 );

  if ( !myFacesAndShells.isEmpty() ) {
    QString aName = myFacesAndShells.count() > 1 ? QString( "%1_objects").arg( myFacesAndShells.count() ) : GEOMBase::GetName( myFacesAndShells[0].get() );
    myEditCurrentArgument->setText( aName );
  }
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BuildGUI_ShellDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();
  if ( send != GroupShell->PushButton1 )
    return;

  TColStd_MapOfInteger aMap;
  aMap.Add( GEOM_SHELL );
  aMap.Add( GEOM_FACE );
  aMap.Add( GEOM_COMPOUNDFILTER );

  QList<int> aSubShapes;
  aSubShapes.append( GEOM_FACE );
  globalSelection( aMap, aSubShapes );
  
  myEditCurrentArgument = GroupShell->LineEdit1;

  myEditCurrentArgument->setFocus();
  SelectionIntoArgument();
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void BuildGUI_ShellDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr(),
           SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );
  TColStd_MapOfInteger aMap;
  aMap.Add( GEOM_SHELL );
  aMap.Add( GEOM_FACE );
  aMap.Add( GEOM_COMPOUNDFILTER );
  
  QList<int> aSubShapes;
  aSubShapes.append( GEOM_FACE );
  globalSelection( aMap, aSubShapes );
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void BuildGUI_ShellDlg::enterEvent( QEvent* )
{
  if ( !mainFrame()->GroupConstructors->isEnabled() )
    ActivateThisDialog();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr BuildGUI_ShellDlg::createOperation()
{
  return getGeomEngine()->GetIShapesOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool BuildGUI_ShellDlg::isValid( QString& )
{
  return !myFacesAndShells.isEmpty();
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool BuildGUI_ShellDlg::execute( ObjectList& objects )
{
  GEOM::GEOM_IShapesOperations_var anOper = GEOM::GEOM_IShapesOperations::_narrow( getOperation() );

  GEOM::ListOfGO_var objlist = new GEOM::ListOfGO();
  objlist->length( myFacesAndShells.count() );
  for ( int i = 0; i < myFacesAndShells.count(); i++ )
    objlist[i] = myFacesAndShells[i].copy();

  GEOM::GEOM_Object_var anObj = anOper->MakeShell( objlist.in() );

  if ( !anObj->_is_nil() ) {
    if (anObj->GetShapeType() == GEOM::COMPOUND) {
      SUIT_MessageBox::warning(this,
                               QObject::tr("GEOM_WRN_WARNING"),
                               QObject::tr("GEOM_WRN_FACES_NOT_SHELL"));
    }
    objects.push_back( anObj._retn() );
  }

  return true;
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> BuildGUI_ShellDlg::getSourceObjects()
{
  return myFacesAndShells;
}

