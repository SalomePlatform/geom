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
// File   : BuildGUI_SolidDlg.cxx
// Author : Damien COQUERET, Open CASCADE S.A.S.

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

#include <TColStd_MapOfInteger.hxx>

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
  QPixmap image0( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );
  QPixmap image1( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_BUILD_SOLID" ) ) );
  QPixmap image2( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_SOLID_FROM_FACES" ) ) );

  setWindowTitle( tr( "GEOM_SOLID_TITLE" ) );

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle( tr( "GEOM_SOLID" ) );
  mainFrame()->RadioButton1->setIcon( image1 );
  mainFrame()->RadioButton2->setIcon( image2 );
  mainFrame()->RadioButton3->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton3->close();

  GroupSolid = new DlgRef_1Sel1Check( centralWidget() );
  GroupSolid->GroupBox1->setTitle( tr( "GEOM_SOLID_SHELLS" ) );
  GroupSolid->TextLabel1->setText( tr( "GEOM_OBJECTS" ) );
  GroupSolid->CheckButton1->setText( tr( "GEOM_CREATE_SINGLE_SOLID" ) );
  GroupSolid->PushButton1->setIcon( image0 );
  GroupSolid->LineEdit1->setReadOnly( true );

  GroupFaces = new DlgRef_1Sel1Check( centralWidget() );
  GroupFaces->GroupBox1->setTitle( tr( "GEOM_SOLID_FACES" ) );
  GroupFaces->TextLabel1->setText( tr( "GEOM_OBJECTS" ) );
  GroupFaces->CheckButton1->setText( tr( "GEOM_SOLID_FROM_FACE_OPT" ) );
  GroupFaces->PushButton1->setIcon( image0 );
  GroupFaces->LineEdit1->setReadOnly( true );

  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( GroupSolid );
  layout->addWidget( GroupFaces );
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

  myShells.clear();

  //globalSelection( GEOM_SHELL );
  TColStd_MapOfInteger aMap;
  aMap.Add( GEOM_SHELL );
  aMap.Add( GEOM_COMPOUNDFILTER );
  QList<int> aSubShapes;
  aSubShapes.append( GEOM_SHELL );
  globalSelection( aMap, aSubShapes );

  /* signals and slots connections */
  connect( buttonOk(),    SIGNAL( clicked() ), this, SLOT( ClickOnOk() ) );
  connect( buttonApply(), SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );

  connect(this,      SIGNAL(constructorsClicked(int)), this, SLOT(ConstructorsClicked(int)));

  connect( GroupSolid->PushButton1,  SIGNAL( clicked() ),       this, SLOT( SetEditCurrentArgument() ) );
  connect( GroupFaces->PushButton1,  SIGNAL( clicked() ),       this, SLOT( SetEditCurrentArgument() ) );
  connect( GroupSolid->CheckButton1, SIGNAL( toggled( bool ) ), this, SLOT( EnableNameField( bool ) ) );

  connect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr(),
           SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );

  initName( tr( "GEOM_SOLID" ) );
  SelectionIntoArgument();

  ConstructorsClicked(0);
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void BuildGUI_SolidDlg::ClickOnOk()
{
  setIsApplyAndClose( true );
  if ( ClickOnApply() )
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool BuildGUI_SolidDlg::ClickOnApply()
{
  if ( !onAccept() )
    return false;

  initName();

  myEditCurrentArgument->setText("");
  ConstructorsClicked(getConstructorId());

  return true;
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void BuildGUI_SolidDlg::ConstructorsClicked(int constructorId)
{
  switch (constructorId) {
  case 0:
    {
      globalSelection();
      GroupFaces->hide();
      GroupSolid->show();
      myEditCurrentArgument = GroupSolid->LineEdit1;
      GroupSolid->LineEdit1->setText("");
      break;
    }
  case 1:
    {
      globalSelection();
      GroupSolid->hide();
      GroupFaces->show();
      myEditCurrentArgument = GroupFaces->LineEdit1;
      GroupFaces->LineEdit1->setText("");
      break;
    }
  }

  qApp->processEvents();
  updateGeometry();
  resize(minimumSizeHint());
  SelectionIntoArgument();
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void BuildGUI_SolidDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText( "" );

  //myShells = getSelected( TopAbs_SHELL, -1 );
  QList<TopAbs_ShapeEnum> types;
  if (myEditCurrentArgument == GroupSolid->LineEdit1)
  types << TopAbs_SHELL << TopAbs_COMPOUND;
  else if (myEditCurrentArgument == GroupFaces->LineEdit1)
  types << TopAbs_FACE << TopAbs_SHELL << TopAbs_COMPOUND;
  myShells = getSelected( types, -1 );

  if ( !myShells.isEmpty() ) {
    QString aName = myShells.count() > 1 ? QString( "%1_objects").arg( myShells.count() ) : GEOMBase::GetName( myShells[0].get() );
    myEditCurrentArgument->setText( aName );
  }
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BuildGUI_SolidDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  TColStd_MapOfInteger aMap;
  QList<int> aSubShapes;
  aMap.Add( GEOM_COMPOUNDFILTER );

  if (send == GroupSolid->PushButton1) {
    aMap.Add( GEOM_SHELL );
	aSubShapes.append( GEOM_SHELL );
	globalSelection( aMap, aSubShapes );
	myEditCurrentArgument = GroupSolid->LineEdit1;
  }
  else if (send == GroupFaces->PushButton1) {
    aMap.Add( GEOM_SHELL );
    aMap.Add( GEOM_FACE );
    aSubShapes.append( GEOM_SHELL );
    aSubShapes.append( GEOM_FACE );
    globalSelection( aMap, aSubShapes );
    myEditCurrentArgument = GroupFaces->LineEdit1;
  }

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
  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
           this, SLOT(SelectionIntoArgument()));

  ConstructorsClicked(getConstructorId());
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
bool BuildGUI_SolidDlg::isValid (QString& msg)
{
  bool ok = !myShells.isEmpty();

  GEOM::MeasureOpPtr anOp;
  anOp.take(myGeomGUI->GetGeomGen()->GetIMeasureOperations(getStudyId()));
  if (getConstructorId() == 0 && (!GroupSolid->CheckButton1->isChecked() || myShells.count() == 1)) {
	for (int i = 0, n = myShells.count(); i < n && ok; i++) {
	  CORBA::String_var aRes = anOp->IsGoodForSolid(myShells[i].get());
	  if (strlen(aRes.in())) {
		msg = QObject::tr(aRes.in()).arg(GEOMBase::GetName(myShells[i].get()));
		ok = false;
	  }
	}
  }
  return ok;
}

/*
//=================================================================================
// function : isClosed
// purpose  : Check the object 'i' in myShells list is closed or unclosed
//=================================================================================
bool BuildGUI_SolidDlg::isClosed( GEOM::GEOM_Object_ptr shell )
{
  bool ok = false;

  GEOM::GEOM_IKindOfShape::shape_kind aKind;
  GEOM::ListOfLong_var anInts;
  GEOM::ListOfDouble_var aDbls;

  if ( !CORBA::is_nil( shell ) ) {
    GEOM::MeasureOpPtr anOp;
    anOp.take( myGeomGUI->GetGeomGen()->GetIMeasureOperations( getStudyId() ) );

    // Detect kind of shape and parameters
    aKind = anOp->KindOfShape(shell, anInts, aDbls);

    if ( anOp->IsDone() ) {
      if ( anInts[0] == 1 )
        ok = true;
      else if ( anInts[0] == 2 )
        ok = false;
    }
    else {
      MESSAGE ("KindOfShape Operation is NOT DONE!!!");
    }
  }
  else {
    MESSAGE ("Shape is NULL!!!");
  }

  return ok;
}
*/

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool BuildGUI_SolidDlg::execute( ObjectList& objects )
{
  GEOM::GEOM_IShapesOperations_var anOper = GEOM::GEOM_IShapesOperations::_narrow( getOperation() );
  GEOM::GEOM_Object_var anObj;

  switch (getConstructorId()) {
    case 0:
    {
	if ( GroupSolid->CheckButton1->isChecked() ) {
	  GEOM::ListOfGO_var objlist = new GEOM::ListOfGO();
	  objlist->length( myShells.count() );
	  for ( int i = 0; i < myShells.count(); i++ )
		objlist[i] = myShells[i].copy();

	  anObj = anOper->MakeSolidShells( objlist.in() );

	  if ( !anObj->_is_nil() ) objects.push_back( anObj._retn() );
	}
	else {
	  for ( int i = 0, n = myShells.count(); i< n; i++ ){
		anObj = anOper->MakeSolidShell( myShells[ i ].get() );

		if ( !anObj->_is_nil() ) objects.push_back( anObj._retn() );
	  }
	}
	break;
    }
    case 1:
    {
      GEOM::ListOfGO_var objlist = new GEOM::ListOfGO();
      objlist->length( myShells.count() );
      for ( int i = 0; i < myShells.count(); i++ )
        objlist[i] = myShells[i].copy();

    	anObj = anOper->MakeSolidFromConnectedFaces( objlist.in(), GroupFaces->CheckButton1->isChecked() );

    	if ( !anObj->_is_nil() ) objects.push_back( anObj._retn() );
      break;
    }
  }

  return true;
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> BuildGUI_SolidDlg::getSourceObjects()
{
  return myShells;
}
