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
//  File   : MeasureGUI_DistanceDlg.cxx
//  Author : Nicolas REJNERI
//  Module : GEOM
//  $Header$

using namespace std;
#include "MeasureGUI_DistanceDlg.h"

#include "QAD_RightFrame.h"
#include "OCCViewer_Viewer3d.h"

#include <Geom_Plane.hxx>
#include <TopoDS_Edge.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <AIS_LengthDimension.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <gce_MakePln.hxx>
#include <Precision.hxx>

//=================================================================================
// class    : MeasureGUI_DistanceDlg()
// purpose  : Constructs a MeasureGUI_DistanceDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
MeasureGUI_DistanceDlg::MeasureGUI_DistanceDlg(QWidget* parent, const char* name, SALOME_Selection* Sel, bool modal, WFlags fl)
  :MeasureGUI_Skeleton(parent, name, Sel, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_DLG_MINDIST")));
  QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_MINDIST_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_DISTANCE"));
  RadioButton1->setPixmap(image0);

  GroupC1 = new MeasureGUI_2Sel1LineEdit_QTD(this, "GroupC1");
  GroupC1->GroupBox1->setTitle(tr("GEOM_MINDIST_OBJ"));
  GroupC1->TextLabel1->setText(tr("GEOM_OBJECT_I").arg("1"));
  GroupC1->TextLabel2->setText(tr("GEOM_OBJECT_I").arg("2"));
  GroupC1->TextLabel3->setText(tr("GEOM_LENGTH"));
  GroupC1->LineEdit3->setReadOnly(TRUE);
  GroupC1->PushButton1->setPixmap(image1);
  GroupC1->PushButton2->setPixmap(image1);

  Layout1->addWidget(GroupC1, 1, 0);
  /***************************************************************/

  /* Initialisation */
  Init();
}


//=================================================================================
// function : ~MeasureGUI_DistanceDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
MeasureGUI_DistanceDlg::~MeasureGUI_DistanceDlg()
{  
  /* no need to delete child widgets, Qt does it all for us */
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void MeasureGUI_DistanceDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupC1->LineEdit1;

  myOkShape1 = myOkShape2 = false;

   /* signals and slots connections */
  connect(buttonClose, SIGNAL(clicked()), this, SLOT(ClickOnCancel()));
  connect(myGeomGUI, SIGNAL(SignalDeactivateActiveDialog()), this, SLOT(DeactivateActiveDialog()));
  connect(myGeomGUI, SIGNAL(SignalCloseAllDialogs()), this, SLOT(ClickOnCancel()));

  connect(GroupC1->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(GroupC1->LineEdit2, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(GroupC1->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupC1->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  /* displays Dialog */
  GroupC1->show();
  this->show();

  return;
}


//=================================================================================
// function : ClickOnCancel()
// purpose  :
//=================================================================================
void MeasureGUI_DistanceDlg::ClickOnCancel()
{
  this->EraseDistance();
  MeasureGUI_Skeleton::ClickOnCancel();
  return;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection has changed
//=================================================================================
void MeasureGUI_DistanceDlg::SelectionIntoArgument()
{
  myGeomBase->EraseSimulationShape();
  mySimulationTopoDs.Nullify();
  this->EraseDistance();
  myEditCurrentArgument->setText("");
  QString aString = "";

  GroupC1->LineEdit3->setText("");

  int nbSel = myGeomBase->GetNameOfSelectedIObjects(mySelection, aString);
  if(nbSel != 1) {
    if(myEditCurrentArgument == GroupC1->LineEdit1)
      myOkShape1 = false;
    else if(myEditCurrentArgument == GroupC1->LineEdit2)
      myOkShape2 = false;
    return;
  }

  /*  nbSel == 1  */
  TopoDS_Shape S;
  if(!myGeomBase->GetTopoFromSelection(mySelection, S))
    return;

  Standard_Boolean testResult;
  Handle(SALOME_InteractiveObject) IO = mySelection->firstIObject();
  
  if(myEditCurrentArgument == GroupC1->LineEdit1) {
    myGeomShape1 = myGeomBase->ConvertIOinGEOMShape(IO, testResult);
    if(!testResult)
      return;
    myShape1 = S;
    myEditCurrentArgument->setText(aString);
    myOkShape1 = true;
  }    
  else if(myEditCurrentArgument == GroupC1->LineEdit2) {
    myGeomShape2 = myGeomBase->ConvertIOinGEOMShape(IO, testResult);
    if(!testResult)
      return;
    myShape2 = S;
    myEditCurrentArgument->setText(aString);
    myOkShape2 = true;
  }

  if(myOkShape1 && myOkShape2)
    this->MakeDistanceSimulationAndDisplay(myShape1 ,myShape2);
  return; 
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void MeasureGUI_DistanceDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  if(send == GroupC1->PushButton1) {
    GroupC1->LineEdit1->setFocus();
    myEditCurrentArgument = GroupC1->LineEdit1;
  }
  else if(send == GroupC1->PushButton2) {
    GroupC1->LineEdit2->setFocus();
    myEditCurrentArgument = GroupC1->LineEdit2;
  }

  this->SelectionIntoArgument();
  return;
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void MeasureGUI_DistanceDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if(send == GroupC1->LineEdit1)
    myEditCurrentArgument = GroupC1->LineEdit1;
  else if(send == GroupC1->LineEdit2)
    myEditCurrentArgument = GroupC1->LineEdit2;
  else
    return;

  MeasureGUI_Skeleton::LineEditReturnPressed();
  return;
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void MeasureGUI_DistanceDlg::ActivateThisDialog()
{
  MeasureGUI_Skeleton::ActivateThisDialog();
  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
  if(!mySimulationTopoDs.IsNull())
    myGeomBase->DisplaySimulationShape(mySimulationTopoDs);
  return;
}


//=================================================================================
// function : enterEvent()
// purpose  : when mouse enter onto the QWidget
//=================================================================================
void MeasureGUI_DistanceDlg::enterEvent(QEvent* e)
{
  if(GroupConstructors->isEnabled())
    return;
  this->ActivateThisDialog();
  return;
}


//=================================================================================
// function : closeEvent()
// purpose  :
//=================================================================================
void MeasureGUI_DistanceDlg::closeEvent(QCloseEvent* e)
{
  /* same than click on cancel button */
  this->ClickOnCancel();
  return;
}


//=================================================================================
// function : MakeDistanceSimulationAndDisplay()
// purpose  :
//=================================================================================
void MeasureGUI_DistanceDlg::MakeDistanceSimulationAndDisplay(const TopoDS_Shape& S1, const TopoDS_Shape& S2) 
{
  myGeomBase->EraseSimulationShape();
  mySimulationTopoDs.Nullify();
  GroupC1->LineEdit3->setText("");
  
  BRepExtrema_DistShapeShape dst(S1, S2);

  try {
    if(dst.IsDone()) {
      gp_Pnt P1, P2;
      Standard_Real Dist = 1.e9;
      for(int i = 1; i <= dst.NbSolution(); i++) {
	P1 = (dst.PointOnShape1(i));
	P2 = (dst.PointOnShape2(i));

	Standard_Real MinDist = P1.Distance(P2);
	if(Dist > MinDist)
	  Dist = MinDist;
      }

      if(Dist <= 1.e-9) {
	BRepBuilderAPI_MakeVertex MakeVertex(P1);
	mySimulationTopoDs = MakeVertex.Vertex();
	myGeomBase->DisplaySimulationShape(mySimulationTopoDs);

	GroupC1->LineEdit3->setText("0.0");
      } 
      else {
	BRepBuilderAPI_MakeEdge MakeEdge(P1, P2);
	mySimulationTopoDs = MakeEdge.Edge();

	TopoDS_Vertex V1 = BRepBuilderAPI_MakeVertex(P1);
	TopoDS_Vertex V2 = BRepBuilderAPI_MakeVertex(P2);

	QString S;
	S.sprintf("%.1f", Dist);

	gp_Pnt P3;
	P3.SetX((P1.X() + P2.X()) / 2);
	P3.SetY((P1.Y() + P2.Y()) / 2);
	P3.SetZ(((P1.Z() + P2.Z()) / 2) + 100);
	gp_Vec va(P3, P1);
	gp_Vec vb(P3, P2);
	if(va.IsParallel(vb, Precision::Angular())) {
	  P3.SetY(((P1.Y() + P2.Y()) / 2) + 100);
	  P3.SetZ(((P1.Z() + P2.Z()) / 2));
	}
	gce_MakePln gce_MP(P1, P2, P3);
	gp_Pln gp_P = gce_MP.Value();
	Handle(Geom_Plane) P = new Geom_Plane(gp_P);

	Handle(AIS_LengthDimension) Distance = new AIS_LengthDimension(V1, V2, P, Dist, TCollection_ExtendedString(strdup(S)));

	GroupC1->LineEdit3->setText(S);

	if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC) {
	  OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
	  Handle (AIS_InteractiveContext) ic = v3d->getAISContext();
	  ic->Display(Distance);
	  ic->UpdateCurrentViewer();
	}
      }
    }
  }
  catch(Standard_Failure) {
    MESSAGE("Catch intercepted in MakeDistanceSimulationAndDisplay()");
  }
  return;
}


//=================================================================================
// function : EraseDistance()
// purpose  :
//=================================================================================
void MeasureGUI_DistanceDlg::EraseDistance()
{
  int count = QAD_Application::getDesktop()->getActiveStudy()->getStudyFramesCount();
  for(int i = 0; i < count; i++) {
    if (QAD_Application::getDesktop()->getActiveStudy()->getStudyFrame(i)->getTypeView() == VIEW_OCC ) {
      OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getStudyFrame(i)->getRightFrame()->getViewFrame())->getViewer();
      Handle (AIS_InteractiveContext) ic = v3d->getAISContext();

      AIS_ListOfInteractive L;
      ic->DisplayedObjects(AIS_KOI_Relation, -1, L);
      AIS_ListIteratorOfListOfInteractive ite(L);
      while (ite.More()) {
	ic->Remove(ite.Value());
	ic->UpdateCurrentViewer();
	ite.Next();
      }
    }
  }
  return;
}
