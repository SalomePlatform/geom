//  File      : GeometryGUI_CylinderDlg.h
//  Created   :
//  Author    : Lucien PIGNOLONI
//  Project   : SALOME
//  Module    : GeometryGUI
//  Copyright : OPEN CASCADE
//  $Header$

#ifndef DIALOGBOX_CYLINDER_H
#define DIALOGBOX_CYLINDER_H

#include "SALOME_Selection.h"

#include "GEOM_ShapeTypeFilter.hxx"
#include "GEOM_EdgeFilter.hxx"
#include "GeometryGUI_SpinBox.h"

#include <BRepPrimAPI_MakeCylinder.hxx>
#include <Precision.hxx>

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
class QSpinBox;
class QRadioButton;
class GeometryGUI;


//=================================================================================
// class    : GeometryGUI_CylinderDlg
// purpose  :
//=================================================================================
class GeometryGUI_CylinderDlg : public QDialog
{ 
    Q_OBJECT

public:
    GeometryGUI_CylinderDlg( QWidget* parent = 0, const char* name = 0, SALOME_Selection* Sel = 0,  bool modal = FALSE, WFlags fl = 0 );
    ~GeometryGUI_CylinderDlg();

private:

    void Init( SALOME_Selection* Sel ) ;
    void closeEvent( QCloseEvent* e ) ;
    void enterEvent ( QEvent * ) ;                /* mouse enter the QWidget */
    void MakeCylinderSimulationAndDisplay() ;

    GEOM::GEOM_Gen_var myGeom ;                /* Current GeomI object */
    GeometryGUI*          myGeomGUI ;             /* Current GeomGUI object */
    TopoDS_Shape          mySimulationTopoDs ;
    SALOME_Selection*     mySelection ;           /* User shape selection */
    
    gp_Pnt                myPoint1 ;              /* topology used  */
    gp_Dir                myDir ;

    Standard_Real         myRadius ;
    Standard_Real         myHeight ;
    bool                  myOkRadius ;
    bool                  myOkHeight ;
    QDoubleValidator      *myVa ;                  /* Double validator for numeric input */
    QDoubleValidator      *myVb ;                  /* Double validator for numeric input */

    bool                  myOkPoint1 ;
    bool                  myOkDir ;               /* to check when arguments is defined */
    int                   myConstructorId ;       /* Current constructor id = radio button id */
    QWidget*              myEditCurrentArgument;  /* Current LineEdit or spin box */
    Handle(GEOM_ShapeTypeFilter) myVertexFilter ; /* Filter selection */
    Handle(GEOM_EdgeFilter) myEdgeFilter ;        /* Filter selection */

    QGroupBox* GroupButtons;
    QPushButton* buttonApply;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QButtonGroup* GroupConstructors;
    QRadioButton* Constructor1;
    QRadioButton* Constructor2;
    QGroupBox* GroupC1;
    QGroupBox* GroupC2;
    QPushButton* SelectButtonC1A1;
    QLineEdit* LineEditC1A1;
    QLabel* TextLabelC1A1;
    QPushButton* SelectButtonC1A2;
    QLineEdit* LineEditC1A2;
    QLabel* TextLabelC1A2;

    QLabel* TextLabelC1A3;
    GeometryGUI_SpinBox*  SpinBox_C1A3 ;
    QLabel* TextLabelC1A4 ;
    GeometryGUI_SpinBox*  SpinBox_C1A4 ;

    QLabel* TextLabel_Radius ;
    GeometryGUI_SpinBox*  SpinBox_Radius ;
    QLabel* TextLabel_Height ;
    GeometryGUI_SpinBox*  SpinBox_Height ;

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
    QGridLayout* GeometryGUI_CylinderDlgLayout;
    QGridLayout* GroupButtonsLayout;
    QGridLayout* GroupConstructorsLayout;
    QGridLayout* GroupC1Layout;
    QGridLayout* GroupC2Layout;
};

#endif // DIALOGBOX_CYLINDER_H
