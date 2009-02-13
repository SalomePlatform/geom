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
// File   : BasicGUI_LineDlg.cxx
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.
//
#include "BasicGUI_LineDlg.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>

#include <TopoDS_Shape.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#include <GEOMImpl_Types.hxx>

//=================================================================================
// class    : BasicGUI_LineDlg()
// purpose  : Constructs a BasicGUI_LineDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
BasicGUI_LineDlg::BasicGUI_LineDlg( GeometryGUI* theGeometryGUI, QWidget* parent,
				    bool modal, Qt::WindowFlags fl )
  : GEOMBase_Skeleton( theGeometryGUI, parent, modal, fl )
{
  QPixmap image0( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_LINE_2P" ) ) );
  QPixmap image1( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );
  QPixmap image2( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_LINE_2F" ) ) );

  setWindowTitle( tr( "GEOM_LINE_TITLE" ) );
 
  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle( tr( "GEOM_LINE" ) );
  mainFrame()->RadioButton1->setIcon( image0 );
  mainFrame()->RadioButton2->setIcon( image2 );
  mainFrame()->RadioButton3->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton3->close();

  GroupPoints = new DlgRef_2Sel( centralWidget() );

  GroupPoints->GroupBox1->setTitle( tr( "GEOM_POINTS" ) );
  GroupPoints->TextLabel1->setText( tr( "GEOM_POINT_I" ).arg( 1 ) );
  GroupPoints->TextLabel2->setText( tr( "GEOM_POINT_I" ).arg( 2 ) );
  GroupPoints->PushButton1->setIcon( image1 );
  GroupPoints->PushButton2->setIcon( image1 );
  GroupPoints->LineEdit1->setReadOnly( true );
  GroupPoints->LineEdit2->setReadOnly( true );
  GroupPoints->LineEdit1->setEnabled( true );
  GroupPoints->LineEdit2->setEnabled( false );

  GroupFaces = new DlgRef_2Sel( centralWidget() );
  GroupFaces->GroupBox1->setTitle( tr( "GEOM_FACES" ) );
  GroupFaces->TextLabel1->setText( tr( "GEOM_FACE_I" ).arg( "1" ) );
  GroupFaces->TextLabel2->setText( tr( "GEOM_FACE_I" ).arg( "2" ) );
  GroupFaces->PushButton1->setIcon( image1 );
  GroupFaces->PushButton2->setIcon( image1 );
  GroupFaces->LineEdit1->setReadOnly( true );
  GroupFaces->LineEdit2->setReadOnly( true );
  GroupFaces->LineEdit1->setEnabled( true );
  GroupFaces->LineEdit2->setEnabled( false );

  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( GroupPoints );
  layout->addWidget( GroupFaces );

  /***************************************************************/

  setHelpFileName( "create_line_page.html" );

  Init();
}


//=================================================================================
// function : ~BasicGUI_LineDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
BasicGUI_LineDlg::~BasicGUI_LineDlg()
{  
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BasicGUI_LineDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupPoints->LineEdit1;
  myPoint1 = myPoint2 = GEOM::GEOM_Object::_nil();
  globalSelection(); // close local contexts, if any
  localSelection( GEOM::GEOM_Object::_nil(), TopAbs_VERTEX );
  GroupPoints->PushButton1->setDown(true);
  
	
  /* signals and slots connections */
  connect( myGeomGUI, SIGNAL( SignalDeactivateActiveDialog() ), this, SLOT( DeactivateActiveDialog() ) );
  connect( myGeomGUI, SIGNAL( SignalCloseAllDialogs() ),        this, SLOT( ClickOnCancel() ) );

  connect( this,           SIGNAL( constructorsClicked( int ) ), this, SLOT( ConstructorsClicked( int ) ) );

  connect( buttonOk(),     SIGNAL( clicked() ), this, SLOT( ClickOnOk() ) );
  connect( buttonApply(),  SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );

  connect( GroupPoints->PushButton1, SIGNAL( clicked() ),       this, SLOT( SetEditCurrentArgument() ) );
  connect( GroupPoints->PushButton2, SIGNAL( clicked() ),       this, SLOT( SetEditCurrentArgument() ) );
  connect( GroupFaces->PushButton1,  SIGNAL( clicked() ),       this, SLOT( SetEditCurrentArgument() ) );
  connect( GroupFaces->PushButton2,  SIGNAL( clicked() ),       this, SLOT( SetEditCurrentArgument() ) );

  connect( GroupPoints->LineEdit1,   SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );
  connect( GroupPoints->LineEdit2,   SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );
  connect( GroupFaces->LineEdit1,    SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );
  connect( GroupFaces->LineEdit2,    SIGNAL( returnPressed() ), this, SLOT( LineEditReturnPressed() ) );

  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
	   this, SLOT( SelectionIntoArgument() ) );
  
  initName( tr( "GEOM_LINE" ) );

  ConstructorsClicked( 0 );
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void BasicGUI_LineDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool BasicGUI_LineDlg::ClickOnApply()
{
  if ( !onAccept() )
    return false;
  
  initName();

  myEditCurrentArgument->setText( "" );
  ConstructorsClicked( getConstructorId() );

  return true;
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void BasicGUI_LineDlg::ConstructorsClicked( int constructorId )
{  
  switch ( constructorId ) {
  case 0:
    {
      globalSelection(); // close local contexts, if any
      localSelection( GEOM::GEOM_Object::_nil(), TopAbs_VERTEX );
      
      myEditCurrentArgument = GroupPoints->LineEdit1;
      myEditCurrentArgument->setText( "" );
      GroupPoints->LineEdit2->setText( "" );
      myPoint1 = GEOM::GEOM_Object::_nil();
      myPoint2 = GEOM::GEOM_Object::_nil();
      GroupPoints->PushButton1->setDown(true);
      GroupPoints->PushButton2->setDown(false);
      GroupPoints->LineEdit1->setEnabled(true);
      GroupPoints->LineEdit2->setEnabled(false);
      GroupPoints->show();
      GroupFaces->hide();
      break;
    }
  case 1:
    {
      globalSelection(); // close local contexts, if any
      localSelection( GEOM::GEOM_Object::_nil(), TopAbs_FACE );

      myEditCurrentArgument = GroupFaces->LineEdit1;
      myEditCurrentArgument->setText("");
      myFace1 = GEOM::GEOM_Object::_nil();
      myFace2 = GEOM::GEOM_Object::_nil();
      GroupFaces->PushButton1->setDown(true);
      GroupFaces->PushButton2->setDown(false);
      GroupFaces->LineEdit1->setEnabled(true);
      GroupFaces->LineEdit2->setEnabled(false);
      GroupPoints->hide();
      GroupFaces->show();
      break;
    }
  }

  qApp->processEvents();
  updateGeometry();
  resize( minimumSizeHint() );

  SelectionIntoArgument();
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void BasicGUI_LineDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText( "" );

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  if (aSelList.Extent() != 1) {
    if      (myEditCurrentArgument == GroupPoints->LineEdit1) myPoint1 = GEOM::GEOM_Object::_nil();
    else if (myEditCurrentArgument == GroupPoints->LineEdit2) myPoint2 = GEOM::GEOM_Object::_nil();
    else if (myEditCurrentArgument == GroupFaces->LineEdit1)  myFace1  = GEOM::GEOM_Object::_nil();
    else if (myEditCurrentArgument == GroupFaces->LineEdit2)  myFace2  = GEOM::GEOM_Object::_nil();
    displayPreview();
    return;
  }

  Standard_Boolean aRes = Standard_False;
  GEOM::GEOM_Object_var aSelectedObject = GEOMBase::ConvertIOinGEOMObject(aSelList.First(), aRes);
  if ( !CORBA::is_nil( aSelectedObject ) && aRes ) {
    QString aName = GEOMBase::GetName( aSelectedObject );
    TopAbs_ShapeEnum aNeedType = TopAbs_VERTEX;
    if ( myEditCurrentArgument == GroupFaces->LineEdit1 ||
	 myEditCurrentArgument == GroupFaces->LineEdit2 )
      aNeedType = TopAbs_FACE;
    
    TopoDS_Shape aShape;
    if ( GEOMBase::GetShape( aSelectedObject, aShape, TopAbs_SHAPE ) && !aShape.IsNull() ) {
      TColStd_IndexedMapOfInteger aMap;
      aSelMgr->GetIndexes(aSelList.First(), aMap);
      if ( aMap.Extent() == 1 ) { // Local Selection
	int anIndex = aMap( 1 );
        if ( aNeedType == TopAbs_FACE )
          aName += QString( ":face_%1" ).arg( anIndex );
        else
          aName += QString( ":vertex_%1" ).arg( anIndex );

	//Find SubShape Object in Father
	GEOM::GEOM_Object_var aFindedObject = GEOMBase_Helper::findObjectInFather( aSelectedObject, aName );

	if ( aFindedObject == GEOM::GEOM_Object::_nil() ) { // Object not found in study
	  GEOM::GEOM_IShapesOperations_var aShapesOp = getGeomEngine()->GetIShapesOperations( getStudyId() );
	  aSelectedObject = aShapesOp->GetSubShape( aSelectedObject, anIndex );
	}
	else {
	  aSelectedObject = aFindedObject; // get Object from study
	}
      }
      else { // Global Selection
        if ( aShape.ShapeType() != aNeedType ) {
          aSelectedObject = GEOM::GEOM_Object::_nil();
          aName = "";
        }
      }
    }

    myEditCurrentArgument->setText( aName );

    if (!aSelectedObject->_is_nil()) { // clear selection if something selected
      globalSelection();
      localSelection( GEOM::GEOM_Object::_nil(), aNeedType );      
    }

    if ( myEditCurrentArgument == GroupPoints->LineEdit1 ) {
      myPoint1 = aSelectedObject;
      if ( !myPoint1->_is_nil() && myPoint2->_is_nil() )
	GroupPoints->PushButton2->click();
    }
    else if ( myEditCurrentArgument == GroupPoints->LineEdit2 ) {
      myPoint2 = aSelectedObject;
      if ( !myPoint2->_is_nil() && myPoint1->_is_nil() )
	GroupPoints->PushButton1->click();
    }
    else if ( myEditCurrentArgument == GroupFaces->LineEdit1 ) {
      myFace1 = aSelectedObject;
      if ( !myFace1->_is_nil() && myFace2->_is_nil() )
	GroupFaces->PushButton2->click();
    }
    else if ( myEditCurrentArgument == GroupFaces->LineEdit2 ) {
      myFace2 = aSelectedObject;
      if ( !myFace2->_is_nil() && myFace1->_is_nil() )
	GroupFaces->PushButton1->click();      
    }
  }
  
  displayPreview();
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BasicGUI_LineDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();
  if ( send == GroupPoints->PushButton1 ) {
    myEditCurrentArgument = GroupPoints->LineEdit1;
    GroupPoints->PushButton2->setDown(false);
    GroupPoints->LineEdit1->setEnabled(true);
    GroupPoints->LineEdit2->setEnabled(false);
  }
  else if ( send == GroupPoints->PushButton2 ) {
    myEditCurrentArgument = GroupPoints->LineEdit2;
    GroupPoints->PushButton1->setDown(false);
    GroupPoints->LineEdit1->setEnabled(false);
    GroupPoints->LineEdit2->setEnabled(true);
  }
  else if ( send == GroupFaces->PushButton1 ) {
    myEditCurrentArgument = GroupFaces->LineEdit1;
    GroupFaces->PushButton2->setDown(false);
    GroupFaces->LineEdit1->setEnabled(true);
    GroupFaces->LineEdit2->setEnabled(false);
  }
  else if ( send == GroupFaces->PushButton2 ) {
    myEditCurrentArgument = GroupFaces->LineEdit2;
    GroupFaces->PushButton1->setDown(false);
    GroupFaces->LineEdit1->setEnabled(false);
    GroupFaces->LineEdit2->setEnabled(true);
  }

  globalSelection(); // close local selection to clear it
  TopAbs_ShapeEnum aNeedType = TopAbs_VERTEX;
  if ( myEditCurrentArgument == GroupFaces->LineEdit1 || myEditCurrentArgument == GroupFaces->LineEdit2 )
    aNeedType = TopAbs_FACE;
  localSelection( GEOM::GEOM_Object::_nil(), aNeedType );

  myEditCurrentArgument->setFocus();
  //  SelectionIntoArgument();
  send->setDown(true);
  displayPreview();
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void BasicGUI_LineDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if      ( send == GroupPoints->LineEdit1 ) myEditCurrentArgument = GroupPoints->LineEdit1;
  else if ( send == GroupPoints->LineEdit2 ) myEditCurrentArgument = GroupPoints->LineEdit2;
  else if ( send == GroupFaces->LineEdit1 )  myEditCurrentArgument = GroupFaces->LineEdit1;
  else if ( send == GroupFaces->LineEdit2 )  myEditCurrentArgument = GroupFaces->LineEdit2;
  else return;
  GEOMBase_Skeleton::LineEditReturnPressed();
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void BasicGUI_LineDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
	   this, SLOT( SelectionIntoArgument() ) );

  ConstructorsClicked( getConstructorId() );
}

//=================================================================================
// function : DeactivateActiveDialog()
// purpose  : public slot to deactivate if active
//=================================================================================
void BasicGUI_LineDlg::DeactivateActiveDialog()
{
  GEOMBase_Skeleton::DeactivateActiveDialog();
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void BasicGUI_LineDlg::enterEvent( QEvent* )
{
  if ( !mainFrame()->GroupConstructors->isEnabled() )
    ActivateThisDialog();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr BasicGUI_LineDlg::createOperation()
{
  return myGeomGUI->GetGeomGen()->GetIBasicOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool BasicGUI_LineDlg::isValid( QString& msg )
{
  const int id = getConstructorId();
  if ( id == 0 )
    return !myPoint1->_is_nil() && !myPoint2->_is_nil();
  else if ( id == 1 )
    return !myFace1->_is_nil() && !myFace2->_is_nil();
  return false;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool BasicGUI_LineDlg::execute( ObjectList& objects )
{
  bool res = false;
  GEOM::GEOM_Object_var anObj;
  switch ( getConstructorId() ) {
  case 0 :
    anObj = GEOM::GEOM_IBasicOperations::_narrow( getOperation() )->MakeLineTwoPnt( myPoint1, myPoint2 );
    res = true;
    break;
  case 1 :
    anObj = GEOM::GEOM_IBasicOperations::_narrow( getOperation() )->MakeLineTwoFaces( myFace1, myFace2 );
    res = true;
    break;
  }
  
  if ( !anObj->_is_nil() )
    objects.push_back( anObj._retn() );

  return res;
}

//=================================================================================
// function : addSubshapeToStudy
// purpose  : virtual method to add new SubObjects if local selection
//=================================================================================
void BasicGUI_LineDlg::addSubshapesToStudy()
{
  QMap<QString, GEOM::GEOM_Object_var> objMap;
  switch ( getConstructorId() ) {
  case 0 :
    objMap[GroupPoints->LineEdit1->text()] = myPoint1;
    objMap[GroupPoints->LineEdit2->text()] = myPoint2;
    break;
  case 1 :
    objMap[GroupFaces->LineEdit1->text()] = myFace1;
    objMap[GroupFaces->LineEdit2->text()] = myFace2;
    break;
  }
  addSubshapesToFather( objMap );
}
