//  File      : GeometryGUI_OrientationDlg.h
//  Created   :
//  Author    : Lucien PIGNOLONI
//  Project   : SALOME
//  Module    : GeometryGUI
//  Copyright : OPEN CASCADE
//  $Header$

#ifndef DIALOGBOX_ORIENTATION_H
#define DIALOGBOX_ORIENTATION_H

#include "SALOME_Selection.h"
#include "GEOM_ShapeTypeFilter.hxx"
#include "GeometryGUI_SpinBox.h"

#include <Precision.hxx>

#include <qvariant.h>
#include <qdialog.h>
#include <qvalidator.h>

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
// class    : DialogBox_ORIENTATION
// purpose  :
//=================================================================================
class GeometryGUI_OrientationDlg : public QDialog
{ 
    Q_OBJECT

public:
    GeometryGUI_OrientationDlg( QWidget* parent = 0, const char* name = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0 );
    ~GeometryGUI_OrientationDlg();

private :
    
    GEOM::GEOM_Gen_var            myGeom ;                 /* Current Geom object */
    GeometryGUI*                     myGeomGUI ;              /* Current GeomGUI object */
    TopoDS_Shape                     mySimulationTopoDs ;     /* Shape used for simulation display */
    SALOME_Selection*                mySelection ;            /* User shape selection */
    GEOM::GEOM_Shape_var                   myGeomShape ;            /* is myShape */
    TopoDS_Shape                     myShape ;                /* topology used */
    Standard_Real                    myLength ;               /* to simulate normal vector */
    bool                             myOkShape ;
    bool                             myOkLength ;
    QDoubleValidator                 *myVa ;                  /* Double validator for numeric input */
    QLineEdit*                       myEditCurrentArgument;   /* Current LineEdit */
    int                              myConstructorId ;        /* Current constructor id = radio button id */

    void closeEvent( QCloseEvent* e ) ;
    void enterEvent( QEvent* e);
    void Init( SALOME_Selection* Sel ) ;
    void MakeOrientationSimulationAndDisplay( const TopoDS_Shape& aTopoDS, Standard_Real length ) ;

    QButtonGroup* GroupConstructors;
    QRadioButton* Constructor1;
    QGroupBox* GroupButtons;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QPushButton* buttonApply;
    QGroupBox* GroupC1;
    QLineEdit* LineEditC1A1;
    GeometryGUI_SpinBox*  SpinBox_C1A2 ;

    QPushButton* SelectButtonC1A1;
    QLabel* TextLabelC1A1;
    QLabel* TextLabelC1A2;
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
    void ReverseOrientation(int state) ;
    void ValueChangedInSpinBox( double newValue ) ;

protected:

    QGridLayout* GeometryGUI_OrientationDlgLayout;
    QGridLayout* GroupConstructorsLayout;
    QGridLayout* GroupButtonsLayout;
    QGridLayout* GroupC1Layout;
};

#endif // DIALOGBOX_ORIENTATION_H
