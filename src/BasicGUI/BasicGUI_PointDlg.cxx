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
//  File   : BasicGUI_PointDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

using namespace std;
#include "BasicGUI_PointDlg.h"

#include "QAD_RightFrame.h"
#include "OCCViewer_Viewer3d.h"

#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRep_Tool.hxx>
#include <Geom_Curve.hxx>
#if OCC_VERSION_MAJOR >= 5
#include <BRepAlgo.hxx>
#else
#include <BRepAlgoAPI.hxx>
#endif

//=================================================================================
// class    : BasicGUI_PointDlg()
// purpose  : Constructs a BasicGUI_PointDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
BasicGUI_PointDlg::BasicGUI_PointDlg(QWidget* parent, const char* name, BasicGUI* theBasicGUI, SALOME_Selection* Sel, const Handle (AIS_InteractiveContext)& ic, bool modal, WFlags fl)
  :GEOMBase_Skeleton(parent, name, Sel, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu )
{
  QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_DLG_POINT")));
  QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_DLG_POINT_EDGE")));
  QPixmap image2(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_POINT_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_POINTS"));
  RadioButton1->setPixmap(image0);
  RadioButton2->setPixmap(image1);
  RadioButton3->close(TRUE);

  GroupPoints = new DlgRef_3Spin(this, "GroupPoints");
  GroupPoints->GroupBox1->setTitle(tr("GEOM_COORDINATES"));
  GroupPoints->TextLabel1->setText(tr("GEOM_X"));
  GroupPoints->TextLabel2->setText(tr("GEOM_Y"));
  GroupPoints->TextLabel3->setText(tr("GEOM_Z"));

  GroupDimensions = new DlgRef_1Sel1Spin(this, "GroupDimensions");
  GroupDimensions->GroupBox1->setTitle(tr("GEOM_PARAM_POINT"));
  GroupDimensions->TextLabel1->setText(tr("GEOM_EDGE"));
  GroupDimensions->TextLabel2->setText(tr("GEOM_PARAMETER"));
  GroupDimensions->PushButton1->setPixmap(image2);

  Layout1->addWidget(GroupPoints, 1, 0);
  Layout1->addWidget(GroupDimensions, 1, 0);
  /***************************************************************/

  /* Initialisations */
  myBasicGUI = theBasicGUI;
  Init(ic);
}


//=======================================================================
// function : ~BasicGUI_PointDlg()
// purpose  : Destructor
//=======================================================================
BasicGUI_PointDlg::~BasicGUI_PointDlg()
{
  /* no need to delete child widgets, Qt does it all for us */
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void BasicGUI_PointDlg::Init(const Handle(AIS_InteractiveContext)& ic)
{
  /* init variables */
  myConstructorId = 0;
  myEditCurrentArgument = GroupDimensions->LineEdit1;

  myIC = ic;
  myParameter = 0.50;
  myPoint.SetCoord(0.0, 0.0, 0.0);
  myOkEdge = false;

  myGeomGUI->myState = 0;

  /*  Vertices Filter for all arguments */
  myEdgeFilter = new GEOM_ShapeTypeFilter(TopAbs_EDGE, myGeom);

  /* Get setting of step value from file configuration */
  QString St = QAD_CONFIG->getSetting("Geometry:SettingsGeomStep");
  step = St.toDouble();

  /* min, max, step and decimals for spin boxes */
  double specificStep = 0.1;
  GroupDimensions->SpinBox_DX->RangeStepAndValidator(-999999.99999, 999999.99999, specificStep, 5);
  GroupPoints->SpinBox_DX->RangeStepAndValidator(-999.999, 999.999, step, 3);
  GroupPoints->SpinBox_DY->RangeStepAndValidator(-999.999, 999.999, step, 3);
  GroupPoints->SpinBox_DZ->RangeStepAndValidator(-999.999, 999.999, step, 3);
  GroupDimensions->SpinBox_DX->SetValue(myParameter);
  GroupPoints->SpinBox_DX->SetValue(0.0);
  GroupPoints->SpinBox_DY->SetValue(0.0);
  GroupPoints->SpinBox_DZ->SetValue(0.0);

  if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC) {
    myLocalContextId = myIC->OpenLocalContext();
    //myGeomBase->SetDisplayedObjectList();
    /* sub shapes selection */
    myLocalContextMode = TopAbs_VERTEX;
    myIC->ActivateStandardMode(myLocalContextMode);
    myUseLocalContext = true;
  } 
  else
    myUseLocalContext = false;

  TopoDS_Shape S;
  bool displayPoint = true;
  if(myGeomBase->GetTopoFromSelection(mySelection, S)) {
    /* Filter a possibly previous selection and try to put it into coordinates */
    if(myGeomBase->VertexToPoint(S, myPoint))
      displayPoint = false;
  }
  this->PointIntoCoordinates(myPoint, displayPoint);
    
  /* signals and slots connections */
  connect(buttonCancel, SIGNAL(clicked()), this, SLOT(ClickOnCancel()));
  connect(myGeomGUI, SIGNAL(SignalDeactivateActiveDialog()), this, SLOT(DeactivateActiveDialog()));
  connect(myGeomGUI, SIGNAL(SignalCloseAllDialogs()), this, SLOT(ClickOnCancel()));

  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));
  connect(GroupConstructors, SIGNAL(clicked(int)), this, SLOT(ConstructorsClicked(int)));

  connect(GroupDimensions->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupDimensions->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));

  connect(GroupDimensions->SpinBox_DX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupPoints->SpinBox_DX, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupPoints->SpinBox_DY, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupPoints->SpinBox_DZ, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));

  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupDimensions->SpinBox_DX, SLOT(SetStep(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupPoints->SpinBox_DX, SLOT(SetStep(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupPoints->SpinBox_DY, SLOT(SetStep(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupPoints->SpinBox_DZ, SLOT(SetStep(double)));

  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  /* displays Dialog */
  GroupDimensions->hide();
  GroupPoints->show();
  this->show();

  return;
}


//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void BasicGUI_PointDlg::ConstructorsClicked(int constructorId)
{
  myConstructorId = constructorId;
  mySelection->ClearFilters();
  myGeomBase->EraseSimulationShape();
  mySimulationTopoDs.Nullify();
  disconnect(mySelection, 0, this, 0);

  switch (constructorId)
    {
    case 0:
      {
	if(myUseLocalContext == false && QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC) {
	  myLocalContextId = myIC->OpenLocalContext();
	  //myGeomBase->SetDisplayedObjectList();
	  /* sub shapes selection */
	  myLocalContextMode = TopAbs_VERTEX;
	  myIC->ActivateStandardMode(myLocalContextMode);
	  myUseLocalContext = true;
	}

	GroupDimensions->hide();
	resize(0, 0);
	GroupPoints->show();

	/* Display point simulation */
	PointIntoCoordinates(myPoint, true);
	break;
      }
    case 1:
      {
	if(myUseLocalContext == true && QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC) {
	  myIC->CloseLocalContext(myLocalContextId);
	  myUseLocalContext = false;
	}

	GroupPoints->hide();
	resize(0, 0);
	GroupDimensions->show();

	myEditCurrentArgument = GroupDimensions->LineEdit1;
	GroupDimensions->LineEdit1->setText("");
	myOkEdge = false;

	/* filter for next selections */
	mySelection->AddFilter(myEdgeFilter);
	connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
	break;
      }
    }
 return;
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void BasicGUI_PointDlg::ClickOnOk()
{
  this->ClickOnApply();
  this->ClickOnCancel();
  return;
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
void BasicGUI_PointDlg::ClickOnApply()
{
  buttonApply->setFocus();
  QAD_Application::getDesktop()->putInfo(tr(""));
  if(mySimulationTopoDs.IsNull())
    return;
  myGeomBase->EraseSimulationShape();
  mySimulationTopoDs.Nullify();

  /* Close local context */
  if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC) {
    myIC->CloseLocalContext(myLocalContextId);
    myUseLocalContext = false;
  }

  switch(myConstructorId) 
    {
    case 0 :
      {
	/* Recup args and call method */
	double x = GroupPoints->SpinBox_DX->GetValue();
	double y = GroupPoints->SpinBox_DY->GetValue();
	double z = GroupPoints->SpinBox_DZ->GetValue();
      
	myBasicGUI->MakePointAndDisplay(x,y,z);  /* WARNING : no display if a local context is opened */
	if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC) {
	  /* no display if a local context is opened */
	  myLocalContextId = myIC->OpenLocalContext();
	  //myGeomBase->SetDisplayedObjectList();
	  /* sub shapes selection */
	  myLocalContextMode = TopAbs_VERTEX;
	  myIC->ActivateStandardMode(myLocalContextMode);
	  myUseLocalContext = true;
	}
	break;
      }
    case 1 :
      {
	if(myOkEdge == true) {
	  /* this constructor method has no idl interface : we use same than constructor 0 */
	  myBasicGUI->MakePointAndDisplay(myPoint.X(), myPoint.Y(), myPoint.Z());
	}
	break;
      }
    }
  return;
}


//=======================================================================
// function : ClickOnCancel()
// purpose  :
//=======================================================================
void BasicGUI_PointDlg::ClickOnCancel()
{
  if(myConstructorId == 0 && QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC) {
    OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
    myIC = v3d->getAISContext();
    if(myIC->HasOpenedContext()) {
      myIC->CloseLocalContext(myLocalContextId);
      myUseLocalContext = false;
    }
  }

  myGeomGUI->myState = -1;
  GEOMBase_Skeleton::ClickOnCancel();
  return;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed (for constructors not using local context)
//=================================================================================
void BasicGUI_PointDlg::SelectionIntoArgument()
{
  myGeomBase->EraseSimulationShape();
  mySimulationTopoDs.Nullify();
  myEditCurrentArgument->setText("");
  QString aString = ""; /* name of selection */

  int nbSel = myGeomBase->GetNameOfSelectedIObjects(mySelection, aString);
  if(nbSel != 1) {
    myOkEdge = false;
    return;
  }

  // nbSel == 1
  TopoDS_Shape S; 
  if(!myGeomBase->GetTopoFromSelection(mySelection, S))
    return;

  if(S.ShapeType() == TopAbs_EDGE) {
    if(CalculateVertexOnCurve(TopoDS::Edge(S), myParameter, mySimulationTopoDs)) {
      if(myGeomBase->VertexToPoint(mySimulationTopoDs, myPoint)) {
	GroupDimensions->LineEdit1->setText(aString);
	myOkEdge = true;
	myGeomBase->DisplaySimulationShape(mySimulationTopoDs);
      }
    }
  }
  return;
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void BasicGUI_PointDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();
  if(send == GroupDimensions->LineEdit1)
    myEditCurrentArgument = GroupDimensions->LineEdit1;
  else
    return;

  GEOMBase_Skeleton::LineEditReturnPressed();
  return;
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void BasicGUI_PointDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  if(send == GroupDimensions->PushButton1) {
    GroupDimensions->LineEdit1->setFocus();
    myEditCurrentArgument = GroupDimensions->LineEdit1;
    mySelection->AddFilter(myEdgeFilter);
    this->SelectionIntoArgument();
  }
  return;
}


//=================================================================================
// function : enterEvent()
// purpose  : to reactivate this dialog box when mouse enter onto the window
//=================================================================================
void BasicGUI_PointDlg::enterEvent(QEvent* e)
{
  if (GroupConstructors->isEnabled())
    return;
  this->ActivateThisDialog();
  return;
}


//=======================================================================
// function : closeEvent()
// purpose  :
//=======================================================================
void BasicGUI_PointDlg::closeEvent(QCloseEvent* e)
{
  /* same than click on cancel button */
  this->ClickOnCancel();
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void BasicGUI_PointDlg::ActivateThisDialog( )
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  myGeomGUI->myState = 0;
  if(QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC) {
    OCCViewer_Viewer3d* v3d = ((OCCViewer_ViewFrame*)QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getRightFrame()->getViewFrame())->getViewer();
    myIC = v3d->getAISContext();
    myLocalContextId = myIC->OpenLocalContext();
    //myGeomBase->SetDisplayedObjectList();
    /* sub shapes selection */
    myLocalContextMode = TopAbs_VERTEX;
    myIC->ActivateStandardMode(myLocalContextMode);
    myUseLocalContext = true;
  }

  if(myConstructorId == 1)
    mySelection->AddFilter(myEdgeFilter);

  if(!mySimulationTopoDs.IsNull())
    myGeomBase->DisplaySimulationShape(mySimulationTopoDs);
  return;
}


//=================================================================================
// function : DeactivateActiveDialog()
// purpose  : public slot to deactivate if active
//=================================================================================
void BasicGUI_PointDlg::DeactivateActiveDialog()
{
  if(myConstructorId == 0 && QAD_Application::getDesktop()->getActiveStudy()->getActiveStudyFrame()->getTypeView() == VIEW_OCC) {
    myIC->CloseLocalContext(myLocalContextId);
    myUseLocalContext = false;
  }
  myGeomGUI->myState = -1;
  GEOMBase_Skeleton::DeactivateActiveDialog();
  return;
}


//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void BasicGUI_PointDlg::ValueChangedInSpinBox(double newValue)
{
  DlgRef_SpinBox* send = (DlgRef_SpinBox*)sender();
  double vx, vy, vz;

  if(send == GroupPoints->SpinBox_DX) {
    vx = newValue;
    vy = GroupPoints->SpinBox_DY->GetValue();
    vz = GroupPoints->SpinBox_DZ->GetValue();
  } else if(send == GroupPoints->SpinBox_DY) {
    vx = GroupPoints->SpinBox_DX->GetValue();
    vy = newValue;
    vz = GroupPoints->SpinBox_DZ->GetValue();
  } else if(send == GroupPoints->SpinBox_DZ) {
    vx = GroupPoints->SpinBox_DX->GetValue();
    vy = GroupPoints->SpinBox_DY->GetValue();
    vz = newValue;
  }  else if(send == GroupDimensions->SpinBox_DX) {
    myParameter = newValue;
  } else
    return;

  switch(myConstructorId) 
    {
    case 0: // default constructor 
      {
	myPoint.SetCoord(vx, vy, vz);
	mySimulationTopoDs = BRepBuilderAPI_MakeVertex (myPoint).Shape();
	myGeomBase->DisplaySimulationShape(mySimulationTopoDs);
	break;
      }
    case 1: 
      {
	this->SelectionIntoArgument();	
	break;
      }
    }
  return;
}


//=================================================================================
// function : CalculateVertexOnCurve()
// purpose  : Calculate a Vertex on the curve given by 'anEdge'.
//          : The position of resultVertex is given by aParameter.
//          : For a linear edge, aParameter=0.0 gives the first vertex of edge
//          :                    aParameter=1.0 gives the last vertex of edge
//          :                    aParameter=0.5 gives the vertex on the middle of edge
//          : It is possible to get vertices out of edge using values > 1.0 or < 0.0
//=================================================================================
bool BasicGUI_PointDlg::CalculateVertexOnCurve(const TopoDS_Edge& anEdge, const Standard_Real aParameter, TopoDS_Shape& resultVertex) 
{
#if OCC_VERSION_MAJOR >= 5
  if(anEdge.IsNull() || !BRepAlgo::IsValid(anEdge))
#else
  if(anEdge.IsNull() || !BRepAlgoAPI::IsValid(anEdge))
#endif
    return false;

  Standard_Real first, last;
  Handle(Geom_Curve) curv = BRep_Tool::Curve(anEdge, first, last);
  if(!curv->IsCN(0))
    return false;
  
  Standard_Real param;
  if(anEdge.Orientation() == TopAbs_FORWARD)
    param = first + (last-first) * aParameter;
  else
    param = last + (first-last) * aParameter;

  gp_Pnt paramPoint;
  curv->D0(param, paramPoint);
  resultVertex = BRepBuilderAPI_MakeVertex(paramPoint);
  return true;
}


//=======================================================================
// funcion  : PointIntoCoordinates()
// purpose  : Sets user point coordinates into this dialog Spin boxes
//          : and displays it or not according to 'bool displayPoint'
//=======================================================================
void BasicGUI_PointDlg::PointIntoCoordinates(gp_Pnt P, bool displayPoint)
{
  GroupPoints->SpinBox_DX->SetValue(P.X());
  GroupPoints->SpinBox_DY->SetValue(P.Y());
  GroupPoints->SpinBox_DZ->SetValue(P.Z());
  myPoint.SetCoord(P.X(), P.Y(), P.Z());
  if(displayPoint) {
    mySimulationTopoDs = BRepBuilderAPI_MakeVertex(P).Shape();
    myGeomBase->DisplaySimulationShape(mySimulationTopoDs);
  }
  return;
}
