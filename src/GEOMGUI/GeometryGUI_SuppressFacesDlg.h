//  File      : GeometryGUI_SuppressFacesDlg.h
//  Created   :
//  Author    : Lucien PIGNOLONI
//  Project   : SALOME
//  Module    : GeometryGUI
//  Copyright : OPEN CASCADE
//  $Header$

#ifndef DIALOGBOX_SUPPRESSFACES_H
#define DIALOGBOX_SUPPRESSFACES_H

#include "SALOME_Selection.h"
#include "GEOM_ShapeTypeFilter.hxx"

#include <qvariant.h>
#include <qdialog.h>

#include <AIS_InteractiveContext.hxx>

class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QButtonGroup;
class QCheckBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;
class GeometryGUI;


//=================================================================================
// class    : GeometryGUI_SuppressFacesDlg
// purpose  :
//=================================================================================
class GeometryGUI_SuppressFacesDlg : public QDialog
{ 
    Q_OBJECT

public:
    GeometryGUI_SuppressFacesDlg( QWidget* parent = 0,
				  const char* name = 0,
				  SALOME_Selection* Sel = 0,		
				  Handle (AIS_InteractiveContext) ic = 0,       
				  bool modal = FALSE,
				  WFlags fl = 0 );
    
    ~GeometryGUI_SuppressFacesDlg();

private :
      
    void Init( SALOME_Selection* Sel, Handle (AIS_InteractiveContext) ic ) ;
    void closeEvent( QCloseEvent* e ) ;
    void enterEvent ( QEvent * ) ;                /* mouse enter the QWidget */
    void ResetStateOfDialog() ;
    
    /* Interactive and local context management see also : bool myUseLocalContext() */
    Handle (AIS_InteractiveContext) myIC ;                 /* Interactive context */ 
    Standard_Integer                myLocalContextId ;     /* identify a local context used by this method */
    bool                            myUseLocalContext ;    /* true when this method as opened a local context  */

    GEOM::GEOM_Gen_var           myGeom ;                /* Current Geom object */
    GeometryGUI*                    myGeomGUI ;             /* Current GeomGUI object */
    SALOME_Selection*               mySelection ;           /* User shape selection */

    TopoDS_Shape                    myShape ;
    char*                           myShapeIOR ;
    bool                            myOkShape ;

    bool                            myOkSelectSubMode ;     /* true = sub mode selection activated */
    QLineEdit*                      myEditCurrentArgument;  /* Current LineEdit */   
    int                             myConstructorId ;       /* Current constructor id = radio button id */
 
    QButtonGroup* GroupConstructors;
    QRadioButton* Constructor1;
    QGroupBox* GroupC1;
    QLabel* TextLabelC1A1;
    QPushButton* SelectButtonC1A1;
    QLineEdit* LineEditC1A1;
    QGroupBox* GroupButtons;
    QPushButton* buttonOk;
    QPushButton* buttonApply;
    QPushButton* buttonClose;

    QCheckBox* CheckBox1;

private slots:

    void ConstructorsClicked(int constructorId);
    void ClickOnOk();
    void ClickOnApply() ;
    void ClickOnClose();

    void LineEditReturnPressed() ;
    void SetEditCurrentArgument() ;
    void SelectionIntoArgument() ;
    void DeactivateActiveDialog() ;
    void ActivateThisDialog() ;
    void ActivateUserSelection() ;

protected:
    QGridLayout* GeometryGUI_SuppressFacesDlgLayout;
    QGridLayout* GroupConstructorsLayout;
    QGridLayout* GroupC1Layout;
    QHBoxLayout* Layout2;
    QGridLayout* GroupButtonsLayout;
};

#endif // DIALOGBOX_SUPPRESSFACES_H
