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
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org
//
//
//
//  File   : BasicGUI_WorkingPlaneDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM

using namespace std;
#include "BasicGUI_WorkingPlaneDlg.h"

#include "GEOMBase.h"

#include "OCCViewer_Viewer3d.h"
#include "OCCViewer_ViewPort3d.h"
#include "QAD_StudyFrame.h"
#include "QAD_RightFrame.h"
#include "QAD_Desktop.h"

#include <Geom_Surface.hxx>
#include <Geom_Plane.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopExp.hxx>
#include <BRep_Tool.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <V3d_View.hxx>

#include "GEOMImpl_Types.hxx"

#include <qcheckbox.h>

//=================================================================================
// class    : BasicGUI_WorkingPlaneDlg()
// purpose  : Constructs a BasicGUI_WorkingPlaneDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
BasicGUI_WorkingPlaneDlg::BasicGUI_WorkingPlaneDlg(QWidget* parent, const char* name, SALOME_Selection* Sel, bool modal, WFlags fl)
  :GEOMBase_Skeleton(parent, name, Sel, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_SELECT")));
  QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_DLG_WPLANE_FACE")));
  QPixmap image2(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_DLG_WPLANE_VECTOR")));
  QPixmap image3(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_DLG_WPLANE_ORIGIN")));

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
  myWPlane = GeometryGUI::GetGeomGUI()->GetWorkingPlane();

  myGeomGUI->SetState( 0 );

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

  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  initName( tr( "GEOM_WPLANE" ) );
  ConstructorsClicked(0);
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void BasicGUI_WorkingPlaneDlg::ConstructorsClicked(int constructorId)
{
  disconnect(mySelection, 0, this, 0);
  myGeomGUI->SetState( 0 );

  switch (constructorId)
    {
    case 0:
      {
        globalSelection( GEOM_PLANE );

        Group2->hide();
        Group3->hide();
        resize(0, 0);
        Group1->show();

        myEditCurrentArgument = Group1->LineEdit1;
        Group1->LineEdit1->setText("");
        myFace = GEOM::GEOM_Object::_nil();

        connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
        break;
      }
    case 1:
      {
        globalSelection( GEOM_LINE );

        Group1->hide();
        Group3->hide();
        resize(0, 0);
        Group2->show();

        myEditCurrentArgument = Group2->LineEdit1;
        Group2->LineEdit1->setText("");
         Group2->LineEdit2->setText("");
        myVectX = GEOM::GEOM_Object::_nil();
        myVectZ = GEOM::GEOM_Object::_nil();

        connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
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
  displayPreview();
}

//=================================================================================
// function : GroupClicked()
// purpose  : Radio button management
//=================================================================================
void BasicGUI_WorkingPlaneDlg::GroupClicked(int groupId)
{
  aOriginType = groupId;
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
  QAD_Application::getDesktop()->putInfo(tr(""));
  const int id = getConstructorId();

  if (id == 0) {
    if ( !CORBA::is_nil( myFace ) ) {
      TopoDS_Face aPlaneShape;
      if ( GEOMBase::GetShape( myFace, aPlaneShape, TopAbs_FACE ) ) {
        Handle(Geom_Surface) aGS = BRep_Tool::Surface( TopoDS::Face( aPlaneShape ) );
        if ( !aGS.IsNull() && aGS->IsKind( STANDARD_TYPE( Geom_Plane ) ) ) {
          Handle(Geom_Plane) aGPlane = Handle(Geom_Plane)::DownCast( aGS );
          gp_Pln aPln = aGPlane->Pln();

          myWPlane = aPln.Position();
          GeometryGUI::GetGeomGUI()->SetWorkingPlane(myWPlane);
          GeometryGUI::GetGeomGUI()->ActiveWorkingPlane();
          return true;
        }
      }
    }
  } else if (id == 1) {
    if ( CORBA::is_nil( myVectX ) || CORBA::is_nil( myVectZ ) ) {
      showError( "Two vectors have to be selected" );
      return false;
    }

    TopoDS_Edge aVectX, aVectZ;
    TopoDS_Vertex V1, V2;
    gp_Vec aVX, aVZ;
    if (GEOMBase::GetShape( myVectX, aVectX, TopAbs_EDGE ) &&
        GEOMBase::GetShape( myVectZ, aVectZ, TopAbs_EDGE )) {
      TopExp::Vertices(aVectZ, V1, V2, Standard_True);
      if (!V1.IsNull() && !V2.IsNull())
        aVZ = gp_Vec(BRep_Tool::Pnt(V1), BRep_Tool::Pnt(V2));
      else {
        showError( "Bad OZ vector" );
        return false;
      }

      TopExp::Vertices(aVectX, V1, V2, Standard_True);
      if (!V1.IsNull() && !V2.IsNull())
        aVX = gp_Vec(BRep_Tool::Pnt(V1), BRep_Tool::Pnt(V2));
      else {
        showError( "Bad OX vector" );
        return false;
      }

      gp_Dir aDirZ = gp_Dir(aVZ.X(), aVZ.Y(), aVZ.Z());
      gp_Dir aDirX = gp_Dir(aVX.X(), aVX.Y(), aVX.Z());

      if (aDirX.IsParallel(aDirZ, Precision::Confusion())) {
        showError( "Parallel vectors selected" );
        return false;
      }

      myWPlane = gp_Ax3(BRep_Tool::Pnt(V1), aDirZ, aDirX);

      GeometryGUI::GetGeomGUI()->SetWorkingPlane(myWPlane);
      GeometryGUI::GetGeomGUI()->ActiveWorkingPlane();
      return true;
    }
  } else if (id == 2) {
    gp_Pnt P1 = gp_Pnt(0., 0., 0.);
    gp_Dir aDirZ, aDirX;

    if (aOriginType == 1) {
      aDirZ = gp_Dir(0., 0., 1.);
      aDirX = gp_Dir(1., 0., 0.);
    }
    else if (aOriginType == 2) {
      aDirZ = gp_Dir(1., 0., 0.);
      aDirX = gp_Dir(0., 1., 0.);
    }
    else if (aOriginType == 0) {
      aDirZ = gp_Dir(0., 1., 0.);
      aDirX = gp_Dir(0., 0., 1.);
    }

    myWPlane = gp_Ax3(P1, aDirZ, aDirX);

    GeometryGUI::GetGeomGUI()->SetWorkingPlane(myWPlane);
    GeometryGUI::GetGeomGUI()->ActiveWorkingPlane();
    return true;
  }
  return false;
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void BasicGUI_WorkingPlaneDlg::SelectionIntoArgument()
{
  myEditCurrentArgument->setText("");

  const int id = getConstructorId();
  if ( mySelection->IObjectCount() != 1 ) {
    if(id == 0)
      myFace = GEOM::GEOM_Object::_nil();
    else if(id == 1) {
      if (myEditCurrentArgument == Group2->LineEdit1)
        myVectX = GEOM::GEOM_Object::_nil();
      else if (myEditCurrentArgument == Group2->LineEdit2)
        myVectZ = GEOM::GEOM_Object::_nil();
    }
    return;
  }

  // nbSel == 1
  Standard_Boolean aRes = Standard_False;
  GEOM::GEOM_Object_var aSelectedObject = GEOMBase::ConvertIOinGEOMObject(mySelection->firstIObject(), aRes);

  if(!aRes || CORBA::is_nil( aSelectedObject ))
    return;

  if(myEditCurrentArgument == Group1->LineEdit1)
    myFace = aSelectedObject;
  else if(myEditCurrentArgument == Group2->LineEdit1)
    myVectX = aSelectedObject;
  else if(myEditCurrentArgument == Group2->LineEdit2)
    myVectZ = aSelectedObject;

  myEditCurrentArgument->setText( GEOMBase::GetName( aSelectedObject ) );
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BasicGUI_WorkingPlaneDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  if(send == Group1->PushButton1) {
    myEditCurrentArgument = Group1->LineEdit1;
    globalSelection( GEOM_PLANE );
  }
  else if(send == Group2->PushButton1) {
    myEditCurrentArgument = Group2->LineEdit1;
    globalSelection( GEOM_LINE );
  }
  else if(send == Group2->PushButton2) {
    myEditCurrentArgument = Group2->LineEdit2;
    globalSelection( GEOM_LINE );
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
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void BasicGUI_WorkingPlaneDlg::ActivateThisDialog( )
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  ConstructorsClicked( getConstructorId() );
}


//=================================================================================
// function : DeactivateActiveDialog()
// purpose  : public slot to deactivate if active
//=================================================================================
void BasicGUI_WorkingPlaneDlg::DeactivateActiveDialog()
{
  myGeomGUI->SetState( -1 );
  GEOMBase_Skeleton::DeactivateActiveDialog();
}

//=======================================================================
// function : ClickOnCancel()
// purpose  :
//=======================================================================
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
  myGeomGUI->SetState( -1 );
  GEOMBase_Skeleton::closeEvent( e );
}
