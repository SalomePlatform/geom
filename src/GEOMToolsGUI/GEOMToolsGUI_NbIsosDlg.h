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
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : GEOMToolsGUI_NbIsosDlg.h
//  Author : 
//  Module : GEOM
//  $Header: 

#ifndef GEOMTOOLSGUI_NBISOSDLG_H
#define GEOMTOOLSGUI_NBISOSDLG_H

#include <qdialog.h>

class QSpinBox;

//=================================================================================
// class    : GEOMToolsGUI_NbIsosDlg
// purpose  :
//=================================================================================
class GEOMToolsGUI_NbIsosDlg : public QDialog
{ 
    Q_OBJECT

public:
    GEOMToolsGUI_NbIsosDlg( QWidget* parent );
    ~GEOMToolsGUI_NbIsosDlg();
    
    int       getU() const;
    int       getV() const;

    void      setU( const int );
    void      setV( const int );

private slots:
    void ClickOnHelp();

private:
    QSpinBox* SpinBoxU;
    QSpinBox* SpinBoxV;
    QString   myHelpFileName;

};

#endif // GEOMTOOLSGUI_NBISOSDLG_H
