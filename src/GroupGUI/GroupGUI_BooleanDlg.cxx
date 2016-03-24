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
// File   : GroupGUI_BooleanDlg.cxx
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.

#include "GroupGUI_BooleanDlg.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>

#include <GEOMImpl_Types.hxx>

//=================================================================================
// class    : GroupGUI_BooleanDlg()
// purpose  : Constructs a GroupGUI_BooleanDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
GroupGUI_BooleanDlg::GroupGUI_BooleanDlg (const int theOperation, GeometryGUI* theGeometryGUI,
                                          QWidget* parent, bool modal, Qt::WindowFlags fl)
  : GEOMBase_Skeleton(theGeometryGUI, parent, modal, fl),
    myOperation(theOperation)
{
  QPixmap image0;
  QString aTitle, aCaption;
  switch (myOperation) {
  case UNION:
    image0 = QPixmap(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_FUSE")));
    aTitle = tr("GEOM_UNION");
    aCaption = tr("GEOM_UNION_TITLE");
    setHelpFileName("work_with_groups_page.html#union_groups_anchor");
    break;
  case INTERSECT:
    image0 = QPixmap(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_COMMON")));
    aTitle = tr("GEOM_INTERSECT");
    aCaption = tr("GEOM_INTERSECT_TITLE");
    setHelpFileName("work_with_groups_page.html#intersect_groups_anchor");
    break;
  case CUT:
    image0 = QPixmap(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_CUT")));
    aTitle = tr("GEOM_CUT");
    aCaption = tr("GEOM_CUT_TITLE");
    setHelpFileName("work_with_groups_page.html#cut_groups_anchor");
    break;
  }
  QPixmap image1 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_SELECT")));

  setWindowTitle(aCaption);

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(aTitle);
  mainFrame()->RadioButton1->setIcon(image0);
  mainFrame()->RadioButton2->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton3->close();

  myGroup = new DlgRef_2Sel (centralWidget());

  myGroup->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  if (myOperation == CUT) {
    myGroup->TextLabel1->setText(tr("GEOM_MAIN_GROUPS"));
    myGroup->TextLabel2->setText(tr("GEOM_TOOL_GROUPS"));
  }
  else {
    myGroup->TextLabel1->setText(tr("GEOM_GROUPS").arg(1));
    myGroup->TextLabel2->hide();
    myGroup->PushButton2->hide();
    myGroup->LineEdit2->hide();
  }

  myGroup->PushButton1->setIcon(image1);
  myGroup->PushButton2->setIcon(image1);
  myGroup->LineEdit1->setReadOnly(true);
  myGroup->LineEdit2->setReadOnly(true);

  QVBoxLayout* layout = new QVBoxLayout (centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
  layout->addWidget(myGroup);
  /***************************************************************/

  // Initialisation
  Init();
}

//=================================================================================
// function : ~GroupGUI_BooleanDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
GroupGUI_BooleanDlg::~GroupGUI_BooleanDlg()
{
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void GroupGUI_BooleanDlg::Init()
{
  // init variables
  myEditCurrentArgument = myGroup->LineEdit1;

  myGroup->LineEdit1->setText("");
  myGroup->LineEdit2->setText("");
  myListShapes.length( 0 );
  myListTools.length( 0 );

  // signals and slots connections
  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(myGroup->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(myGroup->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(),
           SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  initName(mainFrame()->GroupConstructors->title());

  setTabOrder(mainFrame()->GroupConstructors, mainFrame()->GroupBoxName);
  setTabOrder(mainFrame()->GroupBoxName, mainFrame()->GroupMedium);
  setTabOrder(mainFrame()->GroupMedium, mainFrame()->GroupButtons);

  mainFrame()->RadioButton1->setFocus();

  globalSelection(GEOM_GROUP);

  myGroup->PushButton1->click();
  SelectionIntoArgument();
  resize(100,100);
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void GroupGUI_BooleanDlg::ClickOnOk()
{
  setIsApplyAndClose(true);
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool GroupGUI_BooleanDlg::ClickOnApply()
{
  if (!onAccept())
    return false;

  initName();
  // activate selection and connect selection manager
  myGroup->PushButton1->click();
  return true;
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection is changed or on dialog initialization or activation
//=================================================================================
void GroupGUI_BooleanDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("");

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  QString aString = "";
  GEOMBase::GetNameOfSelectedIObjects(aSelList, aString, true);

  if (myEditCurrentArgument == myGroup->LineEdit1) {
    GEOMBase::ConvertListOfIOInListOfGO(aSelList, myListShapes, true);
  }
  else if ( myEditCurrentArgument == myGroup->LineEdit2 ) {
    GEOMBase::ConvertListOfIOInListOfGO(aSelList, myListTools, true);
  }

  myEditCurrentArgument->setText(aString);
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void GroupGUI_BooleanDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  if (send == myGroup->PushButton1) {
    myEditCurrentArgument = myGroup->LineEdit1;

    myGroup->PushButton2->setDown(false);
    myGroup->LineEdit2->setEnabled(false);
  }
  else if (send == myGroup->PushButton2) {
    myEditCurrentArgument = myGroup->LineEdit2;

    myGroup->PushButton1->setDown(false);
    myGroup->LineEdit1->setEnabled(false);
  }

  // enable line edit
  myEditCurrentArgument->setEnabled(true);
  myEditCurrentArgument->setFocus();
  // after setFocus(), because it will be setDown(false) when loses focus
  send->setDown(true);
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void GroupGUI_BooleanDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();

  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
           this, SLOT(SelectionIntoArgument()));
}

//=================================================================================
// function : enterEvent()
// purpose  : when mouse enter onto the QWidget
//=================================================================================
void GroupGUI_BooleanDlg::enterEvent (QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr GroupGUI_BooleanDlg::createOperation()
{
  return getGeomEngine()->GetIGroupOperations(getStudyId());
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool GroupGUI_BooleanDlg::isValid (QString&)
{
  return (myListShapes.length() > 0);
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool GroupGUI_BooleanDlg::execute (ObjectList& objects)
{
  GEOM::GEOM_Object_var anObj;

  GEOM::GEOM_IGroupOperations_var anOper = GEOM::GEOM_IGroupOperations::_narrow(getOperation());
  switch (myOperation) {
  case UNION:
    anObj = anOper->UnionListOfGroups(myListShapes);
    break;
  case INTERSECT:
    anObj = anOper->IntersectListOfGroups(myListShapes);
    break;
  case CUT:
    anObj = anOper->CutListOfGroups(myListShapes, myListTools);
    break;
  default:
    ;
  }
  if (!anObj->_is_nil())
    objects.push_back(anObj._retn());

  return true;
}

//================================================================
// Function : getFather
// Purpose  : Get father object for object to be added in study
//            (called with addInStudy method)
//================================================================
GEOM::GEOM_Object_ptr GroupGUI_BooleanDlg::getFather(GEOM::GEOM_Object_ptr theObj)
{
  GEOM::GEOM_Object_var aFatherObj;
  if (theObj->GetType() == GEOM_GROUP) {
    GEOM::GEOM_IGroupOperations_var anOper = GEOM::GEOM_IGroupOperations::_narrow(getOperation());
    aFatherObj = anOper->GetMainShape(theObj);
  }
  return aFatherObj._retn();
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> GroupGUI_BooleanDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res;
  for (int i = 0; i < myListShapes.length(); i++) {
    GEOM::GeomObjPtr aGeomObjPtr(myListShapes[i]);
    res << aGeomObjPtr;
  }
  for (int i = 0; i < myListTools.length(); i++) {
    GEOM::GeomObjPtr aGeomObjPtr(myListTools[i]);
    res << aGeomObjPtr;
  }
  return res;
}
