//  File      : GeometryGUI_BndBoxDlg.h
//  Created   : Mon Mar 04 13:12:22 2002
//  Author    : Nicolas REJNERI

//  Project   : SALOME
//  Module    : GEOMGUI
//  Copyright : Open CASCADE 2002
//  $Header$

#ifndef DIALOGBOX_BNDBOX_H
#define DIALOGBOX_BNDBOX_H

#include "SALOME_Selection.h"
#include "GEOM_ShapeTypeFilter.hxx"

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
// class    : DialogBox_PROPERTIES
// purpose  :
//=================================================================================
class GeometryGUI_BndBoxDlg : public QDialog
{ 
    Q_OBJECT

public:
    GeometryGUI_BndBoxDlg( QWidget* parent = 0, const char* name = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0 );
    ~GeometryGUI_BndBoxDlg();

private:

    void Init( SALOME_Selection* Sel ) ;
    void closeEvent( QCloseEvent* e ) ;
    void enterEvent ( QEvent * ) ;                /* mouse enter the QWidget */
    void CalculateAndDisplayBndBox(const TopoDS_Shape& S) ;

    GEOM::GEOM_Gen_var myGeom ;                /* Current GeomI object */
    GeometryGUI*          myGeomGUI ;             /* Current GeomGUI object */
    SALOME_Selection*     mySelection ;           /* User shape selection */
    int                   myConstructorId ;       /* Current constructor id = radio button id */
    QLineEdit*            myEditCurrentArgument;  /* Current LineEdit */
    TopoDS_Shape          mySimulationTopoDs ;    /* Shape used for simulation display */

    QButtonGroup* GroupConstructors;
    QRadioButton* Constructor1;
    QGroupBox* GroupConstructor1;
    QLineEdit* LineEditC1A1;
    QPushButton* SelectButtonC1A1;
    QLabel* TextLabelC1A1;

    QLabel* TextLabel_Min;
    QLabel* TextLabel_Max;

    QLabel* TextLabel_X;
    QLabel* TextLabel_Y;
    QLabel* TextLabel_Z;

    QLineEdit* LineEdit_MinX;
    QLineEdit* LineEdit_MinY;
    QLineEdit* LineEdit_MinZ;
   
    QLineEdit* LineEdit_MaxX;
    QLineEdit* LineEdit_MaxY;
    QLineEdit* LineEdit_MaxZ;

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
    QGridLayout* GeometryGUI_BndBoxDlgLayout;
    QGridLayout* GroupConstructorsLayout;
    QGridLayout* GroupConstructor1Layout;
    QGridLayout* GroupButtonsLayout;
};

#endif // DIALOGBOX_BNDBOX_H
