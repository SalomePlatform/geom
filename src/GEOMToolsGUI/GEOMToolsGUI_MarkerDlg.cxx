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

// File   : GEOMToolsGUI_MarkerDlg.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

#include "GEOMToolsGUI_MarkerDlg.h"

#include <GeometryGUI.h>
#include <GEOM_Constants.h>
#include <GEOM_Displayer.h>

#include <Basics_OCCTVersion.hxx>

#include <QtxComboBox.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SUIT_ViewWindow.h>
#include <SUIT_Desktop.h>
#include <SUIT_ViewManager.h>
#include <LightApp_SelectionMgr.h>
#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>
#include <SALOME_ListIO.hxx>

#include <QButtonGroup>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QStackedWidget>

#define MARGIN  9
#define SPACING 6

GEOMToolsGUI_MarkerDlg::GEOMToolsGUI_MarkerDlg( QWidget* parent )
  : QDialog ( parent )
{
  setWindowTitle( tr( "SET_MARKER_TLT" ) );
  setSizeGripEnabled( true );
  setModal( true );

  // ---

  QRadioButton* aStandardTypeRB = new QRadioButton( tr( "STANDARD_MARKER" ), this );
  QRadioButton* aCustomTypeRB   = new QRadioButton( tr( "CUSTOM_MARKER" ), this );
  myTypeGroup = new QButtonGroup( this );
  myTypeGroup->addButton( aStandardTypeRB, 0 );
  myTypeGroup->addButton( aCustomTypeRB,   1 );

  QHBoxLayout* aRadioLayout = new QHBoxLayout;
  aRadioLayout->setMargin( 0 );
  aRadioLayout->setSpacing( SPACING );
  aRadioLayout->addWidget( aStandardTypeRB );
  aRadioLayout->addWidget( aCustomTypeRB );

  // ---

  myWGStack = new QStackedWidget( this );
  myWGStack->setFrameStyle( QFrame::Box | QFrame::Sunken );

  // ---

  QWidget* aStdWidget = new QWidget( myWGStack );

  QLabel* aTypeLab  = new QLabel( tr( "TYPE" ),  aStdWidget );
  QLabel* aScaleLab = new QLabel( tr( "SCALE" ), aStdWidget );

  myStdTypeCombo  = new QtxComboBox( aStdWidget );
  myStdScaleCombo = new QtxComboBox( aStdWidget );

  QGridLayout* aStdLayout = new QGridLayout;
  aStdLayout->setMargin( MARGIN );
  aStdLayout->setSpacing( SPACING );
  aStdLayout->addWidget( aTypeLab,        0, 0 );
  aStdLayout->addWidget( myStdTypeCombo,  0, 1 );
  aStdLayout->addWidget( aScaleLab,       1, 0 );
  aStdLayout->addWidget( myStdScaleCombo, 1, 1 );
  aStdWidget->setLayout( aStdLayout );

  // ---

  QWidget* aCustomWidget = new QWidget( myWGStack );

  QLabel* aCustomLab = new QLabel( tr( "CUSTOM" ), aCustomWidget );
  myCustomTypeCombo = new QtxComboBox( aCustomWidget );
  QPushButton* aBrowseBtn = new QPushButton( tr( "BROWSE" ), aCustomWidget );

  QGridLayout* aCustomLayout = new QGridLayout;
  aCustomLayout->setMargin( MARGIN );
  aCustomLayout->setSpacing( SPACING );
  aCustomLayout->addWidget( aCustomLab,        0, 0 );
  aCustomLayout->addWidget( myCustomTypeCombo, 0, 1 );
  aCustomLayout->addWidget( aBrowseBtn,       0, 2 );
  aCustomLayout->setRowStretch( 1, 5 );
  aCustomWidget->setLayout( aCustomLayout );

  // ---
  
  myWGStack->insertWidget( 0, aStdWidget );
  myWGStack->insertWidget( 1, aCustomWidget );

  // ---
  
  QPushButton* aOkBtn     = new QPushButton( tr( "OK_BTN" ),     this );
  aOkBtn->setAutoDefault( true ); aOkBtn->setDefault( true ); 
  QPushButton* aCancelBtn = new QPushButton( tr( "CANCEL_BTN" ), this );
  aCancelBtn->setAutoDefault( true );
  QPushButton* aHelpBtn   = new QPushButton( tr( "HELP_BTN" ), this );
  aHelpBtn->setAutoDefault( true );

  QHBoxLayout* aBtnLayout = new QHBoxLayout;
  aBtnLayout->setMargin( 0 );
  aBtnLayout->setSpacing( SPACING );
  aBtnLayout->addWidget( aOkBtn );
  aBtnLayout->addSpacing( 10 );
  aBtnLayout->addStretch();
  aBtnLayout->addWidget( aCancelBtn );
  aBtnLayout->addWidget( aHelpBtn );

  // ---

  QVBoxLayout* aTopLayout = new QVBoxLayout;
  aTopLayout->setMargin( MARGIN );
  aTopLayout->setSpacing( SPACING );
  aTopLayout->addLayout( aRadioLayout );
  aTopLayout->addWidget( myWGStack );
  aTopLayout->addLayout( aBtnLayout );
  setLayout( aTopLayout );

  // ---

  connect( myTypeGroup, SIGNAL( buttonClicked( int ) ), myWGStack, SLOT( setCurrentIndex( int ) ) );
  connect( aBrowseBtn,  SIGNAL( clicked() ), this, SLOT( browse() ) );
  connect( aOkBtn,      SIGNAL( clicked() ), this, SLOT( accept() ) );
  connect( aCancelBtn,  SIGNAL( clicked() ), this, SLOT( reject() ) );
  connect( aHelpBtn,    SIGNAL( clicked() ), this, SLOT( help() ) );

  // ---

  aStandardTypeRB->setChecked( true );
  init();
}

GEOMToolsGUI_MarkerDlg::~GEOMToolsGUI_MarkerDlg()
{
  myOperation->UnRegister();
}

void GEOMToolsGUI_MarkerDlg::setStandardMarker( GEOM::marker_type type, GEOM::marker_size size )
{
  if ( type > GEOM::MT_NONE && type < GEOM::MT_USER ) {
    myTypeGroup->button( 0 )->setChecked( true );
    myWGStack->setCurrentIndex( 0 );
    myStdTypeCombo->setCurrentId( (int)type );
#ifdef WIN32
    int asize = max( (int)GEOM::MS_10, min( (int)GEOM::MS_70, (int)size ) );
#else
    int asize = std::max( (int)GEOM::MS_10, std::min( (int)GEOM::MS_70, (int)size ) );
#endif
    myStdScaleCombo->setCurrentId( asize );
  }
}

void GEOMToolsGUI_MarkerDlg::setCustomMarker( int id )
{
  if ( id > 0 ) {
    myTypeGroup->button( 1 )->setChecked( true );
    myWGStack->setCurrentIndex( 1 );
    addTexture( id );
    myCustomTypeCombo->setCurrentId( id );
  }
}

GEOM::marker_type GEOMToolsGUI_MarkerDlg::getMarkerType() const
{
  return myWGStack->currentIndex() == 0 ? (GEOM::marker_type)myStdTypeCombo->currentId().toInt() : GEOM::MT_USER;
}

GEOM::marker_size GEOMToolsGUI_MarkerDlg::getStandardMarkerScale() const
{
  return myWGStack->currentIndex() == 0 ? (GEOM::marker_size)myStdScaleCombo->currentId().toInt() : GEOM::MS_NONE;
}

int GEOMToolsGUI_MarkerDlg::getCustomMarkerID() const
{
  return myWGStack->currentIndex() == 1 ? myCustomTypeCombo->currentId().toInt() : 0;
}

void GEOMToolsGUI_MarkerDlg::accept()
{
  if ( getStudy() ) {
    LightApp_SelectionMgr* selMgr = qobject_cast<SalomeApp_Application*>( getStudy()->application() )->selectionMgr();
    
    SUIT_ViewWindow* window =  getStudy()->application()->desktop()->activeWindow();
    if (window && window->getViewManager()) {
      int mgrId = window->getViewManager()->getGlobalId();
      if ( selMgr ) {
        SALOME_ListIO selected;
        selMgr->selectedObjects( selected );
        if ( !selected.IsEmpty() ) {
          _PTR(Study) study = getStudy()->studyDS();
          for ( SALOME_ListIteratorOfListIO it( selected ); it.More(); it.Next() ) {
            _PTR(SObject) aSObject( study->FindObjectID( it.Value()->getEntry() ) );
            GEOM::GEOM_Object_var anObject =
              GEOM::GEOM_Object::_narrow( GeometryGUI::ClientSObjectToObject( aSObject ) );
            if ( !anObject->_is_nil() ) {
              if ( myWGStack->currentIndex() == 0 ) {
                anObject->SetMarkerStd( getMarkerType(), getStandardMarkerScale() );
                QString aMarker = "%1%2%3";
                aMarker = aMarker.arg(getMarkerType());
                aMarker = aMarker.arg(GEOM::subSectionSeparator());
                aMarker = aMarker.arg(getStandardMarkerScale());
                getStudy()->setObjectProperty(mgrId ,it.Value()->getEntry(),GEOM::propertyName( GEOM::PointMarker ), aMarker);
              }
              else if ( getCustomMarkerID() > 0 ) {
                anObject->SetMarkerTexture( getCustomMarkerID() );
                getStudy()->setObjectProperty(mgrId ,it.Value()->getEntry(),GEOM::propertyName( GEOM::PointMarker ), QString::number(getCustomMarkerID()));
              }
            }
          }
          GEOM_Displayer displayer( getStudy() );
          displayer.Redisplay( selected, true );
          selMgr->setSelectedObjects( selected );
        }
      }
    }
  }
  QDialog::accept();
}

void GEOMToolsGUI_MarkerDlg::keyPressEvent( QKeyEvent* e )
{
  if ( e->key() == Qt::Key_F1 )
    help();
  QDialog::keyPressEvent( e );
}

void GEOMToolsGUI_MarkerDlg::init()
{
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();

  // ---

  for ( int i = GEOM::MT_POINT; i < GEOM::MT_USER; i++ ) {
    QString icoFile = QString( "ICON_VERTEX_MARKER_%1" ).arg( i );
    QPixmap pixmap = resMgr->loadPixmap( "GEOM", tr( qPrintable( icoFile ) ) );
    myStdTypeCombo->addItem( pixmap, QString() );
    myStdTypeCombo->setId( myStdTypeCombo->count()-1, i );
  }

  // ---

  for ( int i = GEOM::MS_10; i <= GEOM::MS_70; i++ ) {
    myStdScaleCombo->addItem( QString::number( (i-1)*0.5 + 1.0 ) );
    myStdScaleCombo->setId( myStdScaleCombo->count()-1, i );
  }

  // ---
  
  GEOM::GEOM_Gen_var engine = GeometryGUI::GetGeomGen();
  myOperation = engine->GetIInsertOperations( getStudy()->id() );
  GEOM::ListOfLong_var ids = myOperation->GetAllTextures();
  for ( int i = 0; i < ids->length(); i++ )
    addTexture( ids[i] );

  // ---

  GEOM::marker_type aType    = (GEOM::marker_type)-1;
  GEOM::marker_size aSize    = (GEOM::marker_size)-1;
  int               aTexture = 0;
  if ( getStudy() ) {
    LightApp_SelectionMgr* selMgr = qobject_cast<SalomeApp_Application*>( getStudy()->application() )->selectionMgr();
    if ( selMgr ) {
      SALOME_ListIO selected;
      selMgr->selectedObjects( selected );
      if ( !selected.IsEmpty() ) {
        _PTR(Study) study = getStudy()->studyDS();
        for ( SALOME_ListIteratorOfListIO it( selected ); it.More(); it.Next() ) {
          _PTR(SObject) aSObject( study->FindObjectID( it.Value()->getEntry() ) );
          GEOM::GEOM_Object_var anObject =
            GEOM::GEOM_Object::_narrow( GeometryGUI::ClientSObjectToObject( aSObject ) );
          if ( !anObject->_is_nil() ) {
            GEOM::marker_type mtype = anObject->GetMarkerType();
            if ( aType == -1 )
              aType = mtype;
            else if ( aType != mtype ) {
              aType = (GEOM::marker_type)-1;
              break;
            }
            if ( mtype > GEOM::MT_NONE && mtype < GEOM::MT_USER ) {
              GEOM::marker_size msize = anObject->GetMarkerSize();
              if ( aSize == -1 )
                aSize = msize;
              else if ( aSize != msize )
                break;
            }
            else if ( mtype == GEOM::MT_USER ) {
              int mtexture = anObject->GetMarkerTexture();
              if ( aTexture == 0 )
                aTexture = mtexture;
              else if ( aTexture != mtexture )
                break;
            }
          }
        }
      }
    }
  }
  if ( aType > GEOM::MT_NONE && aType < GEOM::MT_USER )
    setStandardMarker( aType, aSize );
  else if ( aType == GEOM::MT_USER )
    setCustomMarker( aTexture );
  else
    setStandardMarker((GEOM::marker_type)(resMgr->integerValue("Geometry", "type_of_marker", (int)Aspect_TOM_PLUS) + 1),
                      (GEOM::marker_size)(resMgr->integerValue("Geometry", "marker_scale", 1)));
}

void GEOMToolsGUI_MarkerDlg::addTexture( int id, bool select ) const
{
  if ( id > 0 && myCustomTypeCombo->index( id ) == -1 ) {
    int tWidth, tHeight;

    Handle(TColStd_HArray1OfByte) texture = GeometryGUI::getTexture(getStudy(), id, tWidth, tHeight);

    if ( !texture.IsNull() && texture->Length() == tWidth*tHeight/8 ) {
      QImage image( tWidth, tHeight, QImage::Format_Mono );
      image.setColor( 0, qRgba( 0, 0, 0, 0   ) );
      image.setColor( 1, qRgba( 0, 0, 0, 255 ) );
      int bytesperline = tWidth/8;
      for ( int j = texture->Lower(); j <= texture->Upper(); j++ ) {
        uchar val = (uchar)texture->Value( j );
        for ( int k = 0; k < 8; k++ ) {
          int row = ( j - texture->Lower() ) / bytesperline;
          int col = ( ( j - texture->Lower() ) % bytesperline ) * 8 + k;
          image.setPixel( row, col, ( val & (1<<(8-k-1)) ) ? 1 : 0 );
        }
      }
      QPixmap pixmap = QPixmap::fromImage( image );
      if ( !pixmap.isNull() ) {
        myCustomTypeCombo->addItem( pixmap, QString::number( id ) );
        myCustomTypeCombo->setId( myCustomTypeCombo->count()-1, id );
        if ( select ) myCustomTypeCombo->setCurrentId( id );
      }
    }
  }
}

SalomeApp_Study* GEOMToolsGUI_MarkerDlg::getStudy() const
{
  return qobject_cast<SalomeApp_Study*>( SUIT_Session::session()->activeApplication()->activeStudy() );
}

void GEOMToolsGUI_MarkerDlg::browse()
{
  QStringList filters;
  filters << tr( "Texture files (*.dat)" ) << tr( "All files (*)" );
  QString aFileName = SUIT_Session::session()->activeApplication()->getFileName( true, QString(), filters.join( ";;" ), tr( "LOAD_TEXTURE_TLT" ), this );
  if ( !aFileName.isEmpty() ) {
    addTexture( myOperation->LoadTexture( aFileName.toLatin1().constData() ), true );
  }
}

void GEOMToolsGUI_MarkerDlg::help()
{
  if ( getStudy() ) {
    SalomeApp_Application* app = qobject_cast<SalomeApp_Application*>( getStudy()->application() );
    app->onHelpContextModule( "GEOM", "point_marker_page.html" );
  }
}
