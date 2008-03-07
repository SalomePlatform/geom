//  GEOM GEOMGUI : GUI for Geometry component
//
//  Copyright (C) 2003  OPEN CASCADE 
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
//  File   : BasicGUI_CurveDlg.h
//  Author : Alexander SLADKOV
//  Module : GEOM

#ifndef BASICGUI_CURVE_H
#define BASICGUI_CURVE_H

#include "GEOM_BasicGUI.hxx"

#include "GEOMBase_Skeleton.h"
#include "DlgRef_1Sel_QTD.h"

#include "BasicGUI.h"

#include <list>

//=================================================================================
// class    : BasicGUI_CurveDlg
// purpose  :
//=================================================================================
class GEOM_BASICGUI_EXPORT BasicGUI_CurveDlg : public GEOMBase_Skeleton
{ 
    Q_OBJECT

public:
    BasicGUI_CurveDlg(GeometryGUI* theGeometryGUI, QWidget* parent = 0,
		      const char* name = 0, bool modal = FALSE, WFlags fl = 0);
    ~BasicGUI_CurveDlg();

protected:
    // redefined from GEOMBase_Helper
    virtual GEOM::GEOM_IOperations_ptr createOperation();
    virtual bool isValid( QString& );
    virtual bool execute( ObjectList& objects );

    virtual void closeEvent( QCloseEvent* e );    

private :
    void Init();
    void enterEvent(QEvent* e);

    DlgRef_1Sel_QTD* GroupPoints;
    GEOM::ListOfGO_var myPoints;
    list<GEOM::GEOM_Object_var> myOrderedSel;//!< This list used for managing orderes selection

private slots:
    void ClickOnOk();
    void ClickOnCancel();
    bool ClickOnApply();

    void ActivateThisDialog();
    void DeactivateActiveDialog();    
    
    void ConstructorsClicked( int );
    void LineEditReturnPressed();
    void SelectionIntoArgument();
    void SetEditCurrentArgument();

};

#endif // BASICGUI_CURVE_H
