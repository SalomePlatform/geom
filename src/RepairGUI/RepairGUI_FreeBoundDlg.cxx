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
// File   : RepairGUI_FreeBoundDlg.cxx
// Author : Sergey LITONIN, Open CASCADE S.A.S. (sergey.litonin@opencascade.com)

#include "RepairGUI_FreeBoundDlg.h"

#include <GEOMBase.h>
#include <DlgRef.h>

#include <GeometryGUI.h>
#include <GEOM_Displayer.h>
#include <GEOMImpl_Types.hxx>

#include <SalomeApp_Application.h>
#include <LightApp_Application.h>
#include <LightApp_SelectionMgr.h>
#include <SUIT_MessageBox.h>
#include <SUIT_Desktop.h>
#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>

#include <TColStd_MapOfInteger.hxx>

#include <QLineEdit>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QKeyEvent>

#define SPACING 6
#define MARGIN  0

/*!
  Class       : RepairGUI_FreeBoundDlg
  Description : Dialog for displaying free boundaries of selected face, shell or solid
*/

//=================================================================================
// function : RepairGUI_FreeBoundDlg
// purpose  : Constructor
//=================================================================================
RepairGUI_FreeBoundDlg::RepairGUI_FreeBoundDlg (GeometryGUI* theGUI, QWidget* theParent)
  : GEOMBase_Skeleton(theGUI, theParent)
{
  QPixmap image0 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_FREE_BOUNDS")));
  QPixmap image1 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_SELECT")));

  setWindowTitle(tr("CAPTION"));

  /***************************************************************/

  mainFrame()->GroupConstructors->setTitle(tr("FREE_BOUND"));
  mainFrame()->RadioButton1->setIcon(image0);
  mainFrame()->RadioButton2->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton3->close();

  mainFrame()->GroupBoxName->hide();

  QGroupBox* aMainGrp = new QGroupBox (tr("FREE_BOUND"), this);

  QLabel* lab = new QLabel (tr("GEOM_OBJECT"), aMainGrp);
  QPushButton* btn = new QPushButton (aMainGrp);
  btn->setIcon(image1);
  myEdit = new QLineEdit (aMainGrp);
  myEdit->setReadOnly(true);
  myEdit->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));

  myClosedLbl = new QLabel (tr("NUMBER_CLOSED"), aMainGrp);
  myOpenLbl   = new QLabel (tr("NUMBER_OPEN"),   aMainGrp);
  myClosedLbl->setMinimumWidth(150);
  myOpenLbl->setMinimumWidth(150);

  QGridLayout* aMainGrpLayout = new QGridLayout (aMainGrp);
  aMainGrpLayout->addWidget(lab,    0, 0);
  aMainGrpLayout->addWidget(btn,    0, 1);
  aMainGrpLayout->addWidget(myEdit, 0, 2);
  aMainGrpLayout->addWidget(myClosedLbl, 1, 0, 1, 3);
  aMainGrpLayout->addWidget(myOpenLbl,   2, 0, 1, 3);

  QVBoxLayout* aLay = new QVBoxLayout (centralWidget());
  aLay->setSpacing(SPACING);
  aLay->setMargin(MARGIN);
  aLay->addWidget(aMainGrp);

  resize(minimumSizeHint());
  /***************************************************************/

  myHelpFileName = "boundaries_page.html";

  Init();
}

//=================================================================================
// function : ~RepairGUI_FreeBoundDlg
// purpose  : Destructor
//=================================================================================
RepairGUI_FreeBoundDlg::~RepairGUI_FreeBoundDlg()
{
}

//=================================================================================
// function : Init
// purpose  : Initialize dialog fields
//=================================================================================
void RepairGUI_FreeBoundDlg::Init()
{
  myNbClosed = myNbOpen = 0;
  myObj = GEOM::GEOM_Object::_nil();

  myEditCurrentArgument = myEdit;

  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));

  activateSelection();
  SelectionIntoArgument();
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void RepairGUI_FreeBoundDlg::ClickOnOk()
{
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool RepairGUI_FreeBoundDlg::ClickOnApply()
{
  if (!onAccept())
    return false;
  return true;
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void RepairGUI_FreeBoundDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();

  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));

  activateSelection();
  displayPreview(true);
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void RepairGUI_FreeBoundDlg::enterEvent(QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : SelectionIntoArgument
// purpose  : SLOT. Called when selection changed.
//=================================================================================
void RepairGUI_FreeBoundDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("");
  myClosedLbl->setText(tr("NUMBER_CLOSED"));
  myOpenLbl->setText(tr("NUMBER_OPEN"));
  myObj = GEOM::GEOM_Object::_nil();
  erasePreview();

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  if (aSelList.Extent() != 1)
    return;

  GEOM::GEOM_Object_var anObj = GEOMBase::ConvertIOinGEOMObject(aSelList.First());

  if (!GEOMBase::IsShape(anObj))
    return;

  myObj = anObj;
  myEditCurrentArgument->setText(GEOMBase::GetName(myObj));
  displayPreview(true, false, true, true, 3, 0);
}

//=================================================================================
// function : activateSelection
// purpose  : activate selection of faces, shells, and solids
//=================================================================================
void RepairGUI_FreeBoundDlg::activateSelection()
{
  TColStd_MapOfInteger aMap;
  aMap.Add(GEOM_FACE);
  aMap.Add(GEOM_SHELL);
  aMap.Add(GEOM_SOLID);
  aMap.Add(GEOM_COMPOUND);
  globalSelection(aMap);
}

//=================================================================================
// function : createOperation
// purpose  : Create operation
//=================================================================================
GEOM::GEOM_IOperations_ptr RepairGUI_FreeBoundDlg::createOperation()
{
  return getGeomEngine()->GetIHealingOperations(getStudyId());
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool RepairGUI_FreeBoundDlg::isValid (QString&)
{
  return !myObj->_is_nil();
}

//=================================================================================
// function : execute
// purpose  : Get free boundaries
//=================================================================================
bool RepairGUI_FreeBoundDlg::execute (ObjectList& objects)
{
  GEOM::ListOfGO_var aClosed, anOpen;

  GEOM::ListOfGO_var objList = new GEOM::ListOfGO;
  objList->length(1);
  objList[0] = myObj;
  GEOM::GEOM_IHealingOperations_var anOper = GEOM::GEOM_IHealingOperations::_narrow(getOperation());
  bool result = anOper->GetFreeBoundary(objList, aClosed, anOpen);

  if (result) {
    myNbClosed = aClosed->length();
    myNbOpen = anOpen->length();
    int i;
    for (i = 0; i < myNbClosed; i++)
      objects.push_back(aClosed[i]._retn());
    for (i = 0; i < myNbOpen; i++)
      objects.push_back(anOpen[i]._retn());

    QString aLabelText = tr("NUMBER_CLOSED") + QString("%1").arg(myNbClosed);
    myClosedLbl->setText(aLabelText);
    aLabelText = tr("NUMBER_OPEN") + QString("%1").arg(myNbOpen);
    myOpenLbl->setText(aLabelText);
  }

  myCurrObj = 1;
  return result;
}

//================================================================
// Function : getNewObjectName
// Purpose  : Redefine this method to return proper name for a new object
//================================================================
QString RepairGUI_FreeBoundDlg::getNewObjectName (int currObj) const
{
  QString aName = tr("NAME_CLOSED").arg(currObj);
  if (currObj > myNbClosed)
    aName = tr("NAME_OPEN").arg(currObj);
  return aName;
}

//================================================================
// Function : getFather
// Purpose  : Get father object for object to be added in study
//            (called with addInStudy method)
//================================================================
GEOM::GEOM_Object_ptr RepairGUI_FreeBoundDlg::getFather (GEOM::GEOM_Object_ptr)
{
  return myObj;
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> RepairGUI_FreeBoundDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res;
  GEOM::GeomObjPtr aGeomObjPtr(myObj);
  res << aGeomObjPtr;
  return res;
}
