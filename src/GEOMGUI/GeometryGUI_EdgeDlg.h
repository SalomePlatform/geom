//  File      : GeometryGUI_EdgeDlg.h
//  Created   :
//  Author    : Lucien PIGNOLONI
//  Project   : SALOME
//  Module    : GeometryGUI
//  Copyright : OPEN CASCADE
//  $Header$

#ifndef DIALOGBOX_EDGE_H
#define DIALOGBOX_EDGE_H

#include "SALOME_Selection.h"
#include "GEOM_ShapeTypeFilter.hxx"

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <Precision.hxx>
#include <gp_Pnt.hxx>

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
// class    : GeometryGUI_EdgeDlg
// purpose  :
//=================================================================================
class GeometryGUI_EdgeDlg : public QDialog
{ 
    Q_OBJECT

public:
    GeometryGUI_EdgeDlg( QWidget* parent = 0, const char* name = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0 );
    ~GeometryGUI_EdgeDlg();

private :
    
    GEOM::GEOM_Gen_var        myGeom ;               /* Current Geom object */   
    GeometryGUI*                 myGeomGUI ;            /* Current GeomGUI object */
    TopoDS_Shape                 mySimulationTopoDs;    /* Shape used for simulation display */    
    SALOME_Selection*            mySelection ;          /* User shape selection */    
    gp_Pnt                       myPoint1 ;             /* Points containing the vector */   
    gp_Pnt                       myPoint2 ;

    bool                         myOkPoint1 ;            /* true when myPoint is defined */    
    bool                         myOkPoint2 ; 
    QLineEdit*                   myEditCurrentArgument;  /* Current LineEdit */   
    int                          myConstructorId ;       /* Current constructor id = radio button id */    
    Handle(GEOM_ShapeTypeFilter) myVertexFilter;         /* Filter selection */

    void closeEvent( QCloseEvent* e ) ;
    void enterEvent( QEvent* e);
    void Init( SALOME_Selection* Sel ) ;

    QButtonGroup* GroupConstructors;
    QRadioButton* Constructor1;
    QGroupBox* GroupC1;
    QPushButton* SelectButtonC1A2;
    QLineEdit* LineEditC1A1;
    QLineEdit* LineEditC1A2;
    QPushButton* SelectButtonC1A1;
    QLabel* TextLabelC1A1;
    QLabel* TextLabelC1A2;
    QGroupBox* GroupButtons;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QPushButton* buttonApply;

private slots:

    void ConstructorsClicked(int constructorId);
    void ClickOnOk();
    void ClickOnCancel();
    void ClickOnApply();
    void SetEditCurrentArgument() ;
    void LineEditReturnPressed() ;
    void SelectionIntoArgument() ;
    void DeactivateActiveDialog() ;
    void ActivateThisDialog() ;

protected:
    QGridLayout* GeometryGUI_EdgeDlgLayout;
    QGridLayout* GroupConstructorsLayout;
    QGridLayout* GroupC1Layout;
    QGridLayout* GroupButtonsLayout;
};

#endif // DIALOGBOX_EDGE_H
