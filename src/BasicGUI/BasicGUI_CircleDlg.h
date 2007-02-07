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
//  File   : BasicGUI_CircleDlg.h
//  Author : Lucien PIGNOLONI
//  Module : GEOM

#ifndef DIALOGBOX_CIRCLE_H
#define DIALOGBOX_CIRCLE_H

#include "GEOM_BasicGUI.hxx"

#include "GEOMBase_Skeleton.h"
#include "DlgRef_2Sel1Spin.h"
#include "DlgRef_3Sel_QTD.h"

#include "BasicGUI.h"

//=================================================================================
// class    : BasicGUI_CircleDlg
// purpose  :
//=================================================================================
class GEOM_BASICGUI_EXPORT BasicGUI_CircleDlg : public GEOMBase_Skeleton
{
    Q_OBJECT

public:
    BasicGUI_CircleDlg(GeometryGUI* theGeometryGUI, QWidget* parent = 0,
		       const char* name = 0, bool modal = FALSE, WFlags fl = 0);
    ~BasicGUI_CircleDlg();

protected:
    // redefined from GEOMBase_Helper
    virtual GEOM::GEOM_IOperations_ptr createOperation();
    virtual bool isValid( QString& );
    virtual bool execute( ObjectList& objects );

    virtual void closeEvent( QCloseEvent* e );    

private:
    void   Init();
    void   enterEvent(QEvent* e);
    double getRadius() const;

    GEOM::GEOM_Object_var myPoint, myDir, myPoint1, myPoint2, myPoint3;

    DlgRef_2Sel1Spin* GroupPntVecR;
    DlgRef_3Sel_QTD*  Group3Pnts;

private slots:
    void ClickOnOk();
    void ClickOnCancel();
    bool ClickOnApply();

    void ActivateThisDialog();
    void DeactivateActiveDialog();
    
    void SelectionIntoArgument();

    void ConstructorsClicked( int );
    void LineEditReturnPressed();
    void SetEditCurrentArgument();
    void ValueChangedInSpinBox();
};

#endif // DIALOGBOX_CIRCLE_H
