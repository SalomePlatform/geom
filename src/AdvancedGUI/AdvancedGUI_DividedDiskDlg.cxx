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

#include "AdvancedGUI_DividedDiskDlg.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

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

// enum
// {
//  SQUARE,
//  HEXAGON
// };

//=================================================================================
// Constructor
//=================================================================================
AdvancedGUI_DividedDiskDlg::AdvancedGUI_DividedDiskDlg (GeometryGUI* theGeometryGUI, QWidget* parent)
  : GEOMBase_Skeleton(theGeometryGUI, parent, false)
{
//   QPixmap imageOp  (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_DIVIDEDDISK_R_RATIO")));
  QPixmap imageOp1 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_DISK_R")));
  QPixmap imageOp2 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_DISK_PNT_VEC_R")));
  QPixmap imageSel (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_SELECT")));

  setWindowTitle(tr("GEOM_DIVIDEDDISK_TITLE"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_DIVIDEDDISK"));
  mainFrame()->RadioButton1->setIcon(imageOp1);
  mainFrame()->RadioButton2->setIcon(imageOp2);
//   mainFrame()->RadioButton2->setAttribute(Qt::WA_DeleteOnClose);
//   mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton3->close();

  GroupParams      = new DlgRef_1Spin(centralWidget());
  GroupParams->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  GroupParams->TextLabel1->setText(tr("GEOM_RADIUS"));
  
  GroupOrientation = new DlgRef_3Radio(centralWidget());
  GroupOrientation->GroupBox1->setTitle(tr("GEOM_ORIENTATION"));
  GroupOrientation->RadioButton1->setText(tr("GEOM_WPLANE_OXY"));
  GroupOrientation->RadioButton2->setText(tr("GEOM_WPLANE_OYZ"));
  GroupOrientation->RadioButton3->setText(tr("GEOM_WPLANE_OZX"));
  
  GroupPattern = new DlgRef_3Radio(centralWidget());
  GroupPattern->GroupBox1->setTitle(tr("GEOM_PATTERN"));
  GroupPattern->RadioButton1->setText(tr("GEOM_SQUARE"));
  GroupPattern->RadioButton2->setText(tr("GEOM_HEXAGON"));
  GroupPattern->RadioButton3->setAttribute(Qt::WA_DeleteOnClose);
  GroupPattern->RadioButton3->close();
  
  GroupPntVecR = new DlgRef_2Sel1Spin(centralWidget());
  GroupPntVecR->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  GroupPntVecR->TextLabel1->setText(tr("GEOM_CENTER_POINT"));
  GroupPntVecR->TextLabel2->setText(tr("GEOM_VECTOR"));
  GroupPntVecR->TextLabel3->setText(tr("GEOM_RADIUS"));
  GroupPntVecR->PushButton1->setIcon(imageSel);
  GroupPntVecR->PushButton2->setIcon(imageSel);
  GroupPntVecR->LineEdit1->setReadOnly(true);
  GroupPntVecR->LineEdit2->setReadOnly(true);
  //@@ setup dialog box layout here @@//

  QVBoxLayout* layout = new QVBoxLayout(centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
  layout->addWidget(GroupParams);
  layout->addWidget(GroupOrientation);
  layout->addWidget(GroupPntVecR);
  layout->addWidget(GroupPattern);
  /***************************************************************/

  setHelpFileName("create_divideddisk_page.html");

  Init();
}

//=================================================================================
// Destructor
//=================================================================================
AdvancedGUI_DividedDiskDlg::~AdvancedGUI_DividedDiskDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void AdvancedGUI_DividedDiskDlg::Init()
{
  // Get setting of step value from file configuration
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  double step = resMgr->doubleValue("Geometry", "SettingsGeomStep", 100);

  // min, max, step and decimals for spin boxes & initial values
  initSpinBox(GroupParams->SpinBox_DX, 0.00001, COORD_MAX, step, "length_precision" );
  initSpinBox(GroupPntVecR->SpinBox_DX, 0.00001, COORD_MAX, step, "length_precision" );
  GroupParams ->SpinBox_DX->setValue(100);
  GroupPntVecR->SpinBox_DX->setValue(100);
  
  GroupOrientation->RadioButton1->setChecked(true);
  myOrientation = 1;
  
  GroupPattern->RadioButton1->setChecked(true);
  myPattern = GEOM::SQUARE;

  // Signal/slot connections
  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));
  connect(myGeomGUI,     SIGNAL(SignalDefaultStepValueChanged(double)),
          this,          SLOT(SetDoubleSpinBoxStep(double)));
  
  connect(this,                           SIGNAL(constructorsClicked(int)), this, SLOT(ConstructorsClicked(int)));

  connect(GroupPntVecR->PushButton1,      SIGNAL(clicked()),                this, SLOT(SetEditCurrentArgument()));
  connect(GroupPntVecR->PushButton2,      SIGNAL(clicked()),                this, SLOT(SetEditCurrentArgument()));

  connect(GroupParams->SpinBox_DX,        SIGNAL(valueChanged(double)),     this, SLOT(ValueChangedInSpinBox()));  
  connect(GroupPntVecR->SpinBox_DX,       SIGNAL(valueChanged(double)),     this, SLOT(ValueChangedInSpinBox()));
  
  connect(GroupOrientation->RadioButton1, SIGNAL(clicked()),                this, SLOT(RadioButtonClicked()));
  connect(GroupOrientation->RadioButton2, SIGNAL(clicked()),                this, SLOT(RadioButtonClicked()));
  connect(GroupOrientation->RadioButton3, SIGNAL(clicked()),                this, SLOT(RadioButtonClicked()));
  
  connect(GroupPattern->RadioButton1,     SIGNAL(clicked()),                this, SLOT(RadioButtonClicked()));
  connect(GroupPattern->RadioButton2,     SIGNAL(clicked()),                this, SLOT(RadioButtonClicked()));

  initName(tr("GEOM_DIVIDEDDISK"));
  
  ConstructorsClicked(0);
}

//=================================================================================
// function : SetDoubleSpinBoxStep()
// purpose  : Double spin box management
//=================================================================================
void AdvancedGUI_DividedDiskDlg::SetDoubleSpinBoxStep (double step)
{
  //@@ set double spin box step for all spin boxes here @@//
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void AdvancedGUI_DividedDiskDlg::ConstructorsClicked (int constructorId)
{
  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);

  switch (constructorId) {
  case 0:
    {
      GroupPntVecR->hide();
      GroupParams->show();
      GroupOrientation->show();

      disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
      globalSelection(); // close local contexts, if any
      break;
    }
  case 1:
    {
      GroupParams->hide();
      GroupOrientation->hide();
      GroupPntVecR->show();

      GroupPntVecR->PushButton1->click();
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
// function : RadioButtonClicked()
// purpose  : Radio button management
//=================================================================================
void AdvancedGUI_DividedDiskDlg::RadioButtonClicked()
{ 
  QRadioButton* send = (QRadioButton*)sender();
  
  // Orientation
  if (send == GroupOrientation->RadioButton1)
    myOrientation = 1;
  else if ( send == GroupOrientation->RadioButton2)
    myOrientation = 2;
  else if ( send == GroupOrientation->RadioButton3)
    myOrientation = 3;
  
  // Division pattern
  else if (send == GroupPattern->RadioButton1)
    myPattern = GEOM::SQUARE;
  else if (send == GroupPattern->RadioButton2)
    myPattern = GEOM::HEXAGON;
  
  displayPreview(true);
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void AdvancedGUI_DividedDiskDlg::ClickOnOk()
{
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool AdvancedGUI_DividedDiskDlg::ClickOnApply()
{
  if (!onAccept())
    return false;

  initName();

  return true;
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection is changed or on dialog initialization or activation
//=================================================================================
void AdvancedGUI_DividedDiskDlg::SelectionIntoArgument()
{
  if (getConstructorId() == 0)
    return;

  erasePreview();
  myEditCurrentArgument->setText("");

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  if (aSelList.Extent() != 1) {
    if      (myEditCurrentArgument == GroupPntVecR->LineEdit1) myPoint.nullify();
    else if (myEditCurrentArgument == GroupPntVecR->LineEdit2) myDir.nullify();
    return;
  }

  TopAbs_ShapeEnum aNeedType = myEditCurrentArgument == GroupPntVecR->LineEdit2 ? TopAbs_EDGE : TopAbs_VERTEX;
  GEOM::GeomObjPtr aSelectedObject = getSelected( aNeedType );
  TopoDS_Shape aShape;
  if ( aSelectedObject && GEOMBase::GetShape( aSelectedObject.get(), aShape ) && !aShape.IsNull() ){
    QString aName = GEOMBase::GetName( aSelectedObject.get() );

    myEditCurrentArgument->setText(aName);
    
    // clear selection
    disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
    myGeomGUI->getApp()->selectionMgr()->clearSelected();
    connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
        this, SLOT(SelectionIntoArgument()));
    
    if (myEditCurrentArgument == GroupPntVecR->LineEdit1) {
      myPoint = aSelectedObject;
      if (myPoint && !myDir)
    GroupPntVecR->PushButton2->click();
    }
    else if (myEditCurrentArgument == GroupPntVecR->LineEdit2) {
      myDir = aSelectedObject;
      if (myDir && !myPoint)
    GroupPntVecR->PushButton1->click();
    }
  }
  displayPreview(true);
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void AdvancedGUI_DividedDiskDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  if (send == GroupPntVecR->PushButton1) {
    myEditCurrentArgument = GroupPntVecR->LineEdit1;

    GroupPntVecR->PushButton2->setDown(false);
    GroupPntVecR->LineEdit2->setEnabled(false);
  }
  else if (send == GroupPntVecR->PushButton2) {
    myEditCurrentArgument = GroupPntVecR->LineEdit2;

    GroupPntVecR->PushButton1->setDown(false);
    GroupPntVecR->LineEdit1->setEnabled(false);
  }

  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
  if (myEditCurrentArgument == GroupPntVecR->LineEdit2) {
    globalSelection(); // close local contexts, if any
    localSelection(TopAbs_EDGE);
  }
  else {
    globalSelection(); // close local contexts, if any
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
  displayPreview(true);
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void AdvancedGUI_DividedDiskDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
           this, SLOT( SelectionIntoArgument() ) );
  
  ConstructorsClicked( getConstructorId() );
}

//=================================================================================
// function : enterEvent [REDEFINED]
// purpose  :
//=================================================================================
void AdvancedGUI_DividedDiskDlg::enterEvent (QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void AdvancedGUI_DividedDiskDlg::ValueChangedInSpinBox()
{
  displayPreview(true);
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr AdvancedGUI_DividedDiskDlg::createOperation()
{
  return getGeomEngine()->GetPluginOperations(getStudyId(), "AdvancedEngine");
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool AdvancedGUI_DividedDiskDlg::isValid (QString& msg)
{
  bool ok = true;

  //@@ add custom validation actions here @@//

  return ok;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool AdvancedGUI_DividedDiskDlg::execute (ObjectList& objects)
{
  bool res = false;

  GEOM::GEOM_Object_var anObj;

  GEOM::IAdvancedOperations_var anOper = GEOM::IAdvancedOperations::_narrow(getOperation());

  CORBA::Double theRatio = 67; // About  2/3  
  CORBA::Double theR = 0;
  
  switch (getConstructorId()) {
  case 0:
    theR = GroupParams->SpinBox_DX->value(); // init parameter value from dialog box ;

    // call engine function
    anObj = anOper->MakeDividedDisk(theR, theRatio, myOrientation, myPattern);
    res = !anObj->_is_nil();
    if (res && !IsPreview())
    {
      QStringList aParameters;
      aParameters << GroupParams->SpinBox_DX->text();
      if ( aParameters.count() > 0 ) anObj->SetParameters(aParameters.join(":").toLatin1().constData());
    }
    break;
  case 1:
    theR = GroupPntVecR->SpinBox_DX->value(); 
    
    // call engine function
    anObj = anOper->MakeDividedDiskPntVecR(myPoint.get(), myDir.get(), theR, theRatio, myPattern);
    res = !anObj->_is_nil();
    if (res && !IsPreview())
    {
      QStringList aParameters;
      aParameters << GroupPntVecR->SpinBox_DX->text();
      if ( aParameters.count() > 0 ) anObj->SetParameters(aParameters.join(":").toLatin1().constData());
    }
    break;
  }
    
  
  if (res)
    objects.push_back(anObj._retn());

  return res;
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> AdvancedGUI_DividedDiskDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res;
  res << myPoint << myDir;
  return res;
}
