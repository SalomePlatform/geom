//  File      : GeometryGUI_TranslationDlg.h
//  Created   :
//  Author    : Lucien PIGNOLONI
//  Project   : SALOME
//  Module    : GeometryGUI
//  Copyright : OPEN CASCADE
//  $Header$

#ifndef DIALOGBOX_TRANSLATION_H
#define DIALOGBOX_TRANSLATION_H

#include "SALOME_Selection.h"
#include "GEOM_ShapeTypeFilter.hxx"
#include "GeometryGUI_SpinBox.h"

#include <gp_Vec.hxx>
#include <TopoDS_Shape.hxx>
#include <BRepBuilderAPI_Transform.hxx>

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
// class    : GeometryGUI_TranslationDlg
// purpose  :
//=================================================================================
class GeometryGUI_TranslationDlg : public QDialog
{ 
    Q_OBJECT

public:
    GeometryGUI_TranslationDlg( QWidget* parent = 0, const char* name = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0 );
    ~GeometryGUI_TranslationDlg();

private :
    
    GEOM::GEOM_Gen_var               myGeom ;               /* Current Geom object */   
    GeometryGUI*                        myGeomGUI ;            /* Current GeomGUI object */
    TopoDS_Shape                        mySimulationTopoDs;    /* Shape used for simulation display */    
    SALOME_Selection*                   mySelection ;          /* User shape selection */ 
    TopoDS_Shape                        myBase ;
    GEOM::GEOM_Shape_var                      myGeomShape ;           /* is myBase */
    bool                                myOkBase ;
    gp_Vec                              myVec ;
    QLineEdit*                          myEditCurrentArgument;  /* Current LineEdit */   
    int                                 myConstructorId ;       /* Current constructor id = radio button id */
    Handle(GEOM_ShapeTypeFilter)        myEdgeFilter;           /* Filter selection */

    void closeEvent( QCloseEvent* e ) ;
    void enterEvent( QEvent* e);
    void Init( SALOME_Selection* Sel ) ;
    void MakeTranslationSimulationAndDisplay() ;

    QButtonGroup* GroupConstructors;
    QRadioButton* Constructor1;
    QGroupBox* GroupC1;
    QLabel* TextLabel_DX;
    QLabel* TextLabel_DY;
    QLabel* TextLabel_DZ;

    GeometryGUI_SpinBox*  SpinBox_DX ;
    GeometryGUI_SpinBox*  SpinBox_DY ;
    GeometryGUI_SpinBox*  SpinBox_DZ ;

    QLabel* TextLabelC1A1;
    QPushButton* SelectButtonC1A1;
    QLineEdit* LineEditC1A1;
    QGroupBox* GroupButtons;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QPushButton* buttonApply;

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
    QGridLayout* GeometryGUI_TranslationDlgLayout;
    QGridLayout* GroupConstructorsLayout;
    QGridLayout* GroupC1Layout;
    QGridLayout* GroupButtonsLayout;

    QHBoxLayout* Layout1 ;
};

#endif // DIALOGBOX_TRANSLATION_H
