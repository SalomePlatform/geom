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
//  File   : BasicGUI_ArcDlg.h
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#ifndef DIALOGBOX_ARC_H
#define DIALOGBOX_ARC_H

#include "GEOMBase_Skeleton.h"
#include "DlgRef_3Sel_QTD.h"

//=================================================================================
// class    : BasicGUI_ArcDlg
// purpose  : 
//=================================================================================
class BasicGUI_ArcDlg : public GEOMBase_Skeleton
{ 
    Q_OBJECT

public:
    BasicGUI_ArcDlg( QWidget* parent = 0, const char* name = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0 );
    ~BasicGUI_ArcDlg();

protected:
    // redefined from GEOMBase_Helper
    virtual GEOM::GEOM_IOperations_ptr createOperation();
    virtual bool isValid( QString& );
    virtual bool execute( ObjectList& objects );

    virtual void closeEvent( QCloseEvent* e );    

private :
    void Init();
    void enterEvent(QEvent* e);

    GEOM::GEOM_Object_var myPoint1, myPoint2, myPoint3;

    DlgRef_3Sel_QTD* Group3Pnts;

private slots:
    void ClickOnOk();
    void ClickOnCancel();
    bool ClickOnApply();
    
    void ActivateThisDialog();
    void DeactivateActiveDialog();
    
    void LineEditReturnPressed();
    void SelectionIntoArgument();
    void SetEditCurrentArgument();

};

#endif // DIALOGBOX_ARC_H
