// Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
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

// File   : GEOMToolsGUI_MaterialPropertiesDlg.cxx
// Author : Margarita KARPUNINA, Open CASCADE S.A.S. (margarita.karpunina@opencascade.com)

#include "GEOMToolsGUI_MaterialPropertiesDlg.h"

#include "Material_Model.h"
#include "Material_ResourceMgr.h"

#include <GeometryGUI.h>
#include <GEOM_Constants.h>
#include <GEOM_Displayer.h>
#include <GEOM_Actor.h>

#include <GEOMBase.h>

#include <Basics_OCCTVersion.hxx>

#include <SALOME_ListIO.hxx>
#include <SALOME_ListIteratorOfListIO.hxx>

#include <SVTK_Functor.h>
#include <SVTK_Prs.h>
#include <SVTK_ViewModel.h>
#include <SVTK_ViewWindow.h>
#include <SVTK_View.h>

#include <VTKViewer_Algorithm.h>

#include <OCCViewer_ViewModel.h>

#include <SUIT_Desktop.h>
#include <SUIT_MessageBox.h>
#include <SUIT_OverrideCursor.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SUIT_ViewManager.h>

#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>

#include <LightApp_SelectionMgr.h>

#include <QtxColorButton.h>
#include <QtxDoubleSpinBox.h>

// OCCT Includes
#include <Graphic3d_AspectFillArea3d.hxx>

// VTK includes
#include <vtkRenderer.h>
#include <vtkProperty.h>

// QT Includes
#include <QApplication>
#include <QButtonGroup>
#include <QCheckBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QListWidget>
#include <QMap>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QTabWidget>
#include <QVBoxLayout>

#define MARGIN  9
#define SPACING 6

/*!
  \class GEOMToolsGUI_MaterialPropertiesDlg
  \brief GEOM material properties dialog box class.

  The dialog box lists all GEOM materials available via the application and allows
  user to create own materials.
*/

/*!
  \brief Constructor
  \param parent parent widget
*/
GEOMToolsGUI_MaterialPropertiesDlg::GEOMToolsGUI_MaterialPropertiesDlg( QWidget* parent )
  : QtxDialog( parent, true, true, OK | Close | Apply | Help),
    myResMgr( 0 )
{
  // Set title
  setWindowTitle( tr( "MATERIAL_PROPERTIES_TLT" ) );

  // Set viewer type
  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  if ( app ) {
    SUIT_ViewWindow* window = app->desktop()->activeWindow();
    if ( window && window->getViewManager()->getType() == OCCViewer_Viewer::Type() )
      myViewerType = OCC;
    else if ( window && window->getViewManager()->getType() == SVTK_Viewer::Type() )
      myViewerType = VTK;
  }

  // Create main layout
  QVBoxLayout* main = new QVBoxLayout( mainFrame() );
  main->setMargin( 0 ); main->setSpacing( SPACING );

  // Create main widgets
  myBackMaterialCheck = new QCheckBox( tr( "MATERIAL_BACK_CHK" ), this );
  QFrame* fr = new QFrame( this );
  fr->setFrameStyle( QFrame::Box | QFrame::Sunken );

  main->addWidget( myBackMaterialCheck );
  main->addWidget( fr );

  // Create editor widgets
  myMaterialList = new QListWidget( fr );
  myMaterialTab  = new QTabWidget( fr );

  QHBoxLayout* frLayout = new QHBoxLayout( fr );
  frLayout->setMargin( MARGIN ); frLayout->setSpacing( SPACING );
  frLayout->addWidget( myMaterialList );
  frLayout->addWidget( myMaterialTab );
  frLayout->setStretchFactor( myMaterialList, 1 );
  frLayout->setStretchFactor( myMaterialTab, 2 );

  // ======================= Create a tab for front material ======================= 
  QWidget* w1 = new QWidget( myMaterialTab );
  QVBoxLayout* vLayout1 = new QVBoxLayout( w1 );

  QGridLayout* gLayout1 = new QGridLayout( w1 );
  gLayout1->setMargin( MARGIN ); gLayout1->setSpacing( SPACING );

  // ----------------- "Ambient" reflection type group box -----------------
  myAmbientGroupF = new QGroupBox( tr( "AMBIENT_GRP" ), w1 );
  myAmbientGroupF->setCheckable(true);
  connect( myAmbientGroupF, SIGNAL( toggled( bool ) ), this, SLOT( onReflectionTypeToggled( bool ) ) ); 

  // Ambient color
  QLabel* ambColorLab1 = new QLabel( tr( "COLOR" ), myAmbientGroupF );
  myAmbientColorF = new QtxColorButton( myAmbientGroupF );
  myAmbientColorF->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  connect( myAmbientColorF, SIGNAL( changed( QColor ) ), this, SIGNAL( changed() ) ); 

  // Ambient coefficient
  QLabel* ambCoefficientLab1 = new QLabel( tr( "COEFFICIENT" ), myAmbientGroupF );
  myAmbientCoefntF = new QtxDoubleSpinBox( myAmbientGroupF );
  myAmbientCoefntF->setMaximum(1);
  myAmbientCoefntF->setSingleStep(0.05);
  connect( myAmbientCoefntF, SIGNAL( valueChanged( double ) ), this, SIGNAL( materialChanged() ) );

  // Ambient group box layout
  QGridLayout* ambientLayout1 = new QGridLayout( myAmbientGroupF );
  ambientLayout1->setMargin( MARGIN ); ambientLayout1->setSpacing( SPACING );
  ambientLayout1->addWidget( ambColorLab1,       0, 0 );
  ambientLayout1->addWidget( myAmbientColorF,    0, 1 );
  ambientLayout1->addWidget( ambCoefficientLab1, 1, 0 );
  ambientLayout1->addWidget( myAmbientCoefntF,   1, 1 );

  // ----------------- "Diffuse" reflection type group box -----------------
  myDiffuseGroupF = new QGroupBox( tr( "DIFFUSE_GRP" ), w1 );
  myDiffuseGroupF->setCheckable(true);
  connect( myDiffuseGroupF, SIGNAL( toggled( bool ) ), this, SLOT( onReflectionTypeToggled( bool ) ) );

  // Diffuse color
  QLabel* difColorLab1 = new QLabel( tr( "COLOR" ), myDiffuseGroupF );
  myDiffuseColorF = new QtxColorButton( myDiffuseGroupF );
  myDiffuseColorF->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  connect( myDiffuseColorF, SIGNAL( changed( QColor ) ), this, SIGNAL( changed() ) ); 

  // Diffuse coefficient
  QLabel* difCoefficientLab1 = new QLabel( tr( "COEFFICIENT" ), myDiffuseGroupF );
  myDiffuseCoefntF = new QtxDoubleSpinBox( myDiffuseGroupF );
  myDiffuseCoefntF->setMaximum(1);
  myDiffuseCoefntF->setSingleStep(0.05);
  connect( myDiffuseCoefntF, SIGNAL( valueChanged( double ) ), this, SIGNAL( materialChanged() ) );

  // Diffuse group box layout
  QGridLayout* diffuseLayout1 = new QGridLayout( myDiffuseGroupF );
  diffuseLayout1->setMargin( MARGIN ); diffuseLayout1->setSpacing( SPACING );
  diffuseLayout1->addWidget( difColorLab1,       0, 0 );
  diffuseLayout1->addWidget( myDiffuseColorF,    0, 1 );
  diffuseLayout1->addWidget( difCoefficientLab1, 1, 0 );
  diffuseLayout1->addWidget( myDiffuseCoefntF,   1, 1 );

  // ----------------- "Specular" reflection type group box -----------------
  mySpecularGroupF = new QGroupBox( tr( "SPECULAR_GRP" ), w1 );
  mySpecularGroupF->setCheckable(true);
  connect( mySpecularGroupF, SIGNAL( toggled( bool ) ), this, SLOT( onReflectionTypeToggled( bool ) ) );

  // Specular color
  QLabel* specColorLab1 = new QLabel( tr( "COLOR" ), mySpecularGroupF );
  mySpecularColorF = new QtxColorButton( mySpecularGroupF );
  mySpecularColorF->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  connect( mySpecularColorF, SIGNAL( changed( QColor ) ), this, SIGNAL( changed() ) ); 

  // Specular coefficient
  QLabel* specCoefficientLab1 = new QLabel( tr( "COEFFICIENT" ), mySpecularGroupF );
  mySpecularCoefntF = new QtxDoubleSpinBox( mySpecularGroupF );
  mySpecularCoefntF->setMaximum(1);
  mySpecularCoefntF->setSingleStep(0.05);
  connect( mySpecularCoefntF, SIGNAL( valueChanged( double ) ), this, SIGNAL( materialChanged() ) );

  // Specular group box layout
  QGridLayout* specularLayout1 = new QGridLayout( mySpecularGroupF );
  specularLayout1->setMargin( MARGIN ); specularLayout1->setSpacing( SPACING );
  specularLayout1->addWidget( specColorLab1,       0, 0 );
  specularLayout1->addWidget( mySpecularColorF,    0, 1 );
  specularLayout1->addWidget( specCoefficientLab1, 1, 0 );
  specularLayout1->addWidget( mySpecularCoefntF,   1, 1 );

  // ----------------- "Emission" reflection type group box -----------------
  myEmissionGroupF = new QGroupBox( tr( "EMISSION_GRP" ), w1 );
  myEmissionGroupF->setCheckable(true);
  connect( myEmissionGroupF, SIGNAL( toggled( bool ) ), this, SLOT( onReflectionTypeToggled( bool ) ) );

  // Emission color
  QLabel* emisColorLab1 = new QLabel( tr( "COLOR" ), myEmissionGroupF );
  myEmissionColorF = new QtxColorButton( myEmissionGroupF );
  myEmissionColorF->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  connect( myEmissionColorF, SIGNAL( changed( QColor ) ), this, SIGNAL( changed() ) ); 

  // Emission coefficient
  QLabel* emisCoefficientLab1 = new QLabel( tr( "COEFFICIENT" ), myEmissionGroupF );
  myEmissionCoefntF = new QtxDoubleSpinBox( myEmissionGroupF );
  myEmissionCoefntF->setMaximum(1);
  myEmissionCoefntF->setSingleStep(0.05);
  connect( myEmissionCoefntF, SIGNAL( valueChanged( double ) ), this, SIGNAL( materialChanged() ) );

  // Emission group box layout
  QGridLayout* emissionLayout1 = new QGridLayout( myEmissionGroupF );
  emissionLayout1->setMargin( MARGIN ); emissionLayout1->setSpacing( SPACING );
  emissionLayout1->addWidget( emisColorLab1,       0, 0 );
  emissionLayout1->addWidget( myEmissionColorF,    0, 1 );
  emissionLayout1->addWidget( emisCoefficientLab1, 1, 0 );
  emissionLayout1->addWidget( myEmissionCoefntF,   1, 1 );

  // Erase emission group in case of VTK viewer
  if ( myViewerType == VTK )
    myEmissionGroupF->hide();

  // Add group boxes to the main grid layout of the frame with material properties
  gLayout1->addWidget( myAmbientGroupF,  0, 0 );
  gLayout1->addWidget( myDiffuseGroupF,  0, 1 );
  gLayout1->addWidget( mySpecularGroupF, 1, 0 );
  gLayout1->addWidget( myEmissionGroupF, 1, 1 );

  // Shininess
  QLabel* shininessLab1 = new QLabel( tr( "SHININESS" ), w1 );
  myShininessF = new QtxDoubleSpinBox( w1 );
  myShininessF->setMaximum(1);
  myShininessF->setSingleStep(0.05);
  connect( myShininessF, SIGNAL( valueChanged( double ) ), this, SIGNAL( materialChanged() ) );

  // Shininess layout
  QHBoxLayout* shLayout1 = new QHBoxLayout( w1 );
  shLayout1->setMargin( MARGIN ); shLayout1->setSpacing( SPACING );
  shLayout1->addWidget( shininessLab1 );
  shLayout1->addWidget( myShininessF );

  // Fill initial vertical layout of the reflection type group box
  vLayout1->addLayout( gLayout1 );
  vLayout1->addLayout( shLayout1 );
  vLayout1->addStretch();

  // ======================= Create a tab for back material ======================= 
  myMaterialBWidget = new QWidget( myMaterialTab );
  QVBoxLayout* vLayout2 = new QVBoxLayout( myMaterialBWidget );

  QGridLayout* gLayout2 = new QGridLayout( myMaterialBWidget );
  gLayout2->setMargin( MARGIN ); gLayout2->setSpacing( SPACING );

  // ----------------- "Ambient" reflection type group box -----------------
  myAmbientGroupB = new QGroupBox( tr( "AMBIENT_GRP" ), myMaterialBWidget );
  myAmbientGroupB->setCheckable(true);
  connect( myAmbientGroupB, SIGNAL( toggled( bool ) ), this, SLOT( onReflectionTypeToggled( bool ) ) ); 

  // Ambient color
  QLabel* ambColorLab2 = new QLabel( tr( "COLOR" ), myAmbientGroupB );
  myAmbientColorB = new QtxColorButton( myAmbientGroupB );
  myAmbientColorB->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  connect( myAmbientColorB, SIGNAL( changed( QColor ) ), this, SIGNAL( changed() ) ); 

  // Ambient coefficient
  QLabel* ambCoefficientLab2 = new QLabel( tr( "COEFFICIENT" ), myAmbientGroupB );
  myAmbientCoefntB = new QtxDoubleSpinBox( myAmbientGroupB );
  myAmbientCoefntB->setMaximum(1);
  myAmbientCoefntB->setSingleStep(0.05);
  connect( myAmbientCoefntB, SIGNAL( valueChanged( double ) ), this, SIGNAL( materialChanged() ) );

  // Ambient group box layout
  QGridLayout* ambientLayout2 = new QGridLayout( myAmbientGroupB );
  ambientLayout2->setMargin( MARGIN ); ambientLayout2->setSpacing( SPACING );
  ambientLayout2->addWidget( ambColorLab2,       0, 0 );
  ambientLayout2->addWidget( myAmbientColorB,    0, 1 );
  ambientLayout2->addWidget( ambCoefficientLab2, 1, 0 );
  ambientLayout2->addWidget( myAmbientCoefntB,   1, 1 );

  // ----------------- "Diffuse" reflection type group box -----------------
  myDiffuseGroupB = new QGroupBox( tr( "DIFFUSE_GRP" ), myMaterialBWidget );
  myDiffuseGroupB->setCheckable(true);
  connect( myDiffuseGroupB, SIGNAL( toggled( bool ) ), this, SLOT( onReflectionTypeToggled( bool ) ) );

  // Diffuse color
  QLabel* difColorLab2 = new QLabel( tr( "COLOR" ), myDiffuseGroupB );
  myDiffuseColorB = new QtxColorButton( myDiffuseGroupB );
  myDiffuseColorB->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  connect( myDiffuseColorB, SIGNAL( changed( QColor ) ), this, SIGNAL( changed() ) ); 

  // Diffuse coefficient
  QLabel* difCoefficientLab2 = new QLabel( tr( "COEFFICIENT" ), myDiffuseGroupB );
  myDiffuseCoefntB = new QtxDoubleSpinBox( myDiffuseGroupB );
  myDiffuseCoefntB->setMaximum(1);
  myDiffuseCoefntB->setSingleStep(0.05);
  connect( myDiffuseCoefntB, SIGNAL( valueChanged( double ) ), this, SIGNAL( materialChanged() ) );

  // Diffuse group box layout
  QGridLayout* diffuseLayout2 = new QGridLayout( myDiffuseGroupB );
  diffuseLayout2->setMargin( MARGIN ); diffuseLayout2->setSpacing( SPACING );
  diffuseLayout2->addWidget( difColorLab2,       0, 0 );
  diffuseLayout2->addWidget( myDiffuseColorB,    0, 1 );
  diffuseLayout2->addWidget( difCoefficientLab2, 1, 0 );
  diffuseLayout2->addWidget( myDiffuseCoefntB,   1, 1 );

  // ----------------- "Specular" reflection type group box -----------------
  mySpecularGroupB = new QGroupBox( tr( "SPECULAR_GRP" ), myMaterialBWidget );
  mySpecularGroupB->setCheckable(true);
  connect( mySpecularGroupB, SIGNAL( toggled( bool ) ), this, SLOT( onReflectionTypeToggled( bool ) ) );

  // Specular color
  QLabel* specColorLab2 = new QLabel( tr( "COLOR" ), mySpecularGroupB );
  mySpecularColorB = new QtxColorButton( mySpecularGroupB );
  mySpecularColorB->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  connect( mySpecularColorB, SIGNAL( changed( QColor ) ), this, SIGNAL( changed() ) ); 

  // Specular coefficient
  QLabel* specCoefficientLab2 = new QLabel( tr( "COEFFICIENT" ), mySpecularGroupB );
  mySpecularCoefntB = new QtxDoubleSpinBox( mySpecularGroupB );
  mySpecularCoefntB->setMaximum(1);
  mySpecularCoefntB->setSingleStep(0.05);
  connect( mySpecularCoefntB, SIGNAL( valueChanged( double ) ), this, SIGNAL( materialChanged() ) );

  // Specular group box layout
  QGridLayout* specularLayout2 = new QGridLayout( mySpecularGroupB );
  specularLayout2->setMargin( MARGIN ); specularLayout2->setSpacing( SPACING );
  specularLayout2->addWidget( specColorLab2,       0, 0 );
  specularLayout2->addWidget( mySpecularColorB,    0, 1 );
  specularLayout2->addWidget( specCoefficientLab2, 1, 0 );
  specularLayout2->addWidget( mySpecularCoefntB,   1, 1 );

  // ----------------- "Emission" reflection type group box -----------------
  myEmissionGroupB = new QGroupBox( tr( "EMISSION_GRP" ), myMaterialBWidget );
  myEmissionGroupB->setCheckable(true);
  connect( myEmissionGroupB, SIGNAL( toggled( bool ) ), this, SLOT( onReflectionTypeToggled( bool ) ) );

  // Emission color
  QLabel* emisColorLab2 = new QLabel( tr( "COLOR" ), myEmissionGroupB );
  myEmissionColorB = new QtxColorButton( myEmissionGroupB );
  myEmissionColorB->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  connect( myEmissionColorB, SIGNAL( changed( QColor ) ), this, SIGNAL( changed() ) ); 

  // Emission coefficient
  QLabel* emisCoefficientLab2 = new QLabel( tr( "COEFFICIENT" ), myEmissionGroupB );
  myEmissionCoefntB = new QtxDoubleSpinBox( myEmissionGroupB );
  myEmissionCoefntB->setMaximum(1);
  myEmissionCoefntB->setSingleStep(0.05);
  connect( myEmissionCoefntB, SIGNAL( valueChanged( double ) ), this, SIGNAL( materialChanged() ) );

  // Emission group box layout
  QGridLayout* emissionLayout2 = new QGridLayout( myEmissionGroupB );
  emissionLayout2->setMargin( MARGIN ); emissionLayout2->setSpacing( SPACING );
  emissionLayout2->addWidget( emisColorLab2,       0, 0 );
  emissionLayout2->addWidget( myEmissionColorB,    0, 1 );
  emissionLayout2->addWidget( emisCoefficientLab2, 1, 0 );
  emissionLayout2->addWidget( myEmissionCoefntB,   1, 1 );

  // Erase emission group in case of VTK viewer
  if ( myViewerType == VTK )
    myEmissionGroupB->hide();

  // Add group boxes to the main grid layout of the frame with material properties
  gLayout2->addWidget( myAmbientGroupB,  0, 0 );
  gLayout2->addWidget( myDiffuseGroupB,  0, 1 );
  gLayout2->addWidget( mySpecularGroupB, 1, 0 );
  gLayout2->addWidget( myEmissionGroupB, 1, 1 );

  // Shininess
  QLabel* shininessLab2 = new QLabel( tr( "SHININESS" ), myMaterialBWidget );
  myShininessB = new QtxDoubleSpinBox( myMaterialBWidget );
  myShininessB->setMaximum(1);
  myShininessB->setSingleStep(0.05);
  connect( myShininessB, SIGNAL( valueChanged( double ) ), this, SIGNAL( materialChanged() ) );

  // Shininess layout
  QHBoxLayout* shLayout2 = new QHBoxLayout( myMaterialBWidget );
  shLayout2->setMargin( MARGIN ); shLayout2->setSpacing( SPACING );
  shLayout2->addWidget( shininessLab2 );
  shLayout2->addWidget( myShininessB );

  // Fill initial vertical layout of the reflection type group box
  vLayout2->addLayout( gLayout2 );
  vLayout2->addLayout( shLayout2 );
  vLayout2->addStretch();

  // Add tabs to material tab widget
  myMaterialTab->addTab( w1, tr( "Front material" ) );
  myMaterialTab->addTab( myMaterialBWidget, tr( "Back material" ) );

  // Initialize dialog box
  setFocusProxy( fr );
  setButtonPosition( Right, Close );
  setDialogFlags( AlignOnce );
  myMaterialList->setEditTriggers( QAbstractItemView::EditKeyPressed );
  myMaterialList->installEventFilter( this );

  // ! RESOURCES
  QStringList globalMaterials = resourceMgr()->materials( Material_ResourceMgr::Global );
  QStringList userMaterials = resourceMgr()->materials( Material_ResourceMgr::User );

  QListWidgetItem* item;

  // Current material
  item = new QListWidgetItem( tr( "[ Current ]" ) );
  item->setForeground( QColor( Qt::red ) );
  item->setData( TypeRole, QVariant( Current ) );
  myMaterialList->addItem( item );
  // Default material
  item = new QListWidgetItem( tr( "[ Default ]" ) );
  item->setForeground( QColor( Qt::green ) );
  item->setData( TypeRole, QVariant( Default ) );
  myMaterialList->addItem( item );  
  // ! RESOURCES
  // Global materials
  foreach ( QString sname, globalMaterials ) {
    item = new QListWidgetItem( sname );
    item->setForeground( QColor( Qt::blue ) );
    item->setData( TypeRole, QVariant( Global ) );
    item->setData( NameRole, QVariant( sname ) );
    myMaterialList->addItem( item );
  }
  // ! RESOURCES
  // User materials
  foreach ( QString sname, userMaterials ) {
    item = new QListWidgetItem( sname );
    item->setData( TypeRole, QVariant( User ) );
    item->setData( NameRole, QVariant( sname ) );
    item->setFlags( item->flags() | Qt::ItemIsEditable );
    myMaterialList->addItem( item );
  }
  
  // Connect signals
  connect( myMaterialTab,       SIGNAL( currentChanged( int ) ),
	   this,                SLOT( onCurrentTabChanged( int ) ) );
  connect( myBackMaterialCheck, SIGNAL( toggled( bool ) ),
	   this,                SLOT( onBackMaterialChecked( bool ) ) );

  connect( myMaterialList,      SIGNAL( itemSelectionChanged() ),
	   this,                SLOT( onMaterialChanged() ) );
  connect( myMaterialList,      SIGNAL( itemChanged( QListWidgetItem* ) ),       
           this,                SLOT( onItemChanged( QListWidgetItem* ) ) );
  connect( myMaterialList,      SIGNAL( itemDoubleClicked( QListWidgetItem* ) ), 
           this,                SLOT( onApply() ) );

  connect( this, SIGNAL( changed() ),      this, SIGNAL( materialChanged() ) );
  connect( this, SIGNAL( materialChanged() ), this, SLOT( onChanged() ) );
  
  connect( this, SIGNAL( dlgApply() ), this, SLOT( onApply() ) );
  connect( this, SIGNAL( dlgHelp() ),  this, SLOT( onHelp() ) );

  // Initialize current fornt and back material models of the selected shape
  if ( app ) {
    LightApp_SelectionMgr* aSelMgr = app->selectionMgr();
    if ( aSelMgr ) {
      SalomeApp_Study* aStudy = dynamic_cast<SalomeApp_Study*>(app->activeStudy());
      if( aStudy ) {
	SALOME_ListIO selected;
	aSelMgr->selectedObjects( selected );

	Handle(SALOME_InteractiveObject) FirstIOS =  selected.First();
	if ( !FirstIOS.IsNull() ) {
	  SUIT_ViewWindow* window = app->desktop()->activeWindow();
	  int aMgrId = window->getViewManager()->getGlobalId();
	  
	  QString aMaterialF;
	  QString aMaterialB;

	  for ( SALOME_ListIteratorOfListIO It( selected ); It.More(); It.Next() ) {

	    PropMap aPropMap = aStudy->getObjectPropMap( aMgrId, It.Value()->getEntry() );
	    aMaterialF = aPropMap.value(FRONT_MATERIAL_PROP).toString();	    
	    aMaterialB = aPropMap.value(BACK_MATERIAL_PROP).toString();
	      
	    if ( !aMaterialF.isEmpty() ) {
	      
	      QStringList aPropsF = aMaterialF.split(DIGIT_SEPARATOR);
	      
	      myCurrentModelF = Material_Model::getMaterialModel( aPropsF );	        
	      
	      if ( !aMaterialB.isEmpty() ) {		
		QStringList aPropsB = aMaterialB.split(DIGIT_SEPARATOR);		
		myCurrentModelB = Material_Model::getMaterialModel( aPropsB );
		
		myBackMaterialCheck->setChecked( true );
	      }
	      else {
		myCurrentModelB = Material_Model::getMaterialModel( aPropsF );
		
		myBackMaterialCheck->setChecked( false );
		myMaterialTab->removeTab( 1 );
	      }
	      
	      break;
	    }
	  }
	    
	  if ( aMaterialF.isEmpty() ) {
	    myCurrentModelF = new Material_Model();
	    myCurrentModelF->fromResources( SUIT_Session::session()->resourceMgr(), "Geometry" );

	    myCurrentModelB = new Material_Model();
	    myCurrentModelB->fromResources( SUIT_Session::session()->resourceMgr(), "Geometry", false );
	  }
	}
      }
    }
  }

  myMaterialList->setCurrentRow( 0 );
  myMaterialListFId = 0;
  myMaterialListBId = 0;

  myIsBTabWasActivated = false;

  myHelpFileName = "material_page.html";
}

/*!
  \brief Destructor
*/
GEOMToolsGUI_MaterialPropertiesDlg::~GEOMToolsGUI_MaterialPropertiesDlg()
{
  if ( myCurrentModelF )
    delete myCurrentModelF;

  if ( myCurrentModelB )
    delete myCurrentModelB;
}

/*!
  \brief Called when "OK" button is clicked
*/
void GEOMToolsGUI_MaterialPropertiesDlg::accept()
{
  onApply();
  QtxDialog::accept();
}

/*!
  \brief Process key press event
  \param e key event
*/
bool GEOMToolsGUI_MaterialPropertiesDlg::eventFilter( QObject* o, QEvent* e )
{
  if ( o == myMaterialList && e->type() == QEvent::KeyPress ) {
    QKeyEvent* ke = (QKeyEvent*)e;
    if ( ke->key() == Qt::Key_Delete ) {
      QListWidgetItem* item = myMaterialList->currentItem();
      if ( item && item->data( TypeRole ).toInt() == User ) {
	if ( QMessageBox::question( this,
				    tr( "Delete user material" ),
				    tr( "Remove material %1?" ).arg( item->text() ),
				    QMessageBox::Yes | QMessageBox::No,
				    QMessageBox::Yes ) == QMessageBox::Yes ) {
	  resourceMgr()->remove( item->data( NameRole ).toString() );
	  resourceMgr()->save();
	  delete item;
	}
      }
    }
  }
  return QtxDialog::eventFilter( o, e );
}

/*!
  \brief Get GEOM materials resource manager
  \return materials resource manager
*/
Material_ResourceMgr* GEOMToolsGUI_MaterialPropertiesDlg::resourceMgr()
{
  if ( !myResMgr )
    myResMgr = new Material_ResourceMgr();
  return myResMgr;
}

/*!
  \brief Initialize dialog box fields from material model
  \param model material model
*/
void GEOMToolsGUI_MaterialPropertiesDlg::fromModel( Material_Model* model)
{
  if ( !model ) return;

  bool isReflectionTypeActive;

  if ( isFrontTabActive() ) { // Fill in front material tab
    
    // Ambient reflection type
    isReflectionTypeActive = model->hasAmbientReflection();
    myAmbientGroupF->setChecked( isReflectionTypeActive );
    if ( isReflectionTypeActive ) {
      // Load ambient color
      myAmbientColorF->setColor( model->color(Material_Model::Ambient) );
      // Load ambient coefficient
      myAmbientCoefntF->setValue( model->coefficient(Material_Model::Ambient) );
    }
    
    // Diffuse reflection type
    isReflectionTypeActive = model->hasDiffuseReflection();
    myDiffuseGroupF->setChecked( isReflectionTypeActive );
    if ( isReflectionTypeActive ) {
      // Load diffuse color
      myDiffuseColorF->setColor( model->color(Material_Model::Diffuse) );
      // Load diffuse coefficient
      myDiffuseCoefntF->setValue( model->coefficient(Material_Model::Diffuse) );
    }
    
    // Specular reflection type
    isReflectionTypeActive = model->hasSpecularReflection();
    mySpecularGroupF->setChecked( isReflectionTypeActive );
    if ( isReflectionTypeActive ) {
      // Load specular color
      mySpecularColorF->setColor( model->color(Material_Model::Specular) );
      // Load specular coefficient
      mySpecularCoefntF->setValue( model->coefficient(Material_Model::Specular) );
    }
    
    // Emission reflection type
    isReflectionTypeActive = model->hasEmissionReflection();
    myEmissionGroupF->setChecked( isReflectionTypeActive );
    if ( isReflectionTypeActive ) {
      // Load emission color
      myEmissionColorF->setColor( model->color(Material_Model::Emission) );
      // Load emission coefficient
      myEmissionCoefntF->setValue( model->coefficient(Material_Model::Emission) );
    }
    
    // Shininess
    myShininessF->setValue( model->shininess() );
    
  }
  else { // Fill in back material tab
    
    // Ambient reflection type
    isReflectionTypeActive = model->hasAmbientReflection();
    myAmbientGroupB->setChecked( isReflectionTypeActive );
    if ( isReflectionTypeActive ) {
      // Load ambient color
      myAmbientColorB->setColor( model->color(Material_Model::Ambient) );
      // Load ambient coefficient
      myAmbientCoefntB->setValue( model->coefficient(Material_Model::Ambient) );
    }
    
    // Diffuse reflection type
    isReflectionTypeActive = model->hasDiffuseReflection();
    myDiffuseGroupB->setChecked( isReflectionTypeActive );
    if ( isReflectionTypeActive ) {
      // Load diffuse color
      myDiffuseColorB->setColor( model->color(Material_Model::Diffuse) );
      // Load diffuse coefficient
      myDiffuseCoefntB->setValue( model->coefficient(Material_Model::Diffuse) );
    }
    
    // Specular reflection type
    isReflectionTypeActive = model->hasSpecularReflection();
    mySpecularGroupB->setChecked( isReflectionTypeActive );
    if ( isReflectionTypeActive ) {
      // Load specular color
      mySpecularColorB->setColor( model->color(Material_Model::Specular) );
      // Load specular coefficient
      mySpecularCoefntB->setValue( model->coefficient(Material_Model::Specular) );
    }
    
    // Emission reflection type
    isReflectionTypeActive = model->hasEmissionReflection();
    myEmissionGroupB->setChecked( isReflectionTypeActive );
    if ( isReflectionTypeActive ) {
      // Load emission color
      myEmissionColorB->setColor( model->color(Material_Model::Emission) );
      // Load emission coefficient
      myEmissionCoefntB->setValue( model->coefficient(Material_Model::Emission) );
    }
    
    // Shininess
    myShininessB->setValue( model->shininess() );
    
  }
}
 
/*!
  \brief Save values from dialog box fields to material model
  \param model material model
*/
void GEOMToolsGUI_MaterialPropertiesDlg::toModel( Material_Model* model ) const
{
  if ( !model ) return;
    
  if ( isFrontTabActive() )
    toFrontModel( model );
  else
    toBackModel( model );
}

/*!
  \brief Save values from dialog box fields to front material model
  \param model front material model to be filled
*/
void GEOMToolsGUI_MaterialPropertiesDlg::toFrontModel( Material_Model* model ) const
{
  if ( !model ) return;
    
  // "Ambient" reflection type
  if ( myAmbientGroupF->isChecked() ) {
    model->setColor( Material_Model::Ambient, myAmbientColorF->color() );
    model->setCoefficient( Material_Model::Ambient, myAmbientCoefntF->value() );
  }
  else {
    model->removeColor( Material_Model::Ambient );
    model->removeCoefficient( Material_Model::Ambient );
  }
  
  // "Diffuse" reflection type
  if ( myDiffuseGroupF->isChecked() ) {
    model->setColor( Material_Model::Diffuse, myDiffuseColorF->color() );
    model->setCoefficient( Material_Model::Diffuse, myDiffuseCoefntF->value() );
  }
  else {
    model->removeColor( Material_Model::Diffuse );
    model->removeCoefficient( Material_Model::Diffuse );
  }
  
  // "Specular" reflection type
  if ( mySpecularGroupF->isChecked() ) {
    model->setColor( Material_Model::Specular, mySpecularColorF->color() );
    model->setCoefficient( Material_Model::Specular, mySpecularCoefntF->value() );
  }
  else {
    model->removeColor( Material_Model::Specular );
    model->removeCoefficient( Material_Model::Specular );
  }
  
  // "Emission" reflection type
  if ( myEmissionGroupF->isChecked() ) {
    model->setColor( Material_Model::Emission, myEmissionColorF->color() );
    model->setCoefficient( Material_Model::Emission, myEmissionCoefntF->value() );
  }
  else {
    model->removeColor( Material_Model::Emission );
    model->removeCoefficient( Material_Model::Emission );
  }
  
  // Shininess
  model->setShininess( myShininessF->value() );
}

/*!
  \brief Save values from dialog box fields to back material model
  \param model back material model to be filled
*/
void GEOMToolsGUI_MaterialPropertiesDlg::toBackModel( Material_Model* model ) const
{
  if ( !model )
    return;

  // "Ambient" reflection type
  if ( myAmbientGroupB->isChecked() ) {
    model->setColor( Material_Model::Ambient, myAmbientColorB->color() );
    model->setCoefficient( Material_Model::Ambient, myAmbientCoefntB->value() );
  }
  else {
    model->removeColor( Material_Model::Ambient );
    model->removeCoefficient( Material_Model::Ambient );
  }
  
  // "Diffuse" reflection type
  if ( myDiffuseGroupB->isChecked() ) {
    model->setColor( Material_Model::Diffuse, myDiffuseColorB->color() );
    model->setCoefficient( Material_Model::Diffuse, myDiffuseCoefntB->value() );
  }
  else {
    model->removeColor( Material_Model::Diffuse );
    model->removeCoefficient( Material_Model::Diffuse );
  }
  
  // "Specular" reflection type
  if ( mySpecularGroupB->isChecked() ) {
    model->setColor( Material_Model::Specular, mySpecularColorB->color() );
    model->setCoefficient( Material_Model::Specular, mySpecularCoefntB->value() );
  }
  else {
    model->removeColor( Material_Model::Specular );
    model->removeCoefficient( Material_Model::Specular );
  }
  
  // "Emission" reflection type
  if ( myEmissionGroupB->isChecked() ) {
    model->setColor( Material_Model::Emission, myEmissionColorB->color() );
    model->setCoefficient( Material_Model::Emission, myEmissionCoefntB->value() );
  }
  else {
    model->removeColor( Material_Model::Emission );
    model->removeCoefficient( Material_Model::Emission );
  }
  
  // Shininess
  model->setShininess( myShininessB->value() );
}

/*!
  \brief Find unique name for the material name
  \param name material name template
  \param item if not 0, used to be ignored when browsing through items list
  \param addSuffix if \c true, the integrer suffix is always added to the material name (otherwise
  suffix is added only if item name is not unique)
  \return new unique material name
*/
QString GEOMToolsGUI_MaterialPropertiesDlg::findUniqueName( const QString& name, QListWidgetItem* item, bool addSuffix )
{
  bool found = false;
  int idx = 0;
  for( int i = 2; i < myMaterialList->count(); i++ ) {
    if ( item == myMaterialList->item( i ) ) continue;
    QString iname = myMaterialList->item( i )->text();
    if ( iname == name ) {
      found = true;
    }
    else {
      iname = iname.mid( name.length() ).trimmed();
      bool ok = false;
      int nx = iname.toInt( &ok );
      if ( ok ) idx = qMax( idx, nx );
    }
  }
  return found || addSuffix ? QString( "%1 %2" ).arg( name ).arg( idx+1 ) : name;
}

/*!
  \brief Check if tab with front material properties is currently active
  \return true if front material tab is active
*/
bool GEOMToolsGUI_MaterialPropertiesDlg::isFrontTabActive() const
{
  return ( myMaterialTab->currentIndex() == 0 ? true : false );
}

/*!
  \brief Called when "Apply" button is pressed
*/
void GEOMToolsGUI_MaterialPropertiesDlg::onApply()
{  
  // save user materials
  resourceMgr()->save();

  toFrontModel( myCurrentModelF );
  if ( myBackMaterialCheck->isChecked() && myIsBTabWasActivated )
    toBackModel( myCurrentModelB );

  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  if ( !app )
    return;
  LightApp_SelectionMgr* aSelMgr = app->selectionMgr();
  if ( !aSelMgr )
    return;
  
  SalomeApp_Study* aStudy = dynamic_cast<SalomeApp_Study*>(app->activeStudy());
  
  if(!aStudy)
    return;
  
  SALOME_ListIO selected;
  aSelMgr->selectedObjects( selected );
  if ( selected.IsEmpty() )
    return;

  Handle(SALOME_InteractiveObject) FirstIOS =  selected.First();
  if ( FirstIOS.IsNull() )
    return;
        
  SUIT_ViewWindow* window = app->desktop()->activeWindow();
  int aMgrId = window->getViewManager()->getGlobalId();

  // Parse material properties and form a string for persistent purpose
  QString aMaterialF = myCurrentModelF->getMaterialProperty();
  QString aMaterialB;
  if ( myBackMaterialCheck->isChecked() )
    aMaterialB = myCurrentModelB->getMaterialProperty();
  
  if ( myViewerType == VTK ) {    
    // Get material properties from the current model
    /*
    vtkProperty* aPropertyF;
    if ( !unsetMaterial )
      aPropertyF = myCurrentModelF->getMaterialVTKProperty();
    */
    vtkProperty* aPropertyF = myCurrentModelF->getMaterialVTKProperty();
    vtkProperty* aPropertyB = aPropertyF;
    if ( myBackMaterialCheck->isChecked() )
      aPropertyB = myCurrentModelB->getMaterialVTKProperty();

    SVTK_ViewWindow* vtkVW = dynamic_cast<SVTK_ViewWindow*>( window );
    if ( !vtkVW )
      return;
    SVTK_View* aView = vtkVW->getView();

    SUIT_OverrideCursor();
    for ( SALOME_ListIteratorOfListIO It( selected ); It.More(); It.Next() ) {

      /*
      if ( unsetMaterial || aMaterialF.isEmpty() )
	// Unset material for the selected shape
	aisShape->UnsetMaterial();
      else
	// Set material for the selected shape
      */
	aView->SetMaterial( It.Value(), aPropertyF, aPropertyB );

      // Persistent
      aStudy->setObjectProperty( aMgrId, It.Value()->getEntry(), FRONT_MATERIAL_PROP, aMaterialF );
      aStudy->setObjectProperty( aMgrId, It.Value()->getEntry(), BACK_MATERIAL_PROP, aMaterialB );
    } // for...
    aView->Repaint();
    GeometryGUI::Modified();
  } // if ( VTK )
        
  else if ( myViewerType == OCC ) {
    // Get material properties from the current model
    /*
    Graphic3d_MaterialAspect aMatF;
    if ( !unsetMaterial )
      aMatF = myCurrentModelF->getMaterialOCCAspect();
    */
    Graphic3d_MaterialAspect aMatF = myCurrentModelF->getMaterialOCCAspect();
    Graphic3d_MaterialAspect aMatB = aMatF;
    if ( myBackMaterialCheck->isChecked() )
      aMatB = myCurrentModelB->getMaterialOCCAspect();

    Handle(GEOM_AISShape) aisShape;
    
    SUIT_OverrideCursor();
    OCCViewer_Viewer* vm = dynamic_cast<OCCViewer_Viewer*>( window->getViewManager()->getViewModel() );
    if ( !vm )
      return;

    GEOMBase* gb = new GEOMBase();

    Handle(AIS_InteractiveContext) ic = vm->getAISContext();
    for ( SALOME_ListIteratorOfListIO It( selected ); It.More(); It.Next() ) {
      aisShape = gb->ConvertIOinGEOMAISShape( It.Value(), true );
      if ( !aisShape.IsNull() ) {

	if(!aisShape->HasInteractiveContext())
	  aisShape->SetContext(ic);

	/*
	if ( unsetMaterial || aMaterialF.isEmpty() )
	  // Unset material for the selected shape
	  aisShape->UnsetMaterial();
	else
	*/
	  if ( myBackMaterialCheck->isChecked() ) {
	    // Set front material for the selected shape
	    aisShape->SetCurrentFacingModel(Aspect_TOFM_FRONT_SIDE);
	    aisShape->SetMaterial(aMatF);
	    // Set back material for the selected shape
	    aisShape->SetCurrentFacingModel(Aspect_TOFM_BACK_SIDE);
	    aisShape->SetMaterial(aMatB);
	    // Return to the default facing mode
	    aisShape->SetCurrentFacingModel(Aspect_TOFM_BOTH_SIDE);
	  }
	  else {
	    // Set the same front and back (is equal to front) materials for the selected shape
	    aisShape->SetMaterial(aMatF);
	  }

	if (aisShape->DisplayMode() != AIS_Shaded/*aisShape->DisplayMode() == GEOM_AISShape::ShadingWithEdges*/)
	  ic->RecomputePrsOnly( aisShape, Standard_False );

	// Persistent	
	aStudy->setObjectProperty( aMgrId, It.Value()->getEntry(), FRONT_MATERIAL_PROP, aMaterialF );
	aStudy->setObjectProperty( aMgrId, It.Value()->getEntry(), BACK_MATERIAL_PROP, aMaterialB );
      }
    } // for...    
    ic->UpdateCurrentViewer();
    GeometryGUI::Modified();
  } // if ( OCC )
}

/*!
  \brief Called when "Help" button is pressed
*/
void GEOMToolsGUI_MaterialPropertiesDlg::onHelp()
{
  LightApp_Application* app = (LightApp_Application*)(SUIT_Session::session()->activeApplication());
  if (app) {
    GeometryGUI* aGeomGUI = dynamic_cast<GeometryGUI*>( app->module( "Geometry" ) );
    app->onHelpContextModule(aGeomGUI ? app->moduleName(aGeomGUI->moduleName()) : QString(""), myHelpFileName);
  }
  else {
                QString platform;
#ifdef WIN32
                platform = "winapplication";
#else
                platform = "application";
#endif
    SUIT_MessageBox::warning(0, QObject::tr("WRN_WARNING"),
                             QObject::tr("EXTERNAL_BROWSER_CANNOT_SHOW_PAGE").
                             arg(app->resourceMgr()->stringValue("ExternalBrowser", platform)).arg(myHelpFileName),
                             QObject::tr("BUT_OK"));
  }  
}

/*!
  \brief Called when user check/uncheck "Enable back material" check box
  \param theIsChecked the check state of the check box
*/
void GEOMToolsGUI_MaterialPropertiesDlg::onBackMaterialChecked( bool theIsChecked )
{
  if ( theIsChecked ) {
    // Tab with back material properties is displayed
    myMaterialTab->addTab( myMaterialBWidget, tr( "Back material" ) );

    // Create a current model of back material
    if ( !myCurrentModelB ) {
      myCurrentModelB = new Material_Model();
      myCurrentModelB->fromResources( SUIT_Session::session()->resourceMgr(), "Geometry", false );
    }
    
    myMaterialListBId = 0;
  }
  else {
    // Tab with back material properties is hidden
    myMaterialTab->removeTab( 1 );
    
    // Remove the current model for back material
    if ( myCurrentModelB ) {
      delete myCurrentModelB;
      myCurrentModelB = 0;
    }
  }
}

/*!
  \brief Called when user activates material tab
  \param theIndex the index of the tab which was activated by the user
*/
void GEOMToolsGUI_MaterialPropertiesDlg::onCurrentTabChanged(int theIndex)
{
  blockSignals( true );

  // Change selection in the list of materials
  if ( isFrontTabActive() )
    myMaterialList->setCurrentRow( myMaterialListFId );
  else if ( myBackMaterialCheck->isChecked() )
    myMaterialList->setCurrentRow( myMaterialListBId );    

  if ( theIndex == 1 )
    myIsBTabWasActivated = true;

  blockSignals( false );

  onMaterialChanged();
}

/*!
  \brief Called when user selects any material item in the materials list
*/
void GEOMToolsGUI_MaterialPropertiesDlg::onMaterialChanged()
{
  blockSignals( true );

  QListWidgetItem* item = myMaterialList->currentItem();
  int type = item->data( TypeRole ).toInt();
  
  Material_Model* model = 0;

  bool isFrontTab = isFrontTabActive();

  if ( isFrontTab )
    myMaterialListFId = myMaterialList->currentRow();
  else
    myMaterialListBId = myMaterialList->currentRow();

  switch ( type ) {    
  case Current:
    // current material
    model = ( isFrontTab ? myCurrentModelF : myCurrentModelB );
    break;    
  case Default:
    // default material
    model = new Material_Model();
    model->fromResources( SUIT_Session::session()->resourceMgr(), "Geometry", ( isFrontTab ? true : false ) );
    break;    
  case Global:
  case User:
    // global material, user material
    model = new Material_Model();
    model->fromResources( resourceMgr(), item->data( NameRole ).toString() );
    break;
  default:
    break;
  }

  fromModel( model );
  if ( type != Current )
    delete model;

  blockSignals( false );
}

/*!
  \brief Called when any material parameter is changed by the user
*/
void GEOMToolsGUI_MaterialPropertiesDlg::onChanged()
{
  QListWidgetItem* item = myMaterialList->currentItem();
  int type = item->data( TypeRole ).toInt();

  bool isFrontTab = isFrontTabActive();

  // for the current and user schemas do not perform any actions
  if ( type == Current ) {    
    Material_Model model = ( isFrontTab ? *( myCurrentModelF ) : *( myCurrentModelB ) );
    toModel( &model );
    model.save( 0, QString(), isFrontTab );
    blockSignals( true );
    fromModel( &model );
    blockSignals( false );    
  }
  else if ( type == User ) {    
    Material_Model model;
    toModel( &model );
    QString oldName = item->data( NameRole ).toString(), newName = item->text();
    if ( oldName == newName ) {
      model.save( resourceMgr(), oldName, isFrontTab );
    }
    else {
      resourceMgr()->remove( oldName );
      model.save( resourceMgr(), newName, isFrontTab );
      item->setData( NameRole, newName );
    }
    blockSignals( true );
    fromModel( &model );
    blockSignals( false );    
  }
  else {
    // if user tries to change global (or default, or no material) material,
    // we create a new user material basing on selected one
    QString newName = findUniqueName( tr( "CUSTOM_MATERIAL" ), 0, true );
    item = new QListWidgetItem( newName );
    item->setData( TypeRole, QVariant( User ) );
    item->setData( NameRole, QVariant( newName ) );
    item->setFlags( item->flags() | Qt::ItemIsEditable );
    myMaterialList->addItem( item );
    
    Material_Model model;
    toModel( &model );
    model.save( resourceMgr(), newName, isFrontTab );
    
    myMaterialList->setCurrentItem( item );

    if ( isFrontTab )
      myMaterialListFId = myMaterialList->currentRow();
    else
      myMaterialListBId = myMaterialList->currentRow();

  }
}

/*!
  \brief Called when user material is renamed by the user
*/
void GEOMToolsGUI_MaterialPropertiesDlg::onItemChanged( QListWidgetItem* item )
{
  QString newName = item->text();
  QString uniqueName = findUniqueName( newName, item );
  if ( uniqueName != newName ) {
    myMaterialList->blockSignals( true );
    item->setText( uniqueName );
    myMaterialList->blockSignals( false );
  }
  onChanged();
}                               

/*!
  \brief Called when widget effect is changed
*/
void GEOMToolsGUI_MaterialPropertiesDlg::onReflectionTypeToggled( bool theIsOn )
{
  QGroupBox* anObj = (QGroupBox*)sender();

  // Set an empty values for color and coefficient
  // of the checked/unchecked reflection type
  QColor c;


  // Make changes on front material tab
  if ( anObj == myAmbientGroupF ) {    
    myAmbientColorF->setColor( c );
    myAmbientCoefntF->setValue( 0.0 );
  }
  else if ( anObj == myDiffuseGroupF ) {
    myDiffuseColorF->setColor( c );
    myDiffuseCoefntF->setValue( 0.0 );
  }
  else if ( anObj == mySpecularGroupF ) {
    mySpecularColorF->setColor( c );
    mySpecularCoefntF->setValue( 0.0 );
  }
  else if ( anObj == myEmissionGroupF ) {
    myEmissionColorF->setColor( c );
    myEmissionCoefntF->setValue( 0.0 );
  }

  // Make changes on back material tab    
  if ( anObj == myAmbientGroupB ) {    
    myAmbientColorB->setColor( c );
    myAmbientCoefntB->setValue( 0.0 );
  }
  else if ( anObj == myDiffuseGroupB ) {
    myDiffuseColorB->setColor( c );
    myDiffuseCoefntB->setValue( 0.0 );
  }
  else if ( anObj == mySpecularGroupB ) {
    mySpecularColorB->setColor( c );
    mySpecularCoefntB->setValue( 0.0 );
  }
  else if ( anObj == myEmissionGroupB ) {
    myEmissionColorB->setColor( c );
    myEmissionCoefntB->setValue( 0.0 );
  }
  
  emit( changed() );
}
