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
//  File   : RepairGUI.cxx
//  Author : Damien COQUERET, Open CASCADE S.A.S.

#include "RepairGUI.h"

#include <GeometryGUI.h>
#include "GeometryGUI_Operations.h"

#include <SUIT_Desktop.h>
#include <SUIT_Session.h>
#include <SUIT_OverrideCursor.h>
#include <SalomeApp_Application.h>

#include "RepairGUI_SewingDlg.h"        // Method SEWING
#include "RepairGUI_SuppressFacesDlg.h" // Method SUPPRESS FACES
#include "RepairGUI_RemoveHolesDlg.h"   // Method SUPPRESS HOLE
#include "RepairGUI_ShapeProcessDlg.h"  // Method PROCESS SHAPE
#include "RepairGUI_CloseContourDlg.h"  // Method CLOSE CONTOUR
#include "RepairGUI_RemoveIntWiresDlg.h"// Method REMOVE INTERNAL WIRES
#include "RepairGUI_DivideEdgeDlg.h"    // Method DEVIDE EDGE
#include "RepairGUI_FreeBoundDlg.h"     // Method FREE BOUNDARIES
#include "RepairGUI_FreeFacesDlg.h"     // Method FREE FACES
#include "RepairGUI_GlueDlg.h"          // Method GLUE FACES
#include "RepairGUI_LimitToleranceDlg.h"    // Method LIMIT TOLERANCE
#include "RepairGUI_ChangeOrientationDlg.h" // Method CHANGE ORIENTATION
#include "RepairGUI_RemoveWebsDlg.h"        // Method REMOVE INTERNAL FACES
#include "RepairGUI_RemoveExtraEdgesDlg.h"  // Method REMOVE EXTRA EDGES
#include "RepairGUI_FuseEdgesDlg.h"         // Method FUSE COLLINEAR EDGES
#include "RepairGUI_UnionFacesDlg.h"        // Method UNION FACES
#include "RepairGUI_InspectObjectDlg.h"     // Method INSPECT OBJECT

//=======================================================================
// function : RepairGUI()
// purpose  : Constructor
//=======================================================================
RepairGUI::RepairGUI( GeometryGUI* parent )
  : GEOMGUI( parent )
{
}

//=======================================================================
// function : ~RepairGUI()
// purpose  : Destructor
//=======================================================================
RepairGUI::~RepairGUI()
{
}


//=======================================================================
// function : OnGUIEvent()
// purpose  :
//=======================================================================
bool RepairGUI::OnGUIEvent( int theCommandID, SUIT_Desktop* parent )
{
  SalomeApp_Application* app = getGeometryGUI()->getApp();
  if ( !app ) return false;

  getGeometryGUI()->EmitSignalDeactivateDialog();

  QDialog* aDlg = NULL;
  switch (theCommandID) {
  case GEOMOp::OpSewing:           aDlg = new RepairGUI_SewingDlg            (getGeometryGUI(), parent); break;
  case GEOMOp::OpGlueFaces: aDlg = new RepairGUI_GlueDlg (getGeometryGUI(), parent, false, TopAbs_FACE); break;
  case GEOMOp::OpGlueEdges: aDlg = new RepairGUI_GlueDlg (getGeometryGUI(), parent, false, TopAbs_EDGE); break;
  case GEOMOp::OpLimitTolerance:   aDlg = new RepairGUI_LimitToleranceDlg    (getGeometryGUI(), parent); break;
  case GEOMOp::OpSuppressFaces:    aDlg = new RepairGUI_SuppressFacesDlg     (getGeometryGUI(), parent); break;
  case GEOMOp::OpSuppressHoles:    aDlg = new RepairGUI_RemoveHolesDlg       (getGeometryGUI(), parent); break;
  case GEOMOp::OpShapeProcess:     aDlg = new RepairGUI_ShapeProcessDlg      (getGeometryGUI(), parent); break;
  case GEOMOp::OpCloseContour:     aDlg = new RepairGUI_CloseContourDlg      (getGeometryGUI(), parent); break;
  case GEOMOp::OpRemoveIntWires:   aDlg = new RepairGUI_RemoveIntWiresDlg    (getGeometryGUI(), parent); break;
  case GEOMOp::OpAddPointOnEdge:   aDlg = new RepairGUI_DivideEdgeDlg        (getGeometryGUI(), parent); break;
  case GEOMOp::OpFreeBoundaries:   aDlg = new RepairGUI_FreeBoundDlg         (getGeometryGUI(), parent); break;
  case GEOMOp::OpFreeFaces:        aDlg = new RepairGUI_FreeFacesDlg         (getGeometryGUI(), parent); break;
  case GEOMOp::OpOrientation:      aDlg = new RepairGUI_ChangeOrientationDlg (getGeometryGUI(), parent); break;
  case GEOMOp::OpRemoveWebs:       aDlg = new RepairGUI_RemoveWebsDlg        (getGeometryGUI(), parent); break;
  case GEOMOp::OpRemoveExtraEdges: aDlg = new RepairGUI_RemoveExtraEdgesDlg  (getGeometryGUI(), parent); break;
  case GEOMOp::OpFuseEdges:        aDlg = new RepairGUI_FuseEdgesDlg         (getGeometryGUI(), parent); break;
  case GEOMOp::OpUnionFaces:       aDlg = new RepairGUI_UnionFacesDlg        (getGeometryGUI(), parent); break;
  case GEOMOp::OpInspectObj:       aDlg = new RepairGUI_InspectObjectDlg     (getGeometryGUI(), parent); break;
  default:
    app->putInfo(tr("GEOM_PRP_COMMAND").arg(theCommandID));
    break;
  }

  if (aDlg)
    aDlg->show();

  return true;
}


//=====================================================================================
// EXPORTED METHODS
//=====================================================================================
extern "C"
{
#ifdef WIN32
  __declspec( dllexport )
#endif
  GEOMGUI* GetLibGUI( GeometryGUI* parent )
  {
    return new RepairGUI( parent );
  }
}

//=====================================================================================
// Statistics dialog
//=====================================================================================

#include <QDialog>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QString>
#include <QStringList>
#include <QTableWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

namespace
{
  class StatsDlg : public QDialog
  {
  public:
    StatsDlg( GEOM::ModifStatistics_var stats, QWidget* parent );
  };

  StatsDlg::StatsDlg( GEOM::ModifStatistics_var stats, QWidget* parent ): QDialog( parent )
  {
    setModal( true );
    setWindowTitle( tr( "GEOM_HEALING_STATS_TITLE" ) );
    setMinimumWidth( 500 );

    const int nbRows = stats->length();
    const int nbCols = 2;
    QTableWidget* table = new QTableWidget( nbRows, nbCols, this );
    table->setEditTriggers( QAbstractItemView::NoEditTriggers );
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    table->horizontalHeader()->setResizeMode( 1, QHeaderView::Interactive );
#else
    table->horizontalHeader()->setSectionResizeMode( 1, QHeaderView::Interactive );
#endif
    table->horizontalHeader()->setStretchLastSection( true );

    QStringList headers;
    headers << tr( "GEOM_HEALING_STATS_COL_1" );
    headers << tr( "GEOM_HEALING_STATS_COL_2" );
    table->setHorizontalHeaderLabels( headers );

    // buttons

    QPushButton* okBtn = new QPushButton( tr( "GEOM_BUT_OK" ), this );
    okBtn->setAutoDefault( true );
    okBtn->setDefault( true );
    okBtn->setFocus();
    // QPushButton* helpBtn = new QPushButton( tr( "GEOM_BUT_HELP" ), this );
    // helpBtn->setAutoDefault( true );

    QHBoxLayout* btnLayout = new QHBoxLayout;
    btnLayout->setMargin( 0 );
    btnLayout->setSpacing( 6 );

    btnLayout->addStretch();
    btnLayout->addWidget( okBtn );
    btnLayout->addStretch();
    // btnLayout->addWidget( helpBtn );

    QVBoxLayout* aLay = new QVBoxLayout( this );
    aLay->setMargin( 9 );
    aLay->setSpacing( 6 );
    aLay->addWidget( table );
    aLay->addLayout( btnLayout );

    // fill the table
    for ( int row = 0; row < nbRows; ++row )
    {
      table->setItem( row, 0, new QTableWidgetItem( QString::number( stats[ row ].count )));
      table->setItem( row, 1, new QTableWidgetItem( tr( stats[ row ].name.in() )));
    }

    connect( okBtn,       SIGNAL( clicked() ), this, SLOT( reject() ));
    //connect( helpBtn,     SIGNAL( clicked() ), this, SLOT( help() ));
  }
}

//================================================================================
/*!
 * \brief Show a dialog providing info on what is done by healing
 */
//================================================================================

void RepairGUI::ShowStatistics( GEOM::GEOM_IHealingOperations_var anOper, QWidget* parent )
{
  GEOM::ModifStatistics_var stats = anOper->GetStatistics();

  if ( ! &stats.in() || stats->length() == 0 )
    return;

  SUIT_OverrideCursor wc;
  wc.suspend();

  StatsDlg dlg( stats, parent );
  dlg.exec();
}
