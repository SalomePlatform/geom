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
//  File   : MeasureGUI_WhatisDlg.h
//  Author : Nicolas REJNERI
//  Module : GEOM
//  $Header$

#ifndef DIALOGBOX_WHATIS_H
#define DIALOGBOX_WHATIS_H

#include "MeasureGUI_Skeleton.h"
#include "MeasureGUI_1Sel1TextView_QTD.h"

#include "MeasureGUI.h"

//=================================================================================
// class    : DialogBox_PROPERTIES
// purpose  :
//=================================================================================
class MeasureGUI_WhatisDlg : public MeasureGUI_Skeleton
{ 
    Q_OBJECT

public:
    MeasureGUI_WhatisDlg(QWidget* parent = 0, const char* name = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0);
    ~MeasureGUI_WhatisDlg();

private:
    void Init();
    void enterEvent(QEvent* e);
    void CalculateWhatis(const TopoDS_Shape& S);

    QString SelectedName;
    MeasureGUI_1Sel1TextView_QTD* GroupC1;

private slots:
    void SetEditCurrentArgument();
    void SelectionIntoArgument();
    void LineEditReturnPressed();
    void ActivateThisDialog();

};

#endif // DIALOGBOX_WHATIS_H
