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
//  File   : MeasureGUI_Skeleton.h
//  Author : Damine COQUERET
//  Module : GEOM
//  $Header: 

#ifndef MEASUREGUI_SKELETON_H
#define MEASUREGUI_SKELETON_H

#include "MeasureGUI_Skeleton_QTD.h"
#include "GEOMBase_Helper.h"

class GeometryGUI;
class QWidget;
class QLineEdit;
class QPushButton;
class SALOME_Selection;
class GEOM_Displayer;

class MeasureGUI_Skeleton : public MeasureGUI_Skeleton_QTD,
                            public GEOMBase_Helper
{ 
    Q_OBJECT

public:
                              MeasureGUI_Skeleton( QWidget*          parent,
                                                   const char*       name,
                                                   SALOME_Selection* Sel = 0 );
                              ~MeasureGUI_Skeleton();

protected slots:

    virtual void              ClickOnCancel();
    virtual void              DeactivateActiveDialog();
    virtual void              ActivateThisDialog();
    virtual void              SelectionIntoArgument();
    virtual void              LineEditReturnPressed();
    virtual void              SetEditCurrentArgument();                              

protected:

    void                      Init( SALOME_Selection* Sel );

    void                      enterEvent( QEvent* e );
    void                      closeEvent( QCloseEvent* e );
    void                      redisplayPreview();
    GEOM_Displayer*           getDisplayer();

    virtual bool              isValid( QString& msg );
    virtual SALOME_Prs*       buildPrs();
    virtual void              processObject();
    virtual void              activateSelection();

    virtual GEOM::GEOM_IOperations_ptr createOperation();

protected:    
    
    QPushButton*              mySelBtn;
    QLineEdit*                mySelEdit;
    GEOM::GEOM_Object_var     myObj;
    
    SALOME_Selection*         mySelection;              
    GeometryGUI*              myGeomGUI;
    GEOM_Displayer*           myDisplayer;
};

#endif // MEASUREGUI_SKELETON_H











