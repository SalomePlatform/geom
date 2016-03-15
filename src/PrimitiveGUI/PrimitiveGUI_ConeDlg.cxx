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
// File   : PrimitiveGUI_ConeDlg.cxx
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.
//
#include "PrimitiveGUI_ConeDlg.h"

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
// class    : PrimitiveGUI_ConeDlg()
// purpose  : Constructs a PrimitiveGUI_ConeDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
PrimitiveGUI_ConeDlg::PrimitiveGUI_ConeDlg (GeometryGUI* theGeometryGUI, QWidget* parent,
                                            bool modal, Qt::WindowFlags fl)
  : GEOMBase_Skeleton(theGeometryGUI, parent, modal, fl)
{
  QPixmap image0 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_CONE_PV")));
  QPixmap image1 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_CONE_DXYZ")));
  QPixmap image2 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_SELECT")));

  setWindowTitle(tr("GEOM_CONE_TITLE"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_CONE"));
  mainFrame()->RadioButton1->setIcon(image0);
  mainFrame()->RadioButton2->setIcon(image1);
  mainFrame()->RadioButton3->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton3->close();

  GroupPoints = new DlgRef_2Sel3Spin(centralWidget());
  GroupPoints->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  GroupPoints->TextLabel1->setText(tr("GEOM_BASE_POINT"));
  GroupPoints->TextLabel2->setText(tr("GEOM_VECTOR"));
  GroupPoints->TextLabel3->setText(tr("GEOM_RADIUS_I").arg(1));
  GroupPoints->TextLabel4->setText(tr("GEOM_RADIUS_I").arg(2));
  GroupPoints->TextLabel5->setText(tr("GEOM_HEIGHT"));
  GroupPoints->PushButton1->setIcon(image2);
  GroupPoints->PushButton2->setIcon(image2);

  GroupDimensions = new DlgRef_3Spin(centralWidget());
  GroupDimensions->GroupBox1->setTitle(tr("GEOM_BOX_OBJ"));
  GroupDimensions->TextLabel1->setText(tr("GEOM_RADIUS_I").arg(1));
  GroupDimensions->TextLabel2->setText(tr("GEOM_RADIUS_I").arg(2));
  GroupDimensions->TextLabel3->setText(tr("GEOM_HEIGHT"));

  QVBoxLayout* layout = new QVBoxLayout(centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
  layout->addWidget(GroupPoints);
  layout->addWidget(GroupDimensions);
  /***************************************************************/

  setHelpFileName("create_cone_page.html");

  Init();
}

//=================================================================================
// function : ~PrimitiveGUI_ConeDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
PrimitiveGUI_ConeDlg::~PrimitiveGUI_ConeDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void PrimitiveGUI_ConeDlg::Init()
{
  // Get setting of step value from file configuration
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  double step = resMgr->doubleValue("Geometry", "SettingsGeomStep", 100);

  // min, max, step and decimals for spin boxes & initial values
  initSpinBox(GroupPoints->SpinBox_DX, 0.0, COORD_MAX, step, "length_precision" );
  initSpinBox(GroupPoints->SpinBox_DY, 0.0, COORD_MAX, step, "length_precision" );
  initSpinBox(GroupPoints->SpinBox_DZ, 0.00001, COORD_MAX, step, "length_precision" );
  initSpinBox(GroupDimensions->SpinBox_DX, 0.0, COORD_MAX, step, "length_precision" );
  initSpinBox(GroupDimensions->SpinBox_DY, 0.0, COORD_MAX, step, "length_precision" );
  initSpinBox(GroupDimensions->SpinBox_DZ, 0.00001, COORD_MAX, step, "length_precision" );

  // init variables
  GroupPoints->LineEdit1->setReadOnly(true);
  GroupPoints->LineEdit2->setReadOnly(true);

  GroupPoints->LineEdit1->setText("");
  GroupPoints->LineEdit2->setText("");
  myPoint.nullify();
  myDir.nullify();

  double aRadius1(100.0), aRadius2(0.0), aHeight(300.0);
  GroupPoints->SpinBox_DX->setValue(aRadius1);
  GroupPoints->SpinBox_DY->setValue(aRadius2);
  GroupPoints->SpinBox_DZ->setValue(aHeight);
  GroupDimensions->SpinBox_DX->setValue(aRadius1);
  GroupDimensions->SpinBox_DY->setValue(aRadius2);
  GroupDimensions->SpinBox_DZ->setValue(aHeight);

  // signals and slots connections
  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(this,          SIGNAL(constructorsClicked(int)), this, SLOT(ConstructorsClicked(int)));

  connect(GroupPoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(GroupPoints->SpinBox_DX,     SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox()));
  connect(GroupPoints->SpinBox_DY,     SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox()));
  connect(GroupPoints->SpinBox_DZ,     SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox()));
  connect(GroupDimensions->SpinBox_DX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox()));
  connect(GroupDimensions->SpinBox_DY, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox()));
  connect(GroupDimensions->SpinBox_DZ, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox()));

  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), this, SLOT(SetDoubleSpinBoxStep(double)));

  initName(tr("GEOM_CONE"));

  setConstructorId(1); // simplest constructor
  ConstructorsClicked(1);
}

//=================================================================================
// function : SetDoubleSpinBoxStep()
// purpose  : Double spin box management
//=================================================================================
void PrimitiveGUI_ConeDlg::SetDoubleSpinBoxStep (double step)
{
  GroupPoints->SpinBox_DX->setSingleStep(step);
  GroupPoints->SpinBox_DY->setSingleStep(step);
  GroupPoints->SpinBox_DZ->setSingleStep(step);
  GroupDimensions->SpinBox_DX->setSingleStep(step);
  GroupDimensions->SpinBox_DY->setSingleStep(step);
  GroupDimensions->SpinBox_DZ->setSingleStep(step);
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void PrimitiveGUI_ConeDlg::ConstructorsClicked (int constructorId)
{
  switch (constructorId) {
  case 0:
    {
      GroupDimensions->hide();
      GroupPoints->show();

      GroupPoints->PushButton1->click();
      break;
    }
  case 1:
    {
      GroupPoints->hide();
      GroupDimensions->show();
      disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
      globalSelection(); // close local contexts, if any

      displayPreview(true);
      break;
    }
  }

  qApp->processEvents();
  updateGeometry();
  resize(minimumSizeHint());
  SelectionIntoArgument();

  displayPreview(true);
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void PrimitiveGUI_ConeDlg::ClickOnOk()
{
  setIsApplyAndClose( true );
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool PrimitiveGUI_ConeDlg::ClickOnApply()
{
  if (!onAccept())
    return false;

  initName();
  // activate selection and connect selection manager
  ConstructorsClicked(getConstructorId());
  return true;
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection is changed or on dialog initialization or activation
//=================================================================================
void PrimitiveGUI_ConeDlg::SelectionIntoArgument()
{
  if (getConstructorId() != 0)
    return;

  erasePreview();
  myEditCurrentArgument->setText("");

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  if (aSelList.Extent() != 1) {
    if      (myEditCurrentArgument == GroupPoints->LineEdit1) myPoint.nullify();
    else if (myEditCurrentArgument == GroupPoints->LineEdit2) myDir.nullify();
    return;
  }

  TopAbs_ShapeEnum aNeedType = myEditCurrentArgument == GroupPoints->LineEdit2 ? TopAbs_EDGE : TopAbs_VERTEX;
  GEOM::GeomObjPtr aSelectedObject = getSelected( aNeedType );
  TopoDS_Shape aShape;
  if ( aSelectedObject && GEOMBase::GetShape( aSelectedObject.get(), aShape ) && !aShape.IsNull() ) {
    QString aName = GEOMBase::GetName( aSelectedObject.get() );

    myEditCurrentArgument->setText(aName);
    
    if (myEditCurrentArgument == GroupPoints->LineEdit1) {
      myPoint = aSelectedObject;
      if (myPoint && !myDir)
        GroupPoints->PushButton2->click();
    }
    else if (myEditCurrentArgument == GroupPoints->LineEdit2) {
      myDir = aSelectedObject;
      if (myDir && !myPoint)
        GroupPoints->PushButton1->click();
    }

    if ((myPoint && !myDir) || (myDir && !myPoint)) {
      // clear selection
      disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
      myGeomGUI->getApp()->selectionMgr()->clearSelected();
      connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
              this, SLOT(SelectionIntoArgument()));
    }
  }

  displayPreview(true);
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void PrimitiveGUI_ConeDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
  if (send == GroupPoints->PushButton1) {
    myEditCurrentArgument = GroupPoints->LineEdit1;

    GroupPoints->PushButton2->setDown(false);
    GroupPoints->LineEdit2->setEnabled(false);

    globalSelection(GEOM_POINT); // to break previous local selection
    localSelection(TopAbs_VERTEX);
  }
  else if (send == GroupPoints->PushButton2) {
    myEditCurrentArgument = GroupPoints->LineEdit2;

    GroupPoints->PushButton1->setDown(false);
    GroupPoints->LineEdit1->setEnabled(false);

    globalSelection(GEOM_LINE);// to break previous local selection
    localSelection(TopAbs_EDGE);
  }
  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));

  // enable line edit
  myEditCurrentArgument->setEnabled(true);
  myEditCurrentArgument->setFocus();
  // after setFocus(), because it will be setDown(false) when loses focus
  send->setDown(true);

  // seems we need it only to avoid preview disappearing, caused by selection mode change
  displayPreview(true);
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void PrimitiveGUI_ConeDlg::ActivateThisDialog()
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
void PrimitiveGUI_ConeDlg::enterEvent (QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void PrimitiveGUI_ConeDlg::ValueChangedInSpinBox()
{
  displayPreview(true);
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr  PrimitiveGUI_ConeDlg::createOperation()
{
  return getGeomEngine()->GetI3DPrimOperations(getStudyId());
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool  PrimitiveGUI_ConeDlg::isValid (QString& msg)
{
  bool ok = false;
  if( getConstructorId() == 0 )
  {
    ok = GroupPoints->SpinBox_DX->isValid( msg, !IsPreview() ) &&
         GroupPoints->SpinBox_DY->isValid( msg, !IsPreview() ) &&
         GroupPoints->SpinBox_DZ->isValid( msg, !IsPreview() ) &&
         myPoint && myDir;
  }
  else if( getConstructorId() == 1 )
  {
    ok = GroupDimensions->SpinBox_DX->isValid( msg, !IsPreview() ) &&
         GroupDimensions->SpinBox_DY->isValid( msg, !IsPreview() ) &&
         GroupDimensions->SpinBox_DZ->isValid( msg, !IsPreview() );
  }
  ok = ( getRadius1() > 0 || getRadius2() > 0 ) && qAbs( getHeight() ) > Precision::Confusion() && ok;
  return ok;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool PrimitiveGUI_ConeDlg::execute (ObjectList& objects)
{
  bool res = false;

  GEOM::GEOM_Object_var anObj;

  GEOM::GEOM_I3DPrimOperations_var anOper = GEOM::GEOM_I3DPrimOperations::_narrow(getOperation());

  switch (getConstructorId()) {
  case 0:
    if ( myPoint && myDir ) {
      anObj = anOper->MakeConePntVecR1R2H(myPoint.get(), myDir.get(), getRadius1(), getRadius2(), getHeight());
      if (!anObj->_is_nil() && !IsPreview())
      {
        QStringList aParameters;
        aParameters << GroupPoints->SpinBox_DX->text();
        aParameters << GroupPoints->SpinBox_DY->text();
        aParameters << GroupPoints->SpinBox_DZ->text();
        anObj->SetParameters(aParameters.join(":").toLatin1().constData());
      }
      res = true;
    }
    break;
  case 1:
    anObj = anOper->MakeConeR1R2H(getRadius1(), getRadius2(), getHeight());
    if (!anObj->_is_nil() && !IsPreview())
    {
      QStringList aParameters;
      aParameters << GroupDimensions->SpinBox_DX->text();
      aParameters << GroupDimensions->SpinBox_DY->text();
      aParameters << GroupDimensions->SpinBox_DZ->text();
      anObj->SetParameters(aParameters.join(":").toLatin1().constData());
    }
    res = true;
    break;
  }

  if (!anObj->_is_nil())
    objects.push_back(anObj._retn());

  return res;
}

//=================================================================================
// function : getRadius1()
// purpose  :
//=================================================================================
double PrimitiveGUI_ConeDlg::getRadius1() const
{
  int aConstructorId = getConstructorId();
  if (aConstructorId == 0)
    return GroupPoints->SpinBox_DX->value();
  else if (aConstructorId == 1)
    return GroupDimensions->SpinBox_DX->value();
  return 0;
}

//=================================================================================
// function : getRadius2()
// purpose  :
//=================================================================================
double PrimitiveGUI_ConeDlg::getRadius2() const
{
  int aConstructorId = getConstructorId();
  if (aConstructorId == 0)
    return GroupPoints->SpinBox_DY->value();
  else if (aConstructorId == 1)
    return GroupDimensions->SpinBox_DY->value();
  return 0;
}

//=================================================================================
// function : getHeight()
// purpose  :
//=================================================================================
double PrimitiveGUI_ConeDlg::getHeight() const
{
  int aConstructorId = getConstructorId();
  if (aConstructorId == 0)
    return GroupPoints->SpinBox_DZ->value();
  else if (aConstructorId == 1)
    return GroupDimensions->SpinBox_DZ->value();
  return 0;
}

//=================================================================================
// function : addSubshapeToStudy
// purpose  : virtual method to add new SubObjects if local selection
//=================================================================================
void PrimitiveGUI_ConeDlg::addSubshapesToStudy()
{
  if ( getConstructorId() == 0 ) {
    GEOMBase::PublishSubObject( myPoint.get() );
    GEOMBase::PublishSubObject( myDir.get() );
  }
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> PrimitiveGUI_ConeDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res;
  res << myPoint << myDir;
  return res;
}
