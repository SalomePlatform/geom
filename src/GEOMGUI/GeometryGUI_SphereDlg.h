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
//  File   : GeometryGUI_SphereDlg.h
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#ifndef DIALOGBOX_SPHERE_H
#define DIALOGBOX_SPHERE_H

#include "SALOME_Selection.h"
#include "GEOM_ShapeTypeFilter.hxx"

#include <gp_Pnt.hxx>
#include <TopoDS_Shape.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>

#include <qvariant.h>
#include <qdialog.h>
#include <qvalidator.h>

class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QButtonGroup;
class QGroupBox;
class QLabel;
class QLineEdit;
class QSpinBox;
class QPushButton;
class GeometryGUI_SpinBox;
class QRadioButton;
class GeometryGUI;


//=================================================================================
// class    : GeometryGUI_SphereDlg
// purpose  :
//=================================================================================
class GeometryGUI_SphereDlg : public QDialog
{ 
    Q_OBJECT

public:
    GeometryGUI_SphereDlg( QWidget* parent = 0, const char* name = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0 );
    ~GeometryGUI_SphereDlg();

private :
    
    GEOM::GEOM_Gen_var          myGeom ;                /* Current Geom object */   
    GeometryGUI*                   myGeomGUI ;             /* Current GeomGUI object */
    TopoDS_Shape                   mySimulationTopoDs;     /* Shape used for simulation display */    
    SALOME_Selection*              mySelection ;           /* User shape selection */    
    gp_Pnt                         myPoint1 ;              /* Points containing the vector */

    Standard_Real                  myRadius ;
    bool                           myOkRadius ;
    QDoubleValidator               *myVa ;                 /* Double validator for numeric input myRadius in LineEditC1A2 */
    QDoubleValidator               *myVb ;                 /* Double validator for numeric input myRadius in LineEditC2A1 */

    bool                           myOkPoint1 ;            /* Are true when myPoint is defined */
    QWidget*                       myEditCurrentArgument;  /* Current LineEdit or SpinBox */
    int                            myConstructorId ;       /* Current constructor id = radio button id */    
    Handle(GEOM_ShapeTypeFilter)   myVertexFilter;         /* Filter selection */

    void closeEvent( QCloseEvent* e ) ;
    void enterEvent( QEvent* e);
    void Init( SALOME_Selection* Sel ) ;

    QGroupBox* GroupButtons;
    QPushButton* buttonApply;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QButtonGroup* GroupConstructors;
    QRadioButton* Constructor1;
    QRadioButton* Constructor2;
    QGroupBox* GroupConstructor1;
    QLineEdit* LineEditC1A1;
    QPushButton* SelectButtonC1A1;
    QLabel* TextLabelC1A1;
    QLabel* TextLabelC1A2;
    // QLineEdit* LineEditC1A2;
    QGroupBox* GroupConstructor2;
    // QLineEdit* LineEditC2A1;
    GeometryGUI_SpinBox*  SpinBox_C1A2 ;
    GeometryGUI_SpinBox*  SpinBox_C2A1 ;
    QLabel* TextLabelC2A1;

private slots:

    void ConstructorsClicked(int constructorId);
    void ClickOnOk();
    void ClickOnCancel();
    void ClickOnApply();
    void LineEditReturnPressed() ;
    void SetEditCurrentArgument() ;
    void SelectionIntoArgument() ;
    void DeactivateActiveDialog() ;
    void ActivateThisDialog() ;
    void ValueChangedInSpinBox( double newValue ) ;

protected:
    QGridLayout* GeometryGUI_SphereDlgLayout;
    QGridLayout* GroupButtonsLayout;
    QGridLayout* GroupConstructorsLayout;
    QGridLayout* GroupConstructor1Layout;
    QGridLayout* GroupConstructor2Layout;
};

#endif // DIALOGBOX_SPHERE_H
