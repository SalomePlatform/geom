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
//  See http://www.salome-platform.org or email : webmaster.salome@opencascade.org
//
//
//
//  File   : OperationGUI_MaterialDlg.cxx
//  Author : Julia DOROVSKIKH
//  Module : GEOM
//  $Header$

using namespace std;
#include "OperationGUI_MaterialDlg.h"
#include "OperationGUI_PartitionDlg.h"

#include "utilities.h"
#include "QAD_Desktop.h"

#include <qlistview.h>
#include <qspinbox.h>

//=================================================================================
// class    : OperationGUI_MaterialDlg()
// purpose  : Constructs a OperationGUI_MaterialDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
OperationGUI_MaterialDlg::OperationGUI_MaterialDlg (QWidget* parent, const char* name,
                                                    SALOME_Selection* Sel,
                                                    GEOM::ListOfGO ListShapes,
                                                    bool modal, WFlags fl)
  :GEOMBase_Skeleton(parent, name, Sel, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  myListShapes = ListShapes;
  myParentDlg = parent;

  QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_DLG_PARTITION")));

  setCaption(tr("GEOM_MATERIAL_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_PARTITION"));
  RadioButton1->setPixmap(image0);
  RadioButton2->close(TRUE);
  RadioButton3->close(TRUE);

  GroupPoints = new DlgRef_1List1Spin1Btn_QTD(this, "GroupPoints");
  GroupPoints->GroupBox1->setTitle(tr("GEOM_PARTITION"));

  myShapeCol = GroupPoints->ListView1->addColumn(tr("GEOM_MATERIAL_SHAPE"));
  myMaterCol = GroupPoints->ListView1->addColumn(tr("GEOM_MATERIAL_MATERIAL"));
  GroupPoints->ListView1->setSelectionMode(QListView::Extended);

  GroupPoints->TextLabel1->setText(tr("GEOM_MATERIAL_ID"));
  GroupPoints->PushButton1->setText(tr("GEOM_MATERIAL_SET"));

  Layout1->addWidget(GroupPoints, 1, 0);
  /***************************************************************/
 
  Init();
}


//=================================================================================
// function : ~OperationGUI_MaterialDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
OperationGUI_MaterialDlg::~OperationGUI_MaterialDlg()
{
  // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void OperationGUI_MaterialDlg::Init()
{
  // get materials list from the parent dialog
  OperationGUI_PartitionDlg* aParentDlg =
    dynamic_cast<OperationGUI_PartitionDlg*>(myParentDlg);
  if (aParentDlg)
    myListMaterials = aParentDlg->GetListMaterials();
  
  /* list filling */
  MESSAGE("Filling list with " << myListShapes.length() << " objects");
  QString aMaterialId;
  for (int ind = 0; ind < myListShapes.length(); ind++) {
    GEOM::GEOM_Object_var anObject = myListShapes[ind];
    if (!anObject->_is_nil()) {
      MESSAGE("Insert " << GEOMBase::GetName( anObject ));
      if (ind < myListMaterials.length())
	aMaterialId = QString::number(myListMaterials[ind]);
      else 
	aMaterialId = "0";
      QListViewItem* aItem =
        new QListViewItem(GroupPoints->ListView1, GEOMBase::GetName( anObject ), aMaterialId);
    }
  }
  MESSAGE("Filled");

  /* signals and slots connections */
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  buttonApply->close(TRUE);
  buttonCancel->setText(tr("GEOM_BUT_CANCEL"));

  connect(GroupPoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetMaterial()));

//  connect(GroupPoints->ListView1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
//  connect(GroupPoints->SpinBox1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  /* displays Dialog */
  MESSAGE("GroupPoints->show() ...");
  GroupPoints->show();
  MESSAGE("this->show() ...");
  this->show();
  MESSAGE("return");

  return;
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void OperationGUI_MaterialDlg::ClickOnOk()
{
  QAD_Application::getDesktop()->putInfo(tr(""));

  int nbSh = myListShapes.length();  
  myListMaterials.length(nbSh);

  QListViewItemIterator it (GroupPoints->ListView1);
  for (int i = 0; it.current() != 0; it++, i++) {
    QString aMatIdStr = it.current()->text(myMaterCol);
    myListMaterials[i] = aMatIdStr.toInt();
    MESSAGE("For shape # " << i << " material ID = " << myListMaterials[i]);
  }

  // set materials list to the parent dialog
  OperationGUI_PartitionDlg* aParentDlg =
    dynamic_cast<OperationGUI_PartitionDlg*>(myParentDlg);
  if (aParentDlg)
  {
    aParentDlg->SetListMaterials(myListMaterials);
  }

  ClickOnCancel();
  return;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void OperationGUI_MaterialDlg::SelectionIntoArgument()
{
  QString aString = ""; /* name of selection */

  int nbSel = myGeomBase->GetNameOfSelectedIObjects(mySelection, aString);
  if(nbSel < 1) {
    return;
  }

//  myGeomBase->ConvertListOfIOInListOfIOR(mySelection->StoredIObjects(), myListShapes);

  /* no simulation */
  return;
}


//=================================================================================
// function : SetMaterial()
// purpose  :
//=================================================================================
void OperationGUI_MaterialDlg::SetMaterial()
{
  int aMatId = GroupPoints->SpinBox1->value();
  QString aMatIdStr;
  aMatIdStr.setNum(aMatId);
  QListViewItemIterator it (GroupPoints->ListView1);
  for (; it.current() != 0; it++) {
    if (it.current()->isSelected())
      it.current()->setText(myMaterCol, aMatIdStr);
  }
  return;
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void OperationGUI_MaterialDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
  return;
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void OperationGUI_MaterialDlg::enterEvent(QEvent* e)
{
  if(!GroupConstructors->isEnabled())
    this->ActivateThisDialog();
}
