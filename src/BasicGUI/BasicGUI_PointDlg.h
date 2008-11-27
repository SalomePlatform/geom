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
//  File   : BasicGUI_PointDlg.h
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//
#ifndef DIALOGBOX_POINT_H
#define DIALOGBOX_POINT_H

#include "GEOM_BasicGUI.hxx"

#include "GEOMBase_Skeleton.h"
#include "DlgRef_1Sel1Spin.h"
#include "DlgRef_3Spin.h"
#include "DlgRef_2Sel_QTD.h"
#include "DlgRef_1Sel3Spin.h"
#include "DlgRef_1Sel2Spin.h"

class QLineEdit;
class QGroupBox;

//=================================================================================
// class    : BasicGUI_PointDlg
// purpose  :
//=================================================================================
class GEOM_BASICGUI_EXPORT BasicGUI_PointDlg : public GEOMBase_Skeleton
{ 
    Q_OBJECT

public:
    BasicGUI_PointDlg(GeometryGUI* theGeometryGUI, QWidget* parent = 0,
		      const char* name = 0, bool modal = FALSE, WFlags fl = 0);

    ~BasicGUI_PointDlg();

    bool acceptMouseEvent() const { return ( getConstructorId() == 0 );  };
    void OnPointSelected( const gp_Pnt& ); // called by BasicGUI::OnMousePress()

protected:
    // redefined from GEOMBase_Helper
    virtual GEOM::GEOM_IOperations_ptr createOperation();
    virtual bool isValid( QString& );
    virtual bool execute( ObjectList& objects );
    virtual void addSubshapesToStudy();
    virtual void closeEvent( QCloseEvent* e );

private :
    void Init();
    void enterEvent(QEvent* e);
    double getParameter() const;
    double getUParameter() const;
    double getVParameter() const;

    GEOM::GEOM_Object_var myEdge;
    GEOM::GEOM_Object_var myFace;
    GEOM::GEOM_Object_var myRefPoint; 
    GEOM::GEOM_Object_var myLine1; 
    GEOM::GEOM_Object_var myLine2;

    DlgRef_3Spin*     GroupXYZ;
    DlgRef_1Sel3Spin* GroupRefPoint;
    DlgRef_1Sel1Spin* GroupOnCurve;
    DlgRef_2Sel_QTD*  GroupLineIntersection;
    DlgRef_1Sel2Spin* GroupOnSurface;

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
