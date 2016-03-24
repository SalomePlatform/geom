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

// File   : TransformationGUI_ProjectionDlg.cxx
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.

#include "TransformationGUI.h"
#include "TransformationGUI_ProjectionDlg.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>
#include <GEOMImpl_Types.hxx>

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>

#include <TColStd_MapOfInteger.hxx>

namespace
{
  enum EOperationType { PROJ_ON_FACE = 0, PROJ_ON_WIRE, PROJ_ON_EDGE };
}

//=================================================================================
// class    : TransformationGUI_ProjectionDlg()
// purpose  : Constructs a TransformationGUI_ProjectionDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
TransformationGUI_ProjectionDlg::TransformationGUI_ProjectionDlg (GeometryGUI* theGeometryGUI,
                                      QWidget* parent, bool modal, Qt::WindowFlags fl)
  : GEOMBase_Skeleton(theGeometryGUI, parent, modal, fl)
{
  QPixmap image1 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_SELECT")));
  QPixmap image2 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_PROJECTION_ON_FACE")));
  QPixmap image3 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_PROJECTION_ON_WIRE")));
  QPixmap image4 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_PROJECTION_ON_EDGE")));

  setWindowTitle(tr("GEOM_PROJECTION_TITLE"));

  mainFrame()->RadioButton1->setIcon(image2);
  mainFrame()->RadioButton2->setIcon(image3);
  mainFrame()->RadioButton3->setIcon(image4);

  myGroup = new DlgRef_2Sel (centralWidget());

  myGroup->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  myGroup->PushButton1->setIcon(image1);
  myGroup->PushButton2->setIcon(image1);
  myGroup->LineEdit1->setReadOnly(true);
  myGroup->LineEdit2->setReadOnly(true);

  QVBoxLayout* layout = new QVBoxLayout(centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
  layout->addWidget(myGroup);

  setHelpFileName("projection_operation_page.html");

  // Initialisation
  Init();
}

//=================================================================================
// function : ~TransformationGUI_ProjectionDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
TransformationGUI_ProjectionDlg::~TransformationGUI_ProjectionDlg()
{
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void TransformationGUI_ProjectionDlg::Init()
{
  mainFrame()->GroupBoxPublish->show();

  // init variables
  myEditCurrentArgument = myGroup->LineEdit1;

  myGroup->LineEdit1->setText("");
  myGroup->LineEdit2->setText("");
  myObject1.nullify();
  myObject2.nullify();

  showOnlyPreviewControl();
 
  // signals and slots connections
  connect(this, SIGNAL( constructorsClicked( int )),
          this, SLOT  ( ConstructorsClicked( int )));
  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(myGroup->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(myGroup->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(),
          SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  initName( windowTitle() );

  setTabOrder(mainFrame()->GroupConstructors, mainFrame()->GroupBoxName);
  setTabOrder(mainFrame()->GroupBoxName, mainFrame()->GroupMedium);
  setTabOrder(mainFrame()->GroupMedium, mainFrame()->GroupButtons);

  //mainFrame()->RadioButton1->setFocus();
  ConstructorsClicked( 0 );

  resize(100,100);
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void TransformationGUI_ProjectionDlg::ClickOnOk()
{
  setIsApplyAndClose(true);
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool TransformationGUI_ProjectionDlg::ClickOnApply()
{
  if (!onAccept())
    return false;

  initName();
  ConstructorsClicked( getConstructorId() );

  return true;
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection is changed or on dialog initialization or activation
//=================================================================================
void TransformationGUI_ProjectionDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("");

  GEOM::GeomObjPtr aSelectedObject;
  if ( myEditCurrentArgument == myGroup->LineEdit1 ) // what to project
    switch(  getConstructorId() ) {
    case PROJ_ON_FACE: aSelectedObject = getSelected(TopAbs_SHAPE); break;
    case PROJ_ON_WIRE:
    case PROJ_ON_EDGE: aSelectedObject = getSelected(TopAbs_VERTEX); break;
    default:;
    }
  else
    switch(  getConstructorId() ) { // where to project
    case PROJ_ON_FACE: aSelectedObject = getSelected(TopAbs_FACE); break;
    case PROJ_ON_WIRE: aSelectedObject = getSelected(TopAbs_WIRE); break;
    case PROJ_ON_EDGE: aSelectedObject = getSelected(TopAbs_EDGE); break;
    default:;
    }

  TopoDS_Shape aShape;
  if (aSelectedObject && GEOMBase::GetShape(aSelectedObject.get(), aShape) && !aShape.IsNull())
  {
    QString aName = GEOMBase::GetName(aSelectedObject.get());
    myEditCurrentArgument->setText(aName);

    // clear selection
    disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
    myGeomGUI->getApp()->selectionMgr()->clearSelected();
    connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
            this, SLOT(SelectionIntoArgument()));

    if (myEditCurrentArgument == myGroup->LineEdit1) {
      myObject1 = aSelectedObject;
      if (!myObject2)
        myGroup->PushButton2->click();
    }
    else if (myEditCurrentArgument == myGroup->LineEdit2) {
      myObject2 = aSelectedObject;
      if (!myObject1)
        myGroup->PushButton1->click();
    }
  }
  else {
    if      (myEditCurrentArgument == myGroup->LineEdit1) myObject1.nullify();
    else if (myEditCurrentArgument == myGroup->LineEdit2) myObject2.nullify();
  }
  processPreview();
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void TransformationGUI_ProjectionDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  globalSelection(); // close local contexts, if any

  if (send == myGroup->PushButton1) // what to project
  {
    myEditCurrentArgument = myGroup->LineEdit1;

    myGroup->PushButton2->setDown(false);
    myGroup->LineEdit2->setEnabled(false);

    switch ( getConstructorId() ) {
    case PROJ_ON_FACE: {
      TColStd_MapOfInteger aMap;
      aMap.Add( GEOM_POINT );
      aMap.Add( GEOM_EDGE );
      aMap.Add( GEOM_WIRE );
      globalSelection( aMap );
      std::list<int> needTypes;
      needTypes.push_back( TopAbs_VERTEX ), needTypes.push_back( TopAbs_EDGE ), needTypes.push_back( TopAbs_WIRE );
      localSelection(needTypes);
      break;
    }
    case PROJ_ON_WIRE:
    case PROJ_ON_EDGE: {
      localSelection(TopAbs_VERTEX);
      break;
    }
    default:;
    }
  }
  else if (send == myGroup->PushButton2) // where to project
  {
    myEditCurrentArgument = myGroup->LineEdit2;

    myGroup->PushButton1->setDown(false);
    myGroup->LineEdit1->setEnabled(false);

    switch ( getConstructorId() ) {
    case PROJ_ON_FACE: localSelection(TopAbs_FACE); break;
    case PROJ_ON_WIRE: localSelection(TopAbs_WIRE); break;
    case PROJ_ON_EDGE: localSelection(TopAbs_EDGE); break;
    default:;
    }
  }

  // enable line edit
  myEditCurrentArgument->setEnabled(true);
  myEditCurrentArgument->setFocus();
  // after setFocus(), because it will be setDown(false) when loses focus
  send->setDown(true);

  SelectionIntoArgument();
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void TransformationGUI_ProjectionDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();

  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));
  processPreview();
}

//=================================================================================
// function : enterEvent()
// purpose  : when mouse enter onto the QWidget
//=================================================================================
void TransformationGUI_ProjectionDlg::enterEvent (QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr TransformationGUI_ProjectionDlg::createOperation()
{
  return getGeomEngine()->GetITransformOperations(getStudyId());
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool TransformationGUI_ProjectionDlg::isValid (QString&)
{
  return myObject1 && myObject2;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool TransformationGUI_ProjectionDlg::execute (ObjectList& objects)
{
  GEOM::GEOM_Object_var anObj;

  GEOM::GEOM_ITransformOperations_var anOper = GEOM::GEOM_ITransformOperations::_narrow(getOperation());
  anObj = anOper->ProjectShapeCopy( myObject1.get(), myObject2.get() );
  if (!anObj->_is_nil())
    objects.push_back(anObj._retn());

  return true;
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void TransformationGUI_ProjectionDlg::ConstructorsClicked( int constructorId )
{
  myObject1.nullify();
  myObject2.nullify();
  myGroup->LineEdit1->setText( "" );
  myGroup->LineEdit2->setText( "" );

  switch ( constructorId ) {
  case PROJ_ON_FACE:
    mainFrame()->GroupConstructors->setTitle(tr("GEOM_PROJECTION_ON_FACE"));
    myGroup->TextLabel1->setText(tr("GEOM_PROJ_ON_FACE_SOURCE"));
    myGroup->TextLabel2->setText(tr("GEOM_PROJ_ON_FACE_TARGET"));
    break;
  case PROJ_ON_EDGE:
    mainFrame()->GroupConstructors->setTitle(tr("GEOM_PROJECTION_ON_EDGE"));
    myGroup->TextLabel1->setText(tr("GEOM_PROJ_ON_EDGE_SOURCE"));
    myGroup->TextLabel2->setText(tr("GEOM_PROJ_ON_EDGE_TARGET"));
    break;
  case PROJ_ON_WIRE:
    mainFrame()->GroupConstructors->setTitle(tr("GEOM_PROJECTION_ON_WIRE"));
    myGroup->TextLabel1->setText(tr("GEOM_PROJ_ON_WIRE_SOURCE"));
    myGroup->TextLabel2->setText(tr("GEOM_PROJ_ON_WIRE_TARGET"));
    break;
  default:;
  }
  myGroup->PushButton1->click();
}

//=================================================================================
// function : addSubshapeToStudy
// purpose  : virtual method to add new SubObjects if local selection
//=================================================================================
void TransformationGUI_ProjectionDlg::addSubshapesToStudy()
{
  GEOMBase::PublishSubObject( myObject1.get() );
  GEOMBase::PublishSubObject( myObject2.get() );
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> TransformationGUI_ProjectionDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res;
  res << myObject1 << myObject2;
  return res;
}
