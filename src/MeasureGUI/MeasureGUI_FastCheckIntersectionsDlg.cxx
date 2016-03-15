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
// File   : MeasureGUI_FastCheckIntersectionsDlg.cxx

#include "MeasureGUI_FastCheckIntersectionsDlg.h"
#include "MeasureGUI.h"

#include <SUIT_OverrideCursor.h>
#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_MessageBox.h>
#include <LightApp_SelectionMgr.h>
#include <SalomeApp_Tools.h>
#include <SalomeApp_Application.h>

#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <TColStd_MapOfInteger.hxx>

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>
#include <GEOMImpl_Types.hxx>

#include <QListWidget>

#define TEXTEDIT_FONT_FAMILY "Courier"
#define TEXTEDIT_FONT_SIZE 11
//=================================================================================
// class    : MeasureGUI_FastCheckIntersectionsDlg()
// purpose  : Constructs a MeasureGUI_FastCheckIntersectionsDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            true to construct a modal dialog.
//=================================================================================
MeasureGUI_FastCheckIntersectionsDlg::MeasureGUI_FastCheckIntersectionsDlg (GeometryGUI* GUI, QWidget* parent)
  : GEOMBase_Skeleton (GUI, parent, false)
{
  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
  QPixmap image0 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_FAST_CHECK_INTERSECTIONS")));
  QPixmap image1 (aResMgr->loadPixmap("GEOM", tr("ICON_SELECT")));

  setWindowTitle(tr("GEOM_FAST_CHECK_INTERSECTIONS"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_FAST_CHECK_INTERSECTIONS"));
  mainFrame()->RadioButton1->setIcon(image0);
  mainFrame()->RadioButton2->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton3->close();

  QGroupBox* aGrp        = new QGroupBox(tr("GEOM_FAST_CHECK_OBJ"), centralWidget());
  QLabel*    anObjLbl1   = new QLabel(tr("GEOM_OBJECT_I").arg("1"), aGrp);
  QLabel*    anObjLbl2   = new QLabel(tr("GEOM_OBJECT_I").arg("2"), aGrp);
  QLabel*    aShapeLbl1  = new QLabel(tr("GEOM_FAST_CHECK_INT_SUBSHAPES").arg("1"), aGrp);
  QLabel*    aShapeLbl2  = new QLabel(tr("GEOM_FAST_CHECK_INT_SUBSHAPES").arg("2"), aGrp);
  QLabel*    aDeflectLbl = new QLabel(tr("GEOM_FAST_CHECK_INT_DEFLECT"), aGrp);

  mySelButton1 = new QPushButton(aGrp);
  mySelButton1->setIcon(image1);
  mySelButton1->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  mySelButton2 = new QPushButton(aGrp);
  mySelButton2->setIcon(image1);
  mySelButton2->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

  myEditObjName1 = new QLineEdit(aGrp);
  myEditObjName1->setReadOnly(true);
  myEditObjName2 = new QLineEdit(aGrp);
  myEditObjName2->setReadOnly(true);

  myDetGaps = new QCheckBox(tr( "GEOM_FAST_CHECK_INT_DETECT_GAPS" ));
  myTolerance = new SalomeApp_DoubleSpinBox(aGrp);
  myDeflection = new SalomeApp_DoubleSpinBox(aGrp);
  myShapeList1  = new QListWidget(aGrp);
  myShapeList2 = new QListWidget(aGrp);
  myShapeList1->setSelectionMode(QAbstractItemView::ExtendedSelection);
  myShapeList2->setSelectionMode(QAbstractItemView::ExtendedSelection);

  myComputeButton = new QPushButton(tr("GEOM_FAST_CHECK_INT_COMPUTE"));

  QGridLayout *aGrpLayout = new QGridLayout(aGrp);
  aGrpLayout->setMargin(9);
  aGrpLayout->setSpacing(6);
  aGrpLayout->addWidget(anObjLbl1,       0, 0);
  aGrpLayout->addWidget(mySelButton1,    0, 1);
  aGrpLayout->addWidget(myEditObjName1,  0, 2);
  aGrpLayout->addWidget(anObjLbl2,       1, 0);
  aGrpLayout->addWidget(mySelButton2,    1, 1);
  aGrpLayout->addWidget(myEditObjName2,  1, 2);
  aGrpLayout->addWidget(aDeflectLbl,     2, 0);
  aGrpLayout->addWidget(myDeflection,    2, 1, 1, 2);
  aGrpLayout->addWidget(myDetGaps,       3, 0);
  aGrpLayout->addWidget(myTolerance,     3, 1, 1, 2);
  aGrpLayout->addWidget(myComputeButton, 4, 0, 1, 3);
  
  QGridLayout *aGrpLayout2 = new QGridLayout();
  aGrpLayout->addLayout(aGrpLayout2,     5, 0, 1, 3);
  aGrpLayout2->addWidget(aShapeLbl1,     0, 0);
  aGrpLayout2->addWidget(aShapeLbl2,     0, 1);
  aGrpLayout2->addWidget(myShapeList1,   1, 0);
  aGrpLayout2->addWidget(myShapeList2,   1, 1);

  QVBoxLayout* layout = new QVBoxLayout (centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
  layout->addWidget(aGrp);

  /***************************************************************/

  myHelpFileName = "fast_intersection_page.html";

  // Initialisation 
  Init();
}

//=================================================================================
// function : ~MeasureGUI_FastCheckIntersectionsDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
MeasureGUI_FastCheckIntersectionsDlg::~MeasureGUI_FastCheckIntersectionsDlg()
{
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void MeasureGUI_FastCheckIntersectionsDlg::Init()
{
  myObj1.nullify();
  myObj2.nullify();
  myEditObjName1->setText("");
  myEditObjName2->setText("");
  myEditObjName1->setEnabled(true);
  myEditObjName2->setEnabled(false);

  myDetGaps->setChecked(false);
  initSpinBox(myTolerance, 0, MAX_NUMBER, 1);
  myTolerance->setValue(0);
  myTolerance->setEnabled(false);

  // Obtain deflection from preferences
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  initSpinBox(myDeflection, 1e-3, 1.0, 1e-3);
  myDeflection->setValue(qMax(1e-3, resMgr->doubleValue("Geometry", "deflection_coeff", 1e-3)));

  myEditCurrentArgument = myEditObjName1;

  connect(myGeomGUI,          SIGNAL(SignalDeactivateActiveDialog()),
          this,               SLOT(DeactivateActiveDialog()));
  connect(myGeomGUI,          SIGNAL(SignalCloseAllDialogs()),
          this,               SLOT(ClickOnCancel()));
  connect(buttonOk(),         SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(),      SIGNAL(clicked()), this, SLOT(ClickOnApply()));
  connect(mySelButton1,       SIGNAL(clicked()),
          this,               SLOT(SetEditCurrentArgument()));
  connect(mySelButton2,       SIGNAL(clicked()),
          this,               SLOT(SetEditCurrentArgument()));
  connect(myDetGaps,          SIGNAL(toggled(bool)), this, SLOT(OnGaps(bool))); 
  connect( myTolerance,       SIGNAL(valueChanged(double)), this, SLOT(clear()));
  connect( myDeflection,      SIGNAL(valueChanged(double)), this, SLOT(clear()));
  connect(myShapeList1,       SIGNAL(itemSelectionChanged()),
          SLOT(onSubShapesListSelectionChanged()));
  connect(myShapeList2,       SIGNAL(itemSelectionChanged()),
          SLOT(onSubShapesListSelectionChanged()));
  connect(myComputeButton,    SIGNAL(clicked()), this, SLOT(onCompute()));

  LightApp_SelectionMgr* aSel = myGeomGUI->getApp()->selectionMgr();

  connect(aSel,               SIGNAL(currentSelectionChanged()),
          this,               SLOT(SelectionIntoArgument()));

  initName(tr("GEOM_FAST_INTERSECTION_NAME"));
  buttonOk()->setEnabled(false);
  buttonApply()->setEnabled(false);
  myComputeButton->setEnabled(false);
  activateSelection();

  mySelButton1->click();
  SelectionIntoArgument();
}

//=================================================================================
// function : clear
// purpose  :
//=================================================================================
void MeasureGUI_FastCheckIntersectionsDlg::clear()
{
  disconnect(myShapeList1, SIGNAL(itemSelectionChanged()), this, 0);
  disconnect(myShapeList2, SIGNAL(itemSelectionChanged()), this, 0);
  myShapeList1->clear();
  myShapeList2->clear();
  connect(myShapeList1,    SIGNAL(itemSelectionChanged()),
          SLOT(onSubShapesListSelectionChanged()));
  connect(myShapeList2,    SIGNAL(itemSelectionChanged()),
          SLOT(onSubShapesListSelectionChanged()));
  erasePreview();
  buttonOk()->setEnabled(false);
  buttonApply()->setEnabled(false);
  myComputeButton->setEnabled(true);
}

//=================================================================================
// function : onCompute
// purpose  :
//=================================================================================
void MeasureGUI_FastCheckIntersectionsDlg::onCompute()
{
  SUIT_OverrideCursor wc;

  myShapeList1->clear();
  myShapeList2->clear();

  if (!findIntersections()) {
    myShapeList1->addItem(tr("GEOM_FAST_INTERSECTION_FAILS"));
    myShapeList1->setEnabled(false);
    myShapeList2->addItem(tr("GEOM_FAST_INTERSECTION_FAILS"));
    myShapeList2->setEnabled(false);
    return;
  }
  
  myShapeList1->setEnabled(true);
  myShapeList2->setEnabled(true);
  QStringList aSubShapeList1, aSubShapeList2;
  TopoDS_Shape aSelShape1, aSelShape2;
  if (myObj1 && GEOMBase::GetShape(myObj1.get(), aSelShape1) && myObj2
                     && GEOMBase::GetShape(myObj2.get(), aSelShape2)) {
    TopTools_IndexedMapOfShape anIndices1, anIndices2;
    TopExp::MapShapes(aSelShape1, anIndices1);
    TopExp::MapShapes(aSelShape2, anIndices2);

    //Sub-shapes of 1st Object
    for (int i = 0; i < myInters1->length(); i++) {
      TopoDS_Shape aSubShape = anIndices1.FindKey(myInters1[i]);
      QString aType = GEOMBase::GetShapeTypeString(aSubShape);
      if (!aType.isEmpty())
        aSubShapeList1.append(QString("%1_%2").arg(aType).arg(myInters1[i]));
    }
    myShapeList1->addItems(aSubShapeList1);
    myShapeList1->selectAll();

    //Sub-shapes of second Object
    for (int i = 0; i < myInters2->length(); i++) {
      TopoDS_Shape aSubShape = anIndices2.FindKey(myInters2[i]);
      QString aType = GEOMBase::GetShapeTypeString(aSubShape);
      if (!aType.isEmpty())
        aSubShapeList2.append(QString("%1_%2").arg(aType).arg(myInters2[i]));
    }
    myShapeList2->addItems(aSubShapeList2);
    myShapeList2->selectAll();
  }

  buttonOk()->setEnabled(true);
  buttonApply()->setEnabled(true);
  myComputeButton->setEnabled(false);
}

//=================================================================================
// function : ActivateThisDialog
// purpose  :
//=================================================================================
void MeasureGUI_FastCheckIntersectionsDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();

  LightApp_SelectionMgr* aSel = myGeomGUI->getApp()->selectionMgr();
  if ( aSel )
    connect( aSel, SIGNAL( currentSelectionChanged() ), this, SLOT( SelectionIntoArgument() ) );

  activateSelection();
  DISPLAY_PREVIEW_MACRO
}

//=================================================================================
// function : DeactivateActiveDialog()
// purpose  : public slot to deactivate if active
//=================================================================================
void MeasureGUI_FastCheckIntersectionsDlg::DeactivateActiveDialog()
{
  GEOMBase_Skeleton::DeactivateActiveDialog();
}

//=================================================================================
// function : activateSelection
// purpose  : 
//=================================================================================
void MeasureGUI_FastCheckIntersectionsDlg::activateSelection()
{
  TColStd_MapOfInteger aTypes;
  aTypes.Add(GEOM_COMPOUND );
  aTypes.Add(GEOM_SOLID );
  aTypes.Add(GEOM_SHELL);
  aTypes.Add(GEOM_FACE);
  globalSelection(aTypes);

  std::list<int> needTypes;
  needTypes.push_back( TopAbs_FACE ), needTypes.push_back( TopAbs_SHELL ), needTypes.push_back( TopAbs_SOLID ), needTypes.push_back( TopAbs_COMPOUND );
  localSelection( needTypes );

}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void MeasureGUI_FastCheckIntersectionsDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool MeasureGUI_FastCheckIntersectionsDlg::ClickOnApply()
{
  if ( !onAccept() )
    return false;

  clear();
  initName();
  return true;
}

//=================================================================================
// function : extractPrefix
// purpose  :
//=================================================================================
bool MeasureGUI_FastCheckIntersectionsDlg::extractPrefix() const
{
  return true;
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr MeasureGUI_FastCheckIntersectionsDlg::createOperation()
{
  return getGeomEngine()->GetIMeasureOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool MeasureGUI_FastCheckIntersectionsDlg::isValid( QString& )
{
  return myObj1 && myObj2;
}

//=================================================================================
// function : SetEditCurrentArgument
// purpose  :
//=================================================================================
void MeasureGUI_FastCheckIntersectionsDlg::SetEditCurrentArgument()
{
    QPushButton* send = (QPushButton*)sender();

  if (send == mySelButton1) {
    myEditCurrentArgument = myEditObjName1;

    mySelButton2->setDown(false);
    myEditObjName2->setEnabled(false);
    myObj1.nullify();
  }
  else {
    myEditCurrentArgument = myEditObjName2;

    mySelButton1->setDown(false);
    myEditObjName1->setEnabled(false);
    myObj2.nullify();
  }

  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
  activateSelection();
  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));

  // enable line edit
  myEditCurrentArgument->setEnabled(true);
  myEditCurrentArgument->setFocus();
  // after setFocus(), because it will be setDown(false) when loses focus
  send->setDown(true);

  SelectionIntoArgument();
}

//=================================================================================
// function : OnGaps()
// purpose  :
//=================================================================================
void MeasureGUI_FastCheckIntersectionsDlg::OnGaps(bool cheked)
{
  clear();
  myTolerance->setEnabled(cheked);
}
//=================================================================================
// function : SelectionIntoArgument
// purpose  :
//=================================================================================
void MeasureGUI_FastCheckIntersectionsDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("");
  // Clear the dialog.
  clear();

  QList<TopAbs_ShapeEnum> typesLst;
  typesLst << TopAbs_FACE << TopAbs_SHELL << TopAbs_SOLID << TopAbs_COMPOUND;
  GEOM::GeomObjPtr aSelectedObject = getSelected( typesLst );

  // clear selection
  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
  myGeomGUI->getApp()->selectionMgr()->clearSelected();
  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));

  if (myEditCurrentArgument == myEditObjName1) {
    myObj1 = aSelectedObject;
    if (myObj1 && !myObj2)
      mySelButton2->click();
  }
  else {
    myObj2 = aSelectedObject;
    if (myObj2 && !myObj1)
      mySelButton1->click();
  }
  myEditObjName1->setText(myObj1 ? GEOMBase::GetName(myObj1.get()) : "");
  myEditObjName2->setText(myObj2 ? GEOMBase::GetName(myObj2.get()) : "");
}

//=================================================================================
// function : enterEvent
// purpose  :
//=================================================================================
void MeasureGUI_FastCheckIntersectionsDlg::enterEvent(QEvent *)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : findIntersections
// purpose  :
//=================================================================================
bool MeasureGUI_FastCheckIntersectionsDlg::findIntersections()
{
  if (!myObj1 || !myObj2)
    return false;

  GEOM::GEOM_IMeasureOperations_var anOper =
    GEOM::GEOM_IMeasureOperations::_narrow(getOperation());
  bool isOK = true;

  bool HasInte;
  try {
    HasInte = anOper->FastIntersect(myObj1.get(), myObj2.get(), getTolerance(), getDeflection(), myInters1, myInters2);
  }
  catch (const SALOME::SALOME_Exception& e) {
    SalomeApp_Tools::QtCatchCorbaException(e);
    isOK = false;
  }

  bool isDone = anOper->IsDone();
  if (!isDone) {
    CORBA::String_var aMsg = anOper->GetErrorCode();
    SUIT_MessageBox::warning(this,
                             QObject::tr("WRN_WARNING"),
                             QObject::tr(aMsg.in()));
  }

  if ( !HasInte || myInters1->length() == 0 || myInters2->length() == 0)
    isOK = false;

  return isOK;
}

//=================================================================================
// function : onSubShapesListSelectionChanged
// purpose  :
//=================================================================================
void MeasureGUI_FastCheckIntersectionsDlg::onSubShapesListSelectionChanged()
{
  erasePreview();
  previewSubShapesListSelection(myShapeList1);
  previewSubShapesListSelection(myShapeList2);
}

//=================================================================================
// function : previewSubShapesListSelection
// purpose  :
//=================================================================================
void MeasureGUI_FastCheckIntersectionsDlg::previewSubShapesListSelection(QListWidget* theWidget)
{
  GEOM::ListOfLong_var anInters;
  GEOM::GeomObjPtr anObj;

  if (theWidget == myShapeList1) { // Sub-Shapes of Object 1
    anObj = myObj1;
    anInters = myInters1;
  }
  else {
    if (theWidget == myShapeList2) { // Sub-Shapes of Object 2
      anObj = myObj2;
      anInters = myInters2;
    }
    else return;
  }
  // Selected IDs
  QList<int> aIds;
  for (int i = 0, n = theWidget->count(); i < n; i++) {
    if (theWidget->item(i)->isSelected())
      aIds.append(i);
  }
  if (aIds.count() < 1)
    return;

  TopoDS_Shape aSelShape;
  TopoDS_Shape aSubShape;
  TopTools_IndexedMapOfShape anIndices;
  if (anObj && GEOMBase::GetShape(anObj.get(), aSelShape)) {
    SALOME_Prs* aPrs = 0;
    TopExp::MapShapes(aSelShape, anIndices);
    QList<int>::iterator it;
    getDisplayer()->SetColor(theWidget == myShapeList1 ? Quantity_NOC_RED : Quantity_NOC_GREEN);
    getDisplayer()->SetWidth(3);
    getDisplayer()->SetToActivate(false);
    for (it = aIds.begin(); it != aIds.end(); ++it) {
      aSubShape = anIndices.FindKey(anInters[(*it)]);
      try {
        aPrs = !aSubShape.IsNull() ? getDisplayer()->BuildPrs(aSubShape) : 0;
        if (aPrs)
          displayPreview(aPrs, true);
      }
      catch (const SALOME::SALOME_Exception& e) {
        SalomeApp_Tools::QtCatchCorbaException(e);
      }
    }
  }
}

//=================================================================================
// function : getLstObjFromListSelection
// purpose  :
//=================================================================================
GEOM::ListOfGO_var MeasureGUI_FastCheckIntersectionsDlg::getLstObjFromListSelection(QListWidget* theWidget)
{
  GEOM::ListOfLong_var anInters;
  GEOM::GeomObjPtr anObj;
  GEOM::ListOfGO_var anObjLst = new GEOM::ListOfGO();

  if (theWidget == myShapeList1) { // Sub-Shapes of Object 1
    anObj = myObj1;
    anInters = myInters1;
  }
  else {
    if (theWidget == myShapeList2) { // Sub-Shapes of Object 2
      anObj = myObj2;
      anInters = myInters2;
    }
    else
      return anObjLst;
  }
  //Add in study myObj if local selection
  GEOMBase::PublishSubObject( anObj.get() );
  // Collect the map of indices and already publised objects
  TColStd_IndexedMapOfInteger aMapIndex;
  ObjectList aObjLstExist;
  for (int i = 0, n = theWidget->count(); i < n; i++) {
    if (theWidget->item(i)->isSelected()) {
      GEOM::GEOM_Object_var aTmpObj = GEOMBase_Helper::findObjectInFather(anObj.get(), anInters[i]);
      if (aTmpObj->_is_nil())
        aMapIndex.Add(anInters[i]);
      else
        aObjLstExist.push_back( GEOM::GEOM_Object::_duplicate(aTmpObj));
    }
  }

  if (aMapIndex.IsEmpty() && aObjLstExist.empty())
    return anObjLst;
 
  // Create objects.
  GEOM::ListOfLong_var anArray   = new GEOM::ListOfLong;
  const int            aNbShapes = aMapIndex.Extent();
  anArray->length(aNbShapes);
  for (int i = 1; i <= aNbShapes; i++) {
    anArray[i - 1] = aMapIndex.FindKey(i);
  }

  if (myShapesOper->_is_nil())
    myShapesOper = getGeomEngine()->GetIShapesOperations(getStudyId());

  GEOM::ListOfGO_var aObjLstCreate = myShapesOper->MakeSubShapes(anObj.get(), anArray);

  //Collect all objects in list
  anObjLst->length(aObjLstCreate->length() + aObjLstExist.size());
  
  ObjectList::iterator anIter;
  int i;
  for (i=0, anIter = aObjLstExist.begin(); anIter != aObjLstExist.end(); i++, ++anIter) {
    anObjLst[i] = *anIter;
  }
  for (int j = 0; j < aObjLstCreate->length(); j++) {
    anObjLst[aObjLstExist.size()+j]=aObjLstCreate[j];
  }
  return  anObjLst._retn();
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool MeasureGUI_FastCheckIntersectionsDlg::execute(ObjectList& objects)
{
  GEOM::ListOfGO_var aList1 = getLstObjFromListSelection(myShapeList1);
  GEOM::ListOfGO_var aList2 = getLstObjFromListSelection(myShapeList2);
  GEOM::ListOfGO_var aCompList = new GEOM::ListOfGO();
  const int nbObj = aList1->length() + aList2->length();
  if (nbObj <= 0)
    return true;

  //Collect general intersection list
  aCompList->length(nbObj);
  int i;
  for (i = 0; i < aList1->length(); i++) {
    GEOMBase::PublishSubObject( aList1[i] );
    aCompList[i]=aList1[i];
  }
  for (int j = 0; j < aList2->length(); j++) {
    GEOMBase::PublishSubObject( aList2[j] );
    aCompList[i+j]=aList2[j];
  }

  // make compound
  objects.push_back(myShapesOper->MakeCompound(aCompList));
  return true; 
}

//=================================================================================
// function : getDeflection
// purpose  :
//=================================================================================
float MeasureGUI_FastCheckIntersectionsDlg::getDeflection()
{
  return (float)myDeflection->value();
}

//=================================================================================
// function : getTolerance
// purpose  :
//=================================================================================
double MeasureGUI_FastCheckIntersectionsDlg::getTolerance()
{
  double aVal = myTolerance->value();
  if (!myDetGaps->isChecked() || aVal < 0.0)
    return 0.0;
  return aVal;
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> MeasureGUI_FastCheckIntersectionsDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res;
  res << myObj1 << myObj2;
  return res;
}
