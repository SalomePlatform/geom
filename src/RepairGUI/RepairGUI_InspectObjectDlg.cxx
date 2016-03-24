// Copyright (C) 2014-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

// internal includes
#include "RepairGUI_InspectObjectDlg.h"

// GEOM includes
#include <GEOMBase.h>
#include <GEOM_Constants.h>
#include <GeometryGUI.h>
#include <GEOMUtils.hxx>

// GUI includes
#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Desktop.h>
#include <SUIT_ViewManager.h>
#include <SUIT_ViewWindow.h>

#include <LightApp_SelectionMgr.h>

#include <SalomeApp_Application.h>
#include <SalomeApp_DoubleSpinBox.h>
#include <SalomeApp_Study.h>

#include <OCCViewer_ViewModel.h>
#include <SVTK_ViewModel.h>

// OCCT includes
#include <BRep_Tool.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_MapOfShape.hxx>

// Qt includes
#include <QButtonGroup>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QHeaderView>
#include <QItemDelegate>
#include <QLineEdit>
#include <QRadioButton>
#include <QStackedLayout>
#include <QTreeWidgetItem>

// Shape type definitions (values are equal to corresponding types of TopAbs_ShapeEnum).
#define TYPE_FACE   4
#define TYPE_EDGE   6
#define TYPE_VERTEX 7

// Comparison type definitions
#define COMPARE_GT 0
#define COMPARE_GE 1
#define COMPARE_LT 2
#define COMPARE_LE 3

// Default tolerance values
#define DEFAULT_TOLERANCE_VALUE         1.e-07

//=================================================================================
// class    : RepairGUI_InspectObjectDlg::TreeWidgetItem
// purpose  : class for "Inspect Object" tree item creation
//=================================================================================
class RepairGUI_InspectObjectDlg::TreeWidgetItem : public QTreeWidgetItem
{
public:
  TreeWidgetItem(QTreeWidget*,
                 const QStringList&,
                 const TopoDS_Shape&,
                 const Handle(SALOME_InteractiveObject)&,
                 double = DEFAULT_TOLERANCE_VALUE,
                 int = Type);

  TreeWidgetItem(QTreeWidgetItem*,
                 const QStringList&,
                 const TopoDS_Shape&,
                 const QString&,
                 double = DEFAULT_TOLERANCE_VALUE,
                 int = Type);

  ~TreeWidgetItem();

  bool                             isVisible();
  void                             setVisible( bool, QIcon& );

  TopoDS_Shape                     getShape() const;
  Handle(SALOME_InteractiveObject) getIO() const;

  double                           getTolerance() const;
  void                             setTolerance(double theTolerance);

private:
  bool                             myIsVisible;
  TopoDS_Shape                     myShape;
  Handle(SALOME_InteractiveObject) myIO;
  double                           myTolerance;

};

RepairGUI_InspectObjectDlg::TreeWidgetItem::TreeWidgetItem
                     (QTreeWidget                            *view,
                      const QStringList                      &strings,
                      const TopoDS_Shape                     &shape,
                      const Handle(SALOME_InteractiveObject) &io,
                      double                                  theTolerance,
                      int                                     type)
: QTreeWidgetItem( view, strings, type ),
  myIsVisible( false ),
  myShape( shape ),
  myIO( io ),
  myTolerance (theTolerance)
{
}

RepairGUI_InspectObjectDlg::TreeWidgetItem::TreeWidgetItem
                     (QTreeWidgetItem    *parent,
                      const QStringList  &strings,
                      const TopoDS_Shape &shape,
                      const QString      &entry,
                      double              theTolerance,
                      int                 type)
: QTreeWidgetItem( parent, strings, type ),
  myIsVisible( false ),
  myShape( shape ),
  myTolerance (theTolerance)
{
  myIO = new SALOME_InteractiveObject( entry.toLatin1(), "GEOM", "TEMP_IO" );
  setFlags( flags() | Qt::ItemIsEditable );
}

RepairGUI_InspectObjectDlg::TreeWidgetItem::~TreeWidgetItem()
{
}

bool RepairGUI_InspectObjectDlg::TreeWidgetItem::isVisible()
{
  return myIsVisible;
}

void RepairGUI_InspectObjectDlg::TreeWidgetItem::setVisible( bool isVisible, QIcon& icon )
{
  myIsVisible = isVisible;
  setIcon( 1, icon );
}

TopoDS_Shape RepairGUI_InspectObjectDlg::TreeWidgetItem::getShape() const
{
  return myShape;
}

Handle(SALOME_InteractiveObject) RepairGUI_InspectObjectDlg::TreeWidgetItem::getIO() const
{
  return myIO;
}

double RepairGUI_InspectObjectDlg::TreeWidgetItem::getTolerance() const
{
  return myTolerance;
}

void RepairGUI_InspectObjectDlg::TreeWidgetItem::setTolerance(double theTolerance)
{
  myTolerance = theTolerance;
}

//=================================================================================
// class    : RepairGUI_InspectObjectDlg::Delegate
// purpose  : class for "Inspect Object" tree item editing
//=================================================================================
class RepairGUI_InspectObjectDlg::Delegate : public QItemDelegate
{
public:
  Delegate( QObject* = 0 );
  ~Delegate();

  void   setEditorData( QWidget*, const QModelIndex& ) const;
  void   setModelData( QWidget*, QAbstractItemModel*, const QModelIndex& ) const;
  QWidget* createEditor( QWidget*, const QStyleOptionViewItem&, const QModelIndex& ) const;
};

RepairGUI_InspectObjectDlg::Delegate::Delegate( QObject* parent )
: QItemDelegate( parent )
{
}

RepairGUI_InspectObjectDlg::Delegate::~Delegate()
{
}

void RepairGUI_InspectObjectDlg::Delegate::setEditorData( QWidget* editor,
                                                          const QModelIndex& index ) const
{
  QItemDelegate::setEditorData( editor, index );
  editor->setProperty( "___name___", editor->property( "text" ) );
}

void RepairGUI_InspectObjectDlg::Delegate::setModelData( QWidget* editor,
                                                         QAbstractItemModel* model,
                                                         const QModelIndex& index ) const
{
  QString aNewName = editor->property( "text" ).toString();
  if ( aNewName.trimmed().isEmpty() )
    editor->setProperty( "text", editor->property( "___name___" ) );
  QItemDelegate::setModelData( editor, model, index );
}

QWidget* RepairGUI_InspectObjectDlg::Delegate::createEditor( QWidget* parent,
                                                             const QStyleOptionViewItem& option,
                                                             const QModelIndex& index ) const
{
  return index.column() == 1 ? 0 : QItemDelegate::createEditor( parent, option, index );
}

//=================================================================================
// class    : RepairGUI_InspectObjectDlg()
// purpose  : Constructs a RepairGUI_InspectObjectDlg which is a child of 'parent'.
//=================================================================================
RepairGUI_InspectObjectDlg::RepairGUI_InspectObjectDlg(GeometryGUI *theGeomGUI, SUIT_Desktop* parent )
: GEOMBase_Helper       (parent),
  QDialog               (parent),
  myGeomGUI             (theGeomGUI),
  myTreeObjects         (0),
  myFilteredTreeObjects (0),
  myCurrentTreeObjects  (0),
  myEditMainShape       (0),
  myTolFilterGrp        (0),
  myShapeTypeBtnGrp     (0),
  myComparisonCompo     (0),
  myMinTolValLabel      (0),
  myMaxTolValLabel      (0),
  myTolEdit             (0),
  myTreesLayout         (0),
  myTransparency        (0.0),
  myIsSelectAll         (false),
  myMaxTol              (-1.),
  myMinTol              (-1.)
{
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  QIcon iconSelect( resMgr->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );
  myVisible = QIcon( resMgr->loadPixmap( "SUIT", tr( "ICON_DATAOBJ_VISIBLE" ) ) );
  myInvisible = QIcon( resMgr->loadPixmap( "SUIT", tr( "ICON_DATAOBJ_INVISIBLE" ) ) );

  QPixmap anImageVtx(resMgr->loadPixmap("GEOM", tr("ICON_OBJBROWSER_VERTEX")));
  QPixmap anImageEdge(resMgr->loadPixmap("GEOM", tr("ICON_OBJBROWSER_EDGE")));
  QPixmap anImageFace(resMgr->loadPixmap("GEOM", tr("ICON_OBJBROWSER_FACE")));

  setWindowTitle( tr( "GEOM_INSPECT_OBJECT_TITLE" ) );
  setAttribute( Qt::WA_DeleteOnClose );

  myViewWindow = myGeomGUI->getApp()->desktop()->activeWindow();

  QGridLayout* topLayout = new QGridLayout( this );
  topLayout->setMargin( 11 ); topLayout->setSpacing( 6 );

  /**********************   Inspected Object    **********************/

  QHBoxLayout* mainShapeLayout = new QHBoxLayout(this);

  QLabel* label = new QLabel( tr( "GEOM_INSPECT_OBJECT_MAIN_SHAPE" ), this );
  QPushButton* selBtn = new QPushButton(this);
  selBtn->setIcon( iconSelect );
  myEditMainShape = new QLineEdit(this);
  myEditMainShape->setReadOnly(true);

  mainShapeLayout->addWidget( label );
  mainShapeLayout->addWidget( selBtn );
  mainShapeLayout->addWidget( myEditMainShape );

  /**********************   Tolerance filter    **********************/

  myTolFilterGrp    = new QGroupBox (tr("GEOM_INSPECT_TOLERANCE_FILTER"), this);
  myShapeTypeBtnGrp = new QButtonGroup(myTolFilterGrp);

  // Filter on shape type
  QRadioButton *aVtx   = new QRadioButton(tr("GEOM_VERTEX"), myTolFilterGrp);
  QRadioButton *anEdge = new QRadioButton(tr("GEOM_EDGE"),   myTolFilterGrp);
  QRadioButton *aFace  = new QRadioButton(tr("GEOM_FACE"),   myTolFilterGrp);

  aVtx->setIcon(anImageVtx);
  anEdge->setIcon(anImageEdge);
  aFace->setIcon(anImageFace);
  myShapeTypeBtnGrp->addButton(aVtx,   TYPE_VERTEX);
  myShapeTypeBtnGrp->addButton(anEdge, TYPE_EDGE);
  myShapeTypeBtnGrp->addButton(aFace,  TYPE_FACE);

  // Filter on sub-shape tolerance
  QLabel      *aTolLabel     = new QLabel(tr("GEOM_TOLERANCE"), myTolFilterGrp);
  QLabel      *aMinTolLabel  = new QLabel(tr("GEOM_MIN"), myTolFilterGrp);
  QLabel      *aMaxTolLabel  = new QLabel(tr("GEOM_MAX"), myTolFilterGrp);
  QGridLayout *aFilterLayout = new QGridLayout(myTolFilterGrp);

  myMinTolValLabel  = new QLabel(myTolFilterGrp);
  myMaxTolValLabel  = new QLabel(myTolFilterGrp);
  myMinTolResetBtn  = new QPushButton(tr("GEOM_INSPECT_RESET_MIN"), myTolFilterGrp);
  myMaxTolResetBtn  = new QPushButton(tr("GEOM_INSPECT_RESET_MAX"), myTolFilterGrp);
  myComparisonCompo = new QComboBox(myTolFilterGrp);
  myTolEdit         = new SalomeApp_DoubleSpinBox(myTolFilterGrp);
  myTolEdit->setMinimumWidth(120);
  aFilterLayout->addWidget(aVtx,   0, 0);
  aFilterLayout->addWidget(anEdge, 0, 1);
  aFilterLayout->addWidget(aFace,  0, 2);
  aFilterLayout->addWidget(aMaxTolLabel,      1, 0, Qt::AlignRight);
  aFilterLayout->addWidget(aTolLabel,         2, 0);
  aFilterLayout->addWidget(aMinTolLabel,      3, 0, Qt::AlignRight);
  aFilterLayout->addWidget(myMaxTolValLabel,  1, 1);
  aFilterLayout->addWidget(myComparisonCompo, 2, 1);
  aFilterLayout->addWidget(myMinTolValLabel,  3, 1);
  aFilterLayout->addWidget(myMaxTolResetBtn,  1, 2);
  aFilterLayout->addWidget(myTolEdit,         2, 2);
  aFilterLayout->addWidget(myMinTolResetBtn,  3, 2);
  aFilterLayout->setRowMinimumHeight(0, 30);

  myTolFilterGrp->setCheckable(true);

  /**********************   Sub-objects trees   **********************/
  createTreeWidget(myTreeObjects);
  createTreeWidget(myFilteredTreeObjects);

  myTreesLayout = new QStackedLayout(this);
  myTreesLayout->addWidget(myTreeObjects);
  myTreesLayout->addWidget(myFilteredTreeObjects);

  /**********************        Buttons        **********************/

  QVBoxLayout* buttonsLayout1 = new QVBoxLayout(this);

  QPushButton* buttonShow = new QPushButton( tr( "GEOM_INSPECT_OBJECT_SHOW" ), this );
  QPushButton* buttonShowOnly = new QPushButton( tr( "GEOM_INSPECT_OBJECT_SHOW_ONLY" ), this );
  QPushButton* buttonHide = new QPushButton( tr( "GEOM_INSPECT_OBJECT_HIDE" ), this );
  QPushButton* buttonPublish = new QPushButton( tr( "GEOM_INSPECT_OBJECT_PUBLISH" ), this );
  QPushButton* aShowAllBtn = new QPushButton(tr("SHOW_ALL_BTN"), this);
  QPushButton* aHideAllBtn = new QPushButton(tr("HIDE_ALL_BTN"), this);


  buttonsLayout1->addWidget( buttonShow );
  buttonsLayout1->addWidget( buttonShowOnly );
  buttonsLayout1->addWidget( buttonHide );
  buttonsLayout1->addWidget( aShowAllBtn );
  buttonsLayout1->addWidget( aHideAllBtn );
  buttonsLayout1->addWidget( buttonPublish );
  buttonsLayout1->addStretch();

  QHBoxLayout* buttonsLayout2 = new QHBoxLayout(this);

  QPushButton* buttonClose = new QPushButton( tr( "GEOM_BUT_CLOSE" ), this );
  QPushButton* buttonHelp = new QPushButton( tr( "GEOM_BUT_HELP" ), this );

  buttonsLayout2->addWidget( buttonClose );
  buttonsLayout2->addStretch();
  buttonsLayout2->addWidget( buttonHelp );

  topLayout->addLayout( mainShapeLayout, 0, 0 );
  topLayout->addWidget( myTolFilterGrp, 1, 0);
  topLayout->addLayout( myTreesLayout, 2, 0 );
  topLayout->addLayout( buttonsLayout1, 0, 1, 3, 1 );
  topLayout->addLayout( buttonsLayout2, 3, 0, 1, 2 );

  connect( selBtn, SIGNAL( clicked() ), this, SLOT( onEditMainShape() ) );

  connect( buttonShow,     SIGNAL( clicked() ), this, SLOT( clickOnShow() ) );
  connect( buttonShowOnly, SIGNAL( clicked() ), this, SLOT( clickOnShowOnly() ) );
  connect( buttonHide,     SIGNAL( clicked() ), this, SLOT( clickOnHide() ) );
  connect( buttonPublish,  SIGNAL( clicked() ), this, SLOT( clickOnPublish() ) );
  connect( buttonClose,    SIGNAL( clicked() ), this, SLOT( reject() ) );
  connect( buttonHelp,     SIGNAL( clicked() ), this, SLOT( clickOnHelp() ) );
  connect( aShowAllBtn,   SIGNAL( clicked() ), this, SLOT( clickOnShowAll() ) );
  connect( aHideAllBtn,   SIGNAL( clicked() ), this, SLOT( clickOnHideAll() ) );

  init();
}

RepairGUI_InspectObjectDlg::~RepairGUI_InspectObjectDlg()
{
  if ( myViewWindow )
    onEditMainShape();
  // restore initial parameters for viewer
  getDisplayer()->UnsetColor();
  getDisplayer()->UnsetWidth();
  // no need to delete child widgets, Qt does it all for us
}

//=================================================================================
// function : createTreeWidget()
// purpose  :
//=================================================================================
void RepairGUI_InspectObjectDlg::createTreeWidget(QTreeWidget *&theTreeObjects)
{
  theTreeObjects = new QTreeWidget(this);
  theTreeObjects->setColumnCount(2);
  QStringList columnNames;
  columnNames.append(tr("GEOM_INSPECT_OBJECT_NAME"));
  columnNames.append("");
  theTreeObjects->setHeaderLabels(columnNames);
  QTreeWidgetItem* headerItem = new QTreeWidgetItem(columnNames);

  headerItem->setIcon(1, myInvisible);
  theTreeObjects->setHeaderItem(headerItem);
  theTreeObjects->header()->moveSection(1, 0);
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
  theTreeObjects->header()->setClickable(true);
  theTreeObjects->header()->setMovable(false);
  theTreeObjects->header()->setResizeMode( 1, QHeaderView::ResizeToContents);
#else
  theTreeObjects->header()->setSectionsClickable(true);
  theTreeObjects->header()->setSectionsMovable(false);
  theTreeObjects->header()->setSectionResizeMode( 1, QHeaderView::ResizeToContents);
#endif
  theTreeObjects->setSelectionMode(QAbstractItemView::ExtendedSelection);
  theTreeObjects->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
  // set custom item delegate
  theTreeObjects->setItemDelegate(new Delegate(theTreeObjects));
}

//=================================================================================
// function : init()
// purpose  : initialize dialog data
//=================================================================================
void RepairGUI_InspectObjectDlg::init()
{
  myTolFilterGrp->setChecked(false);
  myComparisonCompo->addItem(">",  GEOMUtils::CC_GT);
  myComparisonCompo->addItem(">=", GEOMUtils::CC_GE);
  myComparisonCompo->addItem("<",  GEOMUtils::CC_LT);
  myComparisonCompo->addItem("<=", GEOMUtils::CC_LE);

  initSpinBox(myTolEdit, 0., 100., DEFAULT_TOLERANCE_VALUE, "len_tol_precision");
  myTolEdit->setValue(DEFAULT_TOLERANCE_VALUE);

  // Signals and slots connections
  initTreeWidget(myTreeObjects);
  initTreeWidget(myFilteredTreeObjects);
  myCurrentTreeObjects = myTreeObjects;
  myMaxTolResetBtn->setEnabled(false);
  myMinTolResetBtn->setEnabled(false);

  connect(myMinTolResetBtn,  SIGNAL(clicked()), this, SLOT(clickOnResetToMin()));
  connect(myMaxTolResetBtn,  SIGNAL(clicked()), this, SLOT(clickOnResetToMax()));
  connect(myShapeTypeBtnGrp, SIGNAL(buttonClicked(int)), this, SLOT(onInitFilteredData()));
  connect(myGeomGUI, SIGNAL(SignalDeactivateActiveDialog()), this, SLOT(DeactivateActiveDialog()));
  connect(myGeomGUI, SIGNAL(SignalCloseAllDialogs()),        this, SLOT(reject()));

  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
           this, SLOT( onViewSelectionChanged() ) );

  connect( myGeomGUI->getApp()->desktop(), SIGNAL( windowActivated( SUIT_ViewWindow* ) ),
           this, SLOT( onWindowActivated( SUIT_ViewWindow* ) ) );

  // Connect signals and slots for filter group box elements.
  connect(myTolFilterGrp, SIGNAL(toggled(bool)),
          this, SLOT(onFilterToggled(bool)));
  connect(myComparisonCompo, SIGNAL(currentIndexChanged(int)),
          this, SLOT(onFilterData()));
  connect(myTolEdit, SIGNAL(valueChanged(double)),
          this, SLOT(onFilterData()));

  if ( myViewWindow )
    connect( myViewWindow->getViewManager(), SIGNAL( deleteView( SUIT_ViewWindow* ) ),
             this, SLOT( onCloseView( SUIT_ViewWindow* ) ), Qt::UniqueConnection );

  // Initialize the dialog with current selection.
  onViewSelectionChanged();
}

//=================================================================================
// function : initSpinBox()
// purpose  : 
//=================================================================================
void RepairGUI_InspectObjectDlg::initSpinBox(SalomeApp_DoubleSpinBox* spinBox, 
                                             double min,  double max, 
                                             double step, const char* quantity)
{
  // Obtain precision from preferences
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  int aPrecision = resMgr->integerValue( "Geometry", quantity, 6 );
  
  spinBox->setPrecision( aPrecision );
  spinBox->setDecimals( qAbs( aPrecision ) ); // it's necessary to set decimals before the range setting,
                                    // by default Qt rounds boundaries to 2 decimals at setRange
  spinBox->setRange( min, max );
  spinBox->setSingleStep( step );
  
  // Add a hint for the user saying how to tune precision
  QString userPropName = QObject::tr( QString( "GEOM_PREF_%1" ).arg( quantity ).toLatin1().constData() );
  spinBox->setProperty( "validity_tune_hint", 
                        QVariant( QObject::tr( "GEOM_PRECISION_HINT" ).arg( userPropName ) ) );
}

//=================================================================================
// function : initTreeWidget()
// purpose  :
//=================================================================================
void RepairGUI_InspectObjectDlg::initTreeWidget(QTreeWidget *theTreeObjects)
{
  connect(theTreeObjects, SIGNAL(itemClicked (QTreeWidgetItem *, int)),
          this, SLOT(onItemClicked(QTreeWidgetItem *, int)));
  connect(theTreeObjects, SIGNAL(itemChanged (QTreeWidgetItem *, int)),
          this, SLOT(onItemChanged(QTreeWidgetItem *, int)));
  connect(theTreeObjects, SIGNAL(itemExpanded (QTreeWidgetItem *)),
          this, SLOT(onItemExpanded(QTreeWidgetItem *)));
  connect(theTreeObjects, SIGNAL(itemSelectionChanged()),
          this, SLOT(onItemSelectionChanged()));

  connect(theTreeObjects->header(), SIGNAL(sectionClicked(int)),
          this, SLOT(onHeaderClicked(int)));
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void RepairGUI_InspectObjectDlg::enterEvent (QEvent*)
{
  if (!myTolFilterGrp->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : checkVisibleIcon()
// purpose  : set visible or invisible icon in the header of tree
//=================================================================================
void RepairGUI_InspectObjectDlg::checkVisibleIcon()
{
  bool isInvisible = false;
  QTreeWidgetItemIterator it( myCurrentTreeObjects );
  while ( *it ) {
    TreeWidgetItem* anItem = dynamic_cast<TreeWidgetItem*>(*it);
    if ( !anItem->isHidden() &&  !anItem->isVisible() ) {
      isInvisible = true;
      break;
    }
    ++it;
  }

  if ( isInvisible ) {
    myCurrentTreeObjects->headerItem()->setIcon( 1, myInvisible );
    myIsSelectAll = false;
  }
  else {
    myCurrentTreeObjects->headerItem()->setIcon( 1, myVisible );
    myIsSelectAll = true;
  }
}

//=================================================================================
// function : addSubObjects()
// purpose  : add sub-objects to parent object in the tree
//=================================================================================
void RepairGUI_InspectObjectDlg::addSubObjects
                    (TreeWidgetItem                   *theParentItem,
                     const TopTools_IndexedMapOfShape &theIndices)
{
  TreeWidgetItem* aMainItem = dynamic_cast<TreeWidgetItem*>( myTreeObjects->topLevelItem(0) );
  TopoDS_Iterator it( theParentItem->getShape() );
  for ( ; it.More(); it.Next() ) {
    TopoDS_Shape aSubShape = it.Value();
    int anIndex = theIndices.FindIndex(aSubShape);
    QString anEntry = QString( "TEMP_" ) + aMainItem->getIO()->getEntry() + QString("_%1").arg( anIndex );
    TreeWidgetItem* anItem = new TreeWidgetItem( theParentItem, QStringList(), aSubShape, anEntry);
    anItem->setVisible( false, myInvisible );
    addSubObjects(anItem, theIndices);
  }
}

//=================================================================================
// function : onInitFilteredData()
// purpose  : add sub-objects to parent object in the filtered tree
//=================================================================================
void RepairGUI_InspectObjectDlg::onInitFilteredData()
{
  TreeWidgetItem *aMainItem =
          dynamic_cast<TreeWidgetItem*>(myFilteredTreeObjects->topLevelItem(0));

  if (!aMainItem) {
    return;
  }

  // Remove the children.
  SALOME_ListIO           aListOfIO;
  QTreeWidgetItemIterator it(aMainItem);

  while (*it) {
    TreeWidgetItem* anItem = dynamic_cast<TreeWidgetItem*>(*it);
    if (aMainItem != anItem && (anItem->flags() & Qt::ItemIsSelectable) &&
        anItem->isVisible() && !anItem->isHidden()) {
      aListOfIO.Append(anItem->getIO());
    }

    ++it;
  }

  myFilteredTreeObjects->clearSelection();
  myFilteredTreeObjects->headerItem()->setIcon(1, myInvisible);
  getDisplayer()->Erase(aListOfIO);
  getDisplayer()->UpdateViewer();

  // Delete child items.
  QList<QTreeWidgetItem *> aListItems = aMainItem->takeChildren();

  foreach (QTreeWidgetItem *anItem, aListItems) {
    delete anItem;
  }

  // Initialize the tree with a new list.
  TopoDS_Shape      aShape     = aMainItem->getShape();
  TopAbs_ShapeEnum  aShapeType = aShape.ShapeType();

  myShapeTypeBtnGrp->button(TYPE_FACE)->setVisible(aShapeType < TYPE_FACE);
  myShapeTypeBtnGrp->button(TYPE_EDGE)->setVisible(aShapeType < TYPE_EDGE);
  myShapeTypeBtnGrp->button(TYPE_VERTEX)->setVisible(aShapeType < TYPE_VERTEX);

  int anId = myShapeTypeBtnGrp->checkedId();

  myMaxTol = -RealLast();
  myMinTol =  RealLast();

  if (anId != -1 && myShapeTypeBtnGrp->checkedButton()->isVisible()) {
    // Get sub-shapes
    TopTools_MapOfShape         aMapFence;
    TopExp_Explorer             anExp(aShape, (TopAbs_ShapeEnum)anId);
    TopTools_IndexedMapOfShape  anIndices;

    TopExp::MapShapes(aShape, anIndices);

    for (; anExp.More(); anExp.Next()) {
      const TopoDS_Shape &aSubShape = anExp.Current();

      if (aMapFence.Add(aSubShape)) {
        // Compute tolerance
        Standard_Real aTolerance = -1.;

        switch (aSubShape.ShapeType()) {
          case TYPE_FACE:
            aTolerance = BRep_Tool::Tolerance(TopoDS::Face(aSubShape));
            break;
          case TYPE_EDGE:
            aTolerance = BRep_Tool::Tolerance(TopoDS::Edge(aSubShape));
            break;
          case TYPE_VERTEX:
            aTolerance = BRep_Tool::Tolerance(TopoDS::Vertex(aSubShape));
            break;
          default:
            break;
        }

        if (aTolerance < 0.) {
          continue;
        }

        if (aTolerance > myMaxTol) {
          myMaxTol = aTolerance;
        }

        if (aTolerance < myMinTol) {
          myMinTol = aTolerance;
        }

        int anIndex = anIndices.FindIndex(aSubShape);
        QString anEntry = QString( "TEMP_" ) +
                          aMainItem->getIO()->getEntry() +
                          QString::number(anIndex);
        TreeWidgetItem* anItem =
            new TreeWidgetItem(aMainItem, QStringList(),
                               aSubShape, anEntry, aTolerance);
        anItem->setVisible( false, myInvisible );
      }
    }
  }

  // Compose names of sub-items if the main item is expanded.
  if (aMainItem->isExpanded()) {
    onItemExpanded(aMainItem);
  }

  myMaxTolResetBtn->setEnabled(myMaxTol >= myMinTol);
  myMinTolResetBtn->setEnabled(myMaxTol >= myMinTol);

  if (myMaxTol < myMinTol) {
    myMinTol = DEFAULT_TOLERANCE_VALUE;
    myMaxTol = DEFAULT_TOLERANCE_VALUE;
    myMinTolValLabel->setText(QString::number(DEFAULT_TOLERANCE_VALUE));
    myMaxTolValLabel->setText(QString::number(DEFAULT_TOLERANCE_VALUE));
    myTolEdit->setValue(DEFAULT_TOLERANCE_VALUE);
  } else {
    myMinTolValLabel->setText(QString::number(myMinTol));
    myMaxTolValLabel->setText(QString::number(myMaxTol));

    if (GEOMUtils::CompareToleranceValues(myMinTol, myTolEdit->value()) == 1) {
      clickOnResetToMin();
    } else if (GEOMUtils::CompareToleranceValues
                                         (myMaxTol, myTolEdit->value()) == -1) {
      clickOnResetToMax();
    } else {
      onFilterData();
    }
  }
}

//=================================================================================
// function : onFilterData()
// purpose  : filter objects in the filtered tree
//=================================================================================
void RepairGUI_InspectObjectDlg::onFilterData()
{
  TreeWidgetItem *aMainItem =
          dynamic_cast<TreeWidgetItem*>(myFilteredTreeObjects->topLevelItem(0));

  if (!aMainItem) {
    return;
  }

  SALOME_ListIO           aListOfIOToHide;
  QTreeWidgetItemIterator anIt(aMainItem);
  const int               aCompValue =
         myComparisonCompo->itemData(myComparisonCompo->currentIndex()).toInt();
  const double            aTolValue  = myTolEdit->value();

  while (*anIt) {
    TreeWidgetItem* anItem = dynamic_cast<TreeWidgetItem*>(*anIt);

    if (aMainItem != anItem) {
      const bool isToFilter = !GEOMUtils::IsFitCondition
        ((GEOMUtils::ComparisonCondition) aCompValue,
         anItem->getTolerance(), aTolValue);

      if (isToFilter && !anItem->isHidden()) {
        if (anItem->isVisible()) {
          aListOfIOToHide.Append(anItem->getIO());
        }

        anItem->setVisible(false, myInvisible);
      }

      anItem->setHidden(isToFilter);
    }

    ++anIt;
  }

  if (!aListOfIOToHide.IsEmpty()) {
    getDisplayer()->Erase(aListOfIOToHide);
    getDisplayer()->UpdateViewer();
  }

  checkVisibleIcon();
}

//=================================================================================
// function : displayItem()
// purpose  : display sub-object of inspected object according its tree item
//=================================================================================
void RepairGUI_InspectObjectDlg::displayItem( TreeWidgetItem* theItem )
{
  GEOM_Displayer* aDisplayer = getDisplayer();
  if ( theItem == myCurrentTreeObjects->topLevelItem(0) ) {
    aDisplayer->UnsetColor();
    aDisplayer->UnsetWidth();
  }
  else if ( aDisplayer->GetColor() != Quantity_NOC_VIOLET && aDisplayer->GetWidth() != 2.0 ) {
    aDisplayer->SetColor( Quantity_NOC_VIOLET );
    aDisplayer->SetWidth( 2.0 );
  }

  SALOME_Prs* aPrs = aDisplayer->buildSubshapePresentation( theItem->getShape(), theItem->getIO()->getEntry(),
                                                            GEOM_Displayer::GetActiveView() );
  if ( aPrs )
    displayPreview( aPrs, true, false );
}

//=================================================================================
// function : setItemDisplayStatus()
// purpose  : set visible or invisible status for the same items in the tree
//=================================================================================
void RepairGUI_InspectObjectDlg::setItemDisplayStatus( TreeWidgetItem* theItem, bool theIsVisible )
{
  QTreeWidgetItemIterator it( myCurrentTreeObjects );
  while (*it) {
    TreeWidgetItem* anItem = dynamic_cast<TreeWidgetItem*>(*it);
    if ( anItem->getShape().IsSame( theItem->getShape() ) )
      anItem->setVisible( theIsVisible, theIsVisible ? myVisible : myInvisible );
    ++it;
  }
}

//=================================================================================
// function : setMainObjectTransparency()
// purpose  : set transparency for the inspected object
//=================================================================================
void RepairGUI_InspectObjectDlg::setMainObjectTransparency( double theTransparency )
{
  SUIT_ViewManager* aViewMan = myViewWindow->getViewManager();
  SALOME_View* aView = dynamic_cast<SALOME_View*>( aViewMan->getViewModel() );
  SalomeApp_Study* aStudy = dynamic_cast<SalomeApp_Study*>( myGeomGUI->getApp()->activeStudy() );

  TreeWidgetItem* aMainItem = dynamic_cast<TreeWidgetItem*>( myCurrentTreeObjects->topLevelItem(0) );

  if (!aMainItem) {
    return;
  }

  aStudy->setObjectProperty( myViewWindow->getViewManager()->getGlobalId(),
                             QString( aMainItem->getIO()->getEntry() ),
                             GEOM::propertyName( GEOM::Transparency ), theTransparency );

  if ( aView->isVisible( aMainItem->getIO() ) )
    getDisplayer()->Redisplay( aMainItem->getIO(), true, aView );
}

//=================================================================================
// function : restoreParam()
// purpose  : restore initial parameters of the dialog and the viewer
//=================================================================================
void RepairGUI_InspectObjectDlg::restoreParam()
{
  SUIT_ViewManager* aViewMan = myViewWindow->getViewManager();
  SALOME_View* aView = dynamic_cast<SALOME_View*>( aViewMan->getViewModel() );
  GEOM_Displayer* aDisplayer = getDisplayer();
  // restore initial parameters for viewer
  aDisplayer->UnsetColor();
  aDisplayer->UnsetWidth();

  // restore transparency of main object
  setMainObjectTransparency( myTransparency );

  // erase sub-shapes
  TreeWidgetItem* aMainItem = dynamic_cast<TreeWidgetItem*>( myCurrentTreeObjects->topLevelItem(0) );
  QTreeWidgetItemIterator it( myCurrentTreeObjects );
  while (*it) {
    if ( *it != aMainItem ) {
      TreeWidgetItem* anItem = dynamic_cast<TreeWidgetItem*>(*it);
      aDisplayer->Erase( anItem->getIO(), false, false, aView );
      anItem->setVisible( false, myInvisible );
    }
    ++it;
  }
}

//=================================================================================
// function : onEditMainShape()
// purpose  : called when selection button was clicked
//=================================================================================
void RepairGUI_InspectObjectDlg::onEditMainShape()
{
  if ( myEditMainShape->text().isEmpty() || !myViewWindow )
    return;

  restoreParam();

  // restore initial parameters for dialog box
  myEditMainShape->setEnabled( true );
  myEditMainShape->setText("");
  myEditMainShape->setFocus();
  myTreeObjects->clear();
  myFilteredTreeObjects->clear();
}

//=================================================================================
// function : onItemClicked()
// purpose  : called when tree item was clicked
//=================================================================================
void RepairGUI_InspectObjectDlg::onItemClicked( QTreeWidgetItem* theItem, int theColumn )
{
  if ( theColumn!= 1 || !( theItem->flags() & Qt::ItemIsSelectable ) || !myViewWindow )
    return;

  GEOM_Displayer* aDisplayer = getDisplayer();

  TreeWidgetItem* anItem = dynamic_cast<TreeWidgetItem*>( theItem );

  if ( anItem->isVisible() ) {
    aDisplayer->Erase( anItem->getIO(), false, true );
    setItemDisplayStatus( anItem, false );
  }
  else {
    displayItem( anItem );
    setItemDisplayStatus( anItem, true );
  }
  aDisplayer->UpdateViewer();
  checkVisibleIcon();
}

//=================================================================================
// function : onItemChanged()
// purpose  : called when tree item was changed
//=================================================================================
void RepairGUI_InspectObjectDlg::onItemChanged( QTreeWidgetItem* theItem, int theColumn )
{
  if ( theColumn!= 0 || !( theItem->flags() & Qt::ItemIsEditable ) )
    return;

  // rename the same items in the tree
  QTreeWidgetItemIterator it( myCurrentTreeObjects );
  while ( *it ) {
    TreeWidgetItem* anItem = dynamic_cast<TreeWidgetItem*>(*it);
    if ( anItem->getShape().IsSame( dynamic_cast<TreeWidgetItem*>( theItem )->getShape() ) )
      anItem->setText( 0, theItem->text(0) );
    ++it;
  }
}

//=================================================================================
// function : onItemExpanded()
// purpose  : called when tree item was expanded
//=================================================================================
void RepairGUI_InspectObjectDlg::onItemExpanded( QTreeWidgetItem* theItem )
{
  TreeWidgetItem* aMainItem = dynamic_cast<TreeWidgetItem*>( myCurrentTreeObjects->topLevelItem(0) );
  GEOM::GEOM_Object_var aMainObject = GEOMBase::ConvertIOinGEOMObject( aMainItem->getIO() );

  for ( int i = 0; i < theItem->childCount(); i++ ) {
    TreeWidgetItem* aSubItem = dynamic_cast<TreeWidgetItem*>( theItem->child(i) );
    int anIndex = GEOMBase::GetIndex( aSubItem->getShape(), aMainItem->getShape() );
    if ( aSubItem->text(0).isEmpty() ) {
      char* aName = aMainObject->GetSubShapeName( anIndex );
      if ( !QString( aName ).isEmpty() )
        aSubItem->setText( 0, QString( aName ) );
      else
        aSubItem->setText( 0, QString("%1_%2").arg( GEOMBase::TypeName( aSubItem->getShape().ShapeType(), true ) ).arg( anIndex ) );
    }
  }
}

//=================================================================================
// function : onItemSelectionChanged()
// purpose  : called when tree item was selected
//=================================================================================
void RepairGUI_InspectObjectDlg::onItemSelectionChanged()
{
  if ( !myViewWindow )
    return;

  QList<QTreeWidgetItem*> listItem = myCurrentTreeObjects->selectedItems();
  SALOME_ListIO aSelected;
  for ( int i = 0; i < listItem.size(); i++ ) {
    TreeWidgetItem* anItem = dynamic_cast<TreeWidgetItem*>( listItem.at(i) );
    aSelected.Append( anItem->getIO() );
  }
  myGeomGUI->getApp()->selectionMgr()->setSelectedObjects( aSelected );
}

//=================================================================================
// function : onHeaderClicked()
// purpose  : called when header item of tree was clicked
//=================================================================================
void RepairGUI_InspectObjectDlg::onHeaderClicked( int theColumn )
{
  if ( theColumn != 1 || !myViewWindow )
    return;

  GEOM_Displayer* aDisplayer = getDisplayer();

  if ( myIsSelectAll ) {
    myIsSelectAll = false;
    myCurrentTreeObjects->headerItem()->setIcon( 1, myInvisible );
    SALOME_ListIO aListOfIO;
    QTreeWidgetItemIterator it( myCurrentTreeObjects );
    while ( *it ) {
      TreeWidgetItem* anItem = dynamic_cast<TreeWidgetItem*>(*it);
      if ( !anItem->isHidden() && ( anItem->flags() & Qt::ItemIsSelectable ) &&
          anItem->isVisible() ) {
        aListOfIO.Append( anItem->getIO() );
        anItem->setVisible( false, myInvisible );
      }
      ++it;
    }
    aDisplayer->Erase( aListOfIO );
  }
  else {
    myIsSelectAll = true;
    myCurrentTreeObjects->headerItem()->setIcon( 1, myVisible );
    QTreeWidgetItemIterator it( myCurrentTreeObjects );
    while ( *it ) {
      TreeWidgetItem* anItem = dynamic_cast<TreeWidgetItem*>(*it);
      if ( !anItem->isHidden() &&  ( anItem->flags() & Qt::ItemIsSelectable ) &&
           !anItem->isVisible() ) {
        displayItem( anItem );
        anItem->setVisible( true, myVisible );
      }
      ++it;
    }
  }

  aDisplayer->UpdateViewer();
}

//=================================================================================
// function : onViewSelectionChanged()
// purpose  : called when selection of object browser was changed
//=================================================================================
void RepairGUI_InspectObjectDlg::onViewSelectionChanged()
{
  if (!myEditMainShape->isEnabled())
    return;

  //get shape from selection
  SALOME_ListIO selected;
  myGeomGUI->getApp()->selectionMgr()->selectedObjects(selected);

  if ( selected.Extent() != 1 )
    return;

  if ( !myViewWindow ) {
    SUIT_ViewManager* occVm = myGeomGUI->getApp()->getViewManager( OCCViewer_Viewer::Type(), true );
    myViewWindow = occVm->getActiveView();
    connect( occVm, SIGNAL( deleteView( SUIT_ViewWindow* ) ),
             this, SLOT( onCloseView( SUIT_ViewWindow* ) ), Qt::UniqueConnection );
  }

  TopoDS_Shape aShape = GEOMBase::GetTopoFromSelection( selected );
  if ( aShape.IsNull() )
    return;

  Handle(SALOME_InteractiveObject) anIO = selected.First();
  GEOM::GEOM_Object_var anObject = GEOMBase::ConvertIOinGEOMObject( anIO );
  QString aName = anObject->GetName();
  CORBA::String_var anEntry = anObject->GetStudyEntry();

  myEditMainShape->setText( aName );
  myEditMainShape->setEnabled( false );

  // remember initial transparency value
  SalomeApp_Study* aStudy = dynamic_cast<SalomeApp_Study*>( myGeomGUI->getApp()->activeStudy() );
  QVariant v = aStudy->getObjectProperty( myViewWindow->getViewManager()->getGlobalId(),
                                          QString( anEntry.in() ),
                                          GEOM::propertyName( GEOM::Transparency ), myTransparency );
  if ( v.canConvert( QVariant::Double ) )
    myTransparency = v.toDouble();

  TreeWidgetItem* anItem         = new TreeWidgetItem
              (myTreeObjects, QStringList() << aName, aShape, anIO);
  TreeWidgetItem* anItemFiltered = new TreeWidgetItem
              (myFilteredTreeObjects, QStringList() << aName, aShape, anIO);

  if ( getDisplayer()->IsDisplayed( anEntry.in() ) ) {
    anItem->setVisible( true, myVisible );
    anItemFiltered->setVisible( true, myVisible );
  } else {
    anItem->setVisible( false, myInvisible );
    anItemFiltered->setVisible( false, myInvisible );
  }

  setMainObjectTransparency( 0.5 );

  // add sub-objects in the tree
  TopTools_IndexedMapOfShape anIndices;

  TopExp::MapShapes(aShape, anIndices);
  addSubObjects(anItem, anIndices);
  onInitFilteredData();
  updateViewer(false);

  // check icon for tree header
  checkVisibleIcon();
}

//=================================================================================
// function : onWindowActivated()
// purpose  : called when other window was activated
//=================================================================================
void RepairGUI_InspectObjectDlg::onWindowActivated( SUIT_ViewWindow* theViewWindow )
{
  if ( myViewWindow )
    restoreParam();

  connect( theViewWindow->getViewManager(), SIGNAL( deleteView( SUIT_ViewWindow* ) ),
           this, SLOT( onCloseView( SUIT_ViewWindow* ) ), Qt::UniqueConnection );

  if ( theViewWindow->getViewManager()->getType() != OCCViewer_Viewer::Type() &&
       theViewWindow->getViewManager()->getType() != SVTK_Viewer::Type() ) {
    myViewWindow = 0;
    return;
  }
  myViewWindow = theViewWindow;

  if ( myCurrentTreeObjects->topLevelItemCount() > 0 ) {
    setMainObjectTransparency( 0.5 );
    TreeWidgetItem* aMainItem = dynamic_cast<TreeWidgetItem*>( myCurrentTreeObjects->topLevelItem(0) );
    if ( getDisplayer()->IsDisplayed( aMainItem->getIO()->getEntry() ) )
      aMainItem->setVisible( true, myVisible );
    else
      aMainItem->setVisible( false, myInvisible );
  }
  checkVisibleIcon();
}

//=================================================================================
// function : onCloseView()
// purpose  : called when last view was closed
//=================================================================================
void RepairGUI_InspectObjectDlg::onCloseView( SUIT_ViewWindow* )
{
  if ( myGeomGUI->getApp()->desktop()->windows().size() == 0 ) {
    restoreParam();
    myViewWindow = 0;
  }
}

//=================================================================================
// function : clickOnShow()
// purpose  : called when "Show selected" button was clicked
//=================================================================================
void RepairGUI_InspectObjectDlg::clickOnShow()
{
  if ( !myViewWindow )
    return;

  QList<QTreeWidgetItem*> listItem = myCurrentTreeObjects->selectedItems();
  for ( int i = 0; i < listItem.size(); i++ ) {
    TreeWidgetItem* anItem = dynamic_cast<TreeWidgetItem*>( listItem.at(i) );
    if ( !anItem->isVisible() ) {
      displayItem( anItem );
      setItemDisplayStatus( anItem, true );
    }
  }
  getDisplayer()->UpdateViewer();
  checkVisibleIcon();
}

//=================================================================================
// function : clickOnShowOnly()
// purpose  : called when "Show only selected" button was clicked
//=================================================================================
void RepairGUI_InspectObjectDlg::clickOnShowOnly()
{
  if ( !myViewWindow )
    return;

  SALOME_ListIO aListOfIO;
  QTreeWidgetItemIterator it( myCurrentTreeObjects );
  while ( *it ) {
    TreeWidgetItem* anItem = dynamic_cast<TreeWidgetItem*>(*it);
    if ( !anItem->isHidden() && ( anItem->flags() & Qt::ItemIsSelectable ) &&
         anItem->isVisible() ) {
      aListOfIO.Append( anItem->getIO() );
      anItem->setVisible( false, myInvisible );
    }
    ++it;
  }
  getDisplayer()->Erase( aListOfIO );

  clickOnShow();
}

//=================================================================================
// function : clickOnHide()
// purpose  : called when "Hide selected" button was clicked
//=================================================================================
void RepairGUI_InspectObjectDlg::clickOnHide()
{
  if ( !myViewWindow )
    return;

  QList<QTreeWidgetItem*> listItem = myCurrentTreeObjects->selectedItems();
  for ( int i = 0; i < listItem.size(); i++ ) {
    TreeWidgetItem* anItem = dynamic_cast<TreeWidgetItem*>( listItem.at(i) );
    if ( anItem->isVisible() ) {
      getDisplayer()->Erase( anItem->getIO(), false, false );
      setItemDisplayStatus( anItem, false );
    }
  }
  getDisplayer()->UpdateViewer();
  checkVisibleIcon();
}

//=================================================================================
// function : clickOnPublish()
// purpose  : called when "Publish selected" button was clicked
//=================================================================================
void RepairGUI_InspectObjectDlg::clickOnPublish()
{
  _PTR(Study) studyDS = dynamic_cast<SalomeApp_Study*>( myGeomGUI->getApp()->activeStudy() )->studyDS();

  // find main object
  TreeWidgetItem* aMainItem = dynamic_cast<TreeWidgetItem*>( myCurrentTreeObjects->topLevelItem(0) );

  if (!aMainItem) {
    return;
  }

  GEOM::GEOM_Object_var aMainObject = GEOMBase::ConvertIOinGEOMObject( aMainItem->getIO() );

  // find unique indices of selected objects
  QList<QTreeWidgetItem*> selectedItems = myCurrentTreeObjects->selectedItems();
  QMap< int, QString > anIndices;
  GEOM::ListOfLong_var anArray = new GEOM::ListOfLong;
  anArray->length( selectedItems.size() );
  int j = 0;
  for ( int i = 0; i < selectedItems.size(); i++ ) {
    TreeWidgetItem* anItem = dynamic_cast<TreeWidgetItem*>( selectedItems.at(i) );
    int anIndex = GEOMBase::GetIndex( anItem->getShape(), aMainItem->getShape() );
    if ( anIndices.find( anIndex ) == anIndices.end() &&
    	anItem != aMainItem ) {
      anIndices[ anIndex ] = anItem->text(0);
      anArray[j++] = anIndex;
    }
  }
  anArray->length(j);

  // get selected sub-shapes
  GEOM::GEOM_IShapesOperations_var anOper = getGeomEngine()->GetIShapesOperations( getStudyId() );
  GEOM::ListOfGO_var aList = anOper->MakeSubShapes( aMainObject, anArray );

  // publish sub-shapes
  for ( int i = 0; i < aList->length(); i++ )
    GeometryGUI::GetGeomGen()->AddInStudy( GeometryGUI::ClientStudyToStudy( studyDS ), aList[i],
                                           anIndices.values().at(i).toStdString().c_str(), aMainObject );

  updateObjBrowser();
}

//=================================================================================
// function : clickOnHelp()
// purpose  : called when Help button was clicked to open a help page
//=================================================================================
void RepairGUI_InspectObjectDlg::clickOnHelp()
{
  myGeomGUI->getApp()->onHelpContextModule( "GEOM", "inspect_object_operation_page.html" );
}

//=================================================================================
// function : clickOnResetToMin()
// purpose  : called when Reset button was clicked to reset tolerance filter to minimal value.
//=================================================================================
void RepairGUI_InspectObjectDlg::clickOnResetToMin()
{
  if (myMinTol >= myTolEdit->minimum() && myMinTol <= myTolEdit->maximum()) {
    myTolEdit->setValue(myMinTol);
  }
}

//=================================================================================
// function : clickOnResetToMax()
// purpose  : called when Reset button was clicked to reset tolerance filter to maximal value.
//=================================================================================
void RepairGUI_InspectObjectDlg::clickOnResetToMax()
{
  if (myMaxTol >= myTolEdit->minimum() && myMaxTol <= myTolEdit->maximum()) {
    myTolEdit->setValue(myMaxTol);
  }
}

//=================================================================================
// function : clickOnShowAll()
// purpose  : called when Help button was clicked to show all shapes
//=================================================================================
void RepairGUI_InspectObjectDlg::clickOnShowAll()
{
  myIsSelectAll = false;
  onHeaderClicked(1);
}

//=================================================================================
// function : clickOnHideAll()
// purpose  : called when Help button was clicked to hide all shapes
//=================================================================================
void RepairGUI_InspectObjectDlg::clickOnHideAll()
{
  myIsSelectAll = true;
  onHeaderClicked(1);
}

//=================================================================================
// function : DeactivateActiveDialog()
// purpose  :
//=================================================================================
void RepairGUI_InspectObjectDlg::DeactivateActiveDialog()
{
  setEnabled(false);
  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
  myGeomGUI->SetActiveDialogBox(0);
  globalSelection();
  erasePreview();
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void RepairGUI_InspectObjectDlg::ActivateThisDialog()
{
  /* Emit a signal to deactivate the active dialog */
  myGeomGUI->EmitSignalDeactivateDialog();
  setEnabled(true);
  myGeomGUI->SetActiveDialogBox( (QDialog*)this );

  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(onViewSelectionChanged()));

  updateViewer(false);
}

//=================================================================================
// function : onFilterToggled()
// purpose  :
//=================================================================================
void RepairGUI_InspectObjectDlg::onFilterToggled(bool isOn)
{
  if (isOn) {
    myCurrentTreeObjects = myFilteredTreeObjects;
    myTreesLayout->setCurrentIndex(1);
  } else {
    myCurrentTreeObjects = myTreeObjects;
    myTreesLayout->setCurrentIndex(0);
  }

  updateViewer(true);
}

//=================================================================================
// function : updateViewer()
// purpose  :
//=================================================================================
void RepairGUI_InspectObjectDlg::updateViewer(const bool theIsHideOtherTree)
{
  GEOM_Displayer *aDisplayer  = getDisplayer();

  if (theIsHideOtherTree) {
    QTreeWidget    *aTreeToHide = myCurrentTreeObjects == myTreeObjects ?
                                          myFilteredTreeObjects: myTreeObjects;

    // Hide the objects of disappeared tree, do not switch off flags.
    SALOME_ListIO           aListOfIO;
    QTreeWidgetItemIterator it(aTreeToHide);

    while (*it) {
      TreeWidgetItem* anItem = dynamic_cast<TreeWidgetItem*>(*it);
      if ((anItem->flags() & Qt::ItemIsSelectable) &&
          anItem->isVisible() && !anItem->isHidden()) {
        aListOfIO.Append(anItem->getIO());
      }

      ++it;
    }

    aDisplayer->Erase(aListOfIO);
  }

  // Show the objects that are marked as shown in the appeared tree.
  QTreeWidgetItemIterator it2(myCurrentTreeObjects);

  while (*it2) {
    TreeWidgetItem* anItem = dynamic_cast<TreeWidgetItem*>(*it2);
    if ((anItem->flags() & Qt::ItemIsSelectable) &&
        anItem->isVisible() && !anItem->isHidden()) {
      displayItem(anItem);
    }

    ++it2;
  }

  aDisplayer->UpdateViewer();
}
