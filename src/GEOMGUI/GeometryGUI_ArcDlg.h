//  File      : GeometryGUI_ArcDlg.h
//  Created   :
//  Author    : Lucien PIGNOLONI
//  Project   : SALOME
//  Module    : GeometryGUI
//  Copyright : OPEN CASCADE
//  $Header$

#ifndef DIALOGBOX_ARC_H
#define DIALOGBOX_ARC_H

#include "SALOME_Selection.h"
#include "GEOM_ShapeTypeFilter.hxx"

#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <GC_MakeArcOfCircle.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <Geom_TrimmedCurve.hxx>

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
// class    : GeometryGUI_ArcDlg
// purpose  : 
//=================================================================================
class GeometryGUI_ArcDlg : public QDialog
{ 
    Q_OBJECT

public:
    GeometryGUI_ArcDlg( QWidget* parent = 0, const char* name = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0 );
    ~GeometryGUI_ArcDlg();

private :

    GEOM::GEOM_Gen_var        myGeom ;                /* Current Geom object */
    GeometryGUI*                 myGeomGUI ;             /* Current GeomGUI object */   
    SALOME_Selection*            mySelection ;           /* User shape selection */
    TopoDS_Shape                 mySimulationTopoDs;     /* Shape used for simulation display */
    gp_Pnt                       myPoint1 ;
    gp_Pnt                       myPoint2 ;
    gp_Pnt                       myPoint3;

    bool                         myOkPoint1 ;
    bool                         myOkPoint2;
    bool                         myOkPoint3;
    QLineEdit*                   myEditCurrentArgument;  /* Current LineEdit */   
    int                          myConstructorId ;       /* Current constructor id = radio button id */ 
    Handle(GEOM_ShapeTypeFilter) myVertexFilter ;        /* Filter selection */

    void closeEvent( QCloseEvent* e ) ;
    void enterEvent( QEvent* e);
    void Init( SALOME_Selection* Sel ) ;
    void MakeArcSimulationAndDisplay() ;

    QButtonGroup* GroupConstructors;
    QRadioButton* Constructor1;
    QGroupBox* GroupButtons;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QPushButton* buttonApply;
    QGroupBox* GroupC1;
    QLineEdit* LineEditC1A1;
    QPushButton* SelectButtonC1A1;
    QLabel* TextLabelC1A1;
    QLabel* TextLabelC1A2;
    QPushButton* SelectButtonC1A2;
    QLineEdit* LineEditC1A2;
    QLabel* TextLabelC1A3;
    QPushButton* SelectButtonC1A3;
    QLineEdit* LineEditC1A3;

private slots:

    void ConstructorsClicked(int constructorId);
    void ClickOnOk();
    void ClickOnCancel();
    void ClickOnApply();
    void SetEditCurrentArgument() ;
    void SelectionIntoArgument() ;
    void DeactivateActiveDialog() ;
    void LineEditReturnPressed() ;
    void ActivateThisDialog() ;

protected:
    QGridLayout* GeometryGUI_ArcDlgLayout;
    QGridLayout* GroupConstructorsLayout;
    QGridLayout* GroupButtonsLayout;
    QGridLayout* GroupC1Layout;
};

#endif // DIALOGBOX_ARC_H
