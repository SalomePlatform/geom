//  File      : GeometryGUI_MultiTranslationDlg.h
//  Created   :
//  Author    : Damien COQUERET
//  Project   : SALOME
//  Module    : GeometryGUI
//  Copyright : OPEN CASCADE
//  $Header$

#ifndef DIALOGBOX_MULTITRANSLATION_H
#define DIALOGBOX_MULTITRANSLATION_H

#include "SALOME_Selection.h"
#include "GEOM_ShapeTypeFilter.hxx"
#include "GEOM_EdgeFilter.hxx"
#include "GeometryGUI_SpinBox.h"

#include <gp_Vec.hxx>
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
class QSpinBox;
class QLineEdit;
class QPushButton;
class QSpinBox;
class QRadioButton;
class GeometryGUI;

//=================================================================================
// class    : GeometryGUI_MultiTranslationDlg
// purpose  :
//=================================================================================
class GeometryGUI_MultiTranslationDlg : public QDialog
{ 
    Q_OBJECT

public:
    GeometryGUI_MultiTranslationDlg( QWidget* parent = 0, const char* name = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0 );
    ~GeometryGUI_MultiTranslationDlg();

private :
    
    GEOM::GEOM_Gen_var        myGeom ;                /* Current Geom object */   
    GeometryGUI*                 myGeomGUI ;             /* Current GeomGUI object */ 
    TopoDS_Shape                 mySimulationTopoDs ;    /* Shape used for simulation display */
    SALOME_Selection*            mySelection ;           /* User shape selection */
    TopoDS_Shape                 myBase ;
    GEOM::GEOM_Shape_var               myGeomShape ;           /* is myBase */

    gp_Vec                       myVec ;
    int                          myNbTimes1 ;
    int                          myNbTimes2 ;
    Standard_Real                myStep1 ;
    Standard_Real                myStep2 ;
    gp_Dir                       myDir1 ;
    gp_Dir                       myDir2 ;
   
    bool                         myOkBase ; 
    bool                         myOkDir1 ;
    bool                         myOkDir2 ;

    QLineEdit*                   myEditCurrentArgument;  /* Current LineEdit */   
    int                          myConstructorId ;       /* Current constructor id = radio button id */ 
    Handle(GEOM_EdgeFilter)      myEdgeFilter;           /* Filter selection */

    void closeEvent( QCloseEvent* e ) ;
    void enterEvent( QEvent* e);
    void Init( SALOME_Selection* Sel ) ;
    void MakeMultiTranslationSimulationAndDisplay() ;

    QButtonGroup* GroupConstructors;
    QRadioButton* Constructor1;
    QRadioButton* Constructor2;

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
    
    QLabel* TextLabelC1A3;
    QLabel* TextLabelC1A4;
    GeometryGUI_SpinBox* SpinBox_C1A3 ;
    QSpinBox* SpinBox_C1A4 ;

    QGroupBox* GroupC2;
    QPushButton* SelectButtonC2A2;
    QLineEdit* LineEditC2A1;
    QLineEdit* LineEditC2A2;
    QPushButton* SelectButtonC2A1;
    QLineEdit* LineEditC2A3;
    QPushButton* SelectButtonC2A3;
    QLabel* TextLabelC2A1;
    QLabel* TextLabelC2A2;
    QLabel* TextLabelC2A3;
    
    QLabel* TextLabelC2A4;
    QLabel* TextLabelC2A5;
    QLabel* TextLabelC2A6;
    QLabel* TextLabelC2A7;
    GeometryGUI_SpinBox* SpinBox_C2A4 ;
    QSpinBox* SpinBox_C2A5;
    GeometryGUI_SpinBox* SpinBox_C2A6 ;
    QSpinBox* SpinBox_C2A7 ;

    QCheckBox* CheckBoxReverse0;
    QCheckBox* CheckBoxReverse1;
    QCheckBox* CheckBoxReverse2;

private slots:

    void ConstructorsClicked(int constructorId);
    void ClickOnOk();
    void ClickOnCancel();
    void ClickOnApply();
    void SetEditCurrentArgument() ;
    void SelectionIntoArgument() ;
    void DeactivateActiveDialog() ;
    void ActivateThisDialog() ;
    void LineEditReturnPressed() ;
    void ReverseAngle1(int) ;
    void ReverseAngle2(int) ;
    void ValueChangedInSpinBox( double newValue ) ; /* for GeometryGUI_SpinBox       */
    void ValueChangedInt( int newIntValue ) ;       /* for QT spin box ! not const ! */

protected:
    QGridLayout* GeometryGUI_MultiTranslationDlgLayout;
    QGridLayout* GroupConstructorsLayout;
    QGridLayout* GroupButtonsLayout;
    QGridLayout* GroupC1Layout;
    QGridLayout* GroupC2Layout;
};

#endif // DIALOGBOX_MULTITRANSLATION_H
