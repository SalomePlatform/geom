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
// File   : EntityGUI_SubShapeDlg.cxx
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.

#include "EntityGUI_SubShapeDlg.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>
#include <GEOMUtils.hxx>
#ifndef DISABLE_PLOT2DVIEWER
  #include <MeasureGUI_ShapeStatisticsDlg.h>
#endif

#include <OCCViewer_ViewModel.h>
#include <SVTK_ViewModel.h>
#include <SalomeApp_Study.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>
#include <SALOME_ListIO.hxx>
#include <SUIT_MessageBox.h>

#include <SUIT_Desktop.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SUIT_ViewManager.h>
#include <SUIT_ViewWindow.h>

#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#include <TColStd_IndexedMapOfInteger.hxx>

#include <GEOMImpl_Types.hxx>

namespace
{
  const char* const ShapeTypes [] = {
    "Compound",
    "Compsolid",
    "Solid",
    "Shell",
    "Face",
    "Wire",
    "Edge",
    "Vertex",
    "Shape",
    "Flat"
  };

  enum { Filter_LT, Filter_LE, Filter_GT, Filter_GE };

  unsigned int NumberOfSubShapes(const TopoDS_Shape& S, const int shapeType, TopTools_MapOfShape& M)
  {
    unsigned int index = 0;

    if (!S.IsNull()) {
      if (S.ShapeType() == TopAbs_COMPOUND &&
          (shapeType == TopAbs_SHAPE || shapeType == TopAbs_FLAT || shapeType == TopAbs_COMPOUND)) {
        TopoDS_Iterator It(S, Standard_True, Standard_True);
        for (; It.More(); It.Next()) {
          TopoDS_Shape SS = It.Value();
          if (M.Add(SS)) {
            if (shapeType == TopAbs_FLAT) {
              if (SS.ShapeType() != TopAbs_COMPOUND)
                index++;
              else
                index += NumberOfSubShapes(SS, shapeType, M);
            }
            else if (shapeType == TopAbs_SHAPE || shapeType == SS.ShapeType()) {
              index++;
            }
          }
        }
      }
      else {
        TopExp_Explorer Exp (S, TopAbs_ShapeEnum(shapeType));
        for (; Exp.More(); Exp.Next()) {
          if (M.Add(Exp.Current())) {
            index++;
          }
        }
      }
    }
    return index;
  }
  unsigned int NumberOfSubShapes(const TopoDS_Shape& S, const int shapeType)
  {
    TopTools_MapOfShape M;
    return NumberOfSubShapes(S, shapeType, M);
  }
}

//=================================================================================
// class    : EntityGUI_SubShapeDlg
// purpose  : Constructs a EntityGUI_SubShapeDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
EntityGUI_SubShapeDlg::EntityGUI_SubShapeDlg(GeometryGUI* theGeometryGUI, QWidget* parent,
                                              bool modal, Qt::WindowFlags fl)
  : GEOMBase_Skeleton(theGeometryGUI, parent, modal, fl),
    myDmMode( -1 ),
    myIsHiddenMain(false)
{
  QPixmap image0(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_SUBSHAPE")));
  QPixmap image1(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_SELECT")));

  setWindowTitle(tr("GEOM_SUBSHAPE_TITLE"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_SUB_SHAPE"));
  mainFrame()->RadioButton1->setIcon(image0);
  mainFrame()->RadioButton2->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton3->close();

  GroupPoints = new DlgRef_1Sel1List1Check3Btn(centralWidget());

  GroupPoints->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  GroupPoints->TextLabel1->setText(tr("GEOM_MAIN_OBJECT"));
  GroupPoints->TextLabel2->setText(tr("GEOM_SUBSHAPE_TYPE"));
  GroupPoints->CheckButton1->setText(tr("GEOM_SUBSHAPE_SELECT"));
  GroupPoints->PushButton1->setIcon(image1);
  GroupPoints->PushButton2->setText(tr("SHOW_ONLY_SELECTED"));
  GroupPoints->PushButton3->setText(tr("HIDE_SELECTED"));
  GroupPoints->PushButton4->setText(tr("SHOW_ALL_SUB_SHAPES"));
  GroupPoints->LineEdit1->setReadOnly(true);

  //filter group

  myFilterGrp = new QGroupBox(tr("GEOM_FILTER"), centralWidget());
  myLessFilterCheck = new QCheckBox(myFilterGrp);
  myLessFilterCombo = new QComboBox(myFilterGrp);
  myLessFilterCombo->addItem( tr("GEOM_LESS_THAN"), Filter_LT );
  myLessFilterCombo->addItem( tr("GEOM_LESSOREQUAL_THAN"), Filter_LE );
  myGreaterFilterCheck = new QCheckBox(myFilterGrp);
  myGreaterFilterCombo = new QComboBox(myFilterGrp);
  myGreaterFilterCombo->addItem( tr("GEOM_GREAT_THAN"), Filter_GT );
  myGreaterFilterCombo->addItem( tr("GEOM_GREATOREQUAL_THAN"), Filter_GE );
  myLessFilterSpin = new SalomeApp_DoubleSpinBox(myFilterGrp);
  myGreaterFilterSpin = new SalomeApp_DoubleSpinBox(myFilterGrp);
  myApplyFilterButton = new QPushButton(tr("GEOM_BUT_APPLY"), myFilterGrp);
#ifndef DISABLE_PLOT2DVIEWER
  myPlotDistributionButton = new QPushButton(tr("GEOM_PLOT_DISTRIBUTION"), myFilterGrp);
#endif

  QGridLayout* filterLayout = new QGridLayout(myFilterGrp);
  filterLayout->addWidget(myLessFilterCheck,    0, 0);
  filterLayout->addWidget(myLessFilterCombo,    0, 1);
  filterLayout->addWidget(myLessFilterSpin,     0, 2);
  filterLayout->addWidget(myGreaterFilterCheck, 1, 0);
  filterLayout->addWidget(myGreaterFilterCombo, 1, 1);
  filterLayout->addWidget(myGreaterFilterSpin,  1, 2);
  filterLayout->addWidget(myApplyFilterButton,  0, 3);
#ifndef DISABLE_PLOT2DVIEWER
  filterLayout->addWidget(myPlotDistributionButton,  1, 3);
#endif

  QVBoxLayout* layout = new QVBoxLayout(centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
  layout->addWidget(GroupPoints);
  layout->addWidget(myFilterGrp);
  /***************************************************************/

  setIsOptimizedBrowsing(true);

  setHelpFileName("create_explode_page.html");

  mainFrame()->GroupBoxName->hide();

  Init();
}

//=================================================================================
// function : ~EntityGUI_SubShapeDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
EntityGUI_SubShapeDlg::~EntityGUI_SubShapeDlg()
{
  if (myIsHiddenMain) {
    GEOM_Displayer* aDisplayer = getDisplayer();
    aDisplayer->Display(myObject);
    myIsHiddenMain = false;
  }
  myDmMode = -1;
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void EntityGUI_SubShapeDlg::Init()
{
  // Get setting of step value from file configuration
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  double step = resMgr->doubleValue("Geometry", "SettingsGeomStep", 100);

  // min, max, step and decimals for spin boxes
  initSpinBox(myLessFilterSpin, 0., COORD_MAX, step, "length_precision" );
  initSpinBox(myGreaterFilterSpin, 0., COORD_MAX, step, "length_precision" );
  myLessFilterSpin->setValue( 0. );
  myGreaterFilterSpin->setValue( 0. );

  /* init variables */
  myDmMode = -1;
  myEditCurrentArgument = GroupPoints->LineEdit1;
  myObject = GEOM::GEOM_Object::_nil();

  /* type for sub-shape selection */
  for ( int i = 0; i <= (int)GEOM::FLAT; i++ )
    GroupPoints->ComboBox1->addItem(ShapeTypes[i], i);

  if (SUIT_Session::session()->activeApplication()->desktop()->activeWindow()->getViewManager()->getType()
      != OCCViewer_Viewer::Type())
    GroupPoints->CheckButton1->setEnabled(false);

  /* signals and slots connections */
  connect(myGeomGUI, SIGNAL(SignalDeactivateActiveDialog()), this, SLOT(DeactivateActiveDialog()));

  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(GroupPoints->PushButton1,  SIGNAL(clicked()),       this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->LineEdit1,    SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(GroupPoints->ComboBox1,    SIGNAL(activated(int)),    this, SLOT(ComboTextChanged()));
  connect(GroupPoints->CheckButton1, SIGNAL(stateChanged(int)), this, SLOT(SubShapeToggled()));

  connect(GroupPoints->PushButton2, SIGNAL(clicked()), this, SLOT(showOnlySelected()));
  connect(GroupPoints->PushButton3, SIGNAL(clicked()), this, SLOT(showOnlySelected()));
  connect(GroupPoints->PushButton4, SIGNAL(clicked()), this, SLOT(showOnlySelected()));

  connect(myApplyFilterButton, SIGNAL(clicked()),         this, SLOT(ClickOnOkFilter()));
#ifndef DISABLE_PLOT2DVIEWER
  connect(myPlotDistributionButton, SIGNAL(clicked()),    this, SLOT(ClickOnPlot()));
#endif
  connect(myLessFilterCheck,   SIGNAL(stateChanged(int)), this, SLOT(MeasureToggled()));
  connect(myGreaterFilterCheck,   SIGNAL(stateChanged(int)), this, SLOT(MeasureToggled()));

  connect(myGeomGUI->getApp()->selectionMgr(),
          SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  updateButtonState();
  resize(100,100);
  SelectionIntoArgument();
  SubShapeToggled();
  MeasureToggled();
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void EntityGUI_SubShapeDlg::enterEvent(QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void EntityGUI_SubShapeDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(myGeomGUI->getApp()->selectionMgr(),
           SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
  SubShapeToggled();
  updateButtonState();
}

//=================================================================================
//function : closeEvent
//purpose  : remove temporary geom object
//=================================================================================
void EntityGUI_SubShapeDlg::closeEvent(QCloseEvent* e)
{
  erasePreview(true);
  GEOMBase_Skeleton::closeEvent(e);
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void EntityGUI_SubShapeDlg::ClickOnOk()
{
  setIsApplyAndClose(true);

  SUIT_Session::session()->activeApplication()->putInfo("");

  /* Explode all sub-shapes */
  bool isOk = true;
  if (isAllSubShapes()) {
    /* More than 30 sub-shapes : ask confirmation */
    unsigned int nb = NumberOfSubShapes(myShape, shapeType());
    if (nb > 30) {
      if (SUIT_MessageBox::question( this,
                                     tr("GEOM_CONFIRM"),
                                     tr("GEOM_CONFIRM_INFO").arg(nb),
                                     tr("GEOM_BUT_EXPLODE"),
                                     tr("GEOM_BUT_CANCEL") ) != 0 )
        isOk = false;  /* aborted */
    }
  }

  if (isOk)
    isOk = onAccept( true, true, false );

  if (isOk)
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool EntityGUI_SubShapeDlg::ClickOnApply()
{
  SUIT_Session::session()->activeApplication()->putInfo("");

  /* Explode all sub-shapes */
  if (isAllSubShapes()) {
    /* More than 30 sub-shapes : ask confirmation */
    unsigned int nb = NumberOfSubShapes(myShape, shapeType());
    if (nb > 30) {
      if (SUIT_MessageBox::question( this,
                                     tr("GEOM_CONFIRM"),
                                     tr("GEOM_CONFIRM_INFO").arg(nb),
                                     tr("GEOM_BUT_EXPLODE"),
                                     tr("GEOM_BUT_CANCEL") ) != 0 )
        return false;  /* aborted */
    }
  }
  
  setIsDisableBrowsing( true );
  bool isOk = onAccept( true, true, false );
  setIsDisableBrowsing( false );

  // restore selection, corresponding to current selection mode
  SubShapeToggled();

  return isOk;
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection has changed or other case
//=================================================================================
void EntityGUI_SubShapeDlg::SelectionIntoArgument()
{
  if (!isAllSubShapes())
    return;

  int currentType = GroupPoints->ComboBox1->itemData( GroupPoints->ComboBox1->currentIndex() ).toInt();

  ResetStateOfDialog();

  QString aString = ""; /* name of selection */

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  int nbSel = GEOMBase::GetNameOfSelectedIObjects(aSelList, aString, true);
  if (nbSel == 1) {
    Handle(SALOME_InteractiveObject) IO = aSelList.First();
    if (!IO->hasEntry()) {
      SUIT_Session::session()->activeApplication()->putInfo(tr("GEOM_PRP_SHAPE_IN_STUDY"));
    }
    else {
      TopoDS_Shape S = GEOMBase::GetTopoFromSelection(aSelList);
      if (S.IsNull() || S.ShapeType() == TopAbs_VERTEX) {
        myObject = GEOM::GEOM_Object::_nil();
      }
      else {
        myObject = GEOMBase::ConvertIOinGEOMObject(IO);
        if (!CORBA::is_nil(myObject)) {
          myShape = S;
          GroupPoints->LineEdit1->setText(aString);
          int i = 0;
          // Solving PAL5590
          if (myShape.ShapeType() == TopAbs_COMPOUND) {
            unsigned int nb = NumberOfSubShapes(myShape, TopAbs_COMPOUND);
            if (nb > 0)
              i++;
          }
          while (i <= myShape.ShapeType()) {
            GroupPoints->ComboBox1->removeItem(0);
            i++;
          }
          // remove Shape and Flat types for non-compound shapes
          if (myShape.ShapeType() != TopAbs_COMPOUND) {
            int idx = GroupPoints->ComboBox1->findData( (int)GEOM::SHAPE );
            if ( idx != -1 ) GroupPoints->ComboBox1->removeItem( idx );
            idx = GroupPoints->ComboBox1->findData( (int)GEOM::FLAT );
            if ( idx != -1 ) GroupPoints->ComboBox1->removeItem( idx );
          } // if (myShape.ShapeType() != TopAbs_COMPOUND)
        } // if (!CORBA::is_nil(myObject))
      } // if (S.IsNull() || S.ShapeType() == TopAbs_VERTEX)
    } // if (!IO->hasEntry()) ... else
  } // if (nbSel == 1)

  int idx = GroupPoints->ComboBox1->findData( currentType );
  if ( idx != -1 )
    GroupPoints->ComboBox1->setCurrentIndex( idx );

  updateButtonState();
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void EntityGUI_SubShapeDlg::SetEditCurrentArgument()
{
  GroupPoints->LineEdit1->setFocus();
  myEditCurrentArgument = GroupPoints->LineEdit1;

  GroupPoints->CheckButton1->setChecked(false);
  SubShapeToggled();
  SelectionIntoArgument();
}

//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void EntityGUI_SubShapeDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if (send == GroupPoints->LineEdit1)
    SetEditCurrentArgument();
  else
    return;

  GEOMBase_Skeleton::LineEditReturnPressed();
}

//=================================================================================
// function : ResetStateOfDialog()
// purpose  : Completely reset the state of method including local context
//=================================================================================
void EntityGUI_SubShapeDlg::ResetStateOfDialog()
{
  if (myIsHiddenMain) {
    GEOM_Displayer* aDisplayer = getDisplayer();
    aDisplayer->Display(myObject);
    myIsHiddenMain = false;
  }
  myObject = GEOM::GEOM_Object::_nil();
  myShape.Nullify();
  myEditCurrentArgument->setText("");

  /* type for sub-shape selection */
  GroupPoints->ComboBox1->clear();
  for ( int i = 0; i <= (int)GEOM::FLAT; i++ )
    GroupPoints->ComboBox1->addItem(ShapeTypes[i], i);

  updateButtonState();
}

//=================================================================================
// function : SubShapeToggled()
// purpose  : Allow user selection of all or only selected sub-shapes
//          : Called when 'CheckButton1' state change
//=================================================================================
void EntityGUI_SubShapeDlg::SubShapeToggled()
{
  GroupPoints->PushButton2->setEnabled(!isAllSubShapes());
  GroupPoints->PushButton3->setEnabled(!isAllSubShapes());
  GroupPoints->PushButton4->setEnabled(!isAllSubShapes());
  myFilterGrp->setEnabled(GroupPoints->CheckButton1->isEnabled() &&
                          GroupPoints->CheckButton1->isChecked() &&
                          shapeType() < GEOM::VERTEX);

#ifndef DISABLE_PLOT2DVIEWER
  myPlotDistributionButton->setEnabled( myFilterGrp->isEnabled() &&
					( shapeType() == TopAbs_EDGE || 
					  shapeType() == TopAbs_FACE ||
					  shapeType() == TopAbs_SOLID ) );
#endif

  activateSelection();
}

//=================================================================================
// function : ComboTextChanged()
// purpose  :
//=================================================================================
void EntityGUI_SubShapeDlg::ComboTextChanged()
{
  /* Select sub-shapes mode not checked */
  updateButtonState();
  SubShapeToggled();
}

//=================================================================================
// function : updateButtonState
// purpose  :
//=================================================================================
void EntityGUI_SubShapeDlg::updateButtonState()
{
  bool viewOk = SUIT_Session::session()->activeApplication()->desktop()->activeWindow()->getViewManager()->getType() == OCCViewer_Viewer::Type();
  bool shapeTypeOk = shapeType() != TopAbs_SHAPE && shapeType() != TopAbs_FLAT && shapeType() != TopAbs_COMPOUND;
  bool objectOK = !CORBA::is_nil( myObject );

  if ( viewOk && objectOK && shapeTypeOk ) {
    GroupPoints->CheckButton1->setEnabled( true );
  }
  else {
    GroupPoints->CheckButton1->setChecked( false );
    GroupPoints->CheckButton1->setEnabled( false );
  }
  myFilterGrp->setEnabled(GroupPoints->CheckButton1->isEnabled() && GroupPoints->CheckButton1->isChecked());
}

//=================================================================================
// function : isAllSubShapes
// purpose  :
//=================================================================================
bool EntityGUI_SubShapeDlg::isAllSubShapes() const
{
  return !GroupPoints->CheckButton1->isEnabled() || !GroupPoints->CheckButton1->isChecked();
}

//=================================================================================
// function : shapeType
// purpose  :
//=================================================================================
int EntityGUI_SubShapeDlg::shapeType() const
{
  return GroupPoints->ComboBox1->itemData(GroupPoints->ComboBox1->currentIndex()).toInt();
}

//=================================================================================
// function : showOnlySelected
// purpose  :
//=================================================================================
void EntityGUI_SubShapeDlg::showOnlySelected()
{
  if (CORBA::is_nil(myObject) || isAllSubShapes())
    return;

  QPushButton* send = (QPushButton*)sender();
  if (send == GroupPoints->PushButton4) {
    activateSelection();
    return;
  }

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  GEOM_Displayer* aDisplayer = getDisplayer();

  if (send == GroupPoints->PushButton3) {
    aDisplayer->Erase(aSelList, /*forced=*/false, /*updateViewer=*/true);
  }
  else {
    // Mantis issue 0021421: do not hide main shape, if explode on VERTEX
    SALOME_View* view = GEOM_Displayer::GetActiveView();
    if (view) {
      CORBA::String_var aMainEntry = myObject->GetStudyEntry();
      Handle(SALOME_InteractiveObject) io =
        new SALOME_InteractiveObject (aMainEntry.in(), "GEOM", "TEMP_IO");
      if (view->isVisible(io)) myIsHiddenMain = true;

      //keep the selected entry and IO in the map for checking
      std::map<QString, Handle(SALOME_InteractiveObject)> aSelEntriesMap;
      SALOME_ListIteratorOfListIO aSelIt(aSelList);
      for ( ; aSelIt.More(); aSelIt.Next() ) {
        //fill map <entry, io>
        Handle(SALOME_InteractiveObject) anSelIO = aSelIt.Value();
        aSelEntriesMap[anSelIO->getEntry()] = anSelIO;
      }
      //get the displayed sub-shapes
      SALOME_ListIO displayed;
      view->GetVisible(displayed);
      // Erase all, except the selected shapes
      std::map<QString, Handle(SALOME_InteractiveObject)>::iterator 
        aSelDispIter = aSelEntriesMap.end();
      SALOME_ListIteratorOfListIO aDispIt( displayed );
      for ( ; aDispIt.More(); aDispIt.Next() ) {
        Handle(SALOME_InteractiveObject) anIO = aDispIt.Value();
        aSelDispIter = aSelEntriesMap.find( anIO->getEntry() );
        if ( aSelDispIter != aSelEntriesMap.end() ) {
          //sub-shape is selected, so erase it's record from map to keep in it not displayed, but selected sub-shapes only
          aSelEntriesMap.erase(aSelDispIter);
        } else {
          //sub-shape is not in the map of selected, then erase it from view
          aDisplayer->Erase( anIO, /*forced = */false, /*updateViewer = */false );
        }
      }
      
      if ( !aSelEntriesMap.empty() ) {
        // Build a presentation of the selected, but not displayed sub-shapes
        TopTools_IndexedMapOfShape aSubShapesMap;
        TopExp::MapShapes(myShape, aSubShapesMap);
        QString anEntryBase = aMainEntry.in();

        TopExp_Explorer anExp (myShape, (TopAbs_ShapeEnum)shapeType());
        for (; anExp.More(); anExp.Next())
        {
          TopoDS_Shape aSubShape = anExp.Current();
          int index = aSubShapesMap.FindIndex(aSubShape);
          QString anEntry = QString( "TEMP_" ) + anEntryBase + QString("_%1").arg(index);
          if ( aSelEntriesMap.find( anEntry ) == aSelEntriesMap.end() ) {
            //skip not selected sub-shapes
            continue;
          }
          SALOME_Prs* aPrs = aDisplayer->buildSubshapePresentation(aSubShape, anEntry, view);
          if (aPrs) {
            displayPreview(aPrs, true, false); // append, do not update
          }
        }
      }
      aDisplayer->UpdateViewer();
    }

    // Mantis issue 0021421: do not hide main shape, if explode on VERTEX
    if ((TopAbs_ShapeEnum)shapeType() == TopAbs_VERTEX && myIsHiddenMain) {
      aDisplayer->Display(myObject);
    }
  }
}

//=================================================================================
// function : getSelectedSubshapes
// purpose  :
//=================================================================================
int EntityGUI_SubShapeDlg::getSelectedSubshapes (TColStd_IndexedMapOfInteger& theMapIndex)
{
  theMapIndex.Clear();

  SalomeApp_Application* app = myGeomGUI->getApp();
  if (!app || myObject->_is_nil())
    return 0;

  LightApp_SelectionMgr* aSelMgr = app->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  // try to find out and process the global selection
  // (of not published objects and of published sub-shapes)
  {
    SALOME_ListIteratorOfListIO anIter (aSelList);
    for (int i = 0; anIter.More(); anIter.Next(), i++)
    {
      Handle(SALOME_InteractiveObject) anIObj = anIter.Value();
      QString anEntry = anIObj->getEntry();
      QString str = "_";
      int index = anEntry.lastIndexOf(str);
      if (index > 0) // selection among special preview
      {
        anEntry.remove(0, index+1);
        int anIndex = anEntry.toInt();
        if (anIndex)
          theMapIndex.Add(anIndex);
      }
      else // selection among published shapes
      {
        SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>(app->activeStudy());
        if (!appStudy) return 0;
        _PTR(Study) aStudy = appStudy->studyDS();

        _PTR(SObject) aSObj (aStudy->FindObjectID(anEntry.toLatin1().constData()));
        GEOM::GEOM_Object_var aGeomObj =
          GEOM::GEOM_Object::_narrow(GeometryGUI::ClientSObjectToObject(aSObj));
        TopoDS_Shape aShape;
        if (GEOMBase::GetShape(aGeomObj, aShape)) {
          if (aGeomObj->GetType() == GEOM_GROUP || aShape.ShapeType() == (TopAbs_ShapeEnum)shapeType()) {
            TopTools_IndexedMapOfShape aMainMap;
            TopExp::MapShapes(myShape, aMainMap);

            TopExp_Explorer anExp (aShape, (TopAbs_ShapeEnum)shapeType());
            for (; anExp.More(); anExp.Next()) {
              TopoDS_Shape aSubShape = anExp.Current();
              int anIndex = aMainMap.FindIndex(aSubShape);
              if (anIndex >= 0) {
                theMapIndex.Add(anIndex);
              }
            }
          }
        }
      }
    } // for aSelList
  }

  return theMapIndex.Extent();
}

//=================================================================================
// function : activateSelection
// purpose  : Activate selection in accordance with myEditCurrentArgument
//=================================================================================
void EntityGUI_SubShapeDlg::activateSelection()
{
  bool isApply = ((QPushButton*)sender() == buttonApply());
  
  if(!isApply)
    erasePreview(false);

  // local selection
  if (!myObject->_is_nil() && !isAllSubShapes())
  {
    GEOM_Displayer* aDisplayer = getDisplayer();

    //display mode for main shape
    if ( myDmMode == -1 ) {
      SALOME_View* view = GEOM_Displayer::GetActiveView();
      if (view) {
        CORBA::String_var aMainEntry = myObject->GetStudyEntry();
        Handle(SALOME_InteractiveObject) io =
          new SALOME_InteractiveObject (aMainEntry.in(), "GEOM", "TEMP_IO");
        if ( view->isVisible( io ) ) {
          Handle(GEOM_AISShape) aSh = GEOMBase::ConvertIOinGEOMAISShape( io, true );
          if(!aSh.IsNull())
            myDmMode = aSh->isTopLevel() ? aSh->prevDisplayMode() : aSh->DisplayMode();
        }
        else
          myDmMode = SUIT_Session::session()->resourceMgr()->integerValue( "Geometry", "display_mode" );
      }
    }
    // Mantis issue 0021421: do not hide main shape, if explode on VERTEX
    if (shapeType() != TopAbs_VERTEX) {
      aDisplayer->Erase(myObject, false, false);
      myIsHiddenMain = true;
    }
    else
      aDisplayer->Display(myObject);

    aDisplayer->SetDisplayMode(myDmMode);

    if(!isApply) {
      SUIT_ViewWindow* aViewWindow = 0;
      SUIT_Study* activeStudy = SUIT_Session::session()->activeApplication()->activeStudy();
      if (activeStudy)
        aViewWindow = SUIT_Session::session()->activeApplication()->desktop()->activeWindow();
      if (aViewWindow == 0) return;

      SUIT_ViewManager* aViewManager = aViewWindow->getViewManager();
      if (aViewManager->getType() != OCCViewer_Viewer::Type() &&
          aViewManager->getType() != SVTK_Viewer::Type())
        return;

      SUIT_ViewModel* aViewModel = aViewManager->getViewModel();
      SALOME_View* aView = dynamic_cast<SALOME_View*>(aViewModel);
      if (aView == 0) return;

      //TopoDS_Shape aMainShape = GEOM_Client::get_client().GetShape(GeometryGUI::GetGeomGen(), myObject);

      TopTools_IndexedMapOfShape aSubShapesMap;
      TopExp::MapShapes(myShape, aSubShapesMap);
      CORBA::String_var aMainEntry = myObject->GetStudyEntry();
      QString anEntryBase = aMainEntry.in();

      TopExp_Explorer anExp (myShape, (TopAbs_ShapeEnum)shapeType());
      for (; anExp.More(); anExp.Next())
      {
        TopoDS_Shape aSubShape = anExp.Current();
        int index = aSubShapesMap.FindIndex(aSubShape);
        QString anEntry = QString( "TEMP_" ) + anEntryBase + QString("_%1").arg(index);

        SALOME_Prs* aPrs = aDisplayer->buildSubshapePresentation(aSubShape, anEntry, aView);
        if (aPrs) {
          displayPreview(aPrs, true, false); // append, do not update
        }
      }
      aDisplayer->UnsetDisplayMode();
      aDisplayer->UpdateViewer();
    }
  }

  globalSelection(GEOM_ALLSHAPES);
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr EntityGUI_SubShapeDlg::createOperation()
{
  return getGeomEngine()->GetIShapesOperations(getStudyId());
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool EntityGUI_SubShapeDlg::isValid (QString& msg)
{
  bool isOk = false;

  if (myObject->_is_nil()) {
    updateButtonState();
    return isOk;
  }

  if (isAllSubShapes())
    isOk = true;
  else {
    TColStd_IndexedMapOfInteger aMapIndex;
    int nbSel = getSelectedSubshapes(aMapIndex);
    isOk = nbSel > 0;

    if (!isOk)
      msg += tr("NO_SUBSHAPES_SELECTED");
  }

  return isOk;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool EntityGUI_SubShapeDlg::execute (ObjectList& objects)
{
  GEOM::GEOM_IShapesOperations_var anOper = GEOM::GEOM_IShapesOperations::_narrow(getOperation());

  if (!isAllSubShapes()) {
    // manual selection
    TColStd_IndexedMapOfInteger aMapIndex;
    int nbSel = getSelectedSubshapes(aMapIndex);

    if (nbSel > 0) {
      int i;

      GEOM::ListOfLong_var anArray = new GEOM::ListOfLong;
      anArray->length(nbSel);

      for (i = 1; i <= nbSel; i++)
        anArray[i - 1] = aMapIndex.FindKey(i);

      GEOM::ListOfGO_var aList = anOper->MakeSubShapes(myObject, anArray);
      int n = aList->length();
      for (i = 0; i < n; i++)
        objects.push_back(GEOM::GEOM_Object::_duplicate(aList[i]));
    }
  }
  else {
    GEOM::ListOfGO_var aList = anOper->ExtractSubShapes(myObject, shapeType(), true);
    if (!aList->length())
      return false;
    for (int i = 0, n = aList->length(); i < n; i++)
      objects.push_back(GEOM::GEOM_Object::_duplicate(aList[i]));
  }

  return objects.size();
}

//================================================================
// Function : getFather
// Purpose  : Get father object for object to be added in study
//            (called with addInStudy method)
//================================================================
GEOM::GEOM_Object_ptr EntityGUI_SubShapeDlg::getFather(GEOM::GEOM_Object_ptr)
{
  return myObject;
}

//================================================================
// Function : getNewObjectName
// Purpose  :
//================================================================
QString EntityGUI_SubShapeDlg::getNewObjectName (int) const
{
  return QString::null;
}

//=================================================================================
// function : ClickOnOkFilter()
// purpose  : highlight and select entities which parameters (length, area or volume) are less than the value specified by the user
//=================================================================================
void EntityGUI_SubShapeDlg::ClickOnOkFilter()
{
  if (CORBA::is_nil(myObject) || isAllSubShapes() || shapeType() >= GEOM::VERTEX)
    return;
  
  TopTools_IndexedMapOfShape aSubShapesMap;
  TopExp::MapShapes(myShape, aSubShapesMap);
  SALOME_View* view = GEOM_Displayer::GetActiveView();
  getDisplayer()->Erase(myObject, false, false);
  CORBA::String_var aMainEntry = myObject->GetStudyEntry();
  QString anEntryBase = aMainEntry.in();

  SALOME_ListIO toSelect;

  TopExp_Explorer anExp (myShape, (TopAbs_ShapeEnum)shapeType());
  for (; anExp.More(); anExp.Next())
  {
    TopoDS_Shape aSubShape = anExp.Current();
    int index = aSubShapesMap.FindIndex(aSubShape);
    QString anEntry = QString( "TEMP_" ) + anEntryBase + QString("_%1").arg(index);
    if ( !getDisplayer()->IsDisplayed( anEntry ) )
      continue;

    double factor = GEOMUtils::ShapeToDouble(aSubShape).second;
    double v1 = myLessFilterSpin->value();
    double v2 = myGreaterFilterSpin->value();
    bool isLess = myLessFilterCombo->itemData(myLessFilterCombo->currentIndex()).toInt() == Filter_LT ? factor < v1 : factor <= v1;
    bool isGreater = myGreaterFilterCombo->itemData(myGreaterFilterCombo->currentIndex()).toInt() == Filter_GT ? factor > v2 : factor >= v2;
    if ( ( myLessFilterCheck->isChecked() && myGreaterFilterCheck->isChecked() && isLess && isGreater ) ||
         ( myLessFilterCheck->isChecked() && !myGreaterFilterCheck->isChecked() && isLess ) ||
         ( myGreaterFilterCheck->isChecked() && !myLessFilterCheck->isChecked() && isGreater ) ) {
      Handle(SALOME_InteractiveObject) io = new SALOME_InteractiveObject();
      io->setEntry( anEntry.toLatin1().constData() );
      io->setName( myObject->GetName() );
      toSelect.Append(io);
    }
  }
  if ( toSelect.Extent() > 0 ) {
    myGeomGUI->getApp()->selectionMgr()->setSelectedObjects(toSelect);
    SUIT_MessageBox::information( this,
                                  tr( "INF_INFO" ),
                                  tr( "GEOM_SOME_SHAPES_SELECTED").arg( toSelect.Extent() ),
                                  tr( "BUT_OK" ) );
  }
  else {
    SUIT_MessageBox::information( this,
                                  tr( "INF_INFO" ),
                                  tr( "GEOM_NO_SHAPES_SELECTED" ),
                                  tr( "BUT_OK" ) );
  }
  updateButtonState();
}

#ifndef DISABLE_PLOT2DVIEWER
//=================================================================================
// function : ClickOnPlot()
// purpose  : opens "Shape Statistics" dialog box in order to plot sub-shapes distribution.
//=================================================================================
void EntityGUI_SubShapeDlg::ClickOnPlot()
{
  QDialog* dlg = new MeasureGUI_ShapeStatisticsDlg( this, myShape, (TopAbs_ShapeEnum)shapeType() );
  if ( dlg ) {
    dlg->show();
  }
}
#endif

//=================================================================================
// function : MeasureToggled()
// purpose  :
//          : Called when 'myLessFilterCheck' or 'myGreaterFilterCheck' state change
//=================================================================================
void EntityGUI_SubShapeDlg::MeasureToggled()
{
  myLessFilterSpin->setEnabled(myLessFilterCheck->isChecked());
  myLessFilterCombo->setEnabled(myLessFilterCheck->isChecked());
  myGreaterFilterSpin->setEnabled(myGreaterFilterCheck->isChecked());
  myGreaterFilterCombo->setEnabled(myGreaterFilterCheck->isChecked());
  myApplyFilterButton->setEnabled(myLessFilterCheck->isChecked() || myGreaterFilterCheck->isChecked());
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> EntityGUI_SubShapeDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res;
  GEOM::GeomObjPtr aGeomObjPtr(myObject);
  res << aGeomObjPtr;
  return res;
}
