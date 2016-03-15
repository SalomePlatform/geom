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
// File   : BasicGUI_ArcDlg.cxx
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.
//
#include "BasicGUI_ArcDlg.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>

#include <TopTools_IndexedMapOfShape.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>

#include <GEOMImpl_Types.hxx>

//=================================================================================
// class    : BasicGUI_ArcDlg()
// purpose  : Constructs a BasicGUI_ArcDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
BasicGUI_ArcDlg::BasicGUI_ArcDlg( GeometryGUI* theGeometryGUI, QWidget* parent,
                                  bool modal, Qt::WindowFlags fl )
  : GEOMBase_Skeleton( theGeometryGUI, parent, modal, fl )
{
  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
  QPixmap image0( aResMgr->loadPixmap( "GEOM", tr( "ICON_DLG_ARC" ) ) );
  QPixmap image1( aResMgr->loadPixmap( "GEOM", tr( "ICON_DLG_ARC_CENTER" ) ) );
  QPixmap image2( aResMgr->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );
  QPixmap image3( aResMgr->loadPixmap( "GEOM", tr( "ICON_DLG_ARC_OF_ELLIPSE" ) ) );

  setWindowTitle( tr( "GEOM_ARC_TITLE" ) );

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle( tr( "GEOM_ARC" ) );
  mainFrame()->RadioButton1->setIcon( image0 );
  mainFrame()->RadioButton2->setIcon( image1 );
  mainFrame()->RadioButton3->setIcon( image3 );

  Group3Pnts = new DlgRef_3Sel( centralWidget() );

  Group3Pnts->GroupBox1->setTitle( tr( "GEOM_POINTS" ) );
  Group3Pnts->TextLabel1->setText( tr( "GEOM_POINT_I" ).arg( 1 ) );
  Group3Pnts->TextLabel2->setText( tr( "GEOM_POINT_I" ).arg( 2 ) );
  Group3Pnts->TextLabel3->setText( tr( "GEOM_POINT_I" ).arg( 3 ) );

  Group3Pnts->LineEdit1->setReadOnly( true );
  Group3Pnts->LineEdit2->setReadOnly( true );
  Group3Pnts->LineEdit3->setReadOnly( true );

  Group3Pnts->LineEdit1->setEnabled(true);
  Group3Pnts->LineEdit2->setEnabled(false);
  Group3Pnts->LineEdit3->setEnabled(false);

  Group3Pnts->PushButton1->setIcon(image2);
  Group3Pnts->PushButton2->setIcon(image2);
  Group3Pnts->PushButton3->setIcon(image2);


  Group3Pnts2 = new DlgRef_3Sel1Check( centralWidget() );

  Group3Pnts2->GroupBox1->setTitle( tr( "GEOM_POINTS" ) );
  Group3Pnts2->TextLabel1->setText( tr( "GEOM_CENTER_POINT" ) );
  Group3Pnts2->TextLabel2->setText( tr( "GEOM_POINT_I" ).arg( "Start" ) );
  Group3Pnts2->TextLabel3->setText( tr( "GEOM_POINT_I" ).arg( "End" ) );

  Group3Pnts2->LineEdit1->setReadOnly( true );
  Group3Pnts2->LineEdit2->setReadOnly( true );
  Group3Pnts2->LineEdit3->setReadOnly( true );

  Group3Pnts2->LineEdit1->setEnabled(true);
  Group3Pnts2->LineEdit2->setEnabled(false);
  Group3Pnts2->LineEdit3->setEnabled(false);

  Group3Pnts2->PushButton1->setIcon( image2 );
  Group3Pnts2->PushButton2->setIcon( image2 );
  Group3Pnts2->PushButton3->setIcon( image2 );
  
  Group3Pnts2->CheckButton1->setText( tr( "GEOM_REVERSE" ) );

  Group3Pnts3 = new DlgRef_3Sel( centralWidget() );

  Group3Pnts3->GroupBox1->setTitle( tr( "GEOM_POINTS" ) );
  Group3Pnts3->TextLabel1->setText( tr( "GEOM_CENTER_POINT" ) );
  Group3Pnts3->TextLabel2->setText( tr( "GEOM_POINT_I" ).arg( 1 ) );
  Group3Pnts3->TextLabel3->setText( tr( "GEOM_POINT_I" ).arg( 2 ) );

  Group3Pnts3->LineEdit1->setReadOnly( true );
  Group3Pnts3->LineEdit2->setReadOnly( true );
  Group3Pnts3->LineEdit3->setReadOnly( true );

  Group3Pnts3->LineEdit1->setEnabled(true);
  Group3Pnts3->LineEdit2->setEnabled(false);
  Group3Pnts3->LineEdit3->setEnabled(false);

  Group3Pnts3->PushButton1->setIcon(image2);
  Group3Pnts3->PushButton2->setIcon(image2);
  Group3Pnts3->PushButton3->setIcon(image2);

  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( Group3Pnts );
  layout->addWidget( Group3Pnts2 );
  layout->addWidget( Group3Pnts3 );

  setHelpFileName( "create_arc_page.html" );

  Init();
}


//=================================================================================
// function : ~BasicGUI_ArcDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
BasicGUI_ArcDlg::~BasicGUI_ArcDlg()
{
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BasicGUI_ArcDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = Group3Pnts->LineEdit1;

  myPoint1.nullify();
  myPoint2.nullify();
  myPoint3.nullify();
  Group3Pnts2->CheckButton1->setChecked( false );
  Group3Pnts->PushButton1->setDown(true);

  /* signals and slots connections */
  connect( myGeomGUI, SIGNAL( SignalDeactivateActiveDialog() ), this, SLOT( DeactivateActiveDialog() ) );
  connect( myGeomGUI, SIGNAL( SignalCloseAllDialogs() ),        this, SLOT( ClickOnCancel() ) );
 
  connect( this,           SIGNAL( constructorsClicked( int ) ), this, SLOT( ConstructorsClicked( int ) ) );
 
  connect( buttonOk(),     SIGNAL( clicked() ), this, SLOT( ClickOnOk() ) );
  connect( buttonApply(),  SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );

  connect( Group3Pnts->PushButton1, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );
  connect( Group3Pnts->PushButton2, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );
  connect( Group3Pnts->PushButton3, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );

  connect( Group3Pnts2->PushButton1, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );
  connect( Group3Pnts2->PushButton2, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );
  connect( Group3Pnts2->PushButton3, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );

  connect( Group3Pnts2->CheckButton1, SIGNAL( toggled( bool ) ), this, SLOT( ReverseSense() ) );

  connect( Group3Pnts3->PushButton1, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );
  connect( Group3Pnts3->PushButton2, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );
  connect( Group3Pnts3->PushButton3, SIGNAL( clicked() ), this, SLOT( SetEditCurrentArgument() ) );

  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
           this, SLOT( SelectionIntoArgument() ) );

  initName( tr( "GEOM_ARC" ) ); 
  

  ConstructorsClicked( 0 );
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void BasicGUI_ArcDlg::ClickOnOk()
{
  setIsApplyAndClose( true );
  if ( ClickOnApply() )
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool BasicGUI_ArcDlg::ClickOnApply()
{
  if ( !onAccept() )
    return false;

  initName();
  ConstructorsClicked( getConstructorId() );

  return true;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void BasicGUI_ArcDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText( "" );

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  if (aSelList.Extent() != 1) {
    switch (getConstructorId()) {
    case 0:
      if      ( myEditCurrentArgument == Group3Pnts->LineEdit1 )   myPoint1.nullify();
      else if ( myEditCurrentArgument == Group3Pnts->LineEdit2 )   myPoint2.nullify();
      else if ( myEditCurrentArgument == Group3Pnts->LineEdit3 )   myPoint3.nullify();
      break;
    case 1:
      if      ( myEditCurrentArgument == Group3Pnts2->LineEdit1 )   myPoint1.nullify();
      else if ( myEditCurrentArgument == Group3Pnts2->LineEdit2 )   myPoint2.nullify();
      else if ( myEditCurrentArgument == Group3Pnts2->LineEdit3 )   myPoint3.nullify();
      break;
    case 2:
      if      ( myEditCurrentArgument == Group3Pnts3->LineEdit1 )   myPoint1.nullify();
      else if ( myEditCurrentArgument == Group3Pnts3->LineEdit2 )   myPoint2.nullify();
      else if ( myEditCurrentArgument == Group3Pnts3->LineEdit3 )   myPoint3.nullify();
      break;
    default:
      break;
    }
    return;
  }   

  GEOM::GeomObjPtr aSelectedObject = getSelected( TopAbs_VERTEX );
  TopoDS_Shape aShape;
  if ( aSelectedObject && GEOMBase::GetShape( aSelectedObject.get(), aShape ) && !aShape.IsNull() ) {
    QString aName = GEOMBase::GetName( aSelectedObject.get() );
    myEditCurrentArgument->setText(aName);
    switch ( getConstructorId() ) {
    case 0:
      if ( myEditCurrentArgument == Group3Pnts->LineEdit1 ) {
        myPoint1 = aSelectedObject;
        if ( myPoint1 && !myPoint2 )
          Group3Pnts->PushButton2->click();
      }
      else if ( myEditCurrentArgument == Group3Pnts->LineEdit2 ) {
        myPoint2 = aSelectedObject;
        if ( myPoint2 && !myPoint3 )
          Group3Pnts->PushButton3->click();
      }
      else if ( myEditCurrentArgument == Group3Pnts->LineEdit3 ) {
        myPoint3 = aSelectedObject;
        if ( myPoint3 && !myPoint1 )
          Group3Pnts->PushButton1->click();
      }
      break;
    case 1:
      if ( myEditCurrentArgument == Group3Pnts2->LineEdit1 ) {
        myPoint1 = aSelectedObject;
        if ( myPoint1 && !myPoint2 )
          Group3Pnts2->PushButton2->click();
      }
      else if ( myEditCurrentArgument == Group3Pnts2->LineEdit2 ) {
        myPoint2 = aSelectedObject;
        if ( myPoint2 && !myPoint3 )
          Group3Pnts2->PushButton3->click();
      }
      else if ( myEditCurrentArgument == Group3Pnts2->LineEdit3 ) {
        myPoint3 = aSelectedObject;
        if ( myPoint3 && !myPoint1 )
          Group3Pnts2->PushButton1->click();
      }
      break;
    case 2:
      if ( myEditCurrentArgument == Group3Pnts3->LineEdit1 ) {
        myPoint1 = aSelectedObject;
        if ( myPoint1 && !myPoint2 )
          Group3Pnts3->PushButton2->click();
      }
      else if ( myEditCurrentArgument == Group3Pnts3->LineEdit2 ) {
        myPoint2 = aSelectedObject;
        if ( myPoint2 && !myPoint3 )
          Group3Pnts3->PushButton3->click();
      }
      else if ( myEditCurrentArgument == Group3Pnts3->LineEdit3 ) {
        myPoint3 = aSelectedObject;
        if ( myPoint3 && !myPoint1 )
          Group3Pnts3->PushButton1->click();
      }
      break;
    }
  }

  displayPreview(true);
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BasicGUI_ArcDlg::SetEditCurrentArgument()
{
  globalSelection(); // close local selection to clear it
  localSelection( TopAbs_VERTEX );

  QPushButton* send = (QPushButton*)sender();
  switch ( getConstructorId() ) {
  case 0:
    if      ( send == Group3Pnts->PushButton1 ) {
      myEditCurrentArgument = Group3Pnts->LineEdit1;
      Group3Pnts->PushButton2->setDown(false);
      Group3Pnts->PushButton3->setDown(false);
      Group3Pnts->LineEdit1->setEnabled(true);
      Group3Pnts->LineEdit2->setEnabled(false);
      Group3Pnts->LineEdit3->setEnabled(false);
    }
    else if ( send == Group3Pnts->PushButton2 ) {
      myEditCurrentArgument = Group3Pnts->LineEdit2;
      Group3Pnts->PushButton1->setDown(false);
      Group3Pnts->PushButton3->setDown(false);
      Group3Pnts->LineEdit1->setEnabled(false);
      Group3Pnts->LineEdit2->setEnabled(true);
      Group3Pnts->LineEdit3->setEnabled(false);
    }
    else if ( send == Group3Pnts->PushButton3 ) {
      myEditCurrentArgument = Group3Pnts->LineEdit3;
      Group3Pnts->PushButton1->setDown(false);
      Group3Pnts->PushButton2->setDown(false);
      Group3Pnts->LineEdit1->setEnabled(false);
      Group3Pnts->LineEdit2->setEnabled(false);
      Group3Pnts->LineEdit3->setEnabled(true);
    }
    break;
  case 1:
    if      ( send == Group3Pnts2->PushButton1 ) {
      myEditCurrentArgument = Group3Pnts2->LineEdit1;
      Group3Pnts2->PushButton2->setDown(false);
      Group3Pnts2->PushButton3->setDown(false);
      Group3Pnts2->LineEdit1->setEnabled(true);
      Group3Pnts2->LineEdit2->setEnabled(false);
      Group3Pnts2->LineEdit3->setEnabled(false);
    }
    else if ( send == Group3Pnts2->PushButton2 ) {
      myEditCurrentArgument = Group3Pnts2->LineEdit2;
      Group3Pnts2->PushButton1->setDown(false);
      Group3Pnts2->PushButton3->setDown(false);
      Group3Pnts2->LineEdit1->setEnabled(false);
      Group3Pnts2->LineEdit2->setEnabled(true);
      Group3Pnts2->LineEdit3->setEnabled(false);
    }
    else if ( send == Group3Pnts2->PushButton3 ) {
      myEditCurrentArgument = Group3Pnts2->LineEdit3;
      Group3Pnts2->PushButton1->setDown(false);
      Group3Pnts2->PushButton2->setDown(false);
      Group3Pnts2->LineEdit1->setEnabled(false);
      Group3Pnts2->LineEdit2->setEnabled(false);
      Group3Pnts2->LineEdit3->setEnabled(true);
    }
    break;
  case 2:
    if ( send == Group3Pnts3->PushButton1 ) {
      myEditCurrentArgument = Group3Pnts3->LineEdit1;
      Group3Pnts3->PushButton2->setDown(false);
      Group3Pnts3->PushButton3->setDown(false);
      Group3Pnts3->LineEdit1->setEnabled(true);
      Group3Pnts3->LineEdit2->setEnabled(false);
      Group3Pnts3->LineEdit3->setEnabled(false);
    }
    else if ( send == Group3Pnts3->PushButton2 ) {
      myEditCurrentArgument = Group3Pnts3->LineEdit2;
      Group3Pnts3->PushButton1->setDown(false);
      Group3Pnts3->PushButton3->setDown(false);
      Group3Pnts3->LineEdit1->setEnabled(false);
      Group3Pnts3->LineEdit2->setEnabled(true);
      Group3Pnts3->LineEdit3->setEnabled(false);
    }
    else if ( send == Group3Pnts3->PushButton3 ) {
      myEditCurrentArgument = Group3Pnts3->LineEdit3;
      Group3Pnts3->PushButton1->setDown(false);
      Group3Pnts3->PushButton2->setDown(false);
      Group3Pnts3->LineEdit1->setEnabled(false);
      Group3Pnts3->LineEdit2->setEnabled(false);
      Group3Pnts3->LineEdit3->setEnabled(true);
    }
    break;
  }
  myEditCurrentArgument->setFocus();
  //  SelectionIntoArgument();
  send->setDown(true);
  displayPreview(true);
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void BasicGUI_ArcDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();

  connect( myGeomGUI->getApp()->selectionMgr(),
           SIGNAL( currentSelectionChanged() ),
           this,
           SLOT(SelectionIntoArgument() ) );

  ConstructorsClicked( getConstructorId() );
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void BasicGUI_ArcDlg::enterEvent( QEvent* )
{
  if ( !mainFrame()->GroupConstructors->isEnabled() )
    ActivateThisDialog();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr BasicGUI_ArcDlg::createOperation()
{
  return myGeomGUI->GetGeomGen()->GetICurvesOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool BasicGUI_ArcDlg::isValid( QString& msg )
{
  bool ok = myPoint1 && myPoint2 && myPoint3 &&
    myPoint1 != myPoint2 && myPoint1 != myPoint3 && myPoint2 != myPoint3;
  return ok;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool BasicGUI_ArcDlg::execute( ObjectList& objects )
{
  bool res = false;
  GEOM::GEOM_Object_var anObj;

  GEOM::GEOM_ICurvesOperations_var anOper = GEOM::GEOM_ICurvesOperations::_narrow( getOperation() );

  switch ( getConstructorId() ) {
  case 0:
    {
      if ( myPoint1 && myPoint2 && myPoint3 ) {
        anObj = anOper->MakeArc( myPoint1.get(), myPoint2.get(), myPoint3.get() );
        res = true;
      }
      break;
    }
  case 1:
    {
      bool Sense = Group3Pnts2->CheckButton1->isChecked();
      if ( myPoint1 && myPoint2 && myPoint3 ) {
        anObj = anOper->MakeArcCenter( myPoint1.get(), myPoint2.get(), myPoint3.get(), Sense );
        res = true;
      }
      break;
    }
  case 2:
    {
      if ( myPoint1 && myPoint2 && myPoint3 ) {
        anObj = anOper->MakeArcOfEllipse( myPoint1.get(), myPoint2.get(), myPoint3.get() );
        res = true;
      }
      break;
    }
  }
  if ( !anObj->_is_nil() ) {
    objects.push_back( anObj._retn() );
  }
  return res;
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void BasicGUI_ArcDlg::ConstructorsClicked( int constructorId )
{
  disconnect( myGeomGUI->getApp()->selectionMgr(), 0, this, 0 );

  myPoint1.nullify();
  myPoint2.nullify();
  myPoint3.nullify();
  
  switch ( constructorId ) {
  case 0:
    {
      Group3Pnts->show();
      Group3Pnts2->hide();
      Group3Pnts3->hide();
      Group3Pnts->PushButton1->setDown(true);
      Group3Pnts->PushButton2->setDown(false);
      Group3Pnts->PushButton3->setDown(false);
      Group3Pnts->LineEdit1->setText( "" );
      Group3Pnts->LineEdit2->setText( "" );
      Group3Pnts->LineEdit3->setText( "" );
      Group3Pnts->LineEdit1->setEnabled(true);
      Group3Pnts->LineEdit2->setEnabled(false);
      Group3Pnts->LineEdit3->setEnabled(false);

      myEditCurrentArgument = Group3Pnts->LineEdit1;
      break;
    }
  case 1:
    {
      Group3Pnts->hide();
      Group3Pnts2->show();
      Group3Pnts3->hide();
      Group3Pnts2->PushButton1->setDown(true);
      Group3Pnts2->PushButton2->setDown(false);
      Group3Pnts2->PushButton3->setDown(false);
      Group3Pnts2->LineEdit1->setText( "" );
      Group3Pnts2->LineEdit2->setText( "" );
      Group3Pnts2->LineEdit3->setText( "" );
      Group3Pnts2->LineEdit1->setEnabled(true);
      Group3Pnts2->LineEdit2->setEnabled(false);
      Group3Pnts2->LineEdit3->setEnabled(false);
      
      myEditCurrentArgument = Group3Pnts2->LineEdit1;
      break;
    }
  case 2:
    {
      Group3Pnts->hide();
      Group3Pnts2->hide();
      Group3Pnts3->show();
      Group3Pnts3->PushButton1->setDown(true);
      Group3Pnts3->PushButton2->setDown(false);
      Group3Pnts3->PushButton3->setDown(false);
      Group3Pnts3->LineEdit1->setText( "" );
      Group3Pnts3->LineEdit2->setText( "" );
      Group3Pnts3->LineEdit3->setText( "" );
      Group3Pnts3->LineEdit1->setEnabled(true);
      Group3Pnts3->LineEdit2->setEnabled(false);
      Group3Pnts3->LineEdit3->setEnabled(false);

      myEditCurrentArgument = Group3Pnts3->LineEdit1;
      break;
    }
  }

  globalSelection(); // close local contexts, if any
  localSelection( TopAbs_VERTEX ); //Select Vertex on All Shapes
  
  qApp->processEvents();
  updateGeometry();
  resize( minimumSizeHint() );

  myEditCurrentArgument->setFocus();
  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
           this, SLOT( SelectionIntoArgument() ) );

  SelectionIntoArgument();

  displayPreview(true);
}

//=================================================================================
// function : ReverseSense()
// purpose  : Orientation of the arc
//=================================================================================
void BasicGUI_ArcDlg::ReverseSense()
{
  displayPreview(true);
}

//=================================================================================
// function : addSubshapeToStudy
// purpose  : virtual method to add new SubObjects if local selection
//=================================================================================
void BasicGUI_ArcDlg::addSubshapesToStudy()
{
  switch ( getConstructorId() ) {
  case 0:
  case 1:
  case 2:
    GEOMBase::PublishSubObject( myPoint1.get() );
    GEOMBase::PublishSubObject( myPoint2.get() );
    GEOMBase::PublishSubObject( myPoint3.get() );
    break;
  default:
    break;
  }
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> BasicGUI_ArcDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res;
  res << myPoint1 << myPoint2 << myPoint3;
  return res;
}
