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
// File   : TransformationGUI_PositionDlg.cxx
// Author : Damien COQUERET, Open CASCADE S.A.S.
//
#include "TransformationGUI_PositionDlg.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>
#include <TColStd_IndexedMapOfInteger.hxx>

// OCCT Includes
#include <TColStd_MapOfInteger.hxx>

#include <GEOMImpl_Types.hxx>

//=================================================================================
// class    : TransformationGUI_PositionDlg()
// purpose  : Constructs a TransformationGUI_PositionDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
TransformationGUI_PositionDlg::TransformationGUI_PositionDlg
(GeometryGUI* theGeometryGUI, QWidget* parent, bool modal, Qt::WindowFlags fl)
  : GEOMBase_Skeleton(theGeometryGUI, parent, modal, fl),
    myInitial(true)
{
  SUIT_ResourceMgr* aResMgr = myGeomGUI->getApp()->resourceMgr();
  QPixmap image0 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_POSITION")));
  QPixmap image1 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_POSITION2")));
  QPixmap image2 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_POSITION3")));
  QPixmap imageselect (aResMgr->loadPixmap("GEOM", tr("ICON_SELECT")));

  setWindowTitle(tr("GEOM_POSITION_TITLE"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_POSITION"));
  mainFrame()->RadioButton1->setIcon(image0);
  mainFrame()->RadioButton2->setIcon(image1);
  mainFrame()->RadioButton3->setIcon(image2);

  Group1 = new DlgRef_4Sel1Spin3Check(centralWidget());
  Group1->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  Group1->TextLabel1->setText(tr("GEOM_OBJECTS"));
  Group1->TextLabel2->setText(tr("GEOM_START_LCS"));
  Group1->TextLabel3->setText(tr("GEOM_DISTANCE"));
  Group1->TextLabel4->setText(tr("GEOM_END_LCS"));
  Group1->TextLabel5->setText(tr("GEOM_PATH_OBJECT"));
  Group1->PushButton1->setIcon(imageselect);
  Group1->PushButton2->setIcon(imageselect);
  Group1->PushButton4->setIcon(imageselect);
  Group1->PushButton5->setIcon(imageselect);
  Group1->CheckButton1->setText(tr("GEOM_CREATE_COPY"));
  Group1->CheckButton2->setText(tr("SELECT_UNPUBLISHED_EDGES"));
  Group1->CheckButton3->setText(tr("GEOM_REVERSE_DIRECTION"));

  QVBoxLayout* layout = new QVBoxLayout(centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
  layout->addWidget(Group1);
  /***************************************************************/

  setHelpFileName("modify_location_operation_page.html");

  // Activate Create a Copy mode
  Group1->CheckButton1->setChecked(true);
  CreateCopyModeChanged();

  Init();
}

//=================================================================================
// function : ~TransformationGUI_PositionDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
TransformationGUI_PositionDlg::~TransformationGUI_PositionDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void TransformationGUI_PositionDlg::Init()
{
  // init variables
  Group1->LineEdit1->setReadOnly(true);
  Group1->LineEdit2->setReadOnly(true);
  Group1->LineEdit4->setReadOnly(true);
  Group1->LineEdit5->setReadOnly(true);

  Group1->LineEdit1->setText("");
  Group1->LineEdit2->setText("");
  Group1->LineEdit4->setText("");
  Group1->LineEdit5->setText("");

  initSpinBox(Group1->SpinBox_DX, 0, 1, 0.05, "parametric_precision" );
  Group1->SpinBox_DX->setValue(0);

  Group1->CheckButton2->setEnabled(false);

  myObjects.clear();
  myStartLCS.nullify();
  myEndLCS.nullify();
  myPath.nullify();

  mainFrame()->GroupBoxPublish->show();

  // signals and slots connections
  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(this, SIGNAL(constructorsClicked(int)), this, SLOT(ConstructorsClicked(int)));

  connect(Group1->PushButton1, SIGNAL(clicked()),       this, SLOT(SetEditCurrentArgument()));
  connect(Group1->PushButton2, SIGNAL(clicked()),       this, SLOT(SetEditCurrentArgument()));
  connect(Group1->PushButton4, SIGNAL(clicked()),       this, SLOT(SetEditCurrentArgument()));
  connect(Group1->PushButton5, SIGNAL(clicked()),       this, SLOT(SetEditCurrentArgument()));

  connect(Group1->CheckButton1,   SIGNAL(toggled(bool)), this, SLOT(CreateCopyModeChanged()));
  connect(Group1->SpinBox_DX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox()));

  connect(Group1->CheckButton2,   SIGNAL(toggled(bool)), this, SLOT(SelectionTypeButtonClicked()));
  connect(Group1->CheckButton3,   SIGNAL(toggled(bool)), this, SLOT(ValueChangedInSpinBox()));

  initName(tr("GEOM_POSITION"));

  ConstructorsClicked(0);
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void TransformationGUI_PositionDlg::ConstructorsClicked (int constructorId)
{
  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);

  Group1->LineEdit2->clear();
  Group1->LineEdit4->clear();
  Group1->LineEdit5->clear();
  myStartLCS.nullify();
  myEndLCS.nullify();
  myPath.nullify();

  switch (constructorId) {
  case 0:
    {
      Group1->LineEdit2->hide();
      Group1->TextLabel2->hide();
      Group1->PushButton2->hide();

      Group1->LineEdit5->hide();
      Group1->TextLabel5->hide();
      Group1->PushButton5->hide();

      Group1->SpinBox_DX->hide();
      Group1->TextLabel3->hide();

      Group1->LineEdit4->show();
      Group1->TextLabel4->show();
      Group1->PushButton4->show();

      Group1->PushButton1->click();

      Group1->CheckButton2->hide();
      Group1->CheckButton3->hide();
    }
    break;
  case 1:
    {
      Group1->LineEdit2->show();
      Group1->TextLabel2->show();
      Group1->PushButton2->show();

      Group1->LineEdit5->hide();
      Group1->TextLabel5->hide();
      Group1->PushButton5->hide();

      Group1->SpinBox_DX->hide();
      Group1->TextLabel3->hide();

      Group1->PushButton1->click();

      Group1->CheckButton2->hide();
      Group1->CheckButton3->hide();
    }
    break;
  case 2:
    {
      Group1->LineEdit4->hide();
      Group1->TextLabel4->hide();
      Group1->PushButton4->hide();

      Group1->LineEdit2->hide();
      Group1->TextLabel2->hide();
      Group1->PushButton2->hide();

      Group1->LineEdit5->show();
      Group1->TextLabel5->show();
      Group1->PushButton5->show();

      Group1->SpinBox_DX->show();
      Group1->TextLabel3->show();

      Group1->PushButton1->click();

      Group1->CheckButton2->show();
      Group1->CheckButton3->show();
    }
    break;
  }

  qApp->processEvents();
  updateGeometry();
  resize(minimumSizeHint());

  if (myInitial) {
    myInitial = false;
    SelectionIntoArgument();
  }
  else {
    processPreview();
  }
}

//=================================================================================
// function : SelectionBittonClicked()
// purpose  : Selection type Radio button management
//=================================================================================
void TransformationGUI_PositionDlg::SelectionTypeButtonClicked()
{
  if ( Group1->CheckButton2->isChecked() ) {
    localSelection( TopAbs_EDGE );
  } else {
    TColStd_MapOfInteger aMap;
    aMap.Add(GEOM_WIRE);
    aMap.Add(GEOM_LINE);
    globalSelection(aMap);
  }
}

//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void TransformationGUI_PositionDlg::ValueChangedInSpinBox()
{
  processPreview();
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void TransformationGUI_PositionDlg::ClickOnOk()
{
  setIsApplyAndClose( true );
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool TransformationGUI_PositionDlg::ClickOnApply()
{
  if (!onAccept(Group1->CheckButton1->isChecked()))
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
void TransformationGUI_PositionDlg::SelectionIntoArgument()
{
  erasePreview();
  myEditCurrentArgument->setText("");

  if ( myEditCurrentArgument == Group1->LineEdit1 ) {
    myObjects = getSelected( TopAbs_SHAPE, -1 );
    if ( !myObjects.isEmpty() ) {
      QString aName = myObjects.count() > 1 ? QString( "%1_objects").arg( myObjects.count() ) : GEOMBase::GetName( myObjects[0].get() );
      myEditCurrentArgument->setText( aName );
    }
  }
  else if ( myEditCurrentArgument == Group1->LineEdit5 ) {
    QList<TopAbs_ShapeEnum> types;
    types << TopAbs_EDGE << TopAbs_WIRE;
    GEOM::GeomObjPtr aSelectedObject = getSelected( types );
    TopoDS_Shape aShape;
    if ( aSelectedObject && GEOMBase::GetShape( aSelectedObject.get(), aShape ) && !aShape.IsNull() ) {
      QString aName = GEOMBase::GetName( aSelectedObject.get() );
      myEditCurrentArgument->setText( aName );
      myPath = aSelectedObject;
      if ( myObjects.isEmpty() )
        Group1->PushButton1->click();
    }
    else {
      myPath.nullify();
    }
  }
  else {
    GEOM::GeomObjPtr aSelectedObject = getSelected( TopAbs_SHAPE );
    TopoDS_Shape aShape;
    if ( aSelectedObject && GEOMBase::GetShape( aSelectedObject.get(), aShape ) && !aShape.IsNull() ) {
      QString aName = GEOMBase::GetName( aSelectedObject.get() );
      myEditCurrentArgument->setText( aName );
      if ( myEditCurrentArgument == Group1->LineEdit2 ) {
        myStartLCS = aSelectedObject;
        if ( !myEndLCS )
          Group1->PushButton4->click();
        else if ( myObjects.isEmpty() )
          Group1->PushButton1->click();
      }
      else if ( myEditCurrentArgument == Group1->LineEdit4 ) {
        myEndLCS = aSelectedObject;
        if ( myObjects.isEmpty() )
          Group1->PushButton1->click();
        else if ( getConstructorId() == 1 && !myStartLCS )
          Group1->PushButton2->click();
      }
    }
    else {
      if ( myEditCurrentArgument == Group1->LineEdit2 )
        myStartLCS.nullify();
      else if ( myEditCurrentArgument == Group1->LineEdit4 )
        myEndLCS.nullify();
    }
  }

  processPreview();
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void TransformationGUI_PositionDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
  if (send == Group1->PushButton1) {
    myEditCurrentArgument = Group1->LineEdit1;

    globalSelection();

    Group1->PushButton2->setDown(false);
    Group1->PushButton4->setDown(false);
    Group1->PushButton5->setDown(false);
    Group1->LineEdit2->setEnabled(false);
    Group1->LineEdit4->setEnabled(false);
    Group1->LineEdit5->setEnabled(false);

    Group1->CheckButton2->setEnabled(false);
  }
  else if (send == Group1->PushButton2) {
    myEditCurrentArgument = Group1->LineEdit2;

    TColStd_MapOfInteger aMap;
    aMap.Add(GEOM_PLANE);
    aMap.Add(GEOM_MARKER);
    globalSelection(aMap);

    Group1->PushButton1->setDown(false);
    Group1->PushButton4->setDown(false);
    Group1->PushButton5->setDown(false);
    Group1->LineEdit1->setEnabled(false);
    Group1->LineEdit4->setEnabled(false);
    Group1->LineEdit5->setEnabled(false);

    Group1->CheckButton2->setEnabled(false);
  }
  else if (send == Group1->PushButton4) {
    myEditCurrentArgument = Group1->LineEdit4;

    TColStd_MapOfInteger aMap;
    aMap.Add(GEOM_PLANE);
    aMap.Add(GEOM_MARKER);
    globalSelection(aMap);

    Group1->PushButton1->setDown(false);
    Group1->PushButton2->setDown(false);
    Group1->PushButton5->setDown(false);
    Group1->LineEdit1->setEnabled(false);
    Group1->LineEdit2->setEnabled(false);
    Group1->LineEdit5->setEnabled(false);

    Group1->CheckButton2->setEnabled(false);
  }
  else if (send == Group1->PushButton5) {
    myEditCurrentArgument = Group1->LineEdit5;

    Group1->CheckButton2->setEnabled(true);

    if ( Group1->CheckButton2->isChecked() ) {
      localSelection( TopAbs_EDGE );
    } else {
      TColStd_MapOfInteger aMap;
      aMap.Add(GEOM_WIRE);
      aMap.Add(GEOM_LINE);
      globalSelection(aMap);
    }

    Group1->PushButton1->setDown(false);
    Group1->PushButton2->setDown(false);
    Group1->PushButton4->setDown(false);
    Group1->LineEdit1->setEnabled(false);
    Group1->LineEdit2->setEnabled(false);
    Group1->LineEdit4->setEnabled(false);
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
void TransformationGUI_PositionDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
           this, SLOT( SelectionIntoArgument() ) );

  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
           this, SLOT( SelectionIntoArgument() ) );

  ConstructorsClicked( getConstructorId() );
}

//=================================================================================
// function : enterEvent()
// purpose  : when mouse enter onto the QWidget
//=================================================================================
void TransformationGUI_PositionDlg::enterEvent (QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr TransformationGUI_PositionDlg::createOperation()
{
  return getGeomEngine()->GetITransformOperations(getStudyId());
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool TransformationGUI_PositionDlg::isValid (QString& /*msg*/)
{
  bool ok = false;
  switch (getConstructorId()) {
  case 0:
    ok = !myObjects.isEmpty() && myEndLCS;
    break;
  case 1:
    ok = !myObjects.isEmpty() && myStartLCS && myEndLCS;
    break;
  case 2:
    ok = !myObjects.isEmpty() && myPath;
    break;
  default:
    break;
  }
  return ok;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool TransformationGUI_PositionDlg::execute (ObjectList& objects)
{
  bool res = false;
  bool toCreateCopy = IsPreview() || Group1->CheckButton1->isChecked();
  GEOM::GEOM_Object_var anObj;

  GEOM::GEOM_ITransformOperations_var anOper = GEOM::GEOM_ITransformOperations::_narrow(getOperation());

  switch (getConstructorId()) {
  case 0:
    {
      for (int i = 0; i < myObjects.count(); i++) {
        anObj = toCreateCopy ? 
          anOper->PositionShapeCopy(myObjects[i].get(), myObjects[i].get(), myEndLCS.get()) :
          anOper->PositionShape(myObjects[i].get(), myObjects[i].get(), myEndLCS.get());

        if (!anObj->_is_nil())
          objects.push_back(anObj._retn());
      }
      res = true;
      break;
    }
  case 1:
    {
      for (int i = 0; i < myObjects.count(); i++) {
          anObj = toCreateCopy ? 
            anOper->PositionShapeCopy(myObjects[i].get(), myStartLCS.get(), myEndLCS.get()) :
            anOper->PositionShape(myObjects[i].get(), myStartLCS.get(), myEndLCS.get());
        if (!anObj->_is_nil())
          objects.push_back(anObj._retn());
      }
      res = true;
      break;
    }
  case 2:
    {
      double aDistance = Group1->SpinBox_DX->value();
      bool toReverse = Group1->CheckButton3->isChecked();
      for (int i = 0; i < myObjects.count(); i++) {
        anObj = anOper->PositionAlongPath(myObjects[i].get(), myPath.get(), aDistance, toCreateCopy, toReverse);
        if (!anObj->_is_nil())
          objects.push_back(anObj._retn());
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
void TransformationGUI_PositionDlg::restoreSubShapes (SALOMEDS::Study_ptr   theStudy,
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
void TransformationGUI_PositionDlg::CreateCopyModeChanged()
{
  mainFrame()->GroupBoxName->setEnabled(Group1->CheckButton1->isChecked());
}

//=================================================================================
// function : addSubshapeToStudy
// purpose  : virtual method to add new SubObjects if local selection
//=================================================================================
void TransformationGUI_PositionDlg::addSubshapesToStudy()
{
  if ( getConstructorId() == 2 )
    GEOMBase::PublishSubObject( myPath.get() );
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> TransformationGUI_PositionDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res;
  if ( Group1->CheckButton1->isChecked() ) {
    res.append(myObjects);
    res << myStartLCS << myEndLCS << myPath;
  }
  return res;
}
