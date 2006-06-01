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
//  File   : BooleanGUI_Dialog.h
//  Author : Lucien PIGNOLONI
//  Module : GEOM

#ifndef BooleanGUI_Dialog_H
#define BooleanGUI_Dialog_H

#include "GEOMBase_Skeleton.h"

class DlgRef_2Sel_QTD;

//=================================================================================
// class    : BooleanGUI_Dialog
// purpose  :
//=================================================================================
class BooleanGUI_Dialog : public GEOMBase_Skeleton
{
    Q_OBJECT

public:
    BooleanGUI_Dialog( const int theOperation, GeometryGUI* theGeometryGUI, QWidget* parent = 0,
		       const char* name = 0, bool modal = FALSE, WFlags fl = 0);
    ~BooleanGUI_Dialog();

protected:
    // redefined from GEOMBase_Helper
    virtual GEOM::GEOM_IOperations_ptr createOperation();
    virtual bool isValid( QString& );
    virtual bool execute( ObjectList& objects );

private:
    int myOperation;
	  
    void Init();
    void enterEvent(QEvent * e);

    GEOM::GEOM_Object_var myObject1;
    GEOM::GEOM_Object_var myObject2;

    DlgRef_2Sel_QTD* myGroup;

private slots:
    void ClickOnOk();
    bool ClickOnApply();
    void SetEditCurrentArgument();
    void SelectionIntoArgument();
    void LineEditReturnPressed();
    void ActivateThisDialog();
};

#endif // BooleanGUI_Dialog_H
