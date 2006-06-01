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
//  File   : MeasureGUI_InertiaDlg.h
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#ifndef DIALOGBOX_INERTIA_H
#define DIALOGBOX_INERTIA_H

#include "MeasureGUI_Skeleton.h"

class MeasureGUI_1Sel12LineEdit_QTD;
class gp_XYZ;
class gp_Mat;

#if defined WNT && defined MEASUREGUI_EXPORTS
#define MEASUREGUI_EXPORT __declspec( dllexport )
#else
#define MEASUREGUI_EXPORT
#endif

//=================================================================================
// class    : MeasureGUI_InertiaDlg
// purpose  : Dialog for displaying moment of inertia
//=================================================================================

class MEASUREGUI_EXPORT MeasureGUI_InertiaDlg : public MeasureGUI_Skeleton
{ 
    Q_OBJECT

public:
                                        MeasureGUI_InertiaDlg( GeometryGUI* GUI,
							       QWidget*     parent );
                                        ~MeasureGUI_InertiaDlg();
protected:

    // redefined from GEOMBase_Helper and MeasureGUI_Skeleton
    virtual void                        processObject();

private:

    void                                Init();
    bool                                getParameters( gp_Mat& theMatrix,
                                                       gp_XYZ& theMoment );

private:
      
    MeasureGUI_1Sel12LineEdit_QTD*      myGrp;
};

#endif // DIALOGBOX_INERTIA_H
