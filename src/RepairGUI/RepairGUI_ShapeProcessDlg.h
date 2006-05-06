//  GEOM GEOMGUI : GUI for Geometry component
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
// 
//  This library is free software; you can redistribute it and/or 
//  modify it under the terms of the GNU Lesser General Public 
//  License as published by the Free Software Foundation; either 
//  version 2.1 of the License. 
// 
//  This library is distributed in the hope that it will be useful, 
//  but WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
//  Lesser General Public License for more details. 
// 
//  You should have received a copy of the GNU Lesser General Public 
//  License along with this library; if not, write to the Free Software 
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
// 
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : RepairGUI_ShapeProcessDlg.h
//  Author : Lucien PIGNOLONI
//  Module : GEOM

#ifndef DIALOGBOX_ShapeProcess_H
#define DIALOGBOX_ShapeProcess_H

#include "GEOMBase_Skeleton.h"
#include "DlgRef_1Sel_QTD.h"
#include "RepairGUI.h"
#include "QtxDblSpinBox.h"

#include <qspinbox.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qlistview.h>
#include <qwidgetstack.h>
#include <qvaluelist.h>
#include <qdict.h>
#include <qmap.h>

//=================================================================================
// class    : RepairGUI_ShapeProcessDlg
// purpose  :
//=================================================================================
class RepairGUI_ShapeProcessDlg : public GEOMBase_Skeleton
{ 
    Q_OBJECT

public:
    RepairGUI_ShapeProcessDlg(GeometryGUI* theGeometryGUI, QWidget* parent = 0,
			      const char* name = 0, bool modal = FALSE, WFlags fl = 0);
    ~RepairGUI_ShapeProcessDlg();

protected:
    // redefined from GEOMBase_Helper
    virtual GEOM::GEOM_IOperations_ptr createOperation();
    virtual bool isValid( QString& );
    virtual bool execute( ObjectList& objects );

    virtual void closeEvent( QCloseEvent* e );        

private:
    void             init();
    void             reset();
    void             loadDefaults(); // initialize all controls with default values (from resource file)

    GEOM::string_array* getActiveOperators();
    GEOM::string_array* getParameters( const GEOM::string_array& theOperators );
    GEOM::string_array* getValues( const GEOM::string_array& theParameters );

    void             enterEvent(QEvent* e);

    QWidget*         getControl( const char* );
    void             setValue( QWidget*, const char* ); // initialize the given control in the proper way
                                                        // (analize its class and convert the value string)
    const char*      getValue( QWidget* theControl ) const; // retrieve value of the control in the proper way

    QStringList      myOpLst; // list of available Shape Healing Operators
    QMap<QString,QStringList> myValMap; // map of parameters of operators
    //QDict<QString,QWidget*> myCtrlMap;  // map of controls (values) of parameters
    void             initParamsValues(); // initialize the data structures
    void             initSelection();

    GEOM::ListOfGO_var myObjects;  // selected objects

    DlgRef_1Sel_QTD* mySelectWdgt;
    QListView*       myOpList;
    QWidgetStack*    myStack;

    QtxDblSpinBox*   myFixShapeTol3D;
    QtxDblSpinBox*   myFixShapeMaxTol3D;

    QtxDblSpinBox*   myFixFaceSizeTol;

    QtxDblSpinBox*   myDropSmallEdgesTol3D;

    QtxDblSpinBox*   mySplitAngleAngle;
    QtxDblSpinBox*   mySplitAngleMaxTol;

    QSpinBox*        mySplitClosedFacesNum;

    QtxDblSpinBox*   mySplitContTol3D;
    QComboBox*       mySplitContSurfCont;
    QComboBox*       mySplitContCurvCont;

    QCheckBox*       myBSplineSurfModeChk;
    QCheckBox*       myBSpline3DCurveChk;
    QCheckBox*       myBSpline2DCurveChk;
    QtxDblSpinBox*   myBSplineTol3D;
    QtxDblSpinBox*   myBSplineTol2D;
    QSpinBox*        myBSplineDegree;
    QSpinBox*        myBSplineSegments;
    QComboBox*       myBSpline2DCont;
    QComboBox*       myBSpline3DCont;

    QCheckBox*       myToBezierSurfModeChk;
    QCheckBox*       myToBezier3DCurveChk;
    QCheckBox*       myToBezier2DCurveChk;
    QtxDblSpinBox*   myToBezierMaxTol;

    QtxDblSpinBox*   mySameParameterTol3D;
    
private slots:
    void             onOk();
    bool             onApply();

    void             activate();

    void             lineEditReturnPressed();
    void             selectionChanged();
    void             selectClicked();
    void             operationChanged();
    void             advOptionToggled(bool);
};             

#endif // DIALOGBOX_ShapeProcess_H
