//  File      : GeometryGUI_WhatisDlg.h
//  Created   : Mon Mar 04 14:49:51 2002
//  Author    : Nicolas REJNERI

//  Project   : SALOME
//  Module    : GEOM
//  Copyright : Open CASCADE 2002
//  $Header$

#ifndef DIALOGBOX_WHATIS_H
#define DIALOGBOX_WHATIS_H

#include "SALOME_Selection.h"
#include "GEOM_ShapeTypeFilter.hxx"

#include <TopoDS_Shape.hxx>

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
class QTextView;
class GeometryGUI;

//=================================================================================
// class    : DialogBox_PROPERTIES
// purpose  :
//=================================================================================
class GeometryGUI_WhatisDlg : public QDialog
{ 
    Q_OBJECT

public:
    GeometryGUI_WhatisDlg( QWidget* parent = 0, const char* name = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0 );
    ~GeometryGUI_WhatisDlg();

private:

    void Init( SALOME_Selection* Sel ) ;
    void closeEvent( QCloseEvent* e ) ;
    void enterEvent ( QEvent * ) ;                         /* mouse enter the QWidget */
    void CalculateWhatis(const TopoDS_Shape& S) ;

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

    QTextView* Text;
    QString    SelectedName;

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
    QGridLayout* GeometryGUI_WhatisDlgLayout;
    QGridLayout* GroupConstructorsLayout;
    QGridLayout* GroupConstructor1Layout;
    QGridLayout* GroupButtonsLayout;
};

#endif // DIALOGBOX_WHATIS_H
