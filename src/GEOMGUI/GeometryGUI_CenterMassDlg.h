//  File      : GeometryGUI_CenterMassDlg.h
//  Created   :
//  Author    : Lucien PIGNOLONI
//  Project   : SALOME
//  Module    : GeometryGUI
//  Copyright : OPEN CASCADE
//  $Heade$


#ifndef DIALOGBOX_CMASS_H
#define DIALOGBOX_CMASS_H

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
// class    : GeometryGUI_CenterMassDlg
// purpose  :
//=================================================================================
class GeometryGUI_CenterMassDlg : public QDialog
{ 
    Q_OBJECT

public:
    GeometryGUI_CenterMassDlg( QWidget* parent = 0, const char* name = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0 );
    ~GeometryGUI_CenterMassDlg();

private:

    void Init( SALOME_Selection* Sel ) ;
    void closeEvent( QCloseEvent* e ) ;
    void enterEvent ( QEvent * ) ;                /* mouse enter the QWidget */
    bool CalculateAndDisplayCenterMass() ;

    GEOM::GEOM_Gen_var myGeom ;                /* Current GeomI object */
    GeometryGUI*          myGeomGUI ;             /* Current GeomGUI object */
    SALOME_Selection*     mySelection ;           /* User shape selection */
    TopoDS_Shape          mySimulationTopoDs;     /* Shape used for simulation display */

    GEOM::GEOM_Shape_var        myGeomShape ;           /* is myBase */
    TopoDS_Shape          myShape ;               /* Shape argument */
    gp_Pnt                myCenterMass ;
    bool                  myOkCenterMass ;        /* true after center of mass simulation calculation */
    
    int                   myConstructorId ;       /* Current constructor id = radio button id */
    QLineEdit*            myEditCurrentArgument;  /* Current LineEdit */

    QGroupBox* GroupButtons;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QPushButton* buttonApply;
    QButtonGroup* GroupConstructors;
    QRadioButton* Constructor1;
    QGroupBox* GroupC1;
    QPushButton* SelectButtonC1A1;
    QLineEdit* LineEdit_X;
    QLabel* TextLabel_Z;
    QLineEdit* LineEdit_Z;
    QLabel* TextLabelC1A1;
    QLineEdit* LineEdit_Y;
    QLabel* TextLabel_Y;
    QLabel* TextLabel_X;
    QLabel* TextLabel_Center;
    QLineEdit* LineEditC1A1;

private slots:

    void ConstructorsClicked(int constructorId);
    void ClickOnCancel();
    void ClickOnOk() ;
    void ClickOnApply();
    void SetEditCurrentArgument() ;
    void LineEditReturnPressed() ;
    void SelectionIntoArgument() ;
    void DeactivateActiveDialog() ;
    void ActivateThisDialog() ;

protected:
    QGridLayout* GeometryGUI_CenterMassDlgLayout;
    QGridLayout* GroupButtonsLayout;
    QGridLayout* GroupConstructorsLayout;
    QGridLayout* GroupC1Layout;
};

#endif // DIALOGBOX_CMASS_H
