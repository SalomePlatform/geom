//  File      : GeometryGUI_CompoundDlg.h
//  Created   :
//  Author    : Lucien PIGNOLONI
//  Project   : SALOME
//  Module    : GeometryGUI
//  Copyright : OPEN CASCADE
//  $Header$

#ifndef DIALOGBOX_COMPOUND_H
#define DIALOGBOX_COMPOUND_H

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
// class    : GeometryGUI_CompoundDlg
// purpose  :
//=================================================================================
class GeometryGUI_CompoundDlg : public QDialog
{ 
    Q_OBJECT

public:
    GeometryGUI_CompoundDlg( QWidget* parent = 0, const char* name = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0 );
    ~GeometryGUI_CompoundDlg();

private:

    void Init( SALOME_Selection* Sel ) ;
    void closeEvent( QCloseEvent* e ) ;
    void enterEvent ( QEvent * ) ;                         /* mouse enter the QWidget */

    GEOM::GEOM_Gen_var         myGeom ;                 /* Current Geom object */
    GeometryGUI*                  myGeomGUI ;              /* Current GeomGUI object */
    SALOME_Selection*             mySelection ;            /* User shape selection */
    GEOM::GEOM_Gen::ListOfIOR  myListShapes ;
    bool                          myOkListShapes ;          /* to check when arguments is defined */
    int                           myConstructorId ;         /* Current constructor id = radio button id */
    QLineEdit*                    myEditCurrentArgument;    /* Current LineEdit */

    QButtonGroup* GroupConstructors;
    QRadioButton* Constructor1;
    QGroupBox* GroupButtons;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QPushButton* buttonApply;
    QGroupBox* GroupC1;
    QLabel* TextLabelC1A1;
    QPushButton* SelectButtonC1A1;
    QLineEdit* LineEditC1A1;

private slots:

    void ConstructorsClicked(int constructorId);
    void ClickOnOk();
    void ClickOnCancel();
    void ClickOnApply();
    void SetEditCurrentArgument() ;
    void LineEditReturnPressed() ;
    void SelectionIntoArgument() ;
    void DeactivateActiveDialog() ;
    void ActivateThisDialog() ;

protected:
    QGridLayout* GeometryGUI_CompoundDlgLayout;
    QGridLayout* GroupConstructorsLayout;
    QGridLayout* GroupButtonsLayout;
    QGridLayout* GroupC1Layout;
};

#endif // DIALOGBOX_COMPOUND_H
