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
//  File   : RepairGUI_DivideEdgeDlg.h
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#ifndef DIALOGBOX_DivideEdge_H
#define DIALOGBOX_DivideEdge_H

#include "GEOMBase_Skeleton.h"
#include "DlgRef_1Sel_Ext.h"
#include "QAD_SpinBoxDbl.h"

#include <TColStd_IndexedMapOfInteger.hxx>

#include <qradiobutton.h>
#include <qbuttongroup.h>

//=================================================================================
// class    : RepairGUI_DivideEdgeDlg
// purpose  :
//=================================================================================
class RepairGUI_DivideEdgeDlg : public GEOMBase_Skeleton
{
    Q_OBJECT

public:
    RepairGUI_DivideEdgeDlg(QWidget* parent = 0, const char* name = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0);
    ~RepairGUI_DivideEdgeDlg();

protected:
    // redefined from GEOMBase_Helper
    virtual GEOM::GEOM_IOperations_ptr createOperation();
    virtual bool isValid( QString& );
    virtual bool execute( ObjectList& objects );

private :
    void Init();
    void enterEvent(QEvent* e);
    void closeEvent(QCloseEvent* e);
    void initSelection();

    bool getIsByParameter() const;

		GEOM::GEOM_Object_var myObject;
		int myIndex;

    DlgRef_1Sel_Ext* GroupPoints;
    QButtonGroup* myIsParameterGr;
    QAD_SpinBoxDbl* myValEdt;

private slots:
    void ClickOnOk();
    bool ClickOnApply();
    void ClickOnCancel();

    void ActivateThisDialog();
    void DeactivateActiveDialog();

    void LineEditReturnPressed();
    void SelectionIntoArgument();
    void SetEditCurrentArgument();
};

#endif // DIALOGBOX_DivideEdge_H
