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
//  File   : MeasureGUI_MaxToleranceDlg.h
//  Author : Nicolas REJNERI
//  Module : GEOM
//  $Header$

#ifndef DIALOGBOX_MAXTOLERANCE_H
#define DIALOGBOX_MAXTOLERANCE_H

#include "GEOM_MeasureGUI.hxx"

#include "MeasureGUI_Skeleton.h"

class  MeasureGUI_1Sel6LineEdit_QTD;

//=================================================================================
// class    : DialogBox_PROPERTIES
// purpose  :
//=================================================================================
class GEOM_MEASUREGUI_EXPORT MeasureGUI_MaxToleranceDlg : public MeasureGUI_Skeleton
{ 
    Q_OBJECT

public:
                                        MeasureGUI_MaxToleranceDlg( GeometryGUI* GUI,
								    QWidget*     parent );
                                        ~MeasureGUI_MaxToleranceDlg();

protected:

    // redefined from GEOMBase_Helper and MeasureGUI_Skeleton
    virtual void                        processObject();

private:

    void                                Init();
    bool                                getParameters( double& theMinFaceToler,
                                                       double& theMaxFaceToler,
                                                       double& theMinEdgeToler,
                                                       double& theMaxEdgeToler,
                                                       double& theMinVertexToler,
                                                       double& theMaxVertexToler );
                                                       


private:

    MeasureGUI_1Sel6LineEdit_QTD*       myGrp;
};

#endif // DIALOGBOX_MAXTOLERANCE_H










