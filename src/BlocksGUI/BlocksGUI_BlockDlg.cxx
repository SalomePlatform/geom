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
// File   : BlocksGUI_BlockDlg.cxx
// Author : Julia DOROVSKIKH, Open CASCADE S.A.S. (julia.dorovskikh@opencascade.com)
//
#include "BlocksGUI_BlockDlg.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>

#include <GEOMImpl_Types.hxx>

// OCCT Includes
#include <TColStd_IndexedMapOfInteger.hxx>

//=================================================================================
// class    : BlocksGUI_BlockDlg()
// purpose  : Constructs a BlocksGUI_BlockDlg which is a child of 'parent'.
//=================================================================================
BlocksGUI_BlockDlg::BlocksGUI_BlockDlg (GeometryGUI* theGeometryGUI, QWidget* parent)
  : GEOMBase_Skeleton(theGeometryGUI, parent),
    myInitial(true)
{
  QPixmap image0 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_BLOCK_2F")));
  QPixmap image1 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_BLOCK_6F")));
  QPixmap imageS (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_SELECT")));

  setWindowTitle(tr("GEOM_BLOCK_TITLE"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_BLOCK"));

  mainFrame()->RadioButton1->setIcon(image0);
  mainFrame()->RadioButton2->setIcon(image1);
  mainFrame()->RadioButton3->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton3->close();

  // Create first group
  Group2F = new DlgRef_2Sel (centralWidget());
  Group2F->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  Group2F->TextLabel1->setText(tr("FACE_1"));
  Group2F->TextLabel2->setText(tr("FACE_2"));
  Group2F->PushButton1->setIcon(imageS);
  Group2F->PushButton2->setIcon(imageS);

  // Create second group
  Group6F = new DlgRef_6Sel (centralWidget());
  Group6F->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  Group6F->TextLabel1->setText(tr("FACE_1"));
  Group6F->TextLabel2->setText(tr("FACE_2"));
  Group6F->TextLabel3->setText(tr("FACE_3"));
  Group6F->TextLabel4->setText(tr("FACE_4"));
  Group6F->TextLabel5->setText(tr("FACE_5"));
  Group6F->TextLabel6->setText(tr("FACE_6"));
  Group6F->PushButton1->setIcon(imageS);
  Group6F->PushButton2->setIcon(imageS);
  Group6F->PushButton3->setIcon(imageS);
  Group6F->PushButton4->setIcon(imageS);
  Group6F->PushButton5->setIcon(imageS);
  Group6F->PushButton6->setIcon(imageS);

  // Add groups to layout
  QVBoxLayout* layout = new QVBoxLayout (centralWidget());
  layout->setMargin(0);
  layout->setSpacing(6);
  layout->addWidget(Group2F);
  layout->addWidget(Group6F);
  /***************************************************************/

  setHelpFileName("build_by_blocks_page.html#hexa_solid_anchor");

  Init();
}

//=================================================================================
// function : ~BlocksGUI_BlockDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
BlocksGUI_BlockDlg::~BlocksGUI_BlockDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BlocksGUI_BlockDlg::Init()
{
  // init variables
  Group2F->LineEdit1->setReadOnly(true);
  Group2F->LineEdit2->setReadOnly(true);

  Group6F->LineEdit1->setReadOnly(true);
  Group6F->LineEdit2->setReadOnly(true);
  Group6F->LineEdit3->setReadOnly(true);
  Group6F->LineEdit4->setReadOnly(true);
  Group6F->LineEdit5->setReadOnly(true);
  Group6F->LineEdit6->setReadOnly(true);

  // signals and slots connections
  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(this, SIGNAL(constructorsClicked(int)), this, SLOT(ConstructorsClicked(int)));

  connect(Group2F->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(Group2F->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(Group6F->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(Group6F->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(Group6F->PushButton3, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(Group6F->PushButton4, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(Group6F->PushButton5, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(Group6F->PushButton6, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  // init controls and fields
  initName(tr("GEOM_BLOCK"));

  myConstructorId = -1;
  ConstructorsClicked(0);
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void BlocksGUI_BlockDlg::ConstructorsClicked (int constructorId)
{
  if (myConstructorId == constructorId)
    return;

  myConstructorId = constructorId;

  // init fields
  myFace1.nullify();
  myFace2.nullify();
  myFace3.nullify();
  myFace4.nullify();
  myFace5.nullify();
  myFace6.nullify();

  switch (constructorId) {
  case 0:
    Group6F->hide();
    Group2F->show();

    Group2F->LineEdit1->setText(tr(""));
    Group2F->LineEdit2->setText(tr(""));

    Group2F->PushButton1->click();
    break;
  case 1:
    Group2F->hide();
    Group6F->show();

    Group6F->LineEdit1->setText(tr(""));
    Group6F->LineEdit2->setText(tr(""));
    Group6F->LineEdit3->setText(tr(""));
    Group6F->LineEdit4->setText(tr(""));
    Group6F->LineEdit5->setText(tr(""));
    Group6F->LineEdit6->setText(tr(""));

    Group6F->PushButton1->click();
    break;
  default:
    break;
  }

  qApp->processEvents();
  updateGeometry();
  resize(minimumSizeHint());

  // on dialog initialization we init the first field with a selected object (if any)
  SelectionIntoArgument();
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void BlocksGUI_BlockDlg::ClickOnOk()
{
  setIsApplyAndClose( true );
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool BlocksGUI_BlockDlg::ClickOnApply()
{
  if (!onAccept())
    return false;

  initName();
  return true;
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection is changed or on dialog initialization or activation
//=================================================================================
void BlocksGUI_BlockDlg::SelectionIntoArgument()
{
  erasePreview();

  GEOM::GeomObjPtr aSelectedObject = getSelected( TopAbs_FACE );
  TopoDS_Shape aShape;
  if ( aSelectedObject && GEOMBase::GetShape( aSelectedObject.get(), aShape ) && !aShape.IsNull() ) {
    QString aName = GEOMBase::GetName( aSelectedObject.get() );
    myEditCurrentArgument->setText( aName );
    if ( myEditCurrentArgument == Group2F->LineEdit1 ) {
      myFace1 = aSelectedObject;
      if (!myFace2) Group2F->PushButton2->click();
    }
    else if ( myEditCurrentArgument == Group2F->LineEdit2 ) {
      myFace2 = aSelectedObject;
      if (!myFace1) Group2F->PushButton1->click();
    }
    else if ( myEditCurrentArgument == Group6F->LineEdit1 ) {
      myFace1 = aSelectedObject;
      if      (!myFace2) Group6F->PushButton2->click();
      else if (!myFace3) Group6F->PushButton3->click();
      else if (!myFace4) Group6F->PushButton4->click();
      else if (!myFace5) Group6F->PushButton5->click();
      else if (!myFace6) Group6F->PushButton6->click();
    }
    else if ( myEditCurrentArgument == Group6F->LineEdit2 ) {
      myFace2 = aSelectedObject;
      if      (!myFace3) Group6F->PushButton3->click();
      else if (!myFace4) Group6F->PushButton4->click();
      else if (!myFace5) Group6F->PushButton5->click();
      else if (!myFace6) Group6F->PushButton6->click();
      else if (!myFace1) Group6F->PushButton1->click();
    }
    else if ( myEditCurrentArgument == Group6F->LineEdit3 ) {
      myFace3 = aSelectedObject;
      if      (!myFace4) Group6F->PushButton4->click();
      else if (!myFace5) Group6F->PushButton5->click();
      else if (!myFace6) Group6F->PushButton6->click();
      else if (!myFace1) Group6F->PushButton1->click();
      else if (!myFace2) Group6F->PushButton2->click();
    }
    else if ( myEditCurrentArgument == Group6F->LineEdit4 ) {
      myFace4 = aSelectedObject;
      if      (!myFace5) Group6F->PushButton5->click();
      else if (!myFace6) Group6F->PushButton6->click();
      else if (!myFace1) Group6F->PushButton1->click();
      else if (!myFace2) Group6F->PushButton2->click();
      else if (!myFace3) Group6F->PushButton3->click();
    }
    else if ( myEditCurrentArgument == Group6F->LineEdit5 ) {
      myFace5 = aSelectedObject;
      if      (!myFace6) Group6F->PushButton6->click();
      else if (!myFace1) Group6F->PushButton1->click();
      else if (!myFace2) Group6F->PushButton2->click();
      else if (!myFace3) Group6F->PushButton3->click();
      else if (!myFace4) Group6F->PushButton4->click();
    }
    else if ( myEditCurrentArgument == Group6F->LineEdit6 ) {
      myFace6 = aSelectedObject;
      if      (!myFace1) Group6F->PushButton1->click();
      else if (!myFace2) Group6F->PushButton2->click();
      else if (!myFace3) Group6F->PushButton3->click();
      else if (!myFace4) Group6F->PushButton4->click();
      else if (!myFace5) Group6F->PushButton5->click();
    }
  }
  else {
    if      (myEditCurrentArgument == Group2F->LineEdit1) myFace1.nullify();
    else if (myEditCurrentArgument == Group2F->LineEdit2) myFace2.nullify();
    else if (myEditCurrentArgument == Group6F->LineEdit1) myFace1.nullify();
    else if (myEditCurrentArgument == Group6F->LineEdit2) myFace2.nullify();
    else if (myEditCurrentArgument == Group6F->LineEdit3) myFace3.nullify();
    else if (myEditCurrentArgument == Group6F->LineEdit4) myFace4.nullify();
    else if (myEditCurrentArgument == Group6F->LineEdit5) myFace5.nullify();
    else if (myEditCurrentArgument == Group6F->LineEdit6) myFace6.nullify();
    myEditCurrentArgument->setText( "" );
  }

  displayPreview(true);
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BlocksGUI_BlockDlg::SetEditCurrentArgument()
{
  QPushButton* aSender = (QPushButton*)sender();

  // clear selection
  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
  if (myInitial)
    myInitial = false;
  else
    myGeomGUI->getApp()->selectionMgr()->clearSelected();

  // disable all
  switch (myConstructorId) {
  case 0:
    Group2F->PushButton1->setDown(false);
    Group2F->PushButton2->setDown(false);

    Group2F->LineEdit1->setEnabled(false);
    Group2F->LineEdit2->setEnabled(false);
    break;
  case 1:
    Group6F->PushButton1->setDown(false);
    Group6F->PushButton2->setDown(false);
    Group6F->PushButton3->setDown(false);
    Group6F->PushButton4->setDown(false);
    Group6F->PushButton5->setDown(false);
    Group6F->PushButton6->setDown(false);

    Group6F->LineEdit1->setEnabled(false);
    Group6F->LineEdit2->setEnabled(false);
    Group6F->LineEdit3->setEnabled(false);
    Group6F->LineEdit4->setEnabled(false);
    Group6F->LineEdit5->setEnabled(false);
    Group6F->LineEdit6->setEnabled(false);
    break;
  default:
    break;
  }

  // set line edit as current argument
  if (aSender == Group2F->PushButton1) {
    myEditCurrentArgument = Group2F->LineEdit1;
  }
  else if (aSender == Group2F->PushButton2) {
    myEditCurrentArgument = Group2F->LineEdit2;
  }
  else if (aSender == Group6F->PushButton1) {
    myEditCurrentArgument = Group6F->LineEdit1;
  }
  else if (aSender == Group6F->PushButton2) {
    myEditCurrentArgument = Group6F->LineEdit2;
  }
  else if (aSender == Group6F->PushButton3) {
    myEditCurrentArgument = Group6F->LineEdit3;
  }
  else if (aSender == Group6F->PushButton4) {
    myEditCurrentArgument = Group6F->LineEdit4;
  }
  else if (aSender == Group6F->PushButton5) {
    myEditCurrentArgument = Group6F->LineEdit5;
  }
  else if (aSender == Group6F->PushButton6) {
    myEditCurrentArgument = Group6F->LineEdit6;
  }

  // enable push button and line edit
  myEditCurrentArgument->setEnabled(true);
  myEditCurrentArgument->setFocus();
  // after setFocus(), because it will be setDown(false) when loses focus
  aSender->setDown(true);

  globalSelection(); // close local contexts, if any
  localSelection(TopAbs_FACE); //Select Faces on All Shapes
  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void BlocksGUI_BlockDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  globalSelection(); // close local contexts, if any
  localSelection(TopAbs_FACE); //Select Faces on All Shapes
  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));
  displayPreview(true);
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void BlocksGUI_BlockDlg::enterEvent (QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr BlocksGUI_BlockDlg::createOperation()
{
  return getGeomEngine()->GetIBlocksOperations(getStudyId());
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool BlocksGUI_BlockDlg::isValid(QString&)
{
  bool ok = false;
  switch (getConstructorId()) {
  case 0:
    ok = myFace1 && myFace2;
    break;
  case 1:
    ok = myFace1 && myFace2 && myFace3 && myFace4 && myFace5 && myFace6;
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
bool BlocksGUI_BlockDlg::execute (ObjectList& objects)
{
  bool res = false;

  GEOM::GEOM_Object_var anObj;

  GEOM::GEOM_IBlocksOperations_var anOper = GEOM::GEOM_IBlocksOperations::_narrow(getOperation());

  switch (getConstructorId()) {
  case 0:
    if ( myFace1 && myFace2 ) {
      anObj = anOper->MakeHexa2Faces(myFace1.get(), myFace2.get());
      res = true;
    }
    break;
  case 1:
    if ( myFace1 && myFace2 && myFace3 && myFace4 && myFace5 && myFace6 ) {
      anObj = anOper->MakeHexa(myFace1.get(), myFace2.get(), myFace3.get(), myFace4.get(), myFace5.get(), myFace6.get());
      res = true;
    }
    break;
  }

  if (!anObj->_is_nil())
    objects.push_back(anObj._retn());

  return res;
}

//=================================================================================
// function : addSubshapesToStudy
// purpose  : virtual method to add new SubObjects if local selection
//=================================================================================
void BlocksGUI_BlockDlg::addSubshapesToStudy()
{
  switch (getConstructorId()) {
  case 0:
    GEOMBase::PublishSubObject( myFace1.get() );
    GEOMBase::PublishSubObject( myFace2.get() );
    break;
  case 1:
    GEOMBase::PublishSubObject( myFace1.get() );
    GEOMBase::PublishSubObject( myFace2.get() );
    GEOMBase::PublishSubObject( myFace3.get() );
    GEOMBase::PublishSubObject( myFace4.get() );
    GEOMBase::PublishSubObject( myFace5.get() );
    GEOMBase::PublishSubObject( myFace6.get() );
    break;
  default:
    break;
  }
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> BlocksGUI_BlockDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res;
  res << myFace1 << myFace2 << myFace3
      << myFace4 << myFace5 << myFace6;
  return res;
}
