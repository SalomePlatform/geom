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

//  GEOM RepairGUI : GUI for Geometry component
//  File   : RepairGUI_RemoveWebsDlg.cxx

#include "RepairGUI_RemoveWebsDlg.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>
#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>

#include <GEOMImpl_Types.hxx>

#include <TColStd_MapOfInteger.hxx>

//=================================================================================
// class    : RepairGUI_RemoveWebsDlg()
// purpose  : Constructs a RepairGUI_RemoveWebsDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
RepairGUI_RemoveWebsDlg::RepairGUI_RemoveWebsDlg (GeometryGUI* theGeometryGUI, QWidget* parent,
                                                  bool modal)
  : GEOMBase_Skeleton(theGeometryGUI, parent, modal)
{
  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
  QPixmap image0 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_REMOVE_WEBS")));
  QPixmap image1 (aResMgr->loadPixmap("GEOM", tr("ICON_SELECT")));

  setWindowTitle(tr("GEOM_REMOVE_WEBS_TITLE"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_REMOVE_WEBS_TITLE"));
  mainFrame()->RadioButton1->setIcon( image0 );
  mainFrame()->RadioButton2->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton3->close();

  GroupPoints = new DlgRef_1Sel( centralWidget() );

  GroupPoints->GroupBox1->setTitle( tr( "GEOM_REMOVE_WEBS" ) );
  GroupPoints->TextLabel1->setText( tr( "GEOM_SELECTED_SHAPES" ) );
  GroupPoints->PushButton1->setIcon( image1 );
  GroupPoints->LineEdit1->setReadOnly( true );

  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );
  layout->setMargin( 0 ); layout->setSpacing( 6 );
  layout->addWidget( GroupPoints );

  /***************************************************************/

  setHelpFileName( "remove_webs_operation_page.html" );

  Init();
}

//=================================================================================
// function : ~RepairGUI_RemoveWebsDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
RepairGUI_RemoveWebsDlg::~RepairGUI_RemoveWebsDlg()
{
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void RepairGUI_RemoveWebsDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupPoints->LineEdit1;

  myOkObject = false;

  activateSelection();

  mainFrame()->GroupBoxPublish->show();
  //Hide preview checkbox
  mainFrame()->CheckBoxPreview->hide();

  /* signals and slots connections */
  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(GroupPoints->PushButton1, SIGNAL(clicked()),       this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->LineEdit1,   SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));

  initName(tr("REMOVE_WEBS_NEW_OBJ_NAME"));
  resize(100,100);
  SelectionIntoArgument();
}

//=================================================================================
// function : ClickOnOk()
// purpose  : Same than click on apply but close this dialog.
//=================================================================================
void RepairGUI_RemoveWebsDlg::ClickOnOk()
{
  setIsApplyAndClose(true);
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool RepairGUI_RemoveWebsDlg::ClickOnApply()
{
  if (!onAccept())
    return false;

  initName();

  myEditCurrentArgument->setText("");
  myObjects.clear();

  myOkObject = false;

  activateSelection();

  return true;
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//          : used only by SelectButtonC1A1 (LineEditC1A1)
//=================================================================================
void RepairGUI_RemoveWebsDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText( "" );
  myObjects.clear();
  myOkObject = false;

  myObjects = getSelected( TopAbs_SHAPE, -1 );

  if ( !myObjects.isEmpty() ) {
    QString aName = myObjects.count() > 1 ? QString( "%1_objects").arg( myObjects.count() ) : GEOMBase::GetName( myObjects[0].get() );
    myOkObject = true;
    myEditCurrentArgument->setText( aName );
  }
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void RepairGUI_RemoveWebsDlg::SetEditCurrentArgument()
{
  if (sender() == GroupPoints->PushButton1) {
    GroupPoints->LineEdit1->setFocus();
    myEditCurrentArgument = GroupPoints->LineEdit1;
  }
  SelectionIntoArgument();
}

//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void RepairGUI_RemoveWebsDlg::LineEditReturnPressed()
{
  if (sender() == GroupPoints->LineEdit1) {
    myEditCurrentArgument = GroupPoints->LineEdit1;
    GEOMBase_Skeleton::LineEditReturnPressed();
  }
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void RepairGUI_RemoveWebsDlg::ActivateThisDialog()
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
void RepairGUI_RemoveWebsDlg::enterEvent (QEvent* e)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : activateSelection
// purpose  : activate selection of solids and compounds
//=================================================================================
void RepairGUI_RemoveWebsDlg::activateSelection()
{
  TColStd_MapOfInteger aTypes;
  aTypes.Add( GEOM_SOLID );
  aTypes.Add( GEOM_COMPOUND );
  globalSelection( aTypes );
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr RepairGUI_RemoveWebsDlg::createOperation()
{
  return getGeomEngine()->GetIHealingOperations(getStudyId());
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool RepairGUI_RemoveWebsDlg::isValid (QString& msg)
{
  return myOkObject;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool RepairGUI_RemoveWebsDlg::execute (ObjectList& objects)
{
  GEOM::ListOfGO_var objList = new GEOM::ListOfGO;
  objList->length( myObjects.count() );
  for ( int i = 0; i < myObjects.count(); ++i )
    objList[i] = myObjects[i].copy();

  GEOM::GEOM_IHealingOperations_var anOper = GEOM::GEOM_IHealingOperations::_narrow(getOperation());
  GEOM::GEOM_Object_var anObj = anOper->RemoveInternalFaces(objList);

  if (!anObj->_is_nil())
    objects.push_back(anObj._retn());

  return true;
}

//=================================================================================
// function : restoreSubShapes
// purpose  :
//=================================================================================
void RepairGUI_RemoveWebsDlg::restoreSubShapes (SALOMEDS::Study_ptr   theStudy,
                                                SALOMEDS::SObject_ptr theSObject)
{
  if (mainFrame()->CheckBoxRestoreSS->isChecked()) {
    // empty list of arguments means that all arguments should be restored
    getGeomEngine()->RestoreSubShapesSO(theStudy, theSObject, GEOM::ListOfGO(),
                                        /*theFindMethod=*/GEOM::FSM_GetInPlace, // ? GetInPlaceByHistory
                                        /*theInheritFirstArg=*/true,
                                        mainFrame()->CheckBoxAddPrefix->isChecked());
  }
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> RepairGUI_RemoveWebsDlg::getSourceObjects()
{
  return myObjects;
}
