//  File      : GeometryGUI_FillingDlg.h
//  Created   :
//  Author    : Lucien PIGNOLONI
//  Project   : SALOME
//  Module    : GeometryGUI
//  Copyright : OPEN CASCADE
//  $Header$

#ifndef DIALOGBOX_FILLING_H
#define DIALOGBOX_FILLING_H

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
class QSpinBox;
class QPushButton;
class QRadioButton;
class GeometryGUI;


//=================================================================================
// class    : GeometryGUI_FillingDlg
// purpose  :
//=================================================================================
class GeometryGUI_FillingDlg : public QDialog
{ 
    Q_OBJECT

public:
    GeometryGUI_FillingDlg( QWidget* parent = 0, const char* name = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0 );
    ~GeometryGUI_FillingDlg();

private:

    void Init( SALOME_Selection* Sel ) ;
    void closeEvent( QCloseEvent* e ) ;
    void enterEvent ( QEvent * ) ;                 /* mouse enter the QWidget */

    GEOM::GEOM_Gen_var myGeom ;                 /* Current GeomI object */
    GeometryGUI*          myGeomGUI ;              /* Current GeomGUI object */
    SALOME_Selection*     mySelection ;            /* User shape selection */
    GEOM::GEOM_Shape_var        myGeomShape ;            /* is mySectionShape */
    TopoDS_Shape          mySectionShape ;
    Standard_Integer      myMinDeg ;
    Standard_Integer      myMaxDeg ;
    Standard_Real         myTol3D ;
    Standard_Real         myTol2D ;
    Standard_Integer      myNbIter ;

    bool                  myOkSectionShape ;         /* to check when arguments is defined */
    int                   myConstructorId ;          /* Current constructor id = radio button id */
    QLineEdit*            myEditCurrentArgument;     /* Current LineEdit */
    Handle(GEOM_ShapeTypeFilter) myCompoundFilter ;  /* Filter selection */

    QButtonGroup* GroupConstructors;
    QRadioButton* Constructor1;
    QGroupBox* GroupButtons;
    QPushButton* buttonApply;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QGroupBox* GroupC1;
    QLineEdit* LineEditC1A1;
    QLabel* TextLabelC1A2;
    QLabel* TextLabelC1A3;
    QLabel* TextLabelC1A5;

    QLineEdit* LineEditC1A5;
    QLabel* TextLabelC1A1;
    QPushButton* SelectButtonC1A1;
    QLabel* TextLabelC1A4;
    QLabel* TextLabelC1A6;
    QLineEdit* LineEditC1A4;

    QSpinBox* SpinBox_C1A2;
    QSpinBox* SpinBox_C1A3;
    QSpinBox* SpinBox_C1A6;

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
    QGridLayout* GeometryGUI_FillingDlgLayout;
    QGridLayout* GroupConstructorsLayout;
    QGridLayout* GroupButtonsLayout;
    QGridLayout* GroupC1Layout;
};

#endif // DIALOGBOX_FILLING_H
