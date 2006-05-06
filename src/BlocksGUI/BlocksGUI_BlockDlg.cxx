//  GEOM GEOMGUI : GUI for Geometry component
//
//  Copyright (C) 2003  CEA
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
//  See http://www.salome-platform.org or email : webmaster.salome@opencascade.org
//
//
//
//  File   : BlocksGUI_BlockDlg.cxx
//  Author : Julia DOROVSKIKH
//  Module : GEOM
//  $Header$

#include "BlocksGUI_BlockDlg.h"

#include "SUIT_Session.h"
#include "SalomeApp_Application.h"
#include "LightApp_SelectionMgr.h"

#include "GEOMImpl_Types.hxx"

using namespace std;

#include <qlabel.h>

//=================================================================================
// class    : BlocksGUI_BlockDlg()
// purpose  : Constructs a BlocksGUI_BlockDlg which is a child of 'parent'.
//=================================================================================
BlocksGUI_BlockDlg::BlocksGUI_BlockDlg (GeometryGUI* theGeometryGUI, QWidget* parent, bool modal)
     : GEOMBase_Skeleton(theGeometryGUI, parent, "BlockDlg", modal,
                         WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_BLOCK_2F")));
  QPixmap image1 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_BLOCK_6F")));
  QPixmap imageS (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_SELECT")));

  setCaption(tr("GEOM_BLOCK_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_BLOCK"));

  RadioButton1->setPixmap(image0);
  RadioButton2->setPixmap(image1);
  RadioButton3->close(TRUE);

  // Create first group
  Group2F = new DlgRef_2Sel_QTD(this, "Group2F");
  Group2F->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  Group2F->TextLabel1->setText(tr("FACE_1"));
  Group2F->TextLabel2->setText(tr("FACE_2"));
  Group2F->PushButton1->setPixmap(imageS);
  Group2F->PushButton2->setPixmap(imageS);

  // Create second group
  Group6F = new DlgRef_6Sel_QTD(this, "Group2F");
  Group6F->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  Group6F->TextLabel1->setText(tr("FACE_1"));
  Group6F->TextLabel2->setText(tr("FACE_2"));
  Group6F->TextLabel3->setText(tr("FACE_3"));
  Group6F->TextLabel4->setText(tr("FACE_4"));
  Group6F->TextLabel5->setText(tr("FACE_5"));
  Group6F->TextLabel6->setText(tr("FACE_6"));
  Group6F->PushButton1->setPixmap(imageS);
  Group6F->PushButton2->setPixmap(imageS);
  Group6F->PushButton3->setPixmap(imageS);
  Group6F->PushButton4->setPixmap(imageS);
  Group6F->PushButton5->setPixmap(imageS);
  Group6F->PushButton6->setPixmap(imageS);

  // Add groups to layout
  Layout1->addWidget(Group2F, 2, 0);
  Layout1->addWidget(Group6F, 2, 0);
  /***************************************************************/

  setHelpFileName("newentity_blocks.htm#HexahedralSolid");

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
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));
  connect(GroupConstructors, SIGNAL(clicked(int)), this, SLOT(ConstructorsClicked(int)));

  connect(Group2F->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(Group2F->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(Group6F->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(Group6F->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(Group6F->PushButton3, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(Group6F->PushButton4, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(Group6F->PushButton5, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(Group6F->PushButton6, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument())) ;

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

  switch (constructorId) {
  case 0:
    Group6F->hide();
    resize(0, 0);
    Group2F->show();

    myEditCurrentArgument = Group2F->LineEdit1;
    Group2F->LineEdit1->setText(tr(""));
    Group2F->LineEdit2->setText(tr(""));
    break;
  case 1:
    Group2F->hide();
    resize(0, 0);
    Group6F->show();

    myEditCurrentArgument = Group6F->LineEdit1;
    Group6F->LineEdit1->setText(tr(""));
    Group6F->LineEdit2->setText(tr(""));
    Group6F->LineEdit3->setText(tr(""));
    Group6F->LineEdit4->setText(tr(""));
    Group6F->LineEdit5->setText(tr(""));
    Group6F->LineEdit6->setText(tr(""));
    break;
  default:
    break;
  }

  // init fields
  myFace1 = myFace2 = GEOM::GEOM_Object::_nil();
  myFace3 = myFace4 = myFace5 = myFace6 = myFace1;

  globalSelection(GEOM_FACE);
  SelectionIntoArgument();
  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void BlocksGUI_BlockDlg::ClickOnOk()
{
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
// purpose  : Called when selection as changed or other case
//=================================================================================
void BlocksGUI_BlockDlg::SelectionIntoArgument()
{
  erasePreview();
  myEditCurrentArgument->setText("");

  if (IObjectCount() != 1)
  {
    if (myEditCurrentArgument == Group2F->LineEdit1 ||
        myEditCurrentArgument == Group6F->LineEdit1)
      myFace1 = GEOM::GEOM_Object::_nil();
    else if (myEditCurrentArgument == Group2F->LineEdit2 ||
             myEditCurrentArgument == Group6F->LineEdit2)
      myFace2 = GEOM::GEOM_Object::_nil();
    else if (myEditCurrentArgument == Group6F->LineEdit3)
      myFace3 = GEOM::GEOM_Object::_nil();
    else if (myEditCurrentArgument == Group6F->LineEdit4)
      myFace4 = GEOM::GEOM_Object::_nil();
    else if (myEditCurrentArgument == Group6F->LineEdit5)
      myFace5 = GEOM::GEOM_Object::_nil();
    else if (myEditCurrentArgument == Group6F->LineEdit6)
      myFace6 = GEOM::GEOM_Object::_nil();
    return;
  }

  // nbSel == 1
  Standard_Boolean testResult = Standard_False;
  GEOM::GEOM_Object_var aSelectedObject =
    GEOMBase::ConvertIOinGEOMObject( firstIObject(), testResult );

  if (!testResult || CORBA::is_nil( aSelectedObject ))
    return;

  if (myEditCurrentArgument == Group2F->LineEdit1 ||
      myEditCurrentArgument == Group6F->LineEdit1)
    myFace1 = aSelectedObject;
  else if (myEditCurrentArgument == Group2F->LineEdit2 ||
           myEditCurrentArgument == Group6F->LineEdit2)
    myFace2 = aSelectedObject;
  else if (myEditCurrentArgument == Group6F->LineEdit3)
    myFace3 = aSelectedObject;
  else if (myEditCurrentArgument == Group6F->LineEdit4)
    myFace4 = aSelectedObject;
  else if (myEditCurrentArgument == Group6F->LineEdit5)
    myFace5 = aSelectedObject;
  else if (myEditCurrentArgument == Group6F->LineEdit6)
    myFace6 = aSelectedObject;

  myEditCurrentArgument->setText( GEOMBase::GetName( aSelectedObject ) );

  displayPreview();
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BlocksGUI_BlockDlg::SetEditCurrentArgument()
{
  QPushButton* aSender = (QPushButton*)sender();

  if (aSender == Group2F->PushButton1) {
    myEditCurrentArgument = Group2F->LineEdit1;
  } else if (aSender == Group2F->PushButton2) {
    myEditCurrentArgument = Group2F->LineEdit2;

  } else if (aSender == Group6F->PushButton1) {
    myEditCurrentArgument = Group6F->LineEdit1;
  } else if (aSender == Group6F->PushButton2) {
    myEditCurrentArgument = Group6F->LineEdit2;
  } else if (aSender == Group6F->PushButton3) {
    myEditCurrentArgument = Group6F->LineEdit3;
  } else if (aSender == Group6F->PushButton4) {
    myEditCurrentArgument = Group6F->LineEdit4;
  } else if (aSender == Group6F->PushButton5) {
    myEditCurrentArgument = Group6F->LineEdit5;
  } else if (aSender == Group6F->PushButton6) {
    myEditCurrentArgument = Group6F->LineEdit6;
  }

  globalSelection(GEOM_FACE);

  myEditCurrentArgument->setFocus();
  SelectionIntoArgument();
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void BlocksGUI_BlockDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  globalSelection(GEOM_FACE);

  SelectionIntoArgument();
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void BlocksGUI_BlockDlg::enterEvent (QEvent* e)
{
  if (!GroupConstructors->isEnabled())
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
bool BlocksGUI_BlockDlg::isValid (QString&)
{
  switch (getConstructorId()) {
    case 0:
      return !(myFace1->_is_nil() || myFace2->_is_nil());
    case 1:
      return !(myFace1->_is_nil() || myFace2->_is_nil() ||
               myFace3->_is_nil() || myFace4->_is_nil() ||
               myFace5->_is_nil() || myFace6->_is_nil());
    default:
      return false;
  }
  return false;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool BlocksGUI_BlockDlg::execute (ObjectList& objects)
{
  bool res = false;

  GEOM::GEOM_Object_var anObj;

  switch (getConstructorId()) {
    case 0:
      if (!CORBA::is_nil(myFace1) && !CORBA::is_nil(myFace2)) {
        anObj = GEOM::GEOM_IBlocksOperations::_narrow(getOperation())->
          MakeHexa2Faces(myFace1, myFace2);
        res = true;
      }
      break;
    case 1:
      if (!CORBA::is_nil(myFace1) && !CORBA::is_nil(myFace2) &&
          !CORBA::is_nil(myFace3) && !CORBA::is_nil(myFace4) &&
          !CORBA::is_nil(myFace5) && !CORBA::is_nil(myFace6)) {
        anObj = GEOM::GEOM_IBlocksOperations::_narrow(getOperation())->
          MakeHexa(myFace1, myFace2, myFace3, myFace4, myFace5, myFace6);
        res = true;
      }
      break;
  }

  if (!anObj->_is_nil())
    objects.push_back(anObj._retn());

  return res;
}
