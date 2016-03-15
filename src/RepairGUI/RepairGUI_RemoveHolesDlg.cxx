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
// File   : RepairGUI_RemoveHolesDlg.cxx
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.
//
#include "RepairGUI_RemoveHolesDlg.h"

#include "DlgRef.h"
#include "GeometryGUI.h"
#include "GEOMBase.h"
#include "RepairGUI.h"

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_MessageBox.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>

// OCCT Includes
#include <TopAbs.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>

#include <GEOMImpl_Types.hxx>

//=================================================================================
// class    : RepairGUI_RemoveHolesDlg()
// purpose  : Constructs a RepairGUI_RemoveHolesDlg  which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
RepairGUI_RemoveHolesDlg::RepairGUI_RemoveHolesDlg (GeometryGUI* theGeometryGUI, QWidget* parent,
                                                    bool modal)
  : GEOMBase_Skeleton(theGeometryGUI, parent, modal)
{
  QPixmap image0 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_SUPPRESS_HOLES")));
  QPixmap image1 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_SELECT")));

  setWindowTitle(tr("GEOM_REMOVE_HOLES_TITLE"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_REMOVE_HOLES_TITLE"));
  mainFrame()->RadioButton1->setIcon(image0);
  mainFrame()->RadioButton2->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton3->close();

  GroupPoints = new DlgRef_1Sel1Check1Sel(centralWidget());
  GroupPoints->GroupBox1->setTitle(tr("GEOM_HOLES"));
  GroupPoints->TextLabel1->setText(tr("GEOM_SELECTED_SHAPE"));
  GroupPoints->PushButton1->setIcon(image1);
  GroupPoints->LineEdit1->setReadOnly(true);
  GroupPoints->CheckButton1->setText(tr("GEOM_REMOVE_ALL_HOLES"));
  GroupPoints->TextLabel2->setText(tr("GEOM_WIRES_TO_REMOVE"));
  GroupPoints->PushButton2->setIcon(image1);
  GroupPoints->LineEdit2->setReadOnly(true);

  myFreeBoundBtn = new QPushButton(tr("GEOM_DETECT") + QString(" [%1]").arg(tr("GEOM_FREE_BOUNDARIES")),
                                    GroupPoints->Box);
  QVBoxLayout* l = new QVBoxLayout(GroupPoints->Box);
  l->setMargin(0); l->setSpacing(0);
  l->addWidget(myFreeBoundBtn);

  QVBoxLayout* layout = new QVBoxLayout(centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
  layout->addWidget(GroupPoints);
  /***************************************************************/

  setHelpFileName("suppress_holes_operation_page.html");

  Init();
}

//=================================================================================
// function : ~RepairGUI_RemoveHolesDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
RepairGUI_RemoveHolesDlg::~RepairGUI_RemoveHolesDlg()
{
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void RepairGUI_RemoveHolesDlg::Init()
{
  // init variables
  GroupPoints->LineEdit1->clear();
  GroupPoints->LineEdit2->clear();
  myObject = GEOM::GEOM_Object::_nil();
  myWiresInd = new GEOM::short_array();
  myWiresInd->length(0);

  myClosed = -1;
  myOpen = -1;

  // signals and slots connections
  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(GroupPoints->PushButton1, SIGNAL(clicked()),       this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->PushButton2, SIGNAL(clicked()),       this, SLOT(SetEditCurrentArgument()));

  connect(GroupPoints->LineEdit1,   SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupPoints->LineEdit2,   SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(GroupPoints->CheckButton1, SIGNAL(clicked()), this, SLOT(onRemoveAllClicked()));

  connect(myFreeBoundBtn, SIGNAL(clicked()), this, SLOT(onDetect()));

  initName(tr("REMOVE_HOLES_NEW_OBJ_NAME"));

  GroupPoints->PushButton1->click();
  SelectionIntoArgument();
  resize(100,100);
}

//=================================================================================
// function : ClickOnOk()
// purpose  : Same than click on apply but close this dialog.
//=================================================================================
void RepairGUI_RemoveHolesDlg::ClickOnOk()
{
  setIsApplyAndClose( true );
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool RepairGUI_RemoveHolesDlg::ClickOnApply()
{
  if (!onAccept())
    return false;

  initName();
  // activate first line edit
  GroupPoints->PushButton1->click();
  return true;
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection is changed or on dialog initialization or activation
//=================================================================================
void RepairGUI_RemoveHolesDlg::SelectionIntoArgument()
{
  erasePreview();
  myEditCurrentArgument->setText("");
  // the second argument depends on the first one
  GroupPoints->LineEdit2->setText("");
  myWiresInd->length(0);

  if (myEditCurrentArgument == GroupPoints->LineEdit1)
    myObject = GEOM::GEOM_Object::_nil();

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  if (aSelList.Extent() == 1) {
    Handle(SALOME_InteractiveObject) anIO = aSelList.First();

    if (myEditCurrentArgument == GroupPoints->LineEdit1) { // face selection
      myObject = GEOMBase::ConvertIOinGEOMObject( anIO );
      if ( GEOMBase::IsShape(myObject) ) {
        myEditCurrentArgument->setText(GEOMBase::GetName(myObject));

        // clear selection
        disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
        myGeomGUI->getApp()->selectionMgr()->clearSelected();
        connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
                this, SLOT(SelectionIntoArgument()));

        if (!GroupPoints->CheckButton1->isChecked())
          GroupPoints->PushButton2->click();
      }
      else
        myObject = GEOM::GEOM_Object::_nil();
    }
    else if (myEditCurrentArgument == GroupPoints->LineEdit2 &&
             !GroupPoints->CheckButton1->isChecked()) {
      TColStd_IndexedMapOfInteger aMap;
      aSelMgr->GetIndexes(anIO, aMap);
      const int n = aMap.Extent();
      myWiresInd->length(n);
      for (int i = 1; i <= n; i++)
        myWiresInd[i-1] = aMap(i);
      if (n)
        myEditCurrentArgument->setText(QString::number(n) + "_" + tr("GEOM_WIRE") + tr("_S_"));
    }
  }
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void RepairGUI_RemoveHolesDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  bool isEffective = false;

  if (send == GroupPoints->PushButton1) {
    isEffective = true;
    myEditCurrentArgument = GroupPoints->LineEdit1;

    GroupPoints->PushButton2->setDown(false);
    GroupPoints->LineEdit2->setEnabled(false);
  }
  else if (send == GroupPoints->PushButton2 && !myObject->_is_nil()) {
    isEffective = true;
    myEditCurrentArgument = GroupPoints->LineEdit2;

    GroupPoints->PushButton1->setDown(false);
    GroupPoints->LineEdit1->setEnabled(false);
  }

  if (isEffective) {
    initSelection();

    // enable line edit
    myEditCurrentArgument->setEnabled(true);
    myEditCurrentArgument->setFocus();
    // after setFocus(), because it will be setDown(false) when loses focus
    send->setDown(true);
  }
}

//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void RepairGUI_RemoveHolesDlg::LineEditReturnPressed()
{
  const QObject* send = sender();
  if (send == GroupPoints->LineEdit1 || send == GroupPoints->LineEdit2) {
    myEditCurrentArgument = (QLineEdit*)send;
    GEOMBase_Skeleton::LineEditReturnPressed();
  }
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void RepairGUI_RemoveHolesDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
           this, SLOT( SelectionIntoArgument() ) );

  myEditCurrentArgument = GroupPoints->LineEdit1;
  myEditCurrentArgument->setText( "" );
  GroupPoints->LineEdit2->setText( "" );
  myObject = GEOM::GEOM_Object::_nil();
  myWiresInd->length( 0 );

  myClosed = -1;
  myOpen = -1;

  initSelection();
}

//=================================================================================
// function : enterEvent()
// purpose  : Mouse enter onto the dialog to activate it
//=================================================================================
void RepairGUI_RemoveHolesDlg::enterEvent (QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr RepairGUI_RemoveHolesDlg::createOperation()
{
  return getGeomEngine()->GetIHealingOperations(getStudyId());
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool RepairGUI_RemoveHolesDlg::isValid (QString&)
{
  myClosed = -1;
  return !myObject->_is_nil() && (IsPreview() || GroupPoints->CheckButton1->isChecked() || myWiresInd->length());
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool RepairGUI_RemoveHolesDlg::execute (ObjectList& objects)
{
  bool aResult = false;

  if (IsPreview()) {
    // called from onDetect(): detect free boundary edges,
    // highlight them (add to objects), display message dialog
    GEOM::ListOfGO_var aClosed, anOpen;

    GEOM::ListOfGO_var objList = new GEOM::ListOfGO;
    objList->length(1);
    objList[0] = myObject;
    GEOM::GEOM_IHealingOperations_var anOper = GEOM::GEOM_IHealingOperations::_narrow(getOperation());
    aResult = anOper->GetFreeBoundary(objList, aClosed, anOpen);

    if (aResult) {
      myClosed = aClosed->length();
      myOpen = anOpen->length();
      int i;
      for (i = 0; i < myClosed; i++)
        objects.push_back(aClosed[i]._retn());
      for (i = 0; i < myOpen; i++)
        objects.push_back(anOpen[i]._retn());
    }
    else
      myClosed = -1;
  }
  else {
    GEOM::GEOM_IHealingOperations_var anOper = GEOM::GEOM_IHealingOperations::_narrow(getOperation());
    GEOM::GEOM_Object_var anObj = anOper->FillHoles(myObject, myWiresInd);
    aResult = !anObj->_is_nil();
    if (aResult)
    {
      if ( !IsPreview() )
        RepairGUI::ShowStatistics( anOper, this );
      objects.push_back(anObj._retn());
    }
  }

  return aResult;
}

//=================================================================================
// function : onRemoveAllClicked
// purpose  :
//=================================================================================
void RepairGUI_RemoveHolesDlg::onRemoveAllClicked()
{
  bool b = GroupPoints->CheckButton1->isChecked();
  GroupPoints->TextLabel2->setEnabled(!b);
  GroupPoints->PushButton2->setEnabled(!b);
  GroupPoints->LineEdit2->setEnabled(!b);
  if (b) {
    GroupPoints->LineEdit2->setText("");
    myWiresInd->length(0);
  }
}

//=================================================================================
// function : initSelection
// purpose  :
//=================================================================================
void RepairGUI_RemoveHolesDlg::initSelection()
{
  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);

  if (myEditCurrentArgument == GroupPoints->LineEdit1) {
    TColStd_MapOfInteger aTypes;
    aTypes.Add(GEOM_COMPOUND);
    aTypes.Add(GEOM_SOLID);
    aTypes.Add(GEOM_SHELL);
    aTypes.Add(GEOM_FACE);

    globalSelection(aTypes);
  }
  else if (myEditCurrentArgument == GroupPoints->LineEdit2) {
    localSelection(myObject, TopAbs_EDGE);
    localSelection(myObject, TopAbs_WIRE);
  }

  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));
}

//=================================================================================
// function : onDetect
// purpose  :
//=================================================================================
void RepairGUI_RemoveHolesDlg::onDetect()
{
  displayPreview(true, false, true, true, 3);

  // field myClosed,myOpen is initialized in execute() method, called by displayPreview().
  QString msg;
  if (myClosed != -1)
    msg = tr("GEOM_FREE_BOUNDS_MSG").arg(myClosed + myOpen).arg(myClosed).arg(myOpen);
  else
    msg = tr("GEOM_FREE_BOUNDS_ERROR");
  SUIT_MessageBox::information(this, tr("GEOM_FREE_BOUNDS_TLT"), msg);
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> RepairGUI_RemoveHolesDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res;
  GEOM::GeomObjPtr aGeomObjPtr(myObject);
  res << aGeomObjPtr;
  return res;
}
