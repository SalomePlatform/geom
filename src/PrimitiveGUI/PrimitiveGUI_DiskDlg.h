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
// File   : PrimitiveGUI_DiskDlg.h
// Author : Dmitry Matveitchev, OCN
//
#ifndef BASICGUI_DISKDLG_H
#define BASICGUI_DISKDLG_H

#include "GEOMBase_Skeleton.h"
#include "GEOM_GenericObjPtr.h"

class DlgRef_3Sel;
class DlgRef_2Sel1Spin;
class DlgRef_1Spin;
class DlgRef_3Radio;

//=================================================================================
// class    : PrimitiveGUI_DiskDlg
// purpose  :
//=================================================================================
class PrimitiveGUI_DiskDlg : public GEOMBase_Skeleton
{
  Q_OBJECT

public:
  PrimitiveGUI_DiskDlg( GeometryGUI*, QWidget* = 0, bool = false, Qt::WindowFlags = 0 );
  ~PrimitiveGUI_DiskDlg();

protected:
  // redefined from GEOMBase_Helper
  virtual GEOM::GEOM_IOperations_ptr createOperation();
  virtual bool                       isValid( QString& );
  virtual bool                       execute( ObjectList& );
  virtual void                       addSubshapesToStudy();
  virtual QList<GEOM::GeomObjPtr>    getSourceObjects();
  
private:
  void                               Init();
  void                               enterEvent( QEvent* );
  double                             getRadius() const;
  int                                myOrientationType;

private:
  GEOM::GeomObjPtr                   myPoint, myDir, myPoint1, myPoint2, myPoint3;

  DlgRef_2Sel1Spin*                  GroupPntVecR;
  DlgRef_3Sel*                       Group3Pnts;
  DlgRef_1Spin*                      GroupDimensions;
  DlgRef_3Radio*                     GroupOrientation;

private slots:
  void                               ClickOnOk();
  bool                               ClickOnApply();

  void                               ActivateThisDialog();
    
  void                               SelectionIntoArgument();

  void                               ConstructorsClicked( int );
  void                               SetEditCurrentArgument();
  void                               ValueChangedInSpinBox();
  void                               SetDoubleSpinBoxStep( double );
  void                               RadioButtonClicked();
};

#endif // BASICGUI_DISKDLG_H
