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
// File   : GEOMToolsGUI_TransparencyDlg.h
// Author : Lucien PIGNOLONI
//
#ifndef GEOMTOOLSGUI_TRANSPARENCYDLG_H
#define GEOMTOOLSGUI_TRANSPARENCYDLG_H

#include "GEOM_ToolsGUI.hxx"

#include <QDialog>

class QSlider;
class QLabel;

//=================================================================================
// class    : GEOMBase_TransparencyDlg
// purpose  :
//          : WARNING : that is a MODAL dialog.
//=================================================================================
class GEOMTOOLSGUI_EXPORT GEOMToolsGUI_TransparencyDlg : public QDialog
{ 
  Q_OBJECT

public:
  GEOMToolsGUI_TransparencyDlg( QWidget* );    
  ~GEOMToolsGUI_TransparencyDlg();

private:
  void      keyPressEvent( QKeyEvent* );

private:
  bool      myFirstInit;   /* Inform for the first init  */
  QSlider*  mySlider; 
  QLabel*   myValueLab;
  QString   myHelpFileName;

private slots: 
  void      ClickOnOk();
  void      ClickOnClose();
  void      ClickOnHelp();
  void      ValueHasChanged();
  void      SliderHasMoved(int);
  void      SetTransparency();
};

#endif // GEOMTOOLSGUI_TRANSPARENCYDLG_H
