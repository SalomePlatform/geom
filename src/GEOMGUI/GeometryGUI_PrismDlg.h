//  File      : GeometryGUI_PrismDlg.h
//  Created   :
//  Author    : Lucien PIGNOLONI
//  Project   : SALOME
//  Module    : GeometryGUI
//  Copyright : OPEN CASCADE
//  $Header$


#ifndef DIALOGBOX_PRISM_H
#define DIALOGBOX_PRISM_H

#include "SALOME_Selection.h"
#include "GEOM_ShapeTypeFilter.hxx"
#include "GeometryGUI_SpinBox.h"

#include <BRepPrimAPI_MakePrism.hxx>

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QButtonGroup;
class QCheckBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;
class GeometryGUI;


//=================================================================================
// class    : GeometryGUI_PrismDlg
// purpose  :
//=================================================================================
class GeometryGUI_PrismDlg : public QDialog
{ 
    Q_OBJECT

public:
    GeometryGUI_PrismDlg( QWidget* parent = 0, const char* name = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0 );
    ~GeometryGUI_PrismDlg();

private :

    void Init( SALOME_Selection* Sel ) ;
    void closeEvent( QCloseEvent* e ) ;
    void enterEvent( QEvent* e);
    void MakePrismSimulationAndDisplay( const TopoDS_Shape& S) ;

    GEOM::GEOM_Gen_var myGeom ;               /* Current Geom object */
    GeometryGUI*          myGeomGUI ;            /* Current GeomGUI object */
    TopoDS_Shape          mySimulationTopoDs;    /* Shape used for simulation display */    
    SALOME_Selection*     mySelection ;          /* User shape selection */
    Standard_Real         myDx ;
    Standard_Real         myDy ;
    Standard_Real         myDz ;
    Standard_Real         myHeight ;             /* Height used for prism (extrusion) */
    TopoDS_Shape          myBaseTopo ;           /* topology used as base of prism */  
    GEOM::GEOM_Shape_var        myGeomShape ;          /* that is myBaseTopo */
    bool                  myOkBase ;
    bool                  myOkLine ;             /* to check when arguments myDx, myDy, myDz are defined */
    QLineEdit*            myEditCurrentArgument; /* Current LineEdit */    
    int                   myConstructorId ;      /* Current constructor id = radio button id */    
    Handle(GEOM_ShapeTypeFilter) myEdgeFilter;   /* Filters selection */

    QButtonGroup* GroupConstructors;
    QRadioButton* Constructor1;    
    QGroupBox* GroupConstructor1;

    QLineEdit* LineEditC1A1Base;
    QLineEdit* LineEditC1A2Line;
    QPushButton* SelectButtonC1A1Base;
    QPushButton* SelectButtonC1A2Line;
    QLabel* TextLabelBase;
    QLabel* TextLabelVector;
    GeometryGUI_SpinBox* SpinBox_C1A3Height;
    QCheckBox* CheckBoxC3A1Reverse;
    QLabel* TextLabelHeight;

    QGroupBox* GroupButtons;
    QPushButton* buttonApply;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;

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
    void ReverseVector(int state) ;
    void ValueChangedInSpinBox( double newValue ) ;

protected:
    QGridLayout* GeometryGUI_PrismDlgLayout;
    QGridLayout* GroupConstructorsLayout;
    QGridLayout* GroupConstructor1Layout;
    QGridLayout* GroupButtonsLayout;
};

#endif // DIALOGBOX_PRISM_H
