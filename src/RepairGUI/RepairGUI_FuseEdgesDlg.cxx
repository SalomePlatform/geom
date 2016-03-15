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

#include "RepairGUI_FuseEdgesDlg.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <SUIT_Desktop.h>
#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_ViewWindow.h>
#include <SUIT_ViewManager.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>
#include <OCCViewer_ViewModel.h>

// OCCT Includes
#include <TColStd_MapOfInteger.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>

#include <GEOMImpl_Types.hxx>

//=================================================================================
// class    : RepairGUI_FuseEdgesDlg()
// purpose  : Constructs a RepairGUI_FuseEdgesDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
RepairGUI_FuseEdgesDlg::RepairGUI_FuseEdgesDlg (GeometryGUI* theGeometryGUI,
                                                QWidget* parent)
  : GEOMBase_Skeleton(theGeometryGUI, parent, false)
{
  SUIT_ResourceMgr* aResMgr = myGeomGUI->getApp()->resourceMgr();
  QPixmap image0 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_FUSE_EDGES")));
  QPixmap iconSelect (aResMgr->loadPixmap("GEOM", tr("ICON_SELECT")));

  setWindowTitle(tr("GEOM_FUSE_EDGES_TITLE"));

  mainFrame()->GroupConstructors->setTitle(tr("GEOM_FUSE_EDGES"));
  mainFrame()->RadioButton1->setIcon(image0);
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->close();

  GroupVertexes = new DlgRef_2Sel1Spin (centralWidget());
  GroupVertexes->GroupBox1->setTitle(tr("GEOM_FUSE_EDGES"));
  GroupVertexes->TextLabel1->setText(tr("GEOM_WIRE"));
  GroupVertexes->TextLabel2->setText(tr("GEOM_VERTEXES"));
  GroupVertexes->PushButton1->setIcon(iconSelect);
  GroupVertexes->PushButton2->setIcon(iconSelect);
  GroupVertexes->LineEdit1->setReadOnly(true);
  GroupVertexes->LineEdit2->setReadOnly(true);

  GroupVertexes->TextLabel3->setVisible(false);
  GroupVertexes->SpinBox_DX->setVisible(false);

  QVBoxLayout* layout = new QVBoxLayout (centralWidget());
  layout->setMargin(0);
  layout->setSpacing(6);
  layout->addWidget(GroupVertexes);

  setHelpFileName("fuse_edges_operation_page.html");

  // Initialisation
  Init();
  resize(100,100);
}

//=================================================================================
// function : ~RepairGUI_FuseEdgesDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
RepairGUI_FuseEdgesDlg::~RepairGUI_FuseEdgesDlg()
{
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void RepairGUI_FuseEdgesDlg::Init()
{
  // Clear line edits
  GroupVertexes->LineEdit1->setText("");
  GroupVertexes->LineEdit2->setText("");

  myShape = GEOM::GEOM_Object::_nil();

  myPoints.clear();

  // signals and slots connections
  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(GroupVertexes->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupVertexes->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(GroupVertexes->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupVertexes->LineEdit2, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  initName(tr("FUSE_EDGES_NEW_OBJ_NAME"));
  GroupVertexes->PushButton1->click();

  SelectionIntoArgument();
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void RepairGUI_FuseEdgesDlg::ClickOnOk()
{
  setIsApplyAndClose(true);
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool RepairGUI_FuseEdgesDlg::ClickOnApply()
{
  if (!onAccept())
    return false;

  initName();
  // Reset dialog state
  GroupVertexes->LineEdit1->setText("");
  GroupVertexes->LineEdit2->setText("");
  myShape = GEOM::GEOM_Object::_nil();
  myPoints.clear();
  GroupVertexes->PushButton1->click();

  return true;
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection is changed or on dialog initialization or activation
//=================================================================================
void RepairGUI_FuseEdgesDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("");

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  // If selection of main object is activated
  if (myEditCurrentArgument == GroupVertexes->LineEdit1) {
    myShape = GEOM::GEOM_Object::_nil();
    if (aSelList.Extent() == 1) {
      GEOM::GEOM_Object_var anObj =
        GEOMBase::ConvertIOinGEOMObject(aSelList.First());

      if (!anObj->_is_nil()) {
        QString aName = GEOMBase::GetName(anObj);
        TopoDS_Shape aShape;
        if (GEOMBase::GetShape(anObj, aShape, TopAbs_SHAPE) && !aShape.IsNull()) {
          TColStd_IndexedMapOfInteger aMap;
          aSelMgr->GetIndexes(aSelList.First(), aMap);
          if (aMap.Extent() == 1) { // Local Selection
            int anIndex = aMap(1);
            aName += QString(":wire_%1").arg(anIndex);

            //Find SubShape Object in Father
            GEOM::GEOM_Object_var aFindedObject = GEOMBase_Helper::findObjectInFather(anObj, aName);

            if (aFindedObject->_is_nil()) { // Object not found in study
              GEOM::GEOM_IShapesOperations_var aShapesOp = getGeomEngine()->GetIShapesOperations(getStudyId());
              anObj = aShapesOp->GetSubShape(anObj, anIndex);
            }
            else
              anObj = aFindedObject; // get Object from study
          }
          else { // Global Selection
            if (aShape.ShapeType() != TopAbs_WIRE) {
              anObj = GEOM::GEOM_Object::_nil();
              aName = "";
            }
          }
        }
        myShape = anObj;
        myEditCurrentArgument->setText(aName);
      }
    }

    if (!myShape->_is_nil() && myPoints.isEmpty())
      GroupVertexes->PushButton2->click();
  }
  else if (myEditCurrentArgument == GroupVertexes->LineEdit2) {
    myPoints = getSelected(TopAbs_VERTEX, -1);
    if (!myPoints.isEmpty())
      myEditCurrentArgument->setText(QString::number(myPoints.count()) + "_" + tr("GEOM_POINT") + tr("_S_"));
    else
      myEditCurrentArgument->setText("");
  }
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void RepairGUI_FuseEdgesDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  if (send == GroupVertexes->PushButton1) {
    myEditCurrentArgument = GroupVertexes->LineEdit1;
    GroupVertexes->PushButton2->setDown(false);
    GroupVertexes->LineEdit2->setEnabled(false);
  }
  else if (send == GroupVertexes->PushButton2) {
    myEditCurrentArgument = GroupVertexes->LineEdit2;
    GroupVertexes->PushButton1->setDown(false);
    GroupVertexes->LineEdit1->setEnabled(false);
  }

  // enable line edit
  myEditCurrentArgument->setEnabled(true);
  myEditCurrentArgument->setFocus();
  // after setFocus(), because it will be setDown(false) when loses focus
  send->setDown(true);

  activateSelection();
}

//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void RepairGUI_FuseEdgesDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();

  if (send == GroupVertexes->LineEdit1)
    myEditCurrentArgument = GroupVertexes->LineEdit1;
  else if (send == GroupVertexes->LineEdit2)
    myEditCurrentArgument = GroupVertexes->LineEdit2;
  else
    return;

  GEOMBase_Skeleton::LineEditReturnPressed();
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void RepairGUI_FuseEdgesDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void RepairGUI_FuseEdgesDlg::enterEvent (QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    this->ActivateThisDialog();
}

//=================================================================================
// function : activateSelection
// purpose  : Activate selection in accordance with myEditCurrentArgument
//=================================================================================
void RepairGUI_FuseEdgesDlg::activateSelection()
{
  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
  globalSelection();
  if (myEditCurrentArgument == GroupVertexes->LineEdit1)
    globalSelection(GEOM_WIRE);
  else if (!myShape->_is_nil() && myEditCurrentArgument == GroupVertexes->LineEdit2)
    localSelection(myShape, TopAbs_VERTEX);

  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr RepairGUI_FuseEdgesDlg::createOperation()
{
  return getGeomEngine()->GetIHealingOperations(getStudyId());
}

//=================================================================================
// function : isValid()
// purpose  : Verify validity of input data
//=================================================================================
bool RepairGUI_FuseEdgesDlg::isValid (QString& msg)
{
  return (!myShape->_is_nil());
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool RepairGUI_FuseEdgesDlg::execute (ObjectList& objects)
{
  GEOM::GEOM_IHealingOperations_var anOper = GEOM::GEOM_IHealingOperations::_narrow(getOperation());

  GEOM::ListOfGO_var points = new GEOM::ListOfGO();
  points->length(myPoints.count());
  for (int i = 0; i < myPoints.count(); i++)
    points[i] = myPoints[i].copy();

  GEOM::GEOM_Object_var anObj = anOper->FuseCollinearEdgesWithinWire(myShape, points.in());

  if (!anObj->_is_nil())
    objects.push_back(anObj._retn());

  return true;
}

//=================================================================================
// function : addSubshapeToStudy
// purpose  : virtual method to add new SubObjects if local selection
//=================================================================================
void RepairGUI_FuseEdgesDlg::addSubshapesToStudy()
{
  for (int i = 0; i < myPoints.count(); i++)
    GEOMBase::PublishSubObject(myPoints[i].get());
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> RepairGUI_FuseEdgesDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res(myPoints);
  GEOM::GeomObjPtr aGeomObjPtr(myShape);
  res << aGeomObjPtr;
  return res;
}
