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
//  File   : RepairGUI_GlueDlg.h
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#ifndef DIALOGBOX_Glue_H
#define DIALOGBOX_Glue_H

#include "GEOMBase_Skeleton.h"

class QAD_SpinBoxDbl;
class DlgRef_1Sel_Ext;

//=================================================================================
// class    : RepairGUI_GlueDlg
// purpose  :
//=================================================================================
class RepairGUI_GlueDlg : public GEOMBase_Skeleton
{
    Q_OBJECT

public:
    RepairGUI_GlueDlg(QWidget* parent = 0, const char* name = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0);
    ~RepairGUI_GlueDlg();

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

    bool onAcceptLocal( const bool publish = true, const bool useTransaction = true );
    void clearShapeBufferLocal( GEOM::GEOM_Object_ptr );
    // Reimplementation of onAccept for local case of this class.

    GEOM::GEOM_Object_var myObject;

    DlgRef_1Sel_Ext* GroupPoints;
    QAD_SpinBoxDbl*  myTolEdt;

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

#endif // DIALOGBOX_Glue_H
