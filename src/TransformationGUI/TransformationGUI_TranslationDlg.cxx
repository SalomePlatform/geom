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
// File   : TransformationGUI_TranslationDlg.cxx
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.
//
#include "TransformationGUI_TranslationDlg.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Desktop.h>
#include <SUIT_ViewWindow.h>
#include <SUIT_ViewManager.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>
#include <GEOM_AISVector.hxx>
#include <SOCC_Prs.h>
#include <SOCC_ViewModel.h>

// OCCT Includes
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRep_Builder.hxx>

#include <GEOMImpl_Types.hxx>

//=================================================================================
// class    : TransformationGUI_TranslationDlg()
// purpose  : Constructs a TransformationGUI_TranslationDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
TransformationGUI_TranslationDlg::TransformationGUI_TranslationDlg
(GeometryGUI* theGeometryGUI, QWidget* parent, bool modal, Qt::WindowFlags fl)
  : GEOMBase_Skeleton(theGeometryGUI, parent, modal, fl),
    myInitial(true)
{
  SUIT_ResourceMgr* aResMgr = myGeomGUI->getApp()->resourceMgr();
  QPixmap image0 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_TRANSLATION_DXYZ")));
  QPixmap image1 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_TRANSLATION_POINTS")));
  QPixmap image2 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_TRANSLATION_VECTOR")));
  QPixmap image3 (aResMgr->loadPixmap("GEOM", tr("ICON_SELECT")));

  setWindowTitle(tr("GEOM_TRANSLATION_TITLE"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_TRANSLATION"));
  mainFrame()->RadioButton1->setIcon(image0);
  mainFrame()->RadioButton2->setIcon(image1);
  mainFrame()->RadioButton3->setIcon(image2);
  mainFrame()->RadioButton1->setChecked(true);

  GroupPoints = new DlgRef_3Sel3Spin2Check(centralWidget());
  GroupPoints->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  GroupPoints->TextLabel1->setText(tr("GEOM_OBJECTS"));
  GroupPoints->TextLabel3->setText(tr("GEOM_POINT_I").arg(2));
  GroupPoints->TextLabel4->setText(tr("GEOM_DX"));
  GroupPoints->TextLabel5->setText(tr("GEOM_DY"));
  GroupPoints->TextLabel6->setText(tr("GEOM_DZ"));
  GroupPoints->PushButton1->setIcon(image3);
  GroupPoints->PushButton2->setIcon(image3);
  GroupPoints->PushButton3->setIcon(image3);
  GroupPoints->CheckBox1->setText(tr("Activate Distance"));
  GroupPoints->CheckBox2->setText(tr("GEOM_CREATE_COPY"));

  QVBoxLayout* layout = new QVBoxLayout(centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
  layout->addWidget(GroupPoints);
  /***************************************************************/

  setHelpFileName("translation_operation_page.html");

  // Activate Create a Copy mode
  GroupPoints->CheckBox2->setChecked(true);
  CreateCopyModeChanged();

  Init();
}

//=================================================================================
// function : ~TransformationGUI_TranslationDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
TransformationGUI_TranslationDlg::~TransformationGUI_TranslationDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void TransformationGUI_TranslationDlg::Init()
{
  // Get setting of step value from file configuration
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  double step = resMgr->doubleValue("Geometry", "SettingsGeomStep", 100);

  // min, max, step and decimals for spin boxes & initial values
  initSpinBox(GroupPoints->SpinBox1, COORD_MIN, COORD_MAX, step, "length_precision" );
  initSpinBox(GroupPoints->SpinBox2, COORD_MIN, COORD_MAX, step, "length_precision" );
  initSpinBox(GroupPoints->SpinBox3, COORD_MIN, COORD_MAX, step, "length_precision" );

  GroupPoints->SpinBox1->setValue(0.0);
  GroupPoints->SpinBox2->setValue(0.0);
  GroupPoints->SpinBox3->setValue(0.0);

  // init variables
  GroupPoints->LineEdit1->setReadOnly(true);
  GroupPoints->LineEdit2->setReadOnly(true);
  GroupPoints->LineEdit3->setReadOnly(true);

  GroupPoints->LineEdit1->setText("");
  GroupPoints->LineEdit2->setText("");
  GroupPoints->LineEdit3->setText("");

  myObjects.clear();
  myVector.nullify();
  myPoint1.nullify();
  myPoint2.nullify();

  mainFrame()->GroupBoxPublish->show();

  // signals and slots connections
  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(this, SIGNAL(constructorsClicked(int)), this, SLOT(ConstructorsClicked(int)));

  connect(GroupPoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->PushButton3, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(GroupPoints->SpinBox1, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox()));
  connect(GroupPoints->SpinBox2, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox()));
  connect(GroupPoints->SpinBox3, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox()));

  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), this, SLOT(SetDoubleSpinBoxStep(double)));

  connect(GroupPoints->CheckBox1, SIGNAL(toggled(bool)), this, SLOT(ActivateDistanceChanged()));
  connect(GroupPoints->CheckBox2, SIGNAL(toggled(bool)), this, SLOT(CreateCopyModeChanged()));

  initName(tr("GEOM_TRANSLATION"));

  ConstructorsClicked(0);
}

//=================================================================================
// function : SetDoubleSpinBoxStep()
// purpose  : Double spin box management
//=================================================================================
void TransformationGUI_TranslationDlg::SetDoubleSpinBoxStep (double step)
{
  GroupPoints->SpinBox1->setSingleStep(step);
  GroupPoints->SpinBox2->setSingleStep(step);
  GroupPoints->SpinBox3->setSingleStep(step);
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void TransformationGUI_TranslationDlg::ConstructorsClicked (int constructorId)
{
  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);

  switch (constructorId) {
  case 0: // translation an object by dx, dy, dz
    {
      GroupPoints->ShowRows(1, 2, false);
      GroupPoints->ShowRows(3, 5, true);

      GroupPoints->TextLabel6->setText(tr("GEOM_DZ"));
      GroupPoints->SpinBox3->setEnabled(true);

      GroupPoints->CheckBox1->hide();

      GroupPoints->PushButton1->click();
    }
    break;
  case 1: // translation an object by 2 points
    {
      GroupPoints->ShowRows(3, 5, false);
      GroupPoints->ShowRows(0, 2, true);

      GroupPoints->TextLabel6->setText(tr("GEOM_DZ"));
      GroupPoints->TextLabel2->setText(tr("GEOM_POINT_I").arg(1));
      GroupPoints->LineEdit2->clear();
      GroupPoints->LineEdit3->clear();
      GroupPoints->SpinBox3->setEnabled(true);

      GroupPoints->CheckBox1->hide();

      myPoint1.nullify();
      myPoint2.nullify();

      GroupPoints->PushButton1->click();
    }
    break;
  case 2: // translation an object by vector
    {
      GroupPoints->ShowRows(0, 1, true);
      GroupPoints->ShowRows(2, 4, false);
      GroupPoints->ShowRows(5, 5, true);

      GroupPoints->TextLabel6->setText(tr("GEOM_DISTANCE"));
      GroupPoints->SpinBox3->setValue(0.0);
      GroupPoints->SpinBox3->setEnabled(false);
      GroupPoints->TextLabel2->setText(tr("GEOM_VECTOR"));
      GroupPoints->LineEdit2->clear();

      GroupPoints->CheckBox1->show();

      myVector.nullify();
      ActivateDistanceChanged();

      GroupPoints->PushButton1->click();
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
// function : ClickOnOk()
// purpose  :
//=================================================================================
void TransformationGUI_TranslationDlg::ClickOnOk()
{
  setIsApplyAndClose( true );
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool TransformationGUI_TranslationDlg::ClickOnApply()
{
  if (!onAccept(GroupPoints->CheckBox2->isChecked()))
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
void TransformationGUI_TranslationDlg::SelectionIntoArgument()
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
    TopAbs_ShapeEnum aNeedType = ( myEditCurrentArgument == GroupPoints->LineEdit2 && getConstructorId() == 2 ) ?
      TopAbs_EDGE : TopAbs_VERTEX;
    GEOM::GeomObjPtr aSelectedObject = getSelected( aNeedType );
    TopoDS_Shape aShape;
    if ( aSelectedObject && GEOMBase::GetShape( aSelectedObject.get(), aShape ) && !aShape.IsNull() ) {
      QString aName = GEOMBase::GetName( aSelectedObject.get() );
      myEditCurrentArgument->setText( aName );
      if ( myEditCurrentArgument == GroupPoints->LineEdit2 ) {
        if ( getConstructorId() == 1 ) {
          myPoint1 = aSelectedObject;
          if ( !myPoint2 )
            GroupPoints->PushButton3->click();
          else if ( myObjects.isEmpty() )
            GroupPoints->PushButton1->click();
        }
        else if ( getConstructorId() == 2 ) {
          myVector = aSelectedObject;
          if ( myObjects.isEmpty() )
            GroupPoints->PushButton1->click();
        }
      }
      else if ( myEditCurrentArgument == GroupPoints->LineEdit3 ) {
        myPoint2 = aSelectedObject;
        if ( myObjects.isEmpty() )
          GroupPoints->PushButton1->click();
        else if ( !myPoint1 )
          GroupPoints->PushButton2->click();
      }
    }
    else {
      if ( myEditCurrentArgument == GroupPoints->LineEdit2 ) {
        if ( getConstructorId() == 1 ) myPoint1.nullify();
        else myVector.nullify();
      }
      else if ( myEditCurrentArgument == GroupPoints->LineEdit3 ) {
        myPoint2.nullify();
      }
      myEditCurrentArgument->setText("");
    }
  }

  processPreview();
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void TransformationGUI_TranslationDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
  globalSelection();

  if (send == GroupPoints->PushButton1) {
    myEditCurrentArgument = GroupPoints->LineEdit1;

    GroupPoints->PushButton2->setDown(false);
    GroupPoints->PushButton3->setDown(false);
    GroupPoints->LineEdit2->setEnabled(false);
    GroupPoints->LineEdit3->setEnabled(false);
  }
  else if (send == GroupPoints->PushButton2) {
    myEditCurrentArgument = GroupPoints->LineEdit2;

    GroupPoints->PushButton1->setDown(false);
    GroupPoints->PushButton3->setDown(false);
    GroupPoints->LineEdit1->setEnabled(false);
    GroupPoints->LineEdit3->setEnabled(false);

    if (getConstructorId() == 1)
      localSelection(TopAbs_VERTEX);
    else
      localSelection(TopAbs_EDGE);
  }
  else if (send == GroupPoints->PushButton3) {
    myEditCurrentArgument = GroupPoints->LineEdit3;

    GroupPoints->PushButton1->setDown(false);
    GroupPoints->PushButton2->setDown(false);
    GroupPoints->LineEdit1->setEnabled(false);
    GroupPoints->LineEdit2->setEnabled(false);

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
  processPreview();
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void TransformationGUI_TranslationDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
           this, SLOT( SelectionIntoArgument() ) );

  ConstructorsClicked( getConstructorId() );
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void TransformationGUI_TranslationDlg::enterEvent (QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void TransformationGUI_TranslationDlg::ValueChangedInSpinBox()
{
  processPreview();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr TransformationGUI_TranslationDlg::createOperation()
{
  return getGeomEngine()->GetITransformOperations(getStudyId());
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool TransformationGUI_TranslationDlg::isValid (QString& msg)
{
  bool ok = false;
  switch (getConstructorId()) {
  case 0: 
    ok = GroupPoints->SpinBox1->isValid( msg, !IsPreview() ) &&
         GroupPoints->SpinBox2->isValid( msg, !IsPreview() ) &&
         GroupPoints->SpinBox3->isValid( msg, !IsPreview() ) &&
         !myObjects.isEmpty();
    break;
  case 1:
    ok = myPoint1 && myPoint2 && !myObjects.isEmpty();
    break;
  case 2:
      ok = GroupPoints->SpinBox3->isValid( msg, !IsPreview() ) &&
           myVector && !myObjects.isEmpty();
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
bool TransformationGUI_TranslationDlg::execute (ObjectList& objects)
{
  bool res = false;
  bool toCreateCopy = IsPreview() || GroupPoints->CheckBox2->isChecked();

  GEOM::GEOM_Object_var anObj;

  GEOM::GEOM_ITransformOperations_var anOper = GEOM::GEOM_ITransformOperations::_narrow(getOperation());

  switch (getConstructorId()) {
  case 0:
    {
      double dx = GroupPoints->SpinBox1->value();
      double dy = GroupPoints->SpinBox2->value();
      double dz = GroupPoints->SpinBox3->value();

      QStringList aParameters;
      aParameters<<GroupPoints->SpinBox1->text();
      aParameters<<GroupPoints->SpinBox2->text();
      aParameters<<GroupPoints->SpinBox3->text();

      if (toCreateCopy) {
        for (int i = 0; i < myObjects.count(); i++) {
          myCurrObject = myObjects[i];
          anObj = anOper->TranslateDXDYDZCopy(myObjects[i].get(), dx, dy, dz);
          if (!anObj->_is_nil()) {
            if(!IsPreview())
              anObj->SetParameters(aParameters.join(":").toLatin1().constData());
            objects.push_back(anObj._retn());
          }
        }
      }
      else {
        for (int i = 0; i < myObjects.count(); i++) {
          myCurrObject = myObjects[i];
          anObj = anOper->TranslateDXDYDZ(myObjects[i].get(), dx, dy, dz);
          if (!anObj->_is_nil()) {
            if(!IsPreview()) {
              anObj->SetParameters(aParameters.join(":").toLatin1().constData());
              updateAttributes(anObj, aParameters);
            }
            objects.push_back(anObj._retn());
          }
        }
      }
      res = true;
      break;
    }
  case 1:
    {
      if (toCreateCopy) {
        for (int i = 0; i < myObjects.count(); i++) {
          myCurrObject = myObjects[i];
          anObj = anOper->TranslateTwoPointsCopy(myObjects[i].get(), myPoint1.get(), myPoint2.get());
          if (!anObj->_is_nil())
            objects.push_back(anObj._retn());
        }
      }
      else {
        for (int i = 0; i < myObjects.count(); i++) {
          myCurrObject = myObjects[i];
          anObj = anOper->TranslateTwoPoints(myObjects[i].get(), myPoint1.get(), myPoint2.get());
          if (!anObj->_is_nil())
            objects.push_back(anObj._retn());
        }
      }
      res = true;
      break;
    }
  case 2:
    {
      QStringList aParameters;
      aParameters<<GroupPoints->SpinBox3->text();
      bool byDistance = GroupPoints->CheckBox1->isChecked();
      createPathPreview( myVector.get() );

      if (byDistance) {
        double aDistance = GroupPoints->SpinBox3->value();
        for (int i = 0; i < myObjects.count(); i++) {
          myCurrObject = myObjects[i];
          anObj = anOper->TranslateVectorDistance(myObjects[i].get(), myVector.get(), aDistance, toCreateCopy);
          if (!anObj->_is_nil()) {
            if(!IsPreview()) {
              anObj->SetParameters(aParameters.join(":").toLatin1().constData());
              if (!toCreateCopy)
                updateAttributes(anObj, aParameters);
            }
            objects.push_back(anObj._retn());
          }
        }
      }
      else {
        if (toCreateCopy) {
          for (int i = 0; i < myObjects.count(); i++) {
            myCurrObject = myObjects[i];
            anObj = anOper->TranslateVectorCopy(myObjects[i].get(), myVector.get());
            if (!anObj->_is_nil())
              objects.push_back(anObj._retn());
          }
        }
        else {
          for (int i = 0; i < myObjects.count(); i++) {
            myCurrObject = myObjects[i];
            anObj = anOper->TranslateVector(myObjects[i].get(), myVector.get());
            if (!anObj->_is_nil())
              objects.push_back(anObj._retn());
          }
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
void TransformationGUI_TranslationDlg::restoreSubShapes (SALOMEDS::Study_ptr   theStudy,
                                                         SALOMEDS::SObject_ptr theSObject)
{
  if (mainFrame()->CheckBoxRestoreSS->isChecked()) {
    // we pass here the first operation argument (object) through the list of arguments
    // because the rotation operation place its arguments in the data structure in another order,
    // and we need to point the first argument directly
    GEOM::ListOfGO_var anArgs = new GEOM::ListOfGO;
    anArgs->length(1);
    anArgs[0] = myCurrObject.copy();
    getGeomEngine()->RestoreSubShapesSO(theStudy, theSObject, anArgs,
                                        /*theFindMethod=*/GEOM::FSM_Transformed,
                                        /*theInheritFirstArg=*/true,
                                        mainFrame()->CheckBoxAddPrefix->isChecked());
  }
}

//=================================================================================
// function :  CreateCopyModeChanged()
// purpose  :
//=================================================================================
void TransformationGUI_TranslationDlg::CreateCopyModeChanged()
{
  mainFrame()->GroupBoxName->setEnabled(GroupPoints->CheckBox2->isChecked());
}

//=================================================================================
// function :  ActivateDistanceChanged()
// purpose  :
//=================================================================================
void TransformationGUI_TranslationDlg::ActivateDistanceChanged()
{
  GroupPoints->SpinBox3->setEnabled( GroupPoints->CheckBox1->isChecked() );
  processPreview();
}

//=================================================================================
// function : addSubshapeToStudy
// purpose  : virtual method to add new SubObjects if local selection
//=================================================================================
void TransformationGUI_TranslationDlg::addSubshapesToStudy()
{
  bool toCreateCopy = IsPreview() || GroupPoints->CheckBox2->isChecked();
  if (toCreateCopy) {
    switch (getConstructorId()) {
    case 1:
      GEOMBase::PublishSubObject( myPoint1.get() );
      GEOMBase::PublishSubObject( myPoint2.get() );
      break;
    case 2:
      GEOMBase::PublishSubObject( myVector.get() );
      break;
    default:
      break;
    }
  }
}

//=================================================================================
// function : createPathPreview
// purpose  :
//=================================================================================
void TransformationGUI_TranslationDlg::createPathPreview ( GEOM::GEOM_Object_ptr thePath )
{
  if ( IsPreview() ) {
    TopoDS_Shape aShape;
    GEOMBase::GetShape( thePath, aShape, TopAbs_SHAPE );
    TopoDS_Edge anEdge = TopoDS::Edge( aShape );
    ShapeAnalysis_Edge aShapeAnal;
    TopoDS_Vertex aFirst = aShapeAnal.FirstVertex( anEdge );
    TopoDS_Vertex aLast = aShapeAnal.LastVertex( anEdge );
    TopoDS_Shape aVector = BRepBuilderAPI_MakeEdge(BRep_Tool::Pnt(aFirst), BRep_Tool::Pnt(aLast)).Shape();
    const char* aName = "tmpVector";
    Handle(GEOM_AISVector) anIO = new GEOM_AISVector( aVector, aName );
    
    // add Prs to preview
    SUIT_ViewWindow* vw = SUIT_Session::session()->activeApplication()->desktop()->activeWindow();
    SOCC_Prs* aPrs = dynamic_cast<SOCC_Prs*>(((SOCC_Viewer*)(vw->getViewManager()->getViewModel()))->CreatePrs(0));
    if (aPrs)
      aPrs->AddObject(anIO);
    GEOMBase_Helper::displayPreview( aPrs, false, true );
  }
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> TransformationGUI_TranslationDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res;
  if ( GroupPoints->CheckBox2->isChecked() ) {
    res.append(myObjects);
    res << myVector << myPoint1 << myPoint2;
  }
  return res;
}
