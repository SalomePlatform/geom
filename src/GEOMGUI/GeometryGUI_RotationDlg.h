//  File      : GeometryGUI_RotationDlg.h
//  Created   :
//  Author    : Lucien PIGNOLONI
//  Project   : SALOME
//  Module    : GeometryGUI
//  Copyright : OPEN CASCADE
//  $Header$

#ifndef DIALOGBOX_ROTATION_H
#define DIALOGBOX_ROTATION_H


#include "SALOME_Selection.h"
#include "GEOM_ShapeTypeFilter.hxx"
#include "GEOM_EdgeFilter.hxx"
#include "GeometryGUI_SpinBox.h"

#include <TopLoc_Location.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>

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
// class    : GeometryGUI_RotationDlg
// purpose  :
//=================================================================================
class GeometryGUI_RotationDlg : public QDialog
{ 
    Q_OBJECT

public:
    GeometryGUI_RotationDlg( QWidget* parent = 0, const char* name = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0 );
    ~GeometryGUI_RotationDlg();

private :
    
    GEOM::GEOM_Gen_var        myGeom ;                /* Current Geom object */   
    GeometryGUI*                 myGeomGUI ;             /* Current GeomGUI object */ 
    SALOME_Selection*            mySelection ;           /* User shape selection */
    TopoDS_Shape                 mySimulationTopoDs;     /* Shape used for simulation display */
    TopoDS_Shape                 myBase ;
    GEOM::GEOM_Shape_var               myGeomShape ;           /* is myBase */

    gp_Pnt                       myLoc ;
    gp_Dir                       myDir ;    
    Standard_Real                myAngle ;
   
    bool                         myOkBase ; 
    bool                         myOkAxis ;
    QLineEdit*                   myEditCurrentArgument;  /* Current LineEdit */   
    int                          myConstructorId ;       /* Current constructor id = radio button id */ 
    Handle(GEOM_EdgeFilter)      myEdgeFilter;           /* Filter selection */

    void closeEvent( QCloseEvent* e ) ;
    void enterEvent( QEvent* e);
    void Init( SALOME_Selection* Sel ) ;
    void MakeRotationSimulationAndDisplay( const TopoDS_Shape& S) ;

    QButtonGroup* GroupConstructors;
    QRadioButton* Constructor1;
    QGroupBox* GroupButtons;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QPushButton* buttonApply;
    QGroupBox* GroupC1;
    QPushButton* SelectButtonC1A2;
    QLineEdit* LineEditC1A1;
    QLineEdit* LineEditC1A2;
    QPushButton* SelectButtonC1A1;
    QLabel* TextLabelC1A1;
    QLabel* TextLabelC1A2;
    GeometryGUI_SpinBox* SpinBox_C1A3 ; /* for angle */
    QLabel* TextLabelC1A3;
    QCheckBox* CheckBoxReverse;

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
    void ReverseAngle(int state) ;
    void ValueChangedInSpinBox( double newValue ) ;

protected:
    QGridLayout* GeometryGUI_RotationDlgLayout;
    QGridLayout* GroupConstructorsLayout;
    QGridLayout* GroupButtonsLayout;
    QGridLayout* GroupC1Layout;
};

#endif // DIALOGBOX_ROTATION_H
