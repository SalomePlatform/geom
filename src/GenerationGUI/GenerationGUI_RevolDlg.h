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
//  File   : GenerationGUI_RevolDlg.h
//  Author : Lucien PIGNOLONI
//  Module : GEOM

#ifndef DIALOGBOX_REVOLUTION_H
#define DIALOGBOX_REVOLUTION_H

#include "GenerationGUI.h"

#include "GEOMBase_Skeleton.h"
#include "DlgRef_2Sel1Spin2Check.h"

#include <gp_Dir.hxx>

//=================================================================================
// class    : GenerationGUI_RevolDlg
// purpose  :
//=================================================================================
class GENERATIONGUI_EXPORT GenerationGUI_RevolDlg : public GEOMBase_Skeleton
{ 
    Q_OBJECT

public:
    GenerationGUI_RevolDlg(GeometryGUI* theGeometryGUI, QWidget* parent = 0,
			   const char* name = 0, bool modal = FALSE, WFlags fl = 0);
    ~GenerationGUI_RevolDlg();

protected:
    // redefined from GEOMBase_Helper
    virtual GEOM::GEOM_IOperations_ptr createOperation();
    virtual bool isValid( QString& msg );
    virtual bool execute( ObjectList& objects );    

private :
    void Init();
    void enterEvent(QEvent* e);
    double getAngle() const;

    GEOM::GEOM_Object_var myBase; /* Base shape */
    GEOM::GEOM_Object_var myAxis; /* Axis of the revolution */
    bool myOkBase; 
    bool myOkAxis;

    DlgRef_2Sel1Spin2Check* GroupPoints;

private slots:
    void ClickOnOk();
    bool ClickOnApply();
    void ActivateThisDialog();
    void LineEditReturnPressed();
    void SelectionIntoArgument();
    void SetEditCurrentArgument();
    void ValueChangedInSpinBox();
    void onReverse();
};

#endif // DIALOGBOX_REVOLUTION_H
