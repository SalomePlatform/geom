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
//  File   : OperationGUI_FilletDlg.h
//  Author : Damien COQUERET
//  Module : GEOM

#ifndef DIALOGBOX_FILLET_H
#define DIALOGBOX_FILLET_H

#include "GEOMBase_Skeleton.h"

#include <TColStd_IndexedMapOfInteger.hxx>

class DlgRef_1Sel1Spin;
class DlgRef_2Sel1Spin;

//=================================================================================
// class    : OperationGUI_FilletDlg
// purpose  :
//=================================================================================
class OperationGUI_FilletDlg : public GEOMBase_Skeleton
{ 
    Q_OBJECT

public:
    OperationGUI_FilletDlg(GeometryGUI* theGeometryGUI, QWidget* parent);
    ~OperationGUI_FilletDlg();

protected:
    // redefined from GEOMBase_Helper
    virtual                             GEOM::GEOM_IOperations_ptr createOperation();
    virtual                             bool isValid( QString& msg );
    virtual                             bool execute( ObjectList& objects );

private slots:
    void                                ClickOnOk();
    bool                                ClickOnApply();
    void                                ActivateThisDialog();
    void                                LineEditReturnPressed();
    void                                SelectionIntoArgument();
    void                                SetEditCurrentArgument();
    void                                ValueChangedInSpinBox( double newValue );
    void                                ConstructorsClicked( int constructorId );

private:
    void                                Init();
    void                                enterEvent( QEvent* e );
    void                                reset();
    int                                 getConstructorId() const;
    void                                activateSelection();
    void                                enableWidgets();
    double                              getRadius() const;

private:
    int                                 myConstructorId;

    GEOM::GEOM_Object_var               myShape;
    TColStd_IndexedMapOfInteger         myEdges;
    TColStd_IndexedMapOfInteger         myFaces;

    DlgRef_1Sel1Spin*                   Group1;
    DlgRef_2Sel1Spin*                   Group2;
    DlgRef_2Sel1Spin*                   Group3;
};

#endif // DIALOGBOX_FILLET_H
