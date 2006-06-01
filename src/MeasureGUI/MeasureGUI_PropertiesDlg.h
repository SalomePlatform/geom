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
//  File   : MeasureGUI_PropertiesDlg.h
//  Author : Lucien PIGNOLONI
//  Module : GEOM

#ifndef DIALOGBOX_PROPERTIES_H
#define DIALOGBOX_PROPERTIES_H

#include "MeasureGUI_Skeleton.h"
class MeasureGUI_1Sel3LineEdit_QTD;

#if defined WNT && defined MEASUREGUI_EXPORTS
#define MEASUREGUI_EXPORT __declspec( dllexport )
#else
#define MEASUREGUI_EXPORT
#endif

//=================================================================================
// class    : MeasureGUI_PropertiesDlg
// purpose  : Dialog for displaying length, area and volume
//=================================================================================

class MEASUREGUI_EXPORT MeasureGUI_PropertiesDlg : public MeasureGUI_Skeleton
{ 
    Q_OBJECT

public:
                                        MeasureGUI_PropertiesDlg( GeometryGUI* GUI,
								  QWidget*     parent );
                                        ~MeasureGUI_PropertiesDlg();

protected:

    // redefined from GEOMBase_Helper and MeasureGUI_Skeleton
    virtual void                        processObject();
    virtual void                        activateSelection();
    virtual SALOME_Prs*                 buildPrs();

private:

    void                                Init();
    bool                                getParameters( double& theLength,
                                                       double& theArea,
                                                       double& theVolume );

private:    

    MeasureGUI_1Sel3LineEdit_QTD*       myGrp;
};

#endif // DIALOGBOX_PROPERTIES_H














