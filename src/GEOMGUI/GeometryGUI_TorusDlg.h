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
//  File   : GeometryGUI_TorusDlg.h
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#ifndef DIALOGBOX_TORUS_H
#define DIALOGBOX_TORUS_H

#include "SALOME_Selection.h"
#include "GEOM_ShapeTypeFilter.hxx"
#include "GEOM_EdgeFilter.hxx"
#include "GeometryGUI_SpinBox.h"

#include <TopoDS_Shape.hxx>
#include <BRepPrimAPI_MakeTorus.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>

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
class QPushButton;
class QRadioButton;
class QSpinBox ;
class GeometryGUI;


//=================================================================================
// class    : GeometryGUI_TorusDlg
// purpose  :
//=================================================================================
class GeometryGUI_TorusDlg : public QDialog
{ 
    Q_OBJECT

public:
    GeometryGUI_TorusDlg( QWidget* parent = 0, const char* name = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0 );
    ~GeometryGUI_TorusDlg();

private:

    void Init( SALOME_Selection* Sel ) ;
    void closeEvent( QCloseEvent* e ) ;
    void enterEvent ( QEvent * ) ;                 /* mouse enter the QWidget */
    void MakeTorusSimulationAndDisplay() ;

    GEOM::GEOM_Gen_var myGeom ;                 /* Current GeomI object */
    GeometryGUI*          myGeomGUI ;              /* Current GeomGUI object */
    TopoDS_Shape          mySimulationTopoDs ;
    SALOME_Selection*     mySelection ;            /* User shape selection */

    gp_Pnt                myPoint1 ;               /* Topology used  */
    gp_Dir                myDir ;
    bool                  myOkPoint1 ;
    bool                  myOkDir ;                /* to check when arguments is defined */

    Standard_Real         myRadius1 ;
    Standard_Real         myRadius2 ;
    bool                  myOkRadius1 ;
    bool                  myOkRadius2 ;
    QDoubleValidator      *myVa ;                  /* Double validator for numeric input myRadius1 */
    QDoubleValidator      *myVb ;                  /* Double validator for numeric input myRadius2 */

    int                   myConstructorId ;        /* Current constructor id = radio button id */
    QWidget*              myEditCurrentArgument;   /* Current LineEdit or spin box */
    Handle(GEOM_ShapeTypeFilter) myVertexFilter ;  /* Filter selection */
    Handle(GEOM_EdgeFilter) myEdgeFilter ;         /* Filter selection */

    QButtonGroup* GroupConstructors;
    QRadioButton* Constructor1;
    QRadioButton* Constructor2;
    QGroupBox* GroupC1;
    QGroupBox* GroupC2;
    QPushButton* SelectButtonC1A1;
    QPushButton* SelectButtonC1A2;
    QLineEdit* LineEditC1A1;
    QLineEdit* LineEditC1A2;
    
    GeometryGUI_SpinBox* SpinBox_C1A3 ;
    GeometryGUI_SpinBox* SpinBox_C1A4 ;
     
    QLabel* TextLabelC1A1;
    QLabel* TextLabelC1A2;
    QLabel* TextLabelC1A3;
    QLabel* TextLabelC1A4;
    QGroupBox* GroupButtons;
    QPushButton* buttonApply;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;

    QLabel* TextLabel_Radius1 ;
    QLabel* TextLabel_Radius2 ;
    GeometryGUI_SpinBox* SpinBox_Radius1 ;
    GeometryGUI_SpinBox* SpinBox_Radius2 ;

private slots:

    void ConstructorsClicked(int constructorId);
    void ClickOnOk();
    void ClickOnCancel();
    void ClickOnApply();
    void SetEditCurrentArgument() ;
    void SelectionIntoArgument() ;
    void LineEditReturnPressed() ;
    void DeactivateActiveDialog() ;
    void ActivateThisDialog() ;
    void ValueChangedInSpinBox( double newValue ) ;

protected:
    QGridLayout* GeometryGUI_TorusDlgLayout;
    QGridLayout* GroupButtonsLayout;
    QGridLayout* GroupConstructorsLayout;
    QGridLayout* GroupC1Layout;
    QGridLayout* GroupC2Layout;
};

#endif // DIALOGBOX_TORUS_H
