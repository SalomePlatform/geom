//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//  GEOM GEOMGUI : GUI for Geometry component
//  File   : GEOMToolsGUI_DeflectionDlg.h
//  Author : OCC Team

#ifndef GEOMTOOLSGUI_DEFLECTIONDLG_H
#define GEOMTOOLSGUI_DEFLECTIONDLG_H

#include "GEOM_ToolsGUI.hxx"

#include <QDialog>

class SalomeApp_DoubleSpinBox;

//=================================================================================
// class    : GEOMToolsGUI_DeflectionDlg
// purpose  :
//=================================================================================
class GEOMTOOLSGUI_EXPORT GEOMToolsGUI_DeflectionDlg : public QDialog
{
  Q_OBJECT

public:
  GEOMToolsGUI_DeflectionDlg( QWidget* );
  ~GEOMToolsGUI_DeflectionDlg();

  double    getDC() const;
  void      setDC( const double );

private slots:
  void      ClickOnHelp();

private:
 void       keyPressEvent( QKeyEvent* );

private:
 SalomeApp_DoubleSpinBox*  SpinBox;
 QString                   myHelpFileName;
};

#endif // GEOMTOOLSGUI_DEFLECTIONDLG_H
