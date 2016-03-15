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
// File   : GEOMToolsGUI_NbIsosDlg.h
// Author : OCC Team
//
#ifndef GEOMTOOLSGUI_NBISOSDLG_H
#define GEOMTOOLSGUI_NBISOSDLG_H

#include "GEOM_ToolsGUI.hxx"

#include <QDialog>

class QSpinBox;

//=================================================================================
// class    : GEOMToolsGUI_NbIsosDlg
// purpose  :
//=================================================================================
class GEOMTOOLSGUI_EXPORT GEOMToolsGUI_NbIsosDlg : public QDialog
{ 
  Q_OBJECT

public:
  GEOMToolsGUI_NbIsosDlg( QWidget* );
  ~GEOMToolsGUI_NbIsosDlg();
    
  int       getU() const;
  int       getV() const;
  
  void      setU( const int );
  void      setV( const int );
  
private slots:
  void      ClickOnHelp();

private:
 void       keyPressEvent( QKeyEvent* );

private:
 QSpinBox*  SpinBoxU;
 QSpinBox*  SpinBoxV;
 QString    myHelpFileName;
};

#endif // GEOMTOOLSGUI_NBISOSDLG_H
