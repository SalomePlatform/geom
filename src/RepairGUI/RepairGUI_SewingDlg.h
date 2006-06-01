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
//  File   : RepairGUI_SewingDlg.h
//  Author : Lucien PIGNOLONI
//  Module : GEOM

#ifndef DIALOGBOX_Sewing_H
#define DIALOGBOX_Sewing_H

#include "GEOMBase_Skeleton.h"
#include "DlgRef_1Sel_Ext.h"
#include "QtxDblSpinBox.h"

#include <TColStd_IndexedMapOfInteger.hxx>

#include <qcheckbox.h>
#include <qbuttongroup.h>
#include <qlineedit.h>
#include <qlabel.h>

//=================================================================================
// class    : RepairGUI_SewingDlg
// purpose  :
//=================================================================================
class RepairGUI_SewingDlg : public GEOMBase_Skeleton
{
    Q_OBJECT

public:
    RepairGUI_SewingDlg(GeometryGUI* theGeometryGUI, QWidget* parent = 0,
			const char* name = 0, bool modal = FALSE, WFlags fl = 0);
    ~RepairGUI_SewingDlg();

protected:
    // redefined from GEOMBase_Helper
    virtual GEOM::GEOM_IOperations_ptr createOperation();
    virtual bool isValid( QString& );
    virtual bool execute( ObjectList& objects );

private:
    void Init();
    void enterEvent(QEvent* e);
    void closeEvent(QCloseEvent* e);
    void initSelection();

    GEOM::GEOM_Object_var myObject;

    DlgRef_1Sel_Ext* GroupPoints;
    QtxDblSpinBox*   myTolEdt;
    QPushButton*     myFreeBoundBtn;

    int myClosed; // Number of free closed boundaries detected. Calculated in execute(), used in onDetect().
    int myOpen;   // Number of free open   boundaries detected. Calculated in execute(), used in onDetect().

private slots:
    void ClickOnOk();
    bool ClickOnApply();

    void ActivateThisDialog();

    void LineEditReturnPressed();
    void SelectionIntoArgument();
    void SetEditCurrentArgument();

    void onDetect();
};

#endif // DIALOGBOX_Sewing_H
