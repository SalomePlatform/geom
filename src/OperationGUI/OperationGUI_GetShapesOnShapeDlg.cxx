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

// GEOM GEOMGUI : GUI for Geometry component
// File   : OperationGUI_GetShapesOnShapeDlg.cxx
// Author : Sergey KUUL, Open CASCADE S.A.S. (sergey.kuul@opencascade.com)
//
#include "OperationGUI_GetShapesOnShapeDlg.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>

//==============================================================================
// class    : OperationGUI_GetShapesOnShapeDlg()
// purpose  : Constructs a OperationGUI_GetShapesOnShapeDlg which is a child of
//            'parent', with the name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
OperationGUI_GetShapesOnShapeDlg::OperationGUI_GetShapesOnShapeDlg (GeometryGUI* theGeometryGUI,
                                                                    QWidget* parent)
  : GEOMBase_Skeleton(theGeometryGUI, parent, false)
{
  SUIT_ResourceMgr* aResMgr = myGeomGUI->getApp()->resourceMgr();
  QPixmap image0 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_SHAPES_ON_SHAPE")));
  QPixmap image2 (aResMgr->loadPixmap("GEOM", tr("ICON_SELECT")));

  setWindowTitle(tr("GEOM_SHAPES_ON_SHAPE_TITLE"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_SHAPES_ON_SHAPE"));
  mainFrame()->RadioButton1->setIcon(image0);
  mainFrame()->RadioButton2->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton3->close();

  GroupPoints = new DlgRef_2Sel2List(centralWidget());
  //GroupPoints->GroupBox1->setTitle(tr("GEOM_PARTITION"));
  GroupPoints->GroupBox1->setTitle("Input data");
  GroupPoints->TextLabel1->setText(tr("GEOM_SHAPES_ON_SHAPE_ESHAPE"));
  GroupPoints->TextLabel2->setText(tr("GEOM_SHAPES_ON_SHAPE_CSHAPE"));
  GroupPoints->TextLabel3->setText(tr("GEOM_RECONSTRUCTION_LIMIT"));
  GroupPoints->TextLabel4->setText(tr("GEOM_SHAPES_ON_SHAPE_STATE"));
  GroupPoints->PushButton1->setIcon(image2);
  GroupPoints->PushButton2->setIcon(image2);
  GroupPoints->LineEdit1->setReadOnly(true);
  GroupPoints->LineEdit2->setReadOnly(true);

  QVBoxLayout* layout = new QVBoxLayout(centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
  layout->addWidget(GroupPoints);
  /***************************************************************/

  setHelpFileName("shapesonshape_page.html");

  Init();
}

//=================================================================================
// function : ~OperationGUI_GetShapesOnShapeDlg
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
OperationGUI_GetShapesOnShapeDlg::~OperationGUI_GetShapesOnShapeDlg()
{
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void OperationGUI_GetShapesOnShapeDlg::Init()
{
  // type for sub-shape selection
  GroupPoints->ComboBox1->addItem(tr("GEOM_RECONSTRUCTION_LIMIT_SOLID"));
  // commented by skl for IPAL19949 - finder isn't allowed such type
  //GroupPoints->ComboBox1->addItem(tr("GEOM_RECONSTRUCTION_LIMIT_SHELL"));
  GroupPoints->ComboBox1->addItem(tr("GEOM_RECONSTRUCTION_LIMIT_FACE"));
  // commented by skl for IPAL19949 - finder isn't allowed such type
  //GroupPoints->ComboBox1->addItem(tr("GEOM_RECONSTRUCTION_LIMIT_WIRE"));
  GroupPoints->ComboBox1->addItem(tr("GEOM_RECONSTRUCTION_LIMIT_EDGE"));
  GroupPoints->ComboBox1->addItem(tr("GEOM_RECONSTRUCTION_LIMIT_VERTEX"));

  // type for state selection
  GroupPoints->ComboBox2->addItem(tr("GEOM_STATE_IN"));
  GroupPoints->ComboBox2->addItem(tr("GEOM_STATE_OUT"));
  GroupPoints->ComboBox2->addItem(tr("GEOM_STATE_ON"));
  GroupPoints->ComboBox2->addItem(tr("GEOM_STATE_ONIN"));
  GroupPoints->ComboBox2->addItem(tr("GEOM_STATE_ONOUT"));

  GroupPoints->LineEdit1->clear();
  GroupPoints->LineEdit2->clear();
  myObject1 = myObject2 = GEOM::GEOM_Object::_nil();

  showOnlyPreviewControl();

  // signals and slots connections
  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(GroupPoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(GroupPoints->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupPoints->LineEdit2, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(GroupPoints->ComboBox1, SIGNAL(activated(int)), this, SLOT(ComboTextChanged()));
  connect(GroupPoints->ComboBox2, SIGNAL(activated(int)), this, SLOT(ComboTextChanged()));

  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));

  initName(mainFrame()->GroupConstructors->title());

  GroupPoints->TextLabel3->show();
  GroupPoints->ComboBox1->show();
  GroupPoints->ComboBox1->setCurrentIndex(0);
  GroupPoints->ComboBox2->show();
  GroupPoints->ComboBox2->setCurrentIndex(0);

  globalSelection(GEOM_ALLSHAPES);

  GroupPoints->PushButton1->click();
  SelectionIntoArgument();
  resize(100,100);
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void OperationGUI_GetShapesOnShapeDlg::ClickOnOk()
{
  setIsApplyAndClose( true );
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool OperationGUI_GetShapesOnShapeDlg::ClickOnApply()
{
  if (!onAccept())
    return false;

  initName();
  // activate first line edit
  GroupPoints->PushButton1->click();
  return true;
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection is changed or on dialog initialization or activation
//=================================================================================
void OperationGUI_GetShapesOnShapeDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("");

  if      (myEditCurrentArgument == GroupPoints->LineEdit1) myObject1 = GEOM::GEOM_Object::_nil();
  else if (myEditCurrentArgument == GroupPoints->LineEdit2) myObject2 = GEOM::GEOM_Object::_nil();

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  QString aName = "";
  int nbSel = GEOMBase::GetNameOfSelectedIObjects(aSelList, aName, true);

  if (nbSel > 0) {
    GEOM::GEOM_Object_var aSelectedObject = GEOMBase::ConvertIOinGEOMObject( aSelList.First() );
    if ( GEOMBase::IsShape(aSelectedObject) ) {
      myEditCurrentArgument->setText(aName);

      // clear selection
      disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
      myGeomGUI->getApp()->selectionMgr()->clearSelected();
      connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
              this, SLOT(SelectionIntoArgument()));

      if (myEditCurrentArgument == GroupPoints->LineEdit1) {
        myObject1 = aSelectedObject;
        if (myObject2->_is_nil())
          GroupPoints->PushButton2->click();
      }
      else if (myEditCurrentArgument == GroupPoints->LineEdit2) {
        myObject2 = aSelectedObject;
        if (myObject1->_is_nil())
          GroupPoints->PushButton1->click();
      }
    }
  }
  processPreview();
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void OperationGUI_GetShapesOnShapeDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  if      (send == GroupPoints->PushButton1) {
    myEditCurrentArgument = GroupPoints->LineEdit1;

    GroupPoints->PushButton2->setDown(false);
    GroupPoints->LineEdit2->setEnabled(false);
  }
  else if (send == GroupPoints->PushButton2) {
    myEditCurrentArgument = GroupPoints->LineEdit2;

    GroupPoints->PushButton1->setDown(false);
    GroupPoints->LineEdit1->setEnabled(false);
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
void OperationGUI_GetShapesOnShapeDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if (send == GroupPoints->LineEdit1 || send == GroupPoints->LineEdit2)
  {
    myEditCurrentArgument = send;
    GEOMBase_Skeleton::LineEditReturnPressed();
  }
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void OperationGUI_GetShapesOnShapeDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  globalSelection(GEOM_ALLSHAPES);
  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
           this, SLOT( SelectionIntoArgument() ) );
  processPreview();
}

//=================================================================================
// function : enterEvent()
// purpose  : when mouse enter onto the QWidget
//=================================================================================
void OperationGUI_GetShapesOnShapeDlg::enterEvent (QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr OperationGUI_GetShapesOnShapeDlg::createOperation()
{
  return getGeomEngine()->GetIShapesOperations(getStudyId());
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool OperationGUI_GetShapesOnShapeDlg::isValid(QString&)
{
  //Handle(SALOME_InteractiveObject) IO = firstIObject();
  //GEOM::GEOM_Object_var anObject = GEOMBase::ConvertIOinGEOMObject( IO );
  //if ( anObject->_is_nil() )
  //  return false;

  return !CORBA::is_nil(myObject1) && !CORBA::is_nil(myObject2);
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool OperationGUI_GetShapesOnShapeDlg::execute (ObjectList& objects)
{
  int aLimit = GroupPoints->ComboBox1->currentIndex();
  switch (aLimit)
  {
  case 0:  aLimit = GEOM::SOLID ; break;
  case 1:  aLimit = GEOM::FACE  ; break;
  case 2:  aLimit = GEOM::EDGE  ; break;
  case 3:  aLimit = GEOM::VERTEX; break;
  default: aLimit = GEOM::SHAPE ; break;
  }

  GEOM::shape_state aState;
  switch (GroupPoints->ComboBox2->currentIndex())
  {
    case 0:  aState = GEOM::ST_IN;    break;
    case 1:  aState = GEOM::ST_OUT;   break;
    case 2:  aState = GEOM::ST_ON;    break;
    case 3:  aState = GEOM::ST_ONIN;  break;
    case 4:  aState = GEOM::ST_ONOUT; break;
    default: break;
  }

  GEOM::GEOM_IShapesOperations_var anOper = GEOM::GEOM_IShapesOperations::_narrow(getOperation());
  GEOM::GEOM_Object_var anObj = anOper->GetShapesOnShapeAsCompound(myObject2, myObject1,
                                                                   (CORBA::Short) aLimit,
                                                                   aState);

  if (!anObj->_is_nil())
    objects.push_back(anObj._retn());

  return objects.size();
}

//=======================================================================
//function : ComboTextChanged
//purpose  :
//=======================================================================
void OperationGUI_GetShapesOnShapeDlg::ComboTextChanged()
{
  // VRS ???? What is it for ??? commented for a while...
  //bool IsEnabled = GroupPoints->ComboBox1->currentIndex() < 3;
  processPreview();
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> OperationGUI_GetShapesOnShapeDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res;
  GEOM::GeomObjPtr aGeomObjPtr1(myObject1), aGeomObjPtr2(myObject2);
  res << aGeomObjPtr1 << aGeomObjPtr2;
  return res;
}
