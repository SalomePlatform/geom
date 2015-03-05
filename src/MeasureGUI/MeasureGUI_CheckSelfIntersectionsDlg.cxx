// Copyright (C) 2007-2015  CEA/DEN, EDF R&D, OPEN CASCADE
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
    myTextView        (0),
    mySelButton       (0),
    myEditObjName     (0),
    myLevelBox        (0),
    myComputeButton   (0),
    myInteList        (0),
    myShapeList       (0)
{
  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
  QPixmap image0 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_CHECK_SELF_INTERSECTIONS")));
  QPixmap image1 (aResMgr->loadPixmap("GEOM", tr("ICON_SELECT")));

  setWindowTitle(tr("GEOM_CHECK_SELF_INTERSECTIONS"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_CHECK_SELF_INTERSECTIONS"));
  mainFrame()->RadioButton1->setIcon(image0);
  mainFrame()->RadioButton2->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton3->close();

  QGroupBox *aGrp      = new QGroupBox(tr("GEOM_CHECK_INFOS"));
  QLabel    *anObjLbl  = new QLabel(tr("GEOM_OBJECT"));
  QLabel    *anInteLbl = new QLabel(tr("GEOM_CHECK_INTE_INTERSECTIONS"));
  QLabel    *aShapeLbl = new QLabel(tr("GEOM_CHECK_INTE_SUBSHAPES"));
  QLabel    *aLevelLbl = new QLabel(tr("GEOM_CHECK_INTE_CHECK_LEVEL"));
  QLabel    *aSummaryLbl = new QLabel(tr("GEOM_CHECK_INTE_SUMMARY"));
  QFont      aFont (TEXTEDIT_FONT_FAMILY, TEXTEDIT_FONT_SIZE);

  aFont.setStyleHint(QFont::TypeWriter, QFont::PreferAntialias);
  myTextView = new QTextBrowser;
  myTextView->setReadOnly(true);
  myTextView->setFont(aFont);

  mySelButton = new QPushButton;
  mySelButton->setIcon(image1);
  mySelButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

  myEditObjName = new QLineEdit;
  myEditObjName->setReadOnly(true);

  myLevelBox = new QComboBox;

  myComputeButton = new QPushButton(tr("GEOM_CHECK_INTE_COMPUTE"));


  myInteList  = new QListWidget;
  myInteList->setSelectionMode(QAbstractItemView::ExtendedSelection);
  myShapeList = new QListWidget;
  myShapeList->setSelectionMode(QAbstractItemView::ExtendedSelection);

  QGridLayout *aGrpLayout = new QGridLayout(aGrp);

  aGrpLayout->setMargin(9);
  aGrpLayout->setSpacing(6);
  aGrpLayout->addWidget(anObjLbl,        0, 0);
  aGrpLayout->addWidget(anInteLbl,       5, 0);
  aGrpLayout->addWidget(aShapeLbl,       5, 2);
  aGrpLayout->addWidget(aLevelLbl,       1, 0);
  aGrpLayout->addWidget(myLevelBox,      1, 1, 1, 2);
  aGrpLayout->addWidget(myComputeButton, 2, 0, 1, 3);
  aGrpLayout->addWidget(aSummaryLbl,     3, 0);
  aGrpLayout->addWidget(myTextView,      4, 0, 1, 3);
  aGrpLayout->addWidget(mySelButton,     0, 1);
  aGrpLayout->addWidget(myEditObjName,   0, 2);
  aGrpLayout->addWidget(myInteList,      6, 0, 1, 2);
  aGrpLayout->addWidget(myShapeList,     6, 2);

  QVBoxLayout* layout = new QVBoxLayout (centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
  layout->addWidget(aGrp);

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

  connect(myGeomGUI,          SIGNAL(SignalDeactivateActiveDialog()),
          this,               SLOT(DeactivateActiveDialog()));
  connect(myGeomGUI,          SIGNAL(SignalCloseAllDialogs()),
          this,               SLOT(ClickOnCancel()));
  connect(buttonOk(),         SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(),      SIGNAL(clicked()), this, SLOT(ClickOnApply()));
  connect(mySelButton,        SIGNAL(clicked()),
          this,               SLOT(SetEditCurrentArgument()));
  connect(myInteList,         SIGNAL(itemSelectionChanged()),
          SLOT(onInteListSelectionChanged()));
  connect(myShapeList,        SIGNAL(itemSelectionChanged()),
          SLOT(onSubShapesListSelectionChanged()));
  connect(myLevelBox,         SIGNAL(currentIndexChanged(int)),
          this,               SLOT(clear()));
  connect(myComputeButton,    SIGNAL(clicked()), this, SLOT(onCompute()));

  LightApp_SelectionMgr* aSel = myGeomGUI->getApp()->selectionMgr();

  connect(aSel,               SIGNAL(currentSelectionChanged()),
          this,               SLOT(SelectionIntoArgument()));

  initName( tr( "GEOM_SELF_INTERSECTION_NAME") );
  buttonOk()->setEnabled(false);
  buttonApply()->setEnabled(false);
  myComputeButton->setEnabled(false);
  activateSelection();
  SelectionIntoArgument();
}

//=================================================================================
// function : clear
// purpose  :
//=================================================================================
void MeasureGUI_CheckSelfIntersectionsDlg::clear()
{
  myTextView->setText("");

  myInteList->blockSignals(true);
  myShapeList->blockSignals(true);
  myInteList->clear();
  myShapeList->clear();
  myInteList->blockSignals(false);
  myShapeList->blockSignals(false);

  erasePreview();

  buttonOk()->setEnabled(false);
  buttonApply()->setEnabled(false);
  myComputeButton->setEnabled(true);
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
    myTextView->setText(anErrMsg);
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

  myTextView->setText(aMsg);

  // Pairs
  QStringList anInteList;
  QString anInteStr ("");
  int nbPairs = myInters->length()/2;

  for (int i = 1; i <= nbPairs; i++) {
    anInteStr = "Intersection # ";
    anInteStr += QString::number(i);
    anInteList.append(anInteStr);
  }

  myInteList->addItems(anInteList);
  myComputeButton->setEnabled(false);
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
  globalSelection(GEOM_ALLSHAPES);
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

  initName();
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
  return !myObj->_is_nil();
}

//=================================================================================
// function : SetEditCurrentArgument
// purpose  :
//=================================================================================
void MeasureGUI_CheckSelfIntersectionsDlg::SetEditCurrentArgument()
{
  myEditObjName->setFocus();
  SelectionIntoArgument();
}

//=================================================================================
// function : SelectionIntoArgument
// purpose  :
//=================================================================================
void MeasureGUI_CheckSelfIntersectionsDlg::SelectionIntoArgument()
{
  // Clear the dialog.
  clear();
  myObj = GEOM::GEOM_Object::_nil();

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  GEOM::GEOM_Object_var aSelectedObject = GEOM::GEOM_Object::_nil();

  if (aSelList.Extent() > 0) {
    aSelectedObject = GEOMBase::ConvertIOinGEOMObject( aSelList.First() );
  }

  if (aSelectedObject->_is_nil()) {
    myEditObjName->setText("");
    return;
  }

  myObj = aSelectedObject;
  myEditObjName->setText(GEOMBase::GetName(myObj));
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
  if (myObj->_is_nil()) {
    return false;
  }

  GEOM::GEOM_IMeasureOperations_var anOper =
    GEOM::GEOM_IMeasureOperations::_narrow(getOperation());
  bool isOK = true;
  int  nbPairs  = 0;
  int aLevel = myLevelBox->currentIndex();

  SUIT_OverrideCursor wc;

  try {
    HasSelfInte = !anOper->CheckSelfIntersections(myObj, aLevel, myInters);
    nbPairs = myInters->length()/2;

    if (nbPairs*2 != myInters->length()) {
      isOK = false;
    }
  }
  catch (const SALOME::SALOME_Exception& e) {
    SalomeApp_Tools::QtCatchCorbaException(e);
    isOK = false;
  }

  if (!anOper->IsDone()) {
    if (myInters->length() == 0) {
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
  myShapeList->clear();

  TopoDS_Shape aSelShape;
  if (!myObj->_is_nil() && GEOMBase::GetShape(myObj, aSelShape)) {
    TopTools_IndexedMapOfShape anIndices;
    TopExp::MapShapes(aSelShape, anIndices);

    int nbSelected = myInteList->selectedItems().size();

    for (int i = 0; i < myInteList->count(); i++) {
      if ( myInteList->item(i)->isSelected() ) {
        if ( nbSelected > 1 )
          myShapeList->addItem(QString("--- #%1 ---").arg(i+1));
        for (int j = 0; j < 2; j++) {
          TopoDS_Shape aSubShape = anIndices.FindKey(myInters[i*2+j]);
          QString aType = GEOMBase::GetShapeTypeString(aSubShape);
          myShapeList->addItem(QString("%1_%2").arg(aType).arg(myInters[i*2+j]));
          myShapeList->item(myShapeList->count()-1)->setData(Qt::UserRole, myInters[i*2+j]);
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
  QList<QListWidgetItem*> selected = myShapeList->selectedItems();
  QList<int> aIds;
  foreach(QListWidgetItem* item, selected) {
    int idx = item->data(Qt::UserRole).toInt();
    if (idx > 0 && aIds.indexOf(idx) < 0) aIds.append(idx);
  }

  if (aIds.empty()) return;

  TopoDS_Shape aSelShape;
  TopoDS_Shape aSubShape;
  TopTools_IndexedMapOfShape anIndices;
  if (!myObj->_is_nil() && GEOMBase::GetShape(myObj, aSelShape)) {
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

  int nbSelected = myInteList->selectedItems().size();

  // Collect the map of indices
  for (int i = 0; i < myInteList->count(); i++) {
    if ( nbSelected < 1 || myInteList->item(i)->isSelected() ) {
      aMapIndex.Add(myInters[i*2]);
      aMapIndex.Add(myInters[i*2 + 1]);
      pairs << myInters[i*2];
      pairs << myInters[i*2 + 1];
    }
  }

  GEOM::ShapesOpPtr shapesOper = getGeomEngine()->GetIShapesOperations(getStudyId());
  
  // Explode sub-shapes
  GEOM::ListOfLong_var anArray   = new GEOM::ListOfLong;
  anArray->length(aMapIndex.Extent());

  for (int i = 1; i <= aMapIndex.Extent(); i++)
    anArray[i-1] = aMapIndex.FindKey(i);

  GEOM::ListOfGO_var aList = shapesOper->MakeSubShapes(myObj, anArray);

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

//================================================================
// Function : getFather
// Purpose  : Get father object for object to be added in study
//            (called with addInStudy method)
//================================================================
GEOM::GEOM_Object_ptr MeasureGUI_CheckSelfIntersectionsDlg::getFather
                  (GEOM::GEOM_Object_ptr)
{
  return myObj;
}
