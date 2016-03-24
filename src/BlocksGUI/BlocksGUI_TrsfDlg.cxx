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
// File   : BlocksGUI_TrsfDlg.cxx
// Author : Julia DOROVSKIKH, Open CASCADE S.A.S. (julia.dorovskikh@opencascade.com)
//
#include "BlocksGUI_TrsfDlg.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <SUIT_Session.h>
#include <SUIT_Desktop.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_ViewWindow.h>
#include <SUIT_ViewManager.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>
#include <OCCViewer_ViewModel.h>

// OCCT Includes
#include <TColStd_IndexedMapOfInteger.hxx>

//=================================================================================
// class    : BlocksGUI_TrsfDlg()
// purpose  : Constructs a BlocksGUI_TrsfDlg which is a child of 'parent'.
//=================================================================================
BlocksGUI_TrsfDlg::BlocksGUI_TrsfDlg (GeometryGUI* theGeometryGUI, QWidget* parent)
  : GEOMBase_Skeleton(theGeometryGUI, parent),
    myInitial(true)
{
  SUIT_ResourceMgr* aResMgr = myGeomGUI->getApp()->resourceMgr();
  QPixmap image1 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_BLOCK_MULTITRSF_SIMPLE")));
  QPixmap image2 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_BLOCK_MULTITRSF_DOUBLE")));
  QPixmap imageS (aResMgr->loadPixmap("GEOM", tr("ICON_SELECT")));

  setWindowTitle(tr("GEOM_BLOCK_MULTITRSF_TITLE"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_BLOCK_MULTITRSF"));

  mainFrame()->RadioButton1->setIcon(image1);
  mainFrame()->RadioButton2->setIcon(image2);
  mainFrame()->RadioButton3->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton3->close();

  // Create first group
  myGrp1 = new QGroupBox(tr("GEOM_BLOCK_MULTITRSF_SIMPLE"), centralWidget());

  createSelWg(tr("GEOM_MAIN_OBJECT"), imageS, myGrp1, MainObj1);
  createSelWg(tr("FACE_1"),           imageS, myGrp1, Face1);
  createSelWg(tr("FACE_2"),           imageS, myGrp1, Face2);
  createSpinWg(tr("GEOM_NB_TIMES"),           myGrp1, SpinBox1);

  // Create second group
  myGrp2 = new QGroupBox(tr("GEOM_BLOCK_MULTITRSF_DOUBLE"), centralWidget());

  createSelWg(tr("GEOM_MAIN_OBJECT"), imageS, myGrp2, MainObj2);
  createSelWg(tr("FACE_1U"),          imageS, myGrp2, Face1U);
  createSelWg(tr("FACE_2U"),          imageS, myGrp2, Face2U);
  createSpinWg(tr("GEOM_NB_TIMES_U"),         myGrp2, SpinBox2U);
  createSelWg(tr("FACE_1V"),          imageS, myGrp2, Face1V);
  createSelWg(tr("FACE_2V"),          imageS, myGrp2, Face2V);
  createSpinWg(tr("GEOM_NB_TIMES_V"),         myGrp2, SpinBox2V);

  // Add groups to layout
  QVBoxLayout* layout = new QVBoxLayout(centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
  layout->addWidget(myGrp1);
  layout->addWidget(myGrp2);
  /***************************************************************/

  setHelpFileName("multi_transformation_operation_page.html");

  Init();
}

//=================================================================================
// function : ~BlocksGUI_TrsfDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
BlocksGUI_TrsfDlg::~BlocksGUI_TrsfDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BlocksGUI_TrsfDlg::Init()
{
  // Set range of spinboxes
  int SpecificStep = 1;
  QMap<int, SalomeApp_IntSpinBox*>::iterator anIter;
  for (anIter = mySpinBox.begin(); anIter != mySpinBox.end(); ++anIter) {
    initSpinBox(anIter.value(), 1, 999, SpecificStep);
  }

  showOnlyPreviewControl();
  
  // signals and slots connections
  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(this, SIGNAL(constructorsClicked(int)), this, SLOT(ConstructorsClicked(int)));

  QMap<int, QPushButton*>::iterator anIterBtn;
  for (anIterBtn = mySelBtn.begin(); anIterBtn != mySelBtn.end(); ++anIterBtn)
    connect(anIterBtn.value(), SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  QMap<int, SalomeApp_IntSpinBox*>::iterator anIterSpin;
  for (anIterSpin = mySpinBox.begin(); anIterSpin != mySpinBox.end(); ++anIterSpin)
    connect(anIterSpin.value(), SIGNAL(valueChanged(int)), this, SLOT(ValueChangedInSpinBox(int)));

  // init controls and fields
  initName(tr("GEOM_BLOCK_MULTITRSF"));

  myConstructorId = -1;
  ConstructorsClicked(0);
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void BlocksGUI_TrsfDlg::ConstructorsClicked (int constructorId)
{
  if (myConstructorId == constructorId)
    return;

  myConstructorId = constructorId;

  // init fields
  myShape = GEOM::GEOM_Object::_nil();
  myFaces[Face1] = myFaces[Face2] = -1;
  myFaces[Face1U] = myFaces[Face2U] = -1;
  myFaces[Face1V] = myFaces[Face2V] = -1;

  // clear line edits
  QMap<int, QLineEdit*>::iterator anIterLE;
  for (anIterLE = mySelName.begin(); anIterLE != mySelName.end(); ++anIterLE)
    anIterLE.value()->setText("");

  switch (constructorId) {
  case 0:
    myGrp2->hide();
    myGrp1->show();
    mySpinBox[SpinBox1]->setValue(2);
    mySelBtn[MainObj1]->click();
    break;
  case 1:
    myGrp1->hide();
    myGrp2->show();
    mySpinBox[SpinBox2U]->setValue(2);
    mySpinBox[SpinBox2V]->setValue(2);
    mySelBtn[MainObj2]->click();
    break;
  default:
    break;
  }

  qApp->processEvents();
  updateGeometry();
  resize(minimumSizeHint());

  // on dialog initialization we init the first field with a selected object (if any)
  SelectionIntoArgument();
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void BlocksGUI_TrsfDlg::ClickOnOk()
{
  setIsApplyAndClose( true );
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool BlocksGUI_TrsfDlg::ClickOnApply()
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
void BlocksGUI_TrsfDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("");

  // Get index of current selection focus
  int aCurrFocus = -1;
  QMap<int, QLineEdit*>::iterator anIter;
  for (anIter = mySelName.begin(); anIter != mySelName.end(); ++anIter) {
    if (myEditCurrentArgument == anIter.value()) {
      aCurrFocus = anIter.key();
      break;
    }
  }

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  QString aName;
  GEOM::GEOM_Object_var anObj = GEOM::GEOM_Object::_nil();

  if (aCurrFocus == MainObj1 || aCurrFocus == MainObj2)
  {
    // If selection of main object is activated
    if (aSelList.Extent() == 1) {
      anObj = GEOMBase::ConvertIOinGEOMObject( aSelList.First() );

      if ( GEOMBase::IsShape(anObj) ) {
        aName = GEOMBase::GetName(anObj);
      }
    }
    myEditCurrentArgument->setText(aName);
    myShape = anObj;
    enableWidgets();
  }
  else if (aCurrFocus == Face1  || aCurrFocus == Face2  ||
           aCurrFocus == Face1U || aCurrFocus == Face2U ||
           aCurrFocus == Face1V || aCurrFocus == Face2V) {
    // If face selection is activated
    int anIndex = -1;
    if (aSelList.Extent() == 1) {
      anObj = GEOMBase::ConvertIOinGEOMObject( aSelList.First() );
      if ( GEOMBase::IsShape(anObj) ) {
        aName = GEOMBase::GetName(anObj);
        TColStd_IndexedMapOfInteger anIndexes;
        aSelMgr->GetIndexes(aSelList.First(), anIndexes);

        if (anIndexes.Extent() == 1) {
          anIndex = anIndexes(1);
          aName += QString(":face_%1").arg(anIndex);
        }
      }
    }
    myEditCurrentArgument->setText(aName);
    myFaces[aCurrFocus] = anIndex;
  }

  switch (aCurrFocus) {
    // 1D
  case MainObj1:
    if (!myShape->_is_nil() && myFaces[Face1] == -1)
      mySelBtn[Face1]->click();
    break;
  case Face1:
    if (myFaces[Face1] != -1 && myFaces[Face2] == -1)
      mySelBtn[Face2]->click();
    break;
  case Face2:
    if (myFaces[Face2] != -1 && myShape->_is_nil())
      mySelBtn[MainObj1]->click();
    break;

    // 2D
  case MainObj2:
    if (!myShape->_is_nil() && myFaces[Face1U] == -1)
      mySelBtn[Face1U]->click();
    break;
  case Face1U:
    if (myFaces[Face1U] != -1 && myFaces[Face2U] == -1)
      mySelBtn[Face2U]->click();
    break;
  case Face2U:
    if (myFaces[Face2U] != -1 && myFaces[Face1V] == -1)
      mySelBtn[Face1V]->click();
    break;
  case Face1V:
    if (myFaces[Face1V] != -1 && myFaces[Face2V] == -1)
      mySelBtn[Face2V]->click();
    break;
  case Face2V:
    if (myFaces[Face2V] != -1 && myShape->_is_nil())
      mySelBtn[MainObj1]->click();
    break;

  default:
    break;
  }

  processPreview();
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BlocksGUI_TrsfDlg::SetEditCurrentArgument()
{
  QPushButton* aSender = (QPushButton*)sender();

  // clear selection
  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
  if (myInitial)
    myInitial = false;
  else
    myGeomGUI->getApp()->selectionMgr()->clearSelected();

  // disable all
  switch (myConstructorId) {
  case 0:
    mySelBtn[MainObj1]->setDown(false);
    mySelBtn[Face1]->setDown(false);
    mySelBtn[Face2]->setDown(false);

    mySelName[MainObj1]->setEnabled(false);
    mySelName[Face1]->setEnabled(false);
    mySelName[Face2]->setEnabled(false);
    break;
  case 1:
    mySelBtn[MainObj2]->setDown(false);
    mySelBtn[Face1U]->setDown(false);
    mySelBtn[Face2U]->setDown(false);
    mySelBtn[Face1V]->setDown(false);
    mySelBtn[Face2V]->setDown(false);

    mySelName[MainObj2]->setEnabled(false);
    mySelName[Face1U]->setEnabled(false);
    mySelName[Face2U]->setEnabled(false);
    mySelName[Face1V]->setEnabled(false);
    mySelName[Face2V]->setEnabled(false);
    break;
  default:
    break;
  }

  // set line edit as current argument
  QMap<int, QPushButton*>::iterator anIter;
  for (anIter = mySelBtn.begin(); anIter != mySelBtn.end(); ++anIter) {
    if (anIter.value() == aSender) {
      myEditCurrentArgument = mySelName[anIter.key()];
      break;
    }
  }

  // enable line edit
  myEditCurrentArgument->setEnabled(true);
  myEditCurrentArgument->setFocus();

  // enable push button
  // after setFocus(), because it will be setDown(false) when loses focus
  aSender->setDown(true);

  activateSelection();

  processPreview();
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void BlocksGUI_TrsfDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  activateSelection();

  // ??
  processPreview();
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void BlocksGUI_TrsfDlg::enterEvent (QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void BlocksGUI_TrsfDlg::ValueChangedInSpinBox(int)
{
  processPreview();
}

//=================================================================================
// function : createSelWg()
// purpose  :
//=================================================================================
void BlocksGUI_TrsfDlg::createSelWg (const QString& theLbl,
                                     QPixmap&       thePix,
                                     QWidget*       theParent,
                                     const int      theId)
{
  QLabel* lab = new QLabel(theLbl, theParent);
  mySelBtn[theId] = new QPushButton(theParent);
  mySelBtn[theId]->setIcon(thePix);
  mySelBtn[theId]->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
  mySelName[theId] = new QLineEdit(theParent);
  mySelName[theId]->setReadOnly(true);
  QGridLayout* l = 0;
  if (!theParent->layout()) {
    l = new QGridLayout(theParent);
    l->setMargin(9); l->setSpacing(6);
  }
  else {
    l = qobject_cast<QGridLayout*>(theParent->layout());
  }
  int row = l->rowCount();
  l->addWidget(lab,              row, 0);
  l->addWidget(mySelBtn[theId],  row, 1);
  l->addWidget(mySelName[theId], row, 2);
}

//=================================================================================
// function : createSpinWg()
// purpose  :
//=================================================================================
void BlocksGUI_TrsfDlg::createSpinWg (const QString& theLbl,
                                      QWidget*       theParent,
                                      const int      theId)
{
  QLabel* lab = new QLabel(theLbl, theParent);
  mySpinBox[theId] = new SalomeApp_IntSpinBox(theParent);
  QGridLayout* l = 0;
  if (!theParent->layout()) {
    l = new QGridLayout(theParent);
    l->setMargin(9); l->setSpacing(6);
  }
  else {
    l = qobject_cast<QGridLayout*>(theParent->layout());
  }
  int row = l->rowCount();
  l->addWidget(lab,              row, 0);
  l->addWidget(mySpinBox[theId], row, 2);
}

//=================================================================================
// function : activateSelection
// purpose  : Activate selection in accordance with myEditCurrentArgument
//=================================================================================
void BlocksGUI_TrsfDlg::activateSelection()
{
  globalSelection(GEOM_ALLSHAPES);
  if (!myShape->_is_nil() &&
      (myEditCurrentArgument == mySelName[Face1 ] ||
       myEditCurrentArgument == mySelName[Face2 ] ||
       myEditCurrentArgument == mySelName[Face1U] ||
       myEditCurrentArgument == mySelName[Face2U] ||
       myEditCurrentArgument == mySelName[Face1V] ||
       myEditCurrentArgument == mySelName[Face2V]))
  {
    // Local selection is available only in the OCC Viewer
    if (getDesktop()->activeWindow()->getViewManager()->getType() == OCCViewer_Viewer::Type()) {
      localSelection(myShape, TopAbs_FACE);
    }
  }
  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));
}

//=================================================================================
// function : enableWidgets
// purpose  : Enable widgets of faces in accordance with value of main object
//=================================================================================
void BlocksGUI_TrsfDlg::enableWidgets()
{
  int anId = getConstructorId();

  bool toEnable = !myShape->_is_nil();

  if (anId == 0) {
    //mySelName[Face1]->setEnabled(toEnable);
    //mySelName[Face2]->setEnabled(toEnable);
    //mySelBtn[Face1]->setEnabled(toEnable);
    //mySelBtn[Face2]->setEnabled(toEnable);

    if (!toEnable)  {
      mySelName[Face1]->setText("");
      mySelName[Face2]->setText("");
      myFaces[Face1] = -1;
      myFaces[Face2] = -1;
    }
  }
  else if (anId == 1) {
    //mySelName[Face1U]->setEnabled(toEnable);
    //mySelName[Face2U]->setEnabled(toEnable);
    //mySelName[Face1V]->setEnabled(toEnable);
    //mySelName[Face2V]->setEnabled(toEnable);
    //mySelBtn[Face1U]->setEnabled(toEnable);
    //mySelBtn[Face2U]->setEnabled(toEnable);
    //mySelBtn[Face1V]->setEnabled(toEnable);
    //mySelBtn[Face2V]->setEnabled(toEnable);

    if (!toEnable) {
      mySelName[Face1U]->setText("");
      mySelName[Face2U]->setText("");
      mySelName[Face1V]->setText("");
      mySelName[Face2V]->setText("");
      myFaces[Face1U] = -1;
      myFaces[Face2U] = -1;
      myFaces[Face1V] = -1;
      myFaces[Face2V] = -1;
    }
  }
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr BlocksGUI_TrsfDlg::createOperation()
{
  return getGeomEngine()->GetIBlocksOperations(getStudyId());
}

//=================================================================================
// function : isValid
// purpose  : Verify validity of input data
//=================================================================================
bool BlocksGUI_TrsfDlg::isValid (QString& msg)
{
  bool ok = false, okSP = true;
  switch (getConstructorId()) {
  case 0:
    ok = !myShape->_is_nil() && myFaces[Face1] > 0;
    okSP = mySpinBox[SpinBox1]->isValid( msg, !IsPreview() );
    break;
  case 1:
    ok = !myShape->_is_nil() && myFaces[Face1U] > 0 && myFaces[Face1V] > 0;
    okSP = mySpinBox[SpinBox2U]->isValid( msg, !IsPreview() ) && okSP;
    okSP = mySpinBox[SpinBox2V]->isValid( msg, !IsPreview() ) && okSP;
    break;
  default:
    break;
  }
  return ok && okSP;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool BlocksGUI_TrsfDlg::execute (ObjectList& objects)
{
  bool res = false;

  GEOM::GEOM_Object_var anObj;

  GEOM::GEOM_IBlocksOperations_var anOper = GEOM::GEOM_IBlocksOperations::_narrow(getOperation());

  switch (getConstructorId()) {
  case 0:
    anObj = anOper->MakeMultiTransformation1D(myShape,
                                              myFaces[Face1], myFaces[Face2],
                                              mySpinBox[SpinBox1]->value());
    if (!anObj->_is_nil() && !IsPreview())
    {
      QStringList aParameters;
      aParameters << "" << "";
      aParameters << mySpinBox[SpinBox1]->text();
      anObj->SetParameters(aParameters.join(":").toLatin1().constData());
    }
    res = true;
    break;
  case 1:
    anObj = anOper->MakeMultiTransformation2D (myShape,
                                               myFaces[Face1U], myFaces[Face2U],
                                               mySpinBox[SpinBox2U]->value(),
                                               myFaces[Face1V], myFaces[Face2V],
                                               mySpinBox[SpinBox2V]->value());
    if (!anObj->_is_nil() && !IsPreview())
    {
      QStringList aParameters;
      aParameters << "" << "";
      aParameters << mySpinBox[SpinBox2U]->text();
      aParameters << "" << "";
      aParameters << mySpinBox[SpinBox2V]->text();
      anObj->SetParameters(aParameters.join(":").toLatin1().constData());
    }
    res = true;
    break;
  default:
    break;
  }

  if (!anObj->_is_nil())
    objects.push_back(anObj._retn());

  return res;
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> BlocksGUI_TrsfDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res;
  GEOM::GeomObjPtr aGeomObjPtr(myShape);
  res << aGeomObjPtr;
  return res;
}
