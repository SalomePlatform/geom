//  File      : GeometryGUI_CutDlg.h
//  Created   :
//  Author    : Lucien PIGNOLONI
//  Project   : SALOME
//  Module    : GeometryGUI
//  Copyright : OPEN CASCADE
//  $Header$

#ifndef DIALOGBOX_CUT_H
#define DIALOGBOX_CUT_H

#include "SALOME_Selection.h"
#include "GEOM_ShapeTypeFilter.hxx"

#include <BRepAlgoAPI_Cut.hxx>

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QButtonGroup;
class QFrame;
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;
class GeometryGUI;


//=================================================================================
// class    : GeometryGUI_CutDlg
// purpose  :
//=================================================================================
class GeometryGUI_CutDlg : public QDialog
{ 
    Q_OBJECT

public:
    GeometryGUI_CutDlg( QWidget* parent = 0, const char* name = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0 );
    ~GeometryGUI_CutDlg();

private:

    void Init( SALOME_Selection* Sel ) ;
    void closeEvent( QCloseEvent* e ) ;
    void enterEvent ( QEvent * ) ;                /* mouse enter the QWidget */

    GEOM::GEOM_Gen_var myGeom ;                /* Current GeomI object */
    GeometryGUI*          myGeomGUI ;             /* Current GeomGUI object */
    SALOME_Selection*     mySelection ;           /* User shape selection */
    TopoDS_Shape          myShape1 ;              /* topology used to fuse */
    TopoDS_Shape          myShape2 ;              /* topology used to fuse */
    GEOM::GEOM_Shape_var        myGeomShape1 ;          /* is myShape1 */
    GEOM::GEOM_Shape_var        myGeomShape2 ;          /* is myShape2 */
    bool                  myOkShape1 ;
    bool                  myOkShape2 ;            /* to check when arguments are defined */
    int                   myConstructorId ;       /* Current constructor id = radio button id */
    QLineEdit*            myEditCurrentArgument;  /* Current LineEdit */

    QButtonGroup* GroupConstructors;
    QRadioButton* Constructor1;

    QGroupBox* GroupConstructor1;
    QLineEdit* LineEditC1A1Shape;
    QLineEdit* LineEditC1A2Shape;
    QPushButton* SelectButtonC1A1Shape;
    QPushButton* SelectButtonC1A2Shape;
    QLabel* TextLabelC1A2Shape;
    QLabel* TextLabelC1A1Shape;

    QGroupBox* GroupButtons;
    QPushButton* buttonApply;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;

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
    QGridLayout* GeometryGUI_CutDlgLayout;
    QGridLayout* GroupConstructorsLayout;
    QGridLayout* GroupButtonsLayout;
    QGridLayout* GroupConstructor1Layout;
};

#endif // DIALOGBOX_CUT_H
