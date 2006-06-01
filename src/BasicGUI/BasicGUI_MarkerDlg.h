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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : BasicGUI_MarkerDlg.h
//  Author : Sergey LITONIN
//  Module : GEOM

#ifndef DIALOGBOX_Marker_H
#define DIALOGBOX_Marker_H

#include "GEOMBase_Skeleton.h"
#include "DlgRef_1Sel_QTD.h"
#include "DlgRef_3Sel_QTD.h"

#include <qmap.h>

class DlgRef_SpinBox;

#if defined WNT && defined WIN32 && defined SALOME_WNT_EXPORTS
#define BASICGUI_WNT_EXPORT __declspec( dllexport )
#else
#define BASICGUI_WNT_EXPORT
#endif

class BasicGUI_MarkerDlg : public GEOMBase_Skeleton
{
  Q_OBJECT

private:
  enum { X, Y, Z, DX1, DY1, DZ1, DX2, DY2, DZ2 };
  typedef QMap< int, DlgRef_SpinBox* > DataMap;

public:
                                BasicGUI_MarkerDlg( GeometryGUI* theGeometryGUI, QWidget* );
  virtual                       ~BasicGUI_MarkerDlg();
  void                          Init();

protected:
  virtual                       GEOM::GEOM_IOperations_ptr createOperation();
  virtual bool                  isValid( QString& );
  virtual bool                  execute( ObjectList& objects );
  virtual void                  closeEvent( QCloseEvent* e );
  virtual void                  displayPreview ( const bool   activate = false,
                                                 const bool   update = true,
                                                 const bool   toRemoveFromEngine = true,
                                                 const double lineWidth = -1 );

private:
  void                          enterEvent( QEvent* e );
  void                          onSelectionDone0();

private slots:
  void                          onOk();
  void                          onClose();
  bool                          onApply();
  void                          onActivate();
  void                          onDeactivate();
  void                          onSelectionDone();
  void                          onValueChanged( double );
  void                          ConstructorsClicked( int );
  void                          LineEditReturnPressed();
  void                          SetEditCurrentArgument();

private:
  DataMap                       myData;
  int                           myConstructorId;
  int                           myBlockPreview;

  QGroupBox*                    aMainGrp;
  DlgRef_1Sel_QTD*              Group1;
  DlgRef_3Sel_QTD*              Group2;

};

#endif // DIALOGBOX_Marker_H
