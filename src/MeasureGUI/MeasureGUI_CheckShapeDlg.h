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
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : MeasureGUI_CheckShapeDlg.h
//  Author : Nicolas REJNERI
//  Module : GEOM

#ifndef DIALOGBOX_CHECKSHAPEDLG_H
#define DIALOGBOX_CHECKSHAPEDLG_H

#include "GEOM_MeasureGUI.hxx"

#include "MeasureGUI_Skeleton.h"
#include "MeasureGUI_1Sel1TextView_QTD.h"

class QCheckBox;

//=================================================================================
// class    : MeasureGUI_1Sel1TextView1Check_QTD
// purpose  :
//=================================================================================
class MeasureGUI_1Sel1TextView1Check_QTD : public MeasureGUI_1Sel1TextView_QTD
{ 
    Q_OBJECT

public:
    MeasureGUI_1Sel1TextView1Check_QTD( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~MeasureGUI_1Sel1TextView1Check_QTD();

    QCheckBox* CheckBoxGeom;
};

//=================================================================================
// class    : MeasureGUI_CheckShapeDlg
// purpose  :
//=================================================================================
class GEOM_MEASUREGUI_EXPORT MeasureGUI_CheckShapeDlg : public MeasureGUI_Skeleton
{
    Q_OBJECT

public:
    MeasureGUI_CheckShapeDlg(GeometryGUI* GUI, QWidget* parent);
    ~MeasureGUI_CheckShapeDlg();

protected:
    // redefined from GEOMBase_Helper and MeasureGUI_Skeleton
    virtual void processObject();

private:
    void Init();
    bool getParameters(bool& theIsValid, QString& theMsg);

private:
    MeasureGUI_1Sel1TextView1Check_QTD* myGrp;
};

#endif // DIALOGBOX_CHECKSHAPEDLG_H
