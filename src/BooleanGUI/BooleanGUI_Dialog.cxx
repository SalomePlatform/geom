// Copyright (C) 2007-2013  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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
// File   : BooleanGUI_Dialog.cxx
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.
//
#include "BooleanGUI.h"
#include "BooleanGUI_Dialog.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>

// VSR 22/08/2012: issue 0021787: remove "Preview" button from BOP and Partition operations
// Comment next line to enable preview in BOP dialog box
#define NO_PREVIEW

//=================================================================================
// class    : BooleanGUI_Dialog()
// purpose  : Constructs a BooleanGUI_Dialog which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
BooleanGUI_Dialog::BooleanGUI_Dialog (const int theOperation, GeometryGUI* theGeometryGUI,
                                      QWidget* parent, bool modal, Qt::WindowFlags fl)
  : GEOMBase_Skeleton(theGeometryGUI, parent, modal, fl),
    myOperation(theOperation)
{
  QPixmap image0;
  QString aTitle, aCaption;
  switch (myOperation) {
  case BooleanGUI::COMMON:
    image0 = QPixmap(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_COMMON")));
    aTitle = tr("GEOM_COMMON");
    aCaption = tr("GEOM_COMMON_TITLE");
    setHelpFileName("common_operation_page.html");
    break;
  case BooleanGUI::CUT:
    image0 = QPixmap(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_CUT")));
    aTitle = tr("GEOM_CUT");
    aCaption = tr("GEOM_CUT_TITLE");
    setHelpFileName("cut_operation_page.html");
    break;
  case BooleanGUI::FUSE:
    image0 = QPixmap(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_FUSE")));
    aTitle = tr("GEOM_FUSE");
    aCaption = tr("GEOM_FUSE_TITLE");
    setHelpFileName("fuse_operation_page.html");
    break;
  case BooleanGUI::SECTION:
    image0 = QPixmap(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_SECTION")));
    aTitle = tr("GEOM_SECTION");
    aCaption = tr("GEOM_SECTION_TITLE");
    setHelpFileName("section_opeartion_page.html");
    break;
  }
  QPixmap image1(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_SELECT")));

  setWindowTitle(aCaption);

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(aTitle);
  mainFrame()->RadioButton1->setIcon(image0);
  mainFrame()->RadioButton2->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton3->close();

  myGroup = new DlgRef_2Sel(centralWidget());

  myGroup->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  if (myOperation != BooleanGUI::CUT) {
    myGroup->TextLabel1->setText(tr("GEOM_OBJECT_I").arg(1));
    myGroup->TextLabel2->setText(tr("GEOM_OBJECT_I").arg(2));
  }
  else {
    myGroup->TextLabel1->setText(tr("GEOM_MAIN_OBJECT"));
    myGroup->TextLabel2->setText(tr("GEOM_TOOL_OBJECT"));
  }

  myGroup->PushButton1->setIcon(image1);
  myGroup->PushButton2->setIcon(image1);
  myGroup->LineEdit1->setReadOnly(true);
  myGroup->LineEdit2->setReadOnly(true);

  QVBoxLayout* layout = new QVBoxLayout(centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
  layout->addWidget(myGroup);
  /***************************************************************/

#ifdef NO_PREVIEW
  mainFrame()->CheckBoxPreview->setChecked( false );
  mainFrame()->CheckBoxPreview->hide();
#endif
  // Initialisation
  Init();
}

//=================================================================================
// function : ~BooleanGUI_Dialog()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
BooleanGUI_Dialog::~BooleanGUI_Dialog()
{
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BooleanGUI_Dialog::Init()
{
  mainFrame()->GroupBoxPublish->show();

  // init variables
  myEditCurrentArgument = myGroup->LineEdit1;

  myGroup->LineEdit1->setText("");
  myGroup->LineEdit2->setText("");
  myObject1.nullify();
  myObject2.nullify();
 
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

  globalSelection(GEOM_ALLSHAPES);

  myGroup->PushButton1->click();
  SelectionIntoArgument();
  resize(100,100);
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void BooleanGUI_Dialog::ClickOnOk()
{
  setIsApplyAndClose( true );
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool BooleanGUI_Dialog::ClickOnApply()
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
void BooleanGUI_Dialog::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("");

  GEOM::GeomObjPtr aSelectedObject = getSelected( TopAbs_SHAPE );
  TopoDS_Shape aShape;
  if ( aSelectedObject && GEOMBase::GetShape( aSelectedObject.get(), aShape ) && !aShape.IsNull() ) {
    QString aName = GEOMBase::GetName( aSelectedObject.get() );
    myEditCurrentArgument->setText( aName );

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
void BooleanGUI_Dialog::SetEditCurrentArgument()
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
void BooleanGUI_Dialog::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();

  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
           this, SLOT( SelectionIntoArgument() ) );
  processPreview();
}

//=================================================================================
// function : enterEvent()
// purpose  : when mouse enter onto the QWidget
//=================================================================================
void BooleanGUI_Dialog::enterEvent (QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr BooleanGUI_Dialog::createOperation()
{
  return getGeomEngine()->GetIBooleanOperations(getStudyId());
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool BooleanGUI_Dialog::isValid (QString&)
{
  return myObject1 && myObject2;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool BooleanGUI_Dialog::execute (ObjectList& objects)
{
  GEOM::GEOM_Object_var anObj;

  GEOM::GEOM_IBooleanOperations_var anOper = GEOM::GEOM_IBooleanOperations::_narrow(getOperation());
  anObj = anOper->MakeBoolean(myObject1.get(), myObject2.get(), myOperation);
  if (!anObj->_is_nil())
    objects.push_back(anObj._retn());

  return true;
}

//=================================================================================
// function : restoreSubShapes
// purpose  :
//=================================================================================
void BooleanGUI_Dialog::restoreSubShapes (SALOMEDS::Study_ptr   theStudy,
                                          SALOMEDS::SObject_ptr theSObject)
{
  if (mainFrame()->CheckBoxRestoreSS->isChecked()) {
    // empty list of arguments means that all arguments should be restored
    getGeomEngine()->RestoreSubShapesSO(theStudy, theSObject, GEOM::ListOfGO(),
                                         /*theFindMethod=*/GEOM::FSM_GetInPlace, // ? GEOM::FSM_GetSame
                                         /*theInheritFirstArg=*/myOperation == BooleanGUI::CUT,
                                         mainFrame()->CheckBoxAddPrefix->isChecked()); // ? false
  }
}
