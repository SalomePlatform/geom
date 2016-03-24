// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
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

// GEOM GEOMGUI : GUI for Geometry component
// File   : GEOMBase_Skeleton.h
// Author : Damine COQUERET, Open CASCADE S.A.S.

#ifndef GEOMBASE_SKELETON_H
#define GEOMBASE_SKELETON_H

#include "GEOM_GEOMBase.hxx"
#include "GEOMBase_DlgSkeleton.h"
#include "GEOMBase_Helper.h"

#include <QDialog>

class SalomeApp_DoubleSpinBox;
class GeometryGUI;
class GEOMBase_DlgSkeleton;
class QSpinBox;
class QDoubleSpinBox;
class QLineEdit;
class QButtonGroup;
class QPushButton;

#ifndef COORD_MIN
#  define COORD_MIN -1e+15
#  define COORD_MAX +1e+15
#  define MAX_NUMBER 100000
#  define DBL_DIGITS_DISPLAY 16
#endif // COORD_MIN

class GEOMBASE_EXPORT GEOMBase_Skeleton : public QDialog, public GEOMBase_Helper
{
  Q_OBJECT

public:
    GEOMBase_Skeleton( GeometryGUI*, QWidget* = 0, bool = false, Qt::WindowFlags = 0 );
    ~GEOMBase_Skeleton();

private:
    void Init();

protected:
    void                initSpinBox( QSpinBox*, int, int, int = 1 );
    void                initSpinBox( SalomeApp_DoubleSpinBox*, double, double, double = 0.1, const char* = "length_precision" );
    
    void                updateAttributes( GEOM::GEOM_Object_ptr, const QStringList& );

    void                closeEvent( QCloseEvent* );
    void                keyPressEvent( QKeyEvent* );

    /*! initialize "Name" field with a string "thePrefix_X" (Vertex_3)
     */
    void                initName( const QString& = QString() );

    /*! returns contents of "Name" field
     */
    virtual QString getNewObjectName (int CurrObj = -1) const;

    /*! returns id of a selected "constructor" radio button or '-1' in case of error
     */
    int                 getConstructorId() const;
    /*! set selected "constructor" radio button id
     */
    void                setConstructorId( const int );
    /*! unset selection on all "constructor" radio buttons
     */
    void                unsetConstructorId();

    void                showOnlyPreviewControl();
    
    void                setHelpFileName( const QString& );

    GEOMBase_DlgSkeleton* mainFrame();
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
    GEOMBase_DlgSkeleton*    myMainFrame;      //!< dialog box's mainframe widget
    QString             myHelpContext;         //!< Help context, needed for the customization 
                                               //!< path where located plugins help HTML pages

protected slots:
    virtual void        ClickOnCancel();
    virtual void        processPreview();
    void                LineEditReturnPressed();
    void                DeactivateActiveDialog();
    void                ActivateThisDialog();
    virtual void        ClickOnHelp();

signals:
    void                constructorsClicked( int );
};

#endif // GEOMBASE_SKELETON_H
