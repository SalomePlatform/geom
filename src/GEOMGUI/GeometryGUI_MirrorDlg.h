//  File      : GeometryGUI_MirrorDlg.h
//  Created   :
//  Author    : Lucien PIGNOLONI
//  Project   : SALOME
//  Module    : GeometryGUI
//  Copyright : OPEN CASCADE
//  $Header$

#ifndef DIALOGBOX_MIRROR_H
#define DIALOGBOX_MIRROR_H

#include "SALOME_Selection.h"
#include "GEOM_FaceFilter.hxx"

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
// class    : GeometryGUI_MirrorDlg
// purpose  :
//=================================================================================
class GeometryGUI_MirrorDlg : public QDialog
{ 
    Q_OBJECT

public:
    GeometryGUI_MirrorDlg( QWidget* parent = 0, const char* name = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0 );
    ~GeometryGUI_MirrorDlg();

private :

    void closeEvent( QCloseEvent* e ) ;
    void enterEvent( QEvent* e);                 /* Mouse enter the QWidget */
    void Init( SALOME_Selection* Sel ) ;
    void MakeMirrorSimulationAndDisplay( const TopoDS_Shape& S1, const TopoDS_Shape& S2 ) ;

    GEOM::GEOM_Gen_var myGeom ;                /* Current GeomI object */
    GeometryGUI*          myGeomGUI ;             /* Current GeomGUI object */
    SALOME_Selection*     mySelection ;           /* User shape selection */
    TopoDS_Shape          myShape1 ;              /* topology used */
    TopoDS_Shape          myShape2 ;              /* topology used */
    GEOM::GEOM_Shape_var        myGeomShape1 ;          /* is myShape1 */
    GEOM::GEOM_Shape_var        myGeomShape2 ;          /* is myShape2 */
    TopoDS_Shape          mySimulationTopoDs;     /* Shape used for simulation display */    
    bool                  myOkShape1 ;
    bool                  myOkShape2 ;            /* to check when arguments are defined */
    int                   myConstructorId ;       /* Current constructor id = radio button id */
    QLineEdit*            myEditCurrentArgument;  /* Current LineEdit */    
    Handle(GEOM_FaceFilter) myFaceFilter;    /* To filter selections */

    QGroupBox* GroupButtons;
    QPushButton* buttonApply;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QButtonGroup* GroupConstructors;
    QRadioButton* Constructor1;
    QGroupBox* GroupC1;
    QLabel* TextLabelC1A1;
    QLabel* TextLabelC1A2;
    QPushButton* SelectButtonC1A1;
    QPushButton* SelectButtonC1A2;
    QLineEdit* LineEditC1A1;
    QLineEdit* LineEditC1A2;

private slots :

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
    QGridLayout* GeometryGUI_MirrorDlgLayout;
    QGridLayout* GroupButtonsLayout;
    QGridLayout* GroupConstructorsLayout;
    QGridLayout* GroupC1Layout;
};

#endif // DIALOGBOX_MIRROR_H
