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
//  File   : GenerationGUI_PipeDlg.h
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#ifndef DIALOGBOX_PIPE_H
#define DIALOGBOX_PIPE_H

#include "GEOMBase_Skeleton.h"
#include "DlgRef_2Sel_QTD.h"

#include "GenerationGUI.h"

//=================================================================================
// class    : GenerationGUI_PipeDlg
// purpose  :
//=================================================================================
class GenerationGUI_PipeDlg : public GEOMBase_Skeleton
{ 
    Q_OBJECT

public:
    GenerationGUI_PipeDlg(QWidget* parent = 0, const char* name = 0, GenerationGUI* theGenerationGUI = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0);
    ~GenerationGUI_PipeDlg();

private:
    void Init();
    void enterEvent(QEvent* e);
    void MakePipeSimulationAndDisplay();

    GenerationGUI* myGenerationGUI;

    TopoDS_Shape myShape1;   /* topology used */
    TopoDS_Shape myShape2;   /* topology used */
    GEOM::GEOM_Shape_var myGeomShape1; /* is myShape1 */
    GEOM::GEOM_Shape_var myGeomShape2; /* is myShape2 */
    bool myOkShape1;
    bool myOkShape2;   /* to check when arguments are defined */

    DlgRef_2Sel_QTD* GroupPoints;

private slots:
    void ClickOnOk();
    void ClickOnApply();
    void ActivateThisDialog();
    void LineEditReturnPressed();
    void SelectionIntoArgument();
    void SetEditCurrentArgument();

};

#endif // DIALOGBOX_PIPE_H
