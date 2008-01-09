//  GEOM GEOMGUI : GUI for Geometry component
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//
//
//  File   : BasicGUI_WorkingPlaneDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM

#include "BasicGUI_WorkingPlaneDlg.h"

#include "GEOMBase.h"

#include "SUIT_Desktop.h"
#include "SUIT_Session.h"
#include "SalomeApp_Application.h"
#include "LightApp_SelectionMgr.h"

// OCCT Includes
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopExp.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <TopoDS_Shape.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

// QT Includes
#include <qcheckbox.h>
#include <qlabel.h>

#include "GEOMImpl_Types.hxx"

using namespace std;

//=================================================================================
// class    : BasicGUI_WorkingPlaneDlg()
// purpose  : Constructs a BasicGUI_WorkingPlaneDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
BasicGUI_WorkingPlaneDlg::BasicGUI_WorkingPlaneDlg(GeometryGUI* theGeometryGUI, QWidget* parent,
                                                   const char* name, bool modal, WFlags fl)
  :GEOMBase_Skeleton(theGeometryGUI, parent, name, modal, WStyle_Customize |
                     WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_SELECT")));
  QPixmap image1(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_DLG_WPLANE_FACE")));
  QPixmap image2(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_DLG_WPLANE_VECTOR")));
  QPixmap image3(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM",tr("ICON_DLG_WPLANE_ORIGIN")));

  setCaption(tr("GEOM_WPLANE_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_WPLANE"));
  RadioButton1->setPixmap(image1);
  RadioButton2->setPixmap(image2);
  RadioButton3->setPixmap(image3);

  Group1 = new DlgRef_1Sel_QTD(this, "Group1");
  Group1->GroupBox1->setTitle(tr("GEOM_WPLANE_FACE"));
  Group1->TextLabel1->setText(tr("GEOM_SELECTION"));
  Group1->PushButton1->setPixmap(image0);
  Group1->LineEdit1->setReadOnly( true );

  Group2 = new DlgRef_2Sel_QTD(this, "Group2");
  Group2->GroupBox1->setTitle(tr("GEOM_WPLANE_VECTOR"));
  Group2->TextLabel1->setText(tr("GEOM_WPLANE_VX"));
  Group2->TextLabel2->setText(tr("GEOM_WPLANE_VZ"));
  Group2->PushButton1->setPixmap(image0);
  Group2->PushButton2->setPixmap(image0);
  Group2->LineEdit1->setReadOnly( true );
  Group2->LineEdit2->setReadOnly( true );

  Group3 = new DlgRef_3Check_QTD(this, "Group3");
  Group3->GroupBox1->setTitle(tr("GEOM_WPLANE_ORIGIN"));
  Group3->RadioButton1->setText(tr("GEOM_WPLANE_OXY"));
  Group3->RadioButton2->setText(tr("GEOM_WPLANE_OYZ"));
  Group3->RadioButton3->setText(tr("GEOM_WPLANE_OZX"));

  Layout1->addWidget(Group1, 1, 0);
  Layout1->addWidget(Group2, 1, 0);
  Layout1->addWidget(Group3, 1, 0);
  /***************************************************************/
  QGroupBox* aReverseGroupBox = new QGroupBox(this, "aReverseGroupBox");
  aReverseGroupBox->setTitle(tr(""));
  aReverseGroupBox->setColumnLayout(1, Qt::Horizontal);
  aReverseGroupBox->setInsideMargin(10);
  
  myReverseCB = new QCheckBox(aReverseGroupBox, "myReverseCB");
  myReverseCB->setText(tr("GEOM_REVERSE_PLANE"));
    
  Layout1->addWidget(aReverseGroupBox, 2, 0);
  

  setHelpFileName("create_wplane_page.html");

  Init();
}

//=================================================================================
// function : ~BasicGUI_WorkingPlaneDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
BasicGUI_WorkingPlaneDlg::~BasicGUI_WorkingPlaneDlg()
{
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BasicGUI_WorkingPlaneDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = Group1->LineEdit1;
  myWPlane = myGeomGUI->GetWorkingPlane();

  // myGeomGUI->SetState( 0 );

  myFace = GEOM::GEOM_Object::_nil();
  myVectX = GEOM::GEOM_Object::_nil();
  myVectZ = GEOM::GEOM_Object::_nil();

  aOriginType = 1;

  /* Filter definition */
  globalSelection( GEOM_PLANE );

   /* signals and slots connections */
  connect(buttonCancel, SIGNAL(clicked()), this, SLOT(ClickOnCancel()));
  connect(myGeomGUI, SIGNAL(SignalDeactivateActiveDialog()), this, SLOT(DeactivateActiveDialog()));
  connect(myGeomGUI, SIGNAL(SignalCloseAllDialogs()), this, SLOT(ClickOnCancel()));
  connect(GroupConstructors, SIGNAL(clicked(int)), this, SLOT(ConstructorsClicked(int)));

  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(Group1->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(Group1->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(Group2->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(Group2->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(Group2->LineEdit2, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(Group2->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(Group3->GroupBox1, SIGNAL(clicked(int)), this, SLOT(GroupClicked(int)));

  connect(myReverseCB, SIGNAL(clicked()), this, SLOT(onReverse()));

  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(),
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  initName( tr( "GEOM_WPLANE" ) );
  ConstructorsClicked(0);
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void BasicGUI_WorkingPlaneDlg::ConstructorsClicked(int constructorId)
{
  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();

  disconnect(aSelMgr, 0, this, 0);

  switch (constructorId)
    {
    case 0:
      {
        //globalSelection( GEOM_PLANE );
        TColStd_MapOfInteger aMap;
        aMap.Add( GEOM_PLANE );
        aMap.Add( GEOM_MARKER );
        globalSelection( aMap );

        Group2->hide();
        Group3->hide();
        resize(0, 0);
        Group1->show();

        myEditCurrentArgument = Group1->LineEdit1;
        Group1->LineEdit1->setText("");
        myFace = GEOM::GEOM_Object::_nil();

        connect(aSelMgr, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
        break;
      }
    case 1:
      {
	//globalSelection( GEOM_LINE );
	GEOM::GEOM_Object_var anObj;
	localSelection( anObj, TopAbs_EDGE );

        Group1->hide();
        Group3->hide();
        resize(0, 0);
        Group2->show();

        myEditCurrentArgument = Group2->LineEdit1;
        Group2->LineEdit1->setText("");
	Group2->LineEdit2->setText("");
        myVectX = GEOM::GEOM_Object::_nil();
        myVectZ = GEOM::GEOM_Object::_nil();

        connect(aSelMgr, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
        break;
      }
    case 2:
      {
        Group1->hide();
        Group2->hide();
        resize(0, 0);
        Group3->show();

        Group3->RadioButton1->setChecked(true);
        aOriginType = 1;
        break;
      }
    }
  
  updateWPlane();
}

//=================================================================================
// function : GroupClicked()
// purpose  : Radio button management
//=================================================================================
void BasicGUI_WorkingPlaneDlg::GroupClicked(int groupId)
{
  aOriginType = groupId;
  updateWPlane();
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void BasicGUI_WorkingPlaneDlg::ClickOnOk()
{
  if ( ClickOnApply() )
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool BasicGUI_WorkingPlaneDlg::ClickOnApply()
{
  buttonApply->setFocus();
  myGeomGUI->application()->putInfo(tr(""));
 
  if (updateWPlane(false)){
    myGeomGUI->SetWorkingPlane(myWPlane);
    myGeomGUI->ActiveWorkingPlane();
  }
  
  return true;
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void BasicGUI_WorkingPlaneDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("");
  QString aName;

  const int id = getConstructorId();
  if (IObjectCount() != 1) {
    if (id == 0)
      myFace = GEOM::GEOM_Object::_nil();
    else if (id == 1) {
      if (myEditCurrentArgument == Group2->LineEdit1)
        myVectX = GEOM::GEOM_Object::_nil();
      else if (myEditCurrentArgument == Group2->LineEdit2)
        myVectZ = GEOM::GEOM_Object::_nil();
    }
    return;
  }

  // nbSel == 1
  Standard_Boolean aRes = Standard_False;
  GEOM::GEOM_Object_var aSelectedObject = GEOMBase::ConvertIOinGEOMObject(firstIObject(), aRes);

  if (!aRes || CORBA::is_nil(aSelectedObject))
    return;

  aName = GEOMBase::GetName(aSelectedObject);

  if (myEditCurrentArgument == Group1->LineEdit1)
    myFace = aSelectedObject;
  else if (myEditCurrentArgument == Group2->LineEdit1 || myEditCurrentArgument == Group2->LineEdit2)
  {
    if ( aRes && !aSelectedObject->_is_nil() )
    {
      TopoDS_Shape aShape;
      if ( GEOMBase::GetShape( aSelectedObject, aShape, TopAbs_SHAPE ) && !aShape.IsNull() )
      {
        LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
        TColStd_IndexedMapOfInteger aMap;
        aSelMgr->GetIndexes( firstIObject(), aMap );
        if ( aMap.Extent() == 1 )
        {
          int anIndex = aMap( 1 );
          aName = aName + ":edge_" + QString::number( anIndex );

	  GEOM::GEOM_IShapesOperations_var aShapesOp =
	    getGeomEngine()->GetIShapesOperations( getStudyId() );
	  if(myEditCurrentArgument == Group2->LineEdit1)
	    myVectX = aShapesOp->GetSubShape(aSelectedObject, anIndex);
	  else
	    myVectZ = aShapesOp->GetSubShape(aSelectedObject, anIndex);
        }
        else {
          if (myEditCurrentArgument == Group2->LineEdit1)
            myVectX = aSelectedObject;
          else
            myVectZ = aSelectedObject;
        }
        aSelMgr->clearSelected();
      }
    }
  }

  myEditCurrentArgument->setText( aName );

  updateWPlane();
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BasicGUI_WorkingPlaneDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  if (send == Group1->PushButton1) {
    myEditCurrentArgument = Group1->LineEdit1;
    globalSelection( GEOM_PLANE );
  }
  else if (send == Group2->PushButton1) {
    myEditCurrentArgument = Group2->LineEdit1;
    GEOM::GEOM_Object_var anObj;
    localSelection( anObj, TopAbs_EDGE );
  }
  else if (send == Group2->PushButton2) {
    myEditCurrentArgument = Group2->LineEdit2;
    GEOM::GEOM_Object_var anObj;
    localSelection( anObj, TopAbs_EDGE );
  }

  myEditCurrentArgument->setFocus();
  SelectionIntoArgument();
}

//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void BasicGUI_WorkingPlaneDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if(send == Group1->LineEdit1 || send == Group2->LineEdit1 || send == Group2->LineEdit2) {
    myEditCurrentArgument = send;
    GEOMBase_Skeleton::LineEditReturnPressed();
  }
}

//=================================================================================
// function : onReverse()
// purpose  :
//=================================================================================
void BasicGUI_WorkingPlaneDlg::onReverse()
{
  updateWPlane();
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void BasicGUI_WorkingPlaneDlg::ActivateThisDialog( )
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(),
	  SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  ConstructorsClicked( getConstructorId() );
}

//=================================================================================
// function : DeactivateActiveDialog()
// purpose  : public slot to deactivate if active
//=================================================================================
void BasicGUI_WorkingPlaneDlg::DeactivateActiveDialog()
{
  GEOMBase_Skeleton::DeactivateActiveDialog();
}

//=================================================================================
// function : ClickOnCancel()
// purpose  :
//=================================================================================
void BasicGUI_WorkingPlaneDlg::ClickOnCancel()
{
  GEOMBase_Skeleton::ClickOnCancel();
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void BasicGUI_WorkingPlaneDlg::enterEvent(QEvent* e)
{
  if ( !GroupConstructors->isEnabled() )
    ActivateThisDialog();
}

//=================================================================================
// function : closeEvent
// purpose  :
//=================================================================================
void BasicGUI_WorkingPlaneDlg::closeEvent( QCloseEvent* e )
{
  GEOMBase_Skeleton::closeEvent( e );
}

//=================================================================================
// function : updateWPlane
// purpose  :
//=================================================================================
bool BasicGUI_WorkingPlaneDlg::updateWPlane( const bool showPreview )
{
  erasePreview();
  
  const int id = getConstructorId();

  if (id == 0) { // by planar face selection
    if (CORBA::is_nil(myFace)) {
      if(!showPreview)
	showError( "Face has to be selected" );
      return false;
    }

    // PAL12781: set center of WPL to face's center of mass
    // like it is done for LCS creation
    CORBA::Double Ox,Oy,Oz, Zx,Zy,Zz, Xx,Xy,Xz;
    Ox = Oy = Oz = Zx = Zy = Xy = Xz = 0.;
    Zz = Xx = 1.;

    GEOM::GEOM_IMeasureOperations_ptr aMeasureOp =
      myGeomGUI->GetGeomGen()->GetIMeasureOperations(getStudyId());
    aMeasureOp->GetPosition(myFace, Ox,Oy,Oz, Zx,Zy,Zz, Xx,Xy,Xz);

    if (aMeasureOp->IsDone()) {
      gp_Pnt aPnt (Ox,Oy,Oz);
      gp_Dir aDirN (Zx,Zy,Zz);
      gp_Dir aDirX (Xx,Xy,Xz);
      myWPlane = gp_Ax3(aPnt, aDirN, aDirX);
    } else {
      if(!showPreview)
	showError( "Wrong shape selected (has to be a planar face)" );
      return false;
    }
  }
  else if (id == 1) { // by two vectors (Ox & Oz)
    if ( CORBA::is_nil( myVectX ) || CORBA::is_nil( myVectZ ) ) {
      if(!showPreview)
	showError( "Two vectors have to be selected" );
      return false;
    }

    TopoDS_Edge aVectX, aVectZ;
    TopoDS_Vertex VX1, VX2, VZ1, VZ2;
    gp_Vec aVX, aVZ;

    if (!GEOMBase::GetShape( myVectX, aVectX, TopAbs_EDGE ) ||
        !GEOMBase::GetShape( myVectZ, aVectZ, TopAbs_EDGE )) {
      if(!showPreview)
	showError( "Wrong shape selected (two vectors(edges) have to be selected)" );
      return false;
    }

    TopExp::Vertices(aVectX, VX1, VX2, Standard_True);
    TopExp::Vertices(aVectZ, VZ1, VZ2, Standard_True);

    if (VX1.IsNull() || VX2.IsNull()) {
      if(!showPreview)
	showError( "Bad OX vector" );
      return false;
    }
    if (VZ1.IsNull() || VZ2.IsNull()) {
      if(!showPreview)
	showError( "Bad OZ vector" );
      return false;
    }

    aVX = gp_Vec(BRep_Tool::Pnt(VX1), BRep_Tool::Pnt(VX2));
    aVZ = gp_Vec(BRep_Tool::Pnt(VZ1), BRep_Tool::Pnt(VZ2));

    if (aVX.Magnitude() < Precision::Confusion()) {
      if(!showPreview)
	showError( "Bad OX vector" );
      return false;
    }
    if (aVZ.Magnitude() < Precision::Confusion()) {
      if(!showPreview)
	showError( "Bad OZ vector" );
      return false;
    }

    gp_Dir aDirX = gp_Dir(aVX.X(), aVX.Y(), aVX.Z());
    gp_Dir aDirZ = gp_Dir(aVZ.X(), aVZ.Y(), aVZ.Z());

    if (aDirX.IsParallel(aDirZ, Precision::Angular())) {
      if(!showPreview)
	showError( "Parallel vectors selected" );
      return false;
    }

    myWPlane = gp_Ax3(BRep_Tool::Pnt(VX1), aDirZ, aDirX);
  }
  else if (id == 2) { // by selection from standard (OXY or OYZ, or OZX)
    gp_Ax2 anAx2;

    if      (aOriginType == 1) anAx2 = gp::XOY();
    else if (aOriginType == 2) anAx2 = gp::YOZ();
    else if (aOriginType == 0) anAx2 = gp::ZOX();

    myWPlane = gp_Ax3(anAx2);
  }
  else {
    return false;
  }

  if (myReverseCB->isChecked())
  {
    myWPlane.YReverse();
    myWPlane.ZReverse();
  }

  if (showPreview)
  {
    GEOM::GEOM_IBasicOperations_var aBasicOp = getGeomEngine()->GetIBasicOperations(getStudyId());
    GEOM::GEOM_Object_var anObj = aBasicOp->MakeMarker
      (myWPlane.Location().X()  , myWPlane.Location().Y()  , myWPlane.Location().Z(),
       myWPlane.XDirection().X(), myWPlane.XDirection().Y(), myWPlane.XDirection().Z(),
       myWPlane.YDirection().X(), myWPlane.YDirection().Y(), myWPlane.YDirection().Z());
    displayPreview(anObj);
  }

  return true;
}

