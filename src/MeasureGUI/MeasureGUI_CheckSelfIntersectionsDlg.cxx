// Copyright (C) 2007-2014  CEA/DEN, EDF R&D, OPEN CASCADE
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
  disconnect(myInteList, SIGNAL(itemSelectionChanged()), this, 0);
  disconnect(myShapeList, SIGNAL(itemSelectionChanged()), this, 0);
  myInteList->clear();
  myShapeList->clear();
  connect(myInteList,    SIGNAL(itemSelectionChanged()),
          SLOT(onInteListSelectionChanged()));
  connect(myShapeList,    SIGNAL(itemSelectionChanged()),
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
  int aCurItem = myInteList->currentRow();

  if (aCurItem < 0)
    return;

  QStringList aSubShapeList;
  TopoDS_Shape aSelShape;
  if (!myObj->_is_nil() && GEOMBase::GetShape(myObj, aSelShape)) {
    TopTools_IndexedMapOfShape anIndices;
    TopExp::MapShapes(aSelShape, anIndices);

    TopoDS_Shape aSubShape = anIndices.FindKey(myInters[aCurItem*2]);
    QString aType = GEOMBase::GetShapeTypeString(aSubShape);
    if (!aType.isEmpty())
      aSubShapeList.append(QString("%1_%2").arg(aType).arg(myInters[aCurItem*2]));
    aSubShape = anIndices.FindKey(myInters[aCurItem*2 + 1]);
    aType = GEOMBase::GetShapeTypeString(aSubShape);
    if (!aType.isEmpty())
      aSubShapeList.append(QString("%1_%2").arg(aType).arg(myInters[aCurItem*2 + 1]));
  }
  myShapeList->clear();
  myShapeList->addItems(aSubShapeList);
}

//=================================================================================
// function : onSubShapesListSelectionChanged
// purpose  :
//=================================================================================
void MeasureGUI_CheckSelfIntersectionsDlg::onSubShapesListSelectionChanged()
{
  erasePreview();

  // Current pair
  int aErrCurItem = myInteList->currentRow();
  if (aErrCurItem < 0)
    return;

  // Selected IDs
  QList<int> aIds;
  for (int i = 0, n = myShapeList->count(); i < n; i++) {
    if (myShapeList->item(i)->isSelected())
      aIds.append(i);
  }
  if (aIds.count() < 1)
    return;

  TopoDS_Shape aSelShape;
  TopoDS_Shape aSubShape;
  TopTools_IndexedMapOfShape anIndices;
  if (!myObj->_is_nil() && GEOMBase::GetShape(myObj, aSelShape)) {
    SALOME_Prs* aPrs = 0;
    TopExp::MapShapes(aSelShape, anIndices);
    QList<int>::iterator it;
    for (it = aIds.begin(); it != aIds.end(); ++it) {
      aSubShape = anIndices.FindKey(myInters[aErrCurItem*2 + (*it)]);
      try {
        getDisplayer()->SetColor(Quantity_NOC_RED);
        getDisplayer()->SetWidth(3);
        getDisplayer()->SetToActivate(false);
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

  const int  aNbInteSelected    = myInteList->selectedItems().size();
  const bool isPublishAllInte   = (aNbInteSelected < 1);
  const bool isPublishAllShapes =
    (aNbInteSelected != 1 || myShapeList->selectedItems().empty());
  int        i;
  const int  n = myInteList->count();
  TColStd_IndexedMapOfInteger aMapIndex;

  // Collect the map of indices.
  for (i = 0; i < n; i++) {
    if (isPublishAllInte) {
      // Collect the both of two indices.
      aMapIndex.Add(myInters[i*2]);
      aMapIndex.Add(myInters[i*2 + 1]);
    } else if (myInteList->item(i)->isSelected()) {
      if (isPublishAllShapes) {
        // Collect the both of two indices.
        aMapIndex.Add(myInters[i*2]);
        aMapIndex.Add(myInters[i*2 + 1]);
      } else if (myShapeList->count() == 2) {
        // Collect only selected items.
        if (myShapeList->item(0)->isSelected()) {
          aMapIndex.Add(myInters[i*2]);
        }
        if (myShapeList->item(1)->isSelected()) {
          aMapIndex.Add(myInters[i*2 + 1]);
        }
      }
    }
  }

  // Create objects.
  GEOM::ListOfLong_var anArray   = new GEOM::ListOfLong;
  const int            aNbShapes = aMapIndex.Extent();

  anArray->length(aNbShapes);

  for (i = 1; i <= aNbShapes; i++) {
    anArray[i - 1] = aMapIndex.FindKey(i);
  }

  if (myShapesOper->_is_nil()) {
    myShapesOper = getGeomEngine()->GetIShapesOperations(getStudyId());
  }

  GEOM::ListOfGO_var aList = myShapesOper->MakeSubShapes(myObj, anArray);
  const int aNbObj = aList->length();

  for (i = 0; i < aNbObj; i++) {
    objects.push_back(GEOM::GEOM_Object::_duplicate(aList[i]));
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
