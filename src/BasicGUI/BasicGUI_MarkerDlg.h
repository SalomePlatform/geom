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
// File   : BasicGUI_MarkerDlg.h
// Author : Sergey LITONIN, Open CASCADE S.A.S. (sergey.litonin@opencascade.com)
//
#ifndef BASICGUI_MARKERDLG_H
#define BASICGUI_MARKERDLG_H

#include "GEOMBase_Skeleton.h"
#include "GEOM_GenericObjPtr.h"

#include <QMap>

class DlgRef_1Sel;
class DlgRef_3Sel;
class SalomeApp_DoubleSpinBox;
class QFrame;

class BasicGUI_MarkerDlg : public GEOMBase_Skeleton
{
  Q_OBJECT

private:
  enum { X, Y, Z, DX1, DY1, DZ1, DX2, DY2, DZ2 };
  typedef QMap< int, SalomeApp_DoubleSpinBox* > DataMap;

public:
                                  BasicGUI_MarkerDlg( GeometryGUI*, QWidget* );
  virtual                         ~BasicGUI_MarkerDlg();
  void                            Init();

protected:
  virtual                         GEOM::GEOM_IOperations_ptr createOperation();
  virtual bool                    isValid( QString& );
  virtual bool                    execute( ObjectList& );
  virtual void                    displayPreview ( const bool   = false,
                                                   const bool   = true,
                                                   const bool   = true,
                                                   const double = -1 );
  virtual void                    addSubshapesToStudy();
  virtual QList<GEOM::GeomObjPtr> getSourceObjects();

private:
  void                            enterEvent( QEvent* );
  void                            onSelectionDone0();

  GEOM::GeomObjPtr                myShape;
  GEOM::GeomObjPtr                myPoint;
  GEOM::GeomObjPtr                myVectorX;
  GEOM::GeomObjPtr                myVectorY;

private slots:
  void                            onOk();
  bool                            onApply();
  void                            onActivate();
  void                            onDeactivate();
  void                            onSelectionDone();
  void                            onValueChanged( double );
  void                            ConstructorsClicked( int );
  void                            SetEditCurrentArgument();

private:
  DataMap                         myData;
  int                             myConstructorId;
  int                             myBlockPreview;

  QFrame*                         aMainGrp;
  DlgRef_1Sel*                    Group1;
  DlgRef_3Sel*                    Group2;
};

#endif // BASICGUI_MARKERDLG_H
