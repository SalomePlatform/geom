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
// File   : TransformationGUI_MirrorDlg.cxx
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.
//
#include "TransformationGUI_MirrorDlg.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>

// OCCT Includes
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#include <GEOMImpl_Types.hxx>

//=================================================================================
// class    : TransformationGUI_MirrorDlg()
// purpose  : Constructs a TransformationGUI_MirrorDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
TransformationGUI_MirrorDlg::TransformationGUI_MirrorDlg (GeometryGUI* theGeometryGUI, QWidget* parent,
                                                          bool modal, Qt::WindowFlags fl)
  : GEOMBase_Skeleton(theGeometryGUI, parent, modal, fl),
    myInitial(true)
{
  SUIT_ResourceMgr* aResMgr = myGeomGUI->getApp()->resourceMgr();
  QPixmap image0 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_MIRROR_POINT")));
  QPixmap image1 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_MIRROR_AXE")));
  QPixmap image2 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_MIRROR_PLANE")));
  QPixmap image3 (aResMgr->loadPixmap("GEOM", tr("ICON_SELECT")));

  setWindowTitle(tr("GEOM_MIRROR_TITLE"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_MIRROR"));
  mainFrame()->RadioButton1->setIcon(image0);
  mainFrame()->RadioButton2->setIcon(image1);
  mainFrame()->RadioButton3->setIcon(image2);

  GroupPoints = new DlgRef_2Sel1Spin2Check(centralWidget());
  GroupPoints->SpinBox_DX->hide();
  GroupPoints->TextLabel3->hide();
  GroupPoints->CheckButton2->hide();
  GroupPoints->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  GroupPoints->TextLabel1->setText(tr("GEOM_OBJECTS"));
  GroupPoints->TextLabel2->setText(tr("GEOM_POINT_MIRROR"));
  GroupPoints->TextLabel2->setFixedWidth(74);
  GroupPoints->PushButton1->setIcon(image3);
  GroupPoints->PushButton2->setIcon(image3);
  GroupPoints->CheckButton1->setText(tr("GEOM_CREATE_COPY"));

  QVBoxLayout* layout = new QVBoxLayout(centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
  layout->addWidget(GroupPoints);
  /***************************************************************/

  setHelpFileName("mirror_operation_page.html");

  // Activate Create a Copy mode
  GroupPoints->CheckButton1->setChecked(true);
  CreateCopyModeChanged();

  Init();
}

//=================================================================================
// function : ~TransformationGUI_MirrorDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
TransformationGUI_MirrorDlg::~TransformationGUI_MirrorDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void TransformationGUI_MirrorDlg::Init()
{
  // init variables
  GroupPoints->LineEdit1->setReadOnly(true);
  GroupPoints->LineEdit2->setReadOnly(true);

  GroupPoints->LineEdit1->setText("");
  GroupPoints->LineEdit2->setText("");

  myObjects.clear();
  myArgument.nullify();

  mainFrame()->GroupBoxPublish->show();

  // signals and slots connections
  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(this, SIGNAL(constructorsClicked(int)), this, SLOT(ConstructorsClicked(int)));

  connect(GroupPoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(GroupPoints->CheckButton1, SIGNAL(toggled(bool)), this, SLOT(CreateCopyModeChanged()));

  initName(tr("GEOM_MIRROR"));

  ConstructorsClicked(0);
  resize(minimumSizeHint());
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void TransformationGUI_MirrorDlg::ConstructorsClicked (int constructorId)
{
  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);

  GroupPoints->LineEdit2->clear();
  myArgument.nullify();

  switch (constructorId) {
  case 0: // mirror an object by point
    GroupPoints->TextLabel2->setText(tr("GEOM_POINT_MIRROR"));
    break;
  case 1: // mirror an object by axe
    GroupPoints->TextLabel2->setText(tr("GEOM_AXE_MIRROR"));
    break;
  case 2: // mirror an object by plane
    GroupPoints->TextLabel2->setText(tr("GEOM_PLANE_MIRROR"));
    break;
  }

  GroupPoints->PushButton1->click();

  if (myInitial) {
    myInitial = false;
    SelectionIntoArgument();
  }
  else {
    processPreview();
  }
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void TransformationGUI_MirrorDlg::ClickOnOk()
{
  setIsApplyAndClose( true );
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool TransformationGUI_MirrorDlg::ClickOnApply()
{
  if (!onAccept(GroupPoints->CheckButton1->isChecked()))
    return false;

  initName();

  // activate selection and connect selection manager
  ConstructorsClicked(getConstructorId());
  SelectionIntoArgument();

  return true;
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection is changed or on dialog initialization or activation
//=================================================================================
void TransformationGUI_MirrorDlg::SelectionIntoArgument()
{
  erasePreview();

  if ( myEditCurrentArgument == GroupPoints->LineEdit1 ) {
    myObjects = getSelected( TopAbs_SHAPE, -1 );
    if ( !myObjects.isEmpty() ) {
      QString aName = myObjects.count() > 1 ? QString( "%1_objects").arg( myObjects.count() ) : GEOMBase::GetName( myObjects[0].get() );
      myEditCurrentArgument->setText( aName );
    }
    else {
      myEditCurrentArgument->setText("");
    }
  }
  else {
    TopAbs_ShapeEnum aNeedType = ( getConstructorId() == 1 ) ? 
      TopAbs_EDGE : ( ( getConstructorId() == 2 ) ? TopAbs_FACE : TopAbs_VERTEX );
    GEOM::GeomObjPtr aSelectedObject = getSelected( aNeedType );
    TopoDS_Shape aShape;
    if ( aSelectedObject && GEOMBase::GetShape( aSelectedObject.get(), aShape ) && !aShape.IsNull() ) {
      QString aName = GEOMBase::GetName( aSelectedObject.get() );
      myEditCurrentArgument->setText( aName );
      myArgument = aSelectedObject;
      if ( myObjects.isEmpty() )
        GroupPoints->PushButton1->click();
    }
    else {
      if ( myEditCurrentArgument == GroupPoints->LineEdit2 )
        myArgument.nullify();
      myEditCurrentArgument->setText("");
    }
  }

  processPreview();
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void TransformationGUI_MirrorDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
  globalSelection();

  if (send == GroupPoints->PushButton1) {
    myEditCurrentArgument = GroupPoints->LineEdit1;

    GroupPoints->PushButton2->setDown(false);
    GroupPoints->LineEdit2->setEnabled(false);
  }
  else if (send == GroupPoints->PushButton2) {
    myEditCurrentArgument = GroupPoints->LineEdit2;

    switch (getConstructorId()) {
    case 0:
      localSelection(TopAbs_VERTEX);
      break;
    case 1:
      localSelection(TopAbs_EDGE);
      break;
    case 2:
      globalSelection(GEOM_PLANE);
      localSelection(TopAbs_FACE);
      break;
    }

    GroupPoints->PushButton1->setDown(false);
    GroupPoints->LineEdit1->setEnabled(false);
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
void TransformationGUI_MirrorDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
           this, SLOT( SelectionIntoArgument() ) );

  ConstructorsClicked( getConstructorId() );
}

//=================================================================================
// function : enterEvent()
// purpose  : when mouse enter onto the QWidget
//=================================================================================
void TransformationGUI_MirrorDlg::enterEvent (QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr  TransformationGUI_MirrorDlg::createOperation()
{
  return getGeomEngine()->GetITransformOperations(getStudyId());
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool TransformationGUI_MirrorDlg::isValid (QString& /*msg*/)
{
  return !myObjects.isEmpty() && myArgument;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool  TransformationGUI_MirrorDlg::execute (ObjectList& objects)
{
  bool res = false;
  bool toCreateCopy = IsPreview() || GroupPoints->CheckButton1->isChecked();

  GEOM::GEOM_Object_var anObj;

  GEOM::GEOM_ITransformOperations_var anOper = GEOM::GEOM_ITransformOperations::_narrow(getOperation());

  switch (getConstructorId()) {
  case 0:
    {
      if (toCreateCopy) {
        for (int i = 0; i < myObjects.count(); i++) {
          anObj = anOper->MirrorPointCopy(myObjects[i].get(), myArgument.get());
          if (!anObj->_is_nil())
            objects.push_back(anObj._retn());
        }
      }
      else {
        for (int i = 0; i < myObjects.count(); i++) {
          anObj = anOper->MirrorPoint(myObjects[i].get(), myArgument.get());
          if (!anObj->_is_nil())
            objects.push_back(anObj._retn());
        }
      }
      res = true;
      break;
    }
  case 1:
    {
      if (toCreateCopy) {
        for (int i = 0; i < myObjects.count(); i++) {
          anObj = anOper->MirrorAxisCopy(myObjects[i].get(), myArgument.get());
          if (!anObj->_is_nil())
            objects.push_back(anObj._retn());
        }
      }
      else {
        for (int i = 0; i < myObjects.count(); i++) {
          anObj = anOper->MirrorAxis(myObjects[i].get(), myArgument.get());
          if (!anObj->_is_nil())
            objects.push_back(anObj._retn());
        }
      }
      res = true;
      break;
    }
  case 2:
    {
      if (toCreateCopy) {
        for (int i = 0; i < myObjects.count(); i++) {
            anObj = anOper->MirrorPlaneCopy(myObjects[i].get(), myArgument.get());
            if (!anObj->_is_nil())
              objects.push_back(anObj._retn());
        }
      }
      else {
        for (int i = 0; i < myObjects.count(); i++) {
          anObj = anOper->MirrorPlane(myObjects[i].get(), myArgument.get());
          if (!anObj->_is_nil())
            objects.push_back(anObj._retn());
        }
      }
      res = true;
      break;
    }
  }

  return res;
}

//=================================================================================
// function : restoreSubShapes
// purpose  :
//=================================================================================
void TransformationGUI_MirrorDlg::restoreSubShapes (SALOMEDS::Study_ptr   theStudy,
                                                    SALOMEDS::SObject_ptr theSObject)
{
  if (mainFrame()->CheckBoxRestoreSS->isChecked()) {
    // empty list of arguments means that all arguments should be restored
    getGeomEngine()->RestoreSubShapesSO(theStudy, theSObject, GEOM::ListOfGO(),
                                        /*theFindMethod=*/GEOM::FSM_Transformed,
                                        /*theInheritFirstArg=*/true,
                                        mainFrame()->CheckBoxAddPrefix->isChecked());
  }
}

//=================================================================================
// function :  CreateCopyModeChanged()
// purpose  :
//=================================================================================
void TransformationGUI_MirrorDlg::CreateCopyModeChanged()
{
  mainFrame()->GroupBoxName->setEnabled(GroupPoints->CheckButton1->isChecked());
}

//=================================================================================
// function : addSubshapeToStudy
// purpose  : virtual method to add new SubObjects if local selection
//=================================================================================
void TransformationGUI_MirrorDlg::addSubshapesToStudy()
{
  bool toCreateCopy = IsPreview() || GroupPoints->CheckButton1->isChecked();
  if (toCreateCopy) {
    switch (getConstructorId()) {
    case 0:
    case 1:
    case 2:
      GEOMBase::PublishSubObject( myArgument.get() );
      break;
    default:
      break;
    }
  }
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> TransformationGUI_MirrorDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res;
  if ( GroupPoints->CheckButton1->isChecked() ) {
    res.append(myObjects);
    res << myArgument;
  }
  return res;
}
