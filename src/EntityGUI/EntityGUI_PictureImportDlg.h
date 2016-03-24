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
// File   : EntityGUI_PictureImportDlg.h
// Author : Renaud NEDELEC, Open CASCADE S.A.S.
//
#ifndef BASICGUI_LINEDLG_H
#define BASICGUI_LINEDLG_H

#include "GEOMBase_Skeleton.h"
#include "GEOM_GenericObjPtr.h"

class QGroupBox;
//=================================================================================
// class    : EntityGUI_PictureImportDlg
// purpose  :
//=================================================================================
class EntityGUI_PictureImportDlg : public GEOMBase_Skeleton
{ 
  Q_OBJECT

public:
  EntityGUI_PictureImportDlg( GeometryGUI*, QWidget* = 0, bool = false, Qt::WindowFlags = 0 );
  ~EntityGUI_PictureImportDlg();
  
protected:
//   // redefined from GEOMBase_Helper
  virtual GEOM::GEOM_IOperations_ptr createOperation();
//   virtual bool                       isValid( QString& );
  virtual bool                       execute( ObjectList& );
//   virtual void                       addSubshapesToStudy();
    
private:
  void                               Init();
//   void                               enterEvent( QEvent* );
  
private:
  QGroupBox*                         GroupSelFile;
  QPushButton*                       myPushButton;
  QLineEdit*                         myLineEdit;
  
private slots:
  void                               ClickOnOk();
  bool                               ClickOnApply();
// 
//   void                               ActivateThisDialog();
//   void                               DeactivateActiveDialog();
   void                               FileSelectionClicked( );    
//     
//   void                               SelectionIntoArgument();
//   void                               SetEditCurrentArgument();
};

#endif // BASICGUI_LINEDLG_H
