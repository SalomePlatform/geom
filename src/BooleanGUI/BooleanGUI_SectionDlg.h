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
//  File   : BooleanGUI_SectionDlg.h
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header:

#ifndef DIALOGBOX_SECTION_H
#define DIALOGBOX_SECTION_H

#include "GEOMBase_Skeleton.h"
#include "DlgRef_2Sel_QTD.h"

#include "BooleanGUI.h"

#include <BRepAlgoAPI_Section.hxx>

//=================================================================================
// class    : BooleanGUI_SectionDlg
// purpose  :
//=================================================================================
class BooleanGUI_SectionDlg : public GEOMBase_Skeleton
{ 
    Q_OBJECT

public:
    BooleanGUI_SectionDlg(QWidget* parent = 0, const char* name = 0, BooleanGUI* theBooleanGUI = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0);
    ~BooleanGUI_SectionDlg();
    
private:
    void Init();
    void enterEvent(QEvent* e);

    BooleanGUI* myBooleanGUI;

    GEOM::GEOM_Shape_var myGeomShape1;          /* is myShape1 */
    GEOM::GEOM_Shape_var myGeomShape2;          /* is myShape2 */
    bool myOkShape1;        /* to check when arguments are defined */
    bool myOkShape2;

    DlgRef_2Sel_QTD* GroupSection;

private slots:
    void ClickOnOk();
    void ClickOnApply();
    void SetEditCurrentArgument();
    void SelectionIntoArgument();
    void LineEditReturnPressed();
    void ActivateThisDialog();

};

#endif // DIALOGBOX_SECTION_H
