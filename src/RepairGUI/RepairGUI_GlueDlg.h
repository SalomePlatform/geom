//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//  GEOM GEOMGUI : GUI for Geometry component
//  File   : RepairGUI_GlueDlg.h
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//
#ifndef DIALOGBOX_Glue_H
#define DIALOGBOX_Glue_H

#include "GEOMBase_Skeleton.h"

class QtxDblSpinBox;
class DlgRef_1Sel_Ext;
class QCheckBox;

//=================================================================================
// class    : RepairGUI_GlueDlg
// purpose  :
//=================================================================================
class RepairGUI_GlueDlg : public GEOMBase_Skeleton
{
    Q_OBJECT

public:
    RepairGUI_GlueDlg( GeometryGUI* theGeometryGUI, QWidget* parent = 0,
                       const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~RepairGUI_GlueDlg();

protected:
    // redefined from GEOMBase_Helper
    virtual GEOM::GEOM_IOperations_ptr createOperation();
    virtual bool isValid( QString& );
    virtual bool execute( ObjectList& objects );
    virtual void restoreSubShapes (SALOMEDS::Study_ptr, SALOMEDS::SObject_ptr);

private:
    void Init();
    void enterEvent(QEvent* e);
    void initSelection();
    
    void clearTemporary();

    bool onAcceptLocal();
    void clearShapeBufferLocal( GEOM::GEOM_Object_ptr );
    // Reimplementation of onAccept for local case of this class.
    
    void activateSelection();
    void updateButtonState();
    void selectTmpInViewer();
    
private:    
  
    GEOM::GEOM_Object_var myObject;
    ObjectList            myTmpObjs;
    
    DlgRef_1Sel_Ext* GroupPoints;
    DlgRef_1Sel_Ext* GroupPoints2;
    QtxDblSpinBox*   myTolEdt;
    QtxDblSpinBox*   myTolEdt2;
    QPushButton*     myDetectBtn;
    QCheckBox*       mySubShapesChk;

    int myCurrConstrId;
    
protected slots:
    virtual void ClickOnCancel();
    
private slots:
    void ClickOnOk();
    bool ClickOnApply();

    void ActivateThisDialog();

    void LineEditReturnPressed();
    void SelectionIntoArgument();
    void SetEditCurrentArgument();

    void ConstructorsClicked(int);
    //void ValueChangedInSpinBox();

    void onDetect();
    void onTolerChanged( double );
    void onSubShapesChk();
};

#endif // DIALOGBOX_Glue_H
