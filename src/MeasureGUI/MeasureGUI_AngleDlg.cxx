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
// File   : MeasureGUI_AngleDlg.cxx
// Author : Julia DOROVSKIKH, Open CASCADE S.A.S.

#include "MeasureGUI_AngleDlg.h"
#include "MeasureGUI_Widgets.h"

#include <DlgRef.h>
#include <GEOMBase.h>
#include <GeometryGUI.h>

#include <Basics_OCCTVersion.hxx>

#include <SUIT_Session.h>
#include <SUIT_Desktop.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_MessageBox.h>
#include <SUIT_ViewWindow.h>
#include <SUIT_ViewManager.h>
#include <SOCC_Prs.h>
#include <SOCC_ViewModel.h>
#include <SalomeApp_Tools.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>

// OCCT Includes
#include <AIS_AngleDimension.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <Geom_Plane.hxx>
#include <gce_MakePln.hxx>
#include <Precision.hxx>
#include <AIS.hxx>
#include <Prs3d_LineAspect.hxx> 
#include <IntTools_EdgeEdge.hxx>
#include <IntTools_SequenceOfCommonPrts.hxx>
#include <IntTools_CommonPrt.hxx>

// QT Includes
#include <qlineedit.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>

#include "GEOMImpl_Types.hxx"

#include "utilities.h"

#include <Standard_Failure.hxx>
#include <Standard_ErrorHandler.hxx> // CAREFUL ! position of this file is critic : see Lucien PIGNOLONI / OCC

//=================================================================================
// class    : MeasureGUI_AngleDlg()
// purpose  : Constructs a MeasureGUI_AngleDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
MeasureGUI_AngleDlg::MeasureGUI_AngleDlg (GeometryGUI* GUI, QWidget* parent)
  : MeasureGUI_Skeleton(GUI, parent)
{
  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
  QPixmap image0 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_ANGLE")));
  QPixmap image1 (aResMgr->loadPixmap("GEOM", tr("ICON_SELECT")));

  setWindowTitle(tr("GEOM_MEASURE_ANGLE_TITLE"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_MEASURE_ANGLE_ANGLE"));
  mainFrame()->RadioButton1->setIcon(image0);

  myGrp = new MeasureGUI_2Sel1LineEdit(centralWidget());
  myGrp->GroupBox1->setTitle(tr("GEOM_MEASURE_ANGLE_OBJ"));
  myGrp->TextLabel1->setText(tr("GEOM_OBJECT_I").arg("1"));
  myGrp->TextLabel2->setText(tr("GEOM_OBJECT_I").arg("2"));
  myGrp->TextLabel3->setText(tr(aResMgr->stringValue ( "Geometry", "dimensions_angle_units", "deg" ) == "deg" ? "GEOM_MEASURE_ANGLE_DEG" : "GEOM_MEASURE_ANGLE_RAD"));
  myGrp->LineEdit3->setReadOnly(true);
  myGrp->PushButton1->setIcon(image1);
  myGrp->PushButton2->setIcon(image1);
  myGrp->LineEdit1->setReadOnly(true);
  myGrp->LineEdit2->setReadOnly(true);

  QVBoxLayout* layout = new QVBoxLayout(centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
  layout->addWidget(myGrp);
  /***************************************************************/

  // Help page reference
  myHelpFileName = "angle_page.html";

  // Initialisation
  Init();
}

//=================================================================================
// function : ~MeasureGUI_AngleDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
MeasureGUI_AngleDlg::~MeasureGUI_AngleDlg()
{
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void MeasureGUI_AngleDlg::Init()
{
  // init variables
  myGrp->LineEdit1->setText("");
  myGrp->LineEdit2->setText("");
  myGrp->LineEdit1->setEnabled(true);
  myGrp->LineEdit2->setEnabled(false);
  myObj.nullify();
  myObj2.nullify();

  mySelBtn   = myGrp->PushButton1;
  mySelEdit  = myGrp->LineEdit1;
  mySelBtn2  = myGrp->PushButton2;
  mySelEdit2 = myGrp->LineEdit2;

  myEditCurrentArgument = mySelEdit;

  // signals and slots connections
  connect(mySelEdit2, SIGNAL(returnPressed()), this, SLOT(LineEditReturnPressed()));
  connect(mySelBtn2,  SIGNAL(clicked()),       this, SLOT(SetEditCurrentArgument()));

  activateSelection();  
  MeasureGUI_Skeleton::Init();
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection has changed
//=================================================================================
void MeasureGUI_AngleDlg::SelectionIntoArgument()
{
  GEOM::GeomObjPtr aSelectedObject = getSelected( TopAbs_EDGE );

  // clear selection
  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
  myGeomGUI->getApp()->selectionMgr()->clearSelected();
  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));

  if (myEditCurrentArgument == mySelEdit) {
    myObj = aSelectedObject;
    if (myObj && !myObj2)
      myGrp->PushButton2->click();
  }
  else {
    myObj2 = aSelectedObject;
    if (myObj2 && !myObj)
      myGrp->PushButton1->click();
  }

  processObject();
}

//=================================================================================
// function : processObject()
// purpose  : Fill dialogs fields in accordance with myObj and myObj2
//=================================================================================
void MeasureGUI_AngleDlg::processObject()
{
  myGrp->LineEdit1->setText(myObj  ? GEOMBase::GetName(myObj.get()) : "");
  myGrp->LineEdit2->setText(myObj2 ? GEOMBase::GetName(myObj2.get()) : "");

  double anAngle = 0.;
  if (getParameters(anAngle)) {
    // To avoid the using different angle values in viewer and dialog,
    // the value from presentation is used in both cases (see buildPrs())
    redisplayPreview();
  }
  else {
    myGrp->LineEdit3->setText("");
    erasePreview();
  }
}

//=================================================================================
// function : getParameters()
// purpose  : Get angle between objects
//=================================================================================
bool MeasureGUI_AngleDlg::getParameters (double& theAngle)
{
  QString msg;
  if (isValid(msg)) {
    GEOM::GEOM_IMeasureOperations_var anOper = GEOM::GEOM_IMeasureOperations::_narrow( getOperation() );
    try {
      theAngle = anOper->GetAngle(myObj.get(), myObj2.get());
    }
    catch(const SALOME::SALOME_Exception& e) {
      SalomeApp_Tools::QtCatchCorbaException(e);
      return false;
    }

    bool isDone = anOper->IsDone();
    if (!isDone) {
      CORBA::String_var aMsg = anOper->GetErrorCode();
      SUIT_MessageBox::warning(this,
                               QObject::tr("WRN_WARNING"),
                               QObject::tr(aMsg.in()));
    }
    return isDone;
  }

  return false;
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void MeasureGUI_AngleDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  if (send == myGrp->PushButton1) {
    myEditCurrentArgument = myGrp->LineEdit1;

    myGrp->PushButton2->setDown(false);
    myGrp->LineEdit2->setEnabled(false);
  }
  else {
    myEditCurrentArgument = myGrp->LineEdit2;

    myGrp->PushButton1->setDown(false);
    myGrp->LineEdit1->setEnabled(false);
  }

  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
  activateSelection();
  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));

  // enable line edit
  myEditCurrentArgument->setEnabled(true);
  myEditCurrentArgument->setFocus();
  // after setFocus(), because it will be setDown(false) when loses focus
  send->setDown(true);

  // seems we need it only to avoid preview disappearing, caused by selection mode change
  redisplayPreview();
}

//=================================================================================
// function : LineEditReturnPressed()
// purpose  :
//=================================================================================
void MeasureGUI_AngleDlg::LineEditReturnPressed()
{
  QLineEdit* send = (QLineEdit*)sender();

  if (send == mySelEdit)
    myEditCurrentArgument = mySelEdit;
  else
    myEditCurrentArgument = mySelEdit2;

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  if (GEOMBase::SelectionByNameInDialogs(this, mySelEdit->text(), aSelList))
    mySelEdit->setText(mySelEdit->text());
}

//=================================================================================
// function : buildPrs()
// purpose  :
//=================================================================================
SALOME_Prs* MeasureGUI_AngleDlg::buildPrs()
{
  double anAngle = 0.;
  myGrp->LineEdit3->setText("");

  SUIT_ViewWindow* vw = SUIT_Session::session()->activeApplication()->desktop()->activeWindow();

  if (!myObj || !myObj2 || !getParameters(anAngle) ||
       vw->getViewManager()->getType() != OCCViewer_Viewer::Type())
    return 0;

  if (anAngle > Precision::Angular()) {
    try {
      OCC_CATCH_SIGNALS;
      TopoDS_Shape S1, S2;
      if (GEOMBase::GetShape(myObj.get() , S1, TopAbs_EDGE) &&
        GEOMBase::GetShape(myObj2.get(), S2, TopAbs_EDGE)) {
        TopoDS_Edge anEdge1 = TopoDS::Edge(S1);
        TopoDS_Edge anEdge2 = TopoDS::Edge(S2);

        Handle(AIS_AngleDimension) anIO = new AIS_AngleDimension( anEdge1, anEdge2 );

        Handle(Prs3d_DimensionAspect) aDimensionStyle = new Prs3d_DimensionAspect;

        Handle(Geom_Line) geom_lin1,geom_lin2;
        gp_Pnt ptat11,ptat12,ptat21,ptat22;
        Standard_Boolean isInfinite1,isInfinite2;
        Handle(Geom_Curve) extCurv;
        Standard_Integer extShape;
        Handle(Geom_Plane) aPlane;
        if (AIS::ComputeGeometry(anEdge1,
                                  anEdge2,
                                  extShape,
                                  geom_lin1,
                                  geom_lin2,
                                  ptat11,
                                  ptat12,
                                  ptat21,
                                  ptat22,
                                  extCurv,
                                  isInfinite1,
                                  isInfinite2,
                                  aPlane)) {
          Standard_Real arrSize1 = aDimensionStyle->ArrowAspect()->Length();
          Standard_Real arrSize2 = aDimensionStyle->ArrowAspect()->Length();
          if (!isInfinite1) arrSize1 = ptat11.Distance(ptat12)/10.;
          if (!isInfinite2) arrSize2 = ptat21.Distance(ptat22)/10.;
          aDimensionStyle->ArrowAspect()->SetLength( Max(arrSize1, arrSize2) );
        }

        SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
        int w = resMgr->integerValue( "Geometry", "measures_line_width", 1 );
        QString aUnitsAngle = resMgr->stringValue ( "Geometry", "dimensions_angle_units", "deg" );

        aDimensionStyle->LineAspect()->SetWidth( w );
        aDimensionStyle->SetTextHorizontalPosition( Prs3d_DTHP_Center );
        aDimensionStyle->SetTextVerticalPosition( Prs3d_DTVP_Center );
        aDimensionStyle->MakeText3d( Standard_False );
        aDimensionStyle->MakeArrows3d( Standard_True );

        anIO->SetDimensionAspect( aDimensionStyle );
        anIO->SetDisplayUnits( aUnitsAngle.toLatin1().data() );
        if (aUnitsAngle == "rad")
          anIO->SetDisplaySpecialSymbol(AIS_DSS_No);

        SOCC_Prs* aPrs =
          dynamic_cast<SOCC_Prs*>(((SOCC_Viewer*)(vw->getViewManager()->getViewModel()))->CreatePrs(0));

        if (aPrs)
          aPrs->AddObject(anIO);

        // set angle value into dialog
        double anAngle = anIO->GetValue();
        QString anAngleLabel = "GEOM_MEASURE_ANGLE_RAD";
        if (aUnitsAngle == "deg") {
          // using degrees instead of radians
          anAngle *= 180. / M_PI;
          anAngleLabel = "GEOM_MEASURE_ANGLE_DEG";
        }
        myGrp->TextLabel3->setText(tr(anAngleLabel.toLatin1().data()));
        int aPrecision = resMgr->integerValue( "Geometry", "angle_precision", 6 );
        myGrp->LineEdit3->setText(DlgRef::PrintDoubleValue(anAngle, aPrecision));

        return aPrs;
      }
    }
    catch(Standard_Failure) {
    }
  }

  return 0;
}

//=================================================================================
// function : isValid()
// purpose  :
//=================================================================================
bool MeasureGUI_AngleDlg::isValid (QString& msg)
{
  return MeasureGUI_Skeleton::isValid(msg) && myObj2;
}

//=================================================================================
// function : activateSelection()
// purpose  :
//=================================================================================
void MeasureGUI_AngleDlg::activateSelection()
{
  globalSelection(GEOM_LINE);
  localSelection( TopAbs_EDGE );
}
