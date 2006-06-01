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
//  File   : MeasureGUI_BndBoxDlg.h
//  Author : Nicolas REJNERI
//  Module : GEOM

#ifndef DIALOGBOX_BNDBOX_H
#define DIALOGBOX_BNDBOX_H

#include "MeasureGUI_Skeleton.h"

class MeasureGUI_1Sel6LineEdit_QTD;

#if defined WNT && defined MEASUREGUI_EXPORTS
#define MEASUREGUI_EXPORT __declspec( dllexport )
#else
#define MEASUREGUI_EXPORT
#endif

//=================================================================================
// class    : DialogBox_PROPERTIES
// purpose  :
//=================================================================================
class MEASUREGUI_EXPORT MeasureGUI_BndBoxDlg : public MeasureGUI_Skeleton
{ 
    Q_OBJECT

public:
    MeasureGUI_BndBoxDlg(GeometryGUI* GUI,
			 QWidget*     parent);
    ~MeasureGUI_BndBoxDlg();

protected:
    // redefined from GEOMBase_Helper and MeasureGUI_Skeleton
    virtual void                        processObject();
    virtual SALOME_Prs*                 buildPrs();

private:
    void                                Init();
    bool                                getParameters( double& Xmin, double& Xmax,
                                                       double& Ymin, double& Ymax,
                                                       double& Zmin, double& Zmax );

private:
    MeasureGUI_1Sel6LineEdit_QTD*       myGrp;
};

#endif // DIALOGBOX_BNDBOX_H
