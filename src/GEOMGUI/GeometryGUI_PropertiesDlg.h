//  File      : GeometryGUI_PropertiesDlg.h
//  Created   :
//  Author    : Lucien PIGNOLONI
//  Project   : SALOME
//  Module    : GeometryGUI
//  Copyright : OPEN CASCADE
//  $Headers:

#ifndef DIALOGBOX_PROPERTIES_H
#define DIALOGBOX_PROPERTIES_H

#include "SALOME_Selection.h"
#include "GEOM_ShapeTypeFilter.hxx"

#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <GProp_PrincipalProps.hxx>

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
// class    : GeometryGUI_PropertiesDlg
// purpose  :
//=================================================================================
class GeometryGUI_PropertiesDlg : public QDialog
{ 
    Q_OBJECT

public:
    GeometryGUI_PropertiesDlg( QWidget* parent = 0, const char* name = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0 );
    ~GeometryGUI_PropertiesDlg();

private:

    void Init( SALOME_Selection* Sel ) ;
    void closeEvent( QCloseEvent* e ) ;
    void enterEvent ( QEvent * ) ;                         /* mouse enter the QWidget */
    void CalculateAndDisplayProperties(const TopoDS_Shape& S) ;

    GEOM::GEOM_Gen_var            myGeom ;                 /* Current GeomI object */
    GeometryGUI*                  myGeomGUI ;              /* Current GeomGUI object */
    SALOME_Selection*             mySelection ;            /* User shape selection */
    TopoDS_Shape                  mySimulationTopoDs ;     /* Shape used for simulation display */

    int                           myConstructorId ;        /* Current constructor id = radio button id */
    QLineEdit*                    myEditCurrentArgument;   /* Current LineEdit */

    QButtonGroup* GroupConstructors;
    QRadioButton* Constructor1;
    QGroupBox* GroupConstructor1;
    QLineEdit* LineEditC1A1;
    QPushButton* SelectButtonC1A1;
    QLabel* TextLabelC1A1;

    QLabel* TextLabel_Length;
    QLabel* TextLabel_Surface;
    QLabel* TextLabel_Volume;

    QLineEdit* LineEdit_Length;
    QLineEdit* LineEdit_Surface;
    QLineEdit* LineEdit_Volume;
   
    QGroupBox* GroupButtons;
    QPushButton* buttonApply;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;

private slots:

    void ConstructorsClicked(int constructorId);
    void ClickOnCancel();
    void SetEditCurrentArgument() ;
    void LineEditReturnPressed() ;
    void SelectionIntoArgument() ;
    void DeactivateActiveDialog() ;
    void ActivateThisDialog() ;
    
protected:
    QGridLayout* GeometryGUI_PropertiesDlgLayout;
    QGridLayout* GroupConstructorsLayout;
    QGridLayout* GroupConstructor1Layout;
    QGridLayout* GroupButtonsLayout;
};

#endif // DIALOGBOX_PROPERTIES_H
