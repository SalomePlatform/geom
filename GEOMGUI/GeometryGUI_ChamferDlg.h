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
//  File   : GeometryGUI_ChamferDlg.h
//  Author : Damien COQUERET
//  Module : GEOM
//  $Header$

#ifndef DIALOGBOX_CHAMFER_H
#define DIALOGBOX_CHAMFER_H

#include "SALOME_Selection.h"
#include "GEOM_ShapeTypeFilter.hxx"
#include "GeometryGUI_SpinBox.h"

// Qt Includes
#include <qvariant.h>
#include <qdialog.h>
#include <qvalidator.h>

// Open CASCADE Includes
#include <AIS_InteractiveContext.hxx>

class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QButtonGroup;
class QFrame;
class QGroupBox;
class QLineEdit;
class QPushButton;
class QRadioButton;
class QToolButton;
class QLabel;
class GeometryGUI;

//=================================================================================
// class    : GeometryGUI_ChamferDlg
// purpose  :
//=================================================================================
class GeometryGUI_ChamferDlg : public QDialog
{ 
    Q_OBJECT

public:
    GeometryGUI_ChamferDlg( QWidget* parent = 0,
		      const char* name = 0,
		      SALOME_Selection* Sel = 0,
		      Handle (AIS_InteractiveContext) ic = 0,
		      bool modal = FALSE,
		      WFlags fl = 0 );
    
    ~GeometryGUI_ChamferDlg();

private :
 
    void Init( SALOME_Selection* Sel, Handle (AIS_InteractiveContext) ic  ) ;
    void closeEvent( QCloseEvent* e ) ;
    void enterEvent( QEvent* e);
    void ResetStateOfDialog() ;

    /* Interactive and local context management see also : bool myUseLocalContext() */
    Handle (AIS_InteractiveContext) myIC ;                 /* Interactive context */ 
    Standard_Integer                myLocalContextId ;     /* identify a local context used by this method */
    bool                            myUseLocalContext ;    /* true when this method as opened a local context  */

    QDoubleValidator      *myVa ;                  /* Double validator for numeric input */
    QDoubleValidator      *myVb ;                  /* Double validator for numeric input */

    GEOM::GEOM_Gen_var myGeom ;                       /* Current Geom object */
    GeometryGUI*          myGeomGUI ;                    /* Current GeomGUI object */
    SALOME_Selection*     mySelection ;                  /* User shape selection */    

    TopoDS_Shape          myShape ;
    bool                  myOkShape ;
    char*                 myShapeIOR ;
  
    QLineEdit*            myEditCurrentArgument;         /* Current LineEdit */   
    int                   myConstructorId ;              /* Current constructor id = radio button id */
    int                   myShapeType ;

    TopoDS_Shape          mySimulationTopoDs ;    /* Shape used for simulation display */
    void MakePreview();

    bool                  myOkD1 ;
    double                myD1 ;
    bool                  myOkD2 ;
    double                myD2 ;
    
    QButtonGroup* GroupConstructors;

    QRadioButton* Constructor1;
    QRadioButton* Constructor2;
    QRadioButton* Constructor3;

    QGroupBox* GroupButtons;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QPushButton* buttonApply;

    QGroupBox* GroupC1;
    QPushButton* SelectButtonC1A1;   
    QLineEdit* LineEditC1A1;
    QLabel* TextLabelC1A1; 
    GeometryGUI_SpinBox* SpinBox_C1A2 ;
    QLabel* TextLabelC1A2; 
    GeometryGUI_SpinBox* SpinBox_C1A3 ;
    QLabel* TextLabelC1A3; 

    QGroupBox* GroupC2;
    QPushButton* SelectButtonC2A1;   
    QLineEdit* LineEditC2A1;
    QLabel* TextLabelC2A1; 
    GeometryGUI_SpinBox* SpinBox_C2A2 ;
    QLabel* TextLabelC2A2; 
    GeometryGUI_SpinBox* SpinBox_C2A3;
    QLabel* TextLabelC2A3; 

    QGroupBox* GroupC3;
    QPushButton* SelectButtonC3A1;   
    QLineEdit* LineEditC3A1;
    QLabel* TextLabelC3A1; 
    GeometryGUI_SpinBox* SpinBox_C3A2 ;
    QLabel* TextLabelC3A2; 
    GeometryGUI_SpinBox* SpinBox_C3A3;
    QLabel* TextLabelC3A3; 

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

    QGridLayout* GeometryGUI_ChamferDlgLayout;
    QGridLayout* GroupButtonsLayout;
    QGridLayout* GroupConstructorsLayout;
    QGridLayout* GroupC1Layout;
    QGridLayout* GroupC2Layout;
    QGridLayout* GroupC3Layout;
};

#endif // DIALOGBOX_CHAMFER_H

