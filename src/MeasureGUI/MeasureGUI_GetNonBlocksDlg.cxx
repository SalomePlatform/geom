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
// File   : MeasureGUI_GetNonBlocksDlg.cxx

#include "MeasureGUI_GetNonBlocksDlg.h"

#include <DlgRef.h>
#include <GEOMBase.h>

#include <GeometryGUI.h>

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>
#include <SalomeApp_Tools.h>

//=================================================================================
// class    : MeasureGUI_GetNonBlocksDlg()
// purpose  : Constructs a MeasureGUI_GetNonBlocksDlg which is a child of 'parent',
//            with the name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            true to construct a modal dialog.
//=================================================================================
MeasureGUI_GetNonBlocksDlg::MeasureGUI_GetNonBlocksDlg (GeometryGUI* theGeometryGUI, QWidget* parent)
  : GEOMBase_Skeleton(theGeometryGUI, parent, false)
{
  QPixmap image0 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_GETNONBLOCKS")));
  QPixmap image1 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_SELECT")));

  setWindowTitle(tr("GEOM_GETNONBLOCKS_TITLE"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_GETNONBLOCKS"));
  mainFrame()->RadioButton1->setIcon(image0);
  mainFrame()->RadioButton2->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton3->close();

  myGrp = new DlgRef_1Sel (centralWidget());
  myGrp->GroupBox1->setTitle(tr("GEOM_GETNONBLOCKS"));
  myGrp->TextLabel1->setText(tr("GEOM_OBJECT"));
  myGrp->PushButton1->setIcon(image1);
  myGrp->LineEdit1->setReadOnly(true);

  QVBoxLayout* layout = new QVBoxLayout(centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
  layout->addWidget(myGrp);

  /***************************************************************/

  myHelpFileName = "get_non_blocks_page.html";

  /* Initialisation */
  Init();
}

//=================================================================================
// function : ~MeasureGUI_GetNonBlocksDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
MeasureGUI_GetNonBlocksDlg::~MeasureGUI_GetNonBlocksDlg()
{
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void MeasureGUI_GetNonBlocksDlg::Init()
{
  showOnlyPreviewControl();

  /* init variables */
  myEditCurrentArgument = myGrp->LineEdit1;

   /* signals and slots connections */
  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(myGrp->LineEdit1,   SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(myGrp->PushButton1, SIGNAL(clicked()),       this, SLOT(SetEditCurrentArgument()));

  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(),
           SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  //initName(tr("GEOM_NONBLOCKS"));
  mainFrame()->ResultName->setText(tr("GEOM_NONBLOCKS"));
  globalSelection();
  SelectionIntoArgument();
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void MeasureGUI_GetNonBlocksDlg::ClickOnOk()
{
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool MeasureGUI_GetNonBlocksDlg::ClickOnApply()
{
  if (!onAccept())
    return false;

  //initName();
  return true;
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void MeasureGUI_GetNonBlocksDlg::SelectionIntoArgument()
{
  erasePreview();
  myObj = GEOM::GEOM_Object::_nil();

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  if (aSelList.Extent() != 1) {
    processObject();
    return;
  }

  GEOM::GEOM_Object_var aSelectedObject =
    GEOMBase::ConvertIOinGEOMObject(aSelList.First());

  if (aSelectedObject->_is_nil()) {
    processObject();
    return;
  }

  myObj = aSelectedObject;
  processObject();
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void MeasureGUI_GetNonBlocksDlg::SetEditCurrentArgument()
{
  myGrp->LineEdit1->setFocus();
  myEditCurrentArgument = myGrp->LineEdit1;
  SelectionIntoArgument();
}

//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void MeasureGUI_GetNonBlocksDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if (send == myGrp->LineEdit1) {
    myEditCurrentArgument = myGrp->LineEdit1;
    GEOMBase_Skeleton::LineEditReturnPressed();
  }
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void MeasureGUI_GetNonBlocksDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();

  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(), 
           SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  globalSelection();
  processPreview();
}

//=================================================================================
// function : processObject()
// purpose  : Fill dialog fields in accordance with myObj
//=================================================================================
void MeasureGUI_GetNonBlocksDlg::processObject()
{
  if (myObj->_is_nil()) {
    erasePreview();
  }
  else {
    myGrp->LineEdit1->setText(GEOMBase::GetName(myObj));

    processPreview();
  }
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void MeasureGUI_GetNonBlocksDlg::enterEvent (QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr MeasureGUI_GetNonBlocksDlg::createOperation()
{
  return getGeomEngine()->GetIBlocksOperations(getStudyId());
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool MeasureGUI_GetNonBlocksDlg::isValid (QString&)
{
  return !myObj->_is_nil();
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool MeasureGUI_GetNonBlocksDlg::execute (ObjectList& objects)
{
  GEOM::GEOM_IBlocksOperations_var anOper = GEOM::GEOM_IBlocksOperations::_narrow(getOperation());
  GEOM::GEOM_Object_var aNonQuads;
  GEOM::GEOM_Object_var anObj = anOper->GetNonBlocks(myObj, aNonQuads);
  //mainFrame()->ResultName->setText(tr("GEOM_NONBLOCKS"));

  if (!anObj->_is_nil())
    objects.push_back(anObj._retn());
  if (!aNonQuads->_is_nil())
    objects.push_back(aNonQuads._retn());

  return true;
}

//================================================================
// Function : getFather
// Purpose  : Get father object for object to be added in study
//            ( called with addInStudy method )
//================================================================
GEOM::GEOM_Object_ptr MeasureGUI_GetNonBlocksDlg::getFather (GEOM::GEOM_Object_ptr)
{
  return myObj;
}
