//  File      : GeometryGUI_PointDlg.h
//  Created   :
//  Author    : Lucien PIGNOLONI
//  Project   : SALOME
//  Module    : GeometryGUI
//  Copyright : OPEN CASCADE
//  $Header$


#ifndef DIALOGBOX_POINT_H
#define DIALOGBOX_POINT_H

#include "SALOME_Selection.h"
#include "GEOM_ShapeTypeFilter.hxx"
#include "GeometryGUI_SpinBox.h"

#include <gp_Pnt.hxx>
#include <TopoDS_Edge.hxx>

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QButtonGroup;
class QFrame;
class QGroupBox;
class QLabel;
class QSpinBox;
class QPushButton;
class QRadioButton;
class GeometryGUI;

#include <AIS_Shape.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <AIS_Drawer.hxx>


//=================================================================================
// class    : GeometryGUI_PointDlg
// purpose  :
//=================================================================================
class GeometryGUI_PointDlg : public QDialog
{ 
    Q_OBJECT

public:
    GeometryGUI_PointDlg( QWidget* parent = 0, const char* name = 0,
			  SALOME_Selection* Sel = 0,
			  const Handle(AIS_InteractiveContext)& ic = 0,
			  bool modal = FALSE,
			  WFlags fl = 0 );

    ~GeometryGUI_PointDlg();
    
private :

    SALOME_Selection*  mySelection ;        /* Current selection */   
    TopoDS_Shape       mySimulationTopoDs;  /* Shape used to display a simulation */
    gp_Pnt             myPoint ;            /* Is 'mySimulationTopoDs'  */
    GeometryGUI*       myGeomGUI ;          /* Current GeomGUI object */
    GEOM::GEOM_Gen_var myGeom ;             /* Current GeomI object */

    /* Interactive and local context management see also : bool UseLocalContext() */
    Handle (AIS_InteractiveContext) myIC ;                 /* Interactive context from IAPP */
    Standard_Integer                myLocalContextId ;     /* identify a local context for this method */
    TopAbs_ShapeEnum                myLocalContextMode ;   /* identify a selection mode into local context */
    bool                            myUseLocalContext ;    /* true when method as opened a local context  */

    int                             myConstructorId ;      /* Current constructor id = radio button id */
    double                          myParameter ;          /* Parameter used to create a vertex on edge (point on curve) */
    bool                            myOkEdge ;             /* true when an edge is selected by user */

    Handle(GEOM_ShapeTypeFilter)    myEdgeFilter;          /* filter for selection */
    
    void Init(SALOME_Selection* Sel, const Handle(AIS_InteractiveContext)& ic) ; /* Initialize dialog */ 
    void enterEvent(QEvent* e);
    void closeEvent(QCloseEvent* e) ;

    QButtonGroup* GroupConstructors;
    QRadioButton* Constructor1;
    QRadioButton* Constructor2;

    QGroupBox* GroupCoordinates;
    QLabel*    TextLabel_X;
    QLabel*    TextLabel_Y;
    QLabel*    TextLabel_Z;

    GeometryGUI_SpinBox* SpinBox_X ;
    GeometryGUI_SpinBox* SpinBox_Y ;
    GeometryGUI_SpinBox* SpinBox_Z ;

    QGroupBox*           GroupWithEdge;
    QLabel*              TextLabel_Edge;
    QPushButton*         SelectButton_Edge;
    QLineEdit*           LineEdit_Edge;
    QLabel*              TextLabel_Parameter;
    GeometryGUI_SpinBox* SpinBox_Parameter;

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
    void LineEditReturnPressed() ;
    void SelectionIntoArgument() ;
    void DeactivateActiveDialog() ;
    void ActivateThisDialog() ;
    void ValueChangedInSpinBox( double newValue ) ;
    bool CalculateVertexOnCurve(const TopoDS_Edge& anEdge, const Standard_Real aParameter, TopoDS_Shape& resultVertex) ;

public:   
    void PointIntoCoordinates(gp_Pnt P, bool displayPoint) ;
    bool UseLocalContext() ; /* return true if method has opened a local context */

protected:
    QGridLayout* GeometryGUI_PointDlgLayout;    
    QGridLayout* GroupConstructorsLayout;
    QGridLayout* GroupCoordinatesLayout;
    QGridLayout* GroupWithEdgeLayout;   
    QGridLayout* GroupButtonsLayout;
};

#endif // DIALOGBOX_POINT_H
