//  File      : GeometryGUI_CheckShape.h
//  Created   : Tue Mar 12 17:25:52 2002
//  Author    : Nicolas REJNERI
//  Project   : SALOME
//  Module    : GEOM
//  Copyright : Open CASCADE 2002
//  $Header$


#ifndef DIALOGBOX_CHECKSHAPE_H
#define DIALOGBOX_CHECKSHAPE_H

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
class QTextView;
class GeometryGUI;

//=================================================================================
// class    : GeometryGUI_CheckShape
// purpose  :
//=================================================================================
class GeometryGUI_CheckShape : public QDialog
{ 
    Q_OBJECT

public:
    GeometryGUI_CheckShape( QWidget* parent = 0, const char* name = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0 );
    ~GeometryGUI_CheckShape();

private:

    void Init( SALOME_Selection* Sel ) ;
    void closeEvent( QCloseEvent* e ) ;
    void enterEvent ( QEvent * ) ;                         /* mouse enter the QWidget */
    void Check(const TopoDS_Shape S) ;

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
    QGridLayout* GeometryGUI_CheckShapeLayout;
    QGridLayout* GroupConstructorsLayout;
    QGridLayout* GroupConstructor1Layout;
    QGridLayout* GroupButtonsLayout;
};

#endif // DIALOGBOX_CHECKSHAPE_H
