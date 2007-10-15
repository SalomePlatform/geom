// GEOM GEOMGUI : GUI for Geometry component
//
// Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
// 
// This library is free software; you can redistribute it and/or 
// modify it under the terms of the GNU Lesser General Public 
// License as published by the Free Software Foundation; either 
// version 2.1 of the License. 
// 
// This library is distributed in the hope that it will be useful, 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
// Lesser General Public License for more details. 
// 
// You should have received a copy of the GNU Lesser General Public 
// License along with this library; if not, write to the Free Software 
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
// 
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// File   : GEOMBase_Skeleton.h
// Author : Damine COQUERET, Open CASCADE S.A.S.
//

#ifndef GEOMBASE_SKELETON_H
#define GEOMBASE_SKELETON_H

#include "GEOM_GEOMBase.hxx"
#include "GEOMBase_Helper.h"

#include <QDialog>

class GeometryGUI;
class DlgRef_Skeleton;
class QDoubleSpinBox;
class QLineEdit;
class QButtonGroup;
class QPushButton;

#ifndef COORD_MIN
#  define COORD_MIN -1e+15
#  define COORD_MAX +1e+15
#endif // COORD_MIN

class GEOMBASE_EXPORT GEOMBase_Skeleton : public QDialog, public GEOMBase_Helper
{
  Q_OBJECT

public:
    GEOMBase_Skeleton( GeometryGUI*, QWidget* = 0, const char* = 0, bool = false, Qt::WindowFlags = 0 );
    ~GEOMBase_Skeleton();

private:
    void Init();

protected:
    void                initSpinBox( QDoubleSpinBox*, double, double, double = 0.1, int = 3 );
    
    void                closeEvent( QCloseEvent* );
    void                keyPressEvent( QKeyEvent* );

    /*! initialize "Name" field with a string "thePrefix_X" (Vertex_3)
     */
    void                initName( const QString& = QString() );

    /*! returns contents of "Name" field
     */
    virtual const char* getNewObjectName() const;

    /*! returns id of a selected "constructor" radio button or '-1' in case of error
     */
    int                 getConstructorId() const;

    void                setHelpFileName( const QString& );

    DlgRef_Skeleton*    mainFrame();
    QWidget*            centralWidget();
    QPushButton*        buttonCancel() const;
    QPushButton*        buttonOk() const;
    QPushButton*        buttonApply() const;
    QPushButton*        buttonHelp() const;

protected:
    QLineEdit*          myEditCurrentArgument; //!< Current LineEdit
    GeometryGUI*        myGeomGUI;             //!< reference GEOM GUI
    QString             myHelpFileName;        //!< Associated HTML help file name
    
    QButtonGroup*       myRBGroup;             //!< radio button group
    DlgRef_Skeleton*    myMainFrame;           //!< dialog box's mainframe widgetx

protected slots:
    virtual void        ClickOnCancel();
    void                LineEditReturnPressed();
    void                DeactivateActiveDialog();
    void                ActivateThisDialog();
    void                ClickOnHelp();

signals:
    void                constructorsClicked( int );
};

#endif // GEOMBASE_SKELETON_H
