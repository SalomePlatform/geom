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
//  File   : OperationGUI_ClippingDlg.h
//  Author : Michael Zorin
//  Module : GEOM

#ifndef DIALOGBOX_CLIPPINGRANGE_H
#define DIALOGBOX_CLIPPING_H

#include "GEOMBase_Skeleton.h"

class QGroupBox;
class QLabel;
class QPushButton;
class QComboBox;
class DlgRef_SpinBox;

enum ViewerTypes { VTK, OCC, OTHER };

//=================================================================================
// class    : OperationGUI_ClippingDlg
// purpose  :
//=================================================================================
class OperationGUI_ClippingDlg : public GEOMBase_Skeleton
{
    Q_OBJECT

public:
    OperationGUI_ClippingDlg(GeometryGUI* theGeometryGUI, QWidget* parent = 0);
    ~OperationGUI_ClippingDlg();

private:
    void Init();

    ViewerTypes myViewerType;

    virtual void closeEvent( QCloseEvent* e );
    void enterEvent(QEvent* e);

    QGroupBox* GroupArguments;
    QLabel* TextLabelNear;
    DlgRef_SpinBox* SpinBox_Near;
    QLabel* TextLabelFar;
    DlgRef_SpinBox* SpinBox_Far;
    QPushButton* resetButton;
    QComboBox* TypeCB;

private slots:
    void ClickOnOk();
    bool ClickOnApply();
    void onActivate();
    void onReset();
};

#endif // DIALOGBOX_CLIPPINGRANGE_H
