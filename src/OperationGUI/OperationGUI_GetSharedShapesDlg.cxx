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
//  File   : OperationGUI_GetSharedShapesDlg.cxx
//  Author : Lucien PIGNOLONI, Open CASCADE S.A.S.

#include "OperationGUI_GetSharedShapesDlg.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <GEOMImpl_Types.hxx>

#include <SUIT_Desktop.h>
#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_MessageBox.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>

#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>

namespace
{
  GEOM::shape_type maxShapeType(const GEOM::ListOfGO& objs)
  {
    GEOM::shape_type r = GEOM::SHAPE;
    for ( int i = 0; i < objs.length(); i++ ) {
      GEOM::shape_type t = objs[i]->GetShapeType();
      if ( t == GEOM::COMPOUND || t == GEOM::COMPSOLID )
        t = objs[i]->GetMaxShapeType();
      else if ( t < GEOM::VERTEX )
        t = GEOM::shape_type( (int)t+1 );
      r = qMin( r, t );
      if ( r <= GEOM::SOLID ) break;
    }
    return r;
  }
}

//=================================================================================
// class    : OperationGUI_GetSharedShapesDlg()
// purpose  : Constructs a OperationGUI_GetSharedShapesDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
OperationGUI_GetSharedShapesDlg::OperationGUI_GetSharedShapesDlg
(GeometryGUI* theGeometryGUI, QWidget* parent)
  : GEOMBase_Skeleton(theGeometryGUI, parent, false)
{
  SUIT_ResourceMgr* aResMgr = myGeomGUI->getApp()->resourceMgr();
  QPixmap image0(aResMgr->loadPixmap("GEOM", tr("ICON_DLG_SHARED_SHAPES")));
  QPixmap image2(aResMgr->loadPixmap("GEOM", tr("ICON_SELECT")));

  setWindowTitle(tr("GEOM_SHARED_SHAPES_TITLE"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_GET_SHARED_SHAPES"));
  mainFrame()->RadioButton1->setIcon(image0);
  mainFrame()->RadioButton2->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton3->close();

  GroupPoints = new DlgRef_2Sel1List1Check(centralWidget());
  GroupPoints->GroupBox1->setTitle(tr("GEOM_SHARED_SHAPES_INPUT"));
  GroupPoints->TextLabel1->setText(tr("GEOM_SHAPES"));
  GroupPoints->TextLabel2->hide();
  GroupPoints->TextLabel3->setText(tr("GEOM_SUBSHAPE_TYPE"));
  GroupPoints->PushButton1->setIcon(image2);
  GroupPoints->PushButton2->hide();
  GroupPoints->LineEdit1->setReadOnly(true);
  GroupPoints->LineEdit2->hide();
  GroupPoints->LineEdit1->setEnabled(true);
  GroupPoints->CheckButton1->setText(tr("GEOM_SHARED_SHAPES_MULTISHARE"));

  QVBoxLayout* layout = new QVBoxLayout(centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
  layout->addWidget(GroupPoints);

  /***************************************************************/

  setHelpFileName("shared_shapes_page.html");

  Init();
}

//=================================================================================
// function : ~OperationGUI_GetSharedShapesDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
OperationGUI_GetSharedShapesDlg::~OperationGUI_GetSharedShapesDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void OperationGUI_GetSharedShapesDlg::Init()
{
  initTypes();

  showOnlyPreviewControl();

  /* signals and slots connections */
  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(this, SIGNAL(constructorsClicked(int)), this, SLOT(ConstructorsClicked(int)));

  connect(GroupPoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(GroupPoints->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(GroupPoints->ComboBox1, SIGNAL(activated(int)), this, SLOT(ComboTextChanged()));

  connect(GroupPoints->CheckButton1, SIGNAL(clicked()), this, SLOT(processPreview()));

  connect(myGeomGUI->getApp()->selectionMgr(),
           SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  //initName(getNewObjectName());
  initName(getPrefixByType());

  ConstructorsClicked(0);
  GroupPoints->PushButton1->click();
}

void OperationGUI_GetSharedShapesDlg::initTypes()
{
  GEOM::shape_type t = maxShapeType( myListShapes );

  int idx = GroupPoints->ComboBox1->count() > 0 ? 
    GroupPoints->ComboBox1->itemData( GroupPoints->ComboBox1->currentIndex() ).toInt() : -1;

  GroupPoints->ComboBox1->clear();

  if ( t == GEOM::SHAPE || t <= GEOM::SOLID )
    GroupPoints->ComboBox1->addItem( tr( "GEOM_SOLID" ), int( GEOM::SOLID ) );
  if ( t == GEOM::SHAPE || t <= GEOM::SHELL )
    GroupPoints->ComboBox1->addItem( tr( "GEOM_SHELL" ), int( GEOM::SHELL ) );
  if ( t == GEOM::SHAPE || t <= GEOM::FACE )
    GroupPoints->ComboBox1->addItem( tr( "GEOM_FACE" ), int( GEOM::FACE ) );
  if ( t == GEOM::SHAPE || t <= GEOM::WIRE )
    GroupPoints->ComboBox1->addItem( tr( "GEOM_WIRE" ), int( GEOM::WIRE ) );
  if ( t == GEOM::SHAPE || t <= GEOM::EDGE )
    GroupPoints->ComboBox1->addItem( tr( "GEOM_EDGE" ), int( GEOM::EDGE ) );
  if ( t == GEOM::SHAPE || t <= GEOM::VERTEX )
    GroupPoints->ComboBox1->addItem( tr( "GEOM_VERTEX" ), int( GEOM::VERTEX ) );

  idx = GroupPoints->ComboBox1->findData( idx );
  GroupPoints->ComboBox1->setCurrentIndex( idx >= 0 ? idx : 0 );

  if ( idx < 0 )
    ComboTextChanged();
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void OperationGUI_GetSharedShapesDlg::ConstructorsClicked (int constructorId)
{
  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
  globalSelection();

  myListShapes.length(0);

  GroupPoints->PushButton1->setDown(true);

  myEditCurrentArgument = GroupPoints->LineEdit1;
  GroupPoints->LineEdit1->clear();

  qApp->processEvents();
  updateGeometry();
  resize(minimumSizeHint());

  myEditCurrentArgument->setFocus();
  connect(myGeomGUI->getApp()->selectionMgr(),
          SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
  SelectionIntoArgument();
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void OperationGUI_GetSharedShapesDlg::ClickOnOk()
{
  setIsApplyAndClose( true );
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool OperationGUI_GetSharedShapesDlg::ClickOnApply()
{
  if (!onAccept())
    return false;

  //initName(getNewObjectName());
  initName(getPrefixByType());

  return true;
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void OperationGUI_GetSharedShapesDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("");
  QString aString = "";

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  int nbSel = GEOMBase::GetNameOfSelectedIObjects(aSelList, aString, true);

  if (nbSel < 1) {
    myListShapes.length(0);
  }

  GEOMBase::ConvertListOfIOInListOfGO(aSelList, myListShapes, true);

  initTypes();

  if (!myListShapes.length())
    return;

  myEditCurrentArgument->setText(aString);
  processPreview();
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void OperationGUI_GetSharedShapesDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  if (send == GroupPoints->PushButton1) {
    myEditCurrentArgument = GroupPoints->LineEdit1;
    GroupPoints->LineEdit1->setEnabled(true);
  }

  globalSelection(GEOM_ALLSHAPES);

  myEditCurrentArgument->setFocus();
  SelectionIntoArgument();
  send->setDown(true);
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void OperationGUI_GetSharedShapesDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(myGeomGUI->getApp()->selectionMgr(),
          SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  ConstructorsClicked(getConstructorId());
  processPreview();
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void OperationGUI_GetSharedShapesDlg::enterEvent(QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    this->ActivateThisDialog();
}

//=======================================================================
//function : ComboTextChanged
//purpose  :
//=======================================================================
void OperationGUI_GetSharedShapesDlg::ComboTextChanged()
{
  //initName(getNewObjectName());
  initName(getPrefixByType());
  processPreview();
}

//=================================================================================
// function : GetType()
// purpose  :
//=================================================================================
int OperationGUI_GetSharedShapesDlg::GetType() const
{
  return GroupPoints->ComboBox1->count() > 0 ?
    GroupPoints->ComboBox1->itemData( GroupPoints->ComboBox1->currentIndex() ).toInt() : (int)GEOM::SHAPE;
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr OperationGUI_GetSharedShapesDlg::createOperation()
{
  return getGeomEngine()->GetIShapesOperations(getStudyId());
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool OperationGUI_GetSharedShapesDlg::isValid (QString& msg)
{
  bool isOK = myListShapes.length() > 1 ||
    ( myListShapes.length() > 0 && myListShapes[0]->GetShapeType() == GEOM::COMPOUND );

  if ( !isOK )
    msg = tr("MSG_SHARED_SHAPES_TOO_FEW_SHAPES");

  return isOK;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool OperationGUI_GetSharedShapesDlg::execute (ObjectList& objects)
{
  GEOM::GEOM_IShapesOperations_var anOper = GEOM::GEOM_IShapesOperations::_narrow(getOperation());
  GEOM::ListOfGO_var aList = anOper->GetSharedShapesMulti(myListShapes, GetType(), GroupPoints->CheckButton1->isChecked());

  if (!aList->length())
    return false;

  if (aList->length() > 1)
    myMainFrame->ResultName->setText(getPrefixByType());

  for (int i = 0, n = aList->length(); i < n; i++)
    objects.push_back(GEOM::GEOM_Object::_duplicate(aList[i]));

  return objects.size();
}

//================================================================
// Function : getFather
// Purpose  : Get father object for object to be added in study
//            ( called with addInStudy method )
//================================================================
GEOM::GEOM_Object_ptr OperationGUI_GetSharedShapesDlg::getFather (GEOM::GEOM_Object_ptr)
{
  if (myListShapes.length() > 0)
    //return myListShapes[0]._retn();
    return myListShapes[0];
  return NULL;
}

//================================================================
// Function : getPrefixByType
// Purpose  :
//================================================================
QString OperationGUI_GetSharedShapesDlg::getPrefixByType () const
{
  QString aPref;

  switch ( GetType() ) {
  case GEOM::SOLID:  aPref = tr("GEOM_SHARED_SHAPE").arg(tr("GEOM_SOLID")) ; break;
  case GEOM::SHELL:  aPref = tr("GEOM_SHARED_SHAPE").arg(tr("GEOM_SHELL")) ; break;
  case GEOM::FACE:   aPref = tr("GEOM_SHARED_SHAPE").arg(tr("GEOM_FACE"))  ; break;
  case GEOM::WIRE:   aPref = tr("GEOM_SHARED_SHAPE").arg(tr("GEOM_WIRE"))  ; break;
  case GEOM::EDGE:   aPref = tr("GEOM_SHARED_SHAPE").arg(tr("GEOM_EDGE"))  ; break;
  case GEOM::VERTEX: aPref = tr("GEOM_SHARED_SHAPE").arg(tr("GEOM_VERTEX")); break;
  default:           aPref = tr("GEOM_SHARED_SHAPE").arg(tr("GEOM_SHAPE")) ; break;
  }

  return aPref;
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> OperationGUI_GetSharedShapesDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res;
  GEOM::ListOfGO aListPtr(myListShapes);
  for (int i = 0; i < aListPtr.length(); i++) {
    GEOM::GeomObjPtr aGeomObjPtr(aListPtr[i]);
    res << aGeomObjPtr;
  }
  return res;
}
