//  File      : GeometryGUI_PipeDlg.h
//  Created   :
//  Author    : Lucien PIGNOLONI
//  Project   : SALOME
//  Module    : GeometryGUI
//  Copyright : OPEN CASCADE
//  $Header$

#ifndef DIALOGBOX_PIPE_H
#define DIALOGBOX_PIPE_H

#include "SALOME_Selection.h"
#include "GEOM_ShapeTypeFilter.hxx"

#include <BRepOffsetAPI_MakePipe.hxx>

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
// class    : GeometryGUI_PipeDlg
// purpose  :
//=================================================================================
class GeometryGUI_PipeDlg : public QDialog
{ 
    Q_OBJECT

public:
    GeometryGUI_PipeDlg( QWidget* parent = 0, const char* name = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0 );
    ~GeometryGUI_PipeDlg();

private:

    void Init( SALOME_Selection* Sel ) ;
    void closeEvent( QCloseEvent* e ) ;
    void enterEvent ( QEvent * ) ;                /* mouse enter the QWidget */

    GEOM::GEOM_Gen_var myGeom ;                /* Current GeomI object */
    GeometryGUI*          myGeomGUI ;             /* Current GeomGUI object */
    SALOME_Selection*     mySelection ;           /* User shape selection */
    TopoDS_Shape          myShape1 ;              /* topology used */
    TopoDS_Shape          myShape2 ;              /* topology used */
    GEOM::GEOM_Shape_var        myGeomShape1 ;          /* is myShape1 */
    GEOM::GEOM_Shape_var        myGeomShape2 ;          /* is myShape2 */
    bool                  myOkShape1 ;
    bool                  myOkShape2 ;            /* to check when arguments are defined */
    int                   myConstructorId ;       /* Current constructor id = radio button id */
    QLineEdit*            myEditCurrentArgument;  /* Current LineEdit */
    // Handle(GEOM_ShapeTypeFilter) myEdgFilter ;   /* Filter selection */
    // Handle(GEOM_ShapeTypeFilter) myWireFilter ;  /* Filter selection */

    TopoDS_Shape          mySimulationTopoDs ;    /* Shape used for simulation display */

    QButtonGroup* GroupConstructors;
    QRadioButton* Constructor1;
    QGroupBox* GroupButtons;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QPushButton* buttonApply;
    QGroupBox* GroupC1;
    QLineEdit* LineEditC1A1;
    QPushButton* SelectButtonC1A1;
    QLabel* TextLabelC1A1;
    QLabel* TextLabelC1A2;
    QPushButton* SelectButtonC1A2;
    QLineEdit* LineEditC1A2;

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
    QGridLayout* GeometryGUI_PipeDlgLayout;
    QGridLayout* GroupConstructorsLayout;
    QGridLayout* GroupButtonsLayout;
    QGridLayout* GroupC1Layout;
};

#endif // DIALOGBOX_PIPE_H
