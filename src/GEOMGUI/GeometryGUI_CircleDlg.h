//  File      : GeometryGUI_CircleDlg.h
//  Created   :
//  Author    : Lucien PIGNOLONI
//  Project   : SALOME
//  Module    : GeometryGUI
//  Copyright : OPEN CASCADE
//  $Header$

#ifndef DIALOGBOX_CIRCLE_H
#define DIALOGBOX_CIRCLE_H

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
class GeometryGUI;


//=================================================================================
// class    : GeometryGUI_CircleDlg
// purpose  :
//=================================================================================
class GeometryGUI_CircleDlg : public QDialog
{ 
    Q_OBJECT

public:
    GeometryGUI_CircleDlg( QWidget* parent = 0, const char* name = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0 );
    ~GeometryGUI_CircleDlg();

private :
    
    GEOM::GEOM_Gen_var        myGeom ;                /* Current Geom object */   
    GeometryGUI*                 myGeomGUI ;             /* Current GeomGUI object */ 
    SALOME_Selection*            mySelection ;           /* User shape selection */
    TopoDS_Shape                 mySimulationTopoDs;     /* Shape used for simulation display */
    gp_Pnt                       myPoint1 ;
    gp_Dir                       myDir ;
    Standard_Real                myRadius ;
    bool                         myOkPoint1 ;
    bool                         myOkDir ;
    QLineEdit*                   myEditCurrentArgument;  /* Current LineEdit */   
    int                          myConstructorId ;       /* Current constructor id = radio button id */ 
    Handle(GEOM_ShapeTypeFilter) myVertexFilter ;        /* Filter selection */
    Handle(GEOM_EdgeFilter) myEdgeFilter; 

    void closeEvent( QCloseEvent* e ) ;
    void enterEvent( QEvent* e);
    void Init( SALOME_Selection* Sel ) ;
    void MakeCircleSimulationAndDisplay() ;

    QButtonGroup* GroupConstructors;
    QRadioButton* Constructor1;
    QGroupBox* GroupButtons;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QPushButton* buttonApply;
    QGroupBox* GroupC1;
    QPushButton* SelectButtonC1A2;
    QLineEdit* LineEditC1A1;
    QPushButton* SelectButtonC1A1;
    QLabel* TextLabelC1A2;
    QLineEdit* LineEditC1A2;
    QLabel* TextLabelC1A1;
    QLabel* TextLabelC1A3;

    GeometryGUI_SpinBox* SpinBox_C1A3;

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
    QGridLayout* GeometryGUI_CircleDlgLayout;
    QGridLayout* GroupConstructorsLayout;
    QGridLayout* GroupButtonsLayout;
    QGridLayout* GroupC1Layout;
};

#endif // DIALOGBOX_CIRCLE_H
