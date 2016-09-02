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

#include "OperationGUI_ExtractionDlg.h"

#include <GEOMBase.h>
#include <GeometryGUI.h>

#include <LightApp_SelectionMgr.h>
#include <SalomeApp_Application.h>
#include <SalomeApp_Tools.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>

#include <TColStd_MapIteratorOfMapOfInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_MapOfShape.hxx>

#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QRadioButton>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QtGlobal>

#if QT_VERSION >= 0x050300
  #include <QSignalBlocker>
#else
  /**
   * This class is named as QT class as it is introduced since Qt 5.3.
   * It should not be compiled when Salome is ported on Qt 5.3.
   */
  class QSignalBlocker
  {
  public:
    QSignalBlocker(QObject *object)
      : myObject    (object),
        myIsBlocked (object && object->signalsBlocked()) {
      if (myObject) {
        myObject->blockSignals(true);
      }
    }

    ~QSignalBlocker() {
      if (myObject) {
        myObject->blockSignals(myIsBlocked);
      }
    }

  private:
    QObject *myObject;    ///< Blocked object.
    bool     myIsBlocked; ///< Initial blocked state.
  };
#endif


#define ID_ROLE   Qt::DisplayRole
#define TYPE_ROLE Qt::UserRole

static const char* const TMP_STR = "TEMP";

static const char* const SINGLE_SHAPE_TYPE_TR_CODES [] = {
  "GEOM_COMPOUND",
  "GEOM_COMPOUNDSOLID",
  "GEOM_SOLID",
  "GEOM_SHELL",
  "GEOM_FACE",
  "GEOM_WIRE",
  "GEOM_EDGE",
  "GEOM_VERTEX"
};

static const char* const PLURAL_SHAPE_TYPE_TR_CODES [] = {
  "GEOM_COMPOUND",   // Not used
  "GEOM_COMPSOLIDS",
  "GEOM_SOLIDS",
  "GEOM_SHELLS",
  "GEOM_FACES",
  "GEOM_WIREZ",
  "GEOM_EDGES",
  "GEOM_VERTEXES"
};


/**
 * This static function creates a new list widget item with given ID and
 * returns it.
 *
 * \param theID the item ID.
 * \param theListWidget the list widget.
 * \return the created list widget item.
 */
static QListWidgetItem *addNewItem(const int    theID,
                                   QListWidget *theListWidget)
{
  QListWidgetItem *aResult = new QListWidgetItem;

  aResult->setData(ID_ROLE, theID);
  theListWidget->addItem(aResult);

  return aResult;
}

/**
 * This static function creates a new tree widget item as a child of the input
 * one with given ID and returns it.
 *
 * \param theID the item ID.
 * \param theParentItem the parent item.
 * \return the created tree widget item.
 */
static QTreeWidgetItem *addChildItem(const int        theID,
                                     QTreeWidgetItem *theParentItem)
{
  QTreeWidgetItem *aResult = new QTreeWidgetItem;

  aResult->setData(0, ID_ROLE, theID);
  theParentItem->addChild(aResult);

  return aResult;
}

/**
 * This static function returns the maximal shape type of sub-shapes stored in
 * the input compound. If it is not a compound, it returns TopAbs_SHAPE.
 *
 * \param theCompound the compound.
 * \return the maximal shape type of sub-shapes stored in the input compound.
 */
static TopAbs_ShapeEnum GetMaxShapeTypeInComp(const TopoDS_Shape &theCompound)
{
  TopAbs_ShapeEnum aResult = TopAbs_SHAPE;

  if (theCompound.IsNull() || theCompound.ShapeType() != TopAbs_COMPOUND) {
    return aResult;
  }

  TopoDS_Iterator anIt(theCompound, Standard_False, Standard_False);

  for (; anIt.More(); anIt.Next()) {
    const TopoDS_Shape &aSubShape = anIt.Value();

    if (aSubShape.IsNull()) {
      continue;
    }

    // Get the sub-shape type.
    TopAbs_ShapeEnum aSubType = aSubShape.ShapeType();

    if (aSubType == TopAbs_COMPOUND) {
      aSubType = GetMaxShapeTypeInComp(aSubShape);
    }

    if (aSubType == TopAbs_SHAPE) {
      continue;
    }

    if (aResult == TopAbs_SHAPE) {
      // This is an initialization.
      aResult = aSubType;
    } else if (aResult > aSubType) {
      aResult = aSubType;
    }
  }

  return aResult;
}

//=================================================================================
// class    : OperationGUI_ExtractionDlg()
// purpose  : 
//=================================================================================
OperationGUI_ExtractionDlg::OperationGUI_ExtractionDlg
                        (GeometryGUI* GUI, QWidget* parent)
  : GEOMBase_Skeleton (GUI, parent, false),
    mySelBtn          (0),
    myMainShapeEdit   (0),
    mySubShTypeCompo  (0),
    myFilteredList    (0),
    myExtractedTree   (0),
    myRemovedList     (0),
    myModifiedList    (0),
    myAddedList       (0),
    myRebuildBtn      (0),
    myIsHiddenMain    (false)
{
  QPixmap image0(SUIT_Session::session()->resourceMgr()->loadPixmap(
    "GEOM", tr("ICON_DLG_EXTRACTION")));
  QPixmap image1(SUIT_Session::session()->resourceMgr()->loadPixmap(
    "GEOM", tr("ICON_SELECT")));

  setWindowTitle(tr("GEOM_EXTRACT_TITLE"));

  /***************************************************************/

  mainFrame()->GroupConstructors->setTitle(tr("GEOM_EXTRACT_TYPE"));
  mainFrame()->RadioButton1->setIcon( image0 );
  mainFrame()->RadioButton2->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton3->close();

  // Create an input group.
  QGroupBox   *anInputGrp      = new QGroupBox(tr("GEOM_EXTRACT_INPUT_PARAMS"), centralWidget());
  QGridLayout *anInputLayout   = new QGridLayout(anInputGrp);
  QHBoxLayout *aShapeLayout    = new QHBoxLayout(anInputGrp);
  QVBoxLayout *aViewBtnsLayout = new QVBoxLayout(anInputGrp);
  QVBoxLayout *aMoveBtnsLayout = new QVBoxLayout(anInputGrp);
  QLabel      *aMainObjLbl     = new QLabel(tr("GEOM_MAIN_OBJECT"), anInputGrp);
  QLabel      *aSubShTypeLbl   = new QLabel(tr("GEOM_EXTRACT_SUB_SHAPE_TYPE"), anInputGrp);
  QLabel      *aFilteredLbl    = new QLabel(tr("GEOM_EXTRACT_FILTERED_SHAPES"), anInputGrp);
  QLabel      *anExtractedLbl  = new QLabel(tr("GEOM_EXTRACT_SHAPES_TO_EXTRACT"), anInputGrp);
  QPushButton *aShowOnlySelBtn = new QPushButton(tr("SHOW_ONLY_SELECTED"), anInputGrp);
  QPushButton *aHideSelBtn     = new QPushButton(tr("HIDE_SELECTED"), anInputGrp);
  QPushButton *aShowAllBtn     = new QPushButton(tr("SHOW_ALL_SUB_SHAPES"), anInputGrp);
  QPushButton *anAddBtn        = new QPushButton(">>", anInputGrp);
  QPushButton *aRemoveBtn      = new QPushButton("<<", anInputGrp);

  myRebuildBtn     = new QPushButton(tr("GEOM_EXTRACT_REBUILD"), anInputGrp);
  mySelBtn         = new QPushButton(anInputGrp);
  myMainShapeEdit  = new QLineEdit(anInputGrp);
  mySubShTypeCompo = new QComboBox(anInputGrp);
  myFilteredList   = new QListWidget(anInputGrp);
  myExtractedTree  = new QTreeWidget(anInputGrp);
  mySelBtn->setIcon(image1);
  myMainShapeEdit->setReadOnly(true);

  aShapeLayout->addWidget(mySelBtn);
  aShapeLayout->addWidget(myMainShapeEdit);

  aViewBtnsLayout->addStretch();
  aViewBtnsLayout->addWidget(aShowOnlySelBtn);
  aViewBtnsLayout->addWidget(aHideSelBtn);
  aViewBtnsLayout->addWidget(aShowAllBtn);
  aViewBtnsLayout->addStretch();

  aMoveBtnsLayout->addStretch();
  aMoveBtnsLayout->addWidget(anAddBtn);
  aMoveBtnsLayout->addWidget(aRemoveBtn);
  aMoveBtnsLayout->addStretch();

  anInputLayout->setSpacing(6);
  anInputLayout->setContentsMargins(9, 9, 9, 9);
  anInputLayout->addWidget(aMainObjLbl,      0, 0);
  anInputLayout->addLayout(aShapeLayout,     0, 1, 1, 3);
  anInputLayout->addWidget(aSubShTypeLbl,    1, 0);
  anInputLayout->addWidget(mySubShTypeCompo, 1, 1, 1, 3);
  anInputLayout->addWidget(aFilteredLbl,     2, 1);
  anInputLayout->addWidget(anExtractedLbl,   2, 3);
  anInputLayout->addLayout(aViewBtnsLayout,  3, 0);
  anInputLayout->addWidget(myFilteredList,   3, 1);
  anInputLayout->addLayout(aMoveBtnsLayout,  3, 2);
  anInputLayout->addWidget(myExtractedTree,  3, 3);
  anInputLayout->addWidget(myRebuildBtn,     4, 0, 1, 4);

  // Create a statistics group.
  QGroupBox   *aStatGrp     = new QGroupBox(tr("GEOM_EXTRACT_STATISTICS"), centralWidget());
  QGridLayout *aStatLayout  = new QGridLayout(aStatGrp);
  QLabel      *aRemovedLbl  = new QLabel(tr("GEOM_EXTRACT_REMOVED"), aStatGrp);
  QLabel      *aModifiedLbl = new QLabel(tr("GEOM_EXTRACT_MODIFIED"), aStatGrp);
  QLabel      *anAddedLbl   = new QLabel(tr("GEOM_EXTRACT_ADDED"), aStatGrp);

  myRemovedList  = new QListWidget(aStatGrp);
  myModifiedList = new QListWidget(aStatGrp);
  myAddedList    = new QListWidget(aStatGrp);

  aStatLayout->setSpacing(6);
  aStatLayout->setContentsMargins(9, 9, 9, 9);
  aStatLayout->addWidget(aRemovedLbl,    0, 0);
  aStatLayout->addWidget(aModifiedLbl,   0, 1);
  aStatLayout->addWidget(anAddedLbl,     0, 2);
  aStatLayout->addWidget(myRemovedList,  1, 0);
  aStatLayout->addWidget(myModifiedList, 1, 1);
  aStatLayout->addWidget(myAddedList,    1, 2);

  // Create a main layout.
  QVBoxLayout* aLayout = new QVBoxLayout(centralWidget());

  aLayout->setMargin(0);
  aLayout->setSpacing(6);
  aLayout->addWidget(anInputGrp);
  aLayout->addWidget(aStatGrp);

  // signals and slots connections
  connect(anAddBtn,        SIGNAL(clicked()), this, SLOT(onAddExtracted()));
  connect(aRemoveBtn,      SIGNAL(clicked()), this, SLOT(onRemoveExtracted()));
  connect(aShowOnlySelBtn, SIGNAL(clicked()), this, SLOT(showOnlySelected()));
  connect(aHideSelBtn,     SIGNAL(clicked()), this, SLOT(hideSelected()));
  connect(aShowAllBtn,     SIGNAL(clicked()), this, SLOT(showAllSelected()));

  /***************************************************************/
  myHelpFileName = "extract_and_rebuild_page.html";

  resize(525, 600);

  /* Initialisation */
  Init();
}

//=================================================================================
// function : ~OperationGUI_ExtractionDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
OperationGUI_ExtractionDlg::~OperationGUI_ExtractionDlg()
{
  restoreViewer();
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void OperationGUI_ExtractionDlg::Init()
{
  mySelBtn->setCheckable(true);
  mySelBtn->setChecked(true);
  myFilteredList->setSelectionMode(QAbstractItemView::ExtendedSelection);
  myFilteredList->setSortingEnabled(true);
  myExtractedTree->setHeaderHidden(true);
  myExtractedTree->setSelectionMode(QAbstractItemView::ExtendedSelection);
  myExtractedTree->setColumnCount(1);
  myRebuildBtn->setEnabled(false);
  myRemovedList->setSelectionMode(QAbstractItemView::NoSelection);
  myModifiedList->setSelectionMode(QAbstractItemView::NoSelection);
  myAddedList->setSelectionMode(QAbstractItemView::NoSelection);

  // Fill in the extracted tree with initial elements.
  myTopItems[0] = 0; // No need to create a item for compound.

  int i;

  for (i = 1; i < 8; i++) {
    myTopItems[i] = new QTreeWidgetItem;
    myTopItems[i]->setText(0, tr(PLURAL_SHAPE_TYPE_TR_CODES[i]));
    myTopItems[i]->setData(0, TYPE_ROLE, i);
  
    myExtractedTree->addTopLevelItem(myTopItems[i]);
    myTopItems[i]->setHidden(true);
  }

  // signals and slots connections
  connect(mySelBtn,         SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(buttonOk(),       SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(),    SIGNAL(clicked()), this, SLOT(ClickOnApply()));
  connect(mySubShTypeCompo, SIGNAL(currentIndexChanged(int)),
          this,             SLOT(onShapeTypeChanged()));
  connect(myRebuildBtn,     SIGNAL(clicked()), this, SLOT(onRebuild()));
  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));
  connect(myFilteredList,   SIGNAL(itemSelectionChanged()),
          this,             SLOT(onListSelectionChanged()));
  connect(myExtractedTree,  SIGNAL(itemSelectionChanged()),
          this,             SLOT(onListSelectionChanged()));

  initName(tr("GEOM_EXTRACT_NAME"));

  activateSelection();
  SelectionIntoArgument();
}

//=================================================================================
// function : updateSubShTypeCompo()
// purpose  :
//=================================================================================
bool OperationGUI_ExtractionDlg::updateSubShTypeCompo()
{
  bool         isValid  = true;
  int          anIStart = TopAbs_COMPOUND;
  const int    anIEnd   = TopAbs_VERTEX;
  TopoDS_Shape aShape;

  if (GEOMBase::GetShape(myObj, aShape)) {
    const TopAbs_ShapeEnum aType = aShape.ShapeType();

    if (aType == TopAbs_COMPOUND) {
      anIStart = GetMaxShapeTypeInComp(aShape);
      isValid  = anIStart != TopAbs_SHAPE;
    } else {
      anIStart = aType + 1;
    }
  }

  QSignalBlocker aBlocker(mySubShTypeCompo);
  mySubShTypeCompo->clear();

  if (isValid) {
    int i;

    for (i = anIStart; i <= anIEnd; i++) {
      mySubShTypeCompo->addItem(tr(SINGLE_SHAPE_TYPE_TR_CODES[i]), i);
    }

    updateFilteredList();
  }

  return isValid;
}

//=================================================================================
// function : updateFilteredList()
// purpose  :
//=================================================================================
void OperationGUI_ExtractionDlg::updateFilteredList()
{
  TopoDS_Shape   aShape;
  QSignalBlocker aBlocker(myFilteredList);

  myFilteredList->clear();

  if (GEOMBase::GetShape(myObj, aShape)) {
    const TopAbs_ShapeEnum aType = (TopAbs_ShapeEnum)
      mySubShTypeCompo->itemData(mySubShTypeCompo->currentIndex()).toInt();
    TopExp_Explorer        anExp(aShape, aType);

    if (anExp.More()) {
      TopTools_MapOfShape aMapFence;

      for (; anExp.More(); anExp.Next()) {
        const TopoDS_Shape &aSubShape = anExp.Current();

        if (!aSubShape.IsNull() && aMapFence.Add(aSubShape)) {
          int anIndex = myIndices.FindIndex(aSubShape);

          if (!myMapExtractedIDs.Contains(anIndex)) {
            addNewItem(anIndex, myFilteredList);
          }
        }
      }
    }
  }
}

//=================================================================================
// function : resetBuildData()
// purpose  :
//=================================================================================
void OperationGUI_ExtractionDlg::resetBuildData(const bool isEnableBuild)
{
  // Clear result data.
  myRemovedList->clear();
  myModifiedList->clear();
  myAddedList->clear();
  myRebuildBtn->setEnabled(isEnableBuild);
}

//=================================================================================
// function : isEmptyExtracted()
// purpose  :
//=================================================================================
bool OperationGUI_ExtractionDlg::isEmptyExtracted()
{
  bool isEmpty = true;
  int  i;

  // Check if there are sub-shapes to be extracted.
  for (i = 1; i < 8; i++) {
    if (!myTopItems[i]->isHidden()) {
      isEmpty = false;

      break;
    }
  }

  return isEmpty;
}

//=================================================================================
// function : selectMainShape
// purpose  :
//=================================================================================
void OperationGUI_ExtractionDlg::selectMainShape()
{
  LightApp_SelectionMgr *aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO          aSelList;

  aSelMgr->selectedObjects(aSelList);

  if (aSelList.Extent() == 1) {
    GEOM::GEOM_Object_var aSelObject =
      GEOMBase::ConvertIOinGEOMObject(aSelList.First());
    TopoDS_Shape          aSelShape;

    if (GEOMBase::GetShape(aSelObject, aSelShape)) {
      const TopAbs_ShapeEnum aType = aSelShape.ShapeType();

      // Skip verices.
      if (aType != TopAbs_VERTEX) {
        myObj = aSelObject;

        // Initialize map of indices. Note that myIndices should be empty.
        TopExp::MapShapes(aSelShape, myIndices);
      }
    }
  }

  if (!updateSubShTypeCompo()) {
    // Invalid selected object.
    myObj = GEOM::GEOM_Object::_nil();
  }

  if (!CORBA::is_nil(myObj)) {
    mySelBtn->setChecked(false);
    myMainShapeEdit->setEnabled(false);
    myMainShapeEdit->setText(GEOMBase::GetName(myObj));

    // Hide the main object from the viewer.
    SALOME_View* aView = GEOM_Displayer::GetActiveView();

    if (aView) {
      CORBA::String_var                aMainEntry = myObj->GetStudyEntry();
      Handle(SALOME_InteractiveObject) anIO       = createIO(aMainEntry.in());

      if (aView->isVisible(anIO)) {
        GEOM_Displayer *aDisplayer = getDisplayer();

        aDisplayer->Erase(myObj, false, true);
        myIsHiddenMain = true;
      }
    }
  }
}

//=================================================================================
// function : selectSubShapes
// purpose  :
//=================================================================================
void OperationGUI_ExtractionDlg::selectSubShapes()
{
  QSignalBlocker aBlocker(myFilteredList);

  // Clear current selection.
  myFilteredList->clearSelection();

  LightApp_SelectionMgr *aSelMgr  = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO          aSelList;
  const int              aCurType =
      mySubShTypeCompo->itemData(mySubShTypeCompo->currentIndex()).toInt();

  aSelMgr->selectedObjects(aSelList);

  // try to find out and process the global selection
  // (of not published objects and of published sub-shapes)
  SALOME_ListIteratorOfListIO anIter(aSelList);

  for (; anIter.More(); anIter.Next()) {
    Handle(SALOME_InteractiveObject) anIObj      = anIter.Value();
    QString                          anEntry     = anIObj->getEntry();
    QStringList                      aParts      = anEntry.split("_");
    int                              aSubShapeId = -1;

    if (!aParts.isEmpty()) {
      if (aParts.first() == TMP_STR) {
        bool      isOk    = false;
        const int anIndex = aParts.last().toInt(&isOk);

        if (isOk && anIndex > 0) {
          // This is a sub-shape.
          aSubShapeId = anIndex;
        }
      }
    }

    if (aSubShapeId < 0) {
      // This is a published shape.
      GEOM::GEOM_Object_var aSelObject =
                    GEOMBase::ConvertIOinGEOMObject(anIObj);
      TopoDS_Shape          aSelShape;

      if (GEOMBase::GetShape(aSelObject, aSelShape)) {

        if (aSelShape.ShapeType() == aCurType) {
          const int anIndex = myIndices.FindIndex(aSelShape);

          if (anIndex > 0) {
            // This is a sub-shape. Select it in the filtered list.
            aSubShapeId = anIndex;
          }
        }
      }
    }

    // Select a list widget item by Id.
    if (aSubShapeId > 0) {
      QString                  anIdText = QString("%1").arg(aSubShapeId);
      QList<QListWidgetItem *> aFound   =
                    myFilteredList->findItems(anIdText, Qt::MatchExactly);

      foreach (QListWidgetItem *anItem, aFound) {
        anItem->setSelected(true);
      }
    }
  }
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void OperationGUI_ExtractionDlg::ClickOnOk()
{
  if (ClickOnApply()) {
    ClickOnCancel();
  }
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool OperationGUI_ExtractionDlg::ClickOnApply()
{
  if (!onAccept()) {
    return false;
  }

  initName();

  return true;
}

//=================================================================================
// function : onShapeTypeChanged
// purpose  :
//=================================================================================
void OperationGUI_ExtractionDlg::onShapeTypeChanged()
{
  updateFilteredList();
  eraseAll();
}

//=================================================================================
// function : onAddExtracted
// purpose  :
//=================================================================================
void OperationGUI_ExtractionDlg::onAddExtracted()
{
  QList<QListWidgetItem *> aListSelected = myFilteredList->selectedItems();

  if (aListSelected.empty()) {
    return;
  }

  const int aShapeType =
      mySubShTypeCompo->itemData(mySubShTypeCompo->currentIndex()).toInt();
  bool  isTreeUpdated  = false;

  foreach (QListWidgetItem *anItem, aListSelected) {
    const int anIndex = anItem->data(ID_ROLE).toInt();

    if (myMapExtractedIDs.Add(anIndex)) {
      addChildItem(anIndex, myTopItems[aShapeType]);
      isTreeUpdated = true;
    }

    // Remove anItem from the list.
    myFilteredList->removeItemWidget(anItem);
    delete anItem;
  }

  if (isTreeUpdated) {
    myTopItems[aShapeType]->sortChildren(0, Qt::AscendingOrder);

    // Reset build data
    resetBuildData(true);
  }

  myFilteredList->clearSelection();
  myTopItems[aShapeType]->setHidden(false);
}

//=================================================================================
// function : onRemoveExtracted
// purpose  :
//=================================================================================
void OperationGUI_ExtractionDlg::onRemoveExtracted()
{
  QList<QTreeWidgetItem *> aListSelected = myExtractedTree->selectedItems();

  if (aListSelected.empty()) {
    return;
  }

  const int               aShapeType =
      mySubShTypeCompo->itemData(mySubShTypeCompo->currentIndex()).toInt();
  QSet<QTreeWidgetItem *> aSetFence;
  bool                    isTreeUpdated = false;

  foreach (QTreeWidgetItem *anItem, aListSelected) {
    if (!aSetFence.contains(anItem)) {
      aSetFence.insert(anItem);

      QTreeWidgetItem *aParent = anItem->parent();

      if (aParent) {
        const int anIndex = anItem->data(0, ID_ROLE).toInt();
        // This is a ID item. Remove the ID from myMapExtractedIDs.
        if (myMapExtractedIDs.Remove(anIndex)) {
          // The item is not removed yet. Get parent index.
          const int aParentIndex = aParent->data(0, TYPE_ROLE).toInt();

          if (aShapeType == aParentIndex) {
            // Create an item in the filtered list.
            addNewItem(anIndex, myFilteredList);
          }

          aParent->removeChild(anItem);
          delete anItem;
          isTreeUpdated = true;

          // Hilde an empty parent item.
          if (aParent->childCount() == 0) {
            aParent->setHidden(true);
          }
        }
      } else {
        // This is a top level item. Remove all its children.
        QList<QTreeWidgetItem *> aChildItems = anItem->takeChildren();
        const int                anIndex     = anItem->data(0, TYPE_ROLE).toInt();

        // Remove IDs from myMapExtractedIDs.
        foreach (QTreeWidgetItem *aChild, aChildItems) {
          if (!aSetFence.contains(aChild)) {
            aSetFence.insert(aChild);

            const int aChildIndex = aChild->data(0, ID_ROLE).toInt();

            if (myMapExtractedIDs.Remove(aChildIndex)) {
              if (aShapeType == anIndex) {
                // Create items in the filtered list.
                addNewItem(aChildIndex, myFilteredList);
              }

              delete aChild;
              isTreeUpdated = true;
            }
          }
        }

        // Hilde an empty item.
        anItem->setHidden(true);
      }
    }
  }

  myExtractedTree->clearSelection();

  if (isTreeUpdated) {
    // Reset build data
    const bool isEnableRebuild = !isEmptyExtracted();

    resetBuildData(isEnableRebuild);
  }
}

//=================================================================================
// function : onListSelectionChanged
// purpose  :
//=================================================================================
void OperationGUI_ExtractionDlg::onListSelectionChanged()
{
  SALOME_ListIO            anIOList;
  QList<QListWidgetItem *> aListSel = myFilteredList->selectedItems();
  QList<QTreeWidgetItem *> aTreeSel = myExtractedTree->selectedItems();

  // Collect selected items from myFilteredList
  foreach (QListWidgetItem *anItem, aListSel) {
    const int anIndex = anItem->data(ID_ROLE).toInt();

    if (myMapDisplayedIDs.Contains(anIndex)) {
      // Collect only displayed sub-shapes for selection in the viewer.
      QString                          anEntry = getSubShapeEntry(anIndex);
      Handle(SALOME_InteractiveObject) anIO    =
                                      createIO(anEntry.toLatin1().data());

      anIOList.Append(anIO);
    }
  }

  // Collect selected items from myExtractedTree
  foreach (QTreeWidgetItem *anItem, aTreeSel) {
    if (anItem->parent()) {
      // This is a ID item.
      const int anIndex = anItem->data(0, ID_ROLE).toInt();

      if (myMapDisplayedIDs.Contains(anIndex)) {
        // Collect only displayed sub-shapes for selection in the viewer.
        QString                          anEntry = getSubShapeEntry(anIndex);
        Handle(SALOME_InteractiveObject) anIO    =
                                    createIO(anEntry.toLatin1().data());

        anIOList.Append(anIO);
      }
    }
  }

  // Select object in viewer.
  LightApp_SelectionMgr *aSelMgr = myGeomGUI->getApp()->selectionMgr();

  aSelMgr->setSelectedObjects(anIOList);
}

//=================================================================================
// function : showOnlySelected
// purpose  :
//=================================================================================
void OperationGUI_ExtractionDlg::showOnlySelected()
{
  TColStd_MapOfInteger aMapIDsSelected;
  TColStd_MapOfInteger aMapIDsToDisplay;
  const int            aNbItems = myFilteredList->count();
  int                  i;
  QSet<QString>        aSelEntry;

  // Get sub-shape IDs to be displayed.
  for (i = 0; i < aNbItems; ++i) {
    QListWidgetItem *anItem = myFilteredList->item(i);
    const int       anIndex = anItem->data(ID_ROLE).toInt();

    if (anItem->isSelected()) {
      aMapIDsSelected.Add(anIndex);
      aSelEntry.insert(getSubShapeEntry(anIndex));

      if (!myMapDisplayedIDs.Contains(anIndex)) {
        aMapIDsToDisplay.Add(anIndex);
      }
    }
  }

  // Get sub-shape IDs to be erased.
  TColStd_MapOfInteger              aMapIDsToHide;
  TColStd_MapIteratorOfMapOfInteger anIter(myMapDisplayedIDs);

  for (; anIter.More(); anIter.Next()) {
    const int anIndex = anIter.Key();

    if (!aMapIDsSelected.Contains(anIndex)) {
      aMapIDsToHide.Add(anIndex);
    }
  }

  // Display sub-shapes.
  for (anIter.Initialize(aMapIDsToDisplay); anIter.More(); anIter.Next()) {
    displaySubShape(anIter.Key());
  }

  // Hide sub-shapes.
  for (anIter.Initialize(aMapIDsToHide); anIter.More(); anIter.Next()) {
    eraseSubShape(anIter.Key());
  }

  // Hide all objects except already displayed sub-shapes.
  SALOME_ListIO  aDisplayed;
  SALOME_View   *aView = GEOM_Displayer::GetActiveView();

  if (aView) {
    aView->GetVisible(aDisplayed);
  }

  SALOME_ListIteratorOfListIO  aDispIt(aDisplayed);
  GEOM_Displayer              *aDisplayer = getDisplayer();

  for (; aDispIt.More(); aDispIt.Next()) {
    Handle(SALOME_InteractiveObject) anIO = aDispIt.Value();

    if (!aSelEntry.contains(anIO->getEntry())) {
      aDisplayer->Erase(anIO, false, false);
    }
  }

  onListSelectionChanged();
  aDisplayer->UpdateViewer();
}

//=================================================================================
// function : hideSelected
// purpose  :
//=================================================================================
void OperationGUI_ExtractionDlg::hideSelected()
{
  QList<QListWidgetItem *> aListSelected = myFilteredList->selectedItems();

  foreach (QListWidgetItem *anItem, aListSelected) {
    const int anIndex = anItem->data(ID_ROLE).toInt();

    eraseSubShape(anIndex);
  }

  getDisplayer()->UpdateViewer();
}

//=================================================================================
// function : showAllSelected
// purpose  :
//=================================================================================
void OperationGUI_ExtractionDlg::showAllSelected()
{
  const int aNbItems = myFilteredList->count();
  int       i;

  for (i = 0; i < aNbItems; ++i) {
    QListWidgetItem *anItem = myFilteredList->item(i);
    const int anIndex = anItem->data(ID_ROLE).toInt();

    displaySubShape(anIndex);
  }

  onListSelectionChanged();
  getDisplayer()->UpdateViewer();
}

//=================================================================================
// function : onRebuild
// purpose  :
//=================================================================================
void OperationGUI_ExtractionDlg::onRebuild()
{
  GEOM::GEOM_Object_var                         aResShape;
  GEOM::GEOM_IShapesOperations::ExtractionStats aStats;

  if (!getResult(aResShape.out(), aStats)) {
    resetBuildData(false);
    return;
  }

  TopoDS_Shape               anOldShape;
  TopoDS_Shape               aNewShape;
  TopTools_IndexedMapOfShape aNewIndices;

  if (!GEOMBase::GetShape(aResShape, aNewShape)) {
    resetBuildData(false);
    return;
  }

  TopExp::MapShapes(aNewShape, aNewIndices);

  const int aNbStat = aStats.length();
  int       i;

  for (i = 0; i < aNbStat; ++i) {
    // Compute number of sub-shapes of each type.
    const int aNbSubShapes = aStats[i].indices.length();
    int       aNbShapes [] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    int       j;

    TopTools_IndexedMapOfShape *aMapShapes =
      (aStats[i].type == GEOM::GEOM_IShapesOperations::EST_Added) ?
            &aNewIndices : &myIndices;

    for (j = 0; j < aNbSubShapes; ++j) {
      const int anIndex = aStats[i].indices[j];

      if (anIndex < 1 || anIndex > aMapShapes->Extent()) {
        resetBuildData(false);
        return;
      }

      const TopoDS_Shape &aSubShape = aMapShapes->FindKey(anIndex);

      aNbShapes[aSubShape.ShapeType()]++;
    }

    // Fill the statistics.
    QListWidget *aListWidget = 0;

    switch (aStats[i].type) {
      case GEOM::GEOM_IShapesOperations::EST_Removed:
        aListWidget = myRemovedList;
        break;
      case GEOM::GEOM_IShapesOperations::EST_Modified:
        aListWidget = myModifiedList;
        break;
      case GEOM::GEOM_IShapesOperations::EST_Added:
        aListWidget = myAddedList;
        break;
      default:
        resetBuildData(false);
        return;
    }

    QStringList aStrList;

    for (j = 0; j < 8; ++j) {
      if (aNbShapes[j] >= 1) {
        const char *aShapeType = aNbShapes[j] == 1 ?
          SINGLE_SHAPE_TYPE_TR_CODES[j] : PLURAL_SHAPE_TYPE_TR_CODES[j];

        aStrList.append(QString("%1 %2").arg(aNbShapes[j]).arg(tr(aShapeType)));
      }
    }

    aListWidget->addItems(aStrList);
  }

  myRebuildBtn->setEnabled(false);
}

//=================================================================================
// function : SelectionIntoArgument
// purpose  :
//=================================================================================
void OperationGUI_ExtractionDlg::SelectionIntoArgument()
{
  if (myMainShapeEdit->isEnabled()) {
    // Selection of main object
    selectMainShape();
  } else {
    // Selection of filtered shapes
    selectSubShapes();
  }
}

//=================================================================================
// function : SetEditCurrentArgument
// purpose  :
//=================================================================================
void OperationGUI_ExtractionDlg::SetEditCurrentArgument()
{
  QSignalBlocker aBlockerList(myFilteredList);
  QSignalBlocker aBlockerTree(myExtractedTree);

  restoreViewer();
  myObj = GEOM::GEOM_Object::_nil();
  myMainShapeEdit->setEnabled(true);
  myMainShapeEdit->setText("");
  myMainShapeEdit->setFocus();

  updateSubShTypeCompo();

  myFilteredList->clear();
  myRemovedList->clear();
  myModifiedList->clear();
  myAddedList->clear();
  myIndices.Clear();

  // Clear myExtractedTree.
  int i;

  for (i = 1; i < 8; i++) {
    QList<QTreeWidgetItem *> aListItems = myTopItems[i]->takeChildren();

    foreach (QTreeWidgetItem *anItem, aListItems) {
      delete anItem;
    }

    myTopItems[i]->setHidden(true);
  }

  myExtractedTree->clearSelection();

  myMapExtractedIDs.Clear();

  // Update viewer
  eraseAll();
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void OperationGUI_ExtractionDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();

  LightApp_SelectionMgr* aSel = myGeomGUI->getApp()->selectionMgr();

  if (aSel) {
    connect(aSel, SIGNAL(currentSelectionChanged()),
            this, SLOT(SelectionIntoArgument()));
  }

  activateSelection();
}

//=================================================================================
// function : activateSelection
// purpose  : activate selection of all shapes
//=================================================================================
void OperationGUI_ExtractionDlg::activateSelection()
{
  globalSelection(GEOM_ALLSHAPES);
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void OperationGUI_ExtractionDlg::enterEvent(QEvent *)
{
  if (!mainFrame()->GroupConstructors->isEnabled()) {
    ActivateThisDialog();
  }
}

//=================================================================================
// function : getResult
// purpose  :
//=================================================================================
bool OperationGUI_ExtractionDlg::getResult
             (GEOM::GEOM_Object_ptr                         &theResult,
              GEOM::GEOM_IShapesOperations::ExtractionStats &theStats)
{
  if (myObj->_is_nil()) {
    return false;
  }

  // Get IDs of extracted shapes.
  int i;
  int aNbShapes = 0;

  for (i = 1; i < 8; i++) {
    aNbShapes += myTopItems[i]->childCount();
  }

  if (aNbShapes == 0) {
    return false;
  }

  GEOM::ListOfLong_var aSubShapeIDs = new GEOM::ListOfLong;
  int                  j;
  int                  jCur;

  aSubShapeIDs->length(aNbShapes);

  for (jCur = 0, i = 1; i < 8; ++i) {
    aNbShapes = myTopItems[i]->childCount();
    
    for (j = 0; j < aNbShapes; ++j, ++jCur) {
      aSubShapeIDs[jCur] = myTopItems[i]->child(j)->data(0, ID_ROLE).toInt();
    }
  }

  GEOM::GEOM_IShapesOperations_var anOper =
    GEOM::GEOM_IShapesOperations::_narrow(getOperation());

  try {
    GEOM::GEOM_Object_var                             anObj;
    GEOM::GEOM_IShapesOperations::ExtractionStats_var aStats;

    anObj = anOper->MakeExtraction(myObj, aSubShapeIDs, aStats);

    if (anOper->IsDone() && aStats->length() > 0) {
      theStats = aStats;
    }

    if (!CORBA::is_nil(anObj)) {
      theResult = anObj._retn();
    }
  }
  catch (const SALOME::SALOME_Exception& e) {
    SalomeApp_Tools::QtCatchCorbaException(e);
    return false;
  }

  return anOper->IsDone();
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool OperationGUI_ExtractionDlg::isValid(QString &)
{
  bool isOk = !myObj->_is_nil() && !isEmptyExtracted();

  return isOk;
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr OperationGUI_ExtractionDlg::createOperation()
{
  return getGeomEngine()->GetIShapesOperations(getStudyId());
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool OperationGUI_ExtractionDlg::execute(ObjectList &objects)
{
  GEOM::GEOM_Object_var                         aResShape;
  GEOM::GEOM_IShapesOperations::ExtractionStats aStats;

  if (!getResult(aResShape.out(), aStats)) {
    return false;
  }

  if (!aResShape->_is_nil()) {
    objects.push_back(aResShape._retn());
  }

  return true;
}

//=================================================================================
// function : getSubShapeEntry
// purpose  :
//=================================================================================
QString OperationGUI_ExtractionDlg::getSubShapeEntry(const int theId)
{
  CORBA::String_var aMainEntry = myObj->GetStudyEntry();
  QString           anEntry    = QString("%1_").arg(TMP_STR) +
                                 aMainEntry.in() + QString("_%1").arg(theId);

  return anEntry;
}

//=================================================================================
// function : createIO
// purpose  :
//=================================================================================
Handle_SALOME_InteractiveObject OperationGUI_ExtractionDlg::createIO
                                                      (const char *theEntry)
{
  Handle(SALOME_InteractiveObject) anIO = new SALOME_InteractiveObject
                              (theEntry, "GEOM", "TEMP_IO");

  return anIO;
}

//=================================================================================
// function : displaySubShape
// purpose  :
//=================================================================================
void OperationGUI_ExtractionDlg::displaySubShape(const int theId)
{
  if (theId < 1 || theId > myIndices.Extent()) {
    return;
  }

  // Create a presentation
  const TopoDS_Shape &aSubShape  = myIndices.FindKey(theId);
  QString             anEntry    = getSubShapeEntry(theId);
  SALOME_View        *aView      = GEOM_Displayer::GetActiveView();
  GEOM_Displayer     *aDisplayer = getDisplayer();
  SALOME_Prs         *aPrs       = aDisplayer->buildSubshapePresentation
                                  (aSubShape, anEntry, aView);

  if (aPrs) {
    if (aView) {
      aView->Display(aDisplayer, aPrs);
    }

    delete aPrs;

    myMapDisplayedIDs.Add(theId);
  }
}

//=================================================================================
// function : eraseSubShape
// purpose  :
//=================================================================================
void OperationGUI_ExtractionDlg::eraseSubShape(const int theId)
{
  QString                          anEntry = getSubShapeEntry(theId);
  Handle(SALOME_InteractiveObject) anIO    =
                                      createIO(anEntry.toLatin1().data());

  getDisplayer()->Erase(anIO, false, false);
  myMapDisplayedIDs.Remove(theId);
}

//=================================================================================
// function : eraseAll
// purpose  :
//=================================================================================
void OperationGUI_ExtractionDlg::eraseAll()
{
  TColStd_MapIteratorOfMapOfInteger anIter(myMapDisplayedIDs);
  TColStd_ListOfInteger             aDisplayedIDs;

  for (; anIter.More(); anIter.Next()) {
    aDisplayedIDs.Append(anIter.Key());
  }

  TColStd_ListIteratorOfListOfInteger aListIter(aDisplayedIDs);

  for (; aListIter.More(); aListIter.Next()) {
    eraseSubShape(aListIter.Value());
  }

  getDisplayer()->UpdateViewer();
}

//=================================================================================
// function : restoreViewer
// purpose  :
//=================================================================================
void OperationGUI_ExtractionDlg::restoreViewer()
{
  if (!CORBA::is_nil(myObj)) {
    if (myIsHiddenMain) {
      getDisplayer()->Display(myObj, false);
      myIsHiddenMain = false;
    }

    eraseAll();
  }
}
