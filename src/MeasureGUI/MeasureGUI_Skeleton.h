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
// File   : MeasureGUI_Skeleton.h
// Author : Damine COQUERET, Open CASCADE S.A.S.
//
#ifndef MEASUREGUI_SKELETON_H
#define MEASUREGUI_SKELETON_H

#include <QDialog>
#include <GEOMBase_Helper.h>

class GeometryGUI;
class MeasureGUI_SkeletonBox;
class QLineEdit;
class QPushButton;
class GEOM_Displayer;

class MeasureGUI_Skeleton : public QDialog, public GEOMBase_Helper
{ 
  Q_OBJECT

public:
  MeasureGUI_Skeleton( GeometryGUI*, QWidget* = 0, bool = false, Qt::WindowFlags = 0 );
  ~MeasureGUI_Skeleton();
  
protected slots:
  virtual void                       ClickOnCancel();
  virtual void                       ClickOnHelp(); 
  virtual void                       DeactivateActiveDialog();
  virtual void                       ActivateThisDialog();
  virtual void                       SelectionIntoArgument();
  virtual void                       LineEditReturnPressed();
  virtual void                       SetEditCurrentArgument();                              

protected:
  void                               Init();
  
  void                               enterEvent( QEvent* );
  void                               closeEvent( QCloseEvent* );
  void                               keyPressEvent( QKeyEvent* );
  void                               redisplayPreview();
  GEOM_Displayer*                    getDisplayer();
  
  virtual bool                       isValid( QString& );
  virtual SALOME_Prs*                buildPrs();
  virtual void                       processObject();
  virtual void                       activateSelection();
  
  virtual GEOM::GEOM_IOperations_ptr createOperation();

  MeasureGUI_SkeletonBox*            mainFrame() const;
  QWidget*                           centralWidget() const;
  QPushButton*                       buttonClose() const;
  QPushButton*                       buttonHelp() const;

protected:    
  QPushButton*                       mySelBtn;
  QLineEdit*                         mySelEdit;
  GEOM::GeomObjPtr                   myObj;
  
  GeometryGUI*                       myGeomGUI;
  GEOM_Displayer*                    myDisplayer;
  
  QString                            myHelpFileName;
  MeasureGUI_SkeletonBox*            myMainFrame;
};
 
#endif // MEASUREGUI_SKELETON_H
