//  Copyright (C) 2007-2010  CEA/DEN, EDF R&D, OPEN CASCADE
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
// File   : BuildGUI_SolidDlg.cxx
// Author : Damien COQUERET, Open CASCADE S.A.S.
//
#include "BuildGUI_SolidDlg.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <GEOMImpl_Types.hxx>

#include <SUIT_ResourceMgr.h>
#include <SUIT_MessageBox.h>
#include <SUIT_Session.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>

//=================================================================================
// class    : BuildGUI_SolidDlg()
// purpose  : Constructs a BuildGUI_SolidDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
BuildGUI_SolidDlg::BuildGUI_SolidDlg( GeometryGUI* theGeometryGUI, QWidget* parent )
  : GEOMBase_Skeleton( theGeometryGUI, parent )
{
  QPixmap image0( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_BUILD_SOLID" ) ) );
  QPixmap image1( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );

  setWindowTitle( tr( "GEOM_SOLID_TITLE" ) );
    
  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle( tr( "GEOM_SOLID" ) );
  mainFrame()->RadioButton1->setIcon( image0 );
  mainFrame()->RadioButton2->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton3->close();

  GroupSolid = new DlgRef_1Sel1Check( centralWidget() );

  GroupSolid->GroupBox1->setTitle( tr( "GEOM_ARGUMENTS" ) );
  GroupSolid->TextLabel1->setText( tr( "GEOM_OBJECTS" ) );
  GroupSolid->CheckButton1->setText( tr( "GEOM_CREATE_SINGLE_SOLID" ) );
  GroupSolid->PushButton1->setIcon( image1 );
  GroupSolid->LineEdit1->setReadOnly( true );
  
  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( GroupSolid );
  /***************************************************************/

  setHelpFileName("create_solid_page.html");

  /* Initialisations */
  Init();
}


//=================================================================================
// function : ~BuildGUI_SolidDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
BuildGUI_SolidDlg::~BuildGUI_SolidDlg()
{
  // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BuildGUI_SolidDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupSolid->LineEdit1;
  GroupSolid->LineEdit1->setReadOnly( true );
  GroupSolid->CheckButton1->setChecked( true );

  myOkShells = false;

  globalSelection( GEOM_SHELL );

  /* signals and slots connections */
  connect( buttonOk(),    SIGNAL( clicked() ), this, SLOT( ClickOnOk() ) );
  connect( buttonApply(), SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );

  connect( GroupSolid->PushButton1,  SIGNAL( clicked() ),       this, SLOT( SetEditCurrentArgument() ) );
  connect( GroupSolid->CheckButton1, SIGNAL( toggled( bool ) ), this, SLOT( EnableNameField( bool ) ) );

  connect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr(),
           SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );

  initName( tr( "GEOM_SOLID" ) );
  SelectionIntoArgument();
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void BuildGUI_SolidDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool BuildGUI_SolidDlg::ClickOnApply()
{
  for ( int i = 0, n = myShells.length(); i< n; i++ ) {
    if ( !isClosed(i)) {
      QString aName = GEOMBase::GetName( myShells[i] );
      SUIT_MessageBox::warning( (QWidget*)SUIT_Session::session()->activeApplication()->desktop(),
                                 QObject::tr("WRN_WARNING"),
                                 QObject::tr("WRN_SHAPE_UNCLOSED").arg(aName) ,
                                 QObject::tr("BUT_OK") );
       return false;
    }
  }

  if ( !onAccept() )
    return false;

  initName();
  return true;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void BuildGUI_SolidDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText( "" );
  QString aString = "";
  
  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  myOkShells = false;
  int nbSel = GEOMBase::GetNameOfSelectedIObjects(aSelList, aString);
  if ( nbSel == 0 )
    return;
  if ( nbSel != 1 )
    aString = tr( "%1_objects" ).arg( nbSel );
  
  GEOMBase::ConvertListOfIOInListOfGO(aSelList, myShells);
  if ( !myShells.length() )
    return;
  
  myEditCurrentArgument->setText( aString );
  myOkShells = true;
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BuildGUI_SolidDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();
  if ( send != GroupSolid->PushButton1 )
    return;
  
  globalSelection( GEOM_SHELL );
  myEditCurrentArgument = GroupSolid->LineEdit1;

  myEditCurrentArgument->setFocus();
  SelectionIntoArgument();
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void BuildGUI_SolidDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr(),
           SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );
  globalSelection( GEOM_SHELL );
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void BuildGUI_SolidDlg::enterEvent( QEvent* )
{
  if ( !mainFrame()->GroupConstructors->isEnabled() )
    ActivateThisDialog();
}


//=================================================================================
// function :  EnableNameField()
// purpose  :
//=================================================================================
void  BuildGUI_SolidDlg::EnableNameField( bool toEnable )
{
  mainFrame()->GroupBoxName->setEnabled( toEnable ); 
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr BuildGUI_SolidDlg::createOperation()
{
  return getGeomEngine()->GetIShapesOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool BuildGUI_SolidDlg::isValid( QString& )
{
  return myOkShells;
}

//=================================================================================
// function : isClosed
// purpose  : Check the object 'i' in myShells list is closed or unclosed
//=================================================================================
bool BuildGUI_SolidDlg::isClosed(int i)
{
  GEOM::GEOM_Object_var aShape = myShells[i];
  GEOM::GEOM_IKindOfShape::shape_kind aKind;
  GEOM::ListOfLong_var anInts;
  GEOM::ListOfDouble_var aDbls;

  if (aShape->_is_nil()) {
    MESSAGE ("Shape is NULL!!!");
    return false;
  }

  GEOM::GEOM_IMeasureOperations_var anOp = myGeomGUI->GetGeomGen()->GetIMeasureOperations( getStudyId() );

  // Detect kind of shape and parameters
  aKind = anOp->KindOfShape(aShape, anInts, aDbls);

  if ( !anOp->IsDone() ) {
    MESSAGE ("KindOfShape Operation is NOT DONE!!!");
    return false;
  }
  
  if ( anInts[0] == 1 )
    return true;
  else if ( anInts[0] == 2 )
    return false;
 
  return false;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool BuildGUI_SolidDlg::execute( ObjectList& objects )
{
  bool toCreateSingleSolid = GroupSolid->CheckButton1->isChecked();
  
  GEOM::GEOM_IShapesOperations_var anOper = GEOM::GEOM_IShapesOperations::_narrow( getOperation() );
    
  if ( toCreateSingleSolid ) {
    GEOM::GEOM_Object_var anObj = anOper->MakeSolidShells( myShells );

    if ( !anObj->_is_nil() )
      objects.push_back( anObj._retn() );
  }
  else {
    for ( int i = 0, n = myShells.length(); i< n; i++ ) {
      GEOM::GEOM_Object_var anObj = anOper->MakeSolidShell( myShells[ i ] );

     if ( !anObj->_is_nil() )
       objects.push_back( anObj._retn() );
    }
  }

  return true;
}

