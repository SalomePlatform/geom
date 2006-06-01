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
//  File   : TransformationGUI_MultiTranslationDlg.h
//  Author : Damien COQUERET
//  Module : GEOM

#ifndef DIALOGBOX_MULTITRANSLATION_H
#define DIALOGBOX_MULTITRANSLATION_H

#include "GEOMBase_Skeleton.h"
#include "DlgRef_2Sel2Spin1Check.h"
#include "DlgRef_3Sel4Spin2Check.h"


//=================================================================================
// class    : TransformationGUI_MultiTranslationDlg
// purpose  :
//=================================================================================
class TransformationGUI_MultiTranslationDlg : public GEOMBase_Skeleton
{ 
    Q_OBJECT

public:
    TransformationGUI_MultiTranslationDlg(GeometryGUI* theGeometryGUI, QWidget* parent = 0,
					  const char* name = 0, bool modal = FALSE, WFlags fl = 0);
    ~TransformationGUI_MultiTranslationDlg();

protected:
    // redefined from GEOMBase_Helper
    virtual GEOM::GEOM_IOperations_ptr createOperation();
    virtual bool isValid( QString& );
    virtual bool execute( ObjectList& objects );
    
    virtual void closeEvent( QCloseEvent* e );

private :
    void Init();
    void enterEvent(QEvent* e);

    GEOM::GEOM_Object_var myBase, myVectorU, myVectorV ;
    int myNbTimesU;
    int myNbTimesV;
    Standard_Real myStepU;
    Standard_Real myStepV;
    
    DlgRef_2Sel2Spin1Check* GroupPoints;
    DlgRef_3Sel4Spin2Check* GroupDimensions;

private slots:
    void ClickOnOk();
    bool ClickOnApply();
    void ActivateThisDialog();
    void LineEditReturnPressed();
    void SelectionIntoArgument();
    void SetEditCurrentArgument();
    void ReverseStepU();
    void ReverseStepV();
    void ValueChangedInSpinBox(double newValue);
    void ConstructorsClicked(int constructorId);
};

#endif // DIALOGBOX_MULTITRANSLATION_H
