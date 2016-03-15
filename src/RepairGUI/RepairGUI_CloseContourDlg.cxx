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
// File   : RepairGUI_CloseContourDlg.cxx
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.
//
#include "RepairGUI_CloseContourDlg.h"

#include "DlgRef.h"
#include "GeometryGUI.h"
#include "GEOMBase.h"
#include "RepairGUI.h"

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>

// OCCT Includes
#include <TopAbs.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>

#include <GEOMImpl_Types.hxx>

//=================================================================================
// class    : RepairGUI_CloseContourDlg()
// purpose  : Constructs a RepairGUI_CloseContourDlg  which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
RepairGUI_CloseContourDlg::RepairGUI_CloseContourDlg (GeometryGUI* theGeometryGUI, QWidget* parent,
                                                      bool modal)
  : GEOMBase_Skeleton(theGeometryGUI, parent, modal)
{
  QPixmap image0 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_CLOSECONTOUR")));
  QPixmap image1 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_SELECT")));

  setWindowTitle(tr("GEOM_CLOSECONTOUR_TITLE"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_CLOSECONTOUR_TITLE"));
  mainFrame()->RadioButton1->setIcon(image0);
  mainFrame()->RadioButton2->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton3->close();

  GroupPoints = new DlgRef_2SelExt(centralWidget());
  GroupPoints->GroupBox1->setTitle(tr("Contour to close"));
  GroupPoints->TextLabel1->setText(tr("GEOM_SELECTED_SHAPE"));
  GroupPoints->PushButton1->setIcon(image1);
  GroupPoints->LineEdit1->setReadOnly(true);

  GroupPoints->TextLabel2->setText(tr("Contour to close"));
  GroupPoints->PushButton2->setIcon(image1);
  GroupPoints->LineEdit2->setReadOnly(true);

  QRadioButton* rb1 = new QRadioButton(tr("Close by common vertex"), GroupPoints->Box);
  QRadioButton* rb2 = new QRadioButton(tr("Close by new edge"),      GroupPoints->Box);

  myIsVertexGr = new QButtonGroup(GroupPoints->Box);
  myIsVertexGr->addButton(rb1, 0);
  myIsVertexGr->addButton(rb2, 1);
  rb1->setChecked(true);

  QVBoxLayout* l = new QVBoxLayout(GroupPoints->Box);
  l->setMargin(0); l->setSpacing(6);
  l->addWidget(rb1);
  l->addWidget(rb2);

  QVBoxLayout* layout = new QVBoxLayout(centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
  layout->addWidget(GroupPoints);
  /***************************************************************/

  setHelpFileName("close_contour_operation_page.html");

  Init();
}

//=================================================================================
// function : ~RepairGUI_CloseContourDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
RepairGUI_CloseContourDlg::~RepairGUI_CloseContourDlg()
{
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void RepairGUI_CloseContourDlg::Init()
{
  // init variables
  GroupPoints->LineEdit1->clear();
  GroupPoints->LineEdit2->clear();
  myObject = GEOM::GEOM_Object::_nil();
  myWiresInd = new GEOM::short_array();
  myWiresInd->length(0);

  // signals and slots connections
  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(GroupPoints->PushButton1, SIGNAL(clicked()),       this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->PushButton2, SIGNAL(clicked()),       this, SLOT(SetEditCurrentArgument()));

  connect(GroupPoints->LineEdit1,   SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupPoints->LineEdit2,   SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  initName(tr("CLOSE_CONTOUR_NEW_OBJ_NAME"));

  GroupPoints->PushButton1->click();
  SelectionIntoArgument();
  resize(100,100);
}

//=================================================================================
// function : ClickOnOk()
// purpose  : Same than click on apply but close this dialog.
//=================================================================================
void RepairGUI_CloseContourDlg::ClickOnOk()
{
  setIsApplyAndClose( true );
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool RepairGUI_CloseContourDlg::ClickOnApply()
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
void RepairGUI_CloseContourDlg::SelectionIntoArgument()
{
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
        TopoDS_Shape aShape;
        if (GEOMBase::GetShape(myObject, aShape, TopAbs_WIRE))
          GroupPoints->LineEdit2->setText(myEditCurrentArgument->text());

        // clear selection
        disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
        myGeomGUI->getApp()->selectionMgr()->clearSelected();
        connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
                this, SLOT(SelectionIntoArgument()));

        GroupPoints->PushButton2->click();
      }
      else
        myObject = GEOM::GEOM_Object::_nil();
    }
    else if (myEditCurrentArgument == GroupPoints->LineEdit2) {
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
void RepairGUI_CloseContourDlg::SetEditCurrentArgument()
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
void RepairGUI_CloseContourDlg::LineEditReturnPressed()
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
void RepairGUI_CloseContourDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
           this, SLOT( SelectionIntoArgument() ) );

  myEditCurrentArgument = GroupPoints->LineEdit1;
  myEditCurrentArgument->setText( "" );
  GroupPoints->LineEdit2->setText( "" );
  myObject = GEOM::GEOM_Object::_nil();
  myWiresInd->length( 0 );

  initSelection();
}

//=================================================================================
// function : enterEvent()
// purpose  : Mouse enter onto the dialog to activate it
//=================================================================================
void RepairGUI_CloseContourDlg::enterEvent (QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr RepairGUI_CloseContourDlg::createOperation()
{
  return getGeomEngine()->GetIHealingOperations(getStudyId());
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool RepairGUI_CloseContourDlg::isValid (QString&)
{
  TopoDS_Shape aTmpShape;
  return !myObject->_is_nil() && (myWiresInd->length() ||
                                  GEOMBase::GetShape(myObject, aTmpShape, TopAbs_WIRE));
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool RepairGUI_CloseContourDlg::execute (ObjectList& objects)
{
  GEOM::GEOM_IHealingOperations_var anOper = GEOM::GEOM_IHealingOperations::_narrow(getOperation());
  GEOM::GEOM_Object_var anObj = anOper->CloseContour(myObject, myWiresInd, getIsByVertex());

  bool aResult = !anObj->_is_nil();
  if (aResult)
  {
    if ( !IsPreview() )
      RepairGUI::ShowStatistics( anOper, this );
    objects.push_back(anObj._retn());
  }
  return aResult;
}

//=================================================================================
// function : getIsByVertex
// purpose  :
//=================================================================================
bool RepairGUI_CloseContourDlg::getIsByVertex() const
{
  return myIsVertexGr->button(0)->isChecked();
}

//=================================================================================
// function : initSelection
// purpose  :
//=================================================================================
void RepairGUI_CloseContourDlg::initSelection()
{
  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);

  if (myEditCurrentArgument == GroupPoints->LineEdit1) {
    TColStd_MapOfInteger aTypes;
    aTypes.Add(GEOM_COMPOUND);
    aTypes.Add(GEOM_SOLID);
    aTypes.Add(GEOM_SHELL);
    aTypes.Add(GEOM_FACE);
    aTypes.Add(GEOM_WIRE);

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
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> RepairGUI_CloseContourDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res;
  GEOM::GeomObjPtr aGeomObjPtr(myObject);
  res << aGeomObjPtr;
  return res;
}
