//  File      : GeometryGUI_SuppressHoleDlg.h
//  Created   :
//  Author    : Lucien PIGNOLONI
//  Project   : SALOME
//  Module    : GeometryGUI
//  Copyright : OPEN CASCADE
//  $Header$

#ifndef GEOMETRYGUI_SUPPRESSHOLE_H
#define GEOMETRYGUI_SUPPRESSHOLE_H

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
// class    : GeometryGUI_SuppressHoleDlg
// purpose  :
//=================================================================================
class GeometryGUI_SuppressHoleDlg : public QDialog
{ 
    Q_OBJECT
      
public:
    GeometryGUI_SuppressHoleDlg( QWidget* parent = 0,
				 const char* name = 0,
				 SALOME_Selection* Sel = 0,		
				 Handle (AIS_InteractiveContext) ic = 0,  
				 bool modal = FALSE,
				 WFlags fl = 0 );

    ~GeometryGUI_SuppressHoleDlg();

private :
      
    void Init( SALOME_Selection* Sel, Handle (AIS_InteractiveContext) ic ) ;
    void closeEvent( QCloseEvent* e ) ;
    void enterEvent ( QEvent * ) ;                            /* Mouse enter the QWidget (to reactivate it) */

    void ResetStateOfDialog() ;
    void ResetPartial() ;

    /* Return the face selected by user from the main shape and index in a ListOfSub */
    TopoDS_Shape FaceFromList( const TopoDS_Shape& aShape,
			       const GEOM::GEOM_Shape::ListOfSubShapeID& ListOfSub ) ;

    Handle (AIS_InteractiveContext)   myIC ;                  /* Interactive context  */ 
    Standard_Integer                  myLocalContextId ;      /* identify a local context used by this method    */
    bool                              myUseLocalContext ;     /* true when this method as opened a local context */
    
    GEOM::GEOM_Gen_var             myGeom ;                /* Current Geom object */
    GeometryGUI*                      myGeomGUI ;             /* Current GeomGUI object */
    SALOME_Selection*                 mySelection ;           /* User shape selection */

    TopoDS_Shape                      myShape ;               /* Main shape selected  */
    TopoDS_Shape                      myFace ;                /* Face selected        */

    char*                             myShapeIOR ;
    bool                              myOkShape ;

    bool                              myOkSelectFace ;        /* true = sub mode GEOM::FACE selection done               */
    bool                              myOkSelectWire ;        /* true = sub mode GEOM::WIRE selection done (first wire)  */  

    GEOM::GEOM_Shape::ListOfSubShapeID_var  myListOfIdFace ;        /* After selection contains index of face into myShape      */
    GEOM::GEOM_Shape::ListOfSubShapeID_var  myListOfIdWire ;        /* After selection contains index of wire into myFace       */
    GEOM::GEOM_Shape::ListOfSubShapeID_var  myListOfIdEndFace ;     /* After selection contains index of end face into myShape  */
  
    QLineEdit*                        myEditCurrentArgument;  /* Current LineEdit */   
    int                               myConstructorId ;       /* Current constructor id = radio button id */

    QButtonGroup* GroupConstructors;
    QRadioButton* Constructor1;
    QGroupBox* GroupButtons;
    QPushButton* buttonOk;
    QPushButton* buttonApply;
    QPushButton* buttonClose;
    QGroupBox* GroupC1;
    QLabel* TextLabelC1A1;
    QPushButton* SelectButtonC1A1;
    QLineEdit* LineEditC1A1;
    QCheckBox* CheckBox1;
    QCheckBox* CheckBox2;
    QCheckBox* CheckBox3;

    /* Second constructor */
    QRadioButton* Constructor2;
    QGroupBox* GroupC2;
    QLabel* TextLabelC2A1;
    QPushButton* SelectButtonC2A1;
    QLineEdit* LineEditC2A1;
    QCheckBox* CheckBoxC2_1;


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
    void ActivateUserFaceSelection() ;
    void ActivateUserWireSelection() ;
    void ActivateUserEndFaceSelection() ;
    
    /* For the second constructor */
    void ActivateUserWiresOnFaceShellSelection() ;

  
protected:

    QGridLayout* GeometryGUI_SuppressHoleLayout;
    QGridLayout* GroupConstructorsLayout;
    QHBoxLayout* GroupButtonsLayout;
    QGridLayout* GroupC1Layout;
    QHBoxLayout* Layout2;

    QGridLayout* GroupC2Layout;
    QHBoxLayout* Layout3;
};

#endif // GEOMETRYGUI_SUPPRESSHOLE_H
