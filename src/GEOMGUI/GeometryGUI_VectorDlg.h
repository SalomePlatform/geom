//  File      : GeometryGUI_VectorDlg.h
//  Created   :
//  Author    : Lucien PIGNOLONI
//  Project   : SALOME
//  Module    : GeometryGUI
//  Copyright : OPEN CASCADE
//  $Header$


#ifndef DIALOGBOX_VECTOR_H
#define DIALOGBOX_VECTOR_H

#include "SALOME_Selection.h"
#include "GEOM_ShapeTypeFilter.hxx"
#include "GeometryGUI_SpinBox.h"

#include <gp_Pnt.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Compound.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRep_Builder.hxx>
#include <Precision.hxx>

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout; 
class QGridLayout; 
class QButtonGroup;
class QFrame;
class QCheckBox;
class QGroupBox;
class QLineEdit;
class QPushButton;
class QRadioButton;
class QToolButton;
class QLabel;
class GeometryGUI;


//=================================================================================
// class    : GeometryGUI_VectorDlg
// purpose  :
//=================================================================================
class GeometryGUI_VectorDlg : public QDialog
{ 
    Q_OBJECT

public:
    GeometryGUI_VectorDlg( QWidget* parent = 0, const char* name = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0 );
    ~GeometryGUI_VectorDlg();
    
    bool AddArrowToSimulation( TopoDS_Shape& modifiedShape ) ;

private :
    
    void closeEvent( QCloseEvent* e ) ;
    void enterEvent( QEvent* e);
    void Init( SALOME_Selection* Sel ) ;                     /* mouse enter the QWidget */

    GEOM::GEOM_Gen_var               myGeom ;                /* Current Geom object */
    GeometryGUI*                     myGeomGUI ;             /* Current GeomGUI object */
    TopoDS_Shape                     mySimulationTopoDs;     /* Shape used for simulation display */    
    SALOME_Selection*                mySelection ;           /* User shape selection */    
    gp_Pnt                           myPoint1 ;              /* Points containing the vector */   
    gp_Pnt                           myPoint2 ;
    double                           myDx ;
    double                           myDy ;
    double                           myDz ;
    bool                             myOkPoint1 ;            /* Are true when myPoint is defined */    
    bool                             myOkPoint2 ; 
    QWidget*                         myEditCurrentArgument;  /* Current LineEdit or Spin box */   
    int                              myConstructorId ;       /* Current constructor id = radio button id */    
    Handle(GEOM_ShapeTypeFilter)     myVertexFilter;         /* Filter selection */
    Handle(GEOM_ShapeTypeFilter)     myEdgeFilter;           /* Filter selection */

    QGroupBox* GroupPoints;
    QPushButton* SelectButtonPt1;
    QPushButton* SelectButtonPt2;
    QLabel* TextLabelPt1;
    QLabel* TextLabelPt2;

    QButtonGroup* GroupConstructors;
    QRadioButton* Constructor2;
    QRadioButton* Constructor1;

    QGroupBox* GroupButtons;
    QPushButton* buttonApply;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;

    QLineEdit* LineEditPt1;
    QLineEdit* LineEditPt2;

    QGroupBox* GroupCoordinates;

    GeometryGUI_SpinBox*  SpinBox_DX ;
    GeometryGUI_SpinBox*  SpinBox_DY ;
    GeometryGUI_SpinBox*  SpinBox_DZ ;
    QLabel* TextLabel_DZ;
    QLabel* TextLabel_DX;
    QLabel* TextLabel_DY;

    QCheckBox* CheckBoxReverseVector;

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

    QGridLayout* GeometryGUI_VectorDlgLayout;
    QGridLayout* GroupPointsLayout;
    QGridLayout* GroupButtonsLayout;
    QGridLayout* GroupConstructorsLayout;
    QGridLayout* GroupCoordinatesLayout;

};

#endif // DIALOGBOX_VECTOR_H
