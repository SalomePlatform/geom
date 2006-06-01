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
//  File   : BlocksGUI_PropagateDlg.h
//  Author : VKN
//  Module : GEOM

#ifndef DIALOGBOX_BlocksGUI_PropagateDlg_H
#define DIALOGBOX_BlocksGUI_PropagateDlg_H

#include "GEOMBase_Skeleton.h"


//=================================================================================
// class    : BlocksGUI_PropagateDlg
// purpose  :
//=================================================================================
class BlocksGUI_PropagateDlg : public GEOMBase_Skeleton
{
    Q_OBJECT

public:
    BlocksGUI_PropagateDlg(GeometryGUI* theGeometryGUI, QWidget* parent = 0,
			   const char* name = "", bool modal = FALSE, WFlags fl = 0);
    ~BlocksGUI_PropagateDlg();

protected:
    // redefined from GEOMBase_Helper
    virtual GEOM::GEOM_IOperations_ptr createOperation();
    virtual bool isValid( QString& );
    virtual bool execute( ObjectList& objects );
    virtual GEOM::GEOM_Object_ptr getFather(GEOM::GEOM_Object_ptr theObj);

private slots:
    void ClickOnOk();
    bool ClickOnApply();

    void ActivateThisDialog();

    void LineEditReturnPressed();
    void SelectionIntoArgument();
    void SetEditCurrentArgument();

private:
    void Init();
    void enterEvent(QEvent* e);
    void closeEvent(QCloseEvent* e);
    void activateSelection();

private:
    GEOM::GEOM_Object_var      myObject;
    QPushButton*               mySelBtn;
    QLineEdit*                 mySelName;
};

#endif // DIALOGBOX_BlocksGUI_PropagateDlg_H
