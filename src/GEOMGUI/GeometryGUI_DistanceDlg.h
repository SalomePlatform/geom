//  File      : GeometryGUI_DistanceDlg.h
//  Created   : Mon Mar 04 22:34:22 2002
//  Author    : Nicolas REJNERI
//  Project   : SALOME
//  Module    : GEOM
//  Copyright : Open CASCADE 2002
//  $Header$


#ifndef DIALOGBOX_DISTANCE_H
#define DIALOGBOX_DISTANCE_H

#include "SALOME_Selection.h"
#include "GEOM_ShapeTypeFilter.hxx"

#include <BRepExtrema_DistShapeShape.hxx>
#include <AIS_LengthDimension.hxx>

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QButtonGroup;
class QFrame;
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;
class GeometryGUI;


//=================================================================================
// class    : GeometryGUI_DistanceDlg
// purpose  :
//=================================================================================
class GeometryGUI_DistanceDlg : public QDialog
{ 
    Q_OBJECT

public:
    GeometryGUI_DistanceDlg( QWidget* parent = 0, const char* name = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0 );
    ~GeometryGUI_DistanceDlg();

private:

    void Init( SALOME_Selection* Sel ) ;
    void closeEvent( QCloseEvent* e ) ;
    void enterEvent ( QEvent * ) ;                /* mouse enter the QWidget */
    void MakeDistanceSimulationAndDisplay( const TopoDS_Shape& S1, const TopoDS_Shape& S2 ) ;
    void EraseDistance() ;

    GEOM::GEOM_Gen_var myGeom ;                /* Current GeomI object */
    GeometryGUI*          myGeomGUI ;             /* Current GeomGUI object */
    SALOME_Selection*     mySelection ;           /* User shape selection */
    TopoDS_Shape          myShape1 ;              
    TopoDS_Shape          myShape2 ;              
    GEOM::GEOM_Shape_var        myGeomShape1 ;          
    GEOM::GEOM_Shape_var        myGeomShape2 ;          
    bool                  myOkShape1 ;
    bool                  myOkShape2 ;            /* to check when arguments are defined */
    int                   myConstructorId ;       /* Current constructor id = radio button id */
    QLineEdit*            myEditCurrentArgument;  /* Current LineEdit */    
    TopoDS_Shape          mySimulationTopoDs;     /* Shape used for simulation display */

    QButtonGroup* GroupConstructors;
    QRadioButton* Constructor1;

    QGroupBox* GroupConstructor1;
    QLineEdit* LineEditC1A1Shape;
    QLineEdit* LineEditC1A2Shape;
    QPushButton* SelectButtonC1A1Shape;
    QPushButton* SelectButtonC1A2Shape;
    QLabel* TextLabelC1A2Shape;
    QLabel* TextLabelC1A1Shape;

    QLabel* TextLabel_Length;
    QLineEdit* LineEdit_Length;

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

protected:
    QGridLayout* GeometryGUI_DistanceDlgLayout;
    QGridLayout* GroupConstructorsLayout;
    QGridLayout* GroupButtonsLayout;
    QGridLayout* GroupConstructor1Layout;
};

#endif // DIALOGBOX_DISTANCE_H
