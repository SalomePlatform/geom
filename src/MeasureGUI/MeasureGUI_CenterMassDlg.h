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
//  File   : MeasureGUI_CenterMassDlg.h
//  Author : Lucien PIGNOLONI
//  Module : GEOM

#ifndef DIALOGBOX_CMASS_H
#define DIALOGBOX_CMASS_H

#include "GEOMBase_Skeleton.h"

class MeasureGUI_1Sel3LineEdit_QTD;

//=================================================================================
// class    : MeasureGUI_CenterMassDlg
// purpose  :
//=================================================================================
class MeasureGUI_CenterMassDlg : public GEOMBase_Skeleton
{
    Q_OBJECT

public:
                                        MeasureGUI_CenterMassDlg( QWidget* parent,
                                                                  SALOME_Selection* Sel );
                                        ~MeasureGUI_CenterMassDlg();

protected:
                                        
    // redefined from GEOMBase_Helper
    virtual GEOM::GEOM_IOperations_ptr  createOperation();
    virtual bool                        isValid( QString& msg );
    virtual bool                        execute( ObjectList& objects );

private slots:
    void                                ClickOnOk();
    bool                                ClickOnApply();
    void                                ActivateThisDialog();
    void                                LineEditReturnPressed();
    void                                SelectionIntoArgument();
    void                                SetEditCurrentArgument();    

private:

    void                                Init( SALOME_Selection* );
    void                                enterEvent( QEvent* e );
    void                                processObject();
    bool                                getParameters( double&, double&, double& );

private:
    
    GEOM::GEOM_Object_var               myObj;
    MeasureGUI_1Sel3LineEdit_QTD*       myGrp;
};

#endif // DIALOGBOX_CMASS_H
