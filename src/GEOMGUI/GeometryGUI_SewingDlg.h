//  File      : GeometryGUI_SewingDlg.h
//  Created   :
//  Author    : Lucien PIGNOLONI
//  Project   : SALOME
//  Module    : GeometryGUI
//  Copyright : OPEN CASCADE
//  $Header$

#ifndef DIALOGBOX_SEWING_H
#define DIALOGBOX_SEWING_H

#include "SALOME_Selection.h"
#include "GEOM_ShapeTypeFilter.hxx"

#include <Precision.hxx>

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
// class    : GeometryGUI_SewingDlg
// purpose  :
//=================================================================================
class GeometryGUI_SewingDlg : public QDialog
{ 
    Q_OBJECT

public:
    GeometryGUI_SewingDlg( QWidget* parent = 0, const char* name = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0 );
    ~GeometryGUI_SewingDlg();

private:

    void Init( SALOME_Selection* Sel ) ;
    void closeEvent( QCloseEvent* e ) ;
    void enterEvent ( QEvent * ) ;                          /* mouse enter the QWidget */

    GEOM::GEOM_Gen_var         myGeom ;                  /* Current GeomI object */
    GeometryGUI*                  myGeomGUI ;               /* Current GeomGUI object */
    SALOME_Selection*             mySelection ;             /* User shape selection */
    GEOM::GEOM_Gen::ListOfIOR  myListShapes ;
    Standard_Real                 myPrecision ;
    bool                          myOkListShapes ;          /* to check when arguments is defined */
    int                           myConstructorId ;         /* Current constructor id = radio button id */
    QLineEdit*                    myEditCurrentArgument;    /* Current LineEdit */

    QButtonGroup* GroupConstructors;
    QRadioButton* Constructor1;
    QGroupBox* GroupButtons;
    QPushButton* buttonApply;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QGroupBox* GroupC1;
    QLineEdit* LineEditC1A1;
    QLineEdit* LineEditC1A2;
    QPushButton* SelectButtonC1A1;
    QLabel* TextLabelC1A1;
    QLabel* TextLabelC1A2;

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
    QGridLayout* GeometryGUI_SewingDlgLayout;
    QGridLayout* GroupConstructorsLayout;
    QGridLayout* GroupButtonsLayout;
    QGridLayout* GroupC1Layout;
};

#endif // DIALOGBOX_SEWING_H
