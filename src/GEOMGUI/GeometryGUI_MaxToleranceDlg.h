//  File      : GeometryGUI_MaxToleranceDlg.h
//  Created   : Mon Mar 04 14:10:03 2002
//  Author    : Nicolas REJNERI

//  Project   : SALOME
//  Module    : GEOM
//  Copyright : Open CASCADE 2002
//  $Header$


#ifndef DIALOGBOX_MAXTOLERANCE_H
#define DIALOGBOX_MAXTOLERANCE_H

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
// class    : DialogBox_PROPERTIES
// purpose  :
//=================================================================================
class GeometryGUI_MaxToleranceDlg : public QDialog
{ 
    Q_OBJECT

public:
    GeometryGUI_MaxToleranceDlg( QWidget* parent = 0, const char* name = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0 );
    ~GeometryGUI_MaxToleranceDlg();

private:

    void Init( SALOME_Selection* Sel ) ;
    void closeEvent( QCloseEvent* e ) ;
    void enterEvent ( QEvent * ) ;                         /* mouse enter the QWidget */
    void CalculateMaxTolerance(const TopoDS_Shape& S) ;

    GEOM::GEOM_Gen_var         myGeom ;                 /* Current GeomI object */
    GeometryGUI*                  myGeomGUI ;              /* Current GeomGUI object */
    SALOME_Selection*             mySelection ;            /* User shape selection */

    int                           myConstructorId ;        /* Current constructor id = radio button id */
    QLineEdit*                    myEditCurrentArgument;   /* Current LineEdit */

    QButtonGroup* GroupConstructors;
    QRadioButton* Constructor1;
    QGroupBox* GroupConstructor1;
    QLineEdit* LineEditC1A1;
    QPushButton* SelectButtonC1A1;
    QLabel* TextLabelC1A1;

    QLabel* TextLabel_Min;
    QLabel* TextLabel_Max;

    QLabel* TextLabel_Face;
    QLabel* TextLabel_Edge;
    QLabel* TextLabel_Vertex;

    QLineEdit* LineEdit_MinFace;
    QLineEdit* LineEdit_MinEdge;
    QLineEdit* LineEdit_MinVertex;
   
    QLineEdit* LineEdit_MaxFace;
    QLineEdit* LineEdit_MaxEdge;
    QLineEdit* LineEdit_MaxVertex;

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
    QGridLayout* GeometryGUI_MaxToleranceDlgLayout;
    QGridLayout* GroupConstructorsLayout;
    QGridLayout* GroupConstructor1Layout;
    QGridLayout* GroupButtonsLayout;
};

#endif // DIALOGBOX_MAXTOLERANCE_H
