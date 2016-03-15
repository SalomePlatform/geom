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
// File   : GenerationGUI_PipePathDlg.cxx

#include "GenerationGUI_PipePathDlg.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>

#include <TopoDS_Shape.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <TColStd_MapOfInteger.hxx>

#include <GEOMImpl_Types.hxx>

//=================================================================================
// class    : GenerationGUI_PipePathDlg()
// purpose  : Constructs a GenerationGUI_PipePathDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
GenerationGUI_PipePathDlg::GenerationGUI_PipePathDlg (GeometryGUI* theGeometryGUI, QWidget* parent,
                                                      bool modal, Qt::WindowFlags fl)
  : GEOMBase_Skeleton(theGeometryGUI, parent, modal, fl)
{
  QPixmap image0 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_PIPE_PATH")));
  QPixmap image1 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_SELECT")));

  setWindowTitle(tr("GEOM_PIPE_PATH_TITLE"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_PIPE_PATH"));
  mainFrame()->RadioButton1->setIcon(image0);
  mainFrame()->RadioButton2->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton3->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->close();

  GroupPoints = new DlgRef_3Sel1Check(centralWidget());

  GroupPoints->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  GroupPoints->TextLabel1->setText(tr("GEOM_PIPE_LIKE_SHAPE"));
  GroupPoints->TextLabel2->setText(tr("GEOM_PIPE_BASE1_OBJECT"));
  GroupPoints->TextLabel3->setText(tr("GEOM_PIPE_BASE2_OBJECT"));
  GroupPoints->PushButton1->setIcon(image1);
  GroupPoints->PushButton2->setIcon(image1);
  GroupPoints->PushButton3->setIcon(image1);
  GroupPoints->CheckButton1->setText(tr("GEOM_SELECT_UNPUBLISHED_EDGES"));

  QVBoxLayout* layout = new QVBoxLayout (centralWidget());
  layout->setMargin(0);
  layout->setSpacing(6);
  layout->addWidget(GroupPoints);
  /***************************************************************/

  setHelpFileName("create_pipe_path_page.html");

  // Initialisation
  Init();
}

//=================================================================================
// function : ~GenerationGUI_PipePathDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
GenerationGUI_PipePathDlg::~GenerationGUI_PipePathDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void GenerationGUI_PipePathDlg::Init()
{
  // init variables
  GroupPoints->LineEdit1->setReadOnly(true);
  GroupPoints->LineEdit2->setReadOnly(true);
  GroupPoints->LineEdit3->setReadOnly(true);

  GroupPoints->LineEdit1->setText("");
  GroupPoints->LineEdit2->setText("");
  GroupPoints->LineEdit3->setText("");

  myShape.nullify();

  GroupPoints->CheckButton1->setEnabled(false);

  showOnlyPreviewControl();

  // signals and slots connections
  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(GroupPoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->PushButton3, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(GroupPoints->CheckButton1,   SIGNAL(toggled(bool)), this, SLOT(SelectionTypeButtonClicked()));

  initName(tr("GEOM_PIPE_PATH"));

  updateGeometry();
  resize(100,100);

  GroupPoints->PushButton1->click();
  SelectionIntoArgument();
}

//=================================================================================
// function : SelectionBittonClicked()
// purpose  : Selection type Radio button management
//=================================================================================
void GenerationGUI_PipePathDlg::SelectionTypeButtonClicked()
{
  globalSelection();
  if (GroupPoints->CheckButton1->isChecked()) {
    localSelection(TopAbs_EDGE);
  }
  else {
    TColStd_MapOfInteger aMap;
    aMap.Add(GEOM_FACE);
    aMap.Add(GEOM_WIRE);
    aMap.Add(GEOM_EDGE);
    globalSelection(aMap);
  }
  processPreview();
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void GenerationGUI_PipePathDlg::ClickOnOk()
{
  setIsApplyAndClose(true);
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool GenerationGUI_PipePathDlg::ClickOnApply()
{
  if (!onAccept())
    return false;

  initName();
  // activate selection and connect selection manager
  GroupPoints->PushButton1->click();
  return true;
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection is changed or on dialog initialization or activation
//=================================================================================
void GenerationGUI_PipePathDlg::SelectionIntoArgument()
{
  erasePreview();
  myEditCurrentArgument->setText("");

  if (myEditCurrentArgument == GroupPoints->LineEdit1) {
    QList<TopAbs_ShapeEnum> types;
    types << TopAbs_SOLID << TopAbs_SHELL << TopAbs_FACE;
    myShape = getSelected(types);
    if (myShape) {
      QString aName = GEOMBase::GetName(myShape.get());
      myEditCurrentArgument->setText(aName);
      if (myBase1Objects.isEmpty())
        GroupPoints->PushButton2->click();
      else if (myBase2Objects.isEmpty())
        GroupPoints->PushButton3->click();
    }
  }
  else if (myEditCurrentArgument == GroupPoints->LineEdit2) {
    myBase1Objects.clear();
    QList<GEOM::GeomObjPtr> objects = getSelected(TopAbs_SHAPE, -1);
    for (int i = 0; i < objects.count(); i++) {
      GEOM::shape_type stype = objects[i]->GetMaxShapeType();
     if (GEOM::FACE <= stype && stype <= GEOM::EDGE)
        myBase1Objects << objects[i];
    }
    int nbObj = myBase1Objects.count();
    if (nbObj) {
      QString aName =
        nbObj > 1 ? QString("%1_objects").arg(nbObj) : GEOMBase::GetName(myBase1Objects[0].get());
      myEditCurrentArgument->setText(aName);
    }
  }
  else if (myEditCurrentArgument == GroupPoints->LineEdit3) {
    myBase2Objects.clear();
    QList<GEOM::GeomObjPtr> objects = getSelected(TopAbs_SHAPE, -1);
    for (int i = 0; i < objects.count(); i++) {
      GEOM::shape_type stype = objects[i]->GetMaxShapeType();
      if (GEOM::FACE <= stype && stype <= GEOM::EDGE)
        myBase2Objects << objects[i];
    }
    int nbObj = myBase2Objects.count();
    if (nbObj) {
      QString aName =
        nbObj > 1 ? QString("%1_objects").arg(nbObj) : GEOMBase::GetName(myBase2Objects[0].get());
      myEditCurrentArgument->setText(aName);
    }
  }

  processPreview();
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void GenerationGUI_PipePathDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);

  globalSelection(GEOM_ALLSHAPES);

  GroupPoints->PushButton1->setDown(false);
  GroupPoints->PushButton2->setDown(false);
  GroupPoints->PushButton3->setDown(false);
  GroupPoints->LineEdit1->setEnabled(false);
  GroupPoints->LineEdit2->setEnabled(false);
  GroupPoints->LineEdit3->setEnabled(false);

  if (send == GroupPoints->PushButton1) {
    myEditCurrentArgument = GroupPoints->LineEdit1;
    GroupPoints->CheckButton1->setEnabled(false);
  }
  else if (send == GroupPoints->PushButton2) {
    myEditCurrentArgument = GroupPoints->LineEdit2;

    if (GroupPoints->CheckButton1->isChecked()) {
      localSelection(TopAbs_EDGE);
    }
    else {
      TColStd_MapOfInteger aMap;
      aMap.Add(GEOM_FACE);
      aMap.Add(GEOM_WIRE);
      aMap.Add(GEOM_EDGE);
      globalSelection(aMap);
    }
    GroupPoints->CheckButton1->setEnabled(true);
  }
  else if (send == GroupPoints->PushButton3) {
    myEditCurrentArgument = GroupPoints->LineEdit3;

    if (GroupPoints->CheckButton1->isChecked()) {
      localSelection(TopAbs_EDGE);
    }
    else {
      TColStd_MapOfInteger aMap;
      aMap.Add(GEOM_FACE);
      aMap.Add(GEOM_WIRE);
      aMap.Add(GEOM_EDGE);
      globalSelection(aMap);
    }
    GroupPoints->CheckButton1->setEnabled(true);
  }
  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));

  // enable line edit
  myEditCurrentArgument->setEnabled(true);
  myEditCurrentArgument->setFocus();
  // after setFocus(), because it will be setDown(false) when loses focus
  send->setDown(true);

  // seems we need it only to avoid preview disappearing, caused by selection mode change
  processPreview();
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void GenerationGUI_PipePathDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();

  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));

  GroupPoints->PushButton1->click();
  SelectionIntoArgument();
}

//=================================================================================
// function : enterEvent()
// purpose  : when mouse enter onto the QWidget
//=================================================================================
void GenerationGUI_PipePathDlg::enterEvent (QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr GenerationGUI_PipePathDlg::createOperation()
{
  return getGeomEngine()->GetI3DPrimOperations(getStudyId());
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool GenerationGUI_PipePathDlg::isValid (QString&)
{
  bool ok = myShape && !myBase1Objects.isEmpty() && !myBase2Objects.isEmpty();
  return ok;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool GenerationGUI_PipePathDlg::execute (ObjectList& objects)
{
  GEOM::ListOfGO_var aBase1 = new GEOM::ListOfGO();
  GEOM::ListOfGO_var aBase2 = new GEOM::ListOfGO();

  aBase1->length(myBase1Objects.count());
  aBase2->length(myBase2Objects.count());

  int i;
  for (i = 0; i < myBase1Objects.count(); i++)
    aBase1[i] = myBase1Objects[i].copy();
  for (i = 0; i < myBase2Objects.count(); i++)
    aBase2[i] = myBase2Objects[i].copy();

  GEOM::GEOM_I3DPrimOperations_var anOper = GEOM::GEOM_I3DPrimOperations::_narrow(getOperation());
  GEOM::GEOM_Object_var anObj = anOper->RestorePathEdges(myShape.get(), aBase1.in(), aBase2.in());
    
  if (!anObj->_is_nil())
    objects.push_back(anObj._retn());

  return true;
}

//=================================================================================
// function : addSubshapeToStudy
// purpose  : virtual method to add new SubObjects if local selection
//=================================================================================
void GenerationGUI_PipePathDlg::addSubshapesToStudy()
{
  int i;
  for (i = 0; i < myBase1Objects.count(); i++)
    GEOMBase::PublishSubObject(myBase1Objects[i].get());
  for (i = 0; i < myBase2Objects.count(); i++)
    GEOMBase::PublishSubObject(myBase2Objects[i].get());
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> GenerationGUI_PipePathDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res(myBase1Objects);
  res << myShape ;
  for (int i = 0; i < myBase2Objects.count(); i++)
    res << myBase2Objects[i];
  return res;
}
