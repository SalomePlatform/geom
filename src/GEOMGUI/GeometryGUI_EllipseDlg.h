//  GEOM GEOMGUI : GUI for Geometry component
//
//  Copyright (C) 2003  OPEN CASCADE 
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
//  File   : GeometryGUI_EllipseDlg.h
//  Author : Nicolas REJNERI
//  Module : GEOM
//  $Header$

#ifndef GEOMETRYGUI_ELLIPSE_H
#define GEOMETRYGUI_ELLIPSE_H

#include "SALOME_Selection.h"
#include "GEOM_ShapeTypeFilter.hxx"
#include "GEOM_EdgeFilter.hxx"
#include "GeometryGUI_SpinBox.h"

#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QButtonGroup;
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;
class QSpinBox;
class GeometryGUI;


//=================================================================================
// class    : GeometryGUI_EllipseDlg
// purpose  :
//=================================================================================
class GeometryGUI_EllipseDlg : public QDialog
{ 
    Q_OBJECT

public:
    GeometryGUI_EllipseDlg( QWidget* parent = 0, const char* name = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0 );
    ~GeometryGUI_EllipseDlg();

private :

    void closeEvent( QCloseEvent* e ) ;
    void enterEvent( QEvent* e );
    void Init(SALOME_Selection* Sel) ;
    void MakeEllipseSimulationAndDisplay() ;

    GEOM::GEOM_Gen_var    myGeom ;                /* Current GeomI object */
    GeometryGUI*          myGeomGUI ;             /* Current GeomGUI object */
    TopoDS_Shape          mySimulationTopoDs ;    /* Shape used for simulation display */
    SALOME_Selection*     mySelection ;           /* User shape selection */   
 
    gp_Pnt                myPoint ;               /* Central point of ellipse */   
    bool                  myOkPoint ;             /* true when myPoint is defined */

    gp_Dir                myDir ;                 /* to set normal axis of ellipse */
    bool                  myOkDir ;               /* true when myPoint is defined */
    
    Standard_Real         myMajorRadius ;
    Standard_Real         myMinorRadius ;

    int                   myConstructorId ;       /* Current constructor id = radio button id */ 
    QLineEdit*            myEditCurrentArgument;  /* Current LineEdit */

    Handle(GEOM_ShapeTypeFilter) myVertexFilter;  /* filter for selection */
    Handle(GEOM_EdgeFilter)      myEdgeFilter;    /* filter for selection */

    QButtonGroup* GroupConstructors;
    QRadioButton* Constructor1;
    QGroupBox* GroupButtons;
    QPushButton* buttonApply;
    QPushButton* buttonCancel;
    QPushButton* buttonOk;
    QGroupBox* GroupC1;
    QLineEdit* LineEditC1A1;
    QLineEdit* LineEditC1A2;
    QLabel* TextLabelC1A1;
    QLabel* TextLabelC1A4;
    QLabel* TextLabelC1A3;
    QLabel* TextLabelC1A2;
    QPushButton* SelectButtonC1A1;
    QPushButton* SelectButtonC1A2;
    GeometryGUI_SpinBox* SpinBox_C1A3;
    GeometryGUI_SpinBox* SpinBox_C1A4;

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
    void ValueChangedInSpinBox(double newValue) ;

protected:
    QGridLayout* GeometryGUI_EllipseDlgLayout;
    QGridLayout* GroupConstructorsLayout;
    QGridLayout* GroupButtonsLayout;
    QGridLayout* GroupC1Layout;
};

#endif // GEOMETRYGUI_ELLIPSE_H
