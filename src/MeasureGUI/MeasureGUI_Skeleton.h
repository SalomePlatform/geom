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
//  File   : MeasureGUI_Skeleton.h
//  Author : Damine COQUERET
//  Module : GEOM
//  $Header: 

#ifndef MEASUREGUI_SKELETON_H
#define MEASUREGUI_SKELETON_H

#include "MeasureGUI_Skeleton_QTD.h"

#include "GEOMBase.h"

#include <qwidget.h>
#include <qgroupbox.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qpushbutton.h>

class MeasureGUI_Skeleton : public MeasureGUI_Skeleton_QTD
{ 
    Q_OBJECT

public:
    MeasureGUI_Skeleton(QWidget* parent = 0, const char* name = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0);
    ~MeasureGUI_Skeleton();

private :
    void Init(SALOME_Selection* Sel);

protected:
    void closeEvent(QCloseEvent* e);

    TopoDS_Shape mySimulationTopoDs;    /* Shape used for simulation display */
    QLineEdit* myEditCurrentArgument;   /* Current LineEdit */
    SALOME_Selection* mySelection;      /* User shape selection */
    GEOM::GEOM_Gen_var myGeom;          /* Current GeomI object */
    GEOMBase* myGeomBase;
    GEOMContext* myGeomGUI;  /* Current GeomGUI object */

protected slots:
    void ClickOnCancel();
    void LineEditReturnPressed();
    void DeactivateActiveDialog();
    void ActivateThisDialog();

};

#endif // MEASUREGUI_SKELETON_H
