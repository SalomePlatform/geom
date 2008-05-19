//  GEOM GEOMGUI : GUI for Geometry component
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : MeasureGUI_NormaleDlg.cxx
//  Author : Julia DOROVSKIKH
//  Module : GEOM
//  $Header$

#include "MeasureGUI_NormaleDlg.h"

#include "SUIT_Session.h"
#include "SalomeApp_Application.h"
#include "LightApp_SelectionMgr.h"

#include <TopoDS_Shape.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <qlabel.h>

#include "GEOMImpl_Types.hxx"

#include "utilities.h"

//=================================================================================
// class    : MeasureGUI_NormaleDlg()
// purpose  : Constructs a MeasureGUI_NormaleDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
MeasureGUI_NormaleDlg::MeasureGUI_NormaleDlg (GeometryGUI* theGeometryGUI, QWidget* parent,
                                              const char* name, bool modal, WFlags fl)
  :GEOMBase_Skeleton(theGeometryGUI, parent, name, modal, WStyle_Customize |
                     WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
  QPixmap image0 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_NORMALE")));
  QPixmap image1 (aResMgr->loadPixmap("GEOM", tr("ICON_SELECT")));

  setCaption(tr("GEOM_NORMALE_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_NORMALE"));
  RadioButton1->setPixmap(image0);
  RadioButton2->close(TRUE);
  RadioButton3->close(TRUE);

  GroupArgs = new DlgRef_2Sel_QTD (this, "GroupArgs");
  GroupArgs->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));

  GroupArgs->TextLabel1->setText(tr("GEOM_FACE"));
  GroupArgs->TextLabel2->setText(tr("GEOM_POINT"));

  GroupArgs->PushButton1->setPixmap(image1);
  GroupArgs->PushButton2->setPixmap(image1);

  Layout1->addWidget(GroupArgs, 2, 0);
  /***************************************************************/

  setHelpFileName("using_measurement_tools_page.html#normale_anchor");

  Init();
}

//=================================================================================
// function : ~MeasureGUI_NormaleDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
MeasureGUI_NormaleDlg::~MeasureGUI_NormaleDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void MeasureGUI_NormaleDlg::Init()
{
  /* init variables */
  GroupArgs->LineEdit1->setReadOnly(true);
  GroupArgs->LineEdit2->setReadOnly(true);

  myFace = GEOM::GEOM_Object::_nil();
  myPoint = GEOM::GEOM_Object::_nil();

  myEditCurrentArgument = GroupArgs->LineEdit1;
  globalSelection(GEOM_FACE);

  /* signals and slots connections */
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(GroupArgs->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupArgs->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(GroupArgs->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupArgs->LineEdit2, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(myGeomGUI->getApp()->selectionMgr(),
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  initName(tr("GEOM_VECTOR_NORMALE"));

  //ConstructorsClicked(0);
  SelectionIntoArgument();

  /* displays Dialog */
  GroupArgs->show();
  this->show();
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
  return true;
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
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

  if (IObjectCount() != 1)
    return;

  // nbSel == 1
  Standard_Boolean testResult = Standard_False;
  GEOM::GEOM_Object_var aSelectedObject =
    GEOMBase::ConvertIOinGEOMObject(firstIObject(), testResult);

  if (!testResult)
    return;

  QString aName = GEOMBase::GetName(aSelectedObject);

  if (myEditCurrentArgument == GroupArgs->LineEdit1) {
    TopoDS_Shape aShape;
    if (GEOMBase::GetShape(aSelectedObject, aShape, TopAbs_SHAPE) && !aShape.IsNull())
    {
      LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
      TColStd_IndexedMapOfInteger aMap;
      aSelMgr->GetIndexes(firstIObject(), aMap);
      if (aMap.Extent() == 1) // Local Selection
      {
        GEOM::GEOM_IShapesOperations_var aShapesOp = getGeomEngine()->GetIShapesOperations(getStudyId());
        int anIndex = aMap( 1 );
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
  }
  else if (myEditCurrentArgument == GroupArgs->LineEdit2) {
    TopoDS_Shape aShape;
    if (GEOMBase::GetShape(aSelectedObject, aShape, TopAbs_SHAPE) && !aShape.IsNull())
    {
      LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
      TColStd_IndexedMapOfInteger aMap;
      aSelMgr->GetIndexes(firstIObject(), aMap);
      if (aMap.Extent() == 1) // Local Selection
      {
        GEOM::GEOM_IShapesOperations_var aShapesOp = getGeomEngine()->GetIShapesOperations(getStudyId());
        int anIndex = aMap( 1 );
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
  }

  myEditCurrentArgument->setText( aName );

  displayPreview();
}

//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void MeasureGUI_NormaleDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if (send == GroupArgs->LineEdit1 ||
      send == GroupArgs->LineEdit2)
  {
    myEditCurrentArgument = send;
    GEOMBase_Skeleton::LineEditReturnPressed();
  }
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void MeasureGUI_NormaleDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();
  globalSelection(GEOM_FACE);

  if (send == GroupArgs->PushButton1) {
    myEditCurrentArgument = GroupArgs->LineEdit1;
    localSelection( GEOM::GEOM_Object::_nil(), TopAbs_FACE );
  }
  else if (send == GroupArgs->PushButton2) {
    myEditCurrentArgument = GroupArgs->LineEdit2;
    localSelection( GEOM::GEOM_Object::_nil(), TopAbs_VERTEX );
  }

  myEditCurrentArgument->setFocus();
  SelectionIntoArgument();
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void MeasureGUI_NormaleDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();

  SelectionIntoArgument();
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void MeasureGUI_NormaleDlg::enterEvent (QEvent* e)
{
  if (!GroupConstructors->isEnabled())
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
  GEOM::GEOM_Object_var anObj =
    GEOM::GEOM_IMeasureOperations::_narrow(getOperation())->GetNormal(myFace, myPoint);

  if (!anObj->_is_nil())
    objects.push_back(anObj._retn());

  return true;
}
