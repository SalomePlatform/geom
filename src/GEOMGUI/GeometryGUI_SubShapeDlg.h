//  File      : GeometryGUI_SubShapeDlg.h
//  Created   :
//  Author    : Lucien PIGNOLONI
//  Project   : SALOME
//  Module    : GeometryGUI
//  Copyright : OPEN CASCADE
//  $Header$

#ifndef DIALOGBOX_SUBSHAPE_H
#define DIALOGBOX_SUBSHAPE_H

#include "SALOME_Selection.h"
#include "GEOM_ShapeTypeFilter.hxx"

#include <AIS_InteractiveContext.hxx>

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QButtonGroup;
class QCheckBox;
class QComboBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;
class GeometryGUI;


//=================================================================================
// class    : GeometryGUI_SubShapeDlg
// purpose  :
//=================================================================================
class GeometryGUI_SubShapeDlg : public QDialog
{ 
    Q_OBJECT

public:
    GeometryGUI_SubShapeDlg( QWidget* parent = 0,
			const char* name = 0,
			SALOME_Selection* Sel = 0,
			Handle (AIS_InteractiveContext) ic = 0,
			bool modal = FALSE,
			WFlags fl = 0 );

    ~GeometryGUI_SubShapeDlg();

private :

    void Init( SALOME_Selection* Sel, Handle (AIS_InteractiveContext) ic ) ;
    void closeEvent( QCloseEvent* e ) ;
    void enterEvent ( QEvent * ) ;                         /* mouse enter the QWidget */
    void ResetStateOfDialog() ;

    unsigned int NumberOfSubShapes( const TopoDS_Shape& S, const int shapeType ) ;


    /* Interactive and local context management see also : bool myUseLocalContext() */
    Handle (AIS_InteractiveContext) myIC ;                 /* Interactive context */ 
    Standard_Integer                myLocalContextId ;     /* identify a local context used by this method */
    bool                            myUseLocalContext ;    /* true when this method as opened a local context  */

    GEOM::GEOM_Gen_var              myGeom ;                /* Current Geom object */
    GeometryGUI*                    myGeomGUI ;             /* Current GeomGUI object */
    SALOME_Selection*               mySelection ;           /* User shape selection */

    TopoDS_Shape                    myShape ;
    char*                           myShapeIOR ;
    bool                            myOkShape ;
    int                             myShapeType ;           /* define a type of topology mode of sub selection */

    bool                            myWithShape ;           /* check if Shape item exists */

    bool                            myOkSelectSubMode ;     /* true = sub mode selection activated */
    QLineEdit*                      myEditCurrentArgument;  /* Current LineEdit */   
    int                             myConstructorId ;       /* Current constructor id = radio button id */
    bool                            myAbort ;               /* Indicate if sub Shape All has been aborted by user */
    
    QButtonGroup* GroupConstructors;
    QRadioButton* Constructor1;
    QGroupBox* GroupButtons;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QPushButton* buttonApply;
    QGroupBox* GroupC1;
    QPushButton* SelectButtonC1A1;   
    QLineEdit* LineEditC1A1;
    QLabel* TextLabelC1A1;
    QLabel* TextLabelComboBox1;
    QComboBox* ComboBox1;

    QCheckBox* CheckBox1;

private slots:

    void ConstructorsClicked(int constructorId);
    void ClickOnOk();
    void ClickOnCancel();
    void ClickOnApply();
    void LineEditReturnPressed() ;
    void SetEditCurrentArgument() ;
    void SelectionIntoArgument() ;
    void DeactivateActiveDialog() ;
    void ActivateThisDialog() ;
    void AllOrNotAll() ;
    void ComboTextChanged() ;

protected:
    QGridLayout* GeometryGUI_SubShapeDlgLayout;
    QGridLayout* GroupConstructorsLayout;
    QGridLayout* GroupButtonsLayout;
    QGridLayout* GroupC1Layout;
};

#endif // DIALOGBOX_SUBSHAPE_H
