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
//  File   : MeasureGUI_NormaleDlg.h
//  Author : Julia DOROVSKIKH
//  Module : GEOM
//
#ifndef DIALOGBOX_NORMALE_H
#define DIALOGBOX_NORMALE_H

#include "GEOM_MeasureGUI.hxx"

#include "GEOMBase_Skeleton.h"
#include "DlgRef_2Sel_QTD.h"

//=================================================================================
// class    : MeasureGUI_NormaleDlg
// purpose  :
//=================================================================================
class GEOM_MEASUREGUI_EXPORT MeasureGUI_NormaleDlg : public GEOMBase_Skeleton
{
    Q_OBJECT

public:
    MeasureGUI_NormaleDlg (GeometryGUI* theGeometryGUI, QWidget* parent = 0,
			   const char* name = 0, bool modal = FALSE, WFlags fl = 0);
    ~MeasureGUI_NormaleDlg();

protected:
    // redefined from GEOMBase_Helper
    virtual GEOM::GEOM_IOperations_ptr createOperation();
    virtual bool isValid (QString& msg);
    virtual bool execute (ObjectList& objects);

private:
    void Init();
    void enterEvent (QEvent* e);

    GEOM::GEOM_Object_var myFace;
    GEOM::GEOM_Object_var myPoint;

    DlgRef_2Sel_QTD* GroupArgs;

private slots:
    void ClickOnOk();
    bool ClickOnApply();

    void ActivateThisDialog();
    void LineEditReturnPressed();
    void SelectionIntoArgument();
    void SetEditCurrentArgument();
};

#endif // DIALOGBOX_NORMALE_H
