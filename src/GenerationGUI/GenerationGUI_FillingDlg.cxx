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
//  File   : GenerationGUI_FillingDlg.cxx
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#include "GenerationGUI_FillingDlg.h"

#include "QAD_WaitCursor.h"
#include "QAD_Config.h"

#include <GeomFill_SectionGenerator.hxx>
#include <GeomFill_Line.hxx>
#include <GeomFill_AppSurf.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Iterator.hxx>
#include <BRep_Tool.hxx>
#include <Precision.hxx>
#include <Standard_ErrorHandler.hxx>

#include "utilities.h"

using namespace std;

//=================================================================================
// class    : GenerationGUI_FillingDlg()
// purpose  : Constructs a GenerationGUI_FillingDlg which is a child of 'parent', with the 
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
GenerationGUI_FillingDlg::GenerationGUI_FillingDlg(QWidget* parent, const char* name, GenerationGUI* theGenerationGUI, SALOME_Selection* Sel, bool modal, WFlags fl)
  :GEOMBase_Skeleton(parent, name, Sel, modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
{
  QPixmap image0(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_DLG_FILLING")));
  QPixmap image1(QAD_Desktop::getResourceManager()->loadPixmap("GEOM",tr("ICON_SELECT")));

  setCaption(tr("GEOM_FILLING_TITLE"));

  /***************************************************************/
  GroupConstructors->setTitle(tr("GEOM_FILLING"));
  RadioButton1->setPixmap(image0);
  RadioButton2->close(TRUE);
  RadioButton3->close(TRUE);

  GroupPoints = new DlgRef_1Sel5Spin(this, "GroupPoints");
  GroupPoints->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  GroupPoints->TextLabel1->setText(tr("GEOM_FILLING_COMPOUND"));
  GroupPoints->TextLabel2->setText(tr("GEOM_FILLING_MIN_DEG"));
  GroupPoints->TextLabel3->setText(tr("GEOM_FILLING_TOL_2D"));
  GroupPoints->TextLabel4->setText(tr("GEOM_FILLING_NB_ITER"));
  GroupPoints->TextLabel5->setText(tr("GEOM_FILLING_MAX_DEG"));
  GroupPoints->TextLabel6->setText(tr("GEOM_FILLING_TOL_3D"));
  GroupPoints->PushButton1->setPixmap(image1);

  Layout1->addWidget(GroupPoints, 1, 0);
  /***************************************************************/

  /* Initialisations */
  myGenerationGUI = theGenerationGUI;
  Init();
}


//=================================================================================
// function : ~GenerationGUI_FillingDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
GenerationGUI_FillingDlg::~GenerationGUI_FillingDlg()
{
    // no need to delete child widgets, Qt does it all for us
}


//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void GenerationGUI_FillingDlg::Init()
{
  /* init variables */
  myEditCurrentArgument = GroupPoints->LineEdit1;

  myMinDeg = 2;
  myMaxDeg = 5;
  myTol3D = 0.0001;
  myTol2D = 0.0001;
  myNbIter = 5;
  myOkSectionShape = false;

  myCompoundFilter = new GEOM_ShapeTypeFilter(TopAbs_COMPOUND, myGeom);
  mySelection->AddFilter(myCompoundFilter);

  double SpecificStep1 = 1;
  double SpecificStep2 = 0.0001;
  /* min, max, step and decimals for spin boxes & initial values */
  GroupPoints->SpinBox_1->RangeStepAndValidator(2.0, 999.999, SpecificStep1, 3);
  GroupPoints->SpinBox_2->RangeStepAndValidator(0.00001, 10000.0, SpecificStep2, 5);
  GroupPoints->SpinBox_3->RangeStepAndValidator(1.0, 999.999, SpecificStep1, 3);
  GroupPoints->SpinBox_4->RangeStepAndValidator(1.0, 999.999, SpecificStep1, 3);
  GroupPoints->SpinBox_5->RangeStepAndValidator(0.00001, 10000.0, SpecificStep2, 5);

  GroupPoints->SpinBox_1->SetValue(myMinDeg);
  GroupPoints->SpinBox_2->SetValue(myTol2D);
  GroupPoints->SpinBox_3->SetValue(myNbIter);
  GroupPoints->SpinBox_4->SetValue(myMaxDeg);
  GroupPoints->SpinBox_5->SetValue(myTol3D);

  /* signals and slots connections */
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(GroupPoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->LineEdit1, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  
  connect(GroupPoints->SpinBox_1, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupPoints->SpinBox_2, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupPoints->SpinBox_3, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupPoints->SpinBox_4, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));
  connect(GroupPoints->SpinBox_5, SIGNAL(valueChanged(double)), this, SLOT(ValueChangedInSpinBox(double)));

  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupPoints->SpinBox_1, SLOT(SetStep(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupPoints->SpinBox_2, SLOT(SetStep(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupPoints->SpinBox_3, SLOT(SetStep(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupPoints->SpinBox_4, SLOT(SetStep(double)));
  connect(myGeomGUI, SIGNAL(SignalDefaultStepValueChanged(double)), GroupPoints->SpinBox_5, SLOT(SetStep(double)));

  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument())) ;

  /* displays Dialog */
  GroupPoints->show();
  this->show();

  return;
}


//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void GenerationGUI_FillingDlg::ClickOnOk()
{
  this->ClickOnApply();
  ClickOnCancel();
  return;
}


//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
void GenerationGUI_FillingDlg::ClickOnApply()
{
  buttonApply->setFocus();
  QAD_Application::getDesktop()->putInfo(tr(""));
  if (mySimulationTopoDs.IsNull())
    return;
  myGeomBase->EraseSimulationShape();
  mySimulationTopoDs.Nullify();

  if(myOkSectionShape)	  
    myGenerationGUI->MakeFillingAndDisplay(myGeomShape, myMinDeg, myMaxDeg, myTol3D, myTol2D, myNbIter);
  return;
}


//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void GenerationGUI_FillingDlg::SelectionIntoArgument()
{
  myGeomBase->EraseSimulationShape();
  mySimulationTopoDs.Nullify();
  myEditCurrentArgument->setText("");
  QString aString = ""; /* name of selection */
  
  int nbSel = myGeomBase->GetNameOfSelectedIObjects(mySelection, aString);
  if(nbSel != 1) {
    if(myEditCurrentArgument == GroupPoints->LineEdit1)
      myOkSectionShape = false;
    return;
  }
  
  // nbSel == 1
  Standard_Boolean testResult;
  Handle(SALOME_InteractiveObject) IO = mySelection->firstIObject();
  if(!myGeomBase->GetTopoFromSelection(mySelection, mySectionShape))
    return;
  
  if(myEditCurrentArgument == GroupPoints->LineEdit1 && mySectionShape.ShapeType() == TopAbs_COMPOUND) {
    myGeomShape = myGeomBase->ConvertIOinGEOMShape(IO, testResult);
    if(!testResult)
      return;
    // mySectionShape should be a compound of edges
    for ( TopoDS_Iterator it( mySectionShape ); it.More(); it.Next() )
      if ( it.Value().ShapeType() != TopAbs_EDGE )
        return;
    myEditCurrentArgument->setText(aString);
    myOkSectionShape = true;
  }

  if(myOkSectionShape)
    this->MakeFillingSimulationAndDisplay();
  return;
}


//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void GenerationGUI_FillingDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();
  mySelection->ClearFilters();

  if(send == GroupPoints->PushButton1) {
    GroupPoints->LineEdit1->setFocus();
    myEditCurrentArgument = GroupPoints->LineEdit1;
    mySelection->AddFilter(myCompoundFilter);
    this->SelectionIntoArgument();
  }

  return;
}


//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void GenerationGUI_FillingDlg::LineEditReturnPressed()
{  
  QLineEdit* send = (QLineEdit*)sender();
  if(send == GroupPoints->LineEdit1)
    myEditCurrentArgument = GroupPoints->LineEdit1;
  else
    return;

  GEOMBase_Skeleton::LineEditReturnPressed();
  return;
}


//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void GenerationGUI_FillingDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect(mySelection, SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));
  mySelection->AddFilter(myCompoundFilter);
  if(!mySimulationTopoDs.IsNull())
    myGeomBase->DisplaySimulationShape(mySimulationTopoDs);
  return;
}


//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void GenerationGUI_FillingDlg::enterEvent(QEvent* e)
{
  if (GroupConstructors->isEnabled())
    return;
  this->ActivateThisDialog();
  return;
}


//=================================================================================
// function : ValueChangedInSpinBox()
// purpose  :
//=================================================================================
void GenerationGUI_FillingDlg::ValueChangedInSpinBox(double newValue)
{
  QObject* send = (QObject*)sender();

  if(send == GroupPoints->SpinBox_1)
    myMinDeg = int(newValue);
  else if(send == GroupPoints->SpinBox_2)
    myTol2D = newValue;
  else if(send == GroupPoints->SpinBox_3)
    myNbIter = int(newValue);
  else if(send == GroupPoints->SpinBox_4)
    myMaxDeg = int(newValue);
  else if(send == GroupPoints->SpinBox_5)
    myTol3D = newValue;

  if(myOkSectionShape)
    this->MakeFillingSimulationAndDisplay();
  return;
}


//=================================================================================
// function : MakeFillingSimulationAndDisplay()
// purpose  :
//=================================================================================
void GenerationGUI_FillingDlg::MakeFillingSimulationAndDisplay()
{
  QAD_WaitCursor wc;

  myGeomBase->EraseSimulationShape();
  mySimulationTopoDs.Nullify();

  try {
    /* we verify the contents of the shape */
    TopExp_Explorer Ex;
    TopoDS_Shape Scurrent;	
    Standard_Real First, Last;
    Handle(Geom_Curve) C;
    GeomFill_SectionGenerator Section;
    
    Standard_Integer i = 0;
    for(Ex.Init(mySectionShape, TopAbs_EDGE); Ex.More(); Ex.Next()) {
      Scurrent = Ex.Current();
      if( Scurrent.IsNull() || Scurrent.ShapeType() != TopAbs_EDGE)
	return;
      C = BRep_Tool::Curve(TopoDS::Edge(Scurrent), First, Last);
      if (C.IsNull()) continue;
      C = new Geom_TrimmedCurve(C, First, Last);
      Section.AddCurve(C) ;
      i++ ;
    }
    
    /* a 'tolerance' is used to compare 2 knots : see GeomFill_Generator.cdl */
    /* We set 'tolerance' = tol3d                                            */
    // Section.Perform( tol3d ) ; NRI */
    Section.Perform(Precision::Confusion());
    Handle(GeomFill_Line) Line = new GeomFill_Line(i);

    GeomFill_AppSurf App(myMinDeg, myMaxDeg, myTol3D, myTol2D, myNbIter) ; /* user parameters */
    App.Perform(Line, Section);
    
    if(!App.IsDone())
      return;

    Standard_Integer UDegree, VDegree, NbUPoles, NbVPoles, NbUKnots, NbVKnots;
    App.SurfShape(UDegree, VDegree, NbUPoles, NbVPoles, NbUKnots, NbVKnots);	
    Handle(Geom_BSplineSurface) GBS = new Geom_BSplineSurface(App.SurfPoles(), App.SurfWeights(), App.SurfUKnots(), App.SurfVKnots(), App.SurfUMults(), App.SurfVMults(), App.UDegree(), App.VDegree());
    
    if(GBS.IsNull())
      return;
    mySimulationTopoDs  = BRepBuilderAPI_MakeFace(GBS);    
    if(mySimulationTopoDs.IsNull())
      return;
    else
      myGeomBase->DisplaySimulationShape(mySimulationTopoDs); 
  }
  catch(Standard_Failure) {
    MESSAGE("Exception catched in MakePrismSimulationAndDisplay" << endl);
    return;
  }
  return;
}
