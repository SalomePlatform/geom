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

// File   : GEOMToolsGUI_MaterialPropertiesDlg.cxx
// Author : Margarita KARPUNINA, Open CASCADE S.A.S. (margarita.karpunina@opencascade.com)

#include "GEOMToolsGUI_MaterialPropertiesDlg.h"

#include "GeometryGUI.h"
#include "GEOM_Constants.h"
#include "GEOM_VTKPropertyMaterial.hxx"
#include "GEOMBase.h"
#include "GEOM_Displayer.h"

#include <QtxColorButton.h>
#include <QtxDoubleSpinBox.h>
#include <SUIT_Desktop.h>
#include <SUIT_MessageBox.h>
#include <SUIT_OverrideCursor.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SUIT_ViewManager.h>
#include <SALOME_ListIO.hxx>
#include <OCCViewer_ViewModel.h>
#include <SVTK_ViewModel.h>
#include <SVTK_ViewWindow.h>
#include <SVTK_View.h>
#include <LightApp_SelectionMgr.h>
#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>
#include <SalomeApp_Tools.h>

#include <QCheckBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QMap>
#include <QMenu>
#include <QPushButton>
#include <QVBoxLayout>

#define MARGIN  9  // layout spacing
#define SPACING 6  // layout margin

/*!
  \class GEOMToolsGUI_MaterialList
  \brief Internal class, used to handle context menu event from materials list
  \internal
*/

/*!
  \brief Contructor
 */
GEOMToolsGUI_MaterialList::GEOMToolsGUI_MaterialList( QWidget* parent )
  : QListWidget( parent )
{}

/*!
  \brief Context menu event, emits context menu signal passing event as parameter
*/
void GEOMToolsGUI_MaterialList::contextMenuEvent( QContextMenuEvent* e )
{
  emit contextMenu( e );
}

/*!
  \class GEOMToolsGUI_MaterialPropertiesDlg
  \brief GEOM material properties dialog box class

  The dialog box is used to set material properties for the presentation objects.
*/

/*!
  \brief Constructor
  \param parent parent widget
  \param showSelWidget if \c true then objects can be selected by user
  \param modal if \c true dialog box is modal
  \param f specified control buttons for dialog box (QtxDialog::ButtonFlags)
*/
GEOMToolsGUI_MaterialPropertiesDlg::GEOMToolsGUI_MaterialPropertiesDlg( QWidget* parent,bool showSelWidget, bool modal,const int f )
  : QtxDialog( parent, modal, true, f )
{
  myShowSelWidget = showSelWidget;
  // Set title
  setWindowTitle( tr( "MATERIAL_PROPERTIES_TLT" ) );
        
  // main layout
  QVBoxLayout* main = new QVBoxLayout( mainFrame() );
  main->setMargin( 0 );
  main->setSpacing( SPACING );

  // add top-level frame box to enclose all main widgets (excluding buttons)
  QFrame* fr = new QFrame( mainFrame() );
  fr->setFrameStyle( QFrame::Box | QFrame::Sunken );
  main->addWidget( fr );
  // selection widget
  myLineEditCurArg = new QLineEdit(fr);
  myLineEditCurArg->setReadOnly(true);
  myPushBtn = new QPushButton(fr);
  QLabel* PushBtnLab = new QLabel( tr( "GEOM_OBJECTS" ), fr );

  // materials list widget
  myMaterials = new GEOMToolsGUI_MaterialList( fr );
  // properties widget
  QWidget* propWidget = new QWidget( fr );

  // layout main widgets
  QHBoxLayout* frLayout = new QHBoxLayout( fr );
  frLayout->setMargin( MARGIN );
  frLayout->setSpacing( SPACING );
  frLayout->addWidget( myMaterials );
  frLayout->addWidget( propWidget );

  // layout for material properties widgets
  QGridLayout* propLayout = new QGridLayout( propWidget );
  propLayout->setMargin( 0 );
  propLayout->setSpacing( SPACING );

  // current color widgets
  myColorLab = new QLabel( tr( "CURRENT_COLOR" ), propWidget );
  myColor = new QtxColorButton( propWidget );

  // "physical" material type widgets
  myPhysical = new QCheckBox( tr( "PHYSICAL" ), propWidget );

  // Labels for front and back reflection components
  QLabel* aFrontLabel = new QLabel( tr( "FRONT_FACE" ), propWidget );
  QLabel* aBackLabel = new QLabel( tr( "BACK_FACE" ), propWidget );

  // reflection components widgets
  for ( int i = Material_Model::Ambient; i <= Material_Model::Emissive; i++ )
  {
    Reflection refl;
    
    refl.enabled = new QCheckBox( propWidget );

    refl.label = new QLabel( tr( QString( "REFLECTION_%1" ).arg( i ).toLatin1().data() ), propWidget );

    refl.color = new QtxColorButton( propWidget );
    //refl.color->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );

    refl.front_coef = new QtxDoubleSpinBox( propWidget );
    refl.front_coef->setPrecision( 4 );
    refl.front_coef->setDecimals( 4 ); 
    refl.front_coef->setRange( 0., 1. );
    refl.front_coef->setSingleStep( 0.05 );
    refl.front_coef->setMinimumWidth( 80 );
    
    refl.back_coef = new QtxDoubleSpinBox( propWidget );
    refl.back_coef->setPrecision( 4 );
    refl.back_coef->setDecimals( 4 ); 
    refl.back_coef->setRange( 0., 1. );
    refl.back_coef->setSingleStep( 0.05 );
    refl.back_coef->setMinimumWidth( 80 );

    myReflection << refl;
  }

  // shininess widgets
  QLabel* shininessLab = new QLabel( tr( "SHININESS" ), propWidget );
  myFrontShininess = new QtxDoubleSpinBox( propWidget );
  myFrontShininess->setPrecision( 4 );
  myFrontShininess->setDecimals( 4 ); 
  myFrontShininess->setRange( 0., 1. );
  myFrontShininess->setSingleStep( 0.05 );

  myBackShininess = new QtxDoubleSpinBox( propWidget );
  myBackShininess->setPrecision( 4 );
  myBackShininess->setDecimals( 4 ); 
  myBackShininess->setRange( 0., 1. );
  myBackShininess->setSingleStep( 0.05 );

  // separator widgets
  QFrame* line1 = new QFrame( propWidget );
  line1->setFrameStyle( QFrame::HLine | QFrame::Sunken );
  QFrame* line2 = new QFrame( propWidget );
  line2->setFrameStyle( QFrame::HLine | QFrame::Sunken );
  QFrame* line3 = new QFrame( propWidget );
  line3->setFrameStyle( QFrame::VLine | QFrame::Sunken );
  QFrame* line4 = new QFrame( propWidget );
  line4->setFrameStyle( QFrame::VLine | QFrame::Sunken );
  QFrame* line5 = new QFrame( propWidget );
  line5->setFrameStyle( QFrame::HLine | QFrame::Sunken );

  // add / remove material buttons
  myAddButton = new QPushButton( tr( "ADD_MATERIAL" ),    propWidget );
  myDelButton = new QPushButton( tr( "DELETE_MATERIAL" ), propWidget );

  // buttons layout
  QHBoxLayout* btnLayout = new QHBoxLayout;
  btnLayout->setMargin( 0 );
  btnLayout->setSpacing( SPACING );
  btnLayout->addWidget( myAddButton );
  btnLayout->addWidget( myDelButton );

  // layout all properties widgets together
  propLayout->addWidget( PushBtnLab,0,0);
  propLayout->addWidget( myPushBtn,0,1);
  propLayout->addWidget( myLineEditCurArg, 0, 2, 1, 5 );
  propLayout->addWidget( line1, 1, 0, 1, 7 );
  propLayout->addWidget( myColorLab, 2, 1 );
  propLayout->addWidget( myColor,    2, 2 );
  propLayout->addWidget( myPhysical, 2, 0 );
  propLayout->addWidget( line2, 3, 0, 1, 7 );
  propLayout->addWidget( aFrontLabel, 4, 4);
  propLayout->addWidget( aBackLabel, 4, 6);
  
  propLayout->addWidget( line3, 4, 3, 6, 1 );
  propLayout->addWidget( line4, 4, 5, 6, 1 );

  if( !myShowSelWidget ) {
    myColorLab->hide();
    myColor->hide();
    line1->hide();
    myLineEditCurArg->hide();
    myPushBtn->hide();
    PushBtnLab->hide();
  }
  for ( int i = Material_Model::Ambient; i <= Material_Model::Emissive; i++ ) {
    propLayout->addWidget( myReflection[i].enabled, i+5, 0 );
    propLayout->addWidget( myReflection[i].label,   i+5, 1 );
    propLayout->addWidget( myReflection[i].color,   i+5, 2 );
    propLayout->addWidget( myReflection[i].front_coef,    i+5, 4 );
    propLayout->addWidget( myReflection[i].back_coef,    i+5, 6 );
  }

  propLayout->addWidget( shininessLab,   9, 0 );
  propLayout->addWidget( myFrontShininess,    9, 4 );
  propLayout->addWidget( myBackShininess,    9, 6 );
  propLayout->addWidget( line5, 10, 0, 1, 7 );
  propLayout->setRowStretch( 11, 5 );
  propLayout->addLayout( btnLayout, 12, 0, 1, 7 );

  // initialize dialog box
  setButtonPosition( Right, Close );

  // fill in the list of materials
  QStringList globalMaterials = myResourceMgr.materials( Material_ResourceMgr::Global );
  QStringList userMaterials   = myResourceMgr.materials( Material_ResourceMgr::User );

  QListWidgetItem* item;

  // - current material
  item = new QListWidgetItem( tr( "CURRENT_MATERIAL" ) );
  item->setForeground( QColor( Qt::red ) );
  item->setData( TypeRole, QVariant( Current ) );
  myMaterials->addItem( item );
  // - global materials
  foreach( QString material, globalMaterials ) {
    item = new QListWidgetItem( material );
    item->setForeground( QColor( Qt::blue ) );
    item->setData( TypeRole, QVariant( Global ) );
    item->setData( NameRole, QVariant( material ) );
    myMaterials->addItem( item );
  }
  // - user materials
  foreach ( QString material, userMaterials ) {
    item = new QListWidgetItem( material );
    item->setData( TypeRole, QVariant( User ) );
    item->setData( NameRole, QVariant( material ) );
    item->setFlags( item->flags() | Qt::ItemIsEditable );
    myMaterials->addItem( item );
  }
  // install event filter to the materials list to process key press events
  myMaterials->installEventFilter( this );

  //Set image
  myPushBtn->setIcon(SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_SELECT")));

  // connect signals
  // note: all widgets, that change material properties, are connected to the common signal
  // changed(), instead of connecting directly to the slot - this allows easy temporary blocking 
  // of the change signal
  connect( myPhysical, SIGNAL( toggled( bool ) ), this, SIGNAL( changed() ) ); 
  for ( int i = Material_Model::Ambient; i <= Material_Model::Emissive; i++ ) {
    connect( myReflection[i].color,   SIGNAL( changed( QColor ) ),   this, SIGNAL( changed() ) );
    connect( myReflection[i].front_coef,    SIGNAL( valueChanged( double ) ), this, SIGNAL( changed() ) );
    connect( myReflection[i].back_coef,    SIGNAL( valueChanged( double ) ), this, SIGNAL( changed() ) );
    connect( myReflection[i].enabled, SIGNAL( toggled( bool ) ),     this, SIGNAL( changed() ) );
  }
  connect( myFrontShininess, SIGNAL( valueChanged( double ) ), this, SIGNAL( changed() ) );
  connect( myBackShininess, SIGNAL( valueChanged( double ) ), this, SIGNAL( changed() ) );
  connect( myMaterials, SIGNAL( itemSelectionChanged() ),
           this,        SLOT( onMaterialChanged() ) );
  connect( myMaterials, SIGNAL( itemChanged( QListWidgetItem* ) ),
           this,        SLOT( onItemChanged( QListWidgetItem* ) ) );
  connect( myMaterials, SIGNAL( contextMenu( QContextMenuEvent* ) ),
           this,        SLOT( onContextMenu( QContextMenuEvent* ) ) );
  connect( myAddButton, SIGNAL( clicked() ), this, SLOT( onAddMaterial() ) );
  connect( myDelButton, SIGNAL( clicked() ), this, SLOT( onDeleteMaterial() ) );
  connect( this, SIGNAL( dlgApply() ), this, SLOT( onApply() ) );
  connect( this, SIGNAL( dlgHelp() ),  this, SLOT( onHelp() ) );
  connect( this, SIGNAL( changed() ),  this, SLOT( onChanged() ) );
  connect(myPushBtn, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(((SalomeApp_Application*)(SUIT_Session::session()->activeApplication()))->selectionMgr(),
           SIGNAL(currentSelectionChanged()), this, SLOT(SelectionIntoArgument()));

  // initialize current material model according to the selection
  myColor->setColor( SUIT_Session::session()->resourceMgr()->colorValue( "Geometry", "shading_color", QColor( 255, 0, 0 ) ) );
  myCurrentModel.fromResources( SUIT_Session::session()->resourceMgr()->stringValue( "Geometry", "material", "Plastic" ) );
  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>( app->activeStudy() );
  LightApp_SelectionMgr* selMgr = app->selectionMgr();
  if ( study ) {
    SALOME_ListIO selected;
    selMgr->selectedObjects( selected );
    if ( selected.Extent() > 0 ) {
      Handle(SALOME_InteractiveObject) io = selected.First();
      if ( !io.IsNull() ) {
        SUIT_ViewWindow* window = app->desktop()->activeWindow();
        if ( window ) {
          int mgrId = window->getViewManager()->getGlobalId();
          PropMap propMap = study->getObjectProperties( mgrId, io->getEntry() );
          QString matProp = propMap.value(GEOM::propertyName( GEOM::Material )).toString();         
          if ( !matProp.isEmpty() )
            myCurrentModel.fromProperties( matProp );
          QColor c = propMap.value(GEOM::propertyName( GEOM::Color )).value<QColor>();
          if ( c.isValid() )
            myColor->setColor( c );
        }
      }
    }
  }
  myLineEditCurArg->setText(""); 
  myPushBtn->click();
  SelectionIntoArgument();

  // finally activate current material properties
  myMaterials->setCurrentRow( 0 );
}

/*!
  \brief Destructor
*/
GEOMToolsGUI_MaterialPropertiesDlg::~GEOMToolsGUI_MaterialPropertiesDlg()
{
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
  \brief Event filter
  \param o sender
  \param e event
*/
bool GEOMToolsGUI_MaterialPropertiesDlg::eventFilter( QObject* o, QEvent* e )
{
  // process key press event from materials list
  if ( o == myMaterials && e->type() == QEvent::KeyPress ) {
    QKeyEvent* ke = (QKeyEvent*)e;
    if ( ke->key() == Qt::Key_Delete ) {
      onDeleteMaterial();
    }
  }
  return QtxDialog::eventFilter( o, e );
}

/*!
  \brief Initialize dialog box widgets from material model
  \param model material model
*/
void GEOMToolsGUI_MaterialPropertiesDlg::fromModel( const Material_Model& model )
{
  // reflection components
  for ( int i = Material_Model::Ambient; i <= Material_Model::Emissive; i++ )
  {
    myReflection[i].color->setColor( model.color( (Material_Model::ReflectionType)i ) );
    myReflection[i].front_coef->setValue( model.reflection( (Material_Model::ReflectionType)i, true ) );
    myReflection[i].back_coef->setValue( model.reflection( (Material_Model::ReflectionType)i, false ) );
    myReflection[i].enabled->setChecked( model.hasReflection( (Material_Model::ReflectionType)i ) );
  }
  
  // shininess
  myFrontShininess->setValue( model.shininess( true ) );
  myBackShininess->setValue( model.shininess( false ) );

  // type (physical or no)
  myPhysical->setChecked( model.isPhysical() );
}
 
/*!
  \brief Save values from dialog box widgets to material model
  \param model material model to be filled in
*/
void GEOMToolsGUI_MaterialPropertiesDlg::toModel( Material_Model& model ) const
{
  // type (physical or no)
  model.setPhysical( myPhysical->isChecked() );

  // shininess
  model.setShininess( myFrontShininess->value(), true );
  model.setShininess( myBackShininess->value(), false );

  // reflection components
  for ( int i = Material_Model::Ambient; i <= Material_Model::Emissive; i++ )
  {
    model.setColor     ( (Material_Model::ReflectionType)i, myReflection[i].color->color() );
    model.setReflection( (Material_Model::ReflectionType)i, myReflection[i].front_coef->value(), true );
    model.setReflection( (Material_Model::ReflectionType)i, myReflection[i].back_coef->value(), false );
    model.setReflection( (Material_Model::ReflectionType)i, myReflection[i].enabled->isChecked() );
  }
}

/*!
  \brief Find unique name for the material name
  \param name material name template
  \param item the item to be ignored when browsing through the materials list
  \param addSuffix if \c true, the integrer suffix is always added to the material name (otherwise
  suffix is added only if item name is not unique)
  \return new unique material name
*/
QString GEOMToolsGUI_MaterialPropertiesDlg::findUniqueName( const QString& name, QListWidgetItem* item, bool addSuffix )
{
  bool found = false;
  int idx = 0;
  for( int i = 1; i < myMaterials->count(); i++ ) {
    if ( item == myMaterials->item( i ) ) continue;
    QString iname = myMaterials->item( i )->text();
    if ( iname == name ) {
      found = true;
    }
    else if ( iname.startsWith( name ) ) {
      iname = iname.mid( name.length() ).trimmed();
      bool ok = false;
      int nx = iname.toInt( &ok );
      if ( ok ) idx = qMax( idx, nx );
    }
  }
  return found || addSuffix ? QString( "%1 %2" ).arg( name ).arg( idx+1 ) : name;
}

/*!
  \brief Called when "Apply" button is pressed
*/
void GEOMToolsGUI_MaterialPropertiesDlg::onApply()
{  
  // save user materials
  myResourceMgr.save();
  if( !myShowSelWidget ) return;
  // store selected material properties in the current model
  toModel( myCurrentModel );

  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  LightApp_SelectionMgr* selMgr = app->selectionMgr();
  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>( app->activeStudy() );
  
  if ( !study ) return;

  // get selection
  SALOME_ListIO selected;
  selMgr->selectedObjects( selected );
  if ( selected.IsEmpty() ) return;

  SUIT_ViewWindow* window = app->desktop()->activeWindow();
  int mgrId = window->getViewManager()->getGlobalId();

  // convert current material properties to the string representation
  QString prop = myCurrentModel.toProperties();
 
  if ( window && window->getViewManager()->getType() == SVTK_Viewer::Type() ) {
    // for VTK viewer
    SVTK_ViewWindow* vtkVW = dynamic_cast<SVTK_ViewWindow*>( window );
    if ( !vtkVW )
      return;

    SVTK_View* aView = vtkVW->getView();

    // get VTK material properties from the current model
    GEOM_VTKPropertyMaterial* vtkPropF = myCurrentModel.getMaterialVTKProperty();
    GEOM_VTKPropertyMaterial* vtkPropB = myCurrentModel.getMaterialVTKProperty( false );

    SUIT_OverrideCursor wc();

    for ( SALOME_ListIteratorOfListIO It( selected ); It.More(); It.Next() ) {
      // set material property to the presentation
      aView->SetMaterial( It.Value(), vtkPropF, vtkPropB );
      // store chosen material in the property map
      study->setObjectProperty( mgrId, It.Value()->getEntry(), GEOM::propertyName( GEOM::Material ), prop );
      // set correct color for the non-physical material
      if ( !myCurrentModel.isPhysical() ) {
        aView->SetColor( It.Value(), myColor->color() );
        study->setObjectProperty( mgrId, It.Value()->getEntry(), GEOM::propertyName( GEOM::Color ), myColor->color() );
      }
    }
    aView->Repaint();
    GeometryGUI::Modified();
  }
  else if ( window && window->getViewManager()->getType() == OCCViewer_Viewer::Type() ) {    
    // for OCC viewer
    OCCViewer_Viewer* vm = dynamic_cast<OCCViewer_Viewer*>( window->getViewManager()->getViewModel() );
    if ( !vm )
      return;

    Handle(AIS_InteractiveContext) ic = vm->getAISContext();

    // get OCC material aspect from the current model
    Graphic3d_MaterialAspect front_occAspect = myCurrentModel.getMaterialOCCAspect( true );
    Graphic3d_MaterialAspect back_occAspect = myCurrentModel.getMaterialOCCAspect( false );

    SUIT_OverrideCursor wc();

    for ( SALOME_ListIteratorOfListIO It( selected ); It.More(); It.Next() ) {
      Handle(GEOM_AISShape) aisShape = GEOMBase::ConvertIOinGEOMAISShape( It.Value(), true );
      if ( !aisShape.IsNull() ) {
        // Set front material for the selected shape
        aisShape->SetCurrentFacingModel(Aspect_TOFM_FRONT_SIDE);
        aisShape->SetMaterial(front_occAspect);
        // Set back material for the selected shape
        aisShape->SetCurrentFacingModel(Aspect_TOFM_BACK_SIDE);
        aisShape->SetMaterial(back_occAspect);
        // Return to the default facing mode
        aisShape->SetCurrentFacingModel(Aspect_TOFM_BOTH_SIDE);
        // store chosen material in the property map
        study->setObjectProperty( mgrId, It.Value()->getEntry(), GEOM::propertyName( GEOM::Material ), prop );
        // set correct color for the non-physical material
        if ( !myCurrentModel.isPhysical() ) {
          aisShape->SetShadingColor( SalomeApp_Tools::color( myColor->color() ) );
          study->setObjectProperty( mgrId, It.Value()->getEntry(), GEOM::propertyName( GEOM::Color ), myColor->color() );
          ic->RecomputePrsOnly( aisShape, Standard_False );
        }
        //if ( aisShape->DisplayMode() != AIS_Shaded/*aisShape->DisplayMode() == GEOM_AISShape::ShadingWithEdges*/)
        ic->Redisplay( aisShape, Standard_False );
      }
    }
    ic->UpdateCurrentViewer();
    GeometryGUI::Modified();
  }
}

/*!
  \brief Called when "Help" button is pressed
*/
void GEOMToolsGUI_MaterialPropertiesDlg::onHelp()
{
  LightApp_Application* app = (LightApp_Application*)(SUIT_Session::session()->activeApplication());
  app->onHelpContextModule( "GEOM", "material_page.html" );
}

/*!
  \brief Called when user selects any material item in the materials list
*/
void GEOMToolsGUI_MaterialPropertiesDlg::onMaterialChanged()
{
  // get currently selected item
  QListWidgetItem* item = myMaterials->currentItem();
  if ( !item ) return;

  bool blocked = blockSignals( true );

  int type = item->data( TypeRole ).toInt();
  if ( type == Current ) {
    // initialize widgets from current material model
    fromModel( myCurrentModel );
  }
  else {
    // initialize widgets from chosen material model (using resources manager)
    Material_Model model;
    model.fromResources( item->data( NameRole ).toString(), &myResourceMgr );
    fromModel( model );
  }

  blockSignals( blocked );

  // update buttons state
  updateState();
}

/*!
  \brief Called when any material parameter is changed by the user
*/
void GEOMToolsGUI_MaterialPropertiesDlg::onChanged()
{
  // get currently selected item
  QListWidgetItem* item = myMaterials->currentItem();
  int type = item->data( TypeRole ).toInt();
  if ( !item ) return;

  if ( type == Current ) {    
    // for the current model do not perform any actions except store changed values
    toModel( myCurrentModel );
  }
  else if ( type == User ) {    
    // for the user model, simply store the changes in the resource manager
    Material_Model model;
    toModel( model );
    // check if the user model is renamed
    QString oldName = item->data( NameRole ).toString(), newName = item->text();
    if ( oldName == newName ) {
      // model is not renamed: store data using current name
      model.toResources( oldName, &myResourceMgr );
    }
    else {
      // model is renamed: remove model with old name and store model using new name
      myResourceMgr.remove( oldName );
      model.toResources( newName, &myResourceMgr );
      item->setData( NameRole, newName );
    }
  }
  else {
    // it is no allowed to change global material
    // user is asked about creating of a new user material model based on the currently selected one
    if ( SUIT_MessageBox::question( this,
                                    tr( "GEOM_WRN_WARNING" ),
                                    tr( "QUE_CREATE_NEW_MATERIAL" ),
                                    QMessageBox::Yes | QMessageBox::No,
                                    QMessageBox::Yes ) == QMessageBox::Yes ) {
      // user has chosen creation of new user model
      onAddMaterial();
    }
    else {
      // user has rejected creation of new user model: revert changes
      bool blocked = blockSignals( true );
      Material_Model model;
      model.fromResources( item->data( NameRole ).toString(), &myResourceMgr );
      fromModel( model );
      blockSignals( blocked );
    }
  }

  // update buttons state
  updateState();
}

/*!
  \brief Called when user material is renamed by the user
*/
void GEOMToolsGUI_MaterialPropertiesDlg::onItemChanged( QListWidgetItem* item )
{
  // check new name to be unique (add suffix if necessary)
  QString newName = item->text();
  QString uniqueName = findUniqueName( newName, item );
  if ( uniqueName != newName ) {
    bool blocked = myMaterials->blockSignals( true );
    item->setText( uniqueName );
    myMaterials->blockSignals( blocked );
  }
  onChanged();
}                               

/*!
  \brief Process context menu event from materials list
*/
void GEOMToolsGUI_MaterialPropertiesDlg::onContextMenu( QContextMenuEvent* e )
{
  QListWidgetItem* item = myMaterials->itemAt( e->pos() );
  QMap<QAction*, int> actionMap;
  QMenu m;
  // rename
  if ( item && item->data( TypeRole ).toInt() == User ) {
    actionMap[ m.addAction( tr( "RENAME_MATERIAL" ) ) ] = 0;
    m.addSeparator();
  }
  // add user material
  actionMap[ m.addAction( tr( "ADD_MATERIAL" ) ) ] = 1;
  // delete user material
  if ( item && item->data( TypeRole ).toInt() == User ) {
    actionMap[ m.addAction( tr( "DELETE_MATERIAL" ) ) ] = 2;
  }
  QAction* a = m.exec( e->globalPos() );
  switch( actionMap[ a ] ) {
  case 0:
    // rename
    myMaterials->editItem( item );
    break;
  case 1:
    // add user material
    onAddMaterial();
    break;
  case 2:
    // delete user material
    onDeleteMaterial();
    break;
  default:
    break;
  }
}

/*!
  \brief Delete currently selected user model
*/
void GEOMToolsGUI_MaterialPropertiesDlg::onDeleteMaterial()
{
  QListWidgetItem* item = myMaterials->currentItem();
  if ( item && item->data( TypeRole ).toInt() == User ) {
    if ( SUIT_MessageBox::question( this,
                                    tr( "GEOM_WRN_WARNING" ),
                                    tr( "QUE_REMOVE_MATERIAL" ).arg( item->text() ),
                                    QMessageBox::Yes | QMessageBox::No,
                                    QMessageBox::Yes ) == QMessageBox::Yes ) {
      myResourceMgr.remove( item->data( NameRole ).toString() );
      delete item;
    }
  }
}

/*!
  \brief Add new user material model
*/
void GEOMToolsGUI_MaterialPropertiesDlg::onAddMaterial()
{
  QString newName = findUniqueName( tr( "CUSTOM_MATERIAL" ), 0, true );
  QListWidgetItem* item = new QListWidgetItem( newName );
  item->setData( TypeRole, QVariant( User ) );
  item->setData( NameRole, QVariant( newName ) );
  item->setFlags( item->flags() | Qt::ItemIsEditable );
  myMaterials->addItem( item );
  
  Material_Model model;
  toModel( model );
  model.toResources( newName, &myResourceMgr );
  myMaterials->setCurrentItem( item );
  myMaterials->editItem( item );
}

/*!
  \brief Update buttons state
*/
void GEOMToolsGUI_MaterialPropertiesDlg::updateState()
{
  QListWidgetItem* item = myMaterials->currentItem();
  myDelButton->setEnabled( item && item->data( TypeRole ).toInt() == User );
  myColorLab->setEnabled( !myPhysical->isChecked() );
  myColor->setEnabled( !myPhysical->isChecked() );
  myReflection[0].color->setEnabled( myPhysical->isChecked() );
}


void GEOMToolsGUI_MaterialPropertiesDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  if (send == myPushBtn) {
  // enable line edit
  myLineEditCurArg->setEnabled(true);
  myLineEditCurArg->setFocus();
        SelectionIntoArgument();
  // after setFocus(), because it will be setDown(false) when loses focus
  send->setDown(true);
  }
}

/*!
  \brief  Called when selection as changed or other case
*/
void GEOMToolsGUI_MaterialPropertiesDlg::SelectionIntoArgument()
{
  myLineEditCurArg->setText( "" );
  QString aString = "";

  SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>( app->activeStudy() );
  LightApp_SelectionMgr* aSelMgr = app->selectionMgr();
  if ( study ) {
    SALOME_ListIO aSelList;
    aSelMgr->selectedObjects(aSelList);
                int nbSel = GEOMBase::GetNameOfSelectedIObjects(aSelList, aString, true);
        }
  myLineEditCurArg->setText( aString );
}
