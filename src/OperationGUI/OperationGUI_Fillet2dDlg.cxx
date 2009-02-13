// GEOM GEOMGUI : GUI for Geometry component
//
// Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
// File   : OperationGUI_Fillet2dDlg.cxx
// Author : DMV, OCN.
//

#include "OperationGUI_Fillet2dDlg.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <SUIT_Desktop.h>
#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_ViewWindow.h>
#include <SUIT_ViewManager.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>
#include <OCCViewer_ViewModel.h>

// OCCT Includes
#include <TColStd_MapOfInteger.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>

#include <GEOMImpl_Types.hxx>

//=================================================================================
// class    : OperationGUI_Fillet2dDlg()
// purpose  : Constructs a OperationGUI_Fillet2dDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
OperationGUI_Fillet2dDlg::OperationGUI_Fillet2dDlg (GeometryGUI* theGeometryGUI, QWidget* parent)
  : GEOMBase_Skeleton(theGeometryGUI, parent, false)
{
  SUIT_ResourceMgr* aResMgr = myGeomGUI->getApp()->resourceMgr();
  QPixmap image0 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_FILLET_2D")));
  QPixmap iconSelect (aResMgr->loadPixmap("GEOM", tr("ICON_SELECT")));

  setWindowTitle(tr("GEOM_FILLET_2D_TITLE"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_FILLET_2D"));
  mainFrame()->RadioButton1->setIcon(image0);
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->close();

  GroupVertexes = new DlgRef_2Sel1Spin(centralWidget());
  GroupVertexes->GroupBox1->setTitle(tr("GEOM_FILLET_2D"));
  GroupVertexes->TextLabel1->setText(tr("GEOM_PLANAR_FACE"));
  GroupVertexes->TextLabel2->setText(tr("GEOM_VERTEXES"));
  GroupVertexes->TextLabel3->setText(tr("GEOM_RADIUS"));
  GroupVertexes->PushButton1->setIcon(iconSelect);
  GroupVertexes->PushButton2->setIcon(iconSelect);
  GroupVertexes->LineEdit1->setReadOnly(true);
  GroupVertexes->LineEdit2->setReadOnly(true);

  QVBoxLayout* layout = new QVBoxLayout(centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
  layout->addWidget(GroupVertexes);

  /***************************************************************/

  // Set range of spinboxes
  double SpecificStep = 10.0;
  initSpinBox(GroupVertexes->SpinBox_DX, 0.00001, COORD_MAX, SpecificStep, 5); // VSR: TODO: DBL_DIGITS_DISPLAY

  setHelpFileName("fillet2d_operation_page.html");

  // Initialisation
  Init();
  resize(0,0);
}

//=================================================================================
// function : ~OperationGUI_Fillet2dDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
OperationGUI_Fillet2dDlg::~OperationGUI_Fillet2dDlg()
{
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void OperationGUI_Fillet2dDlg::Init()
{
  // Set Initial values of spinboxes
  GroupVertexes->SpinBox_DX->setValue(5);

  // Clear line edits
  GroupVertexes->LineEdit1->setText("");
  GroupVertexes->LineEdit2->setText("");

  myShape = GEOM::GEOM_Object::_nil();

  myVertexes.Clear();

  // signals and slots connections

  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()   ));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  //  connect(this,          SIGNAL(constructorsClicked(int)), this, SLOT(ConstructorsClicked(int)));

  connect(GroupVertexes->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupVertexes->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(GroupVertexes->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupVertexes->LineEdit2, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(GroupVertexes->SpinBox_DX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));

  initName(tr("GEOM_FILLET_2D"));
  GroupVertexes->PushButton1->click();
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void OperationGUI_Fillet2dDlg::ClickOnOk()
{
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool OperationGUI_Fillet2dDlg::ClickOnApply()
{
  if (!onAccept())
    return false;

  initName();
  // Reset dialog state
  GroupVertexes->LineEdit1->setText("");
  GroupVertexes->LineEdit2->setText("");
  myShape = GEOM::GEOM_Object::_nil();
  myVertexes.Clear();
  GroupVertexes->PushButton1->click();

  return true;
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection is changed or on dialog initialization or activation
//=================================================================================
void OperationGUI_Fillet2dDlg::SelectionIntoArgument()
{
  erasePreview();
  myEditCurrentArgument->setText("");

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  // If selection of main object is activated
  if (myEditCurrentArgument == GroupVertexes->LineEdit1) {
    myShape = GEOM::GEOM_Object::_nil();
    if (aSelList.Extent() == 1) {
      Standard_Boolean aResult = Standard_False;
      GEOM::GEOM_Object_var anObj =
        GEOMBase::ConvertIOinGEOMObject(aSelList.First(), aResult);
      
      if (aResult && !anObj->_is_nil()) {
	QString aName = GEOMBase::GetName( anObj );
	TopoDS_Shape aShape;
	if ( GEOMBase::GetShape( anObj, aShape, TopAbs_SHAPE ) && !aShape.IsNull() ) {
	  TColStd_IndexedMapOfInteger aMap;
	  aSelMgr->GetIndexes(aSelList.First(), aMap);
	  if ( aMap.Extent() == 1 ) { // Local Selection
	    int anIndex = aMap( 1 );
	    aName += QString( ":face_%1" ).arg( anIndex );
   
	    //Find SubShape Object in Father
	    GEOM::GEOM_Object_var aFindedObject = GEOMBase_Helper::findObjectInFather( anObj, aName );
	    
	    if ( aFindedObject == GEOM::GEOM_Object::_nil() ) { // Object not found in study
	      GEOM::GEOM_IShapesOperations_var aShapesOp = getGeomEngine()->GetIShapesOperations( getStudyId() );
	      anObj = aShapesOp->GetSubShape( anObj, anIndex );
	    }
	    else
	      anObj = aFindedObject; // get Object from study
	  }
	  else { // Global Selection
	    if ( aShape.ShapeType() != TopAbs_FACE ) {
	      anObj = GEOM::GEOM_Object::_nil();
	      aName = "";
	    }
	  }
	}
	myShape = anObj;
	myEditCurrentArgument->setText(aName
);
	displayPreview();
      }
    }
  } else if (myEditCurrentArgument == GroupVertexes->LineEdit2) {
    myVertexes.Clear();
    if (aSelList.Extent() == 1) {
      Standard_Boolean aResult = Standard_False;
      GEOM::GEOM_Object_var anObj =
        GEOMBase::ConvertIOinGEOMObject(aSelList.First(), aResult);

      if (aResult && !anObj->_is_nil()) {
        TColStd_IndexedMapOfInteger anIndexes;
        aSelMgr->GetIndexes(aSelList.First(), anIndexes);

        if (anIndexes.Extent() > 0) {
          QString aName;
          if (anIndexes.Extent() == 1) {
            int anIndex = anIndexes(1);
            aName = QString(GEOMBase::GetName(anObj)) + QString(":vertex_%1").arg(anIndex);
          }
          else
            aName = tr("GEOM_MEN_POPUP_NAME").arg(anIndexes.Extent());

          myEditCurrentArgument->setText(aName);
	  myVertexes = anIndexes;
          displayPreview();
        }
      }
    }
  }

  if (myEditCurrentArgument == GroupVertexes->LineEdit1) {
    if (!myShape->_is_nil() && myVertexes.Extent() == 0)
      GroupVertexes->PushButton2->click();
  }
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void OperationGUI_Fillet2dDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  if (send == GroupVertexes->PushButton1) {
    myEditCurrentArgument = GroupVertexes->LineEdit1;
    GroupVertexes->PushButton2->setDown(false);
    GroupVertexes->LineEdit2->setEnabled(false);
  }
  else if (send == GroupVertexes->PushButton2) {
    myEditCurrentArgument = GroupVertexes->LineEdit2;
    GroupVertexes->PushButton1->setDown(false);
    GroupVertexes->LineEdit1->setEnabled(false);
  }

  // enable line edit
  myEditCurrentArgument->setEnabled(true);
  myEditCurrentArgument->setFocus();
  // after setFocus(), because it will be setDown(false) when loses focus
  send->setDown(true);

  activateSelection();

  // seems we need it only to avoid preview disappearing, caused by selection mode change
  displayPreview();
}

//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void OperationGUI_Fillet2dDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();

  if (send == GroupVertexes->LineEdit1)
    myEditCurrentArgument = GroupVertexes->LineEdit1;
  else if (send == GroupVertexes->LineEdit2)
    myEditCurrentArgument = GroupVertexes->LineEdit2;
  else
    return;

  GEOMBase_Skeleton::LineEditReturnPressed();
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void OperationGUI_Fillet2dDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void OperationGUI_Fillet2dDlg::enterEvent (QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    this->ActivateThisDialog();
}

//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void OperationGUI_Fillet2dDlg::ValueChangedInSpinBox (double)
{
  displayPreview();
}

//=================================================================================
// function : activateSelection
// purpose  : Activate selection in accordance with myEditCurrentArgument
//=================================================================================
void OperationGUI_Fillet2dDlg::activateSelection()
{
  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
  globalSelection();
  if (myEditCurrentArgument == GroupVertexes->LineEdit1)
    globalSelection( GEOM_FACE );  //    localSelection(myShape, TopAbs_FACE);
  else if (!myShape->_is_nil() && myEditCurrentArgument == GroupVertexes->LineEdit2)
    localSelection(myShape, TopAbs_VERTEX);

  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr OperationGUI_Fillet2dDlg::createOperation()
{
  return getGeomEngine()->GetILocalOperations(getStudyId());
}

//=================================================================================
// function : isValid()
// purpose  : Verify validity of input data
//=================================================================================
bool OperationGUI_Fillet2dDlg::isValid (QString&)
{
  return !myShape->_is_nil() && myVertexes.Extent() > 0;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool OperationGUI_Fillet2dDlg::execute (ObjectList& objects)
{
  GEOM::GEOM_Object_var anObj;

  GEOM::ListOfLong_var aListOfIndexes = new GEOM::ListOfLong;
  aListOfIndexes->length(myVertexes.Extent());

  for (int i = 1, n = myVertexes.Extent(); i <= n; i++)
    aListOfIndexes[ i - 1 ] = myVertexes(i);

  anObj = GEOM::GEOM_ILocalOperations::_narrow(getOperation())->
          MakeFillet2D(myShape, getRadius(), aListOfIndexes);

  if (!anObj->_is_nil())
    objects.push_back(anObj._retn());

  return true;
}

//=================================================================================
// function : getRadius
// purpose  : Get radius
//=================================================================================
double OperationGUI_Fillet2dDlg::getRadius() const
{
  return GroupVertexes ->SpinBox_DX->value();
}
