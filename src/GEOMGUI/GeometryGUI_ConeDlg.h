//  File      : GeometryGUI_ConeDlg.h
//  Created   :
//  Author    : Lucien PIGNOLONI
//  Project   : SALOME
//  Module    : GeometryGUI
//  Copyright : OPEN CASCADE
//  $Header$

#ifndef DIALOGBOX_CONE_H
#define DIALOGBOX_CONE_H

#include "SALOME_Selection.h"
#include "GEOM_ShapeTypeFilter.hxx"
#include "GEOM_EdgeFilter.hxx"
#include "GeometryGUI_SpinBox.h"

#include <BRepPrimAPI_MakeCone.hxx>
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
class QSpinBox;
class QLineEdit;
class QPushButton;
class QRadioButton;
class GeometryGUI;

//=================================================================================
// class    : GeometryGUI_ConeDlg
// purpose  :
//=================================================================================
class GeometryGUI_ConeDlg : public QDialog
{ 
    Q_OBJECT

public:
    GeometryGUI_ConeDlg( QWidget* parent = 0, const char* name = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0 );
    ~GeometryGUI_ConeDlg();

private:

    void Init( SALOME_Selection* Sel ) ;
    void closeEvent( QCloseEvent* e ) ;
    void enterEvent ( QEvent * ) ;                /* mouse enter the QWidget */
    void MakeConeSimulationAndDisplay() ;

    GEOM::GEOM_Gen_var myGeom ;                /* Current GeomI object */
    GeometryGUI*          myGeomGUI ;             /* Current GeomGUI object */
    SALOME_Selection*     mySelection ;           /* User shape selection */ 
    TopoDS_Shape          mySimulationTopoDs ;

    gp_Pnt                myPoint1 ;              /* Topology used  */
    gp_Dir                myDir ;
    bool                  myOkPoint1 ;
    bool                  myOkDir ;               /* to check when argument is defined */

    Standard_Real         myRadius1 ;
    Standard_Real         myRadius2 ; 
    Standard_Real         myHeight ;
    bool                  myOkRadius1 ;
    bool                  myOkRadius2 ;
    bool                  myOkHeight ;
    QDoubleValidator      *myVa ;                 /* Double validator for numeric input myRadius1 */
    QDoubleValidator      *myVb ;                 /* Double validator for numeric input myRadius2 */
    QDoubleValidator      *myVc ;                 /* Double validator for numeric input myHeight  */

    int                   myConstructorId ;       /* Current constructor id = radio button id */
    QWidget*              myEditCurrentArgument;  /* Current LineEdit or spin box */
    Handle(GEOM_ShapeTypeFilter) myVertexFilter ; /* Filter selection */
    Handle(GEOM_EdgeFilter) myEdgeFilter ;   /* Filter selection */


    QButtonGroup* GroupConstructors;
    QRadioButton* Constructor1;
    QRadioButton* Constructor2;
    QGroupBox* GroupButtons;
    QPushButton* buttonApply;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
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
    QLabel* TextLabelC1A4;
    GeometryGUI_SpinBox*  SpinBox_C1A4 ;
    QLabel* TextLabelC1A5;
    GeometryGUI_SpinBox*  SpinBox_C1A5 ;

    QLabel* TextLabel_Radius1 ;
    QLabel* TextLabel_Radius2 ;
    QLabel* TextLabel_Height ;
    GeometryGUI_SpinBox*  SpinBox_Radius1 ;
    GeometryGUI_SpinBox*  SpinBox_Radius2 ;
    GeometryGUI_SpinBox*  SpinBox_Height  ;

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
    QGridLayout* GeometryGUI_ConeDlgLayout;
    QGridLayout* GroupConstructorsLayout;
    QGridLayout* GroupButtonsLayout;
    QGridLayout* GroupC1Layout;
    QGridLayout* GroupC2Layout;
};

#endif // DIALOGBOX_CONE_H
