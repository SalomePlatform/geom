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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : GEOMBase_aParameterDlg.h
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#ifndef GEOMBase_aParameterDLG_H
#define GEOMBase_aParameterDLG_H

#include "GEOM_GEOMBase.hxx"

#include <qdialog.h>

class QPushButton;
class QtxDblSpinBox;

//=================================================================================
// class    : GEOMBase_aParameterDlg
// purpose  :
//=================================================================================
class GEOMBASE_EXPORT GEOMBase_aParameterDlg : public QDialog
{ 
    Q_OBJECT

public:
    GEOMBase_aParameterDlg(const char* aValue1 = "25", const char* aTitle1 = "Value :", QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0, const double bottom = -1E6, const double top = +1E6, const int decimals = 6);
    ~GEOMBase_aParameterDlg();

    void setValue(double val);
    double getValue();

private:
    QPushButton* myButtonOk;
    QPushButton* myButtonCancel;
    QtxDblSpinBox* mySpinBox;

};

#endif // GEOMBase_aParameterDlg.h
