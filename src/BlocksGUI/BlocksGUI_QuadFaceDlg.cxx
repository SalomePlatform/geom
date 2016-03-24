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
// File   : BlocksGUI_QuadFaceDlg.cxx
// Author : Julia DOROVSKIKH, Open CASCADE S.A.S. (julia.dorovskikh@opencascade.com)
//
#include "BlocksGUI_QuadFaceDlg.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>

// QT Includes
#include <qlabel.h>

// OCCT Includes
#include <TopAbs.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>

#include <GEOMImpl_Types.hxx>

//=================================================================================
// class    : BlocksGUI_QuadFaceDlg()
// purpose  : Constructs a BlocksGUI_QuadFaceDlg which is a child of 'parent'.
//=================================================================================
BlocksGUI_QuadFaceDlg::BlocksGUI_QuadFaceDlg (GeometryGUI* theGeometryGUI, QWidget* parent)
  : GEOMBase_Skeleton(theGeometryGUI, parent),
    myInitial(true)
{
  SUIT_ResourceMgr* aResMgr = myGeomGUI->getApp()->resourceMgr();
  QPixmap image1 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_QUAD_FACE_4_VERT")));
  QPixmap image2 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_QUAD_FACE_2_EDGE")));
  QPixmap image3 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_QUAD_FACE_4_EDGE")));
  QPixmap imageS (aResMgr->loadPixmap("GEOM", tr("ICON_SELECT")));

  setWindowTitle(tr("GEOM_QUAD_FACE_TITLE"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_QUAD_FACE"));

  mainFrame()->RadioButton1->setIcon(image1);
  mainFrame()->RadioButton2->setIcon(image2);
  mainFrame()->RadioButton3->setIcon(image3);

  // Create first group
  myGrp1 = new QGroupBox(tr("GEOM_ARGUMENTS"), centralWidget());

  createSelWg(tr("VERTEX_1"), imageS, myGrp1, Vertex1);
  createSelWg(tr("VERTEX_2"), imageS, myGrp1, Vertex2);
  createSelWg(tr("VERTEX_3"), imageS, myGrp1, Vertex3);
  createSelWg(tr("VERTEX_4"), imageS, myGrp1, Vertex4);

  // Create second group
  myGrp2 = new QGroupBox(tr("GEOM_ARGUMENTS"), centralWidget());

  createSelWg(tr("EDGE_1"), imageS, myGrp2, Edge12);
  createSelWg(tr("EDGE_2"), imageS, myGrp2, Edge22);

  // Create fird group
  myGrp3 = new QGroupBox(tr("GEOM_ARGUMENTS"), centralWidget());

  createSelWg(tr("EDGE_1"), imageS, myGrp3, Edge14);
  createSelWg(tr("EDGE_2"), imageS, myGrp3, Edge24);
  createSelWg(tr("EDGE_3"), imageS, myGrp3, Edge34);
  createSelWg(tr("EDGE_4"), imageS, myGrp3, Edge44);

  // Add groups to layout
  QVBoxLayout* layout = new QVBoxLayout(centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
  layout->addWidget(myGrp1);
  layout->addWidget(myGrp2);
  layout->addWidget(myGrp3);
  /***************************************************************/

  setHelpFileName("build_by_blocks_page.html#quad_face_anchor");

  Init();
}

//=================================================================================
// function : ~BlocksGUI_QuadFaceDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
BlocksGUI_QuadFaceDlg::~BlocksGUI_QuadFaceDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BlocksGUI_QuadFaceDlg::Init()
{
  // signals and slots connections
  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(this, SIGNAL(constructorsClicked(int)), this, SLOT(ConstructorsClicked(int)));

  QMap<int, QPushButton*>::iterator anIterBtn;
  for (anIterBtn = mySelBtn.begin(); anIterBtn != mySelBtn.end(); ++anIterBtn)
    connect(anIterBtn.value(), SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  // init controls and fields
  initName(tr("GEOM_QUAD_FACE"));

  myConstructorId = -1;
  ConstructorsClicked(0);
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void BlocksGUI_QuadFaceDlg::ConstructorsClicked (int constructorId)
{
  if (myConstructorId == constructorId)
    return;

  myConstructorId = constructorId;

  // init fields
  myShape1.nullify();
  myShape2.nullify();
  myShape3.nullify();
  myShape4.nullify();

  // clear line edits
  QMap<int, QLineEdit*>::iterator anIterLE;
  for (anIterLE = mySelName.begin(); anIterLE != mySelName.end(); ++anIterLE)
    anIterLE.value()->setText("");

  switch (constructorId) {
  case 0:
    myGrp2->hide();
    myGrp3->hide();
    myGrp1->show();
    mySelBtn[Vertex1]->click();
    break;
  case 1:
    myGrp1->hide();
    myGrp3->hide();
    myGrp2->show();
    mySelBtn[Edge12]->click();
    break;
  case 2:
    myGrp1->hide();
    myGrp2->hide();
    myGrp3->show();
    mySelBtn[Edge14]->click();
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
void BlocksGUI_QuadFaceDlg::ClickOnOk()
{
  setIsApplyAndClose( true );
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool BlocksGUI_QuadFaceDlg::ClickOnApply()
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
void BlocksGUI_QuadFaceDlg::SelectionIntoArgument()
{
  erasePreview();

  // Get index of current selection focus
  int aCurrFocus = -1;
  QMap<int, QLineEdit*>::iterator anIter;
  for (anIter = mySelName.begin(); anIter != mySelName.end(); ++anIter) {
    if (myEditCurrentArgument == anIter.value()) {
      aCurrFocus = anIter.key();
      break;
    }
  }

  TopAbs_ShapeEnum aType = TopAbs_EDGE;
  if (aCurrFocus == Vertex1 || aCurrFocus == Vertex2 ||
      aCurrFocus == Vertex3 || aCurrFocus == Vertex4)
    aType = TopAbs_VERTEX;

  GEOM::GeomObjPtr aSelectedObject = getSelected( aType );
  TopoDS_Shape aShape;
  if ( aSelectedObject && GEOMBase::GetShape( aSelectedObject.get(), aShape ) && !aShape.IsNull() ) {
    QString aName = GEOMBase::GetName( aSelectedObject.get() );
    myEditCurrentArgument->setText( aName );
    switch (aCurrFocus) {
      // four vertices
    case Vertex1:
      myShape1 = aSelectedObject;
      if      (!myShape2) mySelBtn[Vertex2]->click();
      else if (!myShape3) mySelBtn[Vertex3]->click();
      else if (!myShape4) mySelBtn[Vertex4]->click();
      break;
    case Vertex2:
      myShape2 = aSelectedObject;
      if      (!myShape3) mySelBtn[Vertex3]->click();
      else if (!myShape4) mySelBtn[Vertex4]->click();
      else if (!myShape1) mySelBtn[Vertex1]->click();
      break;
    case Vertex3:
      myShape3 = aSelectedObject;
      if      (!myShape4) mySelBtn[Vertex4]->click();
      else if (!myShape1) mySelBtn[Vertex1]->click();
      else if (!myShape2) mySelBtn[Vertex2]->click();
      break;
    case Vertex4:
      myShape4 = aSelectedObject;
      if      (!myShape1) mySelBtn[Vertex1]->click();
      else if (!myShape2) mySelBtn[Vertex2]->click();
      else if (!myShape3) mySelBtn[Vertex3]->click();
      break;
      // two edges
    case Edge12:
      myShape1 = aSelectedObject;
      if      (!myShape2) mySelBtn[Edge22]->click();
      break;
    case Edge22:
      myShape2 = aSelectedObject;
      if      (!myShape1) mySelBtn[Edge12]->click();
      break;
      // four edges
    case Edge14:
      myShape1 = aSelectedObject;
      if      (!myShape2) mySelBtn[Edge24]->click();
      else if (!myShape3) mySelBtn[Edge34]->click();
      else if (!myShape4) mySelBtn[Edge44]->click();
      break;
    case Edge24:
      myShape2 = aSelectedObject;
      if      (!myShape3) mySelBtn[Edge34]->click();
      else if (!myShape4) mySelBtn[Edge44]->click();
      else if (!myShape1) mySelBtn[Edge14]->click();
      break;
    case Edge34:
      myShape3 = aSelectedObject;
      if      (!myShape4) mySelBtn[Edge44]->click();
      else if (!myShape1) mySelBtn[Edge14]->click();
      else if (!myShape2) mySelBtn[Edge24]->click();
      break;
    case Edge44:
      myShape4 = aSelectedObject;
      if      (!myShape1) mySelBtn[Edge14]->click();
      else if (!myShape2) mySelBtn[Edge24]->click();
      else if (!myShape3) mySelBtn[Edge34]->click();
      break;
    default:
      break;
    }
  }
  else {
    switch (aCurrFocus) {
    case Vertex1:
    case Edge12:
    case Edge14:
      myShape1.nullify();
      break;
    case Vertex2:
    case Edge22:
    case Edge24:
      myShape2.nullify();
      break;
    case Vertex3:
    case Edge34:
      myShape3.nullify();
      break;
    case Vertex4:
    case Edge44:
      myShape4.nullify();
      break;
    default:
      break;
    }
    myEditCurrentArgument->setText( "" );
  }

  displayPreview(true);
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BlocksGUI_QuadFaceDlg::SetEditCurrentArgument()
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
    mySelBtn[Vertex1]->setDown(false);
    mySelBtn[Vertex2]->setDown(false);
    mySelBtn[Vertex3]->setDown(false);
    mySelBtn[Vertex4]->setDown(false);

    mySelName[Vertex1]->setEnabled(false);
    mySelName[Vertex2]->setEnabled(false);
    mySelName[Vertex3]->setEnabled(false);
    mySelName[Vertex4]->setEnabled(false);
    break;
  case 1:
    mySelBtn[Edge12]->setDown(false);
    mySelBtn[Edge22]->setDown(false);

    mySelName[Edge12]->setEnabled(false);
    mySelName[Edge22]->setEnabled(false);
    break;
  case 2:
    mySelBtn[Edge14]->setDown(false);
    mySelBtn[Edge24]->setDown(false);
    mySelBtn[Edge34]->setDown(false);
    mySelBtn[Edge44]->setDown(false);

    mySelName[Edge14]->setEnabled(false);
    mySelName[Edge24]->setEnabled(false);
    mySelName[Edge34]->setEnabled(false);
    mySelName[Edge44]->setEnabled(false);
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
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void BlocksGUI_QuadFaceDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  activateSelection();

  // ??
  displayPreview(true);
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void BlocksGUI_QuadFaceDlg::enterEvent (QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : createSelWg()
// purpose  :
//=================================================================================
void BlocksGUI_QuadFaceDlg::createSelWg (const QString& theLbl,
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
// function : activateSelection
// purpose  : Activate selection in accordance with myEditCurrentArgument
//=================================================================================
void BlocksGUI_QuadFaceDlg::activateSelection()
{
  globalSelection(); // close local contexts, if any
  if (myEditCurrentArgument == mySelName[Vertex1] ||
      myEditCurrentArgument == mySelName[Vertex2] ||
      myEditCurrentArgument == mySelName[Vertex3] ||
      myEditCurrentArgument == mySelName[Vertex4])
  {
    localSelection(TopAbs_VERTEX); //Select Vertices on All Shapes
  }
  else
  {
    localSelection(TopAbs_EDGE); //Select Edges on All Shapes
  }
  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr BlocksGUI_QuadFaceDlg::createOperation()
{
  return getGeomEngine()->GetIBlocksOperations(getStudyId());
}

//=================================================================================
// function : isValid
// purpose  : Verify validity of input data
//=================================================================================
bool BlocksGUI_QuadFaceDlg::isValid (QString&)
{
  bool ok = false;
  switch (getConstructorId()) {
  case 0:
    ok = myShape1 && myShape2 && myShape3 && myShape4;
    break;
  case 1:
    ok = myShape1 && myShape2;
    break;
  case 2:
    ok = myShape1 && myShape2 && myShape3 && myShape4;
    break;
  default:
    break;
  }
  return ok;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool BlocksGUI_QuadFaceDlg::execute (ObjectList& objects)
{
  bool res = false;

  GEOM::GEOM_Object_var anObj;

  GEOM::GEOM_IBlocksOperations_var anOper = GEOM::GEOM_IBlocksOperations::_narrow(getOperation());

  switch (getConstructorId()) {
  case 0:
    anObj = anOper->MakeQuad4Vertices(myShape1.get(), myShape2.get(), myShape3.get(), myShape4.get());
    res = true;
    break;
  case 1:
    anObj = anOper->MakeQuad2Edges(myShape1.get(), myShape2.get());
    res = true;
    break;
  case 2:
    anObj = anOper->MakeQuad(myShape1.get(), myShape2.get(), myShape3.get(), myShape4.get());
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
// function : addSubshapeToStudy
// purpose  : virtual method to add new SubObjects if local selection
//=================================================================================
void BlocksGUI_QuadFaceDlg::addSubshapesToStudy()
{
  switch (getConstructorId()) {
  case 0:
    GEOMBase::PublishSubObject( myShape1.get() );
    GEOMBase::PublishSubObject( myShape2.get() );
    GEOMBase::PublishSubObject( myShape3.get() );
    GEOMBase::PublishSubObject( myShape4.get() );
    break;
  case 1:
    GEOMBase::PublishSubObject( myShape1.get() );
    GEOMBase::PublishSubObject( myShape2.get() );
    break;
  case 2:
    GEOMBase::PublishSubObject( myShape1.get() );
    GEOMBase::PublishSubObject( myShape2.get() );
    GEOMBase::PublishSubObject( myShape3.get() );
    GEOMBase::PublishSubObject( myShape4.get() );
    break;
  default:
    break;
  }
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> BlocksGUI_QuadFaceDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res;
  res << myShape1 << myShape2 << myShape3 << myShape4;
  return res;
}
