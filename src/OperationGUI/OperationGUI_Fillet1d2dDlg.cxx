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
// File   : OperationGUI_Fillet1d2dDlg.cxx
// Author : DMV, OCN.

#include "OperationGUI_Fillet1d2dDlg.h"

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
// class    : OperationGUI_Fillet1d2dDlg()
// purpose  : Constructs a OperationGUI_Fillet1d2dDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
OperationGUI_Fillet1d2dDlg::OperationGUI_Fillet1d2dDlg (GeometryGUI* theGeometryGUI,
                                                        QWidget* parent,
                                                        bool is1D)
  : GEOMBase_Skeleton(theGeometryGUI, parent, false),
    myIs1D( is1D )
{
  SUIT_ResourceMgr* aResMgr = myGeomGUI->getApp()->resourceMgr();
  QPixmap image0 (aResMgr->loadPixmap("GEOM", myIs1D ? tr("ICON_DLG_FILLET_1D") 
                                                     : tr("ICON_DLG_FILLET_2D")));
  QPixmap iconSelect (aResMgr->loadPixmap("GEOM", tr("ICON_SELECT")));

  setWindowTitle(myIs1D ? tr("GEOM_FILLET_1D_TITLE") : tr("GEOM_FILLET_2D_TITLE"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(myIs1D ? tr("GEOM_FILLET_1D") : tr("GEOM_FILLET_2D"));
  mainFrame()->RadioButton1->setIcon(image0);
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->close();

  GroupVertexes = new DlgRef_2Sel1Spin2Check (centralWidget());
  GroupVertexes->GroupBox1->setTitle(myIs1D ? tr("GEOM_FILLET_1D") : tr("GEOM_FILLET_2D"));
  GroupVertexes->TextLabel1->setText(myIs1D ? tr("GEOM_PLANAR_EDGE_WIRE") : tr("GEOM_PLANAR_FACE"));
  GroupVertexes->TextLabel2->setText(tr("GEOM_VERTEXES"));
  GroupVertexes->TextLabel3->setText(tr("GEOM_RADIUS"));
  GroupVertexes->PushButton1->setIcon(iconSelect);
  GroupVertexes->PushButton2->setIcon(iconSelect);
  GroupVertexes->LineEdit1->setReadOnly(true);
  GroupVertexes->LineEdit2->setReadOnly(true);
  if (myIs1D) {
    GroupVertexes->CheckButton1->setText(tr("GEOM_FILLET_1D_IGNORE_SECANT"));
    GroupVertexes->CheckButton1->setChecked(true);
  }
  else
    GroupVertexes->CheckButton1->close();
  GroupVertexes->CheckButton2->close();

  QVBoxLayout* layout = new QVBoxLayout(centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
  layout->addWidget(GroupVertexes);

  /***************************************************************/

  // Set range of spinboxes
  double SpecificStep = 10.0;
  initSpinBox(GroupVertexes->SpinBox_DX, 0.00001, COORD_MAX, SpecificStep, "length_precision" );

  setHelpFileName(myIs1D ? "fillet1d_operation_page.html" : "fillet2d_operation_page.html");

  // Initialisation
  Init();
  resize(100,100);
}

//=================================================================================
// function : ~OperationGUI_Fillet1d2dDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
OperationGUI_Fillet1d2dDlg::~OperationGUI_Fillet1d2dDlg()
{
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void OperationGUI_Fillet1d2dDlg::Init()
{
  // Set Initial values of spinboxes
  GroupVertexes->SpinBox_DX->setValue(5);

  // Clear line edits
  GroupVertexes->LineEdit1->setText("");
  GroupVertexes->LineEdit2->setText("");

  myShape = GEOM::GEOM_Object::_nil();

  myVertexes.Clear();

  showOnlyPreviewControl();

  // signals and slots connections

  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()   ));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(GroupVertexes->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupVertexes->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(GroupVertexes->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupVertexes->LineEdit2, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(GroupVertexes->SpinBox_DX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));

  connect(GroupVertexes->CheckButton1, SIGNAL(toggled(bool)), this, SLOT(processPreview()));

  initName(myIs1D ? tr("GEOM_FILLET_1D") : tr("GEOM_FILLET_2D"));
  GroupVertexes->PushButton1->click();

  SelectionIntoArgument();
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void OperationGUI_Fillet1d2dDlg::ClickOnOk()
{
  setIsApplyAndClose( true );
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool OperationGUI_Fillet1d2dDlg::ClickOnApply()
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
void OperationGUI_Fillet1d2dDlg::SelectionIntoArgument()
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
      GEOM::GEOM_Object_var anObj =
        GEOMBase::ConvertIOinGEOMObject( aSelList.First() );
      
      if ( !anObj->_is_nil() ) {
        QString aName = GEOMBase::GetName( anObj );
        TopoDS_Shape aShape;
        if ( GEOMBase::GetShape( anObj, aShape, TopAbs_SHAPE ) && !aShape.IsNull() ) {
          TColStd_IndexedMapOfInteger aMap;
          aSelMgr->GetIndexes(aSelList.First(), aMap);
          if ( aMap.Extent() == 1 ) { // Local Selection
            int anIndex = aMap( 1 );
            aName += QString( myIs1D ? ":wire_%1" : ":face_%1" ).arg( anIndex );
   
            //Find SubShape Object in Father
            GEOM::GEOM_Object_var aFindedObject = GEOMBase_Helper::findObjectInFather( anObj, aName );
            
            if ( aFindedObject->_is_nil()) { // Object not found in study
              GEOM::GEOM_IShapesOperations_var aShapesOp = getGeomEngine()->GetIShapesOperations( getStudyId() );
              anObj = aShapesOp->GetSubShape( anObj, anIndex );
            }
            else
              anObj = aFindedObject; // get Object from study
          }
          else { // Global Selection
            if ((myIs1D && aShape.ShapeType() != TopAbs_WIRE) ||
                (!myIs1D && aShape.ShapeType() != TopAbs_FACE && aShape.ShapeType() != TopAbs_SHELL)) {
              anObj = GEOM::GEOM_Object::_nil();
              aName = "";
            }
          }
        }
        myShape = anObj;
        myEditCurrentArgument->setText(aName
);
        processPreview();
      }
    }
  } else if (myEditCurrentArgument == GroupVertexes->LineEdit2) {
    myVertexes.Clear();
    bool isPreview = myIs1D;
    if (aSelList.Extent() == 1) {
      GEOM::GEOM_Object_var anObj =
        GEOMBase::ConvertIOinGEOMObject( aSelList.First() );

      if ( !anObj->_is_nil() ) {
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

          isPreview = true;
          myEditCurrentArgument->setText(aName);
          myVertexes = anIndexes;
        }
      }
    }
    if ( isPreview )
      processPreview();
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
void OperationGUI_Fillet1d2dDlg::SetEditCurrentArgument()
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
  processPreview();
}

//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void OperationGUI_Fillet1d2dDlg::LineEditReturnPressed()
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
void OperationGUI_Fillet1d2dDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void OperationGUI_Fillet1d2dDlg::enterEvent (QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    this->ActivateThisDialog();
}

//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void OperationGUI_Fillet1d2dDlg::ValueChangedInSpinBox (double)
{
  processPreview();
}

//=================================================================================
// function : activateSelection
// purpose  : Activate selection in accordance with myEditCurrentArgument
//=================================================================================
void OperationGUI_Fillet1d2dDlg::activateSelection()
{
  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
  globalSelection();
  if (myEditCurrentArgument == GroupVertexes->LineEdit1)
    //localSelection(myShape, myIs1D ? TopAbs_WIRE : TopAbs_FACE);
    if (myIs1D)
      globalSelection(GEOM_WIRE);
    else {
      TColStd_MapOfInteger aMap;
      aMap.Add(GEOM_FACE);
      aMap.Add(GEOM_SHELL);
      globalSelection(aMap);
    }
  else if (!myShape->_is_nil() && myEditCurrentArgument == GroupVertexes->LineEdit2)
    localSelection(myShape, TopAbs_VERTEX);

  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr OperationGUI_Fillet1d2dDlg::createOperation()
{
  return getGeomEngine()->GetILocalOperations(getStudyId());
}

//=================================================================================
// function : isValid()
// purpose  : Verify validity of input data
//=================================================================================
bool OperationGUI_Fillet1d2dDlg::isValid (QString& msg)
{
  bool ok = !myShape->_is_nil();
  ok = GroupVertexes->SpinBox_DX->isValid( msg, !IsPreview() ) && ok;
  ok = (myIs1D || myVertexes.Extent() > 0) && ok;
  return ok;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool OperationGUI_Fillet1d2dDlg::execute (ObjectList& objects)
{
  GEOM::ListOfLong_var aListOfIndexes = new GEOM::ListOfLong;
  aListOfIndexes->length(myVertexes.Extent());

  for (int i = 1, n = myVertexes.Extent(); i <= n; i++)
    aListOfIndexes[ i - 1 ] = myVertexes(i);

  GEOM::GEOM_ILocalOperations_var anOper = GEOM::GEOM_ILocalOperations::_narrow(getOperation());
  GEOM::GEOM_Object_var anObj = myIs1D ?
    anOper->MakeFillet1D(myShape, getRadius(), aListOfIndexes, GroupVertexes->CheckButton1->isChecked()) : 
    anOper->MakeFillet2D(myShape, getRadius(), aListOfIndexes);

  if (!anObj->_is_nil()) {
    if (!IsPreview()) {
      QStringList aParameters;
      aParameters << GroupVertexes->SpinBox_DX->text();
      anObj->SetParameters(aParameters.join(":").toUtf8().constData());
    }
    objects.push_back(anObj._retn());
  }

  return true;
}

//=================================================================================
// function : getRadius
// purpose  : Get radius
//=================================================================================
double OperationGUI_Fillet1d2dDlg::getRadius() const
{
  return GroupVertexes->SpinBox_DX->value();
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> OperationGUI_Fillet1d2dDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res;
  GEOM::GeomObjPtr aGeomObjPtr(myShape);
  res << aGeomObjPtr;
  return res;
}
