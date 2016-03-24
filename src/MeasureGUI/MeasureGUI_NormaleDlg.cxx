// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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
// File   : MeasureGUI_NormaleDlg.cxx
// Author : Julia DOROVSKIKH, Open CASCADE S.A.S.
//
#include "MeasureGUI.h"
#include "MeasureGUI_NormaleDlg.h"

#include <DlgRef.h>
#include <GEOMBase.h>
#include <GeometryGUI.h>

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>

// OCCT Includes
#include <TopoDS_Shape.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#include <GEOMImpl_Types.hxx>

//=================================================================================
// class    : MeasureGUI_NormaleDlg()
// purpose  : Constructs a MeasureGUI_NormaleDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
MeasureGUI_NormaleDlg::MeasureGUI_NormaleDlg (GeometryGUI* theGeometryGUI, QWidget* parent)
  : GEOMBase_Skeleton(theGeometryGUI, parent, false)
{
  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
  QPixmap image0 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_NORMALE")));
  QPixmap image1 (aResMgr->loadPixmap("GEOM", tr("ICON_SELECT")));

  setWindowTitle(tr("GEOM_NORMALE_TITLE"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_NORMALE"));
  mainFrame()->RadioButton1->setIcon(image0);
  mainFrame()->RadioButton2->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton3->close();

  GroupArgs = new DlgRef_2Sel (centralWidget());
  GroupArgs->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));

  GroupArgs->TextLabel1->setText(tr("GEOM_FACE"));
  GroupArgs->TextLabel2->setText(tr("GEOM_POINT"));

  GroupArgs->PushButton1->setIcon(image1);
  GroupArgs->PushButton2->setIcon(image1);

  QVBoxLayout* layout = new QVBoxLayout(centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
  layout->addWidget(GroupArgs);
  /***************************************************************/

  setHelpFileName("normal_page.html");

  Init();
}

//=================================================================================
// function : ~MeasureGUI_NormaleDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
MeasureGUI_NormaleDlg::~MeasureGUI_NormaleDlg()
{
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void MeasureGUI_NormaleDlg::Init()
{
  // init variables
  GroupArgs->LineEdit1->setReadOnly(true);
  GroupArgs->LineEdit2->setReadOnly(true);

  GroupArgs->LineEdit1->setText("");
  GroupArgs->LineEdit2->setText("");
  myFace = myPoint = GEOM::GEOM_Object::_nil();

  // signals and slots connections
  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(GroupArgs->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupArgs->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(GroupArgs->LineEdit1,   SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupArgs->LineEdit2,   SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  initName(tr("GEOM_VECTOR_NORMALE"));

  GroupArgs->PushButton1->click();
  SelectionIntoArgument();
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void MeasureGUI_NormaleDlg::ClickOnOk()
{
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool MeasureGUI_NormaleDlg::ClickOnApply()
{
  if (!onAccept())
    return false;

  initName();
  // activate first line edit
  GroupArgs->PushButton1->click();
  return true;
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection is changed or on dialog initialization or activation
//=================================================================================
void MeasureGUI_NormaleDlg::SelectionIntoArgument()
{
  erasePreview();
  myEditCurrentArgument->setText("");

  if (myEditCurrentArgument == GroupArgs->LineEdit1) {
    myFace = GEOM::GEOM_Object::_nil();
  }
  else if (myEditCurrentArgument == GroupArgs->LineEdit2) {
    myPoint = GEOM::GEOM_Object::_nil();
  }

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  if (aSelList.Extent() != 1)
    return;

  // nbSel == 1
  GEOM::GEOM_Object_var aSelectedObject =
    GEOMBase::ConvertIOinGEOMObject( aSelList.First() );

  if ( CORBA::is_nil(aSelectedObject) )
    return;

  QString aName = GEOMBase::GetName(aSelectedObject);

  if (myEditCurrentArgument == GroupArgs->LineEdit1) {
    TopoDS_Shape aShape;
    if (GEOMBase::GetShape(aSelectedObject, aShape, TopAbs_SHAPE) && !aShape.IsNull())
    {
      TColStd_IndexedMapOfInteger aMap;
      aSelMgr->GetIndexes(aSelList.First(), aMap);
      if (aMap.Extent() == 1) // Local Selection
      {
        GEOM::GEOM_IShapesOperations_var aShapesOp = getGeomEngine()->GetIShapesOperations(getStudyId());
        int anIndex = aMap(1);
        aSelectedObject = aShapesOp->GetSubShape(aSelectedObject, anIndex);
        aName += QString(":face_%1").arg(anIndex);
      }
      else // Global Selection
      {
        if (aShape.ShapeType() != TopAbs_FACE) {
          aSelectedObject = GEOM::GEOM_Object::_nil();
          aName = "";
        }
      }
    }
    myFace = aSelectedObject;
    myEditCurrentArgument->setText(aName);

    // clear selection
    disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
    myGeomGUI->getApp()->selectionMgr()->clearSelected();
    connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
            this, SLOT(SelectionIntoArgument()));

    if (!myFace->_is_nil() && myPoint->_is_nil())
      GroupArgs->PushButton2->click();
  }
  else if (myEditCurrentArgument == GroupArgs->LineEdit2) {
    TopoDS_Shape aShape;
    if (GEOMBase::GetShape(aSelectedObject, aShape, TopAbs_SHAPE) && !aShape.IsNull())
    {
      TColStd_IndexedMapOfInteger aMap;
      aSelMgr->GetIndexes(aSelList.First(), aMap);
      if (aMap.Extent() == 1) // Local Selection
      {
        GEOM::GEOM_IShapesOperations_var aShapesOp = getGeomEngine()->GetIShapesOperations(getStudyId());
        int anIndex = aMap(1);
        aSelectedObject = aShapesOp->GetSubShape(aSelectedObject, anIndex);
        aName += QString(":vertex_%1").arg(anIndex);
      }
      else // Global Selection
      {
        if (aShape.ShapeType() != TopAbs_VERTEX) {
          aSelectedObject = GEOM::GEOM_Object::_nil();
          aName = "";
        }
      }
    }
    myPoint = aSelectedObject;
    myEditCurrentArgument->setText(aName);

    // clear selection
    disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
    myGeomGUI->getApp()->selectionMgr()->clearSelected();
    connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
            this, SLOT(SelectionIntoArgument()));

    if (!myPoint->_is_nil() && myFace->_is_nil())
      GroupArgs->PushButton1->click();
  }

  DISPLAY_PREVIEW_MACRO
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void MeasureGUI_NormaleDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
  globalSelection(GEOM_FACE);

  if (send == GroupArgs->PushButton1) {
    myEditCurrentArgument = GroupArgs->LineEdit1;

    GroupArgs->PushButton2->setDown(false);
    GroupArgs->LineEdit2->setEnabled(false);

    localSelection(TopAbs_FACE);
  }
  else if (send == GroupArgs->PushButton2) {
    myEditCurrentArgument = GroupArgs->LineEdit2;

    GroupArgs->PushButton1->setDown(false);
    GroupArgs->LineEdit1->setEnabled(false);

    localSelection(TopAbs_VERTEX);
  }
  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));

  // enable line edit
  myEditCurrentArgument->setEnabled(true);
  myEditCurrentArgument->setFocus();
  // after setFocus(), because it will be setDown(false) when loses focus
  send->setDown(true);

  // seems we need it only to avoid preview disappearing, caused by selection mode change
  DISPLAY_PREVIEW_MACRO
}

//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void MeasureGUI_NormaleDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if (send == GroupArgs->LineEdit1 ||
      send == GroupArgs->LineEdit2) {
    myEditCurrentArgument = send;
    GEOMBase_Skeleton::LineEditReturnPressed();
  }
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void MeasureGUI_NormaleDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
           this, SLOT( SelectionIntoArgument() ) );

  DISPLAY_PREVIEW_MACRO
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void MeasureGUI_NormaleDlg::enterEvent (QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr MeasureGUI_NormaleDlg::createOperation()
{
  return getGeomEngine()->GetIMeasureOperations(getStudyId());
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool MeasureGUI_NormaleDlg::isValid (QString&)
{
  //return !CORBA::is_nil(myFace) && !CORBA::is_nil(myPoint);
  return !CORBA::is_nil(myFace);
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool MeasureGUI_NormaleDlg::execute (ObjectList& objects)
{
  GEOM::GEOM_IMeasureOperations_var anOper = GEOM::GEOM_IMeasureOperations::_narrow( getOperation() );
  GEOM::GEOM_Object_var anObj = anOper->GetNormal(myFace, myPoint);

  if (!anObj->_is_nil())
    objects.push_back(anObj._retn());

  return true;
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> MeasureGUI_NormaleDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res;
  GEOM::GeomObjPtr aGeomObjPtr1(myFace), aGeomObjPtr2(myPoint);
  res << aGeomObjPtr1 << aGeomObjPtr2;
  return res;
}
