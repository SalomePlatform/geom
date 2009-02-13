//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
// GEOM GEOMGUI : GUI for Geometry component
// File   : GenerationGUI_PipeDlg.cxx
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.
//
#include "GenerationGUI_PipeDlg.h"

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
// class    : GenerationGUI_PipeDlg()
// purpose  : Constructs a GenerationGUI_PipeDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
GenerationGUI_PipeDlg::GenerationGUI_PipeDlg (GeometryGUI* theGeometryGUI, QWidget* parent,
                                              bool modal, Qt::WindowFlags fl)
  : GEOMBase_Skeleton(theGeometryGUI, parent, modal, fl)
{
  QPixmap image0 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_PIPE")));
  QPixmap image1 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_SELECT")));
  QPixmap image2 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_PIPE_BINORMAL")));

  setWindowTitle(tr("GEOM_PIPE_TITLE"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_PIPE"));
  mainFrame()->RadioButton1->setIcon(image0);
  mainFrame()->RadioButton2->setIcon(image2);
  mainFrame()->RadioButton3->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton3->close();

  GroupPoints = new DlgRef_3Sel1Check(centralWidget());

  GroupPoints->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  GroupPoints->TextLabel1->setText(tr("GEOM_BASE_OBJECT"));
  GroupPoints->TextLabel2->setText(tr("GEOM_PATH_OBJECT"));
  GroupPoints->TextLabel3->setText(tr("GEOM_BINORMAL"));
  GroupPoints->PushButton1->setIcon(image1);
  GroupPoints->PushButton2->setIcon(image1);
  GroupPoints->PushButton3->setIcon(image1);
  GroupPoints->CheckButton1->setText(tr("GEOM_SELECT_UNPUBLISHED_EDGES"));

  QVBoxLayout* layout = new QVBoxLayout(centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
  layout->addWidget(GroupPoints);
  /***************************************************************/

  setHelpFileName("create_extrusion_alongpath_page.html");

  // Initialisation
  Init();
}

//=================================================================================
// function : ~GenerationGUI_PipeDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
GenerationGUI_PipeDlg::~GenerationGUI_PipeDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void GenerationGUI_PipeDlg::Init()
{
  // init variables
  GroupPoints->LineEdit1->setReadOnly(true);
  GroupPoints->LineEdit2->setReadOnly(true);
  GroupPoints->LineEdit3->setReadOnly(true);

  GroupPoints->LineEdit1->setText("");
  GroupPoints->LineEdit2->setText("");
  GroupPoints->LineEdit3->setText("");
  myBase = myPath = myVec = GEOM::GEOM_Object::_nil();
  myOkBase = myOkPath = myOkVec = false;

  GroupPoints->CheckButton1->setEnabled(false);

  // signals and slots connections
  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(this,          SIGNAL(constructorsClicked(int)), this, SLOT(ConstructorsClicked(int)));

  connect(GroupPoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->PushButton3, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(GroupPoints->LineEdit1,   SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupPoints->LineEdit2,   SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupPoints->LineEdit3,   SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(GroupPoints->CheckButton1,   SIGNAL(toggled(bool)), this, SLOT(SelectionTypeButtonClicked()));

  initName(tr("GEOM_PIPE"));

  GroupPoints->TextLabel3->hide();
  GroupPoints->PushButton3->hide();
  GroupPoints->LineEdit3->hide();
  ConstructorsClicked(0);

  GroupPoints->PushButton1->click();
  SelectionIntoArgument();
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void GenerationGUI_PipeDlg::ConstructorsClicked( int constructorId )
{
  erasePreview();

  switch (constructorId)
  {
    case 0:
    {
      GroupPoints->TextLabel3->hide();
      GroupPoints->PushButton3->hide();
      GroupPoints->LineEdit3->hide();
      break;
    }
    case 1:
    {
      GroupPoints->TextLabel3->show();
      GroupPoints->PushButton3->show();
      GroupPoints->LineEdit3->show();
      break;
    }
  }

  displayPreview();
}

//=================================================================================
// function : SelectionBittonClicked()
// purpose  : Selection type Radio button management
//=================================================================================
void GenerationGUI_PipeDlg::SelectionTypeButtonClicked()
{
  globalSelection();
  if ( GroupPoints->CheckButton1->isChecked() ) {
    localSelection( GEOM::GEOM_Object::_nil(), TopAbs_EDGE );
  } else {
    TColStd_MapOfInteger aMap;
    aMap.Add(GEOM_WIRE);
    aMap.Add(GEOM_LINE);
    globalSelection(aMap);
  }
  if(myEditCurrentArgument == GroupPoints->LineEdit2) {
    myEditCurrentArgument->setText("");
    myOkPath = false;
  }
  displayPreview();
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void GenerationGUI_PipeDlg::ClickOnOk()
{
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool GenerationGUI_PipeDlg::ClickOnApply()
{
  if (!onAccept())
    return false;

  initName();
  if ( getConstructorId() != 1 )
  	ConstructorsClicked( getConstructorId() );
  // activate selection and connect selection manager
  GroupPoints->PushButton1->click();
  return true;
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection is changed or on dialog initialization or activation
//=================================================================================
void GenerationGUI_PipeDlg::SelectionIntoArgument()
{
  erasePreview();
  myEditCurrentArgument->setText("");
  if      (myEditCurrentArgument == GroupPoints->LineEdit1) myOkBase = false;
  else if (myEditCurrentArgument == GroupPoints->LineEdit2) myOkPath = false;
  else if (myEditCurrentArgument == GroupPoints->LineEdit3) myOkVec  = false;

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  if (aSelList.Extent() != 1)
    return;

  // nbSel == 1
  Standard_Boolean testResult = Standard_False;
  GEOM::GEOM_Object_var aSelectedObject = GEOMBase::ConvertIOinGEOMObject(aSelList.First(), testResult);

  if (!testResult || aSelectedObject->_is_nil())
    return;

  TopoDS_Shape S;
  if (!GEOMBase::GetShape(aSelectedObject, S))
    return;

  if (myEditCurrentArgument == GroupPoints->LineEdit1) {
    if (S.ShapeType() == TopAbs_COMPSOLID ||
        S.ShapeType() == TopAbs_COMPOUND ||
        S.ShapeType() == TopAbs_SOLID ||
        S.ShapeType() == TopAbs_SHAPE)
      return;
    if ( getConstructorId() == 1 && 
	 (S.ShapeType() == TopAbs_SHELL || 
	  S.ShapeType() == TopAbs_VERTEX))
      return;

    myBase = aSelectedObject;
    myEditCurrentArgument->setText(GEOMBase::GetName(aSelectedObject));
    myOkBase = true;
    if (!myOkPath)
      GroupPoints->PushButton2->click();
    else if (!myOkVec)
      GroupPoints->PushButton3->click();
  }
  else if (myEditCurrentArgument == GroupPoints->LineEdit2 || 
	   myEditCurrentArgument == GroupPoints->LineEdit3) {
    myEditCurrentArgument == GroupPoints->LineEdit2 ? myOkPath = false : myOkVec = false;
    bool myOk = false;
    QString aName = GEOMBase::GetName(aSelectedObject);

    if (myEditCurrentArgument == GroupPoints->LineEdit2 && !GroupPoints->CheckButton1->isChecked() ){
      myPath = aSelectedObject;
      myOkPath = true;
      myEditCurrentArgument->setText(aName);
    }
    else 
      { 
	if (aSelectedObject != myBase) {
	  TColStd_IndexedMapOfInteger aMap;
	  aSelMgr->GetIndexes(aSelList.First(), aMap);
	  if (aMap.Extent() == 1) {
	    int anIndex = aMap(1);
	    aName.append(":edge_" + QString::number(anIndex));
	    
	    //Find SubShape Object in Father
	    GEOM::GEOM_Object_var aFindedObject = GEOMBase_Helper::findObjectInFather(aSelectedObject, aName);
	    
	    if (aFindedObject == GEOM::GEOM_Object::_nil()) { // Object not found in study
	      GEOM::GEOM_IShapesOperations_var aShapesOp =
		getGeomEngine()->GetIShapesOperations(getStudyId());
	      aSelectedObject = aShapesOp->GetSubShape(aSelectedObject, anIndex);
	      myOk = true;
	    }
	    else { // get Object from study
	      aSelectedObject = aFindedObject;
	      myOk = true;
	    }
	  }
	  else {
	    myOk = true;
	    if (S.ShapeType() != TopAbs_EDGE) {
	      aSelectedObject = GEOM::GEOM_Object::_nil();
	      aName = "";
	      myOk = false;
	    }
	  }
	  if (myEditCurrentArgument == GroupPoints->LineEdit2) {
	    myPath = aSelectedObject;
	    myOkPath = myOk;
	  }
	  else if (myEditCurrentArgument == GroupPoints->LineEdit3) {
	    myVec = aSelectedObject;
	    myOkVec = myOk;
	  }
	}
	myEditCurrentArgument->setText(aName);

	if (myOkPath) {
	  if (!myOkBase)
	    GroupPoints->PushButton1->click();
	  else if (!myOkVec)
	    GroupPoints->PushButton3->click();
	}
	else if (myOkVec) {
	  if (!myOkBase)
	    GroupPoints->PushButton1->click();
	  else if (!myOkPath)
	    GroupPoints->PushButton2->click();
	}
      }
  }

  // clear selection
  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
  myGeomGUI->getApp()->selectionMgr()->clearSelected();
  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));

  displayPreview();
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void GenerationGUI_PipeDlg::SetEditCurrentArgument()
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

    if ( GroupPoints->CheckButton1->isChecked() ) {
      localSelection( GEOM::GEOM_Object::_nil(), TopAbs_EDGE );
    } else {
      TColStd_MapOfInteger aMap;
      aMap.Add(GEOM_WIRE);
      aMap.Add(GEOM_LINE);
      globalSelection(aMap);
    }
    GroupPoints->CheckButton1->setEnabled(true);
  }
  else if(send == GroupPoints->PushButton3) {
    myEditCurrentArgument = GroupPoints->LineEdit3;
    GroupPoints->CheckButton1->setEnabled(false);
    localSelection(GEOM::GEOM_Object::_nil(), TopAbs_EDGE);
  }
  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));

  // enable line edit
  myEditCurrentArgument->setEnabled(true);
  myEditCurrentArgument->setFocus();
  // after setFocus(), because it will be setDown(false) when loses focus
  send->setDown(true);

  // seems we need it only to avoid preview disappearing, caused by selection mode change
  displayPreview();
}

//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void GenerationGUI_PipeDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if (send == GroupPoints->LineEdit1 ||
      send == GroupPoints->LineEdit2 ||
      send == GroupPoints->LineEdit3) {
    myEditCurrentArgument = send;
    GEOMBase_Skeleton::LineEditReturnPressed();
  }

  if (send == GroupPoints->LineEdit2)
    GroupPoints->CheckButton1->setEnabled(true);
  else
    GroupPoints->CheckButton1->setEnabled(false);
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void GenerationGUI_PipeDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
	   this, SLOT( SelectionIntoArgument() ) );

  ConstructorsClicked(getConstructorId());
}

//=================================================================================
// function : enterEvent()
// purpose  : when mouse enter onto the QWidget
//=================================================================================
void GenerationGUI_PipeDlg::enterEvent (QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr GenerationGUI_PipeDlg::createOperation()
{
  return getGeomEngine()->GetI3DPrimOperations(getStudyId());
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool GenerationGUI_PipeDlg::isValid (QString&)
{
  switch ( getConstructorId() ) {
  case 0 :
    return myOkBase && myOkPath;
  case 1 :
    return myOkBase && myOkPath && myOkVec;
  }
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool GenerationGUI_PipeDlg::execute (ObjectList& objects)
{
  GEOM::GEOM_Object_var anObj;

  switch ( getConstructorId() ) {
  case 0 :
    anObj = GEOM::GEOM_I3DPrimOperations::_narrow(getOperation())->MakePipe(myBase, myPath);
    break;
  case 1 :
    anObj = GEOM::GEOM_I3DPrimOperations::_narrow(getOperation())->
      MakePipeBiNormalAlongVector(myBase, myPath, myVec);
    break;
  }

  if (!anObj->_is_nil())
    objects.push_back(anObj._retn());

  return true;
}

//=================================================================================
// function : addSubshapeToStudy
// purpose  : virtual method to add new SubObjects if local selection
//=================================================================================
void GenerationGUI_PipeDlg::addSubshapesToStudy()
{
  QMap<QString, GEOM::GEOM_Object_var> objMap;

  objMap[GroupPoints->LineEdit2->text()] = myPath;

  addSubshapesToFather(objMap);
}
