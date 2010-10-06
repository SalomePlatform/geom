//  Copyright (C) 2007-2010  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

// GEOM GEOMGUI : GUI for Geometry component
// File   : BuildGUI_EdgeDlg.cxx
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.
//
#include "BuildGUI_EdgeDlg.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>

#include <GEOMImpl_Types.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>

//=================================================================================
// class    : BuildGUI_EdgeDlg()
// purpose  : Constructs a BuildGUI_EdgeDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
BuildGUI_EdgeDlg::BuildGUI_EdgeDlg (GeometryGUI* theGeometryGUI, QWidget* parent)
  : GEOMBase_Skeleton(theGeometryGUI, parent)
{
  QPixmap image0 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_SELECT")));
  QPixmap image1 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_BUILD_EDGE")));
  QPixmap image2 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_BUILD_EDGE_WIRE")));

  setWindowTitle(tr("GEOM_EDGE_TITLE"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_EDGE"));
  mainFrame()->RadioButton1->setIcon(image1);
  mainFrame()->RadioButton2->setIcon(image2);
  mainFrame()->RadioButton3->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton3->close();

  // two points

  GroupPoints = new DlgRef_2Sel(centralWidget());
  GroupPoints->GroupBox1->setTitle(tr("GEOM_POINTS"));
  GroupPoints->TextLabel1->setText(tr("GEOM_POINT_I").arg(1));
  GroupPoints->TextLabel2->setText(tr("GEOM_POINT_I").arg(2));
  GroupPoints->PushButton1->setIcon(image0);
  GroupPoints->PushButton2->setIcon(image0);
  GroupPoints->LineEdit1->setReadOnly(true);
  GroupPoints->LineEdit2->setReadOnly(true);

  // wire

  GroupWire = new DlgRef_1Sel2Spin(centralWidget());
  GroupWire->GroupBox1->setTitle(tr("GEOM_WIRE"));
  GroupWire->TextLabel1->setText(tr("GEOM_WIRE"));
  GroupWire->PushButton1->setIcon(image0);
  GroupWire->LineEdit1->setReadOnly(true);
  GroupWire->TextLabel2->setText( tr( "GEOM_LINEAR_TOLERANCE" ) );
  GroupWire->TextLabel3->setText( tr( "GEOM_ANGULAR_TOLERANCE" ) );
  double SpecificStep = 0.0001;
  double prec1 = Precision::Confusion();
  double prec2 = Precision::Angular();
  initSpinBox(GroupWire->SpinBox_DX, prec1, MAX_NUMBER, SpecificStep, "len_tol_precision" );
  initSpinBox(GroupWire->SpinBox_DY, prec2, MAX_NUMBER, SpecificStep, "ang_tol_precision" );
  GroupWire->SpinBox_DX->setValue(prec1);
  GroupWire->SpinBox_DY->setValue(prec2);

  // layout

  QVBoxLayout* layout = new QVBoxLayout(centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
  layout->addWidget(GroupPoints);
  layout->addWidget(GroupWire);
  /***************************************************************/

  setHelpFileName("create_edge_page.html");

  // initialisation
  Init();
}

//=================================================================================
// function : ~BuildGUI_EdgeDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
BuildGUI_EdgeDlg::~BuildGUI_EdgeDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BuildGUI_EdgeDlg::Init()
{
  // init variables
  myPoint1 = myPoint2 = myWire = GEOM::GEOM_Object::_nil();
  myEditCurrentArgument = GroupPoints->LineEdit1;
  GroupPoints->PushButton1->setDown(true);
  globalSelection(); // close local contexts, if any
  localSelection( GEOM::GEOM_Object::_nil(), TopAbs_VERTEX );

  // signals and slots connections
  connect( myGeomGUI, SIGNAL( SignalDeactivateActiveDialog() ), this, SLOT( DeactivateActiveDialog() ) );
  connect( myGeomGUI, SIGNAL( SignalCloseAllDialogs() ),        this, SLOT( ClickOnCancel() ) );

  connect( this,      SIGNAL( constructorsClicked( int ) ), this, SLOT( ConstructorsClicked( int ) ) );

  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(GroupPoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupWire->PushButton1,   SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(GroupPoints->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupPoints->LineEdit2, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupWire->LineEdit1,   SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
           this, SLOT( SelectionIntoArgument() ) );

  initName(tr("GEOM_EDGE"));

  ConstructorsClicked( 0 );
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void BuildGUI_EdgeDlg::ClickOnOk()
{
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool BuildGUI_EdgeDlg::ClickOnApply()
{
  if (!onAccept())
    return false;

  initName();

  myEditCurrentArgument->setText( "" );
  ConstructorsClicked( getConstructorId() );

  return true;
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void BuildGUI_EdgeDlg::ConstructorsClicked( int constructorId )
{
  switch ( constructorId ) {
  case 0:
    {
      globalSelection(); // close local contexts, if any
      localSelection( GEOM::GEOM_Object::_nil(), TopAbs_VERTEX );

      myEditCurrentArgument = GroupPoints->LineEdit1;
      GroupPoints->LineEdit1->setText( "" );
      GroupPoints->LineEdit2->setText( "" );
      myPoint1 = GEOM::GEOM_Object::_nil();
      myPoint2 = GEOM::GEOM_Object::_nil();
      GroupPoints->PushButton1->setDown(true);
      GroupPoints->PushButton2->setDown(false);
      GroupPoints->LineEdit1->setEnabled(true);
      GroupPoints->LineEdit2->setEnabled(false);
      GroupPoints->show();
      GroupWire->hide();
      break;
    }
  case 1:
    {
      globalSelection(); // close local contexts, if any
      localSelection( GEOM::GEOM_Object::_nil(), TopAbs_WIRE );

      myEditCurrentArgument = GroupWire->LineEdit1;
      GroupWire->LineEdit1->setText("");
      myWire = GEOM::GEOM_Object::_nil();
      GroupWire->PushButton1->setDown(true);
      GroupWire->LineEdit1->setEnabled(true);
      GroupPoints->hide();
      GroupWire->show();
      break;
    }
  }

  qApp->processEvents();
  updateGeometry();
  resize( minimumSizeHint() );
  SelectionIntoArgument();
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection is changed or on dialog initialization or activation
//=================================================================================
void BuildGUI_EdgeDlg::SelectionIntoArgument()
{
  erasePreview();
  myEditCurrentArgument->setText("");

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  if (aSelList.Extent() != 1) {
    if      (myEditCurrentArgument == GroupPoints->LineEdit1) myPoint1 = GEOM::GEOM_Object::_nil();
    else if (myEditCurrentArgument == GroupPoints->LineEdit2) myPoint2 = GEOM::GEOM_Object::_nil();
    else if (myEditCurrentArgument == GroupWire->LineEdit1)   myWire = GEOM::GEOM_Object::_nil();
    displayPreview();
    return;
  }

  // nbSel == 1
  Standard_Boolean testResult = Standard_False;
  GEOM::GEOM_Object_var aSelectedObject = GEOMBase::ConvertIOinGEOMObject(aSelList.First(), testResult);

  if ( testResult && !aSelectedObject->_is_nil() ) {
    QString aName = GEOMBase::GetName( aSelectedObject );
    TopAbs_ShapeEnum aNeedType = myEditCurrentArgument == GroupWire->LineEdit1 ? TopAbs_WIRE : TopAbs_VERTEX;

    TopoDS_Shape aShape;
    if ( GEOMBase::GetShape( aSelectedObject, aShape, TopAbs_SHAPE ) && !aShape.IsNull() ) {
      TColStd_IndexedMapOfInteger aMap;
      aSelMgr->GetIndexes( aSelList.First(), aMap );
      if ( aMap.Extent() == 1 ) { // Local Selection
        int anIndex = aMap( 1 );
	aName += ( aNeedType == TopAbs_WIRE ? QString( ":wire_%1" ).arg( anIndex ) : QString( ":vertex_%1" ).arg( anIndex ) );

        //Find SubShape Object in Father
        GEOM::GEOM_Object_var aFindedObject = GEOMBase_Helper::findObjectInFather( aSelectedObject, aName );
        if ( CORBA::is_nil( aFindedObject ) ) { // Object not found in study
          GEOM::GEOM_IShapesOperations_var aShapesOp = getGeomEngine()->GetIShapesOperations( getStudyId() );
          aSelectedObject = aShapesOp->GetSubShape( aSelectedObject, anIndex );
        }
        else {
          aSelectedObject = aFindedObject; // get Object from study
        }
      }
      else { // Global Selection
        if ( aShape.ShapeType() != aNeedType ) {
          aSelectedObject = GEOM::GEOM_Object::_nil();
          aName = "";
        }
      }
    }

    myEditCurrentArgument->setText( aName );

    if (!aSelectedObject->_is_nil()) { // clear selection if something selected
      globalSelection();
      localSelection( GEOM::GEOM_Object::_nil(), aNeedType );
    }

    if ( myEditCurrentArgument == GroupPoints->LineEdit1 ) {
      myPoint1 = aSelectedObject;
      if ( !myPoint1->_is_nil() && myPoint2->_is_nil() )
        GroupPoints->PushButton2->click();
    }
    else if ( myEditCurrentArgument == GroupPoints->LineEdit2 ) {
      myPoint2 = aSelectedObject;
      if ( !myPoint2->_is_nil() && myPoint1->_is_nil() )
        GroupPoints->PushButton1->click();
    }
    else if ( myEditCurrentArgument == GroupWire->LineEdit1 ) {
      myWire = aSelectedObject;
    }
  }

  displayPreview();
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BuildGUI_EdgeDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  if (send == GroupPoints->PushButton1) {
    myEditCurrentArgument = GroupPoints->LineEdit1;
    GroupPoints->PushButton2->setDown(false);
    GroupPoints->LineEdit2->setEnabled(false);
  }
  else if (send == GroupPoints->PushButton2) {
    myEditCurrentArgument = GroupPoints->LineEdit2;
    GroupPoints->PushButton1->setDown(false);
    GroupPoints->LineEdit1->setEnabled(false);
  }
  else if (send == GroupWire->PushButton1) {
    myEditCurrentArgument = GroupWire->LineEdit1;
  }

  // enable line edit
  myEditCurrentArgument->setEnabled(true);
  myEditCurrentArgument->setFocus();
  send->setDown(true);
  displayPreview();
}

//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void BuildGUI_EdgeDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if (send == GroupPoints->LineEdit1 || send == GroupPoints->LineEdit2 || send == GroupWire->LineEdit1 ) {
    myEditCurrentArgument = send;
    GEOMBase_Skeleton::LineEditReturnPressed();
  }
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void BuildGUI_EdgeDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();

  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
           this, SLOT( SelectionIntoArgument() ) );

  ConstructorsClicked( getConstructorId() );
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void BuildGUI_EdgeDlg::enterEvent (QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr BuildGUI_EdgeDlg::createOperation()
{
  return getGeomEngine()->GetIShapesOperations(getStudyId());
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool BuildGUI_EdgeDlg::isValid (QString& msg)
{
  bool ok = false;
  if ( getConstructorId() == 0 ) {
    ok = !myPoint1->_is_nil() && !myPoint2->_is_nil();
  }
  else {
    ok = !myWire->_is_nil();
    ok = ok && GroupWire->SpinBox_DX->isValid( msg, !IsPreview() );
    ok = ok && GroupWire->SpinBox_DY->isValid( msg, !IsPreview() );
  }
  return ok;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool BuildGUI_EdgeDlg::execute (ObjectList& objects)
{
  bool res = false;
  GEOM::GEOM_Object_var anObj;

  GEOM::GEOM_IShapesOperations_var anOper = GEOM::GEOM_IShapesOperations::_narrow( getOperation() );

  switch ( getConstructorId() ) {
  case 0 :
    {
      anObj = anOper->MakeEdge( myPoint1, myPoint2 );
      res = true;
      break;
    }
  case 1:
    {
      double aLinearTolerance   = GroupWire->SpinBox_DX->value();
      double anAngularTolerance = GroupWire->SpinBox_DY->value();
      
      QStringList aParameters;
      aParameters << GroupWire->SpinBox_DX->text();
      aParameters << GroupWire->SpinBox_DY->text();
      
      anObj = anOper->MakeEdgeWire( myWire, aLinearTolerance, anAngularTolerance );
      
      if ( !anObj->_is_nil() && !IsPreview() )
	anObj->SetParameters( aParameters.join(":").toLatin1().constData() );
      
      res = true;
      break;
    }
  }

  if ( !anObj->_is_nil() ) objects.push_back( anObj._retn() );

  return res;
}

//=================================================================================
// function : addSubshapeToStudy
// purpose  : virtual method to add new SubObjects if local selection
//=================================================================================
void BuildGUI_EdgeDlg::addSubshapesToStudy()
{
  QMap<QString, GEOM::GEOM_Object_var> objMap;
  switch ( getConstructorId() ) {
  case 0 :
    objMap[GroupPoints->LineEdit1->text()] = myPoint1;
    objMap[GroupPoints->LineEdit2->text()] = myPoint2;
    break;
  case 1 :
    objMap[GroupWire->LineEdit1->text()] = myWire;
    break;
  }
  addSubshapesToFather( objMap );
}
