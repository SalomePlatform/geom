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
//  File   : BasicGUI_PointDlg.h
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#ifndef DIALOGBOX_POINT_H
#define DIALOGBOX_POINT_H

#include "GEOMBase_Skeleton.h"
#include "DlgRef_1Sel1Spin.h"
#include "DlgRef_3Spin.h"
#include "DlgRef_1Sel3Spin.h"

class QLineEdit;
class QGroupBox;

//=================================================================================
// class    : BasicGUI_PointDlg
// purpose  :
//=================================================================================
class BasicGUI_PointDlg : public GEOMBase_Skeleton
{ 
    Q_OBJECT

public:
    BasicGUI_PointDlg(QWidget* parent = 0, const char* name = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0);

    ~BasicGUI_PointDlg();

    bool acceptMouseEvent() const { return ( getConstructorId() == 0 );  };
    void OnPointSelected( const gp_Pnt& ); // called by BasicGUI::OnMousePress()
    
protected:
    // redefined from GEOMBase_Helper
    virtual GEOM::GEOM_IOperations_ptr createOperation();
    virtual bool isValid( QString& );
    virtual bool execute( ObjectList& objects );

    virtual void closeEvent( QCloseEvent* e );

private :
    void Init();
    void enterEvent(QEvent* e);
    double getParameter() const;

    GEOM::GEOM_Object_var myEdge;
    GEOM::GEOM_Object_var myRefPoint; 

    DlgRef_3Spin*     GroupXYZ;
    DlgRef_1Sel3Spin* GroupRefPoint;
    DlgRef_1Sel1Spin* GroupOnCurve;

    QGroupBox*        myCoordGrp;
    QLineEdit*        myX;
    QLineEdit*        myY;
    QLineEdit*        myZ;

private slots:
    void ClickOnOk();
    void ClickOnCancel();
    bool ClickOnApply();
    void ActivateThisDialog();
    void DeactivateActiveDialog();
    void LineEditReturnPressed();
    void SelectionIntoArgument();
    void SetEditCurrentArgument();
    void ConstructorsClicked(int constructorId);
    void ValueChangedInSpinBox(double newValue);
};

#endif // DIALOGBOX_POINT_H
