//  File      : GeometryGUI_ArchimedeDlg.h
//  Created   : Mon Feb 25 11:13:13 CET 2002
//  Author    : Nicolas REJNERI
//  Project   : SALOME
//  Module    : GeometryGUI
//  Copyright : OPEN CASCADE
//  $Header$

#ifndef DIALOGBOX_ARCHIMEDE_H
#define DIALOGBOX_ARCHIMEDE_H

#include "SALOME_Selection.h"
#include "GEOM_ShapeTypeFilter.hxx"

#include <qvariant.h>
#include <qdialog.h>
#include <qvalidator.h>

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
// class    : GeometryGUI_ArchimedeDlg
// purpose  :
//=================================================================================
class GeometryGUI_ArchimedeDlg : public QDialog
{ 
    Q_OBJECT

public:
    GeometryGUI_ArchimedeDlg( QWidget* parent = 0, const char* name = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0 );
    ~GeometryGUI_ArchimedeDlg();

private:

    void Init( SALOME_Selection* Sel ) ;
    void closeEvent( QCloseEvent* e ) ;
    void enterEvent ( QEvent * ) ;                         /* mouse enter the QWidget */

    GEOM::GEOM_Gen_var         myGeom ;                 /* Current GeomI object */
    GeometryGUI*                  myGeomGUI ;              /* Current GeomGUI object */
    SALOME_Selection*             mySelection ;            /* User shape selection */

    Handle(SALOME_InteractiveObject)  myIO ; 

    Standard_Real                 myWeight ;
    Standard_Real                 myWaterDensity ;
    Standard_Real                 myMeshingDeflection ;

    bool                          myOkIO ; 
    bool                          myOkWeight ; 
    bool                          myOkWaterDensity ; 
    bool                          myOkMeshingDeflection ;

    int                           myConstructorId ;         /* Current constructor id = radio button id */
    QLineEdit*                    myEditCurrentArgument;    /* Current LineEdit */

    QDoubleValidator              *myVa ;                   /* Double validator for numeric input */
    QDoubleValidator              *myVb ;                   /* Double validator for numeric input */
    QDoubleValidator              *myVc ;                   /* Double validator for numeric input */

    QButtonGroup* GroupConstructors;
    QRadioButton* Constructor1;
    QGroupBox* GroupButtons;
    QPushButton* buttonApply;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QGroupBox* GroupC1;
    QLineEdit* LineEditC1A1;
    QLineEdit* LineEditC1A2;
    QLineEdit* LineEditC1A3;
    QLineEdit* LineEditC1A4;
    QPushButton* SelectButtonC1A1;
    QLabel* TextLabelC1A1;
    QLabel* TextLabelC1A2;
    QLabel* TextLabelC1A3;
    QLabel* TextLabelC1A4;

private slots:

    void ConstructorsClicked(int constructorId);
    void ClickOnOk();
    void ClickOnCancel();
    void ClickOnApply();
    void SetEditCurrentArgument() ;
    void TextChangedInLineEdit(const QString&) ;
    void SelectionIntoArgument() ;
    void DeactivateActiveDialog() ;
    void ActivateThisDialog() ;

protected:
    QGridLayout* GeometryGUI_ArchimedeDlgLayout;
    QGridLayout* GroupConstructorsLayout;
    QGridLayout* GroupButtonsLayout;
    QGridLayout* GroupC1Layout;
};

#endif // DIALOGBOX_ARCHIMEDE_H
