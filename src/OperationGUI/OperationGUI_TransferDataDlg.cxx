// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "OperationGUI_TransferDataDlg.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <SUIT_Desktop.h>
#include <SUIT_MessageBox.h>
#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>

//==============================================================================
// class    : OperationGUI_TransferDataDlg()
// purpose  : Constructs a OperationGUI_TransferDataDlg which is a child of
//            'parent'.
//=================================================================================
OperationGUI_TransferDataDlg::OperationGUI_TransferDataDlg (GeometryGUI* theGeometryGUI,
                                                            QWidget* parent)
  : GEOMBase_Skeleton(theGeometryGUI, parent, false),
    myGroup(NULL)
{
  SUIT_ResourceMgr* aResMgr = myGeomGUI->getApp()->resourceMgr();
  QPixmap image0 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_TRANSFER_DATA")));
  QPixmap image1 (aResMgr->loadPixmap("GEOM", tr("ICON_SELECT")));

  setWindowTitle(tr("GEOM_TRANSFER_DATA_TITLE"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_TRANSFER_DATA"));
  mainFrame()->RadioButton1->setIcon(image0);
  mainFrame()->RadioButton2->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton3->close();

  mainFrame()->GroupBoxName->hide();

  myGroup = new DlgRef_2Sel1List(centralWidget());
  myGroup->GroupBox1->setTitle(tr("GEOM_PARAMETERS"));
  myGroup->TextLabel1->setText(tr("GEOM_TRANSFER_DATA_FROM"));
  myGroup->TextLabel2->setText(tr("GEOM_TRANSFER_DATA_TO"));
  myGroup->TextLabel3->setText(tr("GEOM_TRANSFER_DATA_METHOD"));
  myGroup->PushButton1->setIcon(image1);
  myGroup->PushButton2->setIcon(image1);
  myGroup->LineEdit1->setReadOnly(true);
  myGroup->LineEdit2->setReadOnly(true);

  QVBoxLayout* layout = new QVBoxLayout(centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
  layout->addWidget(myGroup);
  /***************************************************************/

  setHelpFileName("transfer_data_page.html");

  Init();
}

//=================================================================================
// function : ~OperationGUI_TransferDataDlg
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
OperationGUI_TransferDataDlg::~OperationGUI_TransferDataDlg()
{
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void OperationGUI_TransferDataDlg::Init()
{
  // type for detection
  myGroup->ComboBox1->addItem(tr("GEOM_TD_METHOD_GETINPLACE"));
  myGroup->ComboBox1->addItem(tr("GEOM_TD_METHOD_GETINPLACE_OLD"));
  myGroup->ComboBox1->addItem(tr("GEOM_TD_METHOD_GETINPLACE_HISTORY"));

  myGroup->LineEdit1->clear();
  myGroup->LineEdit2->clear();
  myObject1.nullify();
  myObject2.nullify();

  // signals and slots connections
  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(myGroup->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(myGroup->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(myGroup->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(myGroup->LineEdit2, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(myGroup->ComboBox1, SIGNAL(activated(int)), this, SLOT(ComboTextChanged()));

  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));

  myGroup->TextLabel3->show();
  myGroup->ComboBox1->show();
  myGroup->ComboBox1->setCurrentIndex(0);

  globalSelection(GEOM_ALLSHAPES);

  myGroup->PushButton1->click();
  SelectionIntoArgument();
  resize(100,100);
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void OperationGUI_TransferDataDlg::ClickOnOk()
{
  setIsApplyAndClose( true );
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool OperationGUI_TransferDataDlg::ClickOnApply()
{
  if (!onAccept(false))
    return false;
  // activate first line edit
  myGroup->PushButton1->click();
  return true;
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection is changed or on dialog initialization or activation
//=================================================================================
void OperationGUI_TransferDataDlg::SelectionIntoArgument()
{
  GEOM::GeomObjPtr aSelectedObject = getSelected(TopAbs_SHAPE);

  if (aSelectedObject) {
    myEditCurrentArgument->setText(GEOMBase::GetName(aSelectedObject.get()));

    if (myEditCurrentArgument == myGroup->LineEdit1) {
      myObject1 = aSelectedObject;

      if (!myObject2) {
        myGroup->PushButton2->click();
      }
    } else if (myEditCurrentArgument == myGroup->LineEdit2) {
      myObject2 = aSelectedObject;

      if (!myObject1) {
        myGroup->PushButton1->click();
      }
    }
  } else {
    myEditCurrentArgument->setText("");

    if (myEditCurrentArgument == myGroup->LineEdit1) {
      myObject1.nullify();
    } else if (myEditCurrentArgument == myGroup->LineEdit2) {
      myObject2.nullify();
    }
  }
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void OperationGUI_TransferDataDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  if      (send == myGroup->PushButton1) {
    myEditCurrentArgument = myGroup->LineEdit1;

    myGroup->PushButton2->setDown(false);
    myGroup->LineEdit2->setEnabled(false);
  }
  else if (send == myGroup->PushButton2) {
    myEditCurrentArgument = myGroup->LineEdit2;

    myGroup->PushButton1->setDown(false);
    myGroup->LineEdit1->setEnabled(false);
  }

  // enable line edit
  myEditCurrentArgument->setEnabled(true);
  myEditCurrentArgument->setFocus();
  // after setFocus(), because it will be setDown(false) when loses focus
  send->setDown(true);
}

//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void OperationGUI_TransferDataDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if (send == myGroup->LineEdit1 || send == myGroup->LineEdit2)
  {
    myEditCurrentArgument = send;
    GEOMBase_Skeleton::LineEditReturnPressed();
  }
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void OperationGUI_TransferDataDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  globalSelection(GEOM_ALLSHAPES);
  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
           this, SLOT( SelectionIntoArgument() ) );
}

//=================================================================================
// function : enterEvent()
// purpose  : when mouse enter onto the QWidget
//=================================================================================
void OperationGUI_TransferDataDlg::enterEvent (QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr OperationGUI_TransferDataDlg::createOperation()
{
  return getGeomEngine()->GetIInsertOperations(getStudyId());
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool OperationGUI_TransferDataDlg::isValid(QString&)
{
  return myObject1 && myObject2;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool OperationGUI_TransferDataDlg::execute (ObjectList& objects)
{
  const int aLimit = myGroup->ComboBox1->currentIndex();
  GEOM::find_shape_method aMethod;

  switch (aLimit)
  {
  case 0:
    aMethod = GEOM::FSM_GetInPlace;
    break;
  case 1:
    aMethod = GEOM::FSM_GetInPlace_Old;
    break;
  case 2:
  default:
    aMethod = GEOM::FSM_GetInPlaceByHistory;
    break;
  }

  GEOM::GEOM_IInsertOperations_var anOper =
    GEOM::GEOM_IInsertOperations::_narrow(getOperation());
  GEOM::GEOM_IInsertOperations::ListOfTransferDatum_var aResult;

  bool isOK = anOper->TransferData
    (myObject1.get(), myObject2.get(), aMethod, aResult);

  if (isOK) {
    // Construct info message.
    QString aMsg;
    int aNbTypes = aResult->length();

    if (aNbTypes == 0) {
      aMsg = tr("GEOM_TRANSFER_DATA_NOT_COPIED");
    } else {
      aMsg = tr("GEOM_TRANSFER_DATA_COPIED");
      aMsg += "\n";
      int i;

      for (i = 0; i < aNbTypes; i++ ) {
        if (i > 0) {
          aMsg += "\n";
        }

        aMsg += tr(aResult[i].myName).
                  arg(aResult[i].myNumber).arg(aResult[i].myMaxNumber);
      }
    }

    // Show info message.
    SUIT_MessageBox::information
            (SUIT_Session::session()->activeApplication()->desktop(),
             tr("GEOM_TRANSFER_DATA_INFO"), aMsg, tr("BUT_OK"));

    objects.push_back(myObject2.copy());
  }

  return isOK;
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> OperationGUI_TransferDataDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res;
  res << myObject1;
  return res;
}
