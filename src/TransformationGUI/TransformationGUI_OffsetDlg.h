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
//  File   : TransformationGUI_OffsetDlg.h
//  Author : Michael Zorin
//  Module : GEOM

#ifndef DIALOGBOX_OFFSET_H
#define DIALOGBOX_OFFSET_H

#include "GEOMBase_Skeleton.h"
#include "DlgRef_1Sel1Spin1Check.h"
  
  
//=================================================================================
// class    : TransformationGUI_OffsetDlg
// purpose  :
//=================================================================================
class TransformationGUI_OffsetDlg : public GEOMBase_Skeleton
{ 
    Q_OBJECT

public:
    TransformationGUI_OffsetDlg(GeometryGUI* theGeometryGUI, QWidget* parent = 0,
				const char* name = 0, bool modal = FALSE, WFlags fl = 0);
    ~TransformationGUI_OffsetDlg();

protected:
    // redefined from GEOMBase_Helper
    virtual GEOM::GEOM_IOperations_ptr createOperation();
    virtual bool isValid( QString& );
    virtual bool execute( ObjectList& objects );

    virtual void closeEvent( QCloseEvent* e );

private:
    void Init();
    void enterEvent(QEvent* e);
    double GetOffset() const;
    
    GEOM::ListOfGO myObjects;
    
    DlgRef_1Sel1Spin1Check* GroupPoints;
    
private slots:
    void ClickOnOk();
    bool ClickOnApply();
    void ActivateThisDialog();
    void SelectionIntoArgument();
    void LineEditReturnPressed();
    void SetEditCurrentArgument();
    void ValueChangedInSpinBox();
    void CreateCopyModeChanged(bool isCreateCopy);
};

#endif // DIALOGBOX_OFFSET_H
