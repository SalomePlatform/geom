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

// GEOM GEOMGUI : GUI for Geometry component
// File   : MeasureGUI_DistanceDlg.cxx
// Author : Nicolas REJNERI, Open CASCADE S.A.S.

#include "MeasureGUI_DistanceDlg.h"
#include "MeasureGUI_Widgets.h"

#include <DlgRef.h>
#include <GEOMBase.h>
#include <GeometryGUI.h>

#include <SUIT_Session.h>
#include <SUIT_Desktop.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_ViewWindow.h>
#include <SUIT_ViewManager.h>
#include <SUIT_OverrideCursor.h>
#include <SUIT_MessageBox.h>
#include <SOCC_Prs.h>
#include <SOCC_ViewModel.h>
#include <SalomeApp_Tools.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>

// OCCT Includes
#include <Geom_Plane.hxx>
#include <AIS_LengthDimension.hxx>
#include <Prs3d_LineAspect.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <gce_MakePln.hxx>
#include <Precision.hxx>

//=================================================================================
// class    : MeasureGUI_DistanceDlg()
// purpose  : Constructs a MeasureGUI_DistanceDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            true to construct a modal dialog.
//=================================================================================
MeasureGUI_DistanceDlg::MeasureGUI_DistanceDlg (GeometryGUI* GUI, QWidget* parent)
  : GEOMBase_Skeleton(GUI, parent)
{
  SUIT_ResourceMgr* aResMgr = SUIT_Session::session()->resourceMgr();
  QPixmap image0 (aResMgr->loadPixmap("GEOM", tr("ICON_DLG_MINDIST")));
  QPixmap image1 (aResMgr->loadPixmap("GEOM", tr("ICON_SELECT")));

  setWindowTitle(tr("GEOM_MINDIST_TITLE"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_DISTANCE"));
  mainFrame()->RadioButton1->setIcon(image0);
  mainFrame()->RadioButton2->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton3->close();

  mainFrame()->GroupBoxName->hide();

  myGrp = new MeasureGUI_DistanceGroup (centralWidget());
  myGrp->GroupBox1->setTitle(tr("GEOM_MINDIST_OBJ"));

  // Arguments
  myGrp->TextLabel1->setText(tr("GEOM_OBJECT_I").arg("1"));
  myGrp->TextLabel2->setText(tr("GEOM_OBJECT_I").arg("2"));
  myGrp->PushButton1->setIcon(image1);
  myGrp->PushButton2->setIcon(image1);
  myGrp->LineEdit1->setReadOnly(true);
  myGrp->LineEdit2->setReadOnly(true);

  // Solutions combobox
  myGrp->TextLabel7->setText(tr("GEOM_SOLUTION"));

  // Distance, dx, dy and dz
  myGrp->TextLabel3->setText(tr("GEOM_LENGTH"));
  myGrp->TextLabel4->setText(tr("GEOM_DX"));
  myGrp->TextLabel5->setText(tr("GEOM_DY"));
  myGrp->TextLabel6->setText(tr("GEOM_DZ"));
  myGrp->LineEdit3->setReadOnly(true);
  myGrp->LineEdit4->setReadOnly(true);
  myGrp->LineEdit5->setReadOnly(true);
  myGrp->LineEdit6->setReadOnly(true);
  /***************************************************************/

  myHelpFileName = "min_distance_page.html";

  // Initialisation
  Init();
}

//=================================================================================
// function : ~MeasureGUI_DistanceDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
MeasureGUI_DistanceDlg::~MeasureGUI_DistanceDlg()
{
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void MeasureGUI_DistanceDlg::Init()
{
  myEditCurrentArgument = myGrp->LineEdit1;
  myDbls = new GEOM::ListOfDouble();

  myGrp->LineEdit1->setEnabled(true);
  myGrp->LineEdit2->setEnabled(false);

  myObj1.nullify();
  myObj2.nullify();

  // signals and slots connections
  connect(buttonOk(),         SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(),      SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(myGrp->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(myGrp->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(myGrp->ComboBox1, SIGNAL(currentIndexChanged(int)), this, SLOT(SolutionSelected(int)));

  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));

  activateSelection();
  SelectionIntoArgument();
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void MeasureGUI_DistanceDlg::ClickOnOk()
{
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool MeasureGUI_DistanceDlg::ClickOnApply()
{
  if (!onAccept())
    return false;

  initName();
  activateSelection();
  return true;
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void MeasureGUI_DistanceDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();

  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));

  activateSelection();
  redisplayPreview();
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void MeasureGUI_DistanceDlg::enterEvent(QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : SolutionSelected()
// purpose  : Called when ComboBox selection has changed
//=================================================================================
void MeasureGUI_DistanceDlg::SolutionSelected (int i)
{
  if (i < 0 || myDbls->length() < (i+1)*6) {
    myGrp->LineEdit3->setText("");
    myGrp->LineEdit4->setText("");
    myGrp->LineEdit5->setText("");
    myGrp->LineEdit6->setText("");
    erasePreview();
    return;
  }

  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  int aPrecision = resMgr->integerValue( "Geometry", "length_precision", 6 );

  gp_Pnt p1 (myDbls[i*6 + 0], myDbls[i*6 + 1], myDbls[i*6 + 2]);
  gp_Pnt p2 (myDbls[i*6 + 3], myDbls[i*6 + 4], myDbls[i*6 + 5]);

  double aDist = p1.Distance(p2);
  myGrp->LineEdit3->setText(DlgRef::PrintDoubleValue(aDist, aPrecision));

  gp_XYZ aVec = p2.XYZ() - p1.XYZ();
  myGrp->LineEdit4->setText(DlgRef::PrintDoubleValue(aVec.X(), aPrecision));
  myGrp->LineEdit5->setText(DlgRef::PrintDoubleValue(aVec.Y(), aPrecision));
  myGrp->LineEdit6->setText(DlgRef::PrintDoubleValue(aVec.Z(), aPrecision));

  redisplayPreview();
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection has changed
//=================================================================================
void MeasureGUI_DistanceDlg::SelectionIntoArgument()
{
  QList<TopAbs_ShapeEnum> aTypes;
  aTypes << TopAbs_VERTEX << TopAbs_EDGE << TopAbs_WIRE << TopAbs_FACE << TopAbs_SHELL << TopAbs_SOLID << TopAbs_COMPSOLID << TopAbs_COMPOUND << TopAbs_SHAPE;
  GEOM::GeomObjPtr aSelectedObject = getSelected( aTypes );

  // clear selection
  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
  myGeomGUI->getApp()->selectionMgr()->clearSelected();
  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));

  if (myEditCurrentArgument == myGrp->LineEdit1) {
    myObj1 = aSelectedObject;
    if (myObj1 && !myObj2)
      myGrp->PushButton2->click();
  }
  else {
    myObj2 = aSelectedObject;
    if (myObj2 && !myObj1)
      myGrp->PushButton1->click();
  }

  processObject();
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void MeasureGUI_DistanceDlg::SetEditCurrentArgument()
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

  // enable line edit
  myEditCurrentArgument->setEnabled(true);
  myEditCurrentArgument->setFocus();
  // after setFocus(), because it will be setDown(false) when loses focus
  send->setDown(true);

  // seems we need it only to avoid preview disappearing, caused by selection mode change
  redisplayPreview();
}

//=================================================================================
// function : processObject()
// purpose  : Fill dialogs fields in accordance with myObj1 and myObj2
//=================================================================================
void MeasureGUI_DistanceDlg::processObject()
{
  myGrp->LineEdit1->setText(myObj1 ? GEOMBase::GetName(myObj1.get()) : "");
  myGrp->LineEdit2->setText(myObj2 ? GEOMBase::GetName(myObj2.get()) : "");

  myGrp->ComboBox1->clear();
  myDbls->length(0);
  erasePreview();

  int nbSols = 0;

  QString msg;
  if (!isValid(msg)) return;

  GEOM::GEOM_IMeasureOperations_var anOper = GEOM::GEOM_IMeasureOperations::_narrow(getOperation());
  try {
    nbSols = anOper->ClosestPoints(myObj1.get(), myObj2.get(), myDbls);
  }
  catch (const SALOME::SALOME_Exception& e) {
    SalomeApp_Tools::QtCatchCorbaException(e);
    return;
  }

  if (!anOper->IsDone())
    myGrp->ComboBox1->addItem(tr(anOper->GetErrorCode()));
  else if (nbSols <= 0)
    myGrp->ComboBox1->addItem(tr("GEOM_MINDIST_NO_SOL"));
  else {
    for (int i = 0; i < nbSols; i++) {
      myGrp->ComboBox1->addItem(tr("GEOM_SOLUTION_I").arg(i + 1));
    }
    myGrp->ComboBox1->setCurrentIndex(0);
  }
}

//=================================================================================
// function : buildPrs()
// purpose  :
//=================================================================================
SALOME_Prs* MeasureGUI_DistanceDlg::buildPrs()
{
  SUIT_ViewWindow* vw = SUIT_Session::session()->activeApplication()->desktop()->activeWindow();

  int currSol = myGrp->ComboBox1->currentIndex();

  if (!myObj1 || !myObj2 ||
      currSol == -1 || (currSol+1)*6 > myDbls->length() ||
      vw->getViewManager()->getType() != OCCViewer_Viewer::Type())
    return 0;

  gp_Pnt aPnt1 (myDbls[currSol*6 + 0], myDbls[currSol*6 + 1], myDbls[currSol*6 + 2]);
  gp_Pnt aPnt2 (myDbls[currSol*6 + 3], myDbls[currSol*6 + 4], myDbls[currSol*6 + 5]);

  double aDist = aPnt1.Distance(aPnt2);

  try
  {
    if (aDist <= 1.e-9) {
      BRepBuilderAPI_MakeVertex aMaker (aPnt1);
      return getDisplayer()->BuildPrs(aMaker.Vertex());
    }
    else {
      BRepBuilderAPI_MakeEdge MakeEdge (aPnt1, aPnt2);
      TopoDS_Vertex aVert1 = BRepBuilderAPI_MakeVertex(aPnt1);
      TopoDS_Vertex aVert2 = BRepBuilderAPI_MakeVertex(aPnt2);

      //QString aLabel;
      //aLabel.sprintf("%.1f", aDist);

      gp_Pnt aPnt3 ((aPnt1.X() + aPnt2.X()) / 2,
                    (aPnt1.Y() + aPnt2.Y()) / 2,
                    (aPnt1.Z() + aPnt2.Z()) / 2);

      gp_Vec va (aPnt3, aPnt1);
      gp_Vec vb (aPnt3, aPnt2);

      if (va.IsParallel(vb, Precision::Angular())) {
        aPnt3.SetY((aPnt1.Y() + aPnt2.Y()) / 2 + 100);
        aPnt3.SetZ((aPnt1.Z() + aPnt2.Z()) / 2);
      }

      gce_MakePln gce_MP (aPnt1, aPnt2, aPnt3);
      Handle(Geom_Plane) P = new Geom_Plane (gce_MP.Value());

      Handle(AIS_LengthDimension) anIO = new AIS_LengthDimension( aVert1, aVert2, P->Pln() );

      SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
      int w = resMgr->integerValue( "Geometry", "measures_line_width", 1 );

      Handle(Prs3d_DimensionAspect) aDimensionStyle = new Prs3d_DimensionAspect;

      aDimensionStyle->ArrowAspect()->SetLength( aDist / 20.0 );
      aDimensionStyle->LineAspect()->SetWidth( w );
      aDimensionStyle->SetTextHorizontalPosition( Prs3d_DTHP_Center );
      aDimensionStyle->SetTextVerticalPosition( Prs3d_DTVP_Center );
      aDimensionStyle->MakeText3d( Standard_False );
      aDimensionStyle->MakeArrows3d( Standard_True );

      anIO->SetFlyout( 0.0 );
      anIO->SetDimensionAspect( aDimensionStyle );

      SOCC_Prs* aPrs = dynamic_cast<SOCC_Prs*>(((SOCC_Viewer*)(vw->getViewManager()->getViewModel()))->CreatePrs(0));

      if (aPrs)
        aPrs->AddObject(anIO);

      return aPrs;
    }
  }
  catch (Standard_Failure) {
    return 0;
  }
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr MeasureGUI_DistanceDlg::createOperation()
{
  return getGeomEngine()->GetIMeasureOperations(getStudyId());
}

//=================================================================================
// function : isValid()
// purpose  :
//=================================================================================
bool MeasureGUI_DistanceDlg::isValid (QString& msg)
{
  return myObj1 && myObj2;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool MeasureGUI_DistanceDlg::execute (ObjectList& objects)
{
  GEOM::GEOM_IMeasureOperations_var anOper = GEOM::GEOM_IMeasureOperations::_narrow(getOperation());
  GEOM::GEOM_IBasicOperations_var aBasicOper = getGeomEngine()->GetIBasicOperations(getStudyId());

  GEOM::ListOfDouble_var aDbls;
  int nbSols = anOper->ClosestPoints(myObj1.get(), myObj2.get(), aDbls);

  if (anOper->IsDone()) {
    bool doPublishAll = true;
    if (nbSols > 1) {
      QMessageBox::StandardButton anAnswer =
        SUIT_MessageBox::question(this, tr("GEOM_MINDIST_PUBLISH_TITLE"),
                                  tr("GEOM_MINDIST_PUBLISH_TEXT"),
                                  QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
                                  QMessageBox::No);
      if (anAnswer == QMessageBox::No)
        doPublishAll = false;
      else if (anAnswer != QMessageBox::Yes)
        return true;
    }
    if (doPublishAll) {
      for (int i = 0; i < nbSols; i++) {
        GEOM::GEOM_Object_var anObj1 =
          aBasicOper->MakePointXYZ(aDbls[i*6 + 0], aDbls[i*6 + 1], aDbls[i*6 + 2]);
        GEOM::GEOM_Object_var anObj2 =
          aBasicOper->MakePointXYZ(aDbls[i*6 + 3], aDbls[i*6 + 4], aDbls[i*6 + 5]);

        if (!anObj1->_is_nil() && !anObj2->_is_nil()) {
          objects.push_back(anObj1._retn());
          objects.push_back(anObj2._retn());
        }
      }
    }
    else {
      int i = myGrp->ComboBox1->currentIndex();
      GEOM::GEOM_Object_var anObj1 =
        aBasicOper->MakePointXYZ(aDbls[i*6 + 0], aDbls[i*6 + 1], aDbls[i*6 + 2]);
      GEOM::GEOM_Object_var anObj2 =
        aBasicOper->MakePointXYZ(aDbls[i*6 + 3], aDbls[i*6 + 4], aDbls[i*6 + 5]);

      if (!anObj1->_is_nil() && !anObj2->_is_nil()) {
        objects.push_back(anObj1._retn());
        objects.push_back(anObj2._retn());
      }
    }
  }

  return true;
}

//=================================================================================
// function : activateSelection()
// purpose  :
//=================================================================================
void MeasureGUI_DistanceDlg::activateSelection()
{
  globalSelection( GEOM_ALLSHAPES );
  localSelection( TopAbs_SHAPE );
}

//=================================================================================
// function : redisplayPreview()
// purpose  :
//=================================================================================
void MeasureGUI_DistanceDlg::redisplayPreview()
{
  QString aMess;
  if (!isValid(aMess)) {
    erasePreview(true);
    return;
  }

  erasePreview(false);

  try {
    SUIT_OverrideCursor wc;

    getDisplayer()->SetColor(Quantity_NOC_VIOLET);
    getDisplayer()->SetToActivate(false);
    
    if (SALOME_Prs* aPrs = buildPrs())
      displayPreview(aPrs);
  }
  catch (const SALOME::SALOME_Exception& e) {
    SalomeApp_Tools::QtCatchCorbaException(e);
  }
}

//================================================================
// Function : getNewObjectName
// Purpose  : Redefine this method to return proper name for a new object
//================================================================
QString MeasureGUI_DistanceDlg::getNewObjectName (int currObj) const
{
  QString aName = tr("GEOM_MINDIST_NAME") + QString("_%1_").arg((currObj+1)/2);
  aName += GEOMBase::GetName(currObj%2 ? myObj1.get() : myObj2.get());

  return aName;
}

//=================================================================================
// function : addSubshapeToStudy
// purpose  : virtual method to add new SubObjects if local selection
//=================================================================================
void MeasureGUI_DistanceDlg::addSubshapesToStudy()
{
  GEOMBase::PublishSubObject( myObj1.get() );
  GEOMBase::PublishSubObject( myObj2.get() );
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> MeasureGUI_DistanceDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res;
  res << myObj1 << myObj2;
  return res;
}

//=================================================================================
// function : MeasureGUI_DistanceGroup
// purpose  :
//=================================================================================
MeasureGUI_DistanceGroup::MeasureGUI_DistanceGroup (QWidget *parent)
{
  gridLayout = new QGridLayout (parent);
  gridLayout->setSpacing(6);
  gridLayout->setContentsMargins(11, 11, 11, 11);
  gridLayout->setHorizontalSpacing(0);
  gridLayout->setVerticalSpacing(0);
  gridLayout->setContentsMargins(0, 0, 0, 0);

  GroupBox1 = new QGroupBox (parent);

  gridLayout1 = new QGridLayout (GroupBox1);
  gridLayout1->setSpacing(6);
  gridLayout1->setContentsMargins(11, 11, 11, 11);
  gridLayout1->setHorizontalSpacing(6);
  gridLayout1->setVerticalSpacing(6);
  gridLayout1->setContentsMargins(9, 9, 9, 9);

  // 2Sel
  TextLabel1 = new QLabel(GroupBox1);
  TextLabel2 = new QLabel(GroupBox1);

  PushButton1 = new QPushButton (GroupBox1);
  PushButton2 = new QPushButton (GroupBox1);

  LineEdit1 = new QLineEdit(GroupBox1);
  LineEdit2 = new QLineEdit(GroupBox1);

  gridLayout1->addWidget(TextLabel1, 0, 0, 1, 1);
  gridLayout1->addWidget(TextLabel2, 1, 0, 1, 1);
  gridLayout1->addWidget(PushButton1, 0, 1, 1, 1);
  gridLayout1->addWidget(PushButton2, 1, 1, 1, 1);
  gridLayout1->addWidget(LineEdit1, 0, 2, 1, 1);
  gridLayout1->addWidget(LineEdit2, 1, 2, 1, 1);

  // 1Combo
  TextLabel7 = new QLabel (GroupBox1);

  ComboBox1 = new QComboBox (GroupBox1);

  gridLayout1->addWidget(TextLabel7, 2, 0, 1, 1);
  gridLayout1->addWidget(ComboBox1, 2, 1, 1, 2);

  // 4Text
  TextLabel3 = new QLabel (GroupBox1);
  TextLabel4 = new QLabel (GroupBox1);
  TextLabel5 = new QLabel (GroupBox1);
  TextLabel6 = new QLabel (GroupBox1);

  LineEdit3 = new QLineEdit(GroupBox1);
  LineEdit4 = new QLineEdit(GroupBox1);
  LineEdit5 = new QLineEdit(GroupBox1);
  LineEdit6 = new QLineEdit(GroupBox1);

  gridLayout1->addWidget(TextLabel3, 3, 0, 1, 1);
  gridLayout1->addWidget(TextLabel4, 4, 0, 1, 1);
  gridLayout1->addWidget(TextLabel5, 5, 0, 1, 1);
  gridLayout1->addWidget(TextLabel6, 6, 0, 1, 1);

  gridLayout1->addWidget(LineEdit3, 3, 1, 1, 2);
  gridLayout1->addWidget(LineEdit4, 4, 1, 1, 2);
  gridLayout1->addWidget(LineEdit5, 5, 1, 1, 2);
  gridLayout1->addWidget(LineEdit6, 6, 1, 1, 2);

  gridLayout->addWidget(GroupBox1, 0, 0, 1, 1);
}

//=================================================================================
// function : ~MeasureGUI_DistanceGroup()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
MeasureGUI_DistanceGroup::~MeasureGUI_DistanceGroup()
{
  // no need to delete child widgets, Qt does it all for us
}
