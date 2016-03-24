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
// File   : MeasureGUI_CheckSelfIntersectionsDlg.cxx

#include "MeasureGUI_CheckSelfIntersectionsDlg.h"
#include "MeasureGUI.h"

#include <SUIT_MessageBox.h>
#include <SUIT_OverrideCursor.h>
#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
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
#include <GEOM_GenericObjPtr.h>

#include <QListWidget>

#define TEXTEDIT_FONT_FAMILY "Courier"
#define TEXTEDIT_FONT_SIZE 11

//=================================================================================
// class    : MeasureGUI_CheckSelfIntersectionsDlg()
// purpose  : Constructs a MeasureGUI_CheckSelfIntersectionsDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            true to construct a modal dialog.
//=================================================================================
MeasureGUI_CheckSelfIntersectionsDlg::MeasureGUI_CheckSelfIntersectionsDlg (GeometryGUI* GUI, QWidget* parent)
  : GEOMBase_Skeleton (GUI, parent, false),
    myTextView1       (0),
    myTextView2       (0),
    mySelButton1      (0),
    mySelButton2      (0),
    myEditObjName1    (0),
    myEditObjName2    (0),
    myLevelBox        (0),
    myComputeButton1  (0),
    myComputeButton2  (0),
    myInteList1       (0),
    myShapeList1      (0),
    myInteList2       (0),
    myShapeList2      (0),
    myCurrConstrId    (-1)
{
  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
  QPixmap image0 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_CHECK_SELF_INTERSECTIONS")));
  QPixmap image1 (aResMgr->loadPixmap("GEOM", tr("ICON_SELECT")));
  QPixmap image2 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_FAST_CHECK_INTERSECTIONS")));

  setWindowTitle(tr("GEOM_CHECK_SELF_INTERSECTIONS"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_CHECK_INTERSECT_TYPE"));
  mainFrame()->RadioButton1->setIcon(image0);
  mainFrame()->RadioButton2->setIcon(image2);;
  mainFrame()->RadioButton3->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton3->close();

  /***************************************************************/
  /* SIMPLE SELF-INTERSECTION constructor
  /***************************************************************/
  mySimpleGrp             = new QGroupBox(tr("GEOM_CHECK_INFOS"));
  QLabel    *anObjLbl    = new QLabel(tr("GEOM_OBJECT"));
  QLabel    *anInteLbl   = new QLabel(tr("GEOM_CHECK_INTE_INTERSECTIONS"));
  QLabel    *aShapeLbl   = new QLabel(tr("GEOM_CHECK_INTE_SUBSHAPES"));
  QLabel    *aLevelLbl   = new QLabel(tr("GEOM_CHECK_INTE_CHECK_LEVEL"));
  QLabel    *aSummaryLbl1 = new QLabel(tr("GEOM_CHECK_INTE_SUMMARY"));
  QFont      aFont (TEXTEDIT_FONT_FAMILY, TEXTEDIT_FONT_SIZE);

  aFont.setStyleHint(QFont::TypeWriter, QFont::PreferAntialias);
  myTextView1 = new QTextBrowser;
  myTextView1->setReadOnly(true);
  myTextView1->setFont(aFont);

  mySelButton1 = new QPushButton;
  mySelButton1->setIcon(image1);
  mySelButton1->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

  myEditObjName1 = new QLineEdit;
  myEditObjName1->setReadOnly(true);

  myLevelBox = new QComboBox;

  myComputeButton1 = new QPushButton(tr("GEOM_CHECK_INTE_COMPUTE"));


  myInteList1  = new QListWidget;
  myInteList1->setSelectionMode(QAbstractItemView::ExtendedSelection);
  myShapeList1 = new QListWidget;
  myShapeList1->setSelectionMode(QAbstractItemView::ExtendedSelection);

  QGridLayout *aGrpLayout1 = new QGridLayout(mySimpleGrp);

  aGrpLayout1->setMargin(9);
  aGrpLayout1->setSpacing(6);
  aGrpLayout1->addWidget(anObjLbl,         0, 0);
  aGrpLayout1->addWidget(mySelButton1,     0, 1);
  aGrpLayout1->addWidget(myEditObjName1,   0, 2);
  aGrpLayout1->addWidget(aLevelLbl,        1, 0);
  aGrpLayout1->addWidget(myLevelBox,       1, 1, 1, 2);
  aGrpLayout1->addWidget(myComputeButton1, 2, 0, 1, 3);
  aGrpLayout1->addWidget(aSummaryLbl1,     3, 0);
  aGrpLayout1->addWidget(myTextView1,      4, 0, 1, 3);
  aGrpLayout1->addWidget(anInteLbl,        5, 0);
  aGrpLayout1->addWidget(aShapeLbl,        5, 2);
  aGrpLayout1->addWidget(myInteList1,      6, 0, 1, 2);
  aGrpLayout1->addWidget(myShapeList1,     6, 2);

  /***************************************************************/
  /* FAST SELF-INTERSECTION constructor
  /***************************************************************/
  
  myFastGrp               = new QGroupBox(tr("GEOM_CHECK_INFOS"), centralWidget());
  QLabel    *anObjLbl2    = new QLabel(tr("GEOM_OBJECT"), myFastGrp);
  QLabel    *aDeflectLbl  = new QLabel(tr("GEOM_CHECK_INT_DEFLECT"), myFastGrp);
  QLabel    *aSummaryLbl2 = new QLabel(tr("GEOM_CHECK_INTE_SUMMARY"));
  QLabel    *anInteLbl2   = new QLabel(tr("GEOM_CHECK_INTE_INTERSECTIONS"), myFastGrp);
  QLabel    *aShapeLbl2   = new QLabel(tr("GEOM_CHECK_INTE_SUBSHAPES"), myFastGrp);

  mySelButton2 = new QPushButton(myFastGrp);
  mySelButton2->setIcon(image1);
  mySelButton2->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

  myEditObjName2 = new QLineEdit(myFastGrp);
  myEditObjName2->setReadOnly(true);

  myDeflection = new SalomeApp_DoubleSpinBox(myFastGrp);
  myDetGaps = new QCheckBox(tr( "GEOM_CHECK_INT_DETECT_GAPS" ));
  myTolerance = new SalomeApp_DoubleSpinBox(myFastGrp);

  myComputeButton2 = new QPushButton(tr("GEOM_CHECK_INTE_COMPUTE"));

  myTextView2 = new QTextBrowser;
  myTextView2->setReadOnly(true);
  myTextView2->setFont(aFont);

  myInteList2  = new QListWidget(myFastGrp);
  myInteList2->setSelectionMode(QAbstractItemView::ExtendedSelection);
  myShapeList2 = new QListWidget(myFastGrp);
  myShapeList2->setSelectionMode(QAbstractItemView::ExtendedSelection);

  QGridLayout *aGrpLayout2 = new QGridLayout(myFastGrp);
  aGrpLayout2->setMargin(9);
  aGrpLayout2->setSpacing(6);
  aGrpLayout2->addWidget(anObjLbl2,        0, 0);
  aGrpLayout2->addWidget(mySelButton2,     0, 1);
  aGrpLayout2->addWidget(myEditObjName2,   0, 2);
  aGrpLayout2->addWidget(aDeflectLbl,      1, 0);
  aGrpLayout2->addWidget(myDeflection,     1, 1, 1, 2);
  aGrpLayout2->addWidget(myDetGaps,        2, 0);
  aGrpLayout2->addWidget(myTolerance,      2, 1, 1, 2);
  aGrpLayout2->addWidget(myComputeButton2, 3, 0, 1, 3);
  aGrpLayout2->addWidget(aSummaryLbl2,     4, 0);
  aGrpLayout2->addWidget(myTextView2,      5, 0, 1, 3);
  aGrpLayout2->addWidget(anInteLbl2,       6, 0);
  aGrpLayout2->addWidget(aShapeLbl2,       6, 1, 1, 2);
  aGrpLayout2->addWidget(myInteList2,      7, 0);
  aGrpLayout2->addWidget(myShapeList2,     7, 1, 1, 2);
  
  /***************************************************************/

  QVBoxLayout* layout2 = new QVBoxLayout (centralWidget());
  layout2->setMargin(0); layout2->setSpacing(6);
  layout2->addWidget(mySimpleGrp);
  layout2->addWidget(myFastGrp);

 /***************************************************************/

  myHelpFileName = "check_self_intersections_page.html";

  /* Initialisation */
  Init();
}

//=================================================================================
// function : ~MeasureGUI_CheckSelfIntersectionsDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
MeasureGUI_CheckSelfIntersectionsDlg::~MeasureGUI_CheckSelfIntersectionsDlg()
{
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void MeasureGUI_CheckSelfIntersectionsDlg::Init()
{
  // Fill in the combo box.
  myLevelBox->insertItem(GEOM::SI_V_V, tr("GEOM_CHECK_INTE_V_V"));
  myLevelBox->insertItem(GEOM::SI_V_E, tr("GEOM_CHECK_INTE_V_E"));
  myLevelBox->insertItem(GEOM::SI_E_E, tr("GEOM_CHECK_INTE_E_E"));
  myLevelBox->insertItem(GEOM::SI_V_F, tr("GEOM_CHECK_INTE_V_F"));
  myLevelBox->insertItem(GEOM::SI_E_F, tr("GEOM_CHECK_INTE_E_F"));
  myLevelBox->insertItem(GEOM::SI_ALL, tr("GEOM_CHECK_INTE_ALL"));
  myLevelBox->setCurrentIndex(GEOM::SI_ALL);
  myComputeButton1->setEnabled(false);

  connect(mySelButton1,        SIGNAL(clicked()),
          this,               SLOT(SetEditCurrentArgument()));
  connect(myInteList1,         SIGNAL(itemSelectionChanged()),
          SLOT(onInteListSelectionChanged()));
  connect(myShapeList1,        SIGNAL(itemSelectionChanged()),
          SLOT(onSubShapesListSelectionChanged()));
  connect(myLevelBox,         SIGNAL(currentIndexChanged(int)),
          this,               SLOT(clear()));
  connect(myComputeButton1,    SIGNAL(clicked()), this, SLOT(onCompute()));

  /***************************************************************/
  myObj2.nullify();
  myEditObjName2->setText("");
  myEditObjName2->setEnabled(true);

  myDetGaps->setChecked(false);
  initSpinBox(myTolerance, 0, MAX_NUMBER, 1);
  myTolerance->setValue(0);
  myTolerance->setEnabled(false);
  myComputeButton2->setEnabled(false);

  // Obtain deflection from preferences
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  initSpinBox(myDeflection, 1e-3, 1.0, 1e-3);
  myDeflection->setValue(qMax(1e-3, resMgr->doubleValue("Geometry", "deflection_coeff", 1e-3)));

  connect( mySelButton2,       SIGNAL(clicked()),
          this,               SLOT(SetEditCurrentArgument()));
  connect( myDetGaps,          SIGNAL(toggled(bool)), this, SLOT(OnGaps(bool))); 
  connect( myTolerance,       SIGNAL(valueChanged(double)), this, SLOT(clear()));
  connect( myDeflection,      SIGNAL(valueChanged(double)), this, SLOT(clear()));
  connect( myInteList2,         SIGNAL(itemSelectionChanged()),
          SLOT(onInteListSelectionChanged()));
  connect( myShapeList2,       SIGNAL(itemSelectionChanged()),
          SLOT(onSubShapesListSelectionChanged()));
  connect( myComputeButton2,    SIGNAL(clicked()), this, SLOT(onCompute()));

  /***************************************************************/

  connect(this,               SIGNAL(constructorsClicked(int)), 
	  this,               SLOT(ConstructorsClicked(int)));
  connect(myGeomGUI,          SIGNAL(SignalDeactivateActiveDialog()),
          this,               SLOT(DeactivateActiveDialog()));
  connect(myGeomGUI,          SIGNAL(SignalCloseAllDialogs()),
          this,               SLOT(ClickOnCancel()));
  connect(buttonOk(),         SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(),      SIGNAL(clicked()), this, SLOT(ClickOnApply()));
  LightApp_SelectionMgr* aSel = myGeomGUI->getApp()->selectionMgr();
  connect(aSel,               SIGNAL(currentSelectionChanged()),
          this,               SLOT(SelectionIntoArgument()));

  initName( tr( "GEOM_SELF_INTERSECTION_NAME") );
  buttonOk()->setEnabled(false);
  buttonApply()->setEnabled(false);

  ConstructorsClicked(0);

  activateSelection();
  SelectionIntoArgument();
}

//=================================================================================
// function : clear
// purpose  :
//=================================================================================
void MeasureGUI_CheckSelfIntersectionsDlg::clear()
{
  getTextView()->setText("");
  getComputeButton()->setEnabled(true);

  getInteList()->blockSignals(true);
  getShapeList()->blockSignals(true);
  getInteList()->clear();
  getShapeList()->clear();
  getInteList()->blockSignals(false);
  getShapeList()->blockSignals(false);

  erasePreview();
  buttonOk()->setEnabled(false);
  buttonApply()->setEnabled(false);
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void MeasureGUI_CheckSelfIntersectionsDlg::ConstructorsClicked(int constructorId)
{
  if (myCurrConstrId == constructorId)
    return;

  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);

  switch (constructorId) {
  case 0:
    mySimpleGrp->show();
    myFastGrp->hide();
    break;
  case 1:
    mySimpleGrp->hide();
    myFastGrp->show();
    break;
  }

  myCurrConstrId = constructorId;

  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
           this, SLOT(SelectionIntoArgument()));

  qApp->processEvents();
  updateGeometry();
  resize(minimumSizeHint());

  processPreview();
  //updateButtonState();
  activateSelection();
  SelectionIntoArgument();
}

//=================================================================================
// function : onCompute
// purpose  :
//=================================================================================
void MeasureGUI_CheckSelfIntersectionsDlg::onCompute()
{
  bool    hasSelfInte = false;
  QString anErrMsg("");

  if (!findSelfIntersections(hasSelfInte, anErrMsg)) {
    getTextView()->setText(anErrMsg);
    return;
  }

  // Status and apply buttons
  QString aMsg("");

  if (hasSelfInte) {
    aMsg += tr("GEOM_SELF_INTERSECTIONS_FOUND");
    buttonOk()->setEnabled(true);
    buttonApply()->setEnabled(true);
  } else {
    aMsg += tr("GEOM_NO_SELF_INTERSECTIONS");
  }

  if (!anErrMsg.isEmpty()) {
    aMsg += "\n\n";
    aMsg += anErrMsg;
  }

  getTextView()->setText(aMsg);

  // Pairs
  QStringList anInteList;
  QString anInteStr ("");
  int nbPairs = getInters()->length()/2;

  for (int i = 1; i <= nbPairs; i++) {
    anInteStr = "Intersection # ";
    anInteStr += QString::number(i);
    anInteList.append(anInteStr);
  }

  getInteList()->addItems(anInteList);
  getComputeButton()->setEnabled(false);
}

//=================================================================================
// function : ActivateThisDialog
// purpose  :
//=================================================================================
void MeasureGUI_CheckSelfIntersectionsDlg::ActivateThisDialog()
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
void MeasureGUI_CheckSelfIntersectionsDlg::DeactivateActiveDialog()
{
  GEOMBase_Skeleton::DeactivateActiveDialog();
}

//=================================================================================
// function : activateSelection
// purpose  : 
//=================================================================================
void MeasureGUI_CheckSelfIntersectionsDlg::activateSelection()
{
  switch (getConstructorId()) {
  case 0:
    globalSelection(GEOM_ALLSHAPES);
    break;
  case 1:
    TColStd_MapOfInteger aTypes;
    aTypes.Add(GEOM_COMPOUND );
    aTypes.Add(GEOM_SOLID );
    aTypes.Add(GEOM_SHELL);
    aTypes.Add(GEOM_FACE);
    globalSelection(aTypes);

    std::list<int> needTypes;
    needTypes.push_back( TopAbs_FACE ), needTypes.push_back( TopAbs_SHELL ), needTypes.push_back( TopAbs_SOLID ), needTypes.push_back( TopAbs_COMPOUND );
    localSelection( needTypes );
    break;
  }
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void MeasureGUI_CheckSelfIntersectionsDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool MeasureGUI_CheckSelfIntersectionsDlg::ClickOnApply()
{
  if ( !onAccept() )
    return false;

  clear();
  initName();

  ConstructorsClicked(getConstructorId());

  return true;
}

//=================================================================================
// function : extractPrefix
// purpose  :
//=================================================================================
bool MeasureGUI_CheckSelfIntersectionsDlg::extractPrefix() const
{
  return true;
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr MeasureGUI_CheckSelfIntersectionsDlg::createOperation()
{
  return getGeomEngine()->GetIMeasureOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool MeasureGUI_CheckSelfIntersectionsDlg::isValid( QString& )
{
  return !getObj().isNull();
}

//=================================================================================
// function : SetEditCurrentArgument
// purpose  :
//=================================================================================
void MeasureGUI_CheckSelfIntersectionsDlg::SetEditCurrentArgument()
{
  getEditObjName()->setFocus();
  SelectionIntoArgument();
}

//=================================================================================
// function : OnGaps()
// purpose  :
//=================================================================================
void MeasureGUI_CheckSelfIntersectionsDlg::OnGaps(bool checked)
{
  clear();
  myTolerance->setEnabled(checked);
}
//=================================================================================
// function : SelectionIntoArgument
// purpose  :
//=================================================================================
void MeasureGUI_CheckSelfIntersectionsDlg::SelectionIntoArgument()
{
  QList<TopAbs_ShapeEnum> typesLst;

  if ( getConstructorId() == 0 ) {
    typesLst << TopAbs_COMPOUND
	     << TopAbs_COMPSOLID
	     << TopAbs_SOLID
	     << TopAbs_SHELL
	     << TopAbs_FACE
	     << TopAbs_WIRE
	     << TopAbs_EDGE
	     << TopAbs_VERTEX
	     << TopAbs_SHAPE;
  } else {
    typesLst << TopAbs_FACE
	     << TopAbs_SHELL
	     << TopAbs_SOLID
	     << TopAbs_COMPOUND;
  }

  // Clear the dialog.
  clear();

  GEOM::GeomObjPtr aSelectedObject = getSelected( typesLst );

  (getConstructorId() == 0 ? myObj1 :myObj2) = aSelectedObject;
  getEditObjName()->setText(getObj() ? GEOMBase::GetName(getObj().get()) : "");
}

//=================================================================================
// function : enterEvent
// purpose  :
//=================================================================================
void MeasureGUI_CheckSelfIntersectionsDlg::enterEvent(QEvent *)
{
  if ( !mainFrame()->GroupConstructors->isEnabled() )
    ActivateThisDialog();
}

//=================================================================================
// function : findSelfIntersections
// purpose  :
//=================================================================================
bool MeasureGUI_CheckSelfIntersectionsDlg::findSelfIntersections
        (bool &HasSelfInte, QString &theErrMsg)
{
  if (getObj()->_is_nil()) {
    return false;
  }

  GEOM::GEOM_IMeasureOperations_var anOper =
    GEOM::GEOM_IMeasureOperations::_narrow(getOperation());
  bool isOK = true;
  int  nbPairs  = 0;
  int aLevel = myLevelBox->currentIndex();

  SUIT_OverrideCursor wc;

  try {
    if ( getConstructorId() == 0 ) {
      HasSelfInte = !anOper->CheckSelfIntersections(myObj1.get(), aLevel, myInters1);
    } else {
      HasSelfInte = !anOper->CheckSelfIntersectionsFast(myObj2.get(), getDeflection(), getTolerance(), myInters2);
    }
    nbPairs = getInters()->length()/2;

    if (nbPairs*2 != getInters()->length()) {
      isOK = false;
    }
  }
  catch (const SALOME::SALOME_Exception& e) {
    SalomeApp_Tools::QtCatchCorbaException(e);
    isOK = false;
  }

  if (!anOper->IsDone()) {
    if (getInters()->length() == 0) {
      theErrMsg = tr(anOper->GetErrorCode());
      isOK = false;
    } else {
      // Valid case. Display all computed self-intersections
      theErrMsg = tr("GEOM_CHECK_SELF_INTERSECTIONS_ERRORS");
    }
  } else if (!isOK) {
    theErrMsg = tr("GEOM_CHECK_SELF_INTERSECTIONS_FAILED");
  }

  return isOK;
}

//=================================================================================
// function : onInteListSelectionChanged
// purpose  :
//=================================================================================
void MeasureGUI_CheckSelfIntersectionsDlg::onInteListSelectionChanged()
{
  erasePreview();
  getShapeList()->clear();

  TopoDS_Shape aSelShape;
  if (!getObj()->_is_nil() && GEOMBase::GetShape(getObj().get(), aSelShape)) {
    TopTools_IndexedMapOfShape anIndices;
    TopExp::MapShapes(aSelShape, anIndices);

    int nbSelected = getInteList()->selectedItems().size();

    for (int i = 0; i < getInteList()->count(); i++) {
      if ( getInteList()->item(i)->isSelected() ) {
        if ( nbSelected > 1 )
          getShapeList()->addItem(QString("--- #%1 ---").arg(i+1));
        for (int j = 0; j < 2; j++) {
          TopoDS_Shape aSubShape = anIndices.FindKey(getInters()[i*2+j]);
          QString aType = GEOMBase::GetShapeTypeString(aSubShape);
          getShapeList()->addItem(QString("%1_%2").arg(aType).arg(getInters()[i*2+j]));
          getShapeList()->item(getShapeList()->count()-1)->setData(Qt::UserRole, getInters()[i*2+j]);
        }
      }
    }
  }
}

//=================================================================================
// function : onSubShapesListSelectionChanged
// purpose  :
//=================================================================================
void MeasureGUI_CheckSelfIntersectionsDlg::onSubShapesListSelectionChanged()
{
  erasePreview();

  // Selected IDs
  QList<QListWidgetItem*> selected = getShapeList()->selectedItems();
  QList<int> aIds;
  foreach(QListWidgetItem* item, selected) {
    int idx = item->data(Qt::UserRole).toInt();
    if (idx > 0 && aIds.indexOf(idx) < 0) aIds.append(idx);
  }

  if (aIds.empty()) return;

  TopoDS_Shape aSelShape;
  TopoDS_Shape aSubShape;
  TopTools_IndexedMapOfShape anIndices;
  if (!getObj()->_is_nil() && GEOMBase::GetShape(getObj().get(), aSelShape)) {
    TopExp::MapShapes(aSelShape, anIndices);
    getDisplayer()->SetColor(Quantity_NOC_RED);
    getDisplayer()->SetWidth(3);
    getDisplayer()->SetToActivate(false);
    foreach(int idx, aIds) {
      aSubShape = anIndices.FindKey(idx);
      try {
	SALOME_Prs* aPrs = !aSubShape.IsNull() ? getDisplayer()->BuildPrs(aSubShape) : 0;
	if (aPrs) displayPreview(aPrs, true);
      }
      catch (const SALOME::SALOME_Exception& e) {
	SalomeApp_Tools::QtCatchCorbaException(e);
      }
    }
  }
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool MeasureGUI_CheckSelfIntersectionsDlg::execute(ObjectList& objects)
{
  bool hasSelfInte;
  QString anErrMsg;

  if (!findSelfIntersections(hasSelfInte, anErrMsg)) {
    return false;
  }

  TColStd_IndexedMapOfInteger aMapIndex;
  QList<int> pairs;

  int nbSelected = getInteList()->selectedItems().size();

  // Collect the map of indices
  for (int i = 0; i < getInteList()->count(); i++) {
    if ( nbSelected < 1 || getInteList()->item(i)->isSelected() ) {
      aMapIndex.Add(getInters()[i*2]);
      aMapIndex.Add(getInters()[i*2 + 1]);
      pairs << getInters()[i*2];
      pairs << getInters()[i*2 + 1];
    }
  }

  GEOM::ShapesOpPtr shapesOper = getGeomEngine()->GetIShapesOperations(getStudyId());
  
  // Explode sub-shapes
  GEOM::ListOfLong_var anArray   = new GEOM::ListOfLong;
  anArray->length(aMapIndex.Extent());

  for (int i = 1; i <= aMapIndex.Extent(); i++)
    anArray[i-1] = aMapIndex.FindKey(i);

  GEOM::ListOfGO_var aList = shapesOper->MakeSubShapes(getObj().get(), anArray);

  // Make compounds
  for (int i = 0; i < pairs.count()/2; i++) {
    GEOM::ListOfGO_var aPair = new GEOM::ListOfGO();
    aPair->length(2);
    aPair[0] = aList[ aMapIndex.FindIndex(pairs[i*2]) - 1 ];
    aPair[1] = aList[ aMapIndex.FindIndex(pairs[i*2+1]) - 1 ];
    GEOM::GEOM_Object_var aCompound = shapesOper->MakeCompound( aPair );
    objects.push_back(aCompound._retn());
  }

  return true;
}

//=================================================================================
// function : getDeflection
// purpose  :
//=================================================================================
float MeasureGUI_CheckSelfIntersectionsDlg::getDeflection()
{
  return (float)myDeflection->value();
}

//=================================================================================
// function : getTolerance
// purpose  :
//=================================================================================
double MeasureGUI_CheckSelfIntersectionsDlg::getTolerance()
{
  double aVal = myTolerance->value();
  if (!myDetGaps->isChecked() || aVal < 0.0)
    return 0.0;
  return aVal;
}

//================================================================
// Function : getFather
// Purpose  : Get father object for object to be added in study
//            (called with addInStudy method)
//================================================================
GEOM::GEOM_Object_ptr MeasureGUI_CheckSelfIntersectionsDlg::getFather
                  (GEOM::GEOM_Object_ptr)
{
  return getObj().get();
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> MeasureGUI_CheckSelfIntersectionsDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res;
  res << getObj();
  return res;
}

//=================================================================================
// GETTERS
//=================================================================================
QTextBrowser* MeasureGUI_CheckSelfIntersectionsDlg::getTextView()
{
  return ( getConstructorId() == 0 ? myTextView1 : myTextView2 );
}

QListWidget* MeasureGUI_CheckSelfIntersectionsDlg::getInteList()
{
  return ( getConstructorId() == 0 ? myInteList1 : myInteList2 );
}

QListWidget* MeasureGUI_CheckSelfIntersectionsDlg::getShapeList()
{
  return ( getConstructorId() == 0 ? myShapeList1 : myShapeList2 );
}

QPushButton* MeasureGUI_CheckSelfIntersectionsDlg::getComputeButton()
{
  return ( getConstructorId() == 0 ? myComputeButton1 : myComputeButton2 );
}

QLineEdit* MeasureGUI_CheckSelfIntersectionsDlg::getEditObjName()
{
  return ( getConstructorId() == 0 ? myEditObjName1 : myEditObjName2 );
}

GEOM::GeomObjPtr MeasureGUI_CheckSelfIntersectionsDlg::getObj()
{
  return ( getConstructorId() == 0 ? myObj1 : myObj2 );
}

GEOM::ListOfLong_var MeasureGUI_CheckSelfIntersectionsDlg::getInters()
{
  return ( getConstructorId() == 0 ? myInters1 : myInters2 );
}
