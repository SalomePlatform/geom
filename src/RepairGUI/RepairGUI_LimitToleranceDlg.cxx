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
//  File   : RepairGUI_LimitToleranceDlg.cxx

#include "RepairGUI_LimitToleranceDlg.h"

#include "DlgRef.h"
#include "GeometryGUI.h"
#include "GEOMBase.h"
#include "RepairGUI.h"

#include <SalomeApp_DoubleSpinBox.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>
#include <SalomeApp_Study.h>
#include <SalomeApp_Tools.h>
#include <SUIT_Session.h>
#include <SUIT_Desktop.h>
#include <SUIT_MessageBox.h>
#include <SUIT_OverrideCursor.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_ViewWindow.h>
#include <SUIT_ViewManager.h>
#include <OCCViewer_ViewModel.h>
#include <SALOME_ListIO.hxx>
#include "utilities.h"

#include <GEOMImpl_Types.hxx>

#include <TCollection_AsciiString.hxx>

#define DEFAULT_TOLERANCE_VALUE 1e-07

//=================================================================================
// class    : RepairGUI_LimitToleranceDlg()
// purpose  : Constructs a RepairGUI_LimitToleranceDlg  which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
RepairGUI_LimitToleranceDlg::RepairGUI_LimitToleranceDlg(GeometryGUI* theGeometryGUI,
                                                         QWidget* parent, bool modal)
  : GEOMBase_Skeleton(theGeometryGUI, parent, modal)
{
  QPixmap image0 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_LIMIT_TOLERANCE")));
  QPixmap image1 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_SELECT")));

  setWindowTitle(tr("GEOM_LIMIT_TOLERANCE_TITLE"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_LIMIT_TOLERANCE_TITLE"));
  mainFrame()->RadioButton1->setIcon(image0);
  mainFrame()->RadioButton2->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton3->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->close();

  GroupPoints = new DlgRef_1SelExt(centralWidget());
  GroupPoints->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  GroupPoints->TextLabel1->setText(tr("GEOM_SELECTED_SHAPE"));
  GroupPoints->PushButton1->setIcon(image1);
  GroupPoints->LineEdit1->setReadOnly(true);

  QLabel* aTolLab = new QLabel(tr("GEOM_TOLERANCE"), GroupPoints->Box);
  myTolEdt = new SalomeApp_DoubleSpinBox(GroupPoints->Box);
  initSpinBox(myTolEdt, 0., 100., DEFAULT_TOLERANCE_VALUE, "len_tol_precision");
  myTolEdt->setValue(DEFAULT_TOLERANCE_VALUE);

  QGridLayout* boxLayout = new QGridLayout(GroupPoints->Box);
  boxLayout->setMargin(0); boxLayout->setSpacing(6);
  boxLayout->addWidget(aTolLab,  0, 0);
  boxLayout->addWidget(myTolEdt, 0, 2);

  QVBoxLayout* layout = new QVBoxLayout(centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
  layout->addWidget(GroupPoints);

  setHelpFileName("limit_tolerance_operation_page.html");

  Init();
}


//=================================================================================
// function : ~RepairGUI_LimitToleranceDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
RepairGUI_LimitToleranceDlg::~RepairGUI_LimitToleranceDlg()
{
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void RepairGUI_LimitToleranceDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupPoints->LineEdit1;

  myObject = GEOM::GEOM_Object::_nil();

  //myGeomGUI->SetState(0);
  //globalSelection(GEOM_COMPOUND);

  mainFrame()->GroupBoxPublish->show();
  //Hide preview checkbox
  mainFrame()->CheckBoxPreview->hide();

  /* signals and slots connections */
  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(GroupPoints->PushButton1,  SIGNAL(clicked()),       this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->LineEdit1,    SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));

  initName(tr("LIMIT_TOLERANCE_NEW_OBJ_NAME"));

  ConstructorsClicked(0);

  activateSelection();
  updateButtonState();
}


//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void RepairGUI_LimitToleranceDlg::ConstructorsClicked(int constructorId)
{
  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);

  GroupPoints->show();
  GroupPoints->LineEdit1->setText("");
  myEditCurrentArgument = GroupPoints->LineEdit1;
  myEditCurrentArgument->setFocus();

  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
           this, SLOT(SelectionIntoArgument()));

  qApp->processEvents();
  updateGeometry();
  resize(minimumSizeHint());

  updateButtonState();
  activateSelection();
  SelectionIntoArgument();
}


//=================================================================================
// function : ClickOnOk()
// purpose  : Same than click on apply but close this dialog.
//=================================================================================
void RepairGUI_LimitToleranceDlg::ClickOnOk()
{
  setIsApplyAndClose( true );
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool RepairGUI_LimitToleranceDlg::ClickOnApply()
{
  if (!onAcceptLocal())
    return false;

  initName();

  ConstructorsClicked(0);

  return true;
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection
//=================================================================================
void RepairGUI_LimitToleranceDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("");
  myObject = GEOM::GEOM_Object::_nil();

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  if (aSelList.Extent() == 1) {
    Handle(SALOME_InteractiveObject) anIO = aSelList.First();
    myObject = GEOMBase::ConvertIOinGEOMObject( anIO );
    if ( !CORBA::is_nil( myObject ) )
      myEditCurrentArgument->setText(GEOMBase::GetName(myObject));
  }
  updateButtonState();
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void RepairGUI_LimitToleranceDlg::SetEditCurrentArgument()
{
  const QObject* send = sender();
  if (send == GroupPoints->PushButton1)  {
    myEditCurrentArgument->setFocus();
    SelectionIntoArgument();
  }
}

//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void RepairGUI_LimitToleranceDlg::LineEditReturnPressed()
{
  const QObject* send = sender();
  if (send == GroupPoints->LineEdit1) {
    myEditCurrentArgument = GroupPoints->LineEdit1;
    GEOMBase_Skeleton::LineEditReturnPressed();
  }
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void RepairGUI_LimitToleranceDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
           this, SLOT(SelectionIntoArgument()));
  activateSelection();
}

//=================================================================================
// function : enterEvent()
// purpose  : Mouse enter onto the dialog to activate it
//=================================================================================
void RepairGUI_LimitToleranceDlg::enterEvent(QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr RepairGUI_LimitToleranceDlg::createOperation()
{
  return getGeomEngine()->GetIHealingOperations(getStudyId());
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool RepairGUI_LimitToleranceDlg::isValid(QString& msg)
{
  double v = myTolEdt->value();
  bool ok = myTolEdt->isValid(msg, true);
  return !myObject->_is_nil() && (v > 0.) && ok;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool RepairGUI_LimitToleranceDlg::execute(ObjectList& objects)
{
  bool aResult = false;
  objects.clear();

  GEOM::GEOM_IHealingOperations_var anOper = GEOM::GEOM_IHealingOperations::_narrow(getOperation());
  GEOM::GEOM_Object_var anObj = anOper->LimitTolerance(myObject, myTolEdt->value());
  aResult = !anObj->_is_nil();
  if (aResult) {
    QStringList aParameters;
    aParameters << myTolEdt->text();
    anObj->SetParameters(aParameters.join(":").toLatin1().constData());
    if ( !IsPreview() )
      RepairGUI::ShowStatistics( anOper, this );
    objects.push_back(anObj._retn());
  }

  return aResult;
}

//================================================================
// Function : onAccept
// Purpose  : This method should be called from dialog's slots onOk() and onApply()
//            It perfroms user input validation, then it
//            performs a proper operation and manages transactions, etc.
//================================================================
bool RepairGUI_LimitToleranceDlg::onAcceptLocal()
{
  if (!getStudy() || !(getStudy()->studyDS()))
    return false;

  _PTR(Study) aStudy = getStudy()->studyDS();

  bool aLocked = aStudy->GetProperties()->IsLocked();
  if (aLocked) {
    SUIT_MessageBox::warning(this, tr("WRN_WARNING"), tr("WRN_STUDY_LOCKED"), tr("BUT_OK"));
    return false;
  }

  QString msg;
  if (!isValid(msg)) {
    showError(msg);
    return false;
  }

  try {
    if (openCommand()) {
      SUIT_OverrideCursor wc;

      myGeomGUI->getApp()->putInfo("");
      ObjectList objects;

      if (!execute(objects)) {
        wc.suspend();
        abortCommand();
        showError();
      }
      else {
        const int nbObjs = objects.size();
        for (ObjectList::iterator it = objects.begin(); it != objects.end(); ++it) {
          QString aName = getNewObjectName();
          if (nbObjs > 1) {
            if (aName.isEmpty())
              aName = getPrefix(*it);
            aName = GEOMBase::GetDefaultName(aName);
          }
          else {
            // PAL6521: use a prefix, if some dialog box doesn't reimplement getNewObjectName()
            if (aName.isEmpty())
              aName = GEOMBase::GetDefaultName(getPrefix(*it));
          }
          addInStudy(*it, aName.toLatin1().data());
          display(*it, false);
        }

        if (nbObjs) {
          commitCommand();
          updateObjBrowser();
          myGeomGUI->getApp()->putInfo(QObject::tr("GEOM_PRP_DONE"));
        }
        else {
          abortCommand();
        }

        // JFA 28.12.2004 BEGIN // To enable warnings
        GEOM::GEOM_IHealingOperations_var anOper = GEOM::GEOM_IHealingOperations::_narrow(getOperation());
        if (!CORBA::is_nil(anOper) && !anOper->IsDone()) {
          wc.suspend();
          QString msgw = QObject::tr(anOper->GetErrorCode());
          SUIT_MessageBox::warning(this, tr("WRN_WARNING"), msgw, tr("BUT_OK"));
        }
        // JFA 28.12.2004 END
      }
    }
  }
  catch(const SALOME::SALOME_Exception& e) {
    SalomeApp_Tools::QtCatchCorbaException(e);
    abortCommand();
  }

  updateViewer();
  activateSelection();
  updateButtonState();

  return true;
}

//=================================================================================
// function : activateSelection
// purpose  : Activate selection
//=================================================================================
void RepairGUI_LimitToleranceDlg::activateSelection()
{
  disconnect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
             this, SLOT(SelectionIntoArgument()));

  globalSelection(GEOM_ALLSHAPES);
  if (myObject->_is_nil())
    SelectionIntoArgument();

  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));
  updateViewer();
}

//=================================================================================
// function : updateButtonState
// purpose  : Update button state
//=================================================================================
void RepairGUI_LimitToleranceDlg::updateButtonState()
{
  bool hasMainObj = !myObject->_is_nil();
  buttonOk()->setEnabled(hasMainObj);
  buttonApply()->setEnabled(hasMainObj);
}

//=================================================================================
// function : restoreSubShapes
// purpose  :
//=================================================================================
void RepairGUI_LimitToleranceDlg::restoreSubShapes(SALOMEDS::Study_ptr   theStudy,
                                                   SALOMEDS::SObject_ptr theSObject)
{
  if (mainFrame()->CheckBoxRestoreSS->isChecked()) {
    // empty list of arguments means that all arguments should be restored
    getGeomEngine()->RestoreSubShapesSO(theStudy, theSObject, GEOM::ListOfGO(),
                                        GEOM::FSM_GetInPlace, /*theInheritFirstArg=*/true,
                                        mainFrame()->CheckBoxAddPrefix->isChecked());
  }
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> RepairGUI_LimitToleranceDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res;
  GEOM::GeomObjPtr aGeomObjPtr(myObject);
  res << aGeomObjPtr;
  return res;
}
