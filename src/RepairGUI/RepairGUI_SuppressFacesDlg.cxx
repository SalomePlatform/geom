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
//  File   : RepairGUI_SuppressFacesDlg.cxx
//  Author : Lucien PIGNOLONI, Open CASCADE S.A.S.

#include "RepairGUI_SuppressFacesDlg.h"

#include "DlgRef.h"
#include "GeometryGUI.h"
#include "GEOMBase.h"
#include "RepairGUI.h"

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>
#include <LightApp_SelectionMgr.h>
#include <SALOME_ListIO.hxx>

// OCCT Includes
#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>

#include <GEOMImpl_Types.hxx>

//=================================================================================
// class    : RepairGUI_SuppressFacesDlg()
// purpose  : Constructs a RepairGUI_SuppressFacesDlg  which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
RepairGUI_SuppressFacesDlg::RepairGUI_SuppressFacesDlg (GeometryGUI* theGeometryGUI, QWidget* parent,
                                                        bool modal)
  : GEOMBase_Skeleton(theGeometryGUI, parent, modal)
{
  QPixmap image0 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_SUPRESS_FACE")));
  QPixmap image1 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_SELECT")));

  setWindowTitle(tr("GEOM_SUPRESSFACE_TITLE"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_SUPRESSFACE"));
  mainFrame()->RadioButton1->setIcon(image0);
  mainFrame()->RadioButton2->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton3->close();

  GroupArgs = new DlgRef_2Sel(centralWidget());
  GroupArgs->GroupBox1->setTitle(tr("Faces to remove"));
  GroupArgs->TextLabel1->setText(tr("GEOM_SELECTED_SHAPE"));
  GroupArgs->PushButton1->setIcon(image1);
  GroupArgs->LineEdit1->setReadOnly(true);

  GroupArgs->TextLabel2->setText(tr("Faces to remove"));
  GroupArgs->PushButton2->setIcon(image1);
  GroupArgs->LineEdit2->setReadOnly(true);

  QVBoxLayout* layout = new QVBoxLayout(centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
  layout->addWidget(GroupArgs);
  /***************************************************************/

  setHelpFileName("suppress_faces_operation_page.html");

  Init();
}

//=================================================================================
// function : ~RepairGUI_SuppressFacesDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
RepairGUI_SuppressFacesDlg::~RepairGUI_SuppressFacesDlg()
{
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void RepairGUI_SuppressFacesDlg::Init()
{
  // init variables
  GroupArgs->LineEdit1->clear();
  GroupArgs->LineEdit2->clear();
  myObject = GEOM::GEOM_Object::_nil();
  myFacesInd = new GEOM::short_array();
  myFacesInd->length(0);

  mainFrame()->GroupBoxPublish->show();
  //Hide preview checkbox
  mainFrame()->CheckBoxPreview->hide();

  // signals and slots connections
  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(GroupArgs->PushButton1, SIGNAL(clicked()),       this, SLOT(SetEditCurrentArgument()));
  connect(GroupArgs->PushButton2, SIGNAL(clicked()),       this, SLOT(SetEditCurrentArgument()));

  connect(GroupArgs->LineEdit1,   SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupArgs->LineEdit2,   SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  initName(tr("SUPRESS_FACE_NEW_OBJ_NAME"));

  GroupArgs->PushButton1->click();
  SelectionIntoArgument();
  resize(100,100);
}

//=================================================================================
// function : ClickOnOk()
// purpose  : Same than click on apply but close this dialog.
//=================================================================================
void RepairGUI_SuppressFacesDlg::ClickOnOk()
{
  setIsApplyAndClose( true );
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool RepairGUI_SuppressFacesDlg::ClickOnApply()
{
  if (!onAccept())
    return false;

  initName();
  // activate first line edit
  GroupArgs->PushButton1->click();
  return true;
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection is changed or on dialog initialization or activation
//=================================================================================
void RepairGUI_SuppressFacesDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("");
  // the second argument depends on the first one
  GroupArgs->LineEdit2->setText("");
  myFacesInd->length(0);

  if (myEditCurrentArgument == GroupArgs->LineEdit1)
    myObject = GEOM::GEOM_Object::_nil();

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  if (myEditCurrentArgument == GroupArgs->LineEdit1) {
    if (aSelList.Extent() == 1) {
      Handle(SALOME_InteractiveObject) anIO = aSelList.First();

      myObject = GEOMBase::ConvertIOinGEOMObject( anIO );
      if ( GEOMBase::IsShape(myObject) ) {
        myEditCurrentArgument->setText(GEOMBase::GetName(myObject));
        TopoDS_Shape aShape;
        if (GEOMBase::GetShape(myObject, aShape, TopAbs_FACE))
          GroupArgs->LineEdit2->setText(myEditCurrentArgument->text());

        // clear selection
        disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
        myGeomGUI->getApp()->selectionMgr()->clearSelected();
        connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
                this, SLOT(SelectionIntoArgument()));

        GroupArgs->PushButton2->click();
      }
      else
        myObject = GEOM::GEOM_Object::_nil();
    }
  }
  else if (myEditCurrentArgument == GroupArgs->LineEdit2) {
    TColStd_IndexedMapOfInteger aMap;

    if (aSelList.Extent() == 1) {
      Handle(SALOME_InteractiveObject) anIO = aSelList.First();
      aSelMgr->GetIndexes(anIO, aMap);
    }

    if (aMap.IsEmpty() && aSelList.Extent() > 0) { // try to detect selected published sub-shape
      TColStd_MapOfInteger anIds;

      SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>(myGeomGUI->getApp()->activeStudy());
      if (!appStudy) return;
      _PTR(Study) aStudy = appStudy->studyDS();

      TopTools_IndexedMapOfShape aMainMap;
      TopoDS_Shape aMainShape = GEOM_Client::get_client().GetShape(GeometryGUI::GetGeomGen(), myObject);
      TopExp::MapShapes(aMainShape, aMainMap);

      SALOME_ListIteratorOfListIO anIter (aSelList);
      for (int i = 0; anIter.More(); anIter.Next(), i++) {
        Handle(SALOME_InteractiveObject) anIO = anIter.Value();
        QString anEntry = anIO->getEntry();

        _PTR(SObject) aSObj (aStudy->FindObjectID(anEntry.toLatin1().constData()));
        GEOM::GEOM_Object_var aGeomObj =
          GEOM::GEOM_Object::_narrow(GeometryGUI::ClientSObjectToObject(aSObj));
        TopoDS_Shape aShape;
        if (GEOMBase::GetShape(aGeomObj, aShape)) {
          if (aGeomObj->GetType() == GEOM_GROUP || aShape.ShapeType() == TopAbs_FACE) {
            TopExp_Explorer anExp (aShape, TopAbs_FACE);
            for (; anExp.More(); anExp.Next()) {
              TopoDS_Shape aSubShape = anExp.Current();
              int anIndex = aMainMap.FindIndex(aSubShape);
              if (anIndex >= 0) {
                aMap.Add(anIndex);
                anIds.Add(anIndex);
              }
            }
          }
        }
      }
      if (!aMap.IsEmpty()) {
        // highlight local faces, correspondent to OB selection
        disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);

        aSelMgr->clearSelected();

        QString objIOR = GEOMBase::GetIORFromObject(myObject);
        Handle(GEOM_AISShape) aSh = GEOMBase::ConvertIORinGEOMAISShape( objIOR, true );
        if ( aSh.IsNull() )
          return;

        aSelMgr->AddOrRemoveIndex(aSh->getIO(), anIds, false);

        connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
                this, SLOT(SelectionIntoArgument()));
      }
    }

    const int n = aMap.Extent();
    myFacesInd->length(n);
    for (int i = 1; i <= n; i++)
      myFacesInd[i-1] = aMap(i);
    if (n)
      myEditCurrentArgument->setText(QString::number(n) + "_" + tr("GEOM_FACE") + tr("_S_"));
  }
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void RepairGUI_SuppressFacesDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  bool isEffective = false;

  if (send == GroupArgs->PushButton1) {
    isEffective = true;
    myEditCurrentArgument = GroupArgs->LineEdit1;

    GroupArgs->PushButton2->setDown(false);
    GroupArgs->LineEdit2->setEnabled(false);
  }
  else if (send == GroupArgs->PushButton2 && !myObject->_is_nil()) {
    isEffective = true;
    myEditCurrentArgument = GroupArgs->LineEdit2;

    GroupArgs->PushButton1->setDown(false);
    GroupArgs->LineEdit1->setEnabled(false);
  }

  if (isEffective) {
    initSelection();

    // enable line edit
    myEditCurrentArgument->setEnabled(true);
    myEditCurrentArgument->setFocus();
    // after setFocus(), because it will be setDown(false) when loses focus
    send->setDown(true);
  }
}

//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void RepairGUI_SuppressFacesDlg::LineEditReturnPressed()
{
  const QObject* send = sender();
  if (send == GroupArgs->LineEdit1 || send == GroupArgs->LineEdit2) {
    myEditCurrentArgument = (QLineEdit*)send;
    GEOMBase_Skeleton::LineEditReturnPressed();
  }
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void RepairGUI_SuppressFacesDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
           this, SLOT( SelectionIntoArgument() ) );

  myEditCurrentArgument = GroupArgs->LineEdit1;
  myEditCurrentArgument->setText( "" );
  GroupArgs->LineEdit2->setText( "" );
  myObject = GEOM::GEOM_Object::_nil();
  myFacesInd->length( 0 );

  initSelection();
}

//=================================================================================
// function : enterEvent()
// purpose  : Mouse enter onto the dialog to activate it
//=================================================================================
void RepairGUI_SuppressFacesDlg::enterEvent (QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr RepairGUI_SuppressFacesDlg::createOperation()
{
  return getGeomEngine()->GetIHealingOperations(getStudyId());
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool RepairGUI_SuppressFacesDlg::isValid (QString&)
{
  TopoDS_Shape aTmpShape;
  return !myObject->_is_nil() && (myFacesInd->length() ||
                                  GEOMBase::GetShape(myObject, aTmpShape, TopAbs_WIRE));
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool RepairGUI_SuppressFacesDlg::execute (ObjectList& objects)
{
  GEOM::GEOM_IHealingOperations_var anOper = GEOM::GEOM_IHealingOperations::_narrow(getOperation());
  GEOM::GEOM_Object_var anObj = anOper->SuppressFaces(myObject, myFacesInd);

  bool aResult = !anObj->_is_nil();
  if (aResult)
  {
    if ( !IsPreview() )
      RepairGUI::ShowStatistics( anOper, this );
    objects.push_back(anObj._retn());
  }
  return aResult;
}

//=================================================================================
// function : initSelection
// purpose  :
//=================================================================================
void RepairGUI_SuppressFacesDlg::initSelection()
{
  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);

  if (myEditCurrentArgument == GroupArgs->LineEdit1) {
    TColStd_MapOfInteger aTypes;
    aTypes.Add(GEOM_COMPOUND);
    aTypes.Add(GEOM_SOLID);
    aTypes.Add(GEOM_SHELL);
    aTypes.Add(GEOM_FACE);

    globalSelection(aTypes);
  }
  else if (myEditCurrentArgument == GroupArgs->LineEdit2) {
    localSelection(myObject, TopAbs_FACE);
  }

  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));
}

//=================================================================================
// function : restoreSubShapes
// purpose  :
//=================================================================================
void RepairGUI_SuppressFacesDlg::restoreSubShapes (SALOMEDS::Study_ptr   theStudy,
                                                   SALOMEDS::SObject_ptr theSObject)
{
  if (mainFrame()->CheckBoxRestoreSS->isChecked()) {
    // empty list of arguments means that all arguments should be restored
    getGeomEngine()->RestoreSubShapesSO(theStudy, theSObject, GEOM::ListOfGO(),
                                        /*theFindMethod=*/GEOM::FSM_GetInPlace,
                                        /*theInheritFirstArg=*/true,
                                        mainFrame()->CheckBoxAddPrefix->isChecked());
  }
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> RepairGUI_SuppressFacesDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res;
  GEOM::GeomObjPtr aGeomObjPtr(myObject);
  res << aGeomObjPtr;
  return res;
}
