// Copyright (C) 2007-2013  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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

#include "EntityGUI_PolylineDlg.h"
#include <CurveCreator_Curve.hxx>
#include <CurveCreator_Utils.hxx>
#include <CurveCreator_Widget.h>
#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <OCCViewer_ViewManager.h>
#include <LightApp_SelectionMgr.h>
#include <SalomeApp_Application.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>

#include <BRep_Tool.hxx>
#include <Geom_Surface.hxx>
#include <GeomLib_IsPlanarSurface.hxx>
#include <TopoDS.hxx>

#include <QGroupBox>
#include <QVBoxLayout>

//#define SET_PLANE

//=================================================================================
// function : Constructor
// purpose  :
//=================================================================================
EntityGUI_PolylineDlg::EntityGUI_PolylineDlg
        (GeometryGUI* theGeometryGUI, QWidget* parent, bool modal, Qt::WindowFlags fl)
  : GEOMBase_Skeleton( theGeometryGUI, parent, modal, fl ),
    myCurve               (0),
    myEditorWidget        (0),
    myAddElementBox       (0),
    myPlnComboBox         (0),
    myPlnButton           (0),
    myPlnSelButton        (0),
    myWPlaneLineEdit      (0),
    myPolylineSelButton   (0),
    myPolylineEdit        (0),
    myEditCurrentArgument (0)
{
  QPixmap image0(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_CC_POLYLINE")));
  QPixmap image1(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_SELECT")));

  setWindowTitle(tr("POLYLINE_DLG_TITLE"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("POLYLINE_TITLE"));
  mainFrame()->RadioButton1->setIcon(image0);
  mainFrame()->RadioButton2->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton2->close();
  mainFrame()->RadioButton3->setAttribute( Qt::WA_DeleteOnClose );
  mainFrame()->RadioButton3->close();

  QGroupBox   *aGroupBox1 = new QGroupBox(tr("GEOM_CS"), this);
  QGridLayout *aPlaneLayout = new QGridLayout(aGroupBox1);

  aPlaneLayout->setSpacing(6);
  aPlaneLayout->setMargin(11);

  myPlnComboBox = new QComboBox(aGroupBox1);
  aPlaneLayout->addWidget(myPlnComboBox, 0, 0, 1, 3);

  myPlnButton = new QPushButton (aGroupBox1);
  myPlnButton->setText( tr( "GEOM_SKETCHER_RESTORE" ) );
  aPlaneLayout->addWidget(myPlnButton, 0, 3);

#ifdef SET_PLANE
  QLabel *aPlaneLbl = new QLabel(tr("GEOM_PLANE"), aGroupBox1);

  myPlnSelButton = new QPushButton (aGroupBox1);
  myPlnSelButton->setIcon(image1);
  myWPlaneLineEdit = new QLineEdit (aGroupBox1);
  myWPlaneLineEdit->setReadOnly(true);
#endif

  QLabel *aPolylineLbl = new QLabel(tr("POLYLINE_IMPORT"), aGroupBox1);

  myPolylineSelButton = new QPushButton (aGroupBox1);
  myPolylineSelButton->setIcon(image1);
  myPolylineEdit = new QLineEdit (aGroupBox1);
  myPolylineEdit->setReadOnly(true);

#ifdef SET_PLANE
  aPlaneLayout->addWidget(aPlaneLbl, 1, 0);
  aPlaneLayout->addWidget(myPlnSelButton, 1, 1);
  aPlaneLayout->addWidget(myWPlaneLineEdit, 1, 2, 1, 2);
#endif
  aPlaneLayout->addWidget(aPolylineLbl, 2, 0);
  aPlaneLayout->addWidget(myPolylineSelButton, 2, 1);
  aPlaneLayout->addWidget(myPolylineEdit, 2, 2, 1, 2);
  
  aPlaneLayout->setColumnStretch(2, 1);

  myCurve = new CurveCreator_Curve( CurveCreator::Dim2d );
  myEditorWidget = new CurveCreator_Widget (centralWidget(), myCurve);
  myAddElementBox = new QGroupBox (tr("POLYLINE_ADD_SECTION"), centralWidget());

  QBoxLayout* anAddElementLayout = new QVBoxLayout( myAddElementBox );

  anAddElementLayout->setMargin( 0 );
  anAddElementLayout->setSpacing( 6 );

  QVBoxLayout* layout = new QVBoxLayout( centralWidget() );

  layout->setMargin( 0 );
  layout->setSpacing( 6 );
  layout->addWidget( aGroupBox1 );
  layout->addWidget( myEditorWidget );
  layout->addWidget( myAddElementBox );

  /***************************************************************/

  setHelpFileName( "create_polyline_page.html" );

  /* Initialisations */
  Init();
}

//=================================================================================
// function : Destructor
// purpose  :
//=================================================================================
EntityGUI_PolylineDlg::~EntityGUI_PolylineDlg()
{
  delete myCurve;
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void EntityGUI_PolylineDlg::Init()
{
  initName(tr("POLYLINE_NAME"));

  SalomeApp_Application *anApp        = myGeomGUI->getApp();
  OCCViewer_ViewManager *aViewManager = dynamic_cast<OCCViewer_ViewManager*>
    (anApp->getViewManager(OCCViewer_Viewer::Type(), true));
  LightApp_SelectionMgr *aSelMgr = myGeomGUI->getApp()->selectionMgr();

  myEditorWidget->setOCCViewer(aViewManager ? aViewManager->getOCCViewer() : 0);

  // Init the list of local coordinate system
  gp_Pnt aPnt(0., 0., 0.);
  gp_Dir aDirN(0., 0., 1.);
  gp_Dir aDirX(1., 0., 0.);
  gp_Ax3 aLCS(aPnt, aDirN, aDirX);

  //add Global CS
  myPlnComboBox->addItem(tr("GEOM_GCS"), false);
  myWPlaneList.push_back(GEOM::GeomObjPtr());
  myLCSList.push_back(aLCS);

  connect(myGeomGUI,      SIGNAL(SignalDeactivateActiveDialog()), this, SLOT(DeactivateActiveDialog()));
  connect(myGeomGUI,      SIGNAL(SignalCloseAllDialogs()),        this, SLOT(ClickOnCancel()));

#ifdef SET_PLANE
  connect(myPlnSelButton, SIGNAL(clicked()),
          this,           SLOT(SetEditCurrentArgument()));
#endif
  connect(myPolylineSelButton, SIGNAL(clicked()),
          this,                SLOT(SetEditCurrentArgument()));
  connect(aSelMgr,        SIGNAL(currentSelectionChanged()),
          this,           SLOT(SelectionIntoArgument()));
  connect(myEditorWidget, SIGNAL(subOperationStarted(QWidget*, bool)),
          this,           SLOT(processStartedSubOperation(QWidget*, bool)));
  connect(myEditorWidget, SIGNAL(subOperationFinished(QWidget*)),
          this,           SLOT(processFinishedSubOperation(QWidget*)));
  connect(myEditorWidget, SIGNAL(curveModified()),
          this,           SLOT(onUpdatePreview()));
#ifdef SET_PLANE
  connect(myPlnComboBox,  SIGNAL(activated(int)),
          this,           SLOT(ActivateLocalCS()));
  connect(myPlnButton,    SIGNAL(clicked()),
          this,           SLOT(ActivateLocalCS()));
#endif
  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  myAddElementBox->hide();
  myPolylineSelButton->click();
  SelectionIntoArgument();
}

//=================================================================================
// function : Clear
// purpose  :
//=================================================================================
void EntityGUI_PolylineDlg::Clear()
{
  delete myCurve;

  myCurve = new CurveCreator_Curve( CurveCreator::Dim2d );
  myEditorWidget->setCurve(myCurve);
}

//=================================================================================
// function : GetCurveParams
// purpose  :
//=================================================================================
void EntityGUI_PolylineDlg::GetCurveParams(GEOM::ListOfListOfDouble &theCoords,
                                           GEOM::string_array       &theNames,
                                           GEOM::short_array        &theTypes,
                                           GEOM::ListOfBool         &theCloseds)
{
  const int aNbSec = myCurve->getNbSections();
  int       i;
  int       j;

  theCoords.length(aNbSec);
  theNames.length(aNbSec);
  theTypes.length(aNbSec);
  theCloseds.length(aNbSec);

  for (i = 0; i < aNbSec; ++i) {
    // Set coordinates
    CurveCreator::Coordinates aCoords   = myCurve->getPoints(i);
    const int                 aNbPoints = aCoords.size();

    theCoords[i].length(aNbPoints);

    for (j = 0; j < aNbPoints; ++j) {
      theCoords[i][j] = aCoords[j];
    }

    // Set section type
    const CurveCreator::SectionType aType = myCurve->getSectionType(i);

    switch (aType) {
      case CurveCreator::Spline:
        theTypes[i] = GEOM::Interpolation;
        break;
      case CurveCreator::Polyline:
      default:
        theTypes[i] = GEOM::Polyline;
        break;
    }

    // Set section names and closed flags.
    theNames[i]   = CORBA::string_dup(myCurve->getSectionName(i).c_str());
    theCloseds[i] = myCurve->isClosed(i);
  }
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr EntityGUI_PolylineDlg::createOperation()
{
  return getGeomEngine()->GetICurvesOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool EntityGUI_PolylineDlg::isValid( QString& msg )
{
  return true;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool EntityGUI_PolylineDlg::execute( ObjectList& objects )
{
  GEOM::GEOM_ICurvesOperations_var anOper =
    GEOM::GEOM_ICurvesOperations::_narrow(getOperation());

  // Get the polyline creation parameters.
  GEOM::ListOfListOfDouble aCoords;
  GEOM::string_array       aNames;
  GEOM::short_array        aTypes;
  GEOM::ListOfBool         aCloseds;

  GetCurveParams(aCoords, aNames, aTypes, aCloseds);

  // Get Working Plane.
  int ind = myPlnComboBox->currentIndex();

  if (ind != -1) {
    bool                  isPlane = myPlnComboBox->itemData(ind).toBool();
    GEOM::GEOM_Object_var anObj;

    // Perform operation
    if (isPlane) {
      anObj = anOper->MakePolyline2DOnPlane
        (aCoords, aNames, aTypes, aCloseds, myWPlaneList.at(ind).get());
    } else {
      gp_Ax3             anAxis = myLCSList.at(ind);
      GEOM::ListOfDouble aPlane;

      aPlane.length(9);
      aPlane[0] = anAxis.Location().X();
      aPlane[1] = anAxis.Location().Y();
      aPlane[2] = anAxis.Location().Z();
      aPlane[3] = anAxis.Direction().X();
      aPlane[4] = anAxis.Direction().Y();
      aPlane[5] = anAxis.Direction().Z();
      aPlane[6] = anAxis.XDirection().X();
      aPlane[7] = anAxis.XDirection().Y();
      aPlane[8] = anAxis.XDirection().Z();

      anObj = anOper->MakePolyline2D
        (aCoords, aNames, aTypes, aCloseds, aPlane);
    }

    if (!anObj->_is_nil()) {
      objects.push_back(anObj._retn());
    }
  }

  return true;
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void EntityGUI_PolylineDlg::ClickOnOk()
{
  setIsApplyAndClose( true );

  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool EntityGUI_PolylineDlg::ClickOnApply()
{
  if (!onAccept())
    return false;

  initName();

  return true;
}

//=================================================================================
// function : ClickOnCancel()
// purpose  :
//=================================================================================
void EntityGUI_PolylineDlg::ClickOnCancel()
{
  myEditorWidget->SetViewer2DMode(false);
  GEOMBase_Skeleton::ClickOnCancel();
}

//=================================================================================
// function : processStartedSubOperation
// purpose  :
//=================================================================================
void EntityGUI_PolylineDlg::processStartedSubOperation( QWidget* theWidget, bool theIsEdit )
{
  myEditorWidget->setEnabled( false );

  myAddElementBox->setTitle( theIsEdit ? tr( "POLYLINE_EDIT_SECTION" ) : tr( "POLYLINE_ADD_SECTION" ) );
  QBoxLayout* anAddElementLayout = dynamic_cast<QBoxLayout*>( myAddElementBox->layout() );
  anAddElementLayout->addWidget( theWidget );

  theWidget->show();
  myAddElementBox->show();
}


//=================================================================================
// function : processFinishedSubOperation
// purpose  :
//=================================================================================
void EntityGUI_PolylineDlg::processFinishedSubOperation( QWidget* theWidget )
{
  myEditorWidget->setEnabled( true );

  QBoxLayout* anAddElementLayout = dynamic_cast<QBoxLayout*>( myAddElementBox->layout() );
  anAddElementLayout->removeWidget( theWidget );

  theWidget->hide();
  myAddElementBox->hide();
}

//=================================================================================
// function : deleteSelected
// purpose  : Redirect the delete action to editor widget
//=================================================================================
void EntityGUI_PolylineDlg::deleteSelected()
{
  myEditorWidget->removeSelected();
}

//=================================================================================
// function : deleteEnabled
// purpose  : Checks whether there are some to delete
//=================================================================================
bool EntityGUI_PolylineDlg::deleteEnabled()
{
  return myEditorWidget->removeEnabled();
}

//=================================================================================
// function : SelectionIntoArgument
// purpose  : Called when selection is changed
//=================================================================================
void EntityGUI_PolylineDlg::SelectionIntoArgument()
{
  bool             isModified      = false;
  GEOM::GeomObjPtr aSelectedObject = getSelected(TopAbs_SHAPE);
  TopoDS_Shape     aShape;

  if (aSelectedObject && GEOMBase::GetShape(aSelectedObject.get(), aShape) &&
      !aShape.IsNull()) {
    QString aName = GEOMBase::GetName(aSelectedObject.get()); 

    if (myEditCurrentArgument == myPolylineEdit) {
      // Import a curve
      CurveCreator_Curve *aNewCurve = 
        new CurveCreator_Curve(CurveCreator::Dim2d);
      gp_Ax3              aLocalCS;

      if (CurveCreator_Utils::constructCurve(aShape, aNewCurve, aLocalCS)) {
        // Change the current curve be the new one.
        myEditorWidget->setCurve(aNewCurve);
        delete myCurve;
        myCurve = aNewCurve;
        isModified = true;
        myPolylineEdit->setText(aName);
#ifdef SET_PLANE
        AddLocalCS(aSelectedObject.get(), false, aLocalCS);
        myWPlaneLineEdit->clear();
        myPlnSelButton->setDown(false);
#endif
        myPolylineSelButton->setDown(true);
      } else {
        // Does nothing, just clears selection.
        delete aNewCurve;
      }
#ifdef SET_PLANE
    } else if (myEditCurrentArgument == myWPlaneLineEdit) {
      // Import planar face.
      if (aShape.ShapeType() == TopAbs_FACE) {
        // Check if the face is planar
        Handle(Geom_Surface) aSurf = BRep_Tool::Surface(TopoDS::Face(aShape));
        GeomLib_IsPlanarSurface aPlanarCheck(aSurf, Precision::Confusion());

        if (aPlanarCheck.IsPlanar()) {
          myWPlaneLineEdit->setText(aName);
          myPolylineEdit->clear();
          AddLocalCS(aSelectedObject.get(), true, 
                     WPlaneToLCS(aSelectedObject.get()));
          isModified = true;
          myPlnSelButton->setDown(true);
          myPolylineSelButton->setDown(false);
        }
      }
      
      if (!isModified) {
        myEditCurrentArgument->setText(tr("GEOM_SKETCHER_WPLANE"));
      }
#endif
    }
  }

  if (!isModified) {
    // Does nothing, just clears selection.
    disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
    myGeomGUI->getApp()->selectionMgr()->clearSelected();
    connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
            this, SLOT(SelectionIntoArgument()));
  }
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void EntityGUI_PolylineDlg::SetEditCurrentArgument()
{
  if (sender() == myPlnSelButton) {
#ifdef SET_PLANE
    myEditCurrentArgument = myWPlaneLineEdit;
    myEditCurrentArgument->setFocus();
    myPlnSelButton->setDown(true);
    myPolylineSelButton->setDown(false);
#endif
  } else if (sender() == myPolylineSelButton) {
    myEditCurrentArgument = myPolylineEdit;
    myEditCurrentArgument->setFocus();
#ifdef SET_PLANE
    myPlnSelButton->setDown(false);
#endif
    myPolylineSelButton->setDown(true);
  }
}

//=================================================================================
// function : ActivateThisDialog
// purpose  :
//=================================================================================
void EntityGUI_PolylineDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();

  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
           this, SLOT(SelectionIntoArgument()));
}

//=================================================================================
// function : enterEvent()
// purpose  :
//=================================================================================
void EntityGUI_PolylineDlg::enterEvent (QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : onUpdatePreview
// purpose  : 
//=================================================================================
void EntityGUI_PolylineDlg::onUpdatePreview()
{
  displayPreview(true);
}

//=================================================================================
// function : ActivateLocalCS
// purpose  : Activate & Fit Working plane
//=================================================================================
void EntityGUI_PolylineDlg::ActivateLocalCS()
{
  const int ind = myPlnComboBox->currentIndex();

  if (ind == 0) {
    // Default plane
#ifdef SET_PLANE
    myWPlaneLineEdit->clear();
#endif
    myPolylineEdit->clear();
  } else if (ind > 0) { // Skip 0 as it is default
    // Update text on line edits.
    QString aName   = GEOMBase::GetName(GetActiveWPlane().get());
    bool    isPlane = myPlnComboBox->itemData(ind).toBool();

    if (isPlane) {
#ifdef SET_PLANE
      myWPlaneLineEdit->setText(aName);
#endif
      myPolylineEdit->clear();
    } else {
      myPolylineEdit->setText(aName);
#ifdef SET_PLANE
      myWPlaneLineEdit->clear();
#endif
    }
  }

  gp_Ax3 anAxis = GetActiveLocalCS();

  myGeomGUI->SetWorkingPlane(anAxis);
  myGeomGUI->ActiveWorkingPlane();
}

//=================================================================================
// function : GetActiveLocalCS
// purpose  : Get Working plane
//=================================================================================
gp_Ax3 EntityGUI_PolylineDlg::GetActiveLocalCS()
{
  const int ind = myPlnComboBox->currentIndex();

  return ind >= 0 ? myLCSList.at(ind) : myGeomGUI->GetWorkingPlane();
}

//=================================================================================
// function : GetActiveWPlane
// purpose  : Get Working plane
//=================================================================================
GEOM::GeomObjPtr EntityGUI_PolylineDlg::GetActiveWPlane()
{
  const int ind = myPlnComboBox->currentIndex();

  return ind >= 0 ? myWPlaneList.at(ind) : GEOM::GeomObjPtr();
}

//=================================================================================
// function : AddLocalCS()
// purpose  : Add All Coordinates systems in study
//=================================================================================
void EntityGUI_PolylineDlg::AddLocalCS(GEOM::GeomObjPtr  theSelectedObject,
                                       const bool        IsPlane,
                                       const gp_Ax3      &theLCS)
{
  QString aName = GEOMBase::GetName(theSelectedObject.get());

  int index = myPlnComboBox->findText(aName, Qt::MatchExactly);

  if (index == -1) { // If the working plane hasn't been added yet
    myWPlaneList.push_back(theSelectedObject);
    myLCSList.push_back(theLCS);
    myPlnComboBox->addItem(aName, QVariant(IsPlane));
    index = myPlnComboBox->count();
    myPlnComboBox->setCurrentIndex(index - 1);
  } else {
    myPlnComboBox->setCurrentIndex(index);
  }
  ActivateLocalCS();
}

//=================================================================================
// function : WPlaneToLCS ( aWPlane )
// purpose  : 
//=================================================================================
gp_Ax3 EntityGUI_PolylineDlg::WPlaneToLCS(GEOM::GeomObjPtr theGeomObj)
{
  TopoDS_Shape aShape =
    GEOM_Client::get_client().GetShape(GeometryGUI::GetGeomGen(), theGeomObj.get());
  gp_Ax3 aLCS;

  if (theGeomObj || aShape.IsNull()) {
    MESSAGE("CORBA::is_nil(theGeomObj) || aShape.IsNull()")
  }

  aLCS.Transform(aShape.Location().Transformation());

  if (aShape.ShapeType() == TopAbs_FACE) {
    GEOM::GEOM_IMeasureOperations_ptr aMeasureOp =
      myGeomGUI->GetGeomGen()->GetIMeasureOperations(getStudyId());
    double Ox, Oy, Oz, Zx, Zy, Zz, Xx, Xy, Xz;

    aMeasureOp->GetPosition(theGeomObj.get(), Ox, Oy, Oz, Zx, Zy, Zz, Xx, Xy, Xz);

    if (aMeasureOp->IsDone()) {
      gp_Pnt aPnt (Ox, Oy, Oz);
      gp_Dir aDirN (Zx, Zy, Zz);
      gp_Dir aDirX (Xx, Xy, Xz);
      aLCS = gp_Ax3(aPnt, aDirN, aDirX);
    }
  }

  return aLCS;
}
