// Copyright (C) 2007-2013  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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
#include "MeasureGUI_Widgets.h"

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <LightApp_SelectionMgr.h>
#include <SalomeApp_Tools.h>
#include <SalomeApp_Application.h>

#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>
#include <TColStd_MapOfInteger.hxx>

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>
#include <GEOMImpl_Types.hxx>

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
  : MeasureGUI_Skeleton(GUI, parent, false)
{
  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
  QPixmap image0 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_CHECK_SELF_INTERSECTIONS")));
  QPixmap image1 (aResMgr->loadPixmap("GEOM", tr("ICON_SELECT")));

  setWindowTitle(tr("GEOM_CHECK_SELF_INTERSECTIONS"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_CHECK_SELF_INTERSECTIONS"));
  mainFrame()->RadioButton1->setIcon(image0);

  myGrp = new MeasureGUI_1Sel1TextView2ListBox (centralWidget());
  myGrp->GroupBox1->setTitle(tr("GEOM_CHECK_INFOS"));
  myGrp->TextLabel1->setText(tr("GEOM_OBJECT"));

  myGrp->TextView1->setReadOnly(true);
  QFont aFont (TEXTEDIT_FONT_FAMILY, TEXTEDIT_FONT_SIZE);
  aFont.setStyleHint(QFont::TypeWriter, QFont::PreferAntialias);
  myGrp->TextView1->setFont(aFont);

  myGrp->PushButton1->setIcon(image1);
  myGrp->LineEdit1->setReadOnly(true);

  myGrp->TextLabel2->setText(tr("GEOM_CHECK_BLOCKS_COMPOUND_ERRORS"));
  myGrp->TextLabel3->setText(tr("GEOM_CHECK_BLOCKS_COMPOUND_SUBSHAPES"));

  myGrp->ListBox2->setSelectionMode(QAbstractItemView::ExtendedSelection);

  QVBoxLayout* layout = new QVBoxLayout (centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
  layout->addWidget(myGrp);

  /***************************************************************/

  myHelpFileName = "using_measurement_tools_page.html#check_self_intersections_anchor";

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
  mySelBtn = myGrp->PushButton1;
  mySelEdit = myGrp->LineEdit1;
  MeasureGUI_Skeleton::Init();

  connect(myGrp->ListBox1, SIGNAL(itemSelectionChanged()), SLOT(onErrorsListSelectionChanged()));
  connect(myGrp->ListBox2, SIGNAL(itemSelectionChanged()), SLOT(onSubShapesListSelectionChanged()));
}

//=================================================================================
// function : processObject
// purpose  :
//=================================================================================
void MeasureGUI_CheckSelfIntersectionsDlg::processObject()
{
  myGrp->ListBox1->clear();
  myGrp->ListBox2->clear();
  erasePreview();

  if (myObj->_is_nil())
    return;

  QString aMsg ("");
  GEOM::GEOM_IMeasureOperations_var anOper = GEOM::GEOM_IMeasureOperations::_narrow(getOperation());
  bool isGood = false, isFailed = false;
  int nbPairs = 0;
  try {
    isGood = anOper->CheckSelfIntersections(myObj, myInters);
    nbPairs = myInters->length()/2;
    if (nbPairs*2 != myInters->length()) {
      isFailed = true;
    }
  }
  catch (const SALOME::SALOME_Exception& e) {
    SalomeApp_Tools::QtCatchCorbaException(e);
    isFailed = true;
  }

  if (!anOper->IsDone()) {
    aMsg += tr(anOper->GetErrorCode());
    myGrp->TextView1->setText(aMsg);
    return;
  }
  else if (isFailed) {
    aMsg += tr("GEOM_CHECK_SELF_INTERSECTIONS_FAILED");
    myGrp->TextView1->setText(aMsg);
    return;
  }

  // Status
  if (isGood) {
    aMsg += tr("GEOM_NO_SELF_INTERSECTIONS");
  }
  else {
    aMsg += tr("GEOM_SELF_INTERSECTIONS_FOUND");
  }
  myGrp->TextView1->setText(aMsg);

  // Pairs
  QStringList aErrList;
  QString aErrStr ("");
  for (int i = 1; i <= nbPairs; i++) {
    aErrStr = "Intersection # ";
    aErrStr += QString::number(i);
    aErrList.append(aErrStr);
  }

  myGrp->ListBox1->addItems(aErrList);
}

//=================================================================================
// function : onErrorsListSelectionChanged
// purpose  :
//=================================================================================
void MeasureGUI_CheckSelfIntersectionsDlg::onErrorsListSelectionChanged()
{
  erasePreview();
  int aCurItem = myGrp->ListBox1->currentRow();
  if (aCurItem < 0)
    return;

  //int nbPairs = myInters->length()/2;

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
  myGrp->ListBox2->clear();
  myGrp->ListBox2->addItems(aSubShapeList);
}

//=================================================================================
// function : onSubShapesListSelectionChanged
// purpose  :
//=================================================================================
void MeasureGUI_CheckSelfIntersectionsDlg::onSubShapesListSelectionChanged()
{
  erasePreview();

  // Current pair
  int aErrCurItem = myGrp->ListBox1->currentRow();
  if (aErrCurItem < 0)
    return;

  // Selected IDs
  QList<int> aIds;
  for (int i = 0, n = myGrp->ListBox2->count(); i < n; i++) {
    if (myGrp->ListBox2->item(i)->isSelected())
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
