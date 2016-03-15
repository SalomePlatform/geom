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
  myPoint1.nullify();
  myPoint2.nullify();
  myFace1.nullify();
  myFace2.nullify();

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
  setIsApplyAndClose( true );
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
      myEditCurrentArgument = GroupPoints->LineEdit1;
      myEditCurrentArgument->setText( "" );
      GroupPoints->LineEdit2->setText( "" );
      myPoint1.nullify();
      myPoint2.nullify();
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
      myEditCurrentArgument = GroupFaces->LineEdit1;
      myEditCurrentArgument->setText("");
      myFace1.nullify();
      myFace2.nullify();
      GroupFaces->PushButton1->setDown(true);
      GroupFaces->PushButton2->setDown(false);
      GroupFaces->LineEdit1->setEnabled(true);
      GroupFaces->LineEdit2->setEnabled(false);
      GroupPoints->hide();
      GroupFaces->show();
      break;
    }
  }

  TopAbs_ShapeEnum aNeedType = ( myEditCurrentArgument == GroupFaces->LineEdit1 ||
                                 myEditCurrentArgument == GroupFaces->LineEdit2 ) ?
    TopAbs_FACE : TopAbs_VERTEX;
  globalSelection(); // close local selection to clear it
  localSelection( aNeedType );

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
    if      (myEditCurrentArgument == GroupPoints->LineEdit1) myPoint1.nullify();
    else if (myEditCurrentArgument == GroupPoints->LineEdit2) myPoint2.nullify();
    else if (myEditCurrentArgument == GroupFaces->LineEdit1)  myFace1.nullify();
    else if (myEditCurrentArgument == GroupFaces->LineEdit2)  myFace2.nullify();
    displayPreview(true);
    return;
  }

  TopAbs_ShapeEnum aNeedType = ( myEditCurrentArgument == GroupFaces->LineEdit1 ||
                                 myEditCurrentArgument == GroupFaces->LineEdit2 ) ?
    TopAbs_FACE : TopAbs_VERTEX;
  GEOM::GeomObjPtr aSelectedObject = getSelected( aNeedType );
  TopoDS_Shape aShape;
  if ( aSelectedObject && GEOMBase::GetShape( aSelectedObject.get(), aShape ) && !aShape.IsNull() ) {
    QString aName = GEOMBase::GetName( aSelectedObject.get() );
    myEditCurrentArgument->setText( aName );
    if ( myEditCurrentArgument == GroupPoints->LineEdit1 ) {
      myPoint1 = aSelectedObject;
      if ( myPoint1 && !myPoint2 )
        GroupPoints->PushButton2->click();
    }
    else if ( myEditCurrentArgument == GroupPoints->LineEdit2 ) {
      myPoint2 = aSelectedObject;
      if ( myPoint2 && !myPoint1 )
        GroupPoints->PushButton1->click();
    }
    else if ( myEditCurrentArgument == GroupFaces->LineEdit1 ) {
      myFace1 = aSelectedObject;
      if ( myFace1 && !myFace2 )
        GroupFaces->PushButton2->click();
    }
    else if ( myEditCurrentArgument == GroupFaces->LineEdit2 ) {
      myFace2 = aSelectedObject;
      if ( myFace2 && !myFace1 )
        GroupFaces->PushButton1->click();      
    }
  }
  
  displayPreview(true);
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

  TopAbs_ShapeEnum aNeedType = ( myEditCurrentArgument == GroupFaces->LineEdit1 ||
                                 myEditCurrentArgument == GroupFaces->LineEdit2 ) ?
    TopAbs_FACE : TopAbs_VERTEX;
  globalSelection(); // close local selection to clear it
  localSelection( aNeedType );

  myEditCurrentArgument->setFocus();
  //  SelectionIntoArgument();
  send->setDown(true);
  displayPreview(true);
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
  bool ok = false;
  switch ( getConstructorId() ) {
  case 0 :
    ok = myPoint1 && myPoint2;
    break;
  case 1:
    ok = myFace1 && myFace2;
    break;
  default:
    break;
  }
  return ok;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool BasicGUI_LineDlg::execute( ObjectList& objects )
{
  bool res = false;
  GEOM::GEOM_Object_var anObj;
  GEOM::GEOM_IBasicOperations_var anOper = GEOM::GEOM_IBasicOperations::_narrow( getOperation() );
  switch ( getConstructorId() ) {
  case 0 :
    anObj = anOper->MakeLineTwoPnt( myPoint1.get(), myPoint2.get() );
    res = true;
    break;
  case 1 :
    anObj = anOper->MakeLineTwoFaces( myFace1.get(), myFace2.get() );
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
  switch ( getConstructorId() ) {
  case 0 :
    GEOMBase::PublishSubObject( myPoint1.get() );
    GEOMBase::PublishSubObject( myPoint2.get() );
    break;
  case 1 :
    GEOMBase::PublishSubObject( myFace1.get() );
    GEOMBase::PublishSubObject( myFace2.get() );
    break;
  default:
    break;
  }
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> BasicGUI_LineDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res;
  res << myPoint1 << myPoint2 << myFace1 << myFace2;
  return res;
}
