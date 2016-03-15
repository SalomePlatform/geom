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
// File   : EntityGUI_SketcherDlg.cxx
// Author : Renaud NEDELEC, Open CASCADE S.A.S.

// SALOME includes
#include "EntityGUI_FeatureDetectorDlg.h"
#include "ShapeRec_FeatureDetector.hxx"
#include "GEOM_Constants.h"

#include <OCCViewer_ViewWindow.h>
#include <OCCViewer_ViewManager.h>

#include <SOCC_ViewModel.h>

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <EntityGUI.h>
#include <GEOMBase.h>

#include <SUIT_Desktop.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SUIT_ViewWindow.h>
#include <SalomeApp_Application.h>
#include <LightApp_Application.h>
#include <LightApp_SelectionMgr.h>

#include <SALOME_ListIO.hxx>

#include <SalomeApp_Study.h>

#include <utilities.h>
#include <Precision.hxx>

// OCCT includes
#include <gp_Pnt.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>

#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepBuilderAPI_Transform.hxx>

#include <AIS_TexturedShape.hxx>
#include <TCollection_AsciiString.hxx>
#include <StdSelect_DisplayMode.hxx>

#include <Graphic3d_MaterialAspect.hxx>

// C++ includes
#include <set>
#include <utility>

// boost includes
#include <boost/utility.hpp>

// Constructors
enum{
  CONTOURS,
  CORNERS,
  LINES
};

enum {
  KERNEL_SIZE,
  FIND_CONTOURS_METHOD,
  QUALITY_LEVEL,
  MIN_DISTANCE,
  TYPE_CRITERIA,
  MAX_ITER,
  EPSILON,
  LOW_THRESHOLD,
  RATIO,
  L2GRADIENT,
  SMOOTH_SIZE,
  HBINS,
  SBINS,
  HIST_TYPE,
  THRESHOLD_VALUE,
  MAX_THRESHOLD,
};

// // // View
// // enum {
// //   XY,
// //   YZ,
// //   XZ
// // };
  

//=================================================================================
// class    : EntityGUI_FeatureDetectorDlg()
// purpose  : Constructs a EntityGUI_FeatureDetectorDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
EntityGUI_FeatureDetectorDlg::EntityGUI_FeatureDetectorDlg( GeometryGUI* theGeometryGUI, QWidget* parent,
                                              bool modal, Qt::WindowFlags fl)
  : GEOMBase_Skeleton(theGeometryGUI, parent, modal, fl) 
{
  QPixmap image0(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_SELECT")));
  QPixmap image1(SUIT_Session::session()->resourceMgr()->loadPixmap("OCCViewer", tr("ICON_OCCVIEWER_VIEW_DUMP")));

  setWindowTitle(tr("GEOM_DETECT_TITLE"));
  
  /***************************************************************/
  
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_FEATURES"));
  mainFrame()->RadioButton1->setText(tr("GEOM_CONTOURS"));
  mainFrame()->RadioButton2->setText(tr("GEOM_CORNERS"));
  mainFrame()->RadioButton3->setAttribute(Qt::WA_DeleteOnClose);
  mainFrame()->RadioButton3->close();
  
//   myViewGroup = new DlgRef_3Radio(centralWidget());
//   myViewGroup->GroupBox1->setTitle(tr("GEOM_VIEW"));
//   myViewGroup->RadioButton1->setText(tr( "GEOM_TOP"  ));
//   myViewGroup->RadioButton2->setText(tr( "GEOM_FRONT"));
//   myViewGroup->RadioButton3->setText(tr( "GEOM_LEFT" ));
//   myViewButtonGroup = new QButtonGroup( this );
//   myViewButtonGroup->addButton( myViewGroup->RadioButton1, XY ); // Top view
//   myViewButtonGroup->addButton( myViewGroup->RadioButton2, YZ ); // Front View
//   myViewButtonGroup->addButton( myViewGroup->RadioButton3, XZ ); // Left View
//   
//   myViewGroup->hide();
  
  // Widgets for the selection of the picture and the Region Of Interest 
  
  
  mySelectionGroup = new DlgRef_1Sel1Frame(centralWidget());

  mySelectionGroup->PushButton1->setIcon(image0);
  mySelectionGroup->PushButton1->setCheckable(true);
  mySelectionGroup->PushButton1->setAutoExclusive(true);
  
  mySelectionGroup->PushButton2->setIcon(image1);
  mySelectionGroup->PushButton2->setCheckable(true);
  mySelectionGroup->PushButton2->setAutoExclusive(true);

  mySelectionGroup->TextLabel1->setText(tr( "GEOM_PICTURE" ));
  mySelectionGroup->FrameLabel->setText("");
  
  myOutputGroup = new DlgRef_3Radio(centralWidget());
  myOutputGroup->GroupBox1->setTitle(tr("GEOM_DETECT_OUTPUT"));
  myOutputGroup->RadioButton2->setText(tr( "GEOM_POLYLINE"  ));
  myOutputGroup->RadioButton1->setText(tr( "GEOM_SPLINE"));
  myOutputGroup->RadioButton3->hide();
  
//   myOutputGroup->hide(); //caché pour la demo
    
  // Parameters 
  QGridLayout* parametersLayout = new QGridLayout();
  parametersLayout->setMargin(9);
  QComboBox* kernelSize = new QComboBox();
  kernelSize->addItems( QStringList() << "3" << "5" << "7" );
  myWidgets.insert( KERNEL_SIZE, kernelSize );
  myUseROI = new QCheckBox( tr("USE_ROI") );
  myUseROI->setChecked( true );

  parametersLayout->addWidget( new QLabel( tr("KERNEL_SIZE") ), 0, 0 );
  parametersLayout->addWidget( kernelSize, 0, 1 );
  parametersLayout->addWidget( myUseROI, 0, 2 );

  // Corners Parameters 
  myCornersParameters = new QFrame();
  myCornersParameters->setFrameStyle(QFrame::NoFrame);
  QDoubleSpinBox* qualityLevel = new QDoubleSpinBox();
  qualityLevel->setValue( 0.2 );
  qualityLevel->setSingleStep( 0.1 );
  qualityLevel->setRange( 0.01, 0.99 );
  myWidgets.insert( QUALITY_LEVEL, qualityLevel );
  QDoubleSpinBox* minDistance = new QDoubleSpinBox();
  minDistance->setValue( 1 );
  myWidgets.insert( MIN_DISTANCE, minDistance );
  QComboBox* typeCriteria = new QComboBox();
  typeCriteria->addItems( QStringList() << tr("CV_TERMCRIT_ITER") << tr("CV_TERMCRIT_EPS") << tr("CV_TERMCRIT_ITER | CV_TERMCRIT_EPS") );
  typeCriteria->setCurrentIndex( 2 );
  myWidgets.insert( TYPE_CRITERIA, typeCriteria );
  QSpinBox* maxIter = new QSpinBox();
  maxIter->setValue( 20 );
  maxIter->setRange( 1, 100 );
  myWidgets.insert( MAX_ITER, maxIter );
  QDoubleSpinBox* epsilon = new QDoubleSpinBox();
  epsilon->setValue( 0.03 );
  epsilon->setSingleStep( 0.01 );
  myWidgets.insert( EPSILON, epsilon );

  QGridLayout* cornersLayout = new QGridLayout();
  cornersLayout->setMargin( 0 );
  cornersLayout->addWidget( new QLabel( tr("QUALITY_LEVEL") ), 0, 0 );
  cornersLayout->addWidget( qualityLevel, 0, 1 );
  cornersLayout->addWidget( new QLabel( tr("MIN_DISTANCE") ), 0, 2 );
  cornersLayout->addWidget( minDistance, 0 ,3 );
  cornersLayout->addWidget( new QLabel( tr("TYPE_CRITERIA") ), 1, 0 );
  cornersLayout->addWidget( typeCriteria, 1, 1, 1, 3 );
  cornersLayout->addWidget( new QLabel( tr("MAX_ITER") ), 2, 0 );
  cornersLayout->addWidget( maxIter, 2, 1 );
  cornersLayout->addWidget( new QLabel( tr("EPSILON") ), 2, 2 );
  cornersLayout->addWidget( epsilon, 2 , 3 );

  myCornersParameters->setLayout( cornersLayout );

  // Contours Parameters
  myContoursParameters = new QFrame();
  myContoursParameters->setFrameStyle(QFrame::NoFrame);

  QComboBox* findContoursMethod = new QComboBox();
  findContoursMethod->addItems( QStringList() << tr("CV_CHAIN_APPROX_NONE") << tr("CV_CHAIN_APPROX_SIMPLE") << tr("CV_CHAIN_APPROX_TC89_L1") << tr("CV_CHAIN_APPROX_TC89_KCOS") );
  myWidgets.insert( FIND_CONTOURS_METHOD, findContoursMethod );

  myCannyParameters = new QFrame();
  myCannyParameters->setFrameStyle(QFrame::NoFrame);
  QSpinBox* threshold = new QSpinBox();
  threshold->setRange( 0, 255 );
  threshold->setValue( 100 );
  myWidgets.insert( LOW_THRESHOLD, threshold );
  QSpinBox* ratio = new QSpinBox();
  ratio->setValue( 3 );
  ratio->setRange( 0, 255 );
  myWidgets.insert( RATIO, ratio );
  QCheckBox* L2gradient = new QCheckBox(tr("L2GRADIENT"));
  L2gradient->setChecked(true);
  myWidgets.insert( L2GRADIENT, L2gradient );

  QGridLayout* cannyLayout = new QGridLayout();
  cannyLayout->setMargin( 0 );
  cannyLayout->addWidget( new QLabel( tr("LOWTHRESHOLD") ), 0, 0 );
  cannyLayout->addWidget( threshold, 0, 1 );
  cannyLayout->addWidget( new QLabel( tr("RATIO") ), 0, 2 );
  cannyLayout->addWidget( ratio, 0, 3 );
  cannyLayout->addWidget( L2gradient, 1, 0 );

  myCannyParameters->setLayout( cannyLayout );
  myCannyParameters->setHidden( true );

  myColorFilterParameters = new QFrame();
  myColorFilterParameters->setFrameStyle(QFrame::NoFrame);

  QSpinBox* smoothSize = new QSpinBox();
  smoothSize->setValue( 3 );
  smoothSize->setSingleStep( 2 );
  smoothSize->setRange( 1, 1000 );
  myWidgets.insert( SMOOTH_SIZE, smoothSize );
  QSpinBox* hbins = new QSpinBox();
  hbins->setValue( 30 );
  hbins->setRange( 0, 179 );
  myWidgets.insert( HBINS, hbins );
  QSpinBox* sbins = new QSpinBox();
  sbins->setValue( 32 );
  sbins->setRange( 0, 255 );
  myWidgets.insert( SBINS, sbins );
  QComboBox* histType = new QComboBox();
  histType->addItems( QStringList() << tr("CV_HIST_ARRAY") << tr("CV_HIST_SPARSE") );
  myWidgets.insert( HIST_TYPE, histType );
  QDoubleSpinBox* thresholdValue = new QDoubleSpinBox();
  thresholdValue->setRange( 0, 254 );
  // 1 is a good default value for the threshold. It means that we are very permissive
  // about what will be considered INSIDE the zone we want to find the frontier of
  // This makes the algorithm more robust against a bit inhomogeneous parts in the zone
  // that we want to delimitate.
  // The drawback is if we want to delimitate a zone wich color is very similar to the zone
  // we consider as the OUTSIDE, the result will be bad.
  // The current use cases are more of the first form : 
  //  - Strongly contrasted INSIDE and OUTSIDE zones
  //  - Small inhomogenities in each zone
  thresholdValue->setValue( 1 );
  myWidgets.insert( THRESHOLD_VALUE, thresholdValue );
  QDoubleSpinBox* maxThreshold = new QDoubleSpinBox();
  maxThreshold->setRange( 1, 255 );
  maxThreshold->setValue( 255 );
  myWidgets.insert( MAX_THRESHOLD, maxThreshold );

  QGridLayout* colorFilterLayout = new QGridLayout();
  colorFilterLayout->setMargin( 0 );
  colorFilterLayout->addWidget( new QLabel( tr("SMOOTH_SIZE") ), 0, 0 );
  colorFilterLayout->addWidget( smoothSize, 0, 1 );
  colorFilterLayout->addWidget( new QLabel( tr("HBINS") ), 1, 0 );
  colorFilterLayout->addWidget( hbins, 1, 1 );
  colorFilterLayout->addWidget( new QLabel( tr("SBINS") ), 1, 2 );
  colorFilterLayout->addWidget( sbins, 1, 3 );
  colorFilterLayout->addWidget( new QLabel( tr("HIST_TYPE") ), 2, 0 );
  colorFilterLayout->addWidget( histType, 2, 1, 1, 3 );
  colorFilterLayout->addWidget( new QLabel( tr("THRESHOLD_VALUE") ), 3, 0 );
  colorFilterLayout->addWidget( thresholdValue, 3, 1 );
  colorFilterLayout->addWidget( new QLabel( tr("MAX_THRESHOLD") ), 3, 2 );
  colorFilterLayout->addWidget( maxThreshold, 3, 3 );

  myColorFilterParameters->setLayout( colorFilterLayout );
  QGridLayout* contoursLayout = new QGridLayout();
  contoursLayout->setMargin( 0 );
  contoursLayout->addWidget( new QLabel( tr("FIND_CONTOURS_METHOD") ), 0, 0 );
  contoursLayout->addWidget( findContoursMethod, 0, 1 );
  contoursLayout->addWidget( myCannyParameters, 1, 0, 1, 3 );
  contoursLayout->addWidget( myColorFilterParameters, 1, 0, 1, 3  );

  myContoursParameters->setLayout( contoursLayout );

  parametersLayout->addWidget( myCornersParameters, 1, 0, 1, 3 );
  parametersLayout->addWidget( myContoursParameters, 1, 0, 1, 3 );

  QVBoxLayout* layout = new QVBoxLayout(centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
//   layout->addWidget( myViewGroup);
  layout->addLayout( parametersLayout );
  layout->addWidget( mySelectionGroup);
  layout->addWidget( myOutputGroup);
  
//   mainFrame()->GroupBoxName->hide();
  
  // Build an instance of detection used to perform image processing operations
  myDetector = new ShapeRec_FeatureDetector();
  
  setHelpFileName( "shape_recognition_page.html" );
  
  Init();
  
}

//=================================================================================
// function : ~EntityGUI_FeatureDetectorDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
EntityGUI_FeatureDetectorDlg::~EntityGUI_FeatureDetectorDlg()
{
  delete myDetector;
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void EntityGUI_FeatureDetectorDlg::Init()
{
  connect( myGeomGUI,         SIGNAL( SignalCloseAllDialogs() ), this, SLOT( ClickOnCancel() ) );
  connect( buttonOk(),        SIGNAL( clicked() ),               this, SLOT( ClickOnOk() ) );
  connect( buttonApply(),     SIGNAL( clicked() ),               this, SLOT( ClickOnApply() ) );
  connect( myUseROI,          SIGNAL( toggled(bool) ),           this, SLOT( onCheckBoxClicked(bool) ) );
  connect( this,              SIGNAL(constructorsClicked(int)),  this, SLOT(ConstructorsClicked(int))); 
  connect( mySelectionGroup->PushButton2,      SIGNAL( clicked() ),               this, SLOT( onButtonClicked() ) );
  connect( mySelectionGroup->PushButton1,       SIGNAL( clicked() ),               this, SLOT( onButtonClicked() ) );  
//   connect( myViewButtonGroup, SIGNAL( buttonClicked( int ) ),    this, SLOT( onViewClicked( int ) ) );
  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),this, SLOT( SelectionIntoArgument() ) );
  
  myConstructorId = 0;
  mySelectionGroup->PushButton1->click();
  
//   SetEditCurrentArgument();
  SelectionIntoArgument();
  
  initName(tr("GEOM_CONTOURS")); 
  resize(100,100);
  
//   myViewGroup->RadioButton1->setChecked(true);
  myOutputGroup->RadioButton1->setChecked(true);
  
  gp_Pnt aOrigin = gp_Pnt(0, 0, 0);
  gp_Dir aDirZ = gp_Dir(0, 0, 1);
  gp_Dir aDirX = gp_Dir(1, 0, 0);
  
  aGlobalCS = gp_Ax3(aOrigin, aDirZ, aDirX);
  
  myStartPnt = gp_Pnt(0,0,0);
  myEndPnt = myStartPnt;
  
  myGeomGUI->SetWorkingPlane( aGlobalCS );
  myGeomGUI->ActiveWorkingPlane();
  
  ConstructorsClicked(myConstructorId);
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection as changed or other case
//=================================================================================
void EntityGUI_FeatureDetectorDlg::SelectionIntoArgument()
{
  
  // TODO supprimer les lignes qui ne servent à rien le cas échéant
  SUIT_ViewWindow*       theViewWindow  = getDesktop()->activeWindow();
  SOCC_Viewer* soccViewer = (SOCC_Viewer*)(theViewWindow->getViewManager()->getViewModel());

  if (!myEditCurrentArgument->isEnabled())
    return;
  
  myEditCurrentArgument->setText( "" );

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);
  SALOME_ListIteratorOfListIO anIt( aSelList );
  
  for( ; anIt.More(); anIt.Next() )
    if( !anIt.Value().IsNull() )
    {
      myFaceEntry = anIt.Value()->getEntry();
    }

  if (aSelList.Extent() != 1) {
    if (myEditCurrentArgument == mySelectionGroup->LineEdit1) 
      myFace.nullify();
    return;
  }

  TopAbs_ShapeEnum aNeedType = TopAbs_FACE ;
  GEOM::GeomObjPtr aSelectedObject = getSelected( aNeedType );
  TopoDS_Shape aShape;
  if ( aSelectedObject && GEOMBase::GetShape( aSelectedObject.get(), aShape ) && !aShape.IsNull() ) {
    QString aName = GEOMBase::GetName( aSelectedObject.get() );
    myEditCurrentArgument->setText( aName );
    
    if ( myEditCurrentArgument == mySelectionGroup->LineEdit1 ) {
      myFace = aSelectedObject;

      SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>( SUIT_Session::session()->activeApplication()->activeStudy() );
      if ( !study ) return;
      LightApp_Application* app = ::qobject_cast<LightApp_Application*>( study->application() );
      if ( !app ) return;
      SUIT_ViewManager* vm = app->activeViewManager();
      if ( !vm ) return;
      PropMap propMap = study->getObjectProperties( vm->getGlobalId(), myFaceEntry );
      QString theImgFileName = propMap.value( GEOM::propertyName( GEOM::Texture ) ).toString();
      if ( theImgFileName.isEmpty() )
        return;

      // Setting the image caracteristics
      myDetector->SetPath( theImgFileName.toStdString() );
      height            =  myDetector->GetImgHeight();
      width             =  myDetector->GetImgWidth();
      pictureLeft       = -0.5 * width;              // X coordinate of the top left  corner of the background image in the view
      pictureTop        =  0.5 * height;             // Y coordinate of both top corners
      
    } 
  }
  
}

//=================================================================================
// function : acceptMouseEvent()
// purpose  :
//=================================================================================
bool EntityGUI_FeatureDetectorDlg::acceptMouseEvent() const
{ 
  return mySelectionGroup->PushButton2->isChecked();  
}

//=======================================================================
// function : ClickOnOk()
// purpose  :
//=======================================================================
void EntityGUI_FeatureDetectorDlg::ClickOnOk()
{
  setIsApplyAndClose( true );
  if (onAccept())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool EntityGUI_FeatureDetectorDlg::ClickOnApply()
{
  if (!onAccept())
    return false;

//   initName();
  ConstructorsClicked(getConstructorId());
  return true;
}

//=================================================================================
// function : onConstructorClicked(int)
// purpose  :
//=================================================================================
void EntityGUI_FeatureDetectorDlg::ConstructorsClicked(int id)
{
  MESSAGE("Constructor id ="<<id)
  myConstructorId = id;
  switch (id)
  {
    case CORNERS:
      myCornersParameters->show();
      myContoursParameters->hide();
//       myViewGroup->show();
//       mySelectionGroup->show();
      myOutputGroup->hide();
//       mySelectionGroup->TextLabel2->setText(tr("GEOM_DETECT_ZONE"));
      initName(tr("GEOM_CORNERS"));
      break;
    case CONTOURS:
      myCornersParameters->hide();
      myContoursParameters->show();
//       myViewGroup->hide();
//       mySelectionGroup->hide();
//       mySelectionGroup->show();
      myOutputGroup->show();
      mySelectionGroup->TextLabel2->setText(tr("GEOM_COLOR_FILTER"));
      initName(tr("GEOM_CONTOURS"));
      break;
    case LINES:
//       myViewGroup->hide();
//       mySelectionGroup->hide();
//       mySelectionGroup->show();
      myOutputGroup->hide();
      mySelectionGroup->TextLabel2->setText(tr(""));
      initName(tr("GEOM_LINES"));
      break;
  }
}

// //=================================================================================
// // function : onViewClicked()
// // purpose  :
// //=================================================================================
// void EntityGUI_FeatureDetectorDlg::onViewClicked(int id)
// {
//   gp_Pnt aOrigin = gp_Pnt(0, 0, 0);
//   gp_Dir aDirZ;
//   gp_Dir aDirX;
//   
//   switch(id)
//   {
//     case XY:
//         aDirZ = gp_Dir(0, 0, 1);
//         aDirX = gp_Dir(1, 0, 0);
//       break;
//     case YZ:
//         aDirZ = gp_Dir(1, 0, 0);
//         aDirX = gp_Dir(0, 1, 0);
//       break;
//     case XZ:
//         aDirZ = gp_Dir(0, -1, 0);
//         aDirX = gp_Dir(1, 0 , 0);
//       break;
//   }
//   
//   myWPlane = gp_Ax3(aOrigin, aDirZ, aDirX);
//   myGeomGUI->SetWorkingPlane( myWPlane );
//   myGeomGUI->ActiveWorkingPlane();
//   
// }

//=================================================================================
// function : onButtonClicked()
// purpose  :
//=================================================================================
void EntityGUI_FeatureDetectorDlg::onButtonClicked()
{
  QPushButton* send = (QPushButton*)sender();
  if (send == mySelectionGroup->PushButton2)
  {
    mySelectionGroup->LineEdit1->setEnabled(false);
  }
  else if (send == mySelectionGroup->PushButton1)
  {
    myStartPnt = gp_Pnt(0,0,0);
    myEndPnt   = myStartPnt;
    myEditCurrentArgument = mySelectionGroup->LineEdit1;
    mySelectionGroup->LineEdit1->setEnabled(true);   
  }
}

//=================================================================================
// function : onCheckBoxClicked(bool)
// purpose  :
//=================================================================================
void EntityGUI_FeatureDetectorDlg::onCheckBoxClicked( bool isChecked )
{
  mySelectionGroup->TextLabel2->setVisible( isChecked );
  mySelectionGroup->Frame->setVisible( isChecked );
  mySelectionGroup->PushButton2->setVisible( isChecked );
  myCannyParameters->setHidden( isChecked );
  myColorFilterParameters->setHidden( !isChecked );
}

//=================================================================================
// function : parametersChanged()
// purpose  :
//=================================================================================
ShapeRec_Parameters* EntityGUI_FeatureDetectorDlg::parametersChanged()
{
  ShapeRec_Parameters* aParameters = new ShapeRec_Parameters();

  if ( myConstructorId == CORNERS ) {
    ShapeRec_CornersParameters* aCornersParameters = dynamic_cast<ShapeRec_CornersParameters*>( aParameters );
    if ( !aCornersParameters ) aCornersParameters = new ShapeRec_CornersParameters();
    aCornersParameters->qualityLevel = (dynamic_cast<QDoubleSpinBox*>(myWidgets[QUALITY_LEVEL]))->value();
    aCornersParameters->minDistance  = (dynamic_cast<QDoubleSpinBox*>(myWidgets[MIN_DISTANCE]))->value();
    switch ( (dynamic_cast<QComboBox*>(myWidgets[TYPE_CRITERIA]))->currentIndex() ) {
    case 0: aCornersParameters->typeCriteria = CV_TERMCRIT_ITER;
    case 1: aCornersParameters->typeCriteria = CV_TERMCRIT_EPS;
    case 2: aCornersParameters->typeCriteria = CV_TERMCRIT_ITER | CV_TERMCRIT_EPS;
    }
    aCornersParameters->maxIter = (dynamic_cast<QSpinBox*>(myWidgets[MAX_ITER]))->value();
    aCornersParameters->epsilon = (dynamic_cast<QDoubleSpinBox*>(myWidgets[EPSILON]))->value();
    aParameters = aCornersParameters;
  }
  else if ( myConstructorId == CONTOURS ) {
    if ( !myUseROI->isChecked() ) { 
      ShapeRec_CannyParameters* aCannyParameters = dynamic_cast<ShapeRec_CannyParameters*>( aParameters );
      if ( !aCannyParameters ) aCannyParameters = new ShapeRec_CannyParameters();
      aCannyParameters->lowThreshold = (dynamic_cast<QSpinBox*>(myWidgets[LOW_THRESHOLD]))->value();
      aCannyParameters->ratio        = (dynamic_cast<QSpinBox*>(myWidgets[RATIO]))->value();
      aCannyParameters->L2gradient   = (dynamic_cast<QCheckBox*>(myWidgets[L2GRADIENT]))->isChecked();
      aParameters = aCannyParameters;
    }
    else {
      ShapeRec_ColorFilterParameters* aColorFilterParameters = dynamic_cast<ShapeRec_ColorFilterParameters*>( aParameters );
      if ( !aColorFilterParameters ) aColorFilterParameters = new ShapeRec_ColorFilterParameters();
      int aSmoothSize = (dynamic_cast<QSpinBox*>(myWidgets[SMOOTH_SIZE]))->value();
      aColorFilterParameters->smoothSize  = aSmoothSize % 2 == 0 ? aSmoothSize - 1 : aSmoothSize;
      aColorFilterParameters->histSize[0] = (dynamic_cast<QSpinBox*>(myWidgets[HBINS]))->value();
      aColorFilterParameters->histSize[1] = (dynamic_cast<QSpinBox*>(myWidgets[SBINS]))->value();
      switch ( (dynamic_cast<QComboBox*>(myWidgets[HIST_TYPE]))->currentIndex() ) {
      case 0: aColorFilterParameters->histType = CV_HIST_ARRAY;  break;
      case 1: aColorFilterParameters->histType = CV_HIST_SPARSE; break;
      }
      aColorFilterParameters->threshold    = (dynamic_cast<QDoubleSpinBox*>(myWidgets[THRESHOLD_VALUE]))->value();
      aColorFilterParameters->maxThreshold = (dynamic_cast<QDoubleSpinBox*>(myWidgets[MAX_THRESHOLD]))->value();
      aParameters = aColorFilterParameters;
    }
  }
 
  aParameters->kernelSize = ( (dynamic_cast<QComboBox*>(myWidgets[KERNEL_SIZE]))->currentText() ).toInt();
  switch ( (dynamic_cast<QComboBox*>(myWidgets[FIND_CONTOURS_METHOD]))->currentIndex() ) {
  case 0: aParameters->findContoursMethod = CV_CHAIN_APPROX_NONE;      break;
  case 1: aParameters->findContoursMethod = CV_CHAIN_APPROX_SIMPLE;    break;
  case 2: aParameters->findContoursMethod = CV_CHAIN_APPROX_TC89_L1;   break;
  case 3: aParameters->findContoursMethod = CV_CHAIN_APPROX_TC89_KCOS; break;
  }

  return aParameters;
}

//=================================================================================
// function : setStartPnt( const QPoint& )
// purpose  :
//=================================================================================
void EntityGUI_FeatureDetectorDlg::setStartPnt(const gp_Pnt& theStartPnt)
{
  myStartPnt = theStartPnt;
  MESSAGE("myStartPnt = ("<<theStartPnt.X()<<", "<<theStartPnt.Y()<<")")
}

//=================================================================================
// function : setEndPnt( const QPoint& )
// purpose  :
//=================================================================================
void EntityGUI_FeatureDetectorDlg::setEndPnt(const gp_Pnt& theEndPnt)
{
  myEndPnt = theEndPnt;
  MESSAGE("myEndPnt = ("<<theEndPnt.X()<<", "<<theEndPnt.Y()<<")")
  if (setSelectionRect() && myDetector->GetImgHeight() > 0)
    showImageSample();
}

//=================================================================================
// function : setSelectionRect()
// purpose  :
//=================================================================================
bool EntityGUI_FeatureDetectorDlg::setSelectionRect()
{ 
  // Set detection rectangle in the background image coordinates system
  double left    = std::min( myStartPnt.X(), myEndPnt.X() );
  double top     = std::max( myStartPnt.Y(), myEndPnt.Y() );
  double right   = std::max( myStartPnt.X(), myEndPnt.X() );
  double bottom  = std::min( myStartPnt.Y(), myEndPnt.Y() );
  
  QPoint topLeft     = QPoint(left  - pictureLeft, pictureTop - top   );
  QPoint bottomRight = QPoint(right - pictureLeft, pictureTop - bottom);
  
  myRect = QRect(topLeft, bottomRight);
  
  return (!myRect.isEmpty() && myRect.width() > 1);
}

//=================================================================================
// function : showImageSample()
// purpose  : Display a preview of the image sample selected by the user
//=================================================================================
void EntityGUI_FeatureDetectorDlg::showImageSample()
{ 
  // Cropp the image to the selection rectangle given by the user
  myDetector->SetROI( myRect ); 
  std::string samplePicturePath = myDetector->CroppImage();
  
  // Display the result
  QPixmap pixmap(QString(samplePicturePath.c_str()));
  mySelectionGroup->FrameLabel->setPixmap(pixmap);
  mySelectionGroup->FrameLabel->setMask(pixmap.mask());
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr EntityGUI_FeatureDetectorDlg::createOperation()
{
  return myGeomGUI->GetGeomGen()->GetIShapesOperations( getStudyId() );
}

//=================================================================================
// function : execute()
// purpose  :
//=================================================================================
bool EntityGUI_FeatureDetectorDlg::execute( ObjectList& objects )
{
  bool res = false;
  
  GEOM::GEOM_IBasicOperations_var  aBasicOperations  = myGeomGUI->GetGeomGen()->GetIBasicOperations( getStudyId() );
  GEOM::GEOM_IShapesOperations_var aShapesOperations = GEOM::GEOM_IShapesOperations::_narrow( getOperation() );

  ShapeRec_Parameters* parameters = parametersChanged();
  if ( !parameters ) parameters = new ShapeRec_Parameters();

  bool useROI = myUseROI->isChecked() && !myRect.isEmpty();
  try {
    if (myConstructorId == CORNERS)
    {
      double subPictureLeft;
      double subPictureTop;
      if( useROI )
      {
	subPictureLeft    = myStartPnt.X();                
	subPictureTop     = myStartPnt.Y();
      }
      else
      {
	subPictureLeft    = pictureLeft;
	subPictureTop     = pictureTop;
      }
      myDetector->ComputeCorners( useROI, parameters );
      CvPoint2D32f* corners     = myDetector->GetCorners();
      int cornerCount           = myDetector->GetCornerCount();
      int i;
    
      // Build the geom objects associated to the detected corners and returned by execute   
      if( !aBasicOperations->_is_nil() && !aShapesOperations->_is_nil() ) 
      {
	GEOM::GEOM_Object_var  aGeomCorner;
	GEOM::ListOfGO_var     geomCorners = new GEOM::ListOfGO();
	geomCorners->length( cornerCount );
	for (i = 0; i < cornerCount; i++)
        {
	  double x = subPictureLeft + corners[i].x;
	  double y = subPictureTop  - corners[i].y;
	  double z =  0;
        
	  aGeomCorner = aBasicOperations->MakePointXYZ( x,y,z );
        
	  geomCorners[i] = aGeomCorner;  
	} 
	GEOM::GEOM_Object_var aCompound = aShapesOperations->MakeCompound(geomCorners);    
	if ( !aCompound->_is_nil() )
	{
	  // Dark blue color
	  SALOMEDS::Color aColor;
	  aColor.R = 0;
	  aColor.G = 0;
	  aColor.B = 0.8;

	  aCompound->SetColor(aColor);
	  aCompound->SetMarkerStd(GEOM::MT_POINT,GEOM::MS_30);
	  objects.push_back( aCompound._retn() );
	  res = true;
	}
      }   
    }
    else if (myConstructorId == CONTOURS)
    {    
      GEOM::GEOM_ICurvesOperations_var aCurveOperations = myGeomGUI->GetGeomGen()->GetICurvesOperations( getStudyId() );

      myDetector->ComputeContours( useROI, parameters );
      std::vector< std::vector<cv::Point> >   contours  = myDetector->GetContours();
      std::vector<cv::Vec4i>                  hierarchy = myDetector->GetContoursHierarchy();
    
      std::vector< cv::Point >                contour;
      int idx = 0;
      
      GEOM::ListOfGO_var                      geomContours = new GEOM::ListOfGO();
      int contourCount = 0;
    
      bool insert;
    
      MESSAGE("hierarchy.size() =" << hierarchy.size()) 
      if ( hierarchy.size() < 1 ) {
	getOperation()->SetErrorCode( "Impossible detected contours" );
	return false;
      }

      for( ; idx >= 0; idx = hierarchy[idx][0])
      {
//       for(int count=0, child=idx; child>=0, count<1; child=hierarchy[idx][2], count++)
//       {     
//         contour = contours[child];
        contour = contours[idx];
        std::vector< cv::Point >::iterator it;
        std::vector< cv::Point >::iterator it_previous;
        std::vector< cv::Point >::iterator it_next;
        GEOM::GEOM_Object_var  aGeomContourPnt;
        GEOM::ListOfGO_var     geomContourPnts = new GEOM::ListOfGO();
       
        geomContourPnts->length( contour.size() );

        int j = 0;
        std::set< std::vector<int> > existing_points;
        std::pair< std::set< std::vector<int> >::iterator,bool > pnt_it;
        for ( it=contour.begin() ; it < contour.end()-1; it++ )
        {
          int pnt_array[] = {it->x,it->y};     
          std::vector<int> pnt (pnt_array, pnt_array + sizeof(pnt_array) / sizeof(int) );

          pnt_it=existing_points.insert(pnt);
          if (pnt_it.second == true)         // To avoid double points in the contours
          {
            insert = true;
            if (it!=contour.begin())         // From the second point on perform some checking to avoid loops in the contours we build
            {
              it_previous = boost::prior(it);
              it_next = boost::next(it);
              
              double u_v_scalar_product = (it->x - it_previous->x) * (it_next->x - it->x) + 
                                          (it->y - it_previous->y) * (it_next->y - it->y);                                       
              if (u_v_scalar_product < 0)
              {
                double u_v_det = (it->x - it_previous->x) * (it_next->y - it->y) - 
                                 (it->y - it_previous->y) * (it_next->x - it->x);
                                          
                double norme_u = sqrt ( double(it->x - it_previous->x) * double(it->x - it_previous->x) +
                                        double(it->y - it_previous->y) * double(it->y - it_previous->y) );
                
                double norme_v = sqrt ( double(it->x - it_next->x) * double(it->x - it_next->x) +
                                        double(it->y - it_next->y) * double(it->y - it_next->y) );
                                                                                                
                double u_v_sinus = u_v_det / (norme_u * norme_v);
                
                if (fabs(u_v_sinus) < Precision::Confusion())
                { 
                  insert = false;
                }                         
              }
            }
            double x = -0.5 *width  + it->x;
            double y =  0.5 *height - it->y;
            double z =  0;
            if (insert)
            {
              aGeomContourPnt    = aBasicOperations->MakePointXYZ( x,y,z );
              geomContourPnts->length( j+1 );
              geomContourPnts[j] = aGeomContourPnt;
              j++;
            }
          }
        }
        
        GEOM::GEOM_Object_var aWire;
        if(myOutputGroup->RadioButton2->isChecked())
        {
          aWire = aCurveOperations->MakePolyline(geomContourPnts.in(), false);
        }
        else if(myOutputGroup->RadioButton1->isChecked())
        {
          aWire = aCurveOperations->MakeSplineInterpolation(geomContourPnts.in(), /*closed =*/ false, /*reordering =*/ false);
        }
        else
          return res;
        
        if ( !aWire->_is_nil() )
        {
          geomContours->length(contourCount + 1);
          geomContours[contourCount] = aWire;
          contourCount++;
        }
//       }
      }
      GEOM::GEOM_Object_var aContoursCompound = aShapesOperations->MakeCompound(geomContours);
      if ( !aContoursCompound->_is_nil() )
      {
	objects.push_back( aContoursCompound._retn() );
      }
      res=true;
    }
  }
  catch( ... ) {
    getOperation()->SetErrorCode( "Impossible detected contours/corners" );
    return false;
  }
  
  // TEST not very conclusive
  
//   else if(myConstructorId ==LINES)
//   {
//     myDetector->ComputeLines();
//     std::vector<cv::Vec4i>  lines = myDetector->GetLines();
//     GEOM::GEOM_Object_var  Pnt1;
//     GEOM::GEOM_Object_var  Pnt2;
//     GEOM::GEOM_Object_var  aLine;
//     
//     GEOM::ListOfGO_var     geomLines = new GEOM::ListOfGO();
//     int linesCount=0;
//     for( int i = 0; i < lines.size(); i++ )
//     {
//       Pnt1 = aBasicOperations->MakePointXYZ( -0.5 *width + lines[i][0], 0.5 *height - lines[i][1], 0 );
//       Pnt2 = aBasicOperations->MakePointXYZ( -0.5 *width + lines[i][2], 0.5 *height - lines[i][3], 0 );
//       aLine = aBasicOperations->MakeLineTwoPnt( Pnt1, Pnt2 );
//       if ( !aLine->_is_nil() )
//       {
//         geomLines->length(linesCount + 1);
//         geomLines[linesCount] = aLine;
//         linesCount++;
//       }
//     }
//     GEOM::GEOM_Object_var aLinesCompound = aShapesOperations->MakeCompound(geomLines);
//     if ( !aLinesCompound->_is_nil() )
//     {
//       objects.push_back( aLinesCompound._retn() );
//     }
// 
//     res=true;
//   }
  
  return res;
}

//=================================================================================
// function : getSourceObjects
// purpose  : virtual method to get source objects
//=================================================================================
QList<GEOM::GeomObjPtr> EntityGUI_FeatureDetectorDlg::getSourceObjects()
{
  QList<GEOM::GeomObjPtr> res;
  res << myFace;
  return res;
}
