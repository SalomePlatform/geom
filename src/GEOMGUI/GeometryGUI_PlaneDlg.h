//  File      : GeometryGUI_PlaneDlg.h
//  Created   :
//  Author    : Lucien PIGNOLONI
//  Project   : SALOME
//  Module    : GeometryGUI
//  Copyright : OPEN CASCADE
//  $Header$


#ifndef DIALOGBOX_PLANE_H
#define DIALOGBOX_PLANE_H

#include "SALOME_Selection.h"
#include "GEOM_ShapeTypeFilter.hxx"
#include "GEOM_FaceFilter.hxx"
#include "GeometryGUI_SpinBox.h"

#include <BRepBuilderAPI_MakeFace.hxx>
#include <gp_Pnt.hxx>

#include <qvariant.h>
#include <qdialog.h>

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
// class    : GeometryGUI_PlaneDlg
// purpose  :
//=================================================================================
class GeometryGUI_PlaneDlg : public QDialog
{ 
    Q_OBJECT

public:
    GeometryGUI_PlaneDlg( QWidget* parent = 0, const char* name = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0 );
    ~GeometryGUI_PlaneDlg();

private :
 
    void Init( SALOME_Selection* Sel ) ;
    void closeEvent( QCloseEvent* e ) ;
    void enterEvent( QEvent* e);

    GEOM::GEOM_Gen_var    myGeom ;                /* Current Geom object */
    GeometryGUI*          myGeomGUI ;             /* Current GeomGUI object */
    TopoDS_Shape          mySimulationTopoDs;     /* Shape used for simulation display */    
    SALOME_Selection*     mySelection ;           /* User shape selection */    
    gp_Pnt                myPoint1 ;              /* Point on the plane */

    Standard_Real         myDx ;
    Standard_Real         myDy ;
    Standard_Real         myDz ;
    Standard_Real         myTrimSize ;

    bool                  myOkPoint1 ;            /* true when argument is defined */
    bool                  myOkDirection ;
    bool                  myOkCoordinates ;
    bool                  myOkPlanarFace ;
    QLineEdit*            myEditCurrentArgument;  /* Current LineEdit */   
    int                   myConstructorId ;       /* Current constructor id = radio button id */
    
    Handle(GEOM_ShapeTypeFilter) myVertexFilter;  /* Filters selection */
    Handle(GEOM_ShapeTypeFilter) myEdgeFilter;    /* Filters selection */
    Handle(GEOM_FaceFilter)      myFaceFilter;    /* Filters selection */

    // Constructors
    QButtonGroup* GroupConstructors;
    QRadioButton* Constructor1;
    QRadioButton* Constructor2;
    QRadioButton* Constructor3;

    // Constructor with a point + direction (vector)
    QGroupBox* GroupPointDirection;

    QLabel* TextLabelPt1;
    QPushButton* SelectButtonPt1;
    QLineEdit* LineEditPt1;
    
    QLabel* TextLabelDirection;
    QPushButton* SelectButtonDirection;
    QLineEdit* LineEditDirection;
    
    QLabel* TextLabelC1Size;
    GeometryGUI_SpinBox*  SpinBox_C1Size ;

    // Constructor with a point + dx, dy, dz coordinates
    QGroupBox* GroupPointPlusCoordinates;

    QLabel* TextLabelPt2;
    QPushButton* SelectButtonPt2;
    QLineEdit* LineEditPt2;

    QLabel* TextLabelCoordinates;
    QLabel* TextLabel_DX;
    QLabel* TextLabel_DY;
    QLabel* TextLabel_DZ;
    GeometryGUI_SpinBox*  SpinBox_DX ;
    GeometryGUI_SpinBox*  SpinBox_DY ;
    GeometryGUI_SpinBox*  SpinBox_DZ ;

    QLabel* TextLabelC2Size;
    GeometryGUI_SpinBox*  SpinBox_C2Size ;

    // Constructor with a face
    QGroupBox* GroupFace;

    QLabel* TextLabelFace;
    QPushButton* SelectButtonFace;
    QLineEdit* LineEditFace;

    QLabel* TextLabelC3Size;
    GeometryGUI_SpinBox* SpinBox_C3Size ;

    // BUTTONS
    QGroupBox*   GroupButtons;
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
    void MakePlaneSimulationAndDisplay( const gp_Pnt& P, 
					const Standard_Real dx,
					const Standard_Real dy,
					const Standard_Real dz,
					const Standard_Real trimSize ) ;
    void ValueChangedInSpinBox( double newValue ) ;

protected:

    QGridLayout* GeometryGUI_PlaneDlgLayout;
    QGridLayout* GroupButtonsLayout;
    QGridLayout* GroupPointDirectionLayout;
    QGridLayout* GroupConstructorsLayout;
    QGridLayout* GroupPointPlusCoordinatesLayout;
    QGridLayout* GroupFaceLayout;
    QHBoxLayout* Layout2 ;
};

#endif // DIALOGBOX_PLANE_H
