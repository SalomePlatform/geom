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

//  GEOM GEOMGUI : GUI for Geometry component
// File   : BuildGUI_CompoundDlg.cxx
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.
//
#include "BuildGUI_CompoundDlg.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <GEOMImpl_Types.hxx>

#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>

//=================================================================================
// class    : BuildGUI_CompoundDlg()
// purpose  : Constructs a BuildGUI_CompoundDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
BuildGUI_CompoundDlg::BuildGUI_CompoundDlg( GeometryGUI* theGeometryGUI, QWidget* parent )
  : GEOMBase_Skeleton( theGeometryGUI, parent )
{
  QPixmap image0( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_BUILD_COMPOUND" ) ) );
  QPixmap image1( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );

  setWindowTitle( tr( "GEOM_COMPOUND_TITLE" ) );

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle( tr( "GEOM_COMPOUND" ) );
  mainFrame()->RadioButton1->setIcon( image0 );
  mainFrame()->RadioButton2->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton3->close();

  GroupShapes = new DlgRef_1Sel( centralWidget() );

  GroupShapes->GroupBox1->setTitle( tr( "GEOM_ARGUMENTS" ) );
  GroupShapes->TextLabel1->setText( tr( "GEOM_OBJECTS" ) );
  GroupShapes->PushButton1->setIcon( image1 );
  GroupShapes->LineEdit1->setReadOnly( true );

  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( GroupShapes );
  /***************************************************************/

  setHelpFileName("create_compound_page.html");

  /* Initialisations */
  Init();
}


//=================================================================================
// function : ~BuildGUI_CompoundDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
BuildGUI_CompoundDlg::~BuildGUI_CompoundDlg()
{
  // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BuildGUI_CompoundDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupShapes->LineEdit1;
  GroupShapes->LineEdit1->setReadOnly( true );
  
  myShapes.clear();

  mainFrame()->GroupBoxPublish->show();
  mainFrame()->CheckBoxPreview->hide();

  /* signals and slots connections */
  connect( buttonOk(),    SIGNAL( clicked() ), this, SLOT( ClickOnOk() ) );
  connect( buttonApply(), SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );
  connect( GroupShapes->PushButton1, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );
  connect( ( (SalomeApp_Application*)(SUIT_Session::session()->activeApplication() ) )->selectionMgr(), 
           SIGNAL(currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );

  globalSelection( GEOM_ALLSHAPES );

  initName( tr( "GEOM_COMPOUND" ) );
  SelectionIntoArgument();
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void BuildGUI_CompoundDlg::ClickOnOk()
{
  setIsApplyAndClose( true );
  if ( ClickOnApply() )
    ClickOnCancel();
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool BuildGUI_CompoundDlg::ClickOnApply()
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
void BuildGUI_CompoundDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText( "" );

  myShapes = getSelected( TopAbs_SHAPE, -1 );

  if ( !myShapes.isEmpty() ) {
    QString aName = myShapes.count() > 1 ? QString( "%1_objects").arg( myShapes.count() ) : GEOMBase::GetName( myShapes[0].get() );
    myEditCurrentArgument->setText( aName );
  }
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BuildGUI_CompoundDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();
  if ( send != GroupShapes->PushButton1 )
    return;
  
  myEditCurrentArgument = GroupShapes->LineEdit1;
  
  myEditCurrentArgument->setFocus();
  SelectionIntoArgument();
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void BuildGUI_CompoundDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  globalSelection( GEOM_ALLSHAPES );
  connect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr(),
           SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void BuildGUI_CompoundDlg::enterEvent(QEvent* e)
{
  if ( !mainFrame()->GroupConstructors->isEnabled() )
    ActivateThisDialog(); 
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr BuildGUI_CompoundDlg::createOperation()
{
  return getGeomEngine()->GetIShapesOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool BuildGUI_CompoundDlg::isValid( QString& )
{
  return !myShapes.isEmpty();
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool BuildGUI_CompoundDlg::execute( ObjectList& objects )
{
  GEOM::GEOM_IShapesOperations_var anOper = GEOM::GEOM_IShapesOperations::_narrow( getOperation() );

  GEOM::ListOfGO_var objlist = new GEOM::ListOfGO();
  objlist->length( myShapes.count() );
  for ( int i = 0; i < myShapes.count(); i++ )
    objlist[i] = myShapes[i].copy();

  GEOM::GEOM_Object_var anObj = anOper->MakeCompound( objlist );

  if ( !anObj->_is_nil() )
    objects.push_back( anObj._retn() );

  return true;
}

//=================================================================================
// function : restoreSubShapes
// purpose  :
//=================================================================================
void BuildGUI_CompoundDlg::restoreSubShapes( SALOMEDS::Study_ptr   theStudy,
                                             SALOMEDS::SObject_ptr theSObject )
{
  if ( mainFrame()->CheckBoxRestoreSS->isChecked() ) {
    // empty list of arguments means that all arguments should be restored
    getGeomEngine()->RestoreSubShapesSO( theStudy, theSObject, GEOM::ListOfGO(),
                                         /*theFindMethod=*/GEOM::FSM_GetInPlace, // ? GEOM::FSM_GetSame
                                         /*theInheritFirstArg=*/false,
                                         mainFrame()->CheckBoxAddPrefix->isChecked() );
  }
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> BuildGUI_CompoundDlg::getSourceObjects()
{
  return myShapes;
}
