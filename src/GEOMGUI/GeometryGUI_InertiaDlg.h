//  File      : GeometryGUI_InertiaDlg.h
//  Created   :
//  Author    : Lucien PIGNOLONI
//  Project   : SALOME
//  Module    : GeometryGUI
//  Copyright : OPEN CASCADE
//  $Header$

#ifndef DIALOGBOX_INERTIA_H
#define DIALOGBOX_INERTIA_H

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
// class    : GeometryGUI_InertiaDlg
// purpose  :
//=================================================================================
class GeometryGUI_InertiaDlg : public QDialog
{ 
    Q_OBJECT

public:
    GeometryGUI_InertiaDlg( QWidget* parent = 0, const char* name = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0 );
    ~GeometryGUI_InertiaDlg();

    void Init( SALOME_Selection* Sel ) ;
    void closeEvent( QCloseEvent* e ) ;
    void enterEvent ( QEvent * ) ;                /* mouse enter the QWidget */
    void CalculateAndDisplayInertia(const TopoDS_Shape& S) ;

    GEOM::GEOM_Gen_var myGeom ;                /* Current GeomI object */
    GeometryGUI*          myGeomGUI ;             /* Current GeomGUI object */
    SALOME_Selection*     mySelection ;

    int                   myConstructorId ;       /* Current constructor id = radio button id */
    QLineEdit*            myEditCurrentArgument;  /* Current LineEdit */


    QButtonGroup* GroupConstructors;
    QRadioButton* Constructor1;
    QGroupBox* GroupC1;
    QLabel* TextLabelC1A1;
    QPushButton* SelectButtonC1A1;
    QLineEdit* LineEditC1A1;
    QLabel* TextLabel_Matrix;
    QLabel* TextLabelMatrix_11;
    QLabel* TextLabelMatrix_21;
    QLabel* TextLabelMatrix_31;
    QLineEdit* LineEdit_L1C1;
    QLineEdit* LineEdit_L1C2;
    QLineEdit* LineEdit_L1C3;
    QLineEdit* LineEdit_L2C1;
    QLineEdit* LineEdit_L2C2;
    QLineEdit* LineEdit_L2C3;
    QLineEdit* LineEdit_L3C1;
    QLineEdit* LineEdit_L3C2;
    QLineEdit* LineEdit_L3C3;
    QLineEdit* LineEdit_IX;
    QLineEdit* LineEdit_IY;
    QLineEdit* LineEdit_IZ;
    QLabel* TextLabel_IXIYIZ;
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
    QGridLayout* GeometryGUI_InertiaDlgLayout;
    QGridLayout* GroupConstructorsLayout;
    QGridLayout* GroupC1Layout;
    QGridLayout* GroupButtonsLayout;
};

#endif // DIALOGBOX_INERTIA_H
