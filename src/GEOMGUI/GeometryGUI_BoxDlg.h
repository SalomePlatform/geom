//  File      : GeometryGUI_BoxDlg.h
//  Created   :
//  Author    : Lucien PIGNOLONI
//  Project   : SALOME
//  Module    : GeometryGUI
//  Copyright : OPEN CASCADE
//  $Header$


#ifndef DIALOGBOX_BOX_H
#define DIALOGBOX_BOX_H

#include "SALOME_Selection.h"
#include "GEOM_ShapeTypeFilter.hxx"

#include <gp_Pnt.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <Precision.hxx>

#include <qvariant.h>
#include <qdialog.h>
#include <qwidget.h>

class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QButtonGroup;
class QFrame;
class QGroupBox;
class QLineEdit;
class QPushButton;
class QRadioButton;
class QLabel;
class QPushButton;
class GeometryGUI_SpinBox;
class GeometryGUI;


//=================================================================================
// class    : GeometryGUI_BoxDlg
// purpose  :
//=================================================================================
class GeometryGUI_BoxDlg : public QDialog
{ 
    Q_OBJECT

public:
    GeometryGUI_BoxDlg( QWidget* parent = 0, const char* name = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0 );
    ~GeometryGUI_BoxDlg();

private :
 
    void closeEvent( QCloseEvent* e ) ;
    void enterEvent( QEvent* e );
    void Init(SALOME_Selection* Sel) ;
    bool TestBoxDimensions( gp_Pnt P1, gp_Pnt P2 ) ;

    GEOM::GEOM_Gen_var myGeom ;                /* Current GeomI object */
    GeometryGUI*          myGeomGUI ;             /* Current GeomGUI object */
    TopoDS_Shape          mySimulationTopoDs ;    /* Shape used for simulation display */
    SALOME_Selection*     mySelection ;           /* User shape selection */    
    gp_Pnt                myPoint1 ;              /* Points containing the vector */    
    gp_Pnt                myPoint2 ;
    bool                  myOkPoint1 ;            /* true when myPoint is defined */
    bool                  myOkPoint2 ;
    int                   myConstructorId ;       /* Current constructor id = radio button id */ 
    QLineEdit*            myEditCurrentArgument;  /* Current LineEdit */
    Handle(GEOM_ShapeTypeFilter) myVertexFilter;  /* filter for selection */


    QButtonGroup* GroupConstructors;
    QRadioButton* Constructor1;
    QRadioButton* Constructor2;

    QGroupBox* GroupButtons;
    QPushButton* buttonApply;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;

    QGroupBox* GroupPoints;
    QPushButton* SelectButtonPt1;    
    QPushButton* SelectButtonPt2;
    QLineEdit* LineEditPt2;
    QLineEdit* LineEditPt1;
    QLabel* TextLabelPt1;
    QLabel* TextLabelPt2;

    QGroupBox* GroupDimensions ;    
    QLabel* TextLabel_DX ;
    QLabel* TextLabel_DY ;
    QLabel* TextLabel_DZ ;

    GeometryGUI_SpinBox* SpinBox_DX ;
    GeometryGUI_SpinBox* SpinBox_DY ;
    GeometryGUI_SpinBox* SpinBox_DZ ;

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

    QGridLayout* GeometryGUI_BoxDlgLayout;
    QGridLayout* GroupConstructorsLayout;
    QGridLayout* GroupButtonsLayout;
    QGridLayout* GroupPointsLayout;
    QGridLayout* GroupDimensionsLayout;

};

#endif // DIALOGBOX_BOX_H
