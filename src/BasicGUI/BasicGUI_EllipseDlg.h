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
//  File   : BasicGUI_EllipseDlg.h
//  Author : Nicolas REJNERI
//  Module : GEOM

#ifndef BASICGUI_ELLIPSE_H
#define BASICGUI_ELLIPSE_H

#include "GEOM_BasicGUI.hxx"

#include "GEOMBase_Skeleton.h"
#include "DlgRef_2Sel2Spin.h"

#include "BasicGUI.h"

//=================================================================================
// class    : BasicGUI_EllipseDlg
// purpose  :
//=================================================================================
class GEOM_BASICGUI_EXPORT BasicGUI_EllipseDlg : public GEOMBase_Skeleton
{ 
    Q_OBJECT

public:
    BasicGUI_EllipseDlg(GeometryGUI* theGeometryGUI, QWidget* parent = 0,
			const char* name = 0, bool modal = FALSE, WFlags fl = 0);
    ~BasicGUI_EllipseDlg();

protected:
    // redefined from GEOMBase_Helper
    virtual GEOM::GEOM_IOperations_ptr createOperation();
    virtual bool isValid( QString& );
    virtual bool execute( ObjectList& objects );

    virtual void closeEvent( QCloseEvent* e );

private :
    void Init();
    void enterEvent(QEvent* e);

    GEOM::GEOM_Object_var myPoint, myDir;

    DlgRef_2Sel2Spin* GroupPoints;

private slots:
    void ClickOnOk();
    void ClickOnCancel();
    bool ClickOnApply();

    void ActivateThisDialog();
    void DeactivateActiveDialog();
    
    void LineEditReturnPressed();
    void SelectionIntoArgument();
    void SetEditCurrentArgument();
    void ValueChangedInSpinBox(double newValue);

};

#endif // BASICGUI_ELLIPSE_H
