//  GEOM GEOMGUI : GUI for Geometry component
//
//  Copyright (C) 2003  CEA
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : BlocksGUI_ExplodeDlg.cxx
//  Author : Julia DOROVSKIKH
//  Module : GEOM
//  $Header$

#include "BlocksGUI_ExplodeDlg.h"

#include "DlgRef_SpinBox.h"

#include "GEOM_Displayer.h"

#include "SUIT_Session.h"
#include "SalomeApp_Application.h"
#include "LightApp_SelectionMgr.h"
#include "OCCViewer_ViewModel.h"
#include "SALOME_ListIteratorOfListIO.hxx"

#include "utilities.h"

#include <TColStd_IndexedMapOfInteger.hxx>

#include <qmessagebox.h>
#include <qtextedit.h>
#include <qcheckbox.h>
#include <qlabel.h>

//=================================================================================
// class    : BlocksGUI_ExplodeDlg()
// purpose  : Constructs a BlocksGUI_ExplodeDlg which is a child of 'parent'.
//=================================================================================
BlocksGUI_ExplodeDlg::BlocksGUI_ExplodeDlg (GeometryGUI* theGeometryGUI, QWidget* parent, bool modal)
     : GEOMBase_Skeleton(theGeometryGUI, parent, "ExplodeDlg", modal,
                         WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image1 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_DLG_BLOCK_EXPLODE")));
  QPixmap imageS (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_BLOCK_EXPLODE_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_BLOCK_EXPLODE"));

  RadioButton1->setPixmap(image1);
  RadioButton2->close(TRUE);
  RadioButton3->close(TRUE);

  // Create first group
  myGrp1 = new QGroupBox(1, Qt::Horizontal, tr("GEOM_ARGUMENTS"), this);

  QGroupBox* aSelGrp = new QGroupBox(3, Qt::Horizontal, myGrp1);
  aSelGrp->setFrameStyle(QFrame::NoFrame);
  aSelGrp->setInsideMargin(0);

  new QLabel(tr("GEOM_MAIN_OBJECT"), aSelGrp);
  mySelBtn = new QPushButton(aSelGrp);
  mySelBtn->setPixmap(imageS);
  mySelName = new QLineEdit(aSelGrp);
  mySelName->setReadOnly(true);

  QGroupBox* aSpinGrp = new QGroupBox(2, Qt::Horizontal, myGrp1);
  aSpinGrp->setFrameStyle(QFrame::NoFrame);
  aSpinGrp->setInsideMargin(0);

  new QLabel(tr("NB_FACES_MIN"), aSpinGrp);
  mySpinBoxMin = new DlgRef_SpinBox(aSpinGrp);

  new QLabel(tr("NB_FACES_MAX"), aSpinGrp);
  mySpinBoxMax = new DlgRef_SpinBox(aSpinGrp);

  QGroupBox* anInfoGrp = new QGroupBox(2, Qt::Horizontal, myGrp1);
  anInfoGrp->setFrameStyle(QFrame::NoFrame);
  anInfoGrp->setInsideMargin(0);

  myBlocksNb = new QTextEdit(anInfoGrp);
  myBlocksNb->setReadOnly(true);

  QGroupBox* aCheckGrp = new QGroupBox(3, Qt::Horizontal, myGrp1);
  aCheckGrp->setFrameStyle(QFrame::NoFrame);
  aCheckGrp->setInsideMargin(0);

  myCheckBtn = new QCheckBox(aCheckGrp, "CheckButton1");
  myCheckBtn->setText(tr("GEOM_SUBSHAPE_SELECT"));

  // Add groups to layout
  Layout1->addWidget(myGrp1, 1, 0);
  /***************************************************************/

  setHelpFileName("explode_on_blocks.htm");

  Init();
}

//=================================================================================
// function : ~BlocksGUI_ExplodeDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
BlocksGUI_ExplodeDlg::~BlocksGUI_ExplodeDlg()
{
  // no need to delete child widgets, Qt does it all for us
  clearTemporary();
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BlocksGUI_ExplodeDlg::Init()
{
  // Set range of spinboxes
  double SpecificStep = 1.0;
  mySpinBoxMin->RangeStepAndValidator(0.0, 999.0, SpecificStep, 3);
  mySpinBoxMax->RangeStepAndValidator(0.0, 999.0, SpecificStep, 3);

  if (SUIT_Session::session()->activeApplication()->desktop()->activeWindow()->getViewManager()->getType() 
      != OCCViewer_Viewer::Type())
    myCheckBtn->setEnabled(false);

  // signals and slots connections
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(mySelBtn, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(mySpinBoxMin, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(mySpinBoxMax, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));

  connect(myCheckBtn, SIGNAL(stateChanged(int)), this, SLOT(SubShapeToggled()));

  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument())) ;

  myConstructorId = -1;
  ConstructorsClicked(0);
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void BlocksGUI_ExplodeDlg::ConstructorsClicked (int constructorId)
{
  if (myConstructorId == constructorId)
    return;

  myConstructorId = constructorId;

  switch (constructorId) {
  case 0:
    myGrp1->show();
    mySpinBoxMin->SetValue(6.0);
    mySpinBoxMax->SetValue(6.0);
    myCheckBtn->setChecked(FALSE);
    break;
  default:
    break;
  }

  // init fields
  myEditCurrentArgument = mySelName;
  myObject = GEOM::GEOM_Object::_nil();

  activateSelection();
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void BlocksGUI_ExplodeDlg::ClickOnOk()
{
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool BlocksGUI_ExplodeDlg::ClickOnApply()
{
  SUIT_Session::session()->activeApplication()->putInfo(tr(""));

  // Explode all sub shapes
  if (isAllSubShapes()) {
    // More than 30 subshapes : ask confirmation
    if (myNbBlocks > 30) {
      const QString caption = tr("GEOM_CONFIRM");
      const QString text = tr("GEOM_CONFIRM_INFO").arg(myNbBlocks);
      const QString button0 = tr("GEOM_BUT_EXPLODE");
      const QString button1 = tr("GEOM_BUT_CANCEL");

      if (QMessageBox::warning(this, caption, text, button0, button1) != 0)
        return false;  /* aborted */
    }
  }

  if (!onAccept())
    return false;

  activateSelection();

  return true;
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection has changed
//=================================================================================
void BlocksGUI_ExplodeDlg::SelectionIntoArgument()
{
  if (!isAllSubShapes())
    return;

  myObject = GEOM::GEOM_Object::_nil();
  mySelName->setText("");

  if (IObjectCount() == 1) {
    Standard_Boolean aResult = Standard_False;
    GEOM::GEOM_Object_var anObj =
      GEOMBase::ConvertIOinGEOMObject(firstIObject(), aResult);

    if ( aResult && !anObj->_is_nil() && GEOMBase::IsShape( anObj ) ) {
      myObject = anObj;
      mySelName->setText(GEOMBase::GetName(anObj));
    }
  }

  updateButtonState();
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BlocksGUI_ExplodeDlg::SetEditCurrentArgument()
{
  QPushButton* aSender = (QPushButton*)sender();

  if (mySelBtn == aSender) {
    mySelName->setFocus();
    myEditCurrentArgument = mySelName;
    myCheckBtn->setChecked(FALSE);
  }

  activateSelection();
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void BlocksGUI_ExplodeDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument())) ;

  activateSelection();
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void BlocksGUI_ExplodeDlg::enterEvent (QEvent* e)
{
  if (!GroupConstructors->isEnabled())
    this->ActivateThisDialog();
}

//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void BlocksGUI_ExplodeDlg::ValueChangedInSpinBox (double newValue)
{
  if (!isAllSubShapes())
    activateSelection();
  else
    updateButtonState();
}

//=================================================================================
// function : SubShapeToggled()
// purpose  : Allow user selection of all or only selected sub shapes
//          : Called when 'myCheckBtn' state change
//=================================================================================
void BlocksGUI_ExplodeDlg::SubShapeToggled()
{
  activateSelection();
}

//=================================================================================
// function : activateSelection
// purpose  : Redisplay preview and Activate selection
//=================================================================================
void BlocksGUI_ExplodeDlg::activateSelection()
{
  clearTemporary();
  erasePreview(true);

  if (isAllSubShapes()) { // Sub-shapes selection disabled
    disconnect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 
	       SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument())) ;
    globalSelection( GEOM_ALLSHAPES );
    if (myObject->_is_nil()) {
      SelectionIntoArgument();
    }
    connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 
	    SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument())) ;
  } else {
    displayPreview(true, true, false);
    globalSelection(GEOM_PREVIEW);
  }
}

//=================================================================================
// function : updateButtonState
// purpose  :
//=================================================================================
void BlocksGUI_ExplodeDlg::updateButtonState()
{
  if (SUIT_Session::session()->activeApplication()->desktop()->activeWindow()->getViewManager()->getType() 
      != OCCViewer_Viewer::Type() || myObject->_is_nil()) {
    myCheckBtn->setChecked(FALSE);
    myCheckBtn->setEnabled(FALSE);
  } else {
    myCheckBtn->setEnabled(TRUE);
  }

  myNbBlocks = 0;

  if (myObject->_is_nil()) {
    myBlocksNb->setText("");
  } else {
    bool isOnlyBlocks = GEOM::GEOM_IBlocksOperations::_narrow
      (getOperation())->IsCompoundOfBlocks(myObject,
                                           (int)mySpinBoxMin->GetValue(),
                                           (int)mySpinBoxMax->GetValue(),
                                           myNbBlocks);
    if (isOnlyBlocks)
      myBlocksNb->setText(tr("GEOM_NB_BLOCKS_NO_OTHERS").arg(myNbBlocks));
    else
      myBlocksNb->setText(tr("GEOM_NB_BLOCKS_SOME_OTHERS").arg(myNbBlocks));
  }
}

//=================================================================================
// function : isAllSubShapes
// purpose  :
//=================================================================================
bool BlocksGUI_ExplodeDlg::isAllSubShapes() const
{
  return !myCheckBtn->isChecked() || !myCheckBtn->isEnabled();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr BlocksGUI_ExplodeDlg::createOperation()
{
  return getGeomEngine()->GetIBlocksOperations(getStudyId());
}

//=================================================================================
// function : isValid()
// purpose  : Verify validity of input data
//=================================================================================
bool BlocksGUI_ExplodeDlg::isValid (QString&)
{
  switch (getConstructorId()) {
  case 0:
    if (IsPreview())
      return !myObject->_is_nil();
    else
      return !myObject->_is_nil() && (isAllSubShapes() || IObjectCount());
  default:
    return false;
  }

  return false;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool BlocksGUI_ExplodeDlg::execute (ObjectList& objects)
{
  GEOM::ListOfGO_var aList;

  switch (getConstructorId()) {
    case 0:
      aList = GEOM::GEOM_IBlocksOperations::_narrow(getOperation())->ExplodeCompoundOfBlocks
        (myObject,
         (int)mySpinBoxMin->GetValue(),
         (int)mySpinBoxMax->GetValue());
      break;
  }

  if (!aList->length())
    return false;

  if (IsPreview()) {
    clearTemporary();

    // Store objects. They will be put in study when "Apply" is pressed
    for (int i = 0, n = aList->length(); i < n; i++) {
      objects.push_back(GEOM::GEOM_Object::_duplicate(aList[i]));
      myTmpObjs.push_back(GEOM::GEOM_Object::_duplicate(aList[i]));
    }

    return objects.size() ? true : false;
  }

  // Throw away sub-shapes not selected by user if not in preview mode
  // and manual selection is active
  if (!isAllSubShapes())
  {
    QMap<QString, char> selected;

    // Get names of selected objects
    SALOME_ListIteratorOfListIO it (selectedIO());
    for (; it.More(); it.Next()) {
      selected.insert(it.Value()->getName(), 0);
    }

    // Iterate through result and select objects with names from selection
    ObjectList toRemoveFromEnggine;
    ObjectList::iterator anIter;
    for (anIter = myTmpObjs.begin(); anIter != myTmpObjs.end(); ++anIter) {
      CORBA::String_var objStr = myGeomGUI->getApp()->orb()->object_to_string(*anIter);
      if (selected.contains(QString(objStr.in())))
        objects.push_back(*anIter);
      else
        toRemoveFromEnggine.push_back(*anIter);
    }

    // Remove from engine useless objects
    ObjectList::iterator anIter2 = toRemoveFromEnggine.begin();
    for (; anIter2 != toRemoveFromEnggine.end(); ++anIter2)
      getGeomEngine()->RemoveObject(*anIter2);

    myTmpObjs.clear();

  } else {
    for (int i = 0, n = aList->length(); i < n; i++)
      objects.push_back(GEOM::GEOM_Object::_duplicate(aList[i]));
  }

  return objects.size();
}

//=================================================================================
// function : clearTemporary
// purpose  : Remove temporary objects from engine
//=================================================================================
void BlocksGUI_ExplodeDlg::clearTemporary()
{
  ObjectList::iterator anIter;
  for (anIter = myTmpObjs.begin(); anIter != myTmpObjs.end(); ++anIter)
    getGeomEngine()->RemoveObject(*anIter);

  myTmpObjs.clear();
}

//================================================================
// Function : getFather
// Purpose  : Get father object for object to be added in study
//            ( called with addInStudy method )
//================================================================
GEOM::GEOM_Object_ptr BlocksGUI_ExplodeDlg::getFather (GEOM::GEOM_Object_ptr)
{
  return myObject;
}

//================================================================
// Function : getNewObjectName
// Purpose  : Redefine this method to return proper name for a new object
//================================================================
const char* BlocksGUI_ExplodeDlg::getNewObjectName() const
{
  return "";
}
