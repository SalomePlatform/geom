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
//  File   : GEOMBase_Skeleton.h
//  Author : Damine COQUERET
//  Module : GEOM
//  $Header$

#ifndef GEOMBASE_SKELETON_H
#define GEOMBASE_SKELETON_H

#include "DlgRef_Skeleton_QTD.h"

#include "GEOMBase.h"
#include "GEOMBase_Helper.h"
#include "GeometryGUI.h"

#include <qwidget.h>
#include <qgroupbox.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>

class GEOMBase_Skeleton : public DlgRef_Skeleton_QTD, public GEOMBase_Helper
{ 
    Q_OBJECT

public:
    GEOMBase_Skeleton(QWidget* parent = 0, const char* name = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0);
    ~GEOMBase_Skeleton();

private :
    void Init(SALOME_Selection* Sel);

protected:
    void closeEvent(QCloseEvent* e);

    void initName( const char* thePrefix = 0 ); // initialize "Name" field with a string "thePrefix_X" (Vertex_3)
    const char* getNewObjectName() const; // returns contents of "Name" field
    int getConstructorId() const; // returns id of a selected "constructor" radio button or '-1' in case of error

    QLineEdit* myEditCurrentArgument;   /* Current LineEdit */
    SALOME_Selection* mySelection;      /* User shape selection */
    GEOMBase* myGeomBase; // SAN -- TO BE REMOVED !!!
    GeometryGUI* myGeomGUI;             /* reference GEOM GUI */

protected slots:
    void ClickOnCancel();
    void LineEditReturnPressed();
    void DeactivateActiveDialog();
    void ActivateThisDialog();

};

#endif // GEOMBASE_SKELETON_H
