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
// File   : OperationGUI_FilletDlg.cxx
// Author : Damien COQUERET, Open CASCADE S.A.S.
//
#include "OperationGUI_FilletDlg.h"

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
// class    : OperationGUI_FilletDlg()
// purpose  : Constructs a OperationGUI_FilletDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
OperationGUI_FilletDlg::OperationGUI_FilletDlg (GeometryGUI* theGeometryGUI, QWidget* parent)
  : GEOMBase_Skeleton(theGeometryGUI, parent, false),
    myInitial(true)
{
  SUIT_ResourceMgr* aResMgr = myGeomGUI->getApp()->resourceMgr();
  QPixmap image0 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_FILLET_ALL")));
  QPixmap image1 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_FILLET_EDGE")));
  QPixmap image2 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_FILLET_FACE")));

  QPixmap iconSelect (aResMgr->loadPixmap("GEOM", tr("ICON_SELECT")));

  setWindowTitle(tr("GEOM_FILLET_TITLE"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_FILLET"));
  mainFrame()->RadioButton1->setIcon(image0);
  mainFrame()->RadioButton2->setIcon(image1);
  mainFrame()->RadioButton3->setIcon(image2);

  Group1 = new DlgRef_1Sel1Spin(centralWidget());
  Group1->GroupBox1->setTitle(tr("GEOM_FILLET_ALL"));
  Group1->TextLabel1->setText(tr("GEOM_MAIN_OBJECT"));
  Group1->TextLabel2->setText(tr("GEOM_RADIUS"));
  Group1->PushButton1->setIcon(iconSelect);
  Group1->LineEdit1->setReadOnly(true);

  Group2 = new DlgRef_2Sel3Spin2Rb(centralWidget());
  Group2->GroupBox1->setTitle(tr("GEOM_FILLET_EDGES"));
  Group2->TextLabel1->setText(tr("GEOM_MAIN_OBJECT"));
  Group2->TextLabel2->setText(tr("SELECTED_EDGES"));
  Group2->TextLabel3->setText(tr("GEOM_RADIUS"));
  Group2->TextLabel4->setText(tr("GEOM_R1"));
  Group2->TextLabel5->setText(tr("GEOM_R2"));
  Group2->PushButton1->setIcon(iconSelect);
  Group2->PushButton2->setIcon(iconSelect);
  Group2->LineEdit1->setReadOnly(true);
  Group2->LineEdit2->setReadOnly(true);

  Group3 = new DlgRef_2Sel3Spin2Rb(centralWidget());
  Group3->GroupBox1->setTitle(tr("GEOM_FILLET_FACES"));
  Group3->TextLabel1->setText(tr("GEOM_MAIN_OBJECT"));
  Group3->TextLabel2->setText(tr("SELECTED_FACES"));
  Group3->TextLabel3->setText(tr("GEOM_RADIUS"));
  Group3->TextLabel4->setText(tr("GEOM_R1"));
  Group3->TextLabel5->setText(tr("GEOM_R2"));
  Group3->PushButton1->setIcon(iconSelect);
  Group3->PushButton2->setIcon(iconSelect);
  Group3->LineEdit1->setReadOnly(true);
  Group3->LineEdit2->setReadOnly(true);

  QVBoxLayout* layout = new QVBoxLayout(centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
  layout->addWidget(Group1);
  layout->addWidget(Group2);
  layout->addWidget(Group3);
  /***************************************************************/

  // Set range of spinboxes
  double SpecificStep = 10.0;
  initSpinBox(Group1->SpinBox_DX, 0.001, COORD_MAX, SpecificStep, "length_precision" );
  initSpinBox(Group2->SpinBox_DX, 0.001, COORD_MAX, SpecificStep, "length_precision" );
  initSpinBox(Group2->SpinBox_DY, 0.001, COORD_MAX, SpecificStep, "length_precision" );
  initSpinBox(Group2->SpinBox_DZ, 0.001, COORD_MAX, SpecificStep, "length_precision" );
  initSpinBox(Group3->SpinBox_DX, 0.001, COORD_MAX, SpecificStep, "length_precision" );
  initSpinBox(Group3->SpinBox_DY, 0.001, COORD_MAX, SpecificStep, "length_precision" );
  initSpinBox(Group3->SpinBox_DZ, 0.001, COORD_MAX, SpecificStep, "length_precision" );

  setHelpFileName("fillet_operation_page.html");

  // Initialisation
  Init();
}

//=================================================================================
// function : ~OperationGUI_FilletDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
OperationGUI_FilletDlg::~OperationGUI_FilletDlg()
{
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void OperationGUI_FilletDlg::Init()
{
  // Set Initial values of spinboxes
  Group1->SpinBox_DX->setValue(5);
  Group2->SpinBox_DX->setValue(5);
  Group2->SpinBox_DY->setValue(5);
  Group2->SpinBox_DZ->setValue(5);
  Group3->SpinBox_DX->setValue(5);
  Group3->SpinBox_DY->setValue(5);
  Group3->SpinBox_DZ->setValue(5);

  // Clear line edits
  Group1->LineEdit1->setText("");
  Group2->LineEdit1->setText("");
  Group2->LineEdit2->setText("");
  Group3->LineEdit1->setText("");
  Group3->LineEdit2->setText("");

  myShape = GEOM::GEOM_Object::_nil();

  myFaces.Clear();
  myEdges.Clear();

  showOnlyPreviewControl();

  // signals and slots connections

  // main buttons
  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()   ));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  // group box
  connect(this,          SIGNAL(constructorsClicked(int)), this, SLOT(ConstructorsClicked(int)));

  // push buttons
  connect(Group1->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(Group2->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(Group3->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(Group2->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(Group3->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  // line edits
  connect(Group1->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(Group2->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(Group3->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  // spin boxes
  connect(Group1->SpinBox_DX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(Group2->SpinBox_DX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(Group2->SpinBox_DY, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(Group2->SpinBox_DZ, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(Group3->SpinBox_DX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(Group3->SpinBox_DY, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(Group3->SpinBox_DZ, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));

  // radio buttons
  connect(Group2->RadioButton1, SIGNAL(clicked()), this, SLOT(RadioButtonClicked()));
  connect(Group2->RadioButton2, SIGNAL(clicked()), this, SLOT(RadioButtonClicked()));
  connect(Group3->RadioButton1, SIGNAL(clicked()), this, SLOT(RadioButtonClicked()));
  connect(Group3->RadioButton2, SIGNAL(clicked()), this, SLOT(RadioButtonClicked()));

  initName(tr("GEOM_FILLET"));

  myConstructorId = -1;
  ConstructorsClicked(0);
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void OperationGUI_FilletDlg::ConstructorsClicked (int constructorId)
{
  if (myGeomGUI->getApp()->desktop()->activeWindow()->getViewManager()->getType()
      != OCCViewer_Viewer::Type()) {
    mainFrame()->RadioButton1->setChecked(true);
    return;
  }

  if (myConstructorId == constructorId)
    return;

  // Get radius from previous widget
  QString R = "5", R1 = "5", R2 = "5";
  if      (myConstructorId == 0) {
    R = Group1->SpinBox_DX->text();
  }
  else if (myConstructorId == 1) {
    R = Group2->SpinBox_DX->text();
    R1 = Group2->SpinBox_DY->text();
    R2 = Group2->SpinBox_DZ->text();
  }
  else {
    R = Group3->SpinBox_DX->text();
    R1 = Group3->SpinBox_DY->text();
    R2 = Group3->SpinBox_DZ->text();
  }

  myConstructorId = constructorId;

  switch (constructorId) {
  case 0:
    Group2->hide();
    Group3->hide();
    Group1->show();
    Group1->SpinBox_DX->setText(R);
    break;
  case 1:
    Group1->hide();
    Group3->hide();
    Group2->show();
    Group2->SpinBox_DX->setText(R);
    Group2->SpinBox_DY->setText(R1);
    Group2->SpinBox_DZ->setText(R2);
    break;
  case 2:
    Group1->hide();
    Group2->hide();
    Group3->show();
    Group3->SpinBox_DX->setText(R);
    Group3->SpinBox_DY->setText(R1);
    Group3->SpinBox_DZ->setText(R2);
    break;
  default:
    break;
  }

  if      (constructorId == 0) Group1->PushButton1->click();
  else if (constructorId == 1) Group2->PushButton1->click();
  else                         Group3->PushButton1->click();

  enableWidgets();

  if (myInitial) {
    myInitial = false;
    SelectionIntoArgument();
  }
  else {
    if (!myShape->_is_nil()) {
      myEditCurrentArgument->setText(GEOMBase::GetName(myShape));
      GEOMBase_Skeleton::LineEditReturnPressed();
      switch (getConstructorId()) {
      case 1:
        if (myEdges.Extent() == 0)
          Group2->PushButton2->click();
        break;
      case 2:
        if (myFaces.Extent() == 0)
          Group3->PushButton2->click();
        break;
      default:
        break;
      }
    }
    else
      myEditCurrentArgument->setText("");

    processPreview();
  }

  qApp->processEvents();
  updateGeometry();
  resize(minimumSizeHint());
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void OperationGUI_FilletDlg::ClickOnOk()
{
  setIsApplyAndClose( true );
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool OperationGUI_FilletDlg::ClickOnApply()
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
void OperationGUI_FilletDlg::SelectionIntoArgument()
{
  erasePreview();
  myEditCurrentArgument->setText("");

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  // If selection of main object is activated
  if (myEditCurrentArgument == Group1->LineEdit1 ||
      myEditCurrentArgument == Group2->LineEdit1 ||
      myEditCurrentArgument == Group3->LineEdit1)
  {
    myShape = GEOM::GEOM_Object::_nil();
    if (aSelList.Extent() == 1) {
      GEOM::GEOM_Object_var anObj =
        GEOMBase::ConvertIOinGEOMObject( aSelList.First() );

      if ( !anObj->_is_nil() ) {
        myShape = anObj;
        myEditCurrentArgument->setText(GEOMBase::GetName(anObj));
        processPreview();
      }
    }

    enableWidgets();
  }
  // If face or edge selection is activated
  else if (myEditCurrentArgument == Group2->LineEdit2 ||
           myEditCurrentArgument == Group3->LineEdit2)
  {
    if (myEditCurrentArgument == Group2->LineEdit2) myEdges.Clear();
    else myFaces.Clear();
    if (aSelList.Extent() == 1) {
      GEOM::GEOM_Object_var anObj =
        GEOMBase::ConvertIOinGEOMObject( aSelList.First() );

      if ( !anObj->_is_nil() ) {
        TColStd_IndexedMapOfInteger anIndexes;
        aSelMgr->GetIndexes(aSelList.First(), anIndexes);

        if (anIndexes.Extent() > 0) {
          QString aName;
          if (anIndexes.Extent() == 1) {
            int anIndex = anIndexes(1);

            aName = QString(GEOMBase::GetName(anObj)) + QString(":%1").arg(anIndex);
          }
          else {
            aName = tr("GEOM_MEN_POPUP_NAME").arg(anIndexes.Extent());
          }

          myEditCurrentArgument->setText(aName);

          if (myConstructorId == 1)
            myEdges = anIndexes;
          else
            myFaces = anIndexes;

          processPreview();
        }
      }
    }
  }

  // clear selection of the faces or edges
  if (!(myEditCurrentArgument == Group2->LineEdit2 ||
        myEditCurrentArgument == Group3->LineEdit2)) {
    disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
    myGeomGUI->getApp()->selectionMgr()->clearSelected();
    connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
            this, SLOT(SelectionIntoArgument()));
  }

  switch (getConstructorId()) {
  case 1:
    if (myEditCurrentArgument == Group2->LineEdit1) {
      if (!myShape->_is_nil() && myEdges.Extent() == 0)
        Group2->PushButton2->click();
    }
    break;
  case 2:
    if (myEditCurrentArgument == Group3->LineEdit1) {
      if (!myShape->_is_nil() && myFaces.Extent() == 0)
        Group3->PushButton2->click();
    }
    break;
  default:
    break;
  }

  //rnv: To fix the bug IPAL22041 TC5.1.5: "Fillet Construcion" dialog loses current selection.
  // Restore selection of the main shape, if need,
  // because it was canceled.
  aSelMgr->selectedObjects(aSelList);
  if (aSelList.Extent() == 0 && !myShape->_is_nil()) {
    disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
    ObjectList list;
        list.push_back(myShape);
    selectObjects(list);
    connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
            this, SLOT(SelectionIntoArgument()));
  }
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void OperationGUI_FilletDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  if (send == Group1->PushButton1) {
    myEditCurrentArgument = Group1->LineEdit1;
  }
  else if (send == Group2->PushButton1) {
    myEditCurrentArgument = Group2->LineEdit1;
    Group2->PushButton2->setDown(false);
    Group2->LineEdit2->setEnabled(false);
  }
  else if (send == Group2->PushButton2) {
    myEditCurrentArgument = Group2->LineEdit2;
    Group2->PushButton1->setDown(false);
    Group2->LineEdit1->setEnabled(false);
  }
  else if (send == Group3->PushButton1) {
    myEditCurrentArgument = Group3->LineEdit1;
    Group3->PushButton2->setDown(false);
    Group3->LineEdit2->setEnabled(false);
  }
  else if (send == Group3->PushButton2) {
    myEditCurrentArgument = Group3->LineEdit2;
    Group3->PushButton1->setDown(false);
    Group3->LineEdit1->setEnabled(false);
  }

  // enable line edit
  myEditCurrentArgument->setEnabled(true);
  myEditCurrentArgument->setFocus();
  // after setFocus(), because it will be setDown(false) when loses focus
  send->setDown(true);

  activateSelection();

  // seems we need it only to avoid preview disappearing, caused by selection mode change
  processPreview();
}

//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void OperationGUI_FilletDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();

  if (send == Group1->LineEdit1)
    myEditCurrentArgument = Group1->LineEdit1;
  else if (send == Group2->LineEdit1)
    myEditCurrentArgument = Group2->LineEdit1;
  else if (send == Group3->LineEdit1)
    myEditCurrentArgument = Group3->LineEdit1;
  else
    return;

  GEOMBase_Skeleton::LineEditReturnPressed();
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void OperationGUI_FilletDlg::ActivateThisDialog()
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
void OperationGUI_FilletDlg::enterEvent (QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    this->ActivateThisDialog();
}

//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void OperationGUI_FilletDlg::ValueChangedInSpinBox (double)
{
  processPreview();
}

//=================================================================================
// function : activateSelection
// purpose  : Activate selection in accordance with myEditCurrentArgument
//=================================================================================
void OperationGUI_FilletDlg::activateSelection()
{
  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);

  if (!myShape->_is_nil() && myEditCurrentArgument == Group2->LineEdit2)
    localSelection(myShape, TopAbs_EDGE);
  else if (!myShape->_is_nil() && myEditCurrentArgument == Group3->LineEdit2)
    localSelection(myShape, TopAbs_FACE);
  else {
    TColStd_MapOfInteger aMap;
    aMap.Add(GEOM_SHELL);
    aMap.Add(GEOM_SOLID);
    aMap.Add(GEOM_COMPOUND);
    globalSelection(aMap);
  }

  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));
}

//=================================================================================
// function : enableWidgets
// purpose  : Enable widgets of faces in accordance with value of main object
//=================================================================================
void OperationGUI_FilletDlg::enableWidgets()
{
  int anId = getConstructorId();

  bool toEnable = !myShape->_is_nil();

  if (anId == 1) {
    //Group2->LineEdit2->setEnabled(toEnable);
    Group2->PushButton2->setEnabled(toEnable);

    if (!toEnable) {
      Group2->LineEdit2->setText("");
      myEdges.Clear();
    }
  }
  else if (anId == 2) {
    //Group3->LineEdit2->setEnabled(toEnable);
    Group3->PushButton2->setEnabled(toEnable);

    if (!toEnable) {
      Group3->LineEdit2->setText("");
      myFaces.Clear();
    }
  }
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr OperationGUI_FilletDlg::createOperation()
{
  return getGeomEngine()->GetILocalOperations(getStudyId());
}

//=================================================================================
// function : isValid()
// purpose  : Verify validity of input data
//=================================================================================
bool OperationGUI_FilletDlg::isValid (QString& msg)
{
  bool ok = true;
  switch (getConstructorId())
  {
    case 0:
      ok = Group1->SpinBox_DX->isValid( msg, !IsPreview() ) && ok;
      return !myShape->_is_nil() && ok;
    case 1:
      if (Group2->RadioButton1->isChecked())
        ok = Group2->SpinBox_DX->isValid( msg, !IsPreview() );
      else
      {
        ok = Group2->SpinBox_DY->isValid( msg, !IsPreview() ) && ok;
        ok = Group2->SpinBox_DZ->isValid( msg, !IsPreview() ) && ok;
      }
      return !myShape->_is_nil() && myEdges.Extent() > 0 && ok;
    case 2:
      if (Group3->RadioButton1->isChecked())
        ok = Group3->SpinBox_DX->isValid( msg, !IsPreview() );
      else
      {
        ok = Group3->SpinBox_DY->isValid( msg, !IsPreview() ) && ok;
        ok = Group3->SpinBox_DZ->isValid( msg, !IsPreview() ) && ok;
      }
      return !myShape->_is_nil() && myFaces.Extent() > 0 && ok;
    default: return false;
  }
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool OperationGUI_FilletDlg::execute (ObjectList& objects)
{
  QStringList aParameters;
  GEOM::GEOM_Object_var anObj;

  int anId = getConstructorId();

  GEOM::GEOM_ILocalOperations_var anOper = GEOM::GEOM_ILocalOperations::_narrow(getOperation());

  if (anId == 0) {
    anObj = anOper->MakeFilletAll(myShape, getRadius());
    if (!anObj->_is_nil())
      aParameters << Group1->SpinBox_DX->text();
  }
  else if (anId == 1) {
    GEOM::ListOfLong_var aList = new GEOM::ListOfLong;
    aList->length(myEdges.Extent());

    for (int i = 1, n = myEdges.Extent(); i <= n; i++)
      aList[ i - 1 ] = myEdges(i);

    if (Group2->RadioButton1->isChecked())
    {
      anObj = anOper->MakeFilletEdges(myShape, getRadius(), aList);
      if (!anObj->_is_nil())
        aParameters << Group2->SpinBox_DX->text();
    }
    else
    {
      anObj = anOper->MakeFilletEdgesR1R2(myShape,
                                          Group2->SpinBox_DY->value(),
                                          Group2->SpinBox_DZ->value(),
                                          aList);
      if (!anObj->_is_nil())
      {
        aParameters << Group2->SpinBox_DY->text();
        aParameters << Group2->SpinBox_DZ->text();
      }
    }
  }
  else if (anId == 2) {
    GEOM::ListOfLong_var aList = new GEOM::ListOfLong;
    aList->length(myFaces.Extent());

    for (int i = 1, n = myFaces.Extent(); i <= n; i++)
      aList[ i - 1 ] = myFaces(i);

    if (Group3->RadioButton1->isChecked()) {
      anObj = anOper->MakeFilletFaces(myShape, getRadius(), aList);
      if (!anObj->_is_nil())
        aParameters << Group3->SpinBox_DX->text();
    }
    else {
      anObj = anOper->MakeFilletFacesR1R2(myShape,
                                          Group3->SpinBox_DY->value(),
                                          Group3->SpinBox_DZ->value(), aList);
      if (!anObj->_is_nil())
      {
        aParameters << Group3->SpinBox_DY->text();
        aParameters << Group3->SpinBox_DZ->text();
      }
    }
  }

  if (!anObj->_is_nil())
  {
    if (!IsPreview())
      anObj->SetParameters(aParameters.join(":").toLatin1().constData());
    objects.push_back(anObj._retn());
  }

  return true;
}

//=================================================================================
// function : getRadius
// purpose  : Get radius
//=================================================================================
double OperationGUI_FilletDlg::getRadius() const
{
  int anId = getConstructorId();
  if      (anId == 0) return Group1->SpinBox_DX->value();
  else if (anId == 1) return Group2->SpinBox_DX->value();
  else                return Group3->SpinBox_DX->value();
}

//=================================================================================
// function : RadiobuttonClicked
// purpose  :
//=================================================================================
void OperationGUI_FilletDlg::RadioButtonClicked()
{
  const QObject* s = sender();

  bool flag = s == Group2->RadioButton1 || s == Group3->RadioButton1;

  Group2->RadioButton1->blockSignals(true);
  Group2->RadioButton2->blockSignals(true);
  Group3->RadioButton1->blockSignals(true);
  Group3->RadioButton2->blockSignals(true);

  Group2->SpinBox_DX->setEnabled(flag);
  Group2->SpinBox_DY->setEnabled(!flag);
  Group2->SpinBox_DZ->setEnabled(!flag);
  Group2->RadioButton1->setChecked(flag);
  Group2->RadioButton2->setChecked(!flag);
  Group3->SpinBox_DX->setEnabled(flag);
  Group3->SpinBox_DY->setEnabled(!flag);
  Group3->SpinBox_DZ->setEnabled(!flag);
  Group3->RadioButton1->setChecked(flag);
  Group3->RadioButton2->setChecked(!flag);

  Group2->RadioButton1->blockSignals(false);
  Group2->RadioButton2->blockSignals(false);
  Group3->RadioButton1->blockSignals(false);
  Group3->RadioButton2->blockSignals(false);

  processPreview();
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> OperationGUI_FilletDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res;
  GEOM::GeomObjPtr aGeomObjPtr(myShape);
  res << aGeomObjPtr;
  return res;
}
