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
//  File   : BuildGUI_SolidDlg.h
//  Author : Damien COQUERET
//  Module : GEOM

#ifndef DIALOGBOX_SOLID_H
#define DIALOGBOX_SOLID_H

#include "GEOMBase_Skeleton.h"
#include "DlgRef_1Sel1Check_QTD.h"

//=================================================================================
// class    : BuildGUI_SolidDlg
// purpose  :
//=================================================================================
class BuildGUI_SolidDlg : public GEOMBase_Skeleton
{ 
    Q_OBJECT

public:
    BuildGUI_SolidDlg(GeometryGUI* theGeometryGUI, QWidget* parent = 0,
		      const char* name = 0, bool modal = FALSE, WFlags fl = 0);
    ~BuildGUI_SolidDlg();

protected:
    // redefined from GEOMBase_Helper
    virtual GEOM::GEOM_IOperations_ptr createOperation();
    virtual bool isValid( QString& msg );
    virtual bool execute( ObjectList& objects );    

private:
    void Init();
    void enterEvent(QEvent * e);

    GEOM::ListOfGO myShells;
    bool myOkShells;          /* to check when arguments is defined and
                                 all shells are closed */

    DlgRef_1Sel1Check_QTD* GroupSolid;

private slots:
    void ClickOnOk();
    bool ClickOnApply();
    void SetEditCurrentArgument();
    void SelectionIntoArgument();
    void ActivateThisDialog();
    void EnableNameField(bool toEnable);
};

#endif // DIALOGBOX_SOLID_H
