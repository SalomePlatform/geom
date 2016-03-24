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
// File   : TransformationGUI_ExtensionDlg.h
// Author : Sergey KHROMOV, Open CASCADE S.A.S.
//
#ifndef TRANSFORMATIONGUI_EXTENSIONDLG_H
#define TRANSFORMATIONGUI_EXTENSIONDLG_H

#include "GEOMBase_Skeleton.h"
#include "GEOM_GenericObjPtr.h"

class DlgRef_3Sel4Spin2Check;

/**
 *  This is a dialog for ExtendEdge and ExtendFace functionality
 */
class TransformationGUI_ExtensionDlg : public GEOMBase_Skeleton
{ 
  Q_OBJECT

public:
  TransformationGUI_ExtensionDlg(GeometryGUI*, QWidget* = 0,
                                 bool = false, Qt::WindowFlags = 0);
  ~TransformationGUI_ExtensionDlg();

protected:
  // redefined from GEOMBase_Helper
  virtual GEOM::GEOM_IOperations_ptr createOperation();
  virtual bool                       isValid( QString& );
  virtual bool                       execute( ObjectList& );    
  virtual QList<GEOM::GeomObjPtr>    getSourceObjects();

private:
  void                               Init();
  void                               enterEvent(QEvent *);

private:

  QLabel                            *myObjLbl;
  QLabel                            *myUMinLbl;
  QLabel                            *myUMaxLbl;
  QLabel                            *myVMinLbl;
  QLabel                            *myVMaxLbl;
  QPushButton                       *mySelButton;
  QLineEdit                         *myEditObjName;
  SalomeApp_DoubleSpinBox           *myUMinSpinBox;
  SalomeApp_DoubleSpinBox           *myUMaxSpinBox;
  SalomeApp_DoubleSpinBox           *myVMinSpinBox;
  SalomeApp_DoubleSpinBox           *myVMaxSpinBox;
  GEOM::GeomObjPtr                   myBase;
    
private slots:
  void                               ClickOnOk();
  bool                               ClickOnApply();
  void                               ActivateThisDialog();
  void                               SelectionIntoArgument();
  void                               SetEditCurrentArgument();
  void                               ValueChangedInSpinBox( double );
  void                               ConstructorsClicked( int );
  void                               SetDoubleSpinBoxStep( double );
};

#endif // TRANSFORMATIONGUI_EXTENSIONDLG_H
