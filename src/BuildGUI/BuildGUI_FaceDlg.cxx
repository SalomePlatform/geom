// Copyright (C) 2007-2014  CEA/DEN, EDF R&D, OPEN CASCADE
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
  : GEOMBase_Skeleton( theGeometryGUI, parent ),
    GroupWire        (0),
    myGroupSurf      (0)
{
  QPixmap image0( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );
  QPixmap image1( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_BUILD_FACE" ) ) );
  QPixmap image2( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_DLG_BUILD_FACE_SURFACE" ) ) );

  setWindowTitle( tr( "GEOM_FACE_TITLE" ) );

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle( tr( "GEOM_FACE" ) );
  mainFrame()->RadioButton1->setIcon( image1 );
  mainFrame()->RadioButton2->setIcon( image2 );
  mainFrame()->RadioButton3->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton3->close();

  GroupWire = new DlgRef_1Sel1Check( centralWidget() );

  GroupWire->GroupBox1->setTitle( tr( "GEOM_FACE_FFW" ) );
  GroupWire->TextLabel1->setText( tr( "GEOM_OBJECTS" ) );
  GroupWire->CheckButton1->setText( tr( "GEOM_FACE_OPT" ) );
  GroupWire->PushButton1->setIcon( image0 );

  myGroupSurf = new DlgRef_2Sel(centralWidget());

  myGroupSurf->GroupBox1->setTitle(tr("GEOM_FACE_FROM_SURFACE" ) );
  myGroupSurf->TextLabel1->setText(tr("GEOM_FACE"));
  myGroupSurf->TextLabel2->setText(tr("GEOM_WIRE"));
  myGroupSurf->PushButton1->setIcon(image0);
  myGroupSurf->PushButton2->setIcon(image0);

  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( GroupWire );
  layout->addWidget(myGroupSurf);
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
  myGroupSurf->LineEdit1->setReadOnly( true );
  myGroupSurf->LineEdit2->setReadOnly( true );

  GroupWire->CheckButton1->setChecked( true );
  myWires.clear();
  myFace.nullify();
  myWire.nullify();

  /* signals and slots connections */
  connect(myGeomGUI, SIGNAL(SignalDeactivateActiveDialog()), this, SLOT(DeactivateActiveDialog()));
  connect(myGeomGUI, SIGNAL(SignalCloseAllDialogs()),        this, SLOT(ClickOnCancel()));

  connect(this,      SIGNAL(constructorsClicked(int)), this, SLOT(ConstructorsClicked(int)));

  connect( buttonOk(),    SIGNAL( clicked() ), this, SLOT( ClickOnOk() ) );
  connect( buttonApply(), SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );
  connect( GroupWire->LineEdit1,   SIGNAL( returnPressed()), this, SLOT( LineEditReturnPressed() ) );
  connect( GroupWire->PushButton1, SIGNAL( clicked() ),      this, SLOT( SetEditCurrentArgument() ) );
  connect( myGroupSurf->LineEdit1,   SIGNAL( returnPressed()), this, SLOT( LineEditReturnPressed() ) );
  connect( myGroupSurf->PushButton1, SIGNAL( clicked() ),      this, SLOT( SetEditCurrentArgument() ) );
  connect( myGroupSurf->LineEdit2,   SIGNAL( returnPressed()), this, SLOT( LineEditReturnPressed() ) );
  connect( myGroupSurf->PushButton2, SIGNAL( clicked() ),      this, SLOT( SetEditCurrentArgument() ) );
  connect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr(),
           SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );

  initName( tr( "GEOM_FACE" ) );

  ConstructorsClicked(0);
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void BuildGUI_FaceDlg::ConstructorsClicked(int constructorId)
{
  switch (constructorId) {
  case 0:
    {
      TColStd_MapOfInteger aMap;

      aMap.Add(GEOM_EDGE);
      aMap.Add(GEOM_WIRE);
      aMap.Add(GEOM_FACE);
      aMap.Add(GEOM_SHELL);
      aMap.Add(GEOM_SOLID);
      aMap.Add(GEOM_COMPOUND);
      globalSelection(aMap);

      myEditCurrentArgument = GroupWire->LineEdit1;
      GroupWire->LineEdit1->setText("");
      GroupWire->show();
      myGroupSurf->hide();
      break;
    }
  case 1:
    {
      globalSelection(GEOM_FACE); // For the first element.

      myEditCurrentArgument = myGroupSurf->LineEdit1;
      myGroupSurf->LineEdit1->setText("");
      myGroupSurf->PushButton1->setDown(true);
      myGroupSurf->PushButton2->setDown(false);
      GroupWire->hide();
      myGroupSurf->show();
      break;
    }
  }

  myWires.clear();
  myFace.nullify();
  myWire.nullify();
  qApp->processEvents();
  updateGeometry();
  resize(minimumSizeHint());
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
  if (myEditCurrentArgument == GroupWire->LineEdit1) {
    myEditCurrentArgument->setText( "" );

    QList<TopAbs_ShapeEnum> types;
    types << TopAbs_EDGE  << TopAbs_WIRE  << TopAbs_FACE
          << TopAbs_SHELL << TopAbs_SOLID << TopAbs_COMPOUND;
    myWires = getSelected( types, -1 );

    if ( !myWires.isEmpty() ) {
      QString aName = myWires.count() > 1 ? QString( "%1_objects").arg( myWires.count() ) : GEOMBase::GetName( myWires[0].get() );
      myEditCurrentArgument->setText( aName );
    }
  } else if (myEditCurrentArgument == myGroupSurf->LineEdit1 ||
             myEditCurrentArgument == myGroupSurf->LineEdit2) {
    const bool isEditFace = myEditCurrentArgument == myGroupSurf->LineEdit1;
    const TopAbs_ShapeEnum aType = isEditFace ? TopAbs_FACE : TopAbs_WIRE;
    GEOM::GeomObjPtr aSelectedObject = getSelected(aType);
    GEOM::GeomObjPtr &anObj = isEditFace ? myFace : myWire;

    if (aSelectedObject) {
      myEditCurrentArgument->setText(GEOMBase::GetName(aSelectedObject.get()));
      anObj = aSelectedObject;
    } else {
      myEditCurrentArgument->setText("");
      anObj.nullify();
    }

    if (isEditFace) {
      if (myFace && !myWire) {
        myGroupSurf->PushButton2->click();
      }
    } else {
      if (!myFace && myWire) {
        myGroupSurf->PushButton1->click();
      }
    }
  }

  displayPreview(true);
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BuildGUI_FaceDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();
  if (send == GroupWire->PushButton1) {
    TColStd_MapOfInteger aMap;
  
    aMap.Add(GEOM_EDGE);
    aMap.Add(GEOM_WIRE);
    aMap.Add(GEOM_FACE);
    aMap.Add(GEOM_SHELL);
    aMap.Add(GEOM_SOLID);
    aMap.Add(GEOM_COMPOUND);
    globalSelection(aMap);
    myEditCurrentArgument = GroupWire->LineEdit1;
  }
  else if (send == myGroupSurf->PushButton1) {
    globalSelection(GEOM_FACE);
    myEditCurrentArgument = myGroupSurf->LineEdit1;
    myGroupSurf->PushButton2->setDown(false);
    myGroupSurf->LineEdit2->setEnabled(false);
  }
  else if (send == myGroupSurf->PushButton2) {
    globalSelection(GEOM_WIRE);
    myEditCurrentArgument = myGroupSurf->LineEdit2;
    myGroupSurf->PushButton1->setDown(false);
    myGroupSurf->LineEdit1->setEnabled(false);
  }

  // enable line edit
  myEditCurrentArgument->setEnabled(true);
  myEditCurrentArgument->setFocus();
  send->setDown(true);
  displayPreview(true);
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
  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
           this, SLOT(SelectionIntoArgument()));

  ConstructorsClicked(getConstructorId());
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
  bool ok = false;

  switch (getConstructorId()) {
  case 0:
    ok = !myWires.isEmpty();
    break;
  case 1:
    ok = myFace && myWire;
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
bool BuildGUI_FaceDlg::execute( ObjectList& objects )
{
  bool res = false;
  GEOM::GEOM_IShapesOperations_var anOper = GEOM::GEOM_IShapesOperations::_narrow( getOperation() );
  GEOM::GEOM_Object_var anObj;

  switch (getConstructorId()) {
  case 0:
    {
      GEOM::ListOfGO_var objlist = new GEOM::ListOfGO();
      
      objlist->length( myWires.count() );

      for ( int i = 0; i < myWires.count(); i++ ) {
        objlist[i] = myWires[i].copy();
      }

      anObj = anOper->MakeFaceWires( objlist.in(), GroupWire->CheckButton1->isChecked() );
      res   = true;
    }
    break;
  case 1:
    anObj = anOper->MakeFaceFromSurface(myFace.get(), myWire.get());
    res   = true;
    break;
  default:
    break;
  }

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

  return res;
}
