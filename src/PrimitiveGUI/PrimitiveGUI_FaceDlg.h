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
// File   : BasicGUI_FaceDlg.h
// Author : Dmitry Matveithev, OCN.
//
#ifndef BASICGUI_FACEDLG_H
#define BASICGUI_FACEDLG_H

#include "GEOMBase_Skeleton.h"
#include "GEOM_GenericObjPtr.h"

class DlgRef_2Spin;
class DlgRef_1Sel2Spin;
class DlgRef_3Radio;

//=================================================================================
// class    : PrimitiveGUI_FaceDlg
// purpose  :
//=================================================================================
class PrimitiveGUI_FaceDlg : public GEOMBase_Skeleton
{ 
  Q_OBJECT

public:
  PrimitiveGUI_FaceDlg( GeometryGUI*, QWidget* = 0, bool = false, Qt::WindowFlags = 0 );
  ~PrimitiveGUI_FaceDlg();
  
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
  
private:
  GEOM::GeomObjPtr                   myEdge;
  GEOM::GeomObjPtr                   myFace;

  int                                myOrientationType;
  
  DlgRef_2Spin*                      GroupDimensions;
  DlgRef_1Sel2Spin*                  GroupPlane;
  DlgRef_3Radio*                     GroupOrientation;
  DlgRef_3Radio*                     GroupType;
    
private slots:
  void                               ClickOnOk();
  bool                               ClickOnApply();

  void                               ActivateThisDialog();
  void                               DeactivateActiveDialog();
  void                               ConstructorsClicked( int );    
    
  void                               SelectionIntoArgument();
  void                               SetEditCurrentArgument();
  void                               ValueChangedInSpinBox( double );
  void                               SetDoubleSpinBoxStep( double );
  void                               RadioButtonClicked();
  void                               TypeButtonClicked();
};

#endif // BASICGUI_FACEDLG_H
